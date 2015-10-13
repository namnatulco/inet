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
#include "inet/common/OSGUtils.h"
#include "inet/environment/contract/IPhysicalEnvironment.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/visualizer/networknode/NetworkNodeOsgVisualizer.h"
#include "inet/visualizer/scene/SceneOsgEarthVisualizer.h"
#include <osg/Group>
#include <osgDB/ReadFile>
#include <osgEarth/Capabilities>
#include <osgEarth/Viewpoint>
#include <osgEarthAnnotation/RectangleNode>

namespace inet {

namespace visualizer {

Define_Module(SceneOsgEarthVisualizer);

#ifdef WITH_OSG

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace inet::physicalenvironment;

void SceneOsgEarthVisualizer::initialize(int stage)
{
    SceneVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        playgroundLatitude = par("playgroundLatitude");
        playgroundLongitude = par("playgroundLongitude");
        playgroundOrientation = par("playgroundOrientation");
        cameraDistanceFactor = par("cameraDistanceFactor");
        initializeScene();
    }
    else if (stage == INITSTAGE_LAST) {
        initializeViewpoint();
    }
}

void SceneOsgEarthVisualizer::initializeScene()
{
    const char* sceneMapString = par("sceneMap");
    auto sceneMap = osgDB::readNodeFile(sceneMapString);
    if (sceneMap == nullptr)
        throw cRuntimeError("Could not read earth map file '%s'", sceneMapString);
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    osgCanvas->setScene(sceneMap);
    osgCanvas->setViewerStyle(cOsgCanvas::STYLE_EARTH);
    mapNode = MapNode::findMapNode(sceneMap);
    if (mapNode == nullptr)
        throw cRuntimeError("Could not find map node");
    locatorNode = new osgEarth::Util::ObjectLocatorNode(mapNode->getMap());
    locatorNode->getLocator()->setPosition(osg::Vec3d(playgroundLongitude, playgroundLatitude, 1.5));
    locatorNode->getLocator()->setOrientation(osg::Vec3d(playgroundOrientation, 0.0, 0.0));
    locatorNode->getLocator()->getLocatorMatrix(locatorMatrix);
    mapNode->getModelLayerGroup()->addChild(locatorNode);
}

void SceneOsgEarthVisualizer::initializeViewpoint()
{
    auto mapSrs = mapNode->getMapSRS();
    auto boundingSphere = getNetworkBoundingSphere();
    auto radius = boundingSphere.radius();
    auto euclideanCenter = boundingSphere.center();
    auto mapSrsEye = osg::Vec4d(euclideanCenter.x() + radius, euclideanCenter.y() + radius, euclideanCenter.z() + radius, 1.0) * locatorMatrix;
    osg::Vec3d geographicSrsEye;
    mapSrs->getECEF()->transform(osg::Vec3d(mapSrsEye.x(), mapSrsEye.y(), mapSrsEye.z()), mapSrs->getGeographicSRS(), geographicSrsEye);
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    osgCanvas->setEarthViewpoint(osgEarth::Viewpoint(geographicSrsEye.x(), geographicSrsEye.y(), geographicSrsEye.z(), playgroundOrientation - 135, -45, cameraDistanceFactor * radius, mapSrs->getGeographicSRS()));
}

osg::Group *SceneOsgEarthVisualizer::getMainPart()
{
    return locatorNode;
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

