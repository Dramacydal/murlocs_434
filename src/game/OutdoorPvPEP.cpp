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
#include "OutdoorPvPEP.h"
#include "GameObject.h"


OutdoorPvPEP::OutdoorPvPEP(uint8 _id) : OutdoorPvP(_id),
    m_uiTowersAlliance(0),
    m_uiTowersHorde(0)
{
    m_uiTowerWorldState[0] = WORLD_STATE_NORTHPASS_NEUTRAL;
    m_uiTowerWorldState[1] = WORLD_STATE_CROWNGUARD_NEUTRAL;
    m_uiTowerWorldState[2] = WORLD_STATE_EASTWALL_NEUTRAL;
    m_uiTowerWorldState[3] = WORLD_STATE_PLAGUEWOOD_NEUTRAL;

    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        m_capturePointOwner[i] = TEAM_NONE;
}

bool OutdoorPvPEP::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_EASTERN_PLAGUELANDS);
    RegisterZone(ZONE_ID_STRATHOLME);
    RegisterZone(ZONE_ID_SCHOLOMANCE);

    return true;
}

void OutdoorPvPEP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_ALLIANCE, m_uiTowersAlliance);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HORDE, m_uiTowersHorde);

    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        FillInitialWorldState(data, count, m_uiTowerWorldState[i], WORLD_STATE_ADD);
}

void OutdoorPvPEP::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALLIANCE, WORLD_STATE_REMOVE);
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        pPlayer->SendUpdateWorldState(m_uiTowerWorldState[i], WORLD_STATE_REMOVE);
}

void OutdoorPvPEP::UpdateWorldState()
{
    // update only tower count; tower states are sent in the process event
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALLIANCE, m_uiTowersAlliance);
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE, m_uiTowersHorde);
}

void OutdoorPvPEP::HandlePlayerEnterZone(Player* pPlayer)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? m_aPlaguelandsTowerBuffs[i].uiSpellIdAlliance : m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde);

    // buff the player
    switch (pPlayer->GetTeam())
    {
        case ALLIANCE:
            if (m_uiTowersAlliance > 0)
                pPlayer->CastSpell(pPlayer, m_aPlaguelandsTowerBuffs[m_uiTowersAlliance - 1].uiSpellIdAlliance, true);
            break;
        case HORDE:
            if (m_uiTowersHorde > 0)
                pPlayer->CastSpell(pPlayer, m_aPlaguelandsTowerBuffs[m_uiTowersHorde - 1].uiSpellIdHorde, true);
            break;
    }

    OutdoorPvP::HandlePlayerEnterZone(pPlayer);
}

void OutdoorPvPEP::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? m_aPlaguelandsTowerBuffs[i].uiSpellIdAlliance : m_aPlaguelandsTowerBuffs[i].uiSpellIdHorde);

    OutdoorPvP::HandlePlayerLeaveZone(pPlayer);
}

void OutdoorPvPEP::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_1:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_2:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_3:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_4:
        case GO_TOWER_BANNER:
            // sort banners
            if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[0].m_fX, m_aTowersSpawnLocs[0].m_fY, 50.0f))
            {
                m_lTowerBanners[0].push_back(pGo->GetObjectGuid());
                pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[0], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[1].m_fX, m_aTowersSpawnLocs[1].m_fY, 50.0f))
            {
                m_lTowerBanners[1].push_back(pGo->GetObjectGuid());
                pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[1], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[2].m_fX, m_aTowersSpawnLocs[2].m_fY, 50.0f))
            {
                m_lTowerBanners[2].push_back(pGo->GetObjectGuid());
                pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[2], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            else if (pGo->IsWithinDist2d(m_aTowersSpawnLocs[3].m_fX, m_aTowersSpawnLocs[3].m_fY, 50.0f))
            {
                m_lTowerBanners[3].push_back(pGo->GetObjectGuid());
                pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[3], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            break;
        case GO_LORDAERON_SHRINE_ALLIANCE:
            m_uiLordaeronShrineAllianceGUID = pGo->GetObjectGuid();
            break;
        case GO_LORDAERON_SHRINE_HORDE:
            m_uiLordaeronShrineHordeGUID = pGo->GetObjectGuid();
            break;
    }
}

