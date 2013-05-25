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
SDName: instance_well_of_eternity
SD%Complete: 0
SDComment: Placeholder
SDCategory: Well of Eternity
EndScriptData */

#include "precompiled.h"
#include "well_of_eternity.h"

instance_well_of_eternity::instance_well_of_eternity(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
};

void instance_well_of_eternity::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_well_of_eternity::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_AZSHARA:
            if (m_auiEncounter[TYPE_AZSHARA] == DONE)
                pCreature->SetVisibility(VISIBILITY_OFF);
            break;
        case NPC_PEROTHARN:
            if (m_auiEncounter[TYPE_ENERGY_FOCUS] != MAX_FOCUS)
            {
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                pCreature->SetVisibility(VISIBILITY_OFF);
            }
            break;
        case NPC_GUARDIAN_DEMON:
            guardianDemons.insert(pCreature->GetObjectGuid());
            if (m_auiEncounter[TYPE_ENERGY_FOCUS] >= MAX_FOCUS)
                pCreature->ForcedDespawn();
            return;
        case NPC_DRAKE_VEHICLE:
            if (m_auiEncounter[TYPE_AZSHARA] != DONE)
                pCreature->SetVisibility(VISIBILITY_OFF);
            drakes.insert(pCreature->GetObjectGuid());
            return;
        case NPC_CORRUPTED_ARCANIST:
        case NPC_DREADLORD_DEFENDER:
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            return;
        case NPC_ESCORT_VAROTHEN:
            pCreature->SetPhaseMask(2, true);
            return;
        case NPC_ENCHANTED_MAGUS_1:
        case NPC_ENCHANTED_MAGUS_2:
        case NPC_ENCHANTED_MAGUS_3:
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
            return;
        case NPC_FEL_CRYSTAL:
            felCrystals.insert(pCreature->GetObjectGuid());
            return;
        default:
            return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_well_of_eternity::OnCreatureDeath(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_LEGION_DEMON:
        {
            uint32 val = m_auiEncounter[TYPE_GUARDS_SLAIN];
            ++val;
            if (val > MAX_GUARDS)
                val = MAX_GUARDS;

            SetData(TYPE_GUARDS_SLAIN, val);
            break;
        }
        case NPC_ENCHANTED_MAGUS_1:
        case NPC_ENCHANTED_MAGUS_2:
        case NPC_ENCHANTED_MAGUS_3:
        {
            if (Creature* azshara = GetSingleCreatureFromStorage(NPC_AZSHARA))
                azshara->AI()->SetData(DATA_MAGUS_DEAD, pCreature->GetGUIDLow());
            break;
        }
    }
}

