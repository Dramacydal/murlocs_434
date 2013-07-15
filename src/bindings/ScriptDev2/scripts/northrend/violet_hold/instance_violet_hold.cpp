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
SDName: Instance The Violet Hold
SD%Complete: 60%
SDComment:
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"
#include "MapPersistentStateMgr.h"

/* The Violet Hold encounters:
0 Whole Event
1 Rift
2 Erekem
3 Moragg
4 Ichoron
5 Xevozz
6 Lavanthor
7 Zuramat
*/
//inline uint32 RandRiftBoss() { return ((rand()%2) ? NPC_GUARDIAN : NPC_KEEPER); }

struct MANGOS_DLL_DECL instance_violet_hold : public ScriptedInstance
{
    instance_violet_hold(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER+1];
    std::string m_strInstData;

    bool bIsInBoss;

    uint8 m_uiLastBossID;
    uint8 m_uiLastBossIDConst;
    uint8 m_uiRiftPortalCount;
    uint32 m_uiShieldPercent;
    uint32 m_uiDisruptions;
    int8 m_uiPortalTime;

    void Initialize()
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            m_auiEncounter[i] = NOT_STARTED;

        Clear();
    }

    void Clear(){
        bIsInBoss = false;

        m_uiLastBossID = 0;
        m_uiRiftPortalCount = 0;
        m_uiPortalTime = 0;
        m_uiShieldPercent = 100;
    }

    void InitWorldState(bool Enable = true)
    {
        DoUpdateWorldState(WORLD_STATE_VH,Enable ? 1 : 0);
        DoUpdateWorldState(WORLD_STATE_VH_PRISON,100);
        DoUpdateWorldState(WORLD_STATE_VH_PORTALS,0);
    }

    void OnPlayerEnter(Player* pPlayer)
    {
        if(m_auiEncounter[0] != NOT_STARTED)
            pPlayer->SendUpdateWorldState(WORLD_STATE_VH,1);
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;

        return false;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch(pCreature->GetEntry())
        {
            case NPC_SINCLARI:
            case NPC_DOOR_SEAL:
            case NPC_EREKEM:
            case NPC_MORAGG:
            case NPC_ICHORON:
            case NPC_XEVOZZ:
            case NPC_LAVANTHOR:
            case NPC_ZURAMAT:
                m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                break;
        }
    }

    void OnObjectCreate(GameObject* pGo)
    {
        m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch(uiType)
        {
            case TYPE_EVENT:
                if (uiData == IN_PROGRESS)
                {
                    Clear();
                    InitWorldState();
                }
                else if (uiData == FAIL || uiData == DONE)
                {
                    DoUpdateWorldState(WORLD_STATE_VH, 0);
                    DoUseDoorOrButton(GO_DOOR_SEAL);
                    if (Creature* pSinclari = GetSingleCreatureFromStorage(NPC_SINCLARI))
                        pSinclari->ForcedDespawn(1000);
                }
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_EREKEM:
                m_auiEncounter[2] = uiData;
                if (uiData == IN_PROGRESS) bIsInBoss = true;
                break;
            case TYPE_MORAGG:
                m_auiEncounter[3] = uiData;
                if (uiData == IN_PROGRESS) bIsInBoss = true;
                break;
            case TYPE_ICHORON:
                m_auiEncounter[4] = uiData;
                if (uiData == IN_PROGRESS) bIsInBoss = true;
                break;
            case TYPE_XEVOZZ:
                m_auiEncounter[5] = uiData;
                if (uiData == IN_PROGRESS) bIsInBoss = true;
                break;
            case TYPE_LAVANTHOR:
                m_auiEncounter[6] = uiData;
                if (uiData == IN_PROGRESS) bIsInBoss = true;
                break;
            case TYPE_ZURAMAT:
                m_auiEncounter[7] = uiData;
                if (uiData == IN_PROGRESS) bIsInBoss = true;
                break;
            case TYPE_CYANIGOSA:
                m_auiEncounter[8] = uiData;
                if (uiData == IN_PROGRESS) bIsInBoss = true;
                break;
            case TYPE_PORTAL6:
                m_auiEncounter[9] = uiData;
                if (uiData == DONE)
                {
                    #ifndef WIN32
                    if (DungeonPersistentState* state = ((DungeonMap*)this->instance)->GetPersistanceState())
                        state->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, ENCOUNTER_CREDIT_1ST_BOSS, NULL);
                    #endif
                }
                break;
            case TYPE_PORTAL12:
                m_auiEncounter[10] = uiData;
                if (uiData == DONE)
                {
                    #ifndef WIN32
                    if (DungeonPersistentState* state = ((DungeonMap*)this->instance)->GetPersistanceState())
                        state->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, ENCOUNTER_CREDIT_2ND_BOSS, NULL);
                    #endif
                }
                break;
            case TYPE_RIFT:
                if (uiData == FAIL)
                   DoUseDoorOrButton(GO_DOOR_SEAL);
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_DOOR:
                if (uiData == SPECIAL)
                {
                    m_uiShieldPercent = m_uiShieldPercent - 5;
                    if(m_uiShieldPercent > 0)
                        DoUpdateWorldState(WORLD_STATE_VH_PRISON, m_uiShieldPercent);
                    else
                    {   DoUpdateWorldState(WORLD_STATE_VH, 0);
                        DoUseDoorOrButton(GO_DOOR_SEAL);
                        m_auiEncounter[0] = FAIL;
                    }
                }
                break;
            case TYPE_DISRUPTIONS:
            m_uiDisruptions = uiData;
