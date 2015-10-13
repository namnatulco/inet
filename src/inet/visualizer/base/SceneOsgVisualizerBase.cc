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
#include "inet/visualizer/base/SceneOsgVisualizerBase.h"
#include "inet/visualizer/networknode/NetworkNodeOsgVisualizer.h"
#include <osg/Geode>
#include <osg/Shape>
#include <osg/ShapeDrawable>

namespace inet {

namespace visualizer {

osg::BoundingSphere SceneOsgVisualizerBase::getNetworkBoundingSphere()
{
    auto nodes = new osg::Group();
    auto networkNodeVisualizer = getModuleFromPar<NetworkNodeOsgVisualizer>(par("networkNodeVisualizerModule"), this);
    for (cModule::SubmoduleIterator it(getSystemModule()); !it.end(); it++) {
        auto networkNode = *it;
        if (isNetworkNode(networkNode)) {
            // NOTE: ignore network node annotations
            auto visualRepresentation = networkNodeVisualizer->getNeworkNodeVisualization(networkNode);
            auto mainNode = visualRepresentation->getMainPart();
            auto radius = std::max(0.0f, mainNode->computeBound().radius());
            auto drawable = new osg::ShapeDrawable(new osg::Sphere(visualRepresentation->getPosition(), radius));
            auto geode = new osg::Geode();
            geode->addDrawable(drawable);
            nodes->addChild(geode);
        }
    }
    return nodes->getBound();
}

} // namespace visualizer

} // namespace inet

