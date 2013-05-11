/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER                       = 7,

    TYPE_ENCOUNTER_MASK                 = 0,
    TYPE_BAINE                          = 1,
    TYPE_JAINA                          = 2,
    TYPE_SYLVANAS                       = 3,
    TYPE_TYRANDE                        = 4,
    TYPE_MUROZOND                       = 5,
    TYPE_FRAGMENTS                      = 6,

    NPC_ECHO_OF_BAINE                   = 54431,
    NPC_ECHO_OF_JAINA                   = 54445,
    NPC_ECHO_OF_SYLVANAS                = 54123,
    NPC_ECHO_OF_TYRANDE                 = 54544,
    NPC_MUROZOND                        = 54432,
    NPC_JAINA_CIRCLE_VISUAL             = 54639,

    GO_MUROZOND_CACHE                   = 209547,
    GO_HOURGLASS_OF_TIME                = 209249,

    GO_BAINE_PLATFORM_01                = 209693,
    GO_BAINE_PLATFORM_02                = 209694,
    GO_BAINE_PLATFORM_03                = 209695,
    GO_BAINE_PLATFORM_04                = 209670,

    SPELL_TELEPORT_ENTRANCE             = 102564,
    SPELL_TELEPORT_BLUE_DRAGONSHRINE    = 102126,
    SPELL_TELEPORT_RUBY_DRAGONSHRINE    = 102579,
    SPELL_TELEPORT_BLACK_DRAGONSHRINE   = 103868,
    SPELL_TELEPORT_EMERALD_DRAGONSHRINE = 104761,
    SPELL_TELEPORT_BRONZE_DRAGONSHRINE  = 104764,

    SPELL_SANDS_OF_THE_HOURGLASS        = 102668,   // progress bar
    SPELL_REWIND_TIME                   = 101590,   // casted by go
    MAX_HOURGLASS_USES                  = 5,

    GO_STAFF_FRAGMENT                   = 209318,
    MAX_FRAGMENTS_COUNT                 = 16,

    WORLDSTATE_FRAGMENTS_COLLECTED      = 6025,
    WORLDSTATE_SHOW_FRAGMENTS           = 6046,

    AREA_AZURE_DRAGONSHRINE             = 5793,
};

struct SaveStruct;

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

        void OnHourglassUse(Player* who);

        void OnPlayerEnter(Player* who) override;
        void OnPlayerLeave(Player* who) override;

        void OnPlayerEnterArea(Player* who, uint32 uiNewAreaId) override;

        int32 hourglassUseCount;
        std::list<ObjectGuid> bainePlatforms;
        std::map<uint32, SaveStruct> savedData;

    private:

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

