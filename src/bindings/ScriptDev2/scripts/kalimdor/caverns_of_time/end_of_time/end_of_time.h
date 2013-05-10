/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER                       = 6,

    TYPE_ENCOUNTER_MASK                 = 0,
    TYPE_BAINE                          = 1,
    TYPE_JAINA                          = 2,
    TYPE_SYLVANAS                       = 3,
    TYPE_TYRANDE                        = 4,
    TYPE_MUROZOND                       = 5,

    NPC_ECHO_OF_BAINE                   = 54431,
    NPC_ECHO_OF_JAINA                   = 54445,
    NPC_ECHO_OF_SYLVANAS                = 54123,
    NPC_ECHO_OF_TYRANDE                 = 54544,
    NPC_MUROZOND                        = 54432,

    SPELL_TELEPORT_ENTRANCE             = 102564,
    SPELL_TELEPORT_BLUE_DRAGONSHRINE    = 102126,
    SPELL_TELEPORT_RUBY_DRAGONSHRINE    = 102579,
    SPELL_TELEPORT_BLACK_DRAGONSHRINE   = 103868,
    SPELL_TELEPORT_EMERALD_DRAGONSHRINE = 104761,
    SPELL_TELEPORT_BRONZE_DRAGONSHRINE  = 104764,

    GO_STAFF_FRAGMENT                   = 209318,
};

class MANGOS_DLL_DECL instance_end_of_time : public ScriptedInstance
{
    public:
        instance_end_of_time(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) override;

        const char* Save() override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

