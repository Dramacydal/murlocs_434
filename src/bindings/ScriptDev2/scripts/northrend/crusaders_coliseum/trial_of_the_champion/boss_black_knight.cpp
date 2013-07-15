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
SDName: boss_black_knight
SD%Complete: 70%
SDComment: missing yells. not sure about timers. modified by /dev/rsa
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"
#include "MapPersistentStateMgr.h"

enum
{
    SPELL_BERSERK                           = 47008,
    // Yells

    // Undead
    SPELL_PLAGUE_STRIKE                     = 67724,
    SPELL_PLAGUE_STRIKE_H                   = 67884,
    SPELL_ICY_TOUCH                         = 67718,
    SPELL_ICY_TOUCH_H                       = 67881,
    SPELL_OBLITERATE                        = 67725,
    SPELL_OBLITERATE_H                      = 67883,
    SPELL_CHOKE                             = 68306,
    // Skeleton
    SPELL_ARMY                              = 42650, //replacing original one, since that one spawns millions of ghouls!!
    // Ghost
    SPELL_DEATH                             = 67808,
    SPELL_DEATH_H                           = 67875,
    SPELL_MARK                              = 67823,

    // Risen ghoul
    SPELL_CLAW                              = 67879,
    SPELL_EXPLODE                           = 67729,
    SPELL_EXPLODE_H                         = 67886,
    SPELL_LEAP                              = 67749,
    SPELL_LEAP_H                            = 67880,

    // Sword ID
    EQUIP_SWORD                             = 40343
};

// Risen Ghoul
struct MANGOS_DLL_DECL mob_toc5_risen_ghoulAI : public ScriptedAI
{
    mob_toc5_risen_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Attack;

    void Reset()
    {
        Attack = 2500;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Attack < diff)
        {
            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_BLACK_KNIGHT))
            {
                if (pTemp->isAlive())
                    if ((pTemp->GetHealth()*100 / pTemp->GetMaxHealth()) < 25)
                        DoCast(m_creature, m_bIsRegularMode ? SPELL_EXPLODE : SPELL_EXPLODE_H);
            }

            if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 4))
            {
                DoCast(m_creature->getVictim(), SPELL_CLAW);
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1))
                    m_creature->AI()->AttackStart(target);

                Attack = 2500;
            }
            else if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 30))
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_LEAP : SPELL_LEAP_H);
                Attack = 2500;
            }
        }
        else
            Attack -= diff;

        if ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 25)
            DoCast(m_creature, m_bIsRegularMode ? SPELL_EXPLODE : SPELL_EXPLODE_H);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_toc5_risen_ghoul(Creature* pCreature)
{
    return new mob_toc5_risen_ghoulAI(pCreature);
}

// The Black Knight
struct MANGOS_DLL_DECL boss_black_knightAI : public ScriptedAI
{
    boss_black_knightAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Plague_Strike_Timer;
    uint32 Icy_Touch_Timer;
    uint32 Obliterate_Timer;
    uint32 Choke_Timer;
    uint32 Death_Timer;
    uint32 Mark_Timer;
    uint32 Phase_Delay;
    uint32 Summon_Ghoul;
    uint32 m_uiBerserk_Timer;
    bool phase1;
    bool phase2;
    bool phase3;
    bool ghoul;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetDisplayId(29837);
        SetEquipmentSlots(false, EQUIP_SWORD, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
        Plague_Strike_Timer = m_bIsRegularMode ? 5000 : 4000;
        Icy_Touch_Timer = m_bIsRegularMode ? 10000 : 7000;
        Obliterate_Timer = m_bIsRegularMode ? 16000 : 10000;
        Choke_Timer = 15000;
        Summon_Ghoul = 4000;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        phase1 = true;
        phase2 = false;
        phase3 = false;
        ghoul = false;
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_BLACK_KNIGHT) != DONE)
            m_pInstance->SetData(TYPE_BLACK_KNIGHT, IN_PROGRESS);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if ((uiDamage > m_creature->GetHealth() || m_creature->GetHealth()/m_creature->GetHealth() <= 0.1 )  && !phase3)
        {
            uiDamage = 0;

            if (phase2)
                StartPhase3();

            if (phase1)
                StartPhase2();
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        if (phase3 && !phase1 && !phase2)
        {
            m_pInstance->SetData(TYPE_BLACK_KNIGHT, DONE);

            #ifndef WIN32
            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_BLACK_KNIGHT, m_creature);
            #endif

            m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_BLACK_KNIGHT);
        }

