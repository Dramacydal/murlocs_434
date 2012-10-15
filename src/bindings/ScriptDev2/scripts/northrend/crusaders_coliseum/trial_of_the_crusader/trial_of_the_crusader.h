/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef DEF_CRUSADER_H
#define DEF_CRUSADER_H
#include "BSW_ai.h"
#include "BSW_instance.h"

enum
{
    TYPE_STAGE                  = 0,
    TYPE_BEASTS                 = 1,
    TYPE_JARAXXUS               = 2,
    TYPE_CRUSADERS              = 3,
    TYPE_VALKIRIES              = 4,
    TYPE_LICH_KING              = 5,
    TYPE_ANUBARAK               = 6,
    TYPE_COUNTER                = 7,
    TYPE_EVENT                  = 8,
    MAX_ENCOUNTERS              = 9,

    TYPE_UPPER_BACK_PAIN        = 0,
    TYPE_THREE_SIXTY_PAIN_SPIKE = 1,
    TYPE_SALT_AND_PEPPER        = 2,
    TYPE_SKILL                  = 3,
    TYPE_MAD_SKILL              = 4,
    TYPE_INSANITY               = 5,
    TYPE_DEDICATED_INSANITY     = 6,
    TYPE_IMMORTALITY            = 7,
    MAX_SPECIAL_ACHIEV_CRITS    = 8,

    NPC_BARRENT                 = 34816,
    NPC_TIRION                  = 34996,
    NPC_FIZZLEBANG              = 35458,
    NPC_GARROSH                 = 34995,
    NPC_RINN                    = 34990,
    NPC_LICH_KING_0             = 16980,
    NPC_LICH_KING_1             = 35877,

    NPC_THRALL                  = 34994,
    NPC_PROUDMOORE              = 34992,
    NPC_TRIGGER                 = 22517,
    NPC_WILFRED_PORTAL          = 35651,

    NPC_ICEHOWL                 = 34797,
    NPC_GORMOK                  = 34796,
    NPC_DREADSCALE              = 34799,
    NPC_ACIDMAW                 = 35144,

    NPC_JARAXXUS                = 34780,

    NPC_CRUSADER_1_1            = 34460, //Druid
    NPC_CRUSADER_1_2            = 34463, //Shaman
    NPC_CRUSADER_1_3            = 34461, //DK
    NPC_CRUSADER_1_4            = 34472, //Rogue
    NPC_CRUSADER_1_5            = 34475, //Warrior
    NPC_CRUSADER_1_6            = 34471, //Retro pal
    NPC_CRUSADER_1_7            = 34473, //Shadow priest
    NPC_CRUSADER_1_8            = 34468, //Mage
    NPC_CRUSADER_1_9            = 34467, //Hunter
    NPC_CRUSADER_1_10           = 34474, //Warlock
    NPC_CRUSADER_1_11           = 34470, //Enh shaman
    NPC_CRUSADER_1_12           = 34466, //Priest
    NPC_CRUSADER_1_13           = 34465, //Holy paladin
    NPC_CRUSADER_1_14           = 34469, //Moonkin

    NPC_CRUSADER_2_1            = 34451, //Druid
    NPC_CRUSADER_2_2            = 34455, //Shaman
    NPC_CRUSADER_2_3            = 34458, //DK
    NPC_CRUSADER_2_4            = 34454, //Rogue
    NPC_CRUSADER_2_5            = 34453, //Warrior
    NPC_CRUSADER_2_6            = 34456, //Retro pal
    NPC_CRUSADER_2_7            = 34441, //Shadow Priest
    NPC_CRUSADER_2_8            = 34449, //Mage
    NPC_CRUSADER_2_9            = 34448, //Hunter
    NPC_CRUSADER_2_10           = 34450, //Warlock
    NPC_CRUSADER_2_11           = 34444, //Enh shaman
    NPC_CRUSADER_2_12           = 34447, //Priest
    NPC_CRUSADER_2_13           = 34445, //Holy paladin
    NPC_CRUSADER_2_14           = 34459, //Moonkin

    NPC_CRUSADER_0_1            = 35465,
    NPC_CRUSADER_0_2            = 35610,

    NPC_LIGHTBANE               = 34497,
    NPC_DARKBANE                = 34496,

    NPC_ANUBARAK                = 34564,

    GO_CRUSADERS_CACHE_10       = 195631,
    GO_CRUSADERS_CACHE_25       = 195632,
    GO_CRUSADERS_CACHE_10_H     = 195633,
    GO_CRUSADERS_CACHE_25_H     = 195635,

    GO_TRIBUTE_CHEST_10H_25     = 195665,
    GO_TRIBUTE_CHEST_10H_45     = 195666,
    GO_TRIBUTE_CHEST_10H_50     = 195667,
    GO_TRIBUTE_CHEST_10H_99     = 195668,

    GO_TRIBUTE_CHEST_25H_25     = 195669,
    GO_TRIBUTE_CHEST_25H_45     = 195670,
    GO_TRIBUTE_CHEST_25H_50     = 195671,
    GO_TRIBUTE_CHEST_25H_99     = 195672,

