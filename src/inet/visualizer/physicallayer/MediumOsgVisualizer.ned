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

package inet.visualizer.physicallayer;

import inet.visualizer.base.MediumVisualizerBase;
import inet.visualizer.contract.IMediumVisualizer;

//
// This module visualizes a medium using osg.
//
simple MediumOsgVisualizer extends MediumVisualizerBase like IMediumVisualizer
{
    parameters:
        @class(MediumOsgVisualizer);
        string networkNodeVisualizerModule = default("^.networkNodeVisualizer"); // module path of the network node visualizer module

        bool displaySignals = default(false); // display signals propagating through the medium
        double signalOpacityExponent = default(0.5); // specifies how opacity decreases with distance
        string signalShape @enum("ring","sphere") = default("ring"); // specifies the shape for displaying signals
        string signalPlane @enum("camera","xy","xz","yz") = default("xy"); // specifies the plane for 2 dimensional signal shapes

        bool displayTransmissions = default(true); // display an image where a transmission is currently in progress
        string transmissionImage = default("rightup.gif"); // transmission image for ongoing transmissions
        string transmissionPlane @enum("camera","xy","xz","yz") = default("camera"); // specifies the plane for transmission image

        bool displayReceptions = default(true); // display an image where a reception is currently in progress
        string receptionImage = default("rightdown.gif"); // reception image for ongoing receptions
        string receptionPlane @enum("camera","xy","xz","yz") = default("camera"); // specifies the plane for reception image

        bool displayCommunicationRanges = default(true); // display communication ranges as blue circles around radios
        string communicationRangePlane @enum("camera","xy","xz","yz") = default("xy"); // specifies the plane for the transmission range circle

        bool displayInterferenceRanges = default(false); // display interference ranges as gray circles around radios
        string interferenceRangePlane @enum("camera","xy","xz","yz") = default("xy"); // specifies the plane for the interference range circle

        bool leaveCommunicationTrail = default(false); // display recent successful receptions as straight blue lines from transmission location to reception location

        double signalPropagationUpdateInterval @unit(s) = default(0s); // the time interval between updates when there are propagating signals on the medium
}
