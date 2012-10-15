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
SDName: Boss_Sladran
SD%Complete: 75%
SDComment:
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"
#include "gundrak.h"

enum
{
    SAY_AGGRO                 = -1604000,
    SAY_SUMMON_SNAKE          = -1604001,
    SAY_SUMMON_CONSTRICTOR    = -1604002,
    SAY_SLAY_1                = -1604003,
    SAY_SLAY_2                = -1604004,
    SAY_SLAY_3                = -1604005,
    SAY_DEATH                 = -1604006,
    EMOTE_NOVA                = -1604007,

    // Slad'Ran spells
    SPELL_POISON_NOVA         = 55081,
    SPELL_POISON_NOVA_H       = 59842,
    SPELL_POWERFUL_BITE       = 48287,
    SPELL_POWERFUL_BITE_H     = 59840,
    SPELL_VENOM_BOLT          = 54970,
    SPELL_VENOM_BOLT_H        = 59839,

    // Summon spells
    SPELL_SUMMON_VIPER        = 55060,
    SPELL_SUMMON_CONSTRICTOR  = 54969,

    SPELL_GRIP_OF_SLADRAN     = 55093,
    SPELL_GRIP_OF_SLADRAN_H   = 61474,
    SPELL_SNAKE_WRAP          = 55126,
    SPELL_SNAKE_WRAP_H        = 61476,

    NPC_SLADRAN_CONSTRICTOR   = 29713,
    NPC_SLADRAN_VIPER         = 29680,
    NPC_SNAKE_WRAP            = 29742,
    NPC_SLADRAN_SUMMON_TARGET = 29682
};

/*######
## mob_sladran_constrictor
######*/
struct MANGOS_DLL_DECL mob_sladran_constrictorAI : public ScriptedAI
{
    mob_sladran_constrictorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiWrapTimer;

    void Reset()
    {
        m_uiWrapTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiWrapTimer <= uiDiff)
        {
            if (m_pInstance)
                if (Creature* pSladran = m_pInstance->GetSingleCreatureFromStorage(NPC_SLADRAN))
                    m_creature->CastSpell(m_creature->getVictim(), m_bIsRegularMode ? SPELL_GRIP_OF_SLADRAN : SPELL_GRIP_OF_SLADRAN_H, false, 0, 0, pSladran->GetObjectGuid());
            m_uiWrapTimer = 5000;
        }
        else m_uiWrapTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_sladran_constrictor(Creature* pCreature)
{
    return new mob_sladran_constrictorAI(pCreature);
}

/*######
## mob_sladran_snake_wrap
######*/
struct MANGOS_DLL_DECL mob_sladran_snake_wrapAI : public ScriptedAI
{
    mob_sladran_snake_wrapAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_gundrak* m_pInstance;

    void Reset()
    {
        m_creature->setFaction(14);
        SetCombatMovement(false);
    }
    void MoveInLineOfSight(Unit* pWho) {}
    void AttackStart(Unit* pWho) {}

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        if (Unit *pTarget = m_pInstance->instance->GetUnit(m_creature->GetCreatorGuid()))
        {
            pTarget->RemoveAurasDueToSpell(SPELL_SNAKE_WRAP);
            pTarget->RemoveAurasDueToSpell(SPELL_SNAKE_WRAP_H);
        }
    }

    void UpdateAI(const uint32 diff) {}
};

CreatureAI* GetAI_mob_sladran_snake_wrap(Creature* pCreature)
{
    return new mob_sladran_snake_wrapAI(pCreature);
}

/*######
## mob_sladran_summon_target
######*/
struct MANGOS_DLL_DECL mob_sladran_summon_targetAI : public ScriptedAI
{
    mob_sladran_summon_targetAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_gundrak* m_pInstance;

    void Reset() {}
    void MoveInLineOfSight(Unit* pWho) {}
    void AttackStart(Unit* pWho) {}

