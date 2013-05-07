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

#include "Common.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "Corpse.h"
#include "Player.h"
#include "Vehicle.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "MapManager.h"
#include "Transports.h"
#include "BattleGround/BattleGround.h"
#include "WaypointMovementGenerator.h"
#include "MapPersistentStateMgr.h"
#include "ObjectMgr.h"
#include "TemporarySummon.h"
#include "World.h"

void WorldSession::HandleMoveWorldportAckOpcode( WorldPacket & /*recv_data*/ )
{
    DEBUG_LOG( "WORLD: got MSG_MOVE_WORLDPORT_ACK." );
    HandleMoveWorldportAckOpcode();
}

void WorldSession::HandleMoveWorldportAckOpcode()
{
    // ignore unexpected far teleports
    if(!GetPlayer()->IsBeingTeleportedFar())
        return;

    if (_player->GetVehicleKit())
        _player->GetVehicleKit()->RemoveAllPassengers();

    // get start teleport coordinates (will used later in fail case)
    WorldLocation old_loc;
    GetPlayer()->GetPosition(old_loc);

    // get the teleport destination
    WorldLocation &loc = GetPlayer()->GetTeleportDest();

    // possible errors in the coordinate validity check (only cheating case possible)
    if (!MapManager::IsValidMapCoord(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation))
    {
        ERROR_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s was teleported far to a not valid location "
            "(map:%u, x:%f, y:%f, z:%f) We port him to his homebind instead..",
            GetPlayer()->GetGuidStr().c_str(), loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);
        // stop teleportation else we would try this again and again in LogoutPlayer...
        GetPlayer()->SetSemaphoreTeleportFar(false);
        // and teleport the player to a valid place
        GetPlayer()->TeleportToHomebind();
        return;
    }

    // get the destination map entry, not the current one, this will fix homebind and reset greeting
    MapEntry const* mEntry = sMapStore.LookupEntry(loc.mapid);

    Map* map = NULL;

    // prevent crash at attempt landing to not existed battleground instance
    if(mEntry->IsBattleGroundOrArena())
    {
        if (GetPlayer()->GetBattleGroundId())
            map = sMapMgr.FindMap(loc.mapid, GetPlayer()->GetBattleGroundId());

        if (!map)
        {
            DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s was teleported far to nonexisten battleground instance "
                " (map:%u, x:%f, y:%f, z:%f) Trying to port him to his previous place..",
                GetPlayer()->GetGuidStr().c_str(), loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);

            GetPlayer()->SetSemaphoreTeleportFar(false);

            // Teleport to previous place, if cannot be ported back TP to homebind place
            if (!GetPlayer()->TeleportTo(old_loc))
            {
                DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s cannot be ported to his previous place, teleporting him to his homebind place...",
                    GetPlayer()->GetGuidStr().c_str());
                GetPlayer()->TeleportToHomebind();
            }
            return;
        }
    }

    InstanceTemplate const* mInstance = ObjectMgr::GetInstanceTemplate(loc.mapid);

    // reset instance validity, except if going to an instance inside an instance
    if (GetPlayer()->m_InstanceValid == false && !mInstance)
        GetPlayer()->m_InstanceValid = true;

    GetPlayer()->SetSemaphoreTeleportFar(false);

    // relocate the player to the teleport destination
    if (!map)
        map = sMapMgr.CreateMap(loc.mapid, GetPlayer());

    if (!map)
    {
        DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: cannot create requested map %u for teleport!",loc.mapid);
        GetPlayer()->SetSemaphoreTeleportFar(false);
        GetPlayer()->TeleportToHomebind();
        return;
    }

    GetPlayer()->SetMap(map);
    GetPlayer()->Relocate(loc.coord_x, loc.coord_y, loc.coord_z, loc.orientation);

    GetPlayer()->SendInitialPacketsBeforeAddToMap();
    // the CanEnter checks are done in TeleporTo but conditions may change
    // while the player is in transit, for example the map may get full
    if (!map->Add(GetPlayer()))
    {
        // if player wasn't added to map, reset his map pointer!
        GetPlayer()->ResetMap();

        DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s was teleported far but couldn't be added to map "
            " (map:%u, x:%f, y:%f, z:%f) Trying to port him to his previous place..",
            GetPlayer()->GetGuidStr().c_str(), loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);

        // Teleport to previous place, if cannot be ported back TP to homebind place
        if (!GetPlayer()->TeleportTo(old_loc))
        {
            DETAIL_LOG("WorldSession::HandleMoveWorldportAckOpcode: %s cannot be ported to his previous place, teleporting him to his homebind place...",
                GetPlayer()->GetGuidStr().c_str());
            GetPlayer()->TeleportToHomebind();
        }
        return;
    }

    // battleground state prepare (in case join to BG), at relogin/tele player not invited
    // only add to bg group and object, if the player was invited (else he entered through command)
    if(_player->InBattleGround())
    {
        // cleanup setting if outdated
        if(!mEntry->IsBattleGroundOrArena())
        {
            // We're not in BG
            _player->SetBattleGroundId(0, BATTLEGROUND_TYPE_NONE);
            // reset destination bg team
            _player->SetBGTeam(TEAM_NONE);
        }
        // join to bg case
        else if(BattleGround *bg = _player->GetBattleGround())
        {
            if(_player->IsInvitedForBattleGroundInstance(_player->GetBattleGroundId()))
                bg->AddPlayer(_player);
        }
    }

    GetPlayer()->SendInitialPacketsAfterAddToMap();

    // flight fast teleport case
    if(GetPlayer()->GetMotionMaster()->GetCurrentMovementGeneratorType() == FLIGHT_MOTION_TYPE)
    {
        if(!_player->InBattleGround())
        {
            // short preparations to continue flight
            FlightPathMovementGenerator* flight = (FlightPathMovementGenerator*)(GetPlayer()->GetMotionMaster()->top());
            flight->Reset(*GetPlayer());
            return;
        }

        // battleground state prepare, stop flight
        GetPlayer()->GetMotionMaster()->MovementExpired();
        GetPlayer()->m_taxi.ClearTaxiDestinations();
    }

    if (mInstance)
    {
        Difficulty diff = GetPlayer()->GetDifficulty(mEntry->IsRaid());
        if(MapDifficultyEntry const* mapDiff = GetMapDifficultyData(mEntry->MapID,diff))
        {
            if (mapDiff->resetTime)
            {
                if (time_t timeReset = sMapPersistentStateMgr.GetScheduler().GetResetTimeFor(mEntry->MapID,diff))
                {
                    uint32 timeleft = uint32(timeReset - time(NULL));
                    GetPlayer()->SendInstanceResetWarning(mEntry->MapID, diff, timeleft);
                }
            }
        }
    }

    // mount allow check
    if (!mEntry->IsMountAllowed())
    {
        _player->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        _player->RemoveSpellsCausingAura(SPELL_AURA_FLY);
    }
    else
    {
        // recheck mount capabilities at far teleport
        Unit::AuraList const& mMountAuras = _player->GetAurasByType(SPELL_AURA_MOUNTED);
        for (Unit::AuraList::const_iterator itr = mMountAuras.begin(); itr != mMountAuras.end(); )
        {
            Aura const* aura = *itr;

            // mount is no longer suitable
            MountCapabilityEntry const* entry = _player->GetMountCapability(aura->GetSpellEffect()->EffectMiscValueB);
            if (!entry)
            {
                _player->RemoveAurasDueToSpell(aura->GetId());
                itr = mMountAuras.begin();
                continue;
            }

            // mount capability changed
            if (entry->Id != aura->GetModifier()->m_amount)
            {
                if (MountCapabilityEntry const* oldEntry = sMountCapabilityStore.LookupEntry(aura->GetModifier()->m_amount))
                    _player->RemoveAurasDueToSpell(oldEntry->SpeedModSpell);

                _player->CastSpell(_player, entry->SpeedModSpell, true);

                const_cast<Aura*>(aura)->ChangeAmount(entry->Id);
            }

            ++itr;
        }

        uint32 zone, area;
        _player->GetZoneAndAreaId(zone, area);
        // recheck fly auras
        Unit::AuraList const& mFlyAuras = _player->GetAurasByType(SPELL_AURA_FLY);
        for (Unit::AuraList::const_iterator itr = mFlyAuras.begin(); itr != mFlyAuras.end(); )
        {
            Aura const* aura = *itr;
            if (!_player->CanStartFlyInArea(_player->GetMapId(), zone, area))
            {
                _player->RemoveAurasDueToSpell(aura->GetId());
                itr = mFlyAuras.begin();
                continue;
            }

            ++itr;
        }
    }

    // honorless target
    if(GetPlayer()->pvpInfo.inHostileArea)
        GetPlayer()->CastSpell(GetPlayer(), 2479, true);

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMoveTeleportAckOpcode(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_MOVE_TELEPORT_ACK");

    ObjectGuid guid;
    uint32 counter, time;
    recv_data >> counter >> time;

    recv_data.ReadGuidMask<5, 0, 1, 6, 3, 7, 2, 4>(guid);
    recv_data.ReadGuidBytes<4, 2, 7, 6, 5, 1, 3, 0>(guid);

    DEBUG_LOG("Guid: %s", guid.GetString().c_str());
    DEBUG_LOG("Counter %u, time %u", counter, time/IN_MILLISECONDS);

    Unit *mover = _player->GetMover();
    Player *plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;

    if(!plMover || !plMover->IsBeingTeleportedNear())
        return;

    if(guid != plMover->GetObjectGuid())
        return;

    plMover->SetSemaphoreTeleportNear(false);

    uint32 old_zone = plMover->GetZoneId();

    WorldLocation const& dest = plMover->GetTeleportDest();

    plMover->SetPosition(dest.coord_x, dest.coord_y, dest.coord_z, dest.orientation, true);

    uint32 newzone, newarea;
    plMover->GetZoneAndAreaId(newzone, newarea);
    plMover->UpdateZone(newzone, newarea);

    // new zone
    if(old_zone != newzone)
    {
        // honorless target
        if(plMover->pvpInfo.inHostileArea)
            plMover->CastSpell(plMover, 2479, true);
    }

    // resummon pet
    GetPlayer()->ResummonPetTemporaryUnSummonedIfAny();

    //lets process all delayed operations on successful teleport
    GetPlayer()->ProcessDelayedOperations();
}

