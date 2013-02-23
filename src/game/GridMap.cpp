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

#include "MapManager.h"
#include "Log.h"
#include "GridStates.h"
#include "CellImpl.h"
#include "Map.h"
#include "DBCEnums.h"
#include "DBCStores.h"
#include "GridMap.h"
#include "VMapFactory.h"
#include "MoveMap.h"
#include "World.h"
#include "Policies/Singleton.h"
#include "Util.h"

char const* MAP_MAGIC         = "MAPS";
char const* MAP_VERSION_MAGIC = "c1.3";
char const* MAP_AREA_MAGIC    = "AREA";
char const* MAP_HEIGHT_MAGIC  = "MHGT";
char const* MAP_LIQUID_MAGIC  = "MLIQ";

GridMap::GridMap()
{
    m_flags = 0;

    // Area data
    m_gridArea = 0;
    m_area_map = NULL;

    // Height level data
    m_gridHeight = INVALID_HEIGHT_VALUE;
    m_gridGetHeight = &GridMap::getHeightFromFlat;
    m_V9 = NULL;
    m_V8 = NULL;

    // Liquid data
    m_liquidType    = 0;
    m_liquid_offX   = 0;
    m_liquid_offY   = 0;
    m_liquid_width  = 0;
    m_liquid_height = 0;
    m_liquidLevel = INVALID_HEIGHT_VALUE;
    m_liquidFlags = NULL;
    m_liquidEntry = NULL;
    m_liquid_map  = NULL;
    currMemUsg = 0;
}

GridMap::~GridMap()
{
    unloadData();
}

bool GridMap::loadData(char *filename)
{
    // Unload old data if exist
    unloadData();

    GridMapFileHeader header;
    // Not return error if file not found
    FILE *in = fopen(filename, "rb");
    if (!in)
        return true;

    fread(&header, sizeof(header),1,in);
    if (header.mapMagic     == *((uint32 const*)(MAP_MAGIC)) &&
        header.versionMagic == *((uint32 const*)(MAP_VERSION_MAGIC)) &&
        IsAcceptableClientBuild(header.buildMagic))
    {
        // loadup area data
        if (header.areaMapOffset && !loadAreaData(in, header.areaMapOffset, header.areaMapSize))
        {
            ERROR_LOG("Error loading map area data\n");
            fclose(in);
            return false;
        }

        // loadup height data
        if (header.heightMapOffset && !loadHeightData(in, header.heightMapOffset, header.heightMapSize))
        {
            ERROR_LOG("Error loading map height data\n");
            fclose(in);
            return false;
        }

        // loadup liquid data
        if (header.liquidMapOffset && !loadGridMapLiquidData(in, header.liquidMapOffset, header.liquidMapSize))
        {
            ERROR_LOG("Error loading map liquids data\n");
            fclose(in);
            return false;
        }

        fclose(in);
        return true;
    }

    ERROR_LOG("Map file '%s' is non-compatible version (outdated?). Please, create new using ad.exe program.", filename);
    fclose(in);
    return false;
}

void GridMap::unloadData()
{
    delete[] m_area_map;
    delete[] m_V9;
    delete[] m_V8;
    delete[] m_liquidEntry;
    delete[] m_liquidFlags;
    delete[] m_liquid_map;

    m_area_map = NULL;
    m_V9 = NULL;
    m_V8 = NULL;
    m_liquidEntry = NULL;
    m_liquidFlags = NULL;
    m_liquid_map  = NULL;
    m_gridGetHeight = &GridMap::getHeightFromFlat;
    currMemUsg = 0;
}

bool GridMap::loadAreaData(FILE *in, uint32 offset, uint32 /*size*/)
{
    GridMapAreaHeader header;
    fseek(in, offset, SEEK_SET);
    fread(&header, sizeof(header), 1, in);
    if (header.fourcc != *((uint32 const*)(MAP_AREA_MAGIC)))
        return false;

    m_gridArea = header.gridArea;
    if (!(header.flags & MAP_AREA_NO_AREA))
    {
        m_area_map = new uint16 [16*16];
        fread(m_area_map, sizeof(uint16), 16*16, in);
        currMemUsg += 16*16 * 2;
    }

    return true;
}

bool GridMap::loadHeightData(FILE *in, uint32 offset, uint32 /*size*/)
{
    GridMapHeightHeader header;
    fseek(in, offset, SEEK_SET);
    fread(&header, sizeof(header), 1, in);
    if (header.fourcc != *((uint32 const*)(MAP_HEIGHT_MAGIC)))
        return false;

    m_gridHeight = header.gridHeight;
    if (!(header.flags & MAP_HEIGHT_NO_HEIGHT))
    {
        if ((header.flags & MAP_HEIGHT_AS_INT16))
        {
            m_uint16_V9 = new uint16 [129*129];
            m_uint16_V8 = new uint16 [128*128];
            fread(m_uint16_V9, sizeof(uint16), 129*129, in);
            fread(m_uint16_V8, sizeof(uint16), 128*128, in);
            m_gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 65535;
            m_gridGetHeight = &GridMap::getHeightFromUint16;
            currMemUsg += (129*129 + 128*128) * 2;
        }
        else if ((header.flags & MAP_HEIGHT_AS_INT8))
        {
            m_uint8_V9 = new uint8 [129*129];
            m_uint8_V8 = new uint8 [128*128];
            fread(m_uint8_V9, sizeof(uint8), 129*129, in);
            fread(m_uint8_V8, sizeof(uint8), 128*128, in);
            m_gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 255;
            m_gridGetHeight = &GridMap::getHeightFromUint8;
            currMemUsg += 129*129 + 128*128;
        }
        else
        {
            m_V9 = new float [129*129];
            m_V8 = new float [128*128];
            fread(m_V9, sizeof(float), 129*129, in);
            fread(m_V8, sizeof(float), 128*128, in);
            m_gridGetHeight = &GridMap::getHeightFromFloat;
            currMemUsg += (129*129 + 128*128) * 4;
        }
    }
    else
        m_gridGetHeight = &GridMap::getHeightFromFlat;

    return true;
}

bool GridMap::loadGridMapLiquidData(FILE *in, uint32 offset, uint32 /*size*/)
{
    GridMapLiquidHeader header;
    fseek(in, offset, SEEK_SET);
    fread(&header, sizeof(header), 1, in);
    if (header.fourcc != *((uint32 const*)(MAP_LIQUID_MAGIC)))
        return false;

    m_liquidType    = header.liquidType;
    m_liquid_offX   = header.offsetX;
    m_liquid_offY   = header.offsetY;
    m_liquid_width  = header.width;
    m_liquid_height = header.height;
    m_liquidLevel   = header.liquidLevel;

    if (!(header.flags & MAP_LIQUID_NO_TYPE))
    {
        m_liquidEntry = new uint16[16 * 16];
        fread(m_liquidEntry, sizeof(uint16), 16 * 16, in);

        m_liquidFlags = new uint8[16 * 16];
        fread(m_liquidFlags, sizeof(uint8), 16 * 16, in);
        currMemUsg += 16*16*2;
    }

    if (!(header.flags & MAP_LIQUID_NO_HEIGHT))
    {
        m_liquid_map = new float [m_liquid_width*m_liquid_height];
        fread(m_liquid_map, sizeof(float), m_liquid_width*m_liquid_height, in);
        currMemUsg += m_liquid_width*m_liquid_height * 4;
    }

    return true;
}

