/*
 * Copyright (C) 2005-2013 MaNGOS <http://getmangos.com/>
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

#include "Player.h"
#include "ObjectMgr.h"
#include "Util.h"
#include "World.h"

#define MAX_RESEARCH_PROJECTS 9

enum ArchaeologyBotDistance
{
    ARCHAEOLOGY_DIG_SITE_FAR_DIST   = 40,
    ARCHAEOLOGY_DIG_SITE_MED_DIST   = 20,
    ARCHAEOLOGY_DIG_SITE_CLOSE_DIST = 8
};

enum ArchaeologyFinds
{
    GO_DWARF_FIND       = 204282,
    GO_DRAENEI_FIND     = 207188,
    GO_FISSIL_FIND      = 206836,
    GO_NIGHT_ELF_FIND   = 203071,
    GO_NERUBIAN_FIND    = 203078,
    GO_ORC_FIND         = 207187,
    GO_TOLVIR_FIND      = 207190,
    GO_TROLL_FIND       = 202655,
    GO_VRYKUL_FIND      = 207189,
};

enum ArchaeologyBots
{
    ARCHAEOLOGY_DIG_SITE_FAR_SURVEYBOT      = 206590,
    ARCHAEOLOGY_DIG_SITE_MEDIUM_SURVEYBOT   = 206589,
    ARCHAEOLOGY_DIG_SITE_CLOSE_SURVEYBOT    = 204272
};

const static uint16 _mapIds[] = { 0, 1, 530, 571, 0 };

const static uint8 _races[] = {1, 2, 3, 4, 5, 6, 7, 8, 27, 0 };

const static int q_patt[2][2] = { { 0, 1 }, { 3, 2 } };

const static uint8 currencyAmountForLevel[8][2] =
{
    { 3, 5 },   // 1
    { 5, 7 },   // 75
    { 7, 8 },   // 150
    { 8, 10 },  // 225
    { 10, 11 }, // 300
    { 11, 12 }, // 375
    { 12, 13 }, // 450
    { 13, 14 }  // 525
};

typedef std::set<uint32> SiteSet;
typedef std::map<uint32, SiteSet> Sites;
typedef std::set<uint32> ProjectSet;
typedef std::map<uint32, ProjectSet> Projects;

bool Player::GenerateDigitLoot(uint16 siteId, DigSite &site)
{
    ResearchLootVector const& loot = sObjectMgr.GetResearchLoot();
    if (loot.empty())
        return false;

    site.find_id = 0;

    ResearchLootVector lootList;
    for (ResearchLootVector::const_iterator itr = loot.begin(); itr != loot.end(); ++itr)
    {
        ResearchLoot entry = (*itr);
        if (entry.site_id != siteId)
            continue;

        if (site.find_id == 0)
        {
            switch (entry.branch_id)
            {
                case 1: site.find_id = GO_DWARF_FIND; break;
                case 2: site.find_id = GO_DRAENEI_FIND; break;
                case 3: site.find_id = GO_FISSIL_FIND; break;
                case 4: site.find_id = GO_NIGHT_ELF_FIND; break;
                case 5: site.find_id = GO_NERUBIAN_FIND; break;
                case 6: site.find_id = GO_ORC_FIND; break;
                case 7: site.find_id = GO_TOLVIR_FIND; break;
                case 8: site.find_id = GO_TROLL_FIND; break;
                case 27: site.find_id = GO_VRYKUL_FIND; break;
                default: site.find_id = 0; break;
            }
        }

        lootList.push_back(entry);
    }

    if (lootList.empty())
        return false;

    ResearchLootVector::const_iterator entry = lootList.begin();
    std::advance(entry, urand(0, lootList.size() - 1));

    site.loot_x = entry->x;
    site.loot_y = entry->y;
    //site.loot_z = entry->z;

    return true;
}

uint32 Player::GetSurveyBotEntry(float &orientation)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    uint16 siteId = GetResearchSiteID();
    if (!siteId)
        return 0;

    uint32 at_pos = 0xFFFF;

    for(uint8 i = 0; i < MAX_RESEARCH_SITES / 2; ++i)
    {
        //Replace by GetUInt16Value
        uint32 site_now_1 = GetUInt32Value(PLAYER_FIELD_RESERACH_SITE_1 + i) & 0xFFFF;
        uint32 site_now_2 = GetUInt32Value(PLAYER_FIELD_RESERACH_SITE_1 + i) >> 16;

        if (siteId == site_now_1)
        {
            at_pos = i * 2;
            break;
        }
        if (siteId == site_now_2)
        {
            at_pos = i * 2 + 1;
            break;
        }
    }

    if (at_pos == 0xFFFF)
        return 0;

    MANGOS_ASSERT(at_pos < MAX_RESEARCH_SITES);

    DigSite &site = _digSites[at_pos];
    if (site.site_id != siteId)
    {
        if (!GenerateDigitLoot(siteId, site))
            return 0;

        site.site_id = siteId;
    }

    orientation = GetAngle(site.loot_x, site.loot_y);
    float dist_now = GetDistance2d(site.loot_x, site.loot_y);

    if (dist_now >= ARCHAEOLOGY_DIG_SITE_FAR_DIST)
        return ARCHAEOLOGY_DIG_SITE_FAR_SURVEYBOT;
    if (dist_now >= ARCHAEOLOGY_DIG_SITE_MED_DIST)
        return ARCHAEOLOGY_DIG_SITE_MEDIUM_SURVEYBOT;
    if (dist_now >= ARCHAEOLOGY_DIG_SITE_CLOSE_DIST)
        return ARCHAEOLOGY_DIG_SITE_CLOSE_SURVEYBOT;

    if (skill_now < 50)
        UpdateSkill(SKILL_ARCHAEOLOGY, 1);

    uint32 currencyId = 0;
    switch (site.find_id)
    {
        case 204282: currencyId = 384; break;
        case 207188: currencyId = 398; break;
        case 206836: currencyId = 393; break;
        case 203071: currencyId = 394; break;
        case 203078: currencyId = 400; break;
        case 207187: currencyId = 397; break;
        case 207190: currencyId = 401; break;
        case 202655: currencyId = 385; break;
        case 207189: currencyId = 399; break;
    }

    if (currencyId)
    {
        int level = int(skill_now / 75);

        if (level > 8)
            return 0;

        uint32 amount = urand(uint32(currencyAmountForLevel[level][0]), uint32(currencyAmountForLevel[level][1]));
        ModifyCurrencyCount(currencyId, amount);
    }

    // We cannot implement special race fragments, so don't  spawn go
    /*SummonGameObject(
        m_digSites[at_pos].find_id,
        m_digSites[at_pos].loot_x,
        m_digSites[at_pos].loot_y,
        m_digSites[at_pos].loot_z,
        0, 0, 0, 0, 0,
        time(NULL) + 30000);*/

    if (site.count < 2)
    {
        ++site.count;
        if (!GenerateDigitLoot(siteId, site))
            return 0;
    }
    else
    {
        site.clear();
        UseResearchSite(siteId);
    }

    _archaeologyChanged = true;

    return 0;
}

