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
 

#ifndef WORLD_PVP_TF
#define WORLD_PVP_TF

enum
{
    MAX_TF_TOWERS                           = 5,

    // zone ids
    ZONE_ID_TEROKKAR                        = 3519,
    ZONE_ID_SHADOW_LABYRINTH                = 3789,
    ZONE_ID_AUCHENAI_CRYPTS                 = 3790,
    ZONE_ID_MANA_TOMBS                      = 3792,
    ZONE_ID_SETHEKK_HALLS                   = 3791,

    // gameobjects
    GO_TEROKKAR_BANNER_1                    = 183104,           // west tower
    GO_TEROKKAR_BANNER_2                    = 183411,           // north tower
    GO_TEROKKAR_BANNER_3                    = 183412,           // north-east tower
    GO_TEROKKAR_BANNER_4                    = 183413,           // south-east tower
    GO_TEROKKAR_BANNER_5                    = 183414,           // south tower

    // spells
    SPELL_AUCHINDOUN_BLESSING               = 33377,

    // timers
    TIMER_TF_LOCK_TIME                      = 6 * HOUR * IN_MILLISECONDS,
    TIMER_TF_UPDATE_TIME                    = MINUTE * IN_MILLISECONDS,

    // quests
    QUEST_SPIRITS_OF_AUCHINDOUM_ALLIANCE    = 11505,
    QUEST_SPIRITS_OF_AUCHINDOUM_HORDE       = 11506,

    // events
    EVENT_TOWER_1_PROGRESS_ALLIANCE         = 12226,
    EVENT_TOWER_1_PROGRESS_HORDE            = 12225,
    EVENT_TOWER_1_NEUTRAL_ALLIANCE          = 12228,
    EVENT_TOWER_1_NEUTRAL_HORDE             = 12227,

    EVENT_TOWER_2_PROGRESS_ALLIANCE         = 12497,
    EVENT_TOWER_2_PROGRESS_HORDE            = 12496,
    EVENT_TOWER_2_NEUTRAL_ALLIANCE          = 12490,
    EVENT_TOWER_2_NEUTRAL_HORDE             = 12491,

    EVENT_TOWER_3_PROGRESS_ALLIANCE         = 12486,
    EVENT_TOWER_3_PROGRESS_HORDE            = 12487,
    EVENT_TOWER_3_NEUTRAL_ALLIANCE          = 12488,
    EVENT_TOWER_3_NEUTRAL_HORDE             = 12489,

    EVENT_TOWER_4_PROGRESS_ALLIANCE         = 12499,
    EVENT_TOWER_4_PROGRESS_HORDE            = 12498,
    EVENT_TOWER_4_NEUTRAL_ALLIANCE          = 12492,
    EVENT_TOWER_4_NEUTRAL_HORDE             = 12493,

    EVENT_TOWER_5_PROGRESS_ALLIANCE         = 12501,
    EVENT_TOWER_5_PROGRESS_HORDE            = 12500,
    EVENT_TOWER_5_NEUTRAL_ALLIANCE          = 12494,
    EVENT_TOWER_5_NEUTRAL_HORDE             = 12495,

    // world states
    // tower counter before the lock event
    WORLD_STATE_TF_TOWER_COUNT_H            = 2622,
    WORLD_STATE_TF_TOWER_COUNT_A            = 2621,
    WORLD_STATE_TF_TOWERS_CONTROLLED        = 2620,

    // timer for the lock event
    WORLD_STATE_TF_TIME_MIN_FIRST_DIGIT     = 2512,
    WORLD_STATE_TF_TIME_MIN_SECOND_DIGIT    = 2510,
    WORLD_STATE_TF_TIME_HOURS               = 2509,

    // lock period - factions
    WORLD_STATE_TF_LOCKED_NEUTRAL           = 2508,
    WORLD_STATE_TF_LOCKED_HORDE             = 2768,
    WORLD_STATE_TF_LOCKED_ALLIANCE          = 2767,

    // tower world states
    WORLD_STATE_TOWER_1_ALLIANCE            = 2683,
    WORLD_STATE_TOWER_1_HORDE               = 2682,
    WORLD_STATE_TOWER_1_NEUTRAL             = 2681,

    WORLD_STATE_TOWER_2_ALLIANCE            = 2684,
    WORLD_STATE_TOWER_2_HORDE               = 2685,
    WORLD_STATE_TOWER_2_NEUTRAL             = 2686,

    WORLD_STATE_TOWER_3_ALLIANCE            = 2688,
    WORLD_STATE_TOWER_3_HORDE               = 2689,
    WORLD_STATE_TOWER_3_NEUTRAL             = 2690,

    WORLD_STATE_TOWER_4_ALLIANCE            = 2694,
    WORLD_STATE_TOWER_4_HORDE               = 2695,
    WORLD_STATE_TOWER_4_NEUTRAL             = 2696,

    WORLD_STATE_TOWER_5_ALLIANCE            = 2691,
    WORLD_STATE_TOWER_5_HORDE               = 2692,
    WORLD_STATE_TOWER_5_NEUTRAL             = 2693,
};

struct TerokkarTowerEvent
{
    uint32  uiEventEntry;
    Team    team;
    uint32  uiZoneText;
    uint32  uiWorldState;
};

