/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef SC_SCRIPTMGR_H
#define SC_SCRIPTMGR_H

#include "Common.h"
#include "DBCStructure.h"
#include "Database/DatabaseEnv.h"
#include "Util.h"

class Player;
class Creature;
class CreatureAI;
class InstanceData;
class Quest;
class Item;
class GameObject;
class SpellCastTargets;
class Map;
class Unit;
class WorldObject;
class Aura;
class Object;
class ObjectGuid;

// *********************************************************
// ************** Some defines used globally ***************

// Basic defines
#define VISIBLE_RANGE       (166.0f)                        // MAX visible range (size of grid)
#define DEFAULT_TEXT        "<ScriptDev2 Text Entry Missing!>"

// Some typedefs for storing Guids
typedef std::list<ObjectGuid> GUIDList;
typedef std::set<ObjectGuid> GUIDSet;
typedef std::vector<ObjectGuid> GUIDVector;
typedef std::map<uint32, ObjectGuid> EntryGuidMap;

/* Escort Factions
 * TODO: find better namings and definitions.
 * N=Neutral, A=Alliance, H=Horde.
 * NEUTRAL or FRIEND = Hostility to player surroundings (not a good definition)
 * ACTIVE or PASSIVE = Hostility to environment surroundings.
 */
enum EscortFaction
{
    FACTION_ESCORT_A_NEUTRAL_PASSIVE    = 10,
    FACTION_ESCORT_H_NEUTRAL_PASSIVE    = 33,
    FACTION_ESCORT_N_NEUTRAL_PASSIVE    = 113,

    FACTION_ESCORT_A_NEUTRAL_ACTIVE     = 231,
    FACTION_ESCORT_H_NEUTRAL_ACTIVE     = 232,
    FACTION_ESCORT_N_NEUTRAL_ACTIVE     = 250,

    FACTION_ESCORT_N_FRIEND_PASSIVE     = 290,
    FACTION_ESCORT_N_FRIEND_ACTIVE      = 495,

    FACTION_ESCORT_A_PASSIVE            = 774,
    FACTION_ESCORT_H_PASSIVE            = 775,

    FACTION_ESCORT_N_ACTIVE             = 1986,
    FACTION_ESCORT_H_ACTIVE             = 2046
};

// *********************************************************
// ************* Some structures used globally *************

struct Script
{
    Script() :
        pGossipHello(NULL), pGossipHelloGO(NULL), pGossipSelect(NULL), pGossipSelectGO(NULL),
        pGossipSelectWithCode(NULL), pGossipSelectGOWithCode(NULL),
        pDialogStatusNPC(NULL), pDialogStatusGO(NULL),
        pQuestAcceptNPC(NULL), pQuestAcceptGO(NULL), pQuestAcceptItem(NULL),
        pQuestRewardedNPC(NULL), pQuestRewardedGO(NULL),
        pGOUse(NULL), pItemUse(NULL), pAreaTrigger(NULL), pProcessEventId(NULL),
        pEffectDummyNPC(NULL), pEffectDummyGO(NULL), pEffectDummyItem(NULL), pEffectAuraDummy(NULL),
        GetAI(NULL), GetInstanceData(NULL)
    {}

    std::string Name;

    bool (*pGossipHello             )(Player*, Creature*);
    bool (*pGossipHelloGO           )(Player*, GameObject*);
    bool (*pGossipSelect            )(Player*, Creature*, uint32, uint32);
    bool (*pGossipSelectGO          )(Player*, GameObject*, uint32, uint32);
    bool (*pGossipSelectWithCode    )(Player*, Creature*, uint32, uint32, const char*);
    bool (*pGossipSelectGOWithCode  )(Player*, GameObject*, uint32, uint32, const char*);
    uint32 (*pDialogStatusNPC       )(Player*, Creature*);
    uint32 (*pDialogStatusGO        )(Player*, GameObject*);
    bool (*pQuestAcceptNPC          )(Player*, Creature*, Quest const*);
    bool (*pQuestAcceptGO           )(Player*, GameObject*, Quest const*);
    bool (*pQuestAcceptItem         )(Player*, Item*, Quest const*);
    bool (*pQuestRewardedNPC        )(Player*, Creature*, Quest const*);
    bool (*pQuestRewardedGO         )(Player*, GameObject*, Quest const*);
    bool (*pGOUse                   )(Player*, GameObject*);
    bool (*pItemUse                 )(Player*, Item*, SpellCastTargets const&);
    bool (*pAreaTrigger             )(Player*, AreaTriggerEntry const*);
    bool (*pProcessEventId          )(uint32, Object*, Object*, bool);
    bool (*pEffectDummyNPC          )(Unit*, uint32, SpellEffectIndex, Creature*);
    bool (*pEffectDummyGO           )(Unit*, uint32, SpellEffectIndex, GameObject*);
    bool (*pEffectDummyItem         )(Unit*, uint32, SpellEffectIndex, Item*);
    bool (*pEffectAuraDummy         )(const Aura*, bool);

    CreatureAI* (*GetAI             )(Creature*);
    InstanceData* (*GetInstanceData )(Map*);

    void RegisterSelf(bool bReportError = true);
};

// *********************************************************
// ************* Some functions used globally **************

// Generic scripting text function
void DoScriptText(int32 iTextEntry, WorldObject* pSource, Unit* pTarget = NULL);
void DoOrSimulateScriptTextForMap(int32 iTextEntry, uint32 uiCreatureEntry, Map* pMap, Creature* pCreatureSource = NULL, Unit* pTarget = NULL);

//DB query
QueryResult* strSD2Pquery(char*);

// Not registered scripts storage
Script* GetScriptByName(std::string scriptName);

template<class T>
inline
const T& RAND(const T& v1, const T& v2)
{
    return (urand(0, 1)) ? v1 : v2;
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3)
{
    switch (urand(0, 2))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4)
{
    switch (urand(0, 3))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5)
{
    switch (urand(0, 4))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6)
{
    switch (urand(0, 5))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7)
{
    switch (urand(0, 6))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8)
{
    switch (urand(0, 7))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9)
{
    switch (urand(0, 8))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10)
{
    switch (urand(0, 9))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11)
{
    switch (urand(0, 10))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12)
{
    switch (urand(0, 11))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13)
{
    switch (urand(0, 12))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14)
{
    switch (urand(0, 13))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14, const T& v15)
{
    switch (urand(0, 14))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
        case 14: return v15;
    }
}

template<class T>
inline
const T& RAND(const T& v1, const T& v2, const T& v3, const T& v4, const T& v5, const T& v6, const T& v7, const T& v8,
              const T& v9, const T& v10, const T& v11, const T& v12, const T& v13, const T& v14, const T& v15, const T& v16)
{
    switch (urand(0, 15))
    {
        default:
        case 0: return v1;
        case 1: return v2;
        case 2: return v3;
        case 3: return v4;
        case 4: return v5;
        case 5: return v6;
        case 6: return v7;
        case 7: return v8;
        case 8: return v9;
        case 9: return v10;
        case 10: return v11;
        case 11: return v12;
        case 12: return v13;
        case 13: return v14;
        case 14: return v15;
        case 15: return v16;
    }
}

// *********************************************************
// **************** Internal hook mechanics ****************

#if COMPILER == COMPILER_GNU
#define FUNC_PTR(name,callconvention,returntype,parameters)    typedef returntype(*name)parameters __attribute__ ((callconvention));
#else
#define FUNC_PTR(name, callconvention, returntype, parameters)    typedef returntype(callconvention *name)parameters;
#endif

#endif