void WorldSession::HandleMovementOpcodes( WorldPacket & recv_data )
{
    Unit *mover = _player->GetMover();

    Opcodes opcode = recv_data.GetOpcode();
    if (!sLog.HasLogFilter(LOG_FILTER_PLAYER_MOVES))
    {
        DEBUG_LOG("WORLD: Received opcode %s (%u, 0x%X)", LookupOpcodeName(opcode), opcode, opcode);
        recv_data.hexlike();
    }

    Player *plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if(plMover && plMover->IsBeingTeleported())
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    /* extract packet */
    MovementInfo movementInfo;
    recv_data >> movementInfo;

    DEBUG_LOG("Guid: %s MoveFlags: %X, MoveFlags2: %X", movementInfo.GetGuid().GetString().c_str(), movementInfo.GetMovementFlags(), movementInfo.GetMovementFlags2());

    if (!VerifyMovementInfo(movementInfo, movementInfo.GetGuid()))
        return;

    if (mover && _player->GetObjectGuid() != mover->GetObjectGuid())
    {
        if (opcode == CMSG_MOVE_SET_WALK_MODE)
            movementInfo.RemoveMovementFlag(MOVEFLAG_WALK_MODE);
    }

    if (!movementInfo.GetTransportGuid().IsEmpty())
    {
        if (movementInfo.GetPos()->x != movementInfo.GetTransportPos()->x)
            if (GetPlayer()->GetTransport())
                GetPlayer()->GetTransport()->UpdateMovementInfo(&movementInfo);
    }

    // fall damage generation (ignore in flight case that can be triggered also at lags in moment teleportation to another map).
    if (opcode == CMSG_MOVE_FALL_LAND && plMover && !plMover->IsTaxiFlying())
        plMover->HandleFall(movementInfo);

    /* process anticheat check */
    GetPlayer()->GetAntiCheat()->DoAntiCheatCheck(CHECK_MOVEMENT,movementInfo, opcode);

    /* process position-change */
    HandleMoverRelocation(movementInfo);

    if (plMover)
        plMover->UpdateFallInformationIfNeed(movementInfo, opcode);

    // sometimes there is a desync and it should be done here
    if (plMover && opcode == MSG_MOVE_HEARTBEAT && plMover->isMoving())
        plMover->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_NOT_SEATED);

    WorldPacket data(SMSG_PLAYER_MOVE, recv_data.size());
    data << movementInfo;

    mover->SendMessageToSetExcept(&data, _player);

    if (opcode ==  CMSG_MOVE_JUMP && plMover)
        plMover->ProcDamageAndSpell(plMover, PROC_FLAG_NONE, PROC_FLAG_ON_JUMP, PROC_EX_NONE, 0);

    //if (mover && _player->GetObjectGuid() != mover->GetObjectGuid())
    //    if (opcode == CMSG_MOVE_SET_WALK_MODE)
    //        _player->SendDirectMessage(&data);
}

