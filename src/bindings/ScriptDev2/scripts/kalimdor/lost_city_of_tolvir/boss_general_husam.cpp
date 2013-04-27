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
SDName: boss_general_husam
SD%Complete: 10
SDComment: Placeholder
SDCategory: Lost City of Tol'Vir
EndScriptData */

#include "precompiled.h"
#include "lost_city_of_tolvir.h"


enum Spells
{
    SPELL_BAD_INTENTIONS = 83113,
    SPELL_SHOCKWAVE = 83445,
    SPELL_HAMMER_FIST = 83655,

    SPELL_MYSTIC_TRAP_PLANT = 83646,
    SPELL_MYSTIC_TRAP_EXPLODE = 83171,
    SPELL_DETONATE_TRAPS = 91263,
    SPELL_MYSTIC_TRAP_EXPLODE_EFFECT = 83110,

    SPELL_SHOCKWAVE_SUMMON_EFFECT = 83128,
    SPELL_SHOCKWAVE_VISUAL_TRIGGER = 83127,
    SPELL_SHOCKWAVE_TARGET_EST = 83133,
    SPELL_SHOCKWAVE_TARGET_OUEST = 83134,
    SPELL_SHOCKWAVE_TARGET_NORD = 83131,
    SPELL_SHOCKWAVE_TARGET_SUD = 83132,
    SPELL_SHOWKWAVE_VISUAL_DAMAGE = 83130,
};

enum Yells
{
    SAY_AGGRO = -1755001,
    SAY_SHOCKWAVE = -1755002,
    SAY_DETONATE = -1755003,
    SAY_DEATH = -1755004,
};

enum Actions
{
    ACTION_DETONATE = 1,
};

enum Creatures
{
    CREATURE_HUSAM = 44577,
    CREATURE_LAND_MINE = 44840,
};

struct MANGOS_DLL_DECL boss_general_husamAI : public ScriptedAI
{
    boss_general_husamAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 BadIntentionsTimer;
    uint32 ShockwaveTimer;
    uint32 HammerFistTimer;
    uint32 PlantTimer;
    uint32 DetonateTimer;

    void Reset() override
    {
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HUSAM, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        //DoScriptText(SAY_DEATH, me);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HUSAM, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_HUSAM, FAIL);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(BadIntentionsTimer <= diff)
        {
            if (Unit* pTar = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTar,SPELL_BAD_INTENTIONS);
            BadIntentionsTimer = 15000;
        }
        else
            BadIntentionsTimer -= diff;

        if(ShockwaveTimer <= diff)
        {
            Creature* Target1 = m_creature->SummonCreature(44711, m_creature->GetPositionX()+10,m_creature->GetPositionY(),m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
            Creature* Target2 = m_creature->SummonCreature(44711, m_creature->GetPositionX()-10,m_creature->GetPositionY(),m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
            Creature* Target3 = m_creature->SummonCreature(44711, m_creature->GetPositionX(),m_creature->GetPositionY()+10,m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
            Creature* Target4 = m_creature->SummonCreature(44711, m_creature->GetPositionX(),m_creature->GetPositionY()-10,m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000);
            //DoScriptText(SAY_SHOCKWAVE, m_creature);
            DoCastAOE(SPELL_SHOCKWAVE);
            DoCast(Target1, 83130);
            DoCast(Target2, 83130);
            DoCast(Target3, 83130);
            DoCast(Target4, 83130);
            if (Creature* Shockwave1 = m_creature->SummonCreature(44712, m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                Shockwave1->GetMotionMaster()->MoveChase(Target1);
            if (Creature* Shockwave2 = m_creature->SummonCreature(44712, m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                Shockwave2->GetMotionMaster()->MoveChase(Target2);
            if (Creature* Shockwave3 = m_creature->SummonCreature(44712, m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                Shockwave3->GetMotionMaster()->MoveChase(Target3);
            if (Creature* Shockwave4 = m_creature->SummonCreature(44712, m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10000))
                Shockwave4->GetMotionMaster()->MoveChase(Target4);
            ShockwaveTimer = urand(20000,40000);
        }
        else
            ShockwaveTimer -= diff;

        if (HammerFistTimer <= diff)
        {
            DoCast(m_creature->getVictim(), SPELL_HAMMER_FIST);
            HammerFistTimer = 12000;
        }
        else
            HammerFistTimer -= diff;

        if (PlantTimer <= diff)
        {
            if (Unit* pTar = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pTar->CastSpell(pTar, SPELL_MYSTIC_TRAP_PLANT, true);
            PlantTimer = 3000;
        }
        else
            PlantTimer -= diff;

        if (DetonateTimer <= diff && !m_bIsRegularMode)
        {
            DoCast(m_creature, SPELL_DETONATE_TRAPS);
            //DoScriptText(SAY_DETONATE, me);
            DetonateTimer = 20000;
        }
        else
            DetonateTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_shokwave_spikeAI : public Scripted_NoMovementAI
{
    mob_shokwave_spikeAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
        DoCast(m_creature, SPELL_SHOWKWAVE_VISUAL_DAMAGE);
        DoCast(m_creature, SPELL_SHOCKWAVE);
        m_creature->ForcedDespawn(10000);
    }

    void Reset() override { }
};

struct mob_shokwave_spike_visualAI : public Scripted_NoMovementAI
{
    mob_shokwave_spike_visualAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
        m_creature->_AddAura(SPELL_SHOCKWAVE_VISUAL_TRIGGER, -1);
        m_creature->ForcedDespawn(20000);
    }

    void Reset() override { }
};

struct npc_trapAI : public Scripted_NoMovementAI
{
    npc_trapAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature) 
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_ATTACKABLE_1 | UNIT_FLAG_NOT_SELECTABLE | UNIT_FLAG_PACIFIED);
        DoCastAOE(SPELL_MYSTIC_TRAP_EXPLODE);
        m_creature->_AddAura(SPELL_MYSTIC_TRAP_EXPLODE_EFFECT, -1);
    }

    void Reset() override { }
};

CreatureAI* GetAI_boss_general_husam(Creature* pCreature)
{
    return new boss_general_husamAI(pCreature);
}

CreatureAI* GetAI_mob_shokwave_spike(Creature* pCreature)
{
    return new mob_shokwave_spikeAI(pCreature);
}

CreatureAI* GetAI_mob_shokwave_spike_visual(Creature* pCreature)
{
    return new mob_shokwave_spike_visualAI(pCreature);
}

CreatureAI* GetAI_npc_trap(Creature* pCreature)
{
    return new npc_trapAI(pCreature);
}

void AddSC_boss_general_husam()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_general_husam";
    pNewScript->GetAI = &GetAI_boss_general_husam;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shokwave_spike";
    pNewScript->GetAI = &GetAI_mob_shokwave_spike;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_shokwave_spike_visual";
    pNewScript->GetAI = &GetAI_mob_shokwave_spike_visual;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_trap";
    pNewScript->GetAI = &GetAI_npc_trap;
    pNewScript->RegisterSelf();
}
