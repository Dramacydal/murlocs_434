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

#ifndef TRANSPORTS_H
#define TRANSPORTS_H

#include "GameObject.h"
#include "Common.h"

#include <map>
#include <set>
#include <string>

class MovementInfo;

#define TRANSPORT_UPDATE_TIMER (1 * IN_MILLISECONDS)

class MANGOS_DLL_SPEC Transport : public GameObject
{
    public:
        explicit Transport();
        ~Transport();


        struct Passenger
        {
            Passenger(ObjectGuid _guid, uint32 _localGuid) : guid(_guid), localGuid(_localGuid) {}
            ObjectGuid guid;
            uint32 localGuid;
        };

        typedef std::list<Passenger> PassengerList;

        bool Create(uint32 guidlow, uint32 mapid, float x, float y, float z, float ang, uint8 animprogress, uint16 dynamicHighValue);
        bool GenerateWaypoints(uint32 pathid, std::set<uint32> &mapids);
        void Update(uint32 update_diff, uint32 p_time) override;
        void UpdateCreaturePosition(Creature* npc);
        bool AddPassenger(ObjectGuid passenger, uint32 localGuid = 0);
        bool RemovePassenger(ObjectGuid passenger);
        bool EnterThisTransport(Unit* pPas, float xOffset, float yOffset, float zOffset, float oOffset, uint32 localGuid = 0);
        bool LeaveThisTransport(Unit* pPas);

        void BuildStartMovePacket(Map const *targetMap);
        void BuildStopMovePacket(Map const *targetMap);

        PassengerList const& GetPassengers() const { return m_passengers; }
        Passenger* FindPassenger(ObjectGuid guid)
        {
            for (PassengerList::iterator itr = m_passengers.begin(); itr != m_passengers.end(); ++itr)
                if (itr->guid == guid)
                    return &*itr;

            return NULL;
        }

        void BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target);
        void SetStopped(bool values) { isStopped = values; }
        void LoadTransportAccessory();
        void CreatureUpdate();
        void UpdateMovementInfo(MovementInfo* mi);

    private:
        struct WayPoint
        {
            WayPoint() : mapid(0), x(0), y(0), z(0), teleport(false) {}
            WayPoint(uint32 _mapid, float _x, float _y, float _z, bool _teleport, uint32 _arrivalEventID = 0, uint32 _departureEventID = 0)
                : mapid(_mapid), x(_x), y(_y), z(_z), teleport(_teleport),
                arrivalEventID(_arrivalEventID), departureEventID(_departureEventID)
            {
            }

            uint32 mapid;
            float x;
            float y;
            float z;
            bool teleport;
            uint32 arrivalEventID;
            uint32 departureEventID;
        };

        typedef std::map<uint32, WayPoint> WayPointMap;

        WayPointMap::const_iterator m_curr;
        WayPointMap::const_iterator m_next;
        uint32 m_pathTime;
        uint32 m_timer;

        PassengerList m_passengers;
        bool isStopped;

        uint32 m_movementInfoCheckTimer;
        float miX, miY, miZ, miO;
        uint32 m_lastWaypointUpdateTime;
        uint32 m_lastMovementInfoTime;

    public:
        WayPointMap m_WayPoints;
        uint32 m_period;

    private:
        void TeleportTransport(uint32 newMapid, float x, float y, float z);
        void UpdateForMap(Map const* map);
        void DoEventIfAny(WayPointMap::value_type const& node, bool departure);
        void MoveToNextWayPoint();                          // move m_next/m_cur to next points
        void UpdatePosition();
        void UpdatePassengerList();
};
#endif
