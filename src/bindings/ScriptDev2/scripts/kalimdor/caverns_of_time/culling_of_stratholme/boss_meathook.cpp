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
    SPELL_CHAIN_N                 = 52696,
    SPELL_CHAIN_H                 = 58823,
    SPELL_EXPLODED_N              = 52666,
    SPELL_EXPLODED_H              = 58824,
    SPELL_FRENZY                  = 58841,

    SAY_MEATHOOK_AGGRO       = -1594111,
    SAY_MEATHOOK_DEATH       = -1594112,
    SAY_MEATHOOK_SLAY01      = -1594113,
    SAY_MEATHOOK_SLAY02      = -1594114,
    SAY_MEATHOOK_SLAY03      = -1594115
};

struct MANGOS_DLL_DECL boss_meathookAI : public ScriptedAI
{
    boss_meathookAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
        m_creature->SetActiveObjectState(true);
        m_creature->SetInCombatWithZone();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    uint32 m_uiChain_Timer;
    uint32 m_uiExploded_Timer;
    uint32 m_uiFrenzy_Timer;

    void Reset()
    {
        m_uiChain_Timer = 6300;
        m_uiExploded_Timer = 5000;
        m_uiFrenzy_Timer = 22300;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_MEATHOOK_AGGRO, m_creature);
    }

    void JustDied(Unit *pKiller)
    {
        DoScriptText(SAY_MEATHOOK_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_PHASE, 3);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(rand()%3)
        {
            case 0: DoScriptText(SAY_MEATHOOK_SLAY01, m_creature); break;
            case 1: DoScriptText(SAY_MEATHOOK_SLAY02, m_creature); break;
            case 2: DoScriptText(SAY_MEATHOOK_SLAY03, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiChain_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(pTarget, m_bIsHeroic ? SPELL_CHAIN_H : SPELL_CHAIN_N);

            m_uiChain_Timer = 6300;
        }else m_uiChain_Timer -= uiDiff;

        if (m_uiExploded_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(pTarget, m_bIsHeroic ? SPELL_EXPLODED_H : SPELL_EXPLODED_N);

            m_uiExploded_Timer = 5000;
        }else m_uiExploded_Timer -= uiDiff;

        if (m_uiFrenzy_Timer < uiDiff)
        {
                m_creature->InterruptNonMeleeSpells(false);
                DoCast(m_creature,SPELL_FRENZY);

            m_uiFrenzy_Timer = 23300;
        }else m_uiFrenzy_Timer -= uiDiff;

        DoMeleeAttackIfReady();
   }
};

CreatureAI* GetAI_boss_meathook(Creature* pCreature)
{
    return new boss_meathookAI(pCreature);
}

void AddSC_boss_meathook()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_meathook";
    pNewScript->GetAI = &GetAI_boss_meathook;
    pNewScript->RegisterSelf();
}
