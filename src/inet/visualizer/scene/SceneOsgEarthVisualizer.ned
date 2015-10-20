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

package inet.visualizer.scene;

import inet.visualizer.base.SceneOsgVisualizerBase;
import inet.visualizer.contract.ISceneVisualizer;

//
// This module visualizes the scene using osg earth.
//
simple SceneOsgEarthVisualizer extends SceneOsgVisualizerBase like ISceneVisualizer
{
    parameters:
        @class(SceneOsgEarthVisualizer);
        string mapFile; // map file, e.g. some part of the earth
        double playgroundLongitude; // geographic position of the playground origin
        double playgroundLatitude; // geographic position of the playground origin
        double playgroundOrientation @unit(deg); // orientation of the playground
        string convertGeographicLocationsToPlaygroundPositions = default(""); // list of name, latitude, longitude tuples separated by semicolons
}
