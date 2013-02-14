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

#include "Common.h"

#include "Transports.h"
#include "TemporarySummon.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "ObjectGuid.h"
#include "Path.h"
#include "Unit.h"
#include "World.h"

#include "WorldPacket.h"
#include "DBCStores.h"
#include "ProgressBar.h"
#include "ScriptMgr.h"

void MapManager::LoadTransportCreatures()
{
    m_TransportCreatures.clear();

    //                                                 0       1     2      3             4             5             6              7
    QueryResult *result = WorldDatabase.Query("SELECT `guid`, `entry`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `emote` FROM `creature_transport`");
    if (!result)
    {
        sLog.outString(">> Loaded 0 transport creatures. DB table `creature_transport` is empty!");
        sLog.outString();
        return;
    }

    uint32 count = 0;

    do
    {
        Field* fields = result->Fetch();

        TransportCreatureInfo tinfo;
        tinfo.guid          = fields[0].GetUInt32();
        tinfo.entry         = fields[1].GetUInt32();
        tinfo.map           = fields[2].GetUInt32();
        tinfo.position_x    = fields[3].GetFloat();
        tinfo.position_y    = fields[4].GetFloat();
        tinfo.position_z    = fields[5].GetFloat();
        tinfo.orientation   = fields[6].GetFloat();
        tinfo.emote         = fields[7].GetUInt32();
        CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(tinfo.entry);
        if (!cinfo)
        {
            ERROR_LOG("Template for creature entry %u on trasport map %u does not exist", tinfo.entry, tinfo.map); 
            continue;
        }

        m_TransportCreatures[tinfo.map].push_back(tinfo);

        ++count;
    }
    while (result->NextRow());

    sLog.outString(">> Loaded %u transport creatures", count);
    sLog.outString();
}

void MapManager::LoadTransports()
{
    QueryResult *result = WorldDatabase.Query("SELECT `entry`, `name`, `period` FROM `transports`");

    uint32 count = 0;

    if( !result )
    {
        BarGoLink bar(1);
        bar.step();

        sLog.outString();
        sLog.outString( ">> Loaded %u transports", count );
        return;
    }

    BarGoLink bar(result->GetRowCount());

    do
    {
        bar.step();

        Transport *t = new Transport;

        Field *fields = result->Fetch();

        uint32 entry = fields[0].GetUInt32();
        std::string name = fields[1].GetCppString();
        t->m_period = fields[2].GetUInt32();

        const GameObjectInfo *goinfo = ObjectMgr::GetGameObjectInfo(entry);

        if (!goinfo)
        {
            sLog.outErrorDb("Transport ID:%u, Name: %s, will not be loaded, gameobject_template missing", entry, name.c_str());
            delete t;
            continue;
        }

        if (goinfo->type != GAMEOBJECT_TYPE_MO_TRANSPORT)
        {
            sLog.outErrorDb("Transport ID:%u, Name: %s, will not be loaded, gameobject_template type wrong", entry, name.c_str());
            delete t;
            continue;
        }

        // sLog.outString("Loading transport %d between %s, %s", entry, name.c_str(), goinfo->name);

        std::set<uint32> mapsUsed;

        if (!t->GenerateWaypoints(goinfo->moTransport.taxiPathId, mapsUsed))
            // skip transports with empty waypoints list
        {
            sLog.outErrorDb("Transport (path id %u) path size = 0. Transport ignored, check DBC files or transport GO data0 field.",goinfo->moTransport.taxiPathId);
            delete t;
            continue;
        }

        float x, y, z, o;
        uint32 mapid;
        x = t->m_WayPoints[0].x; y = t->m_WayPoints[0].y; z = t->m_WayPoints[0].z; mapid = t->m_WayPoints[0].mapid; o = 1;

        //current code does not support transports in dungeon!
        const MapEntry* pMapInfo = sMapStore.LookupEntry(mapid);
        if(!pMapInfo || pMapInfo->Instanceable())
        {
            delete t;
            continue;
        }

        // creates the Gameobject
        if (!t->Create(entry, mapid, x, y, z, o, GO_ANIMPROGRESS_DEFAULT, 0))
        {
            delete t;
            continue;
        }

        m_Transports.insert(t);

        for (std::set<uint32>::const_iterator i = mapsUsed.begin(); i != mapsUsed.end(); ++i)
            m_TransportsByMap[*i].insert(t);

        //If we someday decide to use the grid to track transports, here:
        t->SetMap(sMapMgr.CreateMap(mapid, t));

        t->LoadTransportAccessory();
        //t->GetMap()->Add<GameObject>((GameObject *)t);
        ++count;
    } while(result->NextRow());
    delete result;

    sLog.outString();
    sLog.outString( ">> Loaded %u transports", count );

    // check transport data DB integrity
    result = WorldDatabase.Query("SELECT gameobject.guid,gameobject.id,transports.name FROM gameobject,transports WHERE gameobject.id = transports.entry");
    if (result)                                              // wrong data found
    {
        do
        {
            Field *fields = result->Fetch();

            uint32 guid  = fields[0].GetUInt32();
            uint32 entry = fields[1].GetUInt32();
            std::string name = fields[2].GetCppString();
            sLog.outErrorDb("Transport %u '%s' have record (GUID: %u) in `gameobject`. Transports MUST NOT have any records in `gameobject` or its behavior will be unpredictable/bugged.",entry,name.c_str(),guid);
        }
        while(result->NextRow());

        delete result;
    }
}

