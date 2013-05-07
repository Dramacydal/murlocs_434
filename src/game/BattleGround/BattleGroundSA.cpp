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

#include "GameObject.h"
#include "BattleGround.h"
#include "BattleGroundSA.h"
#include "WorldPacket.h"
#include "Language.h"
#include "Player.h"
#include "Object.h"
#include "Creature.h"
#include "BattleGroundMgr.h"
#include "Util.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "Transports.h"

/*
* BattleGround Strand of the Ancients:
* TODO:
*/

BattleGroundSA::BattleGroundSA()
{
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = LANG_BG_SA_START_TWO_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_SA_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_SA_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = 0;

    m_BgObjects.resize(BG_SA_BOAT_COUNT);
    for (uint8 i = 0; i < BG_SA_BOAT_COUNT; ++i)
        m_BgObjects[i].Clear();

    m_defender = urand(0, 1) ? ALLIANCE : HORDE;
    isDemolisherDestroyed[TEAM_INDEX_ALLIANCE] = false;
    isDemolisherDestroyed[TEAM_INDEX_HORDE]    = false;
    m_TimerEnabled = false;
    m_SecondRoundStartDelay = BG_SA_ROUND_START_DELAY + 5 * IN_MILLISECONDS;
    m_Round_Timer = 0;
    m_TimerCap = BG_SA_ROUNDLENGTH;
    m_Round = BG_SA_ROUND_ONE;

    m_relicGateDestroyed = false;
    m_shipsStarted = false;
    m_shipsTimer = BG_SA_BOAT_START_TIMER;

    round2MessageHelper = false;
    round2StartPreparation = false;

    for (int i = 0; i < 2; ++i)
        m_yellTimer[i] = 0;

    m_summonedBombs.clear();

    m_spawnTurretsTimer = 0;

    m_roundStartTime = time(NULL);
}

BattleGroundSA::~BattleGroundSA()
{
}

void BattleGroundSA::FillInitialWorldStates(WorldPacket &data, uint32 &count)
{
    FillInitialWorldState(data, count, BG_SA_WS_ALLY_ATTACKS,   GetDefender() == HORDE ? uint32(1) : uint32(0));
    FillInitialWorldState(data, count, BG_SA_WS_HORDE_ATTACKS,  GetDefender() == HORDE ? uint32(0) : uint32(1));

    for (uint8 i = 0; i < BG_SA_GATE_COUNT; ++i)
        FillInitialWorldState(data, count, BG_SA_GateStatusWS[i], m_GateStatus[i]);

    for (int8 i = 0; i < BG_SA_MAX_WS; ++i)
    {
        FillInitialWorldState(data, count, m_BG_SA_WorldStatusH[i], GetDefender() == ALLIANCE ? uint32(0) : uint32(1));
        FillInitialWorldState(data, count, m_BG_SA_WorldStatusA[i], GetDefender() == ALLIANCE ? uint32(1) : uint32(0));
    }

    for (uint32 i = 0; i < BG_SA_GY_COUNT; ++i)
    {
        FillInitialWorldState(data, count, BG_SA_GraveStatusWS[i][TEAM_INDEX_ALLIANCE], m_GraveYard[i] == BG_SA_GY_STATUS_ALLY_OCCUPIED ? uint32(1) : uint32(0));
        FillInitialWorldState(data, count, BG_SA_GraveStatusWS[i][TEAM_INDEX_HORDE], m_GraveYard[i] == BG_SA_GY_STATUS_ALLY_OCCUPIED ? uint32(0) : uint32(1));
    }

    // Time will be sent on first update...
    FillInitialWorldState(data, count, BG_SA_WS_ENABLE_TIMER,   m_TimerEnabled ? uint32(1) : uint32(0));
    FillInitialWorldState(data, count, BG_SA_WS_TIMER_VALUE,  uint32(m_roundStartTime) + m_TimerCap / IN_MILLISECONDS);
}

void BattleGroundSA::StartShips()
{
    if (m_shipsStarted)
        return;

    DEBUG_LOG("Ships started");
    DoorOpen(m_BgObjects[0]);
    DoorOpen(m_BgObjects[1]);

    for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (Player* plr = sObjectMgr.GetPlayer(itr->first))
            SendTransportsUpdate(plr);

    m_shipsStarted = true;
}

void BattleGroundSA::ToggleTimer()
{
    m_TimerEnabled = !m_TimerEnabled;
    UpdateWorldState(BG_SA_WS_ENABLE_TIMER, m_TimerEnabled ? 1 : 0);
    m_roundStartTime = time(NULL);
    UpdateWorldState(BG_SA_WS_TIMER_VALUE,  uint32(m_roundStartTime) + m_TimerCap / IN_MILLISECONDS);
}

void BattleGroundSA::EndBattleGround(Team winner)
{
    if (m_RoundScores[BG_SA_ROUND_ONE].winner == m_RoundScores[BG_SA_ROUND_TWO].winner)
        winner =  m_RoundScores[BG_SA_ROUND_ONE].winner;
    else if (m_RoundScores[BG_SA_ROUND_ONE].time > m_RoundScores[BG_SA_ROUND_TWO].time)
        winner = m_RoundScores[BG_SA_ROUND_TWO].winner;
    else 
        winner = TEAM_NONE;

    // win reward
    if (winner)
    {
        Team looser = winner == ALLIANCE ? HORDE : ALLIANCE;
        PlaySoundToTeam(winner == ALLIANCE ? BG_SA_SOUND_VICTORY_ALLIANCE : BG_SA_SOUND_VICTORY_HORDE, winner);
        PlaySoundToTeam(looser == ALLIANCE ? BG_SA_SOUND_DEFEAT_ALLIANCE : BG_SA_SOUND_DEFEAT_HORDE, looser);
        m_endSoundPlayed = true;

        RewardHonorToTeam(GetBonusHonorFromKill(1), winner);
        RewardXpToTeam(0, 0.8f, winner);
    }

    // complete map_end rewards (even if no team wins)
    RewardHonorToTeam(GetBonusHonorFromKill(2), ALLIANCE);
    RewardHonorToTeam(GetBonusHonorFromKill(2), HORDE);
    RewardXpToTeam(0, 0.8f, ALLIANCE);
    RewardXpToTeam(0, 0.8f, HORDE);
    
    BattleGround::EndBattleGround(winner);
}

