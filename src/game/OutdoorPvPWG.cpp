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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "OutdoorPvP.h"
#include "OutdoorPvPWG.h"
#include "GameObject.h"
#include "MapManager.h"
#include "SpellAuras.h"

OutdoorPvPWG::OutdoorPvPWG(uint8 _id) : OutdoorPvP(_id)
{
    m_defender = TeamIndex(urand(0, 1));

    m_state = WG_STATE_COOLDOWN;
    m_timer = 15 * MINUTE * IN_MILLISECONDS;

    m_damagedTowers[0] = 0;
    m_damagedTowers[1] = 0;
    m_destroyedTowers[0] = 0;
    m_destroyedTowers[1] = 0;

    m_startTime = 0;
    bInvited = false;

    m_BattlefieldId = BATTLEFIELD_WG;
    m_BattlefieldZoneId = ZONE_ID_WINTERGRASP;

    m_queueUpdateTimer = 30000;
    m_scoresUpdateTimer = 5000;

    bAboutSend = false;

    b10MinAchiev = false;
}

OutdoorPvPWG::~OutdoorPvPWG()
{
    for (uint8 i = 0; i < WG_WORKSHOP_COUNT; ++i)
        delete m_workshops[i];

    for (uint8 i = 0; i < WG_KEEPTOWER_COUNT; ++i)
        delete m_keepTowers[i];

    for (uint8 i = 0; i < WG_TOWER_COUNT; ++i)
        delete m_towers[i];

    for (std::list<WGObject*>::iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
        delete *itr;

    for (WGPlayerScoreMap::iterator itr = m_playerScores.begin(); itr != m_playerScores.end(); ++itr)
        delete itr->second;

    for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
        for (std::set<Group*>::iterator itr = m_Raids[i].begin(); itr != m_Raids[i].end();)
        {
            delete *itr;
            itr = m_Raids[i].begin();
        }
}

bool OutdoorPvPWG::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_WINTERGRASP);

    for (uint8 i = 0; i < WG_WORKSHOP_COUNT; ++i)
    {
        WGWorkShop* ws = new WGWorkShop(i, this);
        m_workshops[i] = ws;
        switch(i)
        {
            case WG_WORKSHOP_BROKEN_TEMPLE:
                ws->owner = m_defender;
                ws->worldState = WG_WS_WORKSHOP_BROKEN_TEMPLE;
                ws->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_ALLIANCE_INTACT : WG_OBJECTSTATE_HORDE_INTACT;
                ws->gy = WG_GRAVEYARD_ENTRY_BROKEN_TEMPLE;
                sObjectMgr.AddGraveYardLink(ws->gy, ZONE_ID_WINTERGRASP, GetTeamFromIndex(ws->owner), false);
                break;
            case WG_WORKSHOP_SUNKEN_RING:
                ws->owner = m_defender;
                ws->worldState = WG_WS_WORKSHOP_SUNKEN_RING;
                ws->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_ALLIANCE_INTACT : WG_OBJECTSTATE_HORDE_INTACT;
                ws->gy = WG_GRAVEYARD_ENTRY_SUNKEN_RING;
                sObjectMgr.AddGraveYardLink(ws->gy, ZONE_ID_WINTERGRASP, GetTeamFromIndex(ws->owner), false);
                break;
            case WG_WORKSHOP_KEEP_WEST:
                ws->owner = m_defender;
                ws->worldState = WG_WS_WORKSHOP_KEEP_WEST;
                ws->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_ALLIANCE_INTACT : WG_OBJECTSTATE_HORDE_INTACT;
                ws->gy = WG_GRAVEYARD_ENTRY_KEEP_1;
                sObjectMgr.AddGraveYardLink(ws->gy, ZONE_ID_WINTERGRASP, GetTeamFromIndex(ws->owner), false);
                break;
            case WG_WORKSHOP_KEEP_EAST:
                ws->owner = m_defender;
                ws->worldState = WG_WS_WORKSHOP_KEEP_EAST;
                ws->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_ALLIANCE_INTACT : WG_OBJECTSTATE_HORDE_INTACT;
                ws->gy = WG_GRAVEYARD_ENTRY_KEEP_2;
                sObjectMgr.AddGraveYardLink(ws->gy, ZONE_ID_WINTERGRASP, GetTeamFromIndex(ws->owner), false);
                break;
            case WG_WORKSHOP_WESTPARK:
                ws->owner = GetAttacker();
                ws->worldState = WG_WS_WORKSHOP_WESTPARK;
                ws->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_HORDE_INTACT : WG_OBJECTSTATE_ALLIANCE_INTACT;
                ws->gy = WG_GRAVEYARD_ENTRY_WESTPARK;
                sObjectMgr.AddGraveYardLink(ws->gy, ZONE_ID_WINTERGRASP, GetTeamFromIndex(ws->owner), false);
                break;
            case WG_WORKSHOP_EASTPARK:
                ws->owner = GetAttacker();
                ws->worldState = WG_WS_WORKSHOP_EASTPARK;
                ws->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_HORDE_INTACT : WG_OBJECTSTATE_ALLIANCE_INTACT;
                ws->gy = WG_GRAVEYARD_ENTRY_EASTPARK;
                sObjectMgr.AddGraveYardLink(ws->gy, ZONE_ID_WINTERGRASP, GetTeamFromIndex(ws->owner), false);
                break;
        }
    }

    for (uint8 i = 0; i < WG_KEEPTOWER_COUNT; ++i)
    {
        WGTower* wkt = new WGTower(i, this);
        m_keepTowers[i] = wkt;
        wkt->owner = m_defender;
        wkt->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_ALLIANCE_INTACT : WG_OBJECTSTATE_HORDE_INTACT;

        switch(i)
        {
            case WG_KEEPTOWER_WEST:
                wkt->worldState = WG_WS_KEEPTOWER_WEST;
                break;
            case WG_KEEPTOWER_SOUTHWEST:
                wkt->worldState = WG_WS_KEEPTOWER_SOUTHWEST;
                break;
            case WG_KEEPTOWER_SOUTHEAST:
                wkt->worldState = WG_WS_KEEPTOWER_SOUTHEAST;
                break;
            case WG_KEEPTOWER_EAST:
                wkt->worldState = WG_WS_KEEPTOWER_EAST;
                break;
        }
    }

    for (uint8 i = 0; i < WG_TOWER_COUNT; ++i)
    {
        WGTower* wt = new WGTower(i, this);
        m_towers[i] = wt;
        wt->owner = GetAttacker();
        wt->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_HORDE_INTACT : WG_OBJECTSTATE_ALLIANCE_INTACT;

        switch(i)
        {
            case WG_TOWER_SHADOWSIGHT:
                wt->worldState = WG_WS_TOWER_SHADOWSIGHT;
                break;
            case WG_TOWER_WINTERS_EDGE:
                wt->worldState = WG_WS_TOWER_WINTERS_EDGE;
                break;
            case WG_TOWER_FLAMEWATCH:
                wt->worldState = WG_WS_TOWER_FLAMEWATCH;
                break;
        }
    }

    float minX = 4003.81f;
    float minY = 1452.01f;
    float maxX = 5734.16f;
    float maxY = 4307.05f;
    float xStep = (maxX - minX) / 10.0f;
    float yStep = (maxY - minY) / 10.0f;

    //CellPair cell_val = MaNGOS::ComputeCellPair(5734.16f, 4307.05f);
    //cell_val = MaNGOS::ComputeCellPair(4003.81f, 1452.01f);
    DEBUG_LOG("Loading cells for Wintergrasp");
    Map* map = sMapMgr.CreateMap(MAP_ID_NORTHREND, NULL);
    for (float x = minX; x <= maxX; x += xStep)
        for (float y = minY; y <= maxY; y += yStep)
            map->LoadGrid(Cell(MaNGOS::ComputeCellPair(x, y)), true);
    DEBUG_LOG("Finished loading cells for Wintergrasp");

    return true;
}

void OutdoorPvPWG::FillInitialWorldStates(WorldPacket& data, uint32& count, Player* player)
{
    FillInitialWorldState(data, count, WGClockWorldState[1], uint32(time(NULL) + m_timer / 1000));
    FillInitialWorldState(data, count, WG_WS_SHOW_COOLDOWN_WORLDSTATE, m_state == WG_STATE_IN_PROGRESS ? WORLD_STATE_REMOVE : WORLD_STATE_ADD);

    FillInitialWorldState(data, count, WG_WS_SHOW_BATTLE_WORLDSTATE, m_state == WG_STATE_IN_PROGRESS && player->GetZoneId() == ZONE_ID_WINTERGRASP && IsMember(player->GetObjectGuid()) ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WG_WS_ALLIANCE_DEFENDER, m_defender == TEAM_INDEX_ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, WG_WS_HORDE_DEFENDER, m_defender == TEAM_INDEX_HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    FillInitialWorldState(data, count, WGClockWorldState[0], uint32(time(NULL) + m_timer / 1000));

    FillInitialWorldState(data, count, WG_WS_VEHICLE_A, uint32(m_vehicleGUIDs[0].size()));
    FillInitialWorldState(data, count, WG_WS_MAX_VEHICLE_A, GetWorkshopsOwnedBy(TEAM_INDEX_ALLIANCE) * 4);

    FillInitialWorldState(data, count, WG_WS_VEHICLE_H, uint32(m_vehicleGUIDs[1].size()));
    FillInitialWorldState(data, count, WG_WS_MAX_VEHICLE_H, GetWorkshopsOwnedBy(TEAM_INDEX_HORDE) * 4);

    for (std::map<uint8, WGTower*>::const_iterator itr = m_keepTowers.begin(); itr != m_keepTowers.end(); ++itr)
        FillInitialWorldState(data, count, itr->second->worldState, player->GetMapId() == MAP_ID_NORTHREND ? itr->second->state : WG_OBJECTSTATE_NONE);

    for (std::list<WGObject*>::const_iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
        FillInitialWorldState(data, count, (*itr)->worldState, player->GetMapId() == MAP_ID_NORTHREND ? (*itr)->state : WG_OBJECTSTATE_NONE);

    for (std::map<uint8, WGTower*>::const_iterator itr = m_towers.begin(); itr != m_towers.end(); ++itr)
        FillInitialWorldState(data, count, itr->second->worldState, player->GetMapId() == MAP_ID_NORTHREND ? itr->second->state : WG_OBJECTSTATE_NONE);

    for (std::map<uint8, WGWorkShop*>::const_iterator itr = m_workshops.begin(); itr != m_workshops.end(); ++itr)
        FillInitialWorldState(data, count, itr->second->worldState, player->GetMapId() == MAP_ID_NORTHREND ? itr->second->state : WG_OBJECTSTATE_NONE);
}

void OutdoorPvPWG::SendUpdateWorldStatesToAll()
{
    Map::PlayerList const& pList = GetMap()->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
    {
        Player* player = itr->getSource();
        if (!player || !player->IsInWorld())
            continue;

        // timer sent before function
        SendUpdateWorldStatesTo(player);
    }
}

void OutdoorPvPWG::SendUpdateWorldStatesTo(Player* player)
{
    player->SendUpdateWorldState(WGClockWorldState[1], time(NULL) + m_timer / 1000);
    player->SendUpdateWorldState(WG_WS_SHOW_COOLDOWN_WORLDSTATE, m_state == WG_STATE_IN_PROGRESS ? WORLD_STATE_REMOVE : WORLD_STATE_ADD);

    player->SendUpdateWorldState(WG_WS_SHOW_BATTLE_WORLDSTATE, m_state == WG_STATE_IN_PROGRESS && player->GetZoneId() == ZONE_ID_WINTERGRASP && IsMember(player->GetObjectGuid()) ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(WG_WS_ALLIANCE_DEFENDER, m_defender == TEAM_INDEX_ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(WG_WS_HORDE_DEFENDER, m_defender == TEAM_INDEX_HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    player->SendUpdateWorldState(WGClockWorldState[0], uint32(time(NULL) + m_timer / 1000));

    player->SendUpdateWorldState(WG_WS_VEHICLE_A, uint32(m_vehicleGUIDs[0].size()));
    player->SendUpdateWorldState(WG_WS_MAX_VEHICLE_A, GetWorkshopsOwnedBy(TEAM_INDEX_ALLIANCE) * 4);

    player->SendUpdateWorldState(WG_WS_VEHICLE_H, uint32(m_vehicleGUIDs[1].size()));
    player->SendUpdateWorldState(WG_WS_MAX_VEHICLE_H, GetWorkshopsOwnedBy(TEAM_INDEX_HORDE) * 4);

    for (std::map<uint8, WGTower*>::const_iterator itr = m_keepTowers.begin(); itr != m_keepTowers.end(); ++itr)
        player->SendUpdateWorldState(itr->second->worldState, itr->second->state);

    for (std::list<WGObject*>::const_iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
        player->SendUpdateWorldState((*itr)->worldState, (*itr)->state);

    for (std::map<uint8, WGTower*>::const_iterator itr = m_towers.begin(); itr != m_towers.end(); ++itr)
        player->SendUpdateWorldState(itr->second->worldState, itr->second->state);

    for (std::map<uint8, WGWorkShop*>::const_iterator itr = m_workshops.begin(); itr != m_workshops.end(); ++itr)
        player->SendUpdateWorldState(itr->second->worldState, itr->second->state);   
}

void OutdoorPvPWG::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(WG_WS_SHOW_BATTLE_WORLDSTATE, WORLD_STATE_REMOVE);
}

void OutdoorPvPWG::HandlePlayerEnterZone(Player* pPlayer)
{
    OutdoorPvP::HandlePlayerEnterZone(pPlayer);

    pPlayer->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_ZONE);
    if (m_state == WG_STATE_COOLDOWN && m_defender == GetTeamIndex(pPlayer->GetTeam()))
        pPlayer->CastSpell(pPlayer, SPELL_ESSENCE_OF_WINTERGRASP_ZONE, true);

    pPlayer->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE_SHIFT);
    pPlayer->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE_SHIFT);

    if (m_state == WG_STATE_IN_PROGRESS && pPlayer->IsFreeFlying() && !pPlayer->isGameMaster() && !pPlayer->HasAura(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA))
        pPlayer->CastSpell(pPlayer, SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA, true);

    if (!IsMember(pPlayer->GetObjectGuid()))
    {
        if (m_state == WG_STATE_IN_PROGRESS)
        {
            m_InvitedPlayers[GetTeamIndex(pPlayer->GetTeam())][pPlayer->GetObjectGuid()] = time(NULL) + WG_TIME_TO_ACCEPT;
            pPlayer->GetSession()->SendBfInvitePlayerToWar(m_BattlefieldId, m_BattlefieldZoneId, WG_TIME_TO_ACCEPT);
        }
        else if (m_state == WG_STATE_COOLDOWN && m_timer < WG_START_INVITE_TIME)
            InvitePlayerToQueue(pPlayer);
    }

    WGPlayerScoreMap::iterator itr = m_playerScores.find(pPlayer->GetObjectGuid());
    if (itr == m_playerScores.end())
        m_playerScores[pPlayer->GetObjectGuid()] = new WGPlayerScore();
    else
    {
        if (m_state == WG_STATE_IN_PROGRESS && itr->second->removeTime && m_startTime > itr->second->removeTime)
            SetupPlayerPosition(pPlayer);

        itr->second->removeTime = 0;
    }

    UpdateTowerControllBuff(pPlayer);
    UpdateScoreBuff(pPlayer);

    pPlayer->CastSpell(pPlayer, m_defender == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE_SHIFT : SPELL_HORDE_CONTROL_PHASE_SHIFT, true);
}

void OutdoorPvPWG::HandlePlayerLeaveZone(Player* pPlayer)
{
    OutdoorPvP::HandlePlayerLeaveZone(pPlayer);

    pPlayer->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE_SHIFT);
    pPlayer->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE_SHIFT);
    pPlayer->RemoveAurasDueToSpell(SPELL_ESSENCE_OF_WINTERGRASP_ZONE);

    pPlayer->RemoveAurasDueToSpell(SPELL_RECRUIT);
    pPlayer->RemoveAurasDueToSpell(SPELL_CORPORAL);
    pPlayer->RemoveAurasDueToSpell(SPELL_LIEUTENANT);

    pPlayer->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);

    pPlayer->RemoveAurasDueToSpell(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA);

    WGPlayerScoreMap::iterator itr = m_playerScores.find(pPlayer->GetObjectGuid());
    if (itr != m_playerScores.end())
    {
        if (IsMember(itr->first))
        {
            pPlayer->GetSession()->SendBfLeaveMessage(BATTLEFIELD_WG, BATTLEFIELD_LEAVE_REASON_EXITED);
            itr->second->removeTime = time(NULL);
            itr->second->removeDelay = WG_UNACCEPTED_REMOVE_DELAY;
        }
        else
            m_playerScores.erase(itr);
    }
}

