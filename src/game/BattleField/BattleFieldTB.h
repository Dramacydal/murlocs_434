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


#ifndef WORLD_PVP_TB
#define WORLD_PVP_TB

#include "Common.h"
#include "BattleField.h"
#include "Language.h"
#include "World.h"

class Group;
class BattleFieldTB;

enum TolBaradSpells
{
    SPELL_THICK_LAYER_OF_RUST                   = 95330,

    SPELL_WIN_ALLIANCE_REWARD                   = 89789,
    SPELL_WIN_HORDE_REWARD                      = 89791,
    SPELL_LOSE_REWARD                           = 89793,

    SPELL_DAMAGED_TOWER_REWARD                  = 89795,
    SPELL_DESTROYED_TOWER_REWARD                = 89796,
    SPELL_TOWER_INTACT_REWARD                   = 89794,

    SPELL_TOWER_ATTACK_BONUS                    = 82629,

    SPELL_TB_VETERAN                            = 84655,
    SPELL_TB_SLOW_FALL                          = 88473,

    SPELL_TB_VICTORY_ALLIANCE                   = 94665,
    SPELL_TB_VICTORY_HORDE                      = 94763,

    SPELL_RIDE_TOL_BARAD_VEHICLE                = 84754,
    SPELL_SIEGE_CANNON_PERIODIC                 = 85167,
    SPELL_SIEGE_CANNON                          = 85122,
    SPELL_DEPLOY_SIEGE_MODE                     = 84974,
    SPELL_TOWER_RANGE_FINDER                    = 84979,
    SPELL_TOWER_RANGE_FINDER_PERIODIC           = 85671,
    SPELL_LEAVE_SIEGE_MODE                      = 85078,

    SPELL_TB_SPIRITUAL_IMMUNITY                 = 95332,

    SPELL_ACH_TOWER_PLOWER                      = 95765,
};

enum
{
    MAP_ID_TOL_BARAD                            = 732,
    AREA_ID_BARADIN_HOLD                        = 5399,
};

enum TolBaradGOs
{
    GO_TOLBARAD_DOOR                            = 206576,
    GO_TOLBARAD_GATES                           = 206598,
    GO_GATE_TO_THE_HOLE                         = 206845,
    GO_GATE_D_BLOCK                             = 206844,
    GO_CURSED_DEPTHS_GATE                       = 206843,

    GO_BARADINS_WARDEN_BANNER                   = 207391,
    GO_HELLSCREAM_REACH_BANNER                  = 207400,

    GO_WEST_SPIRE                               = 204588,
    GO_EAST_SPIRE                               = 204589,
    GO_SOUTH_SPIRE                              = 204590,

    GO_CAPTURE_POINT_NORTH_A_DEFENDING          = 205068,
    GO_CAPTURE_POINT_EAST_A_DEFENDING           = 205138,
    GO_CAPTURE_POINT_WEST_A_DEFENDING           = 205101,

    // The Hole
    GO_CRATE_OF_CELLBLOCK_RATIONS               = 206996,

    // D-Block
    GO_CURSED_SHACKLES                          = 206905,

    // Cursed Depths
    GO_DUSTY_PRISON_JOURNAL                     = 206890,
};

enum TolBaradNPCs
{
    NPC_TB_HORDE_SPIRIT_GUIDE                   = 45074,

    NPC_BARADIN_GUARD                           = 51165,
    NPC_HELLSCREAMS_SENTRY                      = 51166,

    NPC_PROBLIM                                 = 47593,    // remove extras

    // The Hole
    NPC_EXILED_MAGE                             = 47552,
    NPC_IMPRISONED_WORKER                       = 47550,
    NPC_WARDEN_SILVA                            = 48036,    // remove waypoints

    // D-Block
    NPC_SHIVAN_DESTROYER                        = 47540,
    NPC_IMPRISONED_IMP                          = 47549,
    NPC_JAILED_WRATHGUARD                       = 47548,
    NPC_CELL_WATCHER                            = 47542,
    NPC_SVARNOS                                 = 47544,

