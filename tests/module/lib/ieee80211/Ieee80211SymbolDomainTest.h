//
// Copyright (C) 2014 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_IEEE80211SYMBOLMODELTEST_H_
#define __INET_IEEE80211SYMBOLMODELTEST_H_

#include "inet/common/INETDefs.h"
#include "inet/physicallayer/ieee80211/layered/Ieee80211LayeredEncoder.h"
#include "inet/physicallayer/ieee80211/layered/Ieee80211OFDMModulator.h"
#include "inet/physicallayer/ieee80211/layered/Ieee80211OFDMDemodulator.h"
#include "inet/physicallayer/ieee80211/layered/Ieee80211LayeredDecoder.h"
#include "inet/physicallayer/common/DummySerializer.h"

using namespace inet::physicallayer;

namespace inet {

class INET_API Ieee80211SymbolDomainTest : public cSimpleModule
{
    protected:
        Ieee80211LayeredEncoder *ieee80211LayeredEncoder;
        Ieee80211OFDMModulator *ieee80211OFDMModulator;
        Ieee80211OFDMDemodulator *ieee80211OFDMDemodulator;
        Ieee80211LayeredDecoder *ieee80211LayeredDecoder;
        DummySerializer *serializer;
        BitVector input;

    protected:
        virtual int numInitStages() const { return NUM_INIT_STAGES; }
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg) { throw cRuntimeError("This module doesn't handle self messages"); }
        void parseInput(const char *fileName);

    public:
        void test() const;
};

} /* namespace inet */

#endif /* __INET_IEEE80211SYMBOLMODELTEST_H_ */