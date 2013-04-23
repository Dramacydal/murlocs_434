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
#ifndef __BATTLEGROUNDRV_H
#define __BATTLEGROUNDRV_H

class BattleGround;

enum BG_RV_Events
{
    RV_EVENT_SPAWN_STATIC_GO    = 5,
    RV_EVENT_SPAWN_PILLARS      = 6,
    RV_EVENT_SPAWN_GEARS        = 7,
    RV_EVENT_OP_DOOR            = 254
};

enum BattlegroundRVData
{
    BG_RV_PILLAR_TIMER          = 25 * IN_MILLISECONDS,
    BG_RV_PILLAR_TIMER_RANDOM   = 0 * IN_MILLISECONDS,
    BG_RV_GEAR_TIME_DIFF        = 3 * IN_MILLISECONDS,
    BG_RV_LOS_DELAY             = 1 * IN_MILLISECONDS,

    BG_RV_LIFT_TIME             = 15 * IN_MILLISECONDS,
};

enum
{
    BG_RV_GO_PILLAR             = 208469,
    BG_RV_GO_LIGHTNING_PILLAR   = 208470,
    BG_RV_GO_IVORY_PILLAR       = 208471,
    BG_RV_GO_AXE_PILLAR         = 208468,
};

class BattleGroundRVScore : public BattleGroundScore
{
    public:
        BattleGroundRVScore() {};
        virtual ~BattleGroundRVScore() {};
};

class BattleGroundRV : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        BattleGroundRV();
        ~BattleGroundRV();
        void Update(uint32 diff) override;

        /* inherited from BattlegroundClass */
        virtual void AddPlayer(Player *plr) override;
        virtual void StartingEventCloseDoors() override;
        virtual void StartingEventOpenDoors() override;

        void RemovePlayer(Player *plr, ObjectGuid guid) override;
        void HandleAreaTrigger(Player *Source, uint32 Trigger) override;
        bool SetupBattleGround() override;
        virtual void Reset() override;
        virtual void FillInitialWorldStates(WorldPacket &d, uint32& count) override;
        void HandleKillPlayer(Player* player, Player *killer) override;
        bool HandlePlayerUnderMap(Player * plr) override;

        void OnObjectDBLoad(GameObject* go) override;
        bool IsWithinLOSInBG(float x1, float y1, float z1, float x2, float y2, float z2);

        void HandleGameObjectCreate(GameObject* go) override;
    private:
        void UpdatePlayerVisibilities();
        void CheckBuggers();

        uint32 m_uiPillarSwitchTimer;
        uint32 m_uiHorizontalLosDelay;
        uint32 m_uiVerticalLosDelay;

        bool m_bHorizontalPillarsUp;
        bool m_bVerticalPillarsUp;
        bool m_bGearsSwitched;
};

#endif
