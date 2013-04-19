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
SDName: boss_lord_obsidius
SD%Complete: 10
SDComment: Placeholder
SDCategory: Blackrock Caverns
EndScriptData */

#include "precompiled.h"
#include "blackrock_caverns.h"

enum Spells
{
    SPELL_THUNDERCLAP = 76186,
    SPELL_TWILIGHT_CORRUPTION = 93613,
    SPELL_STONE_BLOW = 76185,

    SPELL_SHADOWY_CORRUPTION = 75054,
};

enum Events
{
    EVENT_THUNDERCLAP = 1,
    EVENT_TWILIGHT_CORRUPTION = 2,
    EVENT_STONE_BLOW = 3,
};

Position2 const summonPositions[3] =
{
    {349.049f, 569.186f, 66.0078f, 3.11421f},
    {350.668f, 553.744f, 66.0078f, 3.12599f},
    {344.015f, 561.265f, 66.0078f, 3.15348f},
};

struct MANGOS_DLL_DECL boss_lord_obsidiusAI : public ScriptedAI
{
    boss_lord_obsidiusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    ObjectGuid ShadowOfObsidiusList[3];
    EventMap events;
    uint8 Phase;

    void Reset() override
    {
        events.Reset();

        // Spawns his Shadows and Respawns
        for(uint8 i = 0; i <= (m_bIsRegularMode ? 1 : 2); i++)
        {
            if (Creature* shadow = m_creature->GetMap()->GetAnyTypeCreature(ShadowOfObsidiusList[i]))
                shadow->ForcedDespawn();

            if (Creature* shadow = m_creature->SummonCreature(NPC_SHADOW_OF_OBSIDIUS,summonPositions[i].x,summonPositions[i].y,summonPositions[i].z,summonPositions[i].o,TEMPSUMMON_MANUAL_DESPAWN, 0))
                ShadowOfObsidiusList[i] = shadow->GetObjectGuid();
        }
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OBSIDIUS, IN_PROGRESS);
    }

    void EnterCombat(Unit* /*who*/) override
    {
        m_creature->RemoveAurasDueToSpell(SPELL_SHADOWY_CORRUPTION);

        events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, 10000);
        events.ScheduleEvent(EVENT_STONE_BLOW, 13000);

        if (!m_bIsRegularMode)
            events.ScheduleEvent(EVENT_THUNDERCLAP, 7000);

        Phase = 0;

        for(uint8 i = 0; i <= (m_bIsRegularMode ? 1 : 2); i++)
            if (Creature* shadow = m_creature->GetMap()->GetAnyTypeCreature(ShadowOfObsidiusList[i]))
                shadow->Attack(m_creature->getVictim(),false);

        m_creature->MonsterYell("You come seeking answers? Then have them! Look upon your answer to living!", LANG_UNIVERSAL, NULL);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OBSIDIUS, DONE);

        for(uint8 i = 0; i <= (m_bIsRegularMode ? 1 : 2); i++)
            if (Creature* shadow = m_creature->GetMap()->GetAnyTypeCreature(ShadowOfObsidiusList[i]))
                shadow->ForcedDespawn();

        m_creature->MonsterYell("I cannot be destroyed... Only... de... layed...", LANG_UNIVERSAL, NULL);
    }

    void KilledUnit(Unit* pVictim) override
    {
        m_creature->MonsterYell("Earth can be shaped, molded... You cannot! You are useless!", LANG_UNIVERSAL, NULL);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OBSIDIUS, FAIL);

        m_creature->CastSpell(m_creature, SPELL_SHADOWY_CORRUPTION, true);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 69.0f && Phase == 0 || m_creature->GetHealthPercent() < 34.0f && Phase == 1)
        {
            Phase++;

            // Switch Position with a random Shadow of Obsidius and empty Threat list

            Creature* target = m_creature->GetMap()->GetAnyTypeCreature(ShadowOfObsidiusList[urand(0, (m_bIsRegularMode ? 1 : 2))]);
            WorldLocation telePos;

            m_creature->GetPosition(telePos);

            // Switch Positions
            if (target)
            {
                m_creature->NearTeleportTo(target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(),0);
                target->NearTeleportTo(telePos.coord_x,telePos.coord_y,telePos.coord_z, 0);
            }

            // Resetts Aggro
            m_creature->getThreatManager().clearReferences();

            m_creature->MonsterYell("Your kind has no place in the master's world.", LANG_UNIVERSAL, NULL);

            return;
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_THUNDERCLAP:
                    DoCastAOE(SPELL_THUNDERCLAP);
                    events.ScheduleEvent(EVENT_THUNDERCLAP, 7000);
                    break;

                case EVENT_TWILIGHT_CORRUPTION:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget,SPELL_TWILIGHT_CORRUPTION);

                    events.ScheduleEvent(EVENT_TWILIGHT_CORRUPTION, 10000);
                    break;

                case EVENT_STONE_BLOW:
                    DoCastVictim(SPELL_STONE_BLOW);
                    events.ScheduleEvent(EVENT_STONE_BLOW, 13000);
                    break;

                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lord_obsidius(Creature* pCreature)
{
    return new boss_lord_obsidiusAI(pCreature);
}

void AddSC_boss_lord_obsidius()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lord_obsidius";
    pNewScript->GetAI = &GetAI_boss_lord_obsidius;
    pNewScript->RegisterSelf();
}
