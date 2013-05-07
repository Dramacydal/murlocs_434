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

#ifndef __BATTLEGROUNDSA_H
#define __BATTLEGROUNDSA_H

class BattleGround;

#define BG_SA_EVENT_START_BATTLE_1      21702       // Ally / Horde likely
#define BG_SA_EVENT_START_BATTLE_2      23748

#define BG_SA_NPC_DEMOLISHER_1          28781
#define BG_SA_NPC_DEMOLISHER_2          32796

#define BG_SA_SPELL_END_OF_ROUND        52459

#define BG_SA_ITEM_MASSIVE_SEAFORIUM_CHARGE 39213

enum BG_SA_XP_Reward
{
    BG_SA_XP_GATE_DESTROYED             = 2,
    BG_SA_XP_OCUPY_GRAVEYARD            = 2,
    BG_SA_XP_RELIC_GATE_DESTROYED       = 4,
    BG_SA_XP_WIN_ROUND                  = 4
};

enum BG_SA_WorldStates
{
    BG_SA_WS_ALLY_ATTACKS               = 4352,
    BG_SA_WS_HORDE_ATTACKS              = 4353,

    BG_SA_WS_PURPLE_GATE                = 3614,
    BG_SA_WS_RED_GATE                   = 3617,
    BG_SA_WS_BLUE_GATE                  = 3620,
    BG_SA_WS_GREEN_GATE                 = 3623,
    BG_SA_WS_YELLOW_GATE                = 3638,
    BG_SA_WS_ANCIENT_GATE               = 3849,

    BG_SA_WS_WEST_GY_ALLIANCE           = 3635,
    BG_SA_WS_EAST_GY_ALLIANCE           = 3636,
    BG_SA_WS_SOUTH_GY_ALLIANCE          = 3637,
    BG_SA_WS_EAST_GY_HORDE              = 3632,
    BG_SA_WS_WEST_GY_HORDE              = 3633,
    BG_SA_WS_SOUTH_GY_HORDE             = 3634,

    BG_SA_WS_RIGHT_ATT_TAKEN_ALLIANCE   = 3627,
    BG_SA_WS_LEFT_ATT_TAKEN_ALLIANCE    = 3626,
    BG_SA_WS_LEFT_ATT_TAKEN_HORDE       = 3629,
    BG_SA_WS_RIGHT_ATT_TAKEN_HORDE      = 3628,

    BG_SA_WS_HORDE_DEFENCE_TAKEN        = 3631,
    BG_SA_WS_ALLIANCE_DEFENCE_TAKEN     = 3630,

    BG_SA_WS_BONUS_TIMER                = 0xDF3,
    BG_SA_WS_ENABLE_TIMER               = 3564,
    BG_SA_WS_TIMER_VALUE                = 3557,
};

#define BG_SA_GATE_COUNT    6
const uint32 BG_SA_GateStatusWS[BG_SA_GATE_COUNT] =
{
    BG_SA_WS_ANCIENT_GATE,
    BG_SA_WS_GREEN_GATE,
    BG_SA_WS_BLUE_GATE,
    BG_SA_WS_PURPLE_GATE,
    BG_SA_WS_RED_GATE,
    BG_SA_WS_YELLOW_GATE
};

#define BG_SA_MAX_WS        3
const uint32 m_BG_SA_WorldStatusA[BG_SA_MAX_WS] =
{
    BG_SA_WS_ALLIANCE_DEFENCE_TAKEN,
    BG_SA_WS_LEFT_ATT_TAKEN_HORDE,
    BG_SA_WS_RIGHT_ATT_TAKEN_HORDE
};

const uint32 m_BG_SA_WorldStatusH[BG_SA_MAX_WS] =
{
    BG_SA_WS_HORDE_DEFENCE_TAKEN,
    BG_SA_WS_LEFT_ATT_TAKEN_ALLIANCE,
    BG_SA_WS_RIGHT_ATT_TAKEN_ALLIANCE
};

enum BG_SA_Sounds
{
    BG_SA_SOUND_GY_CAPTURED_ALLIANCE    = 8173,
    BG_SA_SOUND_GY_CAPTURED_HORDE       = 8213,

    BG_SA_SOUND_DEFEAT_HORDE            = 15905,
    BG_SA_SOUND_VICTORY_HORDE           = 15906,
    BG_SA_SOUND_VICTORY_ALLIANCE        = 15907,
    BG_SA_SOUND_DEFEAT_ALLIANCE         = 15908,

    BG_SA_SOUND_WALL_DESTROYED_ALLIANCE = 15909,
    BG_SA_SOUND_WALL_DESTROYED_HORDE    = 15910,
    BG_SA_SOUND_WALL_ATTACKED_HORDE     = 15911,
    BG_SA_SOUND_WALL_ATTACKED_ALLIANCE  = 15912,
};

enum BG_SA_GraveYardStatus
{
    BG_SA_GY_STATUS_ALLY_OCCUPIED    = 0,
    BG_SA_GY_STATUS_HORDE_OCCUPIED   = 1
};

