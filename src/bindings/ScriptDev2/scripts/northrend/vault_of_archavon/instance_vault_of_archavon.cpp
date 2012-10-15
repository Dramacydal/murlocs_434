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
SDName: Instance_Vault_of_Archavon
SD%Complete: 0
SDComment:
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

struct MANGOS_DLL_DECL instance_vault_of_archavon : public ScriptedInstance
{
    instance_vault_of_archavon(Map* pMap) : ScriptedInstance(pMap) {Initialize();};

    uint32 m_auiEncounter[MAX_ENCOUNTER];
    std::string strInstData;

    uint8 m_uiMinion;

    void Initialize()
    {
        memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
        m_uiMinion = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        switch (pCreature->GetEntry())
        {
            case NPC_TORAVON:
            case NPC_ARCHAVON:
            case NPC_EMALON:
            case NPC_KORALON:
                m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
                break;
            case NPC_TEMPEST_MINION:
                switch (m_uiMinion)
                {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                        m_mNpcEntryGuidStore[m_uiMinion + DATA_TEMPEST_MINION_1] = pCreature->GetObjectGuid();
                        ++m_uiMinion;
                        break;
                    case 4:
                    default:
                        m_uiMinion = 0;
                        break;
                }
                break;
        }
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
        switch (uiType)
        {
            case TYPE_ARCHAVON:
                m_auiEncounter[0] = uiData;
                break;
            case TYPE_EMALON:
                m_auiEncounter[1] = uiData;
                break;
            case TYPE_KORALON:
                m_auiEncounter[2] = uiData;
                break;
            case TYPE_TORAVON:
                m_auiEncounter[3] = uiData;
                break;
        }

        if (uiData == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

            strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 uiType)
    {
        switch (uiType)
        {
            case TYPE_ARCHAVON:
                return m_auiEncounter[0];
            case TYPE_EMALON:
                return m_auiEncounter[1];
            case TYPE_KORALON:
                return m_auiEncounter[2];
            case TYPE_TORAVON:
                return m_auiEncounter[3];
        }
        return 0;
    }


    const char* Save()
    {
        return strInstData.c_str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (m_auiEncounter[i] == IN_PROGRESS)
                m_auiEncounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }

    bool IsEncounterInProgress() const
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (m_auiEncounter[i] == IN_PROGRESS)
                return true;
        return false;
    }
};

InstanceData* GetInstanceData_instance_vault_of_archavon(Map* pMap)
{
    return new instance_vault_of_archavon(pMap);
}

void AddSC_instance_vault_of_archavon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_vault_of_archavon";
    newscript->GetInstanceData = &GetInstanceData_instance_vault_of_archavon;
    newscript->RegisterSelf();
}