void OutdoorPvPWG::HandlePlayerEnterArea(Player* pPlayer, uint32 uiAreaId)
{
    pPlayer->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT);
    pPlayer->RemoveAurasDueToSpell(SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT);

    switch(uiAreaId)
    {
        case AREA_ID_WINTERGRASP_FORTRESS:
            pPlayer->CastSpell(pPlayer, m_defender == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT : SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT, true);
            return;
        case AREA_ID_THE_SUNKEN_RING:
            pPlayer->CastSpell(pPlayer, m_workshops[WG_WORKSHOP_SUNKEN_RING]->owner == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT : SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT, true);
            return;
        case AREA_ID_THE_BROKEN_TEMPLE:
        case AREA_ID_THE_CHILLED_QUAGMIRE:
            pPlayer->CastSpell(pPlayer, m_workshops[WG_WORKSHOP_BROKEN_TEMPLE]->owner == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT : SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT, true);
            return;
        case AREA_ID_WESTPARK_WORKSHOP:
            pPlayer->CastSpell(pPlayer, m_workshops[WG_WORKSHOP_WESTPARK]->owner == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT : SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT, true);
            return;
        case AREA_ID_EASTPARK_WORKSHOP:
            pPlayer->CastSpell(pPlayer, m_workshops[WG_WORKSHOP_EASTPARK]->owner == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT : SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT, true);
            return;
    }
}

void OutdoorPvPWG::HandlePlayerLeaveArea(Player* pPlayer, uint32 uiAreaId)
{
    pPlayer->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT);
    pPlayer->RemoveAurasDueToSpell(SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT);
}

void OutdoorPvPWG::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_COMMANDER_DARDOSH:
            m_dardoshGuid = pCreature->GetObjectGuid();
            return;
        case NPC_COMMANDER_ZANNETH:
            m_zannethGuid = pCreature->GetObjectGuid();
            return;
        case NPC_CHILLED_EARH_ELEMENTAL:
        case NPC_RAGING_FLAME:
        case NPC_GLACIAL_SPIRIT:
        case NPC_WANDERING_SHADOW:
        case NPC_WHISPERING_WIND:
        case NPC_LIVING_LASHER:
        {
            m_zoneTrashGUIDs.push_back(pCreature->GetObjectGuid());
            if (m_state != WG_STATE_IN_PROGRESS)
                return;
            break;
        }
        case NPC_ALLIANCE_SPIRIT_GUIDE:
        case NPC_HORDE_SPIRIT_GUIDE:
        {
            // relic chamber ones
            if (pCreature->GetPositionZ() > 515.0f || pCreature->GetPositionX() < 5394.0f)
                return;
            m_zoneTrashGUIDs.push_back(pCreature->GetObjectGuid());
            if (m_state != WG_STATE_IN_PROGRESS)
                return;
            break;
        }
        case NPC_WINTERGRASP_TOWER_CANNON:
        {
            switch(GetLocationFor(pCreature->GetPositionX(), pCreature->GetPositionY()))
            {
                case WG_LOCATION_SHADOWSIGHT_TOWER:
                    m_towers[WG_TOWER_SHADOWSIGHT]->cannons.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[GetAttacker()]);
                    if (!m_towers[WG_TOWER_SHADOWSIGHT]->IsDestroyed())
                        return;
                    break;
                case WG_LOCATION_WINTERS_EDGE_TOWER:
                    m_towers[WG_TOWER_WINTERS_EDGE]->cannons.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[GetAttacker()]);
                    if (!m_towers[WG_TOWER_WINTERS_EDGE]->IsDestroyed())
                        return;
                    break;
                case WG_LOCATION_FLAMEWATCH_TOWER:
                    m_towers[WG_TOWER_FLAMEWATCH]->cannons.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[GetAttacker()]);
                    if (!m_towers[WG_TOWER_FLAMEWATCH]->IsDestroyed())
                        return;
                    break;
                case WG_LOCATION_KEEP_TOWER_WEST:
                    m_keepTowers[WG_KEEPTOWER_WEST]->cannons.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[m_defender]);
                    if (!m_keepTowers[WG_KEEPTOWER_WEST]->IsDestroyed())
                        return;
                    break;
                case WG_LOCATION_KEEP_TOWER_SOUTHWEST:
                    m_keepTowers[WG_KEEPTOWER_SOUTHWEST]->cannons.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[m_defender]);
                    if (!m_keepTowers[WG_KEEPTOWER_SOUTHWEST]->IsDestroyed())
                        return;
                    break;
                case WG_LOCATION_KEEP_TOWER_SOUTHEAST:
                    m_keepTowers[WG_KEEPTOWER_SOUTHEAST]->cannons.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[m_defender]);
                    if (!m_keepTowers[WG_KEEPTOWER_SOUTHEAST]->IsDestroyed())
                        return;
                    break;
                case WG_LOCATION_KEEP_TOWER_EAST:
                    m_keepTowers[WG_KEEPTOWER_EAST]->cannons.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[m_defender]);
                    if (!m_keepTowers[WG_KEEPTOWER_EAST]->IsDestroyed())
                        return;
                    break;
                default:
                    m_keepCannonGUIDs.push_back(pCreature->GetObjectGuid());
                    pCreature->setFaction(WGFactions[m_defender]);
                    return;
            }
            break;
        }
        default:
            return;
    }

    // Despawn creatures on create - will be spawned later in script
    SpawnCreature(pCreature, 0, false);
}

void OutdoorPvPWG::_OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_WINTERGRASP_CATAPULT:
        case NPC_WINTERGRASP_DEMOLISHER:
        case NPC_WINTERGRASP_SIEGE_ENGINE_A:
        case NPC_WINTERGRASP_SIEGE_ENGINE_H:
        {
            if (Unit* creator = pCreature->GetCreator())
            {
                if (creator->GetTypeId() == TYPEID_PLAYER)
                {
                    TeamIndex teamIdx = GetTeamIndex(((Player*)creator)->GetTeam());
                    if (CanBuildMoreVehicles(teamIdx))
                    {
                        m_vehicleGUIDs[teamIdx].push_back(pCreature->GetObjectGuid());
                        SendUpdateWorldState(teamIdx == TEAM_INDEX_ALLIANCE ? WG_WS_VEHICLE_A : WG_WS_VEHICLE_H, uint32(m_vehicleGUIDs[teamIdx].size()));
                        return;
                    }
                }
            }
            pCreature->ForcedDespawn();
            return;
        }
        case NPC_WINTERGRASP_SIEGE_TURRET_A:
        case NPC_WINTERGRASP_SIEGE_TURRET_H:
        {
            pCreature->setFaction(WGFactions[pCreature->GetEntry() == NPC_WINTERGRASP_SIEGE_TURRET_A ? TEAM_INDEX_ALLIANCE : TEAM_INDEX_HORDE]);
            return;
        }
    }
}

void OutdoorPvPWG::OnCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_WINTERGRASP_CATAPULT:
        case NPC_WINTERGRASP_DEMOLISHER:
        case NPC_WINTERGRASP_SIEGE_ENGINE_A:
        case NPC_WINTERGRASP_SIEGE_ENGINE_H:
        //case NPC_WINTERGRASP_SIEGE_TURRET_A:
        //case NPC_WINTERGRASP_SIEGE_TURRET_H:
        {
            for (uint8 i = 0; i < 2; ++i)
            {
                std::list<ObjectGuid>::iterator itr;
                for (itr = m_vehicleGUIDs[i].begin(); itr != m_vehicleGUIDs[i].end(); ++itr)
                    if (*itr == pCreature->GetObjectGuid())
                        break;

                if (itr != m_vehicleGUIDs[i].end())
                {
                    m_vehicleGUIDs[i].erase(itr);
                    SendUpdateWorldState(i == TEAM_INDEX_ALLIANCE ? WG_WS_VEHICLE_A : WG_WS_VEHICLE_H, uint32(m_vehicleGUIDs[i].size()));
                    break;
                }
            }
            return;
        }
        default:
            return;
    }
}

