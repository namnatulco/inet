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

#include "inet/common/geometry/base/ShapeBase.h"
#include "inet/common/geometry/common/Rotation.h"
#include "inet/common/geometry/object/LineSegment.h"
#include "inet/physicallayer/obstacleloss/TracingObstacleLoss.h"

namespace inet {

namespace physicallayer {

Define_Module(TracingObstacleLoss);

TracingObstacleLoss::TracingObstacleLoss() :
    medium(nullptr),
    environment(nullptr),
    intersectionComputationCount(0),
    intersectionCount(0)
{
}

void TracingObstacleLoss::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        medium = check_and_cast<IRadioMedium *>(getParentModule());
        environment = check_and_cast<IPhysicalEnvironment *>(getModuleByPath(par("environmentModule")));
    }
}

void TracingObstacleLoss::finish()
{
    EV_INFO << "Obstacle loss intersection computation count: " << intersectionComputationCount << endl;
    EV_INFO << "Obstacle loss intersection count: " << intersectionCount << endl;
    recordScalar("Obstacle loss intersection computation count", intersectionComputationCount);
    recordScalar("Obstacle loss intersection count", intersectionCount);
}

std::ostream& TracingObstacleLoss::printToStream(std::ostream& stream, int level) const
{
    return stream << "TracingObstacleLoss";
}

double TracingObstacleLoss::computeDielectricLoss(const IMaterial *material, Hz frequency, m distance) const
{
    // NOTE: based on http://en.wikipedia.org/wiki/Dielectric_loss
    double lossTangent = material->getDielectricLossTangent(frequency);
    mps propagationSpeed = material->getPropagationSpeed();
    double factor = std::exp(-atan(lossTangent) * unit(2 * M_PI * frequency * distance / propagationSpeed).get());
    ASSERT(0 <= factor && factor <= 1);
    return factor;
}

double TracingObstacleLoss::computeReflectionLoss(const IMaterial *incidentMaterial, const IMaterial *refractiveMaterial, double angle) const
{
    // NOTE: based on http://en.wikipedia.org/wiki/Fresnel_equations
    double n1 = incidentMaterial->getRefractiveIndex();
    double n2 = refractiveMaterial->getRefractiveIndex();
    double st = sin(angle);
    double ct = cos(angle);
    double n1ct = n1 * ct;
    double n2ct = n2 * ct;
    double k = sqrt(1 - pow(n1 / n2 * st, 2));
    double n1k = n1 * k;
    double n2k = n2 * k;
    double rs = pow((n1ct - n2k) / (n1ct + n2k), 2);
    double rp = pow((n1k - n2ct) / (n1k + n2ct), 2);
    double r = (rs + rp) / 2;
    double transmittance = 1 - r;
    ASSERT(0 <= transmittance && transmittance <= 1);
    return transmittance;
}

double TracingObstacleLoss::computeObjectLoss(const IPhysicalObject *object, Hz frequency, const Coord& transmissionPosition, const Coord& receptionPosition) const
{
    double totalLoss = 1;
    const ShapeBase *shape = object->getShape();
    const Coord& position = object->getPosition();
    const EulerAngles& orientation = object->getOrientation();
    Rotation rotation(orientation);
    const LineSegment lineSegment(rotation.rotateVectorCounterClockwise(transmissionPosition - position), rotation.rotateVectorCounterClockwise(receptionPosition - position));
    Coord intersection1, intersection2, normal1, normal2;
    intersectionComputationCount++;
    if (shape->computeIntersection(lineSegment, intersection1, intersection2, normal1, normal2))
    {
        intersectionCount++;
        double intersectionDistance = intersection2.distance(intersection1);
        const IMaterial *material = object->getMaterial();
        totalLoss *= computeDielectricLoss(material, frequency, m(intersectionDistance));
        if (!normal1.isUnspecified()) {
            double angle1 = (intersection1 - intersection2).angle(normal1);
            totalLoss *= computeReflectionLoss(medium->getMaterial(), material, angle1);
        }
        // TODO: this returns NaN because n1 > n2
//        if (!normal2.isUnspecified()) {
//            double angle2 = (intersection2 - intersection1).angle(normal2);
//            totalLoss *= computeReflectionLoss(material, medium->getMaterial(), angle2);
//        }
    }
    return totalLoss;
}

double TracingObstacleLoss::computeObstacleLoss(Hz frequency, const Coord& transmissionPosition, const Coord& receptionPosition) const
{
    double totalLoss = 1;
    TotalObstacleLossComputation obstacleLossVisitor(this, frequency, transmissionPosition, receptionPosition);
    environment->visitObjects(&obstacleLossVisitor, LineSegment(transmissionPosition, receptionPosition));
    totalLoss = obstacleLossVisitor.getTotalLoss();
    return totalLoss;
}

TracingObstacleLoss::TotalObstacleLossComputation::TotalObstacleLossComputation(const TracingObstacleLoss *obstacleLoss, Hz frequency, const Coord& transmissionPosition, const Coord& receptionPosition) :
    totalLoss(1),
    obstacleLoss(obstacleLoss),
    frequency(frequency),
    transmissionPosition(transmissionPosition),
    receptionPosition(receptionPosition)
{
}

void TracingObstacleLoss::TotalObstacleLossComputation::visit(const cObject *object) const
{
    totalLoss *= obstacleLoss->computeObjectLoss(check_and_cast<const IPhysicalObject *>(object), frequency, transmissionPosition, receptionPosition);
}

} // namespace physicallayer

} // namespace inet

