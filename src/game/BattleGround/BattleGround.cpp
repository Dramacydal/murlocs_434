/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
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

#include "Object.h"
#include "Player.h"
#include "BattleGround.h"
#include "BattleGroundMgr.h"
#include "Creature.h"
#include "MapManager.h"
#include "Language.h"
#include "SpellAuras.h"
#include "ArenaTeam.h"
#include "World.h"
#include "Group.h"
#include "ObjectGuid.h"
#include "ObjectMgr.h"
#include "Mail.h"
#include "WorldPacket.h"
#include "Util.h"
#include "Formulas.h"
#include "GridNotifiersImpl.h"
#include "Guild.h"
#include "GuildMgr.h"

namespace MaNGOS
{
    class BattleGroundChatBuilder
    {
        public:
            BattleGroundChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, va_list* args = NULL)
                : i_msgtype(msgtype), i_textId(textId), i_source(source), i_args(args) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);

                if (i_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap,*i_args);

                    char str [2048];
                    vsnprintf(str,2048,text, ap );
                    va_end(ap);

                    do_helper(data,&str[0]);
                }
                else
                    do_helper(data,text);
            }
        private:
            void do_helper(WorldPacket& data, char const* text)
            {
                ObjectGuid targetGuid = i_source ? i_source ->GetObjectGuid() : ObjectGuid();

                data << uint8(i_msgtype);
                data << uint32(LANG_UNIVERSAL);
                data << ObjectGuid(targetGuid);             // there 0 for BG messages
                data << uint32(0);                          // can be chat msg group or something
                data << ObjectGuid(targetGuid);
                data << uint32(strlen(text)+1);
                data << text;
                data << uint8(i_source ? i_source->GetChatTag() : uint8(CHAT_TAG_NONE));
            }

            ChatMsg i_msgtype;
            int32 i_textId;
            Player const* i_source;
            va_list* i_args;
    };

    class BattleGroundYellBuilder
    {
        public:
            BattleGroundYellBuilder(uint32 language, int32 textId, Creature const* source, va_list* args = NULL)
                : i_language(language), i_textId(textId), i_source(source), i_args(args) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);

                if(i_args)
                {
                    // we need copy va_list before use or original va_list will corrupted
                    va_list ap;
                    va_copy(ap,*i_args);

                    char str [2048];
                    vsnprintf(str,2048,text, ap );
                    va_end(ap);

                    do_helper(data,&str[0]);
                }
                else
                    do_helper(data,text);
            }
        private:
            void do_helper(WorldPacket& data, char const* text)
            {
                //copyied from BuildMonsterChat
                data << uint8(CHAT_MSG_MONSTER_YELL);
                data << uint32(i_language);
                data << ObjectGuid(i_source->GetObjectGuid());
                data << uint32(0);                          // 2.1.0
                data << uint32(strlen(i_source->GetName())+1);
                data << i_source->GetName();
                data << ObjectGuid();                       // Unit Target - isn't important for bgs
                data << uint32(strlen(text)+1);
                data << text;
                data << uint8(CHAT_TAG_NONE);               // ChatTag - for bgs allways 0?
            }

            uint32 i_language;
            int32 i_textId;
            Creature const* i_source;
            va_list* i_args;
    };

    class BattleGround2ChatBuilder
    {
        public:
            BattleGround2ChatBuilder(ChatMsg msgtype, int32 textId, Player const* source, int32 arg1, int32 arg2)
                : i_msgtype(msgtype), i_textId(textId), i_source(source), i_arg1(arg1), i_arg2(arg2) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);
                char const* arg1str = i_arg1 ? sObjectMgr.GetMangosString(i_arg1,loc_idx) : "";
                char const* arg2str = i_arg2 ? sObjectMgr.GetMangosString(i_arg2,loc_idx) : "";

                char str [2048];
                snprintf(str,2048,text, arg1str, arg2str );

                ObjectGuid targetGuid = i_source  ? i_source ->GetObjectGuid() : ObjectGuid();

                data << uint8(i_msgtype);
                data << uint32(LANG_UNIVERSAL);
                data << ObjectGuid(targetGuid);             // there 0 for BG messages
                data << uint32(0);                          // can be chat msg group or something
                data << ObjectGuid(targetGuid);
                data << uint32(strlen(str)+1);
                data << str;
                data << uint8(i_source ? i_source->GetChatTag() : uint8(CHAT_TAG_NONE));
            }
        private:

            ChatMsg i_msgtype;
            int32 i_textId;
            Player const* i_source;
            int32 i_arg1;
            int32 i_arg2;
    };

    class BattleGroundWarningBuilder
    {
        public:
            BattleGroundWarningBuilder(int32 textId)
                : i_textId(textId) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);

                data << uint8(CHAT_MSG_RAID_BOSS_EMOTE);
                data << uint32(LANG_UNIVERSAL);
                data << ObjectGuid();
                data << uint32(0);
                data << uint32(1);
                data << uint8(0);
                data << ObjectGuid();
                data << uint32(strlen(text)+1);
                data << text;
                data << uint8(CHAT_TAG_NONE);
            }
        private:

            int32 i_textId;
    };

    class BattleGround2YellBuilder
    {
        public:
            BattleGround2YellBuilder(uint32 language, int32 textId, Creature const* source, int32 arg1, int32 arg2)
                : i_language(language), i_textId(textId), i_source(source), i_arg1(arg1), i_arg2(arg2) {}
            void operator()(WorldPacket& data, int32 loc_idx)
            {
                char const* text = sObjectMgr.GetMangosString(i_textId,loc_idx);
                char const* arg1str = i_arg1 ? sObjectMgr.GetMangosString(i_arg1,loc_idx) : "";
                char const* arg2str = i_arg2 ? sObjectMgr.GetMangosString(i_arg2,loc_idx) : "";

                char str [2048];
                snprintf(str, 2048, text, arg1str, arg2str);
                //copyied from BuildMonsterChat
                data << uint8(CHAT_MSG_MONSTER_YELL);
                data << uint32(i_language);
                data << ObjectGuid(i_source->GetObjectGuid());
                data << uint32(0);                          // 2.1.0
                data << uint32(strlen(i_source->GetName())+1);
                data << i_source->GetName();
                data << ObjectGuid();                       // Unit Target - isn't important for bgs
                data << uint32(strlen(str)+1);
                data << str;
                data << uint8(CHAT_TAG_NONE);               // ChatTag - for bgs allways 0?
            }
        private:

            uint32 i_language;
            int32 i_textId;
            Creature const* i_source;
            int32 i_arg1;
            int32 i_arg2;
    };
}                                                           // namespace MaNGOS

template<class Do>
void BattleGround::BroadcastWorker(Do& _do)
{
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        if (Player *plr = ObjectAccessor::FindPlayer(itr->first))
            _do(plr);
}

BattleGround::BattleGround()
{
    m_TypeID            = BattleGroundTypeId(0);
    m_RandomTypeID      = BattleGroundTypeId(0);
    m_Status            = STATUS_NONE;
    m_ClientInstanceID  = 0;
    m_EndTime           = 0;
    m_BracketId         = BG_BRACKET_ID_TEMPLATE;
    m_InvitedAlliance   = 0;
    m_InvitedHorde      = 0;
    m_ArenaType         = ARENA_TYPE_NONE;
    m_IsArena           = false;
    m_Winner            = TEAM_NONE;
    m_StartTime         = 0;
    m_CountdownTimer    = 0;
    m_Events            = 0;
    m_IsRated           = false;
    m_BuffChange        = false;
    m_IsRandom          = false;
    m_Name              = "";
    m_LevelMin          = 0;
    m_LevelMax          = 0;
    m_InBGFreeSlotQueue = false;

    m_MaxPlayersPerTeam = 0;
    m_MaxPlayers        = 0;
    m_MinPlayersPerTeam = 0;
    m_MinPlayers        = 0;

    m_MapId             = 0;
    m_Map               = NULL;

    m_TeamStartLocX[TEAM_INDEX_ALLIANCE]    = 0;
    m_TeamStartLocX[TEAM_INDEX_HORDE]       = 0;

    m_TeamStartLocY[TEAM_INDEX_ALLIANCE]    = 0;
    m_TeamStartLocY[TEAM_INDEX_HORDE]       = 0;

    m_TeamStartLocZ[TEAM_INDEX_ALLIANCE]    = 0;
    m_TeamStartLocZ[TEAM_INDEX_HORDE]       = 0;

    m_TeamStartLocO[TEAM_INDEX_ALLIANCE]    = 0;
    m_TeamStartLocO[TEAM_INDEX_HORDE]       = 0;

    m_ArenaTeamIds[TEAM_INDEX_ALLIANCE]     = 0;
    m_ArenaTeamIds[TEAM_INDEX_HORDE]        = 0;

    m_ArenaTeamRatingChanges[TEAM_INDEX_ALLIANCE]   = 0;
    m_ArenaTeamRatingChanges[TEAM_INDEX_HORDE]      = 0;

    m_BgRaids[TEAM_INDEX_ALLIANCE]          = NULL;
    m_BgRaids[TEAM_INDEX_HORDE]             = NULL;

    m_PlayersCount[TEAM_INDEX_ALLIANCE]     = 0;
    m_PlayersCount[TEAM_INDEX_HORDE]        = 0;

    m_TeamScores[TEAM_INDEX_ALLIANCE]       = 0;
    m_TeamScores[TEAM_INDEX_HORDE]          = 0;

    m_PrematureCountDown = false;
    m_PrematureCountDownTimer = 0;

    m_ArenaEnded = false;

    m_ArenaGameId = 0;

    m_arenaChartHolder.bg = this;

    m_StartDelayTimes[BG_STARTING_EVENT_FIRST]  = BG_START_DELAY_2M;
    m_StartDelayTimes[BG_STARTING_EVENT_SECOND] = BG_START_DELAY_1M;
    m_StartDelayTimes[BG_STARTING_EVENT_THIRD]  = BG_START_DELAY_30S;
    m_StartDelayTimes[BG_STARTING_EVENT_FOURTH] = BG_START_DELAY_NONE;
    //we must set to some default existing values
    m_StartMessageIds[BG_STARTING_EVENT_FIRST]  = 0;
    m_StartMessageIds[BG_STARTING_EVENT_SECOND] = LANG_BG_WS_START_ONE_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_THIRD]  = LANG_BG_WS_START_HALF_MINUTE;
    m_StartMessageIds[BG_STARTING_EVENT_FOURTH] = LANG_BG_WS_HAS_BEGUN;

    m_BuggersCheckTimer = 1000;
}

