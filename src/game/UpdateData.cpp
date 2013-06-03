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

#include "Common.h"
#include "UpdateData.h"
#include "ByteBuffer.h"
#include "WorldPacket.h"
#include "Log.h"
#include "Opcodes.h"
#include "World.h"
#include "ObjectGuid.h"
#include <zlib/zlib.h>

UpdateData::UpdateData(uint16 map) : m_blockCount(0), m_map(map)
{
}

void UpdateData::AddOutOfRangeGUID(GuidSet& guids)
{
    m_outOfRangeGUIDs.insert(guids.begin(),guids.end());
}

void UpdateData::AddOutOfRangeGUID(ObjectGuid const &guid)
{
    m_outOfRangeGUIDs.insert(guid);
}

void UpdateData::AddUpdateBlock(const ByteBuffer &block)
{
    m_data.append(block);
    ++m_blockCount;
}

bool UpdateData::BuildPacket(WorldPacket *packet)
{
    MANGOS_ASSERT(packet->empty());                         // shouldn't happen

    ByteBuffer buf(4 + (m_outOfRangeGUIDs.empty() ? 0 : 1 + 4 + 9 * m_outOfRangeGUIDs.size()) + m_data.wpos());

    buf << uint16(m_map);
    buf << uint32(!m_outOfRangeGUIDs.empty() ? m_blockCount + 1 : m_blockCount);

    if(!m_outOfRangeGUIDs.empty())
    {
        buf << uint8(UPDATETYPE_OUT_OF_RANGE_OBJECTS);
        buf << uint32(m_outOfRangeGUIDs.size());

        for (GuidSet::const_iterator i = m_outOfRangeGUIDs.begin(); i != m_outOfRangeGUIDs.end(); ++i)
            buf << i->WriteAsPacked();
    }

    buf.append(m_data);
    packet->append(buf);
    packet->SetOpcode(SMSG_UPDATE_OBJECT);

    return true;
}

void UpdateData::Clear()
{
    m_data.clear();
    m_outOfRangeGUIDs.clear();
    m_blockCount = 0;
    m_map = 0;
}
