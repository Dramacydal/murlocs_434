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
SDName: Trial Of the crusader
SD%Complete: 60%
SDComment: event script by /dev/rsa
SDCategory: trial_of_the_crusader
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

struct _Messages
{
    AnnounserMessages msgnum;
    uint32 id;
    bool state;
    uint32 encounter;
};

static _Messages _GossipMessage[]=
{
    {MSG_BEASTS,GOSSIP_ACTION_INFO_DEF+1,false,TYPE_BEASTS},
    {MSG_JARAXXUS,GOSSIP_ACTION_INFO_DEF+2,false,TYPE_JARAXXUS},
    {MSG_CRUSADERS,GOSSIP_ACTION_INFO_DEF+3,false,TYPE_CRUSADERS},
    {MSG_VALKIRIES,GOSSIP_ACTION_INFO_DEF+4,false,TYPE_VALKIRIES},
    {MSG_LICH_KING,GOSSIP_ACTION_INFO_DEF+5,false,TYPE_ANUBARAK},
};

enum
{
    NUM_MESSAGES                = 5,
    SPELL_WILFRED_PORTAL        = 68424,
    SPELL_JARAXXUS_CHAINS       = 67924,
};

struct MANGOS_DLL_DECL npc_toc_announcerAI : public ScriptedAI
{
    npc_toc_announcerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_uiMapDifficulty;

    bool m_bIsHeroic;
    bool m_bIs25Man;

    uint32 DelayTimer;
    uint32 substage;

    uint32 WormsTimer;
    uint32 IcehowlTimer;

    void Reset()
    {
        if (!m_pInstance)
            return;

        WormsTimer     = 150000;
        IcehowlTimer   = 150000;

        m_pInstance->SetData(TYPE_STAGE, 0);
        DelayTimer = 0;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (Creature *pHorde = GetClosestCreatureWithEntry(m_creature, NPC_THRALL, 300.0f))
            pHorde->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (Creature *pAlly = GetClosestCreatureWithEntry(m_creature, NPC_PROUDMOORE, 300.0f))
            pAlly->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if (DelayTimer < diff)
        {
            switch (m_pInstance->GetData(TYPE_STAGE))
            {
                case 0:
                    break;
                case 1:
                    if (m_bIsHeroic)
                    {
                        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == GORMOK_IN_PROGRESS)
                        {
                            if (WormsTimer <= diff)
                            {
                                            m_pInstance->SetData(TYPE_STAGE,2);
                                            m_pInstance->SetData(TYPE_EVENT,200);
                                            m_pInstance->SetData(TYPE_NORTHREND_BEASTS,SNAKES_IN_PROGRESS);
                                            m_pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                            }
                            else
                                WormsTimer -= diff;
                        }
                    }
                    if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == GORMOK_DONE)
                    {
                        m_pInstance->SetData(TYPE_STAGE,2);
                        m_pInstance->SetData(TYPE_EVENT,200);
                        m_pInstance->SetData(TYPE_NORTHREND_BEASTS,SNAKES_IN_PROGRESS);
                        m_pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                    }
                    if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL)
                    {
                        m_pInstance->SetData(TYPE_STAGE,0);
                        m_pInstance->SetData(TYPE_EVENT,666);
                        m_pInstance->SetData(TYPE_BEASTS, NOT_STARTED);
                        m_creature->SetDeathState(JUST_DIED);
                        m_creature->Respawn();
                    }
                    break;
                case 2:
                    if (m_bIsHeroic)
                    {
                        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) != ICEHOWL_IN_PROGRESS)
                        {
                            if (IcehowlTimer <= diff)
                            {
                                m_pInstance->SetData(TYPE_STAGE,3);
                                m_pInstance->SetData(TYPE_EVENT,300);
                                m_pInstance->SetData(TYPE_NORTHREND_BEASTS,ICEHOWL_IN_PROGRESS);
                                m_pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                            }
                            else
                                IcehowlTimer -= diff;
                        }
                    }
                    if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_DONE)
                    {
                        m_pInstance->SetData(TYPE_STAGE,3);
                        m_pInstance->SetData(TYPE_EVENT,300);
                        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, ICEHOWL_IN_PROGRESS);
                        m_pInstance->SetData(TYPE_BEASTS, IN_PROGRESS);
                    }
                    if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL)
                    {
                        m_pInstance->SetData(TYPE_STAGE,0);
                        m_pInstance->SetData(TYPE_EVENT,666);
                        m_pInstance->SetData(TYPE_BEASTS, NOT_STARTED);
                        m_creature->SetDeathState(JUST_DIED);
                        m_creature->Respawn();
                    }
                    break;
                case 3:
                    if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == ICEHOWL_DONE)
                    {
                        m_pInstance->SetData(TYPE_STAGE,0);
                        m_pInstance->SetData(TYPE_BEASTS, DONE);
                        m_pInstance->SetData(TYPE_EVENT,400);
                        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, DONE);
                    }
                    if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL)
                    {
                        m_pInstance->SetData(TYPE_STAGE,0);
                        m_pInstance->SetData(TYPE_EVENT,666);
                        m_pInstance->SetData(TYPE_BEASTS, NOT_STARTED);
                        m_creature->SetDeathState(JUST_DIED);
                        m_creature->Respawn();
                    };
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    if (m_pInstance->GetData(TYPE_CRUSADERS_COUNT) == 0 && m_pInstance->GetData(TYPE_CRUSADERS) == IN_PROGRESS)
                    {
                        m_pInstance->SetData(TYPE_STAGE, 0);
                        m_pInstance->SetData(TYPE_CRUSADERS, DONE);
                        m_pInstance->SetData(TYPE_EVENT, 3100);
                    }
                    break;
                case 7:
                    if (m_pInstance->GetData(TYPE_VALKIRIES) == DONE)
                    {
                        m_pInstance->SetData(TYPE_STAGE, 0);
                        m_pInstance->SetData(TYPE_EVENT, 4020);
                        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                    }
                    if (m_pInstance->GetData(TYPE_VALKIRIES) == FAIL)
                    {
                        m_pInstance->SetData(TYPE_STAGE, 0);
                        m_pInstance->SetData(TYPE_EVENT, 0);
                    }
                    break;
                case 8:
                    break;
                case 9:
                    if (m_pInstance->GetData(TYPE_ANUBARAK) == DONE)
                    {
                        m_pInstance->SetData(TYPE_STAGE, 10);
                        m_pInstance->SetData(TYPE_EVENT, 6000);
                    }
                    if (m_pInstance->GetData(TYPE_ANUBARAK) == FAIL)
                    {
                        m_pInstance->SetData(TYPE_STAGE, 0);
                        m_pInstance->SetData(TYPE_EVENT, 0);
                    }
                    break;
                case 10:
                    //m_creature->ForcedDespawn();
                    break;
            }
        }
        else DelayTimer -= diff;
    }
};