    // Cursed Depths
    NPC_CAPTIVE_SPIRIT                          = 47531,
    NPC_CELLBLOCK_OOZE                          = 47534,
    NPC_GHASTLY_CONVICT                         = 47590,
    NPC_ARCHMAGE_GALLUST                        = 47537,    // remove wayp

    NPC_CROCOLISK                               = 47591,

    NPC_ALLIANCE_WARRIOR_INFANTRY               = 47599,    // correct inhabittype
    NPC_ALLIANCE_PALADIN_INFANTRY               = 47600,    // correct inhabittype
    NPC_ALLIANCE_HUNTER_INFANTRY                = 47595,
    NPC_ALLIANCE_MAGE_INFANTRY                  = 47598,

    NPC_HORDE_DRUID_INFANTRY                    = 47607,
    NPC_HORDE_MAGE_INFANTRY                     = 47608,
    NPC_HORDE_ROGUE_INFANTRY                    = 47609,
    NPC_HORDE_SHAMAN_INFANTRY                   = 47610,

    NPC_PRIVATE_GARNOTH                         = 48071,
    NPC_DRILLMASTER_RAZGOTH                     = 48070,
    NPC_STAFF_SERGEANT_LAZGAR                   = 48062,
    NPC_COMMANDER_ZANOTH                        = 48069,
    NPC_2ND_LEUTENANT_WANSWORTH                 = 48061,

    NPC_MAVEN_ZARA                              = 50173,
    NPC_SERGEANT_PARKER                         = 48066,
    NPC_MARSHAL_FALLOWS                         = 48074,
    NPC_COMMANDER_STEVENS                       = 48039,

    NPC_TOWER_RANGE_FINDER                      = 45492,
    NPC_TOWER_CANNON_TARGET                     = 45561,
    NPC_SIEGE_ENGINE_TURRET                     = 45564,
    NPC_ABANDONED_SIEGE_ENGINE                  = 45344,
};

enum TBSounds
{
    TB_HORDE_WINS                               = 8454,
    TB_ALLIANCE_WINS                            = 8455,
    TB_START                                    = 3439,
};

enum TBWorldStates
{
    TB_WS_TIME_TO_NEXT_BATTLE                   = 5332,
    TB_WS_TIME_TO_END                           = 5333,
    TB_WS_TOWERS_DESTROYED                      = 5347,
    TB_WS_BUILDINGS_CAPTURED                    = 5348,

    TB_WS_HORDE_CONTROLS_SHOW                   = 5384,
    TB_WS_ALLIANCE_CONTROLS_SHOW                = 5385,
    TB_WS_ALLIANCE_ATTACKING_SHOW               = 5546,
    TB_WS_HORDE_ATTACKING_SHOW                  = 5547,

    TB_WS_NEXT_BATTLE_TIMER_SHOW                = 5387,
    TB_WS_TOWERS_DESTROYED_SHOW                 = 5350,
    TB_WS_BUILDINGS_CAPTURED_SHOW               = 5349,
    TB_WS_TIME_REMAINING_SHOW                   = 5346,

    TB_WS_GARRISON_HORDE_CONTROLLED             = 5418,
    TB_WS_GARRISON_HORDE_CAPTURING              = 5419,
    TB_WS_GARRISON_ALLIANCE_CAPTURING           = 5421,
    TB_WS_GARRISON_ALLIANCE_CONTROLLED          = 5422,

    TB_WS_VIGIL_HORDE_CONTROLLED                = 5423,
    TB_WS_VIGIL_HORDE_CAPTURING                 = 5424,
    TB_WS_VIGIL_ALLIANCE_CAPTURING              = 5426,
    TB_WS_VIGIL_ALLIANCE_CONTROLLED             = 5427,

    TB_WS_SLAGWORKS_HORDE_CONTROLLED            = 5428,
    TB_WS_SLAGWORKS_HORDE_CAPTURING             = 5429,
    TB_WS_SLAGWORKS_ALLIANCE_CAPTURING          = 5431,
    TB_WS_SLAGWORKS_ALLIANCE_CONTROLLED         = 5432,