BattleGround::~BattleGround()
{
    // remove objects and creatures
    // (this is done automatically in mapmanager update, when the instance is reset after the reset time)

    int size = m_BgObjects.size();
    for(int i = 0; i < size; ++i)
        DelObject(i);

    sBattleGroundMgr.RemoveBattleGround(GetInstanceID(), GetTypeID());

    // skip template bgs as they were never added to visible bg list
    BattleGroundBracketId bracketId = GetBracketId();
    if (bracketId != BG_BRACKET_ID_TEMPLATE)
        sBattleGroundMgr.DeleteClientVisibleInstanceId(GetTypeID(), bracketId, GetClientInstanceID());

    // unload map
    // map can be null at bg destruction
    if (m_Map)
        m_Map->SetUnload();

    // remove from bg free slot queue
    this->RemoveFromBGFreeSlotQueue();

    for(BattleGroundScoreMap::const_iterator itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
        delete itr->second;
}

void BattleGround::Update(uint32 diff)
{
    if (!GetPlayersSize())
    {
        // BG is empty
        // if there are no players invited, delete BG
        // this will delete arena or bg object, where any player entered
        // [[   but if you use battleground object again (more battles possible to be played on 1 instance)
        //      then this condition should be removed and code:
        //      if (!GetInvitedCount(HORDE) && !GetInvitedCount(ALLIANCE))
        //          this->AddToFreeBGObjectsQueue(); // not yet implemented
        //      should be used instead of current
        // ]]
        // BattleGround Template instance cannot be updated, because it would be deleted
        if (!GetInvitedCount(HORDE) && !GetInvitedCount(ALLIANCE))
            delete this;

        return;
    }

    // remove offline players from bg after 5 minutes
    if (!m_OfflineQueue.empty())
    {
        BattleGroundPlayerMap::iterator itr = m_Players.find(*(m_OfflineQueue.begin()));
        if (itr != m_Players.end())
        {
            if (itr->second.OfflineRemoveTime <= sWorld.GetGameTime())
            {
                RemovePlayerAtLeave(itr->first, true, true);// remove player from BG
                m_OfflineQueue.pop_front();                 // remove from offline queue
                //do not use itr for anything, because it is erased in RemovePlayerAtLeave()
            }
        }
    }

    /*********************************************************/
    /***           BATTLEGROUND BALLANCE SYSTEM            ***/
    /*********************************************************/

    // if less then minimum players are in on one side, then start premature finish timer
    if (GetStatus() == STATUS_IN_PROGRESS && !isArena() && sBattleGroundMgr.GetPrematureFinishTime() && (GetPlayersCountByTeam(ALLIANCE) < GetMinPlayersPerTeam() || GetPlayersCountByTeam(HORDE) < GetMinPlayersPerTeam()))
    {
        if (!m_PrematureCountDown)
        {
            m_PrematureCountDown = true;
            m_PrematureCountDownTimer = sBattleGroundMgr.GetPrematureFinishTime();
        }
        else if (m_PrematureCountDownTimer < diff)
        {
            // time's up!
            Team winner = TEAM_NONE;
            if (GetPlayersCountByTeam(ALLIANCE) >= GetMinPlayersPerTeam())
                winner = ALLIANCE;
            else if (GetPlayersCountByTeam(HORDE) >= GetMinPlayersPerTeam())
                winner = HORDE;

            EndBattleGround(winner);
            m_PrematureCountDown = false;
        }
        else if (!sBattleGroundMgr.isTesting())
        {
            uint32 newtime = m_PrematureCountDownTimer - diff;
            // announce every minute
            if (newtime > (MINUTE * IN_MILLISECONDS))
            {
                if (newtime / (MINUTE * IN_MILLISECONDS) != m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS))
                    PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING, CHAT_MSG_SYSTEM, NULL, (uint32)(m_PrematureCountDownTimer / (MINUTE * IN_MILLISECONDS)));
            }
            else
            {
                //announce every 15 seconds
                if (newtime / (15 * IN_MILLISECONDS) != m_PrematureCountDownTimer / (15 * IN_MILLISECONDS))
                    PSendMessageToAll(LANG_BATTLEGROUND_PREMATURE_FINISH_WARNING_SECS, CHAT_MSG_SYSTEM, NULL, (uint32)(m_PrematureCountDownTimer / IN_MILLISECONDS));
            }
            m_PrematureCountDownTimer = newtime;
        }
    }
    else if (m_PrematureCountDown)
        m_PrematureCountDown = false;

    /*********************************************************/
    /***           ARENA BUFF OBJECT SPAWNING              ***/
    /*********************************************************/
    if (isArena() && !m_ArenaBuffSpawned)
    {
        // 60 seconds after start the buffobjects in arena should get spawned
        if (m_StartTime > uint32(m_StartDelayTimes[BG_STARTING_EVENT_FIRST] + ARENA_SPAWN_BUFF_OBJECTS))
        {
            SpawnEventWithRespawn(ARENA_BUFF_EVENT, 0, 60);
            m_ArenaBuffSpawned = true;
        }
    }

    /*********************************************************/
    /***           BATTLEGROUND STARTING SYSTEM            ***/
    /*********************************************************/

    if (GetStatus() == STATUS_WAIT_JOIN && GetPlayersSize())
    {
        ModifyStartDelayTime(diff);

         // Send packet every 10 seconds until the 2nd field reach 0
        if (m_CountdownTimer >= 10000)
        {
            uint32 countdownMaxForBGType = isArena() ? ARENA_COUNTDOWN_MAX : BATTLEGROUND_COUNTDOWN_MAX;
        
            WorldPacket data(SMSG_START_TIMER, 4+4+4);
            data << uint32(0); // unk
            data << uint32(countdownMaxForBGType - (m_StartTime / 1000));
            data << uint32(countdownMaxForBGType);

            for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
                if (Player* player = sObjectMgr.GetPlayer(itr->first))
                    player->GetSession()->SendPacket(&data);

            m_CountdownTimer = 0;
        }
        else
            m_CountdownTimer += diff;

        if (!(m_Events & BG_STARTING_EVENT_1))
        {
            m_Events |= BG_STARTING_EVENT_1;

            // setup here, only when at least one player has ported to the map
            if (!SetupBattleGround())
            {
                EndNow();
                return;
            }

            StartingEventCloseDoors();
            SetStartDelayTime(m_StartDelayTimes[BG_STARTING_EVENT_FIRST]);
            //first start warning - 2 or 1 minute, only if defined
            if (m_StartMessageIds[BG_STARTING_EVENT_FIRST])
                SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_FIRST], CHAT_MSG_BG_SYSTEM_NEUTRAL);
        }
        // After 1 minute or 30 seconds, warning is signalled
        else if (GetStartDelayTime() <= m_StartDelayTimes[BG_STARTING_EVENT_SECOND] && !(m_Events & BG_STARTING_EVENT_2))
        {
            m_Events |= BG_STARTING_EVENT_2;
            if (m_StartMessageIds[BG_STARTING_EVENT_SECOND])
                SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_SECOND], CHAT_MSG_BG_SYSTEM_NEUTRAL);
        }
        // After 30 or 15 seconds, warning is signalled
        else if (GetStartDelayTime() <= m_StartDelayTimes[BG_STARTING_EVENT_THIRD] && !(m_Events & BG_STARTING_EVENT_3))
        {
            m_Events |= BG_STARTING_EVENT_3;
            if (m_StartMessageIds[BG_STARTING_EVENT_THIRD])
                SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_THIRD], CHAT_MSG_BG_SYSTEM_NEUTRAL);
        }
        // delay expired (atfer 2 or 1 minute)
        else if (GetStartDelayTime() <= 0 && !(m_Events & BG_STARTING_EVENT_4))
        {
            m_Events |= BG_STARTING_EVENT_4;

            StartingEventOpenDoors();

            if (m_StartMessageIds[BG_STARTING_EVENT_FOURTH])
                SendMessageToAll(m_StartMessageIds[BG_STARTING_EVENT_FOURTH], CHAT_MSG_BG_SYSTEM_NEUTRAL);
            SetStatus(STATUS_IN_PROGRESS);
            SetStartDelayTime(m_StartDelayTimes[BG_STARTING_EVENT_FOURTH]);

            // remove preparation
            if (isArena())
            {
                GetBgMap()->SetVisibilityDistance(World::GetMaxVisibleDistanceInBGArenas());

                for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
                {
                    if (Player* plr = sObjectMgr.GetPlayer(itr->first))
                    {
                        // BG Status packet
                        WorldPacket status;
                        BattleGroundQueueTypeId bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(m_TypeID, GetArenaType());
                        uint32 queueSlot = plr->GetBattleGroundQueueIndex(bgQueueTypeId);
                        sBattleGroundMgr.BuildBattleGroundStatusPacket(&status, this, plr, queueSlot, STATUS_IN_PROGRESS, 0, GetStartTime(), GetArenaType());
                        plr->GetSession()->SendPacket(&status);

                        Unit::SpellAuraHolderMap& auraMap = plr->GetSpellAuraHolderMap();
                        for (Unit::SpellAuraHolderMap::iterator iter = auraMap.begin(); iter != auraMap.end();)
                        {
                            SpellAuraHolder* holder = iter->second;
                            if (!(holder->GetSpellProto()->AttributesEx4 & SPELL_ATTR_EX4_UNK21) &&
                                                                                // don't remove stances, shadowform, pally/hunter auras
                                !holder->IsPassive() &&                         // don't remove passive auras
                                (!(holder->GetSpellProto()->Attributes & SPELL_ATTR_UNAFFECTED_BY_INVULNERABILITY) ||
                                !(holder->GetSpellProto()->Attributes & SPELL_ATTR_UNK8)) &&
                                                                                // not unaffected by invulnerability auras or not having that unknown flag (that seemed the most probable)
                                holder->GetSpellProto()->GetDispel() != DISPEL_NONE &&
                                holder->GetAuraMaxDuration() > 0 && iter->second->GetAuraMaxDuration() <= 30 * IN_MILLISECONDS &&
                                holder->IsPositive())
                            {
                                plr->RemoveSpellAuraHolder(holder);
                                iter = auraMap.begin();
                            }
                            else
                                ++iter;
                        }

                        plr->RemoveAurasDueToSpell(SPELL_ARENA_PREPARATION);
                    }
                }

                CheckArenaWinConditions();
            }
            else
            {

                PlaySoundToAll(SOUND_BG_START);

                for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
                    if (Player* plr = sObjectMgr.GetPlayer(itr->first))
                        plr->RemoveAurasDueToSpell(SPELL_PREPARATION);
                //Announce BG starting
                if (sWorld.getConfig(CONFIG_BOOL_BATTLEGROUND_QUEUE_ANNOUNCER_START))
                {
                    sWorld.SendWorldText(LANG_BG_STARTED_ANNOUNCE_WORLD, GetName(), GetMinLevel(), GetMaxLevel());
                }
            }
        }
    }

    /*********************************************************/
    /***           BATTLEGROUND ENDING SYSTEM              ***/
    /*********************************************************/

    if (GetStatus() == STATUS_WAIT_LEAVE)
    {
        // remove all players from battleground after 2 minutes
        m_EndTime -= diff;
        if (m_EndTime <= 0)
        {
            m_EndTime = 0;
            BattleGroundPlayerMap::iterator itr, next;
            for(itr = m_Players.begin(); itr != m_Players.end(); itr = next)
            {
                next = itr;
                ++next;
                //itr is erased here!
                RemovePlayerAtLeave(itr->first, true, true);// remove player from BG
                // do not change any battleground's private variables
            }
        }
    }

    // Arena time limit
    if(isArena() && !m_ArenaEnded)
    {
        if(m_StartTime > uint32(ARENA_TIME_LIMIT))
        {
            EndBattleGround(TEAM_NONE);
            m_ArenaEnded = true;
        }
    }

    if (m_BuggersCheckTimer <= diff)
    {
        CheckBuggers();
        m_BuggersCheckTimer = 1000;
    }
    else
        m_BuggersCheckTimer -= diff;

    //update start time
    m_StartTime += diff;
}

void BattleGround::SetTeamStartLoc(Team team, float X, float Y, float Z, float O)
{
    TeamIndex teamIdx = GetTeamIndex(team);
    m_TeamStartLocX[teamIdx] = X;
    m_TeamStartLocY[teamIdx] = Y;
    m_TeamStartLocZ[teamIdx] = Z;
    m_TeamStartLocO[teamIdx] = O;
}

void BattleGround::SendPacketToAll(WorldPacket *packet)
{
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        if (Player *plr = sObjectMgr.GetPlayer(itr->first))
            plr->GetSession()->SendPacket(packet);
        else
            ERROR_LOG("BattleGround:SendPacketToAll: %s not found!", itr->first.GetString().c_str());
    }
}

