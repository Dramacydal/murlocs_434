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

#ifndef WORLD_PVP_HP
#define WORLD_PVP_HP

enum
{
    MAX_HP_TOWERS                           = 3,

    // spells
    SPELL_HELLFIRE_TOWER_TOKEN_ALLIANCE     = 32155,
    SPELL_HELLFIRE_TOWER_TOKEN_HORDE        = 32158,
    SPELL_HELLFIRE_SUPERIORITY_ALLIANCE     = 32071,
    SPELL_HELLFIRE_SUPERIORITY_HORDE        = 32049,

    // zone ids
    ZONE_ID_HELLFIRE_PENINSULA              = 3483,
    ZONE_ID_HELLFIRE_CITADEL                = 3563,
    ZONE_ID_HELLFIRE_RAMPARTS               = 3562,
    ZONE_ID_BLOOD_FURNACE                   = 3713,
    ZONE_ID_SHATTERED_HALLS                 = 3714,
    ZONE_ID_MAGTHERIDON_LAIR                = 3836,

    // npcs
    NPC_CAPTURE_CREDIT_OVERLOOK             = 19028,
    NPC_CAPTURE_CREDIT_STADIUM              = 19029,
    NPC_CAPTURE_CREDIT_BROKEN_HILL          = 19032,
    HONOR_REWARD_HELLFIRE                   = 18,

    // gameobjects
    GO_TOWER_BANNER_OVERLOOK                = 182525,
    GO_TOWER_BANNER_STADIUM                 = 183515,
    GO_TOWER_BANNER_BROKEN_HILL             = 183514,

    // capture points
    GO_HELLFIRE_BANNER_OVERLOOK             = 182174,
    GO_HELLFIRE_BANNER_STADIUM              = 182173,
    GO_HELLFIRE_BANNER_BROKEN_HILL          = 182175,

    // events
    //EVENT_OVERLOOK_WIN_ALLIANCE           = 11398,
    //EVENT_OVERLOOK_WIN_HORDE              = 11397,
    //EVENT_OVERLOOK_CONTEST_ALLIANCE       = 11392,
    //EVENT_OVERLOOK_CONTEST_HORDE          = 11391,
    EVENT_OVERLOOK_PROGRESS_ALLIANCE        = 11396,
    EVENT_OVERLOOK_PROGRESS_HORDE           = 11395,
    EVENT_OVERLOOK_NEUTRAL_ALLIANCE         = 11394,
    EVENT_OVERLOOK_NEUTRAL_HORDE            = 11393,

    //EVENT_STADIUM_WIN_ALLIANCE            = 11390,
    //EVENT_STADIUM_WIN_HORDE               = 11389,
    //EVENT_STADIUM_CONTEST_ALLIANCE        = 11384,
    //EVENT_STADIUM_CONTEST_HORDE           = 11383,
    EVENT_STADIUM_PROGRESS_ALLIANCE         = 11388,
    EVENT_STADIUM_PROGRESS_HORDE            = 11387,
    EVENT_STADIUM_NEUTRAL_ALLIANCE          = 11386,
    EVENT_STADIUM_NEUTRAL_HORDE             = 11385,

    //EVENT_BROKEN_HILL_WIN_ALLIANCE        = 11406,
    //EVENT_BROKEN_HILL_WIN_HORDE           = 11405,
    //EVENT_BROKEN_HILL_CONTEST_ALLIANCE    = 11400,
    //EVENT_BROKEN_HILL_CONTEST_HORDE       = 11399,
    EVENT_BROKEN_HILL_PROGRESS_ALLIANCE     = 11404,
    EVENT_BROKEN_HILL_PROGRESS_HORDE        = 11403,
    EVENT_BROKEN_HILL_NEUTRAL_ALLIANCE      = 11402,
    EVENT_BROKEN_HILL_NEUTRAL_HORDE         = 11401,

