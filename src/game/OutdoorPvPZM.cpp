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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "OutdoorPvP.h"
#include "OutdoorPvPZM.h"
#include "GameObject.h"


OutdoorPvPZM::OutdoorPvPZM(uint8 _id) : OutdoorPvP(_id),
    m_uiGraveyardWorldState(WORLD_STATE_GRAVEYARD_NEUTRAL),
    m_uiAllianceScoutWorldState(WORLD_STATE_ALLIANCE_FLAG_NOT_READY),
    m_uiHordeScoutWorldState(WORLD_STATE_HORDE_FLAG_NOT_READY),

    m_graveyardOwner(TEAM_NONE),
    m_uiTowersAlliance(0),
    m_uiTowersHorde(0)
{
    // init world states
    m_uiBeaconWorldState[0] = WORLD_STATE_TOWER_EAST_NEUTRAL;
    m_uiBeaconWorldState[1] = WORLD_STATE_TOWER_WEST_NEUTRAL;
    m_uiBeaconMapState[0] = WORLD_STATE_BEACON_EAST_NEUTRAL;
    m_uiBeaconMapState[1] = WORLD_STATE_BEACON_WEST_NEUTRAL;

    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
        m_capturePointOwner[i] = TEAM_NONE;
}

bool OutdoorPvPZM::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_ZANGARMARSH);
    RegisterZone(ZONE_ID_SERPENTSHRINE_CAVERN);
    RegisterZone(ZONE_ID_STREAMVAULT);
    RegisterZone(ZONE_ID_UNDERBOG);
    RegisterZone(ZONE_ID_SLAVE_PENS);

    return true;
}

void OutdoorPvPZM::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, m_uiAllianceScoutWorldState, WORLD_STATE_ADD);
    FillInitialWorldState(data, count, m_uiHordeScoutWorldState, WORLD_STATE_ADD);
    FillInitialWorldState(data, count, m_uiGraveyardWorldState, WORLD_STATE_ADD);

    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        FillInitialWorldState(data, count, m_uiBeaconWorldState[i], WORLD_STATE_ADD);
        FillInitialWorldState(data, count, m_uiBeaconMapState[i], WORLD_STATE_ADD);
    }
}

void OutdoorPvPZM::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(m_uiAllianceScoutWorldState, WORLD_STATE_REMOVE);
    pPlayer->SendUpdateWorldState(m_uiHordeScoutWorldState, WORLD_STATE_REMOVE);
    pPlayer->SendUpdateWorldState(m_uiGraveyardWorldState, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        pPlayer->SendUpdateWorldState(m_uiBeaconWorldState[i], WORLD_STATE_REMOVE);
        pPlayer->SendUpdateWorldState(m_uiBeaconMapState[i], WORLD_STATE_REMOVE);
    }
}

void OutdoorPvPZM::HandlePlayerEnterZone(Player* pPlayer)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    pPlayer->RemoveAurasDueToSpell(SPELL_TWIN_SPIRE_BLESSING);

    // cast buff the the player which enters the zone
    if ((pPlayer->GetTeam() == ALLIANCE ? m_uiTowersAlliance : m_uiTowersHorde) == MAX_ZM_TOWERS)
        pPlayer->CastSpell(pPlayer, SPELL_TWIN_SPIRE_BLESSING, true);

    OutdoorPvP::HandlePlayerEnterZone(pPlayer);
}

void OutdoorPvPZM::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    pPlayer->RemoveAurasDueToSpell(SPELL_TWIN_SPIRE_BLESSING);

    OutdoorPvP::HandlePlayerLeaveZone(pPlayer);
}

