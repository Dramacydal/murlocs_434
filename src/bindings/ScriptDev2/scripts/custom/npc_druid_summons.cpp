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

enum
{
    WILD_MUSHROOM_INVISIBLE_TIMER  = 6 * IN_MILLISECONDS,
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
};

CreatureAI* GetAI_druid_wild_mushroom(Creature* pCreature)
{
    return new npc_druid_wild_mushroomAI(pCreature);
}

void AddSC_npc_druid_summons()
{
    Script *pNewScript;
    pNewScript = new Script;
    pNewScript->Name="npc_druid_wild_mushroom";
    pNewScript->GetAI = &GetAI_druid_wild_mushroom;
    pNewScript->RegisterSelf();
}
