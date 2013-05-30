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

#include "ObjectMgr.h"
#include "Player.h"
#include "Spell.h"
#include "Util.h"
#include "World.h"
#include "WorldSession.h"

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
    GO_FOSSIL_FIND      = 206836,
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

bool Player::GenerateDigSiteLoot(uint16 siteId, DigSite &site)
{
    ResearchSiteDataMap::iterator dataItr = sResearchSiteDataMap.find(siteId);
    if (dataItr == sResearchSiteDataMap.end())
        return false;

    DigSitePositionVector const& loot = dataItr->second.digSites;
    if (loot.empty())
        return false;

    switch (dataItr->second.branch_id)
    {
        case 1: site.find_id = GO_DWARF_FIND; break;
        case 2: site.find_id = GO_DRAENEI_FIND; break;
        case 3: site.find_id = GO_FOSSIL_FIND; break;
        case 4: site.find_id = GO_NIGHT_ELF_FIND; break;
        case 5: site.find_id = GO_NERUBIAN_FIND; break;
        case 6: site.find_id = GO_ORC_FIND; break;
        case 7: site.find_id = GO_TOLVIR_FIND; break;
        case 8: site.find_id = GO_TROLL_FIND; break;
        case 27: site.find_id = GO_VRYKUL_FIND; break;
        default: site.find_id = 0; break;
    }

    DigSitePositionVector::const_iterator entry = loot.begin();
    std::advance(entry, urand(0, loot.size() - 1));

    site.loot_x = entry->x;
    site.loot_y = entry->y;

    return true;
}

uint32 Player::GetSurveyBotEntry(float &orientation)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    uint16 site_id = GetResearchSiteID();
    if (!site_id)
        return 0;

    uint8 i = 0;
    for(; i < MAX_RESEARCH_SITES; ++i)
        if (GetUInt16Value(PLAYER_FIELD_RESERACH_SITE_1 + i / 2, i % 2) == site_id)
            break;

    MANGOS_ASSERT(i < MAX_RESEARCH_SITES);

    DigSite &site = _digSites[i];
    if (site.site_id != site_id)
    {
        if (!GenerateDigSiteLoot(site_id, site))
            return 0;

        site.site_id = site_id;
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
        case GO_DWARF_FIND: currencyId = 384; break;
        case GO_DRAENEI_FIND: currencyId = 398; break;
        case GO_FOSSIL_FIND: currencyId = 393; break;
        case GO_NIGHT_ELF_FIND: currencyId = 394; break;
        case GO_NERUBIAN_FIND: currencyId = 400; break;
        case GO_ORC_FIND: currencyId = 397; break;
        case GO_TOLVIR_FIND: currencyId = 401; break;
        case GO_TROLL_FIND: currencyId = 385; break;
        case GO_VRYKUL_FIND: currencyId = 399; break;
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
        if (!GenerateDigSiteLoot(site_id, site))
            return 0;
    }
    else
    {
        site.clear();
        UseResearchSite(site_id);
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

    for (ResearchSiteDataMap::iterator itr = sResearchSiteDataMap.begin(); itr != sResearchSiteDataMap.end(); ++itr)
    {
        if (itr->second.zone != GetCachedZoneId())
            continue;

        if (!IsPointInZone(pt, itr->second.points))
            continue;

        return itr->second.entry->ID;
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

    int pred_q = q_patt[pred_pt.y < 0][pred_pt.x < 0];

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

    for (ResearchSiteSet::const_iterator itr = _researchSites.begin(); itr != _researchSites.end(); ++itr)
    {
        uint32 id = (*itr);
        ResearchSiteEntry const* rs = GetResearchSiteEntryById(id);

        if (!rs || CanResearchWithSkillLevel(rs->ID) == 2)
            id = 0;

        SetUInt16Value(PLAYER_FIELD_RESERACH_SITE_1 + count / 2, count % 2, id);
        ++count;
    }
}

void Player::ShowResearchProjects()
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return;

    uint8 count = 0;

    for (ResearchProjectSet::const_iterator itr = _researchProjects.begin(); itr != _researchProjects.end(); ++itr)
    {
        SetUInt16Value(PLAYER_FIELD_RESEARCHING_1 + count / 2, count % 2, (*itr));
        ++count;
    }
}

bool Player::CanResearchWithLevel(uint32 site_id)
{
    if (!GetSkillValue(SKILL_ARCHAEOLOGY))
        return false;

    ResearchSiteDataMap::const_iterator itr = sResearchSiteDataMap.find(site_id);
    if (itr != sResearchSiteDataMap.end())
        return getLevel() + 19 >= itr->second.level;

    return true;
}

