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

#ifndef __INET_MOBILITYVISUALIZERBASE_H
#define __INET_MOBILITYVISUALIZERBASE_H

#include "inet/mobility/contract/IMobility.h"
#include "inet/visualizer/base/VisualizerBase.h"

namespace inet {

namespace visualizer {

class INET_API MobilityVisualizerBase : public VisualizerBase, public cListener
{
  protected:
    /** @name Parameters */
    //@{
    cModule *mobilitySubscriptionModule = nullptr;
    bool leaveMovementTrail = false;
    //@}

  protected:
    virtual void initialize(int stage) override;

  public:
    virtual ~MobilityVisualizerBase();
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_MOBILITYVISUALIZERBASE_H

