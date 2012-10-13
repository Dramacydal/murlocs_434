/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
#ifndef MANGOSSERVER_PACKET_BUILDER_H
#define MANGOSSERVER_PACKET_BUILDER_H

#include "MoveSpline.h"

class ByteBuffer;
class WorldPacket;

namespace Movement
{
    class MoveSpline;
    class PacketBuilder
    {
        static void WriteCommonMonsterMovePart(const MoveSpline& mov, WorldPacket& data);
    public:

        static void WriteLinearPath(const MoveSpline& move_spline, ByteBuffer& data);
        static void WriteCatmullRomPath(const MoveSpline& move_spline, ByteBuffer& data);
        static void WriteCatmullRomCyclicPath(const MoveSpline& move_spline, ByteBuffer& data);

        static Vector3 CalcTransportOffset(const MoveSpline& move_spline, Vector3 oldVec);

            static void WriteMonsterMove(const MoveSpline& mov, WorldPacket& data);
            static void WriteCreateBits(const MoveSpline& mov, ByteBuffer& data);
            static void WriteCreateBytes(const MoveSpline& mov, ByteBuffer& data);
    };
}
#endif // MANGOSSERVER_PACKET_BUILDER_H
