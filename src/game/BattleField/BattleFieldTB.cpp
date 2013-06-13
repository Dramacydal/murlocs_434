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

#include "BattleFieldTB.h"
#include "Chat.h"
#include "GameObject.h"
#include "MapManager.h"
#include "Unit.h"
#include "SpellAuras.h"
#include "ObjectMgr.h"

#define TB_TOWER_BONUS (sWorld.getConfig(CONFIG_UINT32_TOL_BARAD_TOWER_BONUS) * MINUTE * IN_MILLISECONDS)

BattleFieldTB::BattleFieldTB(uint32 id) : BattleField(id)
{
    m_battleFieldId = BATTLEFIELD_TB;
    m_mapId = MAP_ID_TOL_BARAD;
    m_zoneId = ZONE_ID_TOL_BARAD;

    m_battleDuration = sWorld.getConfig(CONFIG_UINT32_TOL_BARAD_BATTLE_DURATION) * MINUTE * IN_MILLISECONDS;
    m_startInviteDelay = sWorld.getConfig(CONFIG_UINT32_TOL_BARAD_START_INVITE_TIME);
    m_cooldownDuration = sWorld.getConfig(CONFIG_UINT32_TOL_BARAD_COOLDOWN_DURATION) * MINUTE * IN_MILLISECONDS;
    m_maxPlayersPerTeam = sWorld.getConfig(CONFIG_UINT32_TOL_BARAD_MAX_PLAYERS_PER_TEAM);
    m_stopTeleportingTime = sWorld.getConfig(CONFIG_UINT32_TOL_BARAD_STOP_TELEPORTING_TIME);

    m_damagedTowers = 0;
    m_destroyedTowers = 0;

    m_startTime = 0;
    bInvited = false;

    m_bases.resize(TB_BASE_COUNT);
    m_towers.resize(TB_TOWER_COUNT);
}

bool BattleFieldTB::InitOutdoorPvPArea()
{
    for (uint8 i = 0; i < TB_BASE_COUNT; ++i)
    {
        TBBase* tb = new TBBase(i, this);
        m_bases[i] = tb;
        tb->owner = m_defender;
        switch (i)
        {
            case TB_BASE_IRONCLAD_GARRISON:
                tb->worldState = m_defender == TEAM_INDEX_ALLIANCE ? TB_WS_GARRISON_ALLIANCE_CONTROLLED : TB_WS_GARRISON_HORDE_CONTROLLED;
                break;
            case TB_BASE_WARDENS_VIGIL:
                tb->worldState = m_defender == TEAM_INDEX_ALLIANCE ? TB_WS_VIGIL_ALLIANCE_CONTROLLED : TB_WS_VIGIL_HORDE_CONTROLLED;
                break;
            case TB_BASE_SLAGWORKS:
                tb->worldState = m_defender == TEAM_INDEX_ALLIANCE ? TB_WS_SLAGWORKS_ALLIANCE_CONTROLLED : TB_WS_SLAGWORKS_HORDE_CONTROLLED;
                break;
        }
    }

    for (uint8 i = 0; i < TB_TOWER_COUNT; ++i)
    {
        TBTower* t = new TBTower(i, this);
        m_towers[i] = t;
        t->owner = m_defender;
        t->state = m_defender == TEAM_INDEX_ALLIANCE ? BF_OBJECTSTATE_ALLIANCE_INTACT : BF_OBJECTSTATE_HORDE_INTACT;
    }

    float minX = -2000.0f;
    float minY = 181.0f;
    float maxX = -630.0f;
    float maxY = 1834.0f;
    float xStep = (maxX - minX) / 50.0f;
    float yStep = (maxY - minY) / 50.0f;

    DEBUG_LOG("Loading cells for Tol Barad");
    Map* map = sMapMgr.CreateMap(m_mapId, NULL);
    for (float x = minX; x <= maxX; x += xStep)
        for (float y = minY; y <= maxY; y += yStep)
            map->LoadGrid(Cell(MaNGOS::ComputeCellPair(x, y)), true);

    DEBUG_LOG("Finished loading cells for Tol Barad");

    return true;
}

BattleFieldTB::~BattleFieldTB()
{
    for (uint8 i = 0; i < TB_BASE_COUNT; ++i)
        delete m_bases[i];

    for (uint8 i = 0; i < TB_TOWER_COUNT; ++i)
        delete m_towers[i];
}