void BattleGroundSA::Update(uint32 diff)
{
    BattleGround::Update(diff);

    if (GetStatus() == STATUS_WAIT_JOIN && m_Round == BG_SA_ROUND_ONE)
    {
        if (!m_shipsStarted)
            if (m_shipsTimer <= diff)
                StartShips();
            else
                m_shipsTimer -= diff;
    }

    if (GetStatus() == STATUS_IN_PROGRESS)      // Battleground already in progress
    {
        if (m_Round_Timer >= m_TimerCap)
        {
            m_RoundScores[m_Round].winner = GetDefender();
            m_RoundScores[m_Round].time = m_TimerCap;
            if (m_Round == BG_SA_ROUND_ONE)      // Timeout of first round
            {
                SendWarningToAll(LANG_BG_SA_ROUND_ONE_FINISHED);
                StartSecondRound(true);
                return;
            }
            else                                // Timeout of second round
            {
                SendMessageToAll(LANG_BG_SA_ROUND_TWO_FINISHED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                EndBattleGround(TEAM_NONE);
                return;
            }
        }
        else
            m_Round_Timer += diff;

        for (int i = 0; i < BG_SA_GY_COUNT; ++i)
        {
            // 3 sec delay to spawn new banner instead previous despawned one
            if (m_BannerTimers[i])
            {
                if (m_BannerTimers[i] > diff)
                    m_BannerTimers[i] -= diff;
                else
                {
                    m_BannerTimers[i] = 0;
                    _CreateBanner(i, m_GraveYard[i], false);
                }
            }
        }

        for (int i = 0; i < BG_SA_GY_COUNT - 1; ++i)
        {
            if (m_yellTimer[i])
            {
                if (m_yellTimer[i] < diff)
                {
                    m_yellTimer[i] = 0;
                    CreatureInfo const * cinfo = ObjectMgr::GetCreatureTemplate(i == BG_SA_GY_W ? BG_SA_NPC_RIGSPARK : BG_SA_NPC_SPARKLIGHT);
                    if (!cinfo)
                        continue;

                    GetBgMap()->MonsterYellToMap(cinfo, i == BG_SA_GY_W ? LANG_BG_SA_YELL_RIGSPARK : LANG_BG_SA_YELL_SPARKLIGHT, LANG_UNIVERSAL, NULL);
                }
                else
                    m_yellTimer[i] -= diff;
            }
        }
    }

    if (GetStatus() == STATUS_WAIT_JOIN && m_Round == BG_SA_ROUND_TWO)  // Round two, not yet started
    {
        if (!m_shipsStarted)
        {
            if (m_shipsTimer < diff)
            {
                m_shipsTimer = 0;
                SendMessageToAll(LANG_BG_SA_START_ONE_MINUTE_SECOND_ROUND, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                StartShips();
            }
            else
                m_shipsTimer -= diff;
        }

        if (m_SecondRoundStartDelay < diff)
        {
            m_SecondRoundStartDelay = 0;
            SpawnEvent(SA_EVENT_OP_DOOR, 0, false);
            SpawnEventWithRespawn(SA_EVENT_ADD_VEH_BEACH_WEST, 0, 30);
            SpawnEventWithRespawn(SA_EVENT_ADD_VEH_BEACH_EAST, 0, 30);
            SpawnEvent(SA_EVENT_ADD_BOMB_BEACH, 0, true, m_defender == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
            ToggleTimer();
            SetStatus(STATUS_IN_PROGRESS);                              // Start round two
            PlaySoundToAll(SOUND_BG_START);
            SendWarningToAll(LANG_BG_SA_HAS_BEGUN);

            StartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, BG_SA_EVENT_START_BATTLE_2, m_defender == ALLIANCE ? HORDE : ALLIANCE);

            for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
                if (Player* plr = sObjectMgr.GetPlayer(itr->first))
                    plr->RemoveAurasDueToSpell(SPELL_PREPARATION);
        }
        else
        {
            if (round2StartPreparation && m_SecondRoundStartDelay < BG_SA_ROUND_START_DELAY)
            {
                StartSecondRound(false);
                round2StartPreparation = false;
            }

            if (!round2MessageHelper && m_SecondRoundStartDelay < 30 * IN_MILLISECONDS)
            {
                SendMessageToAll(LANG_BG_SA_START_HALF_MINUTE_SECOND_ROUND, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                round2MessageHelper = true;
            }

            m_SecondRoundStartDelay -= diff;
        }
    }

    if (m_spawnTurretsTimer)
    {
        if (m_spawnTurretsTimer < diff)
        {
            m_spawnTurretsTimer = 0;
            SpawnEvent(SA_EVENT_ADD_TURRETS, 0, true);
        }
        else
            m_spawnTurretsTimer -= diff;
    }
}

void BattleGroundSA::ResetWorldStates()
{
    UpdateWorldState(BG_SA_WS_ALLY_ATTACKS, GetDefender() == HORDE ? 1 : 0);
    UpdateWorldState(BG_SA_WS_HORDE_ATTACKS, GetDefender() == HORDE ? 0 : 1);

    for (uint8 i = 0; i < BG_SA_GATE_COUNT; ++i)
        UpdateWorldState(BG_SA_GateStatusWS[i], BG_SA_GATE_STATUS_NORMAL);

    for (uint8 i = 0; i < BG_SA_GY_COUNT; ++i)
    {
        UpdateWorldState(BG_SA_GraveStatusWS[i][TEAM_INDEX_ALLIANCE], GetDefender() == ALLIANCE ? 1 : 0);
        UpdateWorldState(BG_SA_GraveStatusWS[i][TEAM_INDEX_HORDE], GetDefender() == ALLIANCE ? 0 : 1);
    }

    for (uint8 i = 0; i < BG_SA_MAX_WS; ++i)
    {
        UpdateWorldState(m_BG_SA_WorldStatusH[i], GetDefender() == ALLIANCE ? 0 : 1);
        UpdateWorldState(m_BG_SA_WorldStatusA[i], GetDefender() == ALLIANCE ? 1 : 0);
    }
}

void BattleGroundSA::StartingEventCloseDoors()
{
}

void BattleGroundSA::StartingEventOpenDoors()
{
    StartTimedAchievement(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, BG_SA_EVENT_START_BATTLE_1, m_defender == ALLIANCE ? HORDE : ALLIANCE);
    SendWarningToAll(LANG_BG_SA_HAS_BEGUN);
    SpawnEventWithRespawn(SA_EVENT_ADD_VEH_BEACH_WEST, 0, 30);
    SpawnEventWithRespawn(SA_EVENT_ADD_VEH_BEACH_EAST, 0, 30);
    SpawnEvent(SA_EVENT_ADD_BOMB_BEACH, 0, true, GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
    SpawnEvent(SA_EVENT_OP_DOOR, 0, false);
    ToggleTimer();
}

void BattleGroundSA::RemovePlayer(Player* /*plr*/, ObjectGuid /*guid*/)
{
}

void BattleGroundSA::AddPlayer(Player *plr)
{
    BattleGround::AddPlayer(plr);

    SendTransportsInit(plr);

    TeleportPlayerToCorrectLoc(plr);

    BattleGroundSAScore* sc = new BattleGroundSAScore;
    m_PlayerScores[plr->GetObjectGuid()] = sc;
}

void BattleGroundSA::HandleAreaTrigger(Player* /*Source*/, uint32 /*Trigger*/)
{
    // this is wrong way to implement these things. On official it done by gameobject spell cast.
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;
}

void BattleGroundSA::UpdatePlayerScore(Player* Source, uint32 type, uint32 value)
{
    BattleGroundScoreMap::iterator itr = m_PlayerScores.find(Source->GetGUID());
    if (itr == m_PlayerScores.end()) // player not found...
        return;

    switch(type)
    {
        case SCORE_DEMOLISHERS_DESTROYED:
            ((BattleGroundSAScore*)itr->second)->DemolishersDestroyed += value;
            break;
        case SCORE_GATES_DESTROYED:
            ((BattleGroundSAScore*)itr->second)->GatesDestroyed += value;
            break;
        default:
            BattleGround::UpdatePlayerScore(Source, type, value);
            break;
    }
}

void BattleGroundSA::StartSecondRound(bool prepare)
{
    if (prepare)
    {
        for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        {
            if (itr->second.OfflineRemoveTime)
                continue;

            Player *plr = sObjectMgr.GetPlayer(itr->first);
            if (!plr)
                continue;

            if (!plr->isAlive())
            {
                plr->ResurrectPlayer(1.0f);
                plr->SpawnCorpseBones();
            }

            plr->CastSpell(plr, BG_SA_SPELL_END_OF_ROUND, true);
            plr->DestroyItemCount(BG_SA_ITEM_MASSIVE_SEAFORIUM_CHARGE, 1, true);
        }

        m_Round = BG_SA_ROUND_TWO;
        m_relicGateDestroyed = false;
        m_shipsStarted = false;
        m_shipsTimer = BG_SA_BOAT_START_TIMER + 5 * IN_MILLISECONDS;
        SetStartTime(0);
        m_Round_Timer = 0;
        SetStatus(STATUS_WAIT_JOIN);

        round2StartPreparation = true;
    }
    else
    {
        DespawnSummonedBombs();

        ToggleTimer();
        m_defender = m_defender == ALLIANCE ? HORDE : ALLIANCE;

        ResetRound();
        ResetWorldStates();
        DEBUG_LOG("SetupShips");
        SetupShips();
        SetupPlayers();
    }
}

void BattleGroundSA::Reset()
{
    // call parent's class reset
    BattleGround::Reset();

    for (uint8 i = 0; i <= SA_EVENT_ADD_SIGIL_YELLOW; ++i)
        m_ActiveEvents[i] = BG_EVENT_NONE;
}

void BattleGroundSA::ResetRound()
{
    RestoreGateStatuses();

    if (m_Round == BG_SA_ROUND_TWO)
    {
        SpawnEvent(SA_EVENT_ADD_VEH_GREEN, 0, false);
        SpawnEvent(SA_EVENT_ADD_VEH_BLUE, 0, false);
        SpawnEvent(SA_EVENT_ADD_BOMB_BEACH, 0, false);
        SpawnEvent(SA_EVENT_ADD_VEH_BEACH_WEST, 0, false);
        SpawnEvent(SA_EVENT_ADD_VEH_BEACH_EAST, 0, false);
        SpawnEvent(SA_EVENT_ADD_GOBLIN_GREEN, 0, false);
        SpawnEvent(SA_EVENT_ADD_GOBLIN_BLUE, 0, false);
        SpawnEvent(SA_EVENT_OP_DOOR, 0, true);

        SpawnEvent(SA_EVENT_ADD_BOMB_GREEN, 0, false);
        SpawnEvent(SA_EVENT_ADD_BOMB_BLUE, 0, false);
        SpawnEvent(SA_EVENT_ADD_BOMB_PURPLE, 0, false);
        SpawnEvent(SA_EVENT_ADD_BOMB_RED, 0, false);
        SpawnEvent(SA_EVENT_ADD_BOMB_YELLOW, 0, false);

        SpawnEvent(SA_EVENT_ADD_GO, 0, false);
        SpawnEvent(SA_EVENT_ADD_TURRETS, 0, false);

        SendMessageToAll(LANG_BG_SA_START_TWO_MINUTE_SECOND_ROUND, CHAT_MSG_BG_SYSTEM_NEUTRAL);
    }

    SpawnEvent(SA_EVENT_ADD_SPIR, GetDefender() == ALLIANCE ? 0 : 1, true);

    for (uint8 i = 0; i < BG_SA_GY_COUNT; ++i)
    {
        m_BannerTimers[i] = 0;
        // Spawn Alliance or Horde banners
        m_GraveYard[i] = GetDefender() == ALLIANCE ? BG_SA_GY_STATUS_ALLY_OCCUPIED : BG_SA_GY_STATUS_HORDE_OCCUPIED;
        SpawnEvent(i, m_GraveYard[i], true);
    }

    SpawnEvent(SA_EVENT_ADD_GO, 0, true);
    for (uint8 i = 0; i < BG_SA_GATE_COUNT - 1; ++i)
        SpawnEvent(SA_EVENT_ADD_SIGIL_GREEN + i, 0, true);

    m_spawnTurretsTimer = 5000;
}

bool BattleGroundSA::SetupShips()
{
    SendTransports(true);
    for (uint8 i = 0; i < 2; ++i)
        if (GameObject* obj = GetBGObject(i))
            DelObject(i);

    uint32 boats[4] = { BG_SA_GO_BOAT_ONE_A, BG_GO_SA_BOAT_TWO_A, BG_SA_GO_BOAT_ONE_H, BG_SA_GO_BOAT_TWO_H };
    for (uint8 i = 0; i < 2; ++i)
    {
        uint8 offset = m_defender == ALLIANCE ? 2 : 0;
        if (!AddObject(i, boats[i + offset], BG_SA_BOAT_LOCATIONS[i][0], BG_SA_BOAT_LOCATIONS[i][1], BG_SA_BOAT_LOCATIONS[i][2]+ (offset ? -3.750f: 0) , BG_SA_BOAT_LOCATIONS[i][3], QuaternionData(0.0f, 0.0f, 0.0f, 0.0f), RESPAWN_ONE_DAY, true))
        {
            ERROR_LOG("SA_ERROR: Can't spawn ships!");
            return false;
        }

        if (i == 0)
            GetBGObject(i)->SetTransportPathRotation(QuaternionData(0.0f, 0.0f, 1.0f, 0.0002f));
        else
            GetBGObject(i)->SetTransportPathRotation(QuaternionData(0, 0, 1.0f, 0.00001f));

        SpawnBGObject(m_BgObjects[i], RESPAWN_IMMEDIATELY);
    }

    SendTransports();

    return true;
}

bool BattleGroundSA::SetupBattleGround()
{
    // spiritguides and flags not spawned at beginning
    ResetRound();

    if (!SetupShips())
        return false;

    return true;
}

/* type: 0 - ally occupied, 1 - horde occupied */
void BattleGroundSA::_CreateBanner(uint8 index, uint8 type, bool delay)
{
    // Just put it into the queue
    if (delay)
    {
        m_BannerTimers[index] = BG_SA_BANNER_DELAY;
        return;
    }

    // cause the node-type is in the generic form
    // please see in the headerfile for the ids

    SpawnEvent(index, type, true); // will automaticly despawn other events
}

void BattleGroundSA::EventPlayerClickedOnFlag(Player* source, GameObject* target_obj)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return;

    uint8 event = (sBattleGroundMgr.GetGameObjectEventIndex(target_obj->GetGUIDLow())).event1;
    if (event >= BG_SA_GY_COUNT) // not a node
        return;
    BG_SA_GraveYard graveIndex = BG_SA_GraveYard(event);

    switch(graveIndex)
    {
        case BG_SA_GY_W:
        case BG_SA_GY_E:
        {
            if (m_GateStatus[BG_SA_T_GO_GATE_GREEN_EMERALD] != BG_SA_GATE_STATUS_DESTROYED && 
                m_GateStatus[BG_SA_T_GO_GATE_BLUE_SAPPHIRE] != BG_SA_GATE_STATUS_DESTROYED)
                return;
            break;
        }
        case BG_SA_GY_S:
        {
            if (m_GateStatus[BG_SA_T_GO_GATE_RED_SUN] != BG_SA_GATE_STATUS_DESTROYED && 
                m_GateStatus[BG_SA_T_GO_GATE_PURPLE_AMETHYST] != BG_SA_GATE_STATUS_DESTROYED)
                return;
            break;
        }
    }

    TeamIndex teamIndex = GetTeamIndex(source->GetTeam());

    // Check if player really could use this banner, not cheated
    if (m_GraveYard[graveIndex] == teamIndex)
        return;

    // If node is occupied, change to enemy-occupied
    if (GetTeamIndex(m_defender) == m_GraveYard[graveIndex])
    {
        m_GraveYard[graveIndex] = teamIndex;
        // create new contested banner
        _CreateBanner(graveIndex, teamIndex, true);
        UpdateGraveYardWS(graveIndex, teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);

        uint32 langId;
        switch(graveIndex)
        {
            case BG_SA_GY_W:
            {
                langId = teamIndex == TEAM_INDEX_ALLIANCE ? LANG_BG_SA_ALLIANCE_WEST_CONQUESTED : LANG_BG_SA_HORDE_WEST_CONQUESTED;
                SpawnEventWithRespawn(SA_EVENT_ADD_VEH_GREEN, 0, 30);
                SetEventRespawn(SA_EVENT_ADD_VEH_BEACH_WEST, 0, RESPAWN_IMMEDIATELY);
                SpawnEvent(SA_EVENT_ADD_GOBLIN_GREEN, 0, true);
                m_yellTimer[BG_SA_GY_W] = 10 * IN_MILLISECONDS;
                break;
            }
            case BG_SA_GY_E:
            {
                langId = teamIndex == TEAM_INDEX_ALLIANCE ? LANG_BG_SA_ALLIANCE_EAST_CONQUESTED : LANG_BG_SA_HORDE_EAST_CONQUESTED;
                SpawnEventWithRespawn(SA_EVENT_ADD_VEH_BLUE, 0, 30);
                SetEventRespawn(SA_EVENT_ADD_VEH_BEACH_EAST, 0, RESPAWN_IMMEDIATELY);
                SpawnEvent(SA_EVENT_ADD_GOBLIN_BLUE, 0, true);
                m_yellTimer[BG_SA_GY_E] = 10 * IN_MILLISECONDS;
                break;
            }
            case BG_SA_GY_S:
            {
                langId = teamIndex == TEAM_INDEX_ALLIANCE ? LANG_BG_SA_ALLIANCE_SOUTH_CONQUESTED : LANG_BG_SA_HORDE_SOUTH_CONQUESTED;
                break;
            }
        }

        SendWarningToAll(langId);
        PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_GY_CAPTURED_ALLIANCE : BG_SA_SOUND_GY_CAPTURED_HORDE);
    }
}

void BattleGroundSA::EventPlayerDamageGO(Player* player, GameObject* target_obj, uint32 eventId, uint32 spellId)
{
    TeamIndex teamIndex = GetTeamIndex(player->GetTeam());

    uint32 type;
    switch (target_obj->GetEntry())
    {
        case BG_SA_GO_GATE_ANCIENT_SHRINE:
        {
            type = BG_SA_T_GO_GATE_ANCIENT_SHRINE;
            switch (eventId)
            {
                case 21630:             // Attacked
                    if (!m_GateAttackedSend[type])
                    {
                        SendWarningToAll(LANG_BG_SA_GATE_ROOM_ANCIENT_SHRINE_ATTACKED);
                        PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_ATTACKED_ALLIANCE : BG_SA_SOUND_WALL_ATTACKED_HORDE);
                        m_GateAttackedSend[type] = true;
                    }
                    break;
                case 19836:             // Damaged
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DAMAGED);
                    break;
                case 19837:             // Destroyed
                    SendWarningToAll(LANG_BG_SA_GATE_ROOM_ANCIENT_SHRINE_DESTROYED);
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DESTROYED);
                    UpdatePlayerScore(player, SCORE_GATES_DESTROYED, 1);
                    RewardHonorToTeam(GetBonusHonorFromKill(BG_SA_XP_RELIC_GATE_DESTROYED), teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);
                    PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_DESTROYED_ALLIANCE : BG_SA_SOUND_WALL_DESTROYED_HORDE);
                    m_relicGateDestroyed = true;
                    break;
            }
            break;
        }
        case BG_SA_GO_GATE_GREEN_EMERALD:
        {
            type = BG_SA_T_GO_GATE_GREEN_EMERALD;
            switch (eventId)
            {
                case 21630:             // Attacked
                    if (!m_GateAttackedSend[type])
                    {
                        SendWarningToAll(LANG_BG_SA_GATE_GREEN_EMERALD_ATTACKED);
                        PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_ATTACKED_ALLIANCE : BG_SA_SOUND_WALL_ATTACKED_HORDE);
                        m_GateAttackedSend[type] = true;
                    }
                    break;
                case 19041:             // Damaged
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DAMAGED);
                    break;
                case 19046:             // Destroyed
                    SendWarningToAll(LANG_BG_SA_GATE_GREEN_EMERALD_DESTROYED);
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DESTROYED);
                    UpdatePlayerScore(player, SCORE_GATES_DESTROYED, 1);
                    RewardHonorToTeam(GetBonusHonorFromKill(BG_SA_XP_GATE_DESTROYED), teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);
                    SpawnEvent(SA_EVENT_ADD_BOMB_GREEN, 0, true, GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
                    PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_DESTROYED_ALLIANCE : BG_SA_SOUND_WALL_DESTROYED_HORDE);
                    SpawnEvent(SA_EVENT_ADD_SIGIL_GREEN, 0, false);
                    break;
            }
            break;
        }
        case BG_SA_GO_GATE_BLUE_SAPHIRE:
        {
            type = BG_SA_T_GO_GATE_BLUE_SAPPHIRE;
            switch (eventId)
            {
                case 21630:             // Attacked
                    if (!m_GateAttackedSend[type])
                    {
                        SendWarningToAll(LANG_BG_SA_GATE_BLUE_SAPHIRE_ATTACKED);
                        PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_ATTACKED_ALLIANCE : BG_SA_SOUND_WALL_ATTACKED_HORDE);
                        m_GateAttackedSend[type] = true;
                    }
                    break;
                case 19040:             // Damaged
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DAMAGED);
                    break;
                case 19045:             // Destroyed
                    SendWarningToAll(LANG_BG_SA_GATE_BLUE_SAPHIRE_DESTROYED);
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DESTROYED);
                    UpdatePlayerScore(player, SCORE_GATES_DESTROYED, 1);
                    RewardHonorToTeam(GetBonusHonorFromKill(BG_SA_XP_GATE_DESTROYED), teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);
                    SpawnEvent(SA_EVENT_ADD_BOMB_BLUE, 0, true, GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
                    PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_DESTROYED_ALLIANCE : BG_SA_SOUND_WALL_DESTROYED_HORDE);
                    SpawnEvent(SA_EVENT_ADD_SIGIL_BLUE, 0, false);
                    break;
            }
            break;
        }
        case BG_SA_GO_GATE_PURPLE_AMETHYST:
        {
            type = BG_SA_T_GO_GATE_PURPLE_AMETHYST;
            switch (eventId)
            {
                case 21630:             // Attacked
                    if (!m_GateAttackedSend[type])
                    {
                        SendWarningToAll(LANG_BG_SA_GATE_PURPLE_AMETHYST_ATTACKED);
                        PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_ATTACKED_ALLIANCE : BG_SA_SOUND_WALL_ATTACKED_HORDE);
                        m_GateAttackedSend[type] = true;
                    }
                    break;
                case 19043:             // Damaged
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DAMAGED);
                    break;
                case 19048:             // Destroyed
                    SendWarningToAll(LANG_BG_SA_GATE_PURPLE_AMETHYST_DESTROYED);
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DESTROYED);
                    UpdatePlayerScore(player, SCORE_GATES_DESTROYED, 1);
                    RewardHonorToTeam(GetBonusHonorFromKill(BG_SA_XP_GATE_DESTROYED), teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);
                    SpawnEvent(SA_EVENT_ADD_BOMB_PURPLE, 0, true, GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
                    PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_DESTROYED_ALLIANCE : BG_SA_SOUND_WALL_DESTROYED_HORDE);
                    SpawnEvent(SA_EVENT_ADD_SIGIL_PURPLE, 0, false);
                    break;
            }
            break;
        }
        case BG_SA_GO_GATE_RED_SUN:
        {
            type = BG_SA_T_GO_GATE_RED_SUN;
            switch (eventId)
            {
                case 21630:             // Attacked
                    if (!m_GateAttackedSend[type])
                    {
                        SendWarningToAll(LANG_BG_SA_GATE_RED_SUN_ATTACKED);
                        PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_ATTACKED_ALLIANCE : BG_SA_SOUND_WALL_ATTACKED_HORDE);
                        m_GateAttackedSend[type] = true;
                    }
                    break;
                case 19042:             // Damaged
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DAMAGED);
                    break;
                case 19047:             // Destroyed
                    SendWarningToAll(LANG_BG_SA_GATE_RED_SUN_DESTROYED);
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DESTROYED);
                    UpdatePlayerScore(player, SCORE_GATES_DESTROYED, 1);
                    RewardHonorToTeam(GetBonusHonorFromKill(BG_SA_XP_GATE_DESTROYED), teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);
                    SpawnEvent(SA_EVENT_ADD_BOMB_RED, 0, true, GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
                    PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_DESTROYED_ALLIANCE : BG_SA_SOUND_WALL_DESTROYED_HORDE);
                    SpawnEvent(SA_EVENT_ADD_SIGIL_RED, 0, false);
                    break;
            }
            break;
        }
        case BG_SA_GO_GATE_YELLOW_MOON:
        {
            type = BG_SA_T_GO_GATE_YELLOW_MOON;
            switch (eventId)
            {
                case 21630:             // Attacked
                    if (!m_GateAttackedSend[type])
                    {
                        SendWarningToAll(LANG_BG_SA_GATE_YELLOW_MOON_ATTACKED);
                        PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_ATTACKED_ALLIANCE : BG_SA_SOUND_WALL_ATTACKED_HORDE);
                        m_GateAttackedSend[type] = true;
                    }
                    break;
                case 19044:             // Damaged
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DAMAGED);
                    break;
                case 19049:             // Destroyed
                    SendWarningToAll(LANG_BG_SA_GATE_YELLOW_MOON_DESTROYED);
                    UpdateWorldState(BG_SA_GateStatusWS[type], m_GateStatus[type] = BG_SA_GATE_STATUS_DESTROYED);
                    UpdatePlayerScore(player, SCORE_GATES_DESTROYED, 1);
                    RewardHonorToTeam(GetBonusHonorFromKill(BG_SA_XP_GATE_DESTROYED), teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);
                    SpawnEvent(SA_EVENT_ADD_BOMB_YELLOW, 0, true, GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
                    PlaySoundToAll(teamIndex == TEAM_INDEX_ALLIANCE ? BG_SA_SOUND_WALL_DESTROYED_ALLIANCE : BG_SA_SOUND_WALL_DESTROYED_HORDE);
                    SpawnEvent(SA_EVENT_ADD_SIGIL_YELLOW, 0, false);
                    break;
            }
            break;
        }
        case BG_SA_GO_TITAN_RELIC:
        {
            if (eventId == 22097 && player->GetTeam() != GetDefender())
            {
                if (!m_relicGateDestroyed || GetStatus() != STATUS_IN_PROGRESS)
                {
                    player->GetSession()->KickPlayer();
                    ERROR_LOG("Player %s has clicked SOTA Relic without Relic gate being destroyed", player->GetGuidStr().c_str());
                    return;
                }

                m_RoundScores[m_Round].winner = GetDefender() == ALLIANCE ? HORDE : ALLIANCE;
                m_RoundScores[m_Round].time = m_Round_Timer;
                RewardHonorToTeam(GetBonusHonorFromKill(BG_SA_XP_WIN_ROUND), teamIndex == TEAM_INDEX_ALLIANCE ? ALLIANCE : HORDE);
                SendMessageToAll(m_defender == HORDE ? LANG_BG_SA_TITAN_PORTAL_CAPTURED_ALLIANCE : LANG_BG_SA_TITAN_PORTAL_CAPTURED_HORDE, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                UpdateAchievementCriteriaOnTeam(m_RoundScores[m_Round].winner, ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 65246);
                if (m_Round == BG_SA_ROUND_ONE) // Victory at first round
                {
                    m_TimerCap = m_Round_Timer;
                    SendWarningToAll(LANG_BG_SA_ROUND_ONE_FINISHED);
                    StartSecondRound(true);
                    return;
                }
                else                            // Victory at second round
                {
                    UpdateAchievementCriteriaOnTeam(ALLIANCE, ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, BG_SA_SPELL_END_OF_ROUND, 1);
                    UpdateAchievementCriteriaOnTeam(HORDE, ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, BG_SA_SPELL_END_OF_ROUND, 1);

                    SendMessageToAll(LANG_BG_SA_ROUND_TWO_FINISHED, CHAT_MSG_BG_SYSTEM_NEUTRAL);
                    EndBattleGround(TEAM_NONE);
                    return;
                }
            }
            break;
        }
    }

    // The Dapper Sapper achievement
    if (target_obj->GetEntry() != BG_SA_GO_TITAN_RELIC && !player->GetVehicle())
        player->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 60937, 1);
}

