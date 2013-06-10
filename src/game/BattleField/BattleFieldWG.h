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


#ifndef WORLD_PVP_WG
#define WORLD_PVP_WG

#include "Common.h"
#include "BattleField.h"
#include "Language.h"
#include "World.h"

#define WG_TOWERS_BONUS (sWorld.getConfig(CONFIG_UINT32_WINTERGRASP_TOWERS_BONUS) * MINUTE * IN_MILLISECONDS)

class Group;
class BattleFieldWG;
class WGWorkShop;
class WGGraveYard;

enum WintergraspSpells
{
    // Wartime auras
    SPELL_RECRUIT                               = 37795,
    SPELL_CORPORAL                              = 33280,
    SPELL_LIEUTENANT                            = 55629,
    SPELL_TENACITY                              = 58549,
    SPELL_TENACITY_VEHICLE                      = 59911,
    SPELL_TOWER_CONTROL                         = 62064,
    SPELL_SPIRITUAL_IMMUNITY                    = 58729,
    SPELL_GREAT_HONOR                           = 58555,
    SPELL_GREATER_HONOR                         = 58556,
    SPELL_GREATEST_HONOR                        = 58557,
    SPELL_ALLIANCE_FLAG                         = 14268,
    SPELL_HORDE_FLAG                            = 14267,
    SPELL_GRAB_PASSENGER                        = 61178,
    SPELL_ACTIVATE_ROBOTIC_ARMS                 = 49899,

    // Reward spells
    SPELL_VICTORY_REWARD                        = 56902,
    SPELL_DEFEAT_REWARD                         = 58494,
    SPELL_DAMAGED_TOWER                         = 59135, // add honor
    SPELL_DESTROYED_TOWER                       = 59136, // add honor
    SPELL_DAMAGED_BUILDING                      = 59201,
    SPELL_INTACT_BUILDING                       = 59203,

    SPELL_TELEPORT_DALARAN                      = 53360,
    SPELL_VICTORY_AURA                          = 60044,

    // Other spells
    SPELL_WINTERGRASP_WATER                     = 36444,
    SPELL_ESSENCE_OF_WINTERGRASP                = 57940,
    SPELL_WINTERGRASP_RESTRICTED_FLIGHT_AREA    = 91604,

    // Phasing spells
    SPELL_ESSENCE_OF_WINTERGRASP_ZONE           = 58045, // PHASE 257

    // Teleport spells
    SPELL_TELEPORT_BRIDGE                       = 59096,
    SPELL_TELEPORT_FORTRESS                     = 60035,

    SPELL_TELEPORT_FORTRESS_GRAVEYARD           = 59760,
    SPELL_TELEPORT_SUNKEN_RING                  = 59762,
    SPELL_TELEPORT_BROKEN_TEMPLE                = 59763,
    SPELL_TELEPORT_HORDE_LANDING                = 59765,
    SPELL_TELEPORT_WESTPARK_GRAVEYARD           = 59766,
    SPELL_TELEPORT_EASTPARK_GRAVEYARD           = 59767,
    SPELL_TELEPORT_ALLIANCE_LANDING             = 59769,

    SPELL_TELEPORT_PORTAL                       = 54643,
    SPELL_TELEPORT_VEHICLE                      = 49759,

    SPELL_TELEPORT_WINTERGRASP                  = 58622, // go cast
    SPELL_TELEPORT_WINTERGRASP_ALLIANCE         = 58633, // to alliance landing
    SPELL_TELEPORT_WINTERGRASP_HORDE            = 58632, // to horde landing

    // Vehicle build spells
    SPELL_BUILD_CATAPULT                        = 56663,
    SPELL_BUILD_CATAPULT_FORCE                  = 56664,
    SPELL_BUILD_DEMOLISHER                      = 56575,
    SPELL_BUILD_DEMOLISHER_FORCE                = 56659,
    SPELL_BUILD_SIEGE_VEHICLE_A                 = 56661,
    SPELL_BUILD_SIEGE_VEHICLE_A_FORCE           = 56662,
    SPELL_BUILD_SIEGE_VEHICLE_H                 = 61408,
    SPELL_BUILD_SIEGE_VEHICLE_H_FORCE           = 61409,

    SPELL_ACH_DESTROYED_TOWER                   = 60676,
};

enum
{
    WG_QUEST_VICTORY_IN_WINTERGRASP_A   = 13181,
    WG_QUEST_VICTORY_IN_WINTERGRASP_H   = 13183,

    WG_QUEST_CREDIT_DESTROYED_VEHICLES  = 31093,
    WG_QUEST_CREDIT_DEFEND_VEHICLE      = 31284,
    WG_QUEST_CREDIT_ALLIANCE_SLAIN      = 31086,
    WG_QUEST_CREDIT_SOUTHERN_TOWER      = 35074,
    WG_QUEST_CREDIT_HORDE_SLAIN         = 39019,
};

enum
{
    WG_CRITERIA_WIN_10_MINUTES          = 7666,
};

enum
{
    MAP_ID_NORTHREND                    = 571,

