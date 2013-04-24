/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
SDName: Ebon_Hold
SD%Complete: 95%  ---- still a few minor bugs here and there
SDComment: Quest support: 12641, 12701, 12848, 12733, 12739(and 12742 to 12750), 12720, 12727, 12698. Special Npc (npc_valkyr_battle_maiden)
ToDo:
SDCategory: Ebon Hold
EndScriptData */

/* ContentData
npc_a_special_surprise
npc_death_knight_initiate
npc_unworthy_initiate_anchor
npc_unworthy_initiate
go_acherus_soul_prison
npc_koltira_deathweaver
Mob scarlet miner
npc_scarlet_miner
mob_scarlet_ghoul
Gothik
npc_eye_of_acherus
npc_mine_cart
scourge_gryphon
npc_valkyr_battle_maiden
npc_crusade_persuaded
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"
#include "TemporarySummon.h"
#include "WorldPacket.h"
#include "Vehicle.h"
// not inuse yet but will be
//#define LESS_LAG // if you do not have a good server and do not want it to be laggy as hell -- uncomment this if you do


/*######
## npc_a_special_surprise
######*/

enum SpecialSurprise
{
    SAY_EXEC_START_1            = -1609025,                 // speech for all
    SAY_EXEC_START_2            = -1609026,
    SAY_EXEC_START_3            = -1609027,
    SAY_EXEC_PROG_1             = -1609028,
    SAY_EXEC_PROG_2             = -1609029,
    SAY_EXEC_PROG_3             = -1609030,
    SAY_EXEC_PROG_4             = -1609031,
    SAY_EXEC_PROG_5             = -1609032,
    SAY_EXEC_PROG_6             = -1609033,
    SAY_EXEC_PROG_7             = -1609034,
    SAY_EXEC_NAME_1             = -1609035,
    SAY_EXEC_NAME_2             = -1609036,
    SAY_EXEC_RECOG_1            = -1609037,
    SAY_EXEC_RECOG_2            = -1609038,
    SAY_EXEC_RECOG_3            = -1609039,
    SAY_EXEC_RECOG_4            = -1609040,
    SAY_EXEC_RECOG_5            = -1609041,
    SAY_EXEC_RECOG_6            = -1609042,
    SAY_EXEC_NOREM_1            = -1609043,
    SAY_EXEC_NOREM_2            = -1609044,
    SAY_EXEC_NOREM_3            = -1609045,
    SAY_EXEC_NOREM_4            = -1609046,
    SAY_EXEC_NOREM_5            = -1609047,
    SAY_EXEC_NOREM_6            = -1609048,
    SAY_EXEC_NOREM_7            = -1609049,
    SAY_EXEC_NOREM_8            = -1609050,
    SAY_EXEC_NOREM_9            = -1609051,
    SAY_EXEC_THINK_1            = -1609052,
    SAY_EXEC_THINK_2            = -1609053,
    SAY_EXEC_THINK_3            = -1609054,
    SAY_EXEC_THINK_4            = -1609055,
    SAY_EXEC_THINK_5            = -1609056,
    SAY_EXEC_THINK_6            = -1609057,
    SAY_EXEC_THINK_7            = -1609058,
    SAY_EXEC_THINK_8            = -1609059,
    SAY_EXEC_THINK_9            = -1609060,
    SAY_EXEC_THINK_10           = -1609061,
    SAY_EXEC_LISTEN_1           = -1609062,
    SAY_EXEC_LISTEN_2           = -1609063,
    SAY_EXEC_LISTEN_3           = -1609064,
    SAY_EXEC_LISTEN_4           = -1609065,
    SAY_PLAGUEFIST              = -1609066,
    SAY_EXEC_TIME_1             = -1609067,
    SAY_EXEC_TIME_2             = -1609068,
    SAY_EXEC_TIME_3             = -1609069,
    SAY_EXEC_TIME_4             = -1609070,
    SAY_EXEC_TIME_5             = -1609071,
    SAY_EXEC_TIME_6             = -1609072,
    SAY_EXEC_TIME_7             = -1609073,
    SAY_EXEC_TIME_8             = -1609074,
    SAY_EXEC_TIME_9             = -1609075,
    SAY_EXEC_TIME_10            = -1609076,
    SAY_EXEC_WAITING            = -1609077,
    EMOTE_DIES                  = -1609078,

    NPC_PLAGUEFIST              = 29053
};

struct MANGOS_DLL_DECL npc_a_special_surpriseAI : public ScriptedAI
{
    npc_a_special_surpriseAI(Creature *pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiExecuteSpeech_Timer;
    uint32 m_uiExecuteSpeech_Counter;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        m_uiExecuteSpeech_Timer = 0;
        m_uiExecuteSpeech_Counter = 0;
        m_playerGuid.Clear();
    }

    bool MeetQuestCondition(Player* pPlayer)
    {
        switch(m_creature->GetEntry())
        {
            case 29061:                                     // Ellen Stanbridge
                if (pPlayer->GetQuestStatus(12742) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29072:                                     // Kug Ironjaw
                if (pPlayer->GetQuestStatus(12748) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29067:                                     // Donovan Pulfrost
                if (pPlayer->GetQuestStatus(12744) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29065:                                     // Yazmina Oakenthorn
                if (pPlayer->GetQuestStatus(12743) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29071:                                     // Antoine Brack
                if (pPlayer->GetQuestStatus(12750) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29032:                                     // Malar Bravehorn
                if (pPlayer->GetQuestStatus(12739) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29068:                                     // Goby Blastenheimer
                if (pPlayer->GetQuestStatus(12745) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29073:                                     // Iggy Darktusk
                if (pPlayer->GetQuestStatus(12749) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29074:                                     // Lady Eonys
                if (pPlayer->GetQuestStatus(12747) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29070:                                     // Valok the Righteous
                if (pPlayer->GetQuestStatus(12746) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
        }

        return false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_playerGuid || pWho->GetTypeId() != TYPEID_PLAYER || !pWho->IsWithinDist(m_creature, INTERACTION_DISTANCE))
            return;

        if (MeetQuestCondition((Player*)pWho))
            m_playerGuid = pWho->GetObjectGuid();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_playerGuid && !m_creature->getVictim() && m_creature->isAlive())
        {
            if (m_uiExecuteSpeech_Timer < uiDiff)
            {
                Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

                if (!pPlayer)
                {
                    Reset();
                    return;
                }

                //TODO: simplify text's selection

                switch(pPlayer->getRace())
                {
                    case RACE_HUMAN:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_5, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_5, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_7, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_6, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_ORC:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_6, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_7, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_8, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_8, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_DWARF:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_2, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_2, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_3, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_2, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_5, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_2, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_3, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_NIGHTELF:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_5, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_6, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_2, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_7, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_UNDEAD:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_3, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_4, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_3, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_1, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_3, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_4, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_TAUREN:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_5, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_8, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_9, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_9, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_GNOME:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_4, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_4, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_6, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_5, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_TROLL:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_3, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_7, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_2, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_6, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_9, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_10, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_4, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_10, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_BLOODELF:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            //case 5: //unknown
                            case 6: DoScriptText(SAY_EXEC_THINK_3, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_1, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_DRAENEI:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_2, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_1, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_4, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_2, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->SetDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                }

                if (m_uiExecuteSpeech_Counter >= 9)
                    m_uiExecuteSpeech_Timer = 15000;
                else
                    m_uiExecuteSpeech_Timer = 7000;

                ++m_uiExecuteSpeech_Counter;
            }
            else
                m_uiExecuteSpeech_Timer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_a_special_surprise(Creature* pCreature)
{
    return new npc_a_special_surpriseAI(pCreature);
}

/*######
## npc_death_knight_initiate
######*/

enum
{
    SAY_DUEL_A                  = -1609016,
    SAY_DUEL_B                  = -1609017,
    SAY_DUEL_C                  = -1609018,
    SAY_DUEL_D                  = -1609019,
    SAY_DUEL_E                  = -1609020,
    SAY_DUEL_F                  = -1609021,
    SAY_DUEL_G                  = -1609022,
    SAY_DUEL_H                  = -1609023,
    SAY_DUEL_I                  = -1609024,

    SPELL_DUEL                  = 52996,
    SPELL_DUEL_TRIGGERED        = 52990,
    SPELL_DUEL_VICTORY          = 52994,
    SPELL_DUEL_FLAG             = 52991,

    SPELL_BLOOD_STRIKE_DUEL     = 52374,
    SPELL_DEATH_COIL_DUEL       = 52375,
    SPELL_ICY_TOUCH_DUEL        = 52372,
    SPELL_PLAGUE_STRIKE_DUEL    = 52373,

    QUEST_DEATH_CHALLENGE       = 12733,
    FACTION_HOSTILE             = 2068
};

int32 m_auiRandomSay[] =
{
    SAY_DUEL_A, SAY_DUEL_B, SAY_DUEL_C, SAY_DUEL_D, SAY_DUEL_E, SAY_DUEL_F, SAY_DUEL_G, SAY_DUEL_H, SAY_DUEL_I
};

#define GOSSIP_ACCEPT_DUEL      "I challenge you, death knight!"

struct MANGOS_DLL_DECL npc_death_knight_initiateAI : public ScriptedAI
{
    npc_death_knight_initiateAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_duelerGuid;
    uint32 m_uiDuelTimer;
    bool m_bIsDuelInProgress;
    uint32 m_uiBloodStrike_Timer;
    uint32 m_uiDeathCoil_Timer;
    uint32 m_uiIcyTouch_Timer;
    uint32 m_uiPlagueStrike_Timer;

    void Reset()
    {
        if (m_creature->getFaction() != m_creature->GetCreatureInfo()->faction_A)
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);

        m_duelerGuid.Clear();
        m_uiDuelTimer = 5000;
        m_bIsDuelInProgress = false;
        m_uiBloodStrike_Timer = 4000;
        m_uiDeathCoil_Timer = 6000;
        m_uiIcyTouch_Timer = 2000;
        m_uiPlagueStrike_Timer = 5000;
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (!m_bIsDuelInProgress && pSpell->Id == SPELL_DUEL_TRIGGERED && pCaster->GetTypeId() == TYPEID_PLAYER)
        {
            m_duelerGuid = pCaster->GetObjectGuid();
            m_bIsDuelInProgress = true;
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bIsDuelInProgress && uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;

            if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_duelerGuid))
                m_creature->CastSpell(pPlayer, SPELL_DUEL_VICTORY, true);

            //possibly not evade, but instead have end sequenze
            EnterEvadeMode();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_bIsDuelInProgress)
            {
                if (m_uiDuelTimer < uiDiff)
                {
                    m_creature->setFaction(FACTION_HOSTILE);

                    if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_duelerGuid))
                        AttackStart(pPlayer);
                }
                else
                    m_uiDuelTimer -= uiDiff;
            }
            return;
        }

        if (m_uiBloodStrike_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_BLOOD_STRIKE_DUEL);
            m_uiBloodStrike_Timer = 9000;
        }
        else
            m_uiBloodStrike_Timer -= uiDiff;

        if (m_uiDeathCoil_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_DEATH_COIL_DUEL);
            m_uiDeathCoil_Timer = 8000;
        }
        else
            m_uiDeathCoil_Timer -= uiDiff;

        if (m_uiIcyTouch_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_ICY_TOUCH_DUEL);
            m_uiIcyTouch_Timer = 8000;
        }
        else
            m_uiIcyTouch_Timer -= uiDiff;

        if (m_uiPlagueStrike_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(),SPELL_PLAGUE_STRIKE_DUEL);
            m_uiPlagueStrike_Timer = 8000;
        }
        else
            m_uiPlagueStrike_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_death_knight_initiate(Creature* pCreature)
{
    return new npc_death_knight_initiateAI(pCreature);
}

bool GossipHello_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_DEATH_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ACCEPT_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(13433, pCreature->GetObjectGuid());
        return true;
    }
    return false;
}

bool GossipSelect_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_death_knight_initiateAI* pInitiateAI = dynamic_cast<npc_death_knight_initiateAI*>(pCreature->AI()))
        {
            if (pInitiateAI->m_bIsDuelInProgress)
                return true;
        }

        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);

        int32 uiSayId = rand()% (sizeof(m_auiRandomSay)/sizeof(int32));
        DoScriptText(m_auiRandomSay[uiSayId], pCreature, pPlayer);

        pCreature->CastSpell(pPlayer, SPELL_DUEL, false);
        pCreature->CastSpell(pPlayer, SPELL_DUEL_FLAG, true);
    }
    return true;
}

/*######
## npc_koltira_deathweaver
######*/

enum eKoltira
{
    SAY_BREAKOUT1                   = -1609079,
    SAY_BREAKOUT2                   = -1609080,
    SAY_BREAKOUT3                   = -1609081,
    SAY_BREAKOUT4                   = -1609082,
    SAY_BREAKOUT5                   = -1609083,
    SAY_BREAKOUT6                   = -1609084,
    SAY_BREAKOUT7                   = -1609085,
    SAY_BREAKOUT8                   = -1609086,
    SAY_BREAKOUT9                   = -1609087,
    SAY_BREAKOUT10                  = -1609088,

    SPELL_KOLTIRA_TRANSFORM         = 52899,
    SPELL_ANTI_MAGIC_ZONE           = 52894,

    QUEST_BLOODY_BREAKOUT           = 12727,

    NPC_CRIMSON_ACOLYTE             = 29007,
    NPC_HIGH_INQUISITOR_VALROTH     = 29001,
    NPC_KOLTIRA_ALT                 = 28447,
    NPC_KOLTIRA                     = 28912,

    MODEL_DEATH_KNIGHT_MOUNT        = 25278
};

struct MANGOS_DLL_DECL npc_koltira_deathweaverAI : public npc_escortAI
{
    npc_koltira_deathweaverAI(Creature *pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiWave;
    uint32 m_uiWave_Timer;
    ObjectGuid m_valrothGuid;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiWave = 0;
            m_uiWave_Timer = 3000;
            m_creature->UpdateEntry(NPC_KOLTIRA);  // needs to go back to regular entry at reset
            m_valrothGuid.Clear();
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                DoScriptText(SAY_BREAKOUT1, m_creature);
                break;
            case 1:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 2:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                DoCast(m_creature, SPELL_KOLTIRA_TRANSFORM);   // spell to show graphic changes
                break;
            case 3:
                SetEscortPaused(true);   // needs to pause wp movement
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                DoScriptText(SAY_BREAKOUT2, m_creature);
                m_creature->UpdateEntry(NPC_KOLTIRA_ALT);  // actual change // used to update to npc w antimagic zone template aura read sql for more notes
                break;
            case 4:
                SetRun(true);
                break;
            case 9:
                m_creature->Mount(MODEL_DEATH_KNIGHT_MOUNT);
                break;
            case 10:
                m_creature->Unmount();
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Player* pPlayer = GetPlayerForEscort())
            pSummoned->AI()->AttackStart(pPlayer);