void BattleGroundSA::HandleKillUnit(Creature* unit, Player* killer)
{
    if (!unit)
        return;

    // Demolisher
    if (unit->GetEntry() == BG_SA_NPC_DEMOLISHER_1 || unit->GetEntry() == BG_SA_NPC_DEMOLISHER_2)
    {
        UpdatePlayerScore(killer, SCORE_DEMOLISHERS_DESTROYED, 1);
        isDemolisherDestroyed[m_defender == ALLIANCE ? TEAM_INDEX_HORDE : TEAM_INDEX_ALLIANCE] = true;
    }
}

WorldSafeLocsEntry const* BattleGroundSA::GetClosestGraveYard(Player* player)
{
    TeamIndex teamIndex = GetTeamIndex(player->GetTeam());

    // Is there any occupied node for this team?
    std::vector<uint8> ids;
    for (uint8 i = 0; i < BG_SA_GY_COUNT; ++i)
        if (m_GraveYard[i] == BG_SA_GY_STATUS_ALLY_OCCUPIED && teamIndex == TEAM_INDEX_ALLIANCE ||
            m_GraveYard[i] == BG_SA_GY_STATUS_HORDE_OCCUPIED && teamIndex == TEAM_INDEX_HORDE)
            ids.push_back(i);

    WorldSafeLocsEntry const* good_entry = NULL;
    // If so, select the closest node to place ghost on
    if (!ids.empty())
    {
        float mindist = 999999.0f;
        for (uint8 i = 0; i < ids.size(); ++i)
        {
            WorldSafeLocsEntry const* entry = sWorldSafeLocsStore.LookupEntry(BG_SA_GraveyardIdsPhase[ids[i]]);
            if (!entry)
                continue;

            float dist = player->GetDistance2d(entry->x, entry->y);
            if (dist < mindist)
            {
                mindist = dist;
                good_entry = entry;
            }
        }

        ids.clear();
        if (good_entry)
            return good_entry;
    }

    // If no graveyards occupied, place ghost on starting location
    if (GetTeamIndex(GetDefender()) == teamIndex)
        return sWorldSafeLocsStore.LookupEntry(BG_SA_GraveyardIds[0]);
    else
        return sWorldSafeLocsStore.LookupEntry(BG_SA_GraveyardIds[1]);
}