void BattleGround::SendPacketToTeam(Team teamId, WorldPacket *packet, Player *sender, bool self)
{
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        Player *plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
        {
            ERROR_LOG("BattleGround:SendPacketToTeam: %s not found!", itr->first.GetString().c_str());
            continue;
        }

        if (!self && sender == plr)
            continue;

        Team team = itr->second.PlayerTeam;
        if (!team) team = plr->GetTeam();

        if (team == teamId)
            plr->GetSession()->SendPacket(packet);
    }
}

void BattleGround::PlaySoundToAll(uint32 SoundID)
{
    WorldPacket data;
    sBattleGroundMgr.BuildPlaySoundPacket(&data, SoundID);
    SendPacketToAll(&data);
}

void BattleGround::PlaySoundToTeam(uint32 SoundID, Team teamId)
{
    WorldPacket data;

    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        Player *plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
        {
            ERROR_LOG("BattleGround:PlaySoundToTeam: %s not found!", itr->first.GetString().c_str());
            continue;
        }

        Team team = itr->second.PlayerTeam;
        if(!team) team = plr->GetTeam();

        if (team == teamId)
        {
            sBattleGroundMgr.BuildPlaySoundPacket(&data, SoundID);
            plr->GetSession()->SendPacket(&data);
        }
    }
}

void BattleGround::CastSpellOnTeam(uint32 SpellID, Team teamId)
{
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        Player *plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            ERROR_LOG("BattleGround:CastSpellOnTeam: %s not found!", itr->first.GetString().c_str());
            continue;
        }

        Team team = itr->second.PlayerTeam;
        if(!team) team = plr->GetTeam();

        if (team == teamId)
            plr->CastSpell(plr, SpellID, true);
    }
}

void BattleGround::RewardHonorToTeam(uint32 Honor, Team teamId)
{
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        Player *plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            ERROR_LOG("BattleGround:RewardHonorToTeam: %s not found!", itr->first.GetString().c_str());
            continue;
        }

        Team team = itr->second.PlayerTeam;
        if(!team) team = plr->GetTeam();

        float premiumMod = plr->IsPremiumActive() ? plr->GetPremiumHonorModifier() : 1.0f;
        if (team == teamId)
            UpdatePlayerScore(plr, SCORE_BONUS_HONOR, uint32(Honor * premiumMod));
    }
}

void BattleGround::RewardReputationToTeam(uint32 faction_id, uint32 Reputation, Team teamId)
{
    FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

    if (!factionEntry)
        return;

    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        Player *plr = sObjectMgr.GetPlayer(itr->first);

        if (!plr)
        {
            ERROR_LOG("BattleGround:RewardReputationToTeam: %s not found!", itr->first.GetString().c_str());
            continue;
        }

        Team team = itr->second.PlayerTeam;
        if(!team) team = plr->GetTeam();

        if (team == teamId)
            plr->GetReputationMgr().ModifyReputation(factionEntry, Reputation * sWorld.getConfig(CONFIG_FLOAT_RATE_REPUTATION_GAIN));
    }
}

void BattleGround::RewardXpToTeam(uint32 Xp, float percentOfLevel, Team teamId)
{
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        Player *plr = sObjectMgr.GetPlayer(itr->first);
        
        if (!plr)
        {
            ERROR_LOG("BattleGround:RewardXpToTeam: Player (%s not found!", itr->first.GetString().c_str());
            continue;
        }

        // no xp rewards for lvl 85+
        if (plr->getLevel() >= sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
            continue;

        Team team = itr->second.PlayerTeam;
        if (!team) team = plr->GetTeam();
        
        if (team == teamId)
        {
            uint32 gain = Xp;
            if(gain == 0 && percentOfLevel != 0)
            {
                percentOfLevel = percentOfLevel / 100;
                gain = uint32(float(plr->GetUInt32Value(PLAYER_NEXT_LEVEL_XP)) * percentOfLevel);
            }
            plr->GiveXP(gain, NULL);
        }
    }
}

void BattleGround::UpdateWorldState(uint32 Field, uint32 Value)
{
    WorldPacket data;
    sBattleGroundMgr.BuildUpdateWorldStatePacket(&data, Field, Value);
    SendPacketToAll(&data);
}

void BattleGround::UpdateWorldStateForPlayer(uint32 Field, uint32 Value, Player *Source)
{
    WorldPacket data;
    sBattleGroundMgr.BuildUpdateWorldStatePacket(&data, Field, Value);
    Source->GetSession()->SendPacket(&data);
}

void BattleGround::EndBattleGround(Team winner)
{
    this->RemoveFromBGFreeSlotQueue();

    ArenaTeam* winner_arena_team = NULL;
    ArenaTeam* loser_arena_team = NULL;

    uint32 loser_matchmaker_rating = 0;
    uint32 winner_matchmaker_rating = 0;

    uint32 loser_rating = 0;
    uint32 winner_rating = 0;

    int32  loser_matchmaker_change = 0;
    int32  winner_matchmaker_change = 0;

    int32 winner_change = 0;
    int32 loser_change = 0;

    WorldPacket data;
    int32 winmsg_id = 0;

    if (winner == ALLIANCE)
    {
        winmsg_id = isBattleGround() ? LANG_BG_A_WINS : LANG_ARENA_GOLD_WINS;

        if (!m_endSoundPlayed)
            PlaySoundToAll(SOUND_ALLIANCE_WINS);            // alliance wins sound
    }
    else if (winner == HORDE)
    {
        winmsg_id = isBattleGround() ? LANG_BG_H_WINS : LANG_ARENA_GREEN_WINS;

        if (!m_endSoundPlayed)
            PlaySoundToAll(SOUND_HORDE_WINS);               // horde wins sound
    }

    SetWinner(winner);

    SetStatus(STATUS_WAIT_LEAVE);
    //we must set it this way, because end time is sent in packet!
    m_EndTime = TIME_TO_AUTOREMOVE;

    // arena rating calculation
    if (isArena() && isRated())
    {
        winner_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(winner));
        loser_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(winner)));
        if (winner_arena_team && loser_arena_team && winner != TEAM_NONE)
        {
            loser_rating = loser_arena_team->GetRating();
            winner_rating = winner_arena_team->GetRating();

            loser_matchmaker_rating = GetArenaMatchmakerRating(GetOtherTeam(winner));
            winner_matchmaker_rating = GetArenaMatchmakerRating(winner);

            winner_matchmaker_change = winner_arena_team->WonAgainst(winner_matchmaker_rating, loser_matchmaker_rating, winner_change);
            loser_matchmaker_change = loser_arena_team->LostAgainst(loser_matchmaker_rating, winner_matchmaker_rating, loser_change);

            DEBUG_LOG("--- Winner rating: %u, Loser rating: %u, Winner change: %i, Loser change: %i ---", winner_rating, loser_rating, winner_change, loser_change);
            SetArenaMatchmakerRating(winner, winner_matchmaker_rating + winner_matchmaker_change);
            SetArenaMatchmakerRating(GetOtherTeam(winner), loser_matchmaker_rating + loser_matchmaker_change);

            SetArenaTeamRatingChangeForTeam(winner, winner_change);
            SetArenaTeamRatingChangeForTeam(GetOtherTeam(winner), loser_change);

            m_arenaChartHolder.FillArenaChartAtEnd(winner_arena_team, loser_arena_team, winner_rating, loser_rating, winner_change, loser_change);
        }
        else if (winner_arena_team && loser_arena_team && winner == TEAM_NONE)
        {
            SetArenaTeamRatingChangeForTeam(ALLIANCE, ARENA_TIMEOUT_RATING_CHANGE);
            SetArenaTeamRatingChangeForTeam(HORDE, ARENA_TIMEOUT_RATING_CHANGE);
            winner_arena_team->FinishGame(ARENA_TIMEOUT_RATING_CHANGE);
            loser_arena_team->FinishGame(ARENA_TIMEOUT_RATING_CHANGE);
            m_arenaChartHolder.FillArenaChartAtEnd(winner_arena_team, loser_arena_team, winner_rating, loser_rating, winner_change, loser_change);
        }
        else
        {
            SetArenaTeamRatingChangeForTeam(ALLIANCE, 0);
            SetArenaTeamRatingChangeForTeam(HORDE, 0);
            //m_arenaChartHolder.FillArenaChartAtEnd(winner_arena_team, loser_arena_team, winner_rating, loser_rating, winner_change, loser_change);
        }
    }

    std::set<ObjectGuid> guildAwarded;
    for(BattleGroundPlayerMap::iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        Team team = itr->second.PlayerTeam;

        if (itr->second.OfflineRemoveTime)
        {
            //if rated arena match - make member lost!
            if (isArena() && isRated() && winner_arena_team && loser_arena_team)
            {
                if (team == winner)
                    winner_arena_team->OfflineMemberLost(itr->first, loser_matchmaker_rating, winner_matchmaker_change);
                else
                    loser_arena_team->OfflineMemberLost(itr->first, winner_matchmaker_rating, loser_matchmaker_change);
            }
            continue;
        }

        Player *plr = sObjectMgr.GetPlayer(itr->first);
        if (!plr)
        {
            ERROR_LOG("BattleGround:EndBattleGround %s not found!", itr->first.GetString().c_str());
            continue;
        }

        // should remove spirit of redemption
        if (plr->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            plr->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

        if (!plr->isAlive())
        {
            plr->ResurrectPlayer(1.0f);
            plr->SpawnCorpseBones();
        }
        else
        {
            //needed cause else in av some creatures will kill the players at the end
            plr->CombatStop();
            plr->getHostileRefManager().deleteReferences();
        }

        //this line is obsolete - team is set ALWAYS
        //if(!team) team = plr->GetTeam();

        // per player calculation
        if (isArena() && isRated() && winner_arena_team && loser_arena_team)
        {
            plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_PLAY_ARENA, uint32(winner_arena_team->GetType()));

            if (team == winner)
            {
                // update achievement BEFORE personal rating update
                ArenaTeamMember* member = winner_arena_team->GetMember(plr->GetObjectGuid());
                if (member)
                {
                    plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, member->personal_rating);
                    plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_ARENA, uint32(winner_arena_team->GetType()));
                }

                winner_arena_team->MemberWon(plr, loser_matchmaker_rating, winner_matchmaker_change);
                plr->ModifyCurrencyCount(CURRENCY_CONQUEST_ARENA_META, sWorld.getConfig(CONFIG_UINT32_CURRENCY_ARENA_CONQUEST_POINTS_REWARD));

                // reward guild rep and xp if in guild group
                if (GetBgMap()->HasGuildGroup(plr->GetGuildGuid(), plr))
                {
                    uint32 guildXP = uint32(27900 * sWorld.getConfig(CONFIG_FLOAT_RATE_GUILD_XP_MODIFIER));
                    uint32 guildRep = uint32(27900 * sWorld.getConfig(CONFIG_FLOAT_RATE_GUILD_REPUTATION_GAIN) / 450);

                    if (Guild* guild = sGuildMgr.GetGuildByGuid(plr->GetGuildGuid()))
                        guild->GiveXP(guildXP, plr);
                    plr->RewardGuildReputation(guildRep);
                }

                if (member)
                {
                    plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_PERSONAL_RATING, GetArenaType(), member->personal_rating);
                    plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HIGHEST_TEAM_RATING, GetArenaType(), winner_arena_team->GetStats().rating);
                }
            }
            else
            {
                loser_arena_team->MemberLost(plr, winner_matchmaker_rating, loser_matchmaker_change);

                // Arena lost => reset the win_rated_arena having the "no_loose" condition
                plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, ACHIEVEMENT_CRITERIA_CONDITION_NO_LOOSE);
            }
        }

        uint32 win_kills, win_arena, loose_kills;
        if (plr->getLevel() == sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL))
        {
            win_kills = plr->GetRandomWinner() ? BG_REWARD_WINNER_HONOR_LAST : BG_REWARD_WINNER_HONOR_FIRST;
            loose_kills = plr->GetRandomWinner() ? BG_REWARD_LOOSER_HONOR_LAST : BG_REWARD_LOOSER_HONOR_FIRST;
            win_arena = plr->GetRandomWinner() ? BG_REWARD_WINNER_CONQUEST_LAST : BG_REWARD_WINNER_CONQUEST_FIRST;
        }
        else
        {
            win_kills = plr->GetRandomWinner() ? BG_REWARD_WINNER_HONOR_LAST_LOW : BG_REWARD_WINNER_HONOR_FIRST_LOW;
            loose_kills = plr->GetRandomWinner() ? BG_REWARD_LOOSER_HONOR_LAST_LOW : BG_REWARD_LOOSER_HONOR_FIRST_LOW;
            win_arena = 0;
        }

        win_kills = uint32(win_kills * sWorld.getConfig(CONFIG_FLOAT_RATE_HONOR_RANDOMBG));
        loose_kills = uint32(loose_kills * sWorld.getConfig(CONFIG_FLOAT_RATE_HONOR_RANDOMBG));

        if (team == winner)
        {
            RewardMark(plr,ITEM_WINNER_COUNT);
            RewardQuestComplete(plr);

            if (IsRandom() || BattleGroundMgr::IsBGWeekend(GetTypeID()))
            {
                UpdatePlayerScore(plr, SCORE_BONUS_HONOR, win_kills);
                plr->ModifyCurrencyCount(CURRENCY_CONQUEST_BG_META, win_arena * GetCurrencyPrecision(CURRENCY_CONQUEST_BG_META));
                if(!plr->GetRandomWinner())
                    plr->SetRandomWinner(true);
            }

            if (ObjectGuid guildGuid = plr->GetGuildGuid())
            {
                if (guildAwarded.find(guildGuid) == guildAwarded.end())
                {
                    if (Guild* guild = sGuildMgr.GetGuildByGuid(plr->GetGuildGuid()))
                    {
                        if (GetBgMap()->HasGuildGroup(guildGuid, plr))
                        {
                            // Rated Battlegrounds only
                            //if (isRated())
                            //    guild->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_BG, 1, 0, NULL, 0, plr);
                            if (isArena() && isRated() && winner_arena_team && loser_arena_team && winner_arena_team != loser_arena_team)
                                guild->GetAchievementMgr().UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_RATED_ARENA, std::max<uint32>(winner_arena_team->GetRating(), 1), 0, NULL, 0, plr);
                            guildAwarded.insert(guildGuid);
                        }
                    }
                }
            }
        }
        else
        {
            RewardMark(plr,ITEM_LOSER_COUNT);
            if (IsRandom() || BattleGroundMgr::IsBGWeekend(GetTypeID()))
                UpdatePlayerScore(plr, SCORE_BONUS_HONOR, loose_kills);
        }

        plr->CombatStopWithPets(true);

        BlockMovement(plr);

        sBattleGroundMgr.BuildPvpLogDataPacket(&data, this);
        plr->GetSession()->SendPacket(&data);

        BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());
        sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, plr, plr->GetBattleGroundQueueIndex(bgQueueTypeId), STATUS_IN_PROGRESS, TIME_TO_AUTOREMOVE, GetStartTime(), GetArenaType());
        plr->GetSession()->SendPacket(&data);
        plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_BATTLEGROUND, 1);
    }

    Map::PlayerList const &PlList = GetBgMap()->GetPlayers();
    if (!PlList.isEmpty())
        for (Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
            if (Player* pPlayer = i->getSource())
                if (pPlayer->IsSpectator())
                {
                    WorldPacket data;
                    sBattleGroundMgr.BuildPvpLogDataPacket(&data, this);
                    pPlayer->GetSession()->SendPacket(&data);

                    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());
                    sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, pPlayer, 0, STATUS_IN_PROGRESS, TIME_TO_AUTOREMOVE, GetStartTime(), GetArenaType());
                    pPlayer->GetSession()->SendPacket(&data);
                }

    if (isArena() && isRated() && winner_arena_team && loser_arena_team)
    {
        // save the stat changes
        winner_arena_team->SaveToDB();
        loser_arena_team->SaveToDB();
        // send updated arena team stats to players
        // this way all arena team members will get notified, not only the ones who participated in this match
        winner_arena_team->NotifyStatsChanged();
        loser_arena_team->NotifyStatsChanged();
    }

    if (winmsg_id)
        SendMessageToAll(winmsg_id, CHAT_MSG_BG_SYSTEM_NEUTRAL);
}

