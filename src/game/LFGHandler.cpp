/*
 * Copyright (C) 2008-2012 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */


#include "WorldSession.h"
#include "WorldPacket.h"
#include "DBCStores.h"
#include "Player.h"
#include "Group.h"
#include "LFGMgr.h"
#include "ObjectMgr.h"

void BuildPlayerLockDungeonBlock(WorldPacket& data, LfgLockMap const& lock)
{
    data << uint32(lock.size());                           // Size of lock dungeons
    for (LfgLockMap::const_iterator it = lock.begin(); it != lock.end(); ++it)
    {
        data << uint32(it->first);                         // Dungeon entry (id + type)
        data << uint32(it->second);                        // Lock status
        data << uint32(0);                                 // Required player ILvL
        data << uint32(0);                                 // Player ILvL
    }
}

void BuildPartyLockDungeonBlock(WorldPacket& data, const LfgLockPartyMap& lockMap)
{
    data << uint8(lockMap.size());
    for (LfgLockPartyMap::const_iterator it = lockMap.begin(); it != lockMap.end(); ++it)
    {
        data << ObjectGuid(it->first);                         // Player guid
        BuildPlayerLockDungeonBlock(data, it->second);
    }
}

void WorldSession::HandleLfgJoinOpcode(WorldPacket& recvData)
{
    if (!sLFGMgr.isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER) ||
        (_player->GetGroup() && _player->GetGroup()->GetLeaderGuid() != _player->GetObjectGuid() &&
        (_player->GetGroup()->GetMembersCount() == MAX_GROUP_SIZE || !_player->GetGroup()->isLFGGroup())))
    {
        recvData.rfinish();
        return;
    }

    std::string comment;
    uint32 commentSize;
    uint32 numDungeons;
    uint32 roles;

    recvData >> roles;

    for (uint8 i = 0; i < 3; ++i)
    {
        recvData.read_skip<uint32>();
    }

    commentSize = recvData.ReadBits(9);
    numDungeons = recvData.ReadBits(24);
    if (!numDungeons)
    {
        sLog.outDebug("CMSG_LFG_JOIN %s no dungeons selected", _player->GetGuidStr().c_str());
        recvData.rfinish();
        return;
    }

    comment = recvData.ReadString(commentSize);

    LfgDungeonSet newDungeons;
    for (uint32 i = 0; i < numDungeons; ++i)
    {
        uint32 dungeon;
        recvData >> dungeon;
        newDungeons.insert((dungeon & 0x00FFFFFF));        // remove the type from the dungeon entry
    }

    sLog.outDebug("CMSG_LFG_JOIN %s roles: %u, Dungeons: %u, Comment: %s",
        _player->GetGuidStr().c_str(), roles, uint8(newDungeons.size()), comment.c_str());

    sLFGMgr.JoinLfg(_player, uint8(roles), newDungeons, comment);
}

void WorldSession::HandleLfgLeaveOpcode(WorldPacket&  recvData)
{
    uint32 roles, time, reason, instanceId;
    recvData >> roles >> time >> reason >> instanceId;

    Group* group = _player->GetGroup();
    ObjectGuid guid = _player->GetObjectGuid();
    ObjectGuid gguid = group ? group->GetObjectGuid() : guid;

    sLog.outDebug("CMSG_LFG_LEAVE %s in group: %u",
        guid.GetString().c_str(), group ? 1 : 0);

    // Check cheating - only leader can leave the queue
    if (!group || group->GetLeaderGuid() == guid)
        sLFGMgr.LeaveLfg(gguid);
}

void WorldSession::HandleLfgProposalResultOpcode(WorldPacket& recvData)
{
    uint32 lfgGroupID;                   // Internal lfgGroupID
    uint32 time;
    uint32 roles;
    uint32 unk;
    bool accept;                         // Accept to join?

    ObjectGuid playerGuid;
    ObjectGuid instanceGuid;

    recvData
        >> lfgGroupID // dword28
        >> time // f0[16]
        >> roles // f0[12]
        >> unk; // f0[8]

    recvData.ReadGuidMask<4, 5, 0, 6, 2, 7, 1, 3>(playerGuid);
    recvData.ReadGuidBytes<7, 4, 3, 2, 6, 0, 1, 5>(playerGuid);

    recvData.ReadGuidMask<7>(instanceGuid);

    accept = recvData.ReadBit();

    recvData.ReadGuidMask<1, 3, 0, 5, 4, 6, 2>(instanceGuid);
    recvData.ReadGuidBytes<7, 1, 5, 6, 3, 4, 0, 2>(instanceGuid);

    sLog.outDebug("CMSG_LFG_PROPOSAL_RESULT %s proposal: %u accept: %u",
        _player->GetGuidStr().c_str(), lfgGroupID, accept ? 1 : 0);
    sLFGMgr.UpdateProposal(lfgGroupID, playerGuid, accept);
}

