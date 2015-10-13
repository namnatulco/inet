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

#include "inet/common/OSGUtils.h"
#include "inet/visualizer/networklayer/NetworkLayerOsgVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(NetworkLayerOsgVisualizer);

#ifdef WITH_OSG

void NetworkLayerOsgVisualizer::initialize(int stage)
{
    NetworkLayerVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
}

void NetworkLayerOsgVisualizer::receiveSignal(cComponent *source, simsignal_t signal, cObject *object)
{
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

