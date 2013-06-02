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
SDName: boss_occuthar
SD%Complete: 10
SDComment: Placeholder
SDCategory: Baradin Hold
EndScriptData */

#include "precompiled.h"
#include "baradin_hold.h"

enum Spells
{
    SPELL_BERSERK                   = 47008, 
    SPELL_SEARING_SHADOW            = 96913, 

    SPELL_FOCUSED_FIRE              = 96872,
    SPELL_FOCUSED_FIRE_DUMMY        = 96886,
    SPELL_FOCUSED_FIRE_1            = 96884,

    SPELL_EYE_OF_OCCUTHAR           = 96920, 
    SPELL_EYE_OF_OCCUTHAR_25        = 101006,
    SPELL_EYE_OF_OCCUTHAR_VEHICLE   = 96932,

    SPELL_EYE_OF_OCCUTHAR_DMG       = 96942,

    SPELL_OCCUTHARS_DESTRUCTION     = 96968,
    
};

enum Events
{
    EVENT_BERSERK           = 1,
    EVENT_SEARING_SHADOWS   = 2,
    EVENT_EYE_OF_OCCUTHAR   = 3,
    EVENT_FOCUSED_FIRE      = 4,
};

enum Adds
{
    NPC_EYE_OF_OCCUTHAR = 52389,
    NPC_EYESTALK        = 52369,
};

struct MANGOS_DLL_DECL boss_occutharAI : public ScriptedAI
{
    boss_occutharAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    std::list<ObjectGuid> summons;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
        events.Reset();
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OCCUTHAR, IN_PROGRESS);

        events.ScheduleEvent(EVENT_BERSERK, 300000);
        events.ScheduleEvent(EVENT_EYE_OF_OCCUTHAR, 59000);
        events.ScheduleEvent(EVENT_FOCUSED_FIRE, 26000);
        events.ScheduleEvent(EVENT_SEARING_SHADOWS, 24000);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OCCUTHAR, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OCCUTHAR, FAIL);
    }

    void JustSummoned(Creature* unit) override
    {
        summons.push_back(unit->GetObjectGuid());

        switch (unit->GetEntry())
        {
            case NPC_EYESTALK:
                unit->SetInCombatWithZone();
                me->CastSpell(unit, SPELL_FOCUSED_FIRE_1, true);
                unit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!UpdateVictim())
            return;

        events.Update(uiDiff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoCast(me, SPELL_BERSERK, true);
                    break;
                case EVENT_SEARING_SHADOWS:
                    DoCast(me->getVictim(), SPELL_SEARING_SHADOW);
                    events.ScheduleEvent(EVENT_SEARING_SHADOWS, 24000);
                    break;
                case EVENT_FOCUSED_FIRE:
                    DoCastAOE(SPELL_FOCUSED_FIRE);
                    events.ScheduleEvent(EVENT_FOCUSED_FIRE, 14000);
                    break;
                case EVENT_EYE_OF_OCCUTHAR:
                    DoCastAOE(SPELL_EYE_OF_OCCUTHAR);
                    events.RescheduleEvent(EVENT_FOCUSED_FIRE, 26000);
                    events.ScheduleEvent(EVENT_EYE_OF_OCCUTHAR, 59000);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_occuthar(Creature* pCreature)
{
    return new boss_occutharAI(pCreature);
}

struct MANGOS_DLL_DECL npc_occuthar_eye_of_occutharAI : public Scripted_NoMovementAI
{
    npc_occuthar_eye_of_occutharAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        uiDestructionTimer = 9500;
        bDestruction = false;
    }

    uint32 uiDestructionTimer;
    bool bDestruction;

    void UpdateAI(const uint32 diff) override
    {
        if (uiDestructionTimer <= diff && !bDestruction)
        {
            bDestruction = true;
            DoCastAOE(SPELL_OCCUTHARS_DESTRUCTION, true);
            me->ForcedDespawn();
        }
        else
            uiDestructionTimer -= diff;
    }
};

CreatureAI* GetAI_npc_occuthar_eye_of_occuthar(Creature* pCreature)
{
    return new npc_occuthar_eye_of_occutharAI(pCreature);
}

void AddSC_boss_occuthar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_occuthar";
    pNewScript->GetAI = &GetAI_boss_occuthar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_occuthar_eye_of_occuthar";
    pNewScript->GetAI = &GetAI_npc_occuthar_eye_of_occuthar;
    pNewScript->RegisterSelf();
}
