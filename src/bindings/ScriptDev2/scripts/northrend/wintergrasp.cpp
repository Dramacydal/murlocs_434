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

#include "precompiled.h"
#include "OutdoorPvP/OutdoorPvPMgr.h"
#include "BattleField/BattleFieldWG.h"
#include "Vehicle.h"

enum
{
    SPELL_SPIRIT_HEAL_CHANNEL               = 22011,        // Spirit Heal Channel

    SPELL_SPIRIT_HEAL                       = 22012,        // Spirit Heal
    SPELL_SPIRIT_HEAL_MANA                  = 44535,        // in battlegrounds player get this no-mana-cost-buff

    SPELL_WAITING_TO_RESURRECT              = 2584,         // players who cancel this aura don't want a resurrection

    WG_BUSY_TIMER                           = 150 * IN_MILLISECONDS,
    WG_BUILD_TIMER                          = 150,

    ACTION_ID_FORTRESS_GRAVEYARD            = 2000006664,
    ACTION_ID_SUNKEN_RING_GRAVEYARD         = 2000006665,
    ACTION_ID_BROKEN_TEMPLE_GRAVEYARD       = 2000006666,
    ACTION_ID_WESTPARK_GRAVEYARD            = 2000006667,
    ACTION_ID_EASTPARK_GRAVEYARD            = 2000006668,
    ACTION_ID_ALLIANCE_LANDING_GRAVEYARD    = 2000006669,
    ACTION_ID_HORDE_LANDING_GRAVEYARD       = 2000006670,

    ACTION_ID_BUILD_CATAPULT                = 2000006671,
    ACTION_ID_BUILD_DEMOLISHER              = 2000006672,
    ACTION_ID_BUILD_SIEGE_ENGINE            = 2000006673,

    ACTION_ID_QUEUE_WG                      = 2000006674,

    GOSSIP_TEXT_ID_WG_HELLO                 = 89000,
    GOSSIP_TEXT_ID_WG_HELLO_COOLDOWN        = 89001,
    GOSSIP_TEXT_ID_WG_HELLO_BUSY            = 89002,
    GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_A = 89003,
    GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_H = 89004,
    GOSSIP_TEXT_ID_WG_CANT_BUILD_MORE       = 89005,

    GOSSIT_TEXT_A_C_A_CD                    = 14782, // Greetings, $c.$B$BThe Alliance currently control Wintergrasp Fortress.  When the battle is about to begin, talk to me to enter the battle.$B$BThe next battle begins in $4354k..
    GOSSIT_TEXT_A_C_A_STARTED               = 14781, // Greetings, $c.$B$BThe Alliance currently control Wintergrasp Fortress, but the battle has begun!  Hurry to Wintergrasp and support our forces!
    GOSSIT_TEXT_A_C_A_ABOUT                 = 14791, // Greetings, $c.$B$BThe Alliance currently control Wintergrasp Fortress, but the next battle starts soon.   Get ready to head to Wintergrasp and support our forces!$B$BThe next battle begins in $4354k..
    GOSSIP_TEXT_A_C_A_ENDED                 = 14085, // Greetings, $c.$B$BThe Alliance now controls Wintergrasp Fortress. Use the portal, if you like.$B$BThe next battle begins in $3975w:$3976w$3782w:$3784w$3785w.

    GOSSIT_TEXT_A_C_H_CD                    = 14782, // Greetings, $c.$B$BWhen the Alliance controls Wintergrasp Fortress, I will open a portal for those who wish to travel there.
    GOSSIT_TEXT_A_C_H_ENSUES                = 14781, // Greetings, $c.$B$BWhen the Alliance controls Wintergrasp Fortress, I will open a portal for those who wish to travel there.$B$BThe battle currently enues. Hurry to Wintergrasp and support our forces!

    GOSSIT_TEXT_H_C_H_CD                    = 14775,
    GOSSIT_TEXT_H_C_H_STARTED               = 14777,
    GOSSIT_TEXT_H_C_H_ABOUT                 = 14790,
    GOSSIP_TEXT_H_C_H_ENDED                 = 14086,

