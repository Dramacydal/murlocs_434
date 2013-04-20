/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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

/* ScriptData
SDName: boss_alysrazor
SD%Complete: 10
SDComment: Placeholder
SDCategory: Firelands
EndScriptData */

#include "precompiled.h"
#include "firelands.h"
#include "escort_ai.h"

enum
{
    // ToDo: add spells and yells here
};

Position2 const SpawnPos[31] =
{
    // Position on summon
    {110.404f, -376.861f, 30.465743f, 2.517f}, // startPos
    {49.467f, -344.118f, 90.471171f, 2.517f}, // endPos

    // Starting Waypoints
    {-0.617f, -306.765f, 70.330f, 2.517f}, // start center
    {-66.545f, -246.700f, 60.082f, 2.354f}, // end center
    {-94.406f, -251.076f, 111.694f, 4.390f}, // left 1
    {-95.940f, -306.784f, 110.108f, 4.818f}, // left 2
    {-84.214f, -328.750f, 108.429f, 5.780f}, // left 3
    {-43.594f, -346.574f, 128.981f, 0.035f}, // left 4
    {-3.449f, -337.038f, 122.448f, 0.188f}, // left 5
    {14.184f, -243.657f, 142.407f, 1.779f}, // right 1
    {17.443f, -273.178f, 136.369f, 2.156f}, // right 2
    {17.748f, -236.526f, 139.634f, 1.884f}, // right 3
    {-35.472f, -214.974f, 140.048f, 3.172f}, // right 4
    {-47.900f, -219.760f, 135.246f, 3.401f}, // right 5
    {-78.433f, -237.402f, 65.301f, 5.576f}, // end center 2
    {5.627f, -314.356f, 65.871f, 5.812f}, // start center 2
    {14.184f, -243.657f, 142.407f, 1.779f}, // left(n) 1
    {17.443f, -273.178f, 136.369f, 2.156f}, // left(n) 2
    {17.748f, -236.526f, 139.634f, 1.884f}, // left(n) 3
    {-35.472f, -214.974f, 140.048f, 3.172f}, // left(n) 4
    {-47.900f, -219.760f, 135.246f, 3.401f}, // left(n) 5
    {-94.406f, -251.076f, 111.694f, 4.390f}, // right(n) 1
    {-95.940f, -306.784f, 110.108f, 4.818f}, // right(n) 2
    {-84.214f, -328.750f, 108.429f, 5.780f}, // right(n) 3
    {-43.594f, -346.574f, 128.981f, 0.035f}, // right(n) 4
    {-3.449f, -337.038f, 122.448f, 0.188f}, // right(n) 5
    {-38.087f, -302.436f, 115.977f, 2.848f}, // wayPoints aly on tornados 1
    {-60.484f, -278.438f, 115.977f, 1.265f}, // wayPoints aly on tornados 2
    {-47.210f, -241.612f, 115.977f, 0.201f}, // wayPoints aly on tornados 3
    {-24.410f, -266.740f, 115.977f, 4.980f}, // wayPoints aly on tornados 4
};

Position2 const WormsPos[4] = 
{
    {-24.179f, -311.185f, 56.000f, 1.0f},
    {-75.952f, -296.170f, 58.000f, 4.0f},
    {-71.017f, -255.195f, 58.000f, 0.2f},
    {-17.180f, -257.636f, 56.000f, 2.0f},
};

Position2 const FallPos[2] =
{
    {-41.534f, -282.622f, 56.000f, 0.188f}, // fall pos ground
    {-41.534f, -282.622f, 115.000f, 0.188f}, // fall pos fly
};

Position2 const BirdsPos[2] = 
{
    {46.930f, -207.560f, 105.000f, 3.951f},
    {-28.813f, -378.088f, 107.371f, 1.697f},
};

Position2 const Talon[8] =
{
    {-6.561f,-376.156f,75.623f,2.017f},
    {-27.041f,-329.757f,57.953f,2.402f},

    {-93.998f,-338.753f,67.583f,1.239f},
    {-92.066f,-296.570f,59.674f,0.459f},

    {-85.828f,-218.517f,71.189f,5.014f},
    {-82.602f,-254.543f,61.894f,5.922f},

    {35.679f,-260.996f,67.902f,3.880f},
    {8.355f,-286.534f,56.162f,2.984f},
};

Position2 const ClawTalon[2] = 
{
    {-27.773f, -191.712f, 83.280f, 4.769f},
    {-65.413f, -351.496f, 72.721f, 1.356f},
};

Position2 const Feather[1] = 
{
    {0.760f,-305.456f,53.5113f,5.597f},
};

Position2 const Tornado[85] = 
{
    {-35.268f,-272.739f,55.382f,4.191f},
    {-32.955f,-289.301f,55.673f,3.386f},
    {-49.435f,-291.899f,56.181f,1.705f},
    {-49.891f,-276.776f,55.919f,6.064f},

    {-59.618f,-271.323f,56.323f,5.165f},
    {-63.114f,-285.496f,56.309f,5.699f},
    {-55.805f,-298.918f,56.146f,6.174f},
    {-38.599f,-304.341f,55.907f,1.018f},
    {-23.701f,-296.092f,54.847f,1.646f},
    {-21.894f,-278.830f,54.687f,2.840f},

    {-12.605f,-304.404f,54.111f,3.453f},
    {-37.279f,-314.646f,55.623f,2.773f},
    {-61.806f,-308.852f,56.871f,2.259f},
    {-75.187f,-287.070f,56.577f,1.324f},
    {-69.767f,-265.741f,56.780f,0.574f},
    {-50.085f,-253.181f,55.990f,5.919f},

    {-16.950f,-244.304f,55.931f,5.302f},
    {0.441f,-273.298f,53.492f,4.435f},
    {-2.772f,-312.061f,53.672f,3.524f},
    {-36.467f,-326.278f,55.770f,2.361f},
    {-67.491f,-317.809f,59.693f,1.976f},
    {-87.615f,-290.200f,56.189f,1.430f},

    {-90.258f,-253.685f,58.907f,6.280f},
    {-59.377f,-229.198f,56.394f,6.194f},
    {-10.620f,-230.829f,55.906f,5.181f},
    {11.433f,-272.349f,53.118f,4.101f},
    {6.917f,-318.777f,52.964f,3.492f},
    {-35.572f,-337.599f,55.996f,2.404f},

    {-49.891f,-276.776f,55.919f,6.064f},
    {-49.435f,-291.899f,56.181f,1.705f},
    {-32.955f,-289.301f,55.673f,3.386f},
    {-35.268f,-272.739f,55.382f,4.191f},

    {-21.894f,-278.830f,54.687f,2.840f},
    {-23.701f,-296.092f,54.847f,1.646f},
    {-38.599f,-304.341f,55.907f,1.018f},
    {-55.805f,-298.918f,56.146f,6.174f},
    {-63.114f,-285.496f,56.309f,5.699f},
    {-59.618f,-271.323f,56.323f,5.165f},

    {-50.085f,-253.181f,55.990f,5.919f},
    {-69.767f,-265.741f,56.780f,0.574f},
    {-75.187f,-287.070f,56.577f,1.324f},
    {-61.806f,-308.852f,56.871f,2.259f},
    {-37.279f,-314.646f,55.623f,2.773f},
    {-12.605f,-304.404f,54.111f,3.453f},

    {-87.615f,-290.200f,56.189f,1.430f},
    {-67.491f,-317.809f,59.693f,1.976f},
    {-36.467f,-326.278f,55.770f,2.361f},
    {-2.772f,-312.061f,53.672f,3.524f},
    {0.441f,-273.298f,53.492f,4.435f},
    {-16.950f,-244.304f,55.931f,5.302f},

    {-35.572f,-337.599f,55.996f,2.404f},
    {6.917f,-318.777f,52.964f,3.492f},
    {11.433f,-272.349f,53.118f,4.101f},
    {-10.620f,-230.829f,55.906f,5.181f},
    {-59.377f,-229.198f,56.394f,6.194f},
    {-90.258f,-253.685f,58.907f,6.280f},

    {-49.435f,-291.899f,56.181f,1.705f},
    {-49.891f,-276.776f,55.919f,6.064f},
    {-35.268f,-272.739f,55.382f,4.191f},
    {-32.955f,-289.301f,55.673f,3.386f},

    {-38.599f,-304.341f,55.907f,1.018f},
    {-23.701f,-296.092f,54.847f,1.646f},
    {-21.894f,-278.830f,54.687f,2.840f},
    {-59.618f,-271.323f,56.323f,5.165f},
    {-63.114f,-285.496f,56.309f,5.699f},
    {-55.805f,-298.918f,56.146f,6.174f},

    {-75.187f,-287.070f,56.577f,1.324f},
    {-69.767f,-265.741f,56.780f,0.574f},
    {-50.085f,-253.181f,55.990f,5.919f},
    {-12.605f,-304.404f,54.111f,3.453f},
    {-37.279f,-314.646f,55.623f,2.773f},
    {-61.806f,-308.852f,56.871f,2.259f},

    {-36.467f,-326.278f,55.770f,2.361f},
    {-67.491f,-317.809f,59.693f,1.976f},
    {-87.615f,-290.200f,56.189f,1.430f},
    {-16.950f,-244.304f,55.931f,5.302f},
    {0.441f,-273.298f,53.492f,4.435f},
    {-2.772f,-312.061f,53.672f,3.524f},

    {11.433f,-272.349f,53.118f,4.101f},
    {6.917f,-318.777f,52.964f,3.492f},
    {-35.572f,-337.599f,55.996f,2.404f},
    {-90.258f,-253.685f,58.907f,6.280f},
    {-59.377f,-229.198f,56.394f,6.194f},
    {-10.620f,-230.829f,55.906f,5.181f},
};