    TB_WS_WEST_INTACT_HORDE                     = 5433,
    TB_WS_WEST_DAMAGED_HORDE                    = 5434,
    TB_WS_WEST_DESTROYED_NEUTRAL                = 5435,
    TB_WS_WEST_INTACT_ALLIANCE                  = 5436,
    TB_WS_WEST_DAMAGED_ALLIANCE                 = 5437,
    TB_WS_WEST_INTACT_NEUTRAL                   = 5453,
    TB_WS_WEST_DAMAGED_NEUTRAL                  = 5454,

    TB_WS_SOUTH_INTACT_HORDE                    = 5438,
    TB_WS_SOUTH_DAMAGED_HORDE                   = 5439,
    TB_WS_SOUTH_DESTROYED_NEUTRAL               = 5440,
    TB_WS_SOUTH_INTACT_ALLIANCE                 = 5441,
    TB_WS_SOUTH_DAMAGED_ALLIANCE                = 5442,
    TB_WS_SOUTH_INTACT_NEUTRAL                  = 5455,
    TB_WS_SOUTH_DAMAGED_NEUTRAL                 = 5456,

    TB_WS_EAST_INTACT_HORDE                     = 5443,
    TB_WS_EAST_DAMAGED_HORDE                    = 5444,
    TB_WS_EAST_DESTROYED_NEUTRAL                = 5445,
    TB_WS_EAST_INTACT_ALLIANCE                  = 5446,
    TB_WS_EAST_DAMAGED_ALLIANCE                 = 5447,
    TB_WS_EAST_INTACT_NEUTRAL                   = 5451,
    TB_WS_EAST_DAMAGED_NEUTRAL                  = 5452,
};

const uint32 wsByTower[3][7] =
{
    { TB_WS_WEST_INTACT_HORDE, TB_WS_WEST_DAMAGED_HORDE, TB_WS_WEST_DESTROYED_NEUTRAL, TB_WS_WEST_INTACT_ALLIANCE, TB_WS_WEST_DAMAGED_ALLIANCE, TB_WS_WEST_INTACT_NEUTRAL, TB_WS_WEST_DAMAGED_NEUTRAL },
    { TB_WS_SOUTH_INTACT_HORDE, TB_WS_SOUTH_DAMAGED_HORDE, TB_WS_SOUTH_DESTROYED_NEUTRAL, TB_WS_SOUTH_INTACT_ALLIANCE, TB_WS_SOUTH_DAMAGED_ALLIANCE, TB_WS_SOUTH_INTACT_NEUTRAL, TB_WS_SOUTH_DAMAGED_NEUTRAL },
    { TB_WS_EAST_INTACT_HORDE, TB_WS_EAST_DAMAGED_HORDE, TB_WS_EAST_DESTROYED_NEUTRAL, TB_WS_EAST_INTACT_ALLIANCE, TB_WS_EAST_DAMAGED_ALLIANCE, TB_WS_EAST_INTACT_NEUTRAL, TB_WS_EAST_DAMAGED_NEUTRAL }
};

const uint32 TBFactions[3] = { 1, 2, 35 };

enum
{
    TB_GY_WEST_SPIRE                            = 1784,
    TB_GY_SOUTH_SPIRE                           = 1786,
    TB_GY_EAST_SPIRE                            = 1788,

    TB_GY_IRONCLAD_GARRISON                     = 1783,
    TB_GY_WARDENS_VIGIL                         = 1785,
    TB_GY_SLAGWORKS                             = 1787,
    TB_GY_BARADIN_HOLD                          = 1789,

    TB_GY_ALLIANCE_DAILY                        = 1808,
    TB_GY_HORDE_DAILY                           = 1807,
};

enum
{
    TB_BASE_IRONCLAD_GARRISON       = 0,
    TB_BASE_WARDENS_VIGIL           = 1,
    TB_BASE_SLAGWORKS               = 2,
    TB_BASE_COUNT                   = 3,
};