uint16 GridMap::getArea(float x, float y)
{
    if (!m_area_map)
        return m_gridArea;

    x = 16 * (32 - x/SIZE_OF_GRIDS);
    y = 16 * (32 - y/SIZE_OF_GRIDS);
    int lx = (int)x & 15;
    int ly = (int)y & 15;
    return m_area_map[lx*16 + ly];
}

float GridMap::getHeightFromFlat(float /*x*/, float /*y*/) const
{
    return m_gridHeight;
}

float GridMap::getHeightFromFloat(float x, float y) const
{
    if (!m_V8 || !m_V9)
        return m_gridHeight;

    x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

    int x_int = (int)x;
    int y_int = (int)y;
    x -= x_int;
    y -= y_int;
    x_int &= (MAP_RESOLUTION - 1);
    y_int &= (MAP_RESOLUTION - 1);

    // Height stored as: h5 - its v8 grid, h1-h4 - its v9 grid
    // +--------------> X
    // | h1-------h2     Coordinates is:
    // | | \  1  / |     h1 0,0
    // | |  \   /  |     h2 0,1
    // | | 2  h5 3 |     h3 1,0
    // | |  /   \  |     h4 1,1
    // | | /  4  \ |     h5 1/2,1/2
    // | h3-------h4
    // V Y
    // For find height need
    // 1 - detect triangle
    // 2 - solve linear equation from triangle points
    // Calculate coefficients for solve h = a*x + b*y + c

    float a,b,c;
    // Select triangle:
    if (x+y < 1)
    {
        if (x > y)
        {
            // 1 triangle (h1, h2, h5 points)
            float h1 = m_V9[(x_int  )*129 + y_int];
            float h2 = m_V9[(x_int+1)*129 + y_int];
            float h5 = 2 * m_V8[x_int*128 + y_int];
            a = h2-h1;
            b = h5-h1-h2;
            c = h1;
        }
        else
        {
            // 2 triangle (h1, h3, h5 points)
            float h1 = m_V9[x_int*129 + y_int  ];
            float h3 = m_V9[x_int*129 + y_int+1];
            float h5 = 2 * m_V8[x_int*128 + y_int];
            a = h5 - h1 - h3;
            b = h3 - h1;
            c = h1;
        }
    }
    else
    {
        if (x > y)
        {
            // 3 triangle (h2, h4, h5 points)
            float h2 = m_V9[(x_int+1)*129 + y_int  ];
            float h4 = m_V9[(x_int+1)*129 + y_int+1];
            float h5 = 2 * m_V8[x_int*128 + y_int];
            a = h2 + h4 - h5;
            b = h4 - h2;
            c = h5 - h4;
        }
        else
        {
            // 4 triangle (h3, h4, h5 points)
            float h3 = m_V9[(x_int  )*129 + y_int+1];
            float h4 = m_V9[(x_int+1)*129 + y_int+1];
            float h5 = 2 * m_V8[x_int*128 + y_int];
            a = h4 - h3;
            b = h3 + h4 - h5;
            c = h5 - h4;
        }
    }
    // Calculate height
    return a * x + b * y + c;
}

float GridMap::getHeightFromUint8(float x, float y) const
{
    if (!m_uint8_V8 || !m_uint8_V9)
        return m_gridHeight;

    x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

    int x_int = (int)x;
    int y_int = (int)y;
    x -= x_int;
    y -= y_int;
    x_int &= (MAP_RESOLUTION - 1);
    y_int &= (MAP_RESOLUTION - 1);

    int32 a, b, c;
    uint8 *V9_h1_ptr = &m_uint8_V9[x_int*128 + x_int + y_int];
    if (x+y < 1)
    {
        if (x > y)
        {
            // 1 triangle (h1, h2, h5 points)
            int32 h1 = V9_h1_ptr[  0];
            int32 h2 = V9_h1_ptr[129];
            int32 h5 = 2 * m_uint8_V8[x_int*128 + y_int];
            a = h2-h1;
            b = h5-h1-h2;
            c = h1;
        }
        else
        {
            // 2 triangle (h1, h3, h5 points)
            int32 h1 = V9_h1_ptr[0];
            int32 h3 = V9_h1_ptr[1];
            int32 h5 = 2 * m_uint8_V8[x_int*128 + y_int];
            a = h5 - h1 - h3;
            b = h3 - h1;
            c = h1;
        }
    }
    else
    {
        if (x > y)
        {
            // 3 triangle (h2, h4, h5 points)
            int32 h2 = V9_h1_ptr[129];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint8_V8[x_int*128 + y_int];
            a = h2 + h4 - h5;
            b = h4 - h2;
            c = h5 - h4;
        }
        else
        {
            // 4 triangle (h3, h4, h5 points)
            int32 h3 = V9_h1_ptr[  1];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint8_V8[x_int*128 + y_int];
            a = h4 - h3;
            b = h3 + h4 - h5;
            c = h5 - h4;
        }
    }

    // Calculate height
    return (float)((a * x) + (b * y) + c)*m_gridIntHeightMultiplier + m_gridHeight;
}

float GridMap::getHeightFromUint16(float x, float y) const
{
    if (!m_uint16_V8 || !m_uint16_V9)
        return m_gridHeight;

    x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

    int x_int = (int)x;
    int y_int = (int)y;
    x -= x_int;
    y -= y_int;
    x_int &= (MAP_RESOLUTION - 1);
    y_int &= (MAP_RESOLUTION - 1);

    int32 a, b, c;
    uint16 *V9_h1_ptr = &m_uint16_V9[x_int*128 + x_int + y_int];
    if (x+y < 1)
    {
        if (x > y)
        {
            // 1 triangle (h1, h2, h5 points)
            int32 h1 = V9_h1_ptr[  0];
            int32 h2 = V9_h1_ptr[129];
            int32 h5 = 2 * m_uint16_V8[x_int*128 + y_int];
            a = h2-h1;
            b = h5-h1-h2;
            c = h1;
        }
        else
        {
            // 2 triangle (h1, h3, h5 points)
            int32 h1 = V9_h1_ptr[0];
            int32 h3 = V9_h1_ptr[1];
            int32 h5 = 2 * m_uint16_V8[x_int*128 + y_int];
            a = h5 - h1 - h3;
            b = h3 - h1;
            c = h1;
        }
    }
    else
    {
        if (x > y)
        {
            // 3 triangle (h2, h4, h5 points)
            int32 h2 = V9_h1_ptr[129];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint16_V8[x_int*128 + y_int];
            a = h2 + h4 - h5;
            b = h4 - h2;
            c = h5 - h4;
        }
        else
        {
            // 4 triangle (h3, h4, h5 points)
            int32 h3 = V9_h1_ptr[  1];
            int32 h4 = V9_h1_ptr[130];
            int32 h5 = 2 * m_uint16_V8[x_int*128 + y_int];
            a = h4 - h3;
            b = h3 + h4 - h5;
            c = h5 - h4;
        }
    }

    // Calculate height
    return (float)((a * x) + (b * y) + c)*m_gridIntHeightMultiplier + m_gridHeight;
}