uint32 BattleGround::GetBonusHonorFromKill(uint32 kills) const
{
    //variable kills means how many honorable kills you scored (so we need kills * honor_for_one_kill)
    return (uint32)(MaNGOS::Honor::hk_honor_at_level(GetMaxLevel(), kills) * sWorld.getConfig(CONFIG_FLOAT_RATE_HONOR) / m_MaxPlayers);
}

uint32 BattleGround::GetBattlemasterEntry() const
{
    switch(GetTypeID(true))
    {
        case BATTLEGROUND_AV: return 15972;
        case BATTLEGROUND_WS: return 14623;
        case BATTLEGROUND_AB: return 14879;
        case BATTLEGROUND_EY: return 22516;
        case BATTLEGROUND_NA: return 20200;
        default:              return 0;
    }
}

void BattleGround::RewardMark(Player *plr,uint32 count)
{
    switch(GetTypeID(true))
    {
        case BATTLEGROUND_AV:
            if (count == ITEM_WINNER_COUNT)
                RewardSpellCast(plr,SPELL_AV_MARK_WINNER);
            else
                RewardSpellCast(plr,SPELL_AV_MARK_LOSER);
            break;
        case BATTLEGROUND_WS:
            if (count == ITEM_WINNER_COUNT)
                RewardSpellCast(plr,SPELL_WS_MARK_WINNER);
            else
                RewardSpellCast(plr,SPELL_WS_MARK_LOSER);
            break;
        case BATTLEGROUND_AB:
            if (count == ITEM_WINNER_COUNT)
                RewardSpellCast(plr,SPELL_AB_MARK_WINNER);
            else
                RewardSpellCast(plr,SPELL_AB_MARK_LOSER);
            break;
        case BATTLEGROUND_EY:                               // no rewards
        default:
            break;
    }
}

void BattleGround::RewardSpellCast(Player *plr, uint32 spell_id)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spell_id);
    if(!spellInfo)
    {
        ERROR_LOG("Battleground reward casting spell %u not exist.",spell_id);
        return;
    }

    plr->CastSpell(plr, spellInfo, true);
}

void BattleGround::RewardItem(Player *plr, uint32 item_id, uint32 count)
{
    // 'Inactive' this aura prevents the player from gaining honor points and battleground tokens
    if (plr->GetDummyAura(SPELL_AURA_PLAYER_INACTIVE))
        return;

    ItemPosCountVec dest;
    uint32 no_space_count = 0;
    uint8 msg = plr->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, item_id, count, &no_space_count );

    if( msg == EQUIP_ERR_ITEM_NOT_FOUND)
    {
        sLog.outErrorDb("Battleground reward item (Entry %u) not exist in `item_template`.",item_id);
        return;
    }

    if( msg != EQUIP_ERR_OK )                               // convert to possible store amount
        count -= no_space_count;

    if( count != 0 && !dest.empty())                        // can add some
        if (Item* item = plr->StoreNewItem( dest, item_id, true, 0))
            plr->SendNewItem(item,count,true,false);

    if (no_space_count > 0)
        SendRewardMarkByMail(plr,item_id,no_space_count);
}

void BattleGround::SendRewardMarkByMail(Player *plr,uint32 mark, uint32 count)
{
    uint32 bmEntry = GetBattlemasterEntry();
    if (!bmEntry)
        return;

    ItemPrototype const* markProto = ObjectMgr::GetItemPrototype(mark);
    if (!markProto)
        return;

    if (Item* markItem = Item::CreateItem(mark,count,plr))
    {
        // save new item before send
        markItem->SaveToDB();                               // save for prevent lost at next mail load, if send fail then item will deleted

        int loc_idx = plr->GetSession()->GetSessionDbLocaleIndex();

        // subject: item name
        std::string subject = markProto->Name1;
        sObjectMgr.GetItemLocaleStrings(markProto->ItemId, loc_idx, &subject);

        // text
        std::string textFormat = plr->GetSession()->GetMangosString(LANG_BG_MARK_BY_MAIL);
        char textBuf[300];
        snprintf(textBuf, 300, textFormat.c_str(), GetName(), GetName());

        MailDraft(subject, textBuf)
            .AddItem(markItem)
            .SendMailTo(plr, MailSender(MAIL_CREATURE, bmEntry));
    }
}

void BattleGround::RewardQuestComplete(Player *plr)
{
    uint32 quest;
    switch(GetTypeID(true))
    {
        case BATTLEGROUND_AV:
            quest = SPELL_AV_QUEST_REWARD;
            break;
        case BATTLEGROUND_WS:
            quest = SPELL_WS_QUEST_REWARD;
            break;
        case BATTLEGROUND_AB:
            quest = SPELL_AB_QUEST_REWARD;
            break;
        case BATTLEGROUND_EY:
            quest = SPELL_EY_QUEST_REWARD;
            break;
        default:
            return;
    }

    RewardSpellCast(plr, quest);
}

void BattleGround::BlockMovement(Player *plr)
{
    plr->SetClientControl(plr, 0);                          // movement disabled NOTE: the effect will be automatically removed by client when the player is teleported from the battleground, so no need to send with uint8(1) in RemovePlayerAtLeave()
}

