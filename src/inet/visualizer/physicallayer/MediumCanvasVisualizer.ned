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
// This module visualizes a medium on a canvas.
//
simple MediumCanvasVisualizer extends MediumVisualizerBase like IMediumVisualizer
{
    parameters:
        @class(MediumCanvasVisualizer);
        bool displaySignals = default(false); // display signals propagating through the medium
        string signalShape @enum("ring","sphere") = default("ring"); // specifies the shape for displaying signals

        bool displayTransmissions = default(true); // display an image where a transmission is currently in progress
        string transmissionImage = default("transmission.gif"); // transmission image for ongoing transmissions

        bool displayReceptions = default(true); // display an image where a reception is currently in progress
        string receptionImage = default("reception.gif"); // reception image for ongoing receptions

        bool displayCommunicationRanges = default(true); // display communication ranges as blue circles around radios
        bool displayInterferenceRanges = default(false); // display interference ranges as gray circles around radios

        bool leaveCommunicationTrail = default(false); // when enabled the radio medium displays recent successful receptions as straight blue lines from the transmission to the reception
        bool leaveCommunicationHeat = default(false); // when enabled the radio medium displays recent successful receptions as a heat map overlay

        double signalPropagationUpdateInterval @unit(s) = default(0s); // the time interval between updates when there are propagating signals on the medium
}