void WorldSession::HandleLfgSetRolesOpcode(WorldPacket& recvData)
{
    uint32 roles;
    recvData >> roles;                                     // Player Group Roles
    
    ObjectGuid guid = _player->GetObjectGuid();
    Group* group = _player->GetGroup();
    if (!group)
    {
        sLog.outDebug("CMSG_LFG_SET_ROLES %s Not in group",
            guid.GetString().c_str());
        return;
    }
    ObjectGuid gguid = group->GetObjectGuid();
    sLog.outDebug("CMSG_LFG_SET_ROLES: Group %u, Player %s, Roles: %u",
        gguid.GetCounter(), guid.GetString().c_str(), roles);

    sLFGMgr.UpdateRoleCheck(gguid, guid, roles);
}

void WorldSession::HandleLfgSetCommentOpcode(WorldPacket&  recvData)
{
    std::string comment = recvData.ReadString(recvData.ReadBits(9));

    sLog.outDebug("CMSG_LFG_SET_COMMENT %s comment: %s",
        _player->GetGuidStr().c_str(), comment.c_str());

    sLFGMgr.SetComment(_player->GetObjectGuid(), comment);
}

void WorldSession::HandleLfgSetBootVoteOpcode(WorldPacket& recvData)
{
    bool agree = recvData.ReadBit();

    ObjectGuid guid = _player->GetObjectGuid();
    sLog.outDebug("CMSG_LFG_SET_BOOT_VOTE %s agree: %u",
        guid.GetString().c_str(), agree ? 1 : 0);
    sLFGMgr.UpdateBoot(guid, agree);
}

void WorldSession::HandleLfgTeleportOpcode(WorldPacket& recvData)
{
    bool out = recvData.ReadBit();

    sLog.outDebug("CMSG_LFG_TELEPORT %s out: %u",
        _player->GetGuidStr().c_str(), out ? 1 : 0);
    sLFGMgr.TeleportPlayer(_player, out, true);
}

