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

#ifndef __INET_IEEE80211MACMACPROCESSBASE_H
#define __INET_IEEE80211MACMACPROCESSBASE_H

#include "inet/common/INETDefs.h"
#include "inet/physicallayer/base/packetlevel/PhysicalLayerDefs.h"
//#include "inet/common/INETMath.h"
//#include "inet/common/Units.h"

namespace inet {
namespace ieee80211 {

class INET_API Ieee80211MacMacProcessBase : public cSimpleModule
{
    protected:
        cMessage *resetMac = nullptr;

    protected:
        virtual int numInitStages() const override { return NUM_INIT_STAGES; }

    public:
        virtual ~Ieee80211MacMacProcessBase();

        virtual void emitResetMac();
};

} /* namespace inet */
} /* namespace ieee80211 */

#endif // ifndef __INET_IEEE80211MACMACPROCESSBASE_H