void BattleGroundSA::UpdateGraveYardWS(uint8 i, Team team)
{
    if (i < BG_SA_GY_COUNT)
    {
        UpdateWorldState(BG_SA_GraveStatusWS[i][TEAM_INDEX_ALLIANCE], team == ALLIANCE ? 1 : 0);
        UpdateWorldState(BG_SA_GraveStatusWS[i][TEAM_INDEX_HORDE], team == ALLIANCE ? 0 : 1);
    }
}

void BattleGroundSA::TeleportPlayerToCorrectLoc(Player* plr)
{
    if (!plr)
        return;

    if (plr->GetTeam() == GetDefender())
        plr->TeleportTo(607, 1209.7f, -65.16f, 70.1f, 0.0f);
    else if (!m_shipsStarted)
    {
        uint8 idx = urand(0, 1);
        /*if (GameObject* obj = GetBGObject(idx))
        {
            float xOffset = BG_SA_Trans_Offset[m_defender == ALLIANCE ? 1 : 0][idx].x;
            float yOffset = BG_SA_Trans_Offset[m_defender == ALLIANCE ? 1 : 0][idx].y;
            float zOffset = BG_SA_Trans_Offset[m_defender == ALLIANCE ? 1 : 0][idx].z;
            float oOffset = BG_SA_Trans_Offset[m_defender == ALLIANCE ? 1 : 0][idx].o;

            plr->m_movementInfo.SetTransportData(obj->GetObjectGuid(), xOffset, yOffset, zOffset, oOffset, 0, -1);
            plr->TeleportTo(607, obj->GetPositionX() + xOffset, obj->GetPositionY() + yOffset, obj->GetPositionZ() + zOffset,
                obj->GetOrientation() + oOffset, TELE_TO_NOT_LEAVE_TRANSPORT);
        }
        else*/ if (idx == 0)
            plr->TeleportTo(607, 2682.936f, -830.368f, 30.0f, 2.895f);
        else
            plr->TeleportTo(607, 2580.96f, 984.56f, 30.0f, 0.807f);
    }
    else
        plr->TeleportTo(607, 1600.381f, -106.263f, 8.8745f, 3.78f);
}

