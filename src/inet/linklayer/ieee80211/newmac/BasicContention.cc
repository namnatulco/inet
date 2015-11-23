//
// Copyright (C) 2015 Andras Varga
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// Author: Andras Varga
//

#include "BasicContention.h"
#include "IUpperMac.h"
#include "IMacRadioInterface.h"
#include "IStatistics.h"
#include "IRx.h"
#include "inet/common/FSMA.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"

namespace inet {
namespace ieee80211 {

// for @statistic; don't forget to keep synchronized the C++ enum and the runtime enum definition
Register_Enum(BasicContention::State,
        (BasicContention::IDLE,
         BasicContention::DEFER,
         BasicContention::IFS_AND_BACKOFF,
         BasicContention::OWNING));

Define_Module(BasicContention);

// non-member utility function
void collectContentionModules(cModule *firstContentionModule, IContention **& contentionTx)
{
    ASSERT(firstContentionModule != nullptr);
    int count = firstContentionModule->getVectorSize();

    contentionTx = new IContention *[count + 1];
    for (int i = 0; i < count; i++) {
        cModule *sibling = firstContentionModule->getParentModule()->getSubmodule(firstContentionModule->getName(), i);
        contentionTx[i] = check_and_cast<IContention *>(sibling);
    }
    contentionTx[count] = nullptr;
}

void BasicContention::initialize()
{
    mac = check_and_cast<IMacRadioInterface *>(getModuleByPath(par("macModule")));
    upperMac = check_and_cast<IUpperMac *>(getModuleByPath(par("upperMacModule")));
    collisionController = dynamic_cast<ICollisionController *>(getModuleByPath(par("collisionControllerModule")));
    statistics = check_and_cast<IStatistics*>(getModuleByPath(par("statisticsModule")));
    initialBackoffOptimization = par("initialBackoffOptimization");

    txIndex = getIndex();
    if (txIndex > 0 && !collisionController)
        throw cRuntimeError("No collision controller module -- one is needed when multiple Contention instances are present");

    if (!collisionController)
        startTxEvent = new cMessage("startTx");

    fsm.setName("fsm");
    fsm.setState(IDLE, "IDLE");

    WATCH(txIndex);
    WATCH(ifs);
    WATCH(eifs);
    WATCH(cwMin);
    WATCH(cwMax);
    WATCH(slotTime);
    WATCH(retryCount);
    WATCH(endEifsTime);
    WATCH(backoffSlots);
    WATCH(scheduledTransmissionTime);
    WATCH(channelLastBusyTime);
    WATCH(mediumFree);
    updateDisplayString();
}

BasicContention::~BasicContention()
{
    cancelAndDelete(startTxEvent);
}

void BasicContention::startContention(simtime_t ifs, simtime_t eifs, int cwMin, int cwMax, simtime_t slotTime, int retryCount, IContentionCallback *callback)
{
    Enter_Method("startContention()");
    ASSERT(fsm.getState() == IDLE);
    this->ifs = ifs;
    this->eifs = eifs;
    this->cwMin = cwMin;
    this->cwMax = cwMax;
    this->slotTime = slotTime;
    this->retryCount = retryCount;
    this->callback = callback;

    int cw = computeCw(cwMin, cwMax, retryCount);
    backoffSlots = intrand(cw + 1);
    handleWithFSM(START, nullptr);
}

int BasicContention::computeCw(int cwMin, int cwMax, int retryCount)
{
    int cw = ((cwMin + 1) << retryCount) - 1;
    if (cw > cwMax)
        cw = cwMax;
    return cw;
}

void BasicContention::handleWithFSM(EventType event, cMessage *msg)
{
    // emit(stateSignal, fsm.getState()); TODO
    EV_DEBUG << "handleWithFSM: processing event " << getEventName(event) << "\n";
    bool finallyReportInternalCollision = false;
    bool finallyReportChannelAccessGranted = false;
    FSMA_Switch(fsm) {
        FSMA_State(IDLE) {
            FSMA_Enter(mac->sendDownPendingRadioConfigMsg());
            FSMA_Event_Transition(Starting-IFS-and-Backoff,
                    event == START && mediumFree,
                    IFS_AND_BACKOFF,
                    scheduleTransmissionRequest();
                    );
            FSMA_Event_Transition(Busy,
                    event == START && !mediumFree,
                    DEFER,
                    ;
                    );
            FSMA_Ignore_Event(event==MEDIUM_STATE_CHANGED);
            FSMA_Ignore_Event(event==CORRUPTED_FRAME_RECEIVED);
            FSMA_Ignore_Event(event==CHANNEL_RELEASED);
            FSMA_Fail_On_Unhandled_Event();
        }
        FSMA_State(DEFER) {
            FSMA_Enter(mac->sendDownPendingRadioConfigMsg());
            FSMA_Event_Transition(Restarting-IFS-and-Backoff,
                    event == MEDIUM_STATE_CHANGED && mediumFree,
                    IFS_AND_BACKOFF,
                    scheduleTransmissionRequest();
                    );
            FSMA_Event_Transition(Use-EIFS,
                    event == CORRUPTED_FRAME_RECEIVED,
                    DEFER,
                    endEifsTime = simTime() + eifs;
                    );
            FSMA_Fail_On_Unhandled_Event();
        }
        FSMA_State(IFS_AND_BACKOFF) {
            FSMA_Enter();
            FSMA_Event_Transition(Backoff-expired,
                    event == TRANSMISSION_GRANTED,
                    OWNING,
                    finallyReportChannelAccessGranted = true;
                    );
            FSMA_Event_Transition(Defer-on-channel-busy,
                    event == MEDIUM_STATE_CHANGED && !mediumFree,
                    DEFER,
                    cancelTransmissionRequest();
                    computeRemainingBackoffSlots();
                    );
            FSMA_Event_Transition(Internal-collision,
                    event == INTERNAL_COLLISION,
                    IDLE,
                    finallyReportInternalCollision = true;
                    );
            FSMA_Event_Transition(Use-EIFS,
                    event == CORRUPTED_FRAME_RECEIVED,
                    IFS_AND_BACKOFF,
                    switchToEifs();
                    );
            FSMA_Fail_On_Unhandled_Event();
        }
        FSMA_State(OWNING) {
            FSMA_Event_Transition(Channel-Released,
                    event == CHANNEL_RELEASED,
                    IDLE,
                    ;
                    );
            FSMA_Ignore_Event(event==MEDIUM_STATE_CHANGED);
            FSMA_Fail_On_Unhandled_Event();
        }
    }
    // emit(stateSignal, fsm.getState()); TODO
    if (finallyReportChannelAccessGranted)
        reportChannelAccessGranted();
    if (finallyReportInternalCollision)
        reportInternalCollision();
    if (hasGUI())
        updateDisplayString();
}

void BasicContention::mediumStateChanged(bool mediumFree)
{
    Enter_Method_Silent(mediumFree ? "medium FREE" : "medium BUSY");
    this->mediumFree = mediumFree;
    channelLastBusyTime = simTime();
    handleWithFSM(MEDIUM_STATE_CHANGED, nullptr);
}

void BasicContention::handleMessage(cMessage *msg)
{
    ASSERT(msg == startTxEvent);
    transmissionGranted(txIndex);
}

void BasicContention::corruptedFrameReceived()
{
    Enter_Method("corruptedFrameReceived()");
    handleWithFSM(CORRUPTED_FRAME_RECEIVED, nullptr);
}

void BasicContention::transmissionGranted(int txIndex)
{
    Enter_Method("transmissionGranted()");
    handleWithFSM(TRANSMISSION_GRANTED, nullptr);
}

void BasicContention::internalCollision(int txIndex)
{
    Enter_Method("internalCollision()");
    handleWithFSM(INTERNAL_COLLISION, nullptr);
}

void BasicContention::channelReleased()
{
    Enter_Method("channelReleased()");
    handleWithFSM(CHANNEL_RELEASED, nullptr);
}

void BasicContention::scheduleTransmissionRequestFor(simtime_t txStartTime)
{
    if (collisionController)
        collisionController->scheduleTransmissionRequest(txIndex, txStartTime, this);
    else
        scheduleAt(txStartTime, startTxEvent);
}

void BasicContention::cancelTransmissionRequest()
{
    if (collisionController)
        collisionController->cancelTransmissionRequest(txIndex);
    else
        cancelEvent(startTxEvent);
}

void BasicContention::scheduleTransmissionRequest()
{
    ASSERT(mediumFree);

    simtime_t now = simTime();
    bool useEifs = endEifsTime > now + ifs;
    simtime_t waitInterval = (useEifs ? eifs : ifs) + backoffSlots * slotTime;

    if (retryCount == 0 && initialBackoffOptimization) {
        // we can pretend the frame has arrived into the queue a little bit earlier, and may be able to start transmitting immediately
        simtime_t elapsedFreeChannelTime = now - channelLastBusyTime;
        if (elapsedFreeChannelTime > waitInterval)
            waitInterval = 0;
        else
            waitInterval -= elapsedFreeChannelTime;
    }
    scheduledTransmissionTime = now + waitInterval;
    scheduleTransmissionRequestFor(scheduledTransmissionTime);
}

void BasicContention::switchToEifs()
{
    endEifsTime = simTime() + eifs;
    cancelTransmissionRequest();
    scheduleTransmissionRequest();
}

void BasicContention::computeRemainingBackoffSlots()
{
    simtime_t remainingTime = scheduledTransmissionTime - simTime();
    int remainingSlots = (int)ceil(remainingTime / slotTime);  //TODO this is not accurate
    if (remainingSlots < backoffSlots) { // don't count IFS
        ASSERT(remainingSlots >= 0);
        EV_DEBUG << "old backoff[" << getIndex() << "] period is " << backoffSlots * slotTime << ", remainingTime " << remainingTime << endl;
        backoffSlots = remainingSlots;
        EV_DEBUG << "backoff[" << getIndex() << "] period decreased to " << backoffSlots * slotTime << endl;
    }
}

void BasicContention::reportChannelAccessGranted()
{
    upperMac->channelAccessGranted(callback, txIndex);
}

void BasicContention::reportInternalCollision()
{
    upperMac->internalCollision(callback, txIndex);
}

const char *BasicContention::getEventName(EventType event)
{
#define CASE(x)   case x: return #x;
    switch (event) {
        CASE(START);
        CASE(MEDIUM_STATE_CHANGED);
        CASE(CORRUPTED_FRAME_RECEIVED);
        CASE(TRANSMISSION_GRANTED);
        CASE(INTERNAL_COLLISION);
        CASE(CHANNEL_RELEASED);
        default: ASSERT(false); return "";
    }
#undef CASE
}

void BasicContention::updateDisplayString()
{
    const char *stateName = fsm.getStateName();
    if (strcmp(stateName, "IFS_AND_BACKOFF") == 0)
        stateName = "IFS+BKOFF";  // original text is too long
    getDisplayString().setTagArg("t", 0, stateName);
}

bool BasicContention::isOwning()
{
    return fsm.getState() == OWNING;
}

} // namespace ieee80211
} // namespace inet