Position2 const SpawnVolcanos[11] =
{
    {5.375f, -336.376f, 51.629f, 2.304f},
    {9.478f, -331.562f, 51.387f, 2.304f},
    {16.252f, -323.355f, 51.448f, 2.304f},
    {19.871f, -320.852f, 51.960f, 2.304f},
    {22.058f, -315.533f, 51.959f, 2.304f},
    {28.059f, -307.899f, 51.958f, 2.304f},
    {32.530f, -300.595f, 51.965f, 2.304f},

    {-103.034f, -294.838f, 56.158f, 2.304f},
    {-103.872f, -286.840f, 56.507f, 2.304f},
    {-103.593f, -280.863f, 56.531f, 2.304f},
    {-104.593f, -274.393f, 56.534f, 2.304f},
};

enum actions 
{
    ACTION_MOVE_FROM_LEFT = 0,
    ACTION_MOVE_FROM_RIGHT = 1,
};

enum Spells
{
    SPELL_FIRESTORM = 99605,

    // Volcanos
    SPELL_VOLCANO_DUMMY = 98402,
    SPELL_VOLCANO_AURA = 98462,

    // Tornados
    SPELL_SUMMON_DMG = 99793,

    // Feather
    SPELL_COSMETIC = 99507,
    SPELL_CAST_ON_MOVE_VISUAL = 97128,

    // Firehawk
    SPELL_FIREHAWK_COSMETIC = 100712,
    SPELL_FIREHAWK_TRANSFORM_M = 99550,
    SPELL_FIREHAWK_TRANSFORM_F = 100350,
    SPELL_FIREHAWK_CLAWSHAPPER = 99923,
    SPELL_FIREHAWK_REMOVE_FORM = 99924,
    SPELL_IGNITION = 99919,
    SPELL_BRUSHFIRE = 98868,
    SPELL_FIEROBLAST = 101223,
    SPELL_FIREITUP = 100093,
    SPELL_BRUSHFIRE_VIS = 98884,
    
    // Hatchling
    SPELL_SATIATED     = 99359,
    SPELL_HUNGRY       = 99361,
    SPELL_TANTRUM      = 99362,
    SPELL_IMPRINTED    = 99389,
    SPELL_TAUNT        = 99390,
    SPELL_DEBUFF       = 100024,

    //woorm
    SPELL_LAVA_SPEW    = 99335,

    // Blazing power
    SPELL_BLAZING_POWER = 99462,

    // Cloud
    SPELL_INCENDIARY_CLOUD = 99426,

    // Alysrazor
    SPELL_INDICATOR = 101410,
    SPELL_BLAZING_CLAW = 99843,
    SPELL_PASSIVE_BUF_FROM_FEATHER = 99461,
    SPELL_BURNOUT = 99432,
    SPELL_IGNITED = 99922,
    SPELL_SPARK = 99921,
    SPELL_ZERO_POWER = 72242,
    SPELL_FULL_POWER = 99504,
    SPELL_SUMMON_FEATHER = 99464,
    SPELL_MOLTING = 100698,
    SPELL_FLY = 98619,
    SPELL_HARSH_WINDS = 100640,

    //molten egg
    SPELL_EGG_EXPLOSION = 99943,
    SPELL_SUMM_HATHING  = 99723,

    //broodmother
    SPELL_BROODMOTHER_VIS  = 99734,
    SPELL_BROODMOTHER_VIS1 = 100712,
};

enum Phase
{
    PHASE_NONE = 0,
    PHASE_JUMP = 1,
    PHASE_START_MOVING = 2,
    PHASE_AIR = 3,
    PHASE_GROUND = 4,
    PHASE_TORNADO = 5,
};

enum Events
{
    EVENT_START_ENCOUNTER = 1,

    // Moving
    EVENT_MOVE_TO_START_OF_TOP = 2,
    EVENT_MOVE_TO_END_OF_TOP = 3,
    EVENT_MOVE_TO_LEFT_1 = 4,
    EVENT_MOVE_TO_LEFT_2 = 5,
    EVENT_MOVE_TO_LEFT_3 = 6,
    EVENT_MOVE_TO_LEFT_4 = 7,
    EVENT_MOVE_TO_LEFT_5 = 8,
    EVENT_MOVE_TO_RIGHT_1 = 9,
    EVENT_MOVE_TO_RIGHT_2 = 10,
    EVENT_MOVE_TO_RIGHT_3 = 11,
    EVENT_MOVE_TO_RIGHT_4 = 12,
    EVENT_MOVE_TO_RIGHT_5 = 13,
    EVENT_MOVE_TO_END_CENTER = 14,
    EVENT_MOVE_TO_END_START = 15,
    EVENT_MOVE_TO_LEFT_N_1 = 16,
    EVENT_MOVE_TO_LEFT_N_2 = 17,
    EVENT_MOVE_TO_LEFT_N_3 = 18,
    EVENT_MOVE_TO_LEFT_N_4 = 19,
    EVENT_MOVE_TO_LEFT_N_5 = 20,
    EVENT_MOVE_TO_RIGHT_N_1 = 21,
    EVENT_MOVE_TO_RIGHT_N_2 = 22,
    EVENT_MOVE_TO_RIGHT_N_3 = 23,
    EVENT_MOVE_TO_RIGHT_N_4 = 24,
    EVENT_MOVE_TO_RIGHT_N_5 = 25,

    EVENT_MOVE_TORNADOS_1 = 26,
    EVENT_MOVE_TORNADOS_2 = 27,
    EVENT_MOVE_TORNADOS_3 = 28,
    EVENT_MOVE_TORNADOS_4 = 29,

    EVENT_MOVE_TO_TORNADO = 30,

    // Need for combat
    EVENT_ZONE_COMBAT = 31,

    // Phase
    EVENT_PHASE_2_TORNADOS_START = 32,
    EVENT_PHASE_2_TORNADOS_ENDS = 33,

    EVENT_INCENDIARY_CLOUD = 34,
    EVENT_HARSH_WINDS = 35,

    // Hatchling
    EVENT_EAT_WORM = 36,
    EVENT_HUNGRY = 37,
    EVENT_DEBUFF = 38,

    // Lava worm
    EVENT_CALC_ORIENT = 39,
    EVENT_SUMMON_WORMS = 40,

    EVENT_SUMMON_BROODMOTHER = 41,

    // Egg
    EVENT_EGG_EXPLOSION = 42,
    EVENT_SUMMON_HATCHING = 43,
    EVENT_DESPAWN = 44,
    
    // Clawshaper
    EVENT_SUMMON_DRUIDS = 45,
    EVENT_START_IGNITION = 46,
    EVENT_START_COMBAT = 47,
    EVENT_CHECK_ENERGY = 48,

    EVENT_START_FIEROBLAST = 49,
    EVENT_START_BRUSHFIRE = 50,

    EVENT_SUMMON_TALON = 51,
};

struct MANGOS_DLL_DECL boss_alysrazorAI : public ScriptedAI
{
    boss_alysrazorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    uint8 waypoint,phase,LastTalon;
    bool isNeedJump;
    bool bInit;