Transport::Transport() : GameObject(), m_movementInfoCheckTimer(TRANSPORT_UPDATE_TIMER)
{
    m_updateFlag = UPDATEFLAG_TRANSPORT | UPDATEFLAG_HAS_POSITION | UPDATEFLAG_ROTATION;
    m_lastMovementInfoTime = 0;
    m_lastWaypointUpdateTime = 0;
    miX = miY = miZ = miO = 0.0f;
}

Transport::~Transport()
{
    DEBUG_LOG("Destroying transport %s", GetName());
    for (PassengerList::const_iterator itr = m_passengers.begin(); itr != m_passengers.end();)
    {
        Unit* unit = GetMap()->GetUnit(itr->guid);
        if (!unit)
        {
            ++itr;
            continue;
        }

        if (LeaveThisTransport(unit))
        {
            itr = m_passengers.begin();
            continue;
        }

        ++itr;
    }
}

bool Transport::Create(uint32 guidlow, uint32 mapid, float x, float y, float z, float ang, uint8 animprogress, uint16 dynamicHighValue)
{
    Relocate(x, y, z, ang);
    // instance id and phaseMask isn't set to values different from std.

    if(!IsPositionValid())
    {
        ERROR_LOG("Transport (GUID: %u) not created. Suggested coordinates isn't valid (X: %f Y: %f)",
            guidlow,x,y);
        return false;
    }

    Object::_Create(guidlow, 0, HIGHGUID_MO_TRANSPORT);

    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(guidlow);

    if (!goinfo)
    {
        sLog.outErrorDb("Transport not created: entry in `gameobject_template` not found, guidlow: %u map: %u  (X: %f Y: %f Z: %f) ang: %f",guidlow, mapid, x, y, z, ang);
        return false;
    }

    m_goInfo = goinfo;

    SetObjectScale(goinfo->size);

    SetUInt32Value(GAMEOBJECT_FACTION, goinfo->faction);
    //SetUInt32Value(GAMEOBJECT_FLAGS, goinfo->flags);
    SetUInt32Value(GAMEOBJECT_FLAGS, (GO_FLAG_TRANSPORT | GO_FLAG_NODESPAWN));
    SetUInt32Value(GAMEOBJECT_LEVEL, m_period);
    SetEntry(goinfo->id);

    //SetDisplayId(goinfo->displayId);
    // Use SetDisplayId only if we have the GO assigned to a proper map!
    SetUInt32Value(GAMEOBJECT_DISPLAYID, goinfo->displayId);
    m_displayInfo = sGameObjectDisplayInfoStore.LookupEntry(goinfo->displayId);

    SetGoState(GO_STATE_READY);
    SetGoType(GameobjectTypes(goinfo->type));
    SetGoArtKit(0);
    SetGoAnimProgress(animprogress);

    // low part always 0, dynamicHighValue is some kind of progression (not implemented)
    SetUInt16Value(GAMEOBJECT_DYNAMIC, 0, 0);
    SetUInt16Value(GAMEOBJECT_DYNAMIC, 1, dynamicHighValue);

    SetName(goinfo->name);

    return true;
}

