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
SDName: Eversong_Woods
SD%Complete: 100
SDComment: Quest support: 8483, 8488, 8490, 9686
SDCategory: Eversong Woods
EndScriptData */

/* ContentData
npc_kelerun_bloodmourn
go_harbinger_second_trial
npc_prospector_anvilward
npc_apprentice_mirveda
npc_infused_crystal
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "TemporarySummon.h"

/*######
## npc_kelerun_bloodmourn
######*/

enum
{
    NPC_KELERUN        = 17807,
    NPC_BLOODWRATH     = 17809,
    NPC_LIGHTREND      = 17810,
    NPC_SWIFTBLADE     = 17811,
    NPC_SUNSTRIKER     = 17812,

    GO_SECOND_TRIAL    = 182052,
    QUEST_SECOND_TRIAL = 9686,
    MAX_CHALLENGER     = 4
};

const uint32 uiChallengerId[4] = {NPC_BLOODWRATH, NPC_LIGHTREND, NPC_SWIFTBLADE, NPC_SUNSTRIKER};

const int32 uiSayId[4] =
{
    -1000319,
    -1000320,
    -1000321,
    -1000322
};

float fChallengerLoc[4][4]=
{
    {10110.667f, -6628.059f, 4.100f, 2.708f},
    {10093.919f, -6634.340f, 4.098f, 1.106f},
    {10087.565f, -6617.282f, 4.098f, 5.887f},
    {10104.807f, -6611.145f, 4.101f, 4.265f}
};

struct MANGOS_DLL_DECL npc_kelerun_bloodmournAI : public ScriptedAI
{
    npc_kelerun_bloodmournAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiNpcFlags = pCreature->GetUInt32Value(UNIT_NPC_FLAGS);
        Reset();
    }

    uint32 m_uiNpcFlags;
    ObjectGuid m_playerGuid;
    ObjectGuid m_aChallengerGuids[MAX_CHALLENGER];

    uint8 m_uiChallengerCount;

    uint32 m_uiTimeOutTimer;
    uint32 m_uiCheckAliveStateTimer;
    uint32 m_uiEngageTimer;

    bool m_bIsEventInProgress;

    void Reset()
    {
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, m_uiNpcFlags);

        m_playerGuid.Clear();

        for (uint8 i = 0; i < MAX_CHALLENGER; ++i)
            m_aChallengerGuids[i].Clear();

        m_uiChallengerCount = 0;

        m_uiTimeOutTimer = 60000;
        m_uiCheckAliveStateTimer = 2500;
        m_uiEngageTimer = 0;

        m_bIsEventInProgress = false;
    }

    void StartEvent()
    {
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        m_bIsEventInProgress = true;
    }

    bool CanProgressEvent(Player* pPlayer)
    {
        if (m_bIsEventInProgress)
        {
            m_playerGuid = pPlayer->GetObjectGuid();
            DoSpawnChallengers();
            m_uiEngageTimer = 15000;

            return true;
        }

        return false;
    }

    void DoSpawnChallengers()
    {
        for(uint8 i = 0; i < MAX_CHALLENGER; ++i)
        {
            if (Creature* pCreature = m_creature->SummonCreature(uiChallengerId[i],
                fChallengerLoc[i][0], fChallengerLoc[i][1],
                fChallengerLoc[i][2], fChallengerLoc[i][3],
                TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 600000))
            {
                m_aChallengerGuids[i] = pCreature->GetObjectGuid();
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsEventInProgress)
        {
            if (m_uiTimeOutTimer && m_uiTimeOutTimer < uiDiff)
            {
                if (!m_playerGuid)
                {
                    //player are expected to use GO within a minute, if not, event will fail.
                    Reset();
                    return;
                }

                m_uiTimeOutTimer = 0;
            }
            else
                m_uiTimeOutTimer -= uiDiff;

            if (m_uiCheckAliveStateTimer < uiDiff)
            {
                if (Creature* pChallenger = m_creature->GetMap()->GetCreature(m_aChallengerGuids[m_uiChallengerCount]))
                {
                    if (!pChallenger->isAlive())
                    {
                        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

                        if (pPlayer && !pPlayer->isAlive())
                        {
                            Reset();
                            return;
                        }

                        ++m_uiChallengerCount;

                        //count starts at 0
                        if (m_uiChallengerCount == MAX_CHALLENGER)
                        {
                            if (pPlayer && pPlayer->isAlive())
                                pPlayer->GroupEventHappens(QUEST_SECOND_TRIAL, m_creature);

                            Reset();
                            return;
                        }
                        else
                            m_uiEngageTimer = 15000;
                    }
                }
                m_uiCheckAliveStateTimer = 2500;
            }
            else
                m_uiCheckAliveStateTimer -= uiDiff;

            if (m_uiEngageTimer && m_uiEngageTimer < uiDiff)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

                if (pPlayer && pPlayer->isAlive())
                {
                    if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_aChallengerGuids[m_uiChallengerCount]))
                    {
                        DoScriptText(uiSayId[m_uiChallengerCount], m_creature, pPlayer);
                        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pCreature->AI()->AttackStart(pPlayer);
                    }
                }

                m_uiEngageTimer = 0;
            }
            else
                m_uiEngageTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_kelerun_bloodmourn(Creature* pCreature)
{
    return new npc_kelerun_bloodmournAI(pCreature);
}

