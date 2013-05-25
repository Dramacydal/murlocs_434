/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

enum
{
    MAX_ENCOUNTER               = 3,

    TYPE_ARCURION               = 0,
    TYPE_ASIRA                  = 1,
    TYPE_BENEDICTUS             = 2,

    NPC_ARCURION                = 54590,
    NPC_ASIRA                   = 54968,
    NPC_BENEDICTUS              = 54938,

    GO_ICE_WALL                 = 210048,
};

class MANGOS_DLL_DECL instance_hour_of_twilight : public ScriptedInstance
{
    public:
        instance_hour_of_twilight(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnCreatureDeath(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) override;

        const char* Save() override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

