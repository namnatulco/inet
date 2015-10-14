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

#ifndef __INET_NETWORKLAYERCANVASVISUALIZER_H
#define __INET_NETWORKLAYERCANVASVISUALIZER_H

#include "inet/visualizer/base/NetworkLayerVisualizerBase.h"

namespace inet {

namespace visualizer {

/**
 * This class provides the visualization of the network layer communication.
 */
class INET_API NetworkLayerCanvasVisualizer : public NetworkLayerVisualizerBase
{
  protected:
    virtual void initialize(int stage) override;

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object) override;
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_NETWORKLAYERCANVASVISUALIZER_H

