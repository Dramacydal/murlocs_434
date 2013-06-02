/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER               = 3,

    TYPE_ARGALOTH               = 0,
    TYPE_OCCUTHAR               = 1,
    TYPE_ALIZABAL               = 2,

    GO_DOOR_ARGALOTH            = 207619,
    GO_DOOR_OCCUTHAR            = 208953,
    GO_DOOR_ALIZABAL            = 209849,
};

class MANGOS_DLL_DECL instance_baradin_hold : public ScriptedInstance
{
    public:
        instance_baradin_hold(Map* pMap);

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