    GOSSIT_TEXT_H_C_A_CD                    = 14084,
    GOSSIT_TEXT_H_C_A_ENSUES                = 14241,

    OPTION_WG_FIRST                         = GOSSIP_ACTION_INFO_DEF,
    SENDER_WG_MAIN                          = GOSSIP_SENDER_MAIN + 1,
};

/************************************/

struct MANGOS_DLL_DECL npc_wintergrasp_spirit_guideAI : public ScriptedAI
{
    npc_wintergrasp_spirit_guideAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        bInit = false;
        opvp = NULL;
        Reset();
    }

    BattleFieldWG* opvp;
    bool bInit;
    void Reset() override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!bInit)
        {
            bInit = true;
            opvp = (BattleFieldWG*)sOutdoorPvPMgr.GetBattlefieldById(BATTLEFIELD_WG);
        }

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
            pUnit->CastSpell(pUnit, SPELL_SPIRIT_HEAL_MANA, true);
    }
};

bool GossipHello_npc_wintergrasp_spirit_guide(Player* pPlayer, Creature* pCreature)
{
    BattleFieldWG* opvp = NULL;
    if (pCreature->AI() && (npc_wintergrasp_spirit_guideAI*)pCreature->AI())
        opvp = ((npc_wintergrasp_spirit_guideAI*)pCreature->AI())->opvp;
    if (!opvp)
        return false;

    if (pPlayer->isDead())
        pPlayer->CastSpell(pPlayer, SPELL_WAITING_TO_RESURRECT, true);

    TeamIndex teamIdx = GetTeamIndex(pPlayer->GetTeam());
    if (opvp->GetDefender() == teamIdx)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_FORTRESS_GRAVEYARD, SENDER_WG_MAIN, OPTION_WG_FIRST);
    if (opvp->GetWorkshopOwner(WG_WORKSHOP_SUNKEN_RING) == teamIdx)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_SUNKEN_RING_GRAVEYARD, SENDER_WG_MAIN, OPTION_WG_FIRST + 1);
    if (opvp->GetWorkshopOwner(WG_WORKSHOP_BROKEN_TEMPLE) == teamIdx)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_BROKEN_TEMPLE_GRAVEYARD, SENDER_WG_MAIN, OPTION_WG_FIRST + 2);
    if (opvp->GetWorkshopOwner(WG_WORKSHOP_WESTPARK) == teamIdx)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_WESTPARK_GRAVEYARD, SENDER_WG_MAIN, OPTION_WG_FIRST + 3);
    if (opvp->GetWorkshopOwner(WG_WORKSHOP_EASTPARK) == teamIdx)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_EASTPARK_GRAVEYARD, SENDER_WG_MAIN, OPTION_WG_FIRST + 4);
    if (teamIdx == TEAM_INDEX_ALLIANCE)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_ALLIANCE_LANDING_GRAVEYARD, SENDER_WG_MAIN, OPTION_WG_FIRST + 5);
    if (teamIdx == TEAM_INDEX_HORDE)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_HORDE_LANDING_GRAVEYARD, SENDER_WG_MAIN, OPTION_WG_FIRST + 6);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_wintergrasp_spirit_guide(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    player->CLOSE_GOSSIP_MENU();

    BattleFieldWG* opvp = NULL;
    if (pCreature->AI() && (npc_wintergrasp_spirit_guideAI*)pCreature->AI())
        opvp = ((npc_wintergrasp_spirit_guideAI*)pCreature->AI())->opvp;
    if (!opvp)
        return false;

    if (sender == SENDER_WG_MAIN)
    {
        TeamIndex teamIdx = GetTeamIndex(player->GetTeam());
        switch(action)
        {
            case OPTION_WG_FIRST:
                if (opvp->GetDefender() != teamIdx)
                    break;
                player->CastSpell(player, SPELL_TELEPORT_FORTRESS_GRAVEYARD, true);
                break;
            case OPTION_WG_FIRST + 1:
                if (opvp->GetWorkshopOwner(WG_WORKSHOP_SUNKEN_RING) != teamIdx)
                    break;
                player->CastSpell(player, SPELL_TELEPORT_SUNKEN_RING, true);
                break;
            case OPTION_WG_FIRST + 2:
                if (opvp->GetWorkshopOwner(WG_WORKSHOP_BROKEN_TEMPLE) != teamIdx)
                    break;
                player->CastSpell(player, SPELL_TELEPORT_BROKEN_TEMPLE, true);
                break;
            case OPTION_WG_FIRST + 3:
                if (opvp->GetWorkshopOwner(WG_WORKSHOP_WESTPARK) != teamIdx)
                    break;
                player->CastSpell(player, SPELL_TELEPORT_WESTPARK_GRAVEYARD, true);
                break;
            case OPTION_WG_FIRST + 4:
                if (opvp->GetWorkshopOwner(WG_WORKSHOP_EASTPARK) != teamIdx)
                    break;
                player->CastSpell(player, SPELL_TELEPORT_EASTPARK_GRAVEYARD, true);
                break;
            case OPTION_WG_FIRST + 5:
                if (teamIdx != TEAM_INDEX_ALLIANCE)
                    break;
                player->CastSpell(player, SPELL_TELEPORT_ALLIANCE_LANDING, true);
                break;
            case OPTION_WG_FIRST + 6:
                if (teamIdx != TEAM_INDEX_HORDE)
                    break;
                player->CastSpell(player, SPELL_TELEPORT_HORDE_LANDING, true);
                break;
        }
    }

    return true;
}