        if (pSummoned->GetEntry() == NPC_HIGH_INQUISITOR_VALROTH)
            m_valrothGuid = pSummoned->GetObjectGuid();
    }

    void SummonAcolyte(uint32 uiAmount)
    {
        for(uint32 i = 0; i < uiAmount; ++i)
            m_creature->SummonCreature(NPC_CRIMSON_ACOLYTE, 1642.329f, -6045.818f, 127.583f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if(m_uiWave < 4 && !m_creature->HasAura(SPELL_ANTI_MAGIC_ZONE))
               DoCast(m_creature, SPELL_ANTI_MAGIC_ZONE, true);

            if (m_uiWave_Timer < uiDiff)
            {
                switch(m_uiWave)
                {
                    case 0:
                        SetCombatMovement(false); // needed or he moves and doesnt stay in place
                        DoScriptText(SAY_BREAKOUT3, m_creature);
                        SummonAcolyte(3);
                        m_uiWave_Timer = 20000;
                        break;
                    case 1:
                        SetCombatMovement(false);  // needed or he moves and doesnt stay in place
                        DoScriptText(SAY_BREAKOUT4, m_creature);
                        SummonAcolyte(3);
                        m_uiWave_Timer = 20000;
                        break;
                    case 2:
                        SetCombatMovement(false);  // needed or he moves and doesnt stay in place
                        DoScriptText(SAY_BREAKOUT5, m_creature);
                        SummonAcolyte(4);
                        m_uiWave_Timer = 20000;
                        break;
                    case 3:
                        SetCombatMovement(false);  // needed or he moves and doesnt stay in place
                        DoScriptText(SAY_BREAKOUT6, m_creature);
                        m_creature->SummonCreature(NPC_HIGH_INQUISITOR_VALROTH, 1642.329f, -6045.818f, 127.583f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 1000);
                        m_uiWave_Timer = 1000;
                        break;
                   // missing script text 7 =  SAY_BREAKOUT7
                    case 4:
                    {
                        Creature* pTemp = m_creature->GetMap()->GetCreature(m_valrothGuid);  // var to get valroth 's guid

                        if (!pTemp || !pTemp->isAlive())
                        {
                            DoScriptText(SAY_BREAKOUT8, m_creature);
                            m_uiWave_Timer = 5000;
                        }
                        else
                        {
                            m_uiWave_Timer = 2500;
                            return;                         //return, we don't want m_uiWave to increment now
                        }
                        break;
                    }
                    case 5:
                        DoScriptText(SAY_BREAKOUT9, m_creature);
                        SetCombatMovement(true);    // OK,OK deathweaver do your thing
                        m_creature->RemoveAurasDueToSpell(SPELL_ANTI_MAGIC_ZONE);  // this will remove the aura before he takes off
                        m_uiWave_Timer = 2500;
                        break;
                    case 6:
                        DoScriptText(SAY_BREAKOUT10, m_creature);
                        SetEscortPaused(false);  // moving on wp is ok now
                        break;
                }

                ++m_uiWave;
            }
            else
                m_uiWave_Timer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_koltira_deathweaver(Creature* pCreature)
{
    return new npc_koltira_deathweaverAI(pCreature);
}

bool QuestAccept_npc_koltira_deathweaver(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BLOODY_BREAKOUT)
    {
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        if (npc_koltira_deathweaverAI* pEscortAI = dynamic_cast<npc_koltira_deathweaverAI*>(pCreature->AI()))
            pEscortAI->Start(false, pPlayer, pQuest);
    }
    return true;
}

/*######
## npc_unworthy_initiate_anchor
######*/

enum
{
    SAY_START                       = -1609000,             // 8 texts in total, GetTextId() generates random with this as base
    SAY_AGGRO                       = -1609008,             // 8 texts in total, GetTextId() generates random with this as base

    //SPELL_CHAINED_PESANT_LH         = 54602,              // not used. possible it determine side, where to go get "weapon"
    //SPELL_CHAINED_PESANT_RH         = 54610,
    SPELL_CHAINED_PESANT_CHEST      = 54612,
    SPELL_CHAINED_PESANT_BREATH     = 54613,
    SPELL_INITIATE_VISUAL           = 51519,

    SPELL_BLOOD_STRIKE              = 52374,
    SPELL_DEATH_COIL                = 52375,
    SPELL_ICY_TOUCH                 = 52372,
    SPELL_PLAGUE_STRIKE             = 52373,

    NPC_ANCHOR                      = 29521,
    FACTION_MONSTER                 = 16,

    PHASE_INACTIVE_OR_COMBAT        = 0,
    PHASE_DRESSUP                   = 1,
    PHASE_ACTIVATE                  = 2
};

struct DisplayToSpell
{
    uint32 m_uiDisplayId;
    uint32 m_uiSpellToNewDisplay;
};

DisplayToSpell m_aDisplayToSpell[] =
{
    {25354, 51520},                                         // human M
    {25355, 51534},                                         // human F
    {25356, 51538},                                         // dwarf M
    {25357, 51541},                                         // draenei M
    {25358, 51535},                                         // nelf M
    {25359, 51539},                                         // gnome M
    {25360, 51536},                                         // nelf F
    {25361, 51537},                                         // dwarf F
    {25362, 51540},                                         // gnome F
    {25363, 51542},                                         // draenei F
    {25364, 51543},                                         // orc M
    {25365, 51546},                                         // troll M
    {25366, 51547},                                         // tauren M
    {25367, 51549},                                         // forsaken M
    {25368, 51544},                                         // orc F
    {25369, 51552},                                         // belf F
    {25370, 51545},                                         // troll F
    {25371, 51548},                                         // tauren F
    {25372, 51550},                                         // forsaken F
    {25373, 51551}                                          // belf M
};

struct MANGOS_DLL_DECL npc_unworthy_initiate_anchorAI : public ScriptedAI
{
    npc_unworthy_initiate_anchorAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_myInitiateGuid;
    ObjectGuid m_myPrisonGuid;

    void Reset() {}

    void NotifyMe(Unit* pSource, GameObject* pGo)
    {
        m_myPrisonGuid = pGo->GetObjectGuid();
        Creature* pInitiate = m_creature->GetMap()->GetCreature(m_myInitiateGuid);

        if (pInitiate && pSource)
        {
            pInitiate->SetLootRecipient(pSource);
            m_creature->CastSpell(pInitiate,SPELL_CHAINED_PESANT_BREATH,true);
        }
    }

    void RegisterCloseInitiate(Creature* pCreature)
    {
        m_myInitiateGuid = pCreature->GetObjectGuid();
    }

    void ResetPrison()
    {
        if (GameObject* pPrison = m_creature->GetMap()->GetGameObject(m_myPrisonGuid))
            pPrison->ResetDoorOrButton();
    }
};

CreatureAI* GetAI_npc_unworthy_initiate_anchor(Creature* pCreature)
{
    return new npc_unworthy_initiate_anchorAI(pCreature);
}

/*######
## npc_unworthy_initiate
######*/

struct MANGOS_DLL_DECL npc_unworthy_initiateAI : public ScriptedAI
{
    npc_unworthy_initiateAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pToTransform = NULL;

        uint32 uiDisplayCount = sizeof(m_aDisplayToSpell)/sizeof(DisplayToSpell);

        for (uint8 i=0; i<uiDisplayCount; ++i)
        {
            // check if we find a match, if not, it's NULL and produce error when used
            if (m_aDisplayToSpell[i].m_uiDisplayId == pCreature->GetDisplayId())
            {
                m_pToTransform = &m_aDisplayToSpell[i];
                break;
            }
        }

        m_uiNormFaction = pCreature->getFaction();
        Reset();
    }

    DisplayToSpell* m_pToTransform;

    ObjectGuid m_myAnchorGuid;
    uint32 m_uiNormFaction;
    uint32 m_uiAnchorCheckTimer;
    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBloodStrike_Timer;
    uint32 m_uiDeathCoil_Timer;
    uint32 m_uiIcyTouch_Timer;
    uint32 m_uiPlagueStrike_Timer;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormFaction)
            m_creature->setFaction(m_uiNormFaction);

        m_uiAnchorCheckTimer = 5000;
        m_uiPhase = PHASE_INACTIVE_OR_COMBAT;
        m_uiPhaseTimer = 7500;
        m_uiBloodStrike_Timer = 4000;
        m_uiDeathCoil_Timer = 6000;
        m_uiIcyTouch_Timer = 2000;
        m_uiPlagueStrike_Timer = 5000;
    }

    void JustReachedHome()
    {
        SetAnchor();

        if (Creature* pAnchor = GetAnchor())
        {
            if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
                pAnchorAI->ResetPrison();
        }
    }

    void JustRespawned()
    {
        if (Creature* pAnchor = GetAnchor())
        {
            if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
                pAnchorAI->ResetPrison();
        }

        Reset();
    }

    int32 GetTextId()
    {
        return m_uiPhase == PHASE_DRESSUP ? SAY_START-rand()%8 : SAY_AGGRO-rand()%8;
    }

    Creature* GetAnchor()
    {
        if (m_myAnchorGuid)
            return m_creature->GetMap()->GetCreature(m_myAnchorGuid);
        else
            return GetClosestCreatureWithEntry(m_creature, NPC_ANCHOR, INTERACTION_DISTANCE*2);
    }

    void SetAnchor()
    {
        if (Creature* pAnchor = GetAnchor())
        {
            if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
                pAnchorAI->RegisterCloseInitiate(m_creature);

            pAnchor->CastSpell(m_creature, SPELL_CHAINED_PESANT_CHEST, false);
            m_myAnchorGuid = pAnchor->GetObjectGuid();

            m_uiAnchorCheckTimer = 0;
            return;
        }

        m_uiAnchorCheckTimer = 5000;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_CHAINED_PESANT_BREATH)
        {
            pCaster->InterruptNonMeleeSpells(true);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            m_uiPhase = PHASE_DRESSUP;

            if (Player* pSource = m_creature->GetLootRecipient())
                DoScriptText(GetTextId(), m_creature, pSource);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiAnchorCheckTimer)
        {
            if (m_uiAnchorCheckTimer <= uiDiff)
                SetAnchor();
            else
                m_uiAnchorCheckTimer -= uiDiff;
        }

        if (m_uiPhase == PHASE_INACTIVE_OR_COMBAT)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (m_uiBloodStrike_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_BLOOD_STRIKE);
                m_uiBloodStrike_Timer = 9000;
            }
            else
                m_uiBloodStrike_Timer -= uiDiff;

            if (m_uiDeathCoil_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_DEATH_COIL);
                m_uiDeathCoil_Timer = 8000;
            }
            else
                m_uiDeathCoil_Timer -= uiDiff;

            if (m_uiIcyTouch_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_ICY_TOUCH);
                m_uiIcyTouch_Timer = 8000;
            }
            else
                m_uiIcyTouch_Timer -= uiDiff;

            if (m_uiPlagueStrike_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature->getVictim(),SPELL_PLAGUE_STRIKE);
                m_uiPlagueStrike_Timer = 8000;
            }
            else
                m_uiPlagueStrike_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if (m_uiPhaseTimer < uiDiff)
            {
                if (m_uiPhase == PHASE_DRESSUP)
                {
                    if (m_pToTransform)
                    {
                        m_creature->CastSpell(m_creature, m_pToTransform->m_uiSpellToNewDisplay, true);
                        m_creature->CastSpell(m_creature, SPELL_INITIATE_VISUAL, false);
                    }
                    else
                        error_log("SD2: npc_unworthy_initiate does not have any spell associated with model");

                    m_uiPhase = PHASE_ACTIVATE;
                }
                else
                {
                    m_creature->setFaction(FACTION_MONSTER);

                    m_uiPhase = PHASE_INACTIVE_OR_COMBAT;

                    if (Player* pTarget = m_creature->GetLootRecipient())
                    {
                        DoScriptText(GetTextId(), m_creature, pTarget);
                        AttackStart(pTarget);
                    }
                }

                m_uiPhaseTimer = 5000;
            }
            else
                m_uiPhaseTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_unworthy_initiate(Creature* pCreature)
{
    return new npc_unworthy_initiateAI(pCreature);
}

/*######
## go_acherus_soul_prison
######*/

bool GOUse_go_acherus_soul_prison(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pAnchor = GetClosestCreatureWithEntry(pGo, NPC_ANCHOR, INTERACTION_DISTANCE))
    {
        if (npc_unworthy_initiate_anchorAI* pAnchorAI = dynamic_cast<npc_unworthy_initiate_anchorAI*>(pAnchor->AI()))
            pAnchorAI->NotifyMe(pPlayer, pGo);
    }

    return false;
}

/*######
## npc_eye_of_acherus
######*/

enum eEyeOfAcherus
{
    DISPLAYID_EYE_HUGE      = 26320,
    DISPLAYID_EYE_SMALL     = 25499,

    SPELL_EYE_PHASEMASK     = 70889,
    SPELL_EYE_VISUAL        = 51892,
    //SPELL_EYE_FL_BOOST_RUN  = 51923,
    SPELL_EYE_FL_BOOST_FLY  = 51890,
    SPELL_EYE_CONTROL       = 51852,

    TEXT_EYE_UNDER_CONTROL  = -1666452,
    TEXT_EYE_LAUNCHED       = -1666451,
};

struct MANGOS_DLL_DECL npc_eye_of_acherusAI : public ScriptedAI
{
    npc_eye_of_acherusAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool m_isActive;

    void Reset()
    {
        m_creature->SetDisplayId(DISPLAYID_EYE_HUGE);
        m_isActive = false;
    }

    void AttackStart(Unit *pWho)
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
    }

    void JustDied(Unit* pKiller)
    {
        if (Unit* pCharmer = m_creature->GetCharmer())
            pCharmer->RemoveAurasDueToSpell(SPELL_EYE_CONTROL);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
       if (uiType != POINT_MOTION_TYPE || uiPointId != 0)
            return;

        DoScriptText(TEXT_EYE_UNDER_CONTROL, m_creature);
        m_creature->SetDisplayId(DISPLAYID_EYE_SMALL);
        m_creature->CastSpell(m_creature, SPELL_EYE_FL_BOOST_FLY, true);
    }

    void AttackedBy(Unit* pAttacker)
    {
        // called on remove SPELL_AURA_MOD_POSSESS
        if (!m_creature->isCharmed() && pAttacker->GetTypeId() == TYPEID_PLAYER)
        {
            pAttacker->RemoveAurasDueToSpell(SPELL_EYE_CONTROL);
//            m_creature->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->isCharmed())
        {
            if (!m_isActive)
            {
                m_creature->CastSpell(m_creature, SPELL_EYE_PHASEMASK, true);
                m_creature->CastSpell(m_creature, SPELL_EYE_VISUAL, true);
                //m_creature->CastSpell(m_creature, SPELL_EYE_FL_BOOST_FLY, true);
                //m_creature->SetLevitate(true);   // will be uncommented if any troubles with flying inhabit 4
                m_creature->SetWalk(false);
                m_creature->SetSpeedRate(MOVE_RUN, 5.0f);
                DoScriptText(TEXT_EYE_LAUNCHED, m_creature);
                m_creature->GetMotionMaster()->MovePoint(0,1750.8276f, -5873.788f, 147.2266f);
                m_isActive = true;
            }
        } else m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_npc_eye_of_acherus(Creature* pCreature)
{
    return new npc_eye_of_acherusAI(pCreature);
}

/*######
## Mob scarlet miner
######*/
enum scarletminer
{
    QUEST_GIFT_THAT_KEEPS_GIVING        = 12698,
    SPELL_GIFT_OF_THE_HARVESTER_MISSILE = 52481,
    SPELL_SUMMOM_GHOUL                    = 52490,
    SPELL_SUMMON_GHOST                    = 52505,
};

struct MANGOS_DLL_DECL mob_scarlet_minerAI : public ScriptedAI
{
    mob_scarlet_minerAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        // NEEDS CORRECTED/OR SUPPORT IN CORE SO HACK CAN BE REMOVED
        // hack spell 52481
        // 35% chance to summon ghoul
        if (SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_GIFT_OF_THE_HARVESTER_MISSILE))
        {
            if (SpellEffectEntry const * effect = TempSpell->GetSpellEffect(EFFECT_INDEX_0))
            {
                if (effect->EffectImplicitTargetB != 16)
                {
                    const_cast<SpellEffectEntry*>(effect)->EffectImplicitTargetB = 16;
                    if (effect = TempSpell->GetSpellEffect(EFFECT_INDEX_1))
                        const_cast<SpellEffectEntry*>(effect)->EffectImplicitTargetB = 87;
                    if (effect = TempSpell->GetSpellEffect(EFFECT_INDEX_2))
                        const_cast<SpellEffectEntry*>(effect)->EffectImplicitTargetB = 16;
                }
            }
        }
    }

    void Reset() {}

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && pSpell->Id == SPELL_GIFT_OF_THE_HARVESTER_MISSILE)
        {
            if(((Player*)pCaster)->GetQuestStatus(QUEST_GIFT_THAT_KEEPS_GIVING) == QUEST_STATUS_INCOMPLETE)
            {
                if (rand()%100 < 35)    //35% chance to summon ghoul
                {
                    pCaster->CastSpell(m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),SPELL_SUMMOM_GHOUL, true);
                }
                else
                {
                     pCaster->CastSpell(m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),SPELL_SUMMON_GHOST, true);
                }

                      m_creature->SetDeathState(JUST_DIED);
                      m_creature->RemoveCorpse();
             }
        }
    }
};

