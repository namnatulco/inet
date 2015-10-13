//
// Copyright (C) 2013 OpenSim Ltd.
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

#include "inet/common/LayeredProtocolBase.h"
#include "inet/common/ModuleAccess.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/visualizer/base/NetworkLayerVisualizerBase.h"

namespace inet {

namespace visualizer {

void NetworkLayerVisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        networkLayerSubscriptionModule = *par("networkLayerSubscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("networkLayerSubscriptionModule"), this);
        networkLayerSubscriptionModule->subscribe(LayeredProtocolBase::packetSentToUpperSignal, this);
        networkLayerSubscriptionModule->subscribe(LayeredProtocolBase::packetReceivedFromUpperSignal, this);
        networkLayerSubscriptionModule->subscribe(IMobility::mobilityStateChangedSignal, this);
        packetNameMatcher.setPattern(par("packetNameFilter"), false, true, true);
    }
}

} // namespace visualizer

} // namespace inet

