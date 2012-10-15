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
SDName: pit_of_saron
SD%Complete: 0
SDComment:
SDCategory: Pit of Saron
EndScriptData */

/* ContentData
EndContentData */

#include "precompiled.h"
#include "pit_of_saron.h"

/******** JAINA & SYLVANAS *******/
//Positional defines 
struct LocationsXY
{
    float x, y, z, o;
    uint32 id;
};
static LocationsXY SummonLoc[]=
{
    {441.81f, 217.28f, 528.707f, 0.198f},
    {448.92f, 209.52f, 528.710f, 0.198f},
    {442.13f, 200.32f, 528.710f, 0.198f},
};

static LocationsXY MoveLoc[]=
{
    {504.65f, 241.08f, 528.709f}, 
    {509.11f, 229.02f, 528.709f}, 
    {508.07f, 214.18f, 528.709f}, 
};

enum
{
    SAY_SPEECH_SYLVANAS1    = -1658003,
    SAY_SPEECH_SYLVANAS2    = -1658007,
    SAY_SPEECH_SYLVANAS3    = -1658011,
    SAY_SPEECH_SYLVANAS4    = -1658013,

    SAY_SPEECH_JAINA1       = -1658002,
    SAY_SPEECH_JAINA2       = -1658006,
    SAY_SPEECH_JAINA3       = -1658009,
    SAY_SPEECH_JAINA4       = -1658010,
    SAY_SPEECH_JAINA5       = -1658012,

    SAY_TYRANNUS1           = -1658001,
    SAY_TYRANNUS2           = -1658004,
    SAY_TYRANNUS3           = -1658005,
    SAY_TYRANNUS4           = -1658072,
    SAY_TYRANNUS5           = -1658008, 
    SPELL_NECROTIC_POWER    = 69347,
};

struct MANGOS_DLL_DECL npc_sylvanas_jaina_pos_startAI: public ScriptedAI
{
    npc_sylvanas_jaina_pos_startAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiTyrannusGuid.Clear();
        m_lGuards.clear();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSpeech_Timer;
    uint8 m_uiIntro_Phase;
    bool m_bIsIntro;
    uint32 creatureEntry;

    ObjectGuid m_uiTyrannusGuid;
    GUIDList m_lGuards;