CreatureAI* GetAI_mob_scarlet_miner(Creature* pCreature)
{
    return new mob_scarlet_minerAI (pCreature);
};

/*######
## mob_scarlet_ghoul
######*/

enum
{
    SPELL_HARVESTER_PING_DUMMY  = 52514,
    ENTRY_GOTHIK                = 28658,

    SAY_SCARLET_GHOUL_SPAWN1    = -1609300,
    SAY_SCARLET_GHOUL_SPAWN2    = -1609301,
    SAY_SCARLET_GHOUL_SPAWN3    = -1609302,
    SAY_SCARLET_GHOUL_SPAWN4    = -1609303,
    SAY_SCARLET_GHOUL_SPAWN5    = -1609304,
    SAY_SCARLET_GHOUL_SPAWN6    = -1609305,

    SAY_SCARLET_GOTHIK1         = -1609306,
    SAY_SCARLET_GOTHIK2         = -1609307,
    SAY_SCARLET_GOTHIK3         = -1609308,
    SAY_SCARLET_GOTHIK4         = -1609309,
    SAY_SCARLET_GOTHIK5         = -1609310,
};

struct MANGOS_DLL_DECL mob_scarlet_ghoulAI : public ScriptedAI
{
    mob_scarlet_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsSpawned = false;
        fDist = (float)urand(1, 5);
        m_creatorGuid = m_creature->GetCreatorGuid();
        if (Player* pOwner = m_creature->GetMap()->GetPlayer(m_creatorGuid) )
            fAngle = m_creature->GetAngle(pOwner);

        Reset();
    }


    Unit* pTarget;

    ObjectGuid m_creatorGuid;
    ObjectGuid m_targetGUID;
    ObjectGuid m_harvesterGUID;

    uint32 m_uiWaitForThrowTimer;

    bool m_bWaitForThrow;
    bool m_bIsSpawned;

    float fAngle;
    float fDist;

    void Reset()
    {
        m_uiWaitForThrowTimer   = 3000;
        m_bWaitForThrow         = false;
        pTarget                 = NULL;
        //m_creatorGuid.Clear();
        m_targetGUID.Clear();
        m_harvesterGUID.Clear();
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!m_bWaitForThrow && pWho->GetEntry() == ENTRY_GOTHIK && m_creature->GetDistance(pWho) < 15.0f)
        {
            m_harvesterGUID = pWho->GetObjectGuid();

            if (Player* pOwner = m_creature->GetMap()->GetPlayer(m_creatorGuid) )
            {
                pOwner->KilledMonsterCredit(m_creature->GetEntry(), m_creature->GetObjectGuid() );
                // this will execute if m_creature survived Harvester's wrath
                float x, y, z, o;
                o = float(urand(53, 57))/10.0f;
                pWho->GetNearPoint(pWho, x, y, z, pWho->GetObjectBoundingRadius(), 5.0f, o);
                m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                m_bWaitForThrow = true;
            }
        }
    }

    void AttackStart(Unit *pWho) { return; }

    void UpdateAI(uint32 const uiDiff)
    {
        if (!m_bIsSpawned)
        {
            DoScriptText(SAY_SCARLET_GHOUL_SPAWN1 - urand(0, 5), m_creature);
            m_bIsSpawned = true;
        }

        if (m_bWaitForThrow)
        {
            if (m_uiWaitForThrowTimer <= uiDiff)
            {
                if (Creature* pGothik = m_creature->GetMap()->GetCreature(m_harvesterGUID) )
                {
                    if (pGothik->AI()->DoCastSpellIfCan(m_creature, roll_chance_i(50) ? 52519 : 52521) == CAST_OK)
                        DoScriptText(SAY_SCARLET_GOTHIK1 - urand(0, 4), pGothik);

                    m_uiWaitForThrowTimer = 5000;
                    m_creature->KnockBackFrom(pGothik, 15.0, 5.0);
                    m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
                }
                else m_bWaitForThrow = false;
            }
            else m_uiWaitForThrowTimer -= uiDiff;
            return;
        }

        Player* pOwner = m_creature->GetMap()->GetPlayer(m_creatorGuid);
        if (!pOwner || !pOwner->IsInWorld())
        {
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
            return;
        }

        if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
        {
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMotionMaster()->MoveFollow(pOwner, fDist, fAngle);
        }
    }
};

CreatureAI* GetAI_mob_scarlet_ghoul(Creature* pCreature)
{
    return new mob_scarlet_ghoulAI(pCreature);
};

// quest 12801 from Ckegg
/*######
## Npc Highlord Darion Mograine
######*/

void UpdateWorldState(Map *map, uint32 id, uint32 state)
{
    Map::PlayerList const& players = map->GetPlayers();

    if (!players.isEmpty())
    {
        for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if (Player* pPlayer = itr->getSource())
                pPlayer->SendUpdateWorldState(id,state);
        }
    }
}

enum mograine
{
    ENCOUNTER_DK_NUMBER               = 5,  // how many player queue to start the quest , or -
    ENCOUNTER_DK_TIMER                = 10, // *every 5 minutes. These have to be done in instance data
    ENCOUNTER_DEFENDER_NUMBER         = 20, // how many of defender
    ENCOUNTER_EARTHSHATTER_NUMBER     = 20, // how many of earthshatter
    ENCOUNTER_ABOMINATION_NUMBER      = 3,  // how many of abomination
    ENCOUNTER_BEHEMOTH_NUMBER         = 2,  // how many of behemoth
    ENCOUNTER_GHOUL_NUMBER            = 10, // how many of ghoul
    ENCOUNTER_WARRIOR_NUMBER          = 2,  // how many of warrior

    ENCOUNTER_TOTAL_DAWN              = 300,  // Total number
    ENCOUNTER_TOTAL_SCOURGE           = 10000,

    WORLD_STATE_REMAINS               = 3592,
    WORLD_STATE_COUNTDOWN             = 3603,
    WORLD_STATE_EVENT_BEGIN           = 3605,

    SAY_LIGHT_OF_DAWN01               = -1609201, // pre text
    SAY_LIGHT_OF_DAWN02               = -1609202,
    SAY_LIGHT_OF_DAWN03               = -1609203,
    SAY_LIGHT_OF_DAWN04               = -1609204, // intro
    SAY_LIGHT_OF_DAWN05               = -1609205,
    SAY_LIGHT_OF_DAWN06               = -1609206,
    SAY_LIGHT_OF_DAWN07               = -1609207, // During the fight - Korfax, Champion of the Light
    SAY_LIGHT_OF_DAWN08               = -1609208, // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN09               = -1609209, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN10               = -1609210,
    SAY_LIGHT_OF_DAWN11               = -1609211,
    SAY_LIGHT_OF_DAWN12               = -1609212,
    SAY_LIGHT_OF_DAWN13               = -1609213,
    SAY_LIGHT_OF_DAWN14               = -1609214,
    SAY_LIGHT_OF_DAWN15               = -1609215,
    SAY_LIGHT_OF_DAWN16               = -1609216,
    SAY_LIGHT_OF_DAWN17               = -1609217,
    SAY_LIGHT_OF_DAWN18               = -1609218,
    SAY_LIGHT_OF_DAWN19               = -1609219,
    SAY_LIGHT_OF_DAWN20               = -1609220,
    SAY_LIGHT_OF_DAWN21               = -1609221,
    SAY_LIGHT_OF_DAWN22               = -1609222,
    SAY_LIGHT_OF_DAWN23               = -1609223,
    SAY_LIGHT_OF_DAWN24               = -1609224,
    SAY_LIGHT_OF_DAWN25               = -1609225, // After the fight
    SAY_LIGHT_OF_DAWN26               = -1609226, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN27               = -1609227, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN28               = -1609228, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN29               = -1609229, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN30               = -1609230, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN31               = -1609231, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN32               = -1609232, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN33               = -1609233, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN34               = -1609234, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN35               = -1609235, // Darion Mograine
    SAY_LIGHT_OF_DAWN36               = -1609236, // Darion Mograine
    SAY_LIGHT_OF_DAWN37               = -1609237, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN38               = -1609238, // Darion Mograine
    SAY_LIGHT_OF_DAWN39               = -1609239, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN40               = -1609240, // Darion Mograine
    SAY_LIGHT_OF_DAWN41               = -1609241, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN42               = -1609242, // Highlord Alexandros Mograine
    SAY_LIGHT_OF_DAWN43               = -1609243, // The Lich King
    SAY_LIGHT_OF_DAWN44               = -1609244, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN45               = -1609245, // The Lich King
    SAY_LIGHT_OF_DAWN46               = -1609246, // The Lich King
    SAY_LIGHT_OF_DAWN47               = -1609247, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN48               = -1609248, // The Lich King
    SAY_LIGHT_OF_DAWN49               = -1609249, // The Lich King
    SAY_LIGHT_OF_DAWN50               = -1609250, // Lord Maxwell Tyrosus
    SAY_LIGHT_OF_DAWN51               = -1609251, // The Lich King
    SAY_LIGHT_OF_DAWN52               = -1609252, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN53               = -1609253, // Highlord Darion Mograine
    SAY_LIGHT_OF_DAWN54               = -1609254, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN55               = -1609255, // The Lich King
    SAY_LIGHT_OF_DAWN56               = -1609256, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN57               = -1609257, // The Lich King
    SAY_LIGHT_OF_DAWN58               = -1609258, // The Lich King
    SAY_LIGHT_OF_DAWN59               = -1609259, // The Lich King
    SAY_LIGHT_OF_DAWN60               = -1609260, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN61               = -1609261, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN62               = -1609262, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN63               = -1609263, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN64               = -1609264, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN65               = -1609265, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN66               = -1609266, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN67               = -1609267, // Highlord Tirion Fordring
    SAY_LIGHT_OF_DAWN68               = -1609268, // Highlord Darion Mograine

    EMOTE_LIGHT_OF_DAWN01             = -1609269,  // Emotes
    EMOTE_LIGHT_OF_DAWN02             = -1609270,
    EMOTE_LIGHT_OF_DAWN03             = -1609271,
    EMOTE_LIGHT_OF_DAWN04             = -1609272,
    EMOTE_LIGHT_OF_DAWN05             = -1609273,
    EMOTE_LIGHT_OF_DAWN06             = -1609274,
    EMOTE_LIGHT_OF_DAWN07             = -1609275,
    EMOTE_LIGHT_OF_DAWN08             = -1609276,
    EMOTE_LIGHT_OF_DAWN09             = -1609277,
    EMOTE_LIGHT_OF_DAWN10             = -1609278,
    EMOTE_LIGHT_OF_DAWN11             = -1609279,
    EMOTE_LIGHT_OF_DAWN12             = -1609280,
    EMOTE_LIGHT_OF_DAWN13             = -1609281,
    EMOTE_LIGHT_OF_DAWN14             = -1609282,
    EMOTE_LIGHT_OF_DAWN15             = -1609283,
    EMOTE_LIGHT_OF_DAWN16             = -1609284,
    EMOTE_LIGHT_OF_DAWN17             = -1609285,
    EMOTE_LIGHT_OF_DAWN18             = -1609286,

    GO_LIGHT_OF_DAWN                  = 191330,
    SPELL_THE_LIGHT_OF_DAWN_Q         = 53606, // quest credit

    // ---- Dark Knight npc --------------------
    // Highlord Darion Mograine
    NPC_HIGHLORD_DARION_MOGRAINE      = 29173,
    SPELL_ANTI_MAGIC_ZONE1            = 52893,
    SPELL_DEATH_STRIKE                = 53639,
    SPELL_DEATH_EMBRACE               = 53635,
    SPELL_ICY_TOUCH1                  = 49723,
    SPELL_THE_LIGHT_OF_DAWN           = 53658,
    SPELL_THE_MIGHT_OF_MOGRAINE       = 53642, // on players when begins
    SPELL_UNHOLY_BLIGHT               = 53640,
    SPELL_ALEXANDROS_MOGRAINE_SPAWN   = 53667,
    SPELL_MOGRAINE_CHARGE             = 53679,
    SPELL_ASHBRINGER                  = 53701,

    // Koltira Deathweaver & Orbaz Bloodbane are using the same abilities
    NPC_KOLTIRA_DEATHWEAVER           = 29199,
    NPC_ORBAZ_BLOODBANE               = 29204, // this guy fleed
    NPC_THASSARIAN                    = 29200, // he also does SPELL_THE_LIGHT_OF_DAWN 53658
    //SPELL_BLOOD_STRIKE                = 52374,
    SPELL_DEATH_GRIP                  = 49576,
    //SPELL_ICY_TOUCH                   = 52372,
    SPELL_PLAGUE_STRIKE1              = 50668,
    // all do SPELL_HERO_AGGRO_AURA    53627

    // Lich King
    NPC_THE_LICH_KING                 = 29183, // show up at end
    SPELL_APOCALYPSE                  = 53210,
    SPELL_TELEPORT_VISUAL             = 52233,
    SPELL_SOUL_FEAST_ALEX             = 53677, // on Alexandros
    SPELL_SOUL_FEAST_TIRION           = 53685, // on Tirion
    SPELL_ICEBOUND_VISAGE             = 53274, // not sure what is it for
    SPELL_REBUKE                      = 53680,

    // others
    NPC_RAMPAGING_ABOMINATION         = 29186,
    SPELL_CLEAVE1                     = 53633,
    SPELL_SCOURGE_HOOK                = 50335,
    SPELL_SCOURGE_AGGRO_AURA          = 53624,

    NPC_FLESH_BEHEMOTH                = 29190, // giant guy
    SPELL_STOMP                       = 53634,
    SPELL_THUNDERCLAP                 = 36706,
    SPELL_HERO_AGGRO_AURA             = 53627,