// find id of research site that we are on
uint16 Player::GetResearchSiteID()
{
    ResearchPOIPoint pt;
    pt.x = int32(GetPositionX());
    pt.y = int32(GetPositionY());

    for (ResearchZoneData::iterator itr = sResearchZones.begin(); itr != sResearchZones.end(); ++itr)
    {
        if (itr->second.zone != GetCachedZoneId())
            continue;

        if (!IsPointInZone(pt, itr->second.points))
            continue;

        return itr->second.siteId;
    }

    return 0;
}

bool Player::IsPointInZone(ResearchPOIPoint &test, ResearchPOIPointVector &polygon)
{
    if (polygon.size() < 3)
        return false;

    ResearchPOIPointVector::const_iterator end = polygon.end();
    ResearchPOIPoint pred_pt = polygon.back();
    pred_pt.x -= test.x;
    pred_pt.y -= test.y;

    int pred_q = q_patt[pred_pt.y < 0][pred_pt.x<  0];

    int w = 0;

    for (std::vector<ResearchPOIPoint>::const_iterator iter = polygon.begin(); iter != end; ++iter)
    {
        ResearchPOIPoint cur_pt = *iter;

        cur_pt.x -= test.x;
        cur_pt.y -= test.y;

        int q = q_patt[cur_pt.y < 0][cur_pt.x < 0];

        switch (q - pred_q)
        {
            case -3:
                ++w;
                break;
            case 3:
                --w;
                break;
            case -2:
                if (pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x)
                    ++w;
                break;
            case 2:
                if (!(pred_pt.x * cur_pt.y >= pred_pt.y * cur_pt.x))
                    --w;
                break;
        }

        pred_pt = cur_pt;
        pred_q = q;
    }
    return w != 0;
}

void Player::UseResearchSite(uint32 id)
{
    _researchSites.erase(id);
    GenerateResearchSiteInMap(GetMapId());
}

void Player::ShowResearchSites()
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    uint8 count = 0;
    uint32 newvalue;

    for (ResearchSiteSet::const_iterator itr = _researchSites.begin(); itr != _researchSites.end(); ++itr)
    {
        uint32 id = (*itr);
        ResearchSiteEntry const* rs = GetResearchSiteEntryById(id);

        if (!rs || CanResearchWithSkillLevel(rs->POIid) == 2)
            id = 0;

        if (count % 2 == 1)
        {
            newvalue |= id;
            SetUInt32Value(PLAYER_FIELD_RESERACH_SITE_1 + count / 2, newvalue);
        }
        else
            newvalue = id << 16;

        ++count;
    }
}