void WorldSession::HandleForceSpeedChangeAckOpcodes(WorldPacket &recv_data)
{
    Opcodes opcode = recv_data.GetOpcode();
    DEBUG_LOG("WORLD: Received %s (%u, 0x%X) opcode", recv_data.GetOpcodeName(), opcode, opcode);

    /* extract packet */
    ObjectGuid guid;
    MovementInfo movementInfo;
    float  newspeed;

    recv_data >> guid.ReadAsPacked();
    recv_data >> Unused<uint32>();                          // counter or moveEvent
    recv_data >> movementInfo;
    recv_data >> newspeed;

    // now can skip not our packet
    if(_player->GetObjectGuid() != guid)
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }
    /*----------------*/

    // client ACK send one packet for mounted/run case and need skip all except last from its
    // in other cases anti-cheat check can be fail in false case
    UnitMoveType move_type;
    UnitMoveType force_move_type;

    static char const* move_type_name[MAX_MOVE_TYPE] = {  "Walk", "Run", "RunBack", "Swim", "SwimBack", "TurnRate", "Flight", "FlightBack", "PitchRate" };

    switch(opcode)
    {
        case CMSG_FORCE_WALK_SPEED_CHANGE_ACK:          move_type = MOVE_WALK;          force_move_type = MOVE_WALK;        break;
        case CMSG_FORCE_RUN_SPEED_CHANGE_ACK:           move_type = MOVE_RUN;           force_move_type = MOVE_RUN;         break;
        case CMSG_FORCE_RUN_BACK_SPEED_CHANGE_ACK:      move_type = MOVE_RUN_BACK;      force_move_type = MOVE_RUN_BACK;    break;
        case CMSG_FORCE_SWIM_SPEED_CHANGE_ACK:          move_type = MOVE_SWIM;          force_move_type = MOVE_SWIM;        break;
        case CMSG_FORCE_SWIM_BACK_SPEED_CHANGE_ACK:     move_type = MOVE_SWIM_BACK;     force_move_type = MOVE_SWIM_BACK;   break;
        case CMSG_FORCE_TURN_RATE_CHANGE_ACK:           move_type = MOVE_TURN_RATE;     force_move_type = MOVE_TURN_RATE;   break;
        case CMSG_FORCE_FLIGHT_SPEED_CHANGE_ACK:        move_type = MOVE_FLIGHT;        force_move_type = MOVE_FLIGHT;      break;
        case CMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE_ACK:   move_type = MOVE_FLIGHT_BACK;   force_move_type = MOVE_FLIGHT_BACK; break;
        case CMSG_FORCE_PITCH_RATE_CHANGE_ACK:          move_type = MOVE_PITCH_RATE;    force_move_type = MOVE_PITCH_RATE;  break;
        default:
            ERROR_LOG("WorldSession::HandleForceSpeedChangeAck: Unknown move type opcode: %u", opcode);
            return;
    }

    // skip all forced speed changes except last and unexpected
    // in run/mounted case used one ACK and it must be skipped.m_forced_speed_changes[MOVE_RUN} store both.
    if(_player->m_forced_speed_changes[force_move_type] > 0)
    {
        --_player->m_forced_speed_changes[force_move_type];
        if(_player->m_forced_speed_changes[force_move_type] > 0)
            return;
    }

    if (!_player->GetTransport() && fabs(_player->GetSpeed(move_type) - newspeed) > 0.01f)
    {
        if(_player->GetSpeed(move_type) > newspeed)         // must be greater - just correct
        {
            ERROR_LOG("%sSpeedChange player %s is NOT correct (must be %f instead %f), force set to correct value",
                move_type_name[move_type], _player->GetName(), _player->GetSpeed(move_type), newspeed);
            _player->SetSpeedRate(move_type,_player->GetSpeedRate(move_type),true);
        }
        else                                                // must be lesser - cheating
        {
            BASIC_LOG("Player %s from account id %u kicked for incorrect speed (must be %f instead %f)",
                _player->GetName(),_player->GetSession()->GetAccountId(),_player->GetSpeed(move_type), newspeed);
            _player->GetSession()->KickPlayer();
        }
    }
}