void instance_well_of_eternity::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_INVISIBLE_FIRE_WALL:
            if (m_auiEncounter[TYPE_PEROTHARN] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_SMALL_FIREWALL_DOOR:
            // block Pero'tharn
            if (pGo->GetDistance2d(3335.0f, -4890.0f) < 100.0f)
            {
                perotharnDoors.insert(pGo->GetObjectGuid());
                if (m_auiEncounter[TYPE_ENERGY_FOCUS] >= MAX_FOCUS)
                    pGo->SetGoState(GO_STATE_ACTIVE);
            }
            return;
        case GO_LARGE_FIREWALL_DOOR:
            // courtyard
            if (pGo->GetDistance2d(3194.0f, -4933.0f) < 15.0f)
            {
                courtyardDoors.insert(pGo->GetObjectGuid());
                if (m_auiEncounter[TYPE_GUARDS_SLAIN] > 0)
                    pGo->SetGoState(GO_STATE_ACTIVE);
            }
            // block Pero'tharn
            else if (pGo->GetDistance2d(3335.0f, -4890.0f) < 100.0f)
            {
                perotharnDoors.insert(pGo->GetObjectGuid());
                if (m_auiEncounter[TYPE_ENERGY_FOCUS] >= MAX_FOCUS)
                    pGo->SetGoState(GO_STATE_ACTIVE);
            }
            // before last energy focus
            else if (pGo->GetDistance2d(3452.0f, -4822.0f) < 30.0f)
            {
                energyFocusDoors.insert(pGo->GetObjectGuid());
                if (m_auiEncounter[TYPE_GUARDS_SLAIN] >= MAX_GUARDS)
                    pGo->SetGoState(GO_STATE_ACTIVE);
            }
            return;
        case GO_WOE_COURTYARD_DOOR:
            courtyardDoors.insert(pGo->GetObjectGuid());
            if (m_auiEncounter[TYPE_GUARDS_SLAIN] == 1)
                pGo->SetGoState(GO_STATE_ACTIVE);
            return;
        case GO_PORTAL_ENERGY_FOCUS_1:
        case GO_PORTAL_ENERGY_FOCUS_2:
        case GO_PORTAL_ENERGY_FOCUS_3:
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_LOCKED);
            if (m_auiEncounter[TYPE_ENERGY_FOCUS] >= MAX_FOCUS)
                pGo->Delete();
            break;
        case GO_ROYAL_CACHE:
        case GO_MINOR_CACHE_OF_THE_ASPECTS:
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_well_of_eternity::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_GUARDS_SLAIN:
            m_auiEncounter[uiType] = uiData;
            // open last enery focus door
            if (uiData >= MAX_GUARDS)
                for (GuidSet::iterator itr = energyFocusDoors.begin(); itr != energyFocusDoors.end(); ++itr)
                    DoUseDoorOrButton(*itr);
            // open courtyard doors
            if (uiData == 1)
                for (GuidSet::iterator itr = courtyardDoors.begin(); itr != courtyardDoors.end(); ++itr)
                    DoUseDoorOrButton(*itr);
            break;
        case TYPE_ENERGY_FOCUS:
            m_auiEncounter[uiType] = uiData;
            if (uiData == MAX_FOCUS)
            {
                for (GuidSet::iterator itr = perotharnDoors.begin(); itr != perotharnDoors.end(); ++itr)
                    DoUseDoorOrButton(*itr);
                for (GuidSet::iterator itr = guardianDemons.begin(); itr != guardianDemons.end(); ++itr)
                    if (Creature* demon = instance->GetAnyTypeCreature(*itr))
                        demon->ForcedDespawn();
                if (Creature* pCreature = GetSingleCreatureFromStorage(NPC_PEROTHARN))
                {
                    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    pCreature->SetVisibility(VISIBILITY_ON);
                }
            }
            break;
        case TYPE_PEROTHARN:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoUseDoorOrButton(GO_INVISIBLE_FIRE_WALL);
            break;
        case TYPE_AZSHARA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                for (GuidSet::iterator itr = drakes.begin(); itr != drakes.end(); ++itr)
                    if (Creature* drake = instance->GetAnyTypeCreature(*itr))
                        drake->SetVisibility(VISIBILITY_ON);

                if (Creature* pCreature = GetSingleCreatureFromStorage(NPC_AZSHARA))
                    pCreature->SetVisibility(VISIBILITY_OFF);

                DoRespawnGameObject(GO_ROYAL_CACHE, HOUR);
            }
            break;
        case TYPE_MANNOROTH:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoRespawnGameObject(GO_MINOR_CACHE_OF_THE_ASPECTS, HOUR);
            break;
        default:
            return;
    }

    if (uiData == DONE || uiType == TYPE_ENERGY_FOCUS || uiType == TYPE_GUARDS_SLAIN)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3] << " " << m_auiEncounter[4];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_well_of_eternity::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_well_of_eternity::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3] >> m_auiEncounter[4];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (i == TYPE_ENERGY_FOCUS || i == TYPE_GUARDS_SLAIN)
            continue;

        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_well_of_eternity(Map* pMap)
{
    return new instance_well_of_eternity(pMap);
}

