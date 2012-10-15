/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: Instance_Eye_of_Eternity
SD%Complete: 90%
SDComment:
SDAuthor: Tassadar
SDCategory: Nexus, Eye of Eternity
EndScriptData */

#include "precompiled.h"
#include "eye_of_eternity.h"

struct MANGOS_DLL_DECL instance_eye_of_eternity : public ScriptedInstance
{
    instance_eye_of_eternity(Map* pMap) : ScriptedInstance(pMap) 
    {
        difficulty = pMap->GetDifficulty();
        Initialize();
    }

    std::string strInstData;
    uint32 m_auiEncounter[MAX_ENCOUNTER];

    Difficulty difficulty;

    uint32 m_uiFocusingIrisGUID;
    uint32 m_uiGiftGUID;
    uint32 m_uiHeartGUID;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));

        switch (difficulty)
        {
             case RAID_DIFFICULTY_10MAN_NORMAL:
                       m_uiFocusingIrisGUID = GO_FOCUSING_IRIS;
                       m_uiGiftGUID = GO_ALEXSTRASZAS_GIFT;
                       m_uiHeartGUID = GO_HEART_OF_MAGIC;
                       break;
             case RAID_DIFFICULTY_25MAN_NORMAL:
                       m_uiFocusingIrisGUID = GO_FOCUSING_IRIS_H;
                       m_uiGiftGUID = GO_ALEXSTRASZAS_GIFT_H;
                       m_uiHeartGUID = GO_HEART_OF_MAGIC_H;
                       break;
             default:
                       break;
        };

    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_MALYGOS:
                m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                pCreature->SetActiveObjectState(true);
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_PLATFORM:
            case GO_EXIT_PORTAL:
            case GO_FOCUSING_IRIS:
            case GO_FOCUSING_IRIS_H:
            case GO_ALEXSTRASZAS_GIFT:
            case GO_ALEXSTRASZAS_GIFT_H:
            case GO_HEART_OF_MAGIC:
            case GO_HEART_OF_MAGIC_H:
                m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
                break;
        }
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_MALYGOS:
            {
                if (uiData == NOT_STARTED)
                {
                    if (GameObject* pFocusingIris = GetSingleGameObjectFromStorage(m_uiFocusingIrisGUID))
                    {
                        pFocusingIris->SetGoState(GO_STATE_READY);
                        pFocusingIris->SetPhaseMask(1, true);
                    }
                    if (GameObject* pExitPortal = GetSingleGameObjectFromStorage(GO_EXIT_PORTAL))
                        pExitPortal->SetPhaseMask(1, true);
                    if (GameObject* pPlatform = GetSingleGameObjectFromStorage(GO_PLATFORM))
                        pPlatform->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                }
                if (uiData == IN_PROGRESS)
                {
                    if (GameObject* pFocusingIris = GetSingleGameObjectFromStorage(m_uiFocusingIrisGUID))
                        pFocusingIris->SetPhaseMask(2, true);
                    if (GameObject* pExitPortal = GetSingleGameObjectFromStorage(GO_EXIT_PORTAL))
                        pExitPortal->SetPhaseMask(2, true);
                }
                if (uiData == DONE)
                {
                    if (GameObject* pExitPortal = GetSingleGameObjectFromStorage(GO_EXIT_PORTAL))
                        pExitPortal->SetPhaseMask(1, true);
                    DoRespawnGameObject(m_uiGiftGUID, HOUR*IN_MILLISECONDS);
                    DoRespawnGameObject(m_uiHeartGUID, HOUR*IN_MILLISECONDS);
                }
                m_auiEncounter[0] = uiData;
                break;
            }
        }
        OUT_SAVE_INST_DATA;
        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0];

        strInstData = saveStream.str();
        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;

    }

    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* chrIn)
    {
        if (!chrIn)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(chrIn);

        std::istringstream loadStream(chrIn);
        loadStream >> m_auiEncounter[0];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_MALYGOS:
                return m_auiEncounter[0];
        }
        return 0;
    }

};

InstanceData* GetInstanceData_instance_eye_of_eternity(Map* pMap)
{
    return new instance_eye_of_eternity(pMap);
}

void AddSC_instance_eye_of_eternity()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "instance_eye_of_eternity";
    pNewScript->GetInstanceData = &GetInstanceData_instance_eye_of_eternity;
    pNewScript->RegisterSelf();
}
