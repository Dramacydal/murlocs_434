/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_VIOLET_HOLD_H
#define DEF_VIOLET_HOLD_H

enum
{
    MAX_ENCOUNTER           = 11,

    TYPE_EVENT              = 0,
    TYPE_RIFT               = 1,
    TYPE_EREKEM             = 2,
    TYPE_MORAGG             = 3,
    TYPE_ICHORON            = 4,
    TYPE_XEVOZZ             = 5,
    TYPE_LAVANTHOR          = 6,
    TYPE_ZURAMAT            = 7,
    TYPE_CYANIGOSA          = 8,
    TYPE_PORTAL6            = 9,
    TYPE_PORTAL12           = 10,


    WORLD_STATE_VH          = 3816,
    WORLD_STATE_VH_PRISON   = 3815,
    WORLD_STATE_VH_PORTALS  = 3810,

    TYPE_LASTBOSS           = 19,
    TYPE_DOOR               = 21,
    TYPE_PORTAL_TIME        = 22,

    TYPE_DISRUPTIONS        = 101,
    TYPE_LASTBOSS_ID        = 102,

    DATA_EREKEM             = 23,
    DATA_MORAGG             = 24,
    DATA_ICHORON            = 25,
    DATA_XEVOZZ             = 26,
    DATA_LAVANTHOR          = 27,
    DATA_ZURAMAT            = 28,
    DATA_SINCLARI           = 29,
    DATA_BOSSTIME           = 30,

    NPC_EREKEM             = 29315,
    NPC_EREKEM_GUARD       = 29395,
    NPC_MORAGG             = 29316,
    NPC_ICHORON            = 29313,
    NPC_XEVOZZ             = 29266,
    NPC_LAVANTHOR          = 29312,
    NPC_ZURAMAT            = 29314,
    NPC_CYANIGOSA          = 31134,

    NPC_AZURE_SABOTEUR     = 31079, // Open boss's cell

    NPC_AZURE_CAPTAIN      = 30666, //Melee, 40k - 60k hp, 3 for 1-11, 4 for 13-17
    NPC_AZURE_RAIDER       = 30668, //Melee, 40k - 60k hp
    NPC_AZURE_SORCEROR     = 30667, //Caster, 40k - 60k hp
    NPC_AZURE_STALKER      = 32191, //Melee, 40k - 60k hp
    NPC_GUARDIAN           = 30660, 
    NPC_KEEPER             = 30695, 
    NPC_AZURE_BINDER       = 30663, //Caster, 7k - 10k hp
    NPC_AZURE_INVADER      = 30661, //Melee, 8k - 12k hp
    NPC_AZURE_MAGE_SLAYER  = 30664, //Melee, 10k - 15k hp
    NPC_AZURE_SPELLBREAKER = 30662, //Caster, 10k - 15k hp

    NPC_SINCLARI           = 30658,
    NPC_GUARD              = 30659,
    NPC_PORTAL             = 31011,
    NPC_DOOR_SEAL          = 30896,

    GO_DOOR_SEAL           = 191723,
    GO_DOOR_EREKEM         = 191564,
    GO_DOOR_EREKEM_RIGHT   = 191563,
    GO_DOOR_EREKEM_LEFT    = 191562,
    GO_DOOR_MORAGG         = 191606,
    GO_DOOR_ICHORON        = 191722,
    GO_DOOR_XEVOZZ         = 191556,
    GO_DOOR_LAVANTHOR      = 191566,
    GO_DOOR_ZURAMAT        = 191565,

    SPELL_PORTAL_CHANNEL   = 58012,
    SPELL_CORRUPT          = 58040,

    ENCOUNTER_CREDIT_1ST_BOSS = 29315,
    ENCOUNTER_CREDIT_2ND_BOSS = 29316,