CreatureAI* GetAI_npc_wintergrasp_spirit_guide(Creature* pCreature)
{
    return new npc_wintergrasp_spirit_guideAI(pCreature);
}

/************************************/

struct MANGOS_DLL_DECL npc_wintergrasp_vehicleAI : public ScriptedAI
{
    npc_wintergrasp_vehicleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        bInit = false;
        opvp = NULL;
    }

    uint32 waterCheckTimer;
    uint32 zoneCheckTimer;
    uint32 teleporterCheckTimer;
    bool isInWater;
    BattleFieldWG* opvp;
    bool bInit;
    TeamIndex teamIdx;

    void Reset() override
    {
        waterCheckTimer = 1000;
        zoneCheckTimer = 5000;
        teleporterCheckTimer = 5000;
        isInWater = false;
        teamIdx = TEAM_INDEX_NEUTRAL;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!bInit)
        {
            bInit = true;
            opvp = (BattleFieldWG*)sOutdoorPvPMgr.GetBattlefieldById(BATTLEFIELD_WG);
            if (opvp)
                opvp->_OnCreatureCreate(m_creature);

            if (m_creature->GetEntry() != NPC_WINTERGRASP_SIEGE_TURRET_A && m_creature->GetEntry() != NPC_WINTERGRASP_SIEGE_TURRET_H && m_creature->GetEntry() != NPC_WINTERGRASP_TOWER_CANNON)
            {
                if (Unit* creator = m_creature->GetCreator())
                    if (creator->GetTypeId() == TYPEID_PLAYER)
                    {
                        m_creature->CastSpell(m_creature, ((Player*)creator)->GetTeam() == ALLIANCE ? SPELL_ALLIANCE_FLAG : SPELL_HORDE_FLAG, true);
                        teamIdx = GetTeamIndex(((Player*)creator)->GetTeam());
                    }
            }
        }

        if (waterCheckTimer < uiDiff)
        {
            waterCheckTimer = 1000;

            if (isInWater != m_creature->IsInWater())
            {
                isInWater = !isInWater;
                if (isInWater)
                    m_creature->CastSpell(m_creature, SPELL_WINTERGRASP_WATER, true);
                else
                    m_creature->RemoveAurasDueToSpell(SPELL_WINTERGRASP_WATER);
            }
        }
        else
            waterCheckTimer -= uiDiff;

        if (zoneCheckTimer < uiDiff)
        {
            zoneCheckTimer = 5000;

            if (m_creature->GetEntry() != NPC_WINTERGRASP_SIEGE_TURRET_A && m_creature->GetEntry() != NPC_WINTERGRASP_SIEGE_TURRET_H)
            {
                if (m_creature->GetZoneId() != ZONE_ID_WINTERGRASP)
                {
                    if (opvp)
                        opvp->HandleCreatureDeath(m_creature);
                    m_creature->ForcedDespawn();
                }
            }
        }
        else
            zoneCheckTimer -= uiDiff;

        if (opvp && m_creature->GetEntry() != NPC_WINTERGRASP_SIEGE_TURRET_A && m_creature->GetEntry() != NPC_WINTERGRASP_SIEGE_TURRET_H && teamIdx == opvp->GetDefender())
        {
            if (teleporterCheckTimer < uiDiff)
            {
                teleporterCheckTimer = 5000;

                if (!m_creature->HasAura(SPELL_TELEPORT_VEHICLE))
                {
                    if (m_creature->GetDistance2d(WGTeleportLocations[10].x, WGTeleportLocations[10].y) < 6.0f)
                    {
                        m_creature->NearTeleportTo(WGTeleportLocations[10].destX, WGTeleportLocations[10].destY, WGTeleportLocations[10].destZ, m_creature->GetOrientation(), true);
                        m_creature->CastSpell(m_creature, SPELL_TELEPORT_VEHICLE, true);
                    }
                    else if (m_creature->GetDistance2d(WGTeleportLocations[11].x, WGTeleportLocations[11].y) < 6.0f)
                    {
                        m_creature->NearTeleportTo(WGTeleportLocations[11].destX, WGTeleportLocations[11].destY, WGTeleportLocations[9].destZ, m_creature->GetOrientation(), true);
                        m_creature->CastSpell(m_creature, SPELL_TELEPORT_VEHICLE, true);
                    }
                }
            }
            else
                teleporterCheckTimer -= uiDiff;
        }
    }

    bool HandleSpellClick(Unit* caster, Unit* target, uint32 spellId, int32* /*bp0*/, int32* /*bp1*/, int32* /*bp2*/, ObjectGuid casterGuid) override
    {
        if (caster->GetTypeId() != TYPEID_PLAYER)
            return false;

        if (!opvp || opvp->GetState() != BF_STATE_IN_PROGRESS || !opvp->IsMember(caster->GetObjectGuid()))
            return true;

        switch(m_creature->GetEntry())
        {
            case NPC_WINTERGRASP_CATAPULT:
                if (opvp->GetPlayerRank((Player*)caster) < WG_RANK_CORPORAL)
                    return true;
                break;
            case NPC_WINTERGRASP_DEMOLISHER:
            case NPC_WINTERGRASP_SIEGE_ENGINE_A:
            case NPC_WINTERGRASP_SIEGE_ENGINE_H:
            case NPC_WINTERGRASP_SIEGE_TURRET_A:
            case NPC_WINTERGRASP_SIEGE_TURRET_H:
                if (opvp->GetPlayerRank((Player*)caster) < WG_RANK_LIEUTENANT)
                    return true;
                break;
        }

        return false;
    }

    void PassengerBoarded(Unit* unit, int8 seatId, bool apply) override
    {
        if (!apply)
        {
            if (VehicleKit* vehicle =  m_creature->GetVehicleKit())
                if (VehicleSeat* seat = vehicle->GetSeat(seatId))
                    if (seat->seatInfo->m_flags & SEAT_FLAG_CAN_CONTROL)
                        if (unit->GetTypeId() == TYPEID_PLAYER) 
                            m_creature->setFaction(BFFactions[GetTeamIndex(((Player*)unit)->GetTeam())]);
        }
    }

    void AttackedBy(Unit* pAttacker) override { }
    void AttackStart(Unit* pWho) override { }
    void MoveInLineOfSight(Unit* pWho) override { }
    void EnterCombat(Unit* pEnemy) override { }
};

