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

#include "inet/common/PacketSlice.h"

namespace inet {

Register_Class(PacketSlice);

void PacketSlice::concat(PacketSlice *other)
{
    if ((getOffset() == -1 && other->getOffset() == -1) || (getOffset() + getByteLength() == other->getOffset())) {
        if (encapsulatedTotalPacket_var && other->encapsulatedTotalPacket_var && getEncapsulationId() == other->getEncapsulationId()) {
            addByteLength(other->getByteLength());
            return;
        }
        //TODO add implementation for RawPacket
    }
    throw cRuntimeError("invalid concatenation");
}


void PacketSlice::setPayload(cPacket *pk)
{
    offset_var = 0;
    encapsulatedTotalPacket_var = true;
    setByteLength(0);
    cPacket::encapsulate(pk);
}

cPacket *PacketSlice::getAndRemovePayload()
{
    return cPacket::decapsulate();
}

// Split at offset: 0..offset-1 returns in a new PacketSlice, offset..end remains in this.
PacketSlice *PacketSlice::splitAt(int64_t pos)
{
    if (pos >= getByteLength())
        throw cRuntimeError("Invalid 'pos' parameter");
    if (encapsulatedTotalPacket_var) {
        PacketSlice *newPacket = this->dup();
        addByteLength(-pos);
        if (offset_var != -1)
            offset_var += pos;
        newPacket->setByteLength(pos);
        return newPacket;
    }
    //TODO add implementation for RawPacket
    throw cRuntimeError("Slice in PacketSlice not yet implemented");
}

} // namespace inet

