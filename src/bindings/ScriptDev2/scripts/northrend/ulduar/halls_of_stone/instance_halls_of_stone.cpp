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
SDName: Instance_Halls_of_Stone
SD%Complete: 0%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"

/* Halls of Lightning encounters:
0 - Krystallus
1 - Maiden of Grief
2 - Brann Bronzebeard
3 - Sjonnir The Ironshaper
*/

struct MANGOS_DLL_DECL instance_halls_of_stone : public ScriptedInstance
{
    instance_halls_of_stone(Map* pMap) : ScriptedInstance(pMap) {
    Regular = pMap->IsRegularDifficulty();
    Initialize();
    };

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    bool Regular;
    std::string strSaveData;

    void Initialize()
    {
        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            m_auiEncounter[i]=NOT_STARTED;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_KRYSTALLUS:
            case NPC_GRIEF:
            case NPC_BRANN:
            case NPC_SJONNIR:
            case NPC_KADDRAK:
            case NPC_ABEDNEUM:
            case NPC_MARNAK:
                m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        switch(pGo->GetEntry())
        {
            case GO_GRIEF_DOOR:
                if (m_auiEncounter[0] != DONE)
                    pGo->SetGoState(GO_STATE_READY);
                else
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_BRANN_DOOR:
                if (m_auiEncounter[1] != DONE)
                    pGo->SetGoState(GO_STATE_READY);
                else
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
            case GO_SJONNIR_DOOR:
                if (m_auiEncounter[2] != DONE)
                    pGo->SetGoState(GO_STATE_READY);
                else
                    pGo->SetGoState(GO_STATE_ACTIVE);
                break;
        }
        m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_KRYSTALLUS:
                if (uiData == DONE)
                    DoUseDoorOrButton(GO_GRIEF_DOOR);
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_GRIEF:
                if (uiData == DONE)
                    DoUseDoorOrButton(GO_BRANN_DOOR);
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_BRANN:
                if (uiData == DONE)
                {
                    DoUseDoorOrButton(GO_SJONNIR_DOOR);
                    DoRespawnGameObject(instance->IsRegularDifficulty() ? GO_TRIBUNAL_CHEST : GO_TRIBUNAL_CHEST_H);
                }
                m_auiEncounter[2] = uiData;
                break;
            case TYPE_SJONNIR:
                m_auiEncounter[3] = uiData;
                break;
        }
        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;

            for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
                saveStream << m_auiEncounter[i] << " ";

            strSaveData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }

    }

    uint32 GetData(uint32 uiType)
    {
        switch(uiType)
        {
            case TYPE_KRYSTALLUS:
                return m_auiEncounter[0];
            case TYPE_GRIEF:
                return m_auiEncounter[1];
            case TYPE_BRANN:
                return m_auiEncounter[2];
            case TYPE_SJONNIR:
                return m_auiEncounter[3];
        }
        return 0;
    }

    const char* Save()
    {
        return strSaveData.c_str();
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

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            loadStream >> m_auiEncounter[i];

            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

};

InstanceData* GetInstanceData_instance_halls_of_stone(Map* pMap)
{
    return new instance_halls_of_stone(pMap);
}

void AddSC_instance_halls_of_stone()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_halls_of_stone";
    newscript->GetInstanceData = &GetInstanceData_instance_halls_of_stone;
    newscript->RegisterSelf();
}