void WorldSession::HandleLfgPlayerLockInfoRequestOpcode(WorldPacket& /*recvData*/)
{
    ObjectGuid guid = _player->GetObjectGuid();
    sLog.outDebug( "CMSG_LFG_PLAYER_LOCK_INFO_REQUEST %s",
        guid.GetString().c_str());

    // Get Random dungeons that can be done at a certain level and expansion
    LfgDungeonSet randomDungeons;
    uint8 level = _player->getLevel();
    uint8 expansion = _player->GetSession()->Expansion();

    LFGDungeonContainer& LfgDungeons = sLFGMgr.GetLFGDungeonMap();
    for (LFGDungeonContainer::const_iterator itr = LfgDungeons.begin(); itr != LfgDungeons.end(); ++itr)
    {
        LFGDungeonData const& dungeon = itr->second;
        if ((dungeon.type == LFG_TYPE_RANDOM || (dungeon.seasonal && sLFGMgr.IsSeasonActive(dungeon.id)))
            && dungeon.expansion <= expansion && dungeon.minlevel <= level && level <= dungeon.maxlevel)
            randomDungeons.insert(dungeon.Entry());
    }

    // Get player locked Dungeons
    LfgLockMap const& lock = sLFGMgr.GetLockedDungeons(guid);
    uint32 rsize = uint32(randomDungeons.size());
    uint32 lsize = uint32(lock.size());

    sLog.outDebug("SMSG_LFG_PLAYER_INFO %s", _player->GetGuidStr().c_str());
    WorldPacket data(SMSG_LFG_PLAYER_INFO, 1 + rsize * (4 + 1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4) + 4 + lsize * (1 + 4 + 4 + 4 + 4 + 1 + 4 + 4 + 4));

    data << uint8(randomDungeons.size());                  // Random Dungeon count
    for (LfgDungeonSet::const_iterator it = randomDungeons.begin(); it != randomDungeons.end(); ++it)
    {
        data << uint32(*it);                               // Dungeon Entry (id + type)
        LfgReward const* reward = sLFGMgr.GetRandomDungeonReward(*it, level);
        Quest const* quest = NULL;
        bool done = false;
        if (reward)
        {
            quest = sObjectMgr.GetQuestTemplate(reward->firstQuest);
            if (quest)
            {
                done = !_player->CanRewardQuest(quest, false);
                if (done)
                    quest = sObjectMgr.GetQuestTemplate(reward->otherQuest);
            }
        }

        if (quest)
        {
            data << uint8(done);
            data << uint32(500); // Times precision
            data << uint32(500); // Available times per week

            data << uint32(396); // Unknown 4.3.4
            data << uint32(0); // Unknown 4.3.4

            data << uint32(100000); // Unknown 4.3.4
            data << uint32(0); // Unknown 4.3.4
            data << uint32(0); // Unknown 4.3.4
            data << uint32(0); // Unknown 4.3.4
            data << uint32(100000); // Unknown 4.3.4
            data << uint32(70000); // Unknown 4.3.4
            data << uint32(80000); // Unknown 4.3.4

            data << uint32(90000); // Unknown 4.3.4
            data << uint32(50000); // isComplited

            data << uint8(100); // seasonal ?
            {
                for (uint8 i = 0; i < 3; ++i) // 3 - Max roles ?
                {
                    uint8 callToArmsRoleMask = 0; // TODO Call to arms role check (LfgRoles) Not implemented
                    data << uint32(callToArmsRoleMask);
                    if (callToArmsRoleMask > 0)
                    {
                        /* Call to Arms bonus*/

                        data << uint32(0); // Call to arms Money
                        data << uint32(0); // Call to arms XP

                        uint8 totalRewardCount = uint8(quest->GetRewCurrencyCount() + quest->GetRewItemsCount());
                        if (totalRewardCount > 16)
                            totalRewardCount = 16;

                        data << uint8(totalRewardCount);
                        if (totalRewardCount)
                        {
                            for (uint8 j = 0; j < QUEST_REWARD_CURRENCY_COUNT; ++j)
                            {
                                uint32 id = quest->RewCurrencyId[j];
                                if (!id)
                                    continue;

                                uint32 amount = quest->RewCurrencyCount[j];
                                if (CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(id))
                                    amount *= currency->GetPrecision();

                                data << uint32(id);
                                data << uint32(0);
                                data << uint32(amount);
                                data << uint8(true); // Is currency
                            }

                            ItemPrototype const* iProto = NULL;
                            for (uint8 j = 0; j < QUEST_REWARDS_COUNT; ++j)
                            {
                                if (!quest->RewItemId[j])
                                    continue;

                                iProto = sObjectMgr.GetItemPrototype(quest->RewItemId[j]);

                                data << uint32(quest->RewItemId[j]);
                                data << uint32(iProto ? iProto->DisplayInfoID : 0);
                                data << uint32(quest->RewItemCount[j]);
                                data << uint8(false); // Is currency
                            }
                        }
                    }
                }
            }

            data << uint32(quest->GetRewOrReqMoney());
            data << uint32(quest->XPValue(_player));

            uint8 totalRewardCount = uint8(quest->GetRewCurrencyCount() + quest->GetRewItemsCount());
            if (totalRewardCount > 16)
                totalRewardCount = 16;

            data << uint8(totalRewardCount);
            if (totalRewardCount)
            {
                for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
                {
                    uint32 id = quest->RewCurrencyId[i];
                    if (!id)
                        continue;

                    uint32 amount = quest->RewCurrencyCount[i];
                    if (CurrencyTypesEntry const* currency = sCurrencyTypesStore.LookupEntry(id))
                        amount *= currency->GetPrecision();

                    data << uint32(id);
                    data << uint32(0);
                    data << uint32(amount);
                    data << uint8(true); // Is currency
                }

                ItemPrototype const* iProto = NULL;
                for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
                {
                    if (!quest->RewItemId[i])
                        continue;

                    iProto = sObjectMgr.GetItemPrototype(quest->RewItemId[i]);

                    data << uint32(quest->RewItemId[i]);
                    data << uint32(iProto ? iProto->DisplayInfoID : 0);
                    data << uint32(quest->RewItemCount[i]);
                    data << uint8(false); // Is currency
                }
            }
        }
        else
        {
            data << uint8(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);
            data << uint32(0);

            for (int8 i = 0; i < 9; ++i)
                data << uint32(0); // Unknown 4.3.4

            data << uint8(1);
            for (int8 i = 0; i < 3; ++i)
                data << uint32(0); // Unknown 4.3.4

            for (int8 i = 0; i < 2; ++i)
                data << uint32(0); // Unknown 4.3.4

            data << uint8(0);
        }
    }
    BuildPlayerLockDungeonBlock(data, lock);
    SendPacket(&data);
}

void WorldSession::HandleLfgPartyLockInfoRequestOpcode(WorldPacket&  /*recvData*/)
{
    ObjectGuid guid = _player->GetObjectGuid();
    sLog.outDebug("CMSG_LFG_PARTY_LOCK_INFO_REQUEST %s", guid.GetString().c_str());

    Group* grp = _player->GetGroup();
    if (!grp)
        return;

    // Get the locked dungeons of the other party members
    LfgLockPartyMap lockMap;
    for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
    {
        Player* plrg = itr->getSource();
        if (!plrg)
            continue;

        ObjectGuid pguid = plrg->GetObjectGuid();
        if (pguid == guid)
            continue;

        lockMap[pguid] = sLFGMgr.GetLockedDungeons(pguid);
    }

    uint32 size = 0;
    for (LfgLockPartyMap::const_iterator it = lockMap.begin(); it != lockMap.end(); ++it)
        size += 8 + 4 + uint32(it->second.size()) * (4 + 4 + 4 + 4);

    sLog.outDebug("SMSG_LFG_PARTY_INFO %s", _player->GetGuidStr().c_str());
    WorldPacket data(SMSG_LFG_PARTY_INFO, 1 + size);
    BuildPartyLockDungeonBlock(data, lockMap);
    SendPacket(&data);
}

