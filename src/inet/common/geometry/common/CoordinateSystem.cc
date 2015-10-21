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

#include "inet/common/geometry/common/CoordinateSystem.h"
#include <osgEarthUtil/ObjectLocator>

namespace inet {

Define_Module(SimpleGeographicCoordinateSystem);

void SimpleGeographicCoordinateSystem::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        playgroundLongitude = par("playgroundLongitude");
        playgroundLatitude = par("playgroundLatitude");
    }
}

Coord SimpleGeographicCoordinateSystem::computePlaygroundCoordinate(const Coord& geographicCoordinate)
{
    double playgroundX = (geographicCoordinate.x - playgroundLongitude) * cos(fabs(playgroundLatitude / 180 * M_PI)) * metersPerDegree;
    double playgroundY = (playgroundLatitude - geographicCoordinate.y) * metersPerDegree;
    return Coord(playgroundX, playgroundY, geographicCoordinate.z + playgroundAltitude);
}

Coord SimpleGeographicCoordinateSystem::computeGeographicCoordinate(const Coord& playgroundCoordinate)
{
    double geograpicLongitude = playgroundLongitude + playgroundCoordinate.x / metersPerDegree / cos(fabs(playgroundLatitude / 180 * M_PI));
    double geograpicLatitude = playgroundLatitude - playgroundCoordinate.y / metersPerDegree;
    return Coord(geograpicLongitude, geograpicLatitude, playgroundCoordinate.z - playgroundAltitude);
}

Define_Module(OsgGeographicCoordinateSystem);

void OsgGeographicCoordinateSystem::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        // TODO: lookup map node
        auto mapScene = osgDB::readNodeFile("boston.earth");
        mapNode = osgEarth::MapNode::findMapNode(mapScene);
        double playgroundLongitude = par("playgroundLongitude");
        double playgroundLatitude = par("playgroundLatitude");
        double playgroundAltitude = par("playgroundAltitude");
        double playgroundOrientation = par("playgroundOrientation");
        auto locatorNode = new osgEarth::Util::ObjectLocatorNode(mapNode->getMap());
        locatorNode->getLocator()->setPosition(osg::Vec3d(playgroundLongitude, playgroundLatitude, playgroundAltitude));
        locatorNode->getLocator()->setOrientation(osg::Vec3d(playgroundOrientation, 0.0, 0.0));
        locatorNode->getLocator()->getLocatorMatrix(locatorMatrix);
        inverseLocatorMatrix.invert(locatorMatrix);
        delete locatorNode;
    }
}

Coord OsgGeographicCoordinateSystem::computePlaygroundCoordinate(const Coord& geographicCoordinate)
{
    auto mapSrs = mapNode->getMapSRS();
    osg::Vec3d ecefCoordinate;
    mapSrs->getGeographicSRS()->transform(osg::Vec3d(geographicCoordinate.x, geographicCoordinate.y, geographicCoordinate.z), mapSrs->getECEF(), ecefCoordinate);
    auto playgroundCoordinate = osg::Vec4d(ecefCoordinate.x(), ecefCoordinate.y(), ecefCoordinate.z(), 1.0) * inverseLocatorMatrix;
    return Coord(playgroundCoordinate.x(), playgroundCoordinate.y(), playgroundCoordinate.z());
}

Coord OsgGeographicCoordinateSystem::computeGeographicCoordinate(const Coord& playgroundCoordinate)
{
    auto ecefCoordinate = osg::Vec4d(playgroundCoordinate.x, playgroundCoordinate.y, playgroundCoordinate.z, 1.0) * locatorMatrix;
    auto mapSrs = mapNode->getMapSRS();
    osg::Vec3d geographicCoordinate;
    mapSrs->getECEF()->transform(osg::Vec3d(ecefCoordinate.x(), ecefCoordinate.y(), ecefCoordinate.z()), mapSrs->getGeographicSRS(), geographicCoordinate);
    return Coord(geographicCoordinate.x(), geographicCoordinate.y(), geographicCoordinate.z());
}

} // namespace inet