/*      if (phase2 && !phase1 && !phase3)
            if (!m_creature->isAlive())
            {
                m_creature->Respawn();
                StartPhase3();
            }
        if (phase1 && !phase2 && !phase3)
            if (!m_creature->isAlive())
            {
                m_creature->Respawn();
                StartPhase2();
            }*/
    }

    void StartPhase2()
    {
        m_creature->SetHealth(m_creature->GetMaxHealth());
        m_creature->SetDisplayId(27550);
        phase1 = false;
        phase2 = true;
        phase3 = false;
        DoCast(m_creature, SPELL_ARMY);
        Plague_Strike_Timer = m_bIsRegularMode ? 14000 : 8000;
        Icy_Touch_Timer = m_bIsRegularMode ? 12000 : 7000;
        Obliterate_Timer = m_bIsRegularMode ? 18000 : 10000;
    }

    void StartPhase3()
    {
        m_creature->SetHealth(m_creature->GetMaxHealth());
        m_creature->SetDisplayId(14560);
        SetEquipmentSlots(false, EQUIP_UNEQUIP, EQUIP_NO_CHANGE, EQUIP_NO_CHANGE);
        phase1 = false;
        phase2 = false;
        phase3 = true;
        Death_Timer = m_bIsRegularMode ? 5000 : 3000;
        Mark_Timer = m_bIsRegularMode ? 9000 : 7000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Plague_Strike_Timer < diff && !phase3)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_PLAGUE_STRIKE : SPELL_PLAGUE_STRIKE_H);
            Plague_Strike_Timer = m_bIsRegularMode ? 10500 : 7000;
        }
        else
            Plague_Strike_Timer -= diff;  

        if (Icy_Touch_Timer < diff && !phase3)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ICY_TOUCH : SPELL_ICY_TOUCH_H);
            Icy_Touch_Timer = m_bIsRegularMode ? 10000 : 8000;
        }
        else
            Icy_Touch_Timer -= diff;

        if (Obliterate_Timer < diff && !phase3)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_OBLITERATE : SPELL_OBLITERATE_H);
            Obliterate_Timer = m_bIsRegularMode ? 11000 : 8000;
        }
        else
            Obliterate_Timer -= diff;

        if (Choke_Timer < diff && phase1)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1))
                DoCast(m_creature->getVictim(), SPELL_CHOKE);
            Choke_Timer = m_bIsRegularMode ? 15000 : 10000;
        }
        else
            Choke_Timer -= diff;

        if (Summon_Ghoul < diff && phase1 && !ghoul)
        {
            if (m_pInstance->GetData(DATA_TOC5_ANNOUNCER) == m_pInstance->GetData(DATA_JAEREN))
                m_creature->SummonCreature(NPC_RISEN_JAEREN, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
            else
                m_creature->SummonCreature(NPC_RISEN_ARELAS, 0.0f, 0.0f, 0.0f, 0, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);

            ghoul = true;
        }
        else
            Summon_Ghoul -= diff;

        if (Mark_Timer < diff && phase3)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,1))
                DoCast(target, SPELL_MARK);
            Mark_Timer = m_bIsRegularMode ? 15000 : 10000;
        }
        else
            Mark_Timer -= diff;

        if (Death_Timer < diff && phase3)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_DEATH : SPELL_DEATH_H);
            Death_Timer = 3500;
        }
        else
            Death_Timer -= diff;

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

CreatureAI* GetAI_boss_black_knight(Creature* pCreature)
{
    return new boss_black_knightAI(pCreature);
}

void AddSC_boss_black_knight()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "mob_toc5_risen_ghoul";
    NewScript->GetAI = &GetAI_mob_toc5_risen_ghoul;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_black_knight";
    NewScript->GetAI = &GetAI_boss_black_knight;
    NewScript->RegisterSelf();
}