void WorldSession::HandleSetActiveMoverOpcode(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_SET_ACTIVE_MOVER");
    recv_data.hexlike();

    ObjectGuid guid;
    recv_data.ReadGuidMask<7, 2, 1, 0, 4, 5, 6, 3>(guid);
    recv_data.ReadGuidBytes<3, 2, 4, 0, 5, 1, 6, 7>(guid);

    if (!_player->IsInWorld())
        return;

    if (Unit *mover = ObjectAccessor::GetUnit(*GetPlayer(), guid))
        GetPlayer()->SetMover(mover);
    else
    {
        ERROR_LOG("HandleSetActiveMoverOpcode: incorrect mover guid: mover is %s and should be %s",
            _player->GetMover()->GetGuidStr().c_str(), guid.GetString().c_str());

        GetPlayer()->SetMover(GetPlayer());
    }
}

void WorldSession::HandleMoveNotActiveMoverOpcode(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_MOVE_NOT_ACTIVE_MOVER");
    recv_data.hexlike();

    MovementInfo mi;
    recv_data >> mi;

    _player->m_movementInfo = mi;
}

void WorldSession::HandleDismissControlledVehicle(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_DISMISS_CONTROLLED_VEHICLE");
    recv_data.hexlike();

    MovementInfo mi;
    recv_data >> mi;

    if (!_player->GetVehicle())
        return;

    bool dismiss = true;

    Creature* vehicle = _player->GetMap()->GetAnyTypeCreature(mi.GetGuid());
    if (!vehicle || !vehicle->GetVehicleKit() || !vehicle->GetVehicleKit()->GetVehicleInfo())
        return;

    if (vehicle->GetVehicleKit()->GetVehicleInfo()->m_flags & (VEHICLE_FLAG_NOT_DISMISS | VEHICLE_FLAG_ACCESSORY))
        dismiss = false;

    // Client freezes...
    if (vehicle->GetEntry() == 34812 || vehicle->GetEntry() == 34819 || vehicle->GetEntry() == 34822 ||
        vehicle->GetEntry() == 34823 || vehicle->GetEntry() == 34824)
        dismiss = false;

    GetPlayer()->m_movementInfo = mi;
    GetPlayer()->ExitVehicle();

    if (dismiss)
        vehicle->ForcedDespawn();
}

