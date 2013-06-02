/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: instance_baradin_hold
SD%Complete: 0
SDComment: Placeholder
SDCategory: Baradin Hold
EndScriptData */

#include "precompiled.h"
#include "baradin_hold.h"

instance_baradin_hold::instance_baradin_hold(Map* pMap) : ScriptedInstance(pMap)
{
    m_bIsRegularMode = pMap->IsRegularDifficulty();
    Initialize();
};

void instance_baradin_hold::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_baradin_hold::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        default:
            return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_baradin_hold::OnCreatureDeath(Creature* pCreature)
{
}

void instance_baradin_hold::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_DOOR_ARGALOTH:
            if (m_auiEncounter[TYPE_ARGALOTH] != IN_PROGRESS)
                pGo->SetGoState(GO_STATE_ACTIVE);
            else
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_DOOR_OCCUTHAR:
            if (m_auiEncounter[TYPE_OCCUTHAR] != IN_PROGRESS)
                pGo->SetGoState(GO_STATE_ACTIVE);
            else
                pGo->SetGoState(GO_STATE_READY);
            break;
        case GO_DOOR_ALIZABAL:
            if (m_auiEncounter[TYPE_ALIZABAL] != IN_PROGRESS)
                pGo->SetGoState(GO_STATE_ACTIVE);
            else
                pGo->SetGoState(GO_STATE_READY);
            break;
        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_baradin_hold::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ARGALOTH:
            m_auiEncounter[uiType] = uiData;
            if (GameObject* go = GetSingleGameObjectFromStorage(GO_DOOR_ARGALOTH))
                go->SetGoState(uiData == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE);
            break;
        case TYPE_OCCUTHAR:
            m_auiEncounter[uiType] = uiData;
            if (GameObject* go = GetSingleGameObjectFromStorage(GO_DOOR_OCCUTHAR))
                go->SetGoState(uiData == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE);
            break;
        case TYPE_ALIZABAL:
            m_auiEncounter[uiType] = uiData;
            if (GameObject* go = GetSingleGameObjectFromStorage(GO_DOOR_ALIZABAL))
                go->SetGoState(uiData == IN_PROGRESS ? GO_STATE_READY : GO_STATE_ACTIVE);
            break;
        default:
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_baradin_hold::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_baradin_hold::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_baradin_hold(Map* pMap)
{
    return new instance_baradin_hold(pMap);
}

void AddSC_instance_baradin_hold()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_baradin_hold";
    pNewScript->GetInstanceData = &GetInstanceData_instance_baradin_hold;
    pNewScript->RegisterSelf();
}