    AREA_ID_THE_SUNKEN_RING             = 4538,
    AREA_ID_THE_BROKEN_TEMPLE           = 4539,
    AREA_ID_WINTERGRASP_FORTRESS        = 4575,
    AREA_ID_CENTRAL_BRIDGE              = 4576,
    AREA_ID_EASTERN_BRIDGE              = 4577,
    AREA_ID_WESTERN_BRIDGE              = 4578,
    AREA_ID_FLAMEWATCH_TOWER            = 4581,
    AREA_ID_WINTERS_EDGE_TOWER          = 4582,
    AREA_ID_SHADOWSIGHT_TOWER           = 4583,
    AREA_ID_THE_CAULDRON_OF_FLAMES      = 4584,
    AREA_ID_GLACIAL_FALLS               = 4585,
    AREA_ID_WINDY_BLUFFS                = 4586,
    AREA_ID_THE_FOREST_OF_SHADOWS       = 4587,
    AREA_ID_THE_CHILLED_QUAGMIRE        = 4589,
    AREA_ID_THE_STEPPE_OF_LIFE          = 4590,
    AREA_ID_WESTPARK_WORKSHOP           = 4611,
    AREA_ID_EASTPARK_WORKSHOP           = 4612,

    // ally creatures
    NPC_ANCHORITE_TESSA                 = 31054,
    NPC_COMMANDER_ZANNETH               = 31036,
    NPC_SENIOR_DEMOLITIONIST_LEGOSO     = 31109,
    NPC_SIEGE_MASTER_STOUTHANDLE        = 31108,
    NPC_SORCERESS_KAYLANA               = 31051,
    NPC_TACTICAL_OFFICER_AHBRAMIS       = 31153,
    NPC_BOWYER_RANDOLPH                 = 31052,

    NPC_KNIGHT_DAMERON                  = 32294,
    NPC_MARSHAL_MAGRUDER                = 39172,
    
    NPC_MORGAN_DAY                      = 30489,
    NPC_TRAVIS_DAY                      = 30488,

    // horde keep creatures
    NPC_COMMANDER_DARDOSH               = 31091,
    NPC_HOODOO_MASTER_FUJIN             = 31101,
    NPC_LEUTENANT_MURP                  = 31107,
    NPC_PRIMALIST_MULFORT               = 31053,
    NPC_SIEGESMITH_STRONGHOOF           = 31106,
    NPC_TACTICAL_OFFICER_KILRATH        = 31151,
    NPC_VIERON_BLAZEFEATHER             = 31102,

    NPC_STONE_GUARD_MUKAR               = 32296,
    NPC_CHAMPION_ROSSLAI                = 39173,

    // trash
    NPC_CHILLED_EARH_ELEMENTAL          = 30849,
    NPC_EARTHBOUND_REVENANT             = 30876,
    NPC_RAGING_FLAME                    = 30847,
    NPC_FLAME_REVENANT                  = 30873,
    NPC_GLACIAL_SPIRIT                  = 30846,
    NPC_WATER_REVENANT                  = 30877,
    NPC_WANDERING_SHADOW                = 30842,
    NPC_SHADOW_REVENANT                 = 30872,
    NPC_WHISPERING_WIND                 = 30848,
    NPC_TEMPEST_REVENANT                = 30875,
    NPC_LIVING_LASHER                   = 30845,
    NPC_MATURE_LASHER                   = 34300,

    NPC_GNOMISH_MECHANIC                = 30499,
    NPC_GOBLIN_ENGINEER                 = 30400,

    NPC_ALLIANCE_SPIRIT_GUIDE           = 31842,
    NPC_HORDE_SPIRIT_GUIDE              = 31841,

    NPC_VALLIANCE_EXPEDITION_CHAMPION   = 30740,
    NPC_VALLIANCE_EXPEDITION_GUARD      = 32308,

    NPC_WARSONG_CHAMPION                = 30739,
    NPC_WARSONG_GUARD                   = 32307,

    NPC_ALLIANCE_BRIGADIER_GENERAL      = 32626,
    NPC_HORDE_WARBRINGER                = 32615,

    NPC_WINTERGRASP_CATAPULT            = 27881,
    NPC_WINTERGRASP_DEMOLISHER          = 28094,
    NPC_WINTERGRASP_SIEGE_ENGINE_A      = 28312,
    NPC_WINTERGRASP_SIEGE_ENGINE_H      = 32627,
    NPC_WINTERGRASP_SIEGE_TURRET_A      = 28319,
    NPC_WINTERGRASP_SIEGE_TURRET_H      = 32629,
    NPC_WINTERGRASP_TOWER_CANNON        = 28366,

    NPC_RP_GG                           = 30560,
    NPC_CONTROL_ARMS                    = 27852,

    NPC_WORLD_TRIGGER                   = 23472,

    NPC_ARATHI_BASIN_PORTAL_A           = 32618,
    NPC_ARATHI_BASIN_PORTAL_H           = 32619,

    NPC_WARSONG_GULCH_PORTAL_A          = 32624,
    NPC_WARSONG_GULCH_PORTAL_H          = 32625,