struct keyFrame
{
    explicit keyFrame(TaxiPathNodeEntry const& _node) : node(&_node),
        distSinceStop(-1.0f), distUntilStop(-1.0f), distFromPrev(-1.0f), tFrom(0.0f), tTo(0.0f)
    {
    }

    TaxiPathNodeEntry const* node;

    float distSinceStop;
    float distUntilStop;
    float distFromPrev;
    float tFrom, tTo;
};

bool Transport::GenerateWaypoints(uint32 pathid, std::set<uint32> &mapids)
{
    if (pathid >= sTaxiPathNodesByPath.size())
        return false;

    TaxiPathNodeList const& path = sTaxiPathNodesByPath[pathid];

    std::vector<keyFrame> keyFrames;
    int mapChange = 0;
    mapids.clear();
    for (size_t i = 1; i < path.size() - 1; ++i)
    {
        if (mapChange == 0)
        {
            TaxiPathNodeEntry const& node_i = path[i];
            if (node_i.mapid == path[i+1].mapid)
            {
                keyFrame k(node_i);
                keyFrames.push_back(k);
                mapids.insert(k.node->mapid);
            }
            else
            {
                mapChange = 1;
            }
        }
        else
        {
            --mapChange;
        }
    }

    int lastStop = -1;
    int firstStop = -1;

    // first cell is arrived at by teleportation :S
    keyFrames[0].distFromPrev = 0;
    if (keyFrames[0].node->actionFlag == 2)
    {
        lastStop = 0;
    }

    // find the rest of the distances between key points
    for (size_t i = 1; i < keyFrames.size(); ++i)
    {
        if ((keyFrames[i].node->actionFlag == 1) || (keyFrames[i].node->mapid != keyFrames[i-1].node->mapid))
        {
            keyFrames[i].distFromPrev = 0;
        }
        else
        {
            keyFrames[i].distFromPrev =
                sqrt(pow(keyFrames[i].node->x - keyFrames[i - 1].node->x, 2) +
                    pow(keyFrames[i].node->y - keyFrames[i - 1].node->y, 2) +
                    pow(keyFrames[i].node->z - keyFrames[i - 1].node->z, 2));
        }
        if (keyFrames[i].node->actionFlag == 2)
        {
            // remember first stop frame
            if(firstStop == -1)
                firstStop = i;
            lastStop = i;
        }
    }

    float tmpDist = 0;
    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        int j = (i + lastStop) % keyFrames.size();
        if (keyFrames[j].node->actionFlag == 2)
            tmpDist = 0;
        else
            tmpDist += keyFrames[j].distFromPrev;
        keyFrames[j].distSinceStop = tmpDist;
    }

    for (int i = int(keyFrames.size()) - 1; i >= 0; --i)
    {
        int j = (i + (firstStop+1)) % keyFrames.size();
        tmpDist += keyFrames[(j + 1) % keyFrames.size()].distFromPrev;
        keyFrames[j].distUntilStop = tmpDist;
        if (keyFrames[j].node->actionFlag == 2)
            tmpDist = 0;
    }

    for (size_t i = 0; i < keyFrames.size(); ++i)
    {
        if (keyFrames[i].distSinceStop < (30 * 30 * 0.5f))
            keyFrames[i].tFrom = sqrt(2 * keyFrames[i].distSinceStop);
        else
            keyFrames[i].tFrom = ((keyFrames[i].distSinceStop - (30 * 30 * 0.5f)) / 30) + 30;

        if (keyFrames[i].distUntilStop < (30 * 30 * 0.5f))
            keyFrames[i].tTo = sqrt(2 * keyFrames[i].distUntilStop);
        else
            keyFrames[i].tTo = ((keyFrames[i].distUntilStop - (30 * 30 * 0.5f)) / 30) + 30;

        keyFrames[i].tFrom *= 1000;
        keyFrames[i].tTo *= 1000;
    }

    //    for (int i = 0; i < keyFrames.size(); ++i) {
    //        sLog.outString("%f, %f, %f, %f, %f, %f, %f", keyFrames[i].x, keyFrames[i].y, keyFrames[i].distUntilStop, keyFrames[i].distSinceStop, keyFrames[i].distFromPrev, keyFrames[i].tFrom, keyFrames[i].tTo);
    //    }

    // Now we're completely set up; we can move along the length of each waypoint at 100 ms intervals
    // speed = max(30, t) (remember x = 0.5s^2, and when accelerating, a = 1 unit/s^2
    int t = 0;
    bool teleport = false;
    if (keyFrames[keyFrames.size() - 1].node->mapid != keyFrames[0].node->mapid)
        teleport = true;

    WayPoint pos(keyFrames[0].node->mapid, keyFrames[0].node->x, keyFrames[0].node->y, keyFrames[0].node->z, teleport,
        keyFrames[0].node->arrivalEventID, keyFrames[0].node->departureEventID);
    m_WayPoints[0] = pos;
    t += keyFrames[0].node->delay * 1000;

    uint32 cM = keyFrames[0].node->mapid;
    for (size_t i = 0; i < keyFrames.size() - 1; ++i)
    {
        float d = 0;
        float tFrom = keyFrames[i].tFrom;
        float tTo = keyFrames[i].tTo;

        // keep the generation of all these points; we use only a few now, but may need the others later
        if (((d < keyFrames[i + 1].distFromPrev) && (tTo > 0)))
        {
            while ((d < keyFrames[i + 1].distFromPrev) && (tTo > 0))
            {
                tFrom += 100;
                tTo -= 100;

                if (d > 0)
                {
                    float newX, newY, newZ;
                    newX = keyFrames[i].node->x + (keyFrames[i + 1].node->x - keyFrames[i].node->x) * d / keyFrames[i + 1].distFromPrev;
                    newY = keyFrames[i].node->y + (keyFrames[i + 1].node->y - keyFrames[i].node->y) * d / keyFrames[i + 1].distFromPrev;
                    newZ = keyFrames[i].node->z + (keyFrames[i + 1].node->z - keyFrames[i].node->z) * d / keyFrames[i + 1].distFromPrev;

                    bool teleport = false;
                    if (keyFrames[i].node->mapid != cM)
                    {
                        teleport = true;
                        cM = keyFrames[i].node->mapid;
                    }

                    //                    sLog.outString("T: %d, D: %f, x: %f, y: %f, z: %f", t, d, newX, newY, newZ);
                    WayPoint pos(keyFrames[i].node->mapid, newX, newY, newZ, teleport);
                    if (teleport)
                        m_WayPoints[t] = pos;
                }

                if (tFrom < tTo)                            // caught in tFrom dock's "gravitational pull"
                {
                    if (tFrom <= 30000)
                    {
                        d = 0.5f * (tFrom / 1000) * (tFrom / 1000);
                    }
                    else
                    {
                        d = 0.5f * 30 * 30 + 30 * ((tFrom - 30000) / 1000);
                    }
                    d = d - keyFrames[i].distSinceStop;
                }
                else
                {
                    if (tTo <= 30000)
                    {
                        d = 0.5f * (tTo / 1000) * (tTo / 1000);
                    }
                    else
                    {
                        d = 0.5f * 30 * 30 + 30 * ((tTo - 30000) / 1000);
                    }
                    d = keyFrames[i].distUntilStop - d;
                }
                t += 100;
            }
            t -= 100;
        }

        if (keyFrames[i + 1].tFrom > keyFrames[i + 1].tTo)
            t += 100 - ((long)keyFrames[i + 1].tTo % 100);
        else
            t += (long)keyFrames[i + 1].tTo % 100;

        bool teleport = false;
        if ((keyFrames[i + 1].node->actionFlag == 1) || (keyFrames[i + 1].node->mapid != keyFrames[i].node->mapid))
        {
            teleport = true;
            cM = keyFrames[i + 1].node->mapid;
        }

        WayPoint pos(keyFrames[i + 1].node->mapid, keyFrames[i + 1].node->x, keyFrames[i + 1].node->y, keyFrames[i + 1].node->z, teleport,
            keyFrames[i + 1].node->arrivalEventID, keyFrames[i + 1].node->departureEventID);

        //        sLog.outString("T: %d, x: %f, y: %f, z: %f, t:%d", t, pos.x, pos.y, pos.z, teleport);

        //if (teleport)
        m_WayPoints[t] = pos;

        t += keyFrames[i + 1].node->delay * 1000;
        //        sLog.outString("------");
    }

    uint32 timer = t;

    //    DETAIL_LOG("    Generated %lu waypoints, total time %u.", (unsigned long)m_WayPoints.size(), timer);

    m_next = m_WayPoints.begin();                           // will used in MoveToNextWayPoint for init m_curr
    MoveToNextWayPoint();                                   // m_curr -> first point
    MoveToNextWayPoint();                                   // skip first point

    m_pathTime = timer;

    return true;
}