CreatureAI* GetAI_npc_wintergrasp_vehicle(Creature* pCreature)
{
    return new npc_wintergrasp_vehicleAI(pCreature);
}

/************************************/

struct MANGOS_DLL_DECL npc_wintergrasp_mechanicAI : public ScriptedAI
{
    npc_wintergrasp_mechanicAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        bInit = false;
        opvp = NULL;

        if (Creature* pArms = GetClosestCreatureWithEntry(pCreature, NPC_CONTROL_ARMS, 20.0f))
            armsGuid = pArms->GetObjectGuid();
    }

    BattleFieldWG* opvp;
    bool bInit;
    ObjectGuid armsGuid;

    void Reset() override
    {
        //busyTimer = 0;
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!bInit)
        {
            bInit = true;
            opvp = (BattleFieldWG*)sOutdoorPvPMgr.GetBattlefieldById(BATTLEFIELD_WG);
        }

        /*if (busyTimer)
        {
            if (busyTimer <= uiDiff)
                busyTimer = 0;
            else
                busyTimer -= uiDiff;
        }*/
    }

    void MoveInLineOfSight(Unit* pWho) override { }
    void EnterCombat(Unit* pEnemy) override { }
};

CreatureAI* GetAI_npc_wintergrasp_mechanic(Creature* pCreature)
{
    return new npc_wintergrasp_mechanicAI(pCreature);
}

