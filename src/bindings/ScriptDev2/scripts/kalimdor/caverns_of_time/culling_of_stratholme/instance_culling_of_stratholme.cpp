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
SDName: instance_culling_of_stratholme
SD%Complete: %
SDComment:
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

struct MANGOS_DLL_DECL instance_culling_of_stratholme : public ScriptedInstance
{
    instance_culling_of_stratholme(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint8 m_uiCratesCount;
    uint32 m_auiEncounter[7];
    uint32 m_uiHeroicTimer;
    uint32 m_uiLastTimer;

    ObjectGuid m_uiMalChestGUID;
    ObjectGuid m_uiExitGUID;

    void Initialize()
    {
        m_uiHeroicTimer = 1500000;
        m_uiLastTimer = 1500000;
        m_auiEncounter[0] = NOT_STARTED;
        m_auiEncounter[1] = NOT_STARTED;
        m_auiEncounter[2] = 0;
        m_auiEncounter[3] = NOT_STARTED;
        m_auiEncounter[4] = 0;
        m_auiEncounter[5] = NOT_STARTED;
        m_auiEncounter[6] = NOT_STARTED;

        DoUpdateWorldState(WORLD_STATE_COS_CRATE_COUNT, 0);
        DoUpdateWorldState(WORLD_STATE_COS_CRATE_ON, 0);
        DoUpdateWorldState(WORLD_STATE_COS_WAVE_COUNT, 0);
        DoUpdateWorldState(WORLD_STATE_COS_TIME_COUNT, 0);
        DoUpdateWorldState(WORLD_STATE_COS_TIME_ON, 0);

        m_uiCratesCount = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_CHROMI01:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_CHROMI02:
                         pCreature->SetActiveObjectState(true);
                         if (m_auiEncounter[0] == DONE)
                            pCreature->SetVisibility(VISIBILITY_ON);
                         else
                            pCreature->SetVisibility(VISIBILITY_OFF);
                         break;
            case NPC_MAL_CORICS:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_GRIAN_STONE:
                         pCreature->SetActiveObjectState(true);
                         pCreature->SetStandState(UNIT_STAND_STATE_SIT_MEDIUM_CHAIR);
                         break;
            case NPC_JAMES:
                         pCreature->SetActiveObjectState(true);
                         pCreature->SetStandState(UNIT_STAND_STATE_SIT_MEDIUM_CHAIR);
                         break;
            case NPC_FRAS_FRASIABI:
                         pCreature->SetActiveObjectState(true);
                         pCreature->SetStandState(UNIT_STAND_STATE_SIT_MEDIUM_CHAIR);
                         break;
            case NPC_FORRESTER:
                         pCreature->SetActiveObjectState(true);
                         pCreature->SetStandState(UNIT_STAND_STATE_SIT_MEDIUM_CHAIR);
                         break;
            case NPC_PERELLI:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_MARTHA:
                         pCreature->CastSpell(pCreature, 58925, false);
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_DOG:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_JAINA:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_UTHER:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_MARINE_1:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_MARINE_2:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_MARINE_3:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_MARINE_4:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_PRIEST_1:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_PRIEST_2:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_MALGANIS:
                         pCreature->SetActiveObjectState(true);
                         break;
            case NPC_INFINITE_CORRUPTOR:
                         pCreature->SetPhaseMask(0, true);
                         break;
        }
        m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
    }

    void OnObjectCreate(GameObject* pGo)
    {
         switch(pGo->GetEntry())
         {
             case GO_MALGANIS_GATE1:
             case GO_MALGANIS_GATE2:
             case GO_MALGANIS_CHEST:
             case GO_MALGANIS_CHEST_H:
                 break;
             case GO_EXIT:
                 if (m_auiEncounter[TYPE_MALGANIS] == DONE)
                     pGo->SetGoState(GO_STATE_ACTIVE);
                 break;
         }
         m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
    }

    void ChromiWhispers()
    {
       Map::PlayerList const &PlayerList = instance->GetPlayers();
       if (PlayerList.isEmpty())
           return;

       if (Creature* pChromi = GetSingleCreatureFromStorage(NPC_CHROMI01))
       {
           for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
           {
                pChromi->MonsterWhisper("Good work with crates! Come to me in front of Stratholme for your next assignment!", i->getSource(), false);
                i->getSource()->KilledMonsterCredit(NPC_CRATE_KC_BUNNY, pChromi->GetObjectGuid());
                i->getSource()->DestroyItemCount(ITEM_ARCANE_DISRUPTOR, 1, true);
            }
            pChromi->SetVisibility(VISIBILITY_OFF);
        }
        if (Creature* pChromi2 = GetSingleCreatureFromStorage(NPC_CHROMI02))
            pChromi2->SetVisibility(VISIBILITY_ON);
    }