void WorldSession::HandleRequestVehicleExit(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_REQUEST_VEHICLE_EXIT");

    _player->ExitVehicle();
}

void WorldSession::HandleRequestVehiclePrevSeat(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_REQUEST_VEHICLE_PREV_SEAT");

    GetPlayer()->ChangeSeat(-1, false);
}

void WorldSession::HandleRequestVehicleNextSeat(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_REQUEST_VEHICLE_NEXT_SEAT");

    GetPlayer()->ChangeSeat(-1, true);
}

void WorldSession::HandleRequestVehicleSwitchSeat(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_REQUEST_VEHICLE_SWITCH_SEAT");
    recv_data.hexlike();

    ObjectGuid guid;
    recv_data >> guid.ReadAsPacked();

    int8 seatId;
    recv_data >> seatId;

    VehicleKit* pVehicle = _player->GetVehicle();

    if (!pVehicle)
        return;

    if (pVehicle->GetVehicleInfo()->m_flags & VEHICLE_FLAG_DISABLE_SWITCH)
        return;

    if (pVehicle->GetBase()->GetObjectGuid() == guid)
        _player->ChangeSeat(seatId);
    else if (Unit* Vehicle2 = _player->GetMap()->GetUnit(guid))
    {
        if (VehicleKit *pVehicle2 = Vehicle2->GetVehicleKit())
            if (pVehicle2->HasEmptySeat(seatId))
            {
                _player->ExitVehicle();
                _player->EnterVehicle(pVehicle2, seatId);
            }
    }
}