float GridMap::getLiquidLevel(float x, float y)
{
    if (!m_liquid_map)
        return m_liquidLevel;

    x = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
    y = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

    int cx_int = ((int)x & (MAP_RESOLUTION-1)) - m_liquid_offY;
    int cy_int = ((int)y & (MAP_RESOLUTION-1)) - m_liquid_offX;

    if (cx_int < 0 || cx_int >=m_liquid_height)
        return INVALID_HEIGHT_VALUE;

    if (cy_int < 0 || cy_int >=m_liquid_width )
        return INVALID_HEIGHT_VALUE;

    return m_liquid_map[cx_int*m_liquid_width + cy_int];
}

uint8 GridMap::getTerrainType(float x, float y)
{
    if (!m_liquidFlags)
        return (uint8)m_liquidType;

    x = 16 * (32 - x/SIZE_OF_GRIDS);
    y = 16 * (32 - y/SIZE_OF_GRIDS);
    int lx = (int)x & 15;
    int ly = (int)y & 15;
    return m_liquidFlags[lx * 16 + ly];
}

// Get water state on map
GridMapLiquidStatus GridMap::getLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, GridMapLiquidData *data)
{
    // Check water type (if no water return)
    if (!m_liquidFlags && !m_liquidType)
        return LIQUID_MAP_NO_WATER;

    // Get cell
    float cx = MAP_RESOLUTION * (32 - x/SIZE_OF_GRIDS);
    float cy = MAP_RESOLUTION * (32 - y/SIZE_OF_GRIDS);

    int x_int = (int)cx & (MAP_RESOLUTION-1);
    int y_int = (int)cy & (MAP_RESOLUTION-1);

    // Check water type in cell
    int idx = (x_int >> 3) * 16 + (y_int >> 3);
    uint8 type = m_liquidFlags ? m_liquidFlags[idx] : m_liquidType;
    uint32 entry = 0;
    if (m_liquidEntry)
    {
        if (LiquidTypeEntry const* liquidEntry = sLiquidTypeStore.LookupEntry(m_liquidEntry[idx]))
        {
            entry = liquidEntry->Id;
            type &= MAP_LIQUID_TYPE_DARK_WATER;
            uint32 liqTypeIdx = liquidEntry->Type;
            if (entry < 21)
            {
                if (AreaTableEntry const* area = sAreaStore.LookupEntry(getArea(x, y)))
                {
                    uint32 overrideLiquid = area->LiquidTypeOverride[liquidEntry->Type];
                    if (!overrideLiquid && area->zone)
                    {
                        area = GetAreaEntryByAreaID(area->zone);
                        if (area)
                            overrideLiquid = area->LiquidTypeOverride[liquidEntry->Type];
                    }

                    if (LiquidTypeEntry const* liq = sLiquidTypeStore.LookupEntry(overrideLiquid))
                    {
                        entry = overrideLiquid;
                        liqTypeIdx = liq->Type;
                    }
                }
            }

            type |= 1 << liqTypeIdx;
        }
    }

    if (type == 0)
        return LIQUID_MAP_NO_WATER;

    // Check req liquid type mask
    if (ReqLiquidType && !(ReqLiquidType&type))
        return LIQUID_MAP_NO_WATER;

    // Check water level:
    // Check water height map
    int lx_int = x_int - m_liquid_offY;
    if (lx_int < 0 || lx_int >=m_liquid_height)
        return LIQUID_MAP_NO_WATER;

    int ly_int = y_int - m_liquid_offX;
    if (ly_int < 0 || ly_int >=m_liquid_width )
        return LIQUID_MAP_NO_WATER;

    // Get water level
    float liquid_level = m_liquid_map ? m_liquid_map[lx_int*m_liquid_width + ly_int] : m_liquidLevel;

    // Get ground level (sub 0.2 for fix some errors)
    float ground_level = getHeight(x, y);

    // Check water level and ground level
    if (liquid_level < ground_level || z < ground_level - 2)
        return LIQUID_MAP_NO_WATER;

    // All ok in water -> store data
    if (data)
    {
        data->entry = entry;
        data->type_flags = type;
        data->level = liquid_level;
        data->depth_level = ground_level;
    }

    // For speed check as int values
    int delta = int((liquid_level - z) * 10);

    // Get position delta
    if (delta > 20)                                         // Under water
        return LIQUID_MAP_UNDER_WATER;

    if (delta > 0 )                                         // In water
        return LIQUID_MAP_IN_WATER;

    if (delta > -1)                                         // Walk on water
        return LIQUID_MAP_WATER_WALK;
                                                            // Above water
    return LIQUID_MAP_ABOVE_WATER;
}

bool GridMap::ExistMap(uint32 mapid,int gx,int gy)
{
    int len = sWorld.GetDataPath().length()+strlen("maps/%03u%02u%02u.map")+1;
    char* tmp = new char[len];
    snprintf(tmp, len, (char *)(sWorld.GetDataPath()+"maps/%03u%02u%02u.map").c_str(),mapid,gx,gy);

    FILE *pf=fopen(tmp,"rb");

    if(!pf)
    {
        ERROR_LOG("Check existing of map file '%s': not exist!",tmp);
        delete[] tmp;
        return false;
    }

    GridMapFileHeader header;
    fread(&header, sizeof(header), 1, pf);
    if (header.mapMagic     != *((uint32 const*)(MAP_MAGIC)) ||
        header.versionMagic != *((uint32 const*)(MAP_VERSION_MAGIC)) ||
        !IsAcceptableClientBuild(header.buildMagic))
    {
        ERROR_LOG("Map file '%s' is non-compatible version (outdated?). Please, create new using ad.exe program.",tmp);
        delete[] tmp;
        fclose(pf);                                         //close file before return
        return false;
    }

    delete[] tmp;
    fclose(pf);
    return true;
}

bool GridMap::ExistVMap(uint32 mapid,int gx,int gy)
{
    if(VMAP::IVMapManager* vmgr = VMAP::VMapFactory::createOrGetVMapManager())
    {
        if(vmgr->isMapLoadingEnabled())
        {
                                                            // x and y are swapped !! => fixed now
            bool exists = vmgr->existsMap((sWorld.GetDataPath()+ "vmaps").c_str(),  mapid, gx,gy);
            if(!exists)
            {
                std::string name = vmgr->getDirFileName(mapid,gx,gy);
                ERROR_LOG("VMap file '%s' is missing or point to wrong version vmap file, redo vmaps with latest vmap_assembler.exe program", (sWorld.GetDataPath()+"vmaps/"+name).c_str());
                return false;
            }
        }
    }

    return true;
}

//////////////////////////////////////////////////////////////////////////
TerrainInfo::TerrainInfo(uint32 mapid) : m_mapId(mapid)
{
    for (int k = 0; k < MAX_NUMBER_OF_GRIDS; ++k)
    {
        for (int i = 0; i < MAX_NUMBER_OF_GRIDS; ++i)
        {
            m_GridMaps[i][k] = NULL;
            m_GridRef[i][k] = 0;
        }
    }

    //clean up GridMap objects every minute
    const uint32 iCleanUpInterval = 60;
    //schedule start randlomly
    const uint32 iRandomStart = urand(20, 40);

    i_timer.SetInterval(iCleanUpInterval * 1000);
    i_timer.SetCurrent(iRandomStart * 1000);
}

