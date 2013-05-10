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
SDName: instance_vortex_pinnacle
SD%Complete: 0
SDComment: Placeholder
SDCategory: Vortex Pinnacle
EndScriptData */

#include "precompiled.h"
#include "vortex_pinnacle.h"

struct SlipStreamHelperStruct
{
    float srcX;
    float srcY;
    float destX;
    float destY;
    float destZ;
};

#define MAX_SLIPSTREAM 4

SlipStreamHelperStruct slipstreamLocations[MAX_SLIPSTREAM] =
{
    { -310.0f, -29.74f, -902.12f, -180.40f, 664.51f },      // entrance to Altairus
    { -771.0f, -58.74f, -902.12f, -180.40f, 664.51f },      // Ertan to Altairus
    { -382.0f,  42.31f, -1190.00f, 475.71f, 634.78f },      // entrance to Asaad
    { -1198.79f, 107.05f, -1190.00f, 475.71f, 634.78f },    // Altairus to Asaad
};

instance_vortex_pinnacle::instance_vortex_pinnacle(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
};

void instance_vortex_pinnacle::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_vortex_pinnacle::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_SLIPSTREAM:
            pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pCreature->SetActiveObjectState(true);

            if (pCreature->GetDistance2d(slipstreamLocations[0].srcX, slipstreamLocations[0].srcY) < 5.0f)
            {
                m_streamGuds[TYPE_ETAN].push_back(pCreature->GetObjectGuid());
                if (m_auiEncounter[TYPE_ETAN] == DONE)
                    break;
            }

            if (pCreature->GetDistance2d(slipstreamLocations[1].srcX, slipstreamLocations[1].srcY) < 5.0f)
            {
                m_streamGuds[TYPE_ETAN].push_back(pCreature->GetObjectGuid());
                if (m_auiEncounter[TYPE_ETAN] == DONE)
                    break;
            }

            if (pCreature->GetDistance2d(slipstreamLocations[2].srcX, slipstreamLocations[2].srcY) < 5.0f)
            {
                m_streamGuds[TYPE_ALTAIRUS].push_back(pCreature->GetObjectGuid());
                if (m_auiEncounter[TYPE_ALTAIRUS] == DONE)
                    break;
            }

            if (pCreature->GetDistance2d(slipstreamLocations[3].srcX, slipstreamLocations[3].srcY) < 5.0f)
            {
                m_streamGuds[TYPE_ALTAIRUS].push_back(pCreature->GetObjectGuid());
                if (m_auiEncounter[TYPE_ALTAIRUS] == DONE)
                    break;
            }

            pCreature->SetVisibility(VISIBILITY_OFF);
            return;
    }
}

void instance_vortex_pinnacle::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_vortex_pinnacle::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ETAN:
            m_auiEncounter[uiType] = uiData;

            if (uiData == DONE)
                for (std::list<ObjectGuid>::iterator itr = m_streamGuds[uiType].begin(); itr != m_streamGuds[uiType].end(); ++itr)
                    if (Creature* pCreature = instance->GetAnyTypeCreature(*itr))
                        pCreature->SetVisibility(VISIBILITY_ON);
            break;
        case TYPE_ALTAIRUS:
            m_auiEncounter[uiType] = uiData;

            if (uiData == DONE)
                for (std::list<ObjectGuid>::iterator itr = m_streamGuds[uiType].begin(); itr != m_streamGuds[uiType].end(); ++itr)
                    if (Creature* pCreature = instance->GetAnyTypeCreature(*itr))
                        pCreature->SetVisibility(VISIBILITY_ON);
            break;
        case TYPE_ASSAD:
            m_auiEncounter[uiType] = uiData;
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

uint32 instance_vortex_pinnacle::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_vortex_pinnacle::Load(const char* chrIn)
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

InstanceData* GetInstanceData_instance_vortex_pinnacle(Map* pMap)
{
    return new instance_vortex_pinnacle(pMap);
}

bool OnGossipHello_npc_slipstream(Player* who, Creature* creature)
{
    if (who->IsInCombat())
        return true;

    for (int i = 0; i < MAX_SLIPSTREAM; ++i)
        if (who->GetDistance2d(slipstreamLocations[i].srcX, slipstreamLocations[i].srcY) < 20.0f)
        {
            who->NearTeleportTo(slipstreamLocations[i].destX, slipstreamLocations[i].destY, slipstreamLocations[i].destZ, who->GetOrientation());
            break;
        }

    return true;
}

void AddSC_instance_vortex_pinnacle()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_vortex_pinnacle";
    pNewScript->GetInstanceData = &GetInstanceData_instance_vortex_pinnacle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_slipstream";
    pNewScript->pGossipHello = &OnGossipHello_npc_slipstream;
    pNewScript->RegisterSelf();
}
