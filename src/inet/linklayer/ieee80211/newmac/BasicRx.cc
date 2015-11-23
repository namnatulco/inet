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

#include "BasicRx.h"
#include "IContention.h"
#include "ITx.h"
#include "IUpperMac.h"
#include "IStatistics.h"

namespace inet {
namespace ieee80211 {

Define_Module(BasicRx);

BasicRx::BasicRx()
{
}

BasicRx::~BasicRx()
{
    delete cancelEvent(endNavTimer);
}

void BasicRx::initialize()
{
    upperMac = check_and_cast<IUpperMac *>(getModuleByPath(par("upperMacModule")));
    collectContentionModules(getModuleByPath(par("firstContentionModule")), contention);
    statistics = check_and_cast<IStatistics *>(getModuleByPath(par("statisticsModule")));
    endNavTimer = new cMessage("NAV");
    recomputeMediumFree();

    WATCH(address);
    WATCH(receptionState);
    WATCH(transmissionState);
    WATCH(mediumFree);
}

void BasicRx::handleMessage(cMessage *msg)
{
    if (msg == endNavTimer) {
        EV_INFO << "The radio channel has become free according to the NAV" << std::endl;
        recomputeMediumFree();
    }
    else
        throw cRuntimeError("Unexpected self message");
}

void BasicRx::lowerFrameReceived(Ieee80211Frame *frame)
{
    Enter_Method("lowerFrameReceived(\"%s\")", frame->getName());
    take(frame);

    bool isFrameOk = isFcsOk(frame);
    if (isFrameOk) {
        EV_INFO << "Received frame from PHY: " << frame << endl;
        if (frame->getReceiverAddress() != address)
            setOrExtendNav(frame->getDuration());
        statistics->frameReceived(frame);
        upperMac->lowerFrameReceived(frame);
    }
    else {
        EV_INFO << "Received an erroneous frame from PHY, dropping it." << std::endl;
        delete frame;
        for (int i = 0; contention[i]; i++)
            contention[i]->corruptedFrameReceived();
        upperMac->corruptedFrameReceived();
        statistics->erroneousFrameReceived();
    }
}

void BasicRx::frameTransmitted(simtime_t durationField)
{
    // the txIndex that transmitted the frame should already own the TXOP, so
    // it has no need to (and should not) check the NAV.
    setOrExtendNav(durationField);
}

bool BasicRx::isReceptionInProgress() const
{
    return receptionState == IRadio::RECEPTION_STATE_RECEIVING;
}

bool BasicRx::isFcsOk(Ieee80211Frame *frame) const
{
    return !frame->hasBitError();
}

void BasicRx::recomputeMediumFree()
{
    bool oldMediumFree = mediumFree;
    // note: the duration of mode switching (rx-to-tx or tx-to-rx) should also count as busy
    mediumFree = receptionState == IRadio::RECEPTION_STATE_IDLE && transmissionState == IRadio::TRANSMISSION_STATE_UNDEFINED && !endNavTimer->isScheduled();
    updateDisplayString();
    if (mediumFree != oldMediumFree) {
        for (int i = 0; contention[i]; i++)
            contention[i]->mediumStateChanged(mediumFree);
    }
}

void BasicRx::receptionStateChanged(IRadio::ReceptionState state)
{
    Enter_Method_Silent();
    receptionState = state;
    //std::cout << "Now =" << simTime() << " Reception state changed to " << state << std::endl;
    recomputeMediumFree();
}

void BasicRx::transmissionStateChanged(IRadio::TransmissionState state)
{
    Enter_Method_Silent();
    transmissionState = state;
    std::cout << "Now =" << simTime() << " Transmission state changed to " << state << std::endl;
    recomputeMediumFree();
}

void BasicRx::setOrExtendNav(simtime_t navInterval)
{
    ASSERT(navInterval >= 0);
    if (navInterval > 0) {
        simtime_t endNav = simTime() + navInterval;
        if (endNavTimer->isScheduled()) {
            simtime_t oldEndNav = endNavTimer->getArrivalTime();
            if (endNav < oldEndNav)
                return;    // never decrease NAV
            cancelEvent(endNavTimer);
        }
        EV_INFO << "Setting NAV to " << navInterval << std::endl;
        scheduleAt(endNav, endNavTimer);
        recomputeMediumFree();
    }
}

void BasicRx::updateDisplayString()
{
    if (mediumFree)
        getDisplayString().setTagArg("t", 0, "FREE");
    else {
        std::stringstream os;
        os << "BUSY (";
        bool addSpace = false;
        if (transmissionState != IRadio::TRANSMISSION_STATE_UNDEFINED) {
            switch (transmissionState) {
                case IRadio::IRadio::TRANSMISSION_STATE_UNDEFINED: break; // cannot happen
                case IRadio::IRadio::TRANSMISSION_STATE_IDLE: os << "Tx-Idle"; break;
                case IRadio::IRadio::TRANSMISSION_STATE_TRANSMITTING: os << "Tx"; break;
            }
            addSpace = true;
        }
        else {
            switch (receptionState) {
                case IRadio::RECEPTION_STATE_UNDEFINED: os << "Switching"; break;
                case IRadio::RECEPTION_STATE_IDLE: os << "Rx-Idle"; break; // cannot happen
                case IRadio::RECEPTION_STATE_BUSY: os << "Noise"; break;
                case IRadio::RECEPTION_STATE_RECEIVING: os << "Recv"; break;
                case IRadio::RECEPTION_STATE_SYNCHRONIZING: os << "Syncing"; break;
            }
            addSpace = true;
        }
        if (endNavTimer->isScheduled()) {
            os << (addSpace ? " " : "") << "NAV";
        }
        os << ")";
        getDisplayString().setTagArg("t", 0, os.str().c_str());
    }
}

} // namespace ieee80211
} // namespace inet