void WorldSession::HandleLfrJoinOpcode(WorldPacket& recvData)
{
    uint32 entry;                                          // Raid id to search
    recvData >> entry;
    sLog.outDebug("CMSG_LFG_LFR_JOIN %s dungeon entry: %u",
        _player->GetGuidStr().c_str(), entry);
    //SendLfrUpdateListOpcode(entry);
}

void WorldSession::HandleLfrLeaveOpcode(WorldPacket& recvData)
{
    uint32 dungeonId;                                      // Raid id queue to leave
    recvData >> dungeonId;
    sLog.outDebug("CMSG_LFG_LFR_LEAVE %s dungeonId: %u",
        _player->GetGuidStr().c_str(), dungeonId);
    //sLFGMgr.LeaveLfr(_player, dungeonId);
}

void WorldSession::HandleLfgGetStatus(WorldPacket& /*recvData*/)
{
    sLog.outDebug("CMSG_LFG_GET_STATUS %s", _player->GetGuidStr().c_str());

    ObjectGuid guid = _player->GetObjectGuid();
    LfgUpdateData updateData = sLFGMgr.GetLfgStatus(guid);

    if (_player->GetGroup())
    {
        SendLfgUpdateParty(updateData);
        updateData.dungeons.clear();
        SendLfgUpdatePlayer(updateData);
    }
    else
    {
        SendLfgUpdatePlayer(updateData);
        updateData.dungeons.clear();
        SendLfgUpdateParty(updateData);
    }
}

void WorldSession::SendLfgUpdatePlayer(LfgUpdateData const& updateData)
{
    // LFG_UPDATETYPE_JOIN_QUEUE  for single player and LFG_UPDATETYPE_ADDED_TO_QUEUE for group
    bool queued = false;
    bool join = false;
    uint8 size = uint8(updateData.dungeons.size());

    switch (updateData.updateType)
    {
        case LFG_UPDATETYPE_JOIN_QUEUE:
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:
            queued = true;
            // no break on purpose
        case LFG_UPDATETYPE_PROPOSAL_BEGIN:
            join = true;
            break;
        case LFG_UPDATETYPE_UPDATE_STATUS:
            queued = updateData.state == LFG_STATE_QUEUED;
            join = updateData.state != LFG_STATE_ROLECHECK && updateData.state != LFG_STATE_NONE;
            break;
        default:
            break;
    }

    sLog.outDebug("SMSG_LFG_UPDATE_STATUS %s updatetype: %u",
        _player->GetGuidStr().c_str(), updateData.updateType);

    ObjectGuid guid = _player->GetObjectGuid();

    WorldPacket data(SMSG_LFG_UPDATE_STATUS, 1 + 1 + (size > 0 ? 1 : 0) * (1 + 1 + 1 + 1 + size * 4 + updateData.comment.length()));

    data.WriteGuidMask<1>(guid);
    data.WriteBit(false);
    data.WriteBits(size, 24);
    data.WriteGuidMask<6>(guid);
    data.WriteBit(join);
    data.WriteBits(updateData.comment.length(), 9);
    data.WriteGuidMask<4, 7, 2>(guid);
    data.WriteBit(updateData.updateType == LFG_UPDATETYPE_ADDED_TO_QUEUE);             // LFGJoined
    data.WriteGuidMask<0, 3, 5>(guid);
    data.WriteBit(queued);

    data << uint8(0/*updateData.updateType*/);

    data.WriteStringData(updateData.comment);

    data << uint32(sLFGMgr.GetOrGenerateQueueId<false>(guid));
    data << uint32(WorldTimer::getMSTime());

    data.WriteGuidBytes<6>(guid);

    data << uint8(168);
    for (uint8 i = 0; i < 2; ++i)
    {
        data << uint8(0);
    }

    data.WriteGuidBytes<1, 2, 4, 3, 5, 0>(guid);

    data << uint32(0);
    data.WriteGuidBytes<7>(guid);

    for (LfgDungeonSet::const_iterator it = updateData.dungeons.begin(); it != updateData.dungeons.end(); ++it)
        data << uint32(*it);

    SendPacket(&data);
}