void OutdoorPvPZM::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ALLIANCE_FIELD_SCOUT:
            m_allianceScoutGuid = pCreature->GetObjectGuid();
            break;
        case NPC_HORDE_FIELD_SCOUT:
            m_hordeScoutGuid = pCreature->GetObjectGuid();
            break;
        case NPC_PVP_BEAM_RED:
            // East Beam
            if (pCreature->GetPositionY() < 7000.0f)
            {
                m_BeamRedGUID[0] = pCreature->GetObjectGuid();
                if (m_capturePointOwner[0] == HORDE)
                    return;
            }
            // Center Beam
            else if (pCreature ->GetPositionY() < 7300.0f)
            {
                m_BeamCenterRedGUID = pCreature->GetObjectGuid();
                if (m_graveyardOwner == HORDE)
                    return;
            }
            // West Beam
            else
            {
                m_BeamRedGUID[1] = pCreature->GetObjectGuid();
                if (m_capturePointOwner[1] == HORDE)
                    return;
            }

            pCreature->SetRespawnDelay(7*DAY);
            pCreature->ForcedDespawn();
            break;
        case NPC_PVP_BEAM_BLUE:
            // East Beam
            if (pCreature->GetPositionY() < 7000.0f)
            {
                m_BeamBlueGUID[0] = pCreature->GetObjectGuid();
                if (m_capturePointOwner[0] == ALLIANCE)
                    return;
            }
            // Center Beam
            else if (pCreature ->GetPositionY() < 7300.0f)
            {
                m_BeamCenterBlueGUID = pCreature->GetObjectGuid();
                if (m_graveyardOwner == ALLIANCE)
                    return;
            }
            // West Beam
            else
            {
                m_BeamBlueGUID[1] = pCreature->GetObjectGuid();
                if (m_capturePointOwner[1] == ALLIANCE)
                    return;
            }

            pCreature->SetRespawnDelay(7 * DAY);
            pCreature->ForcedDespawn();
            break;
    }
}

void OutdoorPvPZM::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ZANGA_BANNER_WEST:
            m_TowerBannerGUID[1] = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_EAST:
            m_TowerBannerGUID[0] = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_ALLIANCE:
            m_TowerBannerCenterAllianceGUID = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_HORDE:
            m_TowerBannerCenterHordeGUID = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_NEUTRAL:
            m_TowerBannerCenterNeutralGUID = pGo->GetObjectGuid();
            break;
    }
}

// Cast player spell on opponent kill
void OutdoorPvPZM::HandlePlayerKillInsideArea(Player* pPlayer, Unit* pVictim)
{
    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        if (GameObject* capturePoint = pPlayer->GetMap()->GetGameObject(m_TowerBannerGUID[i]))
        {
            // check capture point range
            GameObjectInfo const* info = capturePoint->GetGOInfo();
            if (info && pPlayer->IsWithinDistInMap(capturePoint, info->capturePoint.radius))
            {
                // check capture point team
                if (pPlayer->GetTeam() == m_capturePointOwner[i])
                    pPlayer->CastSpell(pPlayer, pPlayer->GetTeam() == ALLIANCE ? SPELL_ZANGA_TOWER_TOKEN_ALLIANCE : SPELL_ZANGA_TOWER_TOKEN_HORDE, true);

                return;
            }
        }
    }
}

// process the capture events
void OutdoorPvPZM::ProcessEvent(uint32 uiEventId, GameObject* pGo, Player* pInvoker, uint32 spellId)
{
    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        if (aZangaTowers[i] == pGo->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (aZangaTowerEvents[i][j].uiEventEntry == uiEventId)
                {
                    if (aZangaTowerEvents[i][j].team != m_capturePointOwner[i])
                    {
                        ProcessCaptureEvent(pGo, i, aZangaTowerEvents[i][j].team, aZangaTowerEvents[i][j].uiWorldState, aZangaTowerEvents[i][j].uiMapState);
                        sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(aZangaTowerEvents[i][j].uiZoneText));
                    }
                    return;
                }
            }
            return;
        }
    }
}

void OutdoorPvPZM::ProcessCaptureEvent(GameObject* pGo, uint32 uiTowerId, Team team, uint32 uiNewWorldState, uint32 uiNewMapState)
{
    if (team == ALLIANCE)
    {
        SetBeaconArtKit(pGo, m_BeamBlueGUID[uiTowerId], true);
        ++m_uiTowersAlliance;

        if (m_uiTowersAlliance == MAX_ZM_TOWERS)
            PrepareFactionScouts(pGo, ALLIANCE);
    }
    else if (team == HORDE)
    {
        SetBeaconArtKit(pGo, m_BeamRedGUID[uiTowerId], true);
        ++m_uiTowersHorde;

        if (m_uiTowersHorde == MAX_ZM_TOWERS)
            PrepareFactionScouts(pGo, HORDE);
    }
    else
    {
        if (m_capturePointOwner[uiTowerId] == ALLIANCE)
        {
            SetBeaconArtKit(pGo, m_BeamBlueGUID[uiTowerId], false);

            if (m_uiTowersAlliance == MAX_ZM_TOWERS)
                ResetScouts(pGo, ALLIANCE);

            --m_uiTowersAlliance;
        }
        else
        {
            SetBeaconArtKit(pGo, m_BeamRedGUID[uiTowerId], false);

            if (m_uiTowersHorde == MAX_ZM_TOWERS)
                ResetScouts(pGo, HORDE);

            --m_uiTowersHorde;
        }
    }

    // update tower state
    SendUpdateWorldState(m_uiBeaconWorldState[uiTowerId], WORLD_STATE_REMOVE);
    m_uiBeaconWorldState[uiTowerId] = uiNewWorldState;
    SendUpdateWorldState(m_uiBeaconWorldState[uiTowerId], WORLD_STATE_ADD);

    SendUpdateWorldState(m_uiBeaconMapState[uiTowerId], WORLD_STATE_REMOVE);
    m_uiBeaconMapState[uiTowerId] = uiNewMapState;
    SendUpdateWorldState(m_uiBeaconMapState[uiTowerId], WORLD_STATE_ADD);;

    // update capture point owner
    m_capturePointOwner[uiTowerId] = team;
}

