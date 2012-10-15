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
SDName: Trial Of the Champion
SD%Complete: 80%
SDComment: event script. modified by /dev/rsa
SDCategory: trial_of_the_champion
EndScriptData */

/* ContentData
npc_toc5_announcer
EndContentData */

#include "precompiled.h"
#include "trial_of_the_champion.h"

#define GOSSIP_START_EVENT     "Im ready for the next challenge."

/*######
## npc_toc5_announcer
######*/
struct _Messages
{
    char const* name;
    uint32 id;
    bool state;
    uint32 encounter;
};

static _Messages _GossipMessage[]=
{
    {GOSSIP_START_EVENT,GOSSIP_ACTION_INFO_DEF+1,false,TYPE_GRAND_CHAMPIONS},
    {GOSSIP_START_EVENT,GOSSIP_ACTION_INFO_DEF+2,false,TYPE_ARGENT_CHALLENGE},
    {GOSSIP_START_EVENT,GOSSIP_ACTION_INFO_DEF+3,false,TYPE_BLACK_KNIGHT},
    {GOSSIP_START_EVENT,GOSSIP_ACTION_INFO_DEF+4,true,TYPE_BLACK_KNIGHT},
};

struct MANGOS_DLL_DECL npc_toc5_announcerAI : public ScriptedAI
{
    npc_toc5_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;
    }
};

CreatureAI* GetAI_npc_toc5_announcer(Creature* pCreature)
{
    return new npc_toc5_announcerAI(pCreature);
}

