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
SDName: Boss_Landslide
SD%Complete: 100
SDComment:
SDCategory: Maraudon
EndScriptData */

#include "precompiled.h"

enum
{
    SPELL_KNOCKAWAY        = 18670,
    SPELL_TRAMPLE          = 5568,
    SPELL_LANDSLIDE        = 21808,
};

struct MANGOS_DLL_DECL boss_landslideAI : public ScriptedAI
{
    boss_landslideAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiKnockAway_Timer;
    uint32 m_uiTrample_Timer;
    uint32 m_uiLandslide_Timer;

    void Reset()
    {
        m_uiKnockAway_Timer = 8000;
        m_uiTrample_Timer = 2000;
        m_uiLandslide_Timer = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiKnockAway_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_KNOCKAWAY);
            m_uiKnockAway_Timer = 15000;
        }else m_uiKnockAway_Timer -= uiDiff;

        if (m_uiTrample_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature,SPELL_TRAMPLE);
            m_uiTrample_Timer = 8000;
        }else m_uiTrample_Timer -= uiDiff;

        if (m_creature->GetHealthPercent() < 50.0f)
        {
            if (m_uiLandslide_Timer < uiDiff)
            {
                m_creature->InterruptNonMeleeSpells(false);
                DoCastSpellIfCan(m_creature,SPELL_LANDSLIDE);
                m_uiLandslide_Timer = 60000;
            } else m_uiLandslide_Timer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_landslide(Creature* pCreature)
{
    return new boss_landslideAI(pCreature);
}

void AddSC_boss_landslide()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_landslide";
    pNewScript->GetAI = &GetAI_boss_landslide;
    pNewScript->RegisterSelf();
}