TerrainInfo::~TerrainInfo()
{
    for (int k = 0; k < MAX_NUMBER_OF_GRIDS; ++k)
        for (int i = 0; i < MAX_NUMBER_OF_GRIDS; ++i)
            delete m_GridMaps[i][k];

    VMAP::VMapFactory::createOrGetVMapManager()->unloadMap(m_mapId);
    MMAP::MMapFactory::createOrGetMMapManager()->unloadMap(m_mapId);
}

GridMap * TerrainInfo::Load(const uint32 x, const uint32 y)
{
    MANGOS_ASSERT(x < MAX_NUMBER_OF_GRIDS);
    MANGOS_ASSERT(y < MAX_NUMBER_OF_GRIDS);

    //reference grid as a first step
    RefGrid(x, y);

    //quick check if GridMap already loaded
    GridMap * pMap = m_GridMaps[x][y];
    if(!pMap)
        pMap = LoadMapAndVMap(x, y);

    return pMap;
}

//schedule lazy GridMap object cleanup
void TerrainInfo::Unload(const uint32 x, const uint32 y)
{
    MANGOS_ASSERT(x < MAX_NUMBER_OF_GRIDS);
    MANGOS_ASSERT(y < MAX_NUMBER_OF_GRIDS);

    if(m_GridMaps[x][y])
    {
        //decrease grid reference count...
        if(UnrefGrid(x, y) == 0)
        {
            // TODO: add your additional logic here
        }
    }
}

// call this method only
void TerrainInfo::CleanUpGrids(const uint32 diff)
{
    i_timer.Update(diff);
    if( !i_timer.Passed() )
        return;

    for (int y = 0; y < MAX_NUMBER_OF_GRIDS; ++y)
    {
        for (int x = 0; x < MAX_NUMBER_OF_GRIDS; ++x)
        {
            const int16& iRef = m_GridRef[x][y];
            GridMap * pMap = m_GridMaps[x][y];

            //delete those GridMap objects which have refcount = 0
            if(pMap && iRef == 0 )
            {
                m_GridMaps[x][y] = NULL;
                //delete grid data if reference count == 0
                pMap->unloadData();
                delete pMap;

                //unload VMAPS...
                VMAP::VMapFactory::createOrGetVMapManager()->unloadMap(m_mapId, x, y);

                //unload mmap...
                MMAP::MMapFactory::createOrGetMMapManager()->unloadMap(m_mapId, x, y);
            }
        }
    }

    i_timer.Reset();
}

int TerrainInfo::RefGrid(const uint32& x, const uint32& y)
{
    MANGOS_ASSERT(x < MAX_NUMBER_OF_GRIDS);
    MANGOS_ASSERT(y < MAX_NUMBER_OF_GRIDS);

    LOCK_GUARD _lock(m_refMutex);
    return (m_GridRef[x][y] += 1);
}

int TerrainInfo::UnrefGrid(const uint32& x, const uint32& y)
{
    MANGOS_ASSERT(x < MAX_NUMBER_OF_GRIDS);
    MANGOS_ASSERT(y < MAX_NUMBER_OF_GRIDS);

    int16& iRef = m_GridRef[x][y];

    LOCK_GUARD _lock(m_refMutex);
    if(iRef > 0)
        return (iRef -= 1);

    return 0;
}

float TerrainInfo::GetHeightStatic(float x, float y, float z, bool useVmaps/*=true*/, float maxSearchDist/*=DEFAULT_HEIGHT_SEARCH*/) const
{
    float mapHeight = VMAP_INVALID_HEIGHT_VALUE;            // Store Height obtained by maps
    float vmapHeight = VMAP_INVALID_HEIGHT_VALUE;           // Store Height obtained by vmaps (in "corridor" of z (or slightly above z)

    float z2 = z + 2.f;

    // find raw .map surface under Z coordinates (or well-defined above)
    if (GridMap* gmap = const_cast<TerrainInfo*>(this)->GetGrid(x, y))
        mapHeight = gmap->getHeight(x, y);

    if (useVmaps)
    {
        VMAP::IVMapManager* vmgr = VMAP::VMapFactory::createOrGetVMapManager();
        if (vmgr->isHeightCalcEnabled())
        {
            // if mapHeight has been found search vmap height at least until mapHeight point
            // this prevent case when original Z "too high above ground and vmap height search fail"
            // this will not affect most normal cases (no map in instance, or stay at ground at continent)
            if (mapHeight > INVALID_HEIGHT && z2 - mapHeight > maxSearchDist)
                maxSearchDist = z2 - mapHeight + 1.0f;      // 1.0 make sure that we not fail for case when map height near but above for vamp height

            // look from a bit higher pos to find the floor
            vmapHeight = vmgr->getHeight(GetMapId(), x, y, z2, maxSearchDist);

            // if not found in expected range, look for infinity range (case of far above floor, but below terrain-height)
            if (vmapHeight <= INVALID_HEIGHT)
                vmapHeight = vmgr->getHeight(GetMapId(), x, y, z2, 10000.0f);

            // still not found, look near terrain height
            if (vmapHeight <= INVALID_HEIGHT && mapHeight > INVALID_HEIGHT && z2 < mapHeight)
                vmapHeight = vmgr->getHeight(GetMapId(), x, y, mapHeight + 2.0f, DEFAULT_HEIGHT_SEARCH);
        }
    }

    //hack for LK frozen throne true height
    if (GetAreaId(x,y,z) == 4859)
    {
        mapHeight  += 200.0f;
        vmapHeight += 200.0f;
    } 

    // mapHeight set for any above raw ground Z or <= INVALID_HEIGHT
    // vmapheight set for any under Z value or <= INVALID_HEIGHT
    if (vmapHeight > INVALID_HEIGHT)
    {
        if (mapHeight > INVALID_HEIGHT)
        {
            // we have mapheight and vmapheight and must select more appropriate

            // we are already under the surface or vmap height above map heigt
            if (z < mapHeight || vmapHeight > mapHeight)
                return vmapHeight;
            else
                return mapHeight;                           // better use .map surface height
        }
        else
            return vmapHeight;                              // we have only vmapHeight (if have)
    }

    return mapHeight;
}

bool TerrainInfo::IsNextZcoordOK(float x, float y, float oldZ, float maxDiff) const
{
    // The fastest way to get an accurate result 90% of the time.
    // Better result can be obtained like 99% accuracy with a ray light, but the cost is too high and the code is too long.
    maxDiff = maxDiff >= 100.0f ? 10.0f : sqrtf(maxDiff);
    bool useVmaps = false;
    if (GetHeightStatic(x, y, oldZ, false) <  GetHeightStatic(x, y, oldZ, true)) // check use of vmaps
        useVmaps = true;

    float newZ = GetHeightStatic(x, y, oldZ+maxDiff-2.0f, useVmaps);

    if (fabs(newZ-oldZ) > maxDiff)                              // bad...
    {
        useVmaps = !useVmaps;                                     // try change vmap use
        newZ = GetHeightStatic(x, y, oldZ+maxDiff-2.0f, useVmaps);

        if (fabs(newZ-oldZ) > maxDiff)
            return false;
    }
    return true;
}