void OutdoorPvPWG::OnGameObjectCreate(GameObject* pGo)
{
    switch(pGo->GetEntry())
    {
        case GO_WINTERGRASP_FORTRESS_DOOR:
        case GO_WINTERGRASP_FORTRESS_GATE:
        case GO_WINTERGRASP_FORTRESS_WALL_1:
        case GO_WINTERGRASP_FORTRESS_WALL_2:
        case GO_WINTERGRASP_FORTRESS_WALL_3:
        case GO_WINTERGRASP_FORTRESS_WALL_4:
        case GO_WINTERGRASP_FORTRESS_WALL_5:
        case GO_WINTERGRASP_FORTRESS_WALL_6:
        case GO_WINTERGRASP_FORTRESS_WALL_7:
        case GO_WINTERGRASP_FORTRESS_WALL_8:
        case GO_WINTERGRASP_FORTRESS_WALL_9:
        case GO_WINTERGRASP_FORTRESS_WALL_10:
        case GO_WINTERGRASP_FORTRESS_WALL_11:
        case GO_WINTERGRASP_FORTRESS_WALL_12:
        case GO_WINTERGRASP_FORTRESS_WALL_13:
        case GO_WINTERGRASP_FORTRESS_WALL_14:
        case GO_WINTERGRASP_FORTRESS_WALL_15:
        case GO_WINTERGRASP_FORTRESS_WALL_16:
        case GO_WINTERGRASP_FORTRESS_WALL_17:
        case GO_WINTERGRASP_FORTRESS_WALL_18:
        case GO_WINTERGRASP_FORTRESS_WALL_19:
        case GO_WINTERGRASP_FORTRESS_WALL_20:
        case GO_WINTERGRASP_WALL_1:
        case GO_WINTERGRASP_WALL_2:
        case GO_WINTERGRASP_WALL_3:
        {
            WGObject* wall = new WGObject(pGo->GetEntry(), this);
            wall->owner = m_defender;
            wall->guid = pGo->GetObjectGuid();
            wall->worldState = GetWorldStateFor(pGo->GetEntry());
            wall->state = m_defender == TEAM_INDEX_ALLIANCE ? WG_OBJECTSTATE_ALLIANCE_INTACT : WG_OBJECTSTATE_HORDE_INTACT;

            m_keepWalls.push_back(wall);

            if (pGo->GetEntry() == GO_WINTERGRASP_FORTRESS_DOOR)
                m_keepDoorGUID = pGo->GetObjectGuid();
            else if (pGo->GetEntry() == GO_WINTERGRASP_FORTRESS_GATE)
                m_keepGateGUID = pGo->GetObjectGuid();

            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);

            pGo->SetActiveObjectState(true);

            break;
        }
        case GO_WINTERGRASP_TOWER_WEST:
        {
            m_keepTowers[WG_KEEPTOWER_WEST]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WINTERGRASP_TOWER_SOUTHWEST:
        {
            m_keepTowers[WG_KEEPTOWER_SOUTHWEST]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WINTERGRASP_TOWER_SOUTHEAST:
        {
            m_keepTowers[WG_KEEPTOWER_SOUTHEAST]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WINTERGRASP_TOWER_EAST:
        {
            m_keepTowers[WG_KEEPTOWER_EAST]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WORKSHOP_KEEP_WEST:
        {
            m_workshops[WG_WORKSHOP_KEEP_WEST]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WORKSHOP_KEEP_EAST:
        {
            m_workshops[WG_WORKSHOP_KEEP_EAST]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WORKSHOP_BROKEN_TEMPLE:
        {
            m_workshops[WG_WORKSHOP_BROKEN_TEMPLE]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_workshops[WG_WORKSHOP_BROKEN_TEMPLE]->owner]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WORKSHOP_SUNKEN_RING:
        {
            m_workshops[WG_WORKSHOP_SUNKEN_RING]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_workshops[WG_WORKSHOP_SUNKEN_RING]->owner]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WORKSHOP_WESTPARK:
        {
            m_workshops[WG_WORKSHOP_WESTPARK]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_workshops[WG_WORKSHOP_WESTPARK]->owner]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_WORKSHOP_EASTPARK:
        {
            m_workshops[WG_WORKSHOP_EASTPARK]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_workshops[WG_WORKSHOP_EASTPARK]->owner]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_TOWER_SHADOWSIGHT:
        {
            m_towers[WG_TOWER_SHADOWSIGHT]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[GetAttacker()]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_TOWER_WINTERS_EDGE:
        {
            m_towers[WG_TOWER_WINTERS_EDGE]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[GetAttacker()]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_TOWER_FLAMEWATCH:
        {
            m_towers[WG_TOWER_FLAMEWATCH]->guid = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[GetAttacker()]);
            pGo->SetActiveObjectState(true);
            break;
        }
        case GO_CAPTUREPOINT_BROKEN_TEMPLE_A:
        {
            if (m_state == WG_STATE_COOLDOWN)
            {
                pGo->SetLootState(GO_JUST_DEACTIVATED);
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_BROKEN_TEMPLE]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
            }
            else
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_BROKEN_TEMPLE]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

            m_workshops[WG_WORKSHOP_BROKEN_TEMPLE]->capturePoint = pGo->GetObjectGuid();
            break;
        }
        case GO_CAPTUREPOINT_WESTPARK_A:
        {
            if (m_state == WG_STATE_COOLDOWN)
            {
                pGo->SetLootState(GO_JUST_DEACTIVATED);
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_WESTPARK]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
            }
            else
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_WESTPARK]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

            m_workshops[WG_WORKSHOP_WESTPARK]->capturePoint = pGo->GetObjectGuid();
            break;
        }
        case GO_CAPTUREPOINT_EASTPARK_A:
        {
            if (m_state == WG_STATE_COOLDOWN)
            {
                pGo->SetLootState(GO_JUST_DEACTIVATED);
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_EASTPARK]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
            }
            else
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_EASTPARK]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

            m_workshops[WG_WORKSHOP_EASTPARK]->capturePoint = pGo->GetObjectGuid();
            break;
        }
        case GO_CAPTUREPOINT_SUNKEN_RING_A:
        {
            if (m_state == WG_STATE_COOLDOWN)
            {
                pGo->SetLootState(GO_JUST_DEACTIVATED);
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_SUNKEN_RING]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
            }
            else
                pGo->SetCapturePointSlider(m_workshops[WG_WORKSHOP_SUNKEN_RING]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

            m_workshops[WG_WORKSHOP_SUNKEN_RING]->capturePoint = pGo->GetObjectGuid();
            break;
        }
        case GO_TITAN_RELIC:
        {
            m_titanRelicGUID = pGo->GetObjectGuid();
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[GetAttacker()]);
            break;
        }
        case GO_DEFENDERS_PORTAL_1:
        case GO_DEFENDERS_PORTAL_2:
        case GO_DEFENDERS_PORTAL_3:
        {
            m_portalGUIDs.push_back(pGo->GetObjectGuid());
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);
            break;
        }
    }
}

// Cast player spell on opponent kill
void OutdoorPvPWG::HandlePlayerKillInsideArea(Player* pPlayer, Unit* pVictim)
{
    if (!pVictim || m_state != WG_STATE_IN_PROGRESS || !IsMember(pPlayer->GetObjectGuid()))
        return;

    WGPlayerScoreMap::iterator itr = m_playerScores.find(pPlayer->GetObjectGuid());
    if (itr == m_playerScores.end())
        return;

    WGRank oldRank = itr->second->GetRank();
    if (pVictim->GetTypeId() == TYPEID_PLAYER)
    {
        if (itr->second->GetRank() != WG_RANK_LIEUTENANT)
        {
            ++itr->second->rank;
            UpdateScoreBuff(pPlayer);
        }

        ((Player*)pVictim)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
        pPlayer->KilledMonsterCredit(pPlayer->GetTeam() == ALLIANCE ? WG_QUEST_CREDIT_HORDE_SLAIN : WG_QUEST_CREDIT_ALLIANCE_SLAIN);
    }
    else
    {
        switch(pVictim->GetEntry())
        {
            case NPC_VALLIANCE_EXPEDITION_CHAMPION:
            case NPC_VALLIANCE_EXPEDITION_GUARD:
                if (itr->second->GetRank() != WG_RANK_LIEUTENANT)
                {
                    ++itr->second->rank;
                    UpdateScoreBuff(pPlayer);
                }
                pPlayer->KilledMonsterCredit(WG_QUEST_CREDIT_ALLIANCE_SLAIN);
                break;
            case NPC_WARSONG_CHAMPION:
            case NPC_WARSONG_GUARD:
                if (itr->second->GetRank() != WG_RANK_LIEUTENANT)
                {
                    ++itr->second->rank;
                    UpdateScoreBuff(pPlayer);
                }
                pPlayer->KilledMonsterCredit(WG_QUEST_CREDIT_HORDE_SLAIN);
                break;
            case NPC_WINTERGRASP_CATAPULT:
            case NPC_WINTERGRASP_DEMOLISHER:
            case NPC_WINTERGRASP_SIEGE_ENGINE_A:
            case NPC_WINTERGRASP_SIEGE_ENGINE_H:
            case NPC_WINTERGRASP_TOWER_CANNON:
                if (itr->second->GetRank() != WG_RANK_LIEUTENANT)
                {
                    ++itr->second->rank;
                    UpdateScoreBuff(pPlayer);
                }
                 if (pVictim->GetEntry() != NPC_WINTERGRASP_TOWER_CANNON && GetTeamIndex(pPlayer->GetTeam()) == m_defender)
                    pPlayer->KilledMonsterCredit(WG_QUEST_CREDIT_DESTROYED_VEHICLES);
                break;
        }
    }

    WGRank newRank = itr->second->GetRank();

    if (oldRank != newRank)
    {
        int textId = 0;
        switch(newRank)
        {
            case WG_RANK_CORPORAL:
                textId = LANG_WG_REACHED_RANK_1;
                break;
            case WG_RANK_LIEUTENANT:
                textId = LANG_WG_REACHED_RANK_2;
                break;
        }
        if (textId)
            pPlayer->MonsterWhisper(textId, pPlayer, true);
    }
}

