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
SDName: Orgrimmar
SD%Complete: 100
SDComment: Quest support: 2460, 6566
SDCategory: Orgrimmar
EndScriptData */

/* ContentData
npc_shenthul
npc_thrall_warchief
boss_voljin
EndContentData */

#include "precompiled.h"

/*######
## npc_shenthul
######*/

enum
{
    QUEST_SHATTERED_SALUTE  = 2460
};

struct MANGOS_DLL_DECL npc_shenthulAI : public ScriptedAI
{
    npc_shenthulAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool CanTalk;
    bool CanEmote;
    uint32 Salute_Timer;
    uint32 Reset_Timer;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        CanTalk = false;
        CanEmote = false;
        Salute_Timer = 6000;
        Reset_Timer = 0;
        m_playerGuid.Clear();
    }

    void ReceiveEmote(Player* pPlayer, uint32 emote)
    {
        if (emote == TEXTEMOTE_SALUTE && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
        {
            if (CanEmote)
            {
                pPlayer->AreaExploredOrEventHappens(QUEST_SHATTERED_SALUTE);
                Reset();
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (CanEmote)
        {
            if (Reset_Timer < diff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    if (pPlayer->GetTypeId() == TYPEID_PLAYER && pPlayer->GetQuestStatus(QUEST_SHATTERED_SALUTE) == QUEST_STATUS_INCOMPLETE)
                        pPlayer->FailQuest(QUEST_SHATTERED_SALUTE);
                }
                Reset();
            } else Reset_Timer -= diff;
        }

        if (CanTalk && !CanEmote)
        {
            if (Salute_Timer < diff)
            {
                m_creature->HandleEmote(EMOTE_ONESHOT_SALUTE);
                CanEmote = true;
                Reset_Timer = 60000;
            } else Salute_Timer -= diff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_shenthul(Creature* pCreature)
{
    return new npc_shenthulAI(pCreature);
}

bool QuestAccept_npc_shenthul(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SHATTERED_SALUTE)
    {
        if (npc_shenthulAI* pShenAI = dynamic_cast<npc_shenthulAI*>(pCreature->AI()))
        {
            pShenAI->CanTalk = true;
            pShenAI->m_playerGuid = pPlayer->GetObjectGuid();
        }
    }
    return true;
}

/*######
## npc_thrall_warchief
######*/

#define QUEST_6566              6566

#define SPELL_CHAIN_LIGHTNING   16033
#define SPELL_SHOCK             16034

//TODO: verify abilities/timers
struct MANGOS_DLL_DECL npc_thrall_warchiefAI : public ScriptedAI
{
    npc_thrall_warchiefAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 ChainLightning_Timer;
    uint32 Shock_Timer;

    void Reset()
    {
        ChainLightning_Timer = 2000;
        Shock_Timer = 8000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (ChainLightning_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_CHAIN_LIGHTNING);
            ChainLightning_Timer = 9000;
        }else ChainLightning_Timer -= diff;

        if (Shock_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_SHOCK);
            Shock_Timer = 15000;
        }else Shock_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_thrall_warchief(Creature* pCreature)
{
    return new npc_thrall_warchiefAI(pCreature);
}

bool GossipHello_npc_thrall_warchief(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_6566) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Please share your wisdom with me, Warchief.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_thrall_warchief(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "What discoveries?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(5733, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Usurper?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->SEND_GOSSIP_MENU(5734, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "With all due respect, Warchief - why not allow them to be destroyed? Does this not strengthen our position?", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->SEND_GOSSIP_MENU(5735, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I... I did not think of it that way, Warchief.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(5736, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "I live only to serve, Warchief! My life is empty and meaningless without your guidance.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(5737, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Of course, Warchief!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
            pPlayer->SEND_GOSSIP_MENU(5738, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->AreaExploredOrEventHappens(QUEST_6566);
            break;
    }
    return true;
}

/*######
## boss_voljin
######*/

enum
{
    SPELL_HEX                       = 16097,
    SPELL_SHADOW_SHOCK              = 17289,
    SPELL_SHADOW_WORD_PAIN          = 17146,
    SPELL_SHOOT_VOLJIN              = 20463,
    SPELL_VEIL_OF_SHADOW            = 17820,
};

struct MANGOS_DLL_DECL boss_voljinAI : public ScriptedAI
{
    boss_voljinAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiHexTimer;
    uint32 m_uiShadowShockTimer;
    uint32 m_uiShadowWordPainTimer;
    uint32 m_uiShootTimer;
    uint32 m_uiVeilOfShadowTimer;

    void Reset()
    {
        m_uiHexTimer            = 20000;
        m_uiShadowShockTimer    = 12000;
        m_uiShadowWordPainTimer = 8000;
        m_uiShootTimer          = 6000;
        m_uiVeilOfShadowTimer   = 15000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHexTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_HEX);
            m_uiHexTimer = urand(15000, 20000);
        }
        else
            m_uiHexTimer -= uiDiff;

        if (m_uiShadowShockTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_SHADOW_SHOCK);
            m_uiShadowShockTimer = urand(11000, 14000);
        }
        else
            m_uiShadowShockTimer -= uiDiff;

        if (m_uiShadowWordPainTimer < uiDiff)
        {
           Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                DoCast(pTarget, SPELL_SHADOW_WORD_PAIN);
            m_uiShadowWordPainTimer = 8000;
        }
        else
            m_uiShadowWordPainTimer -= uiDiff;

        if (m_uiShootTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                DoCast(pTarget, SPELL_SHOOT_VOLJIN);
            m_uiShootTimer = urand(6000, 9000);
        }
        else
            m_uiShootTimer -= uiDiff;

        if (m_uiVeilOfShadowTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_VEIL_OF_SHADOW);
            m_uiVeilOfShadowTimer = urand(15000, 17000);
        }
        else
            m_uiVeilOfShadowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_voljin(Creature* pCreature)
{
    return new boss_voljinAI(pCreature);
}
void AddSC_orgrimmar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_shenthul";
    pNewScript->GetAI = &GetAI_npc_shenthul;
    pNewScript->pQuestAcceptNPC =  &QuestAccept_npc_shenthul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_thrall_warchief";
    pNewScript->GetAI = &GetAI_npc_thrall_warchief;
    pNewScript->pGossipHello =  &GossipHello_npc_thrall_warchief;
    pNewScript->pGossipSelect = &GossipSelect_npc_thrall_warchief;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_voljin";
    pNewScript->GetAI = &GetAI_boss_voljin;
    pNewScript->RegisterSelf();
}