void BattleGroundSA::SetupPlayers()
{
    for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
    {
        if (Player *plr = sObjectMgr.GetPlayer(itr->first))
        {
            TeleportPlayerToCorrectLoc(plr);

            if (m_Round == BG_SA_ROUND_TWO && !itr->second.OfflineRemoveTime)
                plr->CastSpell(plr, SPELL_PREPARATION, true);
        }
    }
}

void BattleGroundSA::SendTransports(bool remove)
{
    for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (Player *plr = sObjectMgr.GetPlayer(itr->first))
        {
            if (remove)
                SendTransportsRemove(plr);
            else
                SendTransportsInit(plr);
        }
}

void BattleGroundSA::SendTransportsInit(Player* player)
{
    UpdateData transData(player->GetMapId());
    for (uint8 i = 0; i < 2; ++i)
    {
        if (GameObject* obj = GetBGObject(i))
        {
            DEBUG_LOG("SendTransportsInit for %s for player %s", obj->GetGuidStr().c_str(), player->GetGuidStr().c_str());
            obj->BuildCreateUpdateBlockForPlayer(&transData, player);
        }
        else
            DEBUG_LOG("SendTransportsInit for %u for player %s NOT FOUND!!!", i, player->GetGuidStr().c_str());
    }

    if (!transData.HasData())
        return;

    WorldPacket packet;
    transData.BuildPacket(&packet);
    player->SendDirectMessage(&packet);
}

