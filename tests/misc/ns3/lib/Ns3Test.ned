//
// Copyright (C) 2015 OpenSim Ltd.
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

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.AdhocHost;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;


network Ns3Test
{
    parameters:
        // double simulationTime;
        int numClients;
        int numApps;
        double serverYPos;
        bool withQos;
        double sendInterval;
        @display("b=297,203");
    submodules:
        configurator: IPv4NetworkConfigurator {
            @display("p=61,164");
        }
        srvHost: AdhocHost {
            parameters:
                numUdpApps = numApps;
                mobility.initFromDisplayString = false;
                mobility.initialX = 10m;
                mobility.initialY = serverYPos;
                mobility.initialZ = 0m;
                wlan[*].classifierType =  withQos ? "ExampleQoSClassifier" : "";
                mac.EDCA = withQos;
                @display("r=,,#707070");
        }
        cliHost[numClients]: AdhocHost {
            parameters:
                numUdpApps = numApps;
                udpApp[*].sendInterval = sendInterval;
                mobility.initialX = 10m + 10m * index();
                mobility.initialY = 10m;
                mobility.initialZ = 0m;
                wlan[*].classifierType =  withQos ? "ExampleQoSClassifier" : "";
                mac.EDCA = withQos;
                @display("r=,,#707070");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=61,46");
        }
}