// process the capture events
void OutdoorPvPWG::ProcessEvent(uint32 uiEventId, GameObject* pGo, Player* pInvoker, uint32 spellId)
{
    if (!uiEventId)
        return;

    if (pInvoker && !IsMember(pInvoker->GetObjectGuid()))
        return;

    GameObjectInfo const * info = pGo->GetGOInfo();

    int32 textId = 0;
    switch(pGo->GetEntry())
    {
        case GO_WINTERGRASP_FORTRESS_DOOR:
        case GO_WINTERGRASP_FORTRESS_GATE:
        case GO_WINTERGRASP_FORTRESS_WALL_1:
        case GO_WINTERGRASP_FORTRESS_WALL_2:
        case GO_WINTERGRASP_FORTRESS_WALL_3:
        case GO_WINTERGRASP_FORTRESS_WALL_4:
        case GO_WINTERGRASP_FORTRESS_WALL_5:
        case GO_WINTERGRASP_FORTRESS_WALL_6:
        case GO_WINTERGRASP_FORTRESS_WALL_7:
        case GO_WINTERGRASP_FORTRESS_WALL_8:
        case GO_WINTERGRASP_FORTRESS_WALL_9:
        case GO_WINTERGRASP_FORTRESS_WALL_10:
        case GO_WINTERGRASP_FORTRESS_WALL_11:
        case GO_WINTERGRASP_FORTRESS_WALL_12:
        case GO_WINTERGRASP_FORTRESS_WALL_13:
        case GO_WINTERGRASP_FORTRESS_WALL_14:
        case GO_WINTERGRASP_FORTRESS_WALL_15:
        case GO_WINTERGRASP_FORTRESS_WALL_16:
        case GO_WINTERGRASP_FORTRESS_WALL_17:
        case GO_WINTERGRASP_FORTRESS_WALL_18:
        case GO_WINTERGRASP_FORTRESS_WALL_19:
        case GO_WINTERGRASP_FORTRESS_WALL_20:
        case GO_WINTERGRASP_WALL_1:
        case GO_WINTERGRASP_WALL_2:
        case GO_WINTERGRASP_WALL_3:
        {
            if (uiEventId == info->destructibleBuilding.damagedEvent)
            {
                for (std::list<WGObject*>::iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
                {
                    if ((*itr)->guid == pGo->GetObjectGuid() && !(*itr)->IsDamaged())
                    {
                        (*itr)->SetDamaged();
                        (*itr)->SendUpdateWorldState();
                        break;
                    }
                }
            }
            else if (uiEventId == info->destructibleBuilding.destroyedEvent)
            {
                for (std::list<WGObject*>::iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
                {
                    if ((*itr)->guid == pGo->GetObjectGuid() && !(*itr)->IsDestroyed())
                    {
                        (*itr)->SetDestroyed();
                        (*itr)->SendUpdateWorldState();
                        QuestCreditTeam(WG_QUEST_CREDIT_DEFEND_VEHICLE, pInvoker->GetTeam(), pInvoker, 40.0f);
                        break;
                    }
                }
            }
            break;
        }
        case GO_WINTERGRASP_TOWER_WEST:
        case GO_WINTERGRASP_TOWER_SOUTHWEST:
        case GO_WINTERGRASP_TOWER_SOUTHEAST:
        case GO_WINTERGRASP_TOWER_EAST:
        {
            uint8 idx = 0;
            switch(pGo->GetEntry())
            {
                case GO_WINTERGRASP_TOWER_WEST:
                    idx = WG_KEEPTOWER_WEST;
                    break;
                case GO_WINTERGRASP_TOWER_SOUTHWEST:
                    idx = WG_KEEPTOWER_SOUTHWEST;
                    break;
                case GO_WINTERGRASP_TOWER_SOUTHEAST:
                    idx = WG_KEEPTOWER_SOUTHEAST;
                    break;
                case GO_WINTERGRASP_TOWER_EAST:
                    idx = WG_KEEPTOWER_EAST;
                    break;
            }

            if (uiEventId == info->destructibleBuilding.damagedEvent && !m_keepTowers[idx]->IsDamaged())
            {
                m_keepTowers[idx]->SetDamaged();
                m_keepTowers[idx]->SendUpdateWorldState();
                ++m_damagedTowers[GetAttacker()];
                //BuffTeam(GetTeamFromIndex(m_defender), SPELL_DAMAGED_TOWER
                switch(idx)
                {
                    case WG_KEEPTOWER_WEST:
                        textId = LANG_WG_NW_TOWER_DAMAGED;
                        break;
                    case WG_KEEPTOWER_SOUTHWEST:
                        textId = LANG_WG_SW_TOWER_DAMAGED;
                        break;
                    case WG_KEEPTOWER_SOUTHEAST:
                        textId = LANG_WG_SE_TOWER_DAMAGED;
                        break;
                    case WG_KEEPTOWER_EAST:
                        textId = LANG_WG_NE_TOWER_DAMAGED;
                        break;
                }
            }
            else if (uiEventId == info->destructibleBuilding.destroyedEvent && !m_keepTowers[idx]->IsDestroyed())
            {
                m_keepTowers[idx]->SetDestroyed();
                m_keepTowers[idx]->SpawnCannons(true);
                m_keepTowers[idx]->SendUpdateWorldState();
                ++m_destroyedTowers[GetAttacker()];
                --m_damagedTowers[GetAttacker()];
                //BuffTeam(GetTeamFromIndex(m_defender), SPELL_DAMAGED_TOWER
                switch(idx)
                {
                    case WG_KEEPTOWER_WEST:
                        textId = LANG_WG_NW_TOWER_DESTROYED;
                        break;
                    case WG_KEEPTOWER_SOUTHWEST:
                        textId = LANG_WG_SW_TOWER_DESTROYED;
                        break;
                    case WG_KEEPTOWER_SOUTHEAST:
                        textId = LANG_WG_SE_TOWER_DESTROYED;
                        break;
                    case WG_KEEPTOWER_EAST:
                        textId = LANG_WG_NE_TOWER_DESTROYED;
                        break;
                }
                QuestCreditTeam(WG_QUEST_CREDIT_DEFEND_VEHICLE, pInvoker->GetTeam(), pInvoker, 40.0f);
            }
            break;
        }
        case GO_TOWER_SHADOWSIGHT:
        case GO_TOWER_WINTERS_EDGE:
        case GO_TOWER_FLAMEWATCH:
        {
            uint8 idx = 0;
            switch(pGo->GetEntry())
            {
                case GO_TOWER_SHADOWSIGHT:
                    idx = WG_TOWER_SHADOWSIGHT;
                    break;
                case GO_TOWER_WINTERS_EDGE:
                    idx = WG_TOWER_WINTERS_EDGE;
                    break;
                case GO_TOWER_FLAMEWATCH:
                    idx = WG_TOWER_FLAMEWATCH;
                    break;
            }

            if (uiEventId == info->destructibleBuilding.damagedEvent && !m_towers[idx]->IsDamaged())
            {
                m_towers[idx]->SetDamaged();
                m_towers[idx]->SendUpdateWorldState();
                ++m_damagedTowers[m_defender];
                BuffTeam(GetTeamFromIndex(m_defender), SPELL_DESTROYED_TOWER);
                switch(idx)
                {
                    case WG_TOWER_SHADOWSIGHT:
                        textId = LANG_WG_WESTERN_TOWER_DAMAGED;
                        break;
                    case WG_TOWER_WINTERS_EDGE:
                        textId = LANG_WG_SOUTHERN_TOWER_DAMAGED;
                        break;
                    case WG_TOWER_FLAMEWATCH:
                        textId = LANG_WG_EASTERN_TOWER_DAMAGED;
                        break;
                }
            }
            else if (uiEventId == info->destructibleBuilding.destroyedEvent && !m_towers[idx]->IsDestroyed())
            {
                m_towers[idx]->SetDestroyed();
                m_towers[idx]->SpawnCannons(true);
                UpdateTowerControllBuff();
                m_towers[idx]->SendUpdateWorldState();
                ++m_destroyedTowers[m_defender];
                --m_damagedTowers[m_defender];
                BuffTeam(GetTeamFromIndex(m_defender), SPELL_DAMAGED_TOWER);

                switch(idx)
                {
                    case WG_TOWER_SHADOWSIGHT:
                        textId = LANG_WG_WESTERN_TOWER_DESTROYED;
                        break;
                    case WG_TOWER_WINTERS_EDGE:
                        textId = LANG_WG_SOUTHERN_TOWER_DESTROYED;
                        break;
                    case WG_TOWER_FLAMEWATCH:
                        textId = LANG_WG_EASTERN_TOWER_DESTROYED;
                        break;
                }
                SendCaptainYell();

                for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
                {
                    if (!*itr || !(*itr)->IsInWorld() || !IsMember((*itr)->GetObjectGuid()))
                        continue;

                    if ((*itr)->GetTeam() != pInvoker->GetTeam() || pGo->GetDistance2d((*itr)->GetPositionX(), (*itr)->GetPositionY()) > 40.0f)
                        continue;

                    (*itr)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACH_DESTROYED_TOWER, 1);
                }

                QuestCreditTeam(WG_QUEST_CREDIT_SOUTHERN_TOWER, pInvoker->GetTeam(), pGo, 40.0f);

                if (m_destroyedTowers[m_defender] == 3)
                {
                    if (m_state == WG_STATE_IN_PROGRESS)
                        if (m_timer < WG_TOWERS_BONUS)
                            m_timer = 0;
                        else
                            m_timer -= WG_TOWERS_BONUS;

                    if (m_timer > 0)
                    {
                        SendUpdateWorldState(WGClockWorldState[0], uint32(time(NULL) + m_timer / 1000));
                        SendUpdateWorldState(WGClockWorldState[1], uint32(time(NULL) + m_timer / 1000));
                        sWorld.SendUpdateWintergraspTimerWorldState(this);
                    }
                    // else timer will be updated on battle end
                }
            }
            break;
        }

        case GO_CAPTUREPOINT_BROKEN_TEMPLE_A:
        case GO_CAPTUREPOINT_WESTPARK_A:
        case GO_CAPTUREPOINT_EASTPARK_A:
        case GO_CAPTUREPOINT_SUNKEN_RING_A:
        {
            uint8 idx = 0;
            switch(pGo->GetEntry())
            {
                case GO_CAPTUREPOINT_BROKEN_TEMPLE_A:
                    idx = WG_WORKSHOP_BROKEN_TEMPLE;
                    break;
                case GO_CAPTUREPOINT_WESTPARK_A:
                    idx = WG_WORKSHOP_WESTPARK;
                    break;
                case GO_CAPTUREPOINT_EASTPARK_A:
                    idx = WG_WORKSHOP_EASTPARK;
                    break;
                case GO_CAPTUREPOINT_SUNKEN_RING_A:
                    idx = WG_WORKSHOP_SUNKEN_RING;
                    break;
            }

            if (uiEventId == info->capturePoint.progressEventID1)
            {
                // give to alliance
                if (m_workshops[idx]->owner != TEAM_INDEX_ALLIANCE)
                {
                    m_workshops[idx]->InitFor(TEAM_INDEX_ALLIANCE, false);
                    m_workshops[idx]->SendUpdateWorldState();
                    switch(idx)
                    {
                        case WG_WORKSHOP_BROKEN_TEMPLE:
                            textId = LANG_WG_BROKEN_TEMPLE_CAPTURED_A;
                            break;
                        case WG_WORKSHOP_WESTPARK:
                            textId = LANG_WG_WESTPARK_CAPTURED_A;
                            break;
                        case WG_WORKSHOP_EASTPARK:
                            textId = LANG_WG_EASTPARK_CAPTURED_A;
                            break;
                        case WG_WORKSHOP_SUNKEN_RING:
                            textId = LANG_WG_SUNKEN_RING_CAPTURED_A;
                            break;
                    }
                }
                break;
            }
            else if (uiEventId == info->capturePoint.progressEventID2)
            {
                if (m_workshops[idx]->owner != TEAM_INDEX_HORDE)
                {
                    // give to horde
                    m_workshops[idx]->InitFor(TEAM_INDEX_HORDE, false);
                    m_workshops[idx]->SendUpdateWorldState();
                    switch(idx)
                    {
                        case WG_WORKSHOP_BROKEN_TEMPLE:
                            textId = LANG_WG_BROKEN_TEMPLE_CAPTURED_H;
                            break;
                        case WG_WORKSHOP_WESTPARK:
                            textId = LANG_WG_WESTPARK_CAPTURED_H;
                            break;
                        case WG_WORKSHOP_EASTPARK:
                            textId = LANG_WG_EASTPARK_CAPTURED_H;
                            break;
                        case WG_WORKSHOP_SUNKEN_RING:
                            textId = LANG_WG_SUNKEN_RING_CAPTURED_H;
                            break;
                    }
                }
                break;
            }
            else if (uiEventId == info->capturePoint.contestedEventID1)
            {
                //SetBannerVisual(pGo, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
                switch(idx)
                {
                    case WG_WORKSHOP_BROKEN_TEMPLE:
                        textId = LANG_WG_BROKEN_TEMPLE_ATTACKED_A;
                        break;
                    case WG_WORKSHOP_WESTPARK:
                        textId = LANG_WG_WESTPARK_ATTACKED_A;
                        break;
                    case WG_WORKSHOP_EASTPARK:
                        textId = LANG_WG_EASTPARK_ATTACKED_A;
                        break;
                    case WG_WORKSHOP_SUNKEN_RING:
                        textId = LANG_WG_SUNKEN_RING_ATTACKED_A;
                        break;
                }
            }
            else if (uiEventId == info->capturePoint.contestedEventID2)
            {
                //SetBannerVisual(pGo, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
                switch(idx)
                {
                    case WG_WORKSHOP_BROKEN_TEMPLE:
                        textId = LANG_WG_BROKEN_TEMPLE_ATTACKED_H;
                        break;
                    case WG_WORKSHOP_WESTPARK:
                        textId = LANG_WG_WESTPARK_ATTACKED_H;
                        break;
                    case WG_WORKSHOP_EASTPARK:
                        textId = LANG_WG_EASTPARK_ATTACKED_H;
                        break;
                    case WG_WORKSHOP_SUNKEN_RING:
                        textId = LANG_WG_SUNKEN_RING_ATTACKED_H;
                        break;
                }
            }
            break;
        }
    }

    if (textId)
        SendWarningToAll(textId);
}

bool OutdoorPvPWG::HandleObjectUse(Player* pPlayer, GameObject* pGo)
{
    if (!IsMember(pPlayer->GetObjectGuid()))
        return false;

    switch(pGo->GetEntry())
    {
        case GO_TITAN_RELIC:
        {
            if (m_state != WG_STATE_IN_PROGRESS)
                return true;

            TeamIndex teamIdx = GetTeamIndex(pPlayer->GetTeam());
            if (teamIdx == m_defender)
                return true;

            for (std::list<WGObject*>::iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
                if ((*itr)->id == GO_WINTERGRASP_FORTRESS_DOOR && !(*itr)->IsDestroyed())
                    return true;

            EndBattle(teamIdx, false);
            break;
        }
    }

    return false;
}

void OutdoorPvPWG::Update(uint32 diff)
{
    if (m_timer < diff)
    {
        if (m_state == WG_STATE_COOLDOWN)
            StartBattle(m_defender);
        else if (m_state == WG_STATE_IN_PROGRESS)
            EndBattle(m_defender, true);

        return;
    }
    else
        m_timer -= diff;

    if (m_state == WG_STATE_IN_PROGRESS)
        if (m_scoresUpdateTimer < diff)
        {
            m_scoresUpdateTimer = 5000;
            UpdatePlayerScores();
        }
        else
            m_scoresUpdateTimer -= diff;

    if (m_state == WG_STATE_COOLDOWN && m_timer <= WG_START_INVITE_TIME && !bInvited)
    {
        bInvited = true;
        for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
        {
            if (!*itr || !(*itr)->IsInWorld())
                continue;

            InvitePlayerToQueue(*itr);
        }
    }

    if (m_state == WG_STATE_COOLDOWN && m_timer <= 3 * MINUTE * IN_MILLISECONDS && !bAboutSend)
    {
        bAboutSend = true;
        SendWarningToAll(LANG_WG_ABOUT_TO_BEGIN);
    }

    if (m_state == WG_STATE_IN_PROGRESS)
    {
        for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
        {
            for (std::map<ObjectGuid, time_t>::iterator itr = m_InvitedPlayers[i].begin(); itr != m_InvitedPlayers[i].end();)
            {
                if (itr->second < time(NULL))
                {
                    if (Player* plr = GetMap()->GetPlayer(itr->first))
                        plr->GetSession()->SendBfLeaveMessage(m_BattlefieldId, BATTLEFIELD_LEAVE_REASON_EXITED);
                    m_InvitedPlayers[i].erase(itr++);
                }
                else
                    ++itr;
            }
        }
    }

    if (m_queueUpdateTimer < diff)
    {
        m_queueUpdateTimer = 30000;

        for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
        {
            for (std::set<ObjectGuid>::iterator itr = m_QueuedPlayers[i].begin(); itr != m_QueuedPlayers[i].end();)
            {
                if (!sObjectMgr.GetPlayer(*itr, true))
                    m_QueuedPlayers[i].erase(itr++);
                else
                    ++itr;
            }
        }
    }
    else
        m_queueUpdateTimer -= diff;
}

Map* OutdoorPvPWG::GetMap()
{
    return sMapMgr.CreateMap(MAP_ID_NORTHREND, NULL);
}

void OutdoorPvPWG::SpawnCreature(Creature* pCreature, uint32 respawnTime, bool despawn)
{
    if (despawn)
    {
        pCreature->SetRespawnDelay(respawnTime ? respawnTime : 7 * DAY);
        pCreature->ForcedDespawn();
    }
    else
    {
        pCreature->SetRespawnDelay(respawnTime ? respawnTime : 5 * MINUTE);
        pCreature->Respawn();
    }
}

Creature* OutdoorPvPWG::SpawnCreature(ObjectGuid guid, uint32 respawnTime, bool despawn)
{

    if (Creature* pCreature = GetMap()->GetCreature(guid))
    {
        SpawnCreature(pCreature, respawnTime, despawn);

        return pCreature;
    }

    return NULL;
}

void OutdoorPvPWG::StartBattle(TeamIndex defender)
{
    m_startTime = time(NULL);
    m_state = WG_STATE_IN_PROGRESS;
    m_timer = WG_BATTLE_TIME;
    b10MinAchiev = false;
    m_defender = defender;

    for (uint8 i = 0; i < WG_WORKSHOP_COUNT; ++i)
    {
        switch(i)
        {
            case WG_WORKSHOP_BROKEN_TEMPLE:
            case WG_WORKSHOP_SUNKEN_RING:
            case WG_WORKSHOP_KEEP_WEST:
            case WG_WORKSHOP_KEEP_EAST:
                m_workshops[i]->InitFor(m_defender, true);
                break;
            default:
                m_workshops[i]->InitFor(GetAttacker(), true);
                break;
        }
    }

    for (uint8 i = 0; i < WG_KEEPTOWER_COUNT; ++i)
        m_keepTowers[i]->InitFor(m_defender, true);

    for (uint8 i = 0; i < WG_TOWER_COUNT; ++i)
        m_towers[i]->InitFor(GetAttacker(), true);

    for (std::list<ObjectGuid>::iterator itr = m_zoneTrashGUIDs.begin(); itr != m_zoneTrashGUIDs.end(); ++itr)
        SpawnCreature(*itr, 0, true);

    for (std::list<WGObject*>::iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
        (*itr)->InitFor(m_defender);

    for (std::list<ObjectGuid>::iterator itr = m_keepCannonGUIDs.begin(); itr != m_keepCannonGUIDs.end(); ++itr)
        if (Creature* pCreature = SpawnCreature(*itr, HOUR))
            pCreature->setFaction(WGFactions[m_defender]);

    for (std::list<ObjectGuid>::iterator itr = m_portalGUIDs.begin(); itr != m_portalGUIDs.end(); ++itr)
        if (GameObject* pGo = GetMap()->GetGameObject(*itr))
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);

    if (GameObject* obj = GetMap()->GetGameObject(m_titanRelicGUID))
        obj->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[GetAttacker()]);

    InitPlayerScores();

    UpdateTowerControllBuff();

    BuffTeam(GetTeamFromIndex(m_defender), SPELL_ESSENCE_OF_WINTERGRASP_ZONE, true, false);

    SendUpdateWorldStatesToAll();

    sWorld.SendUpdateWintergraspTimerWorldState(this);

    DEBUG_LOG("Disbanding groups");
    for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        while (!m_Raids[i].empty())
        {
            Group* group = *m_Raids[i].begin();
            group->Disband();
            delete group;
        }
    }

    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld())
            continue;

        uint8 idx = 0;
        std::set<ObjectGuid>::iterator itr2 = m_QueuedPlayers[idx].find((*itr)->GetObjectGuid());
        if (itr2 == m_QueuedPlayers[idx].end())
        {
            ++idx;
            itr2 = m_QueuedPlayers[idx].find((*itr)->GetObjectGuid());
        }

        if ((*itr)->IsFreeFlying() && (*itr)->isGameMaster() && !(*itr)->HasAura(SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA))
            (*itr)->CastSpell(*itr, SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA, true);

        if (itr2 != m_QueuedPlayers[idx].end())
        {
            m_InvitedPlayers[idx][(*itr)->GetObjectGuid()] = time(NULL) + WG_TIME_TO_ACCEPT;
            (*itr)->GetSession()->SendBfInvitePlayerToWar(m_BattlefieldId, m_BattlefieldZoneId, WG_TIME_TO_ACCEPT);
            m_QueuedPlayers[idx].erase(itr2);
        }
        else
        {
            (*itr)->GetSession()->SendBfLeaveMessage(BATTLEFIELD_WG, BATTLEFIELD_LEAVE_REASON_EXITED);
            SendRemoveWorldStates(*itr);
            if (m_playerScores.find((*itr)->GetObjectGuid()) != m_playerScores.end())
            {
                m_playerScores[(*itr)->GetObjectGuid()]->removeTime = time(NULL);
                m_playerScores[(*itr)->GetObjectGuid()]->removeDelay = WG_UNACCEPTED_REMOVE_DELAY;
            }
        }
    }

    // non-zone queued players
    for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        for (std::set<ObjectGuid>::iterator itr = m_QueuedPlayers[i].begin(); itr != m_QueuedPlayers[i].end();)
        {
            if (Player* plr = sObjectMgr.GetPlayer(*itr, true))
            {
                m_InvitedPlayers[i][plr->GetObjectGuid()] = time(NULL) + WG_TIME_TO_ACCEPT;
                plr->GetSession()->SendBfInvitePlayerToWar(m_BattlefieldId, m_BattlefieldZoneId, WG_TIME_TO_ACCEPT);
            }

            m_QueuedPlayers[i].erase(itr++);
        }
    }

    SendWarningToAll(LANG_WG_BATTLE_BEGIN);
}