void WorldSession::SendLfgUpdateParty(const LfgUpdateData& updateData)
{
    bool join = false;
    bool queued = false;
    uint8 size = uint8(updateData.dungeons.size());

    switch (updateData.updateType)
    {
        case LFG_UPDATETYPE_ADDED_TO_QUEUE:                // Rolecheck Success
            queued = true;
            // no break on purpose
        case LFG_UPDATETYPE_PROPOSAL_BEGIN:
            join = true;
            break;
        case LFG_UPDATETYPE_UPDATE_STATUS:
            join = updateData.state != LFG_STATE_ROLECHECK && updateData.state != LFG_STATE_NONE;
            queued = updateData.state == LFG_STATE_QUEUED;
            break;
        default:
            break;
    }

    ObjectGuid guid = _player->GetObjectGuid();

    sLog.outDebug( "SMSG_LFG_UPDATE_STATUS %s updatetype: %u",
        guid.GetString().c_str(), updateData.updateType);

    WorldPacket data(SMSG_LFG_UPDATE_STATUS, 1 + 1 + (size > 0 ? 1 : 0) * (1 + 1 + 1 + 1 + size * 4 + updateData.comment.length()));

    data.WriteGuidMask<1>(guid);
    data.WriteBit(false);
    data.WriteBits(size, 24);
    data.WriteGuidMask<6>(guid);
    data.WriteBit(join);
    data.WriteBits(updateData.comment.length(), 9);
    data.WriteGuidMask<4, 7, 2>(guid);
    data.WriteBit(join);
    data.WriteGuidMask<0, 3, 5>(guid);
    data.WriteBit(queued);

    data << uint8(0/*updateData.updateType*/);

    data.WriteStringData(updateData.comment);

    data << uint32(sLFGMgr.GetOrGenerateQueueId<false>(guid));
    data << uint32(WorldTimer::getMSTime());

    data.WriteGuidBytes<6>(guid);

    data << uint8(168);
    for (uint8 i = 0; i < 2; ++i)
    {
        data << uint8(0);
    }

    data.WriteGuidBytes<1, 2, 4, 3, 5, 0>(guid);
    data << uint32(0);
    data.WriteGuidBytes<7>(guid);

    for (LfgDungeonSet::const_iterator it = updateData.dungeons.begin(); it != updateData.dungeons.end(); ++it)
        data << uint32(*it);

    SendPacket(&data);
}

void WorldSession::SendLfgRoleChosen(ObjectGuid guid, uint8 roles)
{
    sLog.outDebug("SMSG_LFG_ROLE_CHOSEN %s guid: %u roles: %u",
        _player->GetGuidStr().c_str(), guid.GetCounter(), roles);

    WorldPacket data(SMSG_LFG_ROLE_CHOSEN, 8 + 1 + 4);
    data << guid;                                          // Guid
    data << uint8(roles > 0);                              // Ready
    data << uint32(roles);                                 // Roles
    SendPacket(&data);
}

void WorldSession::SendLfgRoleCheckUpdate(const LfgRoleCheck& roleCheck)
{
    LfgDungeonSet dungeons;
    if (roleCheck.rDungeonId)
        dungeons.insert(roleCheck.rDungeonId);
    else
        dungeons = roleCheck.dungeons;

    sLog.outDebug("SMSG_LFG_ROLE_CHECK_UPDATE %s", _player->GetGuidStr().c_str());
    WorldPacket data(SMSG_LFG_ROLE_CHECK_UPDATE, 4 + 1 + 1 + dungeons.size() * 4 + 1 + roleCheck.roles.size() * (8 + 1 + 4 + 1));

    data << uint32(roleCheck.state);                       // Check result
    data << uint8(roleCheck.state == LFG_ROLECHECK_INITIALITING);
    data << uint8(dungeons.size());                        // Number of dungeons
    if (!dungeons.empty())
    {
        for (LfgDungeonSet::iterator it = dungeons.begin(); it != dungeons.end(); ++it)
        {
            LFGDungeonData const* dungeon = sLFGMgr.GetLFGDungeon(*it);
            data << uint32(dungeon ? dungeon->Entry() : 0); // Dungeon
        }
    }

    data << uint8(roleCheck.roles.size());                 // Players in group
    if (!roleCheck.roles.empty())
    {
        // Leader info MUST be sent 1st :S
        ObjectGuid guid = roleCheck.leader;
        uint8 roles = roleCheck.roles.find(guid)->second;
        data << ObjectGuid(guid);                              // Guid
        data << uint8(roles > 0);                          // Ready
        data << uint32(roles);                             // Roles
        Player* player = ObjectAccessor::FindPlayer(guid);
        data << uint8(player ? player->getLevel() : 0);    // Level

        for (LfgRolesMap::const_iterator it = roleCheck.roles.begin(); it != roleCheck.roles.end(); ++it)
        {
            if (it->first == roleCheck.leader)
                continue;

            guid = it->first;
            roles = it->second;
            data << ObjectGuid(guid);                          // Guid
            data << uint8(roles > 0);                      // Ready
            data << uint32(roles);                         // Roles
            player = ObjectAccessor::FindPlayer(guid);
            data << uint8(player ? player->getLevel() : 0);// Level
        }
    }
    SendPacket(&data);
}