void OutdoorPvPEP::HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team team)
{
    uint32 uiCredit = 0;

    switch (uiEventId)
    {
        case EVENT_CROWNGUARD_PROGRESS_ALLIANCE:
        case EVENT_CROWNGUARD_PROGRESS_HORDE:
            uiCredit = NPC_CROWNGUARD_TOWER_QUEST_DOODAD;
            break;
        case EVENT_EASTWALL_PROGRESS_ALLIANCE:
        case EVENT_EASTWALL_PROGRESS_HORDE:
            uiCredit = NPC_EASTWALL_TOWER_QUEST_DOODAD;
            break;
        case EVENT_NORTHPASS_PROGRESS_ALLIANCE:
        case EVENT_NORTHPASS_PROGRESS_HORDE:
            uiCredit = NPC_NORTHPASS_TOWER_QUEST_DOODAD;
            break;
        case EVENT_PLAGUEWOOD_PROGRESS_ALLIANCE:
        case EVENT_PLAGUEWOOD_PROGRESS_HORDE:
            uiCredit = NPC_PLAGUEWOOD_TOWER_QUEST_DOODAD;
            break;
    }

    if (!uiCredit)
        return;

    for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetTeam() == team)
        {
            (*itr)->KilledMonsterCredit(uiCredit);
            (*itr)->RewardHonor(NULL, 1, HONOR_REWARD_PLAGUELANDS);
        }
    }
}

// process the capture events
void OutdoorPvPEP::ProcessEvent(uint32 uiEventId, GameObject* pGo, Player* pInvoker, uint32 spellId)
{
    for (uint8 i = 0; i < TOWER_COUNT; ++i)
    {
        if (aPlaguelandsBanners[i] == pGo->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (aPlaguelandsTowerEvents[i][j].uiEventEntry == uiEventId)
                {
                    if (aPlaguelandsTowerEvents[i][j].team != m_capturePointOwner[i])
                    {
                        ProcessCaptureEvent(pGo, i, aPlaguelandsTowerEvents[i][j].team, aPlaguelandsTowerEvents[i][j].uiWorldState);
                        sWorld.SendZoneText(ZONE_ID_EASTERN_PLAGUELANDS, sObjectMgr.GetMangosStringForDBCLocale(aPlaguelandsTowerEvents[i][j].uiZoneText));
                    }
                    return;
                }
            }
            return;
        }
    }
}

