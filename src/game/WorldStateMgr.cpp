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
 * @addtogroup worldstates
 * @{
 *
 * @file WorldStateMgr.cpp
 * This file contains the the code needed for MaNGOS to handle worldstates.
 *
 */

#include "WorldStateMgr.h"
#include "ProgressBar.h"

/** (Re)-Load static data of world_state_data
 * Format:
 *  int state_id -- IF > THEN WorldState ELSE customState(or perhaps not)
 *  int zone_or_map --  IF > 0 THEN Area/ Zone ELSE mapId -- actually I expect that except custom states the first key is unique
 *  uint min_value
 *  uint max_value
 *  uint initial_value
 *  bool send_by_default
 *  comment
 */
void WorldStateMgr::LoadFromDB()
{
    // clear for reload case
    m_worldStateMap.clear();

    QueryResult* result = WorldDatabase.Query("SELECT state_id, zone_or_map, min_value, max_value, initial_value, send_by_default FROM world_state_data");
    if (!result)
    {
        sLog.outString(">> Table world_state_data is empty:");
        sLog.outString();
        return;
    }

    uint32 count = 0;

    BarGoLink bar((int)result->GetRowCount());
    do
    {
        Field* fields = result->Fetch();

        bar.step();

        WorldStateData tempData;
        int32 iStateId          = fields[0].GetInt32();
        tempData.iZoneOrMap     = fields[1].GetInt32();
        tempData.uiMinValue     = fields[2].GetUInt16();
        tempData.uiMaxValue     = fields[3].GetUInt16();
        tempData.uiInitialValue = fields[4].GetUInt16();
        tempData.bSendByDefault = fields[5].GetBool();

        // Check values
        // TODO check zone / map id
        if (tempData.uiMinValue > tempData.uiMaxValue)
        {
            sLog.outErrorDb("Table world_state_data: Invalid Min and Max values for World State Id %i (Min %u, Max %u), skipped", iStateId, tempData.uiMinValue, tempData.uiMaxValue);
            continue;
        }
        if (tempData.uiInitialValue < tempData.uiMinValue)
        {
            sLog.outErrorDb("Table world_state_data: Invalid initial value for World State Id %i (Min %u, Initial %u), skipped", iStateId, tempData.uiMinValue, tempData.uiInitialValue);
            continue;

        }
        if (tempData.uiInitialValue > tempData.uiMaxValue)
        {
            sLog.outErrorDb("Table world_state_data: Invalid initial value for World State Id %i (Initial %u, Max %u), skipped", iStateId, tempData.uiInitialValue, tempData.uiMaxValue);
            continue;

        }

        // Store the state data
        m_worldStateMap[iStateId] = tempData;

        ++count;
    }
    while
        (result->NextRow());

    sLog.outString();
    sLog.outString( ">> Loaded static data for %u world states", count );
    delete result;

}

// Get initial value for worldstate
uint32 WorldStateMgr::GetInitialValue(int32 iStateId) const
{
    WorldStateMap::const_iterator find = m_worldStateMap.find(iStateId);
    if (find != m_worldStateMap.end())
        return find->second.uiInitialValue;

    return 0;
}

// Get boundary values for a worldstate
void WorldStateMgr::GetBounds(int32 iStateId, uint32& uiMin, uint32& uiMax) const
{
    WorldStateMap::const_iterator find = m_worldStateMap.find(iStateId);
    if (find != m_worldStateMap.end())
    {
        uiMin = find->second.uiMinValue;
        uiMax = find->second.uiMaxValue;
    }
}

/*! @} */