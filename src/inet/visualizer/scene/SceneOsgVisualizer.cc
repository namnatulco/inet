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
#include "inet/visualizer/scene/SceneOsgVisualizer.h"
#include <osg/Group>
#include <osgDB/ReadFile>

namespace inet {

namespace visualizer {

Define_Module(SceneOsgVisualizer);

#ifdef WITH_OSG

void SceneOsgVisualizer::initialize(int stage)
{
    SceneOsgVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL)
        initializeScene();
    else if (stage == INITSTAGE_LAST) {
        if (par("displayPlayground"))
            initializePlayground();
        double axisLength = par("axisLength");
        if (!_isNaN(axisLength))
            initializeAxis(axisLength);
        initializeViewpoint();
    }
}

void SceneOsgVisualizer::initializeScene()
{
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    if (osgCanvas->getScene() != nullptr)
        throw cRuntimeError("OSG canvas scene at '%s' has been already initialized", visualizerTargetModule->getFullPath().c_str());
    else {
        auto scene = new osg::Group();
        osgCanvas->setScene(scene);
        osgCanvas->setClearColor(cFigure::Color(par("clearColor")));
        osgCanvas->setZNear(par("zNear"));
        osgCanvas->setZFar(par("zFar"));
        osgCanvas->setFieldOfViewAngle(par("fieldOfView"));
        const char *cameraManipulatorString = par("cameraManipulator");
        cOsgCanvas::CameraManipulatorType cameraManipulator;
        if (!strcmp(cameraManipulatorString, "auto"))
            cameraManipulator = cOsgCanvas::CAM_AUTO;
        else if (!strcmp(cameraManipulatorString, "trackball"))
            cameraManipulator = cOsgCanvas::CAM_TRACKBALL;
        else if (!strcmp(cameraManipulatorString, "terrain"))
            cameraManipulator = cOsgCanvas::CAM_TERRAIN;
        else if (!strcmp(cameraManipulatorString, "overview"))
            cameraManipulator = cOsgCanvas::CAM_OVERVIEW;
        else if (!strcmp(cameraManipulatorString, "earth"))
            cameraManipulator = cOsgCanvas::CAM_EARTH;
        else
            throw cRuntimeError("Unknown camera manipulator: '%s'", cameraManipulatorString);
        osgCanvas->setCameraManipulatorType(cameraManipulator);
    }
}

void SceneOsgVisualizer::initializeAxis(double axisLength)
{
    auto geode = new osg::Geode();
    geode->addDrawable(inet::osg::createLineGeometry(Coord::ZERO, Coord(axisLength, 0.0, 0.0)));
    geode->addDrawable(inet::osg::createLineGeometry(Coord::ZERO, Coord(0.0, axisLength, 0.0)));
    geode->addDrawable(inet::osg::createLineGeometry(Coord::ZERO, Coord(0.0, 0.0, axisLength)));
    auto stateSet = inet::osg::createStateSet(cFigure::BLACK, 1.0);
    geode->setStateSet(stateSet);
    auto scene = inet::osg::getScene(visualizerTargetModule);
    scene->addChild(geode);
    double spacing = 1;
    scene->addChild(inet::osg::createAutoTransform(inet::osg::createText("X", Coord::ZERO, cFigure::BLACK), osg::AutoTransform::ROTATE_TO_SCREEN, true, Coord(axisLength + spacing, 0.0, 0.0)));
    scene->addChild(inet::osg::createAutoTransform(inet::osg::createText("Y", Coord::ZERO, cFigure::BLACK), osg::AutoTransform::ROTATE_TO_SCREEN, true, Coord(0.0, axisLength + spacing, 0.0)));
    scene->addChild(inet::osg::createAutoTransform(inet::osg::createText("Z", Coord::ZERO, cFigure::BLACK), osg::AutoTransform::ROTATE_TO_SCREEN, true, Coord(0.0, 0.0, axisLength + spacing)));
}

void SceneOsgVisualizer::initializeViewpoint()
{
    auto boundingSphere = getNetworkBoundingSphere();
    auto center = boundingSphere.center();
    auto radius = boundingSphere.radius();
    double cameraDistanceFactor = par("cameraDistanceFactor");
    auto eye = cOsgCanvas::Vec3d(center.x() + cameraDistanceFactor * radius, center.y() + cameraDistanceFactor * radius, center.z() + cameraDistanceFactor * radius);
    auto viewpointCenter = cOsgCanvas::Vec3d(center.x(), center.y(), center.z());
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    osgCanvas->setGenericViewpoint(cOsgCanvas::Viewpoint(eye, viewpointCenter, cOsgCanvas::Vec3d(0, 0, 1)));
}

osg::Group *SceneOsgVisualizer::getMainPart()
{
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    return check_and_cast<osg::Group *>(osgCanvas->getScene());
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