    void Reset() override
    {
        m_creature->SetMaxPower(POWER_ENERGY,100);
        m_creature->SetPower(POWER_ENERGY,100);
        phase = PHASE_NONE;

        events.Reset();
        isNeedJump = false;
        isNeedJump = true;
        //m_creature->SetReactState(REACT_AGGRESSIVE);
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALYSRAZOR, IN_PROGRESS);
    }

    void EnterCombat(Unit* who) override
    {
        EnterAir();

        if (m_pInstance)
        {
            //instance->SetBossState(DATA_ALYSRAZAR, IN_PROGRESS);
            //instance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, me); // Add
        }

        DoCast(m_creature, SPELL_INDICATOR);
    }

    void EnterGround()
    {
        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
        m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
        m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
    }

    void EnterAir()
    {
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALYSRAZOR, DONE);

        m_creature->RemoveAllAuras();
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALYSRAZOR, FAIL);

        DespawnMinions();
        m_creature->SummonCreature(53089,Feather[0].x, Feather[0].y, Feather[0].z, Feather[0].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

    void DespawnMinions()
    {
        DespawnCreatures(53158);
        DespawnCreatures(53681);
        DespawnCreatures(53680);
        DespawnCreatures(53509);
        DespawnCreatures(53693);
        DespawnCreatures(53698);
    }

    void DespawnCreatures(uint32 entry)
    {
        std::list<Creature*> creatures;
        GetCreatureListWithEntryInGrid(creatures, m_creature, entry, 1000.0f);

        if (creatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
            (*iter)->ForcedDespawn();
    }

    void CancelMove()
    {
        events.CancelEvent(EVENT_MOVE_TO_START_OF_TOP);
        events.CancelEvent(EVENT_MOVE_TO_END_OF_TOP);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_1);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_2);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_3);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_4);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_5);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_1);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_2);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_3);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_4);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_5);
        events.CancelEvent(EVENT_MOVE_TO_END_CENTER);
        events.CancelEvent(EVENT_MOVE_TO_END_START);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_N_1);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_N_2);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_N_3);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_N_4);
        events.CancelEvent(EVENT_MOVE_TO_LEFT_N_5);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_N_1);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_N_2);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_N_3);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_N_4);
        events.CancelEvent(EVENT_MOVE_TO_RIGHT_N_5);
        events.CancelEvent(EVENT_MOVE_TORNADOS_1);
        events.CancelEvent(EVENT_MOVE_TORNADOS_2);
        events.CancelEvent(EVENT_MOVE_TORNADOS_3);
        events.CancelEvent(EVENT_MOVE_TORNADOS_4);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!bInit)
        {
            bInit = true;
            phase = PHASE_JUMP;
            isNeedJump = true;
            //m_creature->SetReactState(REACT_PASSIVE);
            //DoZoneInCombat();
            events.ScheduleEvent(EVENT_ZONE_COMBAT, 1000);
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() || m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (phase == PHASE_GROUND && m_creature->GetPower(POWER_ENERGY) >= 50 && !m_creature->HasAura(SPELL_IGNITED))
        {
            m_creature->RemoveAurasDueToSpell(SPELL_BURNOUT);
            m_creature->RemoveAurasDueToSpell(SPELL_SPARK);
            DoCast(m_creature,SPELL_IGNITED);
            DoCast(m_creature,SPELL_BLAZING_CLAW);
            RemoveAllFeathers();
            EnterAir();
            m_creature->GetMotionMaster()->MoveJump(m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ()+10,200,2);
            //m_creature->SetReactState(REACT_AGGRESSIVE);
            m_creature->AI()->MoveInLineOfSight(m_creature->getVictim());
        }

        if (phase == PHASE_GROUND && m_creature->GetPower(POWER_ENERGY) >= 100 && m_creature->HasAura(SPELL_IGNITED))
        {
            phase = PHASE_JUMP;
            LastTalon = 4;
            m_creature->RemoveAurasDueToSpell(SPELL_IGNITED);
            m_creature->RemoveAurasDueToSpell(SPELL_BLAZING_CLAW);
            m_creature->AttackStop();
            //m_creature->SetReactState(REACT_PASSIVE);
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature,SPELL_FULL_POWER);
            events.ScheduleEvent(EVENT_PHASE_2_TORNADOS_START, 195000);
            events.ScheduleEvent(EVENT_MOVE_TO_LEFT_1, 1000);
            events.ScheduleEvent(EVENT_SUMMON_BROODMOTHER, 25000);
            events.ScheduleEvent(EVENT_SUMMON_WORMS, 50000);
            for (uint8 i = 0;i<2;++i)
                events.ScheduleEvent(EVENT_SUMMON_TALON, i == 0 ? urand(5000,10000) : urand(8000,13000));
        }

        if (phase == PHASE_JUMP && isNeedJump)
        {
            isNeedJump = false;
            m_creature->GetMotionMaster()->MoveJump(SpawnPos[1].GetPositionX(),SpawnPos[1].GetPositionY(),SpawnPos[1].GetPositionZ(),200,8);
            EnterAir();
            events.ScheduleEvent(EVENT_START_ENCOUNTER, 1000);
        }

        if (phase == PHASE_AIR || phase == PHASE_TORNADO)
            m_creature->GetMotionMaster()->MovePoint(0, SpawnPos[waypoint].GetPositionX(),SpawnPos[waypoint].GetPositionY(),SpawnPos[waypoint].GetPositionZ());

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_ENCOUNTER:
                    for (uint8 i = 0; i < 11; ++i)
                        m_creature->SummonCreature(53158,SpawnVolcanos[i].GetPositionX(),SpawnVolcanos[i].GetPositionY(),SpawnVolcanos[i].GetPositionZ(), SpawnVolcanos[i].GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);

                    DoCast(m_creature,SPELL_FIRESTORM);

                    events.ScheduleEvent(EVENT_PHASE_2_TORNADOS_START, 195000);
                    events.ScheduleEvent(EVENT_MOVE_TO_START_OF_TOP, 500);
                    events.ScheduleEvent(EVENT_INCENDIARY_CLOUD, 15000);
                    events.ScheduleEvent(EVENT_SUMMON_BROODMOTHER, 25000);
                    events.ScheduleEvent(EVENT_SUMMON_WORMS, 50000);
                    for (uint8 i = 0;i<2;++i)
                        events.ScheduleEvent(EVENT_SUMMON_TALON, i == 0 ? urand(5000,10000) : urand(8000,13000));
                    LastTalon = 4;
                    break;
                case EVENT_ZONE_COMBAT:
                    m_creature->SetInCombatWithZone();
                    events.ScheduleEvent(EVENT_ZONE_COMBAT, 1000);
                    break;
                case EVENT_INCENDIARY_CLOUD:
                    if (phase == PHASE_AIR)
                    {
                        m_creature->SummonCreature(53541,m_creature->GetPositionX()-urand(5,15),m_creature->GetPositionY()-urand(-5,-15),m_creature->GetPositionZ() > 120.0f ? m_creature->GetPositionZ() : 120.0f,m_creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
                        m_creature->SummonCreature(53554,m_creature->GetPositionX()+urand(25,35),m_creature->GetPositionY()+urand(-25,-35),m_creature->GetPositionZ() > 120.0f ? m_creature->GetPositionZ() : 120.0f,m_creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
                    }
                    events.ScheduleEvent(EVENT_INCENDIARY_CLOUD, 5000);
                    break;
                case EVENT_MOVE_TO_START_OF_TOP:
                    waypoint = 2;
                    phase = PHASE_AIR;
                    events.ScheduleEvent(EVENT_MOVE_TO_END_OF_TOP, 9000);
                    break;
                case EVENT_SUMMON_WORMS:
                    for(uint8 i = 0;i<4;++i)
                        m_creature->SummonCreature(53520,WormsPos[i].x, WormsPos[i].y, WormsPos[i].z, WormsPos[i].o, TEMPSUMMON_MANUAL_DESPAWN, 0);

                    events.ScheduleEvent(EVENT_SUMMON_WORMS, 60000);
                    break;
                case EVENT_SUMMON_TALON:
                {
                    uint32 talon = SelectTalon();
                    m_creature->SummonCreature(53896, Talon[talon].x,Talon[talon].y,Talon[talon].z,Talon[talon].o,TEMPSUMMON_MANUAL_DESPAWN, 0);

                    events.ScheduleEvent(EVENT_SUMMON_TALON, urand(40000,45000));
                    break;
                }
                case EVENT_SUMMON_BROODMOTHER:
                    for (uint8 i = 0;i<2;++i)
                        m_creature->SummonCreature(53680,BirdsPos[i].x,BirdsPos[i].y,BirdsPos[i].z,BirdsPos[i].o,TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
                case EVENT_MOVE_TO_END_OF_TOP:
                    DoCast(m_creature,SPELL_BLAZING_CLAW);
                    DoCast(m_creature,SPELL_MOLTING);
                    waypoint = 3;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_1, 12000);
                    break;
                case EVENT_MOVE_TO_LEFT_1:
                    m_creature->RemoveAurasDueToSpell(SPELL_BLAZING_CLAW);
                    phase = PHASE_AIR;
                    waypoint = 4;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_2, 5200);
                    break;
                case EVENT_MOVE_TO_LEFT_2:
                    waypoint = 5;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_3, 5200);
                    break;
                case EVENT_MOVE_TO_LEFT_3:
                    waypoint = 6;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_4, 5200);
                    break;
                case EVENT_MOVE_TO_LEFT_4:
                    waypoint = 7;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_5, 5200);
                    break;
                case EVENT_MOVE_TO_LEFT_5:
                    waypoint = 8;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_1, 5200);
                    break;
                case EVENT_MOVE_TO_RIGHT_1:
                    waypoint = 9;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_2, 5200);
                    break;
                case EVENT_MOVE_TO_RIGHT_2:
                    waypoint = 10;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_3, 5200);
                    break;
                case EVENT_MOVE_TO_RIGHT_3:
                    waypoint = 11;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_4, 5000);
                    break;
                case EVENT_MOVE_TO_RIGHT_4:
                    waypoint = 12;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_5, 5000);
                    break;
                case EVENT_MOVE_TO_RIGHT_5:
                    waypoint = 13;
                    events.ScheduleEvent(EVENT_MOVE_TO_END_CENTER, 5000);
                    break;
                case EVENT_MOVE_TO_END_CENTER:
                    waypoint = 14;
                    events.ScheduleEvent(EVENT_MOVE_TO_END_START, 10000);
                    break;
                case EVENT_MOVE_TO_END_START:
                    DoCast(m_creature,SPELL_BLAZING_CLAW);
                    DoCast(m_creature,SPELL_MOLTING);
                    waypoint = 15;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_N_1, 15000);
                    break;
                case EVENT_MOVE_TO_LEFT_N_1:
                    m_creature->RemoveAurasDueToSpell(SPELL_BLAZING_CLAW);
                    waypoint = 16;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_N_2, 5000);
                    break;
                case EVENT_MOVE_TO_LEFT_N_2:
                    waypoint = 17;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_N_3, 5000);
                    break;
                case EVENT_MOVE_TO_LEFT_N_3:
                    waypoint = 18;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_N_4, 5000);
                    break;
                case EVENT_MOVE_TO_LEFT_N_4:
                    waypoint = 19;
                    events.ScheduleEvent(EVENT_MOVE_TO_LEFT_N_5, 5000);
                    break;
                case EVENT_MOVE_TO_LEFT_N_5:
                    waypoint = 20;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_N_1, 5000);
                    break;
                case EVENT_MOVE_TO_RIGHT_N_1:
                    waypoint = 21;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_N_2, 5200);
                    break;
                case EVENT_MOVE_TO_RIGHT_N_2:
                    waypoint = 22;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_N_3, 5200);
                    break;
                case EVENT_MOVE_TO_RIGHT_N_3:
                    waypoint = 23;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_N_4, 5200);
                    break;
                case EVENT_MOVE_TO_RIGHT_N_4:
                    waypoint = 24;
                    events.ScheduleEvent(EVENT_MOVE_TO_RIGHT_N_5, 5200);
                    break;
                case EVENT_MOVE_TO_RIGHT_N_5:
                    waypoint = 25;
                    events.ScheduleEvent(EVENT_MOVE_TO_START_OF_TOP, 500);
                    break;
                case EVENT_PHASE_2_TORNADOS_START:
                    events.CancelEvent(EVENT_SUMMON_WORMS);
                    events.CancelEvent(EVENT_SUMMON_TALON);
                    m_creature->RemoveAurasDueToSpell(SPELL_BLAZING_CLAW);
                    m_creature->SummonCreature(53693,FallPos[0].GetPositionX(),FallPos[0].GetPositionY(),FallPos[0].GetPositionZ(),FallPos[0].GetOrientation(),TEMPSUMMON_MANUAL_DESPAWN,0);
                    waypoint = 26;
                    phase = PHASE_TORNADO;
                    CancelMove();
                    events.ScheduleEvent(EVENT_MOVE_TORNADOS_1, 5000);
                    events.ScheduleEvent(EVENT_MOVE_TO_TORNADO, 18000);
                    events.ScheduleEvent(EVENT_PHASE_2_TORNADOS_ENDS, 22000);
                    break;
                case EVENT_HARSH_WINDS:
                    CastHarshOnPlayers();
                    events.ScheduleEvent(EVENT_HARSH_WINDS, 2000);
                    break;
                case EVENT_MOVE_TORNADOS_1:
                    DespawnCreatures(53089);
                    RemoveFly();
                    waypoint = 27;
                    events.ScheduleEvent(EVENT_HARSH_WINDS, 2000);
                    events.ScheduleEvent(EVENT_MOVE_TORNADOS_2, 4000);
                    break;
                case EVENT_MOVE_TORNADOS_2:
                    waypoint = 28;
                    events.ScheduleEvent(EVENT_MOVE_TORNADOS_3, 4000);
                    break;
                case EVENT_MOVE_TORNADOS_3:
                    waypoint = 29;
                    events.ScheduleEvent(EVENT_MOVE_TORNADOS_4, 4000);
                    break;
                case EVENT_MOVE_TORNADOS_4:
                    waypoint = 30;
                    events.ScheduleEvent(EVENT_MOVE_TORNADOS_1, 4000);
                    break;
                case EVENT_SUMMON_DRUIDS:
                    for(uint8 i = 0;i<2;++i)
                        m_creature->SummonCreature(53734,ClawTalon[i].x,ClawTalon[i].y,ClawTalon[i].z,ClawTalon[i].o,TEMPSUMMON_MANUAL_DESPAWN,0);
                    break;
                case EVENT_PHASE_2_TORNADOS_ENDS:
                    events.CancelEvent(EVENT_HARSH_WINDS);
                    CancelMove();
                    DespawnCreatures(53693);
                    DespawnCreatures(53698);
                    phase = PHASE_GROUND;
                    DoCast(m_creature,SPELL_BURNOUT);
                    DoCast(m_creature,SPELL_SPARK);
                    m_creature->SetPower(POWER_ENERGY,0);
                    DoCast(m_creature,SPELL_ZERO_POWER);
                    m_creature->GetMotionMaster()->MoveJump(FallPos[0].GetPositionX(),FallPos[0].GetPositionY(),FallPos[0].GetPositionZ(),100,4);
                    events.ScheduleEvent(EVENT_SUMMON_DRUIDS, 500);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    int32 SelectTalon()
    {
        uint8 talon = urand(0,3);

        if (LastTalon != 4)
            while (LastTalon == talon)
                talon = urand(0,3);

        LastTalon = talon;

        if (talon == 0)
            return 0;
        else if (talon == 1)
            return 2;
        else if (talon == 2)
            return 4;
        else if (talon == 3)
            return 6;
        else
            return 6;
    }

    void CastHarshOnPlayers()
    {
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (i->getSource())
                if (Creature* tornado = GetClosestCreatureWithEntry(m_creature,53693, 5000.0f))
                    if (i->getSource()->GetDistance(tornado) > 60.0f)
                        i->getSource()->_AddAura(SPELL_HARSH_WINDS,5000,m_creature);
    }

     void RemoveFly()
    {
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (i->getSource())
                i->getSource()->RemoveAurasDueToSpell(SPELL_FLY);
    }

    void RemoveAllFeathers()
    {
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (i->getSource())
                i->getSource()->RemoveAurasDueToSpell(SPELL_CAST_ON_MOVE_VISUAL);
    }

    void RemoveEncounterAuras()
    {
        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        {
            if (i->getSource())
            {
                i->getSource()->RemoveAurasDueToSpell(SPELL_FLY);
                i->getSource()->RemoveAurasDueToSpell(SPELL_INDICATOR);
                i->getSource()->RemoveAurasDueToSpell(SPELL_CAST_ON_MOVE_VISUAL);
                i->getSource()->RemoveAurasDueToSpell(SPELL_IMPRINTED);
            }
        }
    }
};