    void JustSummoned(Creature* pSummoned)
    {
        if (!m_pInstance)
            return;

        if (Creature* pSladran = m_pInstance->GetSingleCreatureFromStorage(NPC_SLADRAN))
        {
            pSummoned->setFaction(pSladran->getFaction());
            if (Unit *pTarget = pSladran->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void UpdateAI(const uint32 diff) {}
};

CreatureAI* GetAI_mob_sladran_summon_target(Creature* pCreature)
{
    return new mob_sladran_summon_targetAI(pCreature);
}

/*######
## boss_sladran
######*/
struct MANGOS_DLL_DECL boss_sladranAI : public ScriptedAI
{
    boss_sladranAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonTimer;
    uint32 m_uiPoisonNovaTimer;
    uint32 m_uiPowerfulBiteTimer;
    uint32 m_uiVenomBoltTimer;

    void Reset()
    {
        m_uiSummonTimer       = m_bIsRegularMode ? 5000 : 3000;
        m_uiPoisonNovaTimer   = 22000;
        m_uiPowerfulBiteTimer = 10000;
        m_uiVenomBoltTimer    = 15000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLADRAN, IN_PROGRESS);
    }
    void JustReachedHome()
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_SLADRAN, NOT_STARTED);
    }
    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLADRAN, DONE);
    }

    Creature* SelectRandomCreatureOfEntryInRange(uint32 uiEntry, float fRange)
    {
        std::list<Creature* > lCreatureList;
        GetCreatureListWithEntryInGrid(lCreatureList, m_creature, uiEntry, fRange);

        if (lCreatureList.empty())
            return NULL;

        std::list<Creature* >::iterator iter = lCreatureList.begin();
        advance(iter, urand(0, lCreatureList.size()-1));

        return *iter;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPoisonNovaTimer < uiDiff)
        {
            DoScriptText(EMOTE_NOVA, m_creature);
            DoCast(m_creature->getVictim(),m_bIsRegularMode ? SPELL_POISON_NOVA : SPELL_POISON_NOVA_H);
            m_uiPoisonNovaTimer = 22000;
        }
        else
            m_uiPoisonNovaTimer -= uiDiff;

        if (m_uiSummonTimer < uiDiff)
        {
            if (Creature* pSummonTarget = SelectRandomCreatureOfEntryInRange(NPC_SLADRAN_SUMMON_TARGET, 75.0f))
            {
                if (urand(0, 3))
                {
                    // we don't want to get spammed
                    if (!urand(0, 4))
                        DoScriptText(SAY_SUMMON_CONSTRICTOR, m_creature);

                    pSummonTarget->CastSpell(pSummonTarget, SPELL_SUMMON_CONSTRICTOR, false);
                }
                else
                {
                    // we don't want to get spammed
                    if (!urand(0, 4))
                        DoScriptText(SAY_SUMMON_SNAKE, m_creature);

                    pSummonTarget->CastSpell(pSummonTarget, SPELL_SUMMON_VIPER, false);
                }
            }

            m_uiSummonTimer = m_bIsRegularMode ? 5000 : 3000;
        }
        else
            m_uiSummonTimer -= uiDiff;

        if (m_uiPowerfulBiteTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_POWERFUL_BITE : SPELL_POWERFUL_BITE_H);
            m_uiPowerfulBiteTimer = 10000;
        }
        else
            m_uiPowerfulBiteTimer -= uiDiff;

        if (m_uiVenomBoltTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_VENOM_BOLT : SPELL_VENOM_BOLT_H);

            m_uiVenomBoltTimer = 15000;
        }
        else
            m_uiVenomBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sladran(Creature* pCreature)
{
    return new boss_sladranAI(pCreature);
}

void AddSC_boss_sladran()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_sladran";
    newscript->GetAI = &GetAI_boss_sladran;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sladran_summon_target";
    newscript->GetAI = &GetAI_mob_sladran_summon_target;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sladran_snake_wrap";
    newscript->GetAI = &GetAI_mob_sladran_snake_wrap;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_sladran_constrictor";
    newscript->GetAI = &GetAI_mob_sladran_constrictor;
    newscript->RegisterSelf();
}
