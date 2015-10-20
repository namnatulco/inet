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

#include "inet/common/LayeredProtocolBase.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/OSGUtils.h"
#include "inet/linklayer/base/MACProtocolBase.h"
#include "inet/mobility/contract/IMobility.h"
#include "inet/visualizer/linklayer/LinkLayerOsgVisualizer.h"
#include <osg/Geode>
#include <osg/LineWidth>

namespace inet {

namespace visualizer {

Define_Module(LinkLayerOsgVisualizer);

#ifdef WITH_OSG

void LinkLayerOsgVisualizer::initialize(int stage)
{
    LinkLayerVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
}

LinkLayerOsgVisualizer::CacheEntry *LinkLayerOsgVisualizer::getCacheEntry(std::pair<int, int> link)
{
    auto it = cacheEntries.find(link);
    return it == cacheEntries.end() ? nullptr : it->second;
}

void LinkLayerOsgVisualizer::setCacheEntry(std::pair<int, int> link, CacheEntry *cacheEntry)
{
    cacheEntries[link] = cacheEntry;
}

void LinkLayerOsgVisualizer::removeCacheEntry(std::pair<int, int> link)
{
    cacheEntries.erase(cacheEntries.find(link));
}

cModule *LinkLayerOsgVisualizer::getLastModule(int treeId)
{
    auto it = lastModules.find(treeId);
    if (it == lastModules.end())
        return nullptr;
    else
        return it->second;
}

void LinkLayerOsgVisualizer::setLastModule(int treeId, cModule *module)
{
    lastModules[treeId] = module;
}

osg::Geode *LinkLayerOsgVisualizer::createLine(cModule *source, cModule *destination)
{
    auto geode = new osg::Geode();
    auto sourceMobility = check_and_cast<IMobility *>(getContainingNode(source)->getSubmodule("mobility"));
    auto destinationMobility = check_and_cast<IMobility *>(getContainingNode(destination)->getSubmodule("mobility"));
    auto drawable = inet::osg::createLineGeometry(sourceMobility->getCurrentPosition(), destinationMobility->getCurrentPosition());
    geode->addDrawable(drawable);
    auto stateSet = inet::osg::createStateSet(cFigure::BLACK, 1.0, false);
    stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
    geode->setStateSet(stateSet);
    return geode;
}

void LinkLayerOsgVisualizer::updateCache(cModule *source, cModule *destination)
{
    auto key = std::pair<int, int>(source->getId(), destination->getId());
    CacheEntry *cacheEntry = getCacheEntry(key);
    if (cacheEntry == nullptr) {
        auto geode = createLine(source, destination);
        auto scene = inet::osg::getScene(visualizerTargetModule);
        scene->addChild(geode);
        auto cacheEntry = new CacheEntry(geode);
        setCacheEntry(key, cacheEntry);
    }
    else {
        cacheEntry->lastSuccessfulCommunication = simTime();
    }
}

void LinkLayerOsgVisualizer::receiveSignal(cComponent *source, simsignal_t signal, cObject *object)
{
    if (signal == IMobility::mobilityStateChangedSignal) {
        auto now = simTime();
        auto mobility = dynamic_cast<IMobility *>(object);
        auto position = mobility->getCurrentPosition();
        auto module = check_and_cast<cModule *>(source);
        auto node = getContainingNode(module);
        for (auto it : cacheEntries) {
            auto cacheEntry = it.second;
            auto geode = cacheEntry->geode;
            auto geometry = static_cast<osg::Geometry *>(geode->getDrawable(0));
            auto vertices = static_cast<osg::Vec3Array *>(geometry->getVertexArray());
            if (it.first.first == node->getId())
                vertices->at(0) = osg::Vec3d(position.x, position.y, position.z);
            else if (it.first.second == node->getId())
                vertices->at(1) = osg::Vec3d(position.x, position.y, position.z);
            geometry->dirtyBound();
            geometry->dirtyDisplayList();
            auto alpha = std::pow(2.0, -(now - cacheEntry->lastSuccessfulCommunication).dbl());
            if (alpha < 0.1) {
                geode->getParent(0)->removeChild(geode);
                removeCacheEntry(it.first);
            }
            else {
                auto material = static_cast<osg::Material *>(geode->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL));
                material->setAlpha(osg::Material::FRONT_AND_BACK, alpha);
            }
        }
    }
    else if (signal == LayeredProtocolBase::packetReceivedFromUpperSignal) {
        if (dynamic_cast<MACProtocolBase *>(source) != nullptr) {
            auto packet = check_and_cast<cPacket *>(object);
            if (packetNameMatcher.matches(packet->getFullName())) {
                auto treeId = packet->getTreeId();
                auto module = check_and_cast<cModule *>(source);
                setLastModule(treeId, module);
            }
        }
    }
    else if (signal == LayeredProtocolBase::packetSentToUpperSignal) {
        if (dynamic_cast<MACProtocolBase *>(source) != nullptr) {
            auto packet = check_and_cast<cPacket *>(object);
            if (packetNameMatcher.matches(packet->getFullName())) {
                auto treeId = packet->getTreeId();
                auto module = check_and_cast<cModule *>(source);
                auto lastModule = getLastModule(treeId);
                if (lastModule != nullptr)
                    updateCache(getContainingNode(lastModule), getContainingNode(module));
            }
        }
    }
}

#endif // ifdef WITH_OSG

} // namespace visualizer

} // namespace inet

