/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER           = 3,

    TYPE_ETAN               = 0,
    TYPE_ALTAIRUS           = 1,
    TYPE_ASSAD              = 2,

    NPC_ERTAN               = 43878,
    NPC_ALTAIRUS            = 43873,
    NPC_ASAAD               = 43875,

    NPC_SLIPSTREAM          = 45455,
};

class MANGOS_DLL_DECL instance_vortex_pinnacle : public ScriptedInstance
{
    public:
        instance_vortex_pinnacle(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) override;

        const char* Save() override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:

        std::list<ObjectGuid> m_streamGuds[MAX_ENCOUNTER];

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

