/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER               = 7,

    TYPE_COMMANDER_ULTHOK       = 0,
    TYPE_LADY_NAZJAR            = 1,
    TYPE_MINDBENDER_GHURSHA     = 2,
    TYPE_NEPTULON_EVENT         = 3,
    TYPE_OZUMAT                 = 4,
    TYPE_LADY_NAZJAR_EVENT      = 5,
    TYPE_COMMANDER_ULTHOK_EVENT = 6,

    NPC_LADY_NAZJAR_EVENT       = 39959,
    NPC_LADY_NAZJAR             = 40586,
    NPC_COMMANDER_ULTHOK        = 40765,
    NPC_ERUNAK_STONESPEAKER     = 40825,
    NPC_MINDBENDER_GHURSHA      = 40788,
    NPC_OZUMAT                  = 42172,
    NPC_NEPTULON                = 40792,
    NPC_CAPTAIN_TAYLOR          = 50270,
    NPC_LEGIONNAIRE_NAZGRIM     = 50272,

    NPC_TELEPORTER_1            = 43403,
    NPC_TELEPORTER_2            = 51391,
    NPC_TELEPORTER_3            = 51395,

    GO_COMMANDER_ULTHOK_DOOR    = 204338,
    GO_CORALES                  = 205542,
    GO_LADY_NAZJAR_DOOR         = 204339,
    GO_ERUNAK_STONESPEAKER_DOOR = 204340,
    GO_OZUMAT_DOOR              = 204341,
    GO_NEPTULON_CACHE           = 205216,
    GO_NEPTULON_CACHE_H         = 207973,
    GO_TENTACLE_RIGHT           = 208302,
    GO_TENTACLE_LEFT            = 208301,
    GO_CONTROL_SYSTEM           = 203199,
    GO_INVISIBLE_DOOR_1         = 207997,
    GO_INVISIBLE_DOOR_2         = 207998,
};

class MANGOS_DLL_DECL instance_throne_of_the_tides : public ScriptedInstance
{
    public:
        instance_throne_of_the_tides(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) override;

        const char* Save() override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:

        bool m_bIsRegularMode;
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