    EMOTE_GUARDIAN_PORTAL       = -1608005,
    EMOTE_DRAGONFLIGHT_PORTAL   = -1608006,
    EMOTE_KEEPER_PORTAL         = -1608007
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

struct WayPoints
{
    WayPoints(uint32 _id, float _x, float _y, float _z)
    {
        id = _id;
        x = _x;
        y = _y;
        z = _z;
    }
    uint32 id;
    float x, y, z;
};

static Locations SinclariWP[]=
{
    {1830.5f, 799.357f, 44.3418f}, // 0 use activation
    {1832.46f, 800.431f, 44.3117f}, // 1 SAY_BEGIN call back guards
    {1824.79f, 803.828f, 44.3634f}, // 2 SAY_LOCK_DOOR close door
    {1807.25f, 803.904f, 44.3634f}, // 3 
    {1808.07f, 804.259f, 44.3641f}, // 4 

};

static Locations PortalLoc[]=
{
    {1888.271f, 810.781f, 38.441f}, // 0 center
    {1857.125f, 763.295f, 38.654f}, // 1 Lavanthor
    {1925.480f, 849.981f, 47.174f}, // 2 Zuramat
    {1892.737f, 744.589f, 47.666f}, // 3 Moragg
    {1878.198f, 850.005f, 43.333f}, // 4 Portal in front of Erekem
    {1909.381f, 806.796f, 38.645f}, // 5 Portal outside of Ichoron
    {1936.101f, 802.950f, 52.417f}, // 6 at the highest platform
};

static Locations BossLoc[]=
{
    {0,0,0},
    {0,0,0},
    {1876.100f, 857.079f, 43.333f}, // Erekem
    {1892.737f, 744.589f, 47.666f}, // Moragg
    {1908.863f, 785.647f, 37.435f}, // Ichoron
    {1905.364f, 840.607f, 38.670f}, // Xevozz
    {1857.125f, 763.295f, 38.654f}, // Lavanthor
    {1925.480f, 849.981f, 47.174f}, // Zuramat
};

static Locations DragonsWP[]=
{
    //center, ichoron
    {1869.393f, 803.902f, 38.768f}, // 0 
    {1859.843f, 804.222f, 44.008f}, // 1 
    {1829.64f,  804.194f, 44.355f}, // 2 

    //From left side (lavanthor)
    {1861.016f, 789.717f, 38.908f}, // 3 
    {1856.217f, 796.705f, 44.008f}, // 4 
    {1829.64f,  804.194f, 44.355f}, // 5 

    //From Zuramat
    {1931.446f, 826.734f, 47.556f}, // 6 
    {1913.049f, 823.930f, 38.792f}, // 7 
    {1827.960f, 804.208f, 44.364f}, // 8 
    {1869.393f, 803.902f, 38.768f}, // 9 
    {1859.843f, 804.222f, 44.008f}, // 10 
    {1829.64f,  804.194f, 44.355f}, // 11 

    //From Morag
    {1887.500f, 763.096f, 47.666f}, // 12 
    {1880.837f, 775.769f, 38.796f}, // 13 
    {1861.016f, 789.717f, 38.908f}, // 14 
    {1856.217f, 796.705f, 44.008f}, // 15 
    {1829.64f,  804.194f, 44.355f}, // 16 

    //From erekem
    {1878.280f, 843.267f, 43.333f}, // 17 
    {1872.311f, 835.531f, 38.780f}, // 18 
    {1861.997f, 818.766f, 38.650f}, // 19 
    {1857.348f, 811.230f, 44.008f}, // 20
    {1829.64f,  804.194f, 44.355f}, // 21 

    //From Highest platform
    {1937.298f, 824.557f, 52.332f}, // 22
    {1913.049f, 823.930f, 38.792f}, // 23
    {1869.393f, 803.902f, 38.768f}, // 24
    {1859.843f, 804.222f, 44.008f}, // 25
    {1829.64f,  804.194f, 44.355f}, // 26
};

#endif
