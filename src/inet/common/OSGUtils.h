//
// Copyright (C) 2006-2015 Opensim Ltd
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

#ifndef __INET_OSGUTILS_H
#define __INET_OSGUTILS_H

#include "inet/common/geometry/common/Coord.h"
#include "inet/common/geometry/common/EulerAngles.h"
#include <osg/AutoTransform>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Material>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/Texture2D>
#include <osgText/Text>
#include <vector>

namespace inet {

namespace osg {

using namespace ::osg;

Group *getScene(cModule *module);

Vec3Array *createCircleVertices(const Coord& center, double radius, int polygonSize);
Vec3Array *createAnnulusVertices(const Coord& center, double outerRadius, double innerRadius, int polygonSize);

Geometry *createLineGeometry(const Coord& begin, const Coord& end);
Geometry *createCircleGeometry(const Coord& center, double radius, int polygonSize);
Geometry *createAnnulusGeometry(const Coord& center, double outerRadius, double innerRadius, int polygonSize);
Geometry *createQuadGeometry(const Coord& begin, const Coord& end);
Geometry *createPolygonGeometry(const std::vector<Coord>& points, const Coord& translation = Coord::ZERO);

osgText::Text *createText(const char *string, const Coord& position, const cFigure::Color& color);

AutoTransform *createAutoTransform(Drawable *drawable, AutoTransform::AutoRotateMode mode, bool autoScaleToScreen, const Coord& position = Coord::ZERO);
PositionAttitudeTransform *createPositionAttitudeTransform(const Coord& position, const EulerAngles& orientation);

osg::Image* createImage(const char *fileName);
Texture2D *createTexture(const char *name, bool repeat);

StateSet *createStateSet(const cFigure::Color& color, double opacity, bool cullBackFace = true);

} // namespace osg

} // namespace inet

#endif // ifndef __INET_OSGUTILS_H