enum BG_SA_GraveYard
{
    BG_SA_GY_W       = 0,
    BG_SA_GY_E       = 1,
    BG_SA_GY_S       = 2,
    BG_SA_GY_COUNT   = 3
};

static int32 BG_SA_GraveStatusWS[BG_SA_GY_COUNT][PVP_TEAM_COUNT] =
{
    { BG_SA_WS_WEST_GY_ALLIANCE,    BG_SA_WS_WEST_GY_HORDE  },
    { BG_SA_WS_EAST_GY_ALLIANCE,    BG_SA_WS_EAST_GY_HORDE  },
    { BG_SA_WS_SOUTH_GY_ALLIANCE,   BG_SA_WS_SOUTH_GY_HORDE }
};

// WorldSafeLocs ids for 3 controllable graveyards, plus for beach and relic graveyards
const uint32 BG_SA_GraveyardIdsPhase[BG_SA_GY_COUNT] = {1346, 1347, 1348};
const uint32 BG_SA_GraveyardIds[2]  = {1349, 1350}; // 0 - relic, 1 - beach

enum BG_SA_Timers
{
    BG_SA_ROUNDLENGTH           = 10 * MINUTE * IN_MILLISECONDS,
    BG_SA_BOAT_START_TIMER      = 55 * IN_MILLISECONDS,
    BG_SA_ROUND_START_DELAY     = 120 * IN_MILLISECONDS,
    BG_SA_BANNER_DELAY          = 3 * IN_MILLISECONDS,
};

enum BG_SA_GateStatus
{
    BG_SA_GATE_STATUS_NORMAL       = 1,
    BG_SA_GATE_STATUS_DAMAGED      = 2,
    BG_SA_GATE_STATUS_DESTROYED    = 3
};

enum BG_SA_GoEntry
{
    BG_SA_GO_GATE_ANCIENT_SHRINE        = 192549,
    BG_SA_GO_GATE_GREEN_EMERALD         = 190722,
    BG_SA_GO_GATE_BLUE_SAPHIRE          = 190724,
    BG_SA_GO_GATE_PURPLE_AMETHYST       = 190723,
    BG_SA_GO_GATE_RED_SUN               = 190726,
    BG_SA_GO_GATE_YELLOW_MOON           = 190727,
    BG_SA_GO_TITAN_RELIC                = 192829
};

enum BG_SA_GoType
{
    BG_SA_T_GO_GATE_ANCIENT_SHRINE          = 0,
    BG_SA_T_GO_GATE_GREEN_EMERALD           = 1,
    BG_SA_T_GO_GATE_BLUE_SAPPHIRE           = 2,
    BG_SA_T_GO_GATE_PURPLE_AMETHYST         = 3,
    BG_SA_T_GO_GATE_RED_SUN                 = 4,
    BG_SA_T_GO_GATE_YELLOW_MOON             = 5
};

enum BG_SA_Events
{
    SA_EVENT_ADD_SPIR           = 5,
    SA_EVENT_ADD_TURRETS        = 7,
    SA_EVENT_ADD_GO             = 8,

    SA_EVENT_ADD_VEH_GREEN      = 9,
    SA_EVENT_ADD_VEH_BLUE       = 10,
    SA_EVENT_ADD_VEH_BEACH_WEST = 16,
    SA_EVENT_ADD_VEH_BEACH_EAST = 17,
    SA_EVENT_ADD_GOBLIN_GREEN   = 18,
    SA_EVENT_ADD_GOBLIN_BLUE    = 19,

    SA_EVENT_ADD_BOMB_BEACH     = 6,
    SA_EVENT_ADD_BOMB_GREEN     = 11,
    SA_EVENT_ADD_BOMB_BLUE      = 12,
    SA_EVENT_ADD_BOMB_PURPLE    = 13,
    SA_EVENT_ADD_BOMB_RED       = 14,
    SA_EVENT_ADD_BOMB_YELLOW    = 15,

    SA_EVENT_ADD_SIGIL_GREEN    = 22,
    SA_EVENT_ADD_SIGIL_BLUE     = 23,
    SA_EVENT_ADD_SIGIL_PURPLE   = 24,
    SA_EVENT_ADD_SIGIL_RED      = 25,
    SA_EVENT_ADD_SIGIL_YELLOW   = 26,

    SA_EVENT_MAX                = SA_EVENT_ADD_SIGIL_YELLOW,

    SA_EVENT_OP_DOOR            = 254
};

enum BG_SA_Boat
{
    BG_SA_GO_BOAT_ONE_A = 193182,
    BG_GO_SA_BOAT_TWO_A = 193185,
    BG_SA_GO_BOAT_ONE_H = 193184,
    BG_SA_GO_BOAT_TWO_H = 193183,

    BG_SA_BOAT_COUNT    = 2,
};

enum BG_SA_NPC
{
    BG_SA_NPC_SPARKLIGHT    = 29260,
    BG_SA_NPC_RIGSPARK      = 29262,
};