    NPC_STRANG_OF_THE_ANCIENTS_PORTAL_A = 32622,
    NPC_STRANG_OF_THE_ANCIENTS_PORTAL_H = 32623,

    NPC_EYE_OF_THE_STORM_PORTAL_A       = 32621,
    NPC_EYE_OF_THE_STORM_PORTAL_H       = 32620,

    NPC_ALTERAC_VALLEY_PORTAL_A         = 32616,
    NPC_ALTERAC_VALLEY_PORTAL_H         = 32617,

    NPC_ISLE_OF_CONQUEST_PORTAL_A       = 34952,
    NPC_ISLE_OF_CONQUEST_PORTAL_H       = 34953,

    GO_ALLIANCE_BANNER_1                = 192287,
    GO_ALLIANCE_BANNER_2                = 192320,
    GO_ALLIANCE_BANNER_3                = 192314,
    GO_ALLIANCE_BANNER_4                = 192487,
    GO_ALLIANCE_BANNER_5                = 192313,
    GO_ALLIANCE_BANNER_6                = 192312,
    GO_ALLIANCE_BANNER_7                = 192299,
    GO_ALLIANCE_BANNER_8                = 192292,
    GO_ALLIANCE_BANNER_9                = 192308,
    GO_ALLIANCE_BANNER_10               = 192327,
    GO_ALLIANCE_BANNER_11               = 192329,
    GO_ALLIANCE_BANNER_12               = 192309,
    GO_ALLIANCE_BANNER_13               = 192324,
    GO_ALLIANCE_BANNER_14               = 192310,
    GO_ALLIANCE_BANNER_15               = 192325,
    GO_ALLIANCE_BANNER_16               = 192326,
    GO_ALLIANCE_BANNER_17               = 192322,
    GO_ALLIANCE_BANNER_18               = 192323,
    GO_ALLIANCE_BANNER_19               = 192318,
    GO_ALLIANCE_BANNER_20               = 192319,
    GO_ALLIANCE_BANNER_21               = 192304,
    GO_ALLIANCE_BANNER_22               = 192305,
    GO_ALLIANCE_BANNER_23               = 192286,
    GO_ALLIANCE_BANNER_24               = 192317,
    GO_ALLIANCE_BANNER_25               = 192316,
    GO_ALLIANCE_BANNER_26               = 192321,
    GO_ALLIANCE_BANNER_27               = 192335,
    GO_ALLIANCE_BANNER_28               = 192334,
    GO_ALLIANCE_BANNER_29               = 192331,
    GO_ALLIANCE_BANNER_30               = 192332,
    GO_ALLIANCE_BANNER_31               = 192333,
    GO_ALLIANCE_BANNER_32               = 192314,
    GO_ALLIANCE_BANNER_33               = 192306,
    GO_ALLIANCE_BANNER_34               = 192307,
    GO_ALLIANCE_BANNER_35               = 192330,
    GO_ALLIANCE_BANNER_36               = 192328,
    GO_ALLIANCE_BANNER_37               = 192252,
    GO_ALLIANCE_BANNER_38               = 192253,
    // center tower
    GO_ALLIANCE_BANNER_39               = 192269, // named horde banner in db
    GO_ALLIANCE_BANNER_40               = 192501, // all towers have this
    GO_ALLIANCE_BANNER_41               = 192266,
    GO_ALLIANCE_BANNER_42               = 192415,
    GO_ALLIANCE_BANNER_43               = 192429,
    GO_ALLIANCE_BANNER_44               = 192430,
    GO_ALLIANCE_BANNER_45               = 192270,
    GO_ALLIANCE_BANNER_46               = 192431,
    GO_ALLIANCE_BANNER_47               = 192414,
    // wespark
    GO_ALLIANCE_BANNER_48               = 192274,
    GO_ALLIANCE_BANNER_49               = 192409,
    GO_ALLIANCE_BANNER_50               = 192433,
    GO_ALLIANCE_BANNER_51               = 192407,
    GO_ALLIANCE_BANNER_52               = 192406,
    // west tower
    GO_ALLIANCE_BANNER_53               = 192278,
    GO_ALLIANCE_BANNER_54               = 192277,
    // broken temple
    GO_ALLIANCE_BANNER_55               = 192401,
    GO_ALLIANCE_BANNER_56               = 192281,
    GO_ALLIANCE_BANNER_57               = 192400,
    GO_ALLIANCE_BANNER_58               = 192282,
    // sunken ring
    GO_ALLIANCE_BANNER_59               = 192291,
    GO_ALLIANCE_BANNER_60               = 192427,
    GO_ALLIANCE_BANNER_61               = 192428,
    GO_ALLIANCE_BANNER_62               = 192426,
    GO_ALLIANCE_BANNER_63               = 192425,
    GO_ALLIANCE_BANNER_64               = 192288,
    // eastpark
    GO_ALLIANCE_BANNER_65               = 192418,
    GO_ALLIANCE_BANNER_66               = 192273,
    GO_ALLIANCE_BANNER_67               = 192417,
    GO_ALLIANCE_BANNER_68               = 192416,
    // east tower
    GO_ALLIANCE_BANNER_69               = 192423,

