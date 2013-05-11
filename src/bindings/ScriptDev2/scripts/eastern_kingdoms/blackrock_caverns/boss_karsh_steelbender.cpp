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
SDName: boss_karsh_steelbender
SD%Complete: 10
SDComment: Placeholder
SDCategory: Blackrock Caverns
EndScriptData */

#include "precompiled.h"
#include "blackrock_caverns.h"

enum
{
    SPELL_QUECKSILVER_ARMOR = 75842,
    SPELL_SUPERHEATED_QUECKSILVER_ARMOR = 75846,

    SPELL_HEAT_WAVE = 63677,
    SPELL_BURNING_METAL = 76002,
    SPELL_CLEAVE = 845,
    SPELL_LAVA_SPOUT = 76007,

    SPELL_LAVA_POOL = 93628,
};

enum Events
{
    EVENT_CLEAVE = 1,
    EVENT_CHECK_ARMOR_STATE = 2,
    EVENT_LAVA_POOL = 3,
};

Position2 const summonPositions[3] =
{
    {268.717f, 789.984f, 95.3499f, 4.86041f},
    {226.707f, 754.725f, 95.3501f, 2.75163f},
    {216.941f, 808.943f, 95.35f, 0.638911f},
};

struct MANGOS_DLL_DECL boss_karsh_steelbenderAI : public ScriptedAI
{
    boss_karsh_steelbenderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();

        std::list<Creature*> creatures;
        GetCreatureListWithEntryInGrid(creatures, m_creature, NPC_LAVA_SPOUT_TRIGGER, 1000.0f);

        if (creatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
            (*iter)->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    uint8 SpawnCount;

    void Reset() override
    {
        events.Reset();
        m_creature->RemoveAllAuras();

        DespawnCreatures(NPC_BOUND_FLAMES);
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KARSH, IN_PROGRESS);
    }

    void EnterCombat(Unit* /*who*/)
    {
        DoCast(m_creature, SPELL_QUECKSILVER_ARMOR);
        events.ScheduleEvent(EVENT_CLEAVE, urand(15000,18000));
        events.ScheduleEvent(EVENT_CHECK_ARMOR_STATE, 1000);

        m_creature->MonsterYell("Bodies to test my armaments upon!", LANG_UNIVERSAL, NULL);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KARSH, DONE);
        DespawnCreatures(NPC_BOUND_FLAMES);
        m_creature->MonsterYell("Merely an impurity in the compound...", LANG_UNIVERSAL, NULL);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_KARSH, FAIL);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if ((!m_creature->HasAura(SPELL_QUECKSILVER_ARMOR)) && (!m_creature->HasAura(SPELL_SUPERHEATED_QUECKSILVER_ARMOR)))
        {    // Summon Adds
            LavaSpoutErrupt();

            m_creature->MonsterYell("Feel the burn!", LANG_UNIVERSAL, NULL);

            DoCast(m_creature, SPELL_QUECKSILVER_ARMOR);

            if (m_bIsRegularMode)
                return;

            // Heroic: Summon Adds

            uint8 r = urand(0,2);

            for(uint8 i = 0; i<=2; i++)
                m_creature->SummonCreature(NPC_BOUND_FLAMES, summonPositions[r].x, summonPositions[r].y, summonPositions[r].z, summonPositions[r].o, TEMPSUMMON_CORPSE_DESPAWN, 0);

            SpawnCount = 3;

            return;
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CLEAVE:
                    DoCastVictim(SPELL_CLEAVE);
                    events.ScheduleEvent(EVENT_CLEAVE, urand(15000,18000));
                    break;
                case EVENT_CHECK_ARMOR_STATE:
                    // Checks whether the Boss is in heat range
                    if (m_creature->GetDistance(237.166f, 785.067f, 95.67f /*Stream of Molten*/) < 4.5f)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_QUECKSILVER_ARMOR);

                        // We cant do that with CastSpell because with the Spell the Normal Armor is applied too
                        SpellAuraHolder* holder = m_creature->GetSpellAuraHolder(SPELL_SUPERHEATED_QUECKSILVER_ARMOR, m_creature->GetObjectGuid());
                        if (!holder)
                            holder = m_creature->_AddAura(SPELL_SUPERHEATED_QUECKSILVER_ARMOR, 17000);
                        if (holder)
                        {
                            holder->ModStackAmount(1);
                            holder->RefreshHolder();
                        }

                        DoCastAOE(SPELL_HEAT_WAVE);
                    }

                    events.ScheduleEvent(EVENT_CHECK_ARMOR_STATE, 3000);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    void DamageDeal(Unit* victim, uint32& damage) override
    {
        if (damage > 0 && m_creature->HasAura(SPELL_SUPERHEATED_QUECKSILVER_ARMOR))
            DoCastVictim(SPELL_BURNING_METAL);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        if(summon->GetEntry() == NPC_BOUND_FLAMES)
        {
            SpawnCount--;

            if(SpawnCount == 0)
                DoCastAOE(SPELL_LAVA_POOL,true);
        }
    }

    void DespawnCreatures(uint32 entry)
    {
        std::list<Creature*> creatures;
        GetCreatureListWithEntryInGrid(creatures, m_creature, entry, 1000.0f);

        if (creatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
            (*iter)->ForcedDespawn();
    }

    void LavaSpoutErrupt()
    {
        std::list<Creature*> creatures;
        GetCreatureListWithEntryInGrid(creatures, m_creature, NPC_LAVA_SPOUT_TRIGGER, 1000.0f);

        if (creatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
            (*iter)->CastSpell((*iter), SPELL_LAVA_SPOUT, true);
    }
};

CreatureAI* GetAI_boss_karsh_steelbender(Creature* pCreature)
{
    return new boss_karsh_steelbenderAI(pCreature);
}

void AddSC_boss_karsh_steelbender()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_karsh_steelbender";
    pNewScript->GetAI = &GetAI_boss_karsh_steelbender;
    pNewScript->RegisterSelf();
}