inline bool IsOutdoorWMO(uint32 mogpFlags, int32 adtId, int32 rootId, int32 groupId,
                              WMOAreaTableEntry const* wmoEntry, AreaTableEntry const* atEntry)
{
    bool outdoor = true;

    if(wmoEntry && atEntry)
    {
        if(atEntry->flags & AREA_FLAG_OUTSIDE)
            return true;
        if(atEntry->flags & AREA_FLAG_INSIDE)
            return false;
    }

    outdoor = mogpFlags&0x8;

    if(wmoEntry)
    {
        if(wmoEntry->Flags & 4)
            return true;

        if((wmoEntry->Flags & 2)!=0)
            outdoor = false;
    }
    return outdoor;
}

bool TerrainInfo::IsOutdoors(float x, float y, float z) const
{
    uint32 mogpFlags;
    int32 adtId, rootId, groupId;

    // no wmo found? -> outside by default
    if(!GetAreaInfo(x, y, z, mogpFlags, adtId, rootId, groupId))
        return true;

    AreaTableEntry const* atEntry = 0;
    WMOAreaTableEntry const* wmoEntry= GetWMOAreaTableEntryByTripple(rootId, adtId, groupId);
    if(wmoEntry)
    {
        DEBUG_LOG("Got WMOAreaTableEntry! flag %u, areaid %u", wmoEntry->Flags, wmoEntry->areaId);

        atEntry = GetAreaEntryByAreaID(wmoEntry->areaId);
    }

    return IsOutdoorWMO(mogpFlags, adtId, rootId, groupId, wmoEntry, atEntry);
}

bool TerrainInfo::GetAreaInfo(float x, float y, float z, uint32 &flags, int32 &adtId, int32 &rootId, int32 &groupId) const
{
    float vmap_z = z;
    VMAP::IVMapManager* vmgr = VMAP::VMapFactory::createOrGetVMapManager();
    if (vmgr->getAreaInfo(GetMapId(), x, y, vmap_z, flags, adtId, rootId, groupId))
    {
        // check if there's terrain between player height and object height
        if(GridMap *gmap = const_cast<TerrainInfo*>(this)->GetGrid(x, y))
        {
            float _mapheight = gmap->getHeight(x, y);
            // z + 2.0f condition taken from GetHeightStatic(), not sure if it's such a great choice...
            if (z + 2.0f > _mapheight &&  _mapheight > vmap_z)
                return false;
        }
        return true;
    }
    return false;
}