    void Reset()
    {
        m_uiIntro_Phase     = 0;
        m_uiSpeech_Timer    = 1000;
        m_bIsIntro          = false;
        m_uiTyrannusGuid.Clear();
        m_lGuards.clear();
        creatureEntry = m_creature->GetEntry();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_INTRO, NOT_STARTED);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->isInAccessablePlaceFor(m_creature) && !m_bIsIntro && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 50) && m_creature->IsWithinLOSInMap(pWho))
        {
            if (m_pInstance && m_creature->GetPositionZ() > 525.0f)
            {
                if (m_pInstance->GetData(TYPE_INTRO) != DONE)
                {
                    m_bIsIntro = true;
                    m_uiSpeech_Timer = 5000;
                }
            }
        }
    }

    void SummonHordeChampions()
    {
        for (uint8 i = 0; i < 5; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_1_HORDE, SummonLoc[0].x + urand(0, 15), SummonLoc[0].y - urand(0, 15), SummonLoc[0].z, SummonLoc[0].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->HandleEmote(EMOTE_STATE_READY1H);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 5; i < 10; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_2_HORDE, SummonLoc[1].x + urand(0, 15), SummonLoc[1].y + urand(0, 15), SummonLoc[1].z, SummonLoc[1].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->HandleEmote(EMOTE_STATE_READY1H);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 10; i < 15; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_3_HORDE, SummonLoc[2].x + urand(0, 15), SummonLoc[2].y + urand(0, 15), SummonLoc[2].z, SummonLoc[2].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->HandleEmote(EMOTE_STATE_READY1H);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }
    }

    void SummonAlyChampions()
    {
        for (uint8 i = 0; i < 5; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_1_ALLIANCE, SummonLoc[0].x + urand(0, 15), SummonLoc[0].y + urand(0, 15), SummonLoc[0].z, SummonLoc[0].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->HandleEmote(EMOTE_STATE_READY1H);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 5; i < 10; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_2_ALLIANCE, SummonLoc[1].x + urand(0, 15), SummonLoc[1].y + urand(0, 15), SummonLoc[1].z, SummonLoc[1].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->HandleEmote(EMOTE_STATE_READY1H);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 10; i < 15; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_CHAMPION_3_ALLIANCE, SummonLoc[2].x + urand(0, 15), SummonLoc[2].y + urand(0, 15), SummonLoc[2].z, SummonLoc[2].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->HandleEmote(EMOTE_STATE_READY1H);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsIntro)
        {
            if (m_uiSpeech_Timer < uiDiff)
            {
                switch (m_uiIntro_Phase)
                {
                    case 0:
                        if (m_pInstance)
                            m_pInstance->SetData(TYPE_INTRO, IN_PROGRESS);

                        if (Creature* pTyrannus = m_creature->SummonCreature(NPC_TYRANNUS_INTRO, 526.501f, 237.639f, 543.686f, 3.431f, TEMPSUMMON_TIMED_DESPAWN, 40000))
                        {
                            pTyrannus->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            DoScriptText(SAY_TYRANNUS1, pTyrannus);
                            m_uiTyrannusGuid = pTyrannus->GetObjectGuid();
                        }

                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART1:
                                SummonAlyChampions();
                                break;
                            case NPC_SYLVANAS_PART1:
                                SummonHordeChampions();
                                break;
                        }
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 7000;
                        break;
                    case 1:
                        if (Creature* pTyrannus = m_pInstance->instance->GetCreature(m_uiTyrannusGuid))
                            DoScriptText(SAY_TYRANNUS2, pTyrannus);
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 13000;
                        break;
                    case 2:
                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART1:
                                DoScriptText(SAY_SPEECH_JAINA1, m_creature);
                                m_creature->setFaction(1802);
                                for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                                {
                                    if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                                    {
                                        pTemp->SetWalk(false);
                                        pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[1].x + urand(0, 20), MoveLoc[1].y + urand(0, 20), MoveLoc[1].z);
                                    }
                                }
                                break;
                            case NPC_SYLVANAS_PART1:
                                DoScriptText(SAY_SPEECH_SYLVANAS1, m_creature);
                                m_creature->setFaction(1801);
                                for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                                {
                                    if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                                    {
                                        pTemp->SetWalk(false);
                                        pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[1].x + urand(0, 20), MoveLoc[1].y + urand(0, 20), MoveLoc[1].z);
                                    }
                                }
                                break;
                        }
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 5000;
                        break;
                    case 3:
                        float x, y, z;
                        if (Creature* pTyrannus = m_pInstance->instance->GetCreature(m_uiTyrannusGuid))
                            DoScriptText(SAY_TYRANNUS3, pTyrannus);
                        for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                        {
                            if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                            {
                                pTemp->GetPosition(x, y, z);
                                pTemp->GetMotionMaster()->MovePoint(0, x, y, z + 5.0f);
                                pTemp->_AddAura(69413);
                            }
                        }
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 5000;
                        break;
                    case 4:
                        if(Creature* pTyrannus = m_pInstance->instance->GetCreature(m_uiTyrannusGuid))
                        {
                            DoScriptText(SAY_TYRANNUS4, pTyrannus);
                        }
                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART1:
                                DoScriptText(SAY_SPEECH_JAINA2, m_creature);
                                break;
                            case NPC_SYLVANAS_PART1:
                                DoScriptText(SAY_SPEECH_SYLVANAS2, m_creature);
                                break;
                        }
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 5000;
                        break;
                    case 5:
                        if(Creature* pTyrannus = m_pInstance->instance->GetCreature(m_uiTyrannusGuid))
                        {
                            pTyrannus->CastSpell(pTyrannus, 69753, false);
                        }
                        for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                        {
                            if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                            {
                                pTemp->GetPosition(x, y, z);
                                pTemp->GetMotionMaster()->MovePoint(0, x, y, z - 5.0f);
                            }
                        }  
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 1000;
                        break;
                    case 6:
                        if (Creature* pTyrannus = m_pInstance->instance->GetCreature(m_uiTyrannusGuid))
                        {
                            DoScriptText(SAY_TYRANNUS5, pTyrannus);
                            for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                            {
                                if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                                {
                                    pTemp->setFaction(21);
                                    pTemp->SetDisplayId(9785);
                                    pTemp->GetPosition(x, y, z);
                                    pTemp->GetMotionMaster()->MovePoint(0, x, y, z - 5.0f);
                                    pTemp->SetSpeedRate(MOVE_RUN, 0.8f);
                                    pTemp->GetPosition(x, y, z);
                                    pTemp->GetMotionMaster()->MovePoint(0, SummonLoc[2].x + urand(0, 20), SummonLoc[2].y + urand(0, 20), z);
                                    pTemp->RemoveAurasDueToSpell(69413);
                                }
                            }
                        }  
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 5000;
                        break;
                    case 7:
                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART1:
                                DoScriptText(SAY_SPEECH_JAINA3, m_creature);
                                for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                                {
                                    if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                                    {
                                        m_creature->CastSpell(pTemp, 72906, false);
                                    } 
                                }
                                break;
                            case NPC_SYLVANAS_PART1:
                                for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                                {
                                    if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                                    {
                                        m_creature->CastSpell(pTemp, 59514, true);
                                    } 
                                }
                                break;
                        }
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 5000;
                        break;
                    case 8:
                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART1:
                                DoScriptText(SAY_SPEECH_JAINA4, m_creature);
                                break;
                            case NPC_SYLVANAS_PART1:
                                DoScriptText(SAY_SPEECH_SYLVANAS3, m_creature);
                                break;
                        }
                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 7000;
                        break;
                    case 9:
                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART1:
                                DoScriptText(SAY_SPEECH_JAINA5, m_creature);
                                break;
                            case NPC_SYLVANAS_PART1:
                                DoScriptText(SAY_SPEECH_SYLVANAS4, m_creature);
                                break;
                        }
                        if (m_pInstance)
                            m_pInstance->SetData(TYPE_INTRO,DONE);

                        ++m_uiIntro_Phase;
                        m_uiSpeech_Timer = 10000;
                        break;
                    case 10:
                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART1:
                                m_creature->ForcedDespawn();
                                break;
                            case NPC_SYLVANAS_PART1:
                                m_creature->ForcedDespawn();
                                break;
                        }
                        ++m_uiIntro_Phase;
                        m_bIsIntro = false;
                        m_uiSpeech_Timer = 1000;
                        break;
                    default:
                        m_uiSpeech_Timer = 100000;
                }
            }
            else
                m_uiSpeech_Timer -= uiDiff;
        }
    }
};

// TODO: Intelligent slaves
/*bool GOUse_ball_and_chain(Player* pPlayer, GameObject* pGo)
{
    if (ScriptedInstance* m_pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    if (Creature* pSlave = m_pInstance->GetSingleCreatureFromStorage(NPC_SLAVE_1))
    {
        pSlave->HandleEmote(EMOTE_STATE_NONE);
        DoScriptText(SAY_SPEECH_SYLVANAS4, pSlave);
    }

    return true;
}*/

CreatureAI* GetAI_npc_sylvanas_jaina_pos_start(Creature* pCreature)
{
    return new npc_sylvanas_jaina_pos_startAI (pCreature);
}

void AddSC_pit_of_saron()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->GetAI = &GetAI_npc_sylvanas_jaina_pos_start;
    pNewScript->Name = "npc_slyvanas_jaina_pos_start";
    pNewScript->RegisterSelf();

    /*pNewScript = new Script;
    pNewScript->Name = "go_ball_and_chain";
    pNewScript->pGOUse=&GOUse_ball_and_chain;
    pNewScript->RegisterSelf();*/
}
