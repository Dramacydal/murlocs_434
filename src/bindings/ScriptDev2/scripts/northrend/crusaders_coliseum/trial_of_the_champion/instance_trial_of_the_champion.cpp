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
SDName: Instance_Trial_Of_the_Champion
SD%Complete: 70
SDComment: modified by /dev/rsa
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"
#include "World.h"

struct MANGOS_DLL_DECL instance_trial_of_the_champion : public ScriptedInstance
{
    instance_trial_of_the_champion(Map* pMap) : ScriptedInstance(pMap) 
    {
        m_bIsRegularMode = pMap->IsRegularDifficulty();
        Initialize(); 
    }

    uint32 m_auiEncounter[MAX_ENCOUNTER+1];

    std::string m_strInstData;

    uint32 m_uiChampionId1;
    uint32 m_uiChampionId2;
    uint32 m_uiChampionId3;
    uint32 m_uiChampionsCount;
    uint32 m_uiArgentChallengerID;
    uint32 m_uiBlackKnightMinionID;
    uint32 m_uiAnnouncerID;
    bool   m_bIsRegularMode;

    void Initialize()
    {
        m_uiBlackKnightMinionID    = 0;
        m_uiChampionId1            = 0;
        m_uiChampionId2            = 0;
        m_uiChampionId3            = 0;
        m_uiChampionsCount         = 3;
        m_uiArgentChallengerID     = 0;
        m_uiAnnouncerID            = 0;

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            m_auiEncounter[i] = NOT_STARTED;
    }