//easiest way is to expect database to respawn GO at quest accept (quest_start_script)
bool QuestAccept_npc_kelerun_bloodmourn(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_SECOND_TRIAL)
    {
        if (npc_kelerun_bloodmournAI* pKelrunAI = dynamic_cast<npc_kelerun_bloodmournAI*>(pCreature->AI()))
            pKelrunAI->StartEvent();
    }

    return true;
}

bool GOUse_go_harbinger_second_trial(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
    {
        if (Creature* pCreature = GetClosestCreatureWithEntry(pGO, NPC_KELERUN, 30.0f))
        {
            if (npc_kelerun_bloodmournAI* pKelrunAI = dynamic_cast<npc_kelerun_bloodmournAI*>(pCreature->AI()))
                pKelrunAI->CanProgressEvent(pPlayer);
        }
    }

    return false;
}

/*######
## npc_prospector_anvilward
######*/
enum
{
    SAY_ANVIL1            = -1000209,
    SAY_ANVIL2            = -1000210,

    FACTION_DEFAULT       = 35,
    FACTION_HOSTILE       = 24,

    QUEST_THE_DWARVEN_SPY = 8483
};

struct MANGOS_DLL_DECL npc_prospector_anvilwardAI : public npc_escortAI
{
    // CreatureAI functions
    npc_prospector_anvilwardAI(Creature* pCreature) : npc_escortAI(pCreature) {Reset();}

    // Pure Virtual Functions
    void WaypointReached(uint32 uiPointId)
    {
        Player* pPlayer = GetPlayerForEscort();

        if (!pPlayer)
            return;

        switch (uiPointId)
        {
            case 0:
                DoScriptText(SAY_ANVIL1, m_creature, pPlayer);
                break;
            case 5:
                DoScriptText(SAY_ANVIL2, m_creature, pPlayer);
                break;
            case 6:
                m_creature->setFaction(FACTION_HOSTILE);
                break;
        }
    }

    void Reset()
    {
        //Default npc faction
        m_creature->setFaction(FACTION_DEFAULT);
    }

    void JustDied(Unit* pKiller)
    {
        //Default npc faction
        m_creature->setFaction(FACTION_DEFAULT);
    }
};

CreatureAI* GetAI_npc_prospector_anvilward(Creature* pCreature)
{
    return new npc_prospector_anvilwardAI(pCreature);
}

#define GOSSIP_ITEM_MOMENT "I need a moment of your time, sir."
#define GOSSIP_ITEM_SHOW   "Why... yes, of course. I've something to show you right inside this building, Mr. Anvilward."

bool GossipHello_npc_prospector_anvilward(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_THE_DWARVEN_SPY) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_MOMENT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(8239, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_prospector_anvilward(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_SHOW, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->SEND_GOSSIP_MENU(8240, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();

            if (npc_prospector_anvilwardAI* pEscortAI = dynamic_cast<npc_prospector_anvilwardAI*>(pCreature->AI()))
                pEscortAI->Start(false, pPlayer);

            break;
    }
    return true;
}

/*######
## npc_apprentice_mirveda
######*/

enum
{
    QUEST_UNEXPECTED_RESULT = 8488,

    SPELL_FIREBALL          = 20811,

    NPC_GHARSUL             = 15958,
    NPC_ANGERSHADE          = 15656
};

struct MANGOS_DLL_DECL npc_apprentice_mirvedaAI : public ScriptedAI
{
    npc_apprentice_mirvedaAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint8 m_uiMobCount;
    uint32 m_uiFireballTimer;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        m_uiMobCount      = 0;
        m_playerGuid.Clear();
        m_uiFireballTimer = 0;
    }

    void JustDied(Unit* pKiller)
    {
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (pPlayer && pPlayer->GetQuestStatus(QUEST_UNEXPECTED_RESULT) == QUEST_STATUS_INCOMPLETE)
            pPlayer->SendQuestFailed(QUEST_UNEXPECTED_RESULT);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->AI()->AttackStart(m_creature);
        ++m_uiMobCount;
    }

    void SummonedCreatureJustDied(Creature* pKilled)
    {
        --m_uiMobCount;

        if (m_uiMobCount)
            return;

        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        if (pPlayer && pPlayer->GetQuestStatus(QUEST_UNEXPECTED_RESULT) == QUEST_STATUS_INCOMPLETE)
            pPlayer->GroupEventHappens(QUEST_UNEXPECTED_RESULT, m_creature);

        m_playerGuid.Clear();
        m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void StartEvent(Player* pPlayer)
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_playerGuid = pPlayer->GetObjectGuid();

        m_creature->SummonCreature(NPC_GHARSUL,    8745.0f, -7134.32f, 35.22f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 4000);
        m_creature->SummonCreature(NPC_ANGERSHADE, 8745.0f, -7134.32f, 35.22f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 4000);
        m_creature->SummonCreature(NPC_ANGERSHADE, 8745.0f, -7134.32f, 35.22f, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 4000);
    }

    void UpdateAI (const uint32 uiDiff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFireballTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIREBALL) == CAST_OK)
                m_uiFireballTimer = urand(4000, 6000);
        }
        else
            m_uiFireballTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