void Transport::TeleportTransport(uint32 newMapid, float x, float y, float z)
{
    DEBUG_LOG("***** TELEPORTING %s START *****", GetGuidStr().c_str());
    UpdatePassengerList();

    Map* oldMap = GetMap();
    Map* newMap = sMapMgr.CreateMap(newMapid, this);

    if (oldMap != newMap)
        UpdateForMap(newMap);

    {
        for (PassengerList::iterator itr = m_passengers.begin(); itr != m_passengers.end();)
        {
            Unit* unit = oldMap->GetUnit(itr->guid);
            if (!unit)
            {
                ++itr;
                continue;
            }

            bool added = false;
            if (unit->GetVehicleKit())
            {
                for (int8 i = 0; i < MAX_VEHICLE_SEAT; ++i)
                {
                    if (Unit* passenger = unit->GetVehicleKit()->GetPassenger(i))
                    {
                        if (passenger->GetTypeId() == TYPEID_PLAYER)
                        {
                            passenger->ExitVehicle();
                            added = true;
                        }
                    }
                }
            }

            ++itr;

            if (added)
                itr = m_passengers.begin();
        }

        for (PassengerList::iterator itr = m_passengers.begin(); itr != m_passengers.end();)
        {
            // players and pets do not have localGuid
            if (!itr->localGuid)
            {
                ++itr;
                continue;
            }

            Unit* unit = oldMap->GetUnit(itr->guid);
            if (!unit)
            {
                ++itr;
                continue;
            }

            if (LeaveThisTransport(unit))
            {
                ((Creature*)unit)->ForcedDespawn();
                itr = m_passengers.begin();
            }

            ++itr;
        }
    }

    Relocate(x, y, z);

    // we need to create and save new Map object with 'newMapid' because if not done -> lead to invalid Map object reference...
    // player far teleport would try to create same instance, but we need it NOW for transport...
    // correct me if I'm wrong O.o

    RemoveFromWorld();
    ResetMap();
    newMap->Add((GameObject*)this);
    MANGOS_ASSERT(GetMap());
    AddToWorld();

    {
        for (PassengerList::iterator itr = m_passengers.begin(); itr != m_passengers.end(); ++itr)
        {
            Unit* unit = oldMap->GetUnit(itr->guid);
            if (!unit)
                continue;

            if (unit->GetTypeId() == TYPEID_PLAYER)
            {
                Player* pPlayer = (Player*)unit;
                if (pPlayer->isDead() && !pPlayer->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST))
                    pPlayer->ResurrectPlayer(1.0f);
                pPlayer->TeleportTo(newMapid, x, y, z, GetOrientation(), TELE_TO_NOT_LEAVE_TRANSPORT);
            }
        }
    }

    LoadTransportAccessory();

    if (oldMap != newMap)
        UpdateForMap(newMap);

    DEBUG_LOG("***** TELEPORTING %s END *****", GetGuidStr().c_str());
}