void WorldSession::HandleEnterPlayerVehicle(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_PLAYER_VEHICLE_ENTER");
    recv_data.hexlike();

    ObjectGuid guid;
    recv_data >> guid;

    Player* player = sObjectMgr.GetPlayer(guid);

    if (!player)
        return;

    if (!_player->IsInSameRaidWith(player))
        return;

    if (!_player->IsWithinDistInMap(player, INTERACTION_DISTANCE))
        return;

    if (BattleGround const *bg = GetPlayer()->GetBattleGround())
        if (bg->isArena())
            return;

    if (VehicleKit* pVehicle = player->GetVehicleKit())
        _player->EnterVehicle(pVehicle);
}

void WorldSession::HandleEjectPassenger(WorldPacket &recv_data)
{
    DEBUG_LOG("WORLD: Received CMSG_EJECT_PASSENGER");
    recv_data.hexlike();

    ObjectGuid guid;
    recv_data >> guid;

    Unit* passenger = ObjectAccessor::GetUnit(*GetPlayer(), guid);

    if (!passenger)
        return;

    if (!passenger->GetVehicle() || passenger->GetVehicle() != GetPlayer()->GetVehicleKit())
        return;

    passenger->ExitVehicle();

    // eject and remove creatures of player mounts
    if (passenger->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)passenger)->IsTemporarySummon())
        {
            // Fixme: delay must be calculated not from this, but from creature template parameters (off traders ...?).
            //uint32 delay = passenger->GetObjectGuid().IsVehicle() ? 1000 : 60000;
            ((TemporarySummon*)passenger)->UnSummon(1000);
        }
        else
            passenger->AddObjectToRemoveList();
    }
}