void OutdoorPvPWG::EndBattle(TeamIndex winner, bool byTimer)
{
    if (m_defender == winner)
        SendWarningToAll(m_defender == TEAM_INDEX_ALLIANCE ? LANG_WG_ALLIANCE_DEFENDED : LANG_WG_HORDE_DEFENDED);
    else
        SendWarningToAll(winner == TEAM_INDEX_ALLIANCE ? LANG_WG_ALLIANCE_CAPTURED : LANG_WG_HORDE_CAPTURED);

    uint32 oldTimer = m_timer;
    if (!byTimer && winner != m_defender &&
        oldTimer + (m_destroyedTowers[m_defender] == 3 ? WG_TOWERS_BONUS : 0) + 10 * MINUTE * IN_MILLISECONDS > WG_BATTLE_TIME)
        b10MinAchiev = true;

    m_defender = winner;
    m_state = WG_STATE_COOLDOWN;
    m_timer = WG_COOLDOWN_DURATION;
    bInvited = false;
    bAboutSend = false;

    RewardPlayersAtEnd(winner);

    for (uint8 i = 0; i < WG_WORKSHOP_COUNT; ++i)
        m_workshops[i]->InitFor(m_defender, false);

    for (uint8 i = 0; i < WG_KEEPTOWER_COUNT; ++i)
        m_keepTowers[i]->InitFor(m_defender, false);

    for (uint8 i = 0; i < WG_TOWER_COUNT; ++i)
        m_towers[i]->InitFor(TEAM_INDEX_NEUTRAL, false);

    for (std::list<ObjectGuid>::iterator itr = m_zoneTrashGUIDs.begin(); itr != m_zoneTrashGUIDs.end(); ++itr)
        SpawnCreature(*itr);

    for (std::list<WGObject*>::iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
        (*itr)->InitFor(m_defender, false);

    for (std::list<ObjectGuid>::iterator itr = m_keepCannonGUIDs.begin(); itr != m_keepCannonGUIDs.end(); ++itr)
        if (Creature* pCreature = GetMap()->GetCreature(*itr))
            pCreature->setFaction(WGFactions[m_defender]);

    for (std::list<ObjectGuid>::iterator itr = m_portalGUIDs.begin(); itr != m_portalGUIDs.end(); ++itr)
        if (GameObject* pGo = GetMap()->GetGameObject(*itr))
            pGo->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[m_defender]);

    UpdateTowerControllBuff();
    BuffTeam(TEAM_NONE, SPELL_RECRUIT, true, false);
    BuffTeam(TEAM_NONE, SPELL_CORPORAL, true, false);
    BuffTeam(TEAM_NONE, SPELL_LIEUTENANT, true, false);

    BuffTeam(GetTeamFromIndex(m_defender), SPELL_ESSENCE_OF_WINTERGRASP_ZONE, false);

    if (!byTimer)
    {
        BuffTeam(TEAM_NONE, winner == TEAM_INDEX_ALLIANCE ? SPELL_HORDE_CONTROL_PHASE_SHIFT : SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, true, false);
        BuffTeam(TEAM_NONE, winner == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE_SHIFT : SPELL_HORDE_CONTROL_PHASE_SHIFT, false, false);
    }

    // DespawnVehicles()
    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
    {
        for (std::list<ObjectGuid>::iterator itr = m_vehicleGUIDs[i].begin(); itr != m_vehicleGUIDs[i].end(); ++itr)
            if (Creature* pCreature = GetMap()->GetCreature(*itr))
                pCreature->ForcedDespawn();

        m_vehicleGUIDs[i].clear();
    }

    m_damagedTowers[0] = 0;
    m_damagedTowers[1] = 0;
    m_destroyedTowers[0] = 0;
    m_destroyedTowers[1] = 0;

    SendUpdateWorldStatesToAll();

    sWorld.SendUpdateWintergraspTimerWorldState(this);
}

void OutdoorPvPWG::RewardPlayersAtEnd(TeamIndex winner)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld())
            continue;

        Player* plr = *itr;
        TeamIndex teamIdx = GetTeamIndex(plr->GetTeam());

        if (teamIdx == winner)
        {
            int32 amount = uint32(GetPlayerRank(plr));
            if (!amount)
                continue;

            plr->CastCustomSpell(plr, SPELL_VICTORY_REWARD, NULL, &amount, NULL, true);
            plr->AreaExploredOrEventHappens(teamIdx == TEAM_INDEX_ALLIANCE ? WG_QUEST_VICTORY_IN_WINTERGRASP_A : WG_QUEST_VICTORY_IN_WINTERGRASP_H);
        }
        else
            plr->CastSpell(plr, SPELL_DEFEAT_REWARD, true);
    }
}