bool Transport::AddPassenger(ObjectGuid passengerGuid, uint32 localGuid)
{
    if (!passengerGuid)
        return false;

    for (PassengerList::const_iterator itr = m_passengers.begin(); itr != m_passengers.end(); ++itr)
        if (itr->guid == passengerGuid)
            return false;

    m_passengers.push_back(Passenger(passengerGuid, localGuid));

    DEBUG_LOG("%s boarded transport %s.", passengerGuid.GetString().c_str(), GetName());

    return true;
}

bool Transport::RemovePassenger(ObjectGuid passengerGuid)
{
    if (!passengerGuid)
        return false;

    for (PassengerList::iterator itr = m_passengers.begin(); itr != m_passengers.end(); ++itr)
        if (itr->guid == passengerGuid)
        {
            m_passengers.erase(itr);
            DEBUG_LOG("%s removed from transport %s.", passengerGuid.GetString().c_str(), GetName());
            return true;
        }

    return false;
}

void Transport::BuildCreateUpdateBlockForPlayer(UpdateData* data, Player* target)
{
    for (PassengerList::const_iterator itr = m_passengers.begin(); itr != m_passengers.end(); ++itr)
    {
        if (!itr->guid.IsCreatureOrVehicle())
            continue;

        Unit* unit = GetMap()->GetUnit(itr->guid);
        if (!unit)
            continue;

        unit->BuildCreateUpdateBlockForPlayer(data, target);
    }

    Object::BuildCreateUpdateBlockForPlayer(data, target);
}

