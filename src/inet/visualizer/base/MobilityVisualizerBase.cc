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

#include "inet/common/ModuleAccess.h"
#include "inet/visualizer/base/MobilityVisualizerBase.h"

namespace inet {

namespace visualizer {

MobilityVisualizerBase::~MobilityVisualizerBase()
{
    if (!hasGUI()) return;
    mobilitySubscriptionModule->unsubscribe(IMobility::mobilityStateChangedSignal, this);
}

void MobilityVisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        mobilitySubscriptionModule = *par("mobilitySubscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("mobilitySubscriptionModule"), this);
        mobilitySubscriptionModule->subscribe(IMobility::mobilityStateChangedSignal, this);
        leaveMovementTrail = par("leaveMovementTrail");
    }
}

} // namespace visualizer

} // namespace inet

