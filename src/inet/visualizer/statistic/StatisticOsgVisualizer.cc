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
#include "inet/visualizer/statistic/StatisticOsgVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(StatisticOsgVisualizer);

#ifdef WITH_OSG

void StatisticOsgVisualizer::initialize(int stage)
{
    StatisticVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
}

void StatisticOsgVisualizer::receiveSignal(cComponent *source, simsignal_t signal, cObject *object)
{
    auto networkNodeVisualizer = getModuleFromPar<NetworkNodeOsgVisualizer>(par("networkNodeVisualizerModule"), this);
    auto networkNode = getContainingNode(check_and_cast<cModule *>(source));
    auto visualization = networkNodeVisualizer->getNeworkNodeVisualization(networkNode);
    auto annotation = visualization->getAnnotationPart();
    auto packet = check_and_cast<cPacket *>(object);
    auto key = std::pair<int, int>(source->getId(), signal);
    auto it = visualizations.find(key);
    if (it == visualizations.end()) {
        auto label = new osgText::Text();
        label->setCharacterSize(18);
        label->setBoundingBoxColor(osg::Vec4(1.0, 1.0, 1.0, 0.5));
        label->setColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
        label->setAlignment(osgText::Text::CENTER_BOTTOM);
        label->setText(packet->getName());
        label->setDrawMode(osgText::Text::FILLEDBOUNDINGBOX | osgText::Text::TEXT);
        label->setPosition(osg::Vec3(0.0, 0.0, 2));
        auto geode = new osg::Geode();
        geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        geode->getOrCreateStateSet()->setAttributeAndModes(new osg::Program(), osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
        geode->addDrawable(label);
        auto autoTransform = new osg::AutoTransform();
        autoTransform->setPivotPoint(osg::Vec3d(0.0, 0.0, 0.0));
        autoTransform->setAutoScaleToScreen(true);
        autoTransform->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);
        autoTransform->setPosition(osg::Vec3d(0.0, 0.0, 10));
        autoTransform->addChild(geode);
        annotation->addChild(autoTransform);
        visualizations[key] = autoTransform;
    }
    else {
        auto autoTransform = check_and_cast<osg::AutoTransform *>(it->second);
        auto geode = check_and_cast<osg::Geode *>(autoTransform->getChild(0));
        auto label = check_and_cast<osgText::Text *>(geode->getDrawable(0));
        label->setText(packet->getName());
    }
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