CreatureAI* GetAI_npc_toc_announcer(Creature* pCreature)
{
    return new npc_toc_announcerAI(pCreature);
}

bool GossipHello_npc_toc_announcer(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* m_pInstance;
    m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    char const* _message;
    uint8 i;

    if (!m_pInstance)
        return false;

    if (pPlayer->isInCombat() || m_pInstance->IsEncounterInProgress() || m_pInstance->GetData(TYPE_EVENT))
        return true;

    switch (LocaleConstant currentlocale = pPlayer->GetSession()->GetSessionDbcLocale())
    {
        case LOCALE_esES:
             _message = "¡Estamos listos!";
             break;
        case LOCALE_esMX:
             _message = "¡Estamos listos!";
             break;
        case LOCALE_ruRU:
             _message = "Всегда готовы!";
             break;
        case LOCALE_enUS:
        case LOCALE_koKR:
        case LOCALE_frFR:
        case LOCALE_deDE:
        case LOCALE_zhCN:
        case LOCALE_zhTW:
        default:
             _message = "We are ready!";
             break;
    };

    for (i = 0; i < NUM_MESSAGES; i++)
    {
        if (!_GossipMessage[i].state && (m_pInstance->GetData(_GossipMessage[i].encounter) != DONE ))
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN,_GossipMessage[i].id);
            break;
        }
        if (_GossipMessage[i].state && m_pInstance->GetData(_GossipMessage[i].encounter) == DONE)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, _message, GOSSIP_SENDER_MAIN,_GossipMessage[i].id);
            break;
        }
    };

    pPlayer->SEND_GOSSIP_MENU(_GossipMessage[i].msgnum, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_toc_announcer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* pInstance;
    pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if (!pInstance)
        return false;

pPlayer->CLOSE_GOSSIP_MENU();

switch(uiAction) {
    case GOSSIP_ACTION_INFO_DEF+1: 
        if (pInstance->GetData(TYPE_BEASTS) != DONE) 
        {
            pInstance->SetData(TYPE_EVENT,110);
            pInstance->SetData(TYPE_NORTHREND_BEASTS,NOT_STARTED);
            pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
        };
        break;
    case GOSSIP_ACTION_INFO_DEF+2: 
        if (pInstance->GetData(TYPE_JARAXXUS) != DONE)
            pInstance->SetData(TYPE_EVENT,1010);

        break;
    case GOSSIP_ACTION_INFO_DEF+3: 
        if (pInstance->GetData(TYPE_CRUSADERS) != DONE) 
        {
            if (pPlayer->GetTeam() == ALLIANCE)
                pInstance->SetData(TYPE_EVENT,3000);
            else
                pInstance->SetData(TYPE_EVENT,3001);
        };
        break;
    case GOSSIP_ACTION_INFO_DEF+4: 
        if (pInstance->GetData(TYPE_VALKIRIES) != DONE)
            pInstance->SetData(TYPE_EVENT,4000);

        break;
    case GOSSIP_ACTION_INFO_DEF+5: 
        Map* pMap = pCreature->GetMap();
        Map::PlayerList const &lPlayers = pMap->GetPlayers();
        for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
        {
             Unit* pPlayer = itr->getSource();
             if (!pPlayer) 
                 continue;
             pPlayer->KnockBackFrom(pPlayer, 10.0f, 0.0f);
        }
        if (GameObject* pGoFloor = pInstance->GetSingleGameObjectFromStorage(GO_ARGENT_COLISEUM_FLOOR))
        {
             pGoFloor->SetUInt32Value(GAMEOBJECT_DISPLAYID,9060);
             pGoFloor->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
             pGoFloor->SetUInt32Value(GAMEOBJECT_BYTES_1,8449);
        }
        pCreature->CastSpell(pCreature,69016,false);

        Creature* pTemp = pInstance->GetSingleCreatureFromStorage(NPC_ANUBARAK);

        if (!pTemp || !pTemp->isAlive())
            pCreature->SummonCreature(NPC_ANUBARAK, SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
     
        if (pCreature->GetVisibility() == VISIBILITY_ON)
            pCreature->SetVisibility(VISIBILITY_OFF);

        break;
};

return true;
}

struct MANGOS_DLL_DECL boss_lich_king_tocAI : public ScriptedAI
{
    boss_lich_king_tocAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        WayPointList.clear();
        JustRespawned();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;
    uint32 event_state_lich_king;
    bool Event;
    bool MovementStarted;
    std::list<WayPoints> WayPointList;
    std::list<WayPoints>::iterator WayPoint;
    uint32 WalkTimer;
    bool IsWalking;
    ObjectGuid m_uiPortalGuid;

    void Reset()
    {
        UpdateTimer = 0;
        event_state_lich_king = 0;
        Event = false;
        MovementStarted = false;
        m_creature->SetRespawnDelay(DAY);
        if (Creature* pPortal = m_creature->SummonCreature(NPC_TRIGGER, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME))
        {
            pPortal->SetRespawnDelay(DAY);
            pPortal->CastSpell(pPortal, 51807, false);
            pPortal->SetDisplayId(17612);
            m_uiPortalGuid = pPortal->GetObjectGuid();
        }
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LICH_KING,IN_PROGRESS);
    }

    Creature* GetCreatureFromGuid(ObjectGuid m_uiGuid)
    {
        if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_uiGuid))
            return pCreature;

        return NULL;
    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void JustRespawned()
    {
        Reset();
    }

    void StartMovement()
    {
        if(!WayPointList.empty() || MovementStarted)
            return;

        AddWaypoint(0, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z);
        AddWaypoint(1, SpawnLoc[18].x, SpawnLoc[18].y, SpawnLoc[18].z);
        AddWaypoint(2, SpawnLoc[18].x, SpawnLoc[18].y, SpawnLoc[18].z);
        m_creature->SetWalk(true);
        WayPoint = WayPointList.begin();
        MovementStarted = true;
        IsWalking = true;
        WalkTimer = 200;
        event_state_lich_king = 1;
        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);
    }

    void AddWaypoint(uint32 id, float x, float y, float z)
    {
        WayPoints wp(id, x, y, z);
        WayPointList.push_back(wp);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_LICH_KING_1)
            return;

        if (!MovementStarted)
            StartMovement();

        if (IsWalking && WalkTimer)
        {
            if (WalkTimer <= diff)
            {
                if (WayPoint != WayPointList.end())
                {
                    m_creature->GetMotionMaster()->MovePoint(WayPoint->id, WayPoint->x, WayPoint->y,WayPoint->z);
                    WalkTimer = 0;
                }
            }
            else
                WalkTimer -= diff;
        }

        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
            case 5010:
                DoScriptText(-1713550,m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,5020);
                break;
            case 5030:
                DoScriptText(-1713552,m_creature);
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                UpdateTimer =  10000;
                m_pInstance->SetData(TYPE_EVENT,5040);
                break;
            case 5040:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_NONE);
                UpdateTimer =  1000;
                m_pInstance->SetData(TYPE_EVENT,5050);
                break;
            case 5050:
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_EXCLAMATION);
                UpdateTimer =  9000;
                m_pInstance->SetData(TYPE_EVENT,5060);
                break;
            case 5060:
                if (Event)
                {
                    DoScriptText(-1713553,m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_KNEEL);
                    UpdateTimer =  2500;
                    m_pInstance->SetData(TYPE_EVENT,5070);
                }
                break;
            case 5070:
            {
                m_creature->CastSpell(m_creature,68198,false);
                Map* pMap = m_creature->GetMap();
                Map::PlayerList const &lPlayers = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                     Unit* pPlayer = itr->getSource();
                     if (!pPlayer) 
                         continue;

                     pPlayer->KnockBackFrom(pPlayer, 10.0f, 0.0f);
                }
                UpdateTimer = 1500;
                m_pInstance->SetData(TYPE_EVENT,5080);
                break;
            }
            case 5080:
                if (GameObject* pGoFloor = m_pInstance->GetSingleGameObjectFromStorage(GO_ARGENT_COLISEUM_FLOOR))
                {
                    pGoFloor->SetUInt32Value(GAMEOBJECT_DISPLAYID,9060);
                    pGoFloor->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED | GO_FLAG_NODESPAWN);
                    pGoFloor->SetUInt32Value(GAMEOBJECT_BYTES_1,8449);
                }

                m_creature->CastSpell(m_creature,69016,false);

                if(m_pInstance)
                    m_pInstance->SetData(TYPE_LICH_KING,DONE);

                if (Creature *pAnubarak = m_creature->SummonCreature(NPC_ANUBARAK, SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 3.14f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME))
                    pAnubarak->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                m_pInstance->SetData(TYPE_STAGE,9);
                Event=false;
                m_creature->ForcedDespawn();

                if (Creature* pPortal = GetCreatureFromGuid(m_uiPortalGuid))
                    pPortal->ForcedDespawn();

                m_pInstance->SetData(TYPE_EVENT,0);
                UpdateTimer = 20000;
                break;
            }
        }
        else
            UpdateTimer -= diff;

        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if(m_pInstance)
        {
            if(id == 2)
            {
                Event = true;
            }
        }
        if(type != POINT_MOTION_TYPE)
            return;

        if(WayPoint->id != id)
            return;

        ++WayPoint;
        WalkTimer = 200;
    }
};