void BattleGroundSA::SendTransportsRemove(Player* player)
{
    UpdateData transData(player->GetMapId());
    for (uint8 i = 0; i < 2; ++i)
    {
        if (GameObject* obj = GetBGObject(i))
        {
            DEBUG_LOG("SendTransportsRemove for %s for player %s", obj->GetGuidStr().c_str(), player->GetGuidStr().c_str());
            obj->BuildOutOfRangeUpdateBlock(&transData);
        }
        else
            DEBUG_LOG("SendTransportsRemove for %u for player %s NOT FOUND!!!", i, player->GetGuidStr().c_str());
    }

    if (!transData.HasData())
        return;

    WorldPacket packet;
    transData.BuildPacket(&packet);
    player->SendDirectMessage(&packet);
}

void BattleGroundSA::SendTransportsUpdate(Player* player)
{
    UpdateData transData(player->GetMapId());
    for (uint8 i = 0; i < 2; ++i)
    {
        if (GameObject* obj = GetBGObject(i))
        {
            DEBUG_LOG("SendTransportsUpdate for %s for player %s", obj->GetGuidStr().c_str(), player->GetGuidStr().c_str());
            obj->BuildValuesUpdateBlockForPlayer(&transData, player);
        }
        else
            DEBUG_LOG("SendTransportsUpdate for %u for player %s NOT FOUND!!!", i, player->GetGuidStr().c_str());
    }

    if (!transData.HasData())
        return;

    WorldPacket packet;
    transData.BuildPacket(&packet);
    player->SendDirectMessage(&packet);
}