void OutdoorPvPWG::UpdateScoreBuff(Player* plr)
{
    uint32 buffs[3] = { SPELL_RECRUIT, SPELL_CORPORAL, SPELL_LIEUTENANT };

    if (m_state != WG_STATE_IN_PROGRESS || !IsMember(plr->GetObjectGuid()))
    {
        for (uint32 i = 0; i < 3; ++i)
            plr->RemoveAurasDueToSpell(buffs[i]);

        return;
    }

    WGPlayerScoreMap::iterator itr = m_playerScores.find(plr->GetObjectGuid());
    if (itr == m_playerScores.end())
        return;

    uint32 spellId;
    uint8 stack;
    switch(itr->second->GetRank())
    {
        case WG_RANK_RECRUIT:
            spellId = SPELL_RECRUIT;
            stack = itr->second->rank + 1;
            break;
        case WG_RANK_CORPORAL:
            spellId = SPELL_CORPORAL;
            stack = itr->second->rank - 4;
            break;
        case WG_RANK_LIEUTENANT:
            spellId = SPELL_LIEUTENANT;
            stack = 1;
            break;
        default:
            return;
    }

    for (uint32 i = 0; i < 3; ++i)
        if (buffs[i] != spellId)
            plr->RemoveAurasDueToSpell(buffs[i]);

    if (SpellAuraHolder* holder = plr->GetSpellAuraHolder(spellId))
        holder->SetStackAmount(stack);
    else
        for (uint8 i = 0; i < stack; ++i)
            plr->CastSpell(plr, spellId, true);
}

void OutdoorPvPWG::UpdateTowerControllBuff(Player* plr)
{
    uint8 towersCount = GetRemainingTowers();

    if (plr)
    {
        plr->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
        
        if (m_state == WG_STATE_COOLDOWN || !IsMember(plr->GetObjectGuid()))
            return;

        if (GetTeamIndex(plr->GetTeam()) == m_defender)
            towersCount = 3 - towersCount;
        if (towersCount > 1)
            for (uint8 i = 0; i < towersCount; ++i)
                plr->CastSpell(plr, SPELL_TOWER_CONTROL, true);
    }
    else
    {
        for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
        {
            if (!*itr || !(*itr)->IsInWorld())
                continue;

            (*itr)->RemoveAurasDueToSpell(SPELL_TOWER_CONTROL);
            if (!IsMember((*itr)->GetObjectGuid()))
                continue;

            uint8 cnt = towersCount;

            if (m_state == WG_STATE_COOLDOWN)
                continue;

            if (GetTeamIndex((*itr)->GetTeam()) == m_defender)
                cnt = 3 - cnt;
            if (cnt > 1)
                for (uint8 i = 0; i < cnt; ++i)
                    (*itr)->CastSpell(*itr, SPELL_TOWER_CONTROL, true);
        }
    }
}

void OutdoorPvPWG::InitPlayerScores()
{
    for (WGPlayerScoreMap::iterator itr = m_playerScores.begin(); itr != m_playerScores.end(); ++itr)
        delete itr->second;

    m_playerScores.clear();

    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld())
            continue;

        m_playerScores[(*itr)->GetObjectGuid()] = new WGPlayerScore();

        UpdateScoreBuff(*itr);
    }
}

void OutdoorPvPWG::UpdatePlayerScores()
{
    bool bRemoved = false;
    for (WGPlayerScoreMap::iterator itr = m_playerScores.begin(); itr != m_playerScores.end();)
    {
        if (itr->second->removeTime && itr->second->removeTime + itr->second->removeDelay < time(NULL))
        {
            delete itr->second;
            if (Player* plr = GetMap()->GetPlayer(itr->first))
                KickPlayer(plr);

            if (IsMember(itr->first))
            {
                RemovePlayerFromRaid(itr->first);
                bRemoved = true;
            }

            m_playerScores.erase(itr++);

        }
        else
            ++itr;
    }

    if (bRemoved)
        UpdateTenacities();
}

void OutdoorPvPWG::KickPlayer(Player* plr)
{
    if (HasPlayer(plr))
    {
        uint8 idx = 0;
        float dist = plr->GetDistance2d(WGKickPositions[0].x, WGKickPositions[0].y);
        for (uint8 i = 1; i < KICK_POSITIONS_COUNT; ++i)
        {
            float newDist = plr->GetDistance2d(WGKickPositions[i].x, WGKickPositions[i].y);
            if (newDist < dist)
            {
                dist = newDist;
                idx = i;
            }
        }

        plr->TeleportTo(plr->GetMapId(), WGKickPositions[idx].x, WGKickPositions[idx].y, WGKickPositions[idx].radius, plr->GetOrientation());
    }
}

bool OutdoorPvPWG::CanDamageGO(GameObject* pGo, Player* invoker)
{
    if (m_state != WG_STATE_IN_PROGRESS)
        return false;

    if (invoker && !IsMember(invoker->GetObjectGuid()))
        return false;

    switch(pGo->GetEntry())
    {
        case GO_WORKSHOP_KEEP_WEST:
        case GO_WORKSHOP_KEEP_EAST:
        case GO_WORKSHOP_BROKEN_TEMPLE:
        case GO_WORKSHOP_SUNKEN_RING:
        case GO_WORKSHOP_WESTPARK:
        case GO_WORKSHOP_EASTPARK:
            return false;
        case GO_WINTERGRASP_FORTRESS_DOOR:
        case GO_WINTERGRASP_FORTRESS_GATE:
        case GO_WINTERGRASP_FORTRESS_WALL_1:
        case GO_WINTERGRASP_FORTRESS_WALL_2:
        case GO_WINTERGRASP_FORTRESS_WALL_3:
        case GO_WINTERGRASP_FORTRESS_WALL_4:
        case GO_WINTERGRASP_FORTRESS_WALL_5:
        case GO_WINTERGRASP_FORTRESS_WALL_6:
        case GO_WINTERGRASP_FORTRESS_WALL_7:
        case GO_WINTERGRASP_FORTRESS_WALL_8:
        case GO_WINTERGRASP_FORTRESS_WALL_9:
        case GO_WINTERGRASP_FORTRESS_WALL_10:
        case GO_WINTERGRASP_FORTRESS_WALL_11:
        case GO_WINTERGRASP_FORTRESS_WALL_12:
        case GO_WINTERGRASP_FORTRESS_WALL_13:
        case GO_WINTERGRASP_FORTRESS_WALL_14:
        case GO_WINTERGRASP_FORTRESS_WALL_15:
        case GO_WINTERGRASP_FORTRESS_WALL_16:
        case GO_WINTERGRASP_FORTRESS_WALL_17:
        case GO_WINTERGRASP_FORTRESS_WALL_18:
        case GO_WINTERGRASP_FORTRESS_WALL_19:
        case GO_WINTERGRASP_FORTRESS_WALL_20:
        case GO_WINTERGRASP_WALL_1:
        case GO_WINTERGRASP_WALL_2:
        case GO_WINTERGRASP_WALL_3:
        case GO_WINTERGRASP_TOWER_WEST:
        case GO_WINTERGRASP_TOWER_SOUTHWEST:
        case GO_WINTERGRASP_TOWER_SOUTHEAST:
        case GO_WINTERGRASP_TOWER_EAST:
            return !invoker || GetTeamIndex(invoker->GetTeam()) != m_defender;
        case GO_TOWER_SHADOWSIGHT:
        case GO_TOWER_WINTERS_EDGE:
        case GO_TOWER_FLAMEWATCH:
            return !invoker || GetTeamIndex(invoker->GetTeam()) == m_defender;
    }

    return true;
}

void OutdoorPvPWG::SetupPlayerPositions()
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld() || !IsMember((*itr)->GetObjectGuid()))
            continue;

        SetupPlayerPosition(*itr);
    }
}

void OutdoorPvPWG::SetupPlayerPosition(Player* player)
{
    if (GetTeamIndex(player->GetTeam()) != m_defender && player->GetPositionX() > 5395.0f && player->GetPositionY() > 2802.0f && player->GetPositionY() < 2879.0f && player->GetPositionZ() < 476.0f)
        player->CastSpell(player, SPELL_TELEPORT_BRIDGE, true);
}

void OutdoorPvPWG::GraveYardChanged(uint8 id, TeamIndex newOwner)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld())
            continue;

        if (newOwner == GetTeamIndex((*itr)->GetTeam()))
            continue;

        if (!(*itr)->isDead() || !(*itr)->HasAura(SPELL_SPIRITUAL_IMMUNITY))
            continue;

        uint8 idx = 0;
        switch((*itr)->GetAreaId())
        {
            case AREA_ID_THE_CHILLED_QUAGMIRE:
                if ((*itr)->GetPositionY() < 3537.0f)
                    idx = WG_WORKSHOP_BROKEN_TEMPLE;
                else
                    continue;
                break;
            case AREA_ID_WESTPARK_WORKSHOP:
                idx = WG_WORKSHOP_WESTPARK;
                break;
            case AREA_ID_EASTPARK_WORKSHOP:
                idx = WG_WORKSHOP_EASTPARK;
                break;
            case ZONE_ID_WINTERGRASP:
                if ((*itr)->GetPositionY() > 2256.0f)
                    idx = WG_WORKSHOP_SUNKEN_RING;
                else
                    continue;
                break;
            case AREA_ID_THE_SUNKEN_RING:
                idx = WG_WORKSHOP_SUNKEN_RING;
                break;
            case AREA_ID_WINTERGRASP_FORTRESS:
                idx = WG_WORKSHOP_KEEP_WEST;
                break;
            default:
                continue;
        }

        if (idx == id)
            (*itr)->RepopAtGraveyard();
    }
}

TeamIndex OutdoorPvPWG::GetWorkshopOwner(uint8 idx)
{
    return m_workshops[idx]->owner;
}

WGRank OutdoorPvPWG::GetPlayerRank(Player* player)
{
    WGPlayerScoreMap::iterator itr = m_playerScores.find(player->GetObjectGuid());
    if (itr == m_playerScores.end())
        return WG_RANK_NONE;

    return itr->second->GetRank();
}

bool OutdoorPvPWG::CanBuildMoreVehicles(TeamIndex teamIdx)
{
    return m_vehicleGUIDs[teamIdx].size() < GetWorkshopsOwnedBy(teamIdx) * 4;
}

bool WGObject::IsIntact()
{
    return state == WG_OBJECTSTATE_NEUTRAL_INTACT || state == WG_OBJECTSTATE_HORDE_INTACT || state == WG_OBJECTSTATE_ALLIANCE_INTACT;
}

bool WGObject::IsDamaged()
{
    return state == WG_OBJECTSTATE_NEUTRAL_DAMAGED || state == WG_OBJECTSTATE_HORDE_DAMAGED || state == WG_OBJECTSTATE_ALLIANCE_DAMAGED;
}

bool WGObject::IsDestroyed()
{
    return state == WG_OBJECTSTATE_NEUTRAL_DESTROYED || state == WG_OBJECTSTATE_HORDE_DESTROYED || state == WG_OBJECTSTATE_ALLIANCE_DESTROYED;
}

void WGObject::SendUpdateWorldState()
{
    opvp->SendUpdateWorldStateForMap(worldState, state, opvp->GetMap());
}

void WGWorkShop::SendUpdateWorldState()
{
    WGObject::SendUpdateWorldState();

    opvp->SendUpdateWorldState(WG_WS_MAX_VEHICLE_A, opvp->GetWorkshopsOwnedBy(TEAM_INDEX_ALLIANCE) * 4);
    opvp->SendUpdateWorldState(WG_WS_MAX_VEHICLE_H, opvp->GetWorkshopsOwnedBy(TEAM_INDEX_HORDE) * 4);
}

void WGObject::SetIntact()
{
    if (owner == TEAM_INDEX_ALLIANCE)
        state = WG_OBJECTSTATE_ALLIANCE_INTACT;
    else if (owner == TEAM_INDEX_HORDE)
        state = WG_OBJECTSTATE_HORDE_INTACT;
    else
        state = WG_OBJECTSTATE_NEUTRAL_INTACT;
}

void WGObject::SetDamaged()
{
    if (owner == TEAM_INDEX_ALLIANCE)
        state = WG_OBJECTSTATE_ALLIANCE_DAMAGED;
    else if (owner == TEAM_INDEX_HORDE)
        state = WG_OBJECTSTATE_HORDE_DAMAGED;
    else
        state = WG_OBJECTSTATE_NEUTRAL_DAMAGED;
}

void WGObject::SetDestroyed()
{
    if (owner == TEAM_INDEX_ALLIANCE)
        state = WG_OBJECTSTATE_ALLIANCE_DESTROYED;
    else if (owner == TEAM_INDEX_HORDE)
        state = WG_OBJECTSTATE_HORDE_DESTROYED;
    else
        state = WG_OBJECTSTATE_NEUTRAL_DESTROYED;
}

void WGObject::UpdateStateForOwner()
{
    if (IsIntact())
        SetIntact();
    else if (IsDamaged())
        SetDamaged();
    else if (IsDestroyed())
        SetDestroyed();
}

