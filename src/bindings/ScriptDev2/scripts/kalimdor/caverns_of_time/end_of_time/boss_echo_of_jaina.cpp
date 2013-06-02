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
SDName: boss_echo_of_jaina
SD%Complete: 0
SDComment: Placeholder
SDCategory: End of Time
EndScriptData */

#include "precompiled.h"
#include "end_of_time.h"
#include "TemporarySummon.h"

enum Yells
{
    SAY_AGGRO_1 = 1,
    SAY_AGGRO_2,
    SAY_FROST_BLADES, // Spell 1
    SAY_FROSTBOLT_VOLLEY, // Spell 2
    SAY_BLINK, // Spell 3
    SAY_SLAY_1,
    SAY_SLAY_2,
    SAY_SLAY_3,
    SAY_DEATH
};

enum
{
    SPELL_BLINK = 101812,

    SPELL_FLARECORE_MISSILE = 101927,
    SPELL_UNSTABLE_FLARE = 101980, // When touched. Needs radius 5y.
    SPELL_TIME_EXPIRE_FLARE = 101587, // If not exploded in 10s. Needs radius 200y.
    SPELL_CHECK_PLAYER_DIST = 101588, // Periodic dummy on npc, check 500 ms.

    SPELL_FROSTBOLT_VOLLEY = 101810, // Needs radius 65y.
    SPELL_PYROBLAST = 101809,

    SPELL_FROST_BLADES = 101339, // Summon Frost Blades spell. Use with JustSummoned to send them away in a straight line, otherwise npc works ok.
};

enum Events
{
    EVENT_FLARECORE = 1, // 20 sec cd
    EVENT_BLINK, // Every 30 sec.
    EVENT_FROSTBOLT_VOLLEY, // 3 times in succession after blink.
    EVENT_PYROBLAST, // Constantly, no melee. Every 5 to 6 sec.
    EVENT_FROST_BLADES, // 25 sec cd, just after blink.

    // FlareCore
    EVENT_CHECK_PLAYER,
    EVENT_EXPLODE
};

enum Creatures
{
    NPC_FLARECORE   = 54446,
    NPC_FROSTBLADES = 54494,
    NPC_BLINK_TARGT = 54542,
};

static const Position2 JainaSummonPos = { 3051.510f, 510.032f, 21.562f, 3.02f };

struct MANGOS_DLL_DECL boss_echo_of_jainaAI : public ScriptedAI
{
    boss_echo_of_jainaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 VolleyCount;
    EventMap events;
    std::list<ObjectGuid> summons;

    void DespawnAllSummons()
    {
        for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* creature = me->GetMap()->GetAnyTypeCreature(*itr))
                creature->ForcedDespawn();
        summons.clear();
    }

    void Reset() override
    {
        DespawnAllSummons();
        events.Reset();
        VolleyCount = 0;
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JAINA, IN_PROGRESS);

        //Talk(RAND(SAY_AGGRO_1, SAY_AGGRO_2));
        events.ScheduleEvent(EVENT_PYROBLAST, urand(2000, 3000));
        events.ScheduleEvent(EVENT_BLINK, urand(28000, 31000));
        events.ScheduleEvent(EVENT_FLARECORE, urand(14000, 17000));
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JAINA, DONE);

        //Talk(SAY_DEATH);
    }

    void JustSummoned(Creature* summon)
    {
        summons.push_back(summon->GetObjectGuid());
        summon->SetActiveObjectState(true);
        if (me->IsInCombat())
            summon->SetInCombatWithZone();

        summon->setFaction(m_creature->getFaction());

        switch(summon->GetEntry())
        {
            case NPC_FROSTBLADES:
                summon->ForcedDespawn(10000);
                break;
            default:
                summon->SetLevel(m_creature->getLevel());
                break;
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JAINA, FAIL);

        DespawnAllSummons();
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(diff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PYROBLAST:
                    DoCast(me->getVictim(), SPELL_PYROBLAST);
                    events.ScheduleEvent(EVENT_PYROBLAST, urand(3500, 4500));
                    break;
                case EVENT_FLARECORE:
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(target, SPELL_FLARECORE_MISSILE);
                    events.ScheduleEvent(EVENT_FLARECORE, urand(19000, 21000));
                    break;
                case EVENT_BLINK:
                    //Talk(SAY_BLINK);
                    DoCast(me, SPELL_BLINK);
                    if (Creature* blink = GetClosestCreatureWithEntry(me, NPC_BLINK_TARGT, 200.0f, true))
                        me->NearTeleportTo(blink->GetPositionX(), blink->GetPositionY(), blink->GetPositionZ(), blink->GetOrientation());
                    events.CancelEvent(EVENT_PYROBLAST);
                    events.CancelEvent(EVENT_FLARECORE);
                    events.ScheduleEvent(EVENT_FROST_BLADES, 1500);
                    events.ScheduleEvent(EVENT_FROSTBOLT_VOLLEY, 2000);
                    events.ScheduleEvent(EVENT_BLINK, 31500);
                    break;
                case EVENT_FROSTBOLT_VOLLEY:
                    if(VolleyCount < 3)
                    {
                        DoCast(me, SPELL_FROSTBOLT_VOLLEY);
                        events.ScheduleEvent(EVENT_FROSTBOLT_VOLLEY, 2200);
                        ++VolleyCount;
                    }
                    else
                    {
                        //Talk(SAY_FROSTBOLT_VOLLEY);
                        DoCast(me, SPELL_FROSTBOLT_VOLLEY);
                        events.CancelEvent(EVENT_FROSTBOLT_VOLLEY);
                        VolleyCount = 0;
                        events.ScheduleEvent(EVENT_PYROBLAST, urand(3000, 3500));
                        events.ScheduleEvent(EVENT_FLARECORE, urand (7500, 8500));
                    }
                    break;
                case EVENT_FROST_BLADES:
                    //Talk(SAY_FROST_BLADES);
                    me->SummonCreature(NPC_FROSTBLADES, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation() - 0.15f, TEMPSUMMON_TIMED_DESPAWN, 10000, false, me->getLevel());
                    me->SummonCreature(NPC_FROSTBLADES, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 10000, false, me->getLevel());
                    me->SummonCreature(NPC_FROSTBLADES, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation() + 0.15f, TEMPSUMMON_TIMED_DESPAWN, 10000, false, me->getLevel());
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_echo_of_jaina(Creature* pCreature)
{
    return new boss_echo_of_jainaAI(pCreature);
}

struct MANGOS_DLL_DECL npc_flarecoreAI : public ScriptedAI
{
    npc_flarecoreAI(Creature* creature) : ScriptedAI(creature) 
    {
        m_pInstance = (ScriptedInstance*)me->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    EventMap events;

    void Reset() override
    {
        if (!me->HasAura(SPELL_CHECK_PLAYER_DIST))
            me->CastSpell(me, SPELL_CHECK_PLAYER_DIST, true);
        events.Reset();

        events.ScheduleEvent(EVENT_CHECK_PLAYER, 500);
        events.ScheduleEvent(EVENT_EXPLODE, 10000);
    }

    void EnterCombat(Unit* /*who*/) override
    {
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        return;
    }

    void UpdateAI(uint32 const diff) override
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CHECK_PLAYER:
                    if (GetClosestAttackableUnit(me, 5.0f))
                    {
                        DoCast(me, SPELL_UNSTABLE_FLARE);
                        me->ForcedDespawn(100);
                    }
                    events.ScheduleEvent(EVENT_CHECK_PLAYER, 500);
                    break;
                case EVENT_EXPLODE:
                    DoCast(me, SPELL_TIME_EXPIRE_FLARE);
                    me->ForcedDespawn(100);
                    break;
            }
        }
    }
};

