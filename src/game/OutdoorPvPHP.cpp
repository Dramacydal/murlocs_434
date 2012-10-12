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
#include "OutdoorPvPHP.h"
#include "GameObject.h"


OutdoorPvPHP::OutdoorPvPHP(uint8 _id) : OutdoorPvP(_id),
    m_uiTowersAlliance(0),
    m_uiTowersHorde(0)
{
    m_uiTowerWorldState[0] = WORLD_STATE_OVERLOOK_NEUTRAL;
    m_uiTowerWorldState[1] = WORLD_STATE_STADIUM_NEUTRAL;
    m_uiTowerWorldState[2] = WORLD_STATE_BROKEN_HILL_NEUTRAL;

    for (uint8 i = 0; i < MAX_HP_TOWERS; ++i)
        m_capturePointOwner[i] = TEAM_NONE;
}

bool OutdoorPvPHP::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_HELLFIRE_PENINSULA);
    RegisterZone(ZONE_ID_HELLFIRE_CITADEL);
    RegisterZone(ZONE_ID_HELLFIRE_RAMPARTS);
    RegisterZone(ZONE_ID_BLOOD_FURNACE);
    RegisterZone(ZONE_ID_SHATTERED_HALLS);
    RegisterZone(ZONE_ID_MAGTHERIDON_LAIR);

    return true;
}

void OutdoorPvPHP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HP_ALLIANCE, m_uiTowersAlliance);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HP_HORDE, m_uiTowersHorde);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_DISPLAY_HP_A, WORLD_STATE_ADD);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_DISPLAY_HP_H, WORLD_STATE_ADD);

    for (uint8 i = 0; i < MAX_HP_TOWERS; ++i)
        FillInitialWorldState(data, count, m_uiTowerWorldState[i], WORLD_STATE_ADD);
}

void OutdoorPvPHP::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_DISPLAY_HP_A, WORLD_STATE_REMOVE);
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_DISPLAY_HP_H, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_HP_TOWERS; ++i)
        pPlayer->SendUpdateWorldState(m_uiTowerWorldState[i], WORLD_STATE_REMOVE);
}

void OutdoorPvPHP::UpdateWorldState()
{
    // update only tower count; tower states are sent in the process event
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HP_ALLIANCE, m_uiTowersAlliance);
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HP_HORDE, m_uiTowersHorde);
}

void OutdoorPvPHP::HandlePlayerEnterZone(Player* pPlayer)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? SPELL_HELLFIRE_SUPERIORITY_ALLIANCE : SPELL_HELLFIRE_SUPERIORITY_HORDE);

    // buff the player if same team is controlling all capture points
    if (m_uiTowersAlliance == MAX_HP_TOWERS && pPlayer->GetTeam() == ALLIANCE)
        pPlayer->CastSpell(pPlayer, SPELL_HELLFIRE_SUPERIORITY_ALLIANCE, true);
    else if (m_uiTowersHorde == MAX_HP_TOWERS && pPlayer->GetTeam() == HORDE)
        pPlayer->CastSpell(pPlayer, SPELL_HELLFIRE_SUPERIORITY_HORDE, true);

    OutdoorPvP::HandlePlayerEnterZone(pPlayer);
}

void OutdoorPvPHP::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? SPELL_HELLFIRE_SUPERIORITY_ALLIANCE : SPELL_HELLFIRE_SUPERIORITY_HORDE);

    OutdoorPvP::HandlePlayerLeaveZone(pPlayer);
}

void OutdoorPvPHP::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_TOWER_BANNER_OVERLOOK:
            m_bannersGuid[0] = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[0], GO_ARTKIT_OVERLOOK_ALLIANCE, GO_ARTKIT_OVERLOOK_HORDE, GO_ARTKIT_OVERLOOK_NEUTRAL));
            break;
        case GO_TOWER_BANNER_STADIUM:
            m_bannersGuid[1] = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[1], GO_ARTKIT_STADIUM_ALLIANCE, GO_ARTKIT_STADIUM_HORDE, GO_ARTKIT_STADIUM_NEUTRAL));
            break;
        case GO_TOWER_BANNER_BROKEN_HILL:
            m_bannersGuid[2] = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[2], GO_ARTKIT_BROKEN_HILL_ALLIANCE, GO_ARTKIT_BROKEN_HILL_HORDE, GO_ARTKIT_BROKEN_HILL_NEUTRAL));
            break;
        case GO_HELLFIRE_BANNER_OVERLOOK:
            m_capturePointGuid[0] = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[0], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
        case GO_HELLFIRE_BANNER_STADIUM:
            m_capturePointGuid[1] = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[1], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
        case GO_HELLFIRE_BANNER_BROKEN_HILL:
            m_capturePointGuid[2] = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GetBannerArtKit(m_capturePointOwner[2], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
    }
}

