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
SDName: Boss_Celebras_the_Cursed
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"

enum
{
    SPELL_WRATH                = 21807,
    SPELL_ENTANGLINGROOTS      = 12747,
    SPELL_CORRUPT_FORCES       = 21968,
    NPC_CELEBRAS_THE_REDEEMED  = 13716,
};

struct MANGOS_DLL_DECL celebras_the_cursedAI : public ScriptedAI
{
    celebras_the_cursedAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiWrath_Timer;
    uint32 m_uiEntanglingRoots_Timer;
    uint32 m_uiCorruptForces_Timer;

    void Reset()
    {
        m_uiWrath_Timer = 8000;
        m_uiEntanglingRoots_Timer = 2000;
        m_uiCorruptForces_Timer = 30000;
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->SummonCreature(NPC_CELEBRAS_THE_REDEEMED, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 600000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiWrath_Timer < uiDiff)
        {
            Unit* pTarget = NULL;
            pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
            if (pTarget)
                DoCastSpellIfCan(pTarget,SPELL_WRATH);
            m_uiWrath_Timer = 8000;
        }else m_uiWrath_Timer -= uiDiff;

        if (m_uiEntanglingRoots_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_ENTANGLINGROOTS);
            m_uiEntanglingRoots_Timer = 20000;
        }else m_uiEntanglingRoots_Timer -= uiDiff;

        if (m_uiCorruptForces_Timer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCastSpellIfCan(m_creature,SPELL_CORRUPT_FORCES);
            m_uiCorruptForces_Timer = 20000;
        }else m_uiCorruptForces_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_celebras_the_cursed(Creature* pCreature)
{
    return new celebras_the_cursedAI(pCreature);
}

void AddSC_boss_celebras_the_cursed()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "celebras_the_cursed";
    pNewScript->GetAI = &GetAI_celebras_the_cursed;
    pNewScript->RegisterSelf();
}
