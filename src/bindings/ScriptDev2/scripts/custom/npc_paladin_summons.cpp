/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Npc_Paladin_Summons
SD%Complete:
SDComment:
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "TemporarySummon.h"

enum
{
    SPELL_GUARDIAN_PROT             = 86659,
    SPELL_GUARDIAN_HOLY             = 86669,
    SPELL_GUARDIAN_RETRO            = 86698,

    SPELL_ANCIENT_GUARDIAN          = 86657,
    SPELL_ANCIENT_FURY              = 86704,

    SPELL_ANCIENT_HEALER            = 86674,
    SPELL_ANCIENT_CRUSADER_PLAYER   = 86701,
    SPELL_ANCIENT_CRUSADER_PET      = 86703,
};

struct MANGOS_DLL_DECL npc_guardian_of_the_ancient_kingsAI : public ScriptedAI
{

    bool init;
    uint32 summonedBySpell;

    npc_guardian_of_the_ancient_kingsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        init = false;
        summonedBySpell = 0;
    }

    void BeforeDespawn()
    {
        if (summonedBySpell == SPELL_GUARDIAN_RETRO)
            if (Unit* owner = m_creature->GetCreator())
            {
                owner->CastSpell(owner, SPELL_ANCIENT_FURY, true);
                owner->RemoveAurasDueToSpell(SPELL_ANCIENT_CRUSADER_PLAYER);
            }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (summonedBySpell == SPELL_GUARDIAN_RETRO)
            ScriptedAI::MoveInLineOfSight(pWho);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (summonedBySpell == SPELL_GUARDIAN_RETRO)
            CreatureAI::AttackedBy(pAttacker);
    }

    void EnterCombat(Unit* pEnemy)
    {
        if (summonedBySpell == SPELL_GUARDIAN_RETRO)
            ScriptedAI::EnterCombat(pEnemy);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!init)
        {
            init = true;
            Unit* owner = m_creature->GetCreator();
            if (!owner)
                return;

            summonedBySpell = m_creature->GetUInt32Value(UNIT_CREATED_BY_SPELL);

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if (summonedBySpell == SPELL_GUARDIAN_PROT)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                m_creature->StopMoving();
                m_creature->SetTargetGuid(owner->GetObjectGuid());
                m_creature->CastSpell(owner, SPELL_ANCIENT_GUARDIAN, true);
            }
            else if (summonedBySpell == SPELL_GUARDIAN_RETRO)
            {
                owner->CastSpell(owner, SPELL_ANCIENT_CRUSADER_PLAYER, true);
                m_creature->CastSpell(m_creature, SPELL_ANCIENT_CRUSADER_PET, true);
            }
            else if (summonedBySpell == SPELL_GUARDIAN_HOLY)
                owner->CastSpell(owner, SPELL_ANCIENT_HEALER, true);
        }
        else
        {
            if (summonedBySpell == SPELL_GUARDIAN_RETRO)
            {
                Unit* owner = m_creature->GetOwner();
                Unit* ownerVictim = owner ? owner->getVictim() : NULL;

                if (m_creature->getVictim())
                {
                    if (m_creature->getVictim()->hasUnitState(UNIT_STAT_NO_FREE_MOVE))
                    {
                        m_creature->AttackStop();
                        return;
                    }
                    else if (ownerVictim && m_creature->getVictim() != ownerVictim && !ownerVictim->hasUnitState(UNIT_STAT_NO_FREE_MOVE))
                        AttackStart(ownerVictim);
                }
                else if (ownerVictim && !ownerVictim->hasUnitState(UNIT_STAT_NO_FREE_MOVE))
                    AttackStart(ownerVictim);
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (summonedBySpell == SPELL_GUARDIAN_RETRO)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_guardian_of_the_ancient_kings(Creature* pCreature)
{
    return new npc_guardian_of_the_ancient_kingsAI(pCreature);
}

void AddSC_npc_paladin_summons()
{
    Script *pNewScript;
    pNewScript = new Script;
    pNewScript->Name="npc_guardian_of_the_ancient_kings";
    pNewScript->GetAI = &GetAI_npc_guardian_of_the_ancient_kings;
    pNewScript->RegisterSelf();
}