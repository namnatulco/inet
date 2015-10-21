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

namespace inet {

namespace visualizer {

Define_Module(SceneOsgEarthVisualizer);

#ifdef WITH_OSG

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace inet::physicalenvironment;

void SceneOsgEarthVisualizer::initialize(int stage)
{
    SceneOsgVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        playgroundLatitude = par("playgroundLatitude");
        playgroundLongitude = par("playgroundLongitude");
        playgroundOrientation = par("playgroundOrientation");
        cameraDistanceFactor = par("cameraDistanceFactor");
        initializeScene();
    }
    else if (stage == INITSTAGE_LAST) {
        if (par("displayPlayground"))
            initializePlayground();
        initializeViewpoint();
//        const char *convertGeographicLocationsToPlaygroundPositions = par("convertGeographicLocationsToPlaygroundPositions");
//        if (convertGeographicLocationsToPlaygroundPositions != nullptr) {
//            cStringTokenizer t1(convertGeographicLocationsToPlaygroundPositions, ";");
//            while (t1.hasMoreTokens()) {
//                const char *token = t1.nextToken();
//                cStringTokenizer t2(token, ",");
//                const char *name = t2.nextToken();
//                double latitude = atof(t2.nextToken());
//                double longitude = atof(t2.nextToken());
//                auto mapSrs = mapNode->getMapSRS();
//                osg::Vec3d ecefSrsPoint;
//                mapSrs->getGeographicSRS()->transform(osg::Vec3d(longitude, latitude, 0.0), mapSrs->getECEF(), ecefSrsPoint);
//                osg::Matrixd inverseLocatorMatrix;
//                inverseLocatorMatrix.invert(locatorMatrix);
//                auto playgroundPoint = osg::Vec4d(ecefSrsPoint.x(), ecefSrsPoint.y(), ecefSrsPoint.z(), 1.0) * inverseLocatorMatrix;
//                double x = playgroundPoint.x();
//                double y = playgroundPoint.y();
//                std::cout << "Converted " << name << " geographic location " << longitude << ", " << latitude << " to playground position " << x << ", " << y << endl;
//            }
//        }
    }
}

void SceneOsgEarthVisualizer::initializeScene()
{
    const char* mapFileString = par("mapFile");
    auto mapScene = osgDB::readNodeFile(mapFileString);
    if (mapScene == nullptr)
        throw cRuntimeError("Could not read earth map file '%s'", mapFileString);
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    osgCanvas->setScene(mapScene);
    osgCanvas->setViewerStyle(cOsgCanvas::STYLE_EARTH);
    mapNode = MapNode::findMapNode(mapScene);
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
    auto geographicSrsEye = computeGeographicCoordinate(Coord(euclideanCenter.x() + radius, euclideanCenter.y() + radius, euclideanCenter.z() + radius));
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    osgCanvas->setEarthViewpoint(osgEarth::Viewpoint(geographicSrsEye.x(), geographicSrsEye.y(), geographicSrsEye.z(), playgroundOrientation - 135, -45, cameraDistanceFactor * radius, mapSrs->getGeographicSRS()));
}

Coord SceneOsgEarthVisualizer::computePlaygroundCoordinate(const osg::Vec3d& geographicCoordinate)
{
    auto mapSrs = mapNode->getMapSRS();
    osg::Vec3d ecefCoordinate;
    mapSrs->getGeographicSRS()->transform(osg::Vec3d(geographicCoordinate.x(), geographicCoordinate.y(), geographicCoordinate.z()), mapSrs->getECEF(), ecefCoordinate);
    osg::Matrixd inverseLocatorMatrix;
    inverseLocatorMatrix.invert(locatorMatrix);
    auto playgroundCoordinate = osg::Vec4d(ecefCoordinate.x(), ecefCoordinate.y(), ecefCoordinate.z(), 1.0) * inverseLocatorMatrix;
    return Coord(playgroundCoordinate.x(), playgroundCoordinate.y(), playgroundCoordinate.z());
}

osg::Vec3d SceneOsgEarthVisualizer::computeGeographicCoordinate(const Coord& playgroundCoordinate)
{
    auto ecefCoordinate = osg::Vec4d(playgroundCoordinate.x, playgroundCoordinate.y, playgroundCoordinate.z, 1.0) * locatorMatrix;
    auto mapSrs = mapNode->getMapSRS();
    osg::Vec3d geographicCoordinate;
    mapSrs->getECEF()->transform(osg::Vec3d(ecefCoordinate.x(), ecefCoordinate.y(), ecefCoordinate.z()), mapSrs->getGeographicSRS(), geographicCoordinate);
    return geographicCoordinate;
}

osg::Group *SceneOsgEarthVisualizer::getMainPart()
{
    return locatorNode;
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