    // tower artkits
    GO_ARTKIT_BROKEN_HILL_ALLIANCE          = 65,
    GO_ARTKIT_BROKEN_HILL_HORDE             = 64,
    GO_ARTKIT_BROKEN_HILL_NEUTRAL           = 66,

    GO_ARTKIT_OVERLOOK_ALLIANCE             = 62,
    GO_ARTKIT_OVERLOOK_HORDE                = 61,
    GO_ARTKIT_OVERLOOK_NEUTRAL              = 63,

    GO_ARTKIT_STADIUM_ALLIANCE              = 67,
    GO_ARTKIT_STADIUM_HORDE                 = 68,
    GO_ARTKIT_STADIUM_NEUTRAL               = 69,

    // world states
    WORLD_STATE_TOWER_DISPLAY_HP_A          = 2490,
    WORLD_STATE_TOWER_DISPLAY_HP_H          = 2489,
    WORLD_STATE_TOWER_COUNT_HP_ALLIANCE     = 2476,
    WORLD_STATE_TOWER_COUNT_HP_HORDE        = 2478,

    WORLD_STATE_BROKEN_HILL_ALLIANCE        = 2483,
    WORLD_STATE_BROKEN_HILL_HORDE           = 2484,
    WORLD_STATE_BROKEN_HILL_NEUTRAL         = 2485,

    WORLD_STATE_OVERLOOK_ALLIANCE           = 2480,
    WORLD_STATE_OVERLOOK_HORDE              = 2481,
    WORLD_STATE_OVERLOOK_NEUTRAL            = 2482,

    WORLD_STATE_STADIUM_ALLIANCE            = 2471,
    WORLD_STATE_STADIUM_HORDE               = 2470,
    WORLD_STATE_STADIUM_NEUTRAL             = 2472,
};

struct HellfireTowerEvent
{
    uint32  uiEventEntry;
    Team    team;
    uint32  uiZoneText;
    uint32  uiWorldState;
    uint32  uiTowerArtKit;
    uint32  uiTowerAnim;
};