CreatureAI* GetAI_boss_alysrazor(Creature* pCreature)
{
    return new boss_alysrazorAI(pCreature);
}

bool mob_molten_feather_OnGossipHello(Player* player, Creature* creature)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)creature->GetInstanceData();
    if (!m_pInstance)
        return;

    if (Creature* alysrazor = m_pInstance->GetSingleCreatureFromStorage(NPC_ALYSRAZOR))
        player->CastSpell(player,SPELL_CAST_ON_MOVE_VISUAL,true);
    else
        player->SummonCreature(NPC_ALYSRAZOR, SpawnPos[0].x, SpawnPos[0].y, SpawnPos[0].z, SpawnPos[0].o, TEMPSUMMON_MANUAL_DESPAWN, 0);

    creature->ForcedDespawn();
    return true;
}

struct MANGOS_DLL_DECL mob_molten_featherAI : public ScriptedAI
{
    mob_molten_featherAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    void Reset() override { DoCast(m_creature, SPELL_COSMETIC); }
};

CreatureAI* GetAI_mob_molten_feather(Creature* pCreature)
{
    return new mob_molten_featherAI(pCreature);
}

/////

struct MANGOS_DLL_DECL mob_incendiary_cloudAI : public ScriptedAI
{
    mob_incendiary_cloudAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    void Reset() override
    {
        bInit = false;
        m_creature->SetDisplayId(11686);
    }

    bool bInit;

    void UpdateAI(const uint32 diff) override
    {
        if (!bInit)
        {
            m_creature->SetRoot(true);
            bInit = true;
            DoCast(m_creature, SPELL_INCENDIARY_CLOUD);
            m_creature->ForcedDespawn(5000);
        }
    }
};

CreatureAI* GetAI_mob_incendiary_cloud(Creature* pCreature)
{
    return new mob_incendiary_cloudAI(pCreature);

}

