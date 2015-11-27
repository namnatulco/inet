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

#ifndef __INET_EDCAUPPERMAC_H
#define __INET_EDCAUPPERMAC_H

#include "IUpperMac.h"
#include "IFrameExchange.h"
#include "AccessCategory.h"

namespace inet {
namespace ieee80211 {

class IRx;
class ITxCallback;
class Ieee80211NewMac;
class Ieee80211RTSFrame;
class IMacQoSClassifier;
class IMacParameters;
class MacUtils;
class IImmediateTx;
class IContentionTx;
class IDuplicateDetector;
class IFragmenter;
class IReassembly;

/**
 * UpperMac for EDCA (802.11e QoS mode)
 */
class INET_API EdcaUpperMac : public cSimpleModule, public IUpperMac, protected IFrameExchange::IFinishedCallback
{
    public:
        typedef std::list<Ieee80211DataOrMgmtFrame*> Ieee80211DataOrMgmtFrameList;

    protected:
        IMacParameters *params;
        MacUtils *utils;
        Ieee80211NewMac *mac = nullptr;
        IRx *rx = nullptr;
        IImmediateTx *immediateTx;
        IContentionTx **contentionTx;

        int maxQueueSize;
        int fragmentationThreshold = 2346;

        struct AccessCategoryData {
            cQueue transmissionQueue;
            IFrameExchange *frameExchange = nullptr;
        };
        AccessCategoryData *acData = nullptr;  // dynamically allocated array

        IDuplicateDetector *duplicateDetection = nullptr;
        IFragmenter *fragmenter = nullptr;
        IReassembly *reassembly = nullptr;

    protected:
        void initialize() override;
        virtual void readParameters();
        virtual void handleMessage(cMessage *msg) override;

        virtual AccessCategory classifyFrame(Ieee80211DataOrMgmtFrame *frame);
        virtual AccessCategory mapTidToAc(int tid);
        virtual void enqueue(Ieee80211DataOrMgmtFrame *frame, AccessCategory ac);
        virtual void startSendDataFrameExchange(Ieee80211DataOrMgmtFrame *frame, int txIndex, AccessCategory ac);
        virtual void frameExchangeFinished(IFrameExchange *what, bool successful) override;

        void sendAck(Ieee80211DataOrMgmtFrame *frame);
        void sendCts(Ieee80211RTSFrame *frame);

    public:
        EdcaUpperMac();
        ~EdcaUpperMac();
        virtual void upperFrameReceived(Ieee80211DataOrMgmtFrame *frame) override;
        virtual void lowerFrameReceived(Ieee80211Frame *frame) override;
        virtual void transmissionComplete(ITxCallback *callback, int txIndex) override;
        virtual void internalCollision(ITxCallback *callback, int txIndex) override;
};

} // namespace ieee80211
} // namespace inet

#endif

