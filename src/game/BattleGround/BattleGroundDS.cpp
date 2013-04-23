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

#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundDS.h"
#include "ObjectMgr.h"
#include "WorldPacket.h"
#include "Language.h"
#include "Player.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"

BattleGroundDS::BattleGroundDS()
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

BattleGroundDS::~BattleGroundDS()
{
}

void BattleGroundDS::Update(uint32 diff)
{
    BattleGround::Update(diff);
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        if (m_waterfallActive && !m_collisionSpawned)
        {
            if (m_collisionSpawnTimer < diff)
            {
                m_collisionSpawnTimer = 0;
                m_collisionSpawned = true;
                SpawnEvent(DS_EVENT_WATERFALL_COLLISION, 0, true);

                m_uiWaterfallKnockback = 0;
            }
            else
                m_collisionSpawnTimer -= diff;
        }
        else
            m_collisionSpawnTimer = BG_DS_COLLISION_SPAWN_TIMER;

        // knockback
        if (m_uiPipeKnockback < diff)
        {
            if (m_pipeKnockbacks < MAX_PIPE_KNOCKBACKS)
            {
                GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(DS_EVENT_WATER_SPROUTS, 0)].creatures.begin();
                for (; itr != m_EventObjects[MAKE_PAIR32(DS_EVENT_WATER_SPROUTS, 0)].creatures.end(); ++itr)
                {
                    if (Creature* creature = GetBgMap()->GetCreature(*itr))
                        if (!IsWaterfallSprout(creature))
                            creature->CastSpell(creature, BG_DS_SPELL_FLUSH, true);
                }

                ++m_pipeKnockbacks;
            }

            // remove this when spell_dbc is implemented
            for(BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
            {
                Player * plr = sObjectMgr.GetPlayer(itr->first);
                if (plr && !plr->isDead())
                {
                    if (plr->IsWithinLOS(1214.0f, 765.0f, 14.0f) && plr->GetDistance2d(1214.0f, 765.0f) <= 40.0f)
                    {
                        plr->KnockBackWithAngle(6.2f, 35.0f, 7.0f);
                        plr->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    }

                    if (plr->IsWithinLOS(1369.0f, 817.0f, 14.0f) && plr->GetDistance2d(1369.0f, 817.0f) <= 40.0f)
                    {
                        plr->KnockBackWithAngle(3.06f, 35.0f, 7.0f);
                        plr->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    }
                }
            }

            m_uiPipeKnockback = 3000;
        }
        else
            m_uiPipeKnockback -= diff;

        if (m_collisionSpawned)
        {
            if (m_uiWaterfallKnockback < diff)
            {
                GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(DS_EVENT_WATER_SPROUTS, 0)].creatures.begin();
                for (; itr != m_EventObjects[MAKE_PAIR32(DS_EVENT_WATER_SPROUTS, 0)].creatures.end(); ++itr)
                {
                    if (Creature* creature = GetBgMap()->GetCreature(*itr))
                        if (IsWaterfallSprout(creature))
                            creature->CastSpell(creature, BG_DS_SPELL_WATER_SPOUT, true);
                }
                m_uiWaterfallKnockback = 500;
            }
            else
                m_uiWaterfallKnockback -= diff;
        }

        if (m_waterTimer < diff)
        {
            if (m_waterfallActive)
            {
                m_waterTimer = BG_DS_WATERFALL_TIMER;
                m_waterfallActive = false;
                OpenDoorEvent(DS_EVENT_WATERFALL_VISUAL);
                SpawnEvent(DS_EVENT_WATERFALL_COLLISION, 0, false);
                m_collisionSpawned = false;
            }
            else
            {
                m_waterTimer = BG_DS_WATERFALL_DURATION;
                m_waterfallActive = true;

                CloseDoorEvent(DS_EVENT_WATERFALL_VISUAL);
            }
        }
        else
            m_waterTimer -= diff;
    }
}

void BattleGroundDS::StartingEventCloseDoors()
{
}

void BattleGroundDS::StartingEventOpenDoors()
{
    OpenDoorEvent(BG_EVENT_DOOR);

    OpenDoorEvent(DS_EVENT_WATERFALL_VISUAL);

    SpawnEvent(DS_EVENT_WATER_SPROUTS, 0, true);
}

void BattleGroundDS::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);
    //create score and add it to map, default values are set in constructor
    BattleGroundDSScore* sc = new BattleGroundDSScore;

    m_PlayerScores[plr->GetObjectGuid()] = sc;

    UpdateArenaWorldState();
}

void BattleGroundDS::RemovePlayer(Player * /*plr*/, ObjectGuid /*guid*/)
{
    if (GetStatus() == STATUS_WAIT_LEAVE)
        return;

    UpdateArenaWorldState();

    CheckArenaWinConditions();
}

