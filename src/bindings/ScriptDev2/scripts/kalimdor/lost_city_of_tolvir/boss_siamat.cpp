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
SDName: boss_siamat
SD%Complete: 10
SDComment: Placeholder
SDCategory: Lost City of Tol'Vir
EndScriptData */

#include "precompiled.h"
#include "lost_city_of_tolvir.h"

enum Texts
{
    SAY_INTRO = 0,
    SAY_AGGRO = 1,
    SAY_EVENT_1 = 2,
    SAY_EVENT_2 = 2,
    SAY_EVENT_3 = 2,
    SAY_KILL = 3,
    SAY_DEATH = 4,
};

enum Spells
{
    // Siamat
    SPELL_DEFLECTING_WINDS = 84589, // Initial shield
    SPELL_STORM_BOLT_CASTING = 73564,
    SPELL_STORM_BOLT_RANDOM = 91853,
    H_SPELL_STORM_BOLT_RANDOM =95180 ,
    SPELL_ABSORB_STORMS = 83151, // AOE
    SPELL_CLOUD_BURST_SUMMON = 83790,
    SPELL_WAILING_WINDS = 90031,

    // Minion Of Siamat
    SPELL_DEPLETION = 84550, // Aura
    SPELL_TEMPEST_STORM = 83446,
    H_SPELL_TEMPEST_STORM = 90030,
    SPELL_TEMPEST_STORM_FORM =83170, // Form Tempest
    SPELL_TEMPEST_STORM_AURA = 83406, // Aura Tempest
    SPELL_TEMPEST_STORM_ROOT = 84616, // Pre summon Tempest
    SPELL_TEMPEST_STORM_SUMMON = 83414,
    SPELL_CHAIN_LIGHTNING = 83455,
    H_SPELL_CHAIN_LIGHTNING = 90027,

    // Servant Of Siamat
    SPELL_THUNDER_CRASH = 84522,
    H_SPELL_THUNDER_CRASH = 90016,
    SPELL_LIGHTNING_NOVA = 84544,
    H_SPELL_LIGHTNING_NOVA = 90015,
    SPELL_LIGHTNING_CHARGE = 91872,

    // Cloud
    SPELL_CLOUD_BURST_VISUAL = 83048, // Aura Cloud
    SPELL_CLOUD_BURST = 83051,
    H_SPELL_CLOUD_BURST = 90032,

    // Wind Tunnel
    SPELL_VISUAL_WIND = 89698,
};

enum Events
{
    // Siamat Event
    EVENT_DEFLECTING_WINDS = 1,
    EVENT_STORM_BOLT_RANDOM = 2,
    EVENT_REMOVE_DEFLECTIVE_WINGS = 4,
    EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT = 5,
    EVENT_SUMMON_NPC_MINION_OF_SIAMAT = 6,
    EVENT_SUMMON_NPC_MINION_OF_SIAMAT_STORM = 7,
    EVENT_WAILING_WINDS = 8,
    EVENT_ABSORB_STORMS = 9,

    // Servant Of Siamat Event
    EVENT_THUNDER_CRASH = 10,
    EVENT_LIGHTNING_NOVA = 11,
    EVENT_LIGHTNING_CHARGE = 12,
    EVENT_SERVANT_DEATH = 13,

    // Cloud Event
    EVENT_CLOUD_BURST_SUMMON = 14,
    EVENT_CLOUD_BURST = 15,
    EVENT_CLOUD_BURST_VISUAL = 16,
    EVENT_DESPAWN_CLOUD_BURST = 17,
    
    // Minion Of Siamat Event
    EVENT_TEMPEST_STORM = 18,
    EVENT_TEMPEST_STORM_SUMMON = 19,
    EVENT_DEPLETION = 20,
    EVENT_DESPAWN_STORMS = 21,
};

enum Timers
{
    TIME_BETWEEN_SERVANT_RESPAWN = 45000,
    TIME_BETWEEN_MINION_RESPAWN = 25000,
};