static const HellfireTowerEvent aHellfireTowerEvents[MAX_HP_TOWERS][4] =
{
    {
        {EVENT_OVERLOOK_PROGRESS_ALLIANCE,      ALLIANCE,   LANG_OPVP_HP_CAPTURE_OVERLOOK_A,    WORLD_STATE_OVERLOOK_ALLIANCE,      GO_ARTKIT_OVERLOOK_ALLIANCE,    CAPTURE_ANIM_ALLIANCE},
        {EVENT_OVERLOOK_PROGRESS_HORDE,         HORDE,      LANG_OPVP_HP_CAPTURE_OVERLOOK_H,    WORLD_STATE_OVERLOOK_HORDE,         GO_ARTKIT_OVERLOOK_HORDE,       CAPTURE_ANIM_HORDE},
        {EVENT_OVERLOOK_NEUTRAL_HORDE,          TEAM_NONE,  LANG_OPVP_HP_LOOSE_OVERLOOK_A,      WORLD_STATE_OVERLOOK_NEUTRAL,       GO_ARTKIT_OVERLOOK_NEUTRAL,     CAPTURE_ANIM_NEUTRAL},
        {EVENT_OVERLOOK_NEUTRAL_ALLIANCE,       TEAM_NONE,  LANG_OPVP_HP_LOOSE_OVERLOOK_H,      WORLD_STATE_OVERLOOK_NEUTRAL,       GO_ARTKIT_OVERLOOK_NEUTRAL,     CAPTURE_ANIM_NEUTRAL},
    },
    {
        {EVENT_STADIUM_PROGRESS_ALLIANCE,       ALLIANCE,   LANG_OPVP_HP_CAPTURE_STADIUM_A,     WORLD_STATE_STADIUM_ALLIANCE,       GO_ARTKIT_STADIUM_ALLIANCE,     CAPTURE_ANIM_ALLIANCE},
        {EVENT_STADIUM_PROGRESS_HORDE,          HORDE,      LANG_OPVP_HP_CAPTURE_STADIUM_H,     WORLD_STATE_STADIUM_HORDE,          GO_ARTKIT_STADIUM_HORDE,        CAPTURE_ANIM_HORDE},
        {EVENT_STADIUM_NEUTRAL_HORDE,           TEAM_NONE,  LANG_OPVP_HP_LOOSE_STADIUM_A,       WORLD_STATE_STADIUM_NEUTRAL,        GO_ARTKIT_STADIUM_NEUTRAL,      CAPTURE_ANIM_NEUTRAL},
        {EVENT_STADIUM_NEUTRAL_ALLIANCE,        TEAM_NONE,  LANG_OPVP_HP_LOOSE_STADIUM_H,       WORLD_STATE_STADIUM_NEUTRAL,        GO_ARTKIT_STADIUM_NEUTRAL,      CAPTURE_ANIM_NEUTRAL},
    },
    {
        {EVENT_BROKEN_HILL_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_HP_CAPTURE_BROKENHILL_A,  WORLD_STATE_BROKEN_HILL_ALLIANCE,   GO_ARTKIT_BROKEN_HILL_ALLIANCE, CAPTURE_ANIM_ALLIANCE},
        {EVENT_BROKEN_HILL_PROGRESS_HORDE,      HORDE,      LANG_OPVP_HP_CAPTURE_BROKENHILL_H,  WORLD_STATE_BROKEN_HILL_HORDE,      GO_ARTKIT_BROKEN_HILL_HORDE,    CAPTURE_ANIM_HORDE},
        {EVENT_BROKEN_HILL_NEUTRAL_HORDE,       TEAM_NONE,  LANG_OPVP_HP_LOOSE_BROKENHILL_A,    WORLD_STATE_BROKEN_HILL_NEUTRAL,    GO_ARTKIT_BROKEN_HILL_NEUTRAL,  CAPTURE_ANIM_NEUTRAL},
        {EVENT_BROKEN_HILL_NEUTRAL_ALLIANCE,    TEAM_NONE,  LANG_OPVP_HP_LOOSE_BROKENHILL_H,    WORLD_STATE_BROKEN_HILL_NEUTRAL,    GO_ARTKIT_BROKEN_HILL_NEUTRAL,  CAPTURE_ANIM_NEUTRAL},
    },
};

static const uint32 aHellfireBanners[MAX_HP_TOWERS] = {GO_HELLFIRE_BANNER_OVERLOOK, GO_HELLFIRE_BANNER_STADIUM, GO_HELLFIRE_BANNER_BROKEN_HILL};

class OutdoorPvPHP : public OutdoorPvP
{
    public:
        OutdoorPvPHP(uint8 _id);

        bool InitOutdoorPvPArea();

        void OnGameObjectCreate(GameObject* pGo);
        void ProcessEvent(uint32 uiEventId, GameObject* pGo, Player* pInvoker = NULL, uint32 spellId = 0);

        void HandlePlayerEnterZone(Player* pPlayer);
        void HandlePlayerLeaveZone(Player* pPlayer);
        void HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team team);
        void HandlePlayerKillInsideArea(Player* pPlayer, Unit* pVictim);

        void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void SendRemoveWorldStates(Player* pPlayer);

    private:
        // world state update
        void UpdateWorldState();

        // process capture events
        void ProcessCaptureEvent(GameObject* pGo, uint32 uiTowerId, Team team, uint32 uiNewWorldState, uint32 uiTowerArtKit, uint32 uiTowerAnim);

        Team m_capturePointOwner[MAX_HP_TOWERS];
        uint32 m_uiTowerWorldState[MAX_HP_TOWERS];
        uint32 m_uiTowersAlliance;
        uint32 m_uiTowersHorde;

        ObjectGuid m_capturePointGuid[MAX_HP_TOWERS];
        ObjectGuid m_bannersGuid[MAX_HP_TOWERS];
};

#endif