uint16 TerrainInfo::GetAreaFlag(float x, float y, float z, bool *isOutdoors) const
{
    uint32 mogpFlags;
    int32 adtId, rootId, groupId;
    WMOAreaTableEntry const* wmoEntry = 0;
    AreaTableEntry const* atEntry = 0;
    bool haveAreaInfo = false;

    if(GetAreaInfo(x, y, z, mogpFlags, adtId, rootId, groupId))
    {
        haveAreaInfo = true;
        wmoEntry = GetWMOAreaTableEntryByTripple(rootId, adtId, groupId);
        if(wmoEntry)
            atEntry = GetAreaEntryByAreaID(wmoEntry->areaId);
    }

    uint16 areaflag;
    if (atEntry)
        areaflag = atEntry->exploreFlag;
    else
    {
        if(GridMap *gmap = const_cast<TerrainInfo*>(this)->GetGrid(x, y))
            areaflag = gmap->getArea(x, y);
        // this used while not all *.map files generated (instances)
        else
            areaflag = GetAreaFlagByMapId(GetMapId());
    }

    if (isOutdoors)
    {
        if (haveAreaInfo)
            *isOutdoors = IsOutdoorWMO(mogpFlags, adtId, rootId, groupId, wmoEntry, atEntry);
        else
            *isOutdoors = true;
    }

    //FIXME: some hacks for areas above or underground for ground area
    //       required for area specific spells/etc, until map/vmap data
    //       not provided correct areaflag with this hacks
    /*switch(areaflag)
    {
        case 1146:                                          // Blade's Edge Mountains
        case 1409:                                          // Forge Camp: Wrath (Blade's Edge Mountains)
            if (x > 3025.0f && x < 3207.0f && y > 6987.0f && y < 7165.0f && z < 183.0f)
                areaflag = 1404;                            // Blackwing Coven (Blade's Edge Mountains)
            break;
        // Acherus: The Ebon Hold (Plaguelands: The Scarlet Enclave)
        case 1984:                                          // Plaguelands: The Scarlet Enclave
        case 2076:                                          // Death's Breach (Plaguelands: The Scarlet Enclave)
        case 2745:                                          // The Noxious Pass (Plaguelands: The Scarlet Enclave)
            if(z > 350.0f) areaflag = 2048; break;
        // Acherus: The Ebon Hold (Eastern Plaguelands)
        case 856:                                           // The Noxious Glade (Eastern Plaguelands)
        case 2456:                                          // Death's Breach (Eastern Plaguelands)
            if(z > 350.0f) areaflag = 1950; break;
        // Winterfin Caverns
        case 1652:                                          // Coldarra
        case 1653:                                          // The Westrift
        case 1661:                                          // Winterfin Village
            if (x > 3823.0f && x < 4141.5f && y > 6247.0f && y < 64890.0f && z < 42.5f)
                areaflag = 1723;
            break;
        // Moonrest Gardens
        case 1787:
            if (x > 3315.3f && x < 3361.6f && y > 2469.4f && y < 2565.8f && z > 197.0f)
                areaflag = 1786;                            // Surge Needle (cords not entirely correct, will need round circle if this is really needed(see spell 47097 eff 77))
            break;
        // Dalaran
        case 2492:                                          // Forlorn Woods (Crystalsong Forest)
        case 2371:                                          // Valley of Echoes (Icecrown Glacier)
            if (x > 5568.0f && x < 6116.0f && y > 282.0f && y < 982.0f && z > 563.0f)
            {
                // Krasus' Landing (Dalaran), fast check
                if (x > 5758.77f && x < 5869.03f && y < 510.46f)
                {
                    // Krasus' Landing (Dalaran), with open east side
                    if (y < 449.33f || (x-5813.9f)*(x-5813.9f)+(y-449.33f)*(y-449.33f) < 1864.0f)
                    {
                        areaflag = 2531;                    // Note: also 2633, possible one flight allowed and other not allowed case
                        break;
                    }
                }

                // Sunreaver's Sanctuary (Dalaran) (Enter,Left,Right-border lines)
                if ((y-548.11f)/(568.80f-548.11f) <= (x-5849.30f)/(5866.57f-5849.30f) &&
                    (y-582.76f)/(622.97f-582.76f) <= (x-5961.64f)/(5886.09f-5961.64f) &&
                    (y-446.79f)/(508.22f-446.79f) >= (x-5867.66f)/(5846.12f-5867.66f))
                {
                    // need exclude 2 shop rooms
                    // if (((x-5862.26f)*(x-5862.26f)+(y-554.76f)*(y-554.76f) > 310.64f || z > 671.12f) &&
                    if (((x-5862.26f)*(x-5862.26f)+(y-554.76f)*(y-554.76f) > 335.85f || z > 671.12f) &&
                        ((y-580.51f)/(608.37f-580.51f) <= (x-5896.23f)/(5859.79f-5896.23f) ||
                         (y-580.51f)/(610.49f-580.51f) <= (x-5896.23f)/(5919.15f-5896.23f) || z > 669.10f))
                    {
                        areaflag = 2687;
                        break;
                    }
                }

                // The Silver Enclave (Dalaran) (Enter,Left,Right-border lines)
                if ((y-693.19f)/(737.41f-693.19f) >= (x-5732.80f)/(5769.38f-5732.80f) &&
                    (y-693.19f)/(787.00f-693.19f) >= (x-5732.80f)/(5624.17f-5732.80f) &&
                    (y-737.41f)/(831.30f-737.41f) <= (x-5769.38f)/(5671.15f-5769.38f))
                {
                    // need exclude ground floor shop room
                    if ((x-5758.07f)*(x-5758.07f)+(y-738.18f)*(y-738.18f) > 83.30f || z > 650.00f)
                    {
                        areaflag = 3007;
                        break;
                    }
                }

                // Dalaran
                areaflag = 2153;
            }
            break;
        // The Violet Citadel (Dalaran) or Dalaran
        case 2484:                                          // The Twilight Rivulet (Crystalsong Forest)
        case 1593:                                          // Crystalsong Forest
            // Dalaran
            if (x > 5568.0f && x < 6116.0f && y > 282.0f && y < 982.0f && z > 563.0f)
            {
                // The Violet Citadel (Dalaran), fast check
                if (x > 5721.1f && x < 5884.66f && y > 764.4f && y < 948.0f)
                {
                    // The Violet Citadel (Dalaran)
                    if ((x-5803.0f)*(x-5803.0f)+(y-846.18f)*(y-846.18f) < 6690.0f)
                    {
                        areaflag = 2696;
                        break;
                    }
                }

                // Sunreaver's Sanctuary (Dalaran) (Enter,Left,Right-border lines)
                if ((y-581.27f)/(596.73f-581.27f) <= (x-5858.57f)/(5871.87f-5858.57f) &&
                    (y-582.76f)/(622.97f-582.76f) <= (x-5961.64f)/(5886.09f-5961.64f) &&
                    (y-446.79f)/(508.22f-446.79f) >= (x-5867.66f)/(5846.12f-5867.66f))
                {
                    areaflag = 2687;
                    break;
                }

                // The Silver Enclave (Dalaran) (Enter,Left,Right-border lines)
                if ((y-693.19f)/(737.41f-693.19f) >= (x-5732.80f)/(5769.38f-5732.80f) &&
                    (y-693.19f)/(787.00f-693.19f) >= (x-5732.80f)/(5624.17f-5732.80f) &&
                    (y-737.41f)/(831.30f-737.41f) <= (x-5769.38f)/(5671.15f-5769.38f))
                {
                    // need exclude ground floor shop room
                    if ((x-5758.07f)*(x-5758.07f)+(y-738.18f)*(y-738.18f) > 64.30f || z > 650.00f)
                    {
                        areaflag = 3007;
                        break;
                    }
                }

                // Dalaran
                areaflag = 2153;
            }
            break;
        // Vargoth's Retreat (Dalaran) or The Violet Citadel (Dalaran) or Dalaran
        case 2504:                                          // Violet Stand (Crystalsong Forest)
            // Dalaran
            if (x > 5568.0f && x < 6116.0f && y > 282.0f && y < 982.0f && z > 563.0f)
            {
                // The Violet Citadel (Dalaran), fast check
                if (x > 5721.1f && x < 5884.66f && y > 764.4f && y < 948.0f)
                {
                    // Vargoth's Retreat (Dalaran), nice slow circle with upper limit
                    if (z < 898.0f && (x-5765.0f)*(x-5765.0f)+(y-862.4f)*(y-862.4f) < 262.0f)
                    {
                        areaflag = 2748;
                        break;
                    }

                    // The Violet Citadel (Dalaran)
                    if ((x-5803.0f)*(x-5803.0f)+(y-846.18f)*(y-846.18f) < 6690.0f)
                    {
                        areaflag = 2696;
                        break;
                    }
                }

                // Dalaran
                areaflag = 2153;
            }
            break;
        // Maw of Neltharion (cave)
        case 164:                                           // Dragonblight
        case 1797:                                          // Obsidian Dragonshrine (Dragonblight)
        case 1827:                                          // Wintergrasp
        case 2591:                                          // The Cauldron of Flames (Wintergrasp)
            if (x > 4364.0f && x < 4632.0f && y > 1545.0f && y < 1886.0f && z < 200.0f) areaflag = 1853; break;
        // Undercity (sewers enter and path)
        case 179:                                           // Tirisfal Glades
            if (x > 1595.0f && x < 1699.0f && y > 535.0f && y < 643.5f && z < 30.5f) areaflag = 685; break;
        // Undercity (Royal Quarter)
        case 210:                                           // Silverpine Forest
        case 316:                                           // The Shining Strand (Silverpine Forest)
        case 438:                                           // Lordamere Lake (Silverpine Forest)
            if (x > 1237.0f && x < 1401.0f && y > 284.0f && y < 440.0f && z < -40.0f) areaflag = 685; break;
        // Undercity (cave and ground zone, part of royal quarter)
        case 607:                                           // Ruins of Lordaeron (Tirisfal Glades)
            // ground and near to ground (by city walls)
            if(z > 0.0f)
            {
                if (x > 1510.0f && x < 1839.0f && y > 29.77f && y < 433.0f) areaflag = 685;
            }
            // more wide underground, part of royal quarter
            else
            {
                if (x > 1299.0f && x < 1839.0f && y > 10.0f && y < 440.0f) areaflag = 685;
            }
            break;
        // The Makers' Perch (ground) and Makers' Overlook (ground and cave)
        case 1335:                                          // Sholazar Basin
            // The Makers' Perch ground (fast box)
            if (x > 6100.0f && x < 6250.0f && y > 5650.0f && y < 5800.0f)
            {
                // nice slow circle
                if ((x-6183.0f)*(x-6183.0f)+(y-5717.0f)*(y-5717.0f) < 2500.0f)
                    areaflag = 2189;
            }
            // Makers' Overlook (ground and cave)
            else if (x > 5634.48f && x < 5774.53f  && y < 3475.0f && z > 300.0f)
            {
                if (y > 3380.26f || (y > 3265.0f && z < 360.0f))
                    areaflag = 2187;
            }
            break;
        // The Makers' Perch (underground)
        case 2147:                                          // The Stormwright's Shelf (Sholazar Basin)
            if (x > 6199.0f && x < 6283.0f && y > 5705.0f && y < 5817.0f && z < 38.0f) areaflag = 2189; break;
        // Makers' Overlook (deep cave)
        case 267:                                           // Icecrown
            if (x > 5684.0f && x < 5798.0f && y > 3035.0f && y < 3367.0f && z < 358.0f) areaflag = 2187; break;
        // Wyrmrest Temple (Dragonblight)
        case 1814:                                          // Path of the Titans (Dragonblight)
        case 1897:                                          // The Dragon Wastes (Dragonblight)
            // fast box
            if (x > 3400.0f && x < 3700.0f && y > 130.0f && y < 420.0f)
            {
                // nice slow circle
                if ((x-3546.87f)*(x-3546.87f)+(y-272.71f)*(y-272.71f) < 19600.0f) areaflag = 1791;
            }
            break;
        // The Forlorn Mine (The Storm Peaks)
        case 166:                                           // The Storm Peaks
        case 2207:                                          // Brunnhildar Village (The Storm Peaks)
        case 2209:                                          // Sifreldar Village (The Storm Peaks)
        case 2227:                                          // The Foot Steppes (The Storm Peaks)
            // fast big box
            if (x > 6812.0f && x < 7049.5f && y > -1474.5f && y < -1162.5f && z < 866.15f)
            {
                // east, avoid ground east-south corner wrong detection
                if (x > 6925.0f && y > -1474.5f && y < -1290.0f)
                    areaflag = 2213;
                // east middle, wide part
                else if (x > 6812.0f && y > -1400.0f && y < -1290.0f)
                    areaflag = 2213;
                // west middle, avoid ground west-south corner wrong detection
                else if (x > 6833.0f && y > -1474.5f && y < -1233.0f)
                    areaflag = 2213;
                // west, avoid ground west-south corner wrong detection
                else if (x > 6885.0f && y > -1474.5f && y < -1162.5f)
                    areaflag = 2213;
            }
            break;
        default:
            break;
    }*/

    return areaflag;
}

