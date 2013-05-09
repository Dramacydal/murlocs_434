/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BLACKROCK_CAVERNS_H
#define DEF_BLACKROCK_CAVERNS_H

enum
{
    MAX_ENCOUNTER               = 11,

    TYPE_ANHUUR                 = 0,
    TYPE_PTAH                   = 1,
    TYPE_ANRAPHET               = 2,
    TYPE_EARTH_WARDEN           = 3,
    TYPE_WATER_WARDEN           = 4,
    TYPE_AIR_WARDEN             = 5,
    TYPE_FLAME_WARDEN           = 6,
    TYPE_ISISET                 = 7,
    TYPE_AMMUNAE                = 8,
    TYPE_SETESH                 = 9,
    TYPE_RAJH                   = 10,

    NPC_ANHUUR                  = 39425,
    NPC_PTAH                    = 39428,
    NPC_ANRAPHET                = 39788,
    NPC_EARTH_WARDEN            = 39801,
    NPC_WATER_WARDEN            = 39802,
    NPC_AIR_WARDEN              = 39803,
    NPC_FLAME_WARDEN            = 39800,
    NPC_ISISET                  = 39587,
    NPC_AMMUNAE                 = 39731,
    NPC_SETESH                  = 39732,
    NPC_RAJH                    = 39378,

    GO_ANHUUR_BRIDGE            = 206506,
    GO_ANHUUR_BRIDGE_COL        = 207725,
    GO_ANHUUR_DOOR              = 202307,

    GO_VAULT_OF_LIGHTS_ENTRANCE_DOOR    = 202313,
    GO_VAULT_OF_LIGHTS_BOSS_DOOR        = 202314,

    GO_ULDUM_LIGHT_MACHINE_02   = 207374,
    GO_ULDUM_LIGHT_MACHINE_01   = 207375,
    GO_ULDUM_LIGHT_MACHINE_04   = 207376,
    GO_ULDUM_LIGHT_MACHINE_03   = 207377,

    GO_SUN_MIRROR               = 207726,

    GO_ULDUM_LASERS_02          = 207662,
    GO_ULDUM_LASERS_01          = 207663,
    GO_ULDUM_LASERS_04          = 207664,
    GO_ULDUM_LASERS_03          = 207665,
};

class MANGOS_DLL_DECL instance_halls_of_origination : public ScriptedInstance
{
    public:
        instance_halls_of_origination(Map* pMap);

        void Initialize() override;

        void OnCreatureCreate(Creature* pCreature) override;
        void OnObjectCreate(GameObject* pGo) override;

        void SetData(uint32 uiType, uint32 uiData) override;
        uint32 GetData(uint32 uiType) override;

        const char* Save() override { return m_strInstData.c_str(); }
        void Load(const char* chrIn) override;

    private:

        bool WardensDead();

        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string m_strInstData;
};

#endif
