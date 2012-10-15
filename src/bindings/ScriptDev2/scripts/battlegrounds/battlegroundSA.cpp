/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: battlegroundSA
SD%Complete: 90%
SDCategory: Battleground Strand of the Ancients - vehicles handling.
EndScriptData */

#include "precompiled.h"
#include "BattleGround/BattleGroundSA.h"
#include "Vehicle.h"

struct MANGOS_DLL_DECL npc_sa_demolisherAI : public ScriptedAI
{
    npc_sa_demolisherAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        if (m_creature->GetMap()->IsBattleGround())
        {
            BattleGround* bg = ((BattleGroundMap*)m_creature->GetMap())->GetBG();
            if (bg && bg->GetTypeID(true) == BATTLEGROUND_SA)
            {
                BattleGroundSA* bgsa = (BattleGroundSA*)bg;
                m_creature->setFaction(bgsa->GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->isCharmed())
             m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    bool HandleSpellClick(Unit* caster, Unit* target, uint32 spellId, ObjectGuid casterGuid)
    {
        if (caster->GetTypeId() != TYPEID_PLAYER || !m_creature->GetMap()->IsBattleGround() || ((BattleGroundMap*)m_creature->GetMap())->GetBG()->GetStatus() != STATUS_IN_PROGRESS)
            return true;

        BattleGround* bg = ((BattleGroundMap*)m_creature->GetMap())->GetBG();
        if (bg && bg->GetTypeID(true) == BATTLEGROUND_SA)
        {
            BattleGroundSA* bgsa = (BattleGroundSA*)bg;
            if (bgsa->GetDefender() == ((Player*)caster)->GetTeam())
                return true;
        }

        return false;
    }

    void PassengerBoarded(Unit* unit, int8 seatId, bool apply)
    {
        if (!apply)
        {
            if (VehicleKit* vehicle =  m_creature->GetVehicleKit())
                if (VehicleSeat* seat = vehicle->GetSeat(seatId))
                    if (seat->seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
                        if (unit->GetTypeId() == TYPEID_PLAYER) 
                        {
                            BattleGround* bg = ((BattleGroundMap*)m_creature->GetMap())->GetBG();
                            if (bg && bg->GetTypeID(true) == BATTLEGROUND_SA)
                            {
                                BattleGroundSA* bgsa = (BattleGroundSA*)bg;
                                m_creature->setFaction(bgsa->GetDefender() == ALLIANCE ? VEHICLE_FACTION_HORDE : VEHICLE_FACTION_ALLIANCE);
                            }
                        }
        }
    }

    void AttackedBy(Unit* pAttacker) { }
    void AttackStart(Unit* pWho) { }
    void MoveInLineOfSight(Unit* pWho) { }
    void EnterCombat(Unit* pEnemy) { }
};

CreatureAI* GetAI_npc_sa_demolisher(Creature* pCreature)
{
    return new npc_sa_demolisherAI(pCreature);
}

struct MANGOS_DLL_DECL npc_sa_cannonAI : public ScriptedAI
{
    npc_sa_cannonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        if (m_creature->GetMap()->IsBattleGround())
        {
            BattleGround* bg = ((BattleGroundMap*)m_creature->GetMap())->GetBG();
            if (bg && bg->GetTypeID(true) == BATTLEGROUND_SA)
            {
                BattleGroundSA* bgsa = (BattleGroundSA*)bg;
                m_creature->setFaction(bgsa->GetDefender() == ALLIANCE ? VEHICLE_FACTION_ALLIANCE : VEHICLE_FACTION_HORDE);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (!m_creature->isCharmed())
            if (!m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE))
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    bool HandleSpellClick(Unit* caster, Unit* target, uint32 spellId, ObjectGuid casterGuid)
    {
        if (caster->GetTypeId() != TYPEID_PLAYER || !m_creature->GetMap()->IsBattleGround())
            return true;

        BattleGround* bg = ((BattleGroundMap*)m_creature->GetMap())->GetBG();
        if (bg && bg->GetTypeID(true) == BATTLEGROUND_SA)
        {
            BattleGroundSA* bgsa = (BattleGroundSA*)bg;
            if (bgsa->GetDefender() != ((Player*)caster)->GetTeam())
                return true;
        }

        return false;
    }

    void PassengerBoarded(Unit* unit, int8 seatId, bool apply)
    {
        if (!apply)
        {
            if (VehicleKit* vehicle =  m_creature->GetVehicleKit())
                if (VehicleSeat* seat = vehicle->GetSeat(seatId))
                    if (seat->seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
                        if (unit->GetTypeId() == TYPEID_PLAYER) 
                        {
                            BattleGround* bg = ((BattleGroundMap*)m_creature->GetMap())->GetBG();
                            if (bg && bg->GetTypeID(true) == BATTLEGROUND_SA)
                            {
                                BattleGroundSA* bgsa = (BattleGroundSA*)bg;
                                m_creature->setFaction(bgsa->GetDefender() == ALLIANCE ? VEHICLE_FACTION_ALLIANCE : VEHICLE_FACTION_HORDE);
                            }
                        }
        }
    }

    void AttackedBy(Unit* pAttacker) { }
    void AttackStart(Unit* pWho) { }
    void MoveInLineOfSight(Unit* pWho) { }
    void EnterCombat(Unit* pEnemy) { }
};

CreatureAI* GetAI_npc_sa_cannon(Creature* pCreature)
{
    return new npc_sa_cannonAI(pCreature);
}

void AddSC_battlegroundSA()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_sa_demolisher";
    pNewScript->GetAI = &GetAI_npc_sa_demolisher;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sa_cannon";
    pNewScript->GetAI = &GetAI_npc_sa_cannon;
    pNewScript->RegisterSelf();
}
