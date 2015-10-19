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

#ifndef _INET_PACKETSLICE_H_
#define _INET_PACKETSLICE_H_

#include "inet/common/INETDefs.h"

#include "inet/common/PacketSlice_m.h"

namespace inet {

class INET_API PacketSlice : public PacketSlice_Base
{
  private:
    void copy(const PacketSlice& other) {}
    cPacket *payload = nullptr;

  private:
    void getencapsulatedPacket(cPacket *pk) { throw cRuntimeError("Do not use getencapsulatedPacket()"); }
    void encapsulate(cPacket *pk) { throw cRuntimeError("Do not use encapsulate()"); }
    cPacket *decapsulate() { throw cRuntimeError("Do not use decapsulate()"); }

  public:
    PacketSlice(const char *name=NULL, int kind=0) : PacketSlice_Base(name,kind) {}
    PacketSlice(const PacketSlice& other) : PacketSlice_Base(other) {copy(other);}
    PacketSlice& operator=(const PacketSlice& other) {if (this==&other) return *this; PacketSlice_Base::operator=(other); copy(other); return *this;}
    virtual PacketSlice *dup() const { return new PacketSlice(*this); }

    void concat(PacketSlice *other);
    void setPayload(cPacket* pk);
    cPacket *getAndRemovePayload();     //TODO rename

    /// Split at offset, 0..offset-1 remains in this, offset..end returns in a new PacketSlice
    PacketSlice *splitAt(int64_t offset);
};



} // namespace inet

#endif // ifndef _INET_PACKETSLICE_H_

