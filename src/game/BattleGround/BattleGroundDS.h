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
#ifndef __BATTLEGROUNDDS_H
#define __BATTLEGROUNDDS_H

class BattleGround;

enum BG_DS_Events
{
    DS_EVENT_WATERFALL_VISUAL       = 5,
    DS_EVENT_WATERFALL_COLLISION    = 6,
    DS_EVENT_WATER_SPROUTS          = 7,
    DS_EVENT_OP_DOOR                = 254
};

enum BattlegroundDSData
{
    BG_DS_WATERFALL_TIMER_START                 = 20000,
    BG_DS_WATERFALL_TIMER                       = 35000,

    BG_DS_WATERFALL_DURATION                    = 30000,

    BG_DS_COLLISION_SPAWN_TIMER                 = 7000,

    BG_DS_WATERFALL_VISUAL_ENTRY                = 191877,
    BG_DS_WATERFALL_COLLISION_ENTRY             = 194395,

    KNOCKBACK_LOC_PIPE1                         = 0,
    KNOCKBACK_LOC_PIPE2                         = 1,
    KNOCKBACK_LOC_WATERFALL                     = 2,
    MAX_KNOCKBACK_LOCS                          = 3,

    NPC_WATER_SPOUT                             = 28567,

    BG_DS_SPELL_FLUSH                           = 57405, // Visual and target selector for the starting knockback from the pipe
    BG_DS_SPELL_FLUSH_KNOCKBACK                 = 61698, // Knockback effect for previous spell (triggered, not need to be casted)
    BG_DS_SPELL_WATER_SPOUT                     = 58873, // Knockback effect of the central waterfall

    MAX_PIPE_KNOCKBACKS                         = 2
};

class BattleGroundDSScore : public BattleGroundScore
{
    public:
        BattleGroundDSScore() {};
        virtual ~BattleGroundDSScore() {};
        //TODO fix me
};

class BattleGroundDS : public BattleGround
{
    friend class BattleGroundMgr;

    public:
        BattleGroundDS();
        ~BattleGroundDS();
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
        bool HandlePlayerUnderMap(Player * plr);
        bool IsCollisionSpawned() { return m_collisionSpawned; }
        void OnObjectDBLoad(GameObject* go) override;
        bool IsWithinLOSInBG(float x1, float y1, float z1, float x2, float y2, float z2);
    private:
        bool IsWaterfallSprout(Creature* creature);
        void CheckBuggers() override;
        uint32 m_uiPipeKnockback;
        uint32 m_uiWaterfallKnockback;
        uint32 m_waterTimer;
        uint32 m_collisionSpawnTimer;
        uint32 m_pipeKnockbacks;

        bool m_waterfallActive;
        bool m_collisionSpawned;
};
#endif
