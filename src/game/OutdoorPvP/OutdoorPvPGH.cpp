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
#include "OutdoorPvPGH.h"


OutdoorPvPGH::OutdoorPvPGH(uint8 _id) : OutdoorPvP(_id),
    m_zoneOwner(TEAM_NONE)
{
}

bool OutdoorPvPGH::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_GRIZZLY_HILLS);

    return true;
}

void OutdoorPvPGH::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_WESTFALL_BRIGADE_DEFENDER:
        case NPC_COMMANDER_HOWSER:
            lAllianceSoldiers.push_back(pCreature->GetObjectGuid());
            if (m_zoneOwner == ALLIANCE)
                return;
            break;
        case NPC_BLACKSMITH_JASON_RIGGINS:
        case NPC_STABLE_MASTER_TIM:
        case NPC_VENDOR_ADAMS:
        case NPC_HORSE:
            // check the zone id because the horses can be found in other areas too
            if (pCreature->GetZoneId() == ZONE_ID_GRIZZLY_HILLS)
                lAllianceVendors.push_back(pCreature->GetObjectGuid());
            if (m_zoneOwner == ALLIANCE)
                return;
            break;
        case NPC_CONQUEST_HOLD_DEFENDER:
        case NPC_GENERAL_GORLOK:
            lHordeSoldiers.push_back(pCreature->GetObjectGuid());
            if (m_zoneOwner == HORDE)
                return;
            break;
        case NPC_BLACKSMITH_KOLOTH:
        case NPC_STABLE_MASTER_KOR:
        case NPC_VENDOR_PURKOM:
        case NPC_RIDING_WOLF:
            // check the zone id because the wolfs can be found in other areas too
            if (pCreature->GetZoneId() == ZONE_ID_GRIZZLY_HILLS)
                lHordeVendors.push_back(pCreature->GetObjectGuid());
            if (m_zoneOwner == HORDE)
                return;
            break;

        default:
            return;
    }

    // Despawn creatures on create - will be spawned later in script
    pCreature->SetRespawnDelay(7 * DAY);
    pCreature->ForcedDespawn();
}

void OutdoorPvPGH::OnGameObjectCreate(GameObject* pGo)
{
    if (pGo->GetEntry() == GO_VENTURE_BAY_LIGHTHOUSE)
        pGo->SetGoArtKit(GetBannerArtKit(m_zoneOwner, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
}

// process the capture events
void OutdoorPvPGH::ProcessEvent(uint32 uiEventId, GameObject* pGo, Player* pInvoker, uint32 spellId)
{
    // If we are not using the lighthouse return
    if (pGo->GetEntry() != GO_VENTURE_BAY_LIGHTHOUSE)
        return;

    switch (uiEventId)
    {
        case EVENT_LIGHTHOUSE_WIN_ALLIANCE:
            // Spawn the npcs only when the tower is fully controlled
            m_zoneOwner = ALLIANCE;
            RespawnSoldiers(pGo);
            break;
        case EVENT_LIGHTHOUSE_WIN_HORDE:
            // Spawn the npcs only when the tower is fully controlled
            m_zoneOwner = HORDE;
            RespawnSoldiers(pGo);
            break;
        case EVENT_LIGHTHOUSE_PROGRESS_ALLIANCE:
            SetBannerVisual(pGo, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);
            break;
        case EVENT_LIGHTHOUSE_PROGRESS_HORDE:
            SetBannerVisual(pGo, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);
            break;
        case EVENT_LIGHTHOUSE_NEUTRAL_ALLIANCE:
        case EVENT_LIGHTHOUSE_NEUTRAL_HORDE:
            m_zoneOwner = TEAM_NONE;
            SetBannerVisual(pGo, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
            break;
    }
}

void OutdoorPvPGH::RespawnSoldiers(const WorldObject* objRef)
{
    if (m_zoneOwner == ALLIANCE)
    {
        // despawn all horde vendors
        for (std::list<ObjectGuid>::const_iterator itr = lHordeVendors.begin(); itr != lHordeVendors.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
                pSoldier->ForcedDespawn();
        }

        // spawn all alliance soldiers and vendors
        for (std::list<ObjectGuid>::const_iterator itr = lAllianceSoldiers.begin(); itr != lAllianceSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
                pSoldier->Respawn();
        }
        for (std::list<ObjectGuid>::const_iterator itr = lAllianceVendors.begin(); itr != lAllianceVendors.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
                pSoldier->Respawn();
        }
    }
    else
    {
        // despawn all alliance vendors
        for (std::list<ObjectGuid>::const_iterator itr = lAllianceVendors.begin(); itr != lAllianceVendors.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
                pSoldier->ForcedDespawn();
        }

        // spawn all horde soldiers and vendors
        for (std::list<ObjectGuid>::const_iterator itr = lHordeSoldiers.begin(); itr != lHordeSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
                pSoldier->Respawn();
        }
        for (std::list<ObjectGuid>::const_iterator itr = lHordeVendors.begin(); itr != lHordeVendors.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
                pSoldier->Respawn();
        }
    }
}