void BattleGround::RemovePlayerAtLeave(ObjectGuid guid, bool Transport, bool SendPacket)
{
    Team team = GetPlayerTeam(guid);
    bool participant = false;
    Player* plr = sObjectMgr.GetPlayer(guid);

    BattleGroundScoreMap::iterator itr2 = m_PlayerScores.find(guid);
    if (itr2 != m_PlayerScores.end())
    {
        if (isRated() && isArena() && GetStatus() != STATUS_WAIT_LEAVE)
        {
            ArenaTeam* others_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
            ArenaTeam* players_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(team));
            if (others_arena_team && players_arena_team)
                m_arenaChartHolder.FillArenaChartAtPlayerLeave(guid, players_arena_team, others_arena_team, plr);
        }

        delete itr2->second;                                // delete player's score
        m_PlayerScores.erase(itr2);
    }

    // Remove from lists/maps
    BattleGroundPlayerMap::iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
    {
        UpdatePlayersCountByTeam(team, true);               // -1 player
        m_Players.erase(itr);
        // check if the player was a participant of the match, or only entered through gm command (goname)
        participant = true;
    }

    if (plr)
    {
        // should remove spirit of redemption
        if (plr->HasAuraType(SPELL_AURA_SPIRIT_OF_REDEMPTION))
            plr->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);

        plr->RemoveAurasDueToSpell(isArena() ? SPELL_ARENA_DAMPENING : SPELL_BATTLEGROUND_DAMPENING);

        if (!plr->isAlive())                                // resurrect on exit
        {
            plr->ResurrectPlayer(1.0f);
            plr->SpawnCorpseBones();
        }
    }

    if (plr && !isArena())
    {
        plr->m_lastOkBgAfkTime = 0;
        plr->m_bgAfkDistance = 0.f;
    }

    RemovePlayer(plr, guid);                                // BG subclass specific code

    if(participant) // if the player was a match participant, remove auras, calc rating, update queue
    {
        BattleGroundTypeId bgTypeId = GetTypeID();
        BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());
        if (plr)
        {
            plr->ClearAfkReports();

            if (!team) team = plr->GetTeam();

            // if arena, remove the specific arena auras
            if (isArena())
            {
                plr->RemoveArenaAuras(true);                // removes debuffs / dots etc., we don't want the player to die after porting out
                bgTypeId=BATTLEGROUND_AA;                   // set the bg type to all arenas (it will be used for queue refreshing)

                // unsummon current and summon old pet if there was one and there isn't a current pet
                plr->RemovePet(PET_SAVE_NOT_IN_SLOT);
                plr->ResummonPetTemporaryUnSummonedIfAny();

                if (isRated() && GetStatus() == STATUS_IN_PROGRESS)
                {
                    //left a rated match while the encounter was in progress, consider as loser
                    ArenaTeam * winner_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
                    ArenaTeam * loser_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(team));
                    if (winner_arena_team && loser_arena_team)
                        loser_arena_team->MemberLost(plr,winner_arena_team->GetRating());
                }
            }
            if (SendPacket)
            {
                WorldPacket data;
                sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, plr, plr->GetBattleGroundQueueIndex(bgQueueTypeId), STATUS_NONE, 0, 0, ARENA_TYPE_NONE);
                plr->GetSession()->SendPacket(&data);
            }

            // this call is important, because player, when joins to battleground, this method is not called, so it must be called when leaving bg
            plr->RemoveBattleGroundQueueId(bgQueueTypeId);
        }
        else
        // removing offline participant
        {
            if (isRated() && GetStatus() == STATUS_IN_PROGRESS)
            {
                //left a rated match while the encounter was in progress, consider as loser
                ArenaTeam * others_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(GetOtherTeam(team)));
                ArenaTeam * players_arena_team = sObjectMgr.GetArenaTeamById(GetArenaTeamIdForTeam(team));
                if (others_arena_team && players_arena_team)
                    players_arena_team->OfflineMemberLost(guid, GetArenaMatchmakerRating(GetOtherTeam(team)));
            }
        }

        // remove from raid group if player is member
        if (Group *group = GetBgRaid(team))
        {
            if( !group->RemoveMember(guid, GROUP_REMOVEMETHOD_DEFAULT) )             // group was disbanded
            {
                SetBgRaid(team, NULL);
                delete group;
            }
        }
        DecreaseInvitedCount(team);
        //we should update battleground queue, but only if bg isn't ending
        if (isBattleGround() && GetStatus() < STATUS_WAIT_LEAVE)
        {
            // a player has left the battleground, so there are free slots -> add to queue
            AddToBGFreeSlotQueue();
            sBattleGroundMgr.ScheduleQueueUpdate(0, ARENA_TYPE_NONE, bgQueueTypeId, bgTypeId, GetBracketId());
        }

        // Let others know
        WorldPacket data;
        sBattleGroundMgr.BuildPlayerLeftBattleGroundPacket(&data, guid);
        SendPacketToTeam(team, &data, plr, false);
    }

    if (plr)
    {
        // Do next only if found in battleground
        plr->SetBattleGroundId(0, BATTLEGROUND_TYPE_NONE);  // We're not in BG.
        // reset destination bg team
        plr->SetBGTeam(TEAM_NONE);
        plr->SetSpectator(false);

        if (Transport)
            plr->TeleportToBGEntryPoint();

        DETAIL_LOG("BATTLEGROUND: Removed player %s from BattleGround.", plr->GetName());
    }

    //battleground object will be deleted next BattleGround::Update() call
}

// this method is called when no players remains in battleground
void BattleGround::Reset()
{
    SetWinner(TEAM_NONE);
    SetStatus(STATUS_WAIT_QUEUE);
    SetStartTime(0);
    SetEndTime(0);
    SetArenaType(ARENA_TYPE_NONE);
    SetRated(false);

    m_Events = 0;

    // door-event2 is always 0
    m_ActiveEvents[BG_EVENT_DOOR] = 0;
    if (isArena())
    {
        m_ActiveEvents[ARENA_BUFF_EVENT] = BG_EVENT_NONE;
        m_ArenaBuffSpawned = false;
    }

    if (m_InvitedAlliance > 0 || m_InvitedHorde > 0)
        ERROR_LOG("BattleGround system: bad counter, m_InvitedAlliance: %d, m_InvitedHorde: %d", m_InvitedAlliance, m_InvitedHorde);

    m_InvitedAlliance = 0;
    m_InvitedHorde = 0;
    m_InBGFreeSlotQueue = false;

    m_Players.clear();

    for(BattleGroundScoreMap::const_iterator itr = m_PlayerScores.begin(); itr != m_PlayerScores.end(); ++itr)
        delete itr->second;
    m_PlayerScores.clear();

    m_endSoundPlayed = false;
}

void BattleGround::StartBattleGround()
{
    SetStartTime(0);

    // add BG to free slot queue
    AddToBGFreeSlotQueue();

    // add bg to update list
    // This must be done here, because we need to have already invited some players when first BG::Update() method is executed
    // and it doesn't matter if we call StartBattleGround() more times, because m_BattleGrounds is a map and instance id never changes
    sBattleGroundMgr.AddBattleGround(GetInstanceID(), GetTypeID(), this);
}

void BattleGround::StartTimedAchievement(AchievementCriteriaTypes type, uint32 entry, Team team)
{
    for (BattleGroundPlayerMap::const_iterator itr = GetPlayers().begin(); itr != GetPlayers().end(); ++itr)
        if (Player* pPlayer = GetBgMap()->GetPlayer(itr->first))
            if (team == TEAM_NONE || pPlayer->GetTeam() == team)
                pPlayer->GetAchievementMgr().StartTimedAchievementCriteria(type, entry);
}

void BattleGround::AddPlayer(Player *plr)
{
    // remove afk from player
    if (plr->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK))
        plr->ToggleAFK();

    // score struct must be created in inherited class

    ObjectGuid guid = plr->GetObjectGuid();
    Team team = plr->GetBGTeam();

    BattleGroundPlayer bp;
    bp.OfflineRemoveTime = 0;
    bp.PlayerTeam = team;

    // Add to list/maps
    m_Players[guid] = bp;

    UpdatePlayersCountByTeam(team, false);                  // +1 player

    WorldPacket data;
    sBattleGroundMgr.BuildPlayerJoinedBattleGroundPacket(&data, plr);
    SendPacketToTeam(team, &data, plr, false);

    // BG Status packet
    WorldPacket status;
    BattleGroundQueueTypeId bgQueueTypeId = sBattleGroundMgr.BGQueueTypeId(m_TypeID, GetArenaType());
    uint32 queueSlot = plr->GetBattleGroundQueueIndex(bgQueueTypeId);
    //sBattleGroundMgr.BuildBattleGroundStatusPacket(&status, this, queueSlot, STATUS_IN_PROGRESS, 0, GetStartTime(), GetArenaType(), isArena() ? 0 : 1);
    // FIXME: does arenaframe still exists?
    sBattleGroundMgr.BuildBattleGroundStatusPacket(&status, this, plr, queueSlot, STATUS_IN_PROGRESS, 0, GetStartTime(), GetArenaType());
    plr->GetSession()->SendPacket(&status);

    plr->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

    // add arena specific auras
    if (isArena())
    {
        plr->RemoveArenaSpellCooldowns();

        plr->RemoveArenaAuras();
        plr->RemoveAllEnchantments(TEMP_ENCHANTMENT_SLOT, true);
        if (team == ALLIANCE)                               // gold
        {
            if (plr->GetTeam() == HORDE)
                plr->CastSpell(plr, SPELL_HORDE_GOLD_FLAG,true);
            else
                plr->CastSpell(plr, SPELL_ALLIANCE_GOLD_FLAG,true);
        }
        else                                                // green
        {
            if (plr->GetTeam() == HORDE)
                plr->CastSpell(plr, SPELL_HORDE_GREEN_FLAG,true);
            else
                plr->CastSpell(plr, SPELL_ALLIANCE_GREEN_FLAG,true);
        }

        plr->DestroyConjuredItems(true);
        plr->UnsummonPetTemporaryIfAny();

        if(GetStatus() == STATUS_WAIT_JOIN)                 // not started yet
            plr->CastSpell(plr, SPELL_ARENA_PREPARATION, true);

        WorldPacket teammate;
        teammate.Initialize(SMSG_DESTROY_ARENA_UNIT, 8);
        teammate << plr->GetObjectGuid();
        SendPacketToTeam(team == ALLIANCE ? HORDE : ALLIANCE, &teammate, plr, false);

        plr->CastSpell(plr, SPELL_ARENA_DAMPENING, true);

        if (plr->IsSpectator())
            plr->Unmount(true);
    }
    else
    {
        if(GetStatus() == STATUS_WAIT_JOIN)                 // not started yet
            plr->CastSpell(plr, SPELL_PREPARATION, true);   // reduces all mana cost of spells.

        plr->CastSpell(plr, SPELL_BATTLEGROUND_DAMPENING, true);

        plr->m_lastOkBgAfkTime = time(NULL);
        plr->m_bgAfkDistance = 0;
        plr->GetPosition(plr->m_bgLastAfkPosition.x, plr->m_bgLastAfkPosition.y, plr->m_bgLastAfkPosition.z);
    }

    if (GetStatus() == STATUS_WAIT_JOIN)                 // not started yet
    {
        int32 countdownMaxForBGType = isArena() ? ARENA_COUNTDOWN_MAX : BATTLEGROUND_COUNTDOWN_MAX;
        WorldPacket data(SMSG_START_TIMER, 4+4+4);
        data << uint32(0); // unk
        data << uint32(countdownMaxForBGType - (m_StartTime / 1000));
        data << uint32(countdownMaxForBGType);
        plr->GetSession()->SendPacket(&data);
    }

    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HEALING_DONE, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_DAMAGE_DONE, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_WIN_BG, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());    
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BG_OBJECTIVE_CAPTURE, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());    
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_GET_KILLING_BLOWS, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_SPECIAL_PVP_KILL, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());
    plr->GetAchievementMgr().ResetAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL, ACHIEVEMENT_CRITERIA_CONDITION_MAP, GetMapId());

    // setup BG group membership
    PlayerAddedToBGCheckIfBGIsRunning(plr);
    AddOrSetPlayerToCorrectBgGroup(plr, guid, team);

    // Log
    DETAIL_LOG("BATTLEGROUND: Player %s joined the battle.", plr->GetName());
}

/* this method adds player to his team's bg group, or sets his correct group if player is already in bg group */
void BattleGround::AddOrSetPlayerToCorrectBgGroup(Player *plr, ObjectGuid plr_guid, Team team)
{
    if (Group* group = GetBgRaid(team))                     // raid already exist
    {
        if (group->IsMember(plr_guid))
        {
            uint8 subgroup = group->GetMemberGroup(plr_guid);
            plr->SetBattleRaid(group, subgroup);
        }
        else
        {
            group->AddMember(plr_guid, plr->GetName());
            if (Group* originalGroup = plr->GetOriginalGroup())
                if (originalGroup->IsLeader(plr_guid))
                    group->ChangeLeader(plr_guid);
        }
    }
    else                                                    // first player joined
    {
        group = new Group;
        SetBgRaid(team, group);
        group->Create(plr_guid, plr->GetName());
    }
}