uint8 Player::CanResearchWithSkillLevel(uint32 site_id)
{
    uint16 skill_now = GetSkillValue(SKILL_ARCHAEOLOGY);
    if (!skill_now)
        return 0;

    ResearchSiteDataMap::const_iterator itr = sResearchSiteDataMap.find(site_id);
    if (itr != sResearchSiteDataMap.end())
    {
        ResearchSiteData const& entry = itr->second;

        uint16 skill_cap = 0;
        switch (entry.entry->mapId)
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

        if (entry.entry->mapId == 530 || entry.entry->mapId == 571)
            return 2;
    }

    return 0;
}

void Player::GenerateResearchSiteInMap(uint32 mapId)
{
    SiteSet tempSites;

    for (ResearchSiteDataMap::const_iterator itr = sResearchSiteDataMap.begin(); itr != sResearchSiteDataMap.end(); ++itr)
    {
        ResearchSiteEntry const* entry = itr->second.entry;
        if (!HasResearchSite(entry->ID) &&
            entry->mapId == mapId &&
            CanResearchWithLevel(entry->ID) &&
            CanResearchWithSkillLevel(entry->ID))
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
    _researchSites.clear();

    Sites tempSites;
    for (ResearchSiteDataMap::const_iterator itr = sResearchSiteDataMap.begin(); itr != sResearchSiteDataMap.end(); ++itr)
    {
        ResearchSiteEntry const* entry = itr->second.entry;
        if (CanResearchWithLevel(entry->ID) && CanResearchWithSkillLevel(entry->ID))
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
        if (entry->rare && urand(0, 100) > chance_mod || IsCompletedRareProject(entry->ID))
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

bool Player::SolveResearchProject(uint32 spellId, SpellCastTargets& targets)
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

    ResearchBranchEntry const* branch = NULL;
    for (uint32 i = 0; i < sResearchBranchStore.GetNumRows(); ++i)
    {
        ResearchBranchEntry const* _branch = sResearchBranchStore.LookupEntry(i);
        if (!_branch)
            continue;

        if (_branch->ID != entry->branchId)
            continue;

        branch = _branch;
        break;
    }

    if (!branch)
        return false;

    uint32 currencyId = branch->currency;
    int32 currencyAmt = int32(entry->req_currency_amt);

    ArchaeologyWeights weights = targets.GetWeights();
    for (ArchaeologyWeights::iterator itr = weights.begin(); itr != weights.end(); ++itr)
    {
        ArchaeologyWeight& w = *itr;
        if (w.type == WEIGHT_KEYSTONE)
        {
            ItemPrototype const* proto = sObjectMgr.GetItemPrototype(w.keystone.itemId);
            if (!proto)
                return false;

            if (proto->GetCurrencySubstitutionId() != currencyId)
                return false;

            if (w.keystone.itemCount > entry->Complexity)
                return false;

            if (!HasItemCount(w.keystone.itemId, w.keystone.itemCount))
                return false;

            currencyAmt -= int32(proto->CurrencySubstitutionCount * w.keystone.itemCount);
        }
    }

    if (currencyAmt > 0 && !HasCurrencyCount(currencyId, currencyAmt))
        return false;

    ModifyCurrencyCount(currencyId, -currencyAmt);

    for (ArchaeologyWeights::iterator itr = weights.begin(); itr != weights.end(); ++itr)
    {
        ArchaeologyWeight& w = *itr;
        if (w.type == WEIGHT_KEYSTONE)
            DestroyItemCount(w.keystone.itemId, w.keystone.itemCount, true);
    }

    _researchProjects.erase(entry->ID);

    UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_COMPLETE_ARCHAEOLOGY_PROJECTS, entry->ID, 1, NULL, 0);

    AddCompletedProject(entry);

    ProjectSet tempProjects;
    uint32 chance_mod = skill_now / 50;

    for (std::set<ResearchProjectEntry const*>::const_iterator itr = sResearchProjectSet.begin(); itr != sResearchProjectSet.end(); ++itr)
    {
        ResearchProjectEntry const* project = (*itr);
        if (project->branchId == entry->branchId)
        {
            if (project->rare && urand(0, 100) > chance_mod || IsCompletedRareProject(project->ID))
                continue;

            tempProjects.insert(project->ID);
        }
    }

    ProjectSet::const_iterator itr = tempProjects.begin();
    std::advance(itr, urand(0, tempProjects.size() - 1));

    _researchProjects.insert((*itr));
    _archaeologyChanged = true;

    WorldPacket data (SMSG_RESEARCH_COMPLETE, 4*3);
    data << uint32(entry->branchId);
    data << uint32(*itr);
    data << uint32(entry->ID);
    SendDirectMessage(&data);

    ShowResearchProjects();
    return true;
}

void Player::AddCompletedProject(ResearchProjectEntry const* entry)
{
    for (CompletedProjectList::iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
    {
        if (itr->entry->ID == entry->ID)
        {
            ++itr->count;
            return;
        }
    }

    _completedProjects.push_back(CompletedProject(entry));
}

bool Player::IsCompletedRareProject(uint32 id)
{
    for (CompletedProjectList::const_iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
        if (id == itr->entry->ID && itr->entry->rare)
            return true;

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

    ss << "INSERT INTO character_archaeology (guid, sites, counts, projects) VALUES (";

    ss << GetGUIDLow() << ", '";

    for (ResearchSiteSet::const_iterator itr = _researchSites.begin(); itr != _researchSites.end(); ++itr)
        ss << (*itr) << " ";

    ss << "', '";

    for (uint8 j = 0; j < MAX_RESEARCH_SITES; ++j)
        ss << uint32(_digSites[j].count) << " ";

    ss << "', '";

    for (ResearchProjectSet::const_iterator itr = _researchProjects.begin(); itr != _researchProjects.end(); ++itr)
        ss << (*itr) << " ";

    ss << "')";

    CharacterDatabase.Execute(ss.str().c_str());

    for (CompletedProjectList::iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
    {
        CharacterDatabase.PExecute("REPLACE INTO character_archaeology_finds (guid, id, count, date) VALUES (%u, %u, %u, FROM_UNIXTIME(%u))",
            GetGUIDLow(), itr->entry->ID, itr->count, itr->date);
    }

    _archaeologyChanged = false;
}

void Player::_LoadArchaeology(QueryResult* result)
{
    for (uint8 i = 0; i < MAX_RESEARCH_SITES; ++i)
        _digSites[i].count = 0;

    if (!sWorld.getConfig(CONFIG_BOOL_ARCHAEOLOGY_ENABLED))
    {
        delete result;
        return;
    }

    if (!result)
    {
        GenerateResearchSites();
        GenerateResearchProjects();
        return;
    }

    Field* fields = result->Fetch();

    // Loading current zones
    Tokens tokens = StrSplit(fields[0].GetCppString(), " ");
    if (tokens.size() != 0 && tokens.size() <= MAX_RESEARCH_SITES)
    {
        _researchSites.clear();

        for (uint8 i = 0; i < tokens.size(); ++i)
            _researchSites.insert(uint32(atoi(tokens[i].c_str())));
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
            _researchProjects.insert(uint32(atoi(tokens[i].c_str())));
    }
    else
        GenerateResearchProjects();

    delete result;
}

void Player::_LoadArchaeologyFinds(QueryResult* result)
{
    // "SELECT id, count, UNIX_TIMESTAMP(date) FROM character_archaeology_finds WHERE guid = %u"
    if (!result)
        return;

    do
    {
        Field* fields = result->Fetch();

        uint32 id = fields[0].GetUInt32();
        uint32 count = fields[1].GetUInt32();
        uint32 date = fields[2].GetUInt32();

        ResearchProjectEntry const* rs = sResearchProjectStore.LookupEntry(id);
        if (!rs)
            continue;

        CompletedProject cp;
        cp.entry = rs;
        cp.count = count;
        cp.date = date;

        _completedProjects.push_back(cp);
    }
    while (result->NextRow());
    delete result;
}

void Player::SendCompletedProjects()
{
    if (!HasSkill(SKILL_ARCHAEOLOGY))
        return;

    WorldPacket data(SMSG_RESEARCH_SETUP_HISTORY, 3 + _completedProjects.size() * 3 * 4);

    data.WriteBits(_completedProjects.size(), 22);

    for (CompletedProjectList::iterator itr = _completedProjects.begin(); itr != _completedProjects.end(); ++itr)
    {
        data << uint32(itr->entry->ID);
        data << uint32(itr->count);
        data << uint32(itr->date);
    }

    SendDirectMessage(&data);
}

void WorldSession::HandleRequestResearchHistory(WorldPacket& recv_data)
{
    // null opcode
    DEBUG_LOG("World: received CMSG_REQUEST_RESEARCH_HISTORY from %s (account %u)", GetPlayerName(), GetAccountId());

    _player->SendCompletedProjects();
}
