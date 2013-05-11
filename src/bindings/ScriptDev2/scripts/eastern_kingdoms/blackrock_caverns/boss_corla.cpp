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
SDName: boss_corla
SD%Complete: 10
SDComment: Placeholder
SDCategory: Blackrock Caverns
EndScriptData */

#include "precompiled.h"
#include "blackrock_caverns.h"


enum Spells
{
    // Corlas Spells
    SPELL_EVOLUTION = 75697,
    SPELL_TWILIGHT_EVOLUTION = 75732,

    SPELL_AURA_OF_ACCELERATION = 75817,
    SPELL_DARK_COMMAND = 75823,

    SPELL_KNEELING_IN_SUPPLICATION = 75608,
    SPELL_DRAIN_ESSENCE_CHANNELING = 75645,

    SPELL_NETHERESSENCE_AURA = 75649,
    SPELL_NETHERESSENCE_VISUAL = 75650,

    // Envolved Twilight Zealot & Twilight Drakonid
    SPELL_FORCE_BLAST = 76522,
    SPELL_GRAVITY_STRIKE = 76561,
    SPELL_GRIEVOUS_WHIRL = 93658,
    SPELL_SHADOW_STRIKE = 66134,
};

enum Events
{
    EVENT_DARK_COMMAND = 1,

    EVENT_FORCE_BLAST = 2,
    EVENT_GRAVITY_STRIKE = 3,
    EVENT_GRIEVOUS_WHIRL = 4,
    EVENT_SHADOW_STRIKE = 5,
};

enum Actions
{
    ACTION_TRIGGER_START_CHANNELING = 1,
    ACTION_TRIGGER_STOP_CHANNELING = 2,
};

Position2 const summonPositions[4] =
{
    {573.676f, 980.619f, 155.354f, 1.58448f}, // Twilight Zealot Spawn Positions
    {580.919f, 982.981f, 155.354f, 2.05572f},
    {565.629f, 983.0f, 155.354f, 0.689123f},
    {573.534668f, 966.700256f, 160.890472f, 1.482759f}, // Nether Essence Trigger SPawning Position
};

struct MANGOS_DLL_DECL boss_corlaAI : public ScriptedAI
{
    boss_corlaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    ObjectGuid TwilightZealotsList[3];
    ObjectGuid NetherEssenceTrigger[3];