void BattleFieldTB::FillInitialWorldStates(WorldPacket& data, uint32& count, Player* player)
{
    FillInitialWorldState(data, count, TB_WS_TIME_TO_NEXT_BATTLE, m_state == BF_STATE_IN_PROGRESS ? 0 : uint32(time(NULL) + m_timer / 1000));
    FillInitialWorldState(data, count, TB_WS_TIME_TO_END, m_state != BF_STATE_IN_PROGRESS ? 0 : uint32(time(NULL) + m_timer / 1000));

    if (player->GetMapId() == m_mapId)
    {
        FillInitialWorldState(data, count, TB_WS_HORDE_CONTROLS_SHOW, m_state == BF_STATE_COOLDOWN && m_defender == TEAM_INDEX_HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
        FillInitialWorldState(data, count, TB_WS_ALLIANCE_CONTROLS_SHOW, m_state == BF_STATE_COOLDOWN && m_defender == TEAM_INDEX_ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
        FillInitialWorldState(data, count, TB_WS_ALLIANCE_ATTACKING_SHOW, m_state == BF_STATE_IN_PROGRESS && m_defender == TEAM_INDEX_HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
        FillInitialWorldState(data, count, TB_WS_HORDE_ATTACKING_SHOW, m_state == BF_STATE_IN_PROGRESS && m_defender == TEAM_INDEX_ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

        FillInitialWorldState(data, count, TB_WS_NEXT_BATTLE_TIMER_SHOW, m_state == BF_STATE_COOLDOWN ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
        FillInitialWorldState(data, count, TB_WS_TIME_REMAINING_SHOW, m_state == BF_STATE_IN_PROGRESS ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

        for (int i = 0; i < TB_BASE_COUNT; ++i)
            FillInitialWorldState(data, count, m_bases[i]->GetWorldState(), WORLD_STATE_ADD);

        for (int i = 0; i < TB_TOWER_COUNT; ++i)
            FillInitialWorldState(data, count, m_towers[i]->GetWorldState(), WORLD_STATE_ADD);
    }

    TeamIndex teamIdx = GetTeamIndex(player->GetTeam());
    FillInitialWorldState(data, count, TB_WS_TOWERS_DESTROYED_SHOW, m_state == BF_STATE_IN_PROGRESS && teamIdx == m_defender ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    FillInitialWorldState(data, count, TB_WS_BUILDINGS_CAPTURED_SHOW, m_state == BF_STATE_IN_PROGRESS && teamIdx != m_defender ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    if (m_state == BF_STATE_IN_PROGRESS)
    {
        FillInitialWorldState(data, count, TB_WS_TOWERS_DESTROYED, m_destroyedTowers);
        FillInitialWorldState(data, count, TB_WS_BUILDINGS_CAPTURED, GetCapturedBases());
    }
}

void BattleFieldTB::SendUpdateWorldStatesTo(Player* player)
{
    player->SendUpdateWorldState(TB_WS_TIME_TO_NEXT_BATTLE, m_state == BF_STATE_IN_PROGRESS ? 0 : uint32(time(NULL) + m_timer / 1000));
    player->SendUpdateWorldState(TB_WS_TIME_TO_END, m_state != BF_STATE_IN_PROGRESS ? 0 : uint32(time(NULL) + m_timer / 1000));

    player->SendUpdateWorldState(TB_WS_HORDE_CONTROLS_SHOW, m_state == BF_STATE_COOLDOWN && m_defender == TEAM_INDEX_HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(TB_WS_ALLIANCE_CONTROLS_SHOW, m_state == BF_STATE_COOLDOWN && m_defender == TEAM_INDEX_ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(TB_WS_ALLIANCE_ATTACKING_SHOW, m_state == BF_STATE_IN_PROGRESS && m_defender == TEAM_INDEX_HORDE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(TB_WS_HORDE_ATTACKING_SHOW, m_state == BF_STATE_IN_PROGRESS && m_defender == TEAM_INDEX_ALLIANCE ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    player->SendUpdateWorldState(TB_WS_NEXT_BATTLE_TIMER_SHOW, m_state == BF_STATE_COOLDOWN ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(TB_WS_TIME_REMAINING_SHOW, m_state == BF_STATE_IN_PROGRESS ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    for (int i = 0; i < TB_BASE_COUNT; ++i)
        player->SendUpdateWorldState(m_bases[i]->GetWorldState(), WORLD_STATE_ADD);

    for (int i = 0; i < TB_TOWER_COUNT; ++i)
        player->SendUpdateWorldState(m_towers[i]->GetWorldState(), WORLD_STATE_ADD);

    TeamIndex teamIdx = GetTeamIndex(player->GetTeam());
    player->SendUpdateWorldState(TB_WS_TOWERS_DESTROYED_SHOW, m_state == BF_STATE_IN_PROGRESS && teamIdx == m_defender ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(TB_WS_BUILDINGS_CAPTURED_SHOW, m_state == BF_STATE_IN_PROGRESS && teamIdx != m_defender ? WORLD_STATE_ADD : WORLD_STATE_REMOVE);

    if (m_state == BF_STATE_IN_PROGRESS)
    {
        player->SendUpdateWorldState(TB_WS_TOWERS_DESTROYED, m_destroyedTowers);
        player->SendUpdateWorldState(TB_WS_BUILDINGS_CAPTURED, GetCapturedBases());
    }
}

void BattleFieldTB::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(TB_WS_TOWERS_DESTROYED_SHOW, WORLD_STATE_REMOVE);
    pPlayer->SendUpdateWorldState(TB_WS_BUILDINGS_CAPTURED_SHOW, WORLD_STATE_REMOVE);
}

void BattleFieldTB::HandlePlayerEnterZone(Player* pPlayer, bool isMainZone)
{
    BattleField::HandlePlayerEnterZone(pPlayer, isMainZone);

    if (isMainZone)
    {
        if (m_state != BF_STATE_IN_PROGRESS)
            pPlayer->CastSpell(pPlayer, m_defender == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE_SHIFT : SPELL_HORDE_CONTROL_PHASE_SHIFT, true);
        else
        {
            UpdateTowerBuff(pPlayer);
            UpdateScoreBuff(pPlayer);
        }
    }
}

void BattleFieldTB::HandlePlayerLeaveZone(Player* pPlayer, bool isMainZone)
{
    BattleField::HandlePlayerLeaveZone(pPlayer, isMainZone);

    if (isMainZone)
    {
        pPlayer->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE_SHIFT);
        pPlayer->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE_SHIFT);
        UpdateScoreBuff(pPlayer);
    }
}

void BattleFieldTB::HandlePlayerEnterArea(Player* pPlayer, uint32 uiAreaId, bool isMainZone)
{
}

void BattleFieldTB::HandlePlayerLeaveArea(Player* pPlayer, uint32 uiAreaId, bool isMainZone)
{
}

void BattleFieldTB::HandleCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ABANDONED_SIEGE_ENGINE:
            vehicles.insert(pCreature->GetObjectGuid());
            pCreature->setFaction(BFFactions[GetAttacker()]);
            pCreature->SetPhaseMask(m_state == BF_STATE_COOLDOWN ? 2 : 1, true);
            return;
        case NPC_TOWER_RANGE_FINDER:
        case NPC_TOWER_CANNON_TARGET:
            if (pCreature->GetPositionX() > -991.0f && pCreature->GetPositionY() > 1357.0f)
            {
                m_towers[TB_TOWER_WEST]->targets.insert(pCreature->GetObjectGuid());
                pCreature->SetPhaseMask(m_towers[TB_TOWER_WEST]->IsDestroyed() ? 2 : 1, true);
            }
            else if (pCreature->GetPositionX() > -991.0f && pCreature->GetPositionY() < 654.0f)
            {
                m_towers[TB_TOWER_EAST]->targets.insert(pCreature->GetObjectGuid());
                pCreature->SetPhaseMask(m_towers[TB_TOWER_EAST]->IsDestroyed() ? 2 : 1, true);
            }
            else if (pCreature->GetPositionX() < -1150.0f)
            {
                m_towers[TB_TOWER_SOUTH]->targets.insert(pCreature->GetObjectGuid());
                pCreature->SetPhaseMask(m_towers[TB_TOWER_SOUTH]->IsDestroyed() ? 2 : 1, true);
            }

            pCreature->setFaction(BFFactions[pCreature->GetEntry() == NPC_TOWER_RANGE_FINDER ? GetAttacker() : m_defender]);
            return;
        default:
            return;
    }
}

void BattleFieldTB::_OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        default:
            return;
    }
}

void BattleFieldTB::HandleCreatureDeath(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        default:
            return;
    }
}

void BattleFieldTB::HandleGameObjectCreate(GameObject* pGo)
{
    OutdoorPvP::HandleGameObjectCreate(pGo);

    switch (pGo->GetEntry())
    {
        case GO_CAPTURE_POINT_NORTH_A_DEFENDING:
        {
            if (m_state == BF_STATE_COOLDOWN)
            {
                pGo->SetCapturePointSlider(m_bases[TB_BASE_IRONCLAD_GARRISON]->owner == TEAM_INDEX_ALLIANCE ? -CAPTURE_SLIDER_ALLIANCE : -CAPTURE_SLIDER_HORDE);
                pGo->SetLootState(GO_JUST_DEACTIVATED);
            }
            else
                pGo->SetCapturePointSlider(m_bases[TB_BASE_IRONCLAD_GARRISON]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

            m_bases[TB_BASE_IRONCLAD_GARRISON]->capturePoint = pGo->GetObjectGuid();
            break;
        }
        case GO_CAPTURE_POINT_EAST_A_DEFENDING:
        {
            if (m_state == BF_STATE_COOLDOWN)
            {
                pGo->SetCapturePointSlider(m_bases[TB_BASE_WARDENS_VIGIL]->owner == TEAM_INDEX_ALLIANCE ? -CAPTURE_SLIDER_ALLIANCE : -CAPTURE_SLIDER_HORDE);
                pGo->SetLootState(GO_JUST_DEACTIVATED);
            }
            else
                pGo->SetCapturePointSlider(m_bases[TB_BASE_WARDENS_VIGIL]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

            m_bases[TB_BASE_WARDENS_VIGIL]->capturePoint = pGo->GetObjectGuid();
            break;
        }
        case GO_CAPTURE_POINT_WEST_A_DEFENDING:
        {
            if (m_state == BF_STATE_COOLDOWN)
            {
                pGo->SetCapturePointSlider(m_bases[TB_BASE_SLAGWORKS]->owner == TEAM_INDEX_ALLIANCE ? -CAPTURE_SLIDER_ALLIANCE : -CAPTURE_SLIDER_HORDE);
                pGo->SetLootState(GO_JUST_DEACTIVATED);
            }
            else
                pGo->SetCapturePointSlider(m_bases[TB_BASE_SLAGWORKS]->owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);

            m_bases[TB_BASE_SLAGWORKS]->capturePoint = pGo->GetObjectGuid();
            break;
        }
        case GO_TOLBARAD_DOOR:
        case GO_TOLBARAD_GATES:
        case GO_GATE_TO_THE_HOLE:
        case GO_GATE_D_BLOCK:
        case GO_CURSED_DEPTHS_GATE:
            gates.insert(pGo->GetObjectGuid());
            if (m_state == BF_STATE_IN_PROGRESS)
                pGo->SetGoState(GO_STATE_READY);
            else
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_WEST_SPIRE:
            m_towers[TB_TOWER_WEST]->guid = pGo->GetObjectGuid();
            break;
        case GO_SOUTH_SPIRE:
            m_towers[TB_TOWER_SOUTH]->guid = pGo->GetObjectGuid();
            break;
        case GO_EAST_SPIRE:
            m_towers[TB_TOWER_EAST]->guid = pGo->GetObjectGuid();
            break;
        case GO_BARADINS_WARDEN_BANNER:
        case GO_HELLSCREAM_REACH_BANNER:
            // Ironclad Garrison
            if (pGo->GetPositionX() > -1007.0f && pGo->GetPositionX() < -767.0f &&
                pGo->GetPositionY() > 888.0f && pGo->GetPositionY() < 1087.0f)
            {
                m_bases[TB_BASE_IRONCLAD_GARRISON]->banners.insert(pGo->GetObjectGuid());
                if (pGo->GetEntry() == GO_BARADINS_WARDEN_BANNER)
                    pGo->SetPhaseMask(m_bases[TB_BASE_IRONCLAD_GARRISON]->owner == TEAM_INDEX_ALLIANCE ? 1 : 2, true);
                else
                    pGo->SetPhaseMask(m_bases[TB_BASE_IRONCLAD_GARRISON]->owner == TEAM_INDEX_ALLIANCE ? 2 : 1, true);
            }
            // Warden's Vigil
            else if (pGo->GetPositionX() > -1643.0f && pGo->GetPositionX() < -1353.0f &&
                pGo->GetPositionY() > 1187.0f && pGo->GetPositionY() < 1480.0f)
            {
                m_bases[TB_BASE_WARDENS_VIGIL]->banners.insert(pGo->GetObjectGuid());
                if (pGo->GetEntry() == GO_BARADINS_WARDEN_BANNER)
                    pGo->SetPhaseMask(m_bases[TB_BASE_WARDENS_VIGIL]->owner == TEAM_INDEX_ALLIANCE ? 1 : 2, true);
                else
                    pGo->SetPhaseMask(m_bases[TB_BASE_WARDENS_VIGIL]->owner == TEAM_INDEX_ALLIANCE ? 2 : 1, true);
            }
            // Slagworks
            else if (pGo->GetPositionX() > -1596.0f && pGo->GetPositionX() < -1326.0f &&
                pGo->GetPositionY() > 528.0f && pGo->GetPositionY() < 799.0f)
            {
                m_bases[TB_BASE_SLAGWORKS]->banners.insert(pGo->GetObjectGuid());
                if (pGo->GetEntry() == GO_BARADINS_WARDEN_BANNER)
                    pGo->SetPhaseMask(m_bases[TB_BASE_SLAGWORKS]->owner == TEAM_INDEX_ALLIANCE ? 1 : 2, true);
                else
                    pGo->SetPhaseMask(m_bases[TB_BASE_SLAGWORKS]->owner == TEAM_INDEX_ALLIANCE ? 2 : 1, true);
            }
            // zone
            else
            {
                banners.insert(pGo->GetObjectGuid());
                if (pGo->GetEntry() == GO_BARADINS_WARDEN_BANNER)
                    pGo->SetPhaseMask(m_defender == TEAM_INDEX_ALLIANCE ? 1 : 2, true);
                else
                    pGo->SetPhaseMask(m_defender == TEAM_INDEX_ALLIANCE ? 2 : 1, true);
            }
            break;
    }
}

// Cast player spell on opponent kill
void BattleFieldTB::HandlePlayerKillInsideArea(Player* pPlayer, Unit* pVictim)
{
    if (!pVictim || m_state != BF_STATE_IN_PROGRESS || !IsMember(pPlayer->GetObjectGuid()))
        return;

    BFPlayerScoreMap::iterator itr = m_playerScores.find(pPlayer->GetObjectGuid());
    if (itr == m_playerScores.end())
        return;

    TBPlayerScore* score = (TBPlayerScore*)itr->second;
    if (pVictim->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)pVictim)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
        if (!score->IsVeteran() && GetTeamIndex(pPlayer->GetTeam()) != m_defender)
        {
            score->SetVeteran(true);
            UpdateScoreBuff(pPlayer);
        }
    }
    else
    {
        switch (pVictim->GetEntry())
        {
            default:
                break;
        }
    }
}

uint32 baseStates[TB_BASE_COUNT][4] =
{
    { TB_WS_GARRISON_ALLIANCE_CONTROLLED, TB_WS_GARRISON_HORDE_CONTROLLED, TB_WS_GARRISON_ALLIANCE_CAPTURING, TB_WS_GARRISON_HORDE_CAPTURING },
    { TB_WS_VIGIL_ALLIANCE_CONTROLLED, TB_WS_VIGIL_HORDE_CONTROLLED, TB_WS_VIGIL_ALLIANCE_CAPTURING, TB_WS_VIGIL_HORDE_CAPTURING },
    { TB_WS_SLAGWORKS_ALLIANCE_CONTROLLED, TB_WS_SLAGWORKS_HORDE_CONTROLLED, TB_WS_SLAGWORKS_ALLIANCE_CAPTURING, TB_WS_SLAGWORKS_HORDE_CAPTURING }
};

// process the capture events
bool BattleFieldTB::HandleEvent(uint32 uiEventId, GameObject* pGo, Unit* pInvoker, uint32 spellId)
{
    if (!uiEventId)
        return false;

    if (m_state != BF_STATE_IN_PROGRESS)
        return false;

    GameObjectInfo const * info = pGo->GetGOInfo();

    int32 textId = 0;
    switch (pGo->GetEntry())
    {
        case GO_WEST_SPIRE:
        case GO_EAST_SPIRE:
        case GO_SOUTH_SPIRE:
        {
            if (!pInvoker || pInvoker->GetEntry() != NPC_SIEGE_ENGINE_TURRET)
                return false;

            uint8 idx = 0;
            switch(pGo->GetEntry())
            {
                case GO_WEST_SPIRE:
                    idx = TB_TOWER_WEST;
                    break;
                case GO_SOUTH_SPIRE:
                    idx = TB_TOWER_SOUTH;
                    break;
                case GO_EAST_SPIRE:
                    idx = TB_TOWER_EAST;
                    break;
            }

            if (uiEventId == info->destructibleBuilding.damagedEvent && !m_towers[idx]->IsDamaged())
            {
                m_towers[idx]->SetDamaged();
                m_towers[idx]->SendUpdateWorldState();
                ++m_damagedTowers;

                BuffTeam(GetTeamFromIndex(GetAttacker()), SPELL_DAMAGED_TOWER_REWARD);

                switch (idx)
                {
                    case TB_TOWER_WEST:
                        textId = LANG_TB_SPIRE_DAMAGED_W;
                        break;
                    case TB_TOWER_SOUTH:
                        textId = LANG_TB_SPIRE_DAMAGED_S;
                        break;
                    case TB_TOWER_EAST:
                        textId = LANG_TB_SPIRE_DAMAGED_E;
                        break;
                }
            }
            else if (uiEventId == info->destructibleBuilding.destroyedEvent && !m_towers[idx]->IsDestroyed())
            {
                m_towers[idx]->SetDestroyed();
                m_towers[idx]->SendUpdateWorldState();
                m_towers[idx]->SpawnTargets(false);
                UpdateTowerBuff();

                ++m_destroyedTowers;
                --m_damagedTowers;

                BuffTeam(GetTeamFromIndex(GetAttacker()), SPELL_DESTROYED_TOWER_REWARD);

                m_timer += TB_TOWER_BONUS;

                SendUpdateWorldState(TB_WS_TIME_TO_NEXT_BATTLE, m_state != BF_STATE_IN_PROGRESS ? uint32(time(NULL) + m_timer / 1000) : 0);
                SendUpdateWorldState(TB_WS_TIME_TO_END, m_state == BF_STATE_IN_PROGRESS ? uint32(time(NULL) + m_timer / 1000) : 0);
                sWorld.SendUpdateTolBaradTimerWorldState(this);

                switch (idx)
                {
                    case TB_TOWER_WEST:
                        textId = LANG_TB_SPIRE_DESTROYED_W;
                        break;
                    case TB_TOWER_SOUTH:
                        textId = LANG_TB_SPIRE_DESTROYED_S;
                        break;
                    case TB_TOWER_EAST:
                        textId = LANG_TB_SPIRE_DESTROYED_E;
                        break;
                }

                for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
                {
                    if (itr->first.IsEmpty() || !IsMember(itr->first))
                        continue;

                    Player* plr = sObjectMgr.GetPlayer(itr->first);
                    if (!plr)
                        continue;

                    if (GetTeamIndex(plr->GetTeam()) == m_defender || pGo->GetDistance2d(plr->GetPositionX(), plr->GetPositionY()) > 40.0f)
                        continue;

                    plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACH_TOWER_PLOWER, 1);
                }
            }
            break;
        }
        case GO_CAPTURE_POINT_NORTH_A_DEFENDING:
        case GO_CAPTURE_POINT_EAST_A_DEFENDING:
        case GO_CAPTURE_POINT_WEST_A_DEFENDING:
        {
            uint8 idx = 0;
            switch (pGo->GetEntry())
            {
                case GO_CAPTURE_POINT_NORTH_A_DEFENDING:
                    idx = TB_BASE_IRONCLAD_GARRISON;
                    break;
                case GO_CAPTURE_POINT_WEST_A_DEFENDING:
                    idx = TB_BASE_WARDENS_VIGIL;
                    break;
                case GO_CAPTURE_POINT_EAST_A_DEFENDING:
                    idx = TB_BASE_SLAGWORKS;
                    break;
            }

            // contested > alliance
            if (uiEventId == info->capturePoint.progressEventID1)
            {
                if (m_bases[idx]->owner != TEAM_INDEX_ALLIANCE)
                {
                    m_bases[idx]->InitFor(TEAM_INDEX_ALLIANCE, false);
                    m_bases[idx]->worldState = baseStates[idx][0];
                    m_bases[idx]->SendUpdateWorldState();
                    switch(idx)
                    {
                        case TB_BASE_IRONCLAD_GARRISON:
                            textId = LANG_TB_GARRISON_TAKEN_A;
                            break;
                        case TB_BASE_WARDENS_VIGIL:
                            textId = LANG_TB_VIGIL_TAKEN_A;
                            break;
                        case TB_BASE_SLAGWORKS:
                            textId = LANG_TB_SLAGWORKS_TAKEN_A;
                            break;
                    }
                }

                if (GetAttacker() == TEAM_INDEX_ALLIANCE && GetCapturedBases() == TB_BASE_COUNT)
                    EndBattle(GetAttacker(), false);
            }
            // contested > horde
            else if (uiEventId == info->capturePoint.progressEventID2)
            {
                if (m_bases[idx]->owner != TEAM_INDEX_HORDE)
                {
                    m_bases[idx]->InitFor(TEAM_INDEX_HORDE, false);
                    m_bases[idx]->worldState = baseStates[idx][1];
                    m_bases[idx]->SendUpdateWorldState();
                    switch(idx)
                    {
                        case TB_BASE_IRONCLAD_GARRISON:
                            textId = LANG_TB_GARRISON_TAKEN_H;
                            break;
                        case TB_BASE_WARDENS_VIGIL:
                            textId = LANG_TB_VIGIL_TAKEN_H;
                            break;
                        case TB_BASE_SLAGWORKS:
                            textId = LANG_TB_SLAGWORKS_TAKEN_H;
                            break;
                    }
                }

                if (GetAttacker() == TEAM_INDEX_HORDE && GetCapturedBases() == TB_BASE_COUNT)
                    EndBattle(GetAttacker(), false);
            }
            // horde > contested
            else if (uiEventId == info->capturePoint.neutralEventID1)
            {
                m_bases[idx]->InitFor(TEAM_INDEX_NEUTRAL, false);
                m_bases[idx]->worldState = baseStates[idx][2];
                m_bases[idx]->SendUpdateWorldState();

                //SetBannerVisual(pGo, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
                switch(idx)
                {
                    case TB_BASE_IRONCLAD_GARRISON:
                        textId = LANG_TB_GARRISON_LOST_H;
                        break;
                    case TB_BASE_WARDENS_VIGIL:
                        textId = LANG_TB_VIGIL_LOST_H;
                        break;
                    case TB_BASE_SLAGWORKS:
                        textId = LANG_TB_SLAGWORKS_LOST_H;
                        break;
                }
            }
            // ally > contested
            else if (uiEventId == info->capturePoint.neutralEventID2)
            {
                m_bases[idx]->InitFor(TEAM_INDEX_NEUTRAL, false);
                m_bases[idx]->worldState = baseStates[idx][3];
                m_bases[idx]->SendUpdateWorldState();

                //SetBannerVisual(pGo, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
                switch(idx)
                {
                    // TODO: texts
                    case TB_BASE_IRONCLAD_GARRISON:
                        textId = LANG_TB_GARRISON_LOST_A;
                        break;
                    case TB_BASE_WARDENS_VIGIL:
                        textId = LANG_TB_VIGIL_LOST_A;
                        break;
                    case TB_BASE_SLAGWORKS:
                        textId = LANG_TB_SLAGWORKS_LOST_A;
                        break;
                }
            }

            break;
        }
        default:
            return false;
    }

    if (textId)
        SendWarningToAll(textId);

    return true;
}

bool BattleFieldTB::HandleGameObjectUse(Player* pPlayer, GameObject* pGo)
{
    return false;
}

void BattleFieldTB::Update(uint32 diff)
{
    BattleField::Update(diff);

    if (m_state == BF_STATE_COOLDOWN && m_timer <= 3 * MINUTE * IN_MILLISECONDS && !bAboutSend)
    {
        bAboutSend = true;
        SendWarningToAll(LANG_TB_ABOUT_TO_BEGIN);
    }
}

void BattleFieldTB::UpdateBanners()
{
    for (GuidSet::iterator itr = banners.begin(); itr != banners.end(); ++itr)
        if (GameObject* banner = GetMap()->GetGameObject(*itr))
            if (banner->GetEntry() == GO_BARADINS_WARDEN_BANNER)
                banner->SetPhaseMask(m_defender == TEAM_INDEX_ALLIANCE ? 1 : 2, true);
            else
                banner->SetPhaseMask(m_defender == TEAM_INDEX_ALLIANCE ? 2 : 1, true);
}

void BattleFieldTB::StartBattle(TeamIndex defender)
{
    BattleField::StartBattle(defender);

    for (uint8 i = 0; i < TB_BASE_COUNT; ++i)
        m_bases[i]->InitFor(m_defender, true);

    for (uint8 i = 0; i < TB_TOWER_COUNT; ++i)
        m_towers[i]->InitFor(m_defender, true);

    for (GuidSet::iterator itr = gates.begin(); itr != gates.end(); ++itr)
        if (GameObject* gate = GetMap()->GetGameObject(*itr))
            gate->SetGoState(GO_STATE_READY);

    ResetVehicles(true);

    UpdateBanners();

    BuffTeam(TEAM_NONE, SPELL_ALLIANCE_CONTROL_PHASE_SHIFT, true, false);
    BuffTeam(TEAM_NONE, SPELL_HORDE_CONTROL_PHASE_SHIFT, true, false);

    UpdateTowerBuff();

    SendUpdateWorldStatesToAll();

    sWorld.SendUpdateTolBaradTimerWorldState(this);

    InitGraveyards();

    SendWarningToAll(LANG_TB_BATTLE_BEGIN);
}

void BattleFieldTB::EndBattle(TeamIndex winner, bool byTimer)
{
    if (m_defender == winner)
        SendWarningToAll(m_defender == TEAM_INDEX_ALLIANCE ? LANG_TB_ALLIANCE_DEFENDED : LANG_TB_HORDE_DEFENDED);
    else
        SendWarningToAll(winner == TEAM_INDEX_ALLIANCE ? LANG_TB_ALLIANCE_CAPTURED : LANG_TB_HORDE_CAPTURED);

    BattleField::EndBattle(winner, byTimer);

    for (uint8 i = 0; i < TB_BASE_COUNT; ++i)
        m_bases[i]->InitFor(m_defender, true);

    for (uint8 i = 0; i < TB_TOWER_COUNT; ++i)
        m_towers[i]->InitFor(TEAM_INDEX_NEUTRAL, false);

    for (GuidSet::iterator itr = gates.begin(); itr != gates.end(); ++itr)
        if (GameObject* gate = GetMap()->GetGameObject(*itr))
            gate->SetGoState(GO_STATE_ACTIVE);

    ResetVehicles(false);

    UpdateBanners();

    BuffTeam(TEAM_NONE, winner == TEAM_INDEX_ALLIANCE ? SPELL_ALLIANCE_CONTROL_PHASE_SHIFT : SPELL_HORDE_CONTROL_PHASE_SHIFT, false, false);
    BuffTeam(TEAM_NONE, SPELL_TOWER_ATTACK_BONUS, true, false);
    BuffTeam(TEAM_NONE, SPELL_TB_VETERAN, true, false);

    m_damagedTowers = 0;
    m_destroyedTowers = 0;

    SendUpdateWorldStatesToAll();

    InitGraveyards();

    sWorld.SendUpdateTolBaradTimerWorldState(this);
}

void BattleFieldTB::RewardPlayersAtEnd(TeamIndex winner)
{
    for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        if (!itr->first || !itr->second)
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
            continue;

        TeamIndex teamIdx = GetTeamIndex(plr->GetTeam());

        if (teamIdx == winner)
        {
            plr->CastSpell(plr, teamIdx == TEAM_INDEX_ALLIANCE ? SPELL_WIN_ALLIANCE_REWARD : SPELL_WIN_HORDE_REWARD, true);
            int32 remainingTowers = TB_TOWER_COUNT - m_destroyedTowers - m_damagedTowers;
            if (remainingTowers > 0)
                for (int i = 0; i < remainingTowers; ++i)
                    plr->CastSpell(plr, SPELL_TOWER_INTACT_REWARD, true);

            // quest credit spell
            plr->CastSpell(plr, teamIdx == TEAM_INDEX_ALLIANCE ? SPELL_TB_VICTORY_ALLIANCE : SPELL_TB_VICTORY_HORDE, true);
        }
        else
            plr->CastSpell(plr, SPELL_LOSE_REWARD, true);
    }
}

void BattleFieldTB::InitPlayerScore(Player* plr)
{
    m_playerScores[plr->GetObjectGuid()] = new TBPlayerScore();
    UpdateScoreBuff(plr);
}

bool BattleFieldTB::UpdatePlayerScores()
{
    if (BattleField::UpdatePlayerScores())
    {
        // TODO: tenacities ?
        //UpdateTenacities();
    }

    return true;
}

bool BattleFieldTB::CanDamageGO(GameObject* pGo, Player* invoker)
{
    if (m_state != BF_STATE_IN_PROGRESS)
        return false;

    if (invoker && !IsMember(invoker->GetObjectGuid()))
        return false;

    switch (pGo->GetEntry())
    {
        case GO_WEST_SPIRE:
        case GO_SOUTH_SPIRE:
        case GO_EAST_SPIRE:
            return !invoker || GetTeamIndex(invoker->GetTeam()) != m_defender;
    }

    return true;
}

bool BattleFieldTB::IsVeteran(Player* player) const
{
    if (GetTeamIndex(player->GetTeam()) == m_defender)
        return false;

    BFPlayerScoreMap::const_iterator itr = m_playerScores.find(player->GetObjectGuid());
    if (itr == m_playerScores.end())
        return false;

    return ((TBPlayerScore*)itr->second)->IsVeteran();
}

uint32 TBBase::GetWorldState() const
{
    return worldState;
}

void TBBase::SendUpdateWorldState()
{
    opvp->SendUpdateWorldState(TB_WS_BUILDINGS_CAPTURED, ((BattleFieldTB*)opvp)->GetCapturedBases());
    for (int i = 0; i < 4; ++i)
        opvp->SendUpdateWorldStateForMap(baseStates[id][i], WORLD_STATE_REMOVE, opvp->GetMap());

    opvp->SendUpdateWorldStateForMap(GetWorldState(), WORLD_STATE_ADD, opvp->GetMap());
}

void TBBase::InitFor(TeamIndex teamIdx, bool reset)
{
    owner = teamIdx;

    if (reset)
    {
        if (owner == TEAM_INDEX_ALLIANCE)
            worldState = baseStates[id][0];
        else if (owner == TEAM_INDEX_HORDE)
            worldState = baseStates[id][1];
    }

    if (GameObject* obj = opvp->GetMap()->GetGameObject(capturePoint))
    {
        if (reset)
        {
            if (opvp->GetState() == BF_STATE_COOLDOWN)
            {
                obj->SetCapturePointSlider(owner == TEAM_INDEX_ALLIANCE ? -CAPTURE_SLIDER_ALLIANCE : -CAPTURE_SLIDER_HORDE);
                obj->SetLootState(GO_JUST_DEACTIVATED);
            }
            else
                obj->SetCapturePointSlider(owner == TEAM_INDEX_ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);
        }

        if (owner == TEAM_INDEX_NEUTRAL)
            opvp->SetBannerVisual(obj, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
        else if (owner == TEAM_INDEX_ALLIANCE)
            opvp->SetBannerVisual(obj, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);
        else
            opvp->SetBannerVisual(obj, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);
    }
}

void TBTower::InitFor(TeamIndex teamIdx, bool reset)
{
    SpawnTargets(reset || teamIdx != TEAM_INDEX_NEUTRAL && !IsDestroyed());

    if (teamIdx == TEAM_INDEX_NEUTRAL && !IsDestroyed())
        teamIdx = opvp->GetDefender();

    BFObject::InitFor(teamIdx, reset);
}

void TBTower::SpawnTargets(bool spawn)
{
    for (GuidSet::iterator itr = targets.begin(); itr != targets.end(); ++itr)
    {
        if (Creature* pCreature = opvp->GetMap()->GetAnyTypeCreature(*itr))
        {
            pCreature->SetPhaseMask(spawn ? 1 : 2, true);
            if (spawn)
                pCreature->setFaction(BFFactions[pCreature->GetEntry() == NPC_TOWER_RANGE_FINDER ? opvp->GetAttacker() : opvp->GetDefender()]);
        }
    }
}

const uint32 intactStates[TB_TOWER_COUNT][3] =
{
    { TB_WS_WEST_INTACT_ALLIANCE, TB_WS_WEST_INTACT_HORDE, TB_WS_WEST_INTACT_NEUTRAL },
    { TB_WS_SOUTH_INTACT_ALLIANCE, TB_WS_SOUTH_INTACT_HORDE, TB_WS_SOUTH_INTACT_NEUTRAL },
    { TB_WS_EAST_INTACT_ALLIANCE, TB_WS_EAST_INTACT_HORDE, TB_WS_EAST_INTACT_NEUTRAL }
};

const uint32 damagedStates[TB_TOWER_COUNT][3] =
{
    { TB_WS_WEST_DAMAGED_ALLIANCE, TB_WS_WEST_DAMAGED_HORDE, TB_WS_WEST_DAMAGED_NEUTRAL },
    { TB_WS_SOUTH_DAMAGED_ALLIANCE, TB_WS_SOUTH_DAMAGED_HORDE, TB_WS_SOUTH_DAMAGED_NEUTRAL },
    { TB_WS_EAST_DAMAGED_ALLIANCE, TB_WS_EAST_DAMAGED_HORDE, TB_WS_EAST_DAMAGED_NEUTRAL }
};

const uint32 destroyedStates[TB_TOWER_COUNT] =
    { TB_WS_WEST_DESTROYED_NEUTRAL, TB_WS_SOUTH_DESTROYED_NEUTRAL, TB_WS_EAST_DESTROYED_NEUTRAL };

uint32 TBTower::GetWorldState() const
{
    if (IsIntact())
        return intactStates[id][owner];
    else if (IsDamaged())
        return damagedStates[id][owner];
    else if (IsDestroyed())
        return destroyedStates[id];

    return 0;
}

void TBTower::SendUpdateWorldState()
{
    opvp->SendUpdateWorldState(TB_WS_TOWERS_DESTROYED, ((BattleFieldTB*)opvp)->GetDestoryedTowers());
    for (int i = 0; i < 7; ++i)
        opvp->SendUpdateWorldStateForMap(wsByTower[id][i], WORLD_STATE_REMOVE, opvp->GetMap());

    opvp->SendUpdateWorldStateForMap(GetWorldState(), WORLD_STATE_ADD, opvp->GetMap());
}

/********** Grouping *************/

void BattleFieldTB::OnPlayerGroupDisband(Player* plr)
{
    BattleField::OnPlayerGroupDisband(plr);
    //UpdateTenacities();
}

bool BattleFieldTB::CriteriaMeets(uint32 criteriaId, Player* plr)
{
    switch(criteriaId)
    {
        default:
            return false;
    }

    return false;
}

bool BattleFieldTB::GetKickPosition(Player* plr, float& x, float& y, float& z)
{
    if (m_zonePlayers.find(plr->GetObjectGuid()) == m_zonePlayers.end())
        return false;

    if (GetTeamIndex(plr->GetTeam()) == TEAM_INDEX_ALLIANCE)
    {
        x = -353.840973f;
        y = 1047.824951f;
        z = 21.9298f;
    }
    else
    {
        x = -610.733948f;
        y = 1405.3396f;
        z = 21.60853f;
    }

    return true;
}

bool BattleFieldTB::OnPlayerPortResponse(Player* plr, bool accept)
{
    if (BattleField::OnPlayerPortResponse(plr, accept))
    {
        //UpdateTenacities();
        if (plr->GetZoneId() != m_zoneId)
        {
            if (GetTeamIndex(plr->GetTeam()) == m_defender)
            {
                plr->TeleportTo(MAP_ID_TOL_BARAD, -1245.703491f, 982.428467f, 155.3526f, plr->GetOrientation());
                plr->CastSpell(plr, SPELL_TB_SLOW_FALL, true);
            }
            else
                plr->TeleportTo(MAP_ID_TOL_BARAD, -841.177979f, 1188.083252f, 114.256851f, plr->GetOrientation());
        }
        else
        {
            plr->RemoveAurasDueToSpell(SPELL_ALLIANCE_CONTROL_PHASE_SHIFT);
            plr->RemoveAurasDueToSpell(SPELL_HORDE_CONTROL_PHASE_SHIFT);

            UpdateTowerBuff(plr);
            UpdateScoreBuff(plr);
        }

        plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEVEMENT_CRITERIA_CONDITION_MAP, plr->GetMapId());
        return true;
    }

    return false;
}

void BattleFieldTB::UpdateTowerBuff(Player* plr)
{
    if (plr)
    {
        plr->RemoveAurasDueToSpell(SPELL_TOWER_ATTACK_BONUS);
        if (!IsMember(plr->GetObjectGuid()) || GetTeamIndex(plr->GetTeam()) == m_defender || m_state == BF_STATE_COOLDOWN)
            return;

        for (int i = 0; i < m_destroyedTowers; ++i)
            plr->CastSpell(plr, SPELL_TOWER_ATTACK_BONUS, true);
    }
    else
    {
        for (GuidZoneMap::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
        {
            if (!itr->first)
                continue;

            Player* plr = sObjectMgr.GetPlayer(itr->first);
            if (!plr)
                continue;

            plr->RemoveAurasDueToSpell(SPELL_TOWER_ATTACK_BONUS);
            if (!IsMember(plr->GetObjectGuid()) || GetTeamIndex(plr->GetTeam()) == m_defender || !itr->second)
                continue;

            if (m_state == BF_STATE_COOLDOWN)
                continue;

            for (uint8 i = 0; i < m_destroyedTowers; ++i)
                plr->CastSpell(plr, SPELL_TOWER_ATTACK_BONUS, true);
        }
    }
}

bool BattleFieldTB::OnPlayerQueueExitRequest(Player* plr)
{
    if (BattleField::OnPlayerQueueExitRequest(plr))
    {
        //UpdateTenacities();
        return true;
    }
    return false;
}

uint32 BattleFieldTB::GetCapturedBases() const
{
    uint32 count = 0;
    for (int i = 0; i < TB_BASE_COUNT; ++i)
        if (m_bases[i]->owner == GetAttacker())
            ++count;

    return count;
}

void BattleFieldTB::UpdateScoreBuff(Player* plr)
{
    if (m_state != BF_STATE_IN_PROGRESS || !IsMember(plr->GetObjectGuid()) || plr->GetCachedZoneId() != m_zoneId || !IsVeteran(plr))
    {
        plr->RemoveAurasDueToSpell(SPELL_TB_VETERAN);
        return;
    }

    plr->CastSpell(plr, SPELL_TB_VETERAN, true);
}

uint32 attackerGraveyards[] = { TB_GY_IRONCLAD_GARRISON, TB_GY_WARDENS_VIGIL, TB_GY_SLAGWORKS };

uint32 cdGraveYards[PVP_TEAM_COUNT] = { TB_GY_ALLIANCE_DAILY, TB_GY_HORDE_DAILY };

void BattleFieldTB::InitGraveyards()
{
    sObjectMgr.SetGraveYardLinkTeam(TB_GY_BARADIN_HOLD, ZONE_ID_TOL_BARAD, GetTeamFromIndex(m_defender));

    for (int i = 0; i < 3; ++i)
        sObjectMgr.SetGraveYardLinkTeam(attackerGraveyards[i], ZONE_ID_TOL_BARAD, GetTeamFromIndex(GetAttacker()));
}

void BattleFieldTB::ResetVehicles(bool atStart)
{
    for (GuidSet::iterator itr = vehicles.begin(); itr != vehicles.end(); ++itr)
        if (Creature* vehicle = GetMap()->GetAnyTypeCreature(*itr))
        {
            vehicle->SetPhaseMask(atStart ? 1 : 2, true);
            vehicle->CastSpell(vehicle, SPELL_LEAVE_SIEGE_MODE, true);
            if (atStart)
            {
                float x, y, z, o;
                vehicle->GetRespawnCoord(x, y, z, &o);
                GetMap()->CreatureRelocation(vehicle, x, y, z, o);
                vehicle->Respawn();
                vehicle->CastSpell(vehicle, SPELL_THICK_LAYER_OF_RUST, true);
                vehicle->setFaction(BFFactions[GetAttacker()]);
            }
        }
}

bool ChatHandler::HandleTBPromoteCommand(char* args)
{
    if (!*args)
        return false;

    int32 mod;
    ExtractOptInt32(&args, mod, 0);

    Player* target = getSelectedPlayer();
    if (!target)
        target = m_session->GetPlayer();

    BattleFieldTB* opvp = (BattleFieldTB*)sOutdoorPvPMgr.GetScript(ZONE_ID_TOL_BARAD);
    if (!opvp || opvp->GetState() != BF_STATE_IN_PROGRESS)
        return false;

    BFPlayerScoreMap& playerScores = opvp->GetPlayerScoreMap();
    BFPlayerScoreMap::iterator itr = playerScores.find(target->GetObjectGuid());
    if (itr == playerScores.end())
        return true;

    TBPlayerScore* score = (TBPlayerScore*)itr->second;
    if (mod >= 0)
        score->SetVeteran(true);
    else
        score->SetVeteran(false);

    opvp->UpdateScoreBuff(target);

    return true;
}

bool ChatHandler::HandleTBTimerCommand(char* args)
{
    if (!*args)
        return false;

    int32 mod;
    if (!ExtractInt32(&args, mod))
        return false;
    mod *= IN_MILLISECONDS;

    BattleFieldTB* opvp = (BattleFieldTB*)sOutdoorPvPMgr.GetScript(ZONE_ID_TOL_BARAD);
    if (!opvp)
        return false;

    uint32 value = opvp->GetTimer();
    if (mod >= 0)
        value += mod;
    else
        if (abs(mod) >= int32(value))
            value = 0;
        else
            value += mod;

    opvp->SetTimer(value);
    sWorld.SendUpdateTolBaradTimerWorldState(opvp);

    return true;
}

bool ChatHandler::HandleTBStartCommand(char* args)
{
    BattleFieldTB* opvp = (BattleFieldTB*)sOutdoorPvPMgr.GetScript(ZONE_ID_TOL_BARAD);
    if (!opvp || opvp->GetState() != BF_STATE_COOLDOWN)
        return false;

    uint32 teamIdx;
    if (!ExtractUInt32(&args, teamIdx))
        teamIdx = uint32(opvp->GetDefender());

    opvp->StartBattle(teamIdx ? TEAM_INDEX_HORDE : TEAM_INDEX_ALLIANCE);

    return true;
}

bool ChatHandler::HandleTBEndCommand(char* args)
{
    BattleFieldTB* opvp = (BattleFieldTB*)sOutdoorPvPMgr.GetScript(ZONE_ID_TOL_BARAD);
    if (!opvp || opvp->GetState() != BF_STATE_IN_PROGRESS)
        return false;

    uint32 teamIdx;
    if (!ExtractUInt32(&args, teamIdx))
        teamIdx = uint32(opvp->GetDefender());

    opvp->EndBattle(teamIdx ? TEAM_INDEX_HORDE : TEAM_INDEX_ALLIANCE, teamIdx == uint32(opvp->GetDefender()));

    return true;
}
