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
SDName: boss_erunak_and_ghursha
SD%Complete: 0
SDComment: Placeholder
SDCategory: Throne of the Tides
EndScriptData */

#include "precompiled.h"
#include "throne_of_the_tides.h"
#include "Spell.h"
#include "SpellMgr.h"

enum ScriptTextsErunak
{
    SAY_FREED   = 0,
    SAY_VICTORY = 1,
};

enum ScriptTextsGhursha
{
    SAY_PHASE   = 0,
    SAY_KILL    = 1,
    SAY_DEATH   = 2,
    SAY_ENCLAVE = 3,
    SAY_FOG     = 4,
};

enum Spells
{
    // Erunak Stonespeaker
    SPELL_EARTH_SHARDS              = 84931,
    SPELL_EARTH_SHARDS_AURA         = 84935,
    SPELL_EARTH_SHARDS_DMG          = 84945,
    SPELL_EARTH_SHARDS_DMG_H        = 91491,
    SPELL_EMBERSTRIKE               = 76165,
    SPELL_LAVA_BOLT                 = 76171,
    SPELL_LAVA_BOLT_H               = 91412,
    SPELL_MAGMA_SPLASH              = 76170,

    // Mindbender Ghur'sha
    SPELL_ENSLAVE                   = 76207,
    SPELL_ENSLAVE_BUFF              = 76213,
    SPELL_ABSORB_MAGIC              = 76307,
    SPELL_MIND_FOG                  = 76234,
    SPELL_MIND_FOG_AURA             = 76230,
    SPELL_MIND_FOG_VISUAL           = 76231,
    SPELL_UNRELENTING_AGONY         = 76339,
    SPELL_UNRELENTING_AGONY_DMG     = 76341,
    SPELL_UNRELENTING_AGONY_DMG_H   = 91493,
};

enum Events
{
    EVENT_EARTH_SHARDS      = 1,
    EVENT_EMBERSTRIKE       = 2,
    EVENT_LAVA_BOLT         = 3,
    EVENT_MAGMA_SPLASH      = 4,
    EVENT_ENSLAVE           = 5,
    EVENT_ABSORB_MAGIC      = 6,
    EVENT_MIND_FOG          = 7,
    EVENT_UNRELENTING_AGONY = 8,
};

enum Actions
{
    ACTION_GHURSHA_START    = 1,
};

enum Adds
{
    NPC_EARTH_SHARDS    = 45469,
    NPC_MIND_FOG        = 40861,
};

struct MANGOS_DLL_DECL boss_erunak_stonespeakerAI : public ScriptedAI
{
    boss_erunak_stonespeakerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool bPhase;
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

        bPhase = false;
        events.Reset();

        if (m_pInstance)
            if (m_pInstance->GetData(TYPE_MINDBENDER_GHURSHA) == DONE || bPhase)
                me->setFaction(35);
    }

    void DespawnAllSummons()
    {
        for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* unit = me->GetMap()->GetAnyTypeCreature(*itr))
                unit->ForcedDespawn(2000);
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MINDBENDER_GHURSHA, IN_PROGRESS);

        events.ScheduleEvent(EVENT_EARTH_SHARDS, 8000);
        events.ScheduleEvent(EVENT_EMBERSTRIKE, 11000);
        events.ScheduleEvent(EVENT_LAVA_BOLT, 13000);
        events.ScheduleEvent(EVENT_MAGMA_SPLASH, 6000);
    }

    void JustSummoned(Creature* summon) override
    {
        if (me->isInCombat())
            summon->SetInCombatWithZone();
    }

    void JustDied(Unit* pKiller) override
    {
        //if (m_pInstance)
        //    m_pInstance->SetData(TYPE_MINDBENDER_GHURSHA, DONE);
        DespawnAllSummons();
    }

    void KilledUnit(Unit* pVictim) override
    {
        //if (pInstance)
        //    if (Creature* pGhursha = ObjectAccessor::GetCreature(*me, pInstance->GetData64(DATA_MINDBENDER_GHURSHA)))
        //        pGhursha->AI()->Talk(SAY_KILL);
    }

    void SpellHit(Unit* caster, SpellEntry const* spellProto) override
    {
        if (Spell const * spell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            if (spell->m_spellInfo->Id == SPELL_LAVA_BOLT
                || me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_LAVA_BOLT_H)
                if (IsSpellHaveEffect(spellProto, SPELL_EFFECT_INTERRUPT_CAST))
                    me->InterruptSpell(CURRENT_GENERIC_SPELL);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MINDBENDER_GHURSHA, FAIL);
        DespawnAllSummons();
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (me->GetHealthPercent() < 50 && !bPhase)
        {
            bPhase = true;
            events.Reset();
            me->setFaction(35);
            EnterEvadeMode();
            if (m_pInstance)
            {
                if (Creature* pGhursha = m_pInstance->GetSingleCreatureFromStorage(NPC_MINDBENDER_GHURSHA))
                    pGhursha->AI()->DoAction(ACTION_GHURSHA_START);
            }
            return;
        }

        events.Update(uiDiff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_EARTH_SHARDS:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_EARTH_SHARDS);
                    events.ScheduleEvent(EVENT_EARTH_SHARDS, 20000);
                    break;
                case EVENT_EMBERSTRIKE:
                    DoCast(me->getVictim(), SPELL_EMBERSTRIKE);
                    events.ScheduleEvent(EVENT_EMBERSTRIKE, 11000);
                    break;
                case EVENT_LAVA_BOLT:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_LAVA_BOLT);
                    events.ScheduleEvent(EVENT_LAVA_BOLT, 14000);
                    break;
                case EVENT_MAGMA_SPLASH:
                    DoCast(me, SPELL_MAGMA_SPLASH);
                    events.ScheduleEvent(EVENT_MAGMA_SPLASH, 13000);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_erunak_stonespeaker(Creature* pCreature)
{
    return new boss_erunak_stonespeakerAI(pCreature);
}