void BattleGroundSA::RestoreGateStatuses()
{
    for (int32 i = 0; i < BG_SA_GATE_COUNT; ++i)
    {
        m_GateStatus[i] = BG_SA_GATE_STATUS_NORMAL;
        m_GateAttackedSend[i] = false;
    }
}

bool BattleGroundSA::CanDamageGO(GameObject* target_obj, Player* invoker)
{
    if (GetStatus() != STATUS_IN_PROGRESS)
        return false;

    if (invoker->GetTeam() == m_defender)
        return false;

    switch (target_obj->GetEntry())
    {
        case BG_SA_GO_GATE_ANCIENT_SHRINE:
            return m_GateStatus[BG_SA_T_GO_GATE_YELLOW_MOON] == BG_SA_GATE_STATUS_DESTROYED;
        case BG_SA_GO_GATE_YELLOW_MOON:
            return m_GateStatus[BG_SA_T_GO_GATE_RED_SUN] == BG_SA_GATE_STATUS_DESTROYED || m_GateStatus[BG_SA_T_GO_GATE_PURPLE_AMETHYST] == BG_SA_GATE_STATUS_DESTROYED;
        case BG_SA_GO_GATE_RED_SUN:
        case BG_SA_GO_GATE_PURPLE_AMETHYST:
            return m_GateStatus[BG_SA_T_GO_GATE_GREEN_EMERALD] == BG_SA_GATE_STATUS_DESTROYED || m_GateStatus[BG_SA_T_GO_GATE_BLUE_SAPPHIRE] == BG_SA_GATE_STATUS_DESTROYED;
        case BG_SA_GO_GATE_GREEN_EMERALD:
        case BG_SA_GO_GATE_BLUE_SAPHIRE:
            return true;
    }

    return true;
}