void OutdoorPvPZM::PrepareFactionScouts(const WorldObject* objRef, Team team)
{
    if (team == ALLIANCE)
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_allianceScoutGuid))
            scout->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        SendUpdateWorldState(m_uiAllianceScoutWorldState, WORLD_STATE_REMOVE);
        m_uiAllianceScoutWorldState = WORLD_STATE_ALLIANCE_FLAG_READY;
        SendUpdateWorldState(m_uiAllianceScoutWorldState, WORLD_STATE_ADD);
    }
    else
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_hordeScoutGuid))
            scout->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        SendUpdateWorldState(m_uiHordeScoutWorldState, WORLD_STATE_REMOVE);
        m_uiHordeScoutWorldState = WORLD_STATE_HORDE_FLAG_READY;
        SendUpdateWorldState(m_uiHordeScoutWorldState, WORLD_STATE_ADD);
    }
}

void OutdoorPvPZM::ResetScouts(const WorldObject* objRef, Team team, bool includeWorldStates)
{
    if (team == ALLIANCE)
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_allianceScoutGuid))
            scout->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        // reset world states only if requested
        if (includeWorldStates)
        {
            SendUpdateWorldState(m_uiAllianceScoutWorldState, WORLD_STATE_REMOVE);
            m_uiAllianceScoutWorldState = WORLD_STATE_ALLIANCE_FLAG_NOT_READY;
            SendUpdateWorldState(m_uiAllianceScoutWorldState, WORLD_STATE_ADD);
        }
    }
    else
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_hordeScoutGuid))
            scout->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        // reset world states only if requested
        if (includeWorldStates)
        {
            SendUpdateWorldState(m_uiHordeScoutWorldState, WORLD_STATE_REMOVE);
            m_uiHordeScoutWorldState = WORLD_STATE_HORDE_FLAG_NOT_READY;
            SendUpdateWorldState(m_uiHordeScoutWorldState, WORLD_STATE_ADD);
        }
    }
}