    void Reset() override
    {
        m_creature->RemoveAllAuras();
        events.Reset();

        // Resets Twilight Zealots
        for(uint8 i = 0; i <= (m_bIsRegularMode ? 1 : 2); i++)
        {
            Creature* zealot = m_creature->GetMap()->GetAnyTypeCreature(TwilightZealotsList[i]);
            if (!zealot)
            {
                TwilightZealotsList[i].Clear();
                if (Creature* trigger = m_creature->GetMap()->GetAnyTypeCreature(NetherEssenceTrigger[i]))
                    trigger->ForcedDespawn();
                NetherEssenceTrigger[i].Clear();

                if (zealot = m_creature->SummonCreature(NPC_TWILIGHT_ZEALOT_CORLA,summonPositions[i].x,summonPositions[i].y,summonPositions[i].z,summonPositions[i].o,TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    TwilightZealotsList[i] = zealot->GetObjectGuid();
                    if (Creature* creature = zealot->SummonCreature(NPC_NETHER_ESSENCE_TRIGGER,summonPositions[3].x,summonPositions[3].y,summonPositions[3].z,summonPositions[3].o,TEMPSUMMON_MANUAL_DESPAWN,0))
                        NetherEssenceTrigger[i] = creature->GetObjectGuid();
                }
            }
            // Spawns the Trigger if neccesary
            else
            {
                Creature* trigger = m_creature->GetMap()->GetAnyTypeCreature(NetherEssenceTrigger[i]);
                if (!trigger)
                {
                    if (Creature* creature = zealot->SummonCreature(NPC_NETHER_ESSENCE_TRIGGER,summonPositions[3].x,summonPositions[3].y,summonPositions[3].z,summonPositions[3].o,TEMPSUMMON_MANUAL_DESPAWN,0))
                        NetherEssenceTrigger[i] = creature->GetObjectGuid();
                }
                else
                {
                    // Stops Channeling from the Triggers to the Zealots
                    if (trigger->AI())
                        trigger->AI()->DoAction(ACTION_TRIGGER_STOP_CHANNELING);
                }
            }

            if (zealot)
            {
                // Respawns the Zealots if neccesary
                if (zealot->isDead())
                    zealot->Respawn();

                zealot->RemoveAurasDueToSpell(SPELL_AURA_OF_ACCELERATION);
                zealot->RemoveAurasDueToSpell(SPELL_TWILIGHT_EVOLUTION);
                zealot->RemoveAurasDueToSpell(SPELL_EVOLUTION);

                zealot->NearTeleportTo(summonPositions[i].x,summonPositions[i].y,summonPositions[i].z,summonPositions[i].o);

                if (!zealot->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
                    zealot->CastSpell(zealot, SPELL_KNEELING_IN_SUPPLICATION, true);
            }
        }

        m_creature->CastSpell(m_creature, SPELL_DRAIN_ESSENCE_CHANNELING, true);
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORLA, IN_PROGRESS);

        m_creature->CastStop();
        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

        events.ScheduleEvent(EVENT_DARK_COMMAND, 20000);

        // Informs the trigger on what Twilight Zealot he should channel the nether essence
        for(uint8 i = 0; i <= (m_bIsRegularMode ? 1 : 2); i++)
            if (Creature* creature = m_creature->GetMap()->GetAnyTypeCreature(NetherEssenceTrigger[i]))
            {
                if (creature->AI())
                    creature->AI()->DoAction(ACTION_TRIGGER_START_CHANNELING);
                if (i == 0)
                    creature->CastSpell(creature, SPELL_NETHERESSENCE_AURA, true);
            }

        DoCast(m_creature, SPELL_AURA_OF_ACCELERATION);

        m_creature->MonsterYell("HERETICS! You will suffer for this interruption!", LANG_UNIVERSAL, NULL);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORLA, DONE);


        for(uint8 i = 0; i <= (m_bIsRegularMode ? 1 : 2); i++)
            if (Creature* creature = m_creature->GetMap()->GetAnyTypeCreature(NetherEssenceTrigger[i]))
                if (creature->AI())
                    creature->AI()->DoAction(ACTION_TRIGGER_STOP_CHANNELING);

        for(uint8 i = 0; i <= (m_bIsRegularMode ? 1 : 2); i++)
        {
            if (Creature* zealot = m_creature->GetMap()->GetAnyTypeCreature(TwilightZealotsList[i]))
            {
                zealot->ForcedDespawn();
                TwilightZealotsList[i].Clear();
            }
        }

        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            i->getSource()->RemoveAurasDueToSpell(SPELL_TWILIGHT_EVOLUTION);

        m_creature->MonsterYell("For the master I'd die a thousand times... A thousan...", LANG_UNIVERSAL, NULL);
    }

    void KilledUnit(Unit* pVictim) override
    {
        m_creature->MonsterYell("There is only one true path of enlightenment! DEATH!", LANG_UNIVERSAL, NULL);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORLA, FAIL);

        Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            i->getSource()->RemoveAurasDueToSpell(SPELL_TWILIGHT_EVOLUTION);

