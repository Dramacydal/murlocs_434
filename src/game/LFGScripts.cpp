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

/*
 * Interaction between core and LFGScripts
 */


#include "Common.h"
#include "SharedDefines.h"
#include "Player.h"
#include "Group.h"
#include "LFGMgr.h"
#include "ScriptMgr.h"
#include "ObjectAccessor.h"

void LFGMgr::OnLevelChanged(Player* player, uint8 /*oldLevel*/)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    InitializeLockedDungeons(player);
}

void LFGMgr::OnLogout(Player* player)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    if (!player->GetGroup())
    {
        player->GetSession()->SendLfgLfrList(false);
        LeaveLfg(player->GetGUID());
    }
}

void LFGMgr::OnLogin(Player* player)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    // Temporal: Trying to determine when group data and LFG data gets desynched
    uint64 guid = player->GetGUID();
    ObjectGuid gguid = GetGroup(guid);

    if (Group const* group = player->GetGroup())
    {
        ObjectGuid gguid2 = group->GetObjectGuid();
        if (gguid != gguid2)
        {
            SetupGroupMember(guid, group->GetObjectGuid());
        }
    }

    InitializeLockedDungeons(player);
    SetTeam(player->GetGUID(), player->GetTeam());
    // TODO - Restore LfgPlayerData and send proper status to player if it was in a group
}

void LFGMgr::OnBindToInstance(Player* player, Difficulty difficulty, uint32 mapId, bool /*permanent*/)
{
    MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
    if (mapEntry->IsDungeon() && difficulty > DUNGEON_DIFFICULTY_NORMAL)
        InitializeLockedDungeons(player);
}

void LFGMgr::OnAddMember(Group* group, ObjectGuid guid)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    ObjectGuid gguid = group->GetObjectGuid();
    ObjectGuid leader = group->GetLeaderGuid();

    if (leader == guid)
    {
        sLog.outDebug("LFGScripts::OnAddMember [" UI64FMTD "]: added [" UI64FMTD "] leader " UI64FMTD "]", gguid.GetRawValue(), guid.GetRawValue(), leader.GetRawValue());
        SetLeader(gguid, guid);
    }
    else
    {
        LfgState gstate = sLFGMgr.GetState(gguid);
        LfgState state = sLFGMgr.GetState(guid);
        sLog.outDebug("LFGScripts::OnAddMember [" UI64FMTD "]: added [" UI64FMTD "] leader " UI64FMTD "] gstate: %u, state: %u", gguid.GetRawValue(), guid.GetRawValue(), leader.GetRawValue(), gstate, state);

        if (state == LFG_STATE_QUEUED)
            LeaveLfg(guid);

        // TODO - if group is queued and new player is added convert to rolecheck without notify the current players queued
        if (gstate == LFG_STATE_QUEUED)
            LeaveLfg(gguid);
    }

    SetGroup(guid, gguid);
    AddPlayerToGroup(gguid, guid);
}

void LFGMgr::OnRemoveMember(Group* group, ObjectGuid guid, RemoveMethod method, ObjectGuid kicker, char const* reason)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    ObjectGuid gguid = group->GetObjectGuid();
    sLog.outDebug("LFGScripts::OnRemoveMember [" UI64FMTD "]: remove [" UI64FMTD "] Method: %d Kicker: [" UI64FMTD "] Reason: %s", gguid.GetRawValue(), guid.GetRawValue(), method, kicker.GetRawValue(), (reason ? reason : ""));

    bool isLFG = group->isLFGGroup();

    if (isLFG && method == GROUP_REMOVEMETHOD_KICK)        // Player have been kicked
    {
        // TODO - Update internal kick cooldown of kicker
        std::string str_reason = "";
        if (reason)
            str_reason = std::string(reason);
        InitBoot(gguid, kicker, guid, str_reason);
        return;
    }

    LfgState state = GetState(gguid);

    // If group is being formed after proposal success do nothing more
    if (state == LFG_STATE_PROPOSAL && method == GROUP_REMOVEMETHOD_DEFAULT)
    {
        // LfgData: Remove player from group
        SetGroup(guid, 0);
        RemovePlayerFromGroup(gguid, guid);
        return;
    }

    LeaveLfg(guid);
    SetGroup(guid, 0);
    uint8 players = RemovePlayerFromGroup(gguid, guid);

    if (Player* player = ObjectAccessor::FindPlayer(guid))
    {
        if (method == GROUP_REMOVEMETHOD_LEAVE && state == LFG_STATE_DUNGEON &&
            players >= LFG_GROUP_KICK_VOTES_NEEDED)
            player->CastSpell(player, LFG_SPELL_DUNGEON_DESERTER, true);
        //else if (state == LFG_STATE_BOOT)
            // Update internal kick cooldown of kicked

        player->GetSession()->SendLfgUpdateParty(LfgUpdateData(LFG_UPDATETYPE_LEADER_UNK1));
        if (isLFG && player->GetMap()->IsDungeon())            // Teleport player out the dungeon
            TeleportPlayer(player, true);
    }

    if (isLFG && state != LFG_STATE_FINISHED_DUNGEON) // Need more players to finish the dungeon
        if (Player* leader = ObjectAccessor::FindPlayer(sLFGMgr.GetLeader(gguid)))
            leader->GetSession()->SendLfgOfferContinue(sLFGMgr.GetDungeon(gguid, false));
}

void LFGMgr::OnDisband(Group* group)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    ObjectGuid gguid = group->GetObjectGuid();
    sLog.outDebug("LFGScripts::OnDisband [" UI64FMTD "]", gguid.GetRawValue());

    RemoveGroupData(gguid);
}

void LFGMgr::OnChangeLeader(Group* group, ObjectGuid newLeaderGuid, ObjectGuid oldLeaderGuid)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    ObjectGuid gguid = group->GetObjectGuid();

    sLog.outDebug("LFGScripts::OnChangeLeader [" UI64FMTD "]: old [" UI64FMTD "] new [" UI64FMTD "]", gguid.GetRawValue(), newLeaderGuid.GetRawValue(), oldLeaderGuid.GetRawValue());
    SetLeader(gguid, newLeaderGuid);
}

void LFGMgr::OnInviteMember(Group* group, ObjectGuid guid)
{
    if (!isOptionEnabled(LFG_OPTION_ENABLE_DUNGEON_FINDER | LFG_OPTION_ENABLE_RAID_BROWSER))
        return;

    ObjectGuid gguid = group->GetObjectGuid();
    ObjectGuid leader = group->GetLeaderGuid();
    sLog.outDebug("LFGScripts::OnInviteMember [" UI64FMTD "]: invite [" UI64FMTD "] leader [" UI64FMTD "]", gguid.GetRawValue(), guid.GetRawValue(), leader.GetRawValue());
    // No gguid ==  new group being formed
    // No leader == after group creation first invite is new leader
    // leader and no gguid == first invite after leader is added to new group (this is the real invite)
    if (leader && !gguid)
        LeaveLfg(leader);
}
