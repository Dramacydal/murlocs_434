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
#include "Log.h"
#include "ObjectMgr.h"
#include "Vehicle.h"
#include "TemporarySummon.h"
#include "Unit.h"
#include "CreatureAI.h"
#include "Util.h"
#include "WorldPacket.h"

//VehicleInfo::VehicleInfo(VehicleEntry const* entry) :
//    m_vehicleEntry(entry)
//{
//}

VehicleKit::VehicleKit(Unit* base, VehicleEntry const* vehicleInfo) : m_vehicleInfo(vehicleInfo), m_pBase(base), m_uiNumFreeSeats(0)
{
    for (uint32 i = 0; i < MAX_VEHICLE_SEAT; ++i)
    {
        uint32 seatId = m_vehicleInfo->m_seatID[i];

        if (!seatId)
            continue;

        if (VehicleSeatEntry const *veSeat = sVehicleSeatStore.LookupEntry(seatId))
        {
            m_Seats.insert(std::make_pair(i, VehicleSeat(veSeat)));

            if (veSeat->IsUsable())
                ++m_uiNumFreeSeats;
        }
    }

    if (m_pBase)
    {
        if (m_vehicleInfo->m_flags & VEHICLE_FLAG_NO_STRAFE)
            m_pBase->m_movementInfo.AddMovementFlag2(MOVEFLAG2_NO_STRAFE);

        if (m_vehicleInfo->m_flags & VEHICLE_FLAG_NO_JUMPING)
            m_pBase->m_movementInfo.AddMovementFlag2(MOVEFLAG2_NO_JUMPING);

        if (m_vehicleInfo->m_flags & VEHICLE_FLAG_FULLSPEEDTURNING)
            m_pBase->m_movementInfo.AddMovementFlag2(MOVEFLAG2_FULLSPEEDTURNING);

        if (m_vehicleInfo->m_flags & VEHICLE_FLAG_ALLOW_PITCHING)
            m_pBase->m_movementInfo.AddMovementFlag2(MOVEFLAG2_ALLOW_PITCHING);

        if (m_vehicleInfo->m_flags & VEHICLE_FLAG_FULLSPEEDPITCHING)
            m_pBase->m_movementInfo.AddMovementFlag2(MOVEFLAG2_FULLSPEEDPITCHING);
    }

    if (m_pBase->GetTypeId() == TYPEID_UNIT)
    {
        Creature* creature = (Creature*)m_pBase;
        switch (m_vehicleInfo->m_powerType)
        {
            case ENERGY_TYPE_STEAM:
            case ENERGY_TYPE_HEAT:
            case ENERGY_TYPE_BLOOD:
            case ENERGY_TYPE_OOZE:
            case ENERGY_TYPE_WRATH:
                creature->setPowerType(POWER_ENERGY);
                creature->SetMaxPower(POWER_ENERGY, 100);
                break;
            case ENERGY_TYPE_PYRITE:
                creature->setPowerType(POWER_ENERGY);
                creature->SetMaxPower(POWER_ENERGY, 50);
                break;
            default:
                for (uint32 i = 0; i < CREATURE_MAX_SPELLS; ++i)
                {
                    if (!creature->m_spells[i])
                        continue;

                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(creature->m_spells[i]);
                    if (!spellInfo)
                        continue;

                    if (spellInfo->powerType == POWER_ENERGY)
                    {
                        creature->setPowerType(POWER_ENERGY);
                        creature->SetMaxPower(POWER_ENERGY, 100);
                        break;
                    }
                }
                break;
        }
    }
}

VehicleKit::~VehicleKit()
{
}

void VehicleKit::RemoveAllPassengers()
{
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (Unit *passenger = itr->second.passenger)
        {
            passenger->ExitVehicle();

            // remove creatures of player mounts
            if (passenger->GetTypeId() == TYPEID_UNIT)
                if (m_pBase->GetTypeId() == TYPEID_PLAYER || itr->second.minionSeat)
                {
                    if (((Creature*)passenger)->IsTemporarySummon())
                        ((TemporarySummon*)passenger)->UnSummon();
                    else
                        passenger->AddObjectToRemoveList();
                }
        }
    }
}

bool VehicleKit::HasEmptySeat(int8 seatId) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);

    if (seat == m_Seats.end())
        return false;

    return !seat->second.passenger;
}

Unit *VehicleKit::GetPassenger(int8 seatId) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);

    if (seat == m_Seats.end())
        return NULL;

    return seat->second.passenger;
}

