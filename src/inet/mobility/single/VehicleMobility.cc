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

#include "inet/mobility/single/VehicleMobility.h"
#include <fstream>
#include <iostream>

namespace inet {

Define_Module(VehicleMobility);

void VehicleMobility::initialize(int stage)
{
    MovingMobilityBase::initialize(stage);
    EV_TRACE << "initializing VehicleMobility stage " << stage << endl;
    if (stage == INITSTAGE_LOCAL) {
        readWaypointsFromFile(par("waypointFile"));
        speed = par("speed");
        wayponitProximity = par("waypointProximity");
        targetPointIndex = 0;
        heading = 0;
        angularSpeed = 0;
    }
}

void VehicleMobility::setInitialPosition()
{
    lastPosition.x = waypoints[targetPointIndex].x;
    lastPosition.y = waypoints[targetPointIndex].y;
    lastSpeed.x = speed * cos(M_PI * heading / 180);
    lastSpeed.y = speed * sin(M_PI * heading / 180);
}

void VehicleMobility::readWaypointsFromFile(const char *fileName)
{
    char line[256];
    std::ifstream inputFile(fileName);
    while (true) {
        inputFile.getline(line, 256);
        if (!inputFile.fail()) {
            cStringTokenizer tokenizer(line);
            double x = atof(tokenizer.nextToken());
            double y = atof(tokenizer.nextToken());
            waypoints.push_back(Waypoint(x, y, 0.0));
        }
        else
            break;
    }
}

void VehicleMobility::move()
{
    Waypoint target = waypoints[targetPointIndex];
    double dx = target.x - lastPosition.x;
    double dy = target.y - lastPosition.y;
    if (dx*dx + dy*dy < wayponitProximity*wayponitProximity)  // reached so change to next (within the predefined proximity of the waypoint)
        targetPointIndex = (targetPointIndex+1) % waypoints.size();
    double targetDirection = atan2(dy, dx) / M_PI * 180;
    double diff = targetDirection - heading;
    while (diff < -180)
        diff += 360;
    while (diff > 180)
        diff -= 360;
    angularSpeed = diff * 5;
    double timeStep = (simTime() - lastUpdate).dbl();
    heading += angularSpeed * timeStep;
    double distance = speed * timeStep;
    lastPosition.x += distance * cos(M_PI * heading / 180);
    lastPosition.y += distance * sin(M_PI * heading / 180);
    lastSpeed.x = speed * cos(M_PI * heading / 180);
    lastSpeed.y = speed * sin(M_PI * heading / 180);
}

} // namespace inet