void OutdoorPvPEP::ProcessCaptureEvent(GameObject* pGo, uint32 uiTowerId, Team team, uint32 uiNewWorldState)
{
    if (team == ALLIANCE)
    {
        for (std::list<ObjectGuid>::iterator itr = m_lTowerBanners[uiTowerId].begin(); itr != m_lTowerBanners[uiTowerId].end(); ++itr)
            SetBannerVisual(pGo, (*itr), CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

        ++m_uiTowersAlliance;
        BuffTeam(ALLIANCE, m_aPlaguelandsTowerBuffs[m_uiTowersAlliance - 1].uiSpellIdAlliance);
    }
    else if (team == HORDE)
    {
        for (std::list<ObjectGuid>::iterator itr = m_lTowerBanners[uiTowerId].begin(); itr != m_lTowerBanners[uiTowerId].end(); ++itr)
            SetBannerVisual(pGo, (*itr), CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);

        ++m_uiTowersHorde;
        BuffTeam(HORDE, m_aPlaguelandsTowerBuffs[m_uiTowersHorde - 1].uiSpellIdHorde);
    }
    else
    {
        for (std::list<ObjectGuid>::iterator itr = m_lTowerBanners[uiTowerId].begin(); itr != m_lTowerBanners[uiTowerId].end(); ++itr)
            SetBannerVisual(pGo, (*itr), CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        if (m_capturePointOwner[uiTowerId] == ALLIANCE)
        {
            if (--m_uiTowersAlliance == 0)
                BuffTeam(ALLIANCE, m_aPlaguelandsTowerBuffs[0].uiSpellIdAlliance, true);
        }
        else
        {
            if (--m_uiTowersHorde == 0)
                BuffTeam(HORDE, m_aPlaguelandsTowerBuffs[0].uiSpellIdHorde, true);
        }
    }

    // handle rewards of each tower
    if (team != TEAM_NONE)
    {
        // update capture point owner
        m_capturePointOwner[uiTowerId] = team;

        switch (uiTowerId)
        {
            case TOWER_ID_NORTHPASS:
                UpdateShrine(pGo);
                break;
            case TOWER_ID_CROWNGUARD:
                SetGraveyard();
                break;
            case TOWER_ID_EASTWALL:
                if (m_capturePointOwner[TOWER_ID_NORTHPASS] != team)
                    SummonSoldiers(pGo);
                break;
            case TOWER_ID_PLAGUEWOOD:
                SummonFlightMaster(pGo);
                break;
        }
    }
    else
    {
        switch (uiTowerId)
        {
            case TOWER_ID_NORTHPASS:
                UpdateShrine(pGo, true);
                break;
            case TOWER_ID_CROWNGUARD:
                SetGraveyard(true);
                break;
            case TOWER_ID_EASTWALL:
                UnsummonSoldiers(pGo);
                break;
            case TOWER_ID_PLAGUEWOOD:
                UnsummonFlightMaster(pGo);
                break;
        }

        // update capture point owner
        m_capturePointOwner[uiTowerId] = team;
    }

    // update tower state
    SendUpdateWorldState(m_uiTowerWorldState[uiTowerId], WORLD_STATE_REMOVE);
    m_uiTowerWorldState[uiTowerId] = uiNewWorldState;
    SendUpdateWorldState(m_uiTowerWorldState[uiTowerId], WORLD_STATE_ADD);

    // update counter state
    UpdateWorldState();
}

void OutdoorPvPEP::SummonFlightMaster(WorldObject* objRef)
{
    if (Creature* pFlightMaster = objRef->SummonCreature(NPC_SPECTRAL_FLIGHTMASTER, aFlightmasterSpawnLocs[0], aFlightmasterSpawnLocs[1], aFlightmasterSpawnLocs[2], aFlightmasterSpawnLocs[3], TEMPSUMMON_DEAD_DESPAWN, 0))
        m_uiFlightMasterGUID = pFlightMaster->GetObjectGuid();
}

void OutdoorPvPEP::UnsummonFlightMaster(const WorldObject* objRef)
{
    if (Creature* pFlightMaster = objRef->GetMap()->GetCreature(m_uiFlightMasterGUID))
        pFlightMaster->ForcedDespawn();
}

void OutdoorPvPEP::SummonSoldiers(WorldObject* objRef)
{
    uint32 uiEntry = m_capturePointOwner[TOWER_ID_NORTHPASS] == ALLIANCE ? NPC_LORDAERON_COMMANDER : NPC_LORDAERON_VETERAN;

    for (uint8 i = 0; i < 5; ++i)
    {
        if (Creature* pSoldier = objRef->SummonCreature(uiEntry, m_aPlaguelandSoldiersSpawnLocs[i].m_fX, m_aPlaguelandSoldiersSpawnLocs[i].m_fY, m_aPlaguelandSoldiersSpawnLocs[i].m_fZ, 2.2f, TEMPSUMMON_DEAD_DESPAWN, 0))
            m_lSoldiersGuids.push_back(pSoldier->GetObjectGuid());

        // change the entry id to the soldiers for the last 4 iterations
        if (i == 0)
            uiEntry = uiEntry == NPC_LORDAERON_COMMANDER ? NPC_LORDAERON_SOLDIER : NPC_LORDAERON_FIGHTER;
    }
}

void OutdoorPvPEP::UnsummonSoldiers(const WorldObject* objRef)
{
    for (std::list<ObjectGuid>::iterator itr = m_lSoldiersGuids.begin(); itr != m_lSoldiersGuids.end(); ++itr)
    {
        if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
            pSoldier->ForcedDespawn();
    }
}

void OutdoorPvPEP::SetGraveyard(bool remove)
{
    //if (remove)
    //    sObjectMgr.RemoveGraveYardLink(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, m_capturePointOwner[TOWER_ID_CROWNGUARD], false);
    //else
    //    sObjectMgr.AddGraveYardLink(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, m_capturePointOwner[TOWER_ID_CROWNGUARD], false);
}

void OutdoorPvPEP::UpdateShrine(const WorldObject* objRef, bool remove)
{
    ObjectGuid shrineGuid = m_capturePointOwner[TOWER_ID_NORTHPASS] == ALLIANCE ? m_uiLordaeronShrineAllianceGUID : m_uiLordaeronShrineHordeGUID;
    if (GameObject* shrine = objRef->GetMap()->GetGameObject(shrineGuid))
    {
        if (!remove)
        {
            shrine->SetRespawnTime(7 * DAY);
            shrine->Refresh();
        }
        else if (shrine->isSpawned())
            shrine->Delete();
    }
}