int8 VehicleKit::GetNextEmptySeat(int8 seatId, bool next) const
{
    SeatMap::const_iterator seat = m_Seats.find(seatId);

    if (seat == m_Seats.end())
        return -1;

    while (seat->second.passenger || !seat->second.seatInfo->IsUsable())
    {
        if (next)
        {
            ++seat;
            if (seat == m_Seats.end())
                seat = m_Seats.begin();
        }
        else
        {
            if (seat == m_Seats.begin())
                seat = m_Seats.end();
            --seat;
        }

        if (seat->first == seatId)
            return -1; // no available seat
    }

    return seat->first;
}

bool VehicleKit::AddPassenger(Unit *passenger, int8 seatId)
{
    SeatMap::iterator seat;

    if (seatId < 0) // no specific seat requirement
    {
        for (seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
            if (!seat->second.passenger && (seat->second.seatInfo->IsUsable() || (seat->second.seatInfo->m_flags & SEAT_FLAG_UNCONTROLLED)))
                break;

        if (seat == m_Seats.end()) // no available seat
        {
            DEBUG_LOG("VehicleKit::AddPassenger: no available seats for %s in %s", passenger->GetGuidStr().c_str(), m_pBase->GetGuidStr().c_str()); 
            return false;
        }
    }
    else
    {
        seat = m_Seats.find(seatId);

        if (seat == m_Seats.end())
            return false;

        if (seat->second.passenger)
            return false;
    }

    VehicleSeatEntry const* seatInfo = seat->second.seatInfo;
    seat->second.passenger = passenger;

    if (!(seat->second.seatInfo->m_flags & SEAT_FLAG_FREE_ACTION))
        passenger->addUnitState(UNIT_STAT_ON_VEHICLE);

    m_pBase->SetPhaseMask(passenger->GetPhaseMask(), true);

    passenger->m_movementInfo.SetTransportData(m_pBase->GetObjectGuid(),
        seatInfo->m_attachmentOffsetX, seatInfo->m_attachmentOffsetY, seatInfo->m_attachmentOffsetZ,
        seatInfo->m_passengerYaw, WorldTimer::getMSTime(), seat->first, seatInfo);

    if (passenger->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)passenger)->UnsummonPetTemporaryIfAny();
        ((Player*)passenger)->GetCamera().SetView(m_pBase);

        ((Player*)passenger)->SetGravity(false);
        passenger->SetRoot(true);
    }
    else if (passenger->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)passenger)->AI())
            ((Creature*)passenger)->AI()->SetCombatMovement(false);
    }

    //if (seat->second.seatInfo->m_flags & SEAT_FLAG_UNATTACKABLE || seat->second.seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
    //{
    //    passenger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    //    //passenger->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT);
    //}

    switch(m_pBase->GetEntry())
    {
        case 27881:     // Wintergrasp Catapult
        case 27894:     // Antipersonnel Cannon
        case 32795:
        case 28094:     // Wintergrasp Demolisher
        case 28312:     // Wintergrasp Siege Engine
        case 28319:     // Wintergrasp Siege Turret
        case 28781:     // Sota Demolisher
        case 32796:
        case 28366:     // Wintergrasp Tower Canon
        case 28817:     // Mine Car
        case 28833:     // Scarlet Cannon
        case 32627:     // Wintergrasp Siege Engine
        case 32629:     // Wintergrasp Siege Turret
        case 33062:     // Salvaged Chopper
        case 33109:     // Salvaged Demolisher
        case 45344:     // Abandoned Siege Engine
            passenger->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
        default:
            break;
    }

    if ((seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL) && m_pBase->GetTypeId() == TYPEID_UNIT)
    {
        if (!(GetVehicleInfo()->m_flags & VEHICLE_FLAG_ACCESSORY))
        {
            //m_pBase->StopMoving();
            m_pBase->GetMotionMaster()->Clear();
            m_pBase->CombatStop(true);
        }

        m_pBase->DeleteThreatList();
        m_pBase->getHostileRefManager().deleteReferences();
        m_pBase->SetCharmerGuid(passenger->GetObjectGuid());
        m_pBase->addUnitState(UNIT_STAT_CONTROLLED);

        passenger->SetCharm(m_pBase);

        if (m_pBase->HasAuraType(SPELL_AURA_FLY) || m_pBase->HasAuraType(SPELL_AURA_MOD_FLIGHT_SPEED))
        {
            WorldPacket data;
            m_pBase->BuildMoveSetCanFlyPacket(&data, true, 0);
            m_pBase->SendMessageToSet(&data,false);
        }

        if (passenger->GetTypeId() == TYPEID_PLAYER)
        {
            m_pBase->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
            m_pBase->setFaction(passenger->getFaction());

            if (CharmInfo* charmInfo = m_pBase->InitCharmInfo(m_pBase))
            {
                charmInfo->SetState(CHARM_STATE_ACTION, ACTIONS_DISABLE);
                charmInfo->InitVehicleCreateSpells();
            }

            Player* player = (Player*)passenger;
            player->SetMover(m_pBase);
            player->SetClientControl(m_pBase, 1);
            player->VehicleSpellInitialize();
        }

        if (!(((Creature*)m_pBase)->GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_KEEP_AI))
            ((Creature*)m_pBase)->AIM_Initialize();

        /*if (m_pBase->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE))
        {
            WorldPacket data2;
            m_pBase->BuildForceMoveRootPacket(&data2, true, 2);
            m_pBase->SendMessageToSet(&data2,false);
        }
        else */if (passenger->m_movementInfo.GetMovementFlags() & MOVEFLAG_WALK_MODE)
            ((Creature*)m_pBase)->SetWalk(true);
        else
            ((Creature*)m_pBase)->SetWalk(false);
    }
    else if ((seatInfo->m_flags & SEAT_FLAG_FREE_ACTION) || (seatInfo->m_flags & SEAT_FLAG_CAN_ATTACK))
    {
        if (passenger->GetTypeId() == TYPEID_PLAYER)
            ((Player*)passenger)->SetClientControl(m_pBase, 0);
    }

    RelocatePassengers(m_pBase->GetPositionX(), m_pBase->GetPositionY(), m_pBase->GetPositionZ()+0.5f, m_pBase->GetOrientation());
    passenger->SendMonsterMoveVehicle(m_pBase);

    UpdateFreeSeatCount();

    if (m_pBase->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)m_pBase)->AI())
            ((Creature*)m_pBase)->AI()->PassengerBoarded(passenger, seat->first, true);
    }

    return true;
}