    NPC_ACHERUS_GHOUL                 = 29219, // just ghoul....
    SPELL_GHOULPLOSION                = 53632,

    NPC_WARRIOR_OF_THE_FROZEN_WASTES  = 29206, // use SPELL_CLEAVE 53631

    NPC_HIGHLORD_ALEXANDROS_MOGRAINE  = 29227, // ghost
    NPC_DARION_MOGRAINE               = 29228, // ghost

    // ---- Dawn npc --------------------
    // Highlord Tirion Fordring
    NPC_HIGHLORD_TIRION_FORDRING      = 29175,
    EQUIP_HIGHLORD_TIRION_FORDRING    = 13262,
    SPELL_LAY_ON_HANDS                = 53778,
    SPELL_REBIRTH_OF_THE_ASHBRINGER   = 53702,
    SPELL_TIRION_CHARGE               = 53705,
    SPELL_TIRION_CHARGE_VISUAL        = 53706,

    // others
    NPC_KORFAX_CHAMPION_OF_THE_LIGHT  = 29176,
    SPELL_CLEAVE                      = 53631,
    SPELL_HEROIC_LEAP                 = 53625,

    NPC_LORD_MAXWELL_TYROSUS          = 29178,
    NPC_LEONID_BARTHALOMEW_THE_REVERED = 29179,
    NPC_DUKE_NICHOLAS_ZVERENHOFF      = 29180,

    NPC_COMMANDER_ELIGOR_DAWNBRINGER  = 29177,
    SPELL_HOLY_LIGHT2                 = 37979,

    NPC_RAYNE                         = 29181,
    SPELL_REJUVENATION                = 20664,
    SPELL_STARFALL                    = 20678,
    SPELL_TRANQUILITY                 = 25817,
    SPELL_WRATH                       = 21807,

    NPC_DEFENDER_OF_THE_LIGHT         = 29174, // also does SPELL_HEROIC_LEAP 53625
    SPELL_HOLY_LIGHT1                 = 29427,
    SPELL_HOLY_STRIKE                 = 53643,
    SPELL_HOLY_WRATH                  = 53638,
    SPELL_UPPERCUT                    = 53629,

    NPC_RIMBLAT_EARTHSHATTER          = 29182,
    SPELL_CHAIN_HEAL                  = 33642,
    SPELL_THUNDER                     = 53630
};

struct Locations
{
    float x, y, z, o;
    uint32 id;
};

static Locations LightofDawnLoc[]=
{
    {2281.335f, -5300.409f, 85.170f, 0.0f},     // 0 Tirion Fordring loc
    {2283.896f, -5287.914f, 83.066f, 1.55f},  // 1 Tirion Fordring loc2
    {2281.461f, -5263.014f, 81.164f, 0.0f},     // 2 Tirion charges
    {2262.277f, -5293.477f, 82.167f, 0.0f},     // 3 Tirion run
    {2270.286f, -5287.73f, 82.262f, 0.0f},      // 4 Tirion relocate
    {2269.511f, -5288.289f, 82.225f, 0.0f},     // 5 Tirion forward
    {2262.277f, -5293.477f, 82.167f, 0.0f},     // 6 Tirion runs to Darion
    {2270.286f, -5287.73f, 82.262f, 0.0f},
    {2269.511f, -5288.289f, 82.225f, 0.0f},
    {2273.205f, -5288.848f, 82.617f, 0.0f},     // 9 Korfax loc1
    {2274.739f, -5287.926f, 82.684f, 0.0f},     // 10 Korfax loc2
    {2253.673f, -5318.004f, 81.724f, 0.0f},     // 11 Korfax kicked
    {2287.028f, -5309.644f, 87.253f, 0.0f},     // 12 Maxwell loc1
    {2286.978f, -5308.025f, 86.83f, 0.0f},      // 13 Maxwell loc2
    {2248.877f, -5307.586f, 82.166f, 0.0f},     // 14 maxwell kicked
    {2278.58f, -5316.933f, 88.319f, 0.0f},      // 15 Eligor loc1
    {2278.535f, -5315.479f, 88.08f, 0.0f},      // 16 Eligor loc2
    {2259.416f, -5304.505f, 82.149f, 0.0f},     // 17 eligor kicked
    {2289.259f, -5280.355f, 82.112f, 0.0f},     // 18 Koltira loc1
    {2289.02f, -5281.985f, 82.207f, 0.0f},      // 19 Koltira loc2
    {2273.289f, -5273.675f, 81.701f, 0.0f},     // 20 Thassarian loc1
    {2273.332f, -5275.544f, 81.849f, 0.0f},     // 21 Thassarian loc2
    {2281.198f, -5257.397f, 80.224f, 4.66f},  // 22 Alexandros loc1
    {2281.156f, -5259.934f, 80.647f, 0.0f},     // 23 Alexandros loc2
    {2281.294f, -5281.895f, 82.445f, 1.35f},  // 24 Darion loc1
    {2281.093f, -5263.013f, 81.125f, 0.0f},     // 25 Darion loc1
    {2281.313f, -5250.282f, 79.322f, 4.69f},  // 26 Lich King spawns
    {2281.523f, -5261.058f, 80.877f, 0.0f},     // 27 Lich king move forwards
    {2272.709f, -5255.552f, 78.226f, 0.0f},     // 28 Lich king kicked
    {2273.972f, -5257.676f, 78.862f, 0.0f}      // 29 Lich king moves forward
};

struct MANGOS_DLL_DECL npc_highlord_darion_mograineAI : public npc_escortAI
{
    npc_highlord_darion_mograineAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
        Reset();
    }

    bool bIsBattle;
    uint32 uiStep;
    uint32 uiPhase_timer;
    uint32 uiFight_duration;
    uint32 uiTotal_dawn;
    uint32 uiTotal_scourge;
    uint32 uiSummon_counter;

    // Darion Mograine
    uint32 uiAnti_magic_zone;
    uint32 uiDeath_strike;
    uint32 uiDeath_embrace;
    uint32 uiIcy_touch;
    uint32 uiUnholy_blight;
    uint32 uiFight_speech;
    uint32 uiSpawncheck;
    uint32 uiTargetcheck;

    // Dawn
    ObjectGuid uiTirionGUID;
    ObjectGuid uiAlexandrosGUID;
    ObjectGuid uiDarionGUID;
    ObjectGuid uiKorfaxGUID;
    ObjectGuid uiMaxwellGUID;
    ObjectGuid uiEligorGUID;
    ObjectGuid uiRayneGUID;
    ObjectGuid uiDefenderGUID[ENCOUNTER_DEFENDER_NUMBER];
    ObjectGuid uiEarthshatterGUID[ENCOUNTER_EARTHSHATTER_NUMBER];

    // Death
    ObjectGuid uiKoltiraGUID;
    ObjectGuid uiOrbazGUID;
    ObjectGuid uiThassarianGUID;
    ObjectGuid uiLichKingGUID;
    ObjectGuid uiAbominationGUID[ENCOUNTER_ABOMINATION_NUMBER];
    ObjectGuid uiBehemothGUID[ENCOUNTER_BEHEMOTH_NUMBER];
    ObjectGuid uiGhoulGUID[ENCOUNTER_GHOUL_NUMBER];
    ObjectGuid uiWarriorGUID[ENCOUNTER_WARRIOR_NUMBER];

    // Misc
    ObjectGuid uiDawnofLightGUID;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            bIsBattle = false;
            uiStep = 0;
            uiPhase_timer = 3000;
            uiFight_duration = 300000; // 5 minutes
            uiTotal_dawn = ENCOUNTER_TOTAL_DAWN;
            uiTotal_scourge = ENCOUNTER_TOTAL_SCOURGE;
            uiSummon_counter = 0;

            uiDawnofLightGUID.Clear();

            uiAnti_magic_zone = 1000 + rand()%5000;
            uiDeath_strike = 5000 + rand()%5000;
            uiDeath_embrace = 5000 + rand()%5000;
            uiIcy_touch = 5000 + rand()%5000;
            uiUnholy_blight = 5000 + rand()%5000;

            uiFight_speech = 15000;
            uiSpawncheck = 1000;
            uiTargetcheck = 10000;

            ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
            m_creature->Mount(25279);

            UpdateWorldState(m_creature->GetMap(), WORLD_STATE_REMAINS, 0);
            //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_COUNTDOWN, 0);
            UpdateWorldState(m_creature->GetMap(), WORLD_STATE_EVENT_BEGIN, 0);

            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                pTemp->SetDeathState(JUST_DIED);
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID))
                pTemp->SetDeathState(JUST_DIED);
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID))
                pTemp->SetDeathState(JUST_DIED);
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEligorGUID))
                pTemp->SetDeathState(JUST_DIED);
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiRayneGUID))
                pTemp->SetDeathState(JUST_DIED);

            uiTirionGUID.Clear();
            uiKorfaxGUID.Clear();
            uiMaxwellGUID.Clear();
            uiEligorGUID.Clear();
            uiRayneGUID.Clear();

            for (uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDefenderGUID[i]))
                    pTemp->SetDeathState(JUST_DIED);
                uiDefenderGUID[i].Clear();
            }
            for (uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEarthshatterGUID[i]))
                    pTemp->SetDeathState(JUST_DIED);
                uiEarthshatterGUID[i].Clear();
            }

            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKoltiraGUID))
                pTemp->SetDeathState(JUST_DIED);
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiOrbazGUID))
                pTemp->SetDeathState(JUST_DIED);
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiThassarianGUID))
                pTemp->SetDeathState(JUST_DIED);
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                pTemp->SetDeathState(JUST_DIED);

            uiKoltiraGUID.Clear();
            uiOrbazGUID.Clear();
            uiThassarianGUID.Clear();
            uiLichKingGUID.Clear();
            for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAbominationGUID[i]))
                    pTemp->SetDeathState(JUST_DIED);
                uiAbominationGUID[i].Clear();
            }
            for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiBehemothGUID[i]))
                    pTemp->SetDeathState(JUST_DIED);
                uiBehemothGUID[i].Clear();
            }
            for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiGhoulGUID[i]))
                    pTemp->SetDeathState(JUST_DIED);
                uiGhoulGUID[i].Clear();
            }
            for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
            {
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiWarriorGUID[i]))
                    pTemp->SetDeathState(JUST_DIED);
                uiWarriorGUID[i].Clear();
            }
        }
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (who == m_creature)
            return;

        if (m_creature->Attack(who, true))
        {
            m_creature->AddThreat(who, 0.0f);
            m_creature->SetInCombatWith(who);
            who->SetInCombatWith(m_creature);
            DoStartMovement(who);
        }
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!who)
            return;

        if (who->isTargetableForAttack() && m_creature->IsHostileTo(who))
            if (m_creature->IsWithinDistInMap(who, 20) && m_creature->IsWithinLOSInMap(who))
                AttackStart(who);
    }

    void WaypointReached(uint32 i)
    {
        switch(i)
        {
            case 0:
                m_creature->SetWalk(false);
                SetEscortPaused(true);
                break;
            case 1:
                SetEscortPaused(true);

                if (GameObject* pGo = GetClosestGameObjectWithEntry(m_creature, GO_LIGHT_OF_DAWN, 100.0f)) // make dawn of light effect off
                {
                    uiDawnofLightGUID = pGo->GetObjectGuid();
                    pGo->SetPhaseMask(0, true);
                }

                SpawnNPC();
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID))
                    DoScriptText(SAY_LIGHT_OF_DAWN07, pTemp);
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID))
                    DoScriptText(SAY_LIGHT_OF_DAWN08, pTemp);

                for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                    NPCChangeTarget(uiGhoulGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                    NPCChangeTarget(uiWarriorGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                    NPCChangeTarget(uiAbominationGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                    NPCChangeTarget(uiBehemothGUID[i]);
                NPCChangeTarget(uiKoltiraGUID);
                NPCChangeTarget(uiOrbazGUID);
                NPCChangeTarget(uiThassarianGUID);

                m_creature->Unmount();
                m_creature->CastSpell(m_creature, SPELL_THE_MIGHT_OF_MOGRAINE, true); // need to fix, on player only

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKoltiraGUID))
                    pTemp->Unmount();
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiThassarianGUID))
                    pTemp->Unmount();

                bIsBattle = true;
                break;
            case 2:
                m_creature->SetWalk(true);
                DoCast(m_creature, SPELL_THE_LIGHT_OF_DAWN);
                break;
            case 3:
            {
                Creature* pTirion = m_creature->GetMap()->GetCreature(uiTirionGUID);

                DoScriptText(EMOTE_LIGHT_OF_DAWN05, m_creature);
                if (m_creature->HasAura(SPELL_THE_LIGHT_OF_DAWN))
                    m_creature->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKoltiraGUID))
                {
                    if (pTemp->HasAura(SPELL_THE_LIGHT_OF_DAWN))
                        pTemp->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                    pTemp->SetWalk(true);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[19].x, LightofDawnLoc[19].y, LightofDawnLoc[19].z);
                }
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiThassarianGUID))
                {
                    if (pTemp->HasAura(SPELL_THE_LIGHT_OF_DAWN))
                        pTemp->RemoveAurasDueToSpell(SPELL_THE_LIGHT_OF_DAWN);
                    pTemp->SetWalk(true);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[21].x, LightofDawnLoc[21].y, LightofDawnLoc[21].z);
                }
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID))
                {
                    pTemp->SetWalk(true);
                    pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[10].x, LightofDawnLoc[10].y, LightofDawnLoc[10].z);
                }
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID))
                {
                    pTemp->SetWalk(true);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[13].x, LightofDawnLoc[13].y, LightofDawnLoc[13].z);
                }
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEligorGUID))
                {
                    pTemp->SetWalk(true);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[16].x, LightofDawnLoc[16].y, LightofDawnLoc[16].z);
                }
                JumpToNextStep(10000);
            } break;
            case 4:
                DoScriptText(SAY_LIGHT_OF_DAWN27, m_creature);
                ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKoltiraGUID))
                    pTemp->SetStandState(UNIT_STAND_STATE_KNEEL);
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiThassarianGUID))
                    pTemp->SetStandState(UNIT_STAND_STATE_KNEEL);
                SetEscortPaused(true);
                break;
            case 5:
                DoScriptText(SAY_LIGHT_OF_DAWN33, m_creature);
                SetEscortPaused(true);
                break;
            case 6:
                SetEscortPaused(true);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SPECIALATTACK1H);
                JumpToNextStep(1000);
                break;
            case 7:
                SetEscortPaused(true);
                JumpToNextStep(2000);
                break;
            case 8:
                m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_UNEQUIP));
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                    m_creature->CastSpell(pTemp, SPELL_ASHBRINGER, true);
                DoScriptText(EMOTE_LIGHT_OF_DAWN14, m_creature);
                SetEscortPaused(true);
                break;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        npc_escortAI::UpdateAI(diff);

        if (!bIsBattle)
        {
            if (uiPhase_timer < diff)
            {
                // ******* Before battle *****************************************************************
                switch(uiStep)
                {
                    case 0:  // countdown
                        //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_COUNTDOWN, 1);
                        break;

                    case 1:  // just delay
                        //UpdateWorldState(m_creature->GetMap(), WORLD_STATE_REMAINS, 1);
                        UpdateWorldState(m_creature->GetMap(), WORLD_STATE_COUNTDOWN, 0);
                        UpdateWorldState(m_creature->GetMap(), WORLD_STATE_EVENT_BEGIN, 1);
                        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                        JumpToNextStep(3000);
                        break;

                    case 2:
                        DoScriptText(SAY_LIGHT_OF_DAWN04, m_creature);
                        if (Creature* pKoltira = GetClosestCreatureWithEntry(m_creature, NPC_KOLTIRA_DEATHWEAVER, 50.0f))
                            uiKoltiraGUID = pKoltira->GetObjectGuid();
                        if (Creature* pOrbaz = GetClosestCreatureWithEntry(m_creature, NPC_ORBAZ_BLOODBANE, 50.0f))
                            uiOrbazGUID = pOrbaz->GetObjectGuid();
                        if (Creature* pThassarian = GetClosestCreatureWithEntry(m_creature, NPC_THASSARIAN, 50.0f))
                            uiThassarianGUID = pThassarian->GetObjectGuid();
                        JumpToNextStep(10000);
                        break;

                    case 3: // rise
                        DoScriptText(SAY_LIGHT_OF_DAWN05, m_creature);
                        JumpToNextStep(3000);
                        break;

                    case 4: // summon ghoul
                        // Dunno whats the summon spell, so workaround
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_GHOUL_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_ACHERUS_GHOUL, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->SetWalk(false);
                                pTemp->setFaction(2084);
                                uiGhoulGUID[uiSummon_counter] = pTemp->GetObjectGuid();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 5: // summon abomination
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_ABOMINATION_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_RAMPAGING_ABOMINATION, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->SetWalk(false);
                                pTemp->setFaction(2084);
                                uiAbominationGUID[uiSummon_counter] = pTemp->GetObjectGuid();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 6: // summon warrior
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_WARRIOR_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_WARRIOR_OF_THE_FROZEN_WASTES, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->SetWalk(false);
                                pTemp->setFaction(2084);
                                uiWarriorGUID[uiSummon_counter] = pTemp->GetObjectGuid();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 7: // summon warrior
                        DoCast(m_creature, 33271); // shack effect
                        uiPhase_timer = 500;
                        if (uiSummon_counter < ENCOUNTER_BEHEMOTH_NUMBER)
                        {
                            if (Creature* pTemp = m_creature->SummonCreature(NPC_FLESH_BEHEMOTH, (m_creature->GetPositionX()-20)+rand()%40, (m_creature->GetPositionY()-20)+rand()%40, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                            {
                                pTemp->SetWalk(false);
                                pTemp->setFaction(2084);
                                uiBehemothGUID[uiSummon_counter] = pTemp->GetObjectGuid();
                                uiSummon_counter++;
                            }
                        }
                        else
                        {
                            uiSummon_counter = 0;
                            uiStep++;
                        }
                        break;

                    case 8: // summon announce
                        DoScriptText(SAY_LIGHT_OF_DAWN06, m_creature);
                        JumpToNextStep(5000);
                        break;

                    case 9: // charge begins
                        SetEscortPaused(false);
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKoltiraGUID))
                        {
                            pTemp->SetWalk(false);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiOrbazGUID))
                        {
                            pTemp->SetWalk(false);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiThassarianGUID))
                        {
                            pTemp->SetWalk(false);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        }
                        for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAbominationGUID[i]))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiBehemothGUID[i]))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiGhoulGUID[i]))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiWarriorGUID[i]))
                                pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z);
                        JumpToNextStep(5000);
                        break;

                    // ******* After battle *****************************************************************
                    case 11: // Tirion starts to speak
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN28, pTemp);
                        JumpToNextStep(21000);
                        break;

                    case 12:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN29, pTemp);
                        JumpToNextStep(13000);
                        break;

                    case 13:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN30, pTemp);
                        JumpToNextStep(13000);
                        break;

                    case 14:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(SAY_LIGHT_OF_DAWN31, m_creature);
                        JumpToNextStep(7000);
                        break;

                    case 15: // summon gate
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_HIGHLORD_ALEXANDROS_MOGRAINE, LightofDawnLoc[22].x, LightofDawnLoc[22].y, LightofDawnLoc[22].z, LightofDawnLoc[22].o, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                        {
                            pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pTemp->CastSpell(pTemp, SPELL_ALEXANDROS_MOGRAINE_SPAWN, true);
                            DoScriptText(EMOTE_LIGHT_OF_DAWN06, pTemp);
                            uiAlexandrosGUID = pTemp->GetObjectGuid();
                        }
                        JumpToNextStep(4000);
                        break;

                    case 16: // Alexandros out
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAlexandrosGUID))
                        {
                            pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[23].x, LightofDawnLoc[23].y, LightofDawnLoc[23].z);
                            DoScriptText(SAY_LIGHT_OF_DAWN32, pTemp);
                        }
                        SetEscortPaused(false); // makes darion turns back
                        JumpToNextStep(5000);
                        break;

                    case 17:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        DoScriptText(SAY_LIGHT_OF_DAWN34, m_creature);
                        JumpToNextStep(5000);
                        break;

                    case 18: // Darion's spirit out
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_DARION_MOGRAINE, LightofDawnLoc[24].x, LightofDawnLoc[24].y, LightofDawnLoc[24].z, LightofDawnLoc[24].o, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                        {
                            DoScriptText(SAY_LIGHT_OF_DAWN35, pTemp);
                            pTemp->SetWalk(false);
                            uiDarionGUID = pTemp->GetObjectGuid();
                        }
                        JumpToNextStep(4000);
                        break;

                    case 19: // runs to father
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDarionGUID))
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN07, pTemp);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[25].x, LightofDawnLoc[25].y, LightofDawnLoc[25].z);
                        }
                        JumpToNextStep(4000);
                        break;

                    case 20:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDarionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN36, pTemp);
                        JumpToNextStep(4000);
                        break;

                    case 21:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDarionGUID))
                            DoScriptText(EMOTE_LIGHT_OF_DAWN08, pTemp);
                        JumpToNextStep(4000);
                        break;

                    case 22:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAlexandrosGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN37, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 23:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDarionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN38, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 24:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAlexandrosGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN39, pTemp);

                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID)) // Tirion moves forward here
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[1].x, LightofDawnLoc[1].y, LightofDawnLoc[1].z);

                        JumpToNextStep(15000);
                        break;

                    case 25:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDarionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN40, pTemp);
                        JumpToNextStep(11000);
                        break;

                    case 26:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAlexandrosGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN41, pTemp);
                        JumpToNextStep(5000);
                        break;

                    case 27:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDarionGUID))
                            pTemp->SetDeathState(JUST_DIED);
                        JumpToNextStep(24000);
                        break;

                    case 28:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAlexandrosGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN42, pTemp);
                        JumpToNextStep(6000);
                        break;

                    case 29: // lich king spawns
                        if (Creature* pTemp = m_creature->SummonCreature(NPC_THE_LICH_KING, LightofDawnLoc[26].x, LightofDawnLoc[26].y, LightofDawnLoc[26].z, LightofDawnLoc[26].o, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000))
                        {
                            DoScriptText(SAY_LIGHT_OF_DAWN43, pTemp);
                            uiLichKingGUID = pTemp->GetObjectGuid();
                            if (Creature* pAlex = m_creature->GetMap()->GetCreature(uiAlexandrosGUID))
                                pTemp->CastSpell(pAlex, SPELL_SOUL_FEAST_ALEX, false);
                        }
                        JumpToNextStep(2000);
                        break;

                    case 30:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiAlexandrosGUID)) // remove him
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN09, pTemp);
                            pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                        {
                            pTemp->InterruptNonMeleeSpells(false);
                            DoScriptText(SAY_LIGHT_OF_DAWN45, pTemp);
                        }
                        JumpToNextStep(3000);
                        break;

                    case 31:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(EMOTE_LIGHT_OF_DAWN10, m_creature);
                        DoScriptText(SAY_LIGHT_OF_DAWN44, m_creature);
                        JumpToNextStep(3000);
                        break;

                    case 32:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[27].x, LightofDawnLoc[27].y, LightofDawnLoc[27].z);
                        JumpToNextStep(6000);
                        break;

                    case 33: // Darion supports to jump to lich king here