        Reset();
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_DARK_COMMAND:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget,SPELL_DARK_COMMAND);

                    events.ScheduleEvent(EVENT_DARK_COMMAND, 20000);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct mob_twilight_zealotAI : public ScriptedAI
{
    mob_twilight_zealotAI(Creature* creature) : ScriptedAI(creature), Intialized(false) {}

    bool Intialized;
    EventMap events;

    void Reset() override
    {
        events.Reset();
        Intialized = false;

        //m_creature->SetReactState(REACT_PASSIVE);
    }

    void UpdateAI(const uint32 Diff) override
    {
        if(!Intialized && !m_creature->HasAura(SPELL_KNEELING_IN_SUPPLICATION))
        {
            events.ScheduleEvent(EVENT_FORCE_BLAST, 10000);
            events.ScheduleEvent(EVENT_GRAVITY_STRIKE, 22000);
            events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, 7000);
            events.ScheduleEvent(EVENT_SHADOW_STRIKE, 14000);

            Intialized = true;

            //m_creature->SetReactState(REACT_AGGRESSIVE);

            m_creature->GetMotionMaster()->MoveChase(GetPlayerAtMinimumRange(1.0f));
            m_creature->Attack(GetPlayerAtMinimumRange(1.0f), false);
        }

        if (!m_creature->isInCombat() || m_creature->HasAura(SPELL_TWILIGHT_EVOLUTION))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_FORCE_BLAST:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget,SPELL_FORCE_BLAST);

                    events.ScheduleEvent(EVENT_FORCE_BLAST, 10000);
                    break;
                case EVENT_GRAVITY_STRIKE:
                    DoCastVictim(SPELL_GRAVITY_STRIKE);
                    events.ScheduleEvent(EVENT_GRAVITY_STRIKE, 22000);
                    break;
                case EVENT_GRIEVOUS_WHIRL:
                    DoCastAOE(SPELL_GRIEVOUS_WHIRL);
                    events.ScheduleEvent(EVENT_GRIEVOUS_WHIRL, 7000);
                    break;
                case EVENT_SHADOW_STRIKE:
                    DoCastVictim(SPELL_SHADOW_STRIKE);
                    events.ScheduleEvent(EVENT_SHADOW_STRIKE, 14000);
                    break;

                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct mob_corla_netheressence_triggerAI : public ScriptedAI
{
    mob_corla_netheressence_triggerAI(Creature* creature) : ScriptedAI(creature)
    {
        channelTarget = NULL;
        lastTarget = NULL;
        zealot = NULL;
    }

    Unit* zealot;
    Unit* channelTarget;
    Unit* lastTarget;

    // We need no EventMap becouse we have only 1 Event
    uint32 uiCheckPlayerIsBetween;
    uint32 uiNetherEssenceVisual;

    void Reset() override
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS,UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!zealot)
        {
            if (Unit* owner = m_creature->GetCreator())
                zealot = owner;
        }

        if (channelTarget == NULL/* || lastTarget == NULL */|| zealot == NULL)
            return;

        if (zealot->HasAura(SPELL_TWILIGHT_EVOLUTION) || zealot->isDead())
            return;

        if (uiCheckPlayerIsBetween <= diff)
        {
            channelTarget = zealot;
            Map::PlayerList const &PlayerList = m_creature->GetMap()->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                {
                    if(i->getSource()->IsInBetween(m_creature, zealot, 1.0f))
                        channelTarget = i->getSource();
                }
            }

            SpellAuraHolder* holder = channelTarget->GetSpellAuraHolder(SPELL_EVOLUTION, zealot->GetObjectGuid());
            if (!holder)
                holder = channelTarget->_AddAura(SPELL_EVOLUTION, 15000, zealot);

            if (holder)
            {
                holder->ModStackAmount(1);
                holder->RefreshHolder();

                if (holder->GetStackAmount() >= holder->GetSpellProto()->GetStackAmount())
                {
                    if(channelTarget == zealot)
                        channelTarget->RemoveAllAuras();

                    zealot->CastSpell(channelTarget, SPELL_TWILIGHT_EVOLUTION, true);
                }
            }

            uiCheckPlayerIsBetween = 500;
        }
        else
            uiCheckPlayerIsBetween -= diff;

        if (uiNetherEssenceVisual <= diff)
        {
            m_creature->CastSpell(m_creature, SPELL_NETHERESSENCE_VISUAL, true);
            uiNetherEssenceVisual = urand(3500,4000);
        }
        else
            uiNetherEssenceVisual -= diff;
    }

    void DoAction(int32 action) override
    {
        switch (action)
        {
            case ACTION_TRIGGER_START_CHANNELING:
                channelTarget = zealot;

                uiCheckPlayerIsBetween = 100;
                uiNetherEssenceVisual = 500;

                // To avoid that on beginning no spell is casted.
                lastTarget = m_creature;
                break;

            case ACTION_TRIGGER_STOP_CHANNELING:
                m_creature->RemoveAllAuras();
                lastTarget = channelTarget = NULL;
                break;
        }
    }
};

CreatureAI* GetAI_mob_twilight_zealot(Creature* pCreature)
{
    return new mob_twilight_zealotAI(pCreature);
}

CreatureAI* GetAI_mob_corla_netheressence_trigger(Creature* pCreature)
{
    return new mob_corla_netheressence_triggerAI(pCreature);
}

CreatureAI* GetAI_boss_corla(Creature* pCreature)
{
    return new boss_corlaAI(pCreature);
}

void AddSC_boss_corla()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_corla";
    pNewScript->GetAI = &GetAI_boss_corla;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_twilight_zealot";
    pNewScript->GetAI = &GetAI_mob_twilight_zealot;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_corla_netheressence_trigger";
    pNewScript->GetAI = &GetAI_mob_corla_netheressence_trigger;
    pNewScript->RegisterSelf();
}
