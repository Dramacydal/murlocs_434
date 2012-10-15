/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
SDName: Thunder_Bluff
SD%Complete: 100
SDComment: Quest support: 925
SDCategory: Thunder Bluff
EndScriptData */

#include "precompiled.h"

/*#####
# npc_cairne_bloodhoof
######*/

enum
{
    SPELL_BERSERKER_CHARGE  = 16636,
    SPELL_CLEAVE            = 16044,
    SPELL_MORTAL_STRIKE     = 16856,
    SPELL_THUNDERCLAP       = 23931,
    SPELL_UPPERCUT          = 22916,
    SPELL_WAR_STOMP         = 59705,
};

//TODO: verify abilities/timers
struct MANGOS_DLL_DECL npc_cairne_bloodhoofAI : public ScriptedAI
{
    npc_cairne_bloodhoofAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiBerserkerCharge_Timer;
    uint32 m_uiCleave_Timer;
    uint32 m_uiMortalStrike_Timer;
    uint32 m_uiThunderclap_Timer;
    uint32 m_uiUppercut_Timer;
    uint32 m_uiWarStompTimer;

    void Reset()
    {
        m_uiBerserkerCharge_Timer = 30000;
        m_uiCleave_Timer = 5000;
        m_uiMortalStrike_Timer = 10000;
        m_uiThunderclap_Timer = 15000;
        m_uiUppercut_Timer = 10000;
        m_uiWarStompTimer  = 20000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBerserkerCharge_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
            {
                if (DoCastSpellIfCan(pTarget,SPELL_BERSERKER_CHARGE) == CAST_OK)
                    m_uiBerserkerCharge_Timer = 25000;
            }
        }
        else
            m_uiBerserkerCharge_Timer -= uiDiff;

        if (m_uiUppercut_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(),SPELL_UPPERCUT) == CAST_OK)
                m_uiUppercut_Timer = 20000;
        }
        else
            m_uiUppercut_Timer -= uiDiff;

        if (m_uiThunderclap_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(),SPELL_THUNDERCLAP) == CAST_OK)
                m_uiThunderclap_Timer = 15000;
        }
        else
            m_uiThunderclap_Timer -= uiDiff;

        if (m_uiMortalStrike_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(),SPELL_MORTAL_STRIKE) == CAST_OK)
                m_uiMortalStrike_Timer = 15000;
        }
        else
            m_uiMortalStrike_Timer -= uiDiff;

        if (m_uiCleave_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(),SPELL_CLEAVE) == CAST_OK)
                m_uiCleave_Timer = 7000;
        }
        else
            m_uiCleave_Timer -= uiDiff;

        if (m_uiWarStompTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WAR_STOMP) == CAST_OK)
                m_uiWarStompTimer = urand(20000, 25000);
        }
        else
            m_uiWarStompTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_cairne_bloodhoof(Creature* pCreature)
{
    return new npc_cairne_bloodhoofAI(pCreature);
}

bool GossipHello_npc_cairne_bloodhoof(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(925) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I know this is rather silly but a young ward who is a bit shy would like your hoofprint.", GOSSIP_SENDER_MAIN, GOSSIP_SENDER_INFO);

    pPlayer->SEND_GOSSIP_MENU(7013, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_cairne_bloodhoof(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_SENDER_INFO)
    {
        pPlayer->CastSpell(pPlayer, 23123, false);
        pPlayer->SEND_GOSSIP_MENU(7014, pCreature->GetObjectGuid());
    }
    return true;
}

void AddSC_thunder_bluff()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_cairne_bloodhoof";
    pNewScript->GetAI = &GetAI_npc_cairne_bloodhoof;
    pNewScript->pGossipHello = &GossipHello_npc_cairne_bloodhoof;
    pNewScript->pGossipSelect = &GossipSelect_npc_cairne_bloodhoof;
    pNewScript->RegisterSelf();
}
