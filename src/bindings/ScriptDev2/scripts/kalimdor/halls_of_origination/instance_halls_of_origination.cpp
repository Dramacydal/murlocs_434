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
SDName: instance_blackrock_caverns
SD%Complete: 10
SDComment: Placeholder
SDCategory: Halls of Origination
EndScriptData */

#include "precompiled.h"
#include "halls_of_origination.h"

instance_halls_of_origination::instance_halls_of_origination(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
};

void instance_halls_of_origination::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_halls_of_origination::OnCreatureCreate(Creature* pCreature)
{
    // ToDo: add Creature references here
}

void instance_halls_of_origination::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ANHUUR_BRIDGE:
        case GO_ANHUUR_DOOR:
        case GO_VAULT_OF_LIGHTS_ENTRANCE_DOOR:
        case GO_ANHUUR_BRIDGE_COL:
            if (m_auiEncounter[TYPE_ANHUUR] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            pGo->SetActiveObjectState(true);
            break;
        case GO_VAULT_OF_LIGHTS_BOSS_DOOR:
            if (WardensDead())
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ULDUM_LIGHT_MACHINE_02:
        case GO_ULDUM_LASERS_02:
            if (m_auiEncounter[TYPE_EARTH_WARDEN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ULDUM_LIGHT_MACHINE_01:
        case GO_ULDUM_LASERS_01:
            if (m_auiEncounter[TYPE_WATER_WARDEN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ULDUM_LIGHT_MACHINE_04:
        case GO_ULDUM_LASERS_04:
            if (m_auiEncounter[TYPE_AIR_WARDEN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_ULDUM_LIGHT_MACHINE_03:
        case GO_ULDUM_LASERS_03:
            if (m_auiEncounter[TYPE_FLAME_WARDEN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

bool instance_halls_of_origination::WardensDead()
{
    return m_auiEncounter[TYPE_EARTH_WARDEN] == DONE && m_auiEncounter[TYPE_WATER_WARDEN] == DONE &&
        m_auiEncounter[TYPE_AIR_WARDEN] == DONE && m_auiEncounter[TYPE_FLAME_WARDEN] == DONE;
}

void instance_halls_of_origination::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ANHUUR:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ANHUUR_BRIDGE);
                DoUseDoorOrButton(GO_ANHUUR_DOOR);
                DoUseDoorOrButton(GO_VAULT_OF_LIGHTS_ENTRANCE_DOOR);
                DoUseDoorOrButton(GO_ANHUUR_BRIDGE_COL);
            }
            break;
        case TYPE_EARTH_WARDEN:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ULDUM_LIGHT_MACHINE_02);
                DoUseDoorOrButton(GO_ULDUM_LASERS_02);
                if (WardensDead())
                    DoUseDoorOrButton(GO_VAULT_OF_LIGHTS_BOSS_DOOR);
            }
            break;
        case TYPE_WATER_WARDEN:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ULDUM_LIGHT_MACHINE_01);
                DoUseDoorOrButton(GO_ULDUM_LASERS_01);
                if (WardensDead())
                    DoUseDoorOrButton(GO_VAULT_OF_LIGHTS_BOSS_DOOR);
            }
            break;
        case TYPE_AIR_WARDEN:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ULDUM_LIGHT_MACHINE_04);
                DoUseDoorOrButton(GO_ULDUM_LASERS_04);
                if (WardensDead())
                    DoUseDoorOrButton(GO_VAULT_OF_LIGHTS_BOSS_DOOR);
            }
            break;
        case TYPE_FLAME_WARDEN:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                DoUseDoorOrButton(GO_ULDUM_LIGHT_MACHINE_03);
                DoUseDoorOrButton(GO_ULDUM_LASERS_03);
                if (WardensDead())
                    DoUseDoorOrButton(GO_VAULT_OF_LIGHTS_BOSS_DOOR);
            }
            break;
        case TYPE_PTAH:
        case TYPE_ANRAPHET:
        case TYPE_ISISET:
        case TYPE_AMMUNAE:
        case TYPE_SETESH:
        case TYPE_RAJH:
            m_auiEncounter[uiType] = uiData;
            break;
        default:
            return;
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
                   << m_auiEncounter[6] << " " << m_auiEncounter[7] << " " << m_auiEncounter[8] << " "
                   << m_auiEncounter[9] << " " << m_auiEncounter[10];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_halls_of_origination::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_halls_of_origination::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3]
               >> m_auiEncounter[4] >> m_auiEncounter[5] >> m_auiEncounter[6] >> m_auiEncounter[7]
               >> m_auiEncounter[8] >> m_auiEncounter[9] >> m_auiEncounter[10];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_halls_of_origination(Map* pMap)
{
    return new instance_halls_of_origination(pMap);
}

void AddSC_instance_halls_of_origination()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_halls_of_origination";
    pNewScript->GetInstanceData = &GetInstanceData_instance_halls_of_origination;
    pNewScript->RegisterSelf();
}