// disable              if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), uiLichKingGUID)))
//  because mangos          DoCast(m_creature, SPELL_MOGRAINE_CHARGE); // jumping charge
//   doesn't make it looks well, so workarounds, Darion charges, looks better
                        m_creature->SetSpeedRate(MOVE_RUN, 3.0f);
                        m_creature->SetWalk(false);
                        SetEscortPaused(false);
                        JumpToNextStep(0);
                        break;

                    case 35: // Lich king counterattacks
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                        {
                            pTemp->HandleEmoteCommand(EMOTE_ONESHOT_KICK);
                            DoScriptText(SAY_LIGHT_OF_DAWN46, pTemp);
                        }
                        m_creature->SetSpeedRate(MOVE_RUN, 6.0f);
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_DEAD);
                        SetEscortPaused(false); // Darion got kicked by lich king
                        JumpToNextStep(0);
                        break;

                    case 37: // Lich king counterattacks
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        JumpToNextStep(3000);
                        break;

                    case 38:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN47, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 39:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN48, pTemp);
                        JumpToNextStep(15000);
                        break;

                    case 40:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN49, pTemp);
                        JumpToNextStep(17000);
                        break;

                    case 41: // Lich king - Apocalypse
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN11, pTemp);
                            DoScriptText(SAY_LIGHT_OF_DAWN51, pTemp);
                            if (Creature* pTirion = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            {
                                ((Unit*)pTirion)->SetStandState(UNIT_STAND_STATE_KNEEL);
                                //pTemp->CastSpell(pTirion, SPELL_APOCALYPSE, false); // not working
                                pTemp->CastSpell(pTirion, SPELL_SOUL_FEAST_TIRION, false);
                                DoScriptText(EMOTE_LIGHT_OF_DAWN12, pTirion);
                            }
                        }
                        JumpToNextStep(2000);
                        break;

                    case 42: // Maxwell yells for attack
                        {
                            float fLichPositionX, fLichPositionY, fLichPositionZ;
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            {
                                fLichPositionX = pTemp->GetPositionX();
                                fLichPositionY = pTemp->GetPositionY();
                                fLichPositionZ = pTemp->GetPositionZ();
                            }

                            if (fLichPositionX && fLichPositionY)
                            {
                                Creature* pTemp;
                                if (pTemp = m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 10000))
                                {
                                    pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                    pTemp->SetWalk(false);
                                    pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                    pTemp->setFaction(m_creature->getFaction());
                                    pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                                    uiDefenderGUID[0] = pTemp->GetObjectGuid();
                                }

                                if (pTemp = m_creature->SummonCreature(NPC_RIMBLAT_EARTHSHATTER, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 10000))
                                {
                                    pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                    pTemp->SetWalk(false);
                                    pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                    pTemp->setFaction(m_creature->getFaction());
                                    pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                                    uiEarthshatterGUID[0] = pTemp->GetObjectGuid();
                                }
                            }
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID))
                            {
                                pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->SetWalk(false);
                                pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                                DoScriptText(SAY_LIGHT_OF_DAWN50, pTemp);
                            }
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID))
                            {
                                pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->SetWalk(false);
                                pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                pTemp->HandleEmoteCommand(EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                            }
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEligorGUID))
                            {
                                pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                                pTemp->SetWalk(false);
                                pTemp->SetSpeedRate(MOVE_RUN, 2.0f);
                                pTemp->GetMotionMaster()->MovePoint(0, fLichPositionX, fLichPositionY, fLichPositionZ);
                            }
                        }
                        JumpToNextStep(4500);
                        break;

                    case 43: // They all got kicked
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            DoScriptText(EMOTE_LIGHT_OF_DAWN13, pTemp);

                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID))
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[14].x, LightofDawnLoc[14].y, LightofDawnLoc[14].z);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID))
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[11].x, LightofDawnLoc[11].y, LightofDawnLoc[11].z);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEligorGUID))
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[17].x, LightofDawnLoc[17].y, LightofDawnLoc[17].z);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiDefenderGUID[0]))
                        {
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEarthshatterGUID[0]))
                        {
                            pTemp->SetSpeedRate(MOVE_RUN, 6.0f);
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_DEAD);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[0].x+rand()%10, LightofDawnLoc[0].y+rand()%10, LightofDawnLoc[0].z);
                        }
                        JumpToNextStep(3000);
                        break;

                    case 44: // make them stand up
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEligorGUID))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        JumpToNextStep(1000);
                        break;

                    case 45:
                        DoScriptText(SAY_LIGHT_OF_DAWN52, m_creature);
                        JumpToNextStep(5000);
                        break;

                    case 46: // Darion stand up, "not today"
                        m_creature->SetSpeedRate(MOVE_RUN, 1.0f);
                        m_creature->SetWalk(true);
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(SAY_LIGHT_OF_DAWN53, m_creature);
                        SetEscortPaused(false); // Darion throws sword
                        JumpToNextStep(7000);
                        break;

                    case 47: // Ashbringer rebirth
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        DoScriptText(EMOTE_LIGHT_OF_DAWN15, m_creature);
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                        {
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                            pTemp->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_HIGHLORD_TIRION_FORDRING));
                            pTemp->CastSpell(pTemp, SPELL_REBIRTH_OF_THE_ASHBRINGER, false);
                        }
                        JumpToNextStep(1000);
                        break;

                    case 48: // Show the cleansing effect (dawn of light)
                        if (GameObject* pGo = m_creature->GetMap()->GetGameObject(uiDawnofLightGUID))
                            pGo->SetPhaseMask(128, true);
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                        {
                            if (pTemp->HasAura(SPELL_REBIRTH_OF_THE_ASHBRINGER))
                                pTemp->RemoveAurasDueToSpell(SPELL_REBIRTH_OF_THE_ASHBRINGER);
                            pTemp->CastSpell(pTemp, 41542, false); // workarounds, light expoded, makes it cool
                            pTemp->HandleEmoteCommand(EMOTE_ONESHOT_ROAR);
                        }
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            pTemp->InterruptNonMeleeSpells(false);
                        JumpToNextStep(2500);
                        break;

                    case 49:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN54, pTemp);
                        JumpToNextStep(4000);
                        break;

                    case 50:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN55, pTemp);
                        JumpToNextStep(5000);
                        break;

                    case 51:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN56, pTemp);
                        JumpToNextStep(1000);
                        break;

                    case 52: // Tiron charges
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN16, pTemp);
                            pTemp->CastSpell(pTemp, SPELL_TIRION_CHARGE, false); // jumping charge
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2H);
                            pTemp->SetSpeedRate(MOVE_RUN, 3.0f); // workarounds, make Tirion still running
                            pTemp->SetWalk(false);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[2].x, LightofDawnLoc[2].y, LightofDawnLoc[2].z);
                            if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                                pTemp->GetMap()->CreatureRelocation(pTemp, LightofDawnLoc[28].x, LightofDawnLoc[28].y, LightofDawnLoc[28].z, 0.0f); // workarounds, he should kick back by Tirion, but here we relocate him
                        }
                        JumpToNextStep(1500);
                        break;

                    case 53:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN57, pTemp);
                        JumpToNextStep(1000);
                        break;

                    case 54:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                        {
                            pTemp->SetSpeedRate(MOVE_RUN, 1.0f);
                            m_creature->SetWalk(true);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[29].x, LightofDawnLoc[29].y, LightofDawnLoc[29].z); // 26
                        }
                        JumpToNextStep(4000);
                        break;

                    case 55:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_KNEEL);
                        JumpToNextStep(2000);
                        break;

                    case 56:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            ((Unit*)pTemp)->SetStandState(UNIT_STAND_STATE_STAND);
                        JumpToNextStep(1500);
                        break;

                    case 57:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN58, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 58:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN59, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 59:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID))
                            pTemp->CastSpell(pTemp, SPELL_TELEPORT_VISUAL, false);
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID)) // Tirion runs to Darion
                        {
                            pTemp->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                            pTemp->SetSpeedRate(MOVE_RUN, 1.0f);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[6].x, LightofDawnLoc[6].y, LightofDawnLoc[6].z);
                        }
                        JumpToNextStep(2500);
                        break;

                    case 60:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiLichKingGUID)) // Lich king disappears here
                        {
                            DoScriptText(EMOTE_LIGHT_OF_DAWN17, pTemp);
                            pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                        JumpToNextStep(10000);
                        break;

                    case 61:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN60, pTemp);
                        JumpToNextStep(3000);
                        break;

                    case 62:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                        {
                            pTemp->SetWalk(true);
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[7].x, LightofDawnLoc[7].y, LightofDawnLoc[7].z);
                        }
                        JumpToNextStep(5500);
                        break;

                    case 63:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                        {
                            pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[8].x, LightofDawnLoc[8].y, LightofDawnLoc[8].z);
                            DoScriptText(SAY_LIGHT_OF_DAWN61, pTemp);
                        }
                        JumpToNextStep(15000);
                        break;

                    case 64:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN62, pTemp);
                        JumpToNextStep(7000);
                        break;

                    case 65:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN63, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 66:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN64, pTemp);
                        JumpToNextStep(11000);
                        break;

                    case 67:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN65, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 68:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN66, pTemp);
                        JumpToNextStep(8000);
                        break;

                    case 69:
                        if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                            DoScriptText(SAY_LIGHT_OF_DAWN67, pTemp);
                        JumpToNextStep(10000);
                        break;

                    case 70:
                        ((Unit*)m_creature)->SetStandState(UNIT_STAND_STATE_STAND);
                        DoScriptText(SAY_LIGHT_OF_DAWN68, m_creature);
                        JumpToNextStep(10000);
                        break;

                    case 71:
                        if (GameObject* pGo = m_creature->GetMap()->GetGameObject(uiDawnofLightGUID)) // Turn off dawn of light
                            pGo->SetPhaseMask(0, true);

                        {
                            Map *map = m_creature->GetMap(); // search players with in 50 yards for quest credit
                            Map::PlayerList const &PlayerList = map->GetPlayers();
                            if (!PlayerList.isEmpty())
                            {
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    if (i->getSource()->isAlive() && m_creature->IsWithinDistInMap(i->getSource(), 50))
                                        i->getSource()->CastSpell(i->getSource(), SPELL_THE_LIGHT_OF_DAWN_Q, false);
                            }
                        }
                        m_creature->SetVisibility(VISIBILITY_OFF); // respawns another Darion for quest turn in
                        m_creature->SummonCreature(NPC_HIGHLORD_DARION_MOGRAINE, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 180000);
                        JumpToNextStep(1000);
                        break;

                    case 72:
                        SetEscortPaused(false); // Escort ends
                        JumpToNextStep(0);
                        break;
                }

            }else uiPhase_timer -= diff;
        }

        // ******* During battle *****************************************************************
        else
        {
            if (uiAnti_magic_zone < diff)
            {
                DoCast(m_creature, SPELL_ANTI_MAGIC_ZONE1);
                uiAnti_magic_zone = 25000 + rand()%5000;
            }else uiAnti_magic_zone -= diff;

            if (uiDeath_strike < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_DEATH_STRIKE);
                uiDeath_strike = 5000 + rand()%5000;
            }else uiDeath_strike -= diff;

            if (uiDeath_embrace < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_DEATH_EMBRACE);
                uiDeath_embrace = 5000 + rand()%5000;
            }else uiDeath_embrace -= diff;

            if (uiIcy_touch < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_ICY_TOUCH1);
                uiIcy_touch = 5000 + rand()%5000;
            }else uiIcy_touch -= diff;

            if (uiUnholy_blight < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_UNHOLY_BLIGHT);
                uiUnholy_blight = 5000 + rand()%5000;
            }else uiUnholy_blight -= diff;

            if (uiFight_speech < diff)
            {
                switch(rand()%15)
                {
                    case 0: DoScriptText(SAY_LIGHT_OF_DAWN09, m_creature);break;
                    case 1: DoScriptText(SAY_LIGHT_OF_DAWN10, m_creature);break;
                    case 2: DoScriptText(SAY_LIGHT_OF_DAWN11, m_creature);break;
                    case 3: DoScriptText(SAY_LIGHT_OF_DAWN12, m_creature);break;
                    case 4: DoScriptText(SAY_LIGHT_OF_DAWN13, m_creature);break;
                    case 5: DoScriptText(SAY_LIGHT_OF_DAWN14, m_creature);break;
                    case 6: DoScriptText(SAY_LIGHT_OF_DAWN15, m_creature);break;
                    case 7: DoScriptText(SAY_LIGHT_OF_DAWN16, m_creature);break;
                    case 8: DoScriptText(SAY_LIGHT_OF_DAWN17, m_creature);break;
                    case 9: DoScriptText(SAY_LIGHT_OF_DAWN18, m_creature);break;
                    case 10: DoScriptText(SAY_LIGHT_OF_DAWN19, m_creature);break;
                    case 11: DoScriptText(SAY_LIGHT_OF_DAWN20, m_creature);break;
                    case 12: DoScriptText(SAY_LIGHT_OF_DAWN21, m_creature);break;
                    case 13: DoScriptText(SAY_LIGHT_OF_DAWN22, m_creature);break;
                    case 14: DoScriptText(SAY_LIGHT_OF_DAWN23, m_creature);break;
                    case 15: DoScriptText(SAY_LIGHT_OF_DAWN24, m_creature);break;
                }
                uiFight_speech = 15000 + rand()%5000;
            }else uiFight_speech -= diff;

            // Check spawns
            if (uiSpawncheck < diff)
            {
                SpawnNPC();
                uiSpawncheck = 1000;
            }else uiSpawncheck -= diff;

            // Check targets
            if (uiTargetcheck < diff)
            {
                for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                    NPCChangeTarget(uiGhoulGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                    NPCChangeTarget(uiWarriorGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                    NPCChangeTarget(uiAbominationGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                    NPCChangeTarget(uiBehemothGUID[i]);
                NPCChangeTarget(uiKoltiraGUID);
                NPCChangeTarget(uiOrbazGUID);
                NPCChangeTarget(uiThassarianGUID);

                uiTargetcheck = 10000;
            }else uiTargetcheck -= diff;

            // Battle end
            if (uiFight_duration < diff + 5000)
            {
                if (!uiTirionGUID)
                    if (Creature* pTemp = m_creature->SummonCreature(NPC_HIGHLORD_TIRION_FORDRING, LightofDawnLoc[0].x, LightofDawnLoc[0].y, LightofDawnLoc[0].z, 1.528f, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 600000))
                    {
                        pTemp->setFaction(m_creature->getFaction());
                        pTemp->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, uint32(EQUIP_UNEQUIP));
                        DoScriptText(SAY_LIGHT_OF_DAWN25, pTemp);
                        uiTirionGUID = pTemp->GetObjectGuid();
                    }
            }
            if (uiFight_duration < diff)
            {
                bIsBattle = false;
                uiFight_duration = 300000;

                if (m_creature->HasAura(SPELL_THE_MIGHT_OF_MOGRAINE, EFFECT_INDEX_0))
                    m_creature->RemoveAurasDueToSpell(SPELL_THE_MIGHT_OF_MOGRAINE);
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->SetWalk(false);

                for(uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
                    DespawnNPC(uiDefenderGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
                    DespawnNPC(uiEarthshatterGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
                    DespawnNPC(uiAbominationGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
                    DespawnNPC(uiBehemothGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
                    DespawnNPC(uiGhoulGUID[i]);
                for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
                    DespawnNPC(uiWarriorGUID[i]);

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->SetWalk(false);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[9].x, LightofDawnLoc[9].y, LightofDawnLoc[9].z);
                }

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->SetWalk(false);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[12].x, LightofDawnLoc[12].y, LightofDawnLoc[12].z);
                }

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiEligorGUID))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->SetWalk(false);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[15].x, LightofDawnLoc[15].y, LightofDawnLoc[15].z);
                }
                DespawnNPC(uiRayneGUID);

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiKoltiraGUID))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->SetWalk(false);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[18].x, LightofDawnLoc[18].y, LightofDawnLoc[18].z);
                    pTemp->CastSpell(pTemp, SPELL_THE_LIGHT_OF_DAWN, false);
                }

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiOrbazGUID))
                    DoScriptText(EMOTE_LIGHT_OF_DAWN04, pTemp);

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiThassarianGUID))
                {
                    pTemp->RemoveAllAuras();
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->AttackStop();
                    pTemp->setFaction(m_creature->getFaction());
                    pTemp->SetWalk(false);
                    pTemp->GetMotionMaster()->MovePoint(0, LightofDawnLoc[20].x, LightofDawnLoc[20].y, LightofDawnLoc[20].z);
                    pTemp->CastSpell(pTemp, SPELL_THE_LIGHT_OF_DAWN, false);
                }

                if (Creature* pTemp = m_creature->GetMap()->GetCreature(uiTirionGUID))
                    DoScriptText(SAY_LIGHT_OF_DAWN26, pTemp);

                SetEscortPaused(false);

            }else uiFight_duration -= diff;

            DoMeleeAttackIfReady();
        }
    }

    void JumpToNextStep(uint32 uiTimer)
    {
        uiPhase_timer = uiTimer;
        uiStep++;
    }

    void NPCChangeTarget(ObjectGuid ui_GUID)
    {
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(ui_GUID))
            if (pTemp->isAlive())
                if (Unit* pTarger = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    if (pTarger->isAlive())
                    {
                        ((Creature*)pTemp)->AddThreat(pTarger, 0.0f);
                        ((Creature*)pTemp)->AI()->AttackStart(pTarger);
                        ((Creature*)pTemp)->SetInCombatWith(pTarger);
                        pTarger->SetInCombatWith(pTemp);
                    }
    }

    void SpawnNPC()
    {
        Creature* pTemp = NULL;

        // Death
        for(uint8 i = 0; i < ENCOUNTER_GHOUL_NUMBER; ++i)
        {
            if (!(pTemp = m_creature->GetMap()->GetCreature(uiGhoulGUID[i])))
            {
                pTemp = m_creature->SummonCreature(NPC_ACHERUS_GHOUL, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiGhoulGUID[i] = pTemp->GetObjectGuid();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_ABOMINATION_NUMBER; ++i)
        {
            if (!(pTemp = m_creature->GetMap()->GetCreature(uiAbominationGUID[i])))
            {
                pTemp = m_creature->SummonCreature(NPC_WARRIOR_OF_THE_FROZEN_WASTES, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiAbominationGUID[i] = pTemp->GetObjectGuid();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_WARRIOR_NUMBER; ++i)
        {
            if (!(pTemp = m_creature->GetMap()->GetCreature(uiWarriorGUID[i])))
            {
                pTemp = m_creature->SummonCreature(NPC_RAMPAGING_ABOMINATION, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiWarriorGUID[i] = pTemp->GetObjectGuid();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_BEHEMOTH_NUMBER; ++i)
        {
            if (!(pTemp = m_creature->GetMap()->GetCreature(uiBehemothGUID[i])))
            {
                pTemp = m_creature->SummonCreature(NPC_FLESH_BEHEMOTH, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2084);
                uiBehemothGUID[i] = pTemp->GetObjectGuid();
            }
        }

        // Dawn
        for(uint8 i = 0; i < ENCOUNTER_DEFENDER_NUMBER; ++i)
        {
            if (!(pTemp = m_creature->GetMap()->GetCreature(uiDefenderGUID[i])))
            {
                pTemp = m_creature->SummonCreature(NPC_DEFENDER_OF_THE_LIGHT, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2089);
                m_creature->AddThreat(pTemp, 0.0f);
                uiDefenderGUID[i] = pTemp->GetObjectGuid();
            }
        }
        for(uint8 i = 0; i < ENCOUNTER_EARTHSHATTER_NUMBER; ++i)
        {
            if (!(pTemp = m_creature->GetMap()->GetCreature(uiEarthshatterGUID[i])))
            {
                pTemp = m_creature->SummonCreature(NPC_RIMBLAT_EARTHSHATTER, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000);
                pTemp->setFaction(2089);
                m_creature->AddThreat(pTemp, 0.0f);
                uiEarthshatterGUID[i] = pTemp->GetObjectGuid();
            }
        }
        if (!(pTemp = m_creature->GetMap()->GetCreature(uiKorfaxGUID)))
        {
            pTemp = m_creature->SummonCreature(NPC_KORFAX_CHAMPION_OF_THE_LIGHT, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 600000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiKorfaxGUID = pTemp->GetObjectGuid();
        }
        if (!(pTemp = m_creature->GetMap()->GetCreature(uiMaxwellGUID)))
        {
            pTemp = m_creature->SummonCreature(NPC_LORD_MAXWELL_TYROSUS, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 600000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiMaxwellGUID = pTemp->GetObjectGuid();
        }
        if (!(pTemp = m_creature->GetMap()->GetCreature(uiEligorGUID)))
        {
            pTemp = m_creature->SummonCreature(NPC_COMMANDER_ELIGOR_DAWNBRINGER, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 600000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiEligorGUID = pTemp->GetObjectGuid();
        }
        if (!(pTemp = m_creature->GetMap()->GetCreature(uiRayneGUID)))
        {
            pTemp = m_creature->SummonCreature(NPC_RAYNE, LightofDawnLoc[0].x+rand()%30, LightofDawnLoc[0].y+rand()%30, LightofDawnLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 300000);
            pTemp->setFaction(2089);
            m_creature->AddThreat(pTemp, 0.0f);
            uiRayneGUID = pTemp->GetObjectGuid();
        }
    }

    void DespawnNPC(ObjectGuid pGUID)
    {
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(pGUID))
            if (pTemp->isAlive())
            {
                pTemp->SetVisibility(VISIBILITY_OFF);
                pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
    }
};

bool GossipHello_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu( pCreature->GetObjectGuid() );

    if (pPlayer->GetQuestStatus(12801) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM( 0, "I am ready.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_highlord_darion_mograine(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            ((npc_highlord_darion_mograineAI*)pCreature->AI())->uiStep = 1;
            ((npc_highlord_darion_mograineAI*)pCreature->AI())->Start(false, pPlayer);
            break;
    }
    return true;
}

/*######
## npc the lich king in dawn of light
######*/
struct MANGOS_DLL_DECL npc_the_lich_king_tirion_dawnAI : public ScriptedAI
{
    npc_the_lich_king_tirion_dawnAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }
    void Reset() {}
    void AttackStart(Unit *who) { return; } // very sample, just don't make them aggreesive
    void UpdateAI(const uint32 diff) { return; }
    void JustDied(Unit* killer) {}
};

/*######
## npc orbaz, koltira, tassarian
######*/
struct MANGOS_DLL_DECL npc_minibosses_dawn_of_lightAI : public ScriptedAI
{
   npc_minibosses_dawn_of_lightAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

   uint32 uiIcyTouchTimer;
   uint32 uiBloodStrikeTimer;
   uint32 uiPlagueStrikeTimer;

   void Reset()
   {
       uiIcyTouchTimer = urand(10000, 20000);
       uiBloodStrikeTimer = urand(10000, 20000);
       uiPlagueStrikeTimer = urand(10000, 20000);
   }

   bool EnterEvadeIfOutOfCombatArea(const uint32 uiDiff)
   {
       m_creature->GetMotionMaster()->MoveIdle();
       return false;
   }
   void EnterEvadeMode()
   {
       m_creature->GetMotionMaster()->MoveIdle();
       return;
   }

   void UpdateAI(const uint32 uiDiff)
   {
       if (!m_creature->getVictim() || !m_creature->SelectHostileTarget() )
           return;

       if (uiIcyTouchTimer <= uiDiff)
       {
           DoCastSpellIfCan(m_creature->getVictim(), SPELL_ICY_TOUCH);
           uiIcyTouchTimer = urand(10000, 20000);
       }
       else uiIcyTouchTimer -= uiDiff;

       if (uiBloodStrikeTimer <= uiDiff)
       {
           DoCastSpellIfCan(m_creature->getVictim(), SPELL_BLOOD_STRIKE);
           uiBloodStrikeTimer = urand(10000, 20000);
       }
       else uiBloodStrikeTimer -= uiDiff;

       if (uiPlagueStrikeTimer <= uiDiff)
       {
           DoCastSpellIfCan(m_creature->getVictim(), SPELL_PLAGUE_STRIKE1);
           uiPlagueStrikeTimer = urand(10000, 20000);
       }
       else uiPlagueStrikeTimer -= uiDiff;

       DoMeleeAttackIfReady();
   }
};

/*######
## Acherus Ghoul (29219)
######*/
struct MANGOS_DLL_DECL mob_acherus_ghoulAI : public ScriptedAI
{
   mob_acherus_ghoulAI(Creature* pCreature) : ScriptedAI(pCreature)
   {
       m_bIsReady      = false;
       m_bIsSpawned    = false;
       m_bIsInBattle   = (m_creature->GetPositionX() < 2300.0f) ? true : false;

       m_pMap = m_creature->GetMap();

       Reset();
   }

   Map *m_pMap;

   uint32 m_uiReadyTimer;
   uint32 m_uiGhoulplosionTimer;

   bool m_bIsReady;
   bool m_bIsSpawned;
   bool m_bIsInBattle;

   void EnterEvadeIfOutOfCombatArea(const uint32 uiDiff) { return; }

   void Reset()
   {
       m_uiReadyTimer  = 4000;
       m_uiGhoulplosionTimer = 30000;
   }

   void MoveInLineOfSight(Unit *pWho)
   {
       if (!m_bIsReady)
           return;

       ScriptedAI::MoveInLineOfSight(pWho);
   }

   void AttackStart(Unit *pWho)
   {
       if (!m_bIsReady)
           return;

       ScriptedAI::AttackStart(pWho);
   }

   void UpdateAI(uint32 const uiDiff)
   {
       if (!m_bIsReady)
       {
           if (!m_bIsSpawned)
           {
               m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
               m_bIsSpawned = true;
           }

           if (m_uiReadyTimer <= uiDiff)
               m_bIsReady = true;
           else m_uiReadyTimer -= uiDiff;

           return;
       }

       if (!m_creature->getVictim() || !m_creature->SelectHostileTarget() )
       {
           if (m_bIsInBattle && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != CONFUSED_MOTION_TYPE)
                   m_creature->GetMotionMaster()->MoveConfused();

           return;
       }

       if (m_uiGhoulplosionTimer <= uiDiff)
       {
           DoCastSpellIfCan(m_creature, SPELL_GHOULPLOSION);
           m_uiGhoulplosionTimer = 30000;
       }
       else m_uiGhoulplosionTimer -= uiDiff;

       DoMeleeAttackIfReady();
   }
};

/*######
## npc_valkyr_battle_maiden
######*/

enum
{
    SPELL_REVIVE    =    51918,
};

#define REVIVE_WHISPER "It is not yet your time, champion. Rise! Rise and fight once more!"

struct MANGOS_DLL_DECL npc_valkyr_battle_maidenAI : ScriptedAI
{
    npc_valkyr_battle_maidenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    float x, y, z;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    ObjectGuid m_summonerGuid;

    void Reset()
    {
        m_summonerGuid = m_creature->GetGuidValue(UNIT_FIELD_SUMMONEDBY);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        m_creature->SetVisibility(VISIBILITY_OFF);
        m_creature->SetLevitate(true);
        m_uiPhase = 0;
        m_uiPhaseTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_summonerGuid);
        if (!pPlayer)
            m_uiPhase = 3;

        if (m_uiPhaseTimer <= uiDiff)
        {
            switch (m_uiPhase)
            {
                case 0:
                    pPlayer->GetClosePoint(x, y, z, m_creature->GetObjectBoundingRadius());
                    m_creature->GetMotionMaster()->MovementExpired();
                    m_creature->GetMap()->CreatureRelocation(m_creature, x-2.0f , y-1.5f, z+2.5f, 0);
                    m_creature->SetFacingToObject(pPlayer);
                    m_creature->SetVisibility(VISIBILITY_ON);
                    m_uiPhase++;
                    break;
                case 1:
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pPlayer->GetObjectGuid());
                    m_uiPhase++;
                    break;
                case 2:
                    DoCast(pPlayer, SPELL_REVIVE, true);
                    m_creature->MonsterWhisper(REVIVE_WHISPER, pPlayer);
                    // cause 51918 has cast time of 2 seconds
                    m_uiPhaseTimer = 3000;
                    m_uiPhase++;
                    break;
                case 3:
                    m_creature->ForcedDespawn();
                default:
                    break;
            }
        }
        else
            m_uiPhaseTimer -= uiDiff;
    }
};


CreatureAI* GetAI_npc_valkyr_battle_maiden(Creature* pCreature)
{
    return new npc_valkyr_battle_maidenAI(pCreature);
};

/*######
## mob_warrior_of_the_frozen_wastes (53631)
######*/
struct MANGOS_DLL_DECL mob_warrior_of_the_frozen_wastesAI : public ScriptedAI
{
   mob_warrior_of_the_frozen_wastesAI(Creature* pCreature) : ScriptedAI(pCreature)
   {
       m_bIsReady      = false;
       m_bIsSpawned    = false;
       m_bIsInBattle   = (m_creature->GetPositionX() < 2300.0f) ? true : false;

       Reset();
   }

   uint32 m_uiReadyTimer;
   uint32 m_uiCleaveTimer;

   bool m_bIsReady;
   bool m_bIsSpawned;
   bool m_bIsInBattle;

   void EnterEvadeIfOutOfCombatArea(const uint32 uiDiff) { return; }

   void Reset()
   {
       m_uiReadyTimer  = 4000;
       m_uiCleaveTimer = urand(3000, 5000);
   }

   void AttackStart(Unit *pWho)
   {
       if (!m_bIsReady)
           return;

       ScriptedAI::AttackStart(pWho);
   }

   void MoveInLineOfSight(Unit *pWho)
   {
       if (!m_bIsReady)
           return;

       CreatureAI::MoveInLineOfSight(pWho);
   }

   void UpdateAI(uint32 const uiDiff)
   {
       if (!m_bIsReady)
       {
           if (!m_bIsSpawned)
           {
               m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EMERGE);
               m_bIsSpawned = true;
           }

           if (m_uiReadyTimer <= uiDiff)
               m_bIsReady = true;
           else m_uiReadyTimer -= uiDiff;

           return;
       }

       if (!m_creature->getVictim() || !m_creature->SelectHostileTarget() )
       {
           if (m_bIsInBattle && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != CONFUSED_MOTION_TYPE)
                   m_creature->GetMotionMaster()->MoveConfused();

           return;
       }

       if (m_uiCleaveTimer <= uiDiff)
       {
           DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE);
           m_uiCleaveTimer = urand(13000, 15000);
       }
       else m_uiCleaveTimer -= uiDiff;

       DoMeleeAttackIfReady();
   }
};

/*######
## npc_mine_car
######*/

struct MANGOS_DLL_DECL npc_mine_carAI : public ScriptedAI
{
    npc_mine_carAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        Reset();
    }

    ObjectGuid m_scarletMinerGuid;

    void Reset()
    {
        m_scarletMinerGuid.Clear();
    }

    void MoveInLineOfSight(Unit* /*pUnit*/)
    {
        return;
    }

    void EnterCombat(Unit* /*pUnit*/)
    {
        return;
    }

    void AttackStart(Unit* /*pUnit*/)
    {
        return;
    }

    void SetScarletMinerGuid(const ObjectGuid &guid)
    {
        m_scarletMinerGuid = guid;
    }

    void FollowMiner()
    {
        if(Creature* pMiner = m_creature->GetMap()->GetCreature(m_scarletMinerGuid))
        {
            // buggy sometimes...
            m_creature->SetSpeedRate(MOVE_WALK, 1.5f, true);
            m_creature->SetSpeedRate(MOVE_RUN, 1.5f, true);
            m_creature->SetWalk(false);

            m_creature->GetMotionMaster()->MoveFollow(pMiner, 1.0f, 0);
        }
    }

    void ExitMineCar()
    {
        if (Creature* pMiner = m_creature->GetMap()->GetCreature(m_scarletMinerGuid))
            pMiner->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->GetVehicleKit()->HasEmptySeat(0))
        {
            m_creature->ForcedDespawn();
        }
    }
};

/*######
## npc_scarlet_miner
######*/

enum
{
    SPELL_CAR_DRAG  = 52465,
    SPELL_CAR_CHECK = 54173
};

#define SAY_SCARLET_MINER1  "Where'd this come from? I better get this down to the ships before the foreman sees it!"
#define SAY_SCARLET_MINER2  "Now I can have a rest!"

struct MANGOS_DLL_DECL npc_scarlet_minerAI : public npc_escortAI
{
    npc_scarlet_minerAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Reset();
    }

    uint32 m_uiMonoTimer;
    uint32 m_uiMonoPhase;

    ObjectGuid m_mineCarGuid;
    ObjectGuid m_playerGuid;

    bool m_bReachedShip;
    uint32 m_uiShipDelay;

    void Reset()
    {
        m_uiMonoTimer = 0;
        m_uiMonoPhase = 0;

        m_mineCarGuid.Clear();
        m_playerGuid.Clear();

        m_bReachedShip = false;
        m_uiShipDelay = 3000;
    }

    void StartCarEvent(Player *pPlayer)
    {
        if (pPlayer->GetVehicle())
        {
            m_mineCarGuid = pPlayer->GetVehicle()->GetBase()->GetObjectGuid();
            m_playerGuid = pPlayer->GetObjectGuid();
            Start(false, pPlayer);
        }
    }

    void WaypointReached(uint32 uiWp)
    {
        switch(uiWp)
        {
            case 0:
                if (Unit* pMineCar = m_creature->GetMap()->GetCreature(m_mineCarGuid))
                    m_creature->SetInFront(pMineCar);

                SetRun(true);
                m_uiMonoTimer = 4000;
                m_uiMonoPhase = 1;
                break;
            case 17:
                m_bReachedShip = true;
                break;
            default:
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiMonoPhase)
        {
            if (m_uiMonoTimer <= uiDiff)
            {
                if (m_uiMonoPhase == 1)
                {
                    m_creature->MonsterSay(SAY_SCARLET_MINER1,LANG_UNIVERSAL, NULL);

                    if (Creature *pMineCar = m_creature->GetMap()->GetCreature(m_mineCarGuid))
                        DoCast(pMineCar, SPELL_CAR_DRAG);

                        m_uiMonoTimer = 800;
                        m_uiMonoPhase = 2;
                }
                else
                {
                    if (Creature *pMineCar = m_creature->GetMap()->GetCreature(m_mineCarGuid))
                    {
                        if(npc_mine_carAI* pMineCarAI = dynamic_cast<npc_mine_carAI*>(pMineCar->AI()))
                        {
                            pMineCarAI->FollowMiner();
                        }
                        m_uiMonoPhase = 0;
                    }
                }
            }
            else
                m_uiMonoTimer -= uiDiff;
        }

        if (m_bReachedShip)
        {
            if (m_uiShipDelay <= uiDiff)
            {
                if (Creature *pMineCar = m_creature->GetMap()->GetCreature(m_mineCarGuid))
                {
                    m_creature->SetInFront(pMineCar);
                    m_creature->MonsterSay(SAY_SCARLET_MINER2,LANG_UNIVERSAL, NULL);
                    pMineCar->Relocate(pMineCar->GetPositionX(), pMineCar->GetPositionY(), pMineCar->GetPositionZ() + 1);
                    pMineCar->GetMotionMaster()->MovementExpired();
                    pMineCar->StopMoving();
                    pMineCar->RemoveAurasDueToSpell(SPELL_CAR_DRAG);

                    if (Player *pPlayer = m_creature->GetMap()->GetPlayer(ObjectGuid(m_playerGuid)))
                    {
                        if (pPlayer->GetVehicle())
                            pPlayer->ExitVehicle();
                    }

                    if (npc_mine_carAI* pMineCarAI = dynamic_cast<npc_mine_carAI*>(pMineCar->AI()))
                    {
                        pMineCarAI->ExitMineCar();
                    }

                    m_bReachedShip = false;
                }
            }
            else
                m_uiShipDelay -= uiDiff;
        }
        npc_escortAI::UpdateAI(uiDiff);
    }
};