void Player::ShowResearchProjects()
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    uint8 count = 0;
    uint32 newvalue;

    for (ResearchProjectSet::const_iterator itr = _researchProjects.begin(); itr != _researchProjects.end(); ++itr)
    {
        if (count % 2 == 1)
        {
            newvalue |= (*itr);
            SetUInt32Value(PLAYER_FIELD_RESEARCHING_1 + count / 2, newvalue);
        }
        else if (count == 8)
        {
            SetUInt32Value(PLAYER_FIELD_RESEARCHING_1 + count / 2, (*itr));
            break;
        }
        else
            newvalue = (*itr) << 16;

        ++count;
    }
}

bool Player::CanResearchWithLevel(uint32 POIid)
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return false;

    ResearchZoneData::const_iterator itr = sResearchZones.find(POIid);
    if (itr != sResearchZones.end())
        return getLevel() + 19 >= itr->second.level;

    return true;
}

uint8 Player::CanResearchWithSkillLevel(uint32 POIid)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    ResearchZoneData::const_iterator itr = sResearchZones.find(POIid);
    if (itr != sResearchZones.end())
    {
        ResearchZone const& entry = itr->second;

        uint16 skill_cap = 0;
        switch (entry.map)
        {
            case 0:
                if (entry.zone == 4922) // Twilight Hightlands
                    skill_cap = 450;
                break;
            case 1:
                if (entry.zone == 616) // Hyjal
                    skill_cap = 450;
                else if (entry.zone == 5034) // Uldum
                    skill_cap = 450;
                break;
            case 530:
                skill_cap = 275; // Outland
                break;
            case 571:
                skill_cap = 350; // Northrend
                break;
        }

        if (skill_now >= skill_cap)
            return 1;

        if (entry.map == 530 || entry.map == 571)
            return 2;
    }

    return 0;
}

ResearchSiteEntry const* Player::GetResearchSiteEntryById(uint32 id)
{
    if (sResearchSiteSet.empty())
        return NULL;

    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
        if ((*itr)->ID == id)
            return *itr;

    return NULL;
}

void Player::GenerateResearchSiteInMap(uint32 mapId)
{
    if (sResearchSiteSet.empty())
        return;

    SiteSet tempSites;

    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
    {
        ResearchSiteEntry const* entry = (*itr);
        if (!HasResearchSite(entry->ID) &&
            entry->mapId == mapId &&
            CanResearchWithLevel(entry->POIid) &&
            CanResearchWithSkillLevel(entry->POIid))
            tempSites.insert(entry->ID);
    }

    if (tempSites.empty())
        return;

    SiteSet::const_iterator entry = tempSites.begin();
    std::advance(entry, urand(0, tempSites.size() - 1));

    _researchSites.insert((*entry));
    _archaeologyChanged = true;

    ShowResearchSites();
}

void Player::GenerateResearchSites()
{
    if (sResearchSiteSet.empty())
        return;

    _researchSites.clear();

    Sites tempSites;
    for (std::set<ResearchSiteEntry const*>::const_iterator itr = sResearchSiteSet.begin(); itr != sResearchSiteSet.end(); ++itr)
    {
        ResearchSiteEntry const* entry = (*itr);
        if (CanResearchWithLevel(entry->POIid) && CanResearchWithSkillLevel(entry->POIid))
            tempSites[entry->mapId].insert(entry->ID);
    }

    uint16 const* map = _mapIds;
    do
    {
        uint8 mapMax = std::min<int>(tempSites[*map].size(), 4);

        for (uint8 i = 0; i < mapMax;)
        {
            SiteSet::const_iterator itr = tempSites[*map].begin();
            std::advance(itr, urand(0, tempSites[*map].size() - 1));
            if (!HasResearchSite((*itr)))
            {
                _researchSites.insert((*itr));
                ++i;
            }
        }
    }
    while (*++map);

    _archaeologyChanged = true;

    ShowResearchSites();
}

void Player::GenerateResearchProjects()
{
    if (sResearchProjectSet.empty())
        return;

    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return;

    _researchProjects.clear();

    Projects tempProjects;
    uint32 chance_mod = skill_now / 50;

    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        ResearchProjectEntry const* entry = (*itr);
        if (entry->rare && urand(0, 100) > chance_mod || IsCompletedProject(entry->ID))
            continue;

        tempProjects[entry->branchId].insert(entry->ID);
    }

    uint8 const* race = _races;
    ProjectSet::const_iterator itr;
    do
    {
        itr = tempProjects[*race].begin();
        std::advance(itr, urand(0, tempProjects[*race].size() - 1));
        _researchProjects.insert((*itr));
    }
    while (*++race);

    _archaeologyChanged = true;

    ShowResearchProjects();
}