void WorldSession::SendLfgJoinResult(ObjectGuid jguid, LfgJoinResultData const& joinData)
{
    uint32 size = 0;
    for (LfgLockPartyMap::const_iterator it = joinData.lockmap.begin(); it != joinData.lockmap.end(); ++it)
        size += 8 + 4 + uint32(it->second.size()) * (4 + 4 + 4 + 4);

    sLog.outDebug("SMSG_LFG_JOIN_RESULT %s checkResult: %u checkValue: %u",
        _player->GetGuidStr().c_str(), joinData.result, joinData.state);

    WorldPacket data(SMSG_LFG_JOIN_RESULT, 4 + 4 + size);

    ObjectGuid joinGuid(jguid);

    data << uint32(0);                                           // Unk
    data << uint8(0/*joinData.result*/);                              // Check Result
    data << uint32(sLFGMgr.GetOrGenerateQueueId<false>(jguid)); // Queue id
    data << uint8(joinData.state);                               // Check Value
    data << uint32(WorldTimer::getMSTime());

    data.WriteGuidMask<2, 7, 3, 0>(joinGuid);
    data.WriteBits(joinData.lockmap.size(), 24);

    for (LfgLockPartyMap::const_iterator it = joinData.lockmap.begin(); it != joinData.lockmap.end(); ++it)
    {
        ObjectGuid guid(it->first);

        data.WriteGuidMask<7, 5, 3, 6, 0, 2, 4, 1>(guid);
        data.WriteBits(it->second.size(), 22);
    }

    data.WriteGuidMask<4, 5, 1, 6>(joinGuid);

    for (LfgLockPartyMap::const_iterator itr = joinData.lockmap.begin(); itr != joinData.lockmap.end(); ++itr)
    {
        LfgLockMap second = itr->second;
        for (LfgLockMap::const_iterator it = second.begin(); it != second.end(); ++it)
        {
            data << uint32(it->first);
            data << uint32(it->second);
            data << uint32(0); // needed ILvL
            data << uint32(0); // player ILvL
        }

        ObjectGuid guid(itr->first);

        data.WriteGuidBytes<2, 5, 1, 0, 4, 3, 6, 7>(guid);
    }

    data.WriteGuidBytes<1, 4, 3, 5, 0, 7, 2, 6>(joinGuid);

    SendPacket(&data);
}

void WorldSession::SendLfgQueueStatus(LfgQueueStatusData const& queueData)
{
    sLog.outDebug("SMSG_LFG_QUEUE_STATUS %s dungeon: %u, waitTime: %d, "
        "avgWaitTime: %d, waitTimeTanks: %d, waitTimeHealer: %d, waitTimeDps: %d, "
        "queuedTime: %u, tanks: %u, healers: %u, dps: %u",
        _player->GetGuidStr().c_str(), queueData.dungeonId, queueData.waitTime, queueData.waitTimeAvg,
        queueData.waitTimeTank, queueData.waitTimeHealer, queueData.waitTimeDps,
        queueData.queuedTime, queueData.tanks, queueData.healers, queueData.dps);


    uint32 dungeonId = queueData.dungeonId;
    /*Make dungeon entry*/
    {
        LFGDungeonEntry const* dungeonEntry = sLFGDungeonStore.LookupEntry(dungeonId);
        if (dungeonEntry)
            dungeonId = dungeonEntry->Entry();
    }
    ObjectGuid guid = _player->GetObjectGuid();

    WorldPacket data(SMSG_LFG_QUEUE_STATUS, 4 + 4 + 4 + 4 + 4 +4 + 1 + 1 + 1 + 4);

    data.WriteGuidMask<3, 2, 0, 6, 5, 7, 1, 4>(guid);

    data.WriteGuidBytes<0>(guid);

    data << uint8(queueData.tanks);                        // Tanks needed
    data << int32(queueData.waitTimeTank);                 // Wait Tanks
    data << uint8(queueData.healers);                      // Healers needed
    data << int32(queueData.waitTimeHealer);               // Wait Healers
    data << uint8(queueData.dps);                          // Dps needed
    data << int32(queueData.waitTimeDps);                  // Wait Dps

    data.WriteGuidBytes<4, 6>(guid);

    data << int32(queueData.waitTimeAvg);                  // Average Wait time
    data << int32(queueData.joinTime);                     // Join Time
    data << uint32(dungeonId);                             // Dungeon
    data << uint32(queueData.queuedTime);                  // Player wait time in queue

    data.WriteGuidBytes<5, 7, 3>(guid);

    data << uint32(sLFGMgr.GetOrGenerateQueueId<false>(guid));

    data.WriteGuidBytes<1, 2>(guid);

    data << int32(queueData.waitTime);                     // Wait Time
    data << int32(0);                                      // Same value than "Unk_UInt32_1" in SMSG_LFG_JOIN_RESULT - Only seen 3

    SendPacket(&data);
}

