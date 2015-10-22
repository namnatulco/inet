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

#ifndef __INET_STATISTICOSGVISUALIZER_H
#define __INET_STATISTICOSGVISUALIZER_H

#include "inet/visualizer/base/StatisticVisualizerBase.h"
#include <osg/Node>

namespace inet {

namespace visualizer {

/**
 * This class provides the visualization of the network layer communication.
 */
class INET_API StatisticOsgVisualizer : public StatisticVisualizerBase
{
#ifdef WITH_OSG
  protected:
    std::map<std::pair<int, int>, osg::Node *> visualizations;

  protected:
    virtual void initialize(int stage) override;

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object) override;

#else // ifdef WITH_OSG

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object) override {}

#endif // ifdef WITH_OSG
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_STATISTICOSGVISUALIZER_H