bool GossipHello_npc_toc5_announcer(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* m_pInstance;
    m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (!m_pInstance)
        return false;

    if (pPlayer->isInCombat()) 
        return true;

    for(uint8 i = 0; i < MAX_ENCOUNTER+1; i++)
    {
        if (!_GossipMessage[i].state && (m_pInstance->GetData(_GossipMessage[i].encounter) == NOT_STARTED || m_pInstance->GetData(_GossipMessage[i].encounter) == IN_PROGRESS))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _GossipMessage[i].name, GOSSIP_SENDER_MAIN,_GossipMessage[i].id);
            break;
        }
        if (_GossipMessage[i].state && m_pInstance->GetData(_GossipMessage[i].encounter) == DONE)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _GossipMessage[i].name, GOSSIP_SENDER_MAIN,_GossipMessage[i].id);
    }
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_toc5_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* m_pInstance;
    m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    pPlayer->CLOSE_GOSSIP_MENU();

    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
        {
            if (m_pInstance->GetData(DATA_TOC5_ANNOUNCER) == 0)
            {
                m_pInstance->SetData(DATA_TOC5_ANNOUNCER, pCreature->GetEntry());
                if (m_pInstance->GetData(DATA_TOC5_ANNOUNCER) == NPC_ARELAS)
                {
                    m_pInstance->SetData(DATA_BLACK_KNIGHT_MINION, 35564);
                    switch(urand(0, 4))
                    {
                        case 0: m_pInstance->SetData(DATA_CHAMPIONID_1, 35572); break;
                        case 1: m_pInstance->SetData(DATA_CHAMPIONID_1, 35569); break;
                        case 2: m_pInstance->SetData(DATA_CHAMPIONID_1, 35571); break;
                        case 3: m_pInstance->SetData(DATA_CHAMPIONID_1, 35570); break;
                        case 4: m_pInstance->SetData(DATA_CHAMPIONID_1, 35617); break;
                    }
                    do {
                        switch(urand(0, 4))
                        {
                            case 0: m_pInstance->SetData(DATA_CHAMPIONID_2, 35572); break;
                            case 1: m_pInstance->SetData(DATA_CHAMPIONID_2, 35569); break;
                            case 2: m_pInstance->SetData(DATA_CHAMPIONID_2, 35571); break;
                            case 3: m_pInstance->SetData(DATA_CHAMPIONID_2, 35570); break;
                            case 4: m_pInstance->SetData(DATA_CHAMPIONID_2, 35617); break;
                        }
                    } while(m_pInstance->GetData(DATA_CHAMPIONID_1) == m_pInstance->GetData(DATA_CHAMPIONID_2));
                    do{
                        switch(urand(0, 4))
                        {
                            case 0: m_pInstance->SetData(DATA_CHAMPIONID_3, 35572); break;
                            case 1: m_pInstance->SetData(DATA_CHAMPIONID_3, 35569); break;
                            case 2: m_pInstance->SetData(DATA_CHAMPIONID_3, 35571); break;
                            case 3: m_pInstance->SetData(DATA_CHAMPIONID_3, 35570); break;
                            case 4: m_pInstance->SetData(DATA_CHAMPIONID_3, 35617); break;
                        }
                    } while(m_pInstance->GetData(DATA_CHAMPIONID_1) == m_pInstance->GetData(DATA_CHAMPIONID_3) || m_pInstance->GetData(DATA_CHAMPIONID_2) == m_pInstance->GetData(DATA_CHAMPIONID_3));
                }

                if (m_pInstance->GetData(DATA_TOC5_ANNOUNCER) == NPC_JAEREN)
                {
                    m_pInstance->SetData(DATA_BLACK_KNIGHT_MINION, 35545);

                    switch(urand(0, 4))
                    {
                        case 0: m_pInstance->SetData(DATA_CHAMPIONID_1, 34705); break;
                        case 1: m_pInstance->SetData(DATA_CHAMPIONID_1, 34702); break;
                        case 2: m_pInstance->SetData(DATA_CHAMPIONID_1, 34701); break;
                        case 3: m_pInstance->SetData(DATA_CHAMPIONID_1, 34657); break;
                        case 4: m_pInstance->SetData(DATA_CHAMPIONID_1, 34703); break;
                    }
                    do{
                        switch(urand(0, 4))
                        {
                            case 0: m_pInstance->SetData(DATA_CHAMPIONID_2, 34705); break;
                            case 1: m_pInstance->SetData(DATA_CHAMPIONID_2, 34702); break;
                            case 2: m_pInstance->SetData(DATA_CHAMPIONID_2, 34701); break;
                            case 3: m_pInstance->SetData(DATA_CHAMPIONID_2, 34657); break;
                            case 4: m_pInstance->SetData(DATA_CHAMPIONID_2, 34703); break;
                        }
                    } while(m_pInstance->GetData(DATA_CHAMPIONID_1) == m_pInstance->GetData(DATA_CHAMPIONID_2));
                    do{
                        switch(urand(0, 4))
                        {
                            case 0: m_pInstance->SetData(DATA_CHAMPIONID_3, 34705); break;
                            case 1: m_pInstance->SetData(DATA_CHAMPIONID_3, 34702); break;
                            case 2: m_pInstance->SetData(DATA_CHAMPIONID_3, 34701); break;
                            case 3: m_pInstance->SetData(DATA_CHAMPIONID_3, 34657); break;
                            case 4: m_pInstance->SetData(DATA_CHAMPIONID_3, 34703); break;
                        }
                    } while(m_pInstance->GetData(DATA_CHAMPIONID_1) == m_pInstance->GetData(DATA_CHAMPIONID_3) || m_pInstance->GetData(DATA_CHAMPIONID_2) == m_pInstance->GetData(DATA_CHAMPIONID_3));
                }

                switch(urand(0, 1))
                {
                    case 0: m_pInstance->SetData(DATA_ARGENT_CHALLENGER, 35119); break;
                    case 1: m_pInstance->SetData(DATA_ARGENT_CHALLENGER, 34928); break;
                }
            }

            if (m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == NOT_STARTED || m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == IN_PROGRESS)
            {
                m_pInstance->SetData(DATA_CHAMPIONS_COUNT, 3);
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                    pTemp->Respawn();
                else
                    pCreature->SummonCreature(m_pInstance->GetData(DATA_CHAMPIONID_1), 738.665771f, 661.031433f, 412.394623f, 4.698702f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);

                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                    pTemp->Respawn();
                else
                    pCreature->SummonCreature(m_pInstance->GetData(DATA_CHAMPIONID_2), 746.864441f, 660.918762f, 411.695465f, 4.698700f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);

                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                    pTemp->Respawn();
                else
                    pCreature->SummonCreature(m_pInstance->GetData(DATA_CHAMPIONID_3), 754.360779f, 660.816162f, 412.395996f, 4.698700f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
            }
        }
        case GOSSIP_ACTION_INFO_DEF+2:
        {
            if ((m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == NOT_STARTED || m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == IN_PROGRESS) && m_pInstance->GetData(TYPE_GRAND_CHAMPIONS) == DONE)
            {
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_ARGENT_CHALLENGER))
                    pTemp->Respawn();
                else
                    pCreature->SummonCreature(m_pInstance->GetData(DATA_ARGENT_CHALLENGER), 746.864441f, 660.918762f, 411.695465f, 4.698700f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                m_pInstance->SetData(TYPE_ARGENT_CHALLENGE, IN_PROGRESS);
            }
        }
        case GOSSIP_ACTION_INFO_DEF+3:
        {
            if ((m_pInstance->GetData(TYPE_BLACK_KNIGHT) == NOT_STARTED || m_pInstance->GetData(TYPE_BLACK_KNIGHT) == IN_PROGRESS) && m_pInstance->GetData(TYPE_ARGENT_CHALLENGE) == DONE)
            {
                pCreature->SummonCreature(NPC_BLACK_KNIGHT, 746.864441f, 660.918762f, 411.695465f, 4.698700f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                pCreature->DealDamage(pCreature, pCreature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_pInstance->SetData(TYPE_BLACK_KNIGHT, IN_PROGRESS);
            }
        }
        case GOSSIP_ACTION_INFO_DEF+4:
        {
            if (m_pInstance->GetData(TYPE_BLACK_KNIGHT) == DONE)
            {
                pCreature->DealDamage(pCreature, pCreature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }
    return true;
}

void AddSC_trial_of_the_champion()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "npc_toc5_announcer";
    NewScript->GetAI = &GetAI_npc_toc5_announcer;
    NewScript->pGossipHello = &GossipHello_npc_toc5_announcer;
    NewScript->pGossipSelect = &GossipSelect_npc_toc5_announcer;
    NewScript->RegisterSelf();
}