    GO_ARGENT_COLISEUM_FLOOR    = 195527, //20943
    GO_MAIN_GATE_DOOR           = 195647,
    GO_WEB_DOOR                 = 195485,

    GO_WEST_PORTCULLIS          = 195589,
    GO_SOUTH_PORTCULLIS         = 195590,
    GO_NORTH_PORTCULLIS         = 195591,

    TYPE_DIFFICULTY             = 101,
    TYPE_EVENT_TIMER            = 102,
    TYPE_EVENT_NPC              = 103,
    TYPE_NORTHREND_BEASTS       = 104,
    TYPE_CRUSADERS_COUNT        = 105,

    DATA_HEALTH_EYDIS           = 201,
    DATA_HEALTH_FJOLA           = 202,

    DESPAWN_TIME                = 300000,

    CRITERIA_ACHIEV_UPPER_BACK_PAIN_10N             = 11779,
    CRITERIA_ACHIEV_UPPER_BACK_PAIN_25N             = 11780,

    CRITERIA_ACHIEV_THREE_SIXTY_PAIN_SPIKE_10N      = 11838,
    CRITERIA_ACHIEV_THREE_SIXTY_PAIN_SPIKE_10H      = 11861,
    CRITERIA_ACHIEV_THREE_SIXTY_PAIN_SPIKE_25N      = 11839,
    CRITERIA_ACHIEV_THREE_SIXTY_PAIN_SPIKE_25H      = 11862,

    CRITERIA_ACHIEV_SALT_AND_PEPPER_10N             = 11778,
    CRITERIA_ACHIEV_SALT_AND_PEPPER_10H             = 12258,
    CRITERIA_ACHIEV_SALT_AND_PEPPER_25N             = 11818,
    CRITERIA_ACHIEV_SALT_AND_PEPPER_25H             = 11860,

    CRITERIA_ACHIEV_TRIBUTE_TO_SKILL_10             = 12344,
    CRITERIA_ACHIEV_TRIBUTE_TO_SKILL_25             = 12338,
    CRITERIA_ACHIEV_TRIBUTE_TO_MAD_SKILL_10         = 12347,
    CRITERIA_ACHIEV_TRIBUTE_TO_MAD_SKILL_25         = 12341,
    CRITERIA_ACHIEV_TRIBUTE_TO_INSANITY_10          = 12349,
    CRITERIA_ACHIEV_TRIBUTE_TO_INSANITY_25          = 12343,
    CRITERIA_ACHIEV_TRIBUTE_TO_IMMORTALITY_HORDE    = 12247,
    CRITERIA_ACHIEV_TRIBUTE_TO_IMMORTALITY_ALLY     = 12359,
    CRITERIA_ACHIEV_TRIBUTE_TO_DEDICATED_INSANITY   = 12360,
};

