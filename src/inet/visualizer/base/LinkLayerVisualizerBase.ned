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

package inet.visualizer.base;

//
// This is a base module for link layer visualizer simple modules.
//
simple LinkLayerVisualizerBase extends VisualizerBase
{
    parameters:
        string linkLayerSubscriptionModule = default(""); // specifies the module path where the visualizer subscribes for link layer packet sent and packet received signals
        string packetNameFilter = default("*"); // specifies which packets are considered to determine active links
}