/*######
## go_inconspicous_mine_car
######*/

enum
{
    QUEST_MASSACRE_AT_LIGHTS_POINT  = 12701,

    ENTRY_SCARLET_MINER             = 28841,
    ENTRY_MINE_CAR                  = 28817,

    SPELL_MINE_CAR_SUMM             = 52463
};

bool GOUse_go_inconspicous_mine_car(Player* pPlayer, GameObject* /*pGo*/)
{
    if (pPlayer->GetQuestStatus(QUEST_MASSACRE_AT_LIGHTS_POINT) == QUEST_STATUS_INCOMPLETE)
    {
        if (pPlayer->GetVehicle())
            return false;

        if (Creature* pMiner = pPlayer->SummonCreature(ENTRY_SCARLET_MINER, 2383.869f, -5900.312f, 107.996f, pPlayer->GetOrientation(),TEMPSUMMON_DEAD_DESPAWN, 1))
        {
            pPlayer->CastSpell(pPlayer, SPELL_MINE_CAR_SUMM, true);
            if (Creature* pMineCar = (Creature*)pPlayer->GetVehicle()->GetBase())
            {
                if (npc_mine_carAI* pMineCarAI = dynamic_cast<npc_mine_carAI*>(pMineCar->AI()))
                {
                    pMineCarAI->SetScarletMinerGuid(pMiner->GetObjectGuid());
                    if (npc_scarlet_minerAI* pMinerAI = dynamic_cast<npc_scarlet_minerAI*>(pMiner->AI()))
                    {
                        pMinerAI->StartCarEvent(pPlayer);
                    }
                }
            }
        }
    }
    return true;
}