static const TerokkarTowerEvent aTerokkarTowerEvents[MAX_TF_TOWERS][4] =
{
    {
        {EVENT_TOWER_1_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_TF_CAPTURE_A, WORLD_STATE_TOWER_1_ALLIANCE},
        {EVENT_TOWER_1_PROGRESS_HORDE,      HORDE,      LANG_OPVP_TF_CAPTURE_H, WORLD_STATE_TOWER_1_HORDE},
        {EVENT_TOWER_1_NEUTRAL_HORDE,       TEAM_NONE,  LANG_OPVP_TF_LOOSE_A,   WORLD_STATE_TOWER_1_NEUTRAL},
        {EVENT_TOWER_1_NEUTRAL_ALLIANCE,    TEAM_NONE,  LANG_OPVP_TF_LOOSE_H,   WORLD_STATE_TOWER_1_NEUTRAL},
    },
    {
        {EVENT_TOWER_2_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_TF_CAPTURE_A, WORLD_STATE_TOWER_2_ALLIANCE},
        {EVENT_TOWER_2_PROGRESS_HORDE,      HORDE,      LANG_OPVP_TF_CAPTURE_H, WORLD_STATE_TOWER_2_HORDE},
        {EVENT_TOWER_2_NEUTRAL_HORDE,       TEAM_NONE,  LANG_OPVP_TF_LOOSE_A,   WORLD_STATE_TOWER_2_NEUTRAL},
        {EVENT_TOWER_2_NEUTRAL_ALLIANCE,    TEAM_NONE,  LANG_OPVP_TF_LOOSE_H,   WORLD_STATE_TOWER_2_NEUTRAL},
    },
    {
        {EVENT_TOWER_3_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_TF_CAPTURE_A, WORLD_STATE_TOWER_3_ALLIANCE},
        {EVENT_TOWER_3_PROGRESS_HORDE,      HORDE,      LANG_OPVP_TF_CAPTURE_H, WORLD_STATE_TOWER_3_HORDE},
        {EVENT_TOWER_3_NEUTRAL_HORDE,       TEAM_NONE,  LANG_OPVP_TF_LOOSE_A,   WORLD_STATE_TOWER_3_NEUTRAL},
        {EVENT_TOWER_3_NEUTRAL_ALLIANCE,    TEAM_NONE,  LANG_OPVP_TF_LOOSE_H,   WORLD_STATE_TOWER_3_NEUTRAL},
    },
    {
        {EVENT_TOWER_4_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_TF_CAPTURE_A, WORLD_STATE_TOWER_4_ALLIANCE},
        {EVENT_TOWER_4_PROGRESS_HORDE,      HORDE,      LANG_OPVP_TF_CAPTURE_H, WORLD_STATE_TOWER_4_HORDE},
        {EVENT_TOWER_4_NEUTRAL_HORDE,       TEAM_NONE,  LANG_OPVP_TF_LOOSE_A,   WORLD_STATE_TOWER_4_NEUTRAL},
        {EVENT_TOWER_4_NEUTRAL_ALLIANCE,    TEAM_NONE,  LANG_OPVP_TF_LOOSE_H,   WORLD_STATE_TOWER_4_NEUTRAL},
    },
    {
        {EVENT_TOWER_5_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_TF_CAPTURE_A, WORLD_STATE_TOWER_5_ALLIANCE},
        {EVENT_TOWER_5_PROGRESS_HORDE,      HORDE,      LANG_OPVP_TF_CAPTURE_H, WORLD_STATE_TOWER_5_HORDE},
        {EVENT_TOWER_5_NEUTRAL_HORDE,       TEAM_NONE,  LANG_OPVP_TF_LOOSE_A,   WORLD_STATE_TOWER_5_NEUTRAL},
        {EVENT_TOWER_5_NEUTRAL_ALLIANCE,    TEAM_NONE,  LANG_OPVP_TF_LOOSE_H,   WORLD_STATE_TOWER_5_NEUTRAL},
    },
};

static const uint32 aTerokkarTowers[MAX_TF_TOWERS] = {GO_TEROKKAR_BANNER_1, GO_TEROKKAR_BANNER_2, GO_TEROKKAR_BANNER_3, GO_TEROKKAR_BANNER_4, GO_TEROKKAR_BANNER_5};

class OutdoorPvPTF : public OutdoorPvP
{
    friend class OutdoorPvPMgr;

    public:
        OutdoorPvPTF(uint8 _id);

        bool InitOutdoorPvPArea();

        void OnGameObjectCreate(GameObject* pGo);
        void ProcessEvent(uint32 uiEventId, GameObject* pGo, Player* pInvoker = NULL, uint32 spellId = 0);

        void HandlePlayerEnterZone(Player* pPlayer);
        void HandlePlayerLeaveZone(Player* pPlayer);
        void HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team team);

        void Update(uint32 diff);

        void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void SendRemoveWorldStates(Player* pPlayer);

    private:
        void UpdateWorldState(uint8 uiValue);
        void UpdateTimerWorldState();

        // process capture events
        void ProcessCaptureEvent(GameObject* pGo, uint32 uiTowerId, Team team, uint32 uiNewWorldState);

        void LockTowers(const WorldObject* objRef);
        void ResetTowers(const WorldObject* objRef);

        uint32 m_uiTowerWorldState[MAX_TF_TOWERS];
        uint32 m_uiZoneWorldState;

        Team m_capturePointOwner[MAX_TF_TOWERS];
        Team m_zoneOwner;

        uint32 m_uiZoneLockTimer;
        uint32 m_uiZoneUpdateTimer;

        uint8 m_uiTowersAlliance;
        uint8 m_uiTowersHorde;

        ObjectGuid m_TowerBannerGUID[MAX_TF_TOWERS];
};

#endif