    GO_HORDE_BANNER_1                   = 192284,
    GO_HORDE_BANNER_2                   = 192355,
    GO_HORDE_BANNER_3                   = 192364,
    GO_HORDE_BANNER_4                   = 192488,
    GO_HORDE_BANNER_5                   = 192367,
    GO_HORDE_BANNER_6                   = 192366,
    GO_HORDE_BANNER_7                   = 192349,
    GO_HORDE_BANNER_8                   = 192336,
    GO_HORDE_BANNER_9                   = 192375,
    GO_HORDE_BANNER_10                  = 192353,
    GO_HORDE_BANNER_11                  = 192357,
    GO_HORDE_BANNER_12                  = 192374,
    GO_HORDE_BANNER_13                  = 192373,
    //GO_HORDE_BANNER_14                  = ?,
    GO_HORDE_BANNER_15                  = 192360,
    GO_HORDE_BANNER_16                  = 192361,
    GO_HORDE_BANNER_17                  = 192372,
    GO_HORDE_BANNER_18                  = 192371,
    GO_HORDE_BANNER_19                  = 192358,
    GO_HORDE_BANNER_20                  = 192359,
    GO_HORDE_BANNER_21                  = 192338,
    GO_HORDE_BANNER_22                  = 192339,
    GO_HORDE_BANNER_23                  = 192285,
    GO_HORDE_BANNER_24                  = 192379,
    GO_HORDE_BANNER_25                  = 192378,
    GO_HORDE_BANNER_26                  = 192354,
    GO_HORDE_BANNER_27                  = 192363,
    GO_HORDE_BANNER_28                  = 192362,
    GO_HORDE_BANNER_29                  = 192368,
    GO_HORDE_BANNER_30                  = 192369,
    GO_HORDE_BANNER_31                  = 192370,
    GO_HORDE_BANNER_32                  = 192364,
    GO_HORDE_BANNER_33                  = 192350,
    GO_HORDE_BANNER_34                  = 192351,
    GO_HORDE_BANNER_35                  = 192356,
    GO_HORDE_BANNER_36                  = 192352,
    GO_HORDE_BANNER_37                  = 192255,
    GO_HORDE_BANNER_38                  = 192254,
    // center tower
    GO_HORDE_BANNER_39                  = 192268,
    GO_HORDE_BANNER_40                  = 192502, // all towers have this
    GO_HORDE_BANNER_41                  = 192267,
    GO_HORDE_BANNER_42                  = 192449,
    GO_HORDE_BANNER_43                  = 192444,
    GO_HORDE_BANNER_44                  = 192443,
    GO_HORDE_BANNER_45                  = 192271,
    GO_HORDE_BANNER_46                  = 192442,
    GO_HORDE_BANNER_47                  = 192450,
    // westpark
    GO_HORDE_BANNER_48                  = 192275,
    GO_HORDE_BANNER_49                  = 192408,
    GO_HORDE_BANNER_50                  = 192432,
    GO_HORDE_BANNER_51                  = 192441,
    GO_HORDE_BANNER_52                  = 192440,
    // west tower
    GO_HORDE_BANNER_53                  = 192279,
    GO_HORDE_BANNER_54                  = 192276,
    // broken temple
    GO_HORDE_BANNER_55                  = 192435,
    GO_HORDE_BANNER_56                  = 192280,
    GO_HORDE_BANNER_57                  = 192434,
    GO_HORDE_BANNER_58                  = 192283,
    // sunken ring
    GO_HORDE_BANNER_59                  = 192290,
    GO_HORDE_BANNER_60                  = 192460,
    GO_HORDE_BANNER_61                  = 192461,
    GO_HORDE_BANNER_62                  = 192459,
    GO_HORDE_BANNER_63                  = 192458,
    GO_HORDE_BANNER_64                  = 192289,
    // eastpark
    GO_HORDE_BANNER_65                  = 192453,
    GO_HORDE_BANNER_66                  = 192272,
    GO_HORDE_BANNER_67                  = 192452,
    GO_HORDE_BANNER_68                  = 192451,
    // east tower
    GO_HORDE_BANNER_69                  = 192424,

    // Keep misc GOs
    GO_ALLIANCE_MISC_01                 = 193138,
    GO_ALLIANCE_MISC_02                 = 193131,
    GO_ALLIANCE_MISC_03                 = 193137,
    GO_ALLIANCE_MISC_04                 = 193135,
    GO_ALLIANCE_MISC_05                 = 193134,
    GO_ALLIANCE_MISC_06                 = 193132,
    GO_ALLIANCE_MISC_07                 = 193128,
    GO_ALLIANCE_MISC_08                 = 193165,
    GO_ALLIANCE_MISC_09                 = 193124,