Position2 const SummonPositions[6] =
{
    {-10932.976563f, -1412.421875f, 38.000f, 2.3993f}, // Servant position 1
    {-10944.905273f, -1379.742554f, 38.000f, 4.4963f}, // Servant position 2
    {-10966.986328f, -1406.212524f, 38.000f, 0.4218f}, // Servant position 3
    {-10920.614000f, -1389.605000f, 38.000f, 3.5585f}, // minion position 1
    {-10970.000000f, -1380.000000f, 38.000f, 5.6060f}, // minion position 2
    {-10953.592700f, -1427.816000f, 38.000f, 1.3327f}, // minion position 3
};

struct MANGOS_DLL_DECL boss_siamatAI : public ScriptedAI
{
    boss_siamatAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    uint32 DATA_REMOVE_DEFLECTIVE_WINGS;
    uint32 timer[2];
    std::list<ObjectGuid> Summons;

    void Reset() override
    {
        timer[0] = 0;
        timer[1] = 0;
        DATA_REMOVE_DEFLECTIVE_WINGS = 1;
        events.Reset();
        DespawnAllSummon();
        events.CancelEvent(EVENT_DEFLECTING_WINDS);
        events.ScheduleEvent(EVENT_STORM_BOLT_RANDOM, urand(10000, 17000));
        events.ScheduleEvent(EVENT_CLOUD_BURST, urand(10000, 15000));
        events.ScheduleEvent(EVENT_DEFLECTING_WINDS, 0);
        events.ScheduleEvent(EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT, 0);
        events.ScheduleEvent(EVENT_SUMMON_NPC_MINION_OF_SIAMAT, urand(0, 4 * IN_MILLISECONDS));
    }

    void DespawnAllSummon()
    {
        for (std::list<ObjectGuid>::iterator itr = Summons.begin(); itr != Summons.end(); ++itr)
            if (Creature* creature = m_creature->GetMap()->GetAnyTypeCreature(*itr))
                creature->ForcedDespawn();
    }

    void JustSummoned(Creature* pSummoned) override
    {
        Summons.push_back(pSummoned->GetObjectGuid());

        if (pSummoned->GetEntry() == NPC_MINION_OF_SIAMAT_STORM && pSummoned->AI())
        {
            pSummoned->CastSpell(m_creature, SPELL_TEMPEST_STORM_AURA, true);
            pSummoned->CastSpell(m_creature, SPELL_TEMPEST_STORM_FORM, true);
        }

        if (pSummoned->AI())
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
    }

    void SummonedCreatureJustDied(Creature *pSummoned) override
    {
        if (pSummoned->GetEntry() == NPC_SERVANT_OF_SIAMAT_FINAL)
        {
            events.ScheduleEvent(EVENT_REMOVE_DEFLECTIVE_WINGS, 0);
            events.ScheduleEvent(EVENT_WAILING_WINDS, 0);
        }
        else if (pSummoned->GetEntry() == NPC_SERVANT_OF_SIAMAT_NORMAL || pSummoned->GetEntry() == NPC_SERVANT_OF_SIAMAT_HEROIC)
        {
            events.ScheduleEvent(EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT, 0);
        }
        else if (pSummoned->GetEntry() == NPC_MINION_OF_SIAMAT)
        {
            events.ScheduleEvent(EVENT_TEMPEST_STORM_SUMMON, 0);
        }
    }

