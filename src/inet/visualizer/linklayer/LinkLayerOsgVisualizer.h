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

#ifndef __INET_LINKLAYEROSGVISUALIZER_H
#define __INET_LINKLAYEROSGVISUALIZER_H

#include "inet/visualizer/base/LinkLayerVisualizerBase.h"

namespace inet {

namespace visualizer {

/**
 * This class provides the visualization of the link layer communication.
 */
class INET_API LinkLayerOsgVisualizer : public LinkLayerVisualizerBase
{
#ifdef WITH_OSG

  protected:
    class INET_API CacheEntry {
      public:
        osg::Geode *geode = nullptr;
        simtime_t lastSuccessfulCommunication = simTime();

      public:
        CacheEntry(osg::Geode *geode) : geode(geode) {}
    };

  protected:
    std::map<int, cModule *> lastModules;
    std::map<std::pair<int, int>, CacheEntry *> cacheEntries;

  protected:
    virtual void initialize(int stage) override;

    virtual CacheEntry *getCacheEntry(std::pair<int, int> link);
    virtual void setCacheEntry(std::pair<int, int> link, CacheEntry *cacheEntry);
    virtual void removeCacheEntry(std::pair<int, int> link);

    virtual cModule *getLastModule(int treeId);
    virtual void setLastModule(int treeId, cModule *lastModule);

    virtual osg::Geode *createLine(cModule *source, cModule *destination);
    virtual void updateCache(cModule *source, cModule *destination);

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object) override;

#else

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object) override {}

#endif // ifdef WITH_OSG
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_LINKLAYEROSGVISUALIZER_H

