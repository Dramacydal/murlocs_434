/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_argent_challenge
SD%Complete: 50%
SDComment: missing yells. radiance is "wrong". modified by /dev/rsa
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"
#include "MapPersistentStateMgr.h"

enum
{
    SPELL_BERSERK                           = 47008,
    // Yells

    // Eadric
    SPELL_VENGEANCE                         = 66889,
    SPELL_RADIANCE                          = 66935,
    SPELL_HAMMER_OF_JUSTICE                 = 66940,
    SPELL_HAMMER                            = 67680,
    // Paletress
    SPELL_SMITE                             = 66536,
    SPELL_SMITE_H                           = 67674,
    SPELL_HOLY_FIRE                         = 66538,
    SPELL_HOLY_FIRE_H                       = 67676,
    SPELL_RENEW                             = 66537,
    SPELL_RENEW_H                           = 67675,
    SPELL_HOLY_NOVA                         = 66546,
    SPELL_SHIELD                            = 66515,
    SPELL_CONFESS                           = 66547,
    // Memory
    SPELL_FEAR                              = 66552,
    SPELL_FEAR_H                            = 67677,
    SPELL_SHADOWS                           = 66619,
    SPELL_SHADOWS_H                         = 67678,
    SPELL_OLD_WOUNDS                        = 66620,
    SPELL_OLD_WOUNDS_H                      = 67679,
};

// Eadric The Pure
struct MANGOS_DLL_DECL boss_eadricAI : public ScriptedAI
{
    boss_eadricAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Vengeance_Timer;
    uint32 Radiance_Timer;
    uint32 Hammer_Timer;
    uint32 Hammer_Dmg_Timer;
    uint32 m_uiBerserk_Timer;
    ObjectGuid HammerTarget;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        Vengeance_Timer = 1000;
        Radiance_Timer = m_bIsRegularMode ? 15000 : 8000;
        Hammer_Timer = m_bIsRegularMode ? 40000 : 10000;
        Hammer_Dmg_Timer = m_bIsRegularMode ? 45000 : 20000;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        HammerTarget.Clear();
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) != DONE)
            m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, DONE);

        #ifndef WIN32
        if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
            state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_PALETRESS, m_creature);
        #endif

        m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_EADRIC);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Vengeance_Timer < diff)
        {
            DoCast(m_creature, SPELL_VENGEANCE);
            Vengeance_Timer = m_bIsRegularMode ? 12000 : 8000;
        }
        else
            Vengeance_Timer -= diff;  

        if (Radiance_Timer < diff)
        {
            DoCast(m_creature, SPELL_RADIANCE);
            Radiance_Timer = m_bIsRegularMode ? 20000 : 12000;
        }
        else
            Radiance_Timer -= diff;

        if (Hammer_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
            {
                DoCast(target, SPELL_HAMMER_OF_JUSTICE);
                HammerTarget = target->GetObjectGuid();
            }
            Hammer_Timer = m_bIsRegularMode ? 40000 : 15000;
        }
        else
            Hammer_Timer -= diff;

        if (Hammer_Dmg_Timer < diff)
        {
            if (Unit* pHammerTarget = m_creature->GetMap()->GetUnit(HammerTarget))
                DoCast(pHammerTarget, SPELL_HAMMER);

            Hammer_Dmg_Timer = m_bIsRegularMode ? 50000 : 15000;
        }
        else
            Hammer_Dmg_Timer -= diff;

        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        }
        else
            m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eadric(Creature* pCreature)
{
    return new boss_eadricAI(pCreature);
}

// Argent Confessor Paletress
struct MANGOS_DLL_DECL boss_paletressAI : public ScriptedAI
{
    boss_paletressAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Smite_Timer;
    uint32 Holy_Fire_Timer;
    uint32 Renew_Timer;
    uint32 Shield_Delay;
    uint32 m_uiBerserk_Timer;
    bool summoned;
    bool shielded;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->RemoveAurasDueToSpell(SPELL_SHIELD);
        Smite_Timer = 5000;
        Holy_Fire_Timer = m_bIsRegularMode ? 10000 : 8000;
        Renew_Timer = m_bIsRegularMode ? 7000 : 5000;
        Shield_Delay = 0;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        summoned = false;
        shielded = false;
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void JustSummoned(Creature* _summoned)
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
            _summoned->AddThreat(target);

