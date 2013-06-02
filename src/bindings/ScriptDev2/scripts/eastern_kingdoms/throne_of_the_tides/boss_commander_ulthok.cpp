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
SDName: boss_commander_ulthok
SD%Complete: 0
SDComment: Placeholder
SDCategory: Throne of the Tides
EndScriptData */

#include "precompiled.h"
#include "throne_of_the_tides.h"

enum Spells
{
    SPELL_DARK_FISSURE          = 76047,
    SPELL_SQUEEZE               = 76026,
    SPELL_SQUEEZE_VEHICLE       = 76028,
    SPELL_ENRAGE                = 76100,
    SPELL_CURSE_OF_FATIGUE      = 76094,
    SPELL_DARK_FISSURE_AURA     = 76066,
    SPELL_DARK_FISSURE_AURA_H   = 91371,
    SPELL_DARK_FISSURE_DMG      = 76085,
    SPELL_DARK_FISSURE_DMG_H    = 91375,
    SPELL_ULTHOK_INTRO          = 82960,
};

enum Events
{
    EVENT_DARK_FISSURE      = 1,
    EVENT_SQUEEZE           = 2,
    EVENT_CURSE_OF_FATIGUE  = 3,
    EVENT_ENRAGE            = 4,
};

enum Actions
{
    ACTION_COMMANDER_ULTHOK_START_EVENT = 2,
};

enum Adds
{
    NPC_DARK_FISSURE = 40784,
};

struct MANGOS_DLL_DECL boss_commander_ulthokAI : public ScriptedAI
{
    boss_commander_ulthokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    std::list<ObjectGuid> summons;

    void DespawnAllSummons()
    {
        for (std::list<ObjectGuid>::const_iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* unit = me->GetMap()->GetAnyTypeCreature(*itr))
                unit->ForcedDespawn();

        summons.clear();
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
        events.Reset();
        DespawnAllSummons();
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_COMMANDER_ULTHOK_START_EVENT)
        {
            me->SetPhaseMask(PHASEMASK_NORMAL, true);
            DoCast(me, SPELL_ULTHOK_INTRO);
            if (GameObject* pCorales = m_pInstance->GetSingleGameObjectFromStorage(GO_CORALES))
            {
                pCorales->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
                pCorales->SetPhaseMask(2, true);
            }
        }
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COMMANDER_ULTHOK, IN_PROGRESS);

        events.ScheduleEvent(EVENT_DARK_FISSURE, urand(5000, 8000));
        events.ScheduleEvent(EVENT_ENRAGE, urand(20000, 25000));
        events.ScheduleEvent(EVENT_CURSE_OF_FATIGUE, urand(9000, 15000));
        events.ScheduleEvent(EVENT_SQUEEZE, urand(14000, 20000));
        //instance->SetBossState(DATA_COMMANDER_ULTHOK, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COMMANDER_ULTHOK, DONE);

        DespawnAllSummons();
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COMMANDER_ULTHOK, FAIL);

        DespawnAllSummons();
    }

    void JustSummoned(Creature* unit) override
    {
        summons.push_back(unit->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DARK_FISSURE:
                    DoCast(me, SPELL_DARK_FISSURE);
                    events.ScheduleEvent(EVENT_DARK_FISSURE, urand(20000, 22000));
                    break;
                case EVENT_ENRAGE:
                    DoCast(me, SPELL_ENRAGE);
                    events.ScheduleEvent(EVENT_ENRAGE, urand(20000, 25000));
                    break;
                case EVENT_SQUEEZE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                        DoCast(pTarget, SPELL_SQUEEZE);
                    events.ScheduleEvent(EVENT_SQUEEZE, urand(29000, 31000));
                    break;
                case EVENT_CURSE_OF_FATIGUE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                        DoCast(pTarget, SPELL_CURSE_OF_FATIGUE);
                    events.ScheduleEvent(EVENT_CURSE_OF_FATIGUE, urand(13000, 15000));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_commander_ulthok(Creature* pCreature)
{
    return new boss_commander_ulthokAI(pCreature);
}

struct MANGOS_DLL_DECL npc_ulthok_dark_fissureAI : public Scripted_NoMovementAI
{
    npc_ulthok_dark_fissureAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        DoCast(me, SPELL_DARK_FISSURE_AURA, true);
    }

    void UpdateAI(const uint32 diff) override
    {
    }
};

CreatureAI* GetAI_npc_ulthok_dark_fissure(Creature* pCreature)
{
    return new npc_ulthok_dark_fissureAI(pCreature);
}

bool OnTrigger_at_tott_commander_ulthok(Player* pPlayer, const AreaTriggerEntry* /*pAt*/)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance || !pPlayer)
        return true;

    if (pInstance->GetData(TYPE_COMMANDER_ULTHOK_EVENT) != DONE
        && pInstance->GetData(TYPE_LADY_NAZJAR) == DONE)
    {
        pInstance->SetData(TYPE_COMMANDER_ULTHOK_EVENT, DONE);
        if (Creature* pUlthok = pInstance->GetSingleCreatureFromStorage(NPC_COMMANDER_ULTHOK))
        {
            pUlthok->AI()->DoAction(ACTION_COMMANDER_ULTHOK_START_EVENT);
        }
    }

    return true;
}

void AddSC_boss_commander_ulthok()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_commander_ulthok";
    pNewScript->GetAI = &GetAI_boss_commander_ulthok;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ulthok_dark_fissure";
    pNewScript->GetAI = &GetAI_npc_ulthok_dark_fissure;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_tott_commander_ulthok";
    pNewScript->pAreaTrigger = &OnTrigger_at_tott_commander_ulthok;
    pNewScript->RegisterSelf();
}