////

struct MANGOS_DLL_DECL mob_blazing_powerAI : public ScriptedAI
{
    mob_blazing_powerAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    bool bInit;

    void Reset() override
    {
        bInit = false;
        m_creature->SetDisplayId(11686);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!bInit)
        {
            m_creature->SetRoot(true);
            bInit = true;
            DoCast(m_creature, SPELL_BLAZING_POWER);
            m_creature->ForcedDespawn(5000);
        }
    }
};

CreatureAI* GetAI_mob_blazing_power(Creature* pCreature)
{
    return new mob_blazing_powerAI(pCreature);

}

/////

struct MANGOS_DLL_DECL npc_alysrazor_volcanoAI : public ScriptedAI
{
    npc_alysrazor_volcanoAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    bool started;
    uint32 timerAur;

    void Reset() override
    {
        started = false;
        m_creature->SetDisplayId(11686);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!started)
        {
            m_creature->CastSpell(m_creature, SPELL_VOLCANO_DUMMY, true);
            started = true;
            timerAur = 2000;
        }

        if (!started || m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (timerAur <= diff)
        {
            if (!m_creature->HasAura(SPELL_VOLCANO_AURA))
                m_creature->CastSpell(m_creature, SPELL_VOLCANO_AURA, true);
        }
        else
            timerAur -= diff;
    }
};