struct MANGOS_DLL_DECL boss_mindbender_ghurshaAI : public ScriptedAI
{
    boss_mindbender_ghurshaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    std::list<ObjectGuid> summons;

    void Reset()
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
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        events.Reset();

        if (Creature* pErunak = m_pInstance->GetSingleCreatureFromStorage(NPC_ERUNAK_STONESPEAKER))
            pErunak->AI()->EnterEvadeMode();
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_GHURSHA_START)
        {
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            if (Unit* pTarget = GetClosestAttackableUnit(me, 100.0f))
                me->GetMotionMaster()->MoveChase(pTarget);
        }
    }

    void EnterCombat(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_ENSLAVE, 13000);
        events.ScheduleEvent(EVENT_ABSORB_MAGIC, 20000);
        events.ScheduleEvent(EVENT_MIND_FOG, urand(6000,12000));
        events.ScheduleEvent(EVENT_UNRELENTING_AGONY, 10000);
        //Talk(SAY_PHASE);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MINDBENDER_GHURSHA, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_ENSLAVE:
                //Talk(SAY_ENCLAVE);
                events.ScheduleEvent(EVENT_ENSLAVE, 31000);
                break;
            case EVENT_ABSORB_MAGIC:
                DoCast(me, SPELL_ABSORB_MAGIC);
                events.ScheduleEvent(EVENT_ABSORB_MAGIC, 15000);
                break;
            case EVENT_MIND_FOG:
                DoCast(me, SPELL_MIND_FOG);
                events.ScheduleEvent(EVENT_MIND_FOG, urand(23000,25000));
                break;
            case EVENT_UNRELENTING_AGONY:
                DoCast(me, SPELL_UNRELENTING_AGONY);
                events.ScheduleEvent(EVENT_UNRELENTING_AGONY, 30000);
                break;
            }
        }
        DoMeleeAttackIfReady();
    }

    void KilledUnit(Unit* victim) override
    {
        //Talk(SAY_KILL);
    }

    void JustDied(Unit* /*killer*/) override
    {
        //Talk(SAY_DEATH);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MINDBENDER_GHURSHA, DONE);

        if (Creature* pErunak = m_pInstance->GetSingleCreatureFromStorage(NPC_ERUNAK_STONESPEAKER))
        {
            pErunak->AI()->EnterEvadeMode();
            //pErunak->AI()->Talk(SAY_VICTORY);
        }
    }
};

CreatureAI* GetAI_boss_mindbender_ghursha(Creature* pCreature)
{
    return new boss_mindbender_ghurshaAI(pCreature);
}

struct MANGOS_DLL_DECL npc_erunak_earth_shardsAI : public Scripted_NoMovementAI
{
    npc_erunak_earth_shardsAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
    }

    uint32 uiDespawnTimer;

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        uiDespawnTimer = 5000;
        DoCast(me, SPELL_EARTH_SHARDS_AURA);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (uiDespawnTimer <= diff)
            me->ForcedDespawn();
        else
            uiDespawnTimer -= diff;
    }
};

CreatureAI* GetAI_npc_erunak_earth_shards(Creature* pCreature)
{
    return new npc_erunak_earth_shardsAI(pCreature);
}

struct MANGOS_DLL_DECL npc_ghursha_mind_fogAI : public Scripted_NoMovementAI
{
    npc_ghursha_mind_fogAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void Reset() override
    {
        DoCast(me, SPELL_MIND_FOG_AURA, true);
        DoCast(me, SPELL_MIND_FOG_VISUAL, true);
    }

    void UpdateAI(const uint32 diff) override
    {
    }
};

CreatureAI* GetAI_npc_ghursha_mind_fog(Creature* pCreature)
{
    return new npc_ghursha_mind_fogAI(pCreature);
}

void AddSC_boss_erunak_and_ghursha()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_erunak_stonespeaker";
    pNewScript->GetAI = &GetAI_boss_erunak_stonespeaker;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_mindbender_ghursha";
    pNewScript->GetAI = &GetAI_boss_mindbender_ghursha;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_erunak_earth_shards";
    pNewScript->GetAI = &GetAI_npc_erunak_earth_shards;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ghursha_mind_fog";
    pNewScript->GetAI = &GetAI_npc_ghursha_mind_fog;
    pNewScript->RegisterSelf();
}