// This method should be called when player logs into running battleground
void BattleGround::EventPlayerLoggedIn(Player* player, ObjectGuid plr_guid)
{
    // player is correct pointer
    for(OfflineQueue::iterator itr = m_OfflineQueue.begin(); itr != m_OfflineQueue.end(); ++itr)
    {
        if (*itr == plr_guid)
        {
            m_OfflineQueue.erase(itr);
            break;
        }
    }
    m_Players[plr_guid].OfflineRemoveTime = 0;
    PlayerAddedToBGCheckIfBGIsRunning(player);
    // if battleground is starting, then add preparation aura
    // we don't have to do that, because preparation aura isn't removed when player logs out
}

// This method should be called when player logs out from running battleground
void BattleGround::EventPlayerLoggedOut(Player* player)
{
    // player is correct pointer, it is checked in WorldSession::LogoutPlayer()
    m_OfflineQueue.push_back(player->GetObjectGuid());
    m_Players[player->GetObjectGuid()].OfflineRemoveTime = sWorld.GetGameTime() + MAX_OFFLINE_TIME;
    if (GetStatus() == STATUS_IN_PROGRESS)
    {
        // drop flag and handle other cleanups
        RemovePlayer(player, player->GetObjectGuid());

        // 1 player is logging out, if it is the last, then end arena!
        if (isArena())
            if (GetAlivePlayersCountByTeam(player->GetTeam()) <= 1 && GetPlayersCountByTeam(GetOtherTeam(player->GetTeam())))
                EndBattleGround(GetOtherTeam(player->GetTeam()));
    }
}

/* This method should be called only once ... it adds pointer to queue */
void BattleGround::AddToBGFreeSlotQueue()
{
    // make sure to add only once
    if (!m_InBGFreeSlotQueue && isBattleGround())
    {
        sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].push_front(this);
        m_InBGFreeSlotQueue = true;
    }
}

/* This method removes this battleground from free queue - it must be called when deleting battleground - not used now*/
void BattleGround::RemoveFromBGFreeSlotQueue()
{
    // set to be able to re-add if needed
    m_InBGFreeSlotQueue = false;
    for (BGFreeSlotQueueType::iterator itr = sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].begin(); itr != sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].end(); ++itr)
    {
        if ((*itr)->GetInstanceID() == GetInstanceID())
        {
            sBattleGroundMgr.BGFreeSlotQueue[m_TypeID].erase(itr);
            return;
        }
    }
}

// get the number of free slots for team
// returns the number how many players can join battleground to MaxPlayersPerTeam
uint32 BattleGround::GetFreeSlotsForTeam(Team team) const
{
    //return free slot count to MaxPlayerPerTeam
    if (GetStatus() == STATUS_WAIT_JOIN || GetStatus() == STATUS_IN_PROGRESS)
        return (GetInvitedCount(team) < GetMaxPlayersPerTeam()) ? GetMaxPlayersPerTeam() - GetInvitedCount(team) : 0;

    return 0;
}

bool BattleGround::HasFreeSlots() const
{
    return GetPlayersSize() < GetMaxPlayers();
}

void BattleGround::UpdatePlayerScore(Player *Source, uint32 type, uint32 value)
{
    //this procedure is called from virtual function implemented in bg subclass
    BattleGroundScoreMap::const_iterator itr = m_PlayerScores.find(Source->GetObjectGuid());

    if(itr == m_PlayerScores.end())                         // player not found...
        return;

    switch(type)
    {
        case SCORE_KILLING_BLOWS:                           // Killing blows
            itr->second->KillingBlows += value;
            break;
        case SCORE_DEATHS:                                  // Deaths
            itr->second->Deaths += value;
            break;
        case SCORE_HONORABLE_KILLS:                         // Honorable kills
            itr->second->HonorableKills += value;
            break;
        case SCORE_BONUS_HONOR:                             // Honor bonus
            // do not add honor in arenas
            if (isBattleGround())
            {
                // reward honor instantly
                if (Source->RewardHonor(NULL, 1, (float)value * GetCurrencyPrecision(CURRENCY_HONOR_POINTS)))
                    itr->second->BonusHonor += value;
            }
            break;
            // used only in EY, but in SMSG_PVP_LOG_DATA opcode
        case SCORE_DAMAGE_DONE:                             // Damage Done
            itr->second->DamageDone += value;
            break;
        case SCORE_HEALING_DONE:                            // Healing Done
            itr->second->HealingDone += value;
            break;
        /* WoWArmory (arena game chart) */
        case SCORE_DAMAGE_TAKEN:
            itr->second->DamageTaken += value;              // Damage Taken
            break;
        case SCORE_HEALING_TAKEN:
            itr->second->HealingTaken += value;             // Healing Taken
            break;
        default:
            ERROR_LOG("BattleGround: Unknown player score type %u", type);
            break;
    }
}

bool BattleGround::AddObject(uint32 type, uint32 entry, float x, float y, float z, float o, QuaternionData rotation, uint32 /*respawnTime*/, bool manualAnim)
{
    // must be created this way, adding to godatamap would add it to the base map of the instance
    // and when loading it (in go::LoadFromDB()), a new guid would be assigned to the object, and a new object would be created
    // so we must create it specific for this instance
    GameObject * go = new GameObject;
    if (!go->Create(GetBgMap()->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT),entry, GetBgMap(),
        PHASEMASK_NORMAL, x,y,z,o, rotation))
    {
        sLog.outErrorDb("Gameobject template %u not found in database! BattleGround not created!", entry);
        ERROR_LOG("Cannot create gameobject template %u! BattleGround not created!", entry);
        delete go;
        return false;
    }
/*
    uint32 guid = go->GetGUIDLow();

    // without this, UseButtonOrDoor caused the crash, since it tried to get go info from godata
    // iirc that was changed, so adding to go data map is no longer required if that was the only function using godata from GameObject without checking if it existed
    GameObjectData& data = sObjectMgr.NewGOData(guid);

    data.id             = entry;
    data.mapid          = GetMapId();
    data.posX           = x;
    data.posY           = y;
    data.posZ           = z;
    data.orientation    = o;
    data.rotation0      = rotation0;
    data.rotation1      = rotation1;
    data.rotation2      = rotation2;
    data.rotation3      = rotation3;
    data.spawntimesecs  = respawnTime;
    data.spawnMask      = 1;
    data.animprogress   = 100;
    data.go_state       = 1;
*/

    go->SetManualAnim(manualAnim);

    // add to world, so it can be later looked up from HashMapHolder
    go->AddToWorld();
    m_BgObjects[type] = go->GetObjectGuid();
    return true;
}

//some doors aren't despawned so we cannot handle their closing in gameobject::update()
//it would be nice to correctly implement GO_ACTIVATED state and open/close doors in gameobject code
void BattleGround::DoorClose(ObjectGuid guid)
{
    GameObject *obj = GetBgMap()->GetGameObject(guid);
    if (obj)
    {
        //if doors are open, close it
        if (obj->getLootState() == GO_ACTIVATED && obj->GetGoState() != GO_STATE_READY)
        {
            //change state to allow door to be closed
            obj->SetLootState(GO_READY);
            obj->UseDoorOrButton(RESPAWN_ONE_DAY);
        }
    }
    else
        ERROR_LOG("BattleGround: Door %s not found (cannot close doors)", guid.GetString().c_str());
}

void BattleGround::DoorClose(uint32 type)
{
    DoorClose(m_BgObjects[type]);
}

void BattleGround::DoorOpen(ObjectGuid guid)
{
    GameObject *obj = GetBgMap()->GetGameObject(guid);
    if (obj)
    {
        //change state to be sure they will be opened
        obj->SetLootState(GO_READY);
        obj->UseDoorOrButton(RESPAWN_ONE_DAY);
    }
    else
        ERROR_LOG("BattleGround: Door %s not found! - doors will be closed.", guid.GetString().c_str());
}

void BattleGround::DoorOpen(uint32 type)
{
    DoorOpen(m_BgObjects[type]);
}

void BattleGround::OnObjectDBLoad(Creature* creature)
{
    const BattleGroundEventIdx eventId = sBattleGroundMgr.GetCreatureEventIndex(creature->GetGUIDLow());
    if (eventId.event1 == BG_EVENT_NONE)
        return;
    m_EventObjects[MAKE_PAIR32(eventId.event1, eventId.event2)].creatures.push_back(creature->GetObjectGuid());
    if (!IsActiveEvent(eventId.event1, eventId.event2))
    {
        SpawnBGCreature(creature->GetObjectGuid(), RESPAWN_ONE_DAY);
        creature->SetHealth(creature->GetMaxHealth());
    }
}

ObjectGuid BattleGround::GetSingleCreatureGuid(uint8 event1, uint8 event2)
{
    GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.begin();
    if (itr != m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.end())
        return *itr;
    return ObjectGuid();
}

ObjectGuid BattleGround::GetSingleGameObjectGuid(uint8 event1, uint8 event2)
{
    GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr)
        return *itr;
    return ObjectGuid();
}

void BattleGround::OnObjectDBLoad(GameObject* obj)
{
    if (obj->IsTransport())
        obj->SetActiveObjectState(true);

    const BattleGroundEventIdx eventId = sBattleGroundMgr.GetGameObjectEventIndex(obj->GetGUIDLow());
    if (eventId.event1 == BG_EVENT_NONE)
        return;
    m_EventObjects[MAKE_PAIR32(eventId.event1, eventId.event2)].gameobjects.push_back(obj->GetObjectGuid());
    if (!IsActiveEvent(eventId.event1, eventId.event2))
    {
        SpawnBGObject(obj->GetObjectGuid(), RESPAWN_ONE_DAY);
    }
    else
    {
        // it's possible, that doors aren't spawned anymore (wsg)
        if (GetStatus() >= STATUS_IN_PROGRESS && IsDoor(eventId.event1, eventId.event2))
            DoorOpen(obj->GetObjectGuid());
    }
}

bool BattleGround::IsDoor(uint8 event1, uint8 event2)
{
    if (event1 == BG_EVENT_DOOR)
    {
        if (event2 > 0)
        {
            ERROR_LOG("BattleGround too high event2 for event1:%i", event1);
            return false;
        }
        return true;
    }
    return false;
}

void BattleGround::OpenDoorEvent(uint8 event1, uint8 event2 /*=0*/)
{
    if (!IsActiveEvent(event1, event2))                 // maybe already despawned (eye)
    {
        ERROR_LOG("BattleGround:OpenDoorEvent this event isn't active event1:%u event2:%u", event1, event2);
        return;
    }
    GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr)
        DoorOpen(*itr);
}

void BattleGround::CloseDoorEvent(uint8 event1, uint8 event2 /*=0*/)
{
    if (!IsActiveEvent(event1, event2))                 // maybe already despawned (eye)
    {
        ERROR_LOG("BattleGround:CloseDoorEvent this event isn't active event1:%u event2:%u", event1, event2);
        return;
    }
    GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr)
        DoorClose(*itr);
}

void BattleGround::SpawnEvent(uint8 event1, uint8 event2, bool spawn, uint32 faction)
{
    // stop if we want to spawn something which was already spawned
    // or despawn something which was already despawned
    if (event2 == BG_EVENT_NONE || (spawn && m_ActiveEvents[event1] == event2)
        || (!spawn && m_ActiveEvents[event1] != event2))
        return;

    if (spawn)
    {
        // if event gets spawned, the current active event mus get despawned
        SpawnEvent(event1, m_ActiveEvents[event1], false, faction);
        m_ActiveEvents[event1] = event2;                    // set this event to active
    }
    else
        m_ActiveEvents[event1] = BG_EVENT_NONE;             // no event active if event2 gets despawned

    GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.end(); ++itr)
        SpawnBGCreature(*itr, (spawn ? RESPAWN_IMMEDIATELY : RESPAWN_ONE_DAY), faction);
    GuidVector::const_iterator itr2 = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr2 != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr2)
        SpawnBGObject(*itr2, (spawn ? RESPAWN_IMMEDIATELY : RESPAWN_ONE_DAY), faction);
}