void WorldSession::HandleChangeSeatsOnControlledVehicle(WorldPacket &recv_data)
{
    sLog.outDebug("WORLD: Recvd CMSG_CHANGE_SEATS_ON_CONTROLLED_VEHICLE");
    recv_data.hexlike();

    ObjectGuid guid, guid2;
    MovementInfo mi;
    int8 seatId;

    recv_data >> mi;
    guid = mi.GetGuid();
    guid2 = mi.GetGuid2();
    seatId = mi.GetByteParam();

    sLog.outDebug("WORLD: Guid: %s Guid2: %s seatId: %i", guid.GetString().c_str(), guid2.GetString().c_str(), seatId);

    VehicleKit* pVehicle = _player->GetVehicle();

    if (!pVehicle)
        return;

    if (pVehicle->GetVehicleInfo()->m_flags & VEHICLE_FLAG_DISABLE_SWITCH)
        return;

    _player->m_movementInfo = mi;

    if (guid.GetRawValue() == guid2.GetRawValue())
        _player->ChangeSeat(seatId, false);
    else if (guid2.IsVehicle())
    {
        if (Creature* vehicle = GetPlayer()->GetMap()->GetAnyTypeCreature(guid2))
        {
            if (VehicleKit* pVehicle2 = vehicle->GetVehicleKit())
                if(pVehicle2->HasEmptySeat(seatId))
                {
                    _player->ExitVehicle();
                    _player->EnterVehicle(pVehicle2, seatId);
                }
        }
    }
}

void WorldSession::HandleMountSpecialAnimOpcode(WorldPacket& /*recvdata*/)
{
    // DEBUG_LOG("WORLD: Received opcode CMSG_MOUNTSPECIAL_ANIM");

    WorldPacket data(SMSG_MOUNTSPECIAL_ANIM, 8);
    data << GetPlayer()->GetObjectGuid();

    GetPlayer()->SendMessageToSet(&data, false);
}

void WorldSession::HandleMoveKnockBackAck( WorldPacket & recv_data )
{
    DEBUG_LOG("CMSG_MOVE_KNOCK_BACK_ACK");

    Unit *mover = _player->GetMover();
    Player *plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;

    // ignore, waiting processing in WorldSession::HandleMoveWorldportAckOpcode and WorldSession::HandleMoveTeleportAck
    if(plMover && plMover->IsBeingTeleported())
    {
        recv_data.rpos(recv_data.wpos());                   // prevent warnings spam
        return;
    }

    MovementInfo movementInfo;
    recv_data >> movementInfo;

    if (!VerifyMovementInfo(movementInfo, movementInfo.GetGuid()))
        return;

    HandleMoverRelocation(movementInfo);

    WorldPacket data(SMSG_MOVE_UPDATE_KNOCK_BACK, recv_data.size() + 15);
    data << movementInfo;
    mover->SendMessageToSetExcept(&data, _player);
}

void WorldSession::HandleMoveHoverAck( WorldPacket& recv_data )
{
    DEBUG_LOG("CMSG_MOVE_HOVER_ACK");

    recv_data.rfinish();
    /*
    MovementInfo movementInfo;
    recv_data >> movementInfo;
    */
}

void WorldSession::HandleMoveWaterWalkAck(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_MOVE_WATER_WALK_ACK");

    recv_data.rfinish();

    /*
    MovementInfo movementInfo;
    recv_data >> movementInfo;
    */
}

void WorldSession::HandleSummonResponseOpcode(WorldPacket& recv_data)
{
    if (!_player->isAlive() || _player->isInCombat())
        return;

    ObjectGuid summonerGuid;
    bool agree;
    recv_data >> summonerGuid;
    recv_data >> agree;

    _player->SummonIfPossible(agree);
}

bool WorldSession::VerifyMovementInfo(MovementInfo const& movementInfo, ObjectGuid const& guid) const
{
    Unit* mover = _player->GetMover();
    // ignore wrong guid (player attempt cheating own session for not own guid possible...)
    if (guid != mover->GetObjectGuid())
        return false;

    if (!MaNGOS::IsValidMapCoord(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z, movementInfo.GetPos()->o))
        return false;

    if (movementInfo.GetTransportGuid())
    {
        float trans_rad = sqrt(movementInfo.GetTransportPos()->x * movementInfo.GetTransportPos()->x + movementInfo.GetTransportPos()->y * movementInfo.GetTransportPos()->y + movementInfo.GetTransportPos()->z * movementInfo.GetTransportPos()->z);
        if (trans_rad > 300.0f)
            return false;

        if( !MaNGOS::IsValidMapCoord(movementInfo.GetPos()->x + movementInfo.GetTransportPos()->x, movementInfo.GetPos()->y + movementInfo.GetTransportPos()->y,
            movementInfo.GetPos()->z + movementInfo.GetTransportPos()->z, movementInfo.GetPos()->o + movementInfo.GetTransportPos()->o) )
        {
            return false;
        }
    }

    // Ranger: crashfix?
    if (!GetPlayer()->GetMap())
    {
        sLog.outError("WorldSession::HandleMovementOpcodes: player %s (%d) - m_currMap is not available. Crash NOW?", GetPlayer()->GetName(), GetPlayer()->GetGUIDLow());
        // teleport the player home
        GetPlayer()->TeleportToHomebind();
        return false;
    }

    return true;
}

