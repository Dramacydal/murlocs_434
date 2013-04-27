/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_beauty
SD%Complete: 10
SDComment: Placeholder
SDCategory: Blackrock Caverns
EndScriptData */

#include "precompiled.h"
#include "blackrock_caverns.h"

#define spell_magma 76031 // Crachat de magma
#define spell_cassure 76032 // Cassure de Flamme
#define spell_cassure_H 93583
#define spell_charge 74367 // Charge
#define spell_charge_H 93580
#define spell_fear 76028 // Rugissement terrifiant
#define spell_fear_H 93586
#define spell_berserk 82395 // Enrage

#define SAY_AGGRO "!!!"
#define SAY_DIED "..."

enum
{
    // ToDo: add spells and yells here
};

struct MANGOS_DLL_DECL boss_beautyAI : public ScriptedAI
{
    boss_beautyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 charge;
    uint32 magma;
    uint32 cassure;
    uint32 fear;
    uint32 Berserk_Timer;

    void Reset() override
    {
        magma = 10000;
        cassure = 12000;
        charge = 8000;
        fear = 15000;
        Berserk_Timer = 300000; //5 minutes
    }

    void EnterCombat(Unit* who) override
    {
        //me->MonsterYell(SAY_AGGRO, LANG_UNIVERSAL, NULL);
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BEAUTY, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BEAUTY, DONE);
        //me->MonsterYell(SAY_DIED, LANG_UNIVERSAL, NULL);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BEAUTY, FAIL);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (magma<= diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, spell_magma);
            magma   = 10000; 
        }
        else
            magma -= diff; 

        if (cassure <= diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, !m_bIsRegularMode ? spell_cassure_H : spell_cassure);
            cassure = 12000;
        }
        else
            cassure -= diff;

        if (charge<= diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, spell_charge, SELECT_FLAG_NOT_IN_MELEE_RANGE))
                DoCast(pTarget, !m_bIsRegularMode ? spell_charge_H : spell_charge);
            charge   = 8000;
        }
        else
            charge -= diff; 

        if (fear<= diff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, !m_bIsRegularMode ? spell_fear_H : spell_fear);
            fear   = 15000; 
        }
        else
            fear -= diff; 

        if (Berserk_Timer <= diff)
        {
            DoCast(m_creature->getVictim(), spell_berserk, true);

            Berserk_Timer = 300000;
        }
        else
            Berserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_beauty(Creature* pCreature)
{
    return new boss_beautyAI(pCreature);
}

void AddSC_boss_beauty()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_beauty";
    pNewScript->GetAI = &GetAI_boss_beauty;
    pNewScript->RegisterSelf();
}