void BattleGround::SpawnEventWithRespawn(uint8 event1, uint8 event2, uint32 respawnTime, uint32 faction)
{
    // stop if we want to spawn something which was already spawned
    if (event2 == BG_EVENT_NONE || m_ActiveEvents[event1] == event2)
        return;

    // the current active event mus get despawned
    SpawnEvent(event1, m_ActiveEvents[event1], false, faction);
    m_ActiveEvents[event1] = event2;                    // set this event to active

    Map* map = GetBgMap();
    GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.end(); ++itr)
    {
        Creature* obj = map->GetCreature(*itr);
        if (!obj)
        {
            DEBUG_LOG("Can't spawn event with respawn %s - not found in map!", itr->GetString().c_str());
            return;
        }

        obj->Respawn();
        map->Add(obj);
        obj->SetRespawnDelay(respawnTime);
        if (faction)
            obj->setFaction(faction);
        obj->SetHealth(obj->GetMaxHealth());
    }
    GuidVector::const_iterator itr2 = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr2 != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr2)
    {
        GameObject *obj = map->GetGameObject(*itr2);
        if (!obj)
        {
            DEBUG_LOG("Can't spawn event with respawn %s - not found in map!", itr2->GetString().c_str());
            return;
        }

        //we need to change state from GO_JUST_DEACTIVATED to GO_READY in case battleground is starting again
        if (obj->getLootState() == GO_JUST_DEACTIVATED)
            obj->SetLootState(GO_READY);
        obj->SetRespawnTime(0);
        obj->SetRespawnDelay(respawnTime);
        map->Add(obj);
        if (obj->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
            obj->Rebuild(NULL);
        if (faction)
            obj->SetUInt32Value(GAMEOBJECT_FACTION, faction);
    }
}

void BattleGround::SetEventRespawn(uint8 event1, uint8 event2, uint32 respawnTime)
{
    Map* map = GetBgMap();
    GuidVector::const_iterator itr = m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.begin();
    for(; itr != m_EventObjects[MAKE_PAIR32(event1, event2)].creatures.end(); ++itr)
    {
        Creature* obj = map->GetCreature(*itr);
        if (!obj)
        {
            DEBUG_LOG("Can't set event respawn %s - not found in map!", itr->GetString().c_str());
            return;
        }

        obj->SetRespawnDelay(respawnTime);
    }
    GuidVector::const_iterator itr2 = m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.begin();
    for(; itr2 != m_EventObjects[MAKE_PAIR32(event1, event2)].gameobjects.end(); ++itr2)
    {
        GameObject *obj = map->GetGameObject(*itr2);
        if (!obj)
        {
            DEBUG_LOG("Can't set event respawn %s - not found in map!", itr2->GetString().c_str());
            return;
        }

        obj->SetRespawnTime(0);
        obj->SetRespawnDelay(respawnTime);
    }
}

void BattleGround::SpawnBGObject(uint32 type, uint32 respawntime, uint32 faction)
{
    SpawnBGObject(m_BgObjects[type], respawntime, faction);
}