CreatureAI* GetAI_boss_lich_king_toc(Creature* pCreature)
{
    return new boss_lich_king_tocAI(pCreature);
};

struct MANGOS_DLL_DECL npc_fizzlebang_tocAI : public ScriptedAI
{
    npc_fizzlebang_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (BSWScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    BSWScriptedInstance* m_pInstance;
    uint32 UpdateTimer;
    ObjectGuid m_uiPortalGuid;
    ObjectGuid m_uiTriggerGuid;

    Creature* GetCreatureFromGuid(ObjectGuid m_uiGuid)
    {
        if (Creature* pCreature = m_creature->GetMap()->GetCreature(m_uiGuid))
            return pCreature;

        return NULL;
    }

    void JustDied(Unit* pKiller)
    {
        m_pInstance->SetData(TYPE_EVENT, 1179);

        if (Creature* pPortal = GetCreatureFromGuid(m_uiPortalGuid))
            pPortal->ForcedDespawn();

        m_creature->ForcedDespawn(5000);
    }

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->GetMotionMaster()->MovePoint(1, SpawnLoc[27].x, SpawnLoc[27].y, SpawnLoc[27].z);
        m_uiPortalGuid.Clear();
    }

    void UpdateAI(const uint32 diff)
    {
        if(!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_FIZZLEBANG)
            return;

        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);
        if(UpdateTimer <= diff)
        {
                switch(m_pInstance->GetData(TYPE_EVENT))
                {
                case 1110:
                    m_pInstance->SetData(TYPE_EVENT, 1120);
                    UpdateTimer = 3000;
                    m_pInstance->SetData(TYPE_JARAXXUS,IN_PROGRESS);
                    break;
                case 1120:
                    DoScriptText(-1713511, m_creature);
                    m_pInstance->SetData(TYPE_EVENT, 1130);
                    UpdateTimer = 12000;
                    break;
                case 1130:
                    m_creature->GetMotionMaster()->MovementExpired();
                    m_creature->HandleEmoteCommand(EMOTE_STATE_SPELL_CHANNEL_OMNI);

                    if (Creature* pPortal = m_creature->SummonCreature(NPC_WILFRED_PORTAL, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                    {
                        pPortal->SetRespawnDelay(DAY);
                        pPortal->SetDisplayId(22862);
                        pPortal->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.5f);
                        m_uiPortalGuid = pPortal->GetObjectGuid();
                    }

                    DoScriptText(-1713512, m_creature);
                    m_pInstance->SetData(TYPE_EVENT, 1132);
                    UpdateTimer = 4000;
                    break;
               case 1132:
                    m_creature->GetMotionMaster()->MovementExpired();
                    m_pInstance->SetData(TYPE_EVENT, 1134);
                    UpdateTimer = 1000;
                    break;
               case 1134:
                    if (Creature* pPortal = GetCreatureFromGuid(m_uiPortalGuid))
                        pPortal->ForcedDespawn(16000);

                    if (Creature* pTrigger =  m_creature->SummonCreature(NPC_TRIGGER, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5.0f, TEMPSUMMON_MANUAL_DESPAWN, 5000))
                    {
                        pTrigger->SetDisplayId(17612);
                        pTrigger->CastSpell(pTrigger, SPELL_WILFRED_PORTAL, false);
                        pTrigger->SetRespawnDelay(DAY);
                        pTrigger->SetFloatValue(OBJECT_FIELD_SCALE_X, 2.0f);
                        m_uiTriggerGuid = pTrigger->GetObjectGuid();
                    }

                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SPELLCAST_OMNI);
                    UpdateTimer = 4000;
                    m_pInstance->SetData(TYPE_EVENT, 1135);
                    break;
               case 1135:
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_SPELLCAST_OMNI);
                    UpdateTimer = 3000;
                    m_pInstance->SetData(TYPE_EVENT, 1140);
                    break;
               case 1140:
                    m_pInstance->SetData(TYPE_STAGE,4);
                    m_creature->SummonCreature(NPC_JARAXXUS, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JARAXXUS))
                    {
                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        pTemp->SetWalk(true);
                        pTemp->GetMotionMaster()->MovePoint(1, SpawnLoc[28].x, SpawnLoc[28].y, SpawnLoc[28].z);
                    }
                    m_pInstance->SetData(TYPE_EVENT, 1142);
                    UpdateTimer = 3000;
                    break;
               case 1142:
                    if (Creature* pTrigger = GetCreatureFromGuid(m_uiTriggerGuid))
                        pTrigger->RemoveAurasDueToSpell(SPELL_WILFRED_PORTAL);

                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JARAXXUS))
                        pTemp->SetFacingToObject(m_creature);

                    DoScriptText(-1713513, m_creature);
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    m_pInstance->SetData(TYPE_EVENT, 1144);
                    UpdateTimer = 3000;
                    break;
               case 1144:
                    if (Creature* pTrigger = GetCreatureFromGuid(m_uiTriggerGuid))
                        pTrigger->ForcedDespawn();

                    m_pInstance->SetData(TYPE_EVENT, 1145);
                    UpdateTimer = 9000;
                    break;
               case 1145:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JARAXXUS))
                    {
                        DoScriptText(-1713514, pTemp);
                        pTemp->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    }

                    m_pInstance->SetData(TYPE_EVENT, 1150);
                    UpdateTimer = 5000;
                    break;
               case 1150:
                    DoScriptText(-1713515, m_creature);
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JARAXXUS))
                        pTemp->CastSpell(m_creature, 66532, false);

                    m_pInstance->SetData(TYPE_EVENT, 1160);
                    UpdateTimer = 4000;
                    break;
               case 1160:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JARAXXUS))
                        pTemp->SetInCombatWithZone();

                    UpdateTimer = 1000;
                    break;
              }
        }
        else
            UpdateTimer -= diff;

        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }
};

