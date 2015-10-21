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
#include <osgEarthAnnotation/RectangleNode>
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
    double playgroundLongitude = NaN;
    double playgroundLatitude = NaN;
    double playgroundOrientation = NaN;
    double cameraDistanceFactor = NaN;

    osgEarth::MapNode *mapNode = nullptr;
    osgEarth::Util::ObjectLocatorNode *locatorNode = nullptr;
    osg::Matrixd locatorMatrix;

  protected:
    virtual void initialize(int stage) override;
    virtual void initializeScene();
    virtual void initializeViewpoint();

    virtual Coord computePlaygroundCoordinate(const osg::Vec3d& geographicCoordinate);
    virtual osg::Vec3d computeGeographicCoordinate(const Coord& playgroundCoordinate);

  public:
    virtual osg::Group *getMainPart() override;

#endif // ifdef WITH_OSG
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_SCENEOSGEARTHVISUALIZER_H

