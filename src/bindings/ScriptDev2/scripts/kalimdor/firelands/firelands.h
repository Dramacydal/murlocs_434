/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BLACKROCK_CAVERNS_H
#define DEF_BLACKROCK_CAVERNS_H

enum
{
    MAX_ENCOUNTER               = 7,

    TYPE_SHANNOX                = 0,
    TYPE_RHYOLITH               = 1,
    TYPE_BETHTILAC              = 2,
    TYPE_ALYSRAZOR              = 3,
    TYPE_BALEROC                = 4,
    TYPE_STAGHELM               = 5,
    TYPE_RAGNAROS               = 6,

    // bosses
    NPC_BETHTILAC               = 52498,
    NPC_RHYOLITH                = 52558,
    NPC_ALYSRAZOR               = 52530,
    NPC_SHANNOX                 = 53691,
    NPC_BALOROC                 = 53494,
    NPC_MAJORDOMUS              = 52571,
    NPC_RAGNAROS_CATA           = 52409,

    DATA_PHASE                  = 1,

    // Shannox
    NPC_RIPLIMB                 = 53694, // Shannox Dogs
    NPC_RAGEFACE                = 53695,
    NPC_SHANNOX_SPEAR           = 53752, // Shannox Spear
    NPC_FAKE_SHANNOX_SPEAR      = 53910, 
    NPC_CRYSTAL_TRAP            = 53713,
    NPC_IMMOLATION_TRAP         = 53724,
    NPC_CRYSTAL_PRISON          = 53819,
};

class MANGOS_DLL_DECL instance_firelands : public ScriptedInstance
{
    public:
        instance_firelands(Map* pMap);

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

#endif
