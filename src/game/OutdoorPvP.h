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

#ifndef OUTDOOR_PVP_H
#define OUTDOOR_PVP_H

#include "OutdoorPvPMgr.h"
#include "Language.h"
#include "World.h"
#include "ZoneScript.h"
#include "Player.h"
#include "Common.h"
#include "Policies/Singleton.h"
#include "SharedDefines.h"
#include "GameObject.h"
#include "ObjectMgr.h"

enum CapturePointArtKits
{
    CAPTURE_ARTKIT_ALLIANCE = 2,
    CAPTURE_ARTKIT_HORDE    = 1,
    CAPTURE_ARTKIT_NEUTRAL  = 21
};

enum CapturePointAnimations
{
    CAPTURE_ANIM_ALLIANCE   = 1,
    CAPTURE_ANIM_HORDE      = 0,
    CAPTURE_ANIM_NEUTRAL    = 2
};

enum
{
    OUTDOOR_PVP_EP      = 0,
    OUTDOOR_PVP_SI      = 1,
    OUTDOOR_PVP_HP      = 2,
    OUTDOOR_PVP_NA      = 3,
    OUTDOOR_PVP_TF      = 4,
    OUTDOOR_PVP_ZM      = 5,
    OUTDOOR_PVP_GH      = 6,
    OUTDOOR_PVP_WG      = 7,
    OUTDOOR_PVP_COUNT   = 8
};

typedef std::set<Player*> PlayerSet;

class OutdoorPvP : public ZoneScript
{
    friend class OutdoorPvPMgr;

    public:
        OutdoorPvP(uint8 _id) : m_id(_id), m_BattlefieldId(0) { }
        ~OutdoorPvP() {}

        // called when the zone is initialized
        virtual void FillInitialWorldStates(WorldPacket& /*data*/, uint32& /*count*/) {}

        // called when a player triggers an areatrigger
        virtual bool HandleAreaTrigger(Player* /*pPlayer*/, uint32 /*uiTriggerId*/) { return false; }

        // called when a playerd drops a flag
        virtual bool HandleDropFlag(Player* /*pPlayer*/, uint32 /*uiSpellId*/) { return false; }

        // called when a playerd uses a gameobject related to world pvp events
        virtual bool HandleObjectUse(Player* /*pPlayer*/, GameObject* /*pGo*/) { return false; }

        // handle npc/player kill
        virtual void HandlePlayerKillInsideArea(Player* /*pKiller*/, Unit* /*pVictim*/) {}

        // handle capture objective complete
        virtual void HandleObjectiveComplete(uint32 /*uiEventId*/, std::list<Player*> /*players*/, Team /*team*/) {}

        // check if player can damage GO in outdoor pvp zone
        virtual bool CanDamageGO(GameObject* pGo, Player* invoker) { return true; }

        // init all the outdoor pvp area relates stuff
        virtual bool InitOutdoorPvPArea() { return false; }

        // send world state update to all players present
        void SendUpdateWorldState(uint32 field, uint32 value);

        // send world state update to all players present in map
        void SendUpdateWorldStateForMap(uint32 field, uint32 value, Map* map);

        // awards rewards for player kill
        virtual void AwardKillBonus(Player* /*pPlayer*/) {}

        // update - called by the OutdoorPvPMgr
        virtual void Update(uint32 diff) {}

        virtual bool IsMember(ObjectGuid guid) { return true; }

        // applies buff to a team inside the specific zone
        void BuffTeam(Team team, uint32 spellId, bool remove = false, bool onlyMembers = true, uint32 area = 0);

        // get banner artkit based on controlling team
        uint32 GetBannerArtKit(Team team, uint32 artKitAlliance, uint32 artKitHorde, uint32 artKitNeutral);

        // set banner visual
        void SetBannerVisual(const WorldObject* objRef, ObjectGuid goGuid, uint32 artKit, uint32 animId);
        void SetBannerVisual(GameObject* go, uint32 artKit, uint32 animId);

        uint8 GetId() const { return m_id; }

        uint8 GetBatllefieldId() const { return m_BattlefieldId; }

    protected:
        // Player related stuff
        virtual void HandlePlayerEnterZone(Player* pPlayer);
        virtual void HandlePlayerLeaveZone(Player* pPlayer);
        virtual void HandlePlayerEnterArea(Player* pPlayer, uint32 uiAreaId) { };
        virtual void HandlePlayerLeaveArea(Player* pPlayer, uint32 uiAreaId) { };
        virtual void HandlePlayerKill(Player* pKiller, Unit* pVictim);

        // remove world states
        virtual void SendRemoveWorldStates(Player* pPlayer) {}

        void RegisterZone(uint32 zoneId);
        bool HasPlayer(Player* pPlayer) const;
        void SetCapturePointSliderValue(uint32 entry, CapturePointSlider value);

        // store the players inside the area depending on the team
        PlayerSet m_sZonePlayers;

        uint8 m_id;
        uint32 m_BattlefieldId;
};

#endif
