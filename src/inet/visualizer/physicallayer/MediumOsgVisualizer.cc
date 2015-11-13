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
#include "inet/physicallayer/common/packetlevel/RadioMedium.h"
#include "inet/physicallayer/pathloss/FreeSpacePathLoss.h"
#include "inet/visualizer/physicallayer/MediumOsgVisualizer.h"
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/ImageStream>
#include <osg/Material>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

namespace inet {

namespace visualizer {

Define_Module(MediumOsgVisualizer);

#ifdef WITH_OSG

MediumOsgVisualizer::~MediumOsgVisualizer()
{
    cancelAndDelete(updateSceneTimer);
}

void MediumOsgVisualizer::initialize(int stage)
{
    MediumVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        displaySignals = par("displaySignals");
        signalOpacityExponent = par("signalOpacityExponent");
        const char *signalShapeString = par("signalShape");
        if (!strcmp(signalShapeString, "ring"))
            signalShape = SIGNAL_SHAPE_RING;
        else if (!strcmp(signalShapeString, "sphere"))
            signalShape = SIGNAL_SHAPE_SPHERE;
        else
            throw cRuntimeError("Unknown signalShape parameter value: '%s'", signalShapeString);
        signalPlane = par("signalPlane");
        displayTransmissions = par("displayTransmissions");
        if (displayTransmissions) {
            const char *transmissionImageString = par("transmissionImage");
            transmissionImage = inet::osg::createImage(transmissionImageString);
            auto imageStream = dynamic_cast<osg::ImageStream *>(transmissionImage);
            if (imageStream != nullptr)
                imageStream->play();
            if (transmissionImage == nullptr)
                throw cRuntimeError("Transmission image '%s' not found", transmissionImageString);
        }
        displayReceptions = par("displayReceptions");
        if (displayReceptions) {
            const char *receptionImageString = par("receptionImage");
            receptionImage = inet::osg::createImage(receptionImageString);
            auto imageStream = dynamic_cast<osg::ImageStream *>(receptionImage);
            if (imageStream != nullptr)
                imageStream->play();
            if (receptionImage == nullptr)
                throw cRuntimeError("Reception reception '%s' not found", receptionImageString);
        }
        displayInterferenceRanges = par("displayInterferenceRanges");
        displayCommunicationRanges = par("displayCommunicationRanges");
        leaveCommunicationTrail = par("leaveCommunicationTrail");
        updateInterval = par("updateSceneInterval");
        updateSceneTimer = new cMessage("updateScene");
        networkNodeVisualizer = getModuleFromPar<NetworkNodeOsgVisualizer>(par("networkNodeVisualizerModule"), this);
    }
}

void MediumOsgVisualizer::handleMessage(cMessage *message)
{
    if (message == updateSceneTimer)
        scheduleUpdateSceneTimer();
    else
        throw cRuntimeError("Unknown message");
}

osg::Node *MediumOsgVisualizer::getCachedOsgNode(const IRadio *radio) const
{
    auto it = radioOsgNodes.find(radio);
    if (it == radioOsgNodes.end())
        return nullptr;
    else
        return it->second;
}

void MediumOsgVisualizer::setCachedOsgNode(const IRadio *radio, osg::Node *node)
{
    radioOsgNodes[radio] = node;
}

osg::Node *MediumOsgVisualizer::removeCachedOsgNode(const IRadio *radio)
{
    auto it = radioOsgNodes.find(radio);
    if (it == radioOsgNodes.end())
        return nullptr;
    else {
        radioOsgNodes.erase(it);
        return it->second;
    }
}

osg::Node *MediumOsgVisualizer::getCachedOsgNode(const ITransmission *transmission) const
{
    auto it = transmissionOsgNodes.find(transmission);
    if (it == transmissionOsgNodes.end())
        return nullptr;
    else
        return it->second;
}

void MediumOsgVisualizer::setCachedOsgNode(const ITransmission *transmission, osg::Node *node)
{
    transmissionOsgNodes[transmission] = node;
}

osg::Node *MediumOsgVisualizer::removeCachedOsgNode(const ITransmission *transmission)
{
    auto it = transmissionOsgNodes.find(transmission);
    if (it == transmissionOsgNodes.end())
        return nullptr;
    else {
        transmissionOsgNodes.erase(it);
        return it->second;
    }
}

osg::Node *MediumOsgVisualizer::createTransmissionNode(const ITransmission *transmission) const
{
#if OMNETPP_CANVAS_VERSION >= 0x20140908
    cFigure::Color color = cFigure::GOOD_DARK_COLORS[transmission->getId() % (sizeof(cFigure::GOOD_DARK_COLORS) / sizeof(cFigure::Color))];
#else
    cFigure::Color color(64 + rand() % 64, 64 + rand() % 64, 64 + rand() % 64);
#endif
    auto depth = new osg::Depth();
    depth->setWriteMask(false);
    auto stateSet = inet::osg::createStateSet(color, 1.0, false);
    stateSet->setAttributeAndModes(depth, osg::StateAttribute::ON);
    auto transmissionStart = transmission->getStartPosition();
    switch (signalShape) {
        case SIGNAL_SHAPE_RING: {
            // FIXME: there's some random overlapping artifact due to clipping degenerate triangles
            // FIXME: when the inner radius is very small and the outer radius is very large
            // FIXME: split up shape into multiple annuluses having more and more vertices, and being wider outwards
            auto annulus = inet::osg::createAnnulusGeometry(Coord::ZERO, 0, 0, 100);
            annulus->setStateSet(stateSet);
            osg::AutoTransform::AutoRotateMode autoRotateMode;
            if (!strcmp(signalPlane, "xy"))
                autoRotateMode = osg::AutoTransform::NO_ROTATION;
            else if (!strcmp(signalPlane, "xz"))
                autoRotateMode = osg::AutoTransform::ROTATE_TO_AXIS;
            else if (!strcmp(signalPlane, "yz"))
                autoRotateMode = osg::AutoTransform::ROTATE_TO_AXIS;
            else if (!strcmp(signalPlane, "camera"))
                autoRotateMode = osg::AutoTransform::ROTATE_TO_SCREEN;
            else
                throw cRuntimeError("Unknown signalPlane parameter value: '%s'", signalPlane);
            auto autoTransform = inet::osg::createAutoTransform(annulus, autoRotateMode, false);
            autoTransform->setNodeMask(0);
            auto positionAttitudeTransform = inet::osg::createPositionAttitudeTransform(transmissionStart, EulerAngles::ZERO);
            positionAttitudeTransform->addChild(autoTransform);
            if (signalOpacityExponent > 0) {
                auto program = new osg::Program();
                auto vertexShader = new osg::Shader(osg::Shader::VERTEX);
                auto fragmentShader = new osg::Shader(osg::Shader::FRAGMENT);
                vertexShader->setShaderSource(R"(
                    varying vec4 verpos;
                    varying vec4 color;
                    void main(){
                        gl_Position = ftransform();
                        verpos = gl_Vertex;
                        color = gl_Color;
                        gl_TexCoord[0]=gl_MultiTexCoord0;
                    }
                )");
                fragmentShader->setShaderSource(R"(
                    varying vec4 verpos;
                    varying vec4 color;
                    uniform float min, max, exponent;
                    void main( void )
                    {
                        float alpha = 1.0 / pow(length(verpos), exponent);
                        gl_FragColor = vec4(color.rgb, alpha);
                    }
                )");
                program->addShader(vertexShader);
                program->addShader(fragmentShader);
                stateSet->setAttributeAndModes(program, osg::StateAttribute::ON);
                stateSet->addUniform(new osg::Uniform("min", 0.0f));
                stateSet->addUniform(new osg::Uniform("max", 200.0f));
                stateSet->addUniform(new osg::Uniform("exponent", (float)signalOpacityExponent));
            }
            return positionAttitudeTransform;
        }
        case SIGNAL_SHAPE_SPHERE: {
            auto drawable = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(transmissionStart.x, transmissionStart.y, transmissionStart.z), 1));
            auto geode = new osg::Geode();
            geode->addDrawable(drawable);
            return geode;
        }
        default:
            throw cRuntimeError("Unimplemented signal shape");
    }
}

