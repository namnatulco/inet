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
#include "inet/visualizer/networknode/NetworkNodeOsgVisualizer.h"
#include <osg/AutoTransform>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgText/Text>

namespace inet {

namespace visualizer {

Define_Module(NetworkNodeOsgVisualizer);

#ifdef WITH_OSG

void NetworkNodeOsgVisualizer::initialize(int stage)
{
    NetworkNodeVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        displayModuleName = par("displayModuleName");
        auto scene = inet::osg::getScene(visualizerTargetModule);
        for (cModule::SubmoduleIterator it(getSystemModule()); !it.end(); it++) {
            auto node = *it;
            if (isNetworkNode(node)) {
                auto positionAttitudeTransform = createNetworkNodeVisualization(node);
                setNetworkNodeVisualization(node, positionAttitudeTransform);
                scene->addChild(positionAttitudeTransform);
            }
        }
    }
}

NetworkNodeVisualization* NetworkNodeOsgVisualizer::createNetworkNodeVisualization(cModule *module) const
{
    double labelSpacing = 2;
    osgText::Text *label = nullptr;
    if (displayModuleName) {
        label = new osgText::Text();
        label->setCharacterSize(18);
        label->setBoundingBoxColor(osg::Vec4(1.0, 1.0, 1.0, 0.5));
        label->setColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
        label->setAlignment(osgText::Text::CENTER_BOTTOM);
        label->setText(module->getFullName());
        label->setDrawMode(osgText::Text::FILLEDBOUNDINGBOX | osgText::Text::TEXT);
    }
    osg::Node *osgNode = nullptr;
    cDisplayString& displayString = module->getDisplayString();
    if (module->hasPar("osgModel") && strlen(module->par("osgModel")) != 0) {
        const char *osgModelString = module->par("osgModel");
        auto osgModel = osgDB::readNodeFile(osgModelString);
        if (osgModel == nullptr)
            throw cRuntimeError("Visual representation osg model '%s' not found", osgModel);
        osgModel->setStateSet(inet::osg::createStateSet(cFigure::Color(203, 65, 84), 1.0, false));
        auto group = new osg::Group();
        group->addChild(osgModel);
        if (displayModuleName) {
            label->setPosition(osg::Vec3(0.0, 0.0, labelSpacing));
            auto geode = new osg::Geode();
            geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
            geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program(), osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
            geode->addDrawable(label);
            auto boundingSphere = osgModel->getBound();
            auto autoTransform = new osg::AutoTransform();
            // TODO: allow pivot point parameterization
            autoTransform->setPivotPoint(osg::Vec3d(0.0, 0.0, 0.0));
            autoTransform->setAutoScaleToScreen(true);
            autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
            autoTransform->setPosition(osg::Vec3d(0.0, 0.0, boundingSphere.radius()));
            autoTransform->addChild(geode);
            group->addChild(autoTransform);
        }
        osgNode = group;
    }
    else {
        const char *icon = displayString.getTagArg("i", 0);
        // TODO: get real images path
        std::string path("/home/levy/workspace/omnetpp/images/");
        path += icon;
        path += ".png";
        auto image = osgDB::readImageFile(path.c_str());
        if (image == nullptr)
            throw cRuntimeError("Cannot find icon '%s' at '%s'", icon, path.c_str());
        auto texture = new osg::Texture2D();
        texture->setImage(image);
        auto geometry = osg::createTexturedQuadGeometry(osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(image->s(), 0.0, 0.0), osg::Vec3(0.0, image->t(), 0.0), 0.0, 0.0, 1.0, 1.0);
        geometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
        geometry->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
        geometry->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
        geometry->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        auto geode = new osg::Geode();
        geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program(), osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        geode->addDrawable(geometry);
        if (displayModuleName) {
            label->setPosition(osg::Vec3(0.0, image->t() + labelSpacing, 0.0));
            geode->addDrawable(label);
        }
        auto autoTransform = new osg::AutoTransform();
        // TODO: allow pivot point parameterization
        autoTransform->setPivotPoint(osg::Vec3d(image->s() / 2, 0.0, 0.0));
//        autoTransform->setPivotPoint(osg::Vec3d(image->s() / 2, image->t() / 2, 0.0));
        autoTransform->setAutoScaleToScreen(true);
        autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
        autoTransform->setPosition(osg::Vec3d(0.0, 0.0, 0.0));
        autoTransform->addChild(geode);
        osgNode = autoTransform;
    }
    auto objectNode = cOsgCanvas::createOmnetppObjectNode(module);
    objectNode->addChild(osgNode);
    auto positionAttitudeTransform = new NetworkNodeVisualization(objectNode);
    double x = atol(displayString.getTagArg("p", 0));
    double y = atol(displayString.getTagArg("p", 1));
    positionAttitudeTransform->setPosition(osg::Vec3d(x, y, 0.0));
    positionAttitudeTransform->addChild(objectNode);
    return positionAttitudeTransform;
}

NetworkNodeVisualization *NetworkNodeOsgVisualizer::getNeworkNodeVisualization(const cModule *module) const
{
    auto it = networkNodeVisualizations.find(module);
    return it == networkNodeVisualizations.end() ? nullptr : it->second;
}

void NetworkNodeOsgVisualizer::setNetworkNodeVisualization(const cModule *module, NetworkNodeVisualization *networkNodeVisualization)
{
    networkNodeVisualizations[module] = networkNodeVisualization;
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