bool OnGOUse_go_portal_energy_focus(Player* player, GameObject* go)
{
    instance_well_of_eternity* pInstance = (instance_well_of_eternity*)go->GetInstanceData();
    if (!pInstance)
        return true;

    uint32 usedCount = pInstance->GetData(TYPE_ENERGY_FOCUS) + 1;

    if (usedCount < MAX_FOCUS)
        pInstance->SetData(TYPE_ENERGY_FOCUS, usedCount);
    else
    {
        pInstance->SetData(TYPE_ENERGY_FOCUS, MAX_FOCUS);
        if (Creature* creature = pInstance->GetSingleCreatureFromStorage(NPC_PEROTHARN))
        {
            creature->SetVisibility(VISIBILITY_ON);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        }
    }

    for (GuidSet::iterator itr = pInstance->GetFelCrystals().begin(); itr != pInstance->GetFelCrystals().end(); ++itr)
        if (Creature* crystal = go->GetMap()->GetAnyTypeCreature(*itr))
            if (!crystal->isDead() && crystal->GetDistance2d(go))
                crystal->DealDamage(crystal, crystal->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

    go->Delete();

    return true;
}

// start 110089
// 3224.693359 Y: -5000.938477 Z: 194.093
// after perotharn  110090 
// 3492.743896 Y: -5005.039062 Z: 197.619
// azshara palace 110087
// 3489.691895 Y: -5203.373535 Z: 229.94
// after azshara 110088
// 3061.922119 Y: -5564.629395 Z: 18.128


enum
{
    SPELL_COURTYARD_ENTRANCE    = 107934,
    SPELL_AZSHARAS_PALACE       = 107690,
    SPELL_AZSHARAS_OVERLOOK     = 107979,
    SPELL_WELL_OF_ETERNITY      = 107691,
};

enum
{
    TELE_TEXT_START             = 2000006865,
    TELE_TEXT_PALACE            = 2000006866,
    TELE_TEXT_OVERLOOK          = 2000006867,
    TELE_TEXT_WELL              = 2000006868,
};

bool OnGossipHello_go_time_transit_device_woe(Player* who, GameObject* go)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData();
    if (!pInstance || !who)
        return true;

    who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TELE_TEXT_START, GOSSIP_SENDER_MAIN, 0);
    if (pInstance->GetData(TYPE_PEROTHARN) == DONE)
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TELE_TEXT_PALACE, GOSSIP_SENDER_MAIN, 1);
    if (pInstance->GetData(TYPE_AZSHARA) != NOT_STARTED)
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TELE_TEXT_OVERLOOK, GOSSIP_SENDER_MAIN, 2);
    if (pInstance->GetData(TYPE_AZSHARA) == DONE)
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TELE_TEXT_WELL, GOSSIP_SENDER_MAIN, 3);

    who->SEND_GOSSIP_MENU(who->GetGossipTextId(go), go->GetObjectGuid());

    return true;
}

bool OnGossipSelect_go_time_transit_device_woe(Player* who, GameObject* go, uint32 sender, uint32 action)
{
    if (who->IsInCombat())
        return true;

    if (sender != GOSSIP_SENDER_MAIN)
        return true;

    switch (action)
    {
        case 0: who->CastSpell(who, SPELL_COURTYARD_ENTRANCE, true); break;
        case 1: who->CastSpell(who, SPELL_AZSHARAS_PALACE, true); break;
        case 2: who->CastSpell(who, SPELL_AZSHARAS_OVERLOOK, true); break;
        case 3: who->CastSpell(who, SPELL_WELL_OF_ETERNITY, true); break;
    }

    return true;
}

void AddSC_instance_well_of_eternity()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_well_of_eternity";
    pNewScript->GetInstanceData = &GetInstanceData_instance_well_of_eternity;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_portal_energy_focus";
    pNewScript->pGOUse = &OnGOUse_go_portal_energy_focus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_time_transit_device_woe";
    pNewScript->pGossipHelloGO = &OnGossipHello_go_time_transit_device_woe;
    pNewScript->pGossipSelectGO = &OnGossipSelect_go_time_transit_device_woe;
    pNewScript->RegisterSelf();
}