void VehicleKit::RemovePassenger(Unit *passenger)
{
    SeatMap::iterator seat;

    for (seat = m_Seats.begin(); seat != m_Seats.end(); ++seat)
        if (seat->second.passenger == passenger)
            break;

    if (seat == m_Seats.end())
        return;

    seat->second.passenger = NULL;
    passenger->clearUnitState(UNIT_STAT_ON_VEHICLE);

    float px, py, pz, po;
    m_pBase->GetClosePoint(px, py, pz, m_pBase->GetObjectBoundingRadius(), 2.0f, M_PI_F);
    po = m_pBase->GetOrientation();

    passenger->m_movementInfo.ClearTransportData();

    //if (seat->second.seatInfo->m_flags & SEAT_FLAG_UNATTACKABLE || seat->second.seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
    //{
    //    passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    //}

    switch(m_pBase->GetEntry())
    {
        case 27881:     // Wintergrasp Catapult
        case 27894:     // Antipersonnel Cannon
        case 32795:
        case 28094:     // Wintergrasp Demolisher
        case 28312:     // Wintergrasp Siege Engine
        case 28319:     // Wintergrasp Siege Turret
        case 28781:     // Sota Demolisher
        case 32796:
        case 28366:     // Wintergrasp Tower Canon
        case 28817:     // Mine Car
        case 28833:     // Scarlet Cannon
        case 32627:     // Wintergrasp Siege Engine
        case 32629:     // Wintergrasp Siege Turret
        case 33062:     // Salvaged Chopper
        case 33109:     // Salvaged Demolisher
        case 45344:     // Abandoned Siege Engine
            passenger->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            break;
        default:
            break;
    }

    if (seat->second.seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL && m_pBase->GetTypeId() == TYPEID_UNIT)
    {
        passenger->SetCharm(NULL);
        passenger->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);

        m_pBase->SetCharmerGuid(ObjectGuid());
        m_pBase->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        m_pBase->clearUnitState(UNIT_STAT_CONTROLLED);
        m_pBase->setFaction(((Creature*)m_pBase)->GetCreatureInfo()->faction_A);

        if (passenger->GetTypeId() == TYPEID_PLAYER)
        {
            Player* player = (Player*)passenger;
            player->SetMover(NULL);
            player->SetClientControl(m_pBase, 0);
            player->RemovePetActionBar();
        }

        if(!(((Creature*)m_pBase)->GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_KEEP_AI))
            ((Creature*)m_pBase)->AIM_Initialize();
    }

    if (passenger->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)passenger)->GetCamera().ResetView();

        ((Player*)passenger)->SetGravity(true);
        passenger->SetRoot(false);

        ((Player*)passenger)->ResummonPetTemporaryUnSummonedIfAny();

        ((Player*)passenger)->SetFallInformation(0, pz + 0.5f);
    }
    else if (passenger->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)passenger)->AI())
            ((Creature*)passenger)->AI()->SetCombatMovement(true, true);
    }

    passenger->UpdateAllowedPositionZ(px, py, pz);
    passenger->SetPosition(px, py, pz + 0.5f, po);
    UpdateFreeSeatCount();

    // only for flyable vehicles
    if (passenger->IsFlying())
        m_pBase->CastSpell(passenger, VEHICLE_SPELL_PARACHUTE, true);

    if (m_pBase->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)m_pBase)->AI())
            ((Creature*)m_pBase)->AI()->PassengerBoarded(passenger, seat->first, false);

        if (!passenger->getVictim())
            passenger->GetMotionMaster()->Initialize();
    }
}