bool OutdoorPvPZM::HandleObjectUse(Player* pPlayer, GameObject* pGo)
{
    if (!pPlayer->HasAura(pPlayer->GetTeam() == ALLIANCE ? SPELL_BATTLE_STANDARD_ALLIANCE : SPELL_BATTLE_STANDARD_HORDE))
        return false;

    switch (pGo->GetEntry())
    {
        case GO_ZANGA_BANNER_CENTER_ALLIANCE:
            if (pPlayer->GetTeam() == ALLIANCE)
                return false;

            // change banners
            SetGraveyardArtKit(pGo, m_TowerBannerCenterAllianceGUID, false);
            SetGraveyardArtKit(pGo, m_TowerBannerCenterHordeGUID, true);
            SetBeaconArtKit(pGo, m_BeamCenterBlueGUID, false);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_GY_A));

            // remove buff and graveyard
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING, true);
            SetGraveyard(true);

            // update graveyard owner
            m_graveyardOwner = HORDE;

            // add the buff and the graveyard
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING);
            SetGraveyard();

            SendUpdateWorldState(m_uiGraveyardWorldState, WORLD_STATE_REMOVE);
            m_uiGraveyardWorldState = WORLD_STATE_GRAVEYARD_HORDE;
            SendUpdateWorldState(m_uiGraveyardWorldState, WORLD_STATE_ADD);

            // reset scout and remove player aura
            ResetScouts(pGo, m_graveyardOwner);
            pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_HORDE);
            SetBeaconArtKit(pGo, m_BeamCenterRedGUID, true);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));

            return true;
        case GO_ZANGA_BANNER_CENTER_HORDE:
            if (pPlayer->GetTeam() == HORDE)
                return false;

            // change banners
            SetGraveyardArtKit(pGo, m_TowerBannerCenterHordeGUID, false);
            SetGraveyardArtKit(pGo, m_TowerBannerCenterAllianceGUID, true);
            SetBeaconArtKit(pGo, m_BeamCenterRedGUID, false);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_GY_H));

            // remove buff and graveyard
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING, true);
            SetGraveyard(true);

            // update graveyard owner
            m_graveyardOwner = ALLIANCE;

            // add the buff and the graveyard to horde
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING);
            SetGraveyard();

            SendUpdateWorldState(m_uiGraveyardWorldState, WORLD_STATE_REMOVE);
            m_uiGraveyardWorldState = WORLD_STATE_GRAVEYARD_ALLIANCE;
            SendUpdateWorldState(m_uiGraveyardWorldState, WORLD_STATE_ADD);

            // reset scout and remove player aura
            ResetScouts(pGo, m_graveyardOwner);
            pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_ALLIANCE);
            SetBeaconArtKit(pGo, m_BeamCenterBlueGUID, true);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_A));

            return true;
        case GO_ZANGA_BANNER_CENTER_NEUTRAL:

            // remove old world state
            SendUpdateWorldState(m_uiGraveyardWorldState, WORLD_STATE_REMOVE);

            if (pPlayer->GetTeam() == ALLIANCE)
            {
                // update graveyard owner
                m_graveyardOwner = ALLIANCE;

                // change banners
                SetGraveyardArtKit(pGo, m_TowerBannerCenterNeutralGUID, false);
                SetGraveyardArtKit(pGo, m_TowerBannerCenterAllianceGUID, true);

                // add the buff and the graveyard to horde
                m_uiGraveyardWorldState = WORLD_STATE_GRAVEYARD_ALLIANCE;
                BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING);
                SetGraveyard();

                // reset scout and remove player aura
                ResetScouts(pGo, m_graveyardOwner);
                pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_ALLIANCE);
                SetBeaconArtKit(pGo, m_BeamCenterBlueGUID, true);
                sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));
            }
            else
            {
                // update graveyard owner
                m_graveyardOwner = HORDE;

                // change banners
                SetGraveyardArtKit(pGo, m_TowerBannerCenterNeutralGUID, false);
                SetGraveyardArtKit(pGo, m_TowerBannerCenterHordeGUID, true);

                // add the buff and the graveyard to horde
                m_uiGraveyardWorldState = WORLD_STATE_GRAVEYARD_HORDE;
                BuffTeam(HORDE, SPELL_TWIN_SPIRE_BLESSING);
                SetGraveyard();

                // reset scout and remove player aura
                ResetScouts(pGo, m_graveyardOwner);
                pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_HORDE);
                SetBeaconArtKit(pGo, m_BeamCenterRedGUID, true);
                sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));
            }

            // add new world state
            SendUpdateWorldState(m_uiGraveyardWorldState, WORLD_STATE_ADD);
            return true;
    }

    return false;
}

void OutdoorPvPZM::SetGraveyard(bool remove)
{
    //if (remove)
    //    sObjectMgr.RemoveGraveYardLink(GRAVEYARD_ID_TWIN_SPIRE, GRAVEYARD_ZONE_TWIN_SPIRE, m_graveyardOwner, false);
    //else
    //    sObjectMgr.AddGraveYardLink(GRAVEYARD_ID_TWIN_SPIRE, GRAVEYARD_ZONE_TWIN_SPIRE, m_graveyardOwner, false);
}

void OutdoorPvPZM::SetGraveyardArtKit(const WorldObject* objRef, ObjectGuid goGuid, bool respawn)
{
    if (GameObject* pBanner = objRef->GetMap()->GetGameObject(goGuid))
    {
        if (respawn)
        {
            pBanner->SetRespawnTime(7 * DAY);
            pBanner->Refresh();
        }
        else if (pBanner->isSpawned())
            pBanner->Delete();
    }
}

void OutdoorPvPZM::SetBeaconArtKit(const WorldObject* objRef, ObjectGuid goGuid, bool respawn)
{
    if (Creature* pBeam = objRef->GetMap()->GetCreature(goGuid))
    {
        if (respawn)
            pBeam->Respawn();
        else
            pBeam->ForcedDespawn();
    }
}
