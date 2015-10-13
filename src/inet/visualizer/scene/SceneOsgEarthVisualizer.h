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

#ifndef __INET_SCENEOSGEARTHVISUALIZER_H
#define __INET_SCENEOSGEARTHVISUALIZER_H

#include "inet/visualizer/base/SceneOsgVisualizerBase.h"
#include <osgEarth/MapNode>
#include <osgEarthUtil/ObjectLocator>

namespace inet {

namespace visualizer {

/**
 * This class provides the visualization of the scene using osg earth.
 */
class INET_API SceneOsgEarthVisualizer : public SceneOsgVisualizerBase
{
#ifdef WITH_OSG

  protected:
    double playgroundLatitude;
    double playgroundLongitude;
    double playgroundOrientation;
    double cameraDistanceFactor;

    osgEarth::MapNode *mapNode;
    osgEarth::Util::ObjectLocatorNode *locatorNode;
    osg::Matrixd locatorMatrix;

  protected:
    virtual void initialize(int stage) override;
    virtual void initializeScene();
    virtual void initializeViewpoint();

    // https://en.wikipedia.org/wiki/Decimal_degrees
    // TODO: consider playgroundOrientation
    virtual double toLatitude(double y) const { return playgroundLatitude - y / 111320; }
    virtual double toLongitude(double x) const { return playgroundLongitude +  x / 111320 / cos(fabs(playgroundLatitude / 180 * M_PI)); }
    virtual double toY(double latitude) const {  return (playgroundLatitude - latitude) * 111320; }
    virtual double toX(double longitude) const { return (longitude - playgroundLongitude) * cos(fabs(playgroundLatitude / 180 * M_PI)) * 111111; }

  public:
    virtual osg::Group *getMainPart() override;

#endif // ifdef WITH_OSG
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_SCENEOSGEARTHVISUALIZER_H

