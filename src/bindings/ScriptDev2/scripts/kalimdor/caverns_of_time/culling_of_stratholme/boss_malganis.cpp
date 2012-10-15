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
    SAY_MALGANIS_AGGRO       = -1594170,
    SAY_MALGANIS_SLAY01      = -1594172,
    SAY_MALGANIS_SLAY02      = -1594173,
    SAY_MALGANIS_SLAY03      = -1594174,
    SAY_MALGANIS_SLAY04      = -1594175,
    SAY_MALGANIS_SLAY05      = -1594176,
    SAY_MALGANIS_SLAY06      = -1594177,
    SAY_MALGANIS_SLAY07      = -1594166,
    SAY_MALGANIS_SLEEP01     = -1594185,
    SAY_MALGANIS_SLEEP02     = -1594186,
    SAY_MALGANIS_Sleep       = -1594178,
    SAY_MALGANIS_15HP        = -1594179,

    SPELL_SWAMP_N                          = 52720,
    SPELL_SWAMP_H                          = 58852,
    SPELL_MIND_BLAST_N                     = 52722,
    SPELL_MIND_BLAST_H                     = 58850,
    SPELL_SLEEP_N                          = 52721,
    SPELL_SLEEP_H                          = 58849,
    SPELL_VAMPIRE                          = 52723,
    SPELL_ACHIEV_CHECK                     = 58630
};

struct MANGOS_DLL_DECL boss_malganisAI : public ScriptedAI
{
    boss_malganisAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    Unit* pTarget;
    bool Sleep;
    bool Vampire;
    Creature* Malganis;
    Creature* Arthas;

    uint32 m_uiSwamp_Timer;
    uint32 m_uiMindBlast_Timer;
    uint32 m_uiSleep_Timer;
    uint32 m_uiVampire_Timer;

    void Reset()
    {
        Sleep = false;
        Vampire = false;
        m_uiSwamp_Timer = 6300;
        m_uiMindBlast_Timer = 11300;
        m_uiSleep_Timer = 17300;
        m_uiVampire_Timer = 30000;
    }

    void AttackStart(Unit* pWho)
    {
        if (m_pInstance->GetData(TYPE_PHASE) > 9) return;

        if (m_pInstance->GetData(TYPE_MALGANIS) != IN_PROGRESS) return;

        if (!pWho || pWho == m_creature)
            return;

        ScriptedAI::AttackStart(pWho);
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage)
    {
        if (m_creature->GetHealth() <= uiDamage || m_creature->GetHealthPercent() < 5.0f)
            uiDamage = 0;
    }

    void EnterEvadeMode()
    {
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_MALGANIS_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(rand()%7)
        {
            case 0: DoScriptText(SAY_MALGANIS_SLAY01, m_creature); break;
            case 1: DoScriptText(SAY_MALGANIS_SLAY02, m_creature); break;
            case 2: DoScriptText(SAY_MALGANIS_SLAY03, m_creature); break;
            case 3: DoScriptText(SAY_MALGANIS_SLAY04, m_creature); break;
            case 4: DoScriptText(SAY_MALGANIS_SLAY05, m_creature); break;
            case 5: DoScriptText(SAY_MALGANIS_SLAY06, m_creature); break;
            case 6: DoScriptText(SAY_MALGANIS_SLAY07, m_creature); break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSwamp_Timer < uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, m_bIsHeroic ? SPELL_SWAMP_H : SPELL_SWAMP_N);
            m_uiSwamp_Timer = 7300;
        }else m_uiSwamp_Timer -= uiDiff;

        if (m_uiMindBlast_Timer < uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, m_bIsHeroic ? SPELL_MIND_BLAST_H : SPELL_MIND_BLAST_N);

            m_uiMindBlast_Timer = 11300;
        }else m_uiMindBlast_Timer -= uiDiff;

        if (m_creature->GetHealthPercent() < 40.0f)
        {
            if (Sleep == false)
            {
                Sleep = true;
                DoScriptText(SAY_MALGANIS_Sleep, m_creature);
            }
            if (m_uiSleep_Timer < uiDiff)
            {
                if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                    DoCast(target, m_bIsHeroic ? SPELL_SLEEP_H : SPELL_SLEEP_N);
                switch(rand()%2)
                {
                    case 0: DoScriptText(SAY_MALGANIS_SLEEP01, m_creature); break;
                    case 1: DoScriptText(SAY_MALGANIS_SLEEP02, m_creature); break;
                }
                m_uiSleep_Timer = 17300;
            }else m_uiSleep_Timer -= uiDiff;
        }

        if (m_creature->GetHealthPercent() < 25.0f)
        {
            if (Vampire == false)
            {
                Vampire = true;
                DoScriptText(SAY_MALGANIS_15HP, m_creature);
                DoCast(m_creature, SPELL_VAMPIRE);
            }
            if (m_uiVampire_Timer < uiDiff)
            {
                DoCast(m_creature, SPELL_VAMPIRE);
                m_uiVampire_Timer = 30000;
            }else m_uiVampire_Timer -= uiDiff;
        }

        if (m_creature->GetHealthPercent() < 5.0f)
        {
            m_pInstance->SetData(TYPE_PHASE, 10);
            m_pInstance->SetData(TYPE_MALGANIS, DONE);
            DoCastSpellIfCan(m_creature, SPELL_ACHIEV_CHECK, CAST_TRIGGERED);
            EnterEvadeMode();
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_malganis(Creature* pCreature)
{
    return new boss_malganisAI(pCreature);
}

void AddSC_boss_malganis()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_malganis";
    pNewScript->GetAI = &GetAI_boss_malganis;
    pNewScript->RegisterSelf();

}