void OutdoorPvPHP::HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team team)
{
    uint32 uiCredit = 0;

    switch (uiEventId)
    {
        case EVENT_OVERLOOK_PROGRESS_ALLIANCE:
        case EVENT_OVERLOOK_PROGRESS_HORDE:
            uiCredit = NPC_CAPTURE_CREDIT_OVERLOOK;
            break;
        case EVENT_STADIUM_PROGRESS_ALLIANCE:
        case EVENT_STADIUM_PROGRESS_HORDE:
            uiCredit = NPC_CAPTURE_CREDIT_STADIUM;
            break;
        case EVENT_BROKEN_HILL_PROGRESS_ALLIANCE:
        case EVENT_BROKEN_HILL_PROGRESS_HORDE:
            uiCredit = NPC_CAPTURE_CREDIT_BROKEN_HILL;
            break;
    }

    if (!uiCredit)
        return;

    for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetTeam() == team)
        {
            (*itr)->KilledMonsterCredit(uiCredit);
            (*itr)->RewardHonor(NULL, 1, HONOR_REWARD_HELLFIRE);
        }
    }
}

// Cast player spell on opponent kill
void OutdoorPvPHP::HandlePlayerKillInsideArea(Player* pPlayer, Unit* pVictim)
{
    for (uint8 i = 0; i < MAX_HP_TOWERS; ++i)
    {
        if (GameObject* capturePoint = pPlayer->GetMap()->GetGameObject(m_capturePointGuid[i]))
        {
            // check capture point range
            GameObjectInfo const* info = capturePoint->GetGOInfo();
            if (info && pPlayer->IsWithinDistInMap(capturePoint, info->capturePoint.radius))
            {
                // check capture point team
                if (pPlayer->GetTeam() == m_capturePointOwner[i])
                    pPlayer->CastSpell(pPlayer, pPlayer->GetTeam() == ALLIANCE ? SPELL_HELLFIRE_TOWER_TOKEN_ALLIANCE : SPELL_HELLFIRE_TOWER_TOKEN_HORDE, true);

                return;
            }
        }
    }
}

// process the capture events
void OutdoorPvPHP::ProcessEvent(uint32 uiEventId, GameObject* pGo, Player* pInvoker, uint32 spellId)
{
    for (uint8 i = 0; i < MAX_HP_TOWERS; ++i)
    {
        if (aHellfireBanners[i] == pGo->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (aHellfireTowerEvents[i][j].uiEventEntry == uiEventId)
                {
                    if (aHellfireTowerEvents[i][j].team != m_capturePointOwner[i])
                    {
                        ProcessCaptureEvent(pGo, i, aHellfireTowerEvents[i][j].team, aHellfireTowerEvents[i][j].uiWorldState, aHellfireTowerEvents[i][j].uiTowerArtKit, aHellfireTowerEvents[i][j].uiTowerAnim);
                        sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(aHellfireTowerEvents[i][j].uiZoneText));
                    }
                    return;
                }
            }
            return;
        }
    }
}

void OutdoorPvPHP::ProcessCaptureEvent(GameObject* pGo, uint32 uiTowerId, Team team, uint32 uiNewWorldState, uint32 uiTowerArtKit, uint32 uiTowerAnim)
{
    // set artkits and process buffs
    if (team == ALLIANCE)
    {
        SetBannerVisual(pGo, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);
        ++m_uiTowersAlliance;

        if (m_uiTowersAlliance == MAX_HP_TOWERS)
            BuffTeam(ALLIANCE, SPELL_HELLFIRE_SUPERIORITY_ALLIANCE);
    }
    else if (team == HORDE)
    {
        SetBannerVisual(pGo, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);
        ++m_uiTowersHorde;

        if (m_uiTowersHorde == MAX_HP_TOWERS)
            BuffTeam(HORDE, SPELL_HELLFIRE_SUPERIORITY_HORDE);
    }
    else
    {
        SetBannerVisual(pGo, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        if (m_capturePointOwner[uiTowerId] == ALLIANCE)
        {
            if (m_uiTowersAlliance == MAX_HP_TOWERS)
                BuffTeam(ALLIANCE, SPELL_HELLFIRE_SUPERIORITY_ALLIANCE, true);

            --m_uiTowersAlliance;
        }
        else
        {
            if (m_uiTowersHorde == MAX_HP_TOWERS)
                BuffTeam(HORDE, SPELL_HELLFIRE_SUPERIORITY_HORDE, true);

            --m_uiTowersHorde;
        }
    }

    SetBannerVisual(pGo, m_bannersGuid[uiTowerId], uiTowerArtKit, uiTowerAnim);

    // update tower state
    SendUpdateWorldState(m_uiTowerWorldState[uiTowerId], WORLD_STATE_REMOVE);
    m_uiTowerWorldState[uiTowerId] = uiNewWorldState;
    SendUpdateWorldState(m_uiTowerWorldState[uiTowerId], WORLD_STATE_ADD);

    // update counter state
    UpdateWorldState();

    // update capture point owner
    m_capturePointOwner[uiTowerId] = team;
}
