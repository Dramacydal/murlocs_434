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
SDName: boss_prophet_barim
SD%Complete: 10
SDComment: Placeholder
SDCategory: Lost City of Tol'Vir
EndScriptData */

#include "precompiled.h"
#include "lost_city_of_tolvir.h"

enum Spells
{
    // High Prophet Barim
    SPELL_FIFTY_LASHINGS = 82506,
    SPELL_PLAGUE_OF_AGES = 82622,
    SPELL_REPENTANCE = 82139, // PHASE 2 EFFECT.
    SPELL_REPENTANCE_AURA = 82320,
    SPELL_REPENTANCE_KNOCKBACK = 82012,
    SPELL_HEAVENS_FURY = 81939,
    SPELL_HARBINGER_SUMMON = 82203,
    SPELL_BLAZE_SUMMON = 91180,
    SPELL_PULL_ALL = 51336, // Hack, but safe.
    // Harbinger of Darkness
    SPELL_SOUL_SEVER = 82255,
    SPELL_WAIL_OF_DARKNESS_AOE = 82425,
    SPELL_WAIL_OF_DARKNESS_RANDOM = 82533,
    // Heaven's Fury
    SPELL_HEAVENS_FURY_VISUAL = 81940,
    SPELL_HEAVENS_FURY_AURA = 81941,
    // Blaze of Heavens
    SPELL_BLAZE_OF_THE_HEAVENS_AURA = 95248,
    SPELL_BLAZE_OF_THE_HEAVENS_SUMMON_FIRE_AURA = 91185,
    SPELL_BLAZE_OF_THE_HEAVENS_SUMMON_EGG = 91181,
    // Soul Fragment
    SPELL_MERGED_SOULS = 82263,
    SPELL_SOUL_SEVER_CLONE = 82220, // player should cast it on npc.
};

enum Yells
{
    SAY_AGGRO = -1755005,
    SAY_REPETANCE_1 = -1755006,
    SAY_REPETANCE_2 = -1755007,
    SAY_DEATH = -1755008,
    SAY_KILL = -1755009,
};

enum Phases
{
    PHASE_NORMAL = 1,
    PHASE_50_PRECENT = 2
};

enum Creatures
{
    CREATURE_BARIM = 43612,
    CREATURE_BLAZE_OF_HEAVENS = 48906,
    CREATURE_HARBINGER_OF_DARKNESS = 43927,
    CREATURE_SOUL_FRAGMENT = 43934,
    CREATURE_VEIL_OF_TWILIGHT = 43926,
    CREATURE_HEAVENS_FURY = 43801,	
};

struct MANGOS_DLL_DECL boss_prophet_barimAI : public ScriptedAI
{
    boss_prophet_barimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    std::list<ObjectGuid> SummonList;

    uint8 Phase;

    uint32 FenixTimer;
    uint32 PlagueOfAgesTimer;
    uint32 FiftyLashingsTimer;
    uint32 HeavensFuryTimer;
    uint32 PhaseChangeTimer;

    bool FenixSummoned, Phased;

    void Reset() override
    {
        //m_creature->SetReactState(REACT_AGGRESSIVE);
        RemoveSummons();

        Phase = PHASE_NORMAL;

        FenixTimer = 9000;
        PlagueOfAgesTimer = 7000;
        FiftyLashingsTimer = 10000;
        HeavensFuryTimer = 12000;

        FenixSummoned = false;
        Phased = false;

        m_creature->RemoveAurasDueToSpell(SPELL_REPENTANCE_AURA);
    }

    void Aggro(Unit* pWho) override
    {
        //DoScriptText(SAY_AGGRO, m_creature);
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARIM, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        //DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARIM, DONE);
        RemoveSummons();
    }

