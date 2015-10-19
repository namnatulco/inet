//
// Copyright (C) 2015 OpenSim Ltd.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// Author: Zoltan Bojthe
//

cplusplus {{
#include "inet/common/INETDefs.h"

typedef cPacket *cPacketPtr;
}}

namespace inet;

//
// Class that carries a packet slice
// bitLength/byteLength contains the size of slice
// encapsulated packet is the entire packet with full length
// (getEncapsulatedPacket()->getByteLength > getByteLength()),
// or a RawPacket with bytes of slice (not implemented yet)
// (getEncapsulatedPacket()->getByteLength == getByteLength()),
//
packet PacketSlice
{
    @customize(true);
    @fieldNameSuffix("_var"); //TODO remove when OMNeT++ 4.x compatibility is no longer required
    int64_t offset = -1;    // -1 means unknown offset, encapsulated packet should be a RawPacket contains only the slice
    bool encapsulatedTotalPacket = true;
}