/*######
## npc_scourge_gryphon
######*/

enum
{
    RIDE_VEHICLE_HARDCODED = 46598
};

struct MANGOS_DLL_DECL npc_scourge_gryphonAI : public npc_escortAI
{
    npc_scourge_gryphonAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        Reset();
    }

    void Reset()
    {
    }

    void SpellHit(Unit* pUnit, const SpellEntry* pSpell)
    {
        if (pUnit && pUnit->GetTypeId() == TYPEID_PLAYER)
        {
            if (pSpell->Id == RIDE_VEHICLE_HARDCODED)
            {
                FlyToDeathsBreach((Player*)pUnit);
            }
        }
    }

    void FlyToDeathsBreach(Player* pPlayer)
    {
        pPlayer->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        // Need to be set here. If flag is applied earlier, the Spell Immune Mask
        // makes the vehicle mount spell (ID - 46598 Ride Vehicle Hardcoded) disfunctional
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetLevitate(true);

        Start(true, pPlayer);
    }

    void WaypointReached(uint32 uiWp)
    {
        switch(uiWp)
        {
            case 0:
                SetRun();
                break;
            case 4:
                if (Player* pPlayer = GetPlayerForEscort())
                    pPlayer->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                m_creature->ForcedDespawn();   // should be enough to remove players  just take the vehicle away 8)
                    return;
            default:
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        npc_escortAI::UpdateAI(uiDiff);
    }
};

