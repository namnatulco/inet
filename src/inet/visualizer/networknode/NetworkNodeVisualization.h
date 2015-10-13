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

#ifndef __INET_NETWORKNODEVISUALIZATION_H
#define __INET_NETWORKNODEVISUALIZATION_H

#include "inet/common/INETDefs.h"
#include <osg/Group>
#include <osg/PositionAttitudeTransform>

namespace inet {

namespace visualizer {

#ifdef WITH_OSG

class INET_API NetworkNodeVisualization : public osg::PositionAttitudeTransform
{
  public:
    NetworkNodeVisualization(osg::Node *mainNode);

    virtual osg::Node *getMainPart() { return getChild(0); }
    virtual osg::Group *getAnnotationPart() { return static_cast<osg::Group *>(getChild(1)); }
};

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_NETWORKNODEVISUALIZATION_H