bool GossipHello_npc_wintergrasp_mechanic(Player* pPlayer, Creature* pCreature)
{
    BattleFieldWG* opvp = NULL;
    //uint32 busyTimer = 0;
    if (pCreature->AI() && (npc_wintergrasp_mechanicAI*)pCreature->AI())
    {
        opvp = ((npc_wintergrasp_mechanicAI*)pCreature->AI())->opvp;
        //busyTimer = ((npc_wintergrasp_mechanicAI*)pCreature->AI())->busyTimer;
    }
    if (!opvp)
        return false;

    /*if (busyTimer)
    {
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_BUSY, pCreature->GetObjectGuid());
        return true;
    }*/

    /*if (pPlayer->HasSpellCooldown(SPELL_BUILD_CATAPULT_FORCE) || pPlayer->HasSpellCooldown(SPELL_BUILD_DEMOLISHER_FORCE) || pPlayer->HasSpellCooldown(SPELL_BUILD_SIEGE_VEHICLE_A_FORCE))
    {
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_COOLDOWN, pCreature->GetObjectGuid());
        return true;
    }*/

    if (opvp->GetState() != BF_STATE_IN_PROGRESS || !opvp->IsMember(pPlayer->GetObjectGuid()))
    {
        if (pPlayer->GetTeam() == ALLIANCE)
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_A, pCreature->GetObjectGuid());
        else
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_H, pCreature->GetObjectGuid());

        return true;
    }

    if (!opvp->CanBuildMoreVehicles(GetTeamIndex(pPlayer->GetTeam())))
    {
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_CANT_BUILD_MORE, pCreature->GetObjectGuid());
        return true;
    }

    switch(opvp->GetPlayerRank(pPlayer))
    {
        case WG_RANK_CORPORAL:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_BUILD_CATAPULT, SENDER_WG_MAIN, OPTION_WG_FIRST);
            break;
        case WG_RANK_LIEUTENANT:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_BUILD_CATAPULT, SENDER_WG_MAIN, OPTION_WG_FIRST);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_BUILD_DEMOLISHER, SENDER_WG_MAIN, OPTION_WG_FIRST + 1);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_BUILD_SIEGE_ENGINE, SENDER_WG_MAIN, OPTION_WG_FIRST + 2);
            break;
        default:
            if (pPlayer->GetTeam() == ALLIANCE)
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_A, pCreature->GetObjectGuid());
            else
                pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_H, pCreature->GetObjectGuid());
            return true;
    }

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_wintergrasp_mechanic(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    BattleFieldWG* opvp = NULL;
    ObjectGuid armsGuid;
    //uint32 busyTimer = 0;
    if (pCreature->AI() && (npc_wintergrasp_mechanicAI*)pCreature->AI())
    {
        opvp = ((npc_wintergrasp_mechanicAI*)pCreature->AI())->opvp;
        armsGuid = ((npc_wintergrasp_mechanicAI*)pCreature->AI())->armsGuid;
        //busyTimer = ((npc_wintergrasp_mechanicAI*)pCreature->AI())->busyTimer;
    }
    if (!opvp)
        return false;

    if (sender == SENDER_WG_MAIN)
    {
        if (opvp->GetState() != BF_STATE_IN_PROGRESS || !opvp->IsMember(player->GetObjectGuid()))
        {
            player->PlayerTalkClass->ClearMenus();
            if (player->GetTeam() == ALLIANCE)
                player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_A, pCreature->GetObjectGuid());
            else
                player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_RANK_NOT_HIGH_H, pCreature->GetObjectGuid());

            return true;
        }

        /*if (busyTimer)
        {
            player->PlayerTalkClass->ClearMenus();
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_COOLDOWN, pCreature->GetObjectGuid());
            return true;
        }*/

        /*if (player->HasSpellCooldown(SPELL_BUILD_CATAPULT_FORCE) || player->HasSpellCooldown(SPELL_BUILD_DEMOLISHER_FORCE) || player->HasSpellCooldown(SPELL_BUILD_SIEGE_VEHICLE_A_FORCE))
        {
            player->PlayerTalkClass->ClearMenus();
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_HELLO_COOLDOWN, pCreature->GetObjectGuid());
            return true;
        }*/

        if (!opvp->CanBuildMoreVehicles(GetTeamIndex(player->GetTeam())))
        {
            player->PlayerTalkClass->ClearMenus();
            player->SEND_GOSSIP_MENU(GOSSIP_TEXT_ID_WG_CANT_BUILD_MORE, pCreature->GetObjectGuid());
            return true;
        }

        uint32 spellId = 0;
        switch(action)
        {
            case OPTION_WG_FIRST:
                spellId = SPELL_BUILD_CATAPULT_FORCE;
                break;
            case OPTION_WG_FIRST + 1:
                spellId = SPELL_BUILD_DEMOLISHER_FORCE;
                break;
            case OPTION_WG_FIRST + 2:
                if (player->GetTeam() == ALLIANCE)
                    spellId = SPELL_BUILD_SIEGE_VEHICLE_A_FORCE;
                else
                    spellId = SPELL_BUILD_SIEGE_VEHICLE_H_FORCE;
                break;
            default:
                return true;
        }

        player->CastSpell(player, spellId, true);

        if (Creature* arms = pCreature->GetMap()->GetCreature(armsGuid))
            pCreature->CastSpell(arms, SPELL_ACTIVATE_ROBOTIC_ARMS, true);
    }

    return true;
}