uint8 TerrainInfo::GetTerrainType(float x, float y ) const
{
    if(GridMap *gmap = const_cast<TerrainInfo*>(this)->GetGrid(x, y))
        return gmap->getTerrainType(x, y);
    else
        return 0;
}

uint32 TerrainInfo::GetAreaId(float x, float y, float z) const
{
    return TerrainManager::GetAreaIdByAreaFlag(GetAreaFlag(x,y,z),m_mapId);
}

uint32 TerrainInfo::GetZoneId(float x, float y, float z) const
{
    return TerrainManager::GetZoneIdByAreaFlag(GetAreaFlag(x,y,z),m_mapId);
}

void TerrainInfo::GetZoneAndAreaId(uint32& zoneid, uint32& areaid, float x, float y, float z) const
{
    TerrainManager::GetZoneAndAreaIdByAreaFlag(zoneid,areaid,GetAreaFlag(x,y,z),m_mapId);
}

GridMapLiquidStatus TerrainInfo::getLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, GridMapLiquidData* data) const
{
    GridMapLiquidStatus result = LIQUID_MAP_NO_WATER;
    VMAP::IVMapManager* vmgr = VMAP::VMapFactory::createOrGetVMapManager();
    uint32 liquid_type = 0;
    float liquid_level = INVALID_HEIGHT_VALUE;
    float ground_level = GetHeightStatic(x, y, z, true, DEFAULT_WATER_SEARCH);

    if (vmgr->GetLiquidLevel(GetMapId(), x, y, z, ReqLiquidType, liquid_level, ground_level, liquid_type))
    {
        //DEBUG_LOG("getLiquidStatus(): vmap liquid level: %f ground: %f type: %u", liquid_level, ground_level, liquid_type);
        // Check water level and ground level
        if (liquid_level > ground_level && z > ground_level - 2)
        {
            // All ok in water -> store data
            if (data)
            {
                // hardcoded in client like this
                if (GetMapId() == 530 && liquid_type == 2)
                    liquid_type = 15;

                uint32 liquidFlagType = 0;
                if (LiquidTypeEntry const* liq = sLiquidTypeStore.LookupEntry(liquid_type))
                    liquidFlagType = liq->Type;

                if (liquid_type && liquid_type < 21)
                {
                    if (AreaTableEntry const* area = GetAreaEntryByAreaFlagAndMap(GetAreaFlag(x, y, z), GetMapId()))
                    {
                        uint32 overrideLiquid = area->LiquidTypeOverride[liquidFlagType];
                        if (!overrideLiquid && area->zone)
                        {
                            area = GetAreaEntryByAreaID(area->zone);
                            if (area)
                                overrideLiquid = area->LiquidTypeOverride[liquidFlagType];
                        }

                        if (LiquidTypeEntry const* liq = sLiquidTypeStore.LookupEntry(overrideLiquid))
                        {
                            liquid_type = overrideLiquid;
                            liquidFlagType = liq->Type;
                        }
                    }
                }

                data->level = liquid_level;
                data->depth_level = ground_level;

                data->entry = liquid_type;
                data->type_flags = 1 << liquidFlagType;
            }

            // For speed check as int values
            int delta = int((liquid_level - z) * 10);

            // Get position delta
            if (delta > 20)                   // Under water
                return LIQUID_MAP_UNDER_WATER;
            if (delta > 0 )                   // In water
                return LIQUID_MAP_IN_WATER;
            if (delta > -1)                   // Walk on water
                return LIQUID_MAP_WATER_WALK;
            result = LIQUID_MAP_ABOVE_WATER;
        }
    }
    else if (GridMap* gmap = const_cast<TerrainInfo*>(this)->GetGrid(x, y))
    {
        GridMapLiquidData map_data;
        GridMapLiquidStatus map_result = gmap->getLiquidStatus(x, y, z, ReqLiquidType, &map_data);
        // Not override LIQUID_MAP_ABOVE_WATER with LIQUID_MAP_NO_WATER:
        if (map_result != LIQUID_MAP_NO_WATER && (map_data.level > ground_level))
        {
            if (data)
            {
                // hardcoded in client like this
                if (GetMapId() == 530 && map_data.entry == 2)
                    map_data.entry = 15;
                *data = map_data;
            }
            return map_result;
        }
    }
    return result;
}

bool TerrainInfo::IsInWater(float x, float y, float pZ, GridMapLiquidData *data) const
{
    // Check surface in x, y point for liquid
    if (const_cast<TerrainInfo*>(this)->GetGrid(x, y))
    {
        GridMapLiquidData liquid_status;
        GridMapLiquidData *liquid_ptr = data ? data : &liquid_status;
        if (getLiquidStatus(x, y, pZ, MAP_ALL_LIQUIDS, liquid_ptr))
        {
            //if (liquid_prt->level - liquid_prt->depth_level > 2) //???
                return true;
        }
    }
    return false;
}

bool TerrainInfo::IsUnderWater(float x, float y, float z) const
{
    if (const_cast<TerrainInfo*>(this)->GetGrid(x, y))
    {
        if (getLiquidStatus(x, y, z, MAP_LIQUID_TYPE_WATER|MAP_LIQUID_TYPE_OCEAN)&LIQUID_MAP_UNDER_WATER)
            return true;
    }
    return false;
}

/**
 * Function find higher form water or ground height for current floor
 *
 * @param x, y, z    Coordinates original point at floor level
 *
 * @param pGround    optional arg for retrun calculated by function work ground height, it let avoid in caller code recalculate height for point if it need
 *
 * @param swim       z coordinate can be calculated for select above/at or under z coordinate (for fly or swim/walking by bottom)
 *                   in last cases for in water returned under water height for avoid client set swimming unit as saty at water.
 *
 * @return           calculated z coordinate
 */
float TerrainInfo::GetWaterOrGroundLevel(float x, float y, float z, float* pGround /*= NULL*/, bool swim /*= false*/) const
{
    if (const_cast<TerrainInfo*>(this)->GetGrid(x, y))
    {
        // we need ground level (including grid height version) for proper return water level in point
        float ground_z = GetHeightStatic(x, y, z, true, DEFAULT_WATER_SEARCH);
        if (pGround)
            *pGround = ground_z;

        GridMapLiquidData liquid_status;

        GridMapLiquidStatus res = getLiquidStatus(x, y, ground_z, MAP_ALL_LIQUIDS, &liquid_status);
        return res ? ( swim ? liquid_status.level - 2.0f : liquid_status.level) : ground_z;
    }

    return VMAP_INVALID_HEIGHT_VALUE;
}