//            DoUpdateWorldState(WORLD_STATE_VH_PRISON, 100-m_uiDisruptions*5);
            break;
            case TYPE_LASTBOSS_ID:
            m_uiLastBossIDConst = uiData;
            break;

        }
        if (uiData == DONE)
        {
            bIsInBoss = false;
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
            case TYPE_EVENT:
                return m_auiEncounter[0];
            case TYPE_EREKEM:
                return m_auiEncounter[2];
            case TYPE_MORAGG:
                return m_auiEncounter[3];
            case TYPE_ICHORON:
                return m_auiEncounter[4];
            case TYPE_XEVOZZ:
                return m_auiEncounter[5];
            case TYPE_LAVANTHOR:
                return m_auiEncounter[6];
            case TYPE_ZURAMAT:
                return m_auiEncounter[7];
            case TYPE_CYANIGOSA:
                return m_auiEncounter[8];
            case TYPE_PORTAL6:
                return m_auiEncounter[9];
            case TYPE_PORTAL12:
                return m_auiEncounter[10];
            case TYPE_RIFT:
                return m_uiRiftPortalCount;
            case TYPE_LASTBOSS_ID:
                return m_uiLastBossIDConst;
            case TYPE_LASTBOSS:
            {
                if (m_uiLastBossID == 0)
                    m_uiLastBossID = urand(2, 7);
                else
                {
                    m_uiLastBossID = urand(2, 7);
                    if ( m_auiEncounter[2] == DONE &&
                         m_auiEncounter[3] == DONE &&
                         m_auiEncounter[4] == DONE &&
                         m_auiEncounter[5] == DONE &&
                         m_auiEncounter[6] == DONE &&
                         m_auiEncounter[7] == DONE) return 0;
                    while ( m_auiEncounter[m_uiLastBossID] == DONE
                            || m_auiEncounter[m_uiLastBossID] == IN_PROGRESS 
                            || m_auiEncounter[m_uiLastBossID] == SPECIAL ) 
                        {
                            m_uiLastBossID = urand(2, 7);
                        }
                }
                return m_uiLastBossID;
            }
            case DATA_BOSSTIME:
                return bIsInBoss;
            case TYPE_DISRUPTIONS:
                return m_uiDisruptions;
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

            if (m_auiEncounter[i] == IN_PROGRESS && i != 1)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_violet_hold(Map* pMap)
{
    return new instance_violet_hold(pMap);
}

void AddSC_instance_violet_hold()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_violet_hold";
    newscript->GetInstanceData = &GetInstanceData_instance_violet_hold;
    newscript->RegisterSelf();
}