CreatureAI* GetAI_npc_fizzlebang_toc(Creature* pCreature)
{
    return new npc_fizzlebang_tocAI(pCreature);
}

struct MANGOS_DLL_DECL npc_tirion_tocAI : public ScriptedAI
{
    npc_tirion_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;
    uint32 crusader[12];
    uint8 crusaderscount;

    void Reset()
    {
        crusaderscount = 0;
        memset(&crusader, 0, sizeof(crusader));
    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;
        if (m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_TIRION)
            return;

        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
            case 110:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                DoScriptText(-1713500, m_creature);
                UpdateTimer = 22000;
                m_pInstance->SetData(TYPE_EVENT,130);
                break;
            case 140:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                DoScriptText(-1713501, m_creature);
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,150);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 150:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                if (m_pInstance->GetData(TYPE_BEASTS) != DONE)
                {
                    m_creature->SummonCreature(NPC_GORMOK, SpawnLoc[26].x, SpawnLoc[26].y, SpawnLoc[26].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_GORMOK))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->SetWalk(true);
                        pTemp->SetInCombatWithZone();
                    }
                }
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,160);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                m_pInstance->SetData(TYPE_STAGE,1);
                m_pInstance->SetData(TYPE_BEASTS,IN_PROGRESS);
                break;
            case 200:
                DoScriptText(-1713503, m_creature);
                UpdateTimer = 4000;
                m_pInstance->SetData(TYPE_EVENT,205);
                break;
            case 205:
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,210);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 210:
                if (m_pInstance->GetData(TYPE_BEASTS) != DONE)
                {
                    m_creature->SummonCreature(NPC_DREADSCALE, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    m_creature->SummonCreature(NPC_ACIDMAW, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DREADSCALE))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->SetWalk(true);
                        pTemp->SetInCombatWithZone();
                    }
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_ACIDMAW))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->SetWalk(true);
                        pTemp->SetInCombatWithZone();
                    }
                }
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,220);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 300:
                DoScriptText(-1713505, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,305);
                break;
            case 305:
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,310);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 310:
                if (m_pInstance->GetData(TYPE_BEASTS) != DONE)
                {
                    m_creature->SummonCreature(NPC_ICEHOWL, SpawnLoc[26].x, SpawnLoc[26].y, SpawnLoc[26].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_ICEHOWL))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->SetWalk(true);
                        pTemp->SetInCombatWithZone();
                    }
                }
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,320);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 400:
                DoScriptText(-1713509, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,0);
                break;
            case 666:
                DoScriptText(-1713709, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,0);
                m_pInstance->SetData(TYPE_NORTHREND_BEASTS,NOT_STARTED);
                break;
            case 1010:
                DoScriptText(-1713510, m_creature);
                UpdateTimer = 5000;
                m_creature->SummonCreature(NPC_FIZZLEBANG, SpawnLoc[21].x, SpawnLoc[21].y, SpawnLoc[21].z, 2, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                m_pInstance->SetData(TYPE_EVENT,1110);
                break;
            case 1179:
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT, 1180);
                break;
            case 1180:
                DoScriptText(-1713516, m_creature);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,0);
                break;

            case 1999:
                UpdateTimer = 6000;
                m_pInstance->SetData(TYPE_EVENT,2000);
                break;
            case 2000:
                DoScriptText(-1713526, m_creature);
                UpdateTimer = 15000;
                m_pInstance->SetData(TYPE_EVENT,2010);
                break;
            case 2030:
                DoScriptText(-1713529, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,0);
                break;
            case 3000:
                DoScriptText(-1713530, m_creature);
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,3050);
                break;
            case 3001:
                DoScriptText(-1713530, m_creature);
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,3051);
                break;
            case 3060:
                DoScriptText(-1713532, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3070);
                break;
            case 3061:
                DoScriptText(-1713532, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3071);
                break;
            //Summoning crusaders
            case 3091:
                m_pInstance->SetData(TYPE_STAGE,6);
                if (m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_NORMAL ||  m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                    crusaderscount = 12;
                    switch (urand(0,3))
                    {                                       // Healers, 3 in 25-mode
                    case 0:
                        crusader[0] = NPC_CRUSADER_1_1;
                        crusader[1] = NPC_CRUSADER_1_12;
                        crusader[2] = NPC_CRUSADER_1_13;
                        break;
                    case 1:
                        crusader[0] = NPC_CRUSADER_1_1;
                        crusader[1] = NPC_CRUSADER_1_2;
                        crusader[2] = NPC_CRUSADER_1_13;
                        break;
                    case 2:
                        crusader[0] = NPC_CRUSADER_1_1;
                        crusader[1] = NPC_CRUSADER_1_2;
                        crusader[2] = NPC_CRUSADER_1_12;
                        break;
                    case 3:
                        crusader[0] = NPC_CRUSADER_1_2;
                        crusader[1] = NPC_CRUSADER_1_12;
                        crusader[2] = NPC_CRUSADER_1_13;
                        break;
                    }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 25-mode
                    case 0:
                        crusader[3] = NPC_CRUSADER_1_3;
                        crusader[4] = NPC_CRUSADER_1_4;
                        break;
                    case 1:
                        crusader[3] = NPC_CRUSADER_1_3;
                        crusader[4] = NPC_CRUSADER_1_5;
                        break;
                    case 2:
                        crusader[3] = NPC_CRUSADER_1_3;
                        crusader[4] = NPC_CRUSADER_1_6;
                        break;
                    case 3:
                        crusader[3] = NPC_CRUSADER_1_4;
                        crusader[4] = NPC_CRUSADER_1_5;
                        break;
                    case 4:
                        crusader[3] = NPC_CRUSADER_1_4;
                        crusader[4] = NPC_CRUSADER_1_6;
                        break;
                    case 5:
                        crusader[3] = NPC_CRUSADER_1_5;
                        crusader[4] = NPC_CRUSADER_1_6;
                        break;
                    }
                    switch (urand(0,3))
                    {                                       // Random magic DD, 3 in 25-mode
                    case 0:
                        crusader[5] = NPC_CRUSADER_1_7;
                        crusader[6] = NPC_CRUSADER_1_8;
                        crusader[7] = NPC_CRUSADER_1_11;
                        break;
                    case 1:
                        crusader[5] = NPC_CRUSADER_1_7;
                        crusader[6] = NPC_CRUSADER_1_8;
                        crusader[7] = NPC_CRUSADER_1_14;
                        break;
                    case 2:
                        crusader[5] = NPC_CRUSADER_1_8;
                        crusader[6] = NPC_CRUSADER_1_11;
                        crusader[7] = NPC_CRUSADER_1_14;
                        break;
                    case 3:
                        crusader[5] = NPC_CRUSADER_1_7;
                        crusader[6] = NPC_CRUSADER_1_11;
                        crusader[7] = NPC_CRUSADER_1_14;
                        break;
                    }
                    crusader[8]  = NPC_CRUSADER_1_9;  //Hunter+warlock
                    crusader[9]  = NPC_CRUSADER_1_10;
                    crusader[10] = NPC_CRUSADER_0_1;
                    crusader[11] = NPC_CRUSADER_0_2;
                }
                else
                {
                    crusaderscount = 6;
                    switch (urand(0,5))
                    {                                       // Healers, 2 in 10-mode
                    case 0:
                        crusader[0] = NPC_CRUSADER_1_1;
                        crusader[1] = NPC_CRUSADER_1_12;
                        break;
                    case 1:
                        crusader[0] = NPC_CRUSADER_1_1;
                        crusader[1] = NPC_CRUSADER_1_2;
                        break;
                    case 2:
                        crusader[0] = NPC_CRUSADER_1_2;
                        crusader[1] = NPC_CRUSADER_1_12;
                        break;
                    case 3:
                        crusader[0] = NPC_CRUSADER_1_1;
                        crusader[1] = NPC_CRUSADER_1_13;
                        break;
                    case 4:
                        crusader[0] = NPC_CRUSADER_1_2;
                        crusader[1] = NPC_CRUSADER_1_13;
                        break;
                    case 5:
                        crusader[0] = NPC_CRUSADER_1_12;
                        crusader[1] = NPC_CRUSADER_1_13;
                        break;
                    }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 10-mode
                    case 0:
                        crusader[3] = NPC_CRUSADER_1_3;
                        crusader[2] = NPC_CRUSADER_1_4;
                        break;
                    case 1:
                        crusader[3] = NPC_CRUSADER_1_3;
                        crusader[2] = NPC_CRUSADER_1_5;
                        break;
                    case 2:
                        crusader[3] = NPC_CRUSADER_1_3;
                        crusader[2] = NPC_CRUSADER_1_6;
                        break;
                    case 3:
                        crusader[3] = NPC_CRUSADER_1_4;
                        crusader[2] = NPC_CRUSADER_1_5;
                        break;
                    case 4:
                        crusader[3] = NPC_CRUSADER_1_4;
                        crusader[2] = NPC_CRUSADER_1_6;
                        break;
                    case 5:
                        crusader[3] = NPC_CRUSADER_1_5;
                        crusader[2] = NPC_CRUSADER_1_6;
                        break;
                    }
                    switch (urand(0,5))
                    {                                       // Random magic DD, 2 in 10-mode
                    case 0:
                        crusader[4] = NPC_CRUSADER_1_7;
                        crusader[5] = NPC_CRUSADER_1_8;
                        break;
                    case 1:
                        crusader[5] = NPC_CRUSADER_1_7;
                        crusader[4] = NPC_CRUSADER_1_14;
                        break;
                    case 2:
                        crusader[5] = NPC_CRUSADER_1_7;
                        crusader[4] = NPC_CRUSADER_1_11;
                        break;
                    case 3:
                        crusader[5] = NPC_CRUSADER_1_8;
                        crusader[4] = NPC_CRUSADER_1_11;
                        break;
                    case 4:
                        crusader[5] = NPC_CRUSADER_1_8;
                        crusader[4] = NPC_CRUSADER_1_14;
                        break;
                    case 5:
                        crusader[5] = NPC_CRUSADER_1_11;
                        crusader[4] = NPC_CRUSADER_1_14;
                        break;
                    }
                }
                for(uint8 i = 0; i < crusaderscount; ++i)
                {
                    m_creature->SummonCreature(crusader[i], SpawnLoc[i+2].x, SpawnLoc[i+2].y, SpawnLoc[i+2].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(crusader[i]))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->SetWalk(true);
                    }
                }
                m_pInstance->SetData(TYPE_CRUSADERS_COUNT,crusaderscount);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,0);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                m_pInstance->SetData(TYPE_CRUSADERS,IN_PROGRESS);
                break;
            //summoning crusaders
            case 3090:
                m_pInstance->SetData(TYPE_STAGE,6);
                if (m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_NORMAL ||  m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                {
                    crusaderscount = 12;
                    switch (urand(0,3))
                    {                                       // Healers, 3 in 25-mode
                    case 0:
                        crusader[0] = NPC_CRUSADER_2_1;
                        crusader[1] = NPC_CRUSADER_2_12;
                        crusader[2] = NPC_CRUSADER_2_13;
                        break;
                    case 1:
                        crusader[0] = NPC_CRUSADER_2_1;
                        crusader[1] = NPC_CRUSADER_2_2;
                        crusader[2] = NPC_CRUSADER_2_13;
                        break;
                    case 2:
                        crusader[0] = NPC_CRUSADER_2_1;
                        crusader[1] = NPC_CRUSADER_2_2;
                        crusader[2] = NPC_CRUSADER_2_12;
                        break;
                    case 3:
                        crusader[0] = NPC_CRUSADER_2_2;
                        crusader[1] = NPC_CRUSADER_2_12;
                        crusader[2] = NPC_CRUSADER_2_13;
                        break;
                    }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 25-mode
                    case 0:
                        crusader[3] = NPC_CRUSADER_2_3;
                        crusader[4] = NPC_CRUSADER_2_4;
                        break;
                    case 1:
                        crusader[3] = NPC_CRUSADER_2_3;
                        crusader[4] = NPC_CRUSADER_2_5;
                        break;
                    case 2:
                        crusader[3] = NPC_CRUSADER_2_3;
                        crusader[4] = NPC_CRUSADER_2_6;
                        break;
                    case 3:
                        crusader[3] = NPC_CRUSADER_2_4;
                        crusader[4] = NPC_CRUSADER_2_5;
                        break;
                    case 4: crusader[3] = NPC_CRUSADER_2_4;
                        crusader[4] = NPC_CRUSADER_2_6;
                        break;
                    case 5:
                        crusader[3] = NPC_CRUSADER_2_5;
                        crusader[4] = NPC_CRUSADER_2_6;
                        break;
                    }

                    switch (urand(0,3))
                    {                                       // Random magic DD, 3 in 25-mode
                    case 0:
                        crusader[5] = NPC_CRUSADER_2_7;
                        crusader[6] = NPC_CRUSADER_2_8;
                        crusader[7] = NPC_CRUSADER_2_11;
                        break;
                    case 1:
                        crusader[5] = NPC_CRUSADER_2_7;
                        crusader[6] = NPC_CRUSADER_2_8;
                        crusader[7] = NPC_CRUSADER_2_14;
                        break;
                    case 2:
                        crusader[5] = NPC_CRUSADER_2_8;
                        crusader[6] = NPC_CRUSADER_2_11;
                        crusader[7] = NPC_CRUSADER_2_14;
                        break;
                    case 3:
                        crusader[5] = NPC_CRUSADER_2_7;
                        crusader[6] = NPC_CRUSADER_2_11;
                        crusader[7] = NPC_CRUSADER_2_14;
                        break;
                    }
                    crusader[8]  = NPC_CRUSADER_2_9;  //Hunter+warlock
                    crusader[9]  = NPC_CRUSADER_2_10;
                    crusader[10] = NPC_CRUSADER_0_1;
                    crusader[11] = NPC_CRUSADER_0_2;
                }
                else
                {
                    crusaderscount = 6;
                    switch (urand(0,5))
                    {                                       // Healers, 2 in 10-mode
                    case 0:
                        crusader[0] = NPC_CRUSADER_2_1;
                        crusader[1] = NPC_CRUSADER_2_12;
                        break;
                    case 1:
                        crusader[0] = NPC_CRUSADER_2_1;
                        crusader[1] = NPC_CRUSADER_2_2;
                        break;
                    case 2:
                        crusader[0] = NPC_CRUSADER_2_2;
                        crusader[1] = NPC_CRUSADER_2_12;
                        break;
                    case 3:
                        crusader[0] = NPC_CRUSADER_2_1;
                        crusader[1] = NPC_CRUSADER_2_13;
                        break;
                    case 4:
                        crusader[0] = NPC_CRUSADER_2_2;
                        crusader[1] = NPC_CRUSADER_2_13;
                        break;
                    case 5:
                        crusader[0] = NPC_CRUSADER_2_12;
                        crusader[1] = NPC_CRUSADER_2_13;
                        break;
                    }
                    switch (urand(0,5))
                    {                                       // Random melee DD, 2 in 10-mode
                    case 0:
                        crusader[3] = NPC_CRUSADER_2_3;
                        crusader[2] = NPC_CRUSADER_2_4;
                        break;
                    case 1:
                        crusader[3] = NPC_CRUSADER_2_3;
                        crusader[2] = NPC_CRUSADER_2_5;
                        break;
                    case 2:
                        crusader[3] = NPC_CRUSADER_2_3;
                        crusader[2] = NPC_CRUSADER_2_6;
                        break;
                    case 3:
                        crusader[3] = NPC_CRUSADER_2_4;
                        crusader[2] = NPC_CRUSADER_2_5;
                        break;
                    case 4:
                        crusader[3] = NPC_CRUSADER_2_4;
                        crusader[2] = NPC_CRUSADER_2_6;
                        break;
                    case 5:
                        crusader[3] = NPC_CRUSADER_2_5;
                        crusader[2] = NPC_CRUSADER_2_6;
                        break;
                    }
                    switch (urand(0,5))
                    {                                       // Random magic DD, 2 in 10-mode
                    case 0:
                        crusader[4] = NPC_CRUSADER_2_7;
                        crusader[5] = NPC_CRUSADER_2_8;
                        break;
                    case 1:
                        crusader[5] = NPC_CRUSADER_2_7;
                        crusader[4] = NPC_CRUSADER_2_14;
                        break;
                    case 2:
                        crusader[5] = NPC_CRUSADER_2_7;
                        crusader[4] = NPC_CRUSADER_2_11;
                        break;
                    case 3:
                        crusader[5] = NPC_CRUSADER_2_8;
                        crusader[4] = NPC_CRUSADER_2_11;
                        break;
                    case 4:
                        crusader[5] = NPC_CRUSADER_2_8;
                        crusader[4] = NPC_CRUSADER_2_14;
                    break;
                    case 5:
                        crusader[5] = NPC_CRUSADER_2_11;
                        crusader[4] = NPC_CRUSADER_2_14;
                        break;
                    }
                }
                for(uint8 i = 0; i < crusaderscount; ++i)
                {
                    m_creature->SummonCreature(crusader[i], SpawnLoc[i+2].x, SpawnLoc[i+2].y, SpawnLoc[i+2].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(crusader[i]))
                    {
                        pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                        pTemp->SetWalk(true);
                    }
                }
                m_pInstance->SetData(TYPE_CRUSADERS_COUNT,crusaderscount);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,0);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                m_pInstance->SetData(TYPE_CRUSADERS, IN_PROGRESS);
                break;
            //Crusaders battle end
            case 3100:
                DoScriptText(-1713535, m_creature);
                UpdateTimer = 5000;
                m_creature->CastSpell(m_creature, 68184, true);
                m_pInstance->SetData(TYPE_EVENT, 0);
                break;
            case 4000:
                if (GameObject* pGO10N = m_pInstance->GetSingleGameObjectFromStorage(GO_CRUSADERS_CACHE_10))
                    pGO10N->Delete();

                if (GameObject* pGO10H = m_pInstance->GetSingleGameObjectFromStorage(GO_CRUSADERS_CACHE_25))
                    pGO10H->Delete();

                if (GameObject* pGO25N = m_pInstance->GetSingleGameObjectFromStorage(GO_CRUSADERS_CACHE_10_H))
                    pGO25N->Delete();

                if (GameObject* pGO25H = m_pInstance->GetSingleGameObjectFromStorage(GO_CRUSADERS_CACHE_25_H))
                    pGO25H->Delete();

                DoScriptText(-1713536, m_creature);
                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,4010);
                break;
            case 4010:
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,4015);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 4015:
                m_pInstance->SetData(TYPE_STAGE,7);
                m_pInstance->SetData(TYPE_VALKIRIES,IN_PROGRESS);
                m_creature->SummonCreature(NPC_LIGHTBANE, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_LIGHTBANE))
                {
                    pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                    pTemp->SetWalk(true);
                    pTemp->SetInCombatWithZone();
                }
                m_creature->SummonCreature(NPC_DARKBANE, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 5, TEMPSUMMON_CORPSE_TIMED_DESPAWN, DESPAWN_TIME);
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DARKBANE))
                {
                    pTemp->GetMotionMaster()->MovePoint(0, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                    pTemp->SetWalk(true);
                    pTemp->SetInCombatWithZone();
                }
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,4016);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 4040:
                UpdateTimer = 60000;
                m_pInstance->SetData(TYPE_EVENT,5000);
                break;
            case 5000:
                DoScriptText(-1713549, m_creature);
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,5005);
                break;
            case 5005:
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,5010);
                m_pInstance->SetData(TYPE_STAGE,8);
                m_creature->SummonCreature(NPC_LICH_KING_1, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 5, TEMPSUMMON_MANUAL_DESPAWN, 0);
                break;
            case 5020:
                DoScriptText(-1713551, m_creature);
                UpdateTimer = 8000;
                m_pInstance->SetData(TYPE_EVENT,5030);
                break;
            case 6000:
                m_creature->NearTeleportTo(SpawnLoc[19].x, SpawnLoc[19].y, SpawnLoc[19].z, 3.15f);
                UpdateTimer = 20000;
                m_pInstance->SetData(TYPE_EVENT,6005);
                break;
            case 6005:
                DoScriptText(-1713565, m_creature);
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_TALK);
                m_creature->SummonGameobject(195682, 783.57f, 124.45f, 142.21f, 0, 0);
                UpdateTimer = 20000;
                m_pInstance->SetData(TYPE_EVENT,6010);
                break;
            case 6010:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_NONE);
                if (m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_10MAN_HEROIC || m_pInstance->GetData(TYPE_DIFFICULTY) == RAID_DIFFICULTY_25MAN_HEROIC)
                    DoScriptText(-1713566, m_creature);

                UpdateTimer = 60000;
                m_pInstance->SetData(TYPE_EVENT,6020);
                break;
            case 6020:
                m_pInstance->SetData(TYPE_STAGE,10);
                m_creature->ForcedDespawn();
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,6030);
                break;
            }
        }
        else
            UpdateTimer -= diff;

        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }
};

