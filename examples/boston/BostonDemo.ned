//
// Copyright (C) 2006 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

package inet.examples.boston;

import inet.environment.common.PhysicalEnvironment;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.ethernet.Eth1G;
import inet.node.inet.StandardHost;
import inet.node.inet.WirelessHost;
import inet.node.wireless.AccessPoint;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.integrated.IntegratedVisualizer;

network BostonDemo
{
    parameters:
        @display("bgb=0,0");
    submodules:
        visualizer: IntegratedVisualizer {
            parameters:
                @display("p=100,50");
        }
        physicalEnvironment: PhysicalEnvironment {
            parameters:
                @display("p=100,150");
        }
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=100,250");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=100,350");
        }
        server: StandardHost {
            parameters:
                @display("p=300,100");
        }
        accessPoint: AccessPoint {
            parameters:
                @display("p=300,200");
        }
        client[1]: WirelessHost {
            parameters:
                @display("p=300,300");
        }
    connections:
        server.ethg++ <--> Eth1G <--> accessPoint.ethg++;
}