void MediumOsgVisualizer::mediumChanged()
{
    Enter_Method_Silent();
}

void MediumOsgVisualizer::radioAdded(const IRadio *radio)
{
    Enter_Method_Silent();
    if (displayTransmissions || displayReceptions || displayInterferenceRanges || displayCommunicationRanges) {
        auto group = new osg::Group();
        auto module = const_cast<cModule *>(check_and_cast<const cModule *>(radio));
        auto networkNodeOsgNode = networkNodeVisualizer->getNeworkNodeVisualization(getContainingNode(module));
        networkNodeOsgNode->getAnnotationPart()->addChild(group);
        if (displayTransmissions) {
            auto texture = new osg::Texture2D();
            texture->setImage(transmissionImage);
            auto geometry = osg::createTexturedQuadGeometry(osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(transmissionImage->s(), 0.0, 0.0), osg::Vec3(0.0, transmissionImage->t(), 0.0), 0.0, 0.0, 1.0, 1.0);
            auto stateSet = geometry->getOrCreateStateSet();
            stateSet->setTextureAttributeAndModes(0, texture);
            stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
            stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
            stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            stateSet->setAttributeAndModes(new osg::Program(), osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
            auto geode = new osg::Geode();
            geode->addDrawable(geometry);
            auto autoTransform = new osg::AutoTransform();
            autoTransform->setPivotPoint(osg::Vec3d(transmissionImage->s() / 2, 0.0, 0.0));
            autoTransform->setAutoScaleToScreen(true);
            autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
            autoTransform->setPosition(osg::Vec3d(0.0, 0.0, 0.0));
            autoTransform->addChild(geode);
            autoTransform->setNodeMask(0);
            group->addChild(autoTransform);
        }
        if (displayReceptions) {
            auto texture = new osg::Texture2D();
            texture->setImage(receptionImage);
            auto geometry = osg::createTexturedQuadGeometry(osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(receptionImage->s(), 0.0, 0.0), osg::Vec3(0.0, receptionImage->t(), 0.0), 0.0, 0.0, 1.0, 1.0);
            auto stateSet = geometry->getOrCreateStateSet();
            stateSet->setTextureAttributeAndModes(0, texture);
            stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
            stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
            stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
            stateSet->setAttributeAndModes(new osg::Program(), osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
            auto geode = new osg::Geode();
            geode->addDrawable(geometry);
            auto autoTransform = new osg::AutoTransform();
            autoTransform->setPivotPoint(osg::Vec3d(receptionImage->s() / 2, 0.0, 0.0));
            autoTransform->setAutoScaleToScreen(true);
            autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
            autoTransform->setPosition(osg::Vec3d(0.0, 0.0, 0.0));
            autoTransform->addChild(geode);
            autoTransform->setNodeMask(0);
            group->addChild(autoTransform);
        }
        if (displayInterferenceRanges) {
            auto maxInterferenceRage = radioMedium->getMediumLimitCache()->getMaxInterferenceRange(radio);
            auto circle = inet::osg::createCircleGeometry(Coord::ZERO, maxInterferenceRage.get(), 100);
            auto stateSet = inet::osg::createStateSet(cFigure::GREY, 1);
            circle->setStateSet(stateSet);
            auto autoTransform = inet::osg::createAutoTransform(circle, osg::AutoTransform::NO_ROTATION, false);
//            auto autoTransform = inet::osg::createAutoTransform(circle, osg::AutoTransform::ROTATE_TO_SCREEN, false);
            networkNodeOsgNode->addChild(autoTransform);
        }
        if (displayCommunicationRanges) {
            auto maxCommunicationRange = radioMedium->getMediumLimitCache()->getMaxCommunicationRange(radio);
            auto circle = inet::osg::createCircleGeometry(Coord::ZERO, maxCommunicationRange.get(), 100);
            auto stateSet = inet::osg::createStateSet(cFigure::BLUE, 1);
            circle->setStateSet(stateSet);
            auto autoTransform = inet::osg::createAutoTransform(circle, osg::AutoTransform::NO_ROTATION, false);
//            auto autoTransform = inet::osg::createAutoTransform(circle, osg::AutoTransform::ROTATE_TO_SCREEN, false);
            networkNodeOsgNode->addChild(autoTransform);
        }
        setCachedOsgNode(radio, group);
    }
}

void MediumOsgVisualizer::radioRemoved(const IRadio *radio)
{
    Enter_Method_Silent();
    auto node = removeCachedOsgNode(radio);
    if (node != nullptr) {
        auto module = const_cast<cModule *>(check_and_cast<const cModule *>(radio));
        auto networkNodeOsgNode = networkNodeVisualizer->getNeworkNodeVisualization(getContainingNode(module));
        networkNodeOsgNode->getAnnotationPart()->removeChild(node);
    }
}

void MediumOsgVisualizer::transmissionAdded(const ITransmission *transmission)
{
    Enter_Method_Silent();
    if (displaySignals) {
        transmissions.push_back(transmission);
        auto node = createTransmissionNode(transmission);
        auto scene = inet::osg::getScene(visualizerTargetModule);
        scene->addChild(node);
        setCachedOsgNode(transmission, node);
        if (updateInterval > 0)
            scheduleUpdateSceneTimer();
    }
}

void MediumOsgVisualizer::transmissionRemoved(const ITransmission *transmission)
{
    Enter_Method_Silent();
    if (displaySignals) {
        transmissions.erase(std::remove(transmissions.begin(), transmissions.end(), transmission));
        auto node = removeCachedOsgNode(transmission);
        if (node != nullptr)
            node->getParent(0)->removeChild(node);
    }
}

void MediumOsgVisualizer::transmissionStarted(const ITransmission *transmission)
{
    Enter_Method_Silent();
    if (displayTransmissions) {
        auto group = check_and_cast<osg::Group *>(getCachedOsgNode(transmission->getTransmitter()));
        auto autoTransform = check_and_cast<osg::AutoTransform *>(group->getChild(0));
        autoTransform->setNodeMask(1);
    }
}

void MediumOsgVisualizer::transmissionEnded(const ITransmission *transmission)
{
    Enter_Method_Silent();
    if (displayTransmissions) {
        auto group = check_and_cast<osg::Group *>(getCachedOsgNode(transmission->getTransmitter()));
        auto autoTransform = check_and_cast<osg::AutoTransform *>(group->getChild(0));
        autoTransform->setNodeMask(0);
    }
}

void MediumOsgVisualizer::receptionStarted(const IReception *reception)
{
    Enter_Method_Silent();
    if (displayReceptions) {
        auto group = check_and_cast<osg::Group *>(getCachedOsgNode(reception->getReceiver()));
        auto autoTransform = check_and_cast<osg::AutoTransform *>(group->getChild(1));
        autoTransform->setNodeMask(1);
    }
}

void MediumOsgVisualizer::receptionEnded(const IReception *reception)
{
    Enter_Method_Silent();
    if (displayReceptions) {
        auto group = check_and_cast<osg::Group *>(getCachedOsgNode(reception->getReceiver()));
        auto autoTransform = check_and_cast<osg::AutoTransform *>(group->getChild(1));
        autoTransform->setNodeMask(0);
    }
}

void MediumOsgVisualizer::packetReceived(const IReceptionDecision *decision)
{
    Enter_Method_Silent();
    if (decision->isReceptionSuccessful()) {
        auto scene = inet::osg::getScene(visualizerTargetModule);
        const ITransmission *transmission = decision->getReception()->getTransmission();
        const IReception *reception = decision->getReception();
        if (leaveCommunicationTrail) {
            Coord transmissionPosition = transmission->getStartPosition();
            Coord receptionPosition = reception->getStartPosition();
            Coord centerPosition = (transmissionPosition + receptionPosition) / 2;
            osg::Geometry* linesGeom = new osg::Geometry();
            osg::Vec3Array* vertexData = new osg::Vec3Array();
            vertexData->push_back(osg::Vec3(transmissionPosition.x, transmissionPosition.y, transmissionPosition.z));
            vertexData->push_back(osg::Vec3(receptionPosition.x, receptionPosition.y, receptionPosition.z));
            osg::DrawArrays* drawArrayLines = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP);
            drawArrayLines->setFirst(0);
            drawArrayLines->setCount(vertexData->size());
            linesGeom->setVertexArray(vertexData);
            linesGeom->addPrimitiveSet(drawArrayLines);
            osg::Geode* geode = new osg::Geode();
            geode->addDrawable(linesGeom);
            scene->addChild(geode);
        }
    }
}

void MediumOsgVisualizer::refreshDisplay()
{
    if (displaySignals) {
        const IPropagation *propagation = radioMedium->getPropagation();
        for (auto transmission : transmissions) {
            auto transmissionStart = transmission->getStartPosition();
            double startRadius = propagation->getPropagationSpeed().get() * (simTime() - transmission->getStartTime()).dbl();
            double endRadius = std::max(0.0, propagation->getPropagationSpeed().get() * (simTime() - transmission->getEndTime()).dbl());
            auto node = getCachedOsgNode(transmission);
            if (node != nullptr) {
                osg::Drawable *drawable;
                switch (signalShape) {
                    case SIGNAL_SHAPE_RING: {
                        auto autoTransform = static_cast<osg::PositionAttitudeTransform *>(node)->getChild(0);
                        auto geode = static_cast<osg::AutoTransform *>(autoTransform)->getChild(0);
                        drawable = static_cast<osg::Geode *>(geode)->getDrawable(0);
                        auto vertices = inet::osg::createAnnulusVertices(Coord::ZERO, startRadius, endRadius, 100);
                        static_cast<osg::Geometry *>(drawable)->setVertexArray(vertices);
                        autoTransform->setNodeMask(startRadius > 0 ? -1 : 0);
                        break;
                    }
                    case SIGNAL_SHAPE_SPHERE: {
                        drawable = static_cast<osg::Geode *>(node)->getDrawable(0);
                        auto shape = static_cast<osg::Sphere *>(drawable->getShape());
                        shape->setRadius(startRadius);
                        drawable->dirtyDisplayList();
                        drawable->dirtyBound();
                        double alpha = 1.0 / pow(startRadius, signalOpacityExponent);
                        auto material = static_cast<osg::Material *>(drawable->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
                        material->setAlpha(osg::Material::FRONT_AND_BACK, std::max(0.1, alpha));
                        break;
                    }
                    default:
                        throw cRuntimeError("Unimplemented signal shape");
                }
            }
        }
    }
}

void MediumOsgVisualizer::scheduleUpdateSceneTimer()
{
    if (updateSceneTimer->isScheduled())
        cancelEvent(updateSceneTimer);
    simtime_t nextUpdateTime = SimTime::getMaxTime();
    for (auto transmission : transmissions) {
        simtime_t transmissionNextUpdateTime = getTransmissionNextUpdateTime(transmission);
        if (transmissionNextUpdateTime < nextUpdateTime)
            nextUpdateTime = transmissionNextUpdateTime;
    }
    if (nextUpdateTime != SimTime::getMaxTime()) {
        scheduleAt(nextUpdateTime, updateSceneTimer);
    }
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