void WorldSession::HandleMoverRelocation(MovementInfo& movementInfo)
{
    Unit *mover = _player->GetMover();
    Player* plMover = mover->GetTypeId() == TYPEID_PLAYER ? (Player*)mover : NULL;

    if (plMover)
    {
        if (movementInfo.GetTransportGuid())
        {
            // if we boarded a transport, add us to it
            if (!plMover->GetTransport())
            {
                // amaru: wtf here?
                /* process anticheat check */
                GetPlayer()->GetAntiCheat()->DoAntiCheatCheck(CHECK_TRANSPORT,movementInfo);

                // elevators also cause the client to send transport guid - just unmount if the guid can be found in the transport list
                for (MapManager::TransportSet::const_iterator iter = sMapMgr.m_Transports.begin(); iter != sMapMgr.m_Transports.end(); ++iter)
                {
                    Transport* transport = *iter;
                    if (transport->GetObjectGuid() == movementInfo.GetTransportGuid())
                    {
                        plMover->SetTransport(transport);
                        transport->AddPassenger(plMover->GetObjectGuid());
                        break;
                    }
                }
            }
        }
        else if (plMover->GetTransport())               // if we were on a transport, leave
        {
            plMover->GetTransport()->RemovePassenger(plMover->GetObjectGuid());
            plMover->SetTransport(NULL);
            movementInfo.ClearTransportData();
        }

        if (movementInfo.HasMovementFlag(MOVEFLAG_SWIMMING) != plMover->IsInWater())
        {
            // now client not include swimming flag in case jumping under water
            plMover->SetInWater( !plMover->IsInWater() || plMover->GetTerrain()->IsUnderWater(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z) );
        }
    }

    movementInfo.UpdateTime(WorldTimer::getMSTime());
    mover->m_movementInfo = movementInfo;
    mover->SetPosition(movementInfo.GetPos()->x, movementInfo.GetPos()->y, movementInfo.GetPos()->z, movementInfo.GetPos()->o);

    if (plMover)
    {
        // clear unit emote state
        plMover->HandleEmote(EMOTE_ONESHOT_NONE);

        AreaTableEntry const* zone = GetAreaEntryByAreaID(plMover->GetAreaId());

        if(movementInfo.GetPos()->z < (zone ? zone->MaxDepth : -500.0f))
        {
            if (plMover->GetBattleGround()
                    && plMover->GetBattleGround()->HandlePlayerUnderMap(_player))
            {
                // do nothing, the handle already did if returned true
            }
            else
            {
                // NOTE: this is actually called many times while falling
                // even after the player has been teleported away
                // TODO: discard movement packets after the player is rooted
                if(plMover->isAlive())
                {
                    plMover->EnvironmentalDamage(DAMAGE_FALL_TO_VOID, plMover->GetMaxHealth());
                    // pl can be alive if GM/etc
                    if(!plMover->isAlive())
                    {
                        // change the death state to CORPSE to prevent the death timer from
                        // starting in the next player update
                        plMover->KillPlayer();
                        plMover->BuildPlayerRepop();
                    }
                }

                // cancel the death timer here if started
                plMover->RepopAtGraveyard();
            }
        }
    }
}