void WorldSession::SendLfgPlayerReward(LfgPlayerRewardData const& rewardData)
{
    if (!rewardData.rdungeonEntry || !rewardData.sdungeonEntry || !rewardData.quest)
        return;

    uint8 totalRewardCount = uint8(rewardData.quest->GetRewCurrencyCount() + rewardData.quest->GetRewItemsCount());
    if (totalRewardCount > 16)
        totalRewardCount = 16;

    sLog.outDebug("SMSG_LFG_PLAYER_REWARD %s rdungeonEntry: %u - sdungeonEntry: %u - done: %u",
        _player->GetGuidStr().c_str(), rewardData.rdungeonEntry, rewardData.sdungeonEntry, rewardData.done);

    WorldPacket data(SMSG_LFG_PLAYER_REWARD, 4 + 4 + 1 + 4 + 4 + 4 + 4 + 4 + 1 + totalRewardCount * (4 + 4 + 4 + 1));
    data << uint32(rewardData.rdungeonEntry);              // Random Dungeon Finished
    data << uint32(rewardData.sdungeonEntry);              // Dungeon Finished
    data << uint32(rewardData.quest->GetRewOrReqMoney());
    data << uint32(rewardData.quest->XPValue(_player));
    data << uint8(totalRewardCount);
    if (totalRewardCount)
    {
        for (uint8 i = 0; i < QUEST_REWARDS_COUNT; ++i)
        {
            if (uint32 itemId = rewardData.quest->RewItemId[i])
            {
                ItemPrototype const* item = sObjectMgr.GetItemPrototype(itemId);
                data << uint32(itemId);
                data << uint32(item ? item->DisplayInfoID : 0);
                data << uint32(rewardData.quest->RewItemCount[i]);
                data << uint8(false); // Is currency
            }
        }

        for (uint8 i = 0; i < QUEST_REWARD_CURRENCY_COUNT; ++i)
        {
            if (uint32 currencyId = rewardData.quest->RewCurrencyId[i])
            {
                uint32 amount = rewardData.quest->RewCurrencyCount[i];
                amount *= GetCurrencyPrecision(currencyId);

                data << uint32(currencyId);
                data << uint32(0);
                data << uint32(amount);
                data << uint8(true); // Is currency
            }
        }
    }
    SendPacket(&data);
}

void WorldSession::SendLfgBootProposalUpdate(LfgPlayerBoot const& boot)
{
    ObjectGuid guid = _player->GetObjectGuid();
    LfgAnswer playerVote = boot.votes.find(guid)->second;
    uint8 votesNum = 0;
    uint8 agreeNum = 0;
    uint32 secsleft = uint8((boot.cancelTime - time(NULL)) / 1000);
    for (LfgAnswerContainer::const_iterator it = boot.votes.begin(); it != boot.votes.end(); ++it)
    {
        if (it->second != LFG_ANSWER_PENDING)
        {
            ++votesNum;
            if (it->second == LFG_ANSWER_AGREE)
                ++agreeNum;
        }
    }
    sLog.outDebug("SMSG_LFG_BOOT_PROPOSAL_UPDATE %s inProgress: %u - "
        "didVote: %u - agree: %u - victim: %u votes: %u - agrees: %u - left: %u - "
        "needed: %u - reason %s",
        guid.GetString().c_str(), uint8(boot.inProgress), uint8(playerVote != LFG_ANSWER_PENDING),
        uint8(playerVote == LFG_ANSWER_AGREE), boot.victim.GetCounter(), votesNum, agreeNum,
        secsleft, LFG_GROUP_KICK_VOTES_NEEDED, boot.reason.c_str());
    WorldPacket data(SMSG_LFG_BOOT_PROPOSAL_UPDATE, 1 + 1 + 1 + 8 + 4 + 4 + 4 + 4 + boot.reason.length());
    data << uint8(boot.inProgress);                        // Vote in progress
    data << uint8(0);                                      // Succesed ? if inProgress = false then value in this field will lead to UI Error: false - ERR_PARTY_LFG_BOOT_VOTE_FAILED and true - ERR_PARTY_LFG_BOOT_VOTE_SUCCEEDE and 1
    data << uint8(playerVote != LFG_ANSWER_PENDING);       // I Voted
    data << uint8(playerVote == LFG_ANSWER_AGREE);         // My vote
    data << boot.victim;                                   // Victim GUID
    data << uint32(votesNum);                              // Total Votes
    data << uint32(agreeNum);                              // Agree Count
    data << uint32(secsleft);                              // Time Left
    data << uint32(LFG_GROUP_KICK_VOTES_NEEDED);           // Needed Votes
    data << boot.reason.c_str();                           // Kick reason
    SendPacket(&data);
}

