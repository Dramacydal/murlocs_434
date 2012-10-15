/* Copyright (C) 2006 - 2011 MaXx2021 && /dev/rsa for ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_HALL_OF_REFLECTION_H
#define DEF_HALL_OF_REFLECTION_H
#include "BSW_ai.h"
#include "BSW_instance.h"

enum Data
{
    TYPE_PHASE              = 0,
    TYPE_EVENT              = 1,
    TYPE_FALRIC             = 2,
    TYPE_MARWYN             = 3,
    TYPE_FROST_GENERAL      = 4,
    TYPE_LICH_KING          = 5,
    TYPE_HALLS              = 6,
    MAX_ENCOUNTERS,

    DATA_ESCAPE_LIDER       = 101,
    DATA_SUMMONS            = 102,
    DATA_WAVE_COUNT         = 103,

    /*UNITS*/
    NPC_DARK_1                   = 38177, //Shadowy Mercenary
    NPC_DARK_2                   = 38176, //Tortured Rifleman
    NPC_DARK_3                   = 38173, //Spectral Footman
    NPC_DARK_4                   = 38172, //Phantom Mage
    NPC_DARK_5                   = 38567, //Phantom Hallucination
    NPC_DARK_6                   = 38175, //Ghostly Priest
    NPC_JAINA                    = 37221,
    NPC_SYLVANA                  = 37223,
    NPC_JAINA_OUTRO              = 36955,
    NPC_SYLVANA_OUTRO            = 37554,
    NPC_ALTAR_TARGET             = 37704,
    NPC_UTHER                    = 37225,
    NPC_LICH_KING                = 36954,
    BOSS_LICH_KING               = 37226,
    NPC_ICE_WALL                 = 37014,
    NPC_FALRIC                   = 38112,
    NPC_MARWYN                   = 38113,
    NPC_GHOSTLY_ROGUE            = 38177,
    NPC_GHOSTLY_PRIEST           = 38175,
    NPC_GHOSTLY_MAGE             = 38172,
    NPC_GHOSTLY_FOOTMAN          = 38173,
    NPC_GHOSTLY_RIFLEMAN         = 38176,
    NPC_GLUK                     = 38567,

    NPC_RAGING_GNOUL             = 36940,
    NPC_RISEN_WITCH_DOCTOR       = 36941,
    NPC_ABON                     = 37069,

    NPC_FROST_GENERAL            = 36723,
    NPC_SPIRITUAL_REFLECTION     = 37068,

//    NPC_QUEL_DELAR               = 37704,
    NPC_QUEL_DELAR               = 37158,

    GO_ICECROWN_DOOR             = 201976, //72802
    GO_ICECROWN_DOOR_2           = 197342,
    GO_ICECROWN_DOOR_3           = 197343,
    GO_IMPENETRABLE_DOOR         = 197341, //72801
    GO_FROSTMOURNE_ALTAR         = 202236, //3551
    GO_FROSTMOURNE               = 202302, //364

    GO_ICE_WALL                  = 201385,
    GO_CAVE                      = 201596,
    GO_PORTAL                    = 202079,

    GO_CAPTAIN_CHEST_1           = 202212, //3145
    GO_CAPTAIN_CHEST_2           = 201710, //30357
    GO_CAPTAIN_CHEST_3           = 202337, //3246
    GO_CAPTAIN_CHEST_4           = 202336, //3333
};

struct _Locations
{
    float x, y, z, o;
    uint32 id;
};

static _Locations SpawnLoc[]=
{
    //Wing 01
    {5277.409f, 1993.161f, 707.694f, 0.05f}, //27
    {5301.876f, 2041.699f, 707.694f, 4.71f}, //1
    {5339.830f, 2020.887f, 707.694f, 3.14f}, //13
    {5311.041f, 2042.935f, 707.694f, 4.71f}, //3
    {5314.750f, 2039.969f, 707.694f, 4.71f}, //4
    {5342.823f, 2003.801f, 707.694f, 3.14f}, //10
    {5311.579f, 1972.823f, 707.694f, 1.62f}, //16

    //Wing 02
    {5272.491f, 2005.673f, 707.694f, 0.05f}, //23
    {5302.669f, 1973.050f, 707.694f, 1.62f}, //18
    {5346.187f, 2008.058f, 707.694f, 3.14f}, //9
    {5319.752f, 2041.321f, 707.694f, 4.71f}, //5
    {5344.882f, 1998.714f, 707.694f, 3.14f}, //11
    {5340.552f, 1994.735f, 707.694f, 3.14f}, //12
    {5306.441f, 2040.358f, 707.694f, 4.71f}, //2

    //Wing 03
    {5273.297f, 2014.009f, 707.694f, 0.05f}, //25
    {5316.062f, 1970.777f, 707.694f, 1.62f}, //15
    {5322.498f, 2037.415f, 707.694f, 4.71f}, //6
    {5307.087f, 1970.065f, 707.694f, 1.62f}, //17
    {5342.460f, 2012.391f, 707.694f, 3.14f}, //8
    {5297.601f, 1971.420f, 707.694f, 1.62f}, //19
    {5295.668f, 1975.853f, 707.694f, 1.62f}, //20

    //Wing 04
    {5273.661f, 1996.767f, 707.694f, 0.05f}, //21
    {5275.228f, 2001.275f, 707.694f, 0.05f}, //22
    {5344.153f, 2017.753f, 707.694f, 3.14f}, //7
    {5275.310f, 2009.686f, 707.694f, 0.05f}, //24
    {5319.835f, 1975.177f, 707.694f, 1.62f}, //14
    {5277.445f, 2017.197f, 707.694f, 0.05f}, //26
    {5298.198f, 2037.762f, 707.694f, 4.71f}  //0
};

static _Locations WallLoc[]=
{
    {5540.39f, 2086.48f, 731.066f, 1.00057f},
    {5494.3f, 1978.27f, 736.689f, 1.0885f},
    {5434.27f, 1881.12f, 751.303f, 0.923328f},
    {5323.61f, 1755.85f, 770.305f, 0.784186f},
    {5239.01f, 1932.64f, 707.695f, 0.8f},       // Spawn point for Jaina && Silvana intro
    {5266.779785f, 1953.42f, 707.697f, 1.0f},
    {5547.27002f, 2256.949951f, 733.010986f, 0.9f},   // Spawn point for Jaina && Silvana outro
};

enum HallsOfReflectionWorldStates
{
    WORLD_STATE_HOR                               = 4884,
    WORLD_STATE_HOR_WAVE_COUNT                    = 4882,
};

#endif