void VehicleKit::Reset()
{
    InstallAllAccessories(m_pBase->GetEntry());
    UpdateFreeSeatCount();
}

void VehicleKit::InstallAllAccessories(uint32 entry)
{
    VehicleAccessoryList const* mVehicleList = sObjectMgr.GetVehicleAccessoryList(entry);
    if (!mVehicleList)
        return;

    for (VehicleAccessoryList::const_iterator itr = mVehicleList->begin(); itr != mVehicleList->end(); ++itr)
        InstallAccessory(itr->uiAccessory, itr->uiSeat, itr->bMinion);
}

void VehicleKit::InstallAccessory( uint32 entry, int8 seatId, bool minion)
{
    if (Unit *passenger = GetPassenger(seatId))
    {
        // already installed
        if (passenger->GetEntry() == entry)
            return;

        passenger->ExitVehicle();
    }

    if (Creature *accessory = m_pBase->SummonCreature(entry, m_pBase->GetPositionX(), m_pBase->GetPositionY(), m_pBase->GetPositionZ(), 0.0f, minion ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
    {
        accessory->SetCreatorGuid(ObjectGuid());
        accessory->EnterVehicle(this, seatId);
        //accessory->SendHeartBeat();

        if (accessory->GetVehicle())
        {
            DEBUG_LOG("Vehicle::InstallAccessory %s accessory added, seat %u of %s", accessory->GetGuidStr().c_str(), seatId, m_pBase->GetObjectGuid().GetString().c_str());
            if (VehicleSeat* seat = GetSeat(seatId))
                seat->minionSeat = minion;
        }
        else
        {
            sLog.outError("Vehicle::InstallAccessory cannot install %s to seat %u of %s", accessory->GetGuidStr().c_str(), seatId, m_pBase->GetObjectGuid().GetString().c_str());
            accessory->ForcedDespawn();
        }
    }
}

void VehicleKit::UpdateFreeSeatCount()
{
    m_uiNumFreeSeats = 0;

    for (SeatMap::const_iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (!itr->second.passenger && itr->second.seatInfo->IsUsable())
            ++m_uiNumFreeSeats;
    }

    uint32 flag = m_pBase->GetTypeId() == TYPEID_PLAYER ? UNIT_NPC_FLAG_PLAYER_VEHICLE : UNIT_NPC_FLAG_SPELLCLICK;

    if (m_uiNumFreeSeats)
        m_pBase->SetFlag(UNIT_NPC_FLAGS, flag);
    else
        m_pBase->RemoveFlag(UNIT_NPC_FLAGS, flag);
}

void VehicleKit::RelocatePassengers(float x, float y, float z, float ang)
{
    for (SeatMap::const_iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (Unit *passenger = itr->second.passenger)
        {
            float px = x + passenger->m_movementInfo.GetTransportPos()->x;
            float py = y + passenger->m_movementInfo.GetTransportPos()->y;
            float pz = z + passenger->m_movementInfo.GetTransportPos()->z;
            float po = ang + passenger->m_movementInfo.GetTransportPos()->o;

            passenger->UpdateAllowedPositionZ(px, py, pz);
            passenger->SetPosition(px, py, pz, po);
        }
    }
}

VehicleSeatEntry const* VehicleKit::GetSeatInfo(Unit* passenger)
 {
    for (SeatMap::iterator itr = m_Seats.begin(); itr != m_Seats.end(); ++itr)
    {
        if (Unit *_passenger = itr->second.passenger)
            if (_passenger = passenger)
                return itr->second.seatInfo;
    }
    return NULL;
}