/*######
## quest How To Win Friends And Influence Enemies  &&  replaces EventAI for these npcs overall
######*/
enum win_friends
{
    QUEST_HOW_TO_WIN_FRIENDS          = 12720,

    NPC_PREACHER                      = 28939,
    SPELL_HOLY_FURY_OOC               = 34809,
    SPELL_HOLY_SMITE                  = 15498,

    NPC_MARKSMEN                      = 28610,
    SPELL_RAPTOR_STRIKE               = 32915,

    SAY_PERSUADE1                     = -1609101,
    SAY_PERSUADE2                     = -1609102,
    SAY_PERSUADE3                     = -1609103,
    SAY_PERSUADE4                     = -1609104,
    SAY_PERSUADE5                     = -1609105,
    SAY_PERSUADE6                     = -1609106,
    SAY_PERSUADE7                     = -1609107,
    SAY_CRUSADER1                     = -1609108,
    SAY_CRUSADER2                     = -1609109,
    SAY_CRUSADER3                     = -1609110,
    SAY_CRUSADER4                     = -1609111,
    SAY_CRUSADER5                     = -1609112,
    SAY_CRUSADER6                     = -1609113,
    SAY_PERSUADED1                    = -1609114,
    SAY_PERSUADED2                    = -1609115,
    SAY_PERSUADED3                    = -1609116,
    SAY_PERSUADED4                    = -1609117,
    SAY_PERSUADED5                    = -1609118,
    SAY_PERSUADED6                    = -1609119,

    SPELL_PERSUASIVE_STRIKE           = 52781
};

struct MANGOS_DLL_DECL npc_crusade_persuadedAI : public ScriptedAI
{
    npc_crusade_persuadedAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    ObjectGuid m_playerGUID;
    uint32 m_uiSpeech_timer;
    uint32 m_uiSpeech_counter;
    uint32 m_uiCrusade_faction;

    uint32 m_uiHOLYFURYTimer;
    uint32 m_uiHOLYSMITETimer;
    uint32 m_uiRATORSTRIKETimer;

    void Reset()
    {
        m_uiSpeech_timer = 0;
        m_uiSpeech_counter = 0;
        m_uiCrusade_faction = 0;
        m_playerGUID.Clear();

        m_uiHOLYFURYTimer     = 60000;
        m_uiHOLYSMITETimer    = 5500;

        m_uiRATORSTRIKETimer  = 4500;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && spell->Id == SPELL_PERSUASIVE_STRIKE && m_uiSpeech_counter == 0)
        {
            if(((Player*)caster)->GetQuestStatus(QUEST_HOW_TO_WIN_FRIENDS) == QUEST_STATUS_INCOMPLETE)
            {
                if (rand()%100 > 90) // chance
                {
                    m_playerGUID = ((Player*)caster)->GetObjectGuid();
                    m_uiCrusade_faction = m_creature->getFaction();
                    m_uiSpeech_timer = 1000;
                    m_uiSpeech_counter = 1;
                    m_creature->setFaction(35);
                }
                else if (m_uiSpeech_counter == 0)
                {
                    switch(rand()%6)
                    {
                        case 0: DoScriptText(SAY_PERSUADE1, caster);break;
                        case 1: DoScriptText(SAY_PERSUADE2, caster);break;
                        case 2: DoScriptText(SAY_PERSUADE3, caster);break;
                        case 3: DoScriptText(SAY_PERSUADE4, caster);break;
                        case 4: DoScriptText(SAY_PERSUADE5, caster);break;
                        case 5: DoScriptText(SAY_PERSUADE6, caster);break;
                        case 6: DoScriptText(SAY_PERSUADE7, caster);break;
                    }
                    switch(rand()%5)
                    {
                        case 0: DoScriptText(SAY_CRUSADER1, m_creature);break;
                        case 1: DoScriptText(SAY_CRUSADER2, m_creature);break;
                        case 2: DoScriptText(SAY_CRUSADER3, m_creature);break;
                        case 3: DoScriptText(SAY_CRUSADER4, m_creature);break;
                        case 4: DoScriptText(SAY_CRUSADER5, m_creature);break;
                        case 5: DoScriptText(SAY_CRUSADER6, m_creature);break;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiSpeech_counter >= 1 && m_uiSpeech_counter <= 6)
        {
            if (m_uiSpeech_timer < diff)
            {
                m_creature->CombatStop(true);
                m_creature->StopMoving();
                if (Unit* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGUID))
                {
                    switch(m_uiSpeech_counter)
                    {
                        case 1: DoScriptText(SAY_PERSUADED1, m_creature); m_uiSpeech_timer = 8000; m_uiSpeech_counter++; break;
                        case 2: DoScriptText(SAY_PERSUADED2, m_creature); m_uiSpeech_timer = 8000; m_uiSpeech_counter++; break;
                        case 3: DoScriptText(SAY_PERSUADED3, m_creature); m_uiSpeech_timer = 8000; m_uiSpeech_counter++; break;
                        case 4: DoScriptText(SAY_PERSUADED4, m_creature); m_uiSpeech_timer = 8000; m_uiSpeech_counter++; break;
                        case 5: DoScriptText(SAY_PERSUADED5, pPlayer);    m_uiSpeech_timer = 8000; m_uiSpeech_counter++; break;
                        case 6:
                            DoScriptText(SAY_PERSUADED6, m_creature);
                            m_creature->setFaction(m_uiCrusade_faction);
                            m_uiSpeech_timer = 0;
                            m_uiCrusade_faction = 0;
                            m_uiSpeech_counter++;
                            AttackStart(pPlayer);
                            if(((Player*)pPlayer)->GetQuestStatus(QUEST_HOW_TO_WIN_FRIENDS) == QUEST_STATUS_INCOMPLETE)
                                ((Player*)pPlayer)->AreaExploredOrEventHappens(QUEST_HOW_TO_WIN_FRIENDS);
                            break;
                    }
                }
            }
            else
                m_uiSpeech_timer -= diff;
        }
        else
        {
            if (m_creature->GetEntry() == NPC_PREACHER)
            {
               if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                   return;

               if (m_uiHOLYSMITETimer < diff)
               {
                   DoCastSpellIfCan(m_creature->getVictim(), SPELL_HOLY_SMITE);
                   m_uiHOLYSMITETimer = 5500 + rand()%1400;
               }else m_uiHOLYSMITETimer -= diff;

               if (m_uiHOLYFURYTimer < diff)
               {
                   DoCastSpellIfCan(m_creature, SPELL_HOLY_FURY_OOC);
                   m_uiHOLYFURYTimer = 1000 + rand()%90000;
               }else m_uiHOLYFURYTimer -= diff;

                   DoMeleeAttackIfReady();
            }
            else
            {
                if (m_creature->GetEntry() == NPC_MARKSMEN)
                {
                    if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                    if (m_uiRATORSTRIKETimer < diff)
                    {
                        DoCastSpellIfCan(m_creature->getVictim(), SPELL_RAPTOR_STRIKE);
                        m_uiRATORSTRIKETimer = 4500 + rand()%1400;
                    }else m_uiRATORSTRIKETimer -= diff;
                   DoMeleeAttackIfReady();
                }
            }
        }
    }
};

//Scarlet courier
enum ScarletCourierEnum
{
    SAY_TREE1               = -1609531,
    SAY_TREE2               = -1609532,
    GO_INCONSPICUOUS_TREE   = 191144,
    NPC_SCARLET_COURIER     = 29076
};

struct MANGOS_DLL_DECL mob_scarlet_courierAI : ScriptedAI
{
    mob_scarlet_courierAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint8 m_uiStage;
    uint32 m_uiStageTimer;

    void Reset()
    {
        m_creature->Mount(14338); // not sure about this id
        m_uiStage = 1;
        m_uiStageTimer = 3*IN_MILLISECONDS;
    }

    void EnterCombat(Unit* pWho)
    {
        DoScriptText(SAY_TREE2, m_creature);
        m_creature->Unmount();
        m_uiStage = 0;
    }

    void MovementInform(uint32 m_uiType, uint32 m_uiId)
    {
        if (m_uiType != POINT_MOTION_TYPE)
            return;

        if (m_uiId == 1)
            m_uiStage = 2;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiStage && !m_creature->isInCombat())
        {
            if (m_uiStageTimer <= uiDiff)
            {
                switch(m_uiStage)
                {
                    case 1:
                        m_creature->SetWalk(true);
                        if (GameObject* tree = GetClosestGameObjectWithEntry(m_creature,GO_INCONSPICUOUS_TREE, 40.0f))
                        {
                            DoScriptText(SAY_TREE1, m_creature);
                            float x, y, z;
                            tree->GetContactPoint(m_creature, x, y, z);
                            m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
                        }
                        break;
                    case 2:
                        if (GameObject* tree = GetClosestGameObjectWithEntry(m_creature,GO_INCONSPICUOUS_TREE, 40.0f))
                            if (Unit *unit = tree->GetOwner())
                                AttackStart(unit);
                        break;
                    default:
                        break;
                }

                m_uiStageTimer = 3*IN_MILLISECONDS;
                m_uiStage = 0;
            }
            else
                m_uiStageTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_scarlet_courier(Creature* pCreature)
{
    return new mob_scarlet_courierAI(pCreature);
};

CreatureAI* GetAI_npc_crusade_persuaded(Creature* pCreature)
{
   return new npc_crusade_persuadedAI(pCreature);
};

CreatureAI* GetAI_npc_highlord_darion_mograine(Creature* pCreature)
{
    return new npc_highlord_darion_mograineAI(pCreature);
};

CreatureAI* GetAI_npc_the_lich_king_tirion_dawn(Creature* pCreature)
{
   return new npc_the_lich_king_tirion_dawnAI(pCreature);
};

CreatureAI* GetAI_npc_minibosses_dawn_of_light(Creature* pCreature)
{
    return new npc_minibosses_dawn_of_lightAI (pCreature);
};

CreatureAI* GetAI_mob_warrior_of_the_frozen_wastes(Creature* pCreature)
{
   return new mob_warrior_of_the_frozen_wastesAI(pCreature);
};

CreatureAI* GetAI_mob_acherus_ghoul(Creature* pCreature)
{
   return new mob_acherus_ghoulAI(pCreature);
};

CreatureAI* GetAI_npc_mine_car(Creature* pCreature)
{
    return new npc_mine_carAI(pCreature);
};

CreatureAI* GetAI_npc_scarlet_miner(Creature* pCreature)
{
    return new npc_scarlet_minerAI(pCreature);
};

CreatureAI* GetAI_npc_scourge_gryphon(Creature* pCreature)
{
    return new npc_scourge_gryphonAI(pCreature);
};

struct MANGOS_DLL_DECL npc_acherus_deathchargerAI : ScriptedAI
{
    npc_acherus_deathchargerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() { }

    void PassengerBoarded(Unit* who, int8 seatId, bool apply)
    {
        if (apply && who->GetTypeId() == TYPEID_PLAYER)
            ((Player*)who)->AreaExploredOrEventHappens(12687);
    }
};


CreatureAI* GetAI_npc_acherus_deathcharger(Creature* pCreature)
{
    return new npc_acherus_deathchargerAI(pCreature);
};

void AddSC_ebon_hold()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_a_special_surprise";
    pNewScript->GetAI = &GetAI_npc_a_special_surprise;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_death_knight_initiate";
    pNewScript->GetAI = &GetAI_npc_death_knight_initiate;
    pNewScript->pGossipHello = &GossipHello_npc_death_knight_initiate;
    pNewScript->pGossipSelect = &GossipSelect_npc_death_knight_initiate;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_koltira_deathweaver";
    pNewScript->GetAI = &GetAI_npc_koltira_deathweaver;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_koltira_deathweaver;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_unworthy_initiate";
    pNewScript->GetAI = &GetAI_npc_unworthy_initiate;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_unworthy_initiate_anchor";
    pNewScript->GetAI = &GetAI_npc_unworthy_initiate_anchor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_acherus_soul_prison";
    pNewScript->pGOUse = &GOUse_go_acherus_soul_prison;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eye_of_acherus";
    pNewScript->GetAI = &GetAI_npc_eye_of_acherus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_scarlet_miner";
    pNewScript->GetAI = &GetAI_mob_scarlet_miner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_scarlet_ghoul";
    pNewScript->GetAI = &GetAI_mob_scarlet_ghoul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_highlord_darion_mograine";
    pNewScript->GetAI = &GetAI_npc_highlord_darion_mograine;
    pNewScript->pGossipHello = &GossipHello_npc_highlord_darion_mograine;
    pNewScript->pGossipSelect = &GossipSelect_npc_highlord_darion_mograine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_the_lich_king_tirion_dawn";
    pNewScript->GetAI = &GetAI_npc_the_lich_king_tirion_dawn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_minibosses_dawn_of_light";
    pNewScript->GetAI = &GetAI_npc_minibosses_dawn_of_light;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_acherus_ghoul";
    pNewScript->GetAI = &GetAI_mob_acherus_ghoul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mine_car";
    pNewScript->GetAI = &GetAI_npc_mine_car;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scarlet_miner";
    pNewScript->GetAI = &GetAI_npc_scarlet_miner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_inconspicous_mine_car";
    pNewScript->pGOUse = &GOUse_go_inconspicous_mine_car;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_scourge_gryphon";
    pNewScript->GetAI = &GetAI_npc_scourge_gryphon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_warrior_of_the_frozen_wastes";
    pNewScript->GetAI = &GetAI_mob_warrior_of_the_frozen_wastes;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name= "npc_valkyr_battle_maiden";
    pNewScript->GetAI = &GetAI_npc_valkyr_battle_maiden;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_crusade_persuaded";
    pNewScript->GetAI = &GetAI_npc_crusade_persuaded;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name= "mob_scarlet_courier";
    pNewScript->GetAI = &GetAI_mob_scarlet_courier;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name= "mob_acherus_deathcharger";
    pNewScript->GetAI = &GetAI_npc_acherus_deathcharger;
    pNewScript->RegisterSelf();
}