    GO_HORDE_MISC_01                    = 193145,
    GO_HORDE_MISC_02                    = 193139,
    GO_HORDE_MISC_03                    = 193142,
    GO_HORDE_MISC_04                    = 193146,
    GO_HORDE_MISC_05                    = 193141,
    GO_HORDE_MISC_06                    = 193140,
    GO_HORDE_MISC_07                    = 193144,
    GO_HORDE_MISC_08                    = 193143,
    GO_HORDE_MISC_09                    = 193161,
    GO_HORDE_MISC_10                    = 193127,

    GO_TITAN_RELIC                      = 192834,

    // Keep walls
    GO_WINTERGRASP_FORTRESS_DOOR        = 191810,
    GO_WINTERGRASP_FORTRESS_GATE        = 190375,
    GO_WINTERGRASP_FORTRESS_WALL_1      = 190220,
    GO_WINTERGRASP_FORTRESS_WALL_2      = 190219,
    GO_WINTERGRASP_FORTRESS_WALL_3      = 190369,
    GO_WINTERGRASP_FORTRESS_WALL_4      = 190371,
    GO_WINTERGRASP_FORTRESS_WALL_5      = 190374,
    GO_WINTERGRASP_FORTRESS_WALL_6      = 190376,
    GO_WINTERGRASP_FORTRESS_WALL_7      = 190370,
    GO_WINTERGRASP_FORTRESS_WALL_8      = 190372,
    GO_WINTERGRASP_FORTRESS_WALL_9      = 191799,
    GO_WINTERGRASP_FORTRESS_WALL_10     = 191809,
    GO_WINTERGRASP_FORTRESS_WALL_11     = 191806,
    GO_WINTERGRASP_FORTRESS_WALL_12     = 191808,
    GO_WINTERGRASP_FORTRESS_WALL_13     = 191801,
    GO_WINTERGRASP_FORTRESS_WALL_14     = 191807,
    GO_WINTERGRASP_FORTRESS_WALL_15     = 191796,
    GO_WINTERGRASP_FORTRESS_WALL_16     = 191800,
    GO_WINTERGRASP_FORTRESS_WALL_17     = 191804,
    GO_WINTERGRASP_FORTRESS_WALL_18     = 191795,
    GO_WINTERGRASP_FORTRESS_WALL_19     = 191803,
    GO_WINTERGRASP_FORTRESS_WALL_20     = 191802,
    GO_WINTERGRASP_WALL_1               = 191798,
    GO_WINTERGRASP_WALL_2               = 191805,
    GO_WINTERGRASP_WALL_3               = 191797,

    GO_WINTERGRASP_TOWER_WEST           = 190221,
    GO_WINTERGRASP_TOWER_SOUTHWEST      = 190373,
    GO_WINTERGRASP_TOWER_SOUTHEAST      = 190377,
    GO_WINTERGRASP_TOWER_EAST           = 190378,

    GO_WORKSHOP_KEEP_WEST               = 192028,
    GO_WORKSHOP_KEEP_EAST               = 192029,
    GO_WORKSHOP_BROKEN_TEMPLE           = 192030,
    GO_WORKSHOP_SUNKEN_RING             = 192031,
    GO_WORKSHOP_WESTPARK                = 192032,
    GO_WORKSHOP_EASTPARK                = 192033,

    GO_TOWER_SHADOWSIGHT                = 190356,
    GO_TOWER_WINTERS_EDGE               = 190357,
    GO_TOWER_FLAMEWATCH                 = 190358,

    GO_CAPTUREPOINT_BROKEN_TEMPLE_A     = 192627,
    GO_CAPTUREPOINT_BROKEN_TEMPLE_H     = 190487,    

    GO_CAPTUREPOINT_WESTPARK_A          = 194962,
    GO_CAPTUREPOINT_WESTPARK_H          = 194963,

    GO_CAPTUREPOINT_EASTPARK_A          = 194959,
    GO_CAPTUREPOINT_EASTPARK_H          = 194960,

    GO_CAPTUREPOINT_SUNKEN_RING_A       = 192626,
    GO_CAPTUREPOINT_SUNKEN_RING_H       = 190475,

    GO_DEFENDERS_PORTAL_1               = 190763,
    GO_DEFENDERS_PORTAL_2               = 191575,
    GO_DEFENDERS_PORTAL_3               = 192819,

    GO_VEHICLE_TELEPORTER               = 192951,

    GO_PORTAL_TO_WINTERGRASP            = 193772,

    WG_GRAVEYARD_ENTRY_KEEP_1           = 1285,
    WG_GRAVEYARD_ENTRY_KEEP_2           = 1328,
    WG_GRAVEYARD_ENTRY_SUNKEN_RING      = 1329,
    WG_GRAVEYARD_ENTRY_BROKEN_TEMPLE    = 1330,
    WG_GRAVEYARD_ENTRY_EASTPARK         = 1333,
    WG_GRAVEYARD_ENTRY_WESTPARK         = 1334,
};

enum
{
    WG_WS_VEHICLE_H                     = 3490,
    WG_WS_MAX_VEHICLE_H                 = 3491,
    WG_WS_VEHICLE_A                     = 3680,
    WG_WS_MAX_VEHICLE_A                 = 3681,
    WG_WS_SHOW_BATTLE_WORLDSTATE        = 3710,
    WG_WS_SHOW_COOLDOWN_WORLDSTATE      = 3801,
    WG_WS_HORDE_DEFENDER                = 3802,
    WG_WS_ALLIANCE_DEFENDER             = 3803,