static Locations SpawnLoc[]=
{
    {559.257996f, 90.266197f,  395.122986f}, // 0 Barrent
    {563.672974f, 139.571f,    393.837006f}, // 1 Center
    {563.833008f, 187.244995f, 394.5f},      // 2 Backdoor
    {577.347839f, 185.338888f, 395.14f},     // 3 - Right
    {550.955933f, 185.338888f, 395.14f},     // 4 - Left
    {575.042358f, 195.260727f, 395.137146f}, // 5
    {552.248901f, 195.331955f, 395.132658f}, // 6
    {573.342285f, 195.515823f, 395.135956f}, // 7
    {554.239929f, 195.825577f, 395.137909f}, // 8
    {571.042358f, 195.260727f, 395.137146f}, // 9
    {556.720581f, 195.015472f, 395.132658f}, // 10
    {569.534119f, 195.214478f, 395.139526f}, // 11
    {569.231201f, 195.941071f, 395.139526f}, // 12
    {558.811610f, 195.985779f, 394.671661f}, // 13
    {567.641724f, 195.351501f, 394.659943f}, // 14
    {560.633972f, 195.391708f, 395.137543f}, // 15
    {565.816956f, 195.477921f, 395.136810f}, // 16
    {563.549f,    152.474f,    394.393f},    // 17 - Lich king start
    {563.547f,    141.613f,    393.908f},    // 18 - Lich king end
    {787.932556f, 133.28978f,  142.612152f}, // 19 - Anub'arak start location
    {618.157898f, 132.640869f, 139.559769f}, // 20 - Anub'arak move point location
    {563.813660f, 182.842819f, 395.128052f}, // 21 - Fizzlebang start location
    {586.060242f, 117.514809f, 394.314026f}, // 22 - Dark essence 1
    {541.602112f, 161.879837f, 394.587952f}, // 23 - Dark essence 2
    {541.021118f, 117.262932f, 395.314819f}, // 24 - Light essence 1
    {586.200562f, 162.145523f, 394.626129f}, // 25 - Light essence 2
    {563.833008f, 195.244995f, 394.585561f}, // 26 - outdoor
    {563.938477f, 149.210571f, 394.321838f}, // 27 - Fizzlebang end
    {563.710999f, 131.058319f, 394.321838f}, // 28 - Jaraxxus
    {750.78f,     158.48f,     142.68f    }, // 29 - Frost Sphere 1
    {749.69f,     129.47f,     142.68f    }, // 30 - Frost Sphere 2
    {702.69f,     126.47f,     142.68f    }, // 31 - Frost Sphere 3
    {702.53f,     154.69f,     142.68f    }, // 32 - Frost Sphere 4
    {530.80f,     108.18f,     395.14f    }, // 33 - Unleashed light 1
    {530.49f,     172.75f,     395.14f    }, // 34 - Unleashed light 2
    {591.52f,     167.06f,     395.14f    }, // 35 - Unleashed light 3
    {589.40f,     114.05f,     395.14f    }, // 36 - Unleashed light 4
    {516.64f,     139.74f,     395.14f    }, // 37 - Unleashed dark 1
    {563.88f,     169.17f,     395.14f    }, // 38 - Unleashed dark 2
    {604.99f,     139.63f,     395.14f    }, // 39 - Unleashed dark 3
    {563.65f,     103.28f,     395.14f    }, // 40 - Unleashed dark 4
    {519.75f,     119.29f,     395.14f    }, // 41 - Unleashed light 1 extra (25)
    {548.70f,     170.04f,     395.14f    }, // 42 - Unleashed light 2 extra (25)
    {575.52f,     144.13f,     395.14f    }, // 43 - Unleashed light 3 extra (25)
    {570.38f,     122.28f,     395.14f    }, // 44 - Unleashed light 4 extra (25)
    {550.53f,     145.74f,     395.14f    }, // 45 - Unleashed dark 1 extra (25)
    {577.57f,     174.08f,     395.14f    }, // 46 - Unleashed dark 2 extra (25)
    {605.72f,     119.60f,     395.14f    }, // 47 - Unleashed dark 3 extra (25)
    {554.94f,     123.42f,     395.14f    }, // 48 - Unleashed dark 4 extra (25)
    {697.73f,     118.48f,     142.68f    }, // 49 - Frost Sphere 5
    {733.33f,     153.44f,     142.68f    }, // 50 - Frost Sphere 6
    {731.40f,     90.90f,      142.68f    }, // 51 - Frost Sphere 7
    {794.34f,     150.48f,     142.68f    }, // 52 - Frost Sphere 8
};

enum uiWorldStates
{
    UPDATE_STATE_UI_SHOW            = 4390,
    UPDATE_STATE_UI_COUNT           = 4389,
};

enum NorthrendBeasts
{
    GORMOK_IN_PROGRESS              = 1000,
    GORMOK_DONE                     = 1001,
    SNAKES_IN_PROGRESS              = 2000,
    DREADSCALE_SUBMERGED            = 2001,
    ACIDMAW_SUBMERGED               = 2002,
    SNAKES_SPECIAL                  = 2003,
    SNAKES_DONE                     = 2004,
    ICEHOWL_IN_PROGRESS             = 3000,
    ICEHOWL_DONE                    = 3001,
};

enum AnnounserMessages
{
    MSG_BEASTS                      = 724001,
    MSG_JARAXXUS                    = 724002,
    MSG_CRUSADERS                   = 724003,
    MSG_VALKIRIES                   = 724004,
    MSG_LICH_KING                   = 724005,
    MSG_ANUBARAK                    = 724006,
};

class MANGOS_DLL_DECL instance_trial_of_the_crusader : public BSWScriptedInstance
{
public:
    instance_trial_of_the_crusader(Map* pMap);
    ~instance_trial_of_the_crusader() {}

    void Initialize();

    bool IsEncounterInProgress() const;
    void OnPlayerEnter(Player *m_player);
    void OnPlayerDeath(Player *m_player);

    bool IsRaidWiped();
    void UpdateWorldState();

    void OnCreatureCreate(Creature* pCreature);
    void OnObjectCreate(GameObject *pGo);

    bool CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/);
    void SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet);

    void SetData(uint32 uiType, uint32 uiData);
    uint32 GetData(uint32 uiType);

    const char* Save() { return m_strInstData.c_str(); }

    void Load(const char* chrIn);

private:

    uint32 m_auiEncounter[MAX_ENCOUNTERS+1];
    uint32 m_auiEventTimer;
    uint32 m_auiEventNPCId;
    uint32 m_auiNorthrendBeasts;
    uint8 Difficulty;
    std::string m_strInstData;
    bool needsave;

    bool m_bAchievCriteria[MAX_SPECIAL_ACHIEV_CRITS];

    uint32 m_uiDataDamageFjola;
    uint32 m_uiDataDamageEydis;

    uint32 m_uiTributeChest1;
    uint32 m_uiTributeChest2;
    uint32 m_uiTributeChest3;
    uint32 m_uiTributeChest4;

    uint32 m_auiCrusadersCount;
};

#endif
