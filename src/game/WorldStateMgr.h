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

/**
 * @addtogroup worldstates World state Manager system
 * The worldstate handling in MaNGOS consists of mostly 2 files:
 * - WorldStateMgr.h
 * - WorldStateMgr.cpp
 *
 * @{
 *
 * @file WorldStateMgr.h
 * This file contains the the headers needed for MaNGOS to handle worldstates.
 *
 */

#ifndef MANGOS_WORLD_STATE_MGR_H
#define MANGOS_WORLD_STATE_MGR_H

#include "World.h"
#include "Common.h"
#include "Policies/Singleton.h"
#include "SharedDefines.h"
#include "Database/DatabaseEnv.h"

/// Structure representing the static data of a worldstate
struct WorldStateData
{
    int iZoneOrMap; // uniqueness problem for custom case

    uint16 uiMinValue;
    uint16 uiMaxValue;
    uint16 uiInitialValue;

    bool bSendByDefault;
};

typedef std::map<int32, WorldStateData> WorldStateMap;

/**
 * A class to represent static data of worldstates
 */
class WorldStateMgr
{
    public:                                                 // Constructors
        WorldStateMgr() {}

    public:                                                 // Accessors
        /// (Re)-Load static data
        void LoadFromDB();

        /// Get initial value for worldstate
        uint32 GetInitialValue(int32 iStateId) const;
        /// Get boundary values for a worldstate
        void GetBounds(int32 iStateId, uint32& uiMin, uint32& uiMax) const;

    private:
        WorldStateMap m_worldStateMap;
};

#define sWorldStateMgr MaNGOS::Singleton<WorldStateMgr>::Instance()

#endif
/*! @} */