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
SDName: Npc_Professions
SD%Complete: 80
SDComment: Provides learn/unlearn/relearn-options for professions. Not supported: Unlearn engineering, re-learn engineering, re-learn leatherworking.
SDCategory: NPCs
EndScriptData */

#include "precompiled.h"

enum
{
    SPELL_HAND_OF_GULDAN_VISUAL = 85526,
    SPELL_CURSE_OF_GULDAN       = 86000,
    SPELL_FOREBODING_ROOT_R1    = 93974,
    SPELL_FOREBODING_ROOT_R2    = 93987,
    SPELL_FOREBODING_STUN_R1    = 93975,
    SPELL_FOREBODING_STUN_R2    = 93986,
    STUN_TIMER                  = 6 * IN_MILLISECONDS,
    DESPAWN_TIMER               = 15 * IN_MILLISECONDS,

};

struct MANGOS_DLL_DECL npc_hand_of_guldanAI : public ScriptedAI
{
    npc_hand_of_guldanAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool init;
    uint32 rootSpell;
    uint32 stunSpell;
    int32 stunTimer;
    int32 despawnTimer;

    void Reset()
    {
        init = false;
        stunTimer = 0;
        rootSpell = 0;
        stunSpell = 0;
        despawnTimer = 0;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!init)
        {
            if (Unit* creator = m_creature->GetCreator())
            {
                init = true;
                m_creature->setFaction(creator->getFaction());

                despawnTimer = DESPAWN_TIMER;
                m_creature->CastSpell(m_creature, SPELL_HAND_OF_GULDAN_VISUAL, true);
                m_creature->CastSpell(m_creature, SPELL_CURSE_OF_GULDAN, true);

                if (creator->HasAura(89604))      // Aura of Foreboding (Rank 1)
                {
                    m_creature->CastSpell(m_creature, SPELL_FOREBODING_ROOT_R1, true);
                    stunSpell = SPELL_FOREBODING_STUN_R1;
                    stunTimer = STUN_TIMER;
                }
                else if (creator->HasAura(89605)) // Aura of Foreboding (Rank 2)
                {
                    m_creature->CastSpell(m_creature, SPELL_FOREBODING_ROOT_R2, true);
                    stunSpell = SPELL_FOREBODING_STUN_R2;
                    stunTimer = STUN_TIMER;
                }

            }
        }
        else
        {
            if (despawnTimer)
            {
                if (despawnTimer <= diff)
                {
                    despawnTimer = 0;
                    m_creature->ForcedDespawn();
                }
                else
                    despawnTimer -= diff;
            }

            if (stunTimer)
            {
                if (stunTimer <= diff)
                {
                    stunTimer = 0;
                    m_creature->CastSpell(m_creature, stunSpell, true);
                }
                else
                    stunTimer -= diff;
            }
        }
    }
};

CreatureAI* GetAI_npc_hand_of_guldan(Creature* pCreature)
{
    return new npc_hand_of_guldanAI(pCreature);
}

void AddSC_npc_warlock_summons()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_hand_of_guldan";
    pNewScript->GetAI = &GetAI_npc_hand_of_guldan;
    pNewScript->RegisterSelf();
}
