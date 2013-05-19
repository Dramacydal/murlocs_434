/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER               = 5,

    TYPE_GUARDS_SLAIN           = 0,
    TYPE_ENERGY_FOCUS           = 1,
    TYPE_PEROTHARN              = 2,
    TYPE_AZSHARA                = 3,
    TYPE_MANNOROTH              = 4,

    NPC_AZSHARA                 = 54853,
    NPC_GUARDIAN_DEMON          = 54927,
    NPC_MANNOROTH               = 54969,
    NPC_PEROTHARN               = 55085,
    NPC_VAROTHEN                = 55419,
    NPC_LEGION_DEMON            = 55503,
    NPC_CORRUPTED_ARCANIST      = 55654,
    NPC_DREADLORD_DEFENDER      = 55656,
    NPC_DRAKE_VEHICLE           = 57107,

    GO_WOE_COURTYARD_DOOR       = 210084,   // before courtyard
    GO_INVISIBLE_FIRE_WALL      = 210097,   // after peroth'arn

    GO_SMALL_FIREWALL_DOOR      = 210130,
    GO_LARGE_FIREWALL_DOOR      = 210234,

    GO_PORTAL_ENERGY_FOCUS_1    = 209366,
    GO_PORTAL_ENERGY_FOCUS_2    = 209447,
    GO_PORTAL_ENERGY_FOCUS_3    = 209448,

    SPELL_SHADOWCLOAK_PERIODIC  = 103004,
    SPELL_FORCE_SHADOWCLOAK     = 105635,
    SPELL_SHADOW_WALK           = 102994,
    SPELL_SHADOWCLOAK_ILLIDAN   = 105915,

    AREATRIGGER_MEET_ILLIDAN    = 7066,

    WORLDSTATE_DEMONS_PRESENT   = 6017,
    WORLDSTATE_DEMONS_SHOW      = 6018,

    MAX_FOCUS                   = 3,
    MAX_GUARDS                  = 2,
};

class MANGOS_DLL_DECL instance_well_of_eternity : public ScriptedInstance
{
    public:
        instance_well_of_eternity(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) override;

        const char* Save() override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:

        GuidList courtyardDoors;
        GuidList perotharnDoors;
        GuidList energyFocusDoors;
        GuidList guardianDemons;
        GuidList drakes;

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