void WorldSession::SendLfgUpdateProposal(LfgProposal const& proposal)
{
    ObjectGuid guid = _player->GetObjectGuid();
    ObjectGuid gguid = proposal.players.find(guid)->second.group;

    bool silent = !proposal.isNew && gguid == proposal.group;
    uint32 dungeonEntry = proposal.dungeonId;

    sLog.outDebug("SMSG_LFG_PROPOSAL_UPDATE %s state: %u",
        guid.GetString().c_str(), proposal.state);

    // show random dungeon if player selected random dungeon and it's not lfg group
    if (!silent)
    {
        LfgDungeonSet const& playerDungeons = sLFGMgr.GetSelectedDungeons(guid);
        if (playerDungeons.find(proposal.dungeonId) == playerDungeons.end())
            dungeonEntry = (*playerDungeons.begin());
    }

    if (LFGDungeonData const* dungeon = sLFGMgr.GetLFGDungeon(dungeonEntry))
        dungeonEntry = dungeon->Entry();

    WorldPacket data(SMSG_LFG_PROPOSAL_UPDATE, 4 + 1 + 4 + 4 + 1 + 1 + proposal.players.size() * (4 + 1 + 1 + 1 + 1 +1));
    data << uint32(WorldTimer::getMSTime());                           // Date
    data << uint32(proposal.encounters);                   // Bosses killed mask - encounters done?
    data << uint32(proposal.id);                                     // Unk1
    data << uint32(proposal.id);                                     // Unk2
    data << uint32(dungeonEntry);                          // Dungeon
    data << uint32(proposal.id);                                     // Unk3 - proposalId ? proposal.id
    data << uint8(proposal.state);                         // Proposal state

    data.WriteGuidMask<4>(gguid);
    data.WriteGuidMask<3, 7, 0>(guid);
    data.WriteGuidMask<1>(gguid);
    data.WriteBit(silent);
    data.WriteGuidMask<4, 5>(guid);
    data.WriteGuidMask<3>(gguid);
    data.WriteBits(proposal.players.size(), 23);
    data.WriteGuidMask<7>(gguid);

    for (LfgProposalPlayerContainer::const_iterator it = proposal.players.begin(); it != proposal.players.end(); ++it)
    {
        LfgProposalPlayer const& player = it->second;

        data.WriteBit(player.group == proposal.group);      // In Dungeon
        data.WriteBit(player.group == gguid);               // Same Group?
        data.WriteBit(player.accept == LFG_ANSWER_AGREE);   // Accepted
        data.WriteBit(player.accept != LFG_ANSWER_PENDING); // Answered
        data.WriteBit(it->first == guid);                   // Self player
    }

    data.WriteGuidMask<5>(gguid);
    data.WriteGuidMask<6>(guid);
    data.WriteGuidMask<2, 6>(gguid);
    data.WriteGuidMask<2, 1>(guid);
    data.WriteGuidMask<0>(gguid);

    data.WriteGuidBytes<5>(guid);
    data.WriteGuidBytes<3, 6>(gguid);
    data.WriteGuidBytes<6, 0>(guid);
    data.WriteGuidBytes<5>(gguid);
    data.WriteGuidBytes<1>(guid);

    for (LfgProposalPlayerContainer::const_iterator it = proposal.players.begin(); it != proposal.players.end(); ++it)
        data << uint32(it->second.role);                   // Role

    data.WriteGuidBytes<7>(gguid);
    data.WriteGuidBytes<4>(guid);
    data.WriteGuidBytes<0, 1>(gguid);
    data.WriteGuidBytes<2, 7>(guid);
    data.WriteGuidBytes<2>(gguid);
    data.WriteGuidBytes<3>(guid);
    data.WriteGuidBytes<4>(gguid);

    SendPacket(&data);
}

void WorldSession::SendLfgLfrList(bool update)
{
    sLog.outDebug("SMSG_LFG_LFR_LIST %s update: %u",
        _player->GetGuidStr().c_str(), update ? 1 : 0);
    WorldPacket data(SMSG_LFG_UPDATE_SEARCH, 1);
    data << uint8(update);                                 // In Lfg Queue?
    SendPacket(&data);
}

void WorldSession::SendLfgDisabled()
{
    sLog.outDebug("SMSG_LFG_DISABLED %s", _player->GetGuidStr().c_str());
    WorldPacket data(SMSG_LFG_DISABLED, 0);
    SendPacket(&data);
}

void WorldSession::SendLfgOfferContinue(uint32 dungeonEntry)
{
    sLog.outDebug("SMSG_LFG_OFFER_CONTINUE %s dungeon entry: %u",
        _player->GetGuidStr().c_str(), dungeonEntry);
    WorldPacket data(SMSG_LFG_OFFER_CONTINUE, 4);
    data << uint32(dungeonEntry);
    SendPacket(&data);
}

void WorldSession::SendLfgTeleportError(uint8 err)
{
    sLog.outDebug("SMSG_LFG_TELEPORT_DENIED %s reason: %u",
        _player->GetGuidStr().c_str(), err);
    WorldPacket data(SMSG_LFG_TELEPORT_DENIED, 4);
    data << uint32(err);                                   // Error
    SendPacket(&data);
}

/*
void WorldSession::SendLfrUpdateListOpcode(uint32 dungeonEntry)
{
    sLog->outDebug(LOG_FILTER_PACKETIO, "SMSG_LFG_UPDATE_LIST %s dungeon entry: %u",
        _player->GetObjectGuid(), dungeonEntry);
    WorldPacket data(SMSG_LFG_UPDATE_LIST);
    SendPacket(&data);
}
*/