        summoned = true;
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) != DONE)
            m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, DONE);

        #ifndef WIN32
        if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
            state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_PALETRESS, m_creature);
        #endif

        m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_PALETRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Smite_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);

            Smite_Timer = 2000;
        }
        else
            Smite_Timer -= diff;  

        if (Holy_Fire_Timer < diff)
        {
            m_creature->CastStop(m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);

            Holy_Fire_Timer = m_bIsRegularMode ? 10000 : 7000;
        }
        else
            Holy_Fire_Timer -= diff;

        if (Renew_Timer < diff)
        {
            m_creature->CastStop(m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
            m_creature->CastStop(m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);
            switch(urand(0, 1))
            {
                case 0:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_MEMORY))
                    {
                        if (pTemp->isAlive())
                            DoCast(pTemp, m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H);
                        else
                            DoCast(m_creature, m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H);
                    }
                break;
                case 1:
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H);
                break;
            }
            Renew_Timer = 25000;
        }
        else
            Renew_Timer -= diff;

        if (((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 35 ) && !summoned)
        {
            m_creature->CastStop(m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
            m_creature->CastStop(m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);
            DoCast(m_creature, SPELL_HOLY_NOVA);
            switch(urand(0, 24))
            {
                case 0:
                    m_creature->SummonCreature(MEMORY_ALGALON, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 1:
                    m_creature->SummonCreature(MEMORY_CHROMAGGUS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 2:
                    m_creature->SummonCreature(MEMORY_CYANIGOSA, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 3:
                    m_creature->SummonCreature(MEMORY_DELRISSA, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 4:
                    m_creature->SummonCreature(MEMORY_ECK, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 5:
                    m_creature->SummonCreature(MEMORY_ENTROPIUS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 6:
                    m_creature->SummonCreature(MEMORY_GRUUL, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 7:
                    m_creature->SummonCreature(MEMORY_HAKKAR, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 8:
                    m_creature->SummonCreature(MEMORY_HEIGAN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 9:
                    m_creature->SummonCreature(MEMORY_HEROD, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 10:
                    m_creature->SummonCreature(MEMORY_HOGGER, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 11:
                    m_creature->SummonCreature(MEMORY_IGNIS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 12:
                    m_creature->SummonCreature(MEMORY_ILLIDAN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 13:
                    m_creature->SummonCreature(MEMORY_INGVAR, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 14:
                    m_creature->SummonCreature(MEMORY_KALITHRESH, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 15:
                    m_creature->SummonCreature(MEMORY_LUCIFRON, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 16:
                    m_creature->SummonCreature(MEMORY_MALCHEZAAR, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 17:
                    m_creature->SummonCreature(MEMORY_MUTANUS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 18:
                    m_creature->SummonCreature(MEMORY_ONYXIA, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 19:
                    m_creature->SummonCreature(MEMORY_THUNDERAAN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 20:
                    m_creature->SummonCreature(MEMORY_VANCLEEF, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 21:
                    m_creature->SummonCreature(MEMORY_VASHJ, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 22:
                    m_creature->SummonCreature(MEMORY_VEKNILASH, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 23:
                    m_creature->SummonCreature(MEMORY_VEZAX, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
                case 24:
                    m_creature->SummonCreature(MEMORY_ARCHIMONDE, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
                    break;
            }
            Shield_Delay = 1000;
        }
        if (Shield_Delay < diff && !shielded && summoned)
        {
            m_creature->CastStop(m_bIsRegularMode ? SPELL_SMITE : SPELL_SMITE_H);
            m_creature->CastStop(m_bIsRegularMode ? SPELL_HOLY_FIRE : SPELL_HOLY_FIRE_H);
            DoCast(m_creature, SPELL_SHIELD);
            shielded = true;
        }
        else
            Shield_Delay -= diff;

        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        }
        else
            m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SHIELD);
    }
};

CreatureAI* GetAI_boss_paletress(Creature* pCreature)
{
    return new boss_paletressAI(pCreature);
}

// Summoned Memory
struct MANGOS_DLL_DECL mob_toc5_memoryAI : public ScriptedAI
{
    mob_toc5_memoryAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Old_Wounds_Timer;
    uint32 Shadows_Timer;
    uint32 Fear_Timer;

    void Reset()
    {
        Old_Wounds_Timer = 5000;
        Shadows_Timer = 8000;
        Fear_Timer = 13000;
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Old_Wounds_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_OLD_WOUNDS : SPELL_OLD_WOUNDS_H);

            Old_Wounds_Timer = 10000;
        }
        else
            Old_Wounds_Timer -= diff;  

        if (Fear_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_FEAR : SPELL_FEAR_H);
            Fear_Timer = 40000;
        }
        else
            Fear_Timer -= diff; 

        if (Shadows_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1))
                DoCast(target, m_bIsRegularMode ? SPELL_SHADOWS : SPELL_SHADOWS_H);

            Shadows_Timer = 10000;
        }
        else
            Shadows_Timer -= diff; 

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_toc5_memory(Creature* pCreature)
{
    return new mob_toc5_memoryAI(pCreature);
}

void AddSC_boss_argent_challenge()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_eadric";
    NewScript->GetAI = &GetAI_boss_eadric;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_paletress";
    NewScript->GetAI = &GetAI_boss_paletress;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_toc5_memory";
    NewScript->GetAI = &GetAI_mob_toc5_memory;
    NewScript->RegisterSelf();
}
