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
#include <osg/PolygonOffset>
#include <osgDB/ReadFile>

namespace inet {

namespace visualizer {

Define_Module(SceneOsgVisualizer);

#ifdef WITH_OSG

void SceneOsgVisualizer::initialize(int stage)
{
    SceneVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        initializeScene();
    }
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

void SceneOsgVisualizer::initializePlayground()
{
    auto scene = inet::osg::getScene(visualizerTargetModule);
    osg::Geode *playground = nullptr;
    Box playgroundBounds = getPlaygroundBounds();
    if (playgroundBounds.getMin() != playgroundBounds.getMax()) {
        playground = createPlayground(playgroundBounds.getMin(), playgroundBounds.getMax());
        scene->addChild(playground);
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

osg::Geode *SceneOsgVisualizer::createPlayground(const Coord& min, const Coord& max) const
{
    osg::Texture2D *texture = nullptr;
    const char *playgroundImageString = par("playgroundImage");
    if (*playgroundImageString != '\0') {
        auto image = inet::osg::createImage(playgroundImageString);
        if (image == nullptr)
            throw cRuntimeError("Cannot read playground image: '%s'", playgroundImageString);
        texture = new osg::Texture2D();
        texture->setImage(image);
        texture->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT);
        texture->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT);
    }
    double playgroundImageSize = par("playgroundImageSize");
    auto dx = max.x - min.x;
    auto dy = max.y - min.y;
    auto d = sqrt(dx * dx + dy * dy);
    auto width = dx + 2 * d;
    auto height = dy + 2 * d;
    auto r = width / playgroundImageSize / 2;
    auto t = height / playgroundImageSize / 2;
    auto geometry = osg::createTexturedQuadGeometry(osg::Vec3(min.x - d, min.y - d, 0.0), osg::Vec3(width, 0.0, 0.0), osg::Vec3(0.0, height, 0.0), -r, -t, r, t);
    auto color = cFigure::Color(par("playgroundColor"));
    auto stateSet = inet::osg::createStateSet(color, 1.0, false);
    auto center = (max + min) / 2;
    stateSet->setTextureAttributeAndModes(0, texture);
    stateSet->addUniform(new osg::Uniform("center", osg::Vec3(center.x, center.y, center.z)));
    stateSet->addUniform(new osg::Uniform("min", (float)d / 2));
    stateSet->addUniform(new osg::Uniform("max", (float)d));
    auto program = new osg::Program();
    auto vertexShader = new osg::Shader(osg::Shader::VERTEX);
    auto fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
    vertexShader->setShaderSource(R"(
        varying vec4 verpos;
        void main() {
            gl_Position = ftransform();
            verpos = gl_Vertex;
            gl_TexCoord[0]=gl_MultiTexCoord0;
        })");
    if (texture != nullptr) {
        fragmentShader->setShaderSource(R"(
            varying vec4 verpos;
            uniform vec3 center;
            uniform float min, max;
            uniform sampler2D texture;
            void main(void) {
                float alpha = 1.0 - smoothstep(min, max, length(verpos.xyz - center));
                gl_FragColor = vec4(texture2D(texture, gl_TexCoord[0].xy).rgb, alpha);
            })");
        stateSet->addUniform(new osg::Uniform("texture", 0));
    }
    else {
        fragmentShader->setShaderSource(R"(
            varying vec4 verpos;
            uniform vec3 center;
            uniform vec3 color;
            uniform float min, max;
            void main(void) {
                float alpha = 1.0 - smoothstep(min, max, length(verpos.xyz - center));
                gl_FragColor = vec4(color, alpha);
            })");
        stateSet->addUniform(new osg::Uniform("color", osg::Vec3((double)color.red / 255.0, (double)color.green / 255.0, (double)color.blue / 255.0)));
    }
    program->addShader(vertexShader);
    program->addShader(fragmentShader);
    stateSet->setAttributeAndModes(program, osg::StateAttribute::ON);
    geometry->setStateSet(stateSet);
    auto polygonOffset = new osg::PolygonOffset();
    polygonOffset->setFactor(1.0);
    polygonOffset->setUnits(1.0);
    stateSet->setAttributeAndModes(polygonOffset, osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
    auto geode = new osg::Geode();
    geode->addDrawable(geometry);
    return geode;
}

osg::Group *SceneOsgVisualizer::getMainPart()
{
    auto osgCanvas = visualizerTargetModule->getOsgCanvas();
    return check_and_cast<osg::Group *>(osgCanvas->getScene());
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