    WG_WS_WORKSHOP_SUNKEN_RING          = 3701,
    WG_WS_WORKSHOP_BROKEN_TEMPLE        = 3700,
    WG_WS_WORKSHOP_EASTPARK             = 3703,
    WG_WS_WORKSHOP_WESTPARK             = 3702,
    WG_WS_WORKSHOP_KEEP_WEST            = 3698,
    WG_WS_WORKSHOP_KEEP_EAST            = 3699,

    WG_WS_KEEPTOWER_WEST                = 3711,
    WG_WS_KEEPTOWER_SOUTHWEST           = 3713,
    WG_WS_KEEPTOWER_SOUTHEAST           = 3714,
    WG_WS_KEEPTOWER_EAST                = 3712,

    WG_WS_TOWER_SHADOWSIGHT             = 3704,
    WG_WS_TOWER_WINTERS_EDGE            = 3705,
    WG_WS_TOWER_FLAMEWATCH              = 3706,

    WG_WS_TIME_TO_END                   = 3781,
    WG_WS_TIME_TO_NEXT_BATTLE           = 4354,
};

enum WGSounds
{
    WG_HORDE_WINS                                = 8454,
    WG_ALLIANCE_WINS                             = 8455,
    WG_START                                     = 3439,
};

typedef struct
{
    uint32 entry;
    uint32 worldState;
} WGWorldstateByGOEntryStruct;

const WGWorldstateByGOEntryStruct WGWorldstateByGOEntry[] =
{
    // Walls
    { GO_WINTERGRASP_FORTRESS_WALL_2, 3749 },
    { GO_WINTERGRASP_FORTRESS_WALL_1, 3750 },
    { GO_WINTERGRASP_FORTRESS_WALL_18, 3764 },
    { GO_WINTERGRASP_FORTRESS_WALL_15, 3772 },
    { GO_WINTERGRASP_FORTRESS_WALL_9, 3762 },
    { GO_WINTERGRASP_FORTRESS_WALL_16, 3766 },
    { GO_WINTERGRASP_FORTRESS_WALL_13, 3770 },
    { GO_WINTERGRASP_FORTRESS_WALL_20, 3751 },
    { GO_WINTERGRASP_FORTRESS_WALL_19, 3752 },
    { GO_WINTERGRASP_FORTRESS_WALL_17, 3767 },
    { GO_WINTERGRASP_FORTRESS_WALL_11, 3769 },
    { GO_WINTERGRASP_FORTRESS_WALL_14, 3759 },
    { GO_WINTERGRASP_FORTRESS_WALL_12, 3760 },
    { GO_WINTERGRASP_FORTRESS_WALL_10, 3761 },
    { GO_WINTERGRASP_FORTRESS_WALL_3, 3753 },
    { GO_WINTERGRASP_FORTRESS_WALL_7, 3758 },
    { GO_WINTERGRASP_FORTRESS_WALL_4, 3754 },
    { GO_WINTERGRASP_FORTRESS_WALL_8, 3757 },
    { GO_WINTERGRASP_FORTRESS_WALL_5, 3755 },
    { GO_WINTERGRASP_FORTRESS_WALL_6, 3756 },

    // Keep towers
    { GO_WINTERGRASP_TOWER_WEST, WG_WS_KEEPTOWER_WEST },
    { GO_WINTERGRASP_TOWER_SOUTHWEST, WG_WS_KEEPTOWER_SOUTHWEST },
    { GO_WINTERGRASP_TOWER_SOUTHEAST, WG_WS_KEEPTOWER_SOUTHEAST },
    { GO_WINTERGRASP_TOWER_EAST, WG_WS_KEEPTOWER_EAST },

    // Walls (with passage)
    { GO_WINTERGRASP_WALL_1, 3771 },
    { GO_WINTERGRASP_WALL_2, 3768 },
    { GO_WINTERGRASP_WALL_3, 3765 },

    // South towers
    { GO_TOWER_SHADOWSIGHT, WG_WS_TOWER_SHADOWSIGHT },
    { GO_TOWER_WINTERS_EDGE, WG_WS_TOWER_WINTERS_EDGE },
    { GO_TOWER_FLAMEWATCH, WG_WS_TOWER_FLAMEWATCH },

    // Fortress door
    { GO_WINTERGRASP_FORTRESS_GATE, 3763 },

    // Last door
    { GO_WINTERGRASP_FORTRESS_DOOR, 3773 },

    // Workshops
    { GO_WORKSHOP_SUNKEN_RING, WG_WS_WORKSHOP_SUNKEN_RING },
    { GO_WORKSHOP_BROKEN_TEMPLE, WG_WS_WORKSHOP_BROKEN_TEMPLE },
    { GO_WORKSHOP_EASTPARK, WG_WS_WORKSHOP_EASTPARK },
    { GO_WORKSHOP_WESTPARK, WG_WS_WORKSHOP_WESTPARK },
    { GO_WORKSHOP_KEEP_WEST, WG_WS_WORKSHOP_KEEP_WEST },
    { GO_WORKSHOP_KEEP_EAST, WG_WS_WORKSHOP_KEEP_EAST }
};

