/*Copyright (C) 2012 SkyMist Project.
*
* Script 99% done. 1% = Testing live.
*
* THIS particular file is NOT free software; third-party users should NOT have access to it, redistribute it or modify it. :)
* We need to think up a better copyright than this. Who's your third party on the net dude, dude? Should NOT? Er must nicht!
*/

#include "precompiled.h"
#include "stonecore.h"

enum Spells
{    
    SPELL_SANDBLAST   = 80807,  //Sandblast spell
    SPELL_LAVA_FISS   = 80803,  //cast on target - on our server it might already be DB scripted and it works fine. Spell Range needs to be added to damage spells 80800 + 80801 - 5yds.
    SPELL_CRYSTAL     = 92565,  //Crystal breath - 2.5 sec cast time + 6 seconds.
    
    SPELL_STALACTITE  = 80656,  //Trigger Multiple Summon Spell
    SPELL_STALAC_VIS  = 80643,  //Multiple effects - triger spell 80647(DMG)
    SPELL_STALAC_WAR  = 80654,  //Warning for where it falls.
    
    SPELL_ERUPTION    = 80800,  //Cast by lava on self. - 80798 might be visual dummy and 80801 periodic
};

enum Events
{
    EVENT_LAVA_FISS = 1,
    EVENT_SANDBLAST,
    EVENT_AIR_PHASE,
    EVENT_FLIGHT,
    EVENT_CRYSTAL,
    EVENT_GROUND,
    EVENT_LAND,
    EVENT_STALACTITE
};

enum Creatures
{
    NPC_LAVA_FISSURE = 43242,
    NPC_STALACTITE   = 43159
};

enum Phases
{
    PHASE_NULL,
    PHASE_GROUND,
    PHASE_AIR
};

/*** Note: Doors are rock stalactites. Starting position is flying outside of platform. Cast AOE stalactite on Line of Sight. ***/

struct MANGOS_DLL_DECL boss_slabhideAI : public ScriptedAI
{
    boss_slabhideAI(Creature* creature) : ScriptedAI(creature)
    {
        m_pInstance = (ScriptedInstance*)creature->GetInstanceData();
        m_bIsRegularMode = creature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    Phases phase;

    bool introDone, inCombat, introFinished;
    uint32 m_introTimer;
    EventMap events;

    void Reset()
    {
        events.Reset();

        if (m_pInstance)
            m_pInstance->SetBossState(BOSS_SLABHIDE, NOT_STARTED);

        introDone = false;
        phase = PHASE_NULL;
        events.SetPhase(PHASE_NULL);
        inCombat = false;
        introFinished = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        //m_creature->SetReactState(REACT_PASSIVE);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);

        //summons.DespawnAll();
    }

    void Aggro(Unit* who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLABHIDE, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SLABHIDE, FAIL);
    }

    void EnterEvadeMode()
    {
        events.Reset();
        inCombat = false;
        phase = PHASE_NULL;
        events.SetPhase(PHASE_NULL);

        if (m_pInstance)
        {
            m_pInstance->SetBossState(BOSS_SLABHIDE, FAIL);
            m_pInstance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, m_creature); // Remove

            //instance->HandleGameObject(0000000, true); // Open the doors.
            //instance->HandleGameObject(0000000, true);
        }
    }

