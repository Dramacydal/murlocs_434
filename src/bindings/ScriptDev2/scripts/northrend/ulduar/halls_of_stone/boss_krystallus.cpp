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
SDName: Boss Krystallus
SDAuthor: ckegg
SD%Complete: 0%
SDComment: 
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"

enum
{
    SAY_AGGRO                          = -1599000,
    SAY_KILL                           = -1599001,
    SAY_DEATH                          = -1599002,
    SAY_SHATTER                        = -1599003,

    SPELL_BOULDER_TOSS                  = 50843,
    SPELL_BOULDER_TOSS_H                = 59742,
    SPELL_GROUND_SPIKE                  = 59750,
    SPELL_GROUND_SLAM                   = 50827,
    SPELL_SHATTER                       = 50810,
    SPELL_SHATTER_H                     = 61546,
    SPELL_STOMP                         = 50868,
    SPELL_STOMP_H                       = 59744,

};

/*######
## boss_krystallus
######*/

struct MANGOS_DLL_DECL boss_krystallusAI : public ScriptedAI
{
    boss_krystallusAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bIsRegularMode;
    bool m_bIsSlam;

    uint32 m_uiToss_Timer;
    uint32 m_uiSpike_Timer;
    uint32 m_uiSlam_Timer;
    uint32 m_uiShatter_Timer;
    uint32 m_uiStomp_Timer;

    void Reset()
    {
        m_bIsSlam = false;
        m_uiToss_Timer = 3000 + rand()%6000;
        m_uiSpike_Timer = 9000 + rand()%5000;
        m_uiSlam_Timer = 15000 + rand()%3000;
        m_uiStomp_Timer = 20000 + rand()%9000;
        m_uiShatter_Timer = 0;

        if(m_pInstance)
            m_pInstance->SetData(TYPE_KRYSTALLUS, NOT_STARTED);
    }

    void EnterCombat(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO,m_creature);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_KRYSTALLUS, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_KRYSTALLUS, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiToss_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_BOULDER_TOSS_H : SPELL_BOULDER_TOSS);
            m_uiToss_Timer = 9000 + rand()%6000;
        }
        else
            m_uiToss_Timer -= uiDiff;

        if (m_uiSpike_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_GROUND_SPIKE);
            m_uiSpike_Timer = 12000 + rand()%5000;
        }
        else
            m_uiSpike_Timer -= uiDiff;

        if (m_uiStomp_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_STOMP_H : SPELL_STOMP);
            m_uiStomp_Timer = 20000 + rand()%9000;
        }
        else
            m_uiStomp_Timer -= uiDiff;

        if (m_uiSlam_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_GROUND_SLAM);
            m_bIsSlam = true;
            m_uiShatter_Timer = 10000;
            m_uiSlam_Timer = 15000 + rand()%3000;
        }
        else
            m_uiSlam_Timer -= uiDiff;

        if (m_bIsSlam)
        {
            if (m_uiShatter_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_SHATTER_H : SPELL_SHATTER);
                m_bIsSlam = false;
                m_uiShatter_Timer = 0;
            }
            else
                m_uiShatter_Timer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_krystallus(Creature* pCreature)
{
    return new boss_krystallusAI (pCreature);
}

void AddSC_boss_krystallus()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_krystallus";
    newscript->GetAI = &GetAI_boss_krystallus;
    newscript->RegisterSelf();
}