void Transport::CreatureUpdate()
{
    for (PassengerList::const_iterator itr = m_passengers.begin(); itr != m_passengers.end(); ++itr)
    {
        if (!itr->guid.IsCreatureOrVehicle())
            continue;

        Unit* npc = GetMap()->GetUnit(itr->guid);
        if (!npc)
            continue;

        UpdateCreaturePosition((Creature*)npc);
    }
}

void Transport::MoveToNextWayPoint()
{
    m_curr = m_next;

    ++m_next;
    if (m_next == m_WayPoints.end())
        m_next = m_WayPoints.begin();
}

void Transport::Update( uint32 update_diff, uint32 /*p_time*/)
{
    if (m_WayPoints.size() <= 1)
        return;

    m_timer = WorldTimer::getMSTime() % m_period;
    while (((m_timer - m_curr->first) % m_pathTime) > ((m_next->first - m_curr->first) % m_pathTime))
    {
        const MapEntry* pMapInfo = sMapStore.LookupEntry(GetMapId());
        if ((m_next->second.mapid != GetMapId() || m_next->second.teleport) && !pMapInfo->Instanceable())
            TeleportTransport(m_next->second.mapid, m_next->second.x, m_next->second.y, m_next->second.z);
        else
        {
            Relocate(m_next->second.x, m_next->second.y, m_next->second.z);
            CreatureUpdate();
        }
        m_lastWaypointUpdateTime = WorldTimer::getMSTime();
        DoEventIfAny(*m_curr,true);
        MoveToNextWayPoint();
        DoEventIfAny(*m_curr,false);
    }
    
    if (m_movementInfoCheckTimer < update_diff)
    {
        m_movementInfoCheckTimer = TRANSPORT_UPDATE_TIMER;
        UpdatePosition();
    }
    else
        m_movementInfoCheckTimer -= update_diff;

    if (m_curr == m_WayPoints.begin())
        DETAIL_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, " ************ BEGIN ************** %s", GetName());
    DETAIL_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, "%s moved to %f %f %f %d", GetName(), m_curr->second.x, m_curr->second.y, m_curr->second.z, m_curr->second.mapid);
}