bool QuestAccept_unexpected_results(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_UNEXPECTED_RESULT)
        if (npc_apprentice_mirvedaAI* pMirvedaAI = dynamic_cast<npc_apprentice_mirvedaAI*>(pCreature->AI()))
            pMirvedaAI->StartEvent(pPlayer);
    return true;
}

CreatureAI* GetAI_npc_apprentice_mirvedaAI(Creature* pCreature)
{
    return new npc_apprentice_mirvedaAI (pCreature);
}

/*######
## npc_infused_crystal
######*/

enum
{
    NPC_ENRAGED_WRATH           = 17086,
    QUEST_POWERING_OUR_DEFENSES = 8490,
    INFUSED_CRYSTAL_EMOTE       = -1000668,
};

float fEnragedWrathPosition[3][4] =
{
    {8259.375977f, -7202.288574f, 139.287430f, 5.0f},
    {8255.425781f, -7222.026367f, 139.607162f, 5.0f},
    {8267.902344f, -7193.510742f, 139.430374f, 5.0f}
};

struct MANGOS_DLL_DECL npc_infused_crystalAI : public ScriptedAI
{
    npc_infused_crystalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }
    
    uint32 m_uiQuestTimer;
    uint32 m_uiSpawnTimer;
    ObjectGuid m_uiPlayerGUID;

    bool bCompleted;
            
    void Reset()
    {
        m_uiQuestTimer = 60000;
        m_uiSpawnTimer  = 1000;
        m_uiPlayerGUID.Clear();
        bCompleted = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;
        Player* pPlayer = (Player*)pWho;
            
        if (pPlayer->GetQuestStatus(QUEST_POWERING_OUR_DEFENSES) != QUEST_STATUS_INCOMPLETE)
            return;
            
        m_uiPlayerGUID = pPlayer->GetObjectGuid();
    }

    void Aggro(Unit* pWho){}

    void JustDied(Unit* pWho) 
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
            if (pPlayer->GetQuestStatus(QUEST_POWERING_OUR_DEFENSES) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_POWERING_OUR_DEFENSES);
    }

    void UpdateAI(const uint32 uiDiff)
    {    
        if (bCompleted) 
            return;

        if (m_uiSpawnTimer < uiDiff)
        {
            for (uint8 i = 0; i < 3; ++i)
            {
                if (Creature* pEnragedWrath = m_creature->SummonCreature(NPC_ENRAGED_WRATH, fEnragedWrathPosition[i][0], fEnragedWrathPosition[i][1], fEnragedWrathPosition[i][2], fEnragedWrathPosition[i][3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000))
                {
                    pEnragedWrath->AI()->AttackStart(m_creature);
                }
            }
            m_uiSpawnTimer = 40000;
        }
        else
            m_uiSpawnTimer -= uiDiff;

        if (m_uiQuestTimer < uiDiff)
        {
            if(Player* pPlayer = m_creature->GetMap()->GetPlayer(m_uiPlayerGUID))
            {
                pPlayer->KilledMonsterCredit(m_creature->GetEntry());
                DoScriptText(INFUSED_CRYSTAL_EMOTE , m_creature);
                m_creature->ForcedDespawn(5000);
            }
            bCompleted = true;          
        }
        else
            m_uiQuestTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_infused_crystalAI(Creature* pCreature)
{
    return new npc_infused_crystalAI (pCreature);
}

void AddSC_eversong_woods()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_kelerun_bloodmourn";
    pNewScript->GetAI = &GetAI_npc_kelerun_bloodmourn;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_kelerun_bloodmourn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_harbinger_second_trial";
    pNewScript->pGOUse = &GOUse_go_harbinger_second_trial;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_prospector_anvilward";
    pNewScript->GetAI = &GetAI_npc_prospector_anvilward;
    pNewScript->pGossipHello =  &GossipHello_npc_prospector_anvilward;
    pNewScript->pGossipSelect = &GossipSelect_npc_prospector_anvilward;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_apprentice_mirveda";
    pNewScript->GetAI = &GetAI_npc_apprentice_mirvedaAI;
    pNewScript->pQuestAcceptNPC = &QuestAccept_unexpected_results;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_infused_crystal";
    pNewScript->GetAI = &GetAI_npc_infused_crystalAI;
    pNewScript->RegisterSelf();
}
