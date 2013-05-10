/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_LOST_CITY_OF_TOLVIR_H
#define DEF_LOST_CITY_OF_TOLVIR_H

enum
{
    MAX_ENCOUNTER               = 4,

    TYPE_HUSAM                  = 0,
    TYPE_BARIM                  = 1,
    TYPE_LOCKMAW                = 2,
    TYPE_SIAMAT                 = 3,

    NPC_HUSAM                   = 44577,
    NPC_BARIM                   = 43612,
    NPC_AUGH                    = 49045,
    NPC_LOCKMAW                 = 43614,
    NPC_SIAMAT                  = 44819,

    // Trash mobs
    NPC_MINION_OF_SIAMAT_STORM  = 44713,
    NPC_MINION_OF_SIAMAT        = 44704,
    NPC_NEFERSET_DARKCASTER     = 44982,
    NPC_NEFERSET_PLAGUEBRINGER  = 44976,
    NPC_NEFERSET_THEURGIST      = 44980,
    NPC_NEFERSET_TORTURER       = 44977,
    NPC_OATHSWORN_AXEMASTER     = 44922,
    NPC_OATHSWORN_CAPTAIN       = 45122,
    NPC_OATHSWORN_MYRMIDON      = 44924,
    NPC_OATHSWORN_PATHFINDER    = 44932,
    NPC_OATHSWORN_SCORPID_KEEPER = 45062,
    NPC_OATHSWORN_SKINNER       = 44981,
    NPC_OATHSWORN_TAMER         = 45097,
    NPC_WANDERER                = 44926,
    NPC_PYGMY_BRUTE             = 44896,
    NPC_PYGMY_FIREBREATHER      = 44898,
    NPC_PYGMY_SCOUT             = 44897,
    NPC_SERVANT_OF_SIAMAT       = 45269,
    NPC_SERVANT_OF_SIAMAT_NORMAL = 45269,
    NPC_SERVANT_OF_SIAMAT_HEROIC = 45269,
    NPC_SERVANT_OF_SIAMAT_FINAL = 45269,

    // Various NPCs

    NPC_CAPTAIN_HADAN           = 50038,
    NPC_WIND_TUNNEL             = 48092,

    GO_SIAMAT_PLATFORM          = 205365,
};

class MANGOS_DLL_DECL instance_lost_city_of_tolvir : public ScriptedInstance
{
    public:
        instance_lost_city_of_tolvir(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) override;

        const char* Save() override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:

        bool SiamatAvailable();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;

        std::list<ObjectGuid> tunnelList;
};

#endif