void BattleGroundDS::HandleKillPlayer(Player *player, Player *killer)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    if (!killer)
    {
        ERROR_LOG("BattleGroundDS: Killer player not found");
        return;
    }

    BattleGround::HandleKillPlayer(player,killer);

    UpdateArenaWorldState();

    CheckArenaWinConditions();
}

bool BattleGroundDS::HandlePlayerUnderMap(Player *player)
{
    player->TeleportTo(GetMapId(), 1299.046f, 784.825f, 9.338f, player->GetOrientation(), false);
    return true;
}

void BattleGroundDS::HandleAreaTrigger(Player *Source, uint32 Trigger)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    switch(Trigger)
    {
        case 5347:
        case 5348:
            // Remove effects of Demonic Circle Summon
            Source->RemoveAurasDueToSpell(48018);

            // Someone has get back into the pipes and the knockback has already been performed,
            // so we reset the knockback count for kicking the player again into the arena.
            m_pipeKnockbacks = 0;
            break;
        case 5326:
            HandlePlayerUnderMap(Source);
            break;
        default:
            ERROR_LOG("WARNING: Unhandled AreaTrigger in Battleground: %u", Trigger);
            Source->GetSession()->SendAreaTriggerMessage("Warning: Unhandled AreaTrigger in Battleground: %u", Trigger);
            break;
    }
}

void BattleGroundDS::FillInitialWorldStates(WorldPacket &data, uint32& count)
{
    FillInitialWorldState(data, count, 0xe1a, 1);
    UpdateArenaWorldState();
}

void BattleGroundDS::Reset()
{
    //call parent's class reset
    BattleGround::Reset();

    m_ActiveEvents[DS_EVENT_WATERFALL_VISUAL] = 0;
    m_ActiveEvents[DS_EVENT_WATERFALL_COLLISION] = 0;
    m_ActiveEvents[DS_EVENT_WATER_SPROUTS] = 0;

    m_uiPipeKnockback = 5000;
    m_uiWaterfallKnockback = 500;
    m_pipeKnockbacks = 0;

    m_waterTimer = BG_DS_WATERFALL_TIMER_START;
    m_collisionSpawnTimer = BG_DS_COLLISION_SPAWN_TIMER;

    m_waterfallActive = false;
    m_collisionSpawned = false;
}

bool BattleGroundDS::SetupBattleGround()
{
    SpawnEvent(DS_EVENT_WATERFALL_VISUAL, 0, true);
    SpawnEvent(DS_EVENT_WATERFALL_COLLISION, 0, false);
    return true;
}

void BattleGroundDS::OnObjectDBLoad(GameObject* go)
{
    switch (go->GetEntry())
    {
        case 191877:    // waterfall door and collision
        case 194395:
            go->EnableModel(false);
            break;
    }

    BattleGround::OnObjectDBLoad(go);
}

bool BattleGroundDS::IsWithinLOSInBG(float x1, float y1, float z1, float x2, float y2, float z2)
{
    if (!m_waterfallActive || !m_collisionSpawned)
    {
        DEBUG_LOG("Dynamic LOS: Waterfall is not active, nothing to check");
        return true;
    }

    if (IsActiveEvent(DS_EVENT_WATERFALL_COLLISION, 0))
    {
        if (ObjectGuid waterfallGuid = GetSingleGameObjectGuid(DS_EVENT_WATERFALL_COLLISION, 0))
        {
            if (GameObject* waterfall = GetBgMap()->GetGameObject(waterfallGuid))
            {
                DEBUG_LOG("Dynamic LOS: Dalaran Sewers: Found BG Object %s", waterfallGuid.GetString().c_str());
                if (waterfall->BlocksLineOfSight(x1, y1, z1, x2, y2, z2))
                {
                    DEBUG_LOG("Dynamic LOS: Dalaran Sewers: BG Object %s blocks LoS (%f, %f, %f) - (%f, %f, %f)", waterfallGuid.GetString().c_str(),
                        x1, y1, z1, x2, y2, z2);
                    return false;
                }
            }
        }
    }

    DEBUG_LOG("Dynamic LOS: Dalaran Sewers LOS OK");
    return true;
}

bool BattleGroundDS::IsWaterfallSprout(Creature* creature)
{
    return creature->GetPositionZ() < 10.0f;
}

void BattleGroundDS::CheckBuggers()
{
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        {
            if (Player* plr = sObjectMgr.GetPlayer(itr->first))
            {
                if (plr->isGameMaster() || plr->IsBeingTeleported())
                    continue;

                if (plr->GetPositionZ() < 2.5f ||
                    plr->GetPositionX() > 1268.86f && plr->GetPositionX() < 1314.38f &&
                    plr->GetPositionY() > 767.84f && plr->GetPositionY() < 814.05f &&
                    plr->GetPositionZ() < 5.5f)
                    HandlePlayerUnderMap(plr);
            }
        }
    }
}