enum
{
    WG_WORKSHOP_BROKEN_TEMPLE       = 0,
    WG_WORKSHOP_WESTPARK            = 1,
    WG_WORKSHOP_EASTPARK            = 2,
    WG_WORKSHOP_SUNKEN_RING         = 3,
    WG_WORKSHOP_KEEP_WEST           = 4,
    WG_WORKSHOP_KEEP_EAST           = 5,
    WG_WORKSHOP_COUNT               = 6,
};

enum
{
    WG_KEEPTOWER_WEST               = 0,
    WG_KEEPTOWER_SOUTHWEST          = 1,
    WG_KEEPTOWER_SOUTHEAST          = 2,
    WG_KEEPTOWER_EAST               = 3,
    WG_KEEPTOWER_COUNT              = 4,
};

enum
{
    WG_TOWER_SHADOWSIGHT            = 0,
    WG_TOWER_WINTERS_EDGE           = 1,
    WG_TOWER_FLAMEWATCH             = 2,
    WG_TOWER_COUNT                  = 3,
};

/***************************/

class WGWorkShop : public BFObject
{
    public:
        WGWorkShop(uint32 _id, BattleField* _opvp) : BFObject(_id, _opvp) { }

        void InitFor(TeamIndex teamIdx, bool reset = true) override;
        void SendUpdateWorldState();

        ObjectGuid capturePoint;
        uint32 gy;
 };

class WGTower : public BFObject
{
    public:
        WGTower(uint32 _id, BattleField* _opvp) : BFObject(_id, _opvp) { }

        void InitFor(TeamIndex teamIdx, bool reset = true) override;
        void SpawnCannons(bool despawn = false);

        std::list<ObjectGuid> cannons;
};

/***************************/

enum
{
    WG_LOCATION_HORDE_LANDING               = 0,
    WG_LOCATION_BROKEN_TEMPLE_WORKSHOP      = 1,
    WG_LOCATION_SHADOWSIGHT_TOWER           = 2,

    WG_LOCATION_WESTPARK_WORKSHOP           = 3,
    WG_LOCATION_WINTERS_EDGE_TOWER          = 4,
    WG_LOCATION_EASTPARK_WORKSHOP           = 5,

    WG_LOCATION_FLAMEWATCH_TOWER            = 6,
    WG_LOCATION_SUNKEN_RING                 = 7,
    WG_LOCATION_ALLIANCE_LANDING            = 8,

    WG_LOCATION_KEEP_TOWER_WEST             = 9,
    WG_LOCATION_KEEP_TOWER_SOUTHWEST        = 10,
    WG_LOCATION_KEEP_TOWER_SOUTHEAST        = 11,
    WG_LOCATION_KEEP_TOWER_EAST             = 12,
    
    WG_LOCATION_COUNT                       = 13,
};

typedef struct
{
    float x;
    float y;
    float z;
    float destX;
    float destY;
    float destZ;
} WGTeleporterDataStruct;

#define WG_TELEPORTS_COUNT 12
const WGTeleporterDataStruct WGTeleportLocations[WG_TELEPORTS_COUNT] =
{
    { 5314.72f, 3055.77f, 409.08f, 5306.16f, 3020.84f, 411.44f },
    { 5269.23f, 3013.97f, 408.99f, 5306.16f, 3020.84f, 411.44f },

    { 5197.01f, 2944.94f, 409.41f, 5190.47f, 2906.85f, 410.23f },
    { 5153.38f, 2901.42f, 409.41f, 5190.47f, 2906.85f, 410.23f },

    { 5154.08f, 2781.68f, 409.46f, 5192.29f, 2775.30f, 410.28f },
    { 5196.66f, 2737.46f, 409.40f, 5192.29f, 2775.30f, 410.28f },

    { 5268.90f, 2666.41f, 409.31f, 5307.62f, 2659.02f, 410.02f },
    { 5311.56f, 2618.91f, 409.31f, 5307.62f, 2659.02f, 410.02f },

    { 5391.61f, 2828.06f, 418.89f, 5401.91f, 2829.90f, 418.75f },
    { 5401.46f, 2853.68f, 418.89f, 5392.81f, 2854.02f, 418.75f },

    { 5316.25f, 2977.04f, 408.53f, 5246.91f, 2978.32f, 409.27f },  // West vehicle teleporter
    { 5314.50f, 2703.68f, 408.54f, 5249.89f, 2703.11f, 409.27f },  // East vehicle teleporter
};

typedef struct
{
    float x;
    float y;
    union
    {
        float radius;
        float z;
    };
} WGLocationStruct;

