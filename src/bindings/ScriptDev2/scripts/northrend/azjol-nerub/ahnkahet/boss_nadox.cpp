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
SDName: Boss_Nadox
SD%Complete: 90%
SDComment: TODO: some more research on guardian aura needed, BroodRage needs core and db support
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"

enum
{
    SAY_AGGRO                     = -1619000,
    SAY_SUMMON_EGG_1              = -1619001,
    SAY_SUMMON_EGG_2              = -1619002,
    SAY_SLAY_1                    = -1619003,
    SAY_SLAY_2                    = -1619004,
    SAY_SLAY_3                    = -1619005,
    SAY_DEATH                     = -1619006,
    EMOTE_HATCH                   = -1619007,

    SPELL_BROOD_PLAGUE            = 56130,
    SPELL_BROOD_PLAGUE_H          = 59467,
    SPELL_BERSERK                 = 26662,
    SPELL_BROOD_RAGE              = 59465,

    SPELL_GUARDIAN_AURA           = 56151,
    SPELL_GUARDIAN_AURA_TRIGGERED = 56153,
    SPELL_GUARDIAN_SPRINT         = 56354,

    // JustSummoned is not called for spell summoned creatures
    SPELL_SUMMON_SWARM_GUARDIAN   = 56120,
    SPELL_SUMMON_SWARMERS         = 56119,
};

/*######
## boss_nadox
######*/

struct MANGOS_DLL_DECL boss_nadoxAI : public ScriptedAI
{
    boss_nadoxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    bool   m_bBerserk;
    uint32 m_uiBroodPlagueTimer;
    uint32 m_uiBroodRageTimer;
    uint32 m_uiSwarmerSummonTimer;
    uint32 m_uiGuardianSummonTimer;

    void Reset()
    {
        m_bBerserk = false;
        m_uiSwarmerSummonTimer = 10000;
        m_uiGuardianSummonTimer = 25000;
        m_uiBroodPlagueTimer = 15000;
        m_uiBroodRageTimer = 20000;
        m_creature->SetRespawnDelay(DAY);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NADOX, NOT_STARTED);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NADOX, FAIL);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NADOX,IN_PROGRESS);
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

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_NADOX,DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBroodPlagueTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_BROOD_PLAGUE : SPELL_BROOD_PLAGUE_H);

            m_uiBroodPlagueTimer = 20000;
        }
        else
            m_uiBroodPlagueTimer -= uiDiff;

        if (!m_bBerserk && (m_creature->GetPositionZ() < 24.0))
        {
            m_bBerserk = true;
            DoCast(m_creature, SPELL_BERSERK);
        }

        DoMeleeAttackIfReady();

        if (m_uiSwarmerSummonTimer < uiDiff)
        {
            DoScriptText(urand(0, 1) ? SAY_SUMMON_EGG_1 : SAY_SUMMON_EGG_2, m_creature);

            if (Creature* pSwarmerEgg = SelectRandomCreatureOfEntryInRange(NPC_AHNKAHAR_SWARM_EGG, 120.0))
            {
                // pSwarmerEgg->CastSpell(pSwarmerEgg, SPELL_SUMMON_SWARMERS, false);
                if(Creature *pSwarmer = pSwarmerEgg->SummonCreature(NPC_AHNKAHAR_SWARMER, pSwarmerEgg->GetPositionX(), pSwarmerEgg->GetPositionY(), pSwarmerEgg->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                    pSwarmer->SetInCombatWithZone();

                if(Creature *pSwarmer = pSwarmerEgg->SummonCreature(NPC_AHNKAHAR_SWARMER, pSwarmerEgg->GetPositionX(), pSwarmerEgg->GetPositionY(), pSwarmerEgg->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                    pSwarmer->SetInCombatWithZone();
            }
            m_uiSwarmerSummonTimer = 10000;
        }
        else
            m_uiSwarmerSummonTimer -= uiDiff;

        if (m_uiGuardianSummonTimer < uiDiff)
        {
            DoScriptText(EMOTE_HATCH, m_creature);

            if (Creature* pGuardEgg = SelectRandomCreatureOfEntryInRange(NPC_AHNKAHAR_GUARDIAN_EGG, 120.0))
            {
                // pSwarmerEgg->CastSpell(pSwarmerEgg, SPELL_SUMMON_SWARMERS, false);
                if (Creature *pGuard = pGuardEgg->SummonCreature(NPC_AHNKAHAR_GUARDIAN, pGuardEgg->GetPositionX(), pGuardEgg->GetPositionY(), pGuardEgg->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 3*MINUTE*IN_MILLISECONDS))
                    pGuard->SetInCombatWithZone();
            }
            m_uiGuardianSummonTimer = 25000;
        }
        else
            m_uiGuardianSummonTimer -= uiDiff;

        if(!m_bIsRegularMode)
        {
            if (m_uiBroodRageTimer < uiDiff)
            {
                if (Creature* pRageTarget = SelectRandomCreatureOfEntryInRange(NPC_AHNKAHAR_SWARMER, 75.0))
                    DoCast(pRageTarget, SPELL_BROOD_RAGE);

                m_uiBroodRageTimer = 20000;
            }
            else
                m_uiBroodRageTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_nadox(Creature* pCreature)
{
    return new boss_nadoxAI(pCreature);
}

/*######
## mob_nadox_Guardian
######*/

struct MANGOS_DLL_DECL mob_nadox_guardianAI : public ScriptedAI
{
    mob_nadox_guardianAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    void Reset(){}

    void JustDied(Unit* pKiller)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoCastSpellIfCan(m_creature, SPELL_GUARDIAN_AURA, CAST_AURA_NOT_PRESENT);

        if (!m_creature->CanReachWithMeleeAttack(m_creature->getVictim()))
            DoCastSpellIfCan(m_creature, SPELL_GUARDIAN_SPRINT, CAST_AURA_NOT_PRESENT);

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_nadox_guardian(Creature* pCreature)
{
    return new mob_nadox_guardianAI(pCreature);
}

void AddSC_boss_nadox()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_nadox";
    pNewScript->GetAI = &GetAI_boss_nadox;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_nadox_guardian";
    pNewScript->GetAI = &GetAI_mob_nadox_guardian;
    pNewScript->RegisterSelf();
}
