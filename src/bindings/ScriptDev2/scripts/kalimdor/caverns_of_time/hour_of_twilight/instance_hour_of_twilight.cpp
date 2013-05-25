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
SDName: instance_hour_of_twilight
SD%Complete: 0
SDComment: Placeholder
SDCategory: Hour of Twilight
EndScriptData */

#include "precompiled.h"
#include "hour_of_twilight.h"

instance_hour_of_twilight::instance_hour_of_twilight(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
};

void instance_hour_of_twilight::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_hour_of_twilight::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        default:
            return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_hour_of_twilight::OnCreatureDeath(Creature* pCreature)
{
}

void instance_hour_of_twilight::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ICE_WALL:
            if (m_auiEncounter[TYPE_ARCURION] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_hour_of_twilight::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_ARCURION:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_ICE_WALL);
            break;
        case TYPE_ASIRA:
        case TYPE_BENEDICTUS:
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

uint32 instance_hour_of_twilight::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_hour_of_twilight::Load(const char* chrIn)
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

InstanceData* GetInstanceData_instance_hour_of_twilight(Map* pMap)
{
    return new instance_hour_of_twilight(pMap);
}

enum
{
    TELE_TEXT_START             = 2000006869,
    TELE_TEXT_ASIRA             = 2000006870,
    TELE_TEXT_BENEDICTUS        = 2000006871,
};

Position2 const teleLocs[] =
{
    { 4921.965820f, 294.830963f, 96.212616f, 0.0f },
    { 4320.004883f, 573.422424f, -6.957148f, 0.0f },
    { 3945.524414f, 298.948242f, 12.480674f, 0.0f },
};

bool OnGoUse_go_portal_hot(Player* who, GameObject* go)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData();
    if (!pInstance || !who)
        return true;

    who->CLOSE_GOSSIP_MENU();

    who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TELE_TEXT_START, GOSSIP_SENDER_MAIN, 0);
    if (pInstance->GetData(TYPE_ASIRA) == DONE)
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TELE_TEXT_ASIRA, GOSSIP_SENDER_MAIN, 1);
    if (pInstance->GetData(TYPE_ASIRA) == DONE)
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TELE_TEXT_BENEDICTUS, GOSSIP_SENDER_MAIN, 2);

    who->SEND_GOSSIP_MENU(who->GetGossipTextId(go), go->GetObjectGuid());

    return true;
}

bool OnGossipSelect_go_portal_hot(Player* who, GameObject* go, uint32 sender, uint32 action)
{
    who->CLOSE_GOSSIP_MENU();

    if (who->IsInCombat())
        return true;

    if (sender != GOSSIP_SENDER_MAIN)
        return true;

    if (action < 3)
        who->TeleportTo(who->GetMapId(), teleLocs[action].x, teleLocs[action].y, teleLocs[action].z, who->GetOrientation());

    return true;
}

void AddSC_instance_hour_of_twilight()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_hour_of_twilight";
    pNewScript->GetInstanceData = &GetInstanceData_instance_hour_of_twilight;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_portal_hot";
    pNewScript->pGOUse = &OnGoUse_go_portal_hot;
    pNewScript->pGossipSelectGO = &OnGossipSelect_go_portal_hot;
    pNewScript->RegisterSelf();
}