const WGLocationStruct WGLocations[WG_LOCATION_COUNT] =
{
    { 5027.66f, 3677.15f, 150.0f},
    { 4939.61f, 3338.62f, 150.0f},
    { 4563.25f, 3617.49f, 170.0f},

    { 4391.61f, 3299.68f, 150.0f},
    { 4399.34f, 2822.57f, 170.0f},
    { 4396.31f, 2356.58f, 150.0f},

    { 4459.76f, 1947.62f, 270.0f},
    { 4918.19f, 2422.81f, 170.0f},
    { 5093.53f, 2177.48f, 150.0f},

    { 5280.27f, 3046.86f, 40.0f },
    { 5162.81f, 2934.35f, 40.0f },
    { 5164.33f, 2747.60f, 40.0f },
    { 5279.29f, 2632.03f, 40.0f },
};

#define KICK_POSITIONS_COUNT 3

const WGLocationStruct WGKickPositions[KICK_POSITIONS_COUNT] =
{
    { 4804.66f, 4169.23f, 14.93f },
    { 3960.57f, 2848.67f, 213.53f },
    { 5707.41f, 2673.68f, 709.26f },
};

enum WGRank
{
    WG_RANK_NONE        = 0,
    WG_RANK_RECRUIT     = 1,
    WG_RANK_CORPORAL    = 2,
    WG_RANK_LIEUTENANT  = 3,
};

class WGPlayerScore : public BFPlayerScore
{
    public:
        WGPlayerScore() : BFPlayerScore(), rank(0) { }

        WGRank GetRank() const;
        uint32 rank;
};

class MANGOS_DLL_SPEC BattleFieldWG : public BattleField
{
    public:
        BattleFieldWG(uint32 id);
        ~BattleFieldWG();

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

        uint32 GetWorkshopsOwnedBy(TeamIndex teamIdx)
        {
            uint32 count = 0;
            for (uint8 i = 0; i < WG_WORKSHOP_COUNT; ++i)
                if (m_workshops[i]->owner == teamIdx)
                    ++count;

            return count;
        }

        void GraveYardChanged(uint8 id, TeamIndex newOwner) override;

        TeamIndex GetWorkshopOwner(uint8 idx);

        WGRank GetPlayerRank(Player* player);

        bool CanBuildMoreVehicles(TeamIndex teamIdx);

        void OnPlayerGroupDisband(Player* plr) override;
        bool CriteriaMeets(uint32 criteriaId, Player* plr) override;

        bool InitOutdoorPvPArea() override;

        std::list<std::string> GetPlayerDebugInfo(Player* plr);
        std::list<std::string> GetStatusDebugInfo();
        std::list<std::string> GetTowerDebugInfo();
        std::list<std::string> GetWallDebugInfo();

    private:
        uint32 GetWorldStateFor(uint32 entry) const
        {
            for (int i = 0; i < countof(WGWorldstateByGOEntry); ++i)
                if (WGWorldstateByGOEntry[i].entry == entry)
                    return WGWorldstateByGOEntry[i].worldState;

            return 0;
        }

        uint8 GetLocationFor(float x, float y)
        {
            float minDist = 999999.0f;
            uint8 minItr = WG_LOCATION_COUNT;
            float rad;
            for (uint8 i = 0; i < WG_LOCATION_COUNT; ++i)
            {
                rad = sqrt((WGLocations[i].x - x) * (WGLocations[i].x - x) + (WGLocations[i].y - y) * (WGLocations[i].y - y));
                if (rad < WGLocations[i].radius && rad < minDist)
                {
                    minDist = rad;
                    minItr = i;
                }
            }

            return minItr;
        }

        uint8 GetRemainingTowers() { return 3 - m_destroyedTowers[m_defender]; }

        bool CanDamageGO(GameObject* pGo, Player* invoker) override;

        void SetupPlayerPosition(Player* player) override;

        /***** Buff Helpers *****/

        void UpdateTowerControllBuff(Player* plr = NULL);
        void UpdateTenacities();

    public:
        void UpdateScoreBuff(Player* plr);

        /************************/

    private:
        void InitPlayerScore(Player* plr) override;
        bool UpdatePlayerScores() override;
        bool GetKickPosition(Player* plr, float& x, float& y, float& z) override;

        void SendCaptainYell();

        bool b10MinAchiev;

        // counters
        uint32 m_damagedTowers[PVP_TEAM_COUNT];
        uint32 m_destroyedTowers[PVP_TEAM_COUNT];

        std::list<ObjectGuid> m_zoneTrashGUIDs;
        std::list<ObjectGuid> m_keepCannonGUIDs;
        std::list<ObjectGuid> m_portalGUIDs;
        std::list<ObjectGuid> m_vehicleGUIDs[PVP_TEAM_COUNT];

        ObjectGuid m_keepDoorGUID;
        ObjectGuid m_keepGateGUID;
        ObjectGuid m_titanRelicGUID;
        ObjectGuid m_zannethGuid;
        ObjectGuid m_dardoshGuid;

        std::map<uint8, WGTower*> m_keepTowers;
        std::list<BFObject*> m_keepWalls;
        std::map<uint8, WGTower*> m_towers;
        std::map<uint8, WGWorkShop*> m_workshops;
};

#endif