GridMap * TerrainInfo::GetGrid( const float x, const float y )
{
    // half opt method
    int gx=(int)(32-x/SIZE_OF_GRIDS);                       //grid x
    int gy=(int)(32-y/SIZE_OF_GRIDS);                       //grid y

    //quick check if GridMap already loaded
    GridMap * pMap = m_GridMaps[gx][gy];
    if(!pMap)
         pMap = LoadMapAndVMap(gx, gy);

    return pMap;
}

GridMap * TerrainInfo::LoadMapAndVMap( const uint32 x, const uint32 y )
{
    //double checked lock pattern
    if(!m_GridMaps[x][y])
    {
        LOCK_GUARD lock(m_mutex);

        if(!m_GridMaps[x][y])
        {
            GridMap * map = new GridMap();

            // map file name
            int len = sWorld.GetDataPath().length() + strlen("maps/%03u%02u%02u.map") + 1;
            char* tmp = new char[len];
            snprintf(tmp, len, (char*)(sWorld.GetDataPath() + "maps/%03u%02u%02u.map").c_str(), m_mapId, x, y);
            DEBUG_FILTER_LOG(LOG_FILTER_MAP_LOADING, "Loading map %s", tmp);

            if(!map->loadData(tmp))
            {
                sLog.outError("Error load map file: \n %s\n", tmp);
                //ASSERT(false);
            }

            delete[] tmp;
            m_GridMaps[x][y] = map;

            //load VMAPs for current map/grid...
            const MapEntry * i_mapEntry = sMapStore.LookupEntry(m_mapId);
            const char* mapName = i_mapEntry ? i_mapEntry->name[sWorld.GetDefaultDbcLocale()] : "UNNAMEDMAP\x0";

            int vmapLoadResult = VMAP::VMapFactory::createOrGetVMapManager()->loadMap((sWorld.GetDataPath()+ "vmaps").c_str(),  m_mapId, x, y);
            switch(vmapLoadResult)
            {
                case VMAP::VMAP_LOAD_RESULT_OK:
                    DEBUG_FILTER_LOG(LOG_FILTER_MAP_LOADING, "VMAP loaded name:%s, id:%d, x:%d, y:%d (vmap rep.: x:%d, y:%d)", mapName, m_mapId, x, y, x, y);
                    break;
                case VMAP::VMAP_LOAD_RESULT_ERROR:
                    DEBUG_FILTER_LOG(LOG_FILTER_MAP_LOADING, "Could not load VMAP name:%s, id:%d, x:%d, y:%d (vmap rep.: x:%d, y:%d)", mapName, m_mapId, x, y, x, y);
                    break;
                case VMAP::VMAP_LOAD_RESULT_IGNORED:
                    DEBUG_FILTER_LOG(LOG_FILTER_MAP_LOADING, "Ignored VMAP name:%s, id:%d, x:%d, y:%d (vmap rep.: x:%d, y:%d)", mapName, m_mapId, x, y, x, y);
                    break;
            }

            // load navmesh
            MMAP::MMapFactory::createOrGetMMapManager()->loadMap(m_mapId, x, y);
        }
    }

    return  m_GridMaps[x][y];
}

float TerrainInfo::GetWaterLevel(float x, float y, float z, float* pGround /*= NULL*/) const
{
    if (const_cast<TerrainInfo*>(this)->GetGrid(x, y))
    {
        // we need ground level (including grid height version) for proper return water level in point
        float ground_z = GetHeightStatic(x, y, z, true, DEFAULT_WATER_SEARCH);
        if (pGround)
            *pGround = ground_z;

        GridMapLiquidData liquid_status;

        GridMapLiquidStatus res = getLiquidStatus(x, y, ground_z, MAP_ALL_LIQUIDS, &liquid_status);
        if (!res)
            return VMAP_INVALID_HEIGHT_VALUE;

        return liquid_status.level;
    }

    return VMAP_INVALID_HEIGHT_VALUE;
}

//////////////////////////////////////////////////////////////////////////

#define CLASS_LOCK MaNGOS::ClassLevelLockable<TerrainManager, ACE_Thread_Mutex>
INSTANTIATE_SINGLETON_2(TerrainManager, CLASS_LOCK);
INSTANTIATE_CLASS_MUTEX(TerrainManager, ACE_Thread_Mutex);

TerrainManager::TerrainManager()
{
}

TerrainManager::~TerrainManager()
{
    for (TerrainDataMap::iterator it = i_TerrainMap.begin(); it != i_TerrainMap.end(); ++it)
        delete it->second;
}

TerrainInfo * TerrainManager::LoadTerrain(const uint32 mapId)
{
    Guard _guard(*this);

    TerrainInfo * ptr = NULL;
    TerrainDataMap::const_iterator iter = i_TerrainMap.find(mapId);
    if(iter == i_TerrainMap.end())
    {
        ptr = new TerrainInfo(mapId);
        i_TerrainMap[mapId] = ptr;
    }
    else
        ptr = (*iter).second;

    return ptr;
}

void TerrainManager::UnloadTerrain(const uint32 mapId)
{
    if(sWorld.getConfig(CONFIG_BOOL_GRID_UNLOAD) == 0)
        return;

    Guard _guard(*this);

    TerrainDataMap::iterator iter = i_TerrainMap.find(mapId);
    if(iter != i_TerrainMap.end())
    {
        TerrainInfo * ptr = (*iter).second;
        //lets check if this object can be actually freed
        if(ptr->IsReferenced() == false)
        {
            i_TerrainMap.erase(iter);
            delete ptr;
        }
    }
}

void TerrainManager::Update(const uint32 diff)
{
    //global garbage collection for GridMap objects and VMaps
    for (TerrainDataMap::iterator iter = i_TerrainMap.begin(); iter != i_TerrainMap.end(); ++iter)
        iter->second->CleanUpGrids(diff);
}

void TerrainManager::UnloadAll()
{
    for (TerrainDataMap::iterator it = i_TerrainMap.begin(); it != i_TerrainMap.end(); ++it)
        delete it->second;

    i_TerrainMap.clear();
}

uint32 TerrainManager::GetAreaIdByAreaFlag(uint16 areaflag,uint32 map_id)
{
    AreaTableEntry const *entry = GetAreaEntryByAreaFlagAndMap(areaflag,map_id);

    if (entry)
        return entry->ID;
    else
        return 0;
}

uint32 TerrainManager::GetZoneIdByAreaFlag(uint16 areaflag,uint32 map_id)
{
    AreaTableEntry const *entry = GetAreaEntryByAreaFlagAndMap(areaflag,map_id);

    if( entry )
        return ( entry->zone != 0 ) ? entry->zone : entry->ID;
    else
        return 0;
}

void TerrainManager::GetZoneAndAreaIdByAreaFlag(uint32& zoneid, uint32& areaid, uint16 areaflag,uint32 map_id)
{
    AreaTableEntry const *entry = GetAreaEntryByAreaFlagAndMap(areaflag,map_id);

    areaid = entry ? entry->ID : 0;
    zoneid = entry ? (( entry->zone != 0 ) ? entry->zone : entry->ID) : 0;
}