void WGTower::SpawnCannons(bool despawn)
{
    for (std::list<ObjectGuid>::iterator itr = cannons.begin(); itr != cannons.end(); ++itr)
    {
        if (Creature* pCreature = opvp->GetMap()->GetCreature(*itr))
        {
            if (despawn)
                opvp->SpawnCreature(pCreature, 0, true);
            else
            {
                opvp->SpawnCreature(pCreature, 5 * MINUTE, false);
                pCreature->setFaction(WGFactions[owner]);
            }
        }
    }
}

void WGObject::InitFor(TeamIndex teamIdx, bool reset)
{
    owner = teamIdx;
    if (reset)
        SetIntact();
    else
        UpdateStateForOwner();

    if (GameObject* obj = opvp->GetMap()->GetGameObject(guid))
    {
        if (reset)
            obj->Rebuild(NULL);

        obj->SetUInt32Value(GAMEOBJECT_FACTION, WGFactions[owner]);
    }
}

void WGWorkShop::InitFor(TeamIndex teamIdx, bool reset)
{
    bool gyChanged = teamIdx != owner;

    WGObject::InitFor(teamIdx, reset);

    //sObjectMgr.RemoveGraveYardLink(gy, ZONE_ID_WINTERGRASP, TEAM_NONE, false);
    //sObjectMgr.AddGraveYardLink(gy, ZONE_ID_WINTERGRASP, GetTeamFromIndex(owner), false);

    if (gyChanged)
        opvp->GraveYardChanged(id, owner);

    if (GameObject* obj = opvp->GetMap()->GetGameObject(capturePoint))
    {
        if (opvp->GetState() == WG_STATE_COOLDOWN)
        {
            obj->SetLootState(GO_JUST_DEACTIVATED);
            obj->SetCapturePointSlider(owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
        }
        else
            obj->SetCapturePointSlider(owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

        if (owner == TEAM_INDEX_ALLIANCE)
            opvp->SetBannerVisual(obj, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);
        else
            opvp->SetBannerVisual(obj, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);
    }

    uint32 spellToRemove = owner == TEAM_INDEX_ALLIANCE ? SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT : SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT;
    uint32 spellToAdd = owner == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROLS_FACTORY_PHASE_SHIFT : SPELL_HORDE_CONTROLS_FACTORY_PHASE_SHIFT;

    switch(id)
    {
        case WG_WORKSHOP_KEEP_WEST:
        //case WG_WORKSHOP_KEEP_EAST:
            opvp->BuffTeam(TEAM_NONE, spellToRemove, true, false, AREA_ID_WINTERGRASP_FORTRESS);
            opvp->BuffTeam(TEAM_NONE, spellToAdd, false, false, AREA_ID_WINTERGRASP_FORTRESS);
            break;
        case WG_WORKSHOP_WESTPARK:
            opvp->BuffTeam(TEAM_NONE, spellToRemove, true, false, AREA_ID_WESTPARK_WORKSHOP);
            opvp->BuffTeam(TEAM_NONE, spellToAdd, false, false, AREA_ID_WESTPARK_WORKSHOP);
            break;
        case WG_WORKSHOP_BROKEN_TEMPLE:
            opvp->BuffTeam(TEAM_NONE, spellToRemove, true, false, AREA_ID_THE_BROKEN_TEMPLE);
            opvp->BuffTeam(TEAM_NONE, spellToAdd, false, false, AREA_ID_THE_BROKEN_TEMPLE);
            opvp->BuffTeam(TEAM_NONE, spellToRemove, true, false, AREA_ID_THE_CHILLED_QUAGMIRE);
            opvp->BuffTeam(TEAM_NONE, spellToAdd, false, false, AREA_ID_THE_CHILLED_QUAGMIRE);
            break;
        case WG_WORKSHOP_SUNKEN_RING:
            opvp->BuffTeam(TEAM_NONE, spellToRemove, true, false, AREA_ID_THE_SUNKEN_RING);
            opvp->BuffTeam(TEAM_NONE, spellToAdd, false, false, AREA_ID_THE_SUNKEN_RING);
            break;
        case WG_WORKSHOP_EASTPARK:
            opvp->BuffTeam(TEAM_NONE, spellToRemove, true, false, AREA_ID_EASTPARK_WORKSHOP);
            opvp->BuffTeam(TEAM_NONE, spellToAdd, false, false, AREA_ID_EASTPARK_WORKSHOP);
            break;
    }
}

void WGTower::InitFor(TeamIndex teamIdx, bool reset)
{
    WGObject::InitFor(teamIdx, reset);

    if (reset)
        SpawnCannons();
    else if (teamIdx == TEAM_INDEX_NEUTRAL)
        SpawnCannons(true);
    else
    {
        for (std::list<ObjectGuid>::iterator itr = cannons.begin(); itr != cannons.end(); ++itr)
        {
            if (Creature* pCreature = opvp->GetMap()->GetCreature(*itr))
                pCreature->setFaction(WGFactions[teamIdx]);
        }
    }
}

WGRank WGPlayerScore::GetRank()
{
    if (rank <= 4)
        return WG_RANK_RECRUIT;
    else if (rank > 4 && rank <= 14)
        return WG_RANK_CORPORAL;
    else
        return WG_RANK_LIEUTENANT;
}

/********** Grouping *************/

uint32 OutdoorPvPWG::GetPlayerCountByTeam(TeamIndex teamIdx)
{
    uint32 count = 0;
    for (std::set<Group*>::iterator itr = m_Raids[teamIdx].begin(); itr != m_Raids[teamIdx].end(); ++itr)
        count += (*itr)->GetMembersCount();

    return count;
}

bool OutdoorPvPWG::IsTeamFull(TeamIndex teamIdx)
{
    return m_QueuedPlayers[teamIdx].size() + m_InvitedPlayers[teamIdx].size() + GetPlayerCountByTeam(teamIdx) >= WG_MAX_PLAYERS_PER_TEAM;
}

Group* OutdoorPvPWG::GetFreeRaid(TeamIndex teamIdx)
{
    for (std::set<Group*>::iterator itr = m_Raids[teamIdx].begin(); itr != m_Raids[teamIdx].end(); ++itr)
        if (!(*itr)->IsFull())
            return *itr;

    return NULL;
}

bool OutdoorPvPWG::IsMember(ObjectGuid guid)
{
    return GetGroupFor(guid) != NULL;
}

Group* OutdoorPvPWG::GetGroupFor(ObjectGuid guid)
{
    for (uint8 i = 0; i < PVP_TEAM_COUNT; ++i)
        for (std::set<Group*>::iterator itr = m_Raids[i].begin(); itr != m_Raids[i].end(); ++itr)
            if ((*itr)->IsMember(guid))
                return *itr;

    return NULL;
}

bool OutdoorPvPWG::AddPlayerToRaid(Player* player)
{
    if (!player->IsInWorld())
        return false;

    DEBUG_LOG("Wintergrasp: Adding player %s to raid", player->GetGuidStr().c_str());

    if (Group* group = player->GetGroup())
    {
        DEBUG_LOG("Wintergrasp: Player %s already has group %s, uninviting", player->GetGuidStr().c_str(), group->GetObjectGuid().GetString().c_str());
        group->RemoveMember(player->GetObjectGuid(), 0);
    }

    TeamIndex teamIdx = GetTeamIndex(player->GetTeam());
    Group* group = GetFreeRaid(teamIdx);
    if (!group)
    {
        DEBUG_LOG("Wintergrasp: No free raid for %s!", player->GetGuidStr().c_str());
        if (IsTeamFull(teamIdx))
        {
            DEBUG_LOG("Wintergrasp: Battlefield is full! Can't add player %s!", player->GetGuidStr().c_str());
            return false;
        }

        DEBUG_LOG("Wintergrasp: Trying to create new group for %s!", player->GetGuidStr().c_str());
        group = new Group;
        group->SetBattlefieldGroup(this);
        if (group->Create(player->GetObjectGuid(), player->GetName()))
            DEBUG_LOG("Wintergrasp: Successfully created new group %s", group->GetObjectGuid().GetString().c_str());
        else
            DEBUG_LOG("Failed to create group!");

        m_Raids[teamIdx].insert(group);
    }
    else if (group->IsMember(player->GetObjectGuid()))
    {
        DEBUG_LOG("Wintergrasp: Raid already has players %s, making some shit", player->GetGuidStr().c_str());
        uint8 subgroup = group->GetMemberGroup(player->GetObjectGuid());
        player->SetBattleRaid(group, subgroup);
    }
    else
    {
        if (IsTeamFull(teamIdx))
        {
            DEBUG_LOG("Wintergrasp: Group %s found, but battlefield is full! Can't add player %s!", group->GetObjectGuid().GetString().c_str(), player->GetGuidStr().c_str());
            return false;
        }

        return group->AddMember(player->GetObjectGuid(), player->GetName());
    }

    return true;
}

bool OutdoorPvPWG::RemovePlayerFromRaid(ObjectGuid guid)
{
    if (Group* group = GetGroupFor(guid))
    {
        if (group->RemoveMember(guid, 0) == 0)
            delete group;

        return true;
    }

    return false;
}

void OutdoorPvPWG::InvitePlayerToQueue(Player* player)
{
    if (IsMember(player->GetObjectGuid()))
        return;

    TeamIndex teamIdx = GetTeamIndex(player->GetTeam());
    // he is in queue or waiting to accept teleport button
    if (m_QueuedPlayers[teamIdx].find(player->GetObjectGuid()) != m_QueuedPlayers[teamIdx].end())
        return;
    if (m_InvitedPlayers[teamIdx].find(player->GetObjectGuid()) != m_InvitedPlayers[teamIdx].end())
        return;

    if (!IsTeamFull(teamIdx))
        player->GetSession()->SendBfInvitePlayerToQueue(m_BattlefieldId);
    //else
    //    player->GetSession()->SendBfQueueInviteResponse(m_BattlefieldId, m_BattlefieldZoneId, true, true);
}

void OutdoorPvPWG::OnPlayerInviteResponse(Player* plr, bool accept)
{
    if (!accept || IsMember(plr->GetObjectGuid()))
        return;

    TeamIndex teamIdx = GetTeamIndex(plr->GetTeam());
    // not needed
    std::map<ObjectGuid, time_t>::iterator itr = m_InvitedPlayers[teamIdx].find(plr->GetObjectGuid());
    if (itr != m_InvitedPlayers[teamIdx].end())
        m_InvitedPlayers[teamIdx].erase(itr);

    if (IsTeamFull(teamIdx))
    {
        plr->GetSession()->SendBfQueueInviteResponse(m_BattlefieldId, m_BattlefieldZoneId, true, true);
        return;
    }

    if (m_QueuedPlayers[teamIdx].find(plr->GetObjectGuid()) == m_QueuedPlayers[teamIdx].end())
    {
        if (m_state == WG_STATE_IN_PROGRESS)
        {
            m_InvitedPlayers[teamIdx][plr->GetObjectGuid()] = time(NULL) + WG_TIME_TO_ACCEPT;
            plr->GetSession()->SendBfInvitePlayerToWar(m_BattlefieldId, m_BattlefieldZoneId, WG_TIME_TO_ACCEPT);
        }
        else
        {
            m_QueuedPlayers[teamIdx].insert(plr->GetObjectGuid());
            plr->GetSession()->SendBfQueueInviteResponse(m_BattlefieldId, m_BattlefieldZoneId, true, false);
        }
    }
}

void OutdoorPvPWG::OnPlayerPortResponse(Player* plr, bool accept)
{
    if (IsMember(plr->GetObjectGuid()))
        return;

    if (accept)
    {
        TeamIndex teamIdx = GetTeamIndex(plr->GetTeam());
        std::map<ObjectGuid, time_t>::iterator itr = m_InvitedPlayers[teamIdx].find(plr->GetObjectGuid());
        // he was not invited by core
        if (itr == m_InvitedPlayers[teamIdx].end())
            return;

        m_InvitedPlayers[teamIdx].erase(itr);

        if (AddPlayerToRaid(plr))
        {
            DEBUG_LOG("Wintergrasp: AddPlayerToRaid for %s returned: TRUE", plr->GetGuidStr().c_str());
            UpdateTenacities();
            plr->GetSession()->SendBfEntered(m_BattlefieldId);
        }
        else
            DEBUG_LOG("Wintergrasp: AddPlayerToRaid for %s returned: FALSE", plr->GetGuidStr().c_str());        

        if (plr->GetZoneId() != ZONE_ID_WINTERGRASP)
            plr->CastSpell(plr, SPELL_TELEPORT_WINTERGRASP, true);
        else
        {
            if (m_playerScores.find(plr->GetObjectGuid()) == m_playerScores.end())  // must never happen
                m_playerScores[plr->GetObjectGuid()] = new WGPlayerScore();

            plr->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE_SHIFT);
            plr->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE_SHIFT);
            plr->CastSpell(plr, m_defender == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE_SHIFT : SPELL_HORDE_CONTROL_PHASE_SHIFT, true);

            UpdateTowerControllBuff(plr);
            UpdateScoreBuff(plr);

            SendUpdateWorldStatesTo(plr);
        }
    }
    else if (m_state == WG_STATE_IN_PROGRESS && plr->GetZoneId() == ZONE_ID_WINTERGRASP)
    {
        plr->GetSession()->SendBfLeaveMessage(BATTLEFIELD_WG, BATTLEFIELD_LEAVE_REASON_EXITED);
        if (m_playerScores.find(plr->GetObjectGuid()) != m_playerScores.end())
        {
            m_playerScores[plr->GetObjectGuid()]->removeTime = time(NULL);
            m_playerScores[plr->GetObjectGuid()]->removeDelay = WG_UNACCEPTED_REMOVE_DELAY;
        }
    }
}

