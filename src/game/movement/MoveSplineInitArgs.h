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

#ifndef MANGOSSERVER_MOVESPLINEINIT_ARGS_H
#define MANGOSSERVER_MOVESPLINEINIT_ARGS_H

#include "MoveSplineFlag.h"
#include <G3D/Vector3.h>

class Unit;

namespace Movement
{
    struct Location : public Vector3
    {
        Location() : orientation(0) {}
        Location(float x, float y, float z, float o) : Vector3(x,y,z), orientation(o) {}
        Location(const Vector3& v) : Vector3(v), orientation(0) {}
        Location(const Vector3& v, float o) : Vector3(v), orientation(o) {}

        float orientation;
    };

    typedef std::vector<Vector3> PointsArray;

    union FacingInfo
    {
        struct{
            float x,y,z;
        }f;
        uint64  target;
        float   angle;

        FacingInfo(float o) : angle(o) {}
        FacingInfo(uint64 t) : target(t) {}
        FacingInfo() {}
    };

    struct MoveSplineInitArgs
    {
        MoveSplineInitArgs(size_t path_capacity = 16) : path_Idx_offset(0),
            velocity(0.f), parabolic_amplitude(0.f), time_perc(0.f), splineId(0), initialOrientation(0.f)
        {
            path.reserve(path_capacity);
            transportGuid = 0;
            transportSeat = -1;
        }

        PointsArray path;
        FacingInfo facing;
        MoveSplineFlag flags;
        int32 path_Idx_offset;
        float velocity;
        float parabolic_amplitude;
        float time_perc;
        uint32 splineId;
        float initialOrientation;
        uint64 transportGuid;
        int8 transportSeat;
        Location transportPos;

        /** Returns true to show that the arguments were configured correctly and MoveSpline initialization will succeed. */
        bool Validate(Unit* unit) const;

        void SetTransportData(Unit& unit);
    private:
        bool _checkPathBounds() const;
    };
}

#endif // MANGOSSERVER_MOVESPLINEINIT_ARGS_H
