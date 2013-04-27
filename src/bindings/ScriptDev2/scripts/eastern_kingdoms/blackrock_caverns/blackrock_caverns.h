/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BLACKROCK_CAVERNS_H
#define DEF_BLACKROCK_CAVERNS_H

enum
{
    MAX_ENCOUNTER               = 5,

    TYPE_ROMOGG                 = 0,
    TYPE_CORLA                  = 1,
    TYPE_KARSH                  = 2,
    TYPE_BEAUTY                 = 3,
    TYPE_OBSIDIUS               = 4,

    NPC_ROMOGG                  = 39665,
    NPC_CORLA                   = 39679,
    NPC_KARSH                   = 39698,
    NPC_BEAUTY                  = 39700,
    NPC_OBSIDIUS                = 39705,

    // Trash mobs
    NPC_BUSTER                  = 40013,
    NPC_CONFLAGRATION           = 39994,
    NPC_CRAZED_MAGE             = 39982,
    NPC_DEFILED_EARTH_RAGER     = 40023,
    NPC_EVOLVED_TWILIGHT_ZEALOT = 39987,
    NPC_INCENDIARY_SPARK        = 40021,
    NPC_MAD_PRISONER            = 39985,
    NPC_TWILIGHT_ELEMENT_WARDEN = 40017,
    NPC_TWILIGHT_FLAME_CALLER   = 39708,
    NPC_TWILIGHT_OBSIDIAN_BORER = 40019,
    NPC_TWILIGHT_SADIST         = 39980,
    NPC_TWILIGHT_TORTURER       = 39978,
    NPC_TWILIGHT_ZEALOT         = 39990,

    // Various NPCs
    NPC_RAZ_THE_CRAZED          = 39670,
    NPC_FINKLE_EINHORN          = 49476,
    NPC_TWILIGHT_PORTAL         = 51340,

    // Ascendant Lord  Obsidius
    NPC_SHADOW_OF_OBSIDIUS      = 40817,

    // Karsh Steelbender
    NPC_BOUND_FLAMES            = 50417,
    NPC_LAVA_SPOUT_TRIGGER      = 49529,

    // Corla, Herald of Twilight
    NPC_TWILIGHT_ZEALOT_CORLA   = 50284,
    NPC_NETHER_ESSENCE_TRIGGER  = 49526,

    // Romogg Bonecrusher
    NPC_ANGERED_EARTH           = 50376,
    NPC_CHAINS_OF_WOE           = 40447,
};

class MANGOS_DLL_DECL instance_blackrock_caverns : public ScriptedInstance
{
    public:
        instance_blackrock_caverns(Map* pMap);

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