void OutdoorPvPWG::OnPlayerQueueExitRequest(Player* plr)
{
    if (IsMember(plr->GetObjectGuid()))
    {
        plr->GetSession()->SendBfLeaveMessage(BATTLEFIELD_WG, BATTLEFIELD_LEAVE_REASON_EXITED);
        RemovePlayerFromRaid(plr->GetObjectGuid());
        SendRemoveWorldStates(plr);
        UpdateTenacities();
        if (m_playerScores.find(plr->GetObjectGuid()) != m_playerScores.end())
        {
            m_playerScores[plr->GetObjectGuid()]->removeTime = time(NULL);
            m_playerScores[plr->GetObjectGuid()]->removeDelay = WG_UNACCEPTED_REMOVE_DELAY;
        }
        return;
    }

    std::set<ObjectGuid>::iterator itr = m_QueuedPlayers[GetTeamIndex(plr->GetTeam())].find(plr->GetObjectGuid());
    if (itr != m_QueuedPlayers[GetTeamIndex(plr->GetTeam())].end())
    {
        m_QueuedPlayers[GetTeamIndex(plr->GetTeam())].erase(itr);

        plr->GetSession()->SendBfLeaveMessage(BATTLEFIELD_WG, BATTLEFIELD_LEAVE_REASON_EXITED);
        if (m_playerScores.find(plr->GetObjectGuid()) != m_playerScores.end())
        {
            m_playerScores[plr->GetObjectGuid()]->removeTime = time(NULL);
            m_playerScores[plr->GetObjectGuid()]->removeDelay = WG_UNACCEPTED_REMOVE_DELAY;
        }
    }
}

bool OutdoorPvPWG::OnGroupDeleted(Group* group)
{
    for (uint32 i = 0; i < 2; ++i)
        for (std::set<Group*>::iterator itr = m_Raids[i].begin(); itr != m_Raids[i].end(); ++itr)
            if (*itr == group)
            {
                m_Raids[i].erase(itr);
                return true;
            }

    return false;
}

void OutdoorPvPWG::UpdateTenacities()
{
    int32 aCount = GetPlayerCountByTeam(TEAM_INDEX_ALLIANCE);
    int32 hCount = GetPlayerCountByTeam(TEAM_INDEX_HORDE);
    uint32 diff = abs(aCount - hCount);

    diff /= 10;

    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld())
            continue;

        (*itr)->RemoveAurasDueToSpell(SPELL_TENACITY);

        if (diff && (aCount > hCount && (*itr)->GetTeam() == HORDE || 
            hCount > aCount && (*itr)->GetTeam() == ALLIANCE) && IsMember((*itr)->GetObjectGuid()))
            for (uint32 i = 0; i < diff; ++i)
                (*itr)->CastSpell(*itr, SPELL_TENACITY, true);
    }

    for (uint32 i = 0; i < PVP_TEAM_COUNT; ++i)
        for (std::list<ObjectGuid>::iterator itr = m_vehicleGUIDs[i].begin(); itr != m_vehicleGUIDs[i].end(); ++itr)
        {
            if (Creature* pCreature = GetMap()->GetCreature(*itr))
            {
                pCreature->RemoveAurasDueToSpell(SPELL_TENACITY_VEHICLE);

                if (diff && (aCount > hCount && i == TEAM_INDEX_HORDE || 
                    hCount > aCount && i == TEAM_INDEX_ALLIANCE))
                    for (uint32 i = 0; i < diff; ++i)
                        pCreature->CastSpell(pCreature, SPELL_TENACITY_VEHICLE, true);
            }
        }
}

void OutdoorPvPWG::SendWarningToAll(int32 entry)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld() || (*itr)->GetZoneId() != ZONE_ID_WINTERGRASP)
            continue;

        int32 loc_idx = (*itr)->GetSession()->GetSessionDbLocaleIndex();

        char const* text = sObjectMgr.GetMangosString(entry, loc_idx);

        WorldPacket data(SMSG_MESSAGECHAT, 200);
        data << uint8(CHAT_MSG_RAID_BOSS_EMOTE);
        data << uint32(LANG_UNIVERSAL);
        data << ObjectGuid();
        data << uint32(0);
        data << uint32(1);
        data << uint8(0);
        data << ObjectGuid();
        data << uint32(strlen(text)+1);
        data << text;
        data << uint8(0);
        (*itr)->GetSession()->SendPacket(&data);
    }
}

void OutdoorPvPWG::SendCaptainYell()
{
    int32 textId = 0;
    switch(m_destroyedTowers[m_defender])
    {
        case 1:
            textId = m_defender == TEAM_INDEX_ALLIANCE ? LANG_WG_DESTROYED_FIRST_A : LANG_WG_DESTROYED_FIRST_H;
            break;
        case 2:
            textId = m_defender == TEAM_INDEX_ALLIANCE ? LANG_WG_DESTROYED_SECOND_A : LANG_WG_DESTROYED_SECOND_H;
            break;
        case 3:
            textId = m_defender == TEAM_INDEX_ALLIANCE ? LANG_WG_DESTROYED_THIRD_A : LANG_WG_DESTROYED_THIRD_H;
            break;
    }

    if (textId)
        GetMap()->MonsterYellToMap(m_defender == TEAM_INDEX_ALLIANCE ? m_zannethGuid : m_dardoshGuid, textId, m_defender == TEAM_INDEX_ALLIANCE ? LANG_COMMON : LANG_ORCISH, NULL, ZONE_ID_WINTERGRASP);
}

void OutdoorPvPWG::OnPlayerGroupDisband(Player* plr)
{
    plr->GetSession()->SendBfLeaveMessage(BATTLEFIELD_WG, BATTLEFIELD_LEAVE_REASON_EXITED);
    SendRemoveWorldStates(plr);
    UpdateTenacities();

    if (m_playerScores.find(plr->GetObjectGuid()) != m_playerScores.end())
    {
        m_playerScores[plr->GetObjectGuid()]->removeTime = time(NULL);
        m_playerScores[plr->GetObjectGuid()]->removeDelay = WG_UNACCEPTED_REMOVE_DELAY;
    }
}

void OutdoorPvPWG::HandlePlayerAFK(Player* plr)
{
    if (m_state != WG_STATE_IN_PROGRESS || !IsMember(plr->GetObjectGuid()))
        return;

    RemovePlayerFromRaid(plr->GetObjectGuid());
    plr->GetSession()->SendBfLeaveMessage(BATTLEFIELD_WG, BATTLEFIELD_LEAVE_REASON_EXITED);
    KickPlayer(plr);
}

bool OutdoorPvPWG::CriteriaMeets(uint32 criteriaId, Player* plr)
{
    switch(criteriaId)
    {
        case WG_CRITERIA_WIN_10_MINUTES:
            return b10MinAchiev;
    }

    return false;
}

void OutdoorPvPWG::PlayerLoggedIn(Player* plr)
{
    if (Group* group = GetGroupFor(plr->GetObjectGuid()))
    {
        uint8 subgroup = group->GetMemberGroup(plr->GetObjectGuid());
        plr->SetBattleRaid(group, subgroup);
    }
}

void OutdoorPvPWG::QuestCreditTeam(uint32 credit, Team team, WorldObject* source, float radius)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if (!*itr || !(*itr)->IsInWorld() || !IsMember((*itr)->GetObjectGuid()))
            continue;

        if ((*itr)->GetTeam() != team || source && radius > 0.0f && source->GetDistance2d((*itr)->GetPositionX(), (*itr)->GetPositionY()) > radius)
            continue;

        (*itr)->KilledMonsterCredit(credit);
    }
}

std::list<std::string> OutdoorPvPWG::GetPlayerDebugInfo(Player* plr)
{
    std::list<std::string> out;
    std::stringstream ss;
    ss << "IsMember: " << IsMember(plr->GetObjectGuid());
    out.push_back(ss.str()); ss.str("");
    ss << "IsQueued: " << (m_QueuedPlayers[GetTeamIndex(plr->GetTeam())].find(plr->GetObjectGuid()) != m_QueuedPlayers[GetTeamIndex(plr->GetTeam())].end());
    out.push_back(ss.str()); ss.str("");
    ss << "IsInvited: " << (m_InvitedPlayers[GetTeamIndex(plr->GetTeam())].find(plr->GetObjectGuid()) != m_InvitedPlayers[GetTeamIndex(plr->GetTeam())].end());
    out.push_back(ss.str()); ss.str("");
    WGPlayerScoreMap::iterator itr = m_playerScores.find(plr->GetObjectGuid());
    if (itr == m_playerScores.end())
        ss << "Score: no score";
    else
    {
        ss << "Score: Rank = " << itr->second->rank << ", removeTime = ";
        if (itr->second->removeTime)
            ss << int32(time(NULL) - itr->second->removeTime) << " seconds ago";
        else
            ss << "-";
    }

    out.push_back(ss.str());

    return out;
}

std::list<std::string> OutdoorPvPWG::GetStatusDebugInfo()
{
    std::list<std::string> out;
    std::stringstream ss;
    ss << "State: " << (m_state == WG_STATE_COOLDOWN ? "Cooldown" : "In progress") << ", timer: " << uint32(m_timer/1000/60/60) << ":" <<
        (uint32(m_timer/1000/60) % 60) << ":" << (uint32(m_timer/1000) % 60);
    out.push_back(ss.str()); ss.str("");
    ss << "Raids: alliance: " << m_Raids[TEAM_INDEX_ALLIANCE].size() << " horde: " << m_Raids[TEAM_INDEX_HORDE].size();
    out.push_back(ss.str()); ss.str("");
    ss << "Alliance players: " << GetPlayerCountByTeam(TEAM_INDEX_ALLIANCE) << " queued: " << m_QueuedPlayers[TEAM_INDEX_ALLIANCE].size() << " invited: " << m_InvitedPlayers[TEAM_INDEX_ALLIANCE].size();
    out.push_back(ss.str()); ss.str("");
    ss << "Horde players: " << GetPlayerCountByTeam(TEAM_INDEX_HORDE) << " queued: " << m_QueuedPlayers[TEAM_INDEX_HORDE].size() << " invited: " << m_InvitedPlayers[TEAM_INDEX_HORDE].size();
    out.push_back(ss.str()); ss.str("");

    return out;
}

std::list<std::string> OutdoorPvPWG::GetTowerDebugInfo()
{
    std::list<std::string> out;
    std::stringstream ss;

    out.push_back("Towers:");
    for (uint8 i = 0; i < WG_TOWER_COUNT; ++i)
    {
        ss << m_towers[i]->id << ": " << m_towers[i]->guid.GetString() << " Owner: " << m_towers[i]->owner << " State: " << m_towers[i]->state;
        out.push_back(ss.str()); ss.str("");
    }
    out.push_back("Keep towers:");
    for (uint8 i = 0; i < WG_KEEPTOWER_COUNT; ++i)
    {
        ss << m_keepTowers[i]->id << ": " << m_keepTowers[i]->guid.GetString() << " Owner: " << m_keepTowers[i]->owner << " State: " << m_keepTowers[i]->state;
        out.push_back(ss.str()); ss.str("");
    }

    return out;
}

std::list<std::string> OutdoorPvPWG::GetWallDebugInfo()
{
    std::list<std::string> out;
    std::stringstream ss;

    for (std::list<WGObject*>::const_iterator itr = m_keepWalls.begin(); itr != m_keepWalls.end(); ++itr)
    {
        ss << (*itr)->id << ": " << (*itr)->guid.GetString() << " Owner: " << (*itr)->owner << " State: " << (*itr)->state;
        out.push_back(ss.str()); ss.str("");
    }
    ss << "Wall count: " << m_keepWalls.size();
    out.push_back(ss.str());

    return out;
}

