/*Copyright (C) 2012 SkyMist Project.
*
* THIS particular file is NOT free software; third-party users should NOT have access to it, redistribute it or modify it. :)
*/

#include "precompiled.h"
#include "stonecore.h"

class MANGOS_DLL_DECL instance_stonecore : public ScriptedInstance
{
public:
    instance_stonecore(Map* map) : ScriptedInstance(map) { Initialize(); }

    uint64 Corborus;
    uint64 Slabhide;
    uint64 Ozruk;
    uint64 HighPriestessAzil;
    uint32 TeamInInstance;
    uint32 Encounter[MAX_ENCOUNTER];
    std::string m_strInstData;

    void Initialize() override
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            Encounter[i] = NOT_STARTED;

        Corborus = 0;
        Slabhide = 0;
        Ozruk = 0;
        HighPriestessAzil = 0;
    }

    bool IsEncounterInProgress() const override
    {
        for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
            if (Encounter[i] == IN_PROGRESS)
                return true;

            return false;
    }

    void OnCreatureCreate(Creature* creature) override
    {
        //Map::PlayerList const &players = instance->GetPlayers();

        //if (!players.isEmpty())
        //{
        //    if (Player* player = players.begin()->getSource())
        //        TeamInInstance = player->GetTeam();
        //}

        switch (creature->GetEntry())
        {
            case BOSS_CORBORUS:
            case BOSS_SLABHIDE:
            case BOSS_OZRUK:
            case BOSS_HIGH_PRIESTESS_AZIL:
                m_mNpcEntryGuidStore[creature->GetEntry()] = creature->GetObjectGuid();
                break;
        }
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case TYPE_CORBORUS:
                Encounter[0] = data;
                break;
            case TYPE_SLABHIDE:
                Encounter[1] = data;
                break;
            case TYPE_OZRUK:
                Encounter[2] = data;
                break;
            case TYPE_AZIL:
                Encounter[3] = data;
                break;
        }

        if (data == DONE)
        {
             OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << Encounter[0] << " " << Encounter[1] << " " << Encounter[2] << " " << Encounter[3];

            m_strInstData = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 type) override
    {
        if (type < MAX_ENCOUNTER)
            return Encounter[type];

        return 0;
    }

    void Load(const char* in) override
    {
        if (!in)
        {

            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        std::istringstream loadStream(in);
        loadStream >> Encounter[0] >> Encounter[1] >> Encounter[2] >> Encounter[3];

        for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
        {
            if (Encounter[i] == IN_PROGRESS)
                Encounter[i] = NOT_STARTED;
        }

        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_instance_stonecore(Map* pMap)
{
    return new instance_stonecore(pMap);
}

void AddSC_instance_stonecore()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_stonecore";
    pNewScript->GetInstanceData = &GetInstanceData_instance_stonecore;
    pNewScript->RegisterSelf();
}