CreatureAI* GetAI_npc_tirion_toc(Creature* pCreature)
{
    return new npc_tirion_tocAI(pCreature);
}

struct MANGOS_DLL_DECL npc_garrosh_tocAI : public ScriptedAI
{
    npc_garrosh_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_GARROSH)
            return;

        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
            case 120:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_ONESHOT_TALK);
                DoScriptText(-1713702, m_creature);
                m_pInstance->SetData(TYPE_EVENT,121);
                UpdateTimer = 5000;
                break;
            case 2010:
                DoScriptText(-1713527, m_creature);
                UpdateTimer = 10000;
                m_pInstance->SetData(TYPE_EVENT,2020);
                break;
            case 3050:
                DoScriptText(-1713531, m_creature);
                UpdateTimer = 15000;
                m_pInstance->SetData(TYPE_EVENT,3060);
                break;
            case 3070:
                DoScriptText(-1713533, m_creature);
                UpdateTimer = 6000;
                m_pInstance->SetData(TYPE_EVENT,3080);
                break;
            case 3081:
                DoScriptText(-1713734, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3091);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 4030:
                DoScriptText(-1713748, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,4040);
                break;
            }
        }
        else
            UpdateTimer -= diff;

        m_pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
    }
};

CreatureAI* GetAI_npc_garrosh_toc(Creature* pCreature)
{
    return new npc_garrosh_tocAI(pCreature);
};