    void RemoveSummons()
    {
        if (SummonList.empty())
            return;

        for (std::list<ObjectGuid>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetAnyTypeCreature(*itr))
                pTemp->ForcedDespawn();
        }
        SummonList.clear();
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BARIM, FAIL);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        switch(summon->GetEntry())
        {
            case CREATURE_HARBINGER_OF_DARKNESS:
                PhaseChangeTimer = 100;
                break;
        }
    }

    void JustSummoned(Creature* pSummon)
    {
        switch (pSummon->GetEntry())
        {
            case CREATURE_BLAZE_OF_HEAVENS:
            case CREATURE_HARBINGER_OF_DARKNESS:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (pSummon->AI())
                        pSummon->AI()->AttackStart(pTarget);
                SummonList.push_back(pSummon->GetGUID());
                break;
            case CREATURE_HEAVENS_FURY:
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pSummon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pSummon->CastSpell(pSummon, SPELL_HEAVENS_FURY_VISUAL, true);
                pSummon->CastSpell(pSummon, SPELL_HEAVENS_FURY_AURA, true);
                //pSummon->SetReactState(REACT_PASSIVE);
                SummonList.push_back(pSummon->GetGUID());
                break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 51.0f && Phase == PHASE_NORMAL && Phased == false)
        {
            //m_creature->SetReactState(REACT_PASSIVE);
            //DoScriptText(RAND(SAY_REPETANCE_1,SAY_REPETANCE_2), m_creature);
            DoCast(m_creature, SPELL_REPENTANCE_AURA);
            DoCast(m_creature, SPELL_PULL_ALL);
            DoCastSpellOnPlayers(SPELL_REPENTANCE);
            WorldLocation pos;
            m_creature->GetPosition(pos);
            m_creature->SummonCreature(CREATURE_HARBINGER_OF_DARKNESS, pos.coord_x, pos.coord_y, pos.coord_z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 100);
            SetCombatMovement(false);
            Phase = PHASE_50_PRECENT;
            Phased = true;
            return;
        }

        if (PhaseChangeTimer <= diff && Phase == PHASE_50_PRECENT)
        {
            //m_creature->SetReactState(REACT_AGGRESSIVE);
            m_pInstance->DoRemoveAurasDueToSpellOnPlayers(SPELL_REPENTANCE);
            m_creature->RemoveAurasDueToSpell(SPELL_REPENTANCE_AURA);
            PlagueOfAgesTimer = 7000;
            FiftyLashingsTimer = 2000;
            HeavensFuryTimer = 20000;
            Phase = PHASE_NORMAL;
        }
        else
            PhaseChangeTimer -= diff;

        if (FenixTimer <= diff && FenixSummoned == false && Phase == PHASE_NORMAL)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_BLAZE_SUMMON, true);
            FenixSummoned = true;
        }
        else
            FenixTimer -= diff;

        if (PlagueOfAgesTimer <= diff && Phase == PHASE_NORMAL)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_PLAGUE_OF_AGES, true);
            PlagueOfAgesTimer = urand(12000,16000);
        }
        else
            PlagueOfAgesTimer -= diff;

        if (FiftyLashingsTimer <= diff && Phase == PHASE_NORMAL)
        {
            DoCast(m_creature, SPELL_FIFTY_LASHINGS);
            FiftyLashingsTimer = 20000;
        }
        else
            FiftyLashingsTimer -= diff;

        if (HeavensFuryTimer <= diff && Phase == PHASE_NORMAL)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_HEAVENS_FURY, true);
            HeavensFuryTimer = urand(22000,35000);
        }
        else
            HeavensFuryTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_harbinger_of_darknessAI : public ScriptedAI
{
    npc_harbinger_of_darknessAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    std::list<ObjectGuid> SummonList;

    uint32 SoulSeverTimer;
    uint32 WailOfDarknessTimer;

    void Reset() override
    {
        RemoveSummons();

        SoulSeverTimer = 7000;
        WailOfDarknessTimer = 1000;
    }

    void RemoveSummons()
    {
        if (SummonList.empty())
            return;

        for (std::list<ObjectGuid>::const_iterator itr = SummonList.begin(); itr != SummonList.end(); ++itr)
            if (Creature* pTemp = m_creature->GetMap()->GetAnyTypeCreature(*itr))
                pTemp->ForcedDespawn();

        SummonList.clear();
    }

    void JustSummoned(Creature* pSummon) override
    {
        switch (pSummon->GetEntry())
        {
            case CREATURE_VEIL_OF_TWILIGHT:
                //pSummon->SetReactState(REACT_PASSIVE);
                SummonList.push_back(pSummon->GetGUID());
                break;
            case CREATURE_SOUL_FRAGMENT:
                //pSummon->SetReactState(REACT_PASSIVE);
                pSummon->GetMotionMaster()->Clear();
                pSummon->GetMotionMaster()->MoveFollow(m_creature, m_creature->GetObjectBoundingRadius(), 0.0f);
                SummonList.push_back(pSummon->GetGUID());
                break;
        }
    }

    void JustDied(Unit*) override
    {
        DoCastAOE(SPELL_WAIL_OF_DARKNESS_AOE);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (SoulSeverTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_SOUL_SEVER, true);
            SoulSeverTimer = urand(12000,22000);
        }
        else
            SoulSeverTimer -= diff;

        if (WailOfDarknessTimer <= diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_WAIL_OF_DARKNESS_RANDOM, true);
            WailOfDarknessTimer = 2000;
        }
        else
            WailOfDarknessTimer -= diff;

        if (m_creature->getVictim() && m_creature->getVictim()->GetEntry() == CREATURE_SOUL_FRAGMENT)
        {
            if (m_creature->IsWithinDist(m_creature->getVictim(), m_creature->GetCombatDistance(m_creature->getVictim())))
                m_creature->DealDamage(m_creature->getVictim(), m_creature->getVictim()->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
        else
            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_blaze_of_heavensAI : public ScriptedAI
{
    npc_blaze_of_heavensAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() override
    {
        m_creature->SetSpeedRate(MOVE_RUN, 0.7f, true);
        DoCast(m_creature, SPELL_BLAZE_OF_THE_HEAVENS_AURA);
        DoCast(m_creature, SPELL_BLAZE_OF_THE_HEAVENS_SUMMON_FIRE_AURA);
    }

    void JustDied(Unit*) override
    {
        m_creature->CastSpell(m_creature, SPELL_BLAZE_OF_THE_HEAVENS_SUMMON_EGG, true);
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_blaze_of_heavens_eggAI : public ScriptedAI
{
    npc_blaze_of_heavens_eggAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 RegenTimer;

    void Reset() override
    {
        RegenTimer = 2000;
        m_creature->SetHealthPercent(10.0f);
    }

    void JustSummoned(Creature* pSummon) override
    {
        switch (pSummon->GetEntry())
        {
            case CREATURE_BLAZE_OF_HEAVENS:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummon->AI()->AttackStart(pTarget);
                break;
        }
    }

    void DamageTaken(Unit* doneBy, uint32& damage) override
    {
        if (damage > m_creature->GetHealth())
            damage = 0;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_creature->GetHealth() == m_creature->GetMaxHealth())
        {
            m_creature->CastSpell(m_creature, SPELL_BLAZE_SUMMON, true);
            m_creature->RemoveAurasDueToSpell(91179);
            m_creature->ForcedDespawn(100);
        }

        if (m_creature->GetHealthPercent() < 3.0f)
            m_creature->SetHealthPercent(3.0f);

        if (RegenTimer <= diff)
        {
            m_creature->SetHealth(m_creature->GetHealth() + m_creature->GetMaxHealth() / 10);
            RegenTimer = 2000;
        }
        else
            RegenTimer -= diff;
    }
};

CreatureAI* GetAI_boss_prophet_barim(Creature* pCreature)
{
    return new boss_prophet_barimAI(pCreature);
}

CreatureAI* GetAI_npc_harbinger_of_darkness(Creature* pCreature)
{
    return new npc_harbinger_of_darknessAI(pCreature);
}

CreatureAI* GetAI_npc_blaze_of_heavens(Creature* pCreature)
{
    return new npc_blaze_of_heavensAI(pCreature);
}

CreatureAI* GetAI_npc_blaze_of_heavens_egg(Creature* pCreature)
{
    return new npc_blaze_of_heavens_eggAI(pCreature);
}

void AddSC_boss_prophet_barim()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_prophet_barim";
    pNewScript->GetAI = &GetAI_boss_prophet_barim;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harbinger_of_darkness";
    pNewScript->GetAI = &GetAI_npc_harbinger_of_darkness;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blaze_of_heavens";
    pNewScript->GetAI = &GetAI_npc_blaze_of_heavens;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blaze_of_heavens_egg";
    pNewScript->GetAI = &GetAI_npc_blaze_of_heavens_egg;
    pNewScript->RegisterSelf();
}
