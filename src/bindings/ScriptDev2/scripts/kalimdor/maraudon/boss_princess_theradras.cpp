/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
SDName: Boss_Princess_Theradras
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"

enum
{
    SPELL_DUSTFIELD            = 21909,
    SPELL_BOULDER              = 21832,
    SPELL_THRASH               = 3391,
    SPELL_REPULSIVEGAZE        = 21869,
    NPC_ZAETAR_SPIRIT          = 12238,
};

struct MANGOS_DLL_DECL boss_ptheradrasAI : public ScriptedAI
{
    boss_ptheradrasAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiDustfield_Timer;
    uint32 m_uiBoulder_Timer;
    uint32 m_uiThrash_Timer;
    uint32 m_uiRepulsiveGaze_Timer;

    void Reset()
    {
        m_uiDustfield_Timer = 8000;
        m_uiBoulder_Timer = 2000;
        m_uiThrash_Timer = 5000;
        m_uiRepulsiveGaze_Timer = 23000;
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->SummonCreature(NPC_ZAETAR_SPIRIT,28.067f, 61.875f, -123.405f, 4.67f, TEMPSUMMON_TIMED_DESPAWN, 600000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDustfield_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature,SPELL_DUSTFIELD);
            m_uiDustfield_Timer = 14000;
        }else m_uiDustfield_Timer -= uiDiff;

        if (m_uiBoulder_Timer < uiDiff)
        {
            Unit* pTarget = NULL;
            pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (pTarget)
                DoCastSpellIfCan(pTarget,SPELL_BOULDER);
            m_uiBoulder_Timer = 10000;
        }else m_uiBoulder_Timer -= uiDiff;

        if (m_uiRepulsiveGaze_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_REPULSIVEGAZE);
            m_uiRepulsiveGaze_Timer = 20000;
        }else m_uiRepulsiveGaze_Timer -= uiDiff;

        if (m_uiThrash_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature,SPELL_THRASH);
            m_uiThrash_Timer = 18000;
        }else m_uiThrash_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_ptheradras(Creature* pCreature)
{
    return new boss_ptheradrasAI(pCreature);
}

void AddSC_boss_ptheradras()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_princess_theradras";
    pNewScript->GetAI = &GetAI_boss_ptheradras;
    pNewScript->RegisterSelf();
}
