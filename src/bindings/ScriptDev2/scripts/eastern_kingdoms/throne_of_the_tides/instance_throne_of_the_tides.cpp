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
SDName: instance_throne_of_the_tides
SD%Complete: 0
SDComment: Placeholder
SDCategory: Throne of the Tides
EndScriptData */

#include "precompiled.h"
#include "throne_of_the_tides.h"

instance_throne_of_the_tides::instance_throne_of_the_tides(Map* pMap) : ScriptedInstance(pMap)
{
    m_bIsRegularMode = pMap->IsRegularDifficulty();
    Initialize();
};

void instance_throne_of_the_tides::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_throne_of_the_tides::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_MINDBENDER_GHURSHA:
        case NPC_ERUNAK_STONESPEAKER:
        case NPC_NEPTULON:
        case NPC_LADY_NAZJAR_EVENT:
            break;
        case NPC_COMMANDER_ULTHOK:
            if (m_auiEncounter[TYPE_COMMANDER_ULTHOK_EVENT] == DONE)
                pCreature->SetPhaseMask(1, true);
            else
                pCreature->SetPhaseMask(2, true);
            break;
        case NPC_TELEPORTER_1:
        case NPC_TELEPORTER_2:
        case NPC_TELEPORTER_3:
            pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            return;
        default:
            return;
    }

    m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
}

void instance_throne_of_the_tides::OnCreatureDeath(Creature* pCreature)
{
}

void instance_throne_of_the_tides::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CORALES:
            if (m_auiEncounter[TYPE_COMMANDER_ULTHOK_EVENT] == DONE)
                pGo->Delete();
            break;
        case GO_TENTACLE_LEFT:
        case GO_TENTACLE_RIGHT:
            if (m_auiEncounter[TYPE_COMMANDER_ULTHOK] == DONE)
                pGo->Delete();
            break;
        case GO_INVISIBLE_DOOR_1:
        case GO_INVISIBLE_DOOR_2:
        case GO_OZUMAT_DOOR:
        case GO_ERUNAK_STONESPEAKER_DOOR:
            if (m_auiEncounter[TYPE_COMMANDER_ULTHOK] == DONE)
                pGo->SetGoState(GO_STATE_ACTIVE);
            break;
        case GO_CONTROL_SYSTEM:
            if (m_auiEncounter[TYPE_LADY_NAZJAR] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_NEPTULON_CACHE:
        case GO_NEPTULON_CACHE_H:
            pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        default:
            return;
    }

    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_throne_of_the_tides::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_COMMANDER_ULTHOK:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_TENTACLE_LEFT))
                    pGo->Delete();
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_TENTACLE_RIGHT))
                    pGo->Delete();
                DoUseDoorOrButton(GO_INVISIBLE_DOOR_1);
                DoUseDoorOrButton(GO_INVISIBLE_DOOR_2);
                DoUseDoorOrButton(GO_OZUMAT_DOOR);
                DoUseDoorOrButton(GO_ERUNAK_STONESPEAKER_DOOR);
            }
            break;
        case TYPE_OZUMAT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                DoRespawnGameObject(m_bIsRegularMode ? GO_NEPTULON_CACHE : GO_NEPTULON_CACHE_H, HOUR);
            break;
        case TYPE_LADY_NAZJAR_EVENT:
        case TYPE_NEPTULON_EVENT:
        case TYPE_MINDBENDER_GHURSHA:
            m_auiEncounter[uiType] = uiData;
            break;
        case TYPE_LADY_NAZJAR:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                if (GameObject* go = GetSingleGameObjectFromStorage(GO_CONTROL_SYSTEM))
                    go->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case TYPE_COMMANDER_ULTHOK_EVENT:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
                if (Creature* creature = GetSingleCreatureFromStorage(NPC_COMMANDER_ULTHOK))
                    creature->SetPhaseMask(1, true);
            break;
        default:
            return;
    }

    //if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
            << m_auiEncounter[3] << " " << m_auiEncounter[4] << " " << m_auiEncounter[5] << " "
            << m_auiEncounter[6];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_throne_of_the_tides::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_throne_of_the_tides::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2]
        >> m_auiEncounter[3] >> m_auiEncounter[4] >> m_auiEncounter[5]
        >> m_auiEncounter[6];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_throne_of_the_tides(Map* pMap)
{
    return new instance_throne_of_the_tides(pMap);
}

enum ScriptTexts
{
    SAY_START_EVENT = 0,
    SAY_EVADE       = 1,
};

enum Events
{
    EVENT_SUMMON_DRUDGE = 1,
};

enum Actions
{
    ACTION_LADY_NAZJAR_START_EVENT      = 1,
};

enum Points
{
    POINT_EVADE = 1,
};

enum DataOther
{
    DATA_ATTACKER_GUID  = 1,
};

enum Creatures
{
    NPC_THRONE_OF_THE_TIDES_TELEPORTER_1    = 51391,
    NPC_THRONE_OF_THE_TIDES_TELEPORTER_2    = 51395,
    NPC_NAZJAR_INVADER                      = 39616,
    NPC_NAZJAR_SPIRITMENDER                 = 41139,
    NPC_DEEP_MURLOC_DRUDGE                  = 39960,
    NPC_TEMPEST_WITCH                       = 40634,
};

const Position2 eventPos[7] = 
{
    {-121.93f, 807.15f, 797.19f, 3.13f},
    {-122.00f, 799.22f, 797.13f, 3.13f},
    {-97.83f, 798.27f, 797.04f, 3.13f},
    {-98.13f, 806.96f, 797.04f, 3.13f},
    {-65.19f, 808.50f, 796.96f, 3.13f},
    {-66.57f, 798.02f, 796.87f, 3.13f},
    {13.67f, 802.34f, 806.43f, 0.12f},
};