    void OnPlayerEnter(Player *pPlayer)
    {

        enum PhaseControl
        {
            HORDE_CONTROL_PHASE_SHIFT_1    = 55773,
            HORDE_CONTROL_PHASE_SHIFT_2    = 60028,
            ALLIANCE_CONTROL_PHASE_SHIFT_1 = 55774,
            ALLIANCE_CONTROL_PHASE_SHIFT_2 = 60027,
        };

        if (!sWorld.getConfig(CONFIG_BOOL_ALLOW_TWO_SIDE_INTERACTION_GROUP)) 
            return;

        switch (pPlayer->GetTeam())
        {
            case ALLIANCE:
                if (pPlayer && pPlayer->IsInWorld() && pPlayer->HasAura(HORDE_CONTROL_PHASE_SHIFT_1))
                    pPlayer->RemoveAurasDueToSpell(HORDE_CONTROL_PHASE_SHIFT_1);

                pPlayer->CastSpell(pPlayer, HORDE_CONTROL_PHASE_SHIFT_2, false);
                break;
            case HORDE:
                if (pPlayer && pPlayer->IsInWorld() && pPlayer->HasAura(ALLIANCE_CONTROL_PHASE_SHIFT_1)) 
                    pPlayer->RemoveAurasDueToSpell(ALLIANCE_CONTROL_PHASE_SHIFT_1);

                pPlayer->CastSpell(pPlayer, ALLIANCE_CONTROL_PHASE_SHIFT_2, false);
                break;
        }
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            // Champions of the Alliance
            case NPC_JACOB:
            case NPC_AMBROSE:
            case NPC_COLOSOS:
            case NPC_JAELYNE:
            case NPC_LANA:
            // Champions of the Horde
            case NPC_MOKRA:
            case NPC_ERESSEA:
            case NPC_RUNOK:
            case NPC_ZULTORE:
            case NPC_VISCERI:
            {
                if (m_uiChampionId1 == 0)
                    m_uiChampionId1 = pCreature->GetEntry();
                else if (m_uiChampionId2 == 0)
                    m_uiChampionId2 = pCreature->GetEntry();
                else if (m_uiChampionId3 == 0)
                    m_uiChampionId3 = pCreature->GetEntry();
                else
                    return;
                break;
            }
            // Argent Challenge
            case NPC_EADRIC:
            case NPC_PALETRESS:
                m_uiArgentChallengerID = pCreature->GetEntry();
                break;
            // Black Knight
            case NPC_BLACK_KNIGHT:
            case NPC_RISEN_JAEREN:
            case NPC_RISEN_ARELAS:
            // memories
            case MEMORY_ALGALON:
            case MEMORY_ARCHIMONDE:
            case MEMORY_CHROMAGGUS:
            case MEMORY_CYANIGOSA:
            case MEMORY_DELRISSA:
            case MEMORY_ECK:
            case MEMORY_ENTROPIUS:
            case MEMORY_GRUUL:
            case MEMORY_HAKKAR:
            case MEMORY_HEIGAN:
            case MEMORY_HEROD:
            case MEMORY_HOGGER:
            case MEMORY_IGNIS:
            case MEMORY_ILLIDAN:
            case MEMORY_INGVAR:
            case MEMORY_KALITHRESH:
            case MEMORY_LUCIFRON:
            case MEMORY_MALCHEZAAR:
            case MEMORY_MUTANUS:
            case MEMORY_ONYXIA:
            case MEMORY_THUNDERAAN:
            case MEMORY_VANCLEEF:
            case MEMORY_VASHJ:
            case MEMORY_VEKNILASH:
            case MEMORY_VEZAX:
                break;
            default:
                return;
        }
        m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
    }

    void OnObjectCreate(GameObject *pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_CHAMPIONS_LOOT:
            case GO_EADRIC_LOOT:
            case GO_PALETRESS_LOOT:
            case GO_CHAMPIONS_LOOT_H:
            case GO_EADRIC_LOOT_H:
            case GO_PALETRESS_LOOT_H:
                m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
                break;
            default:
                return;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case DATA_TOC5_ANNOUNCER:
                m_uiAnnouncerID = uiData;
                break;
            case DATA_CHAMPIONID_1:
                m_uiChampionId1 = uiData;
                break;
            case DATA_CHAMPIONID_2:
                m_uiChampionId2 = uiData;
                break;
            case DATA_CHAMPIONID_3:
                m_uiChampionId3 = uiData;
                break;
            case DATA_CHAMPIONS_COUNT:
                m_uiChampionsCount = uiData;
                break;
            case DATA_ARGENT_CHALLENGER:
                m_uiArgentChallengerID = uiData;
                break;
            case DATA_BLACK_KNIGHT_MINION:
                m_uiBlackKnightMinionID = uiData;
                break;
            case TYPE_GRAND_CHAMPIONS:
                m_auiEncounter[0] = uiData;
                if (uiData == DONE)
                {
                    if (GameObject* pChest = GetSingleGameObjectFromStorage(m_bIsRegularMode ? GO_CHAMPIONS_LOOT : GO_CHAMPIONS_LOOT_H))
                        if (pChest && !pChest->isSpawned())
                            pChest->SetRespawnTime(DAY);
                }
                if (uiData == FAIL)
                {
                    m_auiEncounter[0] = NOT_STARTED;
                }
                break;
            case TYPE_ARGENT_CHALLENGE:
                m_auiEncounter[1] = uiData;
                if (uiData == DONE)
                {
                    if (m_uiArgentChallengerID == NPC_EADRIC)
                        if (GameObject* pChest = GetSingleGameObjectFromStorage(m_bIsRegularMode ? GO_EADRIC_LOOT : GO_EADRIC_LOOT_H))
                            if (pChest && !pChest->isSpawned())
                                pChest->SetRespawnTime(DAY);

                    if (m_uiArgentChallengerID == NPC_PALETRESS)
                        if (GameObject* pChest = GetSingleGameObjectFromStorage(m_bIsRegularMode ? GO_PALETRESS_LOOT : GO_PALETRESS_LOOT_H))
                            if (pChest && !pChest->isSpawned())
                                pChest->SetRespawnTime(DAY);
                }
                if (uiData == FAIL)
                {
                    m_auiEncounter[1] = NOT_STARTED;
                }
                break;
            case TYPE_BLACK_KNIGHT:
                m_auiEncounter[2] = uiData;
                if (uiData == FAIL)
                {
                    m_auiEncounter[2] = NOT_STARTED;
                }
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;

            for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                saveStream << m_auiEncounter[i] << " ";

            m_strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case DATA_CHAMPIONID_1:
                return m_uiChampionId1;
            case DATA_CHAMPIONID_2:
                return m_uiChampionId2;
            case DATA_CHAMPIONID_3:
                return m_uiChampionId3;
            case DATA_CHAMPIONS_COUNT:
                return m_uiChampionsCount;
            case DATA_ARGENT_CHALLENGER:
                return m_uiArgentChallengerID;
            case DATA_BLACK_KNIGHT_MINION:
                return m_uiBlackKnightMinionID;
            case DATA_TOC5_ANNOUNCER:
                return m_uiAnnouncerID;
            case TYPE_GRAND_CHAMPIONS:
                return m_auiEncounter[0];
            case TYPE_ARGENT_CHALLENGE:
                return m_auiEncounter[1];
            case TYPE_BLACK_KNIGHT:
                return m_auiEncounter[2];
        }

        return 0;
    }

    const char* Save()
    {
        return m_strInstData.c_str();
    }

    void Load(const char* strIn)
    {
        if (!strIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(strIn);

        std::istringstream loadStream(strIn);

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            loadStream >> m_auiEncounter[i];

            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_trial_of_the_champion(Map* pMap)
{
    return new instance_trial_of_the_champion(pMap);
}

void AddSC_instance_trial_of_the_champion()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_trial_of_the_champion";
    newscript->GetInstanceData = &GetInstanceData_instance_trial_of_the_champion;
    newscript->RegisterSelf();
}
