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

#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundRV.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "GameObject.h"
#include "Language.h"

BattleGroundRV::BattleGroundRV()
{
    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_15S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set messageIds
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_ARENA_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_ARENA_THIRTY_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_ARENA_FIFTEEN_SECONDS;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_ARENA_HAS_BEGUN;
}

BattleGroundRV::~BattleGroundRV()
{

}

void BattleGroundRV::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (m_uiHorizontalLosDelay)
        {
            if (m_uiHorizontalLosDelay < diff)
                m_uiHorizontalLosDelay = 0;
            else
                m_uiHorizontalLosDelay -= diff;
        }

        if (m_uiVerticalLosDelay)
        {
            if (m_uiVerticalLosDelay < diff)
                m_uiVerticalLosDelay = 0;
            else
                m_uiVerticalLosDelay -= diff;
        }

        if (m_uiPillarSwitchTimer < BG_RV_GEAR_TIME_DIFF && !m_bGearsSwitched)
        {
            m_bGearsSwitched = true;

            OpenDoorEvent(RV_EVENT_SPAWN_GEARS);
            UpdatePlayerVisibilities();
        }

        if (m_uiPillarSwitchTimer < diff)
        {
            OpenDoorEvent(RV_EVENT_SPAWN_PILLARS);
            m_bHorizontalPillarsUp = !m_bHorizontalPillarsUp;
            m_bVerticalPillarsUp = !m_bVerticalPillarsUp;

            if (!m_bHorizontalPillarsUp)
                m_uiHorizontalLosDelay = BG_RV_LOS_DELAY;

            if (!m_bVerticalPillarsUp)
                m_uiVerticalLosDelay = BG_RV_LOS_DELAY;

            UpdatePlayerVisibilities();
            m_bGearsSwitched = false;
            m_uiPillarSwitchTimer = BG_RV_PILLAR_TIMER + urand(0, BG_RV_PILLAR_TIMER_RANDOM);
        }
        else
            m_uiPillarSwitchTimer -= diff;
    }
}

void BattleGroundRV::StartingEventCloseDoors()
{
}

void BattleGroundRV::StartingEventOpenDoors()
{
    OpenDoorEvent(RV_EVENT_OP_DOOR);
}

void BattleGroundRV::AddPlayer(Player* plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundRVScore* sc = new BattleGroundRVScore;

    m_PlayerScores[plr->GetObjectGuid()] = sc;

    UpdateArenaWorldState();
}

void BattleGroundRV::RemovePlayer(Player* /*plr*/, ObjectGuid /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();

    CheckArenaWinConditions();
}

void BattleGroundRV::HandleKillPlayer(Player* player, Player* killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        ERROR_LOG("BattleGroundRV: Killer player not found");
        return;
    }

    BattleGround::HandleKillPlayer(player, killer);

    UpdateArenaWorldState();

    CheckArenaWinConditions();
}

bool BattleGroundRV::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(), 763.5f, -284.0f, 28.276f, player->GetOrientation());
    return true;
}

void BattleGroundRV::HandleAreaTrigger(Player * Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch(Trigger)
    {
        case 5224:
        case 5226:
        case 5473:
        case 5474:
            break;
        default:
            ERROR_LOG("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

void BattleGroundRV::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe1a, 1);
    UpdateArenaWorldState();
}

void BattleGroundRV::Reset()
{
    BattleGround::Reset();

    m_ActiveEvents[RV_EVENT_SPAWN_STATIC_GO] = 0;
    m_ActiveEvents[RV_EVENT_SPAWN_PILLARS] = 0;
    m_ActiveEvents[RV_EVENT_SPAWN_GEARS] = 0;

    m_uiPillarSwitchTimer = BG_RV_PILLAR_TIMER + urand(0, BG_RV_PILLAR_TIMER_RANDOM);

    m_bHorizontalPillarsUp = true;
    m_bVerticalPillarsUp = false;
    m_bGearsSwitched = false;

    m_uiVerticalLosDelay = BG_RV_LOS_DELAY;
    m_uiHorizontalLosDelay = BG_RV_LOS_DELAY;
}

bool BattleGroundRV::SetupBattleGround()
{
    return true;
}

void BattleGroundRV::OnObjectDBLoad(GameObject* go)
{
    switch (go->GetEntry())
    {
        case 208468:    // horizontal pillars
        case 208470:
        case 208469:    // vertical pillars
        case 208471:
            go->EnableModel(false);
            break;
    }

    BattleGround::OnObjectDBLoad(go);
}

bool BattleGroundRV::IsWithinLOSInBG(float x1, float y1, float z1, float x2, float y2, float z2)
{
    if (IsActiveEvent(RV_EVENT_SPAWN_PILLARS, 0))
    {
        GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(RV_EVENT_SPAWN_PILLARS, 0)].gameobjects.begin();
        for (; itr != m_EventObjects[MAKE_PAIR32(RV_EVENT_SPAWN_PILLARS, 0)].gameobjects.end(); ++itr)
        {
            if (GameObject* obj = GetBgMap()->GetGameObject(*itr))
            {
                DEBUG_LOG("Dynamic LOS: Ring of Valor: Found BG Object %s", itr->GetString().c_str());
                if (obj->GetEntry() == 208468 || obj->GetEntry() == 208470)
                {
                    if (!m_bHorizontalPillarsUp && !m_uiHorizontalLosDelay)
                        continue;
                }
                else if (obj->GetEntry() == 208469 || obj->GetEntry() == 208471)
                {
                    if (!m_bVerticalPillarsUp && !m_uiVerticalLosDelay)
                        continue;
                }
                else
                    continue;

                if (obj->BlocksLineOfSight(x1, y1, z1, x2, y2, z2))
                {
                    DEBUG_LOG("Dynamic LOS: Ring of Valor: BG Object %s blocks LoS (%f, %f, %f) - (%f, %f, %f)", itr->GetString().c_str(),
                        x1, y1, z1, x2, y2, z2);
                    return false;
                }
            }
        }
    }

    DEBUG_LOG("Dynamic LOS: Ring of Valor LOS OK");
    return true;
}

void BattleGroundRV::UpdatePlayerVisibilities()
{
    for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
    {
        if (Player* plr = sObjectMgr.GetPlayer(itr->first))
        {
            if (plr->IsInWorld())
            {
                plr->GetCamera().UpdateVisibilityForOwner();
                plr->UpdateObjectVisibility();
            }
        }
    }
}

void BattleGroundRV::CheckBuggers()
{
    if (GetStatus() == STATUS_IN_PROGRESS && GetStartTime() > 6000)
    {
        for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        {
            if (Player* plr = sObjectMgr.GetPlayer(itr->first))
            {
                if (plr->isGameMaster() || plr->IsBeingTeleported())
                    continue;

                if (plr->GetPositionZ() < 27.0f)
                    plr->TeleportTo(GetMapId(), plr->GetPositionX(), plr->GetPositionY(), 29.0f, plr->GetOrientation());
            }
        }
    }
}

void BattleGroundRV::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case BG_RV_GO_PILLAR:
        case BG_RV_GO_LIGHTNING_PILLAR:
        case BG_RV_GO_IVORY_PILLAR:
        case BG_RV_GO_AXE_PILLAR:
            go->SetManualAnim(true);
            break;
    }
}