    void Aggro(Unit* pWho) override
    {
        //Talk(SAY_INTRO);
        //Talk(SAY_AGGRO);
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SIAMAT, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        //Talk(SAY_DEATH);
        DespawnAllSummon();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SIAMAT, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        //Talk(SAY_KILL);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SIAMAT, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (timer[0] >= TIME_BETWEEN_SERVANT_RESPAWN)
        {
            timer[0] = 0;
            events.ScheduleEvent(EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT, 0);
        }
        else
            timer[0] += uiDiff;

        if (timer[1] >= TIME_BETWEEN_MINION_RESPAWN)
        {
            timer[1] = 0;
            events.ScheduleEvent(EVENT_SUMMON_NPC_MINION_OF_SIAMAT, 0);
        }
        else
            timer[1] += uiDiff;

        events.Update(uiDiff);

        while(uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_DEFLECTING_WINDS:
                    DoCast(m_creature, SPELL_DEFLECTING_WINDS);
                    break;
                case EVENT_REMOVE_DEFLECTIVE_WINGS:
                    m_creature->RemoveAurasDueToSpell(SPELL_DEFLECTING_WINDS);
                    break;
                case EVENT_SUMMON_NPC_SERVANT_OF_SIAMAT:
                    //Talk(SAY_EVENT_3);
                    if (DATA_REMOVE_DEFLECTIVE_WINGS < 3)
                    {
                        uint8 id = urand(0,2);
                        m_creature->SummonCreature(m_bIsRegularMode ? NPC_SERVANT_OF_SIAMAT_NORMAL : NPC_SERVANT_OF_SIAMAT_HEROIC, SummonPositions[id].x, SummonPositions[id].y,SummonPositions[id].z,SummonPositions[id].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
                        DATA_REMOVE_DEFLECTIVE_WINGS++;
                    }
                    else if (DATA_REMOVE_DEFLECTIVE_WINGS == 3)
                    {
                        uint8 id = urand(0,2);
                        m_creature->SummonCreature(NPC_SERVANT_OF_SIAMAT_FINAL, SummonPositions[id].x, SummonPositions[id].y,SummonPositions[id].z,SummonPositions[id].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
                        DATA_REMOVE_DEFLECTIVE_WINGS = -1; // Stop Summons
                    }
                    break;
                case EVENT_SUMMON_NPC_MINION_OF_SIAMAT:
                {
                    //Talk(SAY_EVENT_2);
                    uint8 id = urand(3,5);
                    m_creature->SummonCreature(NPC_MINION_OF_SIAMAT, SummonPositions[id].x, SummonPositions[id].y,SummonPositions[id].z,SummonPositions[id].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
                    break;
                }
                case EVENT_STORM_BOLT_RANDOM:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_STORM_BOLT_RANDOM : H_SPELL_STORM_BOLT_RANDOM, true);

                    events.ScheduleEvent(EVENT_STORM_BOLT_RANDOM, urand(20000, 25000));
                    break;
                case EVENT_CLOUD_BURST:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_CLOUD_BURST_SUMMON, true);

                    events.ScheduleEvent(EVENT_CLOUD_BURST, urand(15000, 20000));
                    break;
                case EVENT_WAILING_WINDS:
                    DoCast(m_creature, SPELL_WAILING_WINDS);
                    events.ScheduleEvent(EVENT_ABSORB_STORMS, urand(0, 10 * IN_MILLISECONDS));
                    break;
                case EVENT_TEMPEST_STORM_SUMMON:
                    //Talk(SAY_EVENT_1);
                    DoCast(m_creature, SPELL_TEMPEST_STORM_SUMMON);
                    break;
                case EVENT_ABSORB_STORMS:
                    DoCast(m_creature, SPELL_ABSORB_STORMS, true);
                    events.ScheduleEvent(EVENT_ABSORB_STORMS, 25 * IN_MILLISECONDS);
                    events.ScheduleEvent(EVENT_DESPAWN_STORMS, 0);
                    break;
                case EVENT_DESPAWN_STORMS:
                    DespawnAllSummon();
                    break;
                default:
                    break;
            }
        }

        DoCastSpellIfCan(m_creature->getVictim(), SPELL_STORM_BOLT_CASTING);

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_minion_of_siamatAI : public ScriptedAI
{
    npc_minion_of_siamatAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    EventMap events;
    bool DATA_TEMPEST_STORM;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset () override
    {
        DATA_TEMPEST_STORM = false;
        events.Reset();
        events.ScheduleEvent(EVENT_DEPLETION, 0);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!UpdateVictim())
            return;

        if (m_creature->GetHealthPercent() < 10.0f && !DATA_TEMPEST_STORM)
        {
            DATA_TEMPEST_STORM = true;
            events.ScheduleEvent(EVENT_TEMPEST_STORM, 0);
        }

        events.Update(uiDiff);

        while(uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_TEMPEST_STORM:
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_TEMPEST_STORM : H_SPELL_TEMPEST_STORM);
                    break;
                case EVENT_DEPLETION:
                    DoCast(m_creature, SPELL_DEPLETION);
                    break;
                default:
                    break;
            }
            }

        DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : H_SPELL_CHAIN_LIGHTNING);
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_servant_of_siamatAI : public ScriptedAI
{
    npc_servant_of_siamatAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    bool DATA_THUNDER_CRASH;

    void Reset () override
    {
        DATA_THUNDER_CRASH = false;
        events.Reset();
        events.ScheduleEvent(EVENT_LIGHTNING_NOVA, urand(10000, 15000));
        events.ScheduleEvent(EVENT_LIGHTNING_CHARGE, urand(15000, 20000));
    }

    void DamageTaken(Unit* /*damageDealer*/, uint32& damage) override
    {
        if (damage >= m_creature->GetMaxHealth())
            if (!DATA_THUNDER_CRASH)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveAllAuras();
            }
            else
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!UpdateVictim())
            return;

        if (m_creature->GetHealthPercent() < 15.0f)
            DATA_THUNDER_CRASH = true;

        events.Update(uiDiff);

        while(uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_LIGHTNING_CHARGE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_LIGHTNING_CHARGE);
                    events.ScheduleEvent(EVENT_LIGHTNING_CHARGE, urand(15000, 20000));
                    break;
                case EVENT_LIGHTNING_NOVA:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_LIGHTNING_NOVA : H_SPELL_LIGHTNING_NOVA);
                    events.ScheduleEvent(EVENT_LIGHTNING_NOVA, urand(10000, 15000));
                    break;
                case EVENT_THUNDER_CRASH:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_THUNDER_CRASH : H_SPELL_THUNDER_CRASH);
                    break;
                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_cloud_burstAI : public Scripted_NoMovementAI
{
    npc_cloud_burstAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    EventMap events;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset () override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_CLOUD_BURST_VISUAL, 0);
        events.ScheduleEvent(EVENT_CLOUD_BURST, 3 * IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(uiDiff);

        while(uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_CLOUD_BURST_VISUAL:
                    DoCast(m_creature, SPELL_CLOUD_BURST_VISUAL);
                    break;
                case EVENT_CLOUD_BURST:
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_CLOUD_BURST : H_SPELL_CLOUD_BURST);
                    events.ScheduleEvent(EVENT_DESPAWN_CLOUD_BURST, 2 * IN_MILLISECONDS);
                    break;
                case EVENT_DESPAWN_CLOUD_BURST:
                    m_creature->ForcedDespawn();
                    break;
                default:
                    break;
            }
        }
    }
};

//struct MANGOS_DLL_DECL mob_shokwave_spike_visualAI : public Scripted_NoMovementAI
//{
//    mob_shokwave_spike_visualAI(Creature *c) : Scripted_NoMovementAI(c)
//    {
//        Reset();
//    }
//
//    void Reset()
//    {
//        m_creature->CastSpell(m_creature, SPELL_VISUAL_WIND, true);
//    }
//};

CreatureAI* GetAI_boss_siamat(Creature* pCreature)
{
    return new boss_siamatAI(pCreature);
}

CreatureAI* GetAI_npc_minion_of_siamat(Creature* pCreature)
{
    return new npc_minion_of_siamatAI(pCreature);
}

CreatureAI* GetAI_npc_servant_of_siamat(Creature* pCreature)
{
    return new npc_servant_of_siamatAI(pCreature);
}

CreatureAI* GetAI_npc_cloud_burst(Creature* pCreature)
{
    return new npc_cloud_burstAI(pCreature);
}

//CreatureAI* GetAI_mob_shokwave_spike_visual(Creature* pCreature)
//{
//    return new mob_shokwave_spike_visualAI(pCreature);
//}

void AddSC_boss_siamat()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_siamat";
    pNewScript->GetAI = &GetAI_boss_siamat;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_minion_of_siamat";
    pNewScript->GetAI = &GetAI_npc_minion_of_siamat;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_servant_of_siamat";
    pNewScript->GetAI = &GetAI_npc_servant_of_siamat;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_cloud_burst";
    pNewScript->GetAI = &GetAI_npc_cloud_burst;
    pNewScript->RegisterSelf();

    //pNewScript = new Script;
    //pNewScript->Name = "mob_shokwave_spike_visual";
    //pNewScript->GetAI = &GetAI_mob_shokwave_spike_visual;
    //pNewScript->RegisterSelf();
}