bool Player::SolveResearchProject(uint32 spellId)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return false;

    ResearchProjectEntry const* entry = NULL;
    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        if ((*itr)->spellId != spellId)
            continue;

        entry = (*itr);
        break;
    }

    if (!entry || !HasResearchProject(entry->ID))
        return false;

    for (uint32 i = 0; i < sResearchBranchStore.GetNumRows(); ++i)
    {
        ResearchBranchEntry const* branch = sResearchBranchStore.LookupEntry(i);
        if (!branch)
            continue;

        if (branch->ID != entry->branchId)
            continue;

        if (!HasCurrencyCount(branch->currency, entry->req_currency_amt))
            return false;

        ModifyCurrencyCount(branch->currency, -int32(entry->req_currency_amt));
        break;
    }

    _researchProjects.erase(entry->ID);

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS, entry->ID, 1, NULL, 0);

    if (entry->rare)
        _completedProjects.insert(entry->ID);

    ProjectSet tempProjects;
    uint32 chance_mod = skill_now / 50;

    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        ResearchProjectEntry const* project = (*itr);
        if (project->branchId == entry->branchId)
        {
            if (project->rare && urand(0, 100) > chance_mod || IsCompletedProject(project->ID))
                continue;

            tempProjects.insert(project->ID);
        }
    }

    ProjectSet::const_iterator itr = tempProjects.begin();
    std::advance(itr, urand(0, tempProjects.size() - 1));

    _researchProjects.insert((*itr));
    _archaeologyChanged = true;

    ShowResearchProjects();
    return true;
}

bool Player::IsCompletedProject(uint32 id)
{
    if (_completedProjects.empty())
        return false;

    for (CompletedProjectSet::const_iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
    {
        if (id == (*itr))
            return true;
    }

    return false;
}

void Player::_SaveArchaeology()
{
    if (!sWorld.getConfig(CONFIG_BOOL_ARCHAEOLOGY_ENABLED))
        return;

    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    if (!_archaeologyChanged)
        return;

    CharacterDatabase.PExecute("DELETE FROM character_archaeology WHERE guid = '%u'", GetGUIDLow());

    std::ostringstream ss;

    ss << "INSERT INTO character_archaeology  (guid, sites, counts, projects, completed) VALUES (";

    ss << GetGUIDLow() << ", '";

    for (ResearchSiteSet::const_iterator itr = _researchSites.begin(); itr != _researchSites.end(); ++itr)
        ss << (*itr) << " ";

    ss << "', '";

    for (uint8 j = 0; j < MAX_RESEARCH_SITES; ++j)
        ss << uint32(_digSites[j].count) << " ";

    ss << "', '";

    for (ResearchProjectSet::const_iterator itr = _researchProjects.begin(); itr != _researchProjects.end(); ++itr)
        ss << (*itr) << " ";

    ss << "', '";

    for (CompletedProjectSet::const_iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
        ss << (*itr) << " ";

    ss << "')";

    CharacterDatabase.Execute(ss.str().c_str());

    _archaeologyChanged = false;
}

void Player::_LoadArchaeology(QueryResult* result)
{
    for (uint8 i = 0; i < MAX_RESEARCH_SITES; ++i)
        _digSites[i].count = 0;

    if (!result)
        return;

    if (!sWorld.getConfig(CONFIG_BOOL_ARCHAEOLOGY_ENABLED))
    {
        delete result;
        return;
    }

    Field* fields = result->Fetch();

    // Loading current zones
    Tokens tokens = StrSplit(fields[0].GetCppString(), " ");
    if (tokens.size() != 0 && tokens.size() <= MAX_RESEARCH_SITES)
    {
        _researchSites.clear();

        for (uint8 i = 0; i < tokens.size(); ++i)
        {
            _researchSites.insert(uint32(atoi(tokens[i].c_str())));
        }
    }
    else
        GenerateResearchSites();

    // Loading current zone info
    tokens = StrSplit(fields[1].GetCppString(), " ");
    if (tokens.size() == MAX_RESEARCH_SITES)
    {
        for (uint8 i = 0; i < MAX_RESEARCH_SITES; ++i)
            _digSites[i].count = uint32(atoi(tokens[i].c_str()));
    }

    // Loading current projects
    tokens = StrSplit(fields[2].GetCppString(), " ");
    if (tokens.size() == MAX_RESEARCH_PROJECTS)
    {
        _researchProjects.clear();

        for (uint8 i = 0; i < MAX_RESEARCH_PROJECTS; ++i)
        {
            _researchProjects.insert(uint32(atoi(tokens[i].c_str())));
        }
    }
    else
        GenerateResearchProjects();

    // Loading completed projects
    tokens = StrSplit(fields[3].GetCppString(), " ");

    for (Tokens::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
        _completedProjects.insert(atoi(itr->c_str()));

    delete result;
}