void BattleGroundSA::HandleSummonedBomb(GameObject* obj)
{
    m_summonedBombs.insert(obj->GetObjectGuid());

    obj->SetUInt32Value(GAMEOBJECT_FACTION, GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
}

void BattleGroundSA::DespawnSummonedBombs()
{
    if (m_summonedBombs.empty())
        return;

    for (std::set<ObjectGuid>::const_iterator itr = m_summonedBombs.begin(); itr != m_summonedBombs.end(); ++itr)
        if (GameObject* pBomb = GetBgMap()->GetGameObject(*itr))
            pBomb->Delete();

    m_summonedBombs.clear();
}

void BattleGroundSA::HandlePlayerLogin(Player* player)
{
    SendTransportsInit(player);
}

uint32 BattleGroundSA::GetPlayerScore(Player *Source, uint32 type)
{
    BattleGroundScoreMap::const_iterator itr = m_PlayerScores.find(Source->GetObjectGuid());

    if (itr == m_PlayerScores.end())                         // player not found...
        return 0;

    switch(type)
    {
        case SCORE_GATES_DESTROYED:
            return ((BattleGroundSAScore*)itr->second)->DemolishersDestroyed;
        case SCORE_DEMOLISHERS_DESTROYED:
            return ((BattleGroundSAScore*)itr->second)->GatesDestroyed;
    }

    return BattleGround::GetPlayerScore(Source, type);
}

