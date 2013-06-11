/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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

#include "precompiled.h"
#include "OutdoorPvP/OutdoorPvPMgr.h"
#include "BattleField/BattleFieldTB.h"
#include "Vehicle.h"

enum
{
    SPELL_SPIRIT_HEAL_CHANNEL               = 22011,        // Spirit Heal Channel

    SPELL_SPIRIT_HEAL                       = 22012,        // Spirit Heal
    SPELL_SPIRIT_HEAL_MANA                  = 44535,        // in battlegrounds player get this no-mana-cost-buff

    SPELL_WAITING_TO_RESURRECT              = 2584,         // players who cancel this aura don't want a resurrection
};

struct MANGOS_DLL_DECL npc_tol_barad_spirit_guideAI : public ScriptedAI
{
    npc_tol_barad_spirit_guideAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() override
    {
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        // auto cast the whole time this spell
        if (!m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
        {
            m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL, true);
            m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL_CHANNEL, false);
        }
    }

    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpellEntry) override
    {
        if (pSpellEntry->Id == SPELL_SPIRIT_HEAL && pUnit->GetTypeId() == TYPEID_PLAYER &&
            pUnit->HasAura(SPELL_WAITING_TO_RESURRECT))
        {
            pUnit->CastSpell(pUnit, SPELL_SPIRIT_HEAL_MANA, true);
            if (((Player*)pUnit)->GetAreaId() == AREA_ID_BARADIN_HOLD)
                pUnit->CastSpell(pUnit, SPELL_TB_SLOW_FALL, true);
        }
    }
};

bool GossipHello_npc_tol_barad_spirit_guide(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetCachedZoneId() != ZONE_ID_TOL_BARAD)
        return false;

    if (pPlayer->isDead())
        pPlayer->CastSpell(pPlayer, SPELL_WAITING_TO_RESURRECT, true);

    return true;
}

CreatureAI* GetAI_npc_tol_barad_spirit_guide(Creature* pCreature)
{
    return new npc_tol_barad_spirit_guideAI(pCreature);
}

#define SIEGE_TURRET_SEAT_ID 7

struct MANGOS_DLL_DECL npc_tol_barad_vehicleAI : public ScriptedAI
{
    npc_tol_barad_vehicleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 zoneCheckTimer;
    uint32 auraCheckTimer;
    BattleFieldTB* opvp;
    bool bInit;

    void Reset() override
    {
        bInit = false;
        opvp = NULL;
        zoneCheckTimer = 5000;
        auraCheckTimer = 2000;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!bInit)
        {
            bInit = true;
            opvp = (BattleFieldTB*)sOutdoorPvPMgr.GetBattlefieldById(BATTLEFIELD_TB);
            if (opvp)
                opvp->_OnCreatureCreate(m_creature);

            if (m_creature->GetEntry() != NPC_SIEGE_ENGINE_TURRET)
            {
                if (opvp)
                    m_creature->setFaction(BFFactions[opvp->GetAttacker()]);

                if (!m_creature->GetCharmerGuid())
                    m_creature->CastSpell(m_creature, SPELL_THICK_LAYER_OF_RUST, true);
            }
        }

        if (zoneCheckTimer < uiDiff)
        {
            zoneCheckTimer = 5000;

            if (m_creature->GetEntry() == NPC_ABANDONED_SIEGE_ENGINE)
            {
                if (m_creature->GetZoneId() != ZONE_ID_TOL_BARAD)
                    m_creature->ForcedDespawn();
            }
        }
        else
            zoneCheckTimer -= uiDiff;

        if (auraCheckTimer < uiDiff)
        {
            auraCheckTimer = 2000;

            if (m_creature->GetEntry() == NPC_ABANDONED_SIEGE_ENGINE)
            {
                if (m_creature->HasAura(SPELL_DEPLOY_SIEGE_MODE) && !GetClosestCreatureWithEntry(m_creature, NPC_TOWER_CANNON_TARGET, 200.0f, false, false))
                {
                    if (VehicleKit* vehicle = m_creature->GetVehicleKit())
                        vehicle->RemoveAllPassengers();
                }
            }
            else if (m_creature->GetEntry() == NPC_SIEGE_ENGINE_TURRET)
            {
                Unit* creator = m_creature->GetCreator();
                if (!creator || !creator->IsInMap(m_creature))
                    m_creature->ForcedDespawn(200);
            }
        }
        else
            auraCheckTimer -= uiDiff;
    }

    bool HandleSpellClick(Unit* caster, Unit* target, uint32 spellId, int32* /*bp0*/, int32* /*bp1*/, int32* /*bp2*/, ObjectGuid casterGuid) override
    {
        if (caster->GetTypeId() != TYPEID_PLAYER)
            return false;

        if (!opvp || opvp->GetState() != BF_STATE_IN_PROGRESS || !opvp->IsMember(caster->GetObjectGuid()))
            return true;

        if (!opvp->IsVeteran((Player*)caster))
            return true;

        if (m_creature->HasAura(SPELL_DEPLOY_SIEGE_MODE))
            return true;

        return false;
    }

    void PassengerBoarded(Unit* unit, int8 seatId, bool apply) override
    {
        if (!apply)
        {
            if (unit->GetTypeId() == TYPEID_PLAYER)
                m_creature->setFaction(BFFactions[GetTeamIndex(((Player*)unit)->GetTeam())]);
            else
            {
                ((Creature*)unit)->ForcedDespawn(200);
                DoCast(m_creature, SPELL_LEAVE_SIEGE_MODE);
            }
        }
        else if (unit->GetTypeId() == TYPEID_PLAYER)
            m_creature->RemoveAurasDueToSpell(SPELL_THICK_LAYER_OF_RUST);
        else if (unit->GetEntry() == NPC_SIEGE_ENGINE_TURRET)
        {
            if (opvp)
                unit->setFaction(BFFactions[opvp->GetAttacker()]);
            unit->CastSpell(unit, SPELL_SIEGE_CANNON_PERIODIC, true);
        }
    }

    void JustSummoned(Creature* creature) override
    {
        if (creature->GetEntry() == NPC_SIEGE_ENGINE_TURRET)
            if (VehicleKit* vehicle = m_creature->GetVehicleKit())
                creature->EnterVehicle(vehicle, SIEGE_TURRET_SEAT_ID);
    }

    void SpellHit(Unit* /*pUnit*/, SpellEntry const* spell) override
    {
        if (spell->Id == SPELL_DEPLOY_SIEGE_MODE)
            m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        else if (spell->Id == SPELL_LEAVE_SIEGE_MODE)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_DEPLOY_SIEGE_MODE);
            m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
        }
    }

    void AttackedBy(Unit* pAttacker) override { }
    void AttackStart(Unit* pWho) override { }
    void MoveInLineOfSight(Unit* pWho) override { }
    void EnterCombat(Unit* pEnemy) override { }
};

CreatureAI* GetAI_npc_tol_barad_vehicle(Creature* pCreature)
{
    return new npc_tol_barad_vehicleAI(pCreature);
}

void AddSC_tol_barad()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_tol_barad_spirit_guide";
    pNewScript->GetAI = &GetAI_npc_tol_barad_spirit_guide;
    pNewScript->pGossipHello = &GossipHello_npc_tol_barad_spirit_guide;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tol_barad_vehicle";
    pNewScript->GetAI = &GetAI_npc_tol_barad_vehicle;
    pNewScript->RegisterSelf();
}