void Transport::UpdateCreaturePosition(Creature* npc)
{
    float transX = GetPositionX();
    float transY = GetPositionY();
    float transZ = GetPositionZ();
    float transO = GetOrientation();
    float xOffset = (npc->GetTransOffsetX() * cos(transO) - npc->GetTransOffsetY() * sin(transO));
    float yOffset = (npc->GetTransOffsetY() * cos(transO) + npc->GetTransOffsetX() * sin(transO));
    CreatureCreatePos pos(GetMap(), transX + xOffset, transY + yOffset, transZ + npc->GetTransOffsetZ(), transO + npc->GetTransOffsetO(), GetPhaseMask());
    npc->SetRespawnCoord(pos);
    npc->SetPosition(pos.m_pos.x, pos.m_pos.y, pos.m_pos.z, pos.m_pos.o);
}

void Transport::UpdateForMap(Map const* targetMap)
{
    Map::PlayerList const& pl = targetMap->GetPlayers();
    if (pl.isEmpty())
        return;

    if(GetMapId() == targetMap->GetId())
    {
        for(Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
        {
            if(this != itr->getSource()->GetTransport())
            {
                UpdateData transData(itr->getSource()->GetMapId());
                BuildCreateUpdateBlockForPlayer(&transData, itr->getSource());
                WorldPacket packet;
                transData.BuildPacket(&packet);

                // Prevent sending transport maps in player update object
                if (packet.ReadUInt16() != itr->getSource()->GetMapId())
                    return;

                itr->getSource()->SendDirectMessage(&packet);
            }
        }
    }
    else
    {
        UpdateData transData(GetMapId());
        BuildOutOfRangeUpdateBlock(&transData);
        WorldPacket out_packet;
        transData.BuildPacket(&out_packet);

        for (Map::PlayerList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
        {
            if (this != itr->getSource()->GetTransport())
            {
                // Prevent sending transport maps in player update object
                if (out_packet.ReadUInt16() != itr->getSource()->GetMapId())
                    return;

                itr->getSource()->SendDirectMessage(&out_packet);
            }
        }
    }
}

void Transport::DoEventIfAny(WayPointMap::value_type const& node, bool departure)
{
    if (uint32 eventid = departure ? node.second.departureEventID : node.second.arrivalEventID)
    {
        DEBUG_FILTER_LOG(LOG_FILTER_TRANSPORT_MOVES, "Taxi %s event %u of node %u of %s \"%s\") path", departure ? "departure" : "arrival", eventid, node.first, GetGuidStr().c_str(), GetName());

        if (!sScriptMgr.OnProcessEvent(eventid, this, this, departure))
            GetMap()->ScriptsStart(sEventScripts, eventid, this, this);
    }
}

void Transport::BuildStartMovePacket(Map const* targetMap)
{
    SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    SetGoState(GO_STATE_ACTIVE);
    UpdateForMap(targetMap);
}

void Transport::BuildStopMovePacket(Map const* targetMap)
{
    RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_IN_USE);
    SetGoState(GO_STATE_READY);
    UpdateForMap(targetMap);
}

bool Transport::EnterThisTransport(Unit* pPas, float xOffset, float yOffset, float zOffset, float oOffset, uint32 localGuid)
{
    if (!pPas)
        return false;

    if (!AddPassenger(pPas->GetObjectGuid(), localGuid))
        return false;

    Map* map = GetMap();

    pPas->SetTransport(this);
    pPas->m_movementInfo.SetTransportData(GetObjectGuid(), xOffset, yOffset, zOffset, oOffset, 0, -1);

    if (pPas->GetTypeId() != TYPEID_PLAYER)
    {
        pPas->SetActiveObjectState(true);
        //pPas->SendMonsterMoveTransport(this);
        UpdateCreaturePosition((Creature*)pPas);
    }

    return true;
}

bool Transport::LeaveThisTransport(Unit* pPas)
{
    if (!pPas)
        return false;

    if (!RemovePassenger(pPas->GetObjectGuid()))
        return false;

    if (pPas->GetTypeId() != TYPEID_PLAYER)
        pPas->SetActiveObjectState(false);

    pPas->SetTransport(NULL);
    pPas->m_movementInfo.ClearTransportData();
    //pPas->MonsterMoveWithSpeed(pPas->GetPositionX(), pPas->GetPositionY(), pPas->GetPositionZ() + 0.5f, 28);

    return true;
}

void Transport::LoadTransportAccessory()
{
    Map* map = GetMap();
    if (!map)
        return;

    uint32 mapId = GetGOInfo()->moTransport.mapID;
    if (!mapId)
        return;

    TransportCreatureMap::const_iterator itr = sMapMgr.m_TransportCreatures.find(mapId);
    if (itr == sMapMgr.m_TransportCreatures.end())
        return;

    TransportCreatureList crlist = itr->second;

    for (TransportCreatureList::const_iterator itr2 = crlist.begin(); itr2 != crlist.end(); ++itr2)
    {
        TransportCreatureInfo data = *itr2;

        uint32 id = data.entry;

        CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(id);
        if (!cinfo)
            continue;

        float xOffset, yOffset;
        xOffset = data.position_x * cos(GetOrientation()) - data.position_y * sin(GetOrientation());
        yOffset = data.position_y * cos(GetOrientation()) + data.position_x * sin(GetOrientation());
        CreatureCreatePos pos(map, GetPositionX() + xOffset, GetPositionY() + yOffset, GetPositionZ() + data.position_z, GetOrientation() + data.orientation, GetPhaseMask());

        uint32 guid = map->GenerateLocalLowGuid(cinfo->GetHighGuid());
        if (!guid)
        {
            ERROR_LOG("Can't generate local low guid for transport %s and creature %u", GetGuidStr().c_str(), id);
            continue;
        }

        Creature* pCreature = new Creature;

        if (!pCreature->Create(guid, pos, cinfo))
        {
            ERROR_LOG("Can't spawn creature %u on transport %s", id, GetGuidStr().c_str());
            delete pCreature;
            continue;
        }

        pCreature->Relocate(GetPositionX() + xOffset, GetPositionY() + yOffset, GetPositionZ() + data.position_z, GetOrientation() + data.orientation);

        if (!pCreature->IsPositionValid())
        {
            ERROR_LOG("Creature (guidlow %d, entry %d) not created. Suggested coordinates aren't valid (X: %f Y: %f)", pCreature->GetGUIDLow(), pCreature->GetEntry(), pCreature->GetPositionX(), pCreature->GetPositionY());
            delete pCreature;
            continue;
        }

        pCreature->AIM_Initialize();

        map->Add(pCreature);
        if (data.emote)
            pCreature->HandleEmote(data.emote);

        if (!EnterThisTransport(pCreature, data.position_x, data.position_y, data.position_z, data.orientation, data.guid))
            pCreature->ForcedDespawn();
    }
}

void Transport::UpdatePosition()
{
    if (!m_lastMovementInfoTime || m_lastWaypointUpdateTime >= m_lastMovementInfoTime)
        return;

    Relocate(miX, miY, miZ, miO);
    CreatureUpdate();
}

void Transport::UpdateMovementInfo(MovementInfo* mi)
{
    miO = mi->GetPos()->o - mi->GetTransportPos()->o;
    miX = mi->GetPos()->x - (mi->GetTransportPos()->x * cos(miO) - mi->GetTransportPos()->y * sin(miO));
    miY = mi->GetPos()->y - (mi->GetTransportPos()->y * cos(miO) + mi->GetTransportPos()->x * sin(miO));
    miZ = mi->GetPos()->z - mi->GetTransportPos()->z;

    m_lastMovementInfoTime = WorldTimer::getMSTime();
}

void Transport::UpdatePassengerList()
{
    for (PassengerList::iterator itr = m_passengers.begin(); itr != m_passengers.end();)
    {
        // should never happen
        /*if (!itr->guid)
        {
            m_passengers.erase(itr);
            itr = m_passengers.begin();
            continue;
        }*/

        Unit* unit = GetMap()->GetUnit(itr->guid);
        if (!unit)
        {
            m_passengers.erase(itr);
            itr = m_passengers.begin();
            continue;
        }

        ++itr;
    }
}
