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
SDName: Stranglethorn_Vale
SD%Complete: 100
SDComment: Quest support: 592, 8193
SDCategory: Stranglethorn Vale
EndScriptData */

/* ContentData
mob_yenniku
npc_riggle_bassbait
EndContentData */

#include "precompiled.h"
#include "GameEventMgr.h"

enum
{
    SAY_START               = -1510356,
    SAY_WINNER              = -1510357,
    SAY_END                 = -1510358,
    QUEST_MASTER_ANGLER     = 8193,
};

/*######
## mob_yenniku
######*/

struct MANGOS_DLL_DECL mob_yennikuAI : public ScriptedAI
{
    mob_yennikuAI(Creature *c) : ScriptedAI(c)
    {
        bReset = false;
        Reset();
    }

    uint32 Reset_Timer;
    bool bReset;

    void Reset()
    {
        Reset_Timer = 0;
        m_creature->HandleEmote(EMOTE_STATE_NONE);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER)
        {
                                                            //Yenniku's Release
            if (!bReset && ((Player*)caster)->GetQuestStatus(592) == QUEST_STATUS_INCOMPLETE && spell->Id == 3607)
            {
                m_creature->HandleEmote(EMOTE_STATE_STUN);
                m_creature->CombatStop();                   //stop combat
                m_creature->DeleteThreatList();             //unsure of this
                m_creature->setFaction(83);                 //horde generic

                bReset = true;
                Reset_Timer = 60000;
            }
        }
        return;
    }

    void Aggro(Unit *who) {}

    void UpdateAI(const uint32 diff)
    {
        if (bReset)
            if (Reset_Timer < diff)
        {
            EnterEvadeMode();
            bReset = false;
            m_creature->setFaction(28);                     //troll, bloodscalp
        }
        else Reset_Timer -= diff;

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_mob_yenniku(Creature *_Creature)
{
    return new mob_yennikuAI (_Creature);
}

/*######
##npc_riggle_bassbait
######*/
/**
 *  AI for Riggle Bassbait.
 *  This is the AI for Riggle Bassbait, see http://www.wowhead.com/?npc=15077
 *  @see ScriptedAI
 *  @author burned, gotisch
 */
struct MANGOS_DLL_DECL npc_riggle_bassbaitAI : public ScriptedAI
{
    /**
     *  Constructor of the Creature.
     *  This is called when the creature is spawned.
     *  @param c The Creature that this AI is for
     */
    npc_riggle_bassbaitAI(Creature *c) : ScriptedAI(c)
    {
        // This will keep the NPC active even if there are no players around!
        c->SetActiveObjectState(true);
        bEventAnnounced = bEventIsOver = bEventWinnerFound = false;
        Reset();
    }
    /**
     *  Flag to check if event was announced. True if event was announced.
     */
    bool bEventAnnounced;
    /**
     *  Flag to check if event is over. True if event is over.
     */
    bool bEventIsOver;
    /**
     *  Flag to check if someone won the event. True if someone has won.
     */
    bool bEventWinnerFound;

    void Reset() { }

    void Aggro(Unit *who) {}

    void UpdateAI(const uint32 diff)
    {
        // Announce the event max 1 minute after being spawned. But only if Fishing extravaganza is running.
        if (!bEventAnnounced && time(NULL) % 60 == 0 && IsHolidayActive(HOLIDAY_FISHING_EXTRAVAGANZA))
        {
            debug_log("SD2: npc_riggle_bassbait announce HOLIDAY_FISHING_EXTRAVAGANZA contest");
            DoScriptText(SAY_START, m_creature);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER); //Quest&Gossip are now active
            bEventAnnounced = true;
        }
        // The Event was started (announced) & It was not yet ended & One minute passed & the Fish are gone
        if ( bEventAnnounced && !bEventIsOver && time(NULL) % 60 == 0 && !IsHolidayActive(HOLIDAY_FISHING_EXTRAVAGANZA))
        {
            debug_log("SD2: npc_riggle_bassbait end HOLIDAY_FISHING_EXTRAVAGANZA contest");
            DoScriptText(SAY_END, m_creature);
            bEventIsOver = true;
        }
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};
/**
 * GossipHello for NPC Riggle Bassbait. 
 * This is called each time a Player tries to talk with the NPC.
 */
bool GossipHello_npc_riggle_bassbait(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver()) // If the quest is still running.
    {
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());
        pPlayer->SEND_GOSSIP_MENU(7614, pCreature->GetObjectGuid());
        return true;
    }
    // The Quest is not there anymore 
    // There is a winner!
    pPlayer->SEND_GOSSIP_MENU(7714, pCreature->GetObjectGuid());
    return true;
}

bool ChooseReward_npc_riggle_bassbait(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    // TODO: check if this can only be called if NPC has QUESTGIVER flag.
    if (pQuest->GetQuestId() == QUEST_MASTER_ANGLER && ((npc_riggle_bassbaitAI*)(pCreature->AI()))->bEventWinnerFound == false)
    {
        DoScriptText(SAY_WINNER, pCreature,pPlayer);
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        ((npc_riggle_bassbaitAI*)(pCreature->AI()))->bEventWinnerFound = true;
        Creature* creature2 = GetClosestCreatureWithEntry(pCreature,15087,60.0f);
        if (creature2)
        {
            creature2->SetFlag(UNIT_NPC_FLAGS,UNIT_NPC_FLAG_QUESTGIVER);
        } else {
            debug_log("Could not change flag of Jang");
        }
        return true;
    }
    return true;
}

CreatureAI* GetAI_npc_riggle_bassbait(Creature* pCreature)
{
    return new npc_riggle_bassbaitAI(pCreature);
}

void AddSC_stranglethorn_vale()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "mob_yenniku";
    pNewScript->GetAI = &GetAI_mob_yenniku;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_riggle_bassbait";
    pNewScript->GetAI = &GetAI_npc_riggle_bassbait;
    pNewScript->pGossipHello = &GossipHello_npc_riggle_bassbait;
    pNewScript->pQuestRewardedNPC = &ChooseReward_npc_riggle_bassbait;
    pNewScript->RegisterSelf();
}
