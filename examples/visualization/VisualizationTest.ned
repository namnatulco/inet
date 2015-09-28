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

package inet.examples.visualization;


import inet.environment.common.PhysicalEnvironment;
import inet.mobility.single.RandomWPMobility;
import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.inet.AdhocHost;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211ScalarRadioMedium;
import inet.visualizer.environment.PhysicalEnvironmentCanvasVisualizer;
import inet.visualizer.environment.PhysicalEnvironmentOsgVisualizer;
import inet.visualizer.integrated.IntegratedCanvasVisualizer;
import inet.visualizer.integrated.IntegratedOsgVisualizer;
import inet.visualizer.integrated.IntegratedVisualizer;
import inet.visualizer.mobility.MobilityCanvasVisualizer;
import inet.visualizer.mobility.MobilityOsgVisualizer;
import inet.visualizer.physicallayer.MediumCanvasVisualizer;
import inet.visualizer.physicallayer.MediumOsgVisualizer;

network PhysicalEnvironmentVisualizationTest
{
    parameters:
        @display("bgb=0,0");
    submodules:
        physicalEnvironment: PhysicalEnvironment {
            parameters:
                @display("p=100,50");
        }
        osgVisualizer: PhysicalEnvironmentOsgVisualizer {
            parameters:
                @display("p=100,150");
        }
        canvasVisualizer: PhysicalEnvironmentCanvasVisualizer {
            parameters:
                @display("p=300,150");
        }
}

network IntegratedVisualizationTest
{
    parameters:
        @display("bgb=0,0");
    submodules:
        physicalEnvironment: PhysicalEnvironment {
            parameters:
                @display("p=100,50");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=100,150");
        }
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=100,250");
        }
        host[2]: AdhocHost {
            parameters:
                @display("p=300,50,row");
        }
        osgVisualizer: IntegratedOsgVisualizer {
            parameters:
                @display("p=300,150");
        }
        isometricVisualizer: IntegratedCanvasVisualizer {
            parameters:
                @display("p=500,150");
        }
        topVisualizer: IntegratedCanvasVisualizer {
            parameters:
                @display("p=700,150");
        }
}

network DefaultVisualizationTest
{
    parameters:
        @display("bgb=0,0");
    submodules:
        physicalEnvironment: PhysicalEnvironment {
            parameters:
                @display("p=100,50");
        }
        radioMedium: Ieee80211ScalarRadioMedium {
            parameters:
                @display("p=100,150");
        }
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=100,250");
        }
        host[2]: AdhocHost {
            parameters:
                @display("p=300,50");
        }
        visualizer: IntegratedVisualizer {
            parameters:
                @display("p=300,150");
        }
}