struct MANGOS_DLL_DECL npc_rinn_tocAI : public ScriptedAI
{
    npc_rinn_tocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)m_creature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_EVENT_NPC) != NPC_RINN)
            return;

        UpdateTimer = m_pInstance->GetData(TYPE_EVENT_TIMER);

        if (UpdateTimer <= diff)
        {
            switch (m_pInstance->GetData(TYPE_EVENT))
            {
            case 130:
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_TIRION))
                   DoScriptText(-1713537, pTemp);

                UpdateTimer = 3000;
                m_pInstance->SetData(TYPE_EVENT,140);
                break;
            case 2020:
                DoScriptText(-1713528, m_creature);
                UpdateTimer = 7000;
                m_pInstance->SetData(TYPE_EVENT,2030);
                break;
            case 3051:
                DoScriptText(-1713731, m_creature);
                UpdateTimer = 15000;
                m_pInstance->SetData(TYPE_EVENT,3061);
                break;
            case 3071:
                DoScriptText(-1713733, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3081);
                break;
            case 3080:
                DoScriptText(-1713534, m_creature);
                UpdateTimer = 5000;
                m_pInstance->SetData(TYPE_EVENT,3090);
                m_pInstance->DoUseDoorOrButton(GO_MAIN_GATE_DOOR);
                break;
            case 4020:
                DoScriptText(-1713548, m_creature);
                UpdateTimer = 7000;
                if (Creature* pBarrent = m_pInstance->GetSingleCreatureFromStorage(NPC_BARRENT))
                    pBarrent->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

                m_pInstance->SetData(TYPE_EVENT,4030);
                break;
            }
        }
        else
            UpdateTimer -= diff;

        m_pInstance->SetData(TYPE_EVENT_TIMER,UpdateTimer);
    }
};

CreatureAI* GetAI_npc_rinn_toc(Creature* pCreature)
{
    return new npc_rinn_tocAI(pCreature);
};

void AddSC_trial_of_the_crusader()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_toc_announcer";
    pNewScript->GetAI = &GetAI_npc_toc_announcer;
    pNewScript->pGossipHello = &GossipHello_npc_toc_announcer;
    pNewScript->pGossipSelect = &GossipSelect_npc_toc_announcer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_lich_king_toc";
    pNewScript->GetAI = &GetAI_boss_lich_king_toc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fizzlebang_toc";
    pNewScript->GetAI = &GetAI_npc_fizzlebang_toc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tirion_toc";
    pNewScript->GetAI = &GetAI_npc_tirion_toc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_garrosh_toc";
    pNewScript->GetAI = &GetAI_npc_garrosh_toc;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rinn_toc";
    pNewScript->GetAI = &GetAI_npc_rinn_toc;
    pNewScript->RegisterSelf();
}