enum
{
    TB_TOWER_WEST                   = 0,
    TB_TOWER_SOUTH                  = 1,
    TB_TOWER_EAST                   = 2,
    TB_TOWER_COUNT                  = 3,
};

/***************************/

class TBBase : public BFObject
{
    public:
        TBBase(uint32 _id, BattleField* _opvp) : BFObject(_id, _opvp) { }

        void InitFor(TeamIndex teamIdx, bool reset = true) override;
        uint32 GetWorldState() const;
        void SendUpdateWorldState() override;

        GuidSet banners;
        ObjectGuid capturePoint;
        WorldSafeLocsEntry const* gy;
 };

class TBTower : public BFObject
{
    public:
        TBTower(uint32 _id, BattleField* _opvp) : BFObject(_id, _opvp) { }

        void InitFor(TeamIndex teamIdx, bool reset = true) override;
        void SpawnTargets(bool spawn);

        uint32 GetWorldState() const;
        void SendUpdateWorldState() override;
        GuidSet targets;
};

class TBPlayerScore : public BFPlayerScore
{
    public:
        TBPlayerScore() : BFPlayerScore(), veteran(false) { }

        void SetVeteran(bool apply) { veteran = apply; }
        bool IsVeteran() const { return veteran; }

    private:
        bool veteran;
};

class MANGOS_DLL_SPEC BattleFieldTB : public BattleField
{
    public:
        BattleFieldTB(uint32 id);
        ~BattleFieldTB();

        void HandleCreatureCreate(Creature* pCreature) override;
        void _OnCreatureCreate(Creature* pCreature);
        void HandleCreatureDeath(Creature * pCreature) override;
        void HandleGameObjectCreate(GameObject* pGo) override;
        bool HandleEvent(uint32 uiEventId, GameObject* pGo, Unit* pInvoker = NULL, uint32 spellId = 0) override;

        void HandlePlayerEnterZone(Player* pPlayer, bool isMainZone) override;
        void HandlePlayerLeaveZone(Player* pPlayer, bool isMainZone) override;
        void HandlePlayerEnterArea(Player* pPlayer, uint32 uiAreaId, bool isMainZone) override;
        void HandlePlayerLeaveArea(Player* pPlayer, uint32 uiAreaId, bool isMainZone) override;
        void HandlePlayerKillInsideArea(Player* pPlayer, Unit* pVictim) override;

        void FillInitialWorldStates(WorldPacket& data, uint32& count, Player* player);
        void SendUpdateWorldStatesTo(Player* player) override;
        void SendRemoveWorldStates(Player* pPlayer) override;

        bool HandleGameObjectUse(Player* pPlayer, GameObject* pGo) override;

        void StartBattle(TeamIndex defender) override;
        void EndBattle(TeamIndex winner, bool byTimer) override;
        void RewardPlayersAtEnd(TeamIndex winner) override;
        bool OnPlayerPortResponse(Player* plr, bool accept) override;
        bool OnPlayerQueueExitRequest(Player* plr) override;
        void Update(uint32 diff) override;

        bool IsVeteran(Player* player) const;
        uint32 GetCapturedBases() const;
        uint32 GetDestoryedTowers() const { return m_destroyedTowers; }
        void UpdateTowerBuff(Player* plr = NULL);
        void UpdateScoreBuff(Player* plr);

        void OnPlayerGroupDisband(Player* plr) override;
        bool CriteriaMeets(uint32 criteriaId, Player* plr) override;

        bool InitOutdoorPvPArea() override;

    private:

        bool CanDamageGO(GameObject* pGo, Player* invoker) override;

        void InitPlayerScore(Player* plr) override;
        bool UpdatePlayerScores() override;
        bool GetKickPosition(Player* plr, float& x, float& y, float& z) override;
        void UpdateBanners();
        void InitGraveyards();
        void ResetVehicles(bool atStart);

        // counters
        uint32 m_damagedTowers;
        uint32 m_destroyedTowers;

        std::vector<TBBase*> m_bases;
        std::vector<TBTower*> m_towers;

        GuidSet banners;
        GuidSet vehicles;
        GuidSet gates;
};

#endif
