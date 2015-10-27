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

#ifndef __INET_STATISTICVISUALIZERBASE_H
#define __INET_STATISTICVISUALIZERBASE_H

#include "inet/common/PatternMatcher.h"
#include "inet/visualizer/base/VisualizerBase.h"

namespace inet {

namespace visualizer {

class INET_API StatisticVisualizerBase : public VisualizerBase, public cListener
{
  protected:
    /** @name Parameters */
    //@{
    cModule *statisticSubscriptionModule = nullptr;
    PatternMatcher statisticNameMatcher;
    //@}

  protected:
    virtual void initialize(int stage) override;

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b) override { }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, long l) override { }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l) override { }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d) override { }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const SimTime& t) override { }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, const char *s) override { }
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj) override { }
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_STATISTICVISUALIZERBASE_H