void BattleGround::SpawnBGObject(ObjectGuid guid, uint32 respawntime, uint32 faction)
{
    Map* map = GetBgMap();

    GameObject *obj = map->GetGameObject(guid);
    if (!obj)
    {
        DEBUG_LOG("Can't spawn BG object %s - not found in map!", guid.GetString().c_str());
        return;
    }

    if (respawntime == 0)
    {
        //we need to change state from GO_JUST_DEACTIVATED to GO_READY in case battleground is starting again
        if (obj->getLootState() == GO_JUST_DEACTIVATED)
            obj->SetLootState(GO_READY);
        obj->SetRespawnTime(0);
        map->Add(obj);
        if (obj->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
            obj->Rebuild(NULL);
    }
    else
    {
        map->Add(obj);
        obj->SetRespawnTime(respawntime);
        obj->SetLootState(GO_JUST_DEACTIVATED);
        if (obj->GetGOInfo()->type == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
            obj->Rebuild(NULL);
    }

    if (faction)
        obj->SetUInt32Value(GAMEOBJECT_FACTION, faction);
}

void BattleGround::SpawnBGCreature(ObjectGuid guid, uint32 respawntime, uint32 faction)
{
    Map* map = GetBgMap();

    Creature* obj = map->GetCreature(guid);
    if (!obj)
    {
        DEBUG_LOG("Can't spawn BG creature %s - not found in map!", guid.GetString().c_str());
        return;
    }

    if (respawntime == 0)
    {
        obj->Respawn();
        map->Add(obj);
        obj->SetHealth(obj->GetMaxHealth());
    }
    else
    {
        map->Add(obj);
        obj->SetRespawnDelay(respawntime);
        obj->SetDeathState(JUST_DIED);
        obj->RemoveCorpse();
    }

    if (faction)
        obj->setFaction(faction);
}

bool BattleGround::DelObject(uint32 type)
{
    if (!m_BgObjects[type])
        return true;

    GameObject *obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if (!obj)
    {
        ERROR_LOG("Can't find gobject: %s", m_BgObjects[type].GetString().c_str());
        return false;
    }

    obj->SetRespawnTime(0);                                 // not save respawn time
    obj->Delete();
    m_BgObjects[type].Clear();
    return true;
}

void BattleGround::SendMessageToAll(int32 entry, ChatMsg type, Player const* source)
{
    MaNGOS::BattleGroundChatBuilder bg_builder(type, entry, source);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::SendWarningToAll(int32 entry)
{
    MaNGOS::BattleGroundWarningBuilder bg_builder(entry);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGroundWarningBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::SendYellToAll(int32 entry, uint32 language, ObjectGuid guid)
{
    Creature* source = GetBgMap()->GetCreature(guid);
    if(!source)
        return;
    MaNGOS::BattleGroundYellBuilder bg_builder(language, entry, source);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGroundYellBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::PSendMessageToAll(int32 entry, ChatMsg type, Player const* source, ...)
{
    va_list ap;
    va_start(ap, source);

    MaNGOS::BattleGroundChatBuilder bg_builder(type, entry, source, &ap);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGroundChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);

    va_end(ap);
}

GameObject* BattleGround::GetBGObject(uint32 type)
{
    GameObject *obj = GetBgMap()->GetGameObject(m_BgObjects[type]);
    if (!obj)
        ERROR_LOG("BattleGround::GetBGObject() :: couldn't get gameobject %i",type);
    return obj;
}

void BattleGround::SendMessage2ToAll(int32 entry, ChatMsg type, Player const* source, int32 arg1, int32 arg2)
{
    MaNGOS::BattleGround2ChatBuilder bg_builder(type, entry, source, arg1, arg2);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGround2ChatBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::SendYell2ToAll(int32 entry, uint32 language, ObjectGuid guid, int32 arg1, int32 arg2)
{
    Creature* source = GetBgMap()->GetCreature(guid);
    if(!source)
        return;
    MaNGOS::BattleGround2YellBuilder bg_builder(language, entry, source, arg1, arg2);
    MaNGOS::LocalizedPacketDo<MaNGOS::BattleGround2YellBuilder> bg_do(bg_builder);
    BroadcastWorker(bg_do);
}

void BattleGround::EndNow()
{
    RemoveFromBGFreeSlotQueue();
    SetStatus(STATUS_WAIT_LEAVE);
    SetEndTime(0);
}

/*
important notice:
buffs aren't spawned/despawned when players captures anything
buffs are in their positions when battleground starts
*/
void BattleGround::HandleTriggerBuff(ObjectGuid go_guid)
{
    GameObject *obj = GetBgMap()->GetGameObject(go_guid);
    if (!obj || obj->GetGoType() != GAMEOBJECT_TYPE_TRAP || !obj->isSpawned())
        return;

    // static buffs are already handled just by database and don't need
    // battleground code
    if (!m_BuffChange)
    {
        obj->SetLootState(GO_JUST_DEACTIVATED);             // can be despawned or destroyed
        return;
    }

    // change buff type, when buff is used:
    // TODO this can be done when poolsystem works for instances
    int32 index = m_BgObjects.size() - 1;
    while (index >= 0 && m_BgObjects[index] != go_guid)
        --index;
    if (index < 0)
    {
        ERROR_LOG("BattleGround (Type: %u) has buff trigger %s GOType: %u but it hasn't that object in its internal data",
            GetTypeID(), go_guid.GetString().c_str(), obj->GetGoType());
        return;
    }

    //randomly select new buff
    uint8 buff = urand(0, 2);
    uint32 entry = obj->GetEntry();
    if (m_BuffChange && entry != Buff_Entries[buff])
    {
        //despawn current buff
        SpawnBGObject(m_BgObjects[index], RESPAWN_ONE_DAY);
        //set index for new one
        for (uint8 currBuffTypeIndex = 0; currBuffTypeIndex < 3; ++currBuffTypeIndex)
        {
            if (entry == Buff_Entries[currBuffTypeIndex])
            {
                index -= currBuffTypeIndex;
                index += buff;
            }
        }
    }

    SpawnBGObject(m_BgObjects[index], BUFF_RESPAWN_TIME);
}

void BattleGround::HandleKillPlayer(Player* player, Player* killer)
{
    // add +1 deaths
    UpdatePlayerScore(player, SCORE_DEATHS, 1);

    if (killer->GetAreaId() == player->GetAreaId())
        killer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, 1);

    // add +1 kills to group and +1 killing_blows to killer
    if (killer && killer != player)
    {
        UpdatePlayerScore(killer, SCORE_HONORABLE_KILLS, 1);
        UpdatePlayerScore(killer, SCORE_KILLING_BLOWS, 1);

        for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
        {
            Player* plr = sObjectMgr.GetPlayer(itr->first);

            if (!plr || plr == killer)
                continue;

            if (player->GetAreaId() == plr->GetAreaId())
                plr->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_HONORABLE_KILL_AT_AREA, 1);

            if (plr->GetTeam() == killer->GetTeam() && plr->IsAtGroupRewardDistance(player))
                UpdatePlayerScore(plr, SCORE_HONORABLE_KILLS, 1);
        }
    }

    // to be able to remove insignia -- ONLY IN BattleGrounds
    if (!isArena())
        player->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
}

// return the player's team based on battlegroundplayer info
// used in same faction arena matches mainly
Team BattleGround::GetPlayerTeam(ObjectGuid guid)
{
    BattleGroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return itr->second.PlayerTeam;
    return TEAM_NONE;
}

bool BattleGround::IsPlayerInBattleGround(ObjectGuid guid)
{
    BattleGroundPlayerMap::const_iterator itr = m_Players.find(guid);
    if (itr != m_Players.end())
        return true;
    return false;
}

void BattleGround::PlayerAddedToBGCheckIfBGIsRunning(Player* plr)
{
    if (GetStatus() != STATUS_WAIT_LEAVE)
        return;

    WorldPacket data;
    BattleGroundQueueTypeId bgQueueTypeId = BattleGroundMgr::BGQueueTypeId(GetTypeID(), GetArenaType());

    BlockMovement(plr);

    sBattleGroundMgr.BuildPvpLogDataPacket(&data, this);
    plr->GetSession()->SendPacket(&data);

    sBattleGroundMgr.BuildBattleGroundStatusPacket(&data, this, plr, plr->GetBattleGroundQueueIndex(bgQueueTypeId), STATUS_IN_PROGRESS, GetEndTime(), GetStartTime(), GetArenaType());
    plr->GetSession()->SendPacket(&data);
}

uint32 BattleGround::GetAlivePlayersCountByTeam(Team team) const
{
    int count = 0;
    for(BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.PlayerTeam == team)
        {
            Player * pl = sObjectMgr.GetPlayer(itr->first);
            if (pl && pl->isAlive())
                ++count;
        }
    }
    return count;
}

void BattleGround::CheckArenaWinConditions()
{
    if (!GetAlivePlayersCountByTeam(ALLIANCE) && GetPlayersCountByTeam(HORDE))
        EndBattleGround(HORDE);
    else if (GetPlayersCountByTeam(ALLIANCE) && !GetAlivePlayersCountByTeam(HORDE))
        EndBattleGround(ALLIANCE);
}

void BattleGround::UpdateArenaWorldState()
{
    UpdateWorldState(0xe10, GetAlivePlayersCountByTeam(HORDE));
    UpdateWorldState(0xe11, GetAlivePlayersCountByTeam(ALLIANCE));
}

void BattleGround::SetBgRaid(Team team, Group *bg_raid)
{
    Group* &old_raid = m_BgRaids[GetTeamIndex(team)];

    if (old_raid)
        old_raid->SetBattlegroundGroup(NULL);

    if (bg_raid)
        bg_raid->SetBattlegroundGroup(this);

    old_raid = bg_raid;
}

WorldSafeLocsEntry const* BattleGround::GetClosestGraveYard( Player* player )
{
    return sObjectMgr.GetClosestGraveYard(player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetMapId(), player->GetTeam());
}

bool BattleGround::IsTeamScoreInRange(Team team, uint32 minScore, uint32 maxScore) const
{
    TeamIndex team_idx = GetTeamIndex(team);
    uint32 score = (m_TeamScores[team_idx] < 0) ? 0 : uint32(m_TeamScores[team_idx]);
    return score >= minScore && score <= maxScore;
}

void BattleGround::SetBracket( PvPDifficultyEntry const* bracketEntry )
{
    m_BracketId  = bracketEntry->GetBracketId();
    SetLevelRange(bracketEntry->minLevel,bracketEntry->maxLevel);
}

void BattleGround::UpdateAchievementCriteriaOnTeam(Team team, AchievementCriteriaTypes type, uint32 miscvalue1, uint32 miscvalue2, Unit* unit, uint32 time)
{
    for (BattleGroundPlayerMap::const_iterator itr = m_Players.begin(); itr != m_Players.end(); ++itr)
    {
        if (itr->second.OfflineRemoveTime)
            continue;

        if (Player* plr = sObjectMgr.GetPlayer(itr->first))
            if (!team || plr->GetTeam() == team)
                plr->UpdateAchievementCriteria(type, miscvalue1, miscvalue2, unit, time);
    }
}

void ArenaChartHolder::FillArenaChartAtStart(GroupQueueInfo* ginfo)
{
    // loop through the players
    for (GroupQueueInfoPlayers::iterator itr = ginfo->Players.begin(); itr != ginfo->Players.end(); ++itr)
    {
        std::map<ObjectGuid, ArenaChartElement>::iterator itr2 = chart.find(itr->first);
        if (itr2 != chart.end())
            continue;

        Player* plr = sObjectMgr.GetPlayer(itr->first);

        chart[itr->first].teamId = ginfo->ArenaTeamId;
        chart[itr->first].changeType = CHANGE_TYPE_LOSE;
        chart[itr->first].ratingChange = 0;
        chart[itr->first].teamRating = ginfo->ArenaTeamRating;
        chart[itr->first].damageDone = 0;
        chart[itr->first].damageTaken = 0;
        chart[itr->first].healingDone = 0;
        chart[itr->first].healingTaken = 0;
        chart[itr->first].deaths = 0;
        chart[itr->first].kills = 0;
        if (ArenaTeam* team = sObjectMgr.GetArenaTeamById(ginfo->ArenaTeamId))
        {
            if (ArenaTeamMember* member = team->GetMember(itr->first))
                chart[itr->first].personalRating = member->personal_rating;
            else
                chart[itr->first].personalRating = 0;
        }
        else
            chart[itr->first].personalRating = 0;
        chart[itr->first].ip = plr && plr->GetSession() ? plr->GetSession()->GetRemoteAddress() : "";
    }
}

void ArenaChartHolder::FillArenaChartAtPlayerLeave(ObjectGuid guid, ArenaTeam* playerTeam, ArenaTeam* otherTeam, Player* player)
{
    if (!playerTeam || !otherTeam)
        return;

    if (chart.find(guid) == chart.end())
        return;

    for (BattleGround::BattleGroundScoreMap::const_iterator itr = bg->GetPlayerScoresBegin(); itr != bg->GetPlayerScoresEnd(); ++itr)
    {
        if (itr->first != guid)
            continue;

        if (player && player->GetSession())
            chart[itr->first].ip = player->GetSession()->GetRemoteAddress();

        if (chart[itr->first].teamId == playerTeam->GetId())
        {
            chart[itr->first].teamRating = playerTeam->GetRating();
            if (ArenaTeamMember* member = playerTeam->GetMember(itr->first))
                chart[itr->first].personalRating = member->personal_rating;
        }
        else
        {
            chart[itr->first].teamRating = otherTeam->GetRating();
            if (ArenaTeamMember* member = otherTeam->GetMember(itr->first))
                chart[itr->first].personalRating = member->personal_rating;
        }

        chart[itr->first].damageDone = itr->second->DamageDone;
        chart[itr->first].damageTaken = itr->second->DamageTaken;
        chart[itr->first].healingDone = itr->second->HealingDone;
        chart[itr->first].healingTaken = itr->second->HealingTaken;
        chart[itr->first].deaths = itr->second->Deaths;
        chart[itr->first].kills = itr->second->KillingBlows;
        break;
    }
}

void ArenaChartHolder::FillArenaChartAtEnd(ArenaTeam* winner, ArenaTeam* loser, uint32 winner_rating, uint32 loser_rating, int32 winner_change, int32 loser_change)
{
    if (!winner || !loser)
        return;

    for (BattleGround::BattleGroundScoreMap::const_iterator itr = bg->GetPlayerScoresBegin(); itr != bg->GetPlayerScoresEnd(); ++itr)
    {
        if (chart.find(itr->first) == chart.end())
            continue;

        if (Player* plr = sObjectMgr.GetPlayer(itr->first))
            if (plr->GetSession())
                chart[itr->first].ip = plr->GetSession()->GetRemoteAddress();

        if (chart[itr->first].teamId == winner->GetId())
        {
            chart[itr->first].teamRating = winner_rating;
            chart[itr->first].ratingChange = abs(winner_change);
            chart[itr->first].changeType = CHANGE_TYPE_WIN;
            if (ArenaTeamMember* member = winner->GetMember(itr->first))
                chart[itr->first].personalRating = member->personal_rating;
        }
        else
        {
            chart[itr->first].teamRating = loser_rating;
            chart[itr->first].ratingChange = abs(loser_change);
            chart[itr->first].changeType = CHANGE_TYPE_LOSE;
            if (ArenaTeamMember* member = loser->GetMember(itr->first))
                chart[itr->first].personalRating = member->personal_rating;
        }

        chart[itr->first].damageDone = itr->second->DamageDone;
        chart[itr->first].damageTaken = itr->second->DamageTaken;
        chart[itr->first].healingDone = itr->second->HealingDone;
        chart[itr->first].healingTaken = itr->second->HealingTaken;
        chart[itr->first].deaths = itr->second->Deaths;
        chart[itr->first].kills = itr->second->KillingBlows;
    }

    FillGaps();
    SaveToDB();
}

void ArenaChartHolder::FillGaps()
{
    uint32 winner = 0;
    uint32 winnerChange, winnerRating;
    for (std::map<ObjectGuid, ArenaChartElement>::iterator itr = chart.begin(); itr != chart.end(); ++itr)
    {
        if (itr->second.changeType = CHANGE_TYPE_WIN)
        {
            // winner found.
            winner = itr->second.teamId;
            winnerRating = itr->second.teamRating;
            winnerChange = itr->second.ratingChange;
            break;
        }
    }

    if (winner = 0)
        return;

    // set winner datas
    for (std::map<ObjectGuid, ArenaChartElement>::iterator itr = chart.begin(); itr != chart.end(); ++itr)
    {
        if (itr->second.teamId == winner)
        {
            itr->second.changeType = CHANGE_TYPE_WIN;
            itr->second.teamRating = winnerRating;
            itr->second.ratingChange = winnerChange;
        }
    }

    uint32 loser = 0;
    uint32 loserChange, loserRating;
    // find loser
    for (std::map<ObjectGuid, ArenaChartElement>::iterator itr = chart.begin(); itr != chart.end(); ++itr)
    {
        if (itr->second.changeType = CHANGE_TYPE_LOSE)
        {
            if (itr->second.ratingChange)
            {
                loser = itr->second.teamId;
                loserChange = itr->second.ratingChange;
                loserRating = itr->second.teamRating;
                break;
            }
        }
    }

    if (!loser)
        return;

    // fill loser datas
    for (std::map<ObjectGuid, ArenaChartElement>::iterator itr = chart.begin(); itr != chart.end(); ++itr)
    {
        if (itr->second.teamId == loser)
        {
            //itr->second.changeType = CHANGE_TYPE_LOSER;
            itr->second.teamRating = loserRating;
            itr->second.ratingChange = loserChange;
        }
    }
}

void ArenaChartHolder::SaveToDB()
{
    for (std::map<ObjectGuid, ArenaChartElement>::iterator itr = chart.begin(); itr != chart.end(); ++itr)
        CharacterDatabase.PExecute("REPLACE INTO `armory_game_chart` "
            "(`gameid`, `teamid`, `guid`, "
            "`changeType`, `ratingChange`, `teamRating`, "
            "`damageDone`, `deaths`, `healingDone`, "
            "`damageTaken`, `healingTaken`, `killingBlows`, "
            "`mapId`, `start`, `end`, "
            "`personalRating`, `ip`, `type`) VALUES "
            "('%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '%u', '"UI64FMTD"', '"UI64FMTD"', '%u', '%s', '%u')",
            bg->GetArenaGameId(), itr->second.teamId, itr->first.GetCounter(),
            itr->second.changeType, itr->second.ratingChange, itr->second.teamRating,
            itr->second.damageDone, itr->second.deaths, itr->second.healingDone,
            itr->second.damageTaken, itr->second.healingTaken, itr->second.kills,
            bg->GetMapId(), time(NULL) - bg->GetStartTime() / 1000, time(NULL),
            itr->second.personalRating, itr->second.ip.c_str(), uint8(bg->GetArenaType()));
}

uint32 BattleGround::GetPlayerScore(Player *Source, uint32 type)
{
    BattleGroundScoreMap::const_iterator itr = m_PlayerScores.find(Source->GetObjectGuid());

    if (itr == m_PlayerScores.end())                         // player not found...
        return 0;

    switch(type)
    {
        case SCORE_KILLING_BLOWS:
            return itr->second->KillingBlows;
        case SCORE_DEATHS:
            return itr->second->Deaths;
        case SCORE_HONORABLE_KILLS:
            return itr->second->HonorableKills;
        case SCORE_BONUS_HONOR:
            return itr->second->BonusHonor;
        case SCORE_DAMAGE_DONE:
            return itr->second->DamageDone;
        case SCORE_HEALING_DONE:
            return itr->second->HealingDone;
        case SCORE_DAMAGE_TAKEN:
            return itr->second->DamageTaken;
        case SCORE_HEALING_TAKEN:
            return itr->second->HealingTaken;
    }

    sLog.outError("BattleGround: Unknown player score type %u for %u", type, GetTypeID(true));
    return 0;
}
