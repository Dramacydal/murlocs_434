/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_ICECROWN_PIT_H
#define DEF_ICECROWN_PIT_H

enum
{
    TYPE_GARFROST                   = 0,
    TYPE_KRICK                      = 1,
    TYPE_TYRANNUS                   = 2,
    TYPE_GAUNTLET                   = 3,
    TYPE_INTRO                      = 4,
    MAX_ENCOUNTER                   = 5,

    NPC_TYRANNUS_INTRO              = 36794,
    NPC_GARFROST                    = 36494,
    NPC_KRICK                       = 36477,
    NPC_KRICK_EVENT                 = 37629,
    NPC_ICK                         = 36476,
    NPC_TYRANNUS                    = 36658,
    NPC_RIMEFANG                    = 36661,
    NPC_SINDRAGOSA                  = 37755,

    NPC_SYLVANAS_PART1              = 36990,
    NPC_SYLVANAS_PART2              = 38189,
    NPC_JAINA_PART1                 = 36993,
    NPC_JAINA_PART2                 = 38188,
    NPC_KALIRA                      = 37583,
    NPC_ELANDRA                     = 37774,
    NPC_LORALEN                     = 37779,
    NPC_KORELN                      = 37582,
    NPC_CHAMPION_1_HORDE            = 37584,
    NPC_CHAMPION_2_HORDE            = 37587,
    NPC_CHAMPION_3_HORDE            = 37588,
    NPC_CHAMPION_1_ALLIANCE         = 37496,
    NPC_CHAMPION_2_ALLIANCE         = 37497,
    NPC_CHAMPION_3_ALLIANCE         = 37498,

    NPC_SLAVE_1                     = 36770,

    NPC_SLAVE_HORDE_1               = 37578,
    NPC_SLAVE_HORDE_2               = 37577,
    NPC_SLAVE_HORDE_3               = 37579,
    NPC_SLAVE_ALY_1                 = 37572,
    NPC_SLAVE_ALY_2                 = 37575,
    NPC_SLAVE_ALY_3                 = 37576,
    NPC_MARTIN_VICTUS_SLAVE         = 37591, 
    NPC_MARTIN_VICTUS_END           = 37580,
    NPC_GORKUN_IRONSKULL_SLAVE      = 37592,
    NPC_GORKUN_IRONSKULL_END        = 37581,

    GO_BALL_AND_CHAIN               = 201969,
    GO_ICEWALL                      = 201885,               // open after gafrost/krick
    GO_HALLS_OF_REFLECT_PORT        = 201848,               // unlocked by jaina/sylvanas at last outro
};

class MANGOS_DLL_DECL instance_pit_of_saron : public ScriptedInstance
{
    public:
        instance_pit_of_saron(Map* pMap);
        ~instance_pit_of_saron() {}

        void Initialize();

        void OnCreatureCreate(Creature* pCreature);
        void OnObjectCreate(GameObject* pGo);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        const char* Save() { return strInstData.c_str(); }
        void Load(const char* chrIn);

    protected:
        uint32 m_auiEncounter[MAX_ENCOUNTER];
        std::string strInstData;

};
 

#endif