/*** Note: www.wowhead.com/npc=43357 - This npc might be a trigger for the AOE in the gauntlet. Need to check sniffs. ***/
    void MoveInLineOfSight(Unit* who)
    {
        if (introDone || !m_creature->IsWithinDistInMap(who, 30.0f, false))
            return;

        if (m_pInstance) // Open the doors.
        {
            //m_pInstance->HandleGameObject(0000000, true);
            //m_pInstance->HandleGameObject(0000000, true);
        }

        DoCastAOE(SPELL_STALACTITE);
        m_creature->GetMotionMaster()->MovePoint(1, 1286.771f, 1237.231f, 247.032f);

        m_introTimer = 15000;
        introDone = true;
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            m_pInstance->SetBossState(BOSS_SLABHIDE, DONE);
            m_pInstance->SetData(TYPE_SLABHIDE, DONE);

            //m_pInstance->HandleGameObject(0000000, true); // Open the doors.
            //m_pInstance->HandleGameObject(0000000, true);

            m_pInstance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, m_creature); // Remove
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetBossState(BOSS_SLABHIDE, IN_PROGRESS);
            m_pInstance->SetBossState(BOSS_SLABHIDE, IN_PROGRESS);
            m_pInstance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, m_creature); // Add

            //m_pInstance->HandleGameObject(0000000, false); // Close doors.
            //m_pInstance->HandleGameObject(0000000, false);
        }

        m_creature->SetInCombatWithZone();

        phase = PHASE_GROUND;
        events.SetPhase(PHASE_GROUND);
        inCombat = true;

        events.ScheduleEvent(EVENT_SANDBLAST, 4000, PHASE_GROUND);
        events.ScheduleEvent(EVENT_LAVA_FISS, 6000, PHASE_GROUND);
        events.ScheduleEvent(EVENT_AIR_PHASE, 20000, PHASE_GROUND);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() && inCombat || m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (m_introTimer <= diff && !introFinished)
        {
            introFinished = true;

            m_creature->SetOrientation(3.864f);
            m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_LEVITATING);
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
            m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
            m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            //m_creature->SetReactState(REACT_AGGRESSIVE);
        }
        else
            m_introTimer -= diff;

        events.Update(diff);

        if (phase == PHASE_GROUND) // Ground phase
        {
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_AIR_PHASE:
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
                        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_LEVITATING);
                        //m_creature->SendMovementFlagUpdate();
                        events.ScheduleEvent(EVENT_FLIGHT, 2000);
                        return;
                                
                    case EVENT_FLIGHT:
                        //m_creature->SetReactState(REACT_PASSIVE);
                        m_creature->AttackStop();
                        m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
                        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_CAN_FLY);
                        m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_FLYING);
                        m_creature->GetMotionMaster()->MovePoint(1, 1275.519f, 1217.723f, m_creature->GetPositionZ() + 15.0f);
                        events.SetPhase(PHASE_AIR);
                        events.ScheduleEvent(EVENT_STALACTITE, 2500, PHASE_AIR);
                        events.ScheduleEvent(EVENT_LAND, 8000, PHASE_AIR);
                        return;

                    case EVENT_SANDBLAST:
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_SANDBLAST);
                        events.ScheduleEvent(EVENT_SANDBLAST, urand(6000, 8000), PHASE_GROUND);
                        break;

                    case EVENT_LAVA_FISS:
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_LAVA_FISS);
                        events.ScheduleEvent(EVENT_LAVA_FISS, urand(8000, 12000), PHASE_GROUND);
                        break;
                                
                    case EVENT_CRYSTAL:
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(target, SPELL_CRYSTAL);
                        break;
                }
            }

            DoMeleeAttackIfReady();
        }
        else if (phase == PHASE_AIR) // Air phase
        {
            if (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_LAND:
                        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                        m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_LEVITATING);
                        //m_creature->SendMovementFlagUpdate();
                        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            m_creature->GetMotionMaster()->MoveChase(target);
                        events.ScheduleEvent(EVENT_GROUND, 2000);
                        return;

                    case EVENT_GROUND:
                        events.SetPhase(PHASE_GROUND);
                        //m_creature->SetReactState(REACT_AGGRESSIVE);
                        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, 0x02);
                        m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_CAN_FLY);
                        m_creature->m_movementInfo.RemoveMovementFlag(MOVEFLAG_FLYING);
                        events.ScheduleEvent(EVENT_SANDBLAST, 6000, PHASE_GROUND);
                        events.ScheduleEvent(EVENT_LAVA_FISS, urand(8000, 12000), PHASE_GROUND);
                        events.ScheduleEvent(EVENT_AIR_PHASE, 30000, PHASE_GROUND);
                        if(!m_bIsRegularMode)
                            events.ScheduleEvent(EVENT_CRYSTAL, 1000, PHASE_GROUND);
                        return;

                    case EVENT_STALACTITE:
                        DoCastAOE(SPELL_STALACTITE);
                        break;
                }
            }
        }
    }
};


CreatureAI* GetAI_boss_slabhide(Creature* pCreature)
{
    return new boss_slabhideAI(pCreature);
}

struct MANGOS_DLL_DECL npc_stalactiteAI : public ScriptedAI
{
    npc_stalactiteAI(Creature* creature) : ScriptedAI(creature)
    {
        timerAura    = 100;
        timerImpact  = 4000;
        timerDespawn = 30000;
        vis = false;
        //creature->SetReactState(REACT_PASSIVE);
    }

    uint32 timerAura;
    uint32 timerDespawn;
    uint32 timerImpact;
    bool vis;

    void Reset() { }

    void UpdateAI(const uint32 diff)
    {
        if (timerAura <= diff)
        {
            DoCast(m_creature, SPELL_STALAC_WAR);
        }
        else
            timerAura -= diff;

        if (timerImpact <= diff && !vis)
        {
            DoCast(m_creature, SPELL_STALAC_VIS);
            vis = true;
        }
        else
            timerImpact -= diff;

        if (timerDespawn <= diff)
        {
            m_creature->ForcedDespawn();
        }
        else
            timerDespawn -= diff;
    }
};

CreatureAI* GetAI_npc_stalactite(Creature* pCreature)
{
    return new npc_stalactiteAI(pCreature);
}

void AddSC_boss_slabhide()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_slabhide";
    pNewScript->GetAI = &GetAI_boss_slabhide;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_stalactite";
    pNewScript->GetAI = &GetAI_npc_stalactite;
    pNewScript->RegisterSelf();
}