struct TransOffset
{
    float x;
    float y;
    float z;
    float o;
};

const TransOffset BG_SA_Trans_Offset[2][2] = // [Faction][pos]
{
    { { 4.56318f, -0.579604f, 9.47299f, 3.08254f }, {5.57011f, -0.105833f, 9.52019f, 3.12388f } },      // Alliance East, West
    { { 8.67439f, -1.40903f, 14.4126f, 3.14625f }, { 9.46426f, -0.0947489f, 14.4819f, 3.17188f } }      // Horde East, West  
};

static float BG_SA_BOAT_LOCATIONS[2][4] =
{
    { 2679.696777f,     -826.891235f,   3.712860f,    5.78367f},    // East
    { 2574.003662f,      981.261475f,   2.603424f,   0.807696f}     // West
};

enum BG_SA_Round
{
    BG_SA_ROUND_ONE    = 0,
    BG_SA_ROUND_TWO    = 1,
    BG_SA_ROUND_COUNT  = 2
};

struct BG_SA_RoundScore
{
    Team winner;
    uint32 time;
};

class BattleGroundSAScore : public BattleGroundScore
{
    public:
        BattleGroundSAScore(): DemolishersDestroyed(0), GatesDestroyed(0) {};
        virtual ~BattleGroundSAScore() {};
        uint32 DemolishersDestroyed;
        uint32 GatesDestroyed;
};

class BattleGroundSA : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        BattleGroundSA();
        ~BattleGroundSA();
        void Update(uint32 diff) override;

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player* plr) override;
        virtual void StartingEventCloseDoors() override;
        virtual void StartingEventOpenDoors() override;
        virtual void EventPlayerDamageGO(Player* player, GameObject* target_obj, uint32 eventId, uint32 spellId = 0);
        virtual bool CanDamageGO(GameObject* target_obj, Player* invoker);
        virtual void FillInitialWorldStates(WorldPacket &data, uint32 &count) override;
        virtual void EventPlayerClickedOnFlag(Player* source, GameObject* target_obj) override;
        virtual void HandleKillUnit(Creature* unit, Player* killer) override;
        virtual WorldSafeLocsEntry const* GetClosestGraveYard(Player* player) override;
        virtual void Reset() override;

        Team GetDefender() const { return m_defender; }
        void RemovePlayer(Player* plr, ObjectGuid guid);
        void HandleAreaTrigger(Player* Source, uint32 Trigger);
        void EndBattleGround(Team winner);
        bool SetupBattleGround();

        bool isDemolisherDestroyed[PVP_TEAM_COUNT];

        /* Scorekeeping */
        void UpdatePlayerScore(Player* Source, uint32 type, uint32 value) override;

        uint32 GetRound() { return m_Round; }
        uint32 GetRoundTimer() { return m_Round_Timer; }

        void SetupPlayers();
        int32 GetGateStatus(int idx) { return m_GateStatus[idx]; }

        void HandleSummonedBomb(GameObject* obj);
        void DespawnSummonedBombs();

        void HandlePlayerLogin(Player* player);

        uint32 GetPlayerScore(Player *Source, uint32 type);

    private:
        uint8               m_GraveYard[BG_SA_GY_COUNT];
        BG_SA_RoundScore    m_RoundScores[BG_SA_ROUND_COUNT];
        Team                m_defender;
        uint32              m_Round;
        uint32              m_Round_Timer;
        uint32              m_TimerCap;
        uint32              m_SecondRoundStartDelay;
        uint32              m_spawnTurretsTimer;
        bool                m_shipsStarted;
        bool                m_relicGateDestroyed;
        uint32              m_shipsTimer;
        uint32              m_yellTimer[BG_SA_GY_COUNT - 1];
        bool                round2MessageHelper;
        bool                round2StartPreparation;
        bool                m_TimerEnabled;
        std::set<ObjectGuid> m_summonedBombs;
        time_t              m_roundStartTime;

        /* Gameobject spawning/despawning */
        uint32              m_BannerTimers[BG_SA_GY_COUNT];
        int32               m_GateStatus[BG_SA_GATE_COUNT];
        bool                m_GateAttackedSend[BG_SA_GATE_COUNT];

        void _CreateBanner(uint8 index, uint8 type, bool delay);
        void UpdateGraveYardWS(uint8 i, Team team);
        void ToggleTimer();
        void ResetWorldStates();
        void RestoreGateStatuses();

        /* This teleports player to correct loc in function of BG status and it resurects player if necesary */
        void TeleportPlayerToCorrectLoc(Player* player);

        /* For boats */
        void SendTransports(bool remove = false);
        void StartShips();

        // Send packet to player for create boats (client part)
        void SendTransportsInit(Player* player);
        // Send packet to player for destroy boats (client part)
        void SendTransportsRemove(Player* player);
        // Send packet to synchronize boat start
        void SendTransportsUpdate(Player* player);

        void SendGraveYardCaptured(uint8 graveId, Team team);
        void StartSecondRound(bool prepare);
        void ResetRound();
        bool SetupShips();
};
#endif