/************************************/

struct MANGOS_DLL_DECL npc_wintergrasp_battlemasterAI : public ScriptedAI
{
    npc_wintergrasp_battlemasterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        bInit = false;
        opvp = NULL;
        Reset();
    }

    BattleFieldWG* opvp;
    bool bInit;

    void Reset() override { }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!bInit)
        {
            bInit = true;
            opvp = (BattleFieldWG*)sOutdoorPvPMgr.GetBattlefieldById(BATTLEFIELD_WG);
        }
    }
};

bool GossipHello_npc_wintergrasp_battlemaster(Player* pPlayer, Creature* pCreature)
{
    BattleFieldWG* opvp = NULL;
    if (pCreature->AI() && (npc_wintergrasp_battlemasterAI*)pCreature->AI())
        opvp = ((npc_wintergrasp_battlemasterAI*)pCreature->AI())->opvp;
    if (!opvp)
        return false;

    uint32 textId = 0;
    TeamIndex teamIdx = GetTeamIndex(pPlayer->GetTeam());

    if (opvp->GetDefender() == teamIdx)
    {
        if (opvp->GetState() == BF_STATE_IN_PROGRESS)
        {
            textId = teamIdx == TEAM_INDEX_ALLIANCE ? GOSSIT_TEXT_A_C_A_STARTED : GOSSIT_TEXT_H_C_H_STARTED;
        }
        else if (opvp->GetTimer() < opvp->GetStartInviteDelay())
        {
            textId = teamIdx == TEAM_INDEX_ALLIANCE ? GOSSIT_TEXT_A_C_A_ABOUT : GOSSIT_TEXT_H_C_H_ABOUT;
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_QUEUE_WG, SENDER_WG_MAIN, OPTION_WG_FIRST);
        }
        else if (opvp->GetTimer() > opvp->GetStopTeleportingTime())
        {
            textId = teamIdx == TEAM_INDEX_ALLIANCE ? GOSSIP_TEXT_A_C_A_ENDED : GOSSIP_TEXT_H_C_H_ENDED;
        }
        else
        {
            textId = teamIdx == TEAM_INDEX_ALLIANCE ? GOSSIT_TEXT_A_C_A_CD : GOSSIT_TEXT_H_C_H_CD;
        }
    }
    else
    {
        if (opvp->GetState() == BF_STATE_IN_PROGRESS)
        {
            textId = teamIdx == TEAM_INDEX_ALLIANCE ? GOSSIT_TEXT_A_C_H_ENSUES : GOSSIT_TEXT_H_C_A_ENSUES;
        }
        else if (opvp->GetTimer() < opvp->GetStartInviteDelay())
        {
            textId = teamIdx == TEAM_INDEX_ALLIANCE ? GOSSIT_TEXT_A_C_H_ENSUES : GOSSIT_TEXT_H_C_A_ENSUES;
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_ID_QUEUE_WG, SENDER_WG_MAIN, OPTION_WG_FIRST);
        }
        else
        {
            textId = teamIdx == TEAM_INDEX_ALLIANCE ? GOSSIT_TEXT_A_C_H_CD : GOSSIT_TEXT_H_C_A_CD;
        }
    }

    pPlayer->SEND_GOSSIP_MENU(textId, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_wintergrasp_battlemaster(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    player->CLOSE_GOSSIP_MENU();

    BattleFieldWG* opvp = NULL;
    if (pCreature->AI() && (npc_wintergrasp_battlemasterAI*)pCreature->AI())
        opvp = ((npc_wintergrasp_battlemasterAI*)pCreature->AI())->opvp;
    if (!opvp)
        return true;

    if (sender == SENDER_WG_MAIN)
    {
        if (action == OPTION_WG_FIRST)
        {
            if (opvp->GetState() != BF_STATE_COOLDOWN || opvp->GetTimer() > opvp->GetStartInviteDelay())
                return true;

            opvp->InvitePlayerToQueue(player);
        }
    }

    return true;
}

CreatureAI* GetAI_npc_wintergrasp_battlemaster(Creature* pCreature)
{
    return new npc_wintergrasp_battlemasterAI(pCreature);
}

/****************************/

bool GOUse_go_wintergrasp_portal(Player* pPlayer, GameObject* pGo)
{
    BattleFieldWG* opvp = (BattleFieldWG*)sOutdoorPvPMgr.GetBattlefieldById(BATTLEFIELD_WG);
    if (!opvp)
        return false;

    TeamIndex teamIdx = GetTeamIndex(pPlayer->GetTeam());
    if (opvp->GetState() == BF_STATE_IN_PROGRESS ||
        opvp->GetTimer() < opvp->GetStartInviteDelay() && opvp->GetDefender() == teamIdx ||
        opvp->GetTimer() > opvp->GetStopTeleportingTime() && opvp->GetDefender() == teamIdx)
        return false;

    return true;
}

void AddSC_wintergrasp()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_wintergrasp_spirit_guide";
    pNewScript->GetAI = &GetAI_npc_wintergrasp_spirit_guide;
    pNewScript->pGossipHello = &GossipHello_npc_wintergrasp_spirit_guide;
    pNewScript->pGossipSelect = &GossipSelect_npc_wintergrasp_spirit_guide;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wintergrasp_vehicle";
    pNewScript->GetAI = &GetAI_npc_wintergrasp_vehicle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wintergrasp_mechanic";
    pNewScript->GetAI = &GetAI_npc_wintergrasp_mechanic;
    pNewScript->pGossipHello = &GossipHello_npc_wintergrasp_mechanic;
    pNewScript->pGossipSelect = &GossipSelect_npc_wintergrasp_mechanic;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wintergrasp_battlemaster";
    pNewScript->GetAI = &GetAI_npc_wintergrasp_battlemaster;
    pNewScript->pGossipHello = &GossipHello_npc_wintergrasp_battlemaster;
    pNewScript->pGossipSelect = &GossipSelect_npc_wintergrasp_battlemaster;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_wintergrasp_portal";
    pNewScript->pGOUse = &GOUse_go_wintergrasp_portal;
    pNewScript->RegisterSelf();
}
