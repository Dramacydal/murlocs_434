/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Maiden_of_Grief
SD%Complete: 20%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"

enum
{
    SAY_AGGRO                   = -1599005,
    SAY_SLAY_1                  = -1599006,
    SAY_SLAY_2                  = -1599007,
    SAY_SLAY_3                  = -1599008,
    SAY_SLAY_4                  = -1599009,
    SAY_STUN                    = -1599010,
    SAY_DEATH                   = -1599011,

    SPELL_PARTING_SORROW        = 59723,
    SPELL_PILLAR_OF_WOE         = 50761,
    SPELL_PILLAR_OF_WOE_H       = 59727,
    SPELL_SHOCK_OF_SORROW       = 50760,
    SPELL_SHOCK_OF_SORROW_H     = 59726,
    SPELL_STORM_OF_GRIEF        = 50752,
    SPELL_STORM_OF_GRIEF_H      = 59772,
};

/*######
## boss_maiden_of_grief
######*/

struct MANGOS_DLL_DECL boss_maiden_of_griefAI : public ScriptedAI
{
    boss_maiden_of_griefAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPartingSorrow_Timer;
    uint32 m_uiPillarWoe_Timer;
    uint32 m_uiShockSorrow_Timer;
    uint32 m_uiStorm_Timer;

    void Reset()
    {
        m_uiPartingSorrow_Timer = 9000 + rand()%5000;
        m_uiPillarWoe_Timer = 3000 + rand()%4000;
        m_uiStorm_Timer = 10000 + rand()%5000;
        m_uiShockSorrow_Timer = 20000 + rand()%5000;

        if(m_pInstance)
            m_pInstance->SetData(TYPE_GRIEF, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_GRIEF, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(rand()%4)
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
            case 3: DoScriptText(SAY_SLAY_4, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GRIEF, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPartingSorrow_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_PARTING_SORROW);
            m_uiPartingSorrow_Timer = 12000 + rand()%5000;
        }
        else
            m_uiPartingSorrow_Timer -= uiDiff;

        if (m_uiPillarWoe_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_PILLAR_OF_WOE_H : SPELL_PILLAR_OF_WOE);
            m_uiPillarWoe_Timer = 9000 + rand()%4000;
        }
        else
            m_uiPillarWoe_Timer -= uiDiff;

        if (m_uiStorm_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_STORM_OF_GRIEF_H : SPELL_STORM_OF_GRIEF);
            m_uiStorm_Timer = 20000 + rand()%5000;
        }
        else
            m_uiStorm_Timer -= uiDiff;

        if (m_uiShockSorrow_Timer < uiDiff)
        {
            DoScriptText(SAY_STUN, m_creature);
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHOCK_OF_SORROW_H : SPELL_SHOCK_OF_SORROW);
            m_uiShockSorrow_Timer = 20000 + rand()%5000;
        }
        else
            m_uiShockSorrow_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_maiden_of_grief(Creature* pCreature)
{
    return new boss_maiden_of_griefAI(pCreature);
}

void AddSC_boss_maiden_of_grief()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_maiden_of_grief";
    pNewScript->GetAI = &GetAI_boss_maiden_of_grief;
    pNewScript->RegisterSelf();
}
