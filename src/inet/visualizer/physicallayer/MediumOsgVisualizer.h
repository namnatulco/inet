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

#ifndef __INET_MEDIUMOSGVISUALIZER_H
#define __INET_MEDIUMOSGVISUALIZER_H

#include "inet/physicallayer/contract/packetlevel/IRadioFrame.h"
#include "inet/physicallayer/contract/packetlevel/IReceptionDecision.h"
#include "inet/physicallayer/contract/packetlevel/ITransmission.h"
#include "inet/visualizer/base/MediumVisualizerBase.h"
#include "inet/visualizer/networknode/NetworkNodeOsgVisualizer.h"

namespace inet {

namespace visualizer {

/**
 * This class provides the visualization of the communication on the medium.
 */
class INET_API MediumOsgVisualizer : public MediumVisualizerBase, public cListener
{
#ifdef WITH_OSG

  protected:
    /** @name Parameters */
    //@{
    bool displaySignals = false;
    double signalOpacityExponent = NaN;
    SignalShape signalShape = SIGNAL_SHAPE_RING;
    const char *signalPlane = nullptr;
    bool displayTransmissions = false;
    osg::Image *transmissionImage = nullptr;
    bool displayReceptions = false;
    osg::Image *receptionImage = nullptr;
    bool displayCommunicationRanges = false;
    bool displayInterferenceRanges = false;
    bool leaveCommunicationTrail = false;
    //@}

    /** @name Internal state */
    //@{
    NetworkNodeOsgVisualizer *networkNodeVisualizer;
    /**
     * The list of ongoing transmissions.
     */
    std::vector<const ITransmission *> transmissions;
    /**
     * The list of radio osg nodes.
     */
    std::map<const IRadio *, osg::Node *> radioOsgNodes;
    /**
     * The list of ongoing transmission osg nodes.
     */
    std::map<const ITransmission *, osg::Node *> transmissionOsgNodes;
    //@}

    /** @name Timer */
    //@{
    /**
     * The message that is used to update the scene when ongoing communications exist.
     */
    cMessage *updateSceneTimer = nullptr;
    //@}

  protected:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *message) override;
    virtual void refreshDisplay() override;

    virtual osg::Node *getCachedOsgNode(const IRadio *radio) const;
    virtual void setCachedOsgNode(const IRadio *radio, osg::Node *node);
    virtual osg::Node *removeCachedOsgNode(const IRadio *radio);

    virtual osg::Node *getCachedOsgNode(const ITransmission *transmission) const;
    virtual void setCachedOsgNode(const ITransmission *transmission, osg::Node *node);
    virtual osg::Node *removeCachedOsgNode(const ITransmission *transmission);

    virtual osg::Node *createTransmissionNode(const ITransmission *transmission) const;

    virtual void scheduleUpdateSceneTimer();

  public:
    virtual ~MediumOsgVisualizer();

    virtual void mediumChanged() override;

    virtual void radioAdded(const IRadio *radio) override;
    virtual void radioRemoved(const IRadio *radio) override;

    virtual void transmissionAdded(const ITransmission *transmission) override;
    virtual void transmissionRemoved(const ITransmission *transmission) override;

    virtual void transmissionStarted(const ITransmission *transmission) override;
    virtual void transmissionEnded(const ITransmission *transmission) override;
    virtual void receptionStarted(const IReception *reception) override;
    virtual void receptionEnded(const IReception *reception) override;

    virtual void packetReceived(const IReceptionDecision *decision) override;

#else

    virtual void mediumChanged() override {}

    virtual void radioAdded(const IRadio *radio) override {}
    virtual void radioRemoved(const IRadio *radio) override {}

    virtual void transmissionAdded(const ITransmission *transmission) override {}
    virtual void transmissionRemoved(const ITransmission *transmission) override {}

    virtual void transmissionStarted(const ITransmission *transmission) override {}
    virtual void transmissionEnded(const ITransmission *transmission) override {}
    virtual void receptionStarted(const IReception *reception) override {}
    virtual void receptionEnded(const IReception *reception) override {}

    virtual void packetReceived(const IReceptionDecision *decision) override {}

#endif // ifdef WITH_OSG
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_MEDIUMOSGVISUALIZER_H

