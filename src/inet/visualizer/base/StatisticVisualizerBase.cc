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
#include "inet/visualizer/base/StatisticVisualizerBase.h"

namespace inet {

namespace visualizer {

void StatisticVisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        const char *statisticNameFilter = par("statisticNameFilter");
        statisticNameMatcher.setPattern(statisticNameFilter, false, true, true);
        statisticSubscriptionModule = *par("statisticSubscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("statisticSubscriptionModule"), this);
        statisticSubscriptionModule->subscribe(registerSignal(statisticNameFilter), this);
    }
}

} // namespace visualizer

} // namespace inet