CreatureAI* GetAI_npc_flarecore(Creature* pCreature)
{
    return new npc_flarecoreAI(pCreature);
}

struct MANGOS_DLL_DECL npc_frostbladeAI : public ScriptedAI
{
    npc_frostbladeAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    uint32 counter;
    bool init;
    float orientation;

    void Reset() override
    {
        init = false;
        counter = 0;
    }

    void MovementInform(uint32 uiMoveType, uint32 uiPointId) override
    {
        if (counter >= 20 || uiMoveType != POINT_MOTION_TYPE || !uiPointId)
            return;

        ++counter;

        Position pos;
        m_creature->GetPosition(pos.x, pos.y, pos.z);
        m_creature->GetNearPoint(m_creature, pos.x, pos.y, pos.z, m_creature->GetObjectBoundingRadius(), 10.0f, orientation);
        m_creature->GetMotionMaster()->MovePoint(1, pos.x, pos.y, pos.z);
    }

    void EnterCombat(Unit* /*who*/) override
    {
        return;
    }

    void MoveInLineOfSight(Unit* pWho) override
    {
        return;
    }

    void UpdateAI(uint32 diff) override
    {
        if (!init && m_creature->IsTemporarySummon())
        {
            init = true;
            if (Unit* summoner = ((TemporarySummon*)m_creature)->GetSummoner())
            {
                orientation = me->GetOrientation();
                m_creature->SetWalk(false);

                Position pos;
                m_creature->GetPosition(pos.x, pos.y, pos.z);
                m_creature->GetNearPoint(m_creature, pos.x, pos.y, pos.z, m_creature->GetObjectBoundingRadius(), 10.0f, orientation);
                m_creature->GetMotionMaster()->MovePoint(1, pos.x, pos.y, pos.z);
            }
        }
    }
};

CreatureAI* GetAI_npc_frostblade(Creature* pCreature)
{
    return new npc_frostbladeAI(pCreature);
}

bool OnGOUse_go_jaina_staff_fragment(Player* player, GameObject* go)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData();
    if (!pInstance)
        return true;

    uint32 fragmetsCount = pInstance->GetData(TYPE_FRAGMENTS) + 1;

    if (fragmetsCount < MAX_FRAGMENTS_COUNT)
    {
        pInstance->SetData(TYPE_FRAGMENTS, fragmetsCount);
        pInstance->DoUpdateWorldState(WORLDSTATE_FRAGMENTS_COLLECTED, fragmetsCount);
        go->Delete();
    }
    else // Fragments reach 16.
    {
        pInstance->SetData(TYPE_FRAGMENTS, MAX_FRAGMENTS_COUNT);
        pInstance->DoUpdateWorldState(WORLDSTATE_FRAGMENTS_COLLECTED, MAX_FRAGMENTS_COUNT);
        if (Creature* creature = pInstance->GetSingleCreatureFromStorage(NPC_ECHO_OF_JAINA))
        {
            creature->SetVisibility(VISIBILITY_ON);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        }
        if (Creature* creature = pInstance->GetSingleCreatureFromStorage(NPC_JAINA_CIRCLE_VISUAL))
            creature->SetVisibility(VISIBILITY_ON);

        go->Delete();
    }

    return true;
}

void AddSC_boss_echo_of_jaina()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_echo_of_jaina";
    pNewScript->GetAI = &GetAI_boss_echo_of_jaina;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_flarecore";
    pNewScript->GetAI = &GetAI_npc_flarecore;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_frostblade";
    pNewScript->GetAI = &GetAI_npc_frostblade;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_jaina_staff_fragment";
    pNewScript->pGOUse = &OnGOUse_go_jaina_staff_fragment;
    pNewScript->RegisterSelf();
}
