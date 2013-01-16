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
SDName: Npc_Druid_Summons
SD%Complete:
SDComment:
SDCategory:
EndScriptData */

#include "precompiled.h"
#include "TemporarySummon.h"

enum
{
    WILD_MUSHROOM_INVISIBLE_TIMER    = 6 * IN_MILLISECONDS,
};

struct MANGOS_DLL_DECL npc_druid_wild_mushroomAI : public ScriptedAI
{
    bool init;
    uint32 invisibleTimer;
    bool invisible;

    npc_druid_wild_mushroomAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        init = false;
        invisibleTimer = 0;
        invisible = false;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!init)
        {
            init = true;
            invisibleTimer = WILD_MUSHROOM_INVISIBLE_TIMER;

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            m_creature->StopMoving();
            m_creature->addUnitState(UNIT_STAT_STUNNED);
        }
        else
        {
            if (!invisible)
            {
                if (invisibleTimer <= diff)
                {
                    m_creature->SetVisibility(VISIBILITY_GROUP_STEALTH);
                    invisible = true;
                }
                else
                    invisibleTimer -= diff;
            }
        }
    }

    void JustDied(Unit* /*killer*/)
    {
        if (m_creature->IsTemporarySummon())
            ((TemporarySummon*)m_creature)->UnSummon();
    }
};

CreatureAI* GetAI_druid_wild_mushroom(Creature* pCreature)
{
    return new npc_druid_wild_mushroomAI(pCreature);
}

enum
{
    SPELL_FUNGAL_GROWTH_VISUAL      = 94339,
    SPELL_FUNGAL_GROWTH_SLOW_RANK1  = 81289,
    SPELL_FUNGAL_GROWTH_SLOW_RANK2  = 81282,
};

struct MANGOS_DLL_DECL npc_druid_fungal_growthAI : public ScriptedAI
{

    bool init;
    uint32 spell_fungal_slow;

    npc_druid_fungal_growthAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        init = false;
        spell_fungal_slow = (m_creature->GetEntry() == 43497 ? SPELL_FUNGAL_GROWTH_SLOW_RANK1 : SPELL_FUNGAL_GROWTH_SLOW_RANK2);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!init)
        {
            init = true;

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            m_creature->StopMoving();
            m_creature->addUnitState(UNIT_STAT_STUNNED);

            m_creature->CastSpell(m_creature, SPELL_FUNGAL_GROWTH_VISUAL, true);
            m_creature->CastSpell(m_creature, spell_fungal_slow, true);
        }
    }
};

CreatureAI* GetAI_druid_fungal_growth(Creature* pCreature)
{
    return new npc_druid_fungal_growthAI(pCreature);
}

void AddSC_npc_druid_summons()
{
    Script *pNewScript;
    pNewScript = new Script;
    pNewScript->Name="npc_druid_wild_mushroom";
    pNewScript->GetAI = &GetAI_druid_wild_mushroom;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="npc_druid_fungal_growth";
    pNewScript->GetAI = &GetAI_druid_fungal_growth;
    pNewScript->RegisterSelf();
}