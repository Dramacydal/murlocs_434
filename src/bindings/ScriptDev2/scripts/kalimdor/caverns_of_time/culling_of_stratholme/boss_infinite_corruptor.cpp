/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * Copyright (C) 2011 MangosR2
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
SDName: instance_culling_of_stratholme
SD%Complete: %
SDComment:
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

enum
{
    SPELL_COURSE      = 60588,
    SPELL_STRIKE      = 60590
};

struct MANGOS_DLL_DECL boss_infinite_corruptorAI : public ScriptedAI
{
    boss_infinite_corruptorAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
         m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
         m_creature->SetActiveObjectState(true);
         Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStrikeTimer;
    uint32 m_uiCourseTimer;

    void Reset()
    {
        m_uiCourseTimer = 7000;
        m_uiStrikeTimer = 5000;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BONUS, SPECIAL);
    }

    void JustDied(Unit *pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BONUS, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        //switch(rand()%3)
       // {
         //   case 0: DoScriptText(SAY_EPOCH_SLAY01, m_creature); break;
      //      case 1: DoScriptText(SAY_EPOCH_SLAY02, m_creature); break;
      //      case 2: DoScriptText(SAY_EPOCH_SLAY03, m_creature); break;
       // }
    }

    void EnterEvadeMode()
    {
        if (!m_pInstance)
            return;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(false);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_BONUS, IN_PROGRESS);

        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void UpdateAI(const uint32 uiDiff)
    {
         if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
             return;

         if (m_uiCourseTimer < uiDiff)
         {
             if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                 DoCast(target, SPELL_COURSE);

             m_uiCourseTimer = 17000;
         }else m_uiCourseTimer -= uiDiff;

         if (m_uiStrikeTimer < uiDiff)
         {
             DoCast(m_creature->getVictim(), SPELL_STRIKE);

             m_uiStrikeTimer = 5000;
         }else m_uiStrikeTimer -= uiDiff;

         DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_infinite_corruptor(Creature* pCreature)
{
    return new boss_infinite_corruptorAI(pCreature);
}

void AddSC_boss_infinite_corruptor()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_infinite_corruptor";
    pNewScript->GetAI = &GetAI_boss_infinite_corruptor;
    pNewScript->RegisterSelf();
}