/*
intro 1 1 1st floor
5806,643,-463.23f, 804.91f, 250.27f
intro 2 1
5834,643,-148.63f, 802.23f, 796.47f
intro gulthok
5873,643,99.62f, 803.47f, 807.60f
intro right
6017,643,-310.11f, 775.83f, 261.34f
intro left
6018,643,-310.354003906,839.882019043,258.59f
intro 0
6073,643,-651.87f, 807.44f, 270.0f
intro 0 0
6197,643,-660.95f, 807.43f, 244.75f
ozumat 1
6646,643,-198.41f, 981.96f, 230.59f
*/

bool OnTrigger_at_tott_lady_nazjar_event(Player* pPlayer, const AreaTriggerEntry* /*pAt*/)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance || !pPlayer)
        return true;

    if (pInstance->GetData(TYPE_LADY_NAZJAR_EVENT) != DONE
        && pInstance->GetData(TYPE_COMMANDER_ULTHOK) != DONE)
    {
        pInstance->SetData(TYPE_LADY_NAZJAR_EVENT, DONE);
        if (Creature* pLadyNazjarEvent = pInstance->GetSingleCreatureFromStorage(NPC_LADY_NAZJAR_EVENT))
            pLadyNazjarEvent->AI()->DoAction(ACTION_LADY_NAZJAR_START_EVENT);
    }

    return true;
}

const Position2 teleporterPos[2] = 
{
    {-560.25f, 819.19f, 245.28f, 5.20f}, // entrance;
    {-14.72f, 796.57f, 808.12f, 1.99f}, // upper
};

bool OnGossipHello_npc_throne_of_the_tides_teleporter(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->isInCombat())
        return false;

    switch (pCreature->GetEntry())
    {
        case NPC_THRONE_OF_THE_TIDES_TELEPORTER_1:
            pPlayer->NearTeleportTo(teleporterPos[1].GetPositionX(),
                teleporterPos[1].GetPositionY(),
                teleporterPos[1].GetPositionZ(),
                teleporterPos[1].GetOrientation());
            break;
        case NPC_THRONE_OF_THE_TIDES_TELEPORTER_2:
            pPlayer->NearTeleportTo(teleporterPos[0].GetPositionX(),
                teleporterPos[0].GetPositionY(),
                teleporterPos[0].GetPositionZ(),
                teleporterPos[0].GetOrientation());
            break;
    }

    return false;
}

struct MANGOS_DLL_DECL npc_lady_nazjar_eventAI : public ScriptedAI
{
    npc_lady_nazjar_eventAI(Creature* creature) : ScriptedAI(creature)
    {
        m_pInstance = (ScriptedInstance*)creature->GetInstanceData();
        m_bIsRegularMode = creature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    bool bEvade;

    void Reset() override
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_LADY_NAZJAR_EVENT) == DONE)
            me->ForcedDespawn();

        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        me->SetActiveObjectState(true);
        bEvade = false;
        events.Reset();
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            if (id == POINT_EVADE)
                me->ForcedDespawn();
        }
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_LADY_NAZJAR_START_EVENT)
        {
            //Talk(SAY_START_EVENT);
            me->SummonCreature(NPC_NAZJAR_INVADER, eventPos[0].x, eventPos[0].y, eventPos[0].z, eventPos[0].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
            me->SummonCreature(NPC_NAZJAR_INVADER, eventPos[1].x, eventPos[1].y, eventPos[1].z, eventPos[1].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
            me->SummonCreature(NPC_NAZJAR_INVADER, eventPos[2].x, eventPos[2].y, eventPos[2].z, eventPos[2].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
            me->SummonCreature(NPC_TEMPEST_WITCH, eventPos[3].x, eventPos[3].y, eventPos[3].z, eventPos[3].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
            me->SummonCreature(NPC_NAZJAR_SPIRITMENDER, eventPos[4].x, eventPos[4].y, eventPos[4].z, eventPos[4].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
            me->SummonCreature(NPC_NAZJAR_SPIRITMENDER, eventPos[5].x, eventPos[5].y, eventPos[5].z, eventPos[5].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_pInstance)
            return;

        if (GetClosestAttackableUnit(me, 50.0f) && !bEvade)
        {
            bEvade = true;
            //Talk(SAY_EVADE);
            me->GetMotionMaster()->MovePoint(POINT_EVADE, eventPos[6].x, eventPos[6].y, eventPos[6].z);
        }
    }
};

CreatureAI* GetAI_npc_lady_nazjar_event(Creature* pCreature)
{
    return new npc_lady_nazjar_eventAI(pCreature);
}

bool OnGOUse_go_totd_defense_system(Player* player, GameObject* go)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData();
    if (!pInstance || !player)
        return true;

    Map::PlayerList const &PlayerList = go->GetMap()->GetPlayers();

    if (!PlayerList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            if (Player* pl = i->getSource())
                pl->SendCinematicStart(169);

    return false;
}

void AddSC_instance_throne_of_the_tides()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_throne_of_the_tides";
    pNewScript->GetInstanceData = &GetInstanceData_instance_throne_of_the_tides;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_tott_lady_nazjar_event"; // 5806
    pNewScript->pAreaTrigger = &OnTrigger_at_tott_lady_nazjar_event;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_throne_of_the_tides_teleporter";
    pNewScript->pGossipHello = &OnGossipHello_npc_throne_of_the_tides_teleporter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_nazjar_event";
    pNewScript->GetAI = &GetAI_npc_lady_nazjar_event;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_totd_defense_system";
    pNewScript->pGOUse = &OnGOUse_go_totd_defense_system;
    pNewScript->RegisterSelf();
}
