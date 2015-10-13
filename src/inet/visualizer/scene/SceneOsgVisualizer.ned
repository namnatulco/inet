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
// This module visualizes the scene using osg.
//
simple SceneOsgVisualizer extends SceneOsgVisualizerBase like ISceneVisualizer
{
    parameters:
        @class(SceneOsgVisualizer);
        bool displayPlayground = default(true); // display the playground
        string playgroundColor = default("#A0E0A0"); // playground solid color if playground texture is not provided 
        string playgroundImage = default(""); // playground texture (repeated)
        double playgroundImageSize @unit(m) = default(1m); // playground texture size
}