CreatureAI* GetAI_npc_alysrazor_volcano(Creature* pCreature)
{
    return new npc_alysrazor_volcanoAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_voracious_hatchlingAI : public ScriptedAI
{
    npc_voracious_hatchlingAI(Creature* creature) : ScriptedAI(creature) { }

    EventMap events;

    bool bInit;
    void Reset() override { bInit = false; }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim() || m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (!bInit)
        {
            m_creature->ModifyAuraState(AURA_STATE_UNKNOWN22, true);
            if (Unit* FirstTarget = GetClosestAttackableUnit(m_creature, 300.0f))
            {
                m_creature->AI()->AttackStart(FirstTarget);
                DoCast(FirstTarget, SPELL_IMPRINTED);
                FirstTarget->CastSpell(m_creature, SPELL_TAUNT,false);
                events.ScheduleEvent(EVENT_EAT_WORM, 1000);
                events.ScheduleEvent(EVENT_HUNGRY, 9000);
                events.ScheduleEvent(EVENT_DEBUFF, 15000);
                m_creature->AI()->MoveInLineOfSight(FirstTarget);
            }
            bInit = true;
            return;
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DEBUFF:
                {
                    DoCast(m_creature,SPELL_DEBUFF);
                    events.ScheduleEvent(EVENT_DEBUFF, 60000);
                    break;
                }
                case EVENT_EAT_WORM:
                {
                    if (Creature* worm = GetClosestCreatureWithEntry(m_creature, 53520, 5.0f, true))
                    {
                        worm->ForcedDespawn();
                        DoCast(m_creature, SPELL_SATIATED, true);
                        m_creature->RemoveAurasDueToSpell(SPELL_HUNGRY);
                        m_creature->RemoveAurasDueToSpell(SPELL_TANTRUM);
                    }
                    events.ScheduleEvent(EVENT_EAT_WORM, 1000);
                    break;
                }
                case EVENT_HUNGRY:
                {
                    if (!m_creature->HasAura(SPELL_SATIATED))
                    {
                        DoCast(m_creature,SPELL_HUNGRY,true);
                        events.ScheduleEvent(EVENT_HUNGRY, 9000);
                    }
                    else
                        events.ScheduleEvent(EVENT_HUNGRY, 2000);
                    break;
                }
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_voracious_hatchling(Creature* pCreature)
{
    return new npc_voracious_hatchlingAI(pCreature);
}

/////

struct MANGOS_DLL_DECL npc_plumb_lava_wormAI : public ScriptedAI
{
    npc_plumb_lava_wormAI(Creature* creature) : ScriptedAI(creature) 
    { 
        Reset();
    }

    virtual void Reset() override
    {
        started = false;
    }

    bool started;
    uint32 timerAur,castTimer;

    void UpdateAI(const uint32 diff) override
    {
        if (!started)
        {
            started = true;
            //m_creature->SetReactState(REACT_PASSIVE);
            started = true;
            timerAur = urand(1000,5000);
            castTimer = timerAur + 4000;
            return;
        }

        if (castTimer <= diff)
            DoCast(m_creature, SPELL_LAVA_SPEW);
        else
            castTimer -= diff;

        if (timerAur <= diff)
        {
            if(m_creature->GetOrientation() >= 6.2f)
                m_creature->SetFacingTo(0.0f);
            else
                m_creature->SetFacingTo(m_creature->GetOrientation() + 0.10f);
            //m_creature->SendMovementFlagUpdate();
            timerAur = 200;

        }
        else
            timerAur -= diff;

    }
};

CreatureAI* GetAI_npc_plumb_lava_worm(Creature* pCreature)
{
    return new npc_plumb_lava_wormAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_blazing_broodmotherAI : public npc_escortAI
{
    npc_blazing_broodmotherAI(Creature* c) : npc_escortAI(c) { Reset(); }

    bool IsPlayerOrGroupInRange() override
    {
        return true;
    }

    bool bInit;

    void Reset() override
    {
        bInit = false;
    }

    void InitLeftBirdWaypoints()
    {
        WaypointList.clear();
        AddWaypoint(1, -32.901f, -272.019f, 105.0f, 1000); //left egg pos z+5
        AddWaypoint(2, 46.930f, -207.560f, 90.000f); //left bird point to despawn
    }

    void InitRightBirdWaypoints()
    {
        WaypointList.clear();
        AddWaypoint(1, -44.463f, -285.802f, 105.0f, 1000); //right egg pos z+5
        AddWaypoint(2, -28.813f, -378.088f, 90.371f); //right bird point to despawn
    }

    void WaypointReached(uint32 point) override
    {
        switch (point)
        {
            case 1:
                m_creature->SummonCreature(53681, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
                break;
            case 2:
                m_creature->ForcedDespawn();
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!bInit)
        {
            bInit = true;
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            DoCast(m_creature, SPELL_BROODMOTHER_VIS);
            //me->SetReactState(REACT_PASSIVE);
            //me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            m_creature->SetLevitate(true);
            if (m_creature->GetPositionX() > 0)
                InitLeftBirdWaypoints();
            else
                InitRightBirdWaypoints();
            Start();
        }
        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_blazing_broodmother(Creature* pCreature)
{
    return new npc_blazing_broodmotherAI(pCreature);
}

/////

struct MANGOS_DLL_DECL npc_blazing_talon_clawshaperAI : public npc_escortAI
{
    npc_blazing_talon_clawshaperAI(Creature* c) : npc_escortAI(c) { Reset(); }

    EventMap events;

    bool bInit;
    void Reset() override
    {
        bInit = false;
    }

    bool IsPlayerOrGroupInRange() override
    {
        return true;
    }

    void InitLeftBirdWaypoints()
    {
        AddWaypoint(1, -34.850f, -240.583f, 77.014f, 5000);
    }

    void InitRightBirdWaypoints()
    {
        AddWaypoint(1, -55.794f, -325.271f, 77.766f, 5000);
    }

    void WaypointReached(uint32 point) override
    {
        switch (point)
        {
            case 1:
                //m_creature->SetReactState(REACT_AGGRESSIVE);
                DoCast(m_creature,SPELL_FIREHAWK_CLAWSHAPPER);
                m_creature->GetMotionMaster()->MoveFall();
                events.ScheduleEvent(EVENT_START_COMBAT, 700);

                m_creature->SetLevitate(false);
                m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
                m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
                m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
                break;
            case 2:
                m_creature->ForcedDespawn();
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!bInit)
        {
            bInit = true;
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            m_creature->AttackStop();
            //m_creature->SetReactState(REACT_PASSIVE);
            m_creature->InterruptNonMeleeSpells(false);

            m_creature->SetLevitate(true);
            //SetDespawnAtFar(false);
            if (m_creature->GetPositionX() > -35)
                InitLeftBirdWaypoints();
            else
                InitRightBirdWaypoints();
            Start();
        }

        npc_escortAI::UpdateAI(diff);

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_COMBAT:
                    if (Unit* nearTarget = GetClosestAttackableUnit(m_creature, 5000.0f))
                        m_creature->AI()->MoveInLineOfSight(nearTarget);

                    events.ScheduleEvent(EVENT_START_IGNITION, 500);
                    events.ScheduleEvent(EVENT_CHECK_ENERGY, 1000);
                    break;
                case EVENT_START_IGNITION:
                    if (Creature* alysrazor = GetClosestCreatureWithEntry(m_creature, NPC_ALYSRAZOR, 5000.0f))
                        if (alysrazor->GetPower(POWER_ENERGY) < 50 && !m_creature->IsNonMeleeSpellCasted(false))
                        {
                            m_creature->StopMoving();
                            DoCast(alysrazor,SPELL_IGNITION);
                        }

                    events.ScheduleEvent(EVENT_START_IGNITION, 7000);
                    break;
                case EVENT_CHECK_ENERGY:
                    if (Creature* alysrazor = GetClosestCreatureWithEntry(m_creature, NPC_ALYSRAZOR, 5000.0f))
                    {
                        if (alysrazor->GetPower(POWER_ENERGY) >= 50)
                        {
                            DoCast(m_creature,SPELL_FIREHAWK_REMOVE_FORM);
                            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
                            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
                            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
                            m_creature->SetLevitate(true);
                            m_creature->AttackStop();
                            //m_creature->SetReactState(REACT_PASSIVE);
                            m_creature->InterruptNonMeleeSpells(false);
                            AddWaypoint(2, ClawTalon[m_creature->GetPositionX() > -35 ? 0 : 1].GetPositionX(), ClawTalon[m_creature->GetPositionX() > -35 ? 0 : 1].GetPositionY(), ClawTalon[m_creature->GetPositionX() > -35 ? 0 : 1].GetPositionZ(), 2000);
                        }
                        else
                            events.ScheduleEvent(EVENT_CHECK_ENERGY, 1000);
                    }
                    else
                        m_creature->ForcedDespawn();
                    break;
            }
        }
    }
};

CreatureAI* GetAI_npc_blazing_talon_clawshaper(Creature* pCreature)
{
    return new npc_blazing_talon_clawshaperAI(pCreature);
}

/////

struct MANGOS_DLL_DECL npc_blazing_talonAI : public npc_escortAI
{
    npc_blazing_talonAI(Creature* c) : npc_escortAI(c) { Reset(); }

    EventMap events;
    uint8 waypoint;

    bool bInit;
    void Reset() override { bInit = false; }

    bool IsPlayerOrGroupInRange() override
    {
        return true;
    }

    void InitBirdWaypoints()
    {
        AddWaypoint(1, Talon[waypoint].GetPositionX(), Talon[waypoint].GetPositionY(), Talon[waypoint].GetPositionZ(), 3000);
    }

    void WaypointReached(uint32 point) override
    {
        switch (point)
        {
            case 1:
                //m_creature->SetReactState(REACT_AGGRESSIVE);
                DoCast(m_creature,urand(0,1) == 0 ? SPELL_FIREHAWK_TRANSFORM_M : SPELL_FIREHAWK_TRANSFORM_F);
                m_creature->GetMotionMaster()->MoveFall();

                events.ScheduleEvent(EVENT_START_COMBAT, 500);

                m_creature->SetLevitate(false);
                m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
                m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
                m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!bInit)
        {
            bInit = true;
            m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
            m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
            m_creature->AttackStop();
            //m_creature->SetReactState(REACT_PASSIVE);
            m_creature->InterruptNonMeleeSpells(false);

            m_creature->SetLevitate(true);
            //SetDespawnAtFar(false);
            for (uint8 i = 0;i<4;++i)
            {
                uint8 tal = 0;
                if (i == 0)
                    tal = 0;
                else if (i == 1)
                    tal = 2;
                else if (i == 2)
                    tal = 4;
                else if (i == 3)
                    tal = 6;

                if (Talon[tal].GetPositionX() == m_creature->GetPositionX())
                    waypoint = ++tal;
            }
            InitBirdWaypoints();
            Start();
        }

        npc_escortAI::UpdateAI(diff);

        if (m_creature->IsNonMeleeSpellCasted(false)5)
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_COMBAT:
                    if (Unit* nearTarget = GetClosestAttackableUnit(m_creature, 5000.0f))
                        m_creature->AI()->MoveInLineOfSight(nearTarget);

                    events.ScheduleEvent(urand(0,1) == 0 ? EVENT_START_FIEROBLAST : EVENT_START_BRUSHFIRE, 500);
                    break;
                case EVENT_START_FIEROBLAST:
                    DoCast(m_creature,SPELL_FIEROBLAST);

                    events.ScheduleEvent(EVENT_START_BRUSHFIRE, 3000);
                    break;
                case EVENT_START_BRUSHFIRE:
                    if (Unit* tempTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(tempTarget,SPELL_BRUSHFIRE);

                    events.ScheduleEvent(urand(0,1) == 0 ? EVENT_START_FIEROBLAST : EVENT_START_BRUSHFIRE, 3000);
                    break;
            }
        }
    }
};

CreatureAI* GetAI_npc_blazing_talon(Creature* pCreature)
{
    return new npc_blazing_talonAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_brushfireAI : public ScriptedAI
{
    npc_brushfireAI(Creature* creature) : ScriptedAI(creature) 
    {
        Reset();
    }

    void Reset() override
    {
        started = false;
        needJump = false;
        m_creature->SetDisplayId(11686);
    }

    bool started,needJump;
    uint32 timerMove,jump,timerDespawn;

    void UpdateAI(const uint32 diff) override
    {
        if (!started)
        {
            started = true;
            m_creature->SetFacingTo(urand(0,6));
            DoCast(m_creature,SPELL_BRUSHFIRE_VIS);
            //m_creature->SetReactState(REACT_PASSIVE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            started = true;
            needJump = true;
            jump = 500;
            timerMove = 1000;
            timerDespawn = 20000;
            return;
        }

        if (jump <= diff && needJump)
        {
            m_creature->GetMotionMaster()->MoveJump(m_creature->GetPositionX(),m_creature->GetPositionY(),60.0f,200,8);
            needJump = false;
        }
        else
            jump -= diff;

        if (timerMove <= diff) // move forward
        {
            m_creature->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX() + cos(m_creature->GetOrientation()) * 35, m_creature->GetPositionY() + sin(m_creature->GetOrientation()) * 35, 58.0f);
        }
        else
            timerMove -= diff;

        if (timerDespawn <= diff) // move forward
        {
            m_creature->ForcedDespawn();
        }
        else
            timerDespawn -= diff;
    }
};

CreatureAI* GetAI_npc_brushfire(Creature* pCreature)
{
    return new npc_brushfireAI(pCreature);
}

/////

struct MANGOS_DLL_DECL npc_fiery_tornadoAI : public npc_escortAI
{
    npc_fiery_tornadoAI(Creature* c) : npc_escortAI(c)
    {
        Reset();
    }

    void Reset() override
    {
        m_creature->SetDisplayId(11686);
        bInit = false;
    }

    uint32 numb;
    bool bInit;

    void InitWaypoints()
    {
        if (numb == 0)
        {
            AddWaypoint(1, Tornado[1].GetPositionX(), Tornado[1].GetPositionY(), Tornado[1].GetPositionZ());
            AddWaypoint(2, Tornado[2].GetPositionX(), Tornado[2].GetPositionY(), Tornado[2].GetPositionZ());
            AddWaypoint(3, Tornado[3].GetPositionX(), Tornado[3].GetPositionY(), Tornado[3].GetPositionZ());
            AddWaypoint(4, Tornado[0].GetPositionX(), Tornado[0].GetPositionY(), Tornado[0].GetPositionZ());
        }
        else if (numb == 4)
        {
            AddWaypoint(1, Tornado[5].GetPositionX(), Tornado[5].GetPositionY(), Tornado[5].GetPositionZ());
            AddWaypoint(2, Tornado[6].GetPositionX(), Tornado[6].GetPositionY(), Tornado[6].GetPositionZ());
            AddWaypoint(3, Tornado[7].GetPositionX(), Tornado[7].GetPositionY(), Tornado[7].GetPositionZ());
            AddWaypoint(4, Tornado[8].GetPositionX(), Tornado[8].GetPositionY(), Tornado[8].GetPositionZ());
            AddWaypoint(5, Tornado[9].GetPositionX(), Tornado[9].GetPositionY(), Tornado[9].GetPositionZ());
            AddWaypoint(6, Tornado[4].GetPositionX(), Tornado[4].GetPositionY(), Tornado[4].GetPositionZ());
        }
        else if (numb == 10)
        {
            AddWaypoint(1, Tornado[11].GetPositionX(), Tornado[11].GetPositionY(), Tornado[11].GetPositionZ());
            AddWaypoint(2, Tornado[12].GetPositionX(), Tornado[12].GetPositionY(), Tornado[12].GetPositionZ());
            AddWaypoint(3, Tornado[13].GetPositionX(), Tornado[13].GetPositionY(), Tornado[13].GetPositionZ());
            AddWaypoint(4, Tornado[14].GetPositionX(), Tornado[14].GetPositionY(), Tornado[14].GetPositionZ());
            AddWaypoint(5, Tornado[15].GetPositionX(), Tornado[15].GetPositionY(), Tornado[15].GetPositionZ());
            AddWaypoint(6, Tornado[10].GetPositionX(), Tornado[10].GetPositionY(), Tornado[10].GetPositionZ());
        }
        else if (numb == 16)
        {
            AddWaypoint(1, Tornado[17].GetPositionX(), Tornado[17].GetPositionY(), Tornado[17].GetPositionZ());
            AddWaypoint(2, Tornado[18].GetPositionX(), Tornado[18].GetPositionY(), Tornado[18].GetPositionZ());
            AddWaypoint(3, Tornado[19].GetPositionX(), Tornado[19].GetPositionY(), Tornado[19].GetPositionZ());
            AddWaypoint(4, Tornado[20].GetPositionX(), Tornado[20].GetPositionY(), Tornado[20].GetPositionZ());
            AddWaypoint(5, Tornado[21].GetPositionX(), Tornado[21].GetPositionY(), Tornado[21].GetPositionZ());
            AddWaypoint(6, Tornado[16].GetPositionX(), Tornado[16].GetPositionY(), Tornado[16].GetPositionZ());
        }
        else if (numb == 22)
        {
            AddWaypoint(1, Tornado[23].GetPositionX(), Tornado[23].GetPositionY(), Tornado[23].GetPositionZ());
            AddWaypoint(2, Tornado[24].GetPositionX(), Tornado[24].GetPositionY(), Tornado[24].GetPositionZ());
            AddWaypoint(3, Tornado[25].GetPositionX(), Tornado[25].GetPositionY(), Tornado[25].GetPositionZ());
            AddWaypoint(4, Tornado[26].GetPositionX(), Tornado[26].GetPositionY(), Tornado[26].GetPositionZ());
            AddWaypoint(5, Tornado[27].GetPositionX(), Tornado[27].GetPositionY(), Tornado[27].GetPositionZ());
            AddWaypoint(6, Tornado[22].GetPositionX(), Tornado[22].GetPositionY(), Tornado[22].GetPositionZ());
        }
        else if (numb == 28)
        {
            AddWaypoint(1, Tornado[29].GetPositionX(), Tornado[29].GetPositionY(), Tornado[29].GetPositionZ());
            AddWaypoint(2, Tornado[30].GetPositionX(), Tornado[30].GetPositionY(), Tornado[30].GetPositionZ());
            AddWaypoint(3, Tornado[31].GetPositionX(), Tornado[31].GetPositionY(), Tornado[31].GetPositionZ());
            AddWaypoint(4, Tornado[32].GetPositionX(), Tornado[32].GetPositionY(), Tornado[32].GetPositionZ());
            AddWaypoint(5, Tornado[33].GetPositionX(), Tornado[33].GetPositionY(), Tornado[33].GetPositionZ());
            AddWaypoint(6, Tornado[28].GetPositionX(), Tornado[28].GetPositionY(), Tornado[28].GetPositionZ());
        }
        else if (numb == 34)
        {
            AddWaypoint(1, Tornado[35].GetPositionX(), Tornado[35].GetPositionY(), Tornado[35].GetPositionZ());
            AddWaypoint(2, Tornado[36].GetPositionX(), Tornado[36].GetPositionY(), Tornado[36].GetPositionZ());
            AddWaypoint(3, Tornado[37].GetPositionX(), Tornado[37].GetPositionY(), Tornado[37].GetPositionZ());
            AddWaypoint(4, Tornado[38].GetPositionX(), Tornado[38].GetPositionY(), Tornado[38].GetPositionZ());
            AddWaypoint(5, Tornado[39].GetPositionX(), Tornado[39].GetPositionY(), Tornado[39].GetPositionZ());
            AddWaypoint(6, Tornado[34].GetPositionX(), Tornado[34].GetPositionY(), Tornado[34].GetPositionZ());
        }
        else if (numb == 40)
        {
            AddWaypoint(1, Tornado[41].GetPositionX(), Tornado[41].GetPositionY(), Tornado[41].GetPositionZ());
            AddWaypoint(2, Tornado[42].GetPositionX(), Tornado[42].GetPositionY(), Tornado[42].GetPositionZ());
            AddWaypoint(3, Tornado[43].GetPositionX(), Tornado[43].GetPositionY(), Tornado[43].GetPositionZ());
            AddWaypoint(4, Tornado[44].GetPositionX(), Tornado[44].GetPositionY(), Tornado[44].GetPositionZ());
            AddWaypoint(5, Tornado[45].GetPositionX(), Tornado[45].GetPositionY(), Tornado[45].GetPositionZ());
            AddWaypoint(6, Tornado[40].GetPositionX(), Tornado[40].GetPositionY(), Tornado[40].GetPositionZ());
        }
        else if (numb == 46)
        {
            AddWaypoint(1, Tornado[47].GetPositionX(), Tornado[47].GetPositionY(), Tornado[47].GetPositionZ());
            AddWaypoint(2, Tornado[48].GetPositionX(), Tornado[48].GetPositionY(), Tornado[48].GetPositionZ());
            AddWaypoint(3, Tornado[49].GetPositionX(), Tornado[49].GetPositionY(), Tornado[49].GetPositionZ());
            AddWaypoint(4, Tornado[50].GetPositionX(), Tornado[50].GetPositionY(), Tornado[50].GetPositionZ());
            AddWaypoint(5, Tornado[51].GetPositionX(), Tornado[51].GetPositionY(), Tornado[51].GetPositionZ());
            AddWaypoint(6, Tornado[46].GetPositionX(), Tornado[46].GetPositionY(), Tornado[46].GetPositionZ());
        }
        else if (numb == 52)
        {
            AddWaypoint(1, Tornado[53].GetPositionX(), Tornado[53].GetPositionY(), Tornado[53].GetPositionZ());
            AddWaypoint(2, Tornado[54].GetPositionX(), Tornado[54].GetPositionY(), Tornado[54].GetPositionZ());
            AddWaypoint(3, Tornado[55].GetPositionX(), Tornado[55].GetPositionY(), Tornado[55].GetPositionZ());
            AddWaypoint(4, Tornado[56].GetPositionX(), Tornado[56].GetPositionY(), Tornado[56].GetPositionZ());
            AddWaypoint(5, Tornado[57].GetPositionX(), Tornado[57].GetPositionY(), Tornado[57].GetPositionZ());
            AddWaypoint(6, Tornado[52].GetPositionX(), Tornado[52].GetPositionY(), Tornado[52].GetPositionZ());
        }
        else if (numb == 58)
        {
            AddWaypoint(1, Tornado[59].GetPositionX(), Tornado[59].GetPositionY(), Tornado[59].GetPositionZ());
            AddWaypoint(2, Tornado[60].GetPositionX(), Tornado[60].GetPositionY(), Tornado[60].GetPositionZ());
            AddWaypoint(3, Tornado[61].GetPositionX(), Tornado[61].GetPositionY(), Tornado[61].GetPositionZ());
            AddWaypoint(4, Tornado[62].GetPositionX(), Tornado[62].GetPositionY(), Tornado[62].GetPositionZ());
            AddWaypoint(5, Tornado[63].GetPositionX(), Tornado[63].GetPositionY(), Tornado[63].GetPositionZ());
            AddWaypoint(6, Tornado[58].GetPositionX(), Tornado[58].GetPositionY(), Tornado[58].GetPositionZ());
        }
        else if (numb == 58)
        {
            AddWaypoint(1, Tornado[59].GetPositionX(), Tornado[59].GetPositionY(), Tornado[59].GetPositionZ());
            AddWaypoint(2, Tornado[60].GetPositionX(), Tornado[60].GetPositionY(), Tornado[60].GetPositionZ());
            AddWaypoint(3, Tornado[61].GetPositionX(), Tornado[61].GetPositionY(), Tornado[61].GetPositionZ());
            AddWaypoint(4, Tornado[62].GetPositionX(), Tornado[62].GetPositionY(), Tornado[62].GetPositionZ());
            AddWaypoint(5, Tornado[63].GetPositionX(), Tornado[63].GetPositionY(), Tornado[63].GetPositionZ());
            AddWaypoint(6, Tornado[58].GetPositionX(), Tornado[58].GetPositionY(), Tornado[58].GetPositionZ());
        }
        else if (numb == 62)
        {
            AddWaypoint(1, Tornado[63].GetPositionX(), Tornado[63].GetPositionY(), Tornado[63].GetPositionZ());
            AddWaypoint(2, Tornado[64].GetPositionX(), Tornado[64].GetPositionY(), Tornado[64].GetPositionZ());
            AddWaypoint(3, Tornado[65].GetPositionX(), Tornado[65].GetPositionY(), Tornado[65].GetPositionZ());
            AddWaypoint(4, Tornado[66].GetPositionX(), Tornado[66].GetPositionY(), Tornado[66].GetPositionZ());
            AddWaypoint(5, Tornado[67].GetPositionX(), Tornado[67].GetPositionY(), Tornado[67].GetPositionZ());
            AddWaypoint(6, Tornado[62].GetPositionX(), Tornado[62].GetPositionY(), Tornado[72].GetPositionZ());
        }
        else if (numb == 68)
        {
            AddWaypoint(1, Tornado[69].GetPositionX(), Tornado[69].GetPositionY(), Tornado[69].GetPositionZ());
            AddWaypoint(2, Tornado[70].GetPositionX(), Tornado[70].GetPositionY(), Tornado[70].GetPositionZ());
            AddWaypoint(3, Tornado[71].GetPositionX(), Tornado[71].GetPositionY(), Tornado[71].GetPositionZ());
            AddWaypoint(4, Tornado[72].GetPositionX(), Tornado[72].GetPositionY(), Tornado[72].GetPositionZ());
            AddWaypoint(5, Tornado[73].GetPositionX(), Tornado[73].GetPositionY(), Tornado[73].GetPositionZ());
            AddWaypoint(6, Tornado[68].GetPositionX(), Tornado[68].GetPositionY(), Tornado[68].GetPositionZ());
        }
        else if (numb == 74)
        {
            AddWaypoint(1, Tornado[75].GetPositionX(), Tornado[75].GetPositionY(), Tornado[75].GetPositionZ());
            AddWaypoint(2, Tornado[76].GetPositionX(), Tornado[76].GetPositionY(), Tornado[76].GetPositionZ());
            AddWaypoint(3, Tornado[77].GetPositionX(), Tornado[77].GetPositionY(), Tornado[77].GetPositionZ());
            AddWaypoint(4, Tornado[78].GetPositionX(), Tornado[78].GetPositionY(), Tornado[78].GetPositionZ());
            AddWaypoint(5, Tornado[79].GetPositionX(), Tornado[79].GetPositionY(), Tornado[79].GetPositionZ());
            AddWaypoint(6, Tornado[74].GetPositionX(), Tornado[74].GetPositionY(), Tornado[74].GetPositionZ());
        }
        else if (numb == 80)
        {
            AddWaypoint(1, Tornado[80].GetPositionX(), Tornado[80].GetPositionY(), Tornado[80].GetPositionZ());
            AddWaypoint(2, Tornado[81].GetPositionX(), Tornado[81].GetPositionY(), Tornado[81].GetPositionZ());
            AddWaypoint(3, Tornado[82].GetPositionX(), Tornado[82].GetPositionY(), Tornado[82].GetPositionZ());
            AddWaypoint(4, Tornado[83].GetPositionX(), Tornado[83].GetPositionY(), Tornado[83].GetPositionZ());
            AddWaypoint(5, Tornado[84].GetPositionX(), Tornado[84].GetPositionY(), Tornado[84].GetPositionZ());
            AddWaypoint(6, Tornado[80].GetPositionX(), Tornado[80].GetPositionY(), Tornado[80].GetPositionZ());
        }
    }

    void WaypointReached(uint32 point)
    {
        if ((point == 4 && numb == 0) || (point == 6))
            InitWaypoints();
    }

    bool IsPlayerOrGroupInRange() override
    {
        return true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!bInit)
        {
            bInit = true;
            //m_creature->SetReactState(REACT_PASSIVE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);

            m_creature->SetLevitate(true);
            //SetDespawnAtFar(false);
            for (uint8 i = 0;i<14;++i)
            {
                uint8 tal = 0;
                if (i == 0)
                    tal = 0;
                else if (i == 1)
                    tal = 4;
                else if (i == 2)
                    tal = 10;
                else if (i == 3)
                    tal = 16;
                else if (i == 4)
                    tal = 22;
                else if (i == 5)
                    tal = 28;
                else if (i == 6)
                    tal = 34;
                else if (i == 7)
                    tal = 40;
                else if (i == 8)
                    tal = 46;
                else if (i == 8)
                    tal = 52;
                else if (i == 9)
                    tal = 58;
                else if (i == 10)
                    tal = 64;
                else if (i == 11)
                    tal = 70;
                else if (i == 12)
                    tal = 76;
                else if (i == 13)
                    tal = 82;
                if ((Tornado[tal].GetPositionX() - m_creature->GetPositionX()) < 0.01f)
                {
                    numb = tal;
                    break;
                }
            }
            InitWaypoints();
            Start();
        }

        npc_escortAI::UpdateAI(diff);
    }
};

CreatureAI* GetAI_npc_fiery_tornado(Creature* pCreature)
{
    return new npc_fiery_tornadoAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_molten_eggAI : public ScriptedAI
{
    npc_molten_eggAI(Creature* creature) : ScriptedAI(creature) 
    {
        Reset();
    }

    void Reset() override
    {
        started = false;
    }

    EventMap events;
    bool started;

    void UpdateAI(const uint32 diff)
    {
        if (!started)
        {
            started = true;
            m_creature->SetPosition(m_creature->GetPositionX(),m_creature->GetPositionY(),56.00f,m_creature->GetOrientation());
            //me->SetReactState(REACT_PASSIVE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
            events.ScheduleEvent(EVENT_EGG_EXPLOSION, 3500);
            started = true;
            return;
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_EGG_EXPLOSION:
                {
                    DoCast(m_creature, SPELL_EGG_EXPLOSION);
                    events.ScheduleEvent(EVENT_SUMMON_HATCHING, 5000);
                    break;
                }
                case EVENT_SUMMON_HATCHING:
                    DoCast(m_creature, SPELL_SUMM_HATHING);
                    events.ScheduleEvent(EVENT_DESPAWN, 600);
                    break;
                case EVENT_DESPAWN:
                    m_creature->ForcedDespawn();
                    break;
            }
        }
    }
};

CreatureAI* GetAI_npc_molten_egg(Creature* pCreature)
{
    return new npc_molten_eggAI(pCreature);
}

struct MANGOS_DLL_DECL npc_fier_tornadoAI : public ScriptedAI
{
    npc_fier_tornadoAI(Creature* creature) : ScriptedAI(creature)
    { 
        Reset();
    }

    void Reset() override
    {
        m_creature->SetDisplayId(11686);
        started = false;
        needSummon = false;
    }

    bool started,needSummon;
    uint32 timerSummon;

    void UpdateAI(const uint32 diff)
    {
        if (!started)
        {
            started = true;
            DoCast(m_creature,SPELL_SUMMON_DMG);
            timerSummon = 5000;
            needSummon = true;
            return;
        }

        if (timerSummon <= diff && needSummon)
        {
            for (uint8 i = 0;i<14;++i)
            {
                uint8 tal = 0;
                if (i == 0)
                    tal = 0;
                else if (i == 1)
                    tal = 4;
                else if (i == 2)
                    tal = 10;
                else if (i == 3)
                    tal = 16;
                else if (i == 4)
                    tal = 22;
                else if (i == 5)
                    tal = 28;
                else if (i == 6)
                    tal = 34;
                else if (i == 7)
                    tal = 40;
                else if (i == 8)
                    tal = 46;
                else if (i == 8)
                    tal = 52;
                else if (i == 9)
                    tal = 58;
                else if (i == 10)
                    tal = 64;
                else if (i == 11)
                    tal = 70;
                else if (i == 12)
                    tal = 76;
                else if (i == 13)
                    tal = 82;
                m_creature->SummonCreature(53698,Tornado[tal].x,Tornado[tal].y,Tornado[tal].z,Tornado[tal].o,TEMPSUMMON_MANUAL_DESPAWN,0);
            }
            needSummon = false;
        }
        else timerSummon -= diff;
    }
};

CreatureAI* GetAI_npc_fier_tornado(Creature* pCreature)
{
    return new npc_fier_tornadoAI(pCreature);
}

///


void AddSC_boss_alysrazor()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_alysrazor";
    pNewScript->GetAI = &GetAI_boss_alysrazor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_molten_feather";
    pNewScript->GetAI = &GetAI_mob_molten_feather;
    pNewScript->pGossipHello = &mob_molten_feather_OnGossipHello;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_incendiary_cloud";
    pNewScript->GetAI = &GetAI_mob_incendiary_cloud;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_blazing_power";
    pNewScript->GetAI = &GetAI_mob_incendiary_cloud;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_alysrazor_volcano";
    pNewScript->GetAI = &GetAI_npc_alysrazor_volcano;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_voracious_hatchling";
    pNewScript->GetAI = &GetAI_npc_voracious_hatchling;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_plumb_lava_worm";
    pNewScript->GetAI = &GetAI_npc_plumb_lava_worm;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blazing_broodmother";
    pNewScript->GetAI = &GetAI_npc_blazing_broodmother;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blazing_talon_clawshaper";
    pNewScript->GetAI = &GetAI_npc_blazing_talon_clawshaper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blazing_talon";
    pNewScript->GetAI = &GetAI_npc_blazing_talon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_brushfire";
    pNewScript->GetAI = &GetAI_npc_brushfire;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fiery_tornado";
    pNewScript->GetAI = &GetAI_npc_fiery_tornado;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_molten_egg";
    pNewScript->GetAI = &GetAI_npc_molten_egg;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fier_tornado";
    pNewScript->GetAI = &GetAI_npc_fier_tornado;
    pNewScript->RegisterSelf();
}