    void KillMalginasCredit()
    {
       Map::PlayerList const &PlayerList = instance->GetPlayers();
       if (PlayerList.isEmpty())
           return;

       if (Creature* pMalganis = GetSingleCreatureFromStorage(NPC_MALGANIS))
       {
           for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
           {
                 pMalganis->SummonCreature(NPC_CHROMI03, 2311.61f, 1497.85f, 128.01f, 4.14f, TEMPSUMMON_TIMED_DESPAWN, 1800000);
                 i->getSource()->KilledMonsterCredit(MALGANIS_KC_BUNNY, pMalganis->GetObjectGuid());
            }
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_QUEST:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_CRATES_COUNT:
                m_uiCratesCount = m_uiCratesCount + uiData;
                if(m_uiCratesCount == 5)
                {
                   m_auiEncounter[0] = DONE;
                   ChromiWhispers();
                }
                DoUpdateWorldState(WORLD_STATE_COS_CRATE_COUNT, m_uiCratesCount);
                break;
            case TYPE_INTRO:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_PHASE:
                m_auiEncounter[2] = uiData;
                break;
            case TYPE_ENCOUNTER:
                m_auiEncounter[3] = uiData;
                break;
            case TYPE_WING:
                m_auiEncounter[4] = uiData;
                break;
            case TYPE_BONUS:
                m_auiEncounter[5] = uiData;
                if(uiData == IN_PROGRESS)
                {
                  if (Creature* pCorruptor = GetSingleCreatureFromStorage(NPC_INFINITE_CORRUPTOR))
                      pCorruptor->SetPhaseMask(1, true);
                  DoUpdateWorldState(WORLD_STATE_COS_TIME_ON, 1);
                  DoUpdateWorldState(WORLD_STATE_COS_TIME_COUNT, 25);
                }
                break;
            case TYPE_MALGANIS:
                m_auiEncounter[6] = uiData;
                if (uiData == DONE)
                {
                    KillMalginasCredit();
                    DoUseDoorOrButton(GO_EXIT);
                    DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_MALGANIS_CHEST : GO_MALGANIS_CHEST_H, 30*MINUTE);
                    if (GameObject* pChest = GetSingleGameObjectFromStorage(instance->IsRegularDifficulty() ? GO_MALGANIS_CHEST : GO_MALGANIS_CHEST_H))
                        pChest->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

                    if (Creature* pChromi2 = GetSingleCreatureFromStorage(NPC_CHROMI02))
                        pChromi2->SetVisibility(VISIBILITY_OFF);
                }
                break;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_QUEST:
                return m_auiEncounter[0];
            case TYPE_INTRO:
                return m_auiEncounter[1];
            case TYPE_PHASE:
                return m_auiEncounter[2];
            case TYPE_ENCOUNTER:
                return m_auiEncounter[3];
            case TYPE_WING:
                return m_auiEncounter[4];
            case TYPE_BONUS:
                return m_auiEncounter[5];
            case TYPE_MALGANIS:
                return m_auiEncounter[6];
        }
        return 0;
    }

    void Update(uint32 uiDiff)
    {
       if(m_auiEncounter[5] == IN_PROGRESS)
       {
         if(m_uiHeroicTimer < uiDiff)
         {
             m_auiEncounter[5] = FAIL;
             DoUpdateWorldState(WORLD_STATE_COS_TIME_ON, 0);
             if (Creature* pCorruptor = GetSingleCreatureFromStorage(NPC_INFINITE_CORRUPTOR))
               pCorruptor->SetPhaseMask(0, true);

         }else m_uiHeroicTimer -= uiDiff;

         if(m_uiHeroicTimer < m_uiLastTimer - 60000)
         {
            m_uiLastTimer = m_uiHeroicTimer;
            uint32 tMinutes = m_uiHeroicTimer / 60000;
            DoUpdateWorldState(WORLD_STATE_COS_TIME_COUNT, tMinutes);
         }
       }
       return;
    }
};

InstanceData* GetInstanceData_instance_culling_of_stratholme(Map* pMap)
{
    return new instance_culling_of_stratholme(pMap);
}

void AddSC_instance_culling_of_stratholme()
{
    Script *pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_culling_of_stratholme";
    pNewScript->GetInstanceData = &GetInstanceData_instance_culling_of_stratholme;
    pNewScript->RegisterSelf();
}
