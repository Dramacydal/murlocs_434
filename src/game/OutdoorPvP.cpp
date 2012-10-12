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

/**
   Function that a players to a players set

   @param   player to be added to set
 */
void OutdoorPvP::HandlePlayerEnterZone(Player* pPlayer)
{
    m_sZonePlayers.insert(pPlayer);
}

/**
   Function that a players to a players set

   @param   player to be removed
 */
void OutdoorPvP::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the world state information from the player
    if (!pPlayer->GetSession()->PlayerLogout())
        SendRemoveWorldStates(pPlayer);

    m_sZonePlayers.erase(pPlayer);

    sLog.outDebug("Player %s left an outdoorpvp zone", pPlayer->GetName());
}

/**
   Function that updates world state for all the players in an outdoor pvp area

   @param   world state it to update
   @param   value which should update the world state
 */
void OutdoorPvP::SendUpdateWorldState(uint32 uiField, uint32 uiValue)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
        if (IsMember((*itr)->GetObjectGuid()))
            (*itr)->SendUpdateWorldState(uiField, uiValue);
}

/**
   Function that updates world state for all the players in an outdoor pvp map

   @param   world state it to update
   @param   value which should update the world state
 */
void OutdoorPvP::SendUpdateWorldStateForMap(uint32 uiField, uint32 uiValue, Map* map)
{
    Map::PlayerList const& pList = map->GetPlayers();
    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
    {
        if (!itr->getSource() || !itr->getSource()->IsInWorld())
            continue;

        itr->getSource()->SendUpdateWorldState(uiField, uiValue);
    }
}

/**
   Function that handles the player kill in outdoor pvp

   @param   player which kills another player
   @param   player or unit (pet) which is victim
 */
void OutdoorPvP::HandlePlayerKill(Player* pKiller, Unit* pVictim)
{
    Player* plr = pVictim->GetCharmerOrOwnerPlayerOrPlayerItself();
    if (plr && pKiller->GetTeam() == plr->GetTeam())
        return;

    if (Group* pGroup = pKiller->GetGroup())
    {
        for (GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* pGroupGuy = itr->getSource();

            if (!pGroupGuy)
                continue;

            // skip if too far away
            if (!pGroupGuy->IsAtGroupRewardDistance(pVictim))
                continue;

            // creature kills must be notified, even if not inside objective / not outdoor pvp active
            // player kills only count if active and inside objective
            if (pGroupGuy->CanUseOutdoorCapturePoint())
                HandlePlayerKillInsideArea(pGroupGuy, pVictim);
        }
    }
    else
    {
        // creature kills must be notified, even if not inside objective / not outdoor pvp active
        if (pKiller && pKiller->CanUseOutdoorCapturePoint())
            HandlePlayerKillInsideArea(pKiller, pVictim);
    }
}

// register this zone as an outdoor pvp script
void OutdoorPvP::RegisterZone(uint32 uiZoneId)
{
    sOutdoorPvPMgr.AddZone(uiZoneId, this);
}

// return if has player inside the zone
bool OutdoorPvP::HasPlayer(Player* pPlayer) const
{
    return m_sZonePlayers.find(pPlayer) != m_sZonePlayers.end();
}

// set a capture point slider value for when the gameobject is being reloaded the next time
void OutdoorPvP::SetCapturePointSliderValue(uint32 entry, CapturePointSlider value)
{
    sOutdoorPvPMgr.SetCapturePointSlider(entry, value);
}

// apply a team buff for the specific zone (and area)
void OutdoorPvP::BuffTeam(Team team, uint32 spellId, bool remove, bool onlyMembers, uint32 area)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if ((*itr) && (*itr)->IsInWorld() && (team == TEAM_NONE || (*itr)->GetTeam() == team) && (!onlyMembers || IsMember((*itr)->GetObjectGuid())))
        {
            if (!area || area == (*itr)->GetAreaId())
            {
                if (remove)
                    (*itr)->RemoveAurasDueToSpell(spellId);
                else
                    (*itr)->CastSpell(*itr, spellId, true);
            }
        }
    }
}

uint32 OutdoorPvP::GetBannerArtKit(Team team, uint32 artKitAlliance, uint32 artKitHorde, uint32 artKitNeutral)
{
    switch (team)
    {
        case ALLIANCE:
            return artKitAlliance;
        case HORDE:
            return artKitHorde;
        default:
            return artKitNeutral;
    }
}

void OutdoorPvP::SetBannerVisual(const WorldObject* objRef, ObjectGuid goGuid, uint32 artKit, uint32 animId)
{
    if (GameObject* go = objRef->GetMap()->GetGameObject(goGuid))
        SetBannerVisual(go, artKit, animId);
}

void OutdoorPvP::SetBannerVisual(GameObject* go, uint32 artKit, uint32 animId)
{
    go->SendGameObjectCustomAnim(go->GetObjectGuid(), animId);
    go->SetGoArtKit(artKit);
    go->Refresh();
}
