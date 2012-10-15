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
SDName: Storm_Peaks
SD%Complete: ?
SDComment:
SDCategory: Storm Peaks
EndScriptData */

/* ContentData
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"
#include "follower_ai.h"

/////////////////////
///npc_injured_goblin
/////////////////////

enum eInjuredGoblin
{
    QUEST_BITTER_DEPARTURE     = 12832,
    SPELL_FEIGN_DEATH          = 51329,
    SAY_QUEST_ACCEPT           =  -1800000,
    SAY_END_WP_REACHED         =  -1800001
};

#define GOSSIP_ITEM_1       "I am ready, lets get you out of here"

struct MANGOS_DLL_DECL npc_injured_goblin : public npc_escortAI
{
    npc_injured_goblin(Creature* pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    void Reset() {}

    void WaypointReached(uint32 i)
    { 
        Player* pPlayer = GetPlayerForEscort();
        switch (i)
        {
        case 26:
            DoScriptText(SAY_END_WP_REACHED, m_creature, pPlayer);
            break;
        case 27:
            if (pPlayer)
                pPlayer->GroupEventHappens(QUEST_BITTER_DEPARTURE, m_creature);
            break;
        }
    }

    void EnterCombat(Unit* pWho) {}

    void JustDied(Unit* pKiller)
    {
        Player* pPlayer = GetPlayerForEscort();
        if (pPlayer && HasEscortState(STATE_ESCORT_ESCORTING))
            pPlayer->FailQuest(QUEST_BITTER_DEPARTURE);
    }

   void UpdateEscortAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_injured_goblin(Creature* pCreature)
{
    return new npc_injured_goblin(pCreature);
}

bool GossipHello_npc_injured_goblin(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_BITTER_DEPARTURE) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->PlayerTalkClass->SendGossipMenu(907, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(907, pCreature->GetObjectGuid());
    return true;
}

bool QuestAccept_npc_injured_goblin(Player* pPlayer, Creature* pCreature, Quest const *pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BITTER_DEPARTURE)
        DoScriptText(SAY_QUEST_ACCEPT, pCreature);

    return false;
}

bool GossipSelect_npc_injured_goblin(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (npc_escortAI* pEscortAI = dynamic_cast<npc_injured_goblin*>(pCreature->AI()))
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
        {
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
            pCreature->RemoveAurasDueToSpell(SPELL_FEIGN_DEATH);

            pEscortAI->Start(true, pPlayer);
            pCreature->SetFactionTemporary(FACTION_ESCORT_N_NEUTRAL_PASSIVE, TEMPFACTION_RESTORE_RESPAWN);
        }
    }
    return true;
}

/*######
## npc_goblin_prisoner
######*/

enum eGoblinPrisoner
{
    GO_RUSTY_CAGE = 191544
};

struct MANGOS_DLL_DECL npc_goblin_prisoner : public ScriptedAI
{
    npc_goblin_prisoner(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

        if (GameObject* pGO = GetClosestGameObjectWithEntry(m_creature, GO_RUSTY_CAGE, INTERACTION_DISTANCE))
        {
            if (pGO->GetGoState() == GO_STATE_ACTIVE)
                pGO->SetGoState(GO_STATE_READY);
        }
    }
};

CreatureAI* GetAI_npc_goblin_prisoner(Creature* pCreature)
{
    return new npc_goblin_prisoner(pCreature);
}

void AddSC_storm_peaks()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_goblin";
    pNewScript->GetAI = &GetAI_npc_injured_goblin;
    pNewScript->pGossipHello = &GossipHello_npc_injured_goblin;
    pNewScript->pGossipSelect = &GossipSelect_npc_injured_goblin;
    pNewScript->pQuestAcceptNPC =  &QuestAccept_npc_injured_goblin;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_goblin_prisoner";
    pNewScript->GetAI = &GetAI_npc_goblin_prisoner;
    pNewScript->RegisterSelf();
}