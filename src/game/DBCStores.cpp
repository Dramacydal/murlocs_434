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

#include "DBCStores.h"
#include "Policies/Singleton.h"
#include "Log.h"
#include "ProgressBar.h"
#include "SharedDefines.h"
#include "SpellAuraDefines.h"
#include "ObjectGuid.h"

#include "DBCfmt.h"
#include "DBCEnums.h"

#include <map>

typedef std::map<uint16,uint32> AreaFlagByAreaID;
typedef std::map<uint32,uint32> AreaFlagByMapID;

struct WMOAreaTableTripple
{
    WMOAreaTableTripple(int32 r, int32 a, int32 g) : groupId(g), rootId(r), adtId(a)
    {
    }

    bool operator <(const WMOAreaTableTripple& b) const
    {
        return memcmp(this, &b, sizeof(WMOAreaTableTripple))<0;
    }

    // ordered by entropy; that way memcmp will have a minimal medium runtime
    int32 groupId;
    int32 rootId;
    int32 adtId;
};

typedef std::map<WMOAreaTableTripple, WMOAreaTableEntry const *> WMOAreaInfoByTripple;

DBCStorage <AreaTableEntry> sAreaStore(AreaTableEntryfmt);
DBCStorage <AreaGroupEntry> sAreaGroupStore(AreaGroupEntryfmt);
static AreaFlagByAreaID sAreaFlagByAreaID;
static AreaFlagByMapID  sAreaFlagByMapID;                   // for instances without generated *.map files

static WMOAreaInfoByTripple sWMOAreaInfoByTripple;

DBCStorage <AchievementEntry> sAchievementStore(Achievementfmt);
DBCStorage <AchievementCriteriaEntry> sAchievementCriteriaStore(AchievementCriteriafmt);
DBCStorage <AreaTriggerEntry> sAreaTriggerStore(AreaTriggerEntryfmt);
DBCStorage <ArmorLocationEntry> sArmorLocationStore(ArmorLocationfmt);
DBCStorage <AuctionHouseEntry> sAuctionHouseStore(AuctionHouseEntryfmt);
DBCStorage <BankBagSlotPricesEntry> sBankBagSlotPricesStore(BankBagSlotPricesEntryfmt);
DBCStorage <BattlemasterListEntry> sBattlemasterListStore(BattlemasterListEntryfmt);
DBCStorage <BarberShopStyleEntry> sBarberShopStyleStore(BarberShopStyleEntryfmt);
DBCStorage <CharStartOutfitEntry> sCharStartOutfitStore(CharStartOutfitEntryfmt);
DBCStorage <CharTitlesEntry> sCharTitlesStore(CharTitlesEntryfmt);
DBCStorage <ChatChannelsEntry> sChatChannelsStore(ChatChannelsEntryfmt);
DBCStorage <ChrClassesEntry> sChrClassesStore(ChrClassesEntryfmt);
DBCStorage <ChrPowerTypesEntry> sChrPowerTypesStore(ChrClassesXPowerTypesfmt);
// pair<class,power> => powerIndex
uint32 sChrClassXPowerTypesStore[MAX_CLASSES][MAX_POWERS];
// pair<class,powerIndex> => power
uint32 sChrClassXPowerIndexStore[MAX_CLASSES][MAX_STORED_POWERS];
DBCStorage <ChrRacesEntry> sChrRacesStore(ChrRacesEntryfmt);
DBCStorage <CinematicSequencesEntry> sCinematicSequencesStore(CinematicSequencesEntryfmt);
DBCStorage <CreatureDisplayInfoEntry> sCreatureDisplayInfoStore(CreatureDisplayInfofmt);
DBCStorage <CreatureDisplayInfoExtraEntry> sCreatureDisplayInfoExtraStore(CreatureDisplayInfoExtrafmt);
DBCStorage <CreatureFamilyEntry> sCreatureFamilyStore(CreatureFamilyfmt);
DBCStorage <CreatureModelDataEntry> sCreatureModelDataStore(CreatureModelDatafmt);
DBCStorage <CreatureSpellDataEntry> sCreatureSpellDataStore(CreatureSpellDatafmt);
DBCStorage <CreatureTypeEntry> sCreatureTypeStore(CreatureTypefmt);
DBCStorage <CurrencyTypesEntry> sCurrencyTypesStore(CurrencyTypesfmt);

DBCStorage <DestructibleModelDataEntry> sDestructibleModelDataStore(DestructibleModelDatafmt);
DBCStorage <DungeonEncounterEntry> sDungeonEncounterStore(DungeonEncounterfmt);
DBCStorage <DurabilityQualityEntry> sDurabilityQualityStore(DurabilityQualityfmt);
DBCStorage <DurabilityCostsEntry> sDurabilityCostsStore(DurabilityCostsfmt);

DBCStorage <EmotesEntry> sEmotesStore(EmotesEntryfmt);
DBCStorage <EmotesTextEntry> sEmotesTextStore(EmotesTextEntryfmt);

typedef std::map<uint32,SimpleFactionsList> FactionTeamMap;
static FactionTeamMap sFactionTeamMap;
DBCStorage <FactionEntry> sFactionStore(FactionEntryfmt);
DBCStorage <FactionTemplateEntry> sFactionTemplateStore(FactionTemplateEntryfmt);

DBCStorage <GameObjectDisplayInfoEntry> sGameObjectDisplayInfoStore(GameObjectDisplayInfofmt);
DBCStorage <GemPropertiesEntry> sGemPropertiesStore(GemPropertiesEntryfmt);
DBCStorage <GlyphPropertiesEntry> sGlyphPropertiesStore(GlyphPropertiesfmt);
DBCStorage <GlyphSlotEntry> sGlyphSlotStore(GlyphSlotfmt);

DBCStorage <GtBarberShopCostBaseEntry>    sGtBarberShopCostBaseStore(GtBarberShopCostBasefmt);
DBCStorage <GtCombatRatingsEntry>         sGtCombatRatingsStore(GtCombatRatingsfmt);
DBCStorage <GtChanceToMeleeCritBaseEntry> sGtChanceToMeleeCritBaseStore(GtChanceToMeleeCritBasefmt);
DBCStorage <GtChanceToMeleeCritEntry>     sGtChanceToMeleeCritStore(GtChanceToMeleeCritfmt);
DBCStorage <GtChanceToSpellCritBaseEntry> sGtChanceToSpellCritBaseStore(GtChanceToSpellCritBasefmt);
DBCStorage <GtChanceToSpellCritEntry>     sGtChanceToSpellCritStore(GtChanceToSpellCritfmt);
DBCStorage <GtOCTClassCombatRatingScalarEntry> sGtOCTClassCombatRatingScalarStore(GtOCTClassCombatRatingScalarfmt);
//DBCStorage <GtOCTRegenMPEntry>            sGtOCTRegenMPStore(GtOCTRegenMPfmt);  -- not used currently
DBCStorage <GtOCTHpPerStaminaEntry>       sGtOCTHpPerStaminaStore(GtOCTHpPerStaminafmt);
DBCStorage <GtRegenMPPerSptEntry>         sGtRegenMPPerSptStore(GtRegenMPPerSptfmt);
DBCStorage <GtSpellScalingEntry>          sGtSpellScalingStore(GtSpellScalingfmt);
DBCStorage <GtOCTBaseHPByClassEntry>      sGtOCTBaseHPByClassStore(GtOCTBaseHPByClassfmt);
DBCStorage <GtOCTBaseMPByClassEntry>      sGtOCTBaseMPByClassStore(GtOCTBaseMPByClassfmt);
DBCStorage <GuildPerkSpellsEntry>         sGuildPerkSpellsStore(GuildPerkSpellsfmt);

DBCStorage <HolidaysEntry>                sHolidaysStore(Holidaysfmt);

DBCStorage <ImportPriceArmorEntry>        sImportPriceArmorStore(ImportPriceArmorfmt);
DBCStorage <ImportPriceQualityEntry>      sImportPriceQualityStore(ImportPriceQualityfmt);
DBCStorage <ImportPriceShieldEntry>       sImportPriceShieldStore(ImportPriceShieldfmt);
DBCStorage <ImportPriceWeaponEntry>       sImportPriceWeaponStore(ImportPriceWeaponfmt);
DBCStorage <ItemPriceBaseEntry>           sItemPriceBaseStore(ItemPriceBasefmt);
DBCStorage <ItemArmorQualityEntry>        sItemArmorQualityStore(ItemArmorQualityfmt);
DBCStorage <ItemArmorShieldEntry>         sItemArmorShieldStore(ItemArmorShieldfmt);
DBCStorage <ItemArmorTotalEntry>          sItemArmorTotalStore(ItemArmorTotalfmt);
DBCStorage <ItemBagFamilyEntry>           sItemBagFamilyStore(ItemBagFamilyfmt);
DBCStorage <ItemClassEntry>               sItemClassStore(ItemClassfmt);
DBCStorage <ItemDamageEntry>              sItemDamageAmmoStore(ItemDamagefmt);
DBCStorage <ItemDamageEntry>              sItemDamageOneHandStore(ItemDamagefmt);
DBCStorage <ItemDamageEntry>              sItemDamageOneHandCasterStore(ItemDamagefmt);
DBCStorage <ItemDamageEntry>              sItemDamageRangedStore(ItemDamagefmt);
DBCStorage <ItemDamageEntry>              sItemDamageThrownStore(ItemDamagefmt);
DBCStorage <ItemDamageEntry>              sItemDamageTwoHandStore(ItemDamagefmt);
DBCStorage <ItemDamageEntry>              sItemDamageTwoHandCasterStore(ItemDamagefmt);
DBCStorage <ItemDamageEntry>              sItemDamageWandStore(ItemDamagefmt);
DBCStorage <ItemDisenchantLootEntry>      sItemDisenchantLootStore(ItemDisenchantLootfmt);
//DBCStorage <ItemDisplayInfoEntry>         sItemDisplayInfoStore(ItemDisplayTemplateEntryfmt); -- not used currently
DBCStorage <ItemLimitCategoryEntry>       sItemLimitCategoryStore(ItemLimitCategoryEntryfmt);
DBCStorage <ItemRandomPropertiesEntry>    sItemRandomPropertiesStore(ItemRandomPropertiesfmt);
DBCStorage <ItemRandomSuffixEntry>        sItemRandomSuffixStore(ItemRandomSuffixfmt);
DBCStorage <ItemReforgeEntry>             sItemReforgeStore(ItemReforgefmt);
DBCStorage <ItemSetEntry> sItemSetStore(ItemSetEntryfmt);

DBCStorage <LFGDungeonEntry> sLFGDungeonStore(LFGDungeonEntryfmt);

DBCStorage <LiquidTypeEntry> sLiquidTypeStore(LiquidTypefmt);
DBCStorage <LockEntry> sLockStore(LockEntryfmt);

DBCStorage <MailTemplateEntry> sMailTemplateStore(MailTemplateEntryfmt);
DBCStorage <MapEntry> sMapStore(MapEntryfmt);

DBCStorage <MapDifficultyEntry> sMapDifficultyStore(MapDifficultyEntryfmt);
MapDifficultyMap sMapDifficultyMap;

DBCStorage <MovieEntry> sMovieStore(MovieEntryfmt);
DBCStorage <MountCapabilityEntry> sMountCapabilityStore(MountCapabilityfmt);
DBCStorage <MountTypeEntry> sMountTypeStore(MountTypefmt);

DBCStorage <NumTalentsAtLevelEntry> sNumTalentsAtLevelStore(NumTalentsAtLevelfmt);

DBCStorage <OverrideSpellDataEntry> sOverrideSpellDataStore(OverrideSpellDatafmt);
DBCStorage <QuestFactionRewardEntry> sQuestFactionRewardStore(QuestFactionRewardfmt);
DBCStorage <QuestPOIPointEntry> sQuestPOIPointStore(QuestPOIPointfmt);
DBCStorage <QuestSortEntry> sQuestSortStore(QuestSortEntryfmt);
DBCStorage <QuestXPLevel> sQuestXPLevelStore(QuestXPLevelfmt);

DBCStorage <PhaseEntry> sPhaseStore(Phasefmt);
DBCStorage <PvPDifficultyEntry> sPvPDifficultyStore(PvPDifficultyfmt);

DBCStorage <RandomPropertiesPointsEntry> sRandomPropertiesPointsStore(RandomPropertiesPointsfmt);

DBCStorage <ResearchBranchEntry> sResearchBranchStore(ResearchBranchfmt);
DBCStorage <ResearchSiteEntry> sResearchSiteStore(ResearchSitefmt);
DBCStorage <ResearchProjectEntry> sResearchProjectStore(ResearchProjectfmt);
std::set<ResearchProjectEntry const*> sResearchProjectSet;
ResearchSiteDataMap sResearchSiteDataMap;

DBCStorage <ScalingStatDistributionEntry> sScalingStatDistributionStore(ScalingStatDistributionfmt);
DBCStorage <ScalingStatValuesEntry> sScalingStatValuesStore(ScalingStatValuesfmt);

DBCStorage <SkillLineEntry> sSkillLineStore(SkillLinefmt);
DBCStorage <SkillLineAbilityEntry> sSkillLineAbilityStore(SkillLineAbilityfmt);
DBCStorage <SkillRaceClassInfoEntry> sSkillRaceClassInfoStore(SkillRaceClassInfofmt);

DBCStorage <SoundEntriesEntry> sSoundEntriesStore(SoundEntriesfmt);

DBCStorage <SpellItemEnchantmentEntry> sSpellItemEnchantmentStore(SpellItemEnchantmentfmt);
DBCStorage <SpellItemEnchantmentConditionEntry> sSpellItemEnchantmentConditionStore(SpellItemEnchantmentConditionfmt);
DBCStorage <SpellEntry> sSpellStore(SpellEntryfmt);
SpellCategoryMap sSpellCategoryMap;
PetFamilySpellsStore sPetFamilySpellsStore;

DBCStorage <SpellAuraOptionsEntry> sSpellAuraOptionsStore(SpellAuraOptionsEntryfmt);
DBCStorage <SpellAuraRestrictionsEntry> sSpellAuraRestrictionsStore(SpellAuraRestrictionsEntryfmt);
DBCStorage <SpellCastingRequirementsEntry> sSpellCastingRequirementsStore(SpellCastingRequirementsEntryfmt);
DBCStorage <SpellCategoryEntry> sSpellCategoryStore(SpellCategoryEntryfmt);
DBCStorage <SpellCategoriesEntry> sSpellCategoriesStore(SpellCategoriesEntryfmt);
DBCStorage <SpellClassOptionsEntry> sSpellClassOptionsStore(SpellClassOptionsEntryfmt);
DBCStorage <SpellCooldownsEntry> sSpellCooldownsStore(SpellCooldownsEntryfmt);
DBCStorage <SpellEffectEntry> sSpellEffectStore(SpellEffectEntryfmt);
DBCStorage <SpellEquippedItemsEntry> sSpellEquippedItemsStore(SpellEquippedItemsEntryfmt);
DBCStorage <SpellInterruptsEntry> sSpellInterruptsStore(SpellInterruptsEntryfmt);
DBCStorage <SpellLevelsEntry> sSpellLevelsStore(SpellLevelsEntryfmt);
DBCStorage <SpellPowerEntry> sSpellPowerStore(SpellPowerEntryfmt);
DBCStorage <SpellReagentsEntry> sSpellReagentsStore(SpellReagentsEntryfmt);
DBCStorage <SpellScalingEntry> sSpellScalingStore(SpellScalingEntryfmt);
DBCStorage <SpellShapeshiftEntry> sSpellShapeshiftStore(SpellShapeshiftEntryfmt);
DBCStorage <SpellTargetRestrictionsEntry> sSpellTargetRestrictionsStore(SpellTargetRestrictionsEntryfmt);
DBCStorage <SpellTotemsEntry> sSpellTotemsStore(SpellTotemsEntryfmt);

SpellEffectMap sSpellEffectMap;

DBCStorage <SpellCastTimesEntry> sSpellCastTimesStore(SpellCastTimefmt);
DBCStorage <SpellDifficultyEntry> sSpellDifficultyStore(SpellDifficultyfmt);
DBCStorage <SpellDurationEntry> sSpellDurationStore(SpellDurationfmt);
DBCStorage <SpellFocusObjectEntry> sSpellFocusObjectStore(SpellFocusObjectfmt);
DBCStorage <SpellRadiusEntry> sSpellRadiusStore(SpellRadiusfmt);
DBCStorage <SpellRangeEntry> sSpellRangeStore(SpellRangefmt);
DBCStorage <SpellRuneCostEntry> sSpellRuneCostStore(SpellRuneCostfmt);
DBCStorage <SpellShapeshiftFormEntry> sSpellShapeshiftFormStore(SpellShapeshiftFormfmt);
//DBCStorage <StableSlotPricesEntry> sStableSlotPricesStore(StableSlotPricesfmt);
DBCStorage <SummonPropertiesEntry> sSummonPropertiesStore(SummonPropertiesfmt);
DBCStorage <TalentEntry> sTalentStore(TalentEntryfmt);
TalentSpellPosMap sTalentSpellPosMap;
DBCStorage <TalentTabEntry> sTalentTabStore(TalentTabEntryfmt);
DBCStorage <TalentTreePrimarySpellsEntry> sTalentTreePrimarySpellsStore(TalentTreePrimarySpellsfmt);
typedef std::map<uint32, std::vector<uint32> > TalentTreeSpellsMap;
TalentTreeSpellsMap sTalentTreeMasterySpellsMap;
TalentTreeSpellsMap sTalentTreePrimarySpellsMap;
typedef std::map<uint32, uint32> TalentTreeRolesMap;
TalentTreeRolesMap sTalentTreeRolesMap;

// store absolute bit position for first rank for talent inspect
static uint32 sTalentTabPages[MAX_CLASSES][3];

DBCStorage <TaxiNodesEntry> sTaxiNodesStore(TaxiNodesEntryfmt);
TaxiMask sTaxiNodesMask;
TaxiMask sOldContinentsNodesMask;
TaxiMask sHordeTaxiNodesMask;
TaxiMask sAllianceTaxiNodesMask;
TaxiMask sDeathKnightTaxiNodesMask;

// DBC used only for initialization sTaxiPathSetBySource at startup.
TaxiPathSetBySource sTaxiPathSetBySource;
DBCStorage <TaxiPathEntry> sTaxiPathStore(TaxiPathEntryfmt);

// DBC store data but sTaxiPathNodesByPath used for fast access to entries (it's not owner pointed data).
TaxiPathNodesByPath sTaxiPathNodesByPath;
static DBCStorage <TaxiPathNodeEntry> sTaxiPathNodeStore(TaxiPathNodeEntryfmt);

DBCStorage <TotemCategoryEntry> sTotemCategoryStore(TotemCategoryEntryfmt);

TransportAnimationsByEntry sTransportAnimationsByEntry;
DBCStorage <TransportAnimationEntry> sTransportAnimationStore(TransportAnimationEntryfmt);
DBCStorage <VehicleEntry> sVehicleStore(VehicleEntryfmt);
DBCStorage <VehicleSeatEntry> sVehicleSeatStore(VehicleSeatEntryfmt);
DBCStorage <WMOAreaTableEntry>  sWMOAreaTableStore(WMOAreaTableEntryfmt);
DBCStorage <WorldMapAreaEntry>  sWorldMapAreaStore(WorldMapAreaEntryfmt);
DBCStorage <WorldMapOverlayEntry> sWorldMapOverlayStore(WorldMapOverlayEntryfmt);
DBCStorage <WorldSafeLocsEntry> sWorldSafeLocsStore(WorldSafeLocsEntryfmt);
DBCStorage <WorldPvPAreaEntry>  sWorldPvPAreaStore(WorldPvPAreaEnrtyfmt);

typedef std::list<std::string> StoreProblemList;

bool IsAcceptableClientBuild(uint32 build)
{
    int accepted_versions[] = EXPECTED_MANGOSD_CLIENT_BUILD;
    for(int i = 0; accepted_versions[i]; ++i)
        if(int(build) == accepted_versions[i])
            return true;

    return false;
}

std::string AcceptableClientBuildsListStr()
{
    std::ostringstream data;
    int accepted_versions[] = EXPECTED_MANGOSD_CLIENT_BUILD;
    for(int i = 0; accepted_versions[i]; ++i)
        data << accepted_versions[i] << " ";
    return data.str();
}

static bool ReadDBCBuildFileText(const std::string& dbc_path, char const* localeName, std::string& text)
{
    std::string filename  = dbc_path + "component.wow-" + localeName + ".txt";

    if(FILE* file = fopen(filename.c_str(),"rb"))
    {
        char buf[100];
        fread(buf,1,100-1,file);
        fclose(file);

        text = &buf[0];
        return true;
    }
    else
        return false;
}

static uint32 ReadDBCBuild(const std::string& dbc_path, LocaleNameStr const*&localeNameStr)
{
    std::string text;

    if (!localeNameStr)
    {
        for(LocaleNameStr const* itr = &fullLocaleNameList[0]; itr->name; ++itr)
        {
            if (ReadDBCBuildFileText(dbc_path,itr->name,text))
            {
                localeNameStr = itr;
                break;
            }
        }
    }
    else
        ReadDBCBuildFileText(dbc_path,localeNameStr->name,text);

    if (text.empty())
        return 0;

    size_t pos = text.find("version=\"");
    size_t pos1 = pos + strlen("version=\"");
    size_t pos2 = text.find("\"",pos1);
    if (pos == text.npos || pos2 == text.npos || pos1 >= pos2)
        return 0;

    std::string build_str = text.substr(pos1,pos2-pos1);

    int build = atoi(build_str.c_str());
    if (build <= 0)
        return 0;

    return build;
}

static bool LoadDBC_assert_print(uint32 fsize,uint32 rsize, const std::string& filename)
{
    sLog.outError("Size of '%s' setted by format string (%u) not equal size of C++ structure (%u).",filename.c_str(),fsize,rsize);

    // ASSERT must fail after function call
    return false;
}

struct LocalData
{
    LocalData(uint32 build, LocaleConstant loc)
        : main_build(build), defaultLocale(loc), availableDbcLocales(0xFFFFFFFF),checkedDbcLocaleBuilds(0) {}

    uint32 main_build;
    LocaleConstant defaultLocale;

    // bitmasks for index of fullLocaleNameList
    uint32 availableDbcLocales;
    uint32 checkedDbcLocaleBuilds;
};

template<class T>
inline void LoadDBC(LocalData& localeData, BarGoLink& bar, StoreProblemList& errlist, DBCStorage<T>& storage, const std::string& dbc_path, const std::string& filename, const std::string * custom_entries = NULL, const std::string * idname = NULL)
{
    // compatibility format and C++ structure sizes
    MANGOS_ASSERT(DBCFileLoader::GetFormatRecordSize(storage.GetFormat()) == sizeof(T) || LoadDBC_assert_print(DBCFileLoader::GetFormatRecordSize(storage.GetFormat()),sizeof(T),filename));

    std::string dbc_filename = dbc_path + filename;

    SqlDbc * sql = NULL;

    if (custom_entries)
        sql = new SqlDbc(&filename,custom_entries,idname,storage.GetFormat());

    if(storage.Load(dbc_filename.c_str(),localeData.defaultLocale,sql))
    {
        bar.step();
        for(uint8 i = 0; fullLocaleNameList[i].name; ++i)
        {
            if (!(localeData.availableDbcLocales & (1 << i)))
                continue;

            LocaleNameStr const* localStr = &fullLocaleNameList[i];

            std::string dbc_dir_loc = dbc_path + localStr->name + "/";

            if (!(localeData.checkedDbcLocaleBuilds & (1 << i)))
            {
                localeData.checkedDbcLocaleBuilds |= (1<<i);// mark as checked for speedup next checks

                uint32 build_loc = ReadDBCBuild(dbc_dir_loc,localStr);
                if(localeData.main_build != build_loc)
                {
                    localeData.availableDbcLocales &= ~(1<<i);  // mark as not available for speedup next checks

                    // exist but wrong build
                    if (build_loc)
                    {
                        std::string dbc_filename_loc = dbc_path + localStr->name + "/" + filename;
                        char buf[200];
                        snprintf(buf,200," (exist, but DBC locale subdir %s have DBCs for build %u instead expected build %u, it and other DBC from subdir skipped)",localStr->name,build_loc,localeData.main_build);
                        errlist.push_back(dbc_filename_loc + buf);
                    }

                    continue;
                }
            }

            std::string dbc_filename_loc = dbc_path + localStr->name + "/" + filename;
            if(!storage.LoadStringsFrom(dbc_filename_loc.c_str(),localStr->locale))
                localeData.availableDbcLocales &= ~(1<<i);  // mark as not available for speedup next checks
        }
    }
    else
    {
        // sort problematic dbc to (1) non compatible and (2) nonexistent
        FILE * f=fopen(dbc_filename.c_str(),"rb");
        if(f)
        {
            char buf[100];
            snprintf(buf, 100, " (exist, but have %u fields instead " SIZEFMTD ") Wrong client version DBC file?", storage.GetFieldCount(), strlen(storage.GetFormat()));
            errlist.push_back(dbc_filename + buf);
            fclose(f);
        }
        else
            errlist.push_back(dbc_filename);
    }

    if (sql)
        delete sql;
}

void LoadDBCStores(const std::string& dataPath)
{
    std::string dbcPath = dataPath+"dbc/";

    LocaleNameStr const* defaultLocaleNameStr = NULL;
    uint32 build = ReadDBCBuild(dbcPath,defaultLocaleNameStr);

    // Check the expected DBC version
    if (!IsAcceptableClientBuild(build))
    {
        if (build)
            sLog.outError("Found DBC files for build %u but mangosd expected DBC for one from builds: %s Please extract correct DBC files.", build, AcceptableClientBuildsListStr().c_str());
        else
            sLog.outError("Incorrect DataDir value in mangosd.conf or not found build info (outdated DBC files). Required one from builds: %s Please extract correct DBC files.",AcceptableClientBuildsListStr().c_str());
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    const uint32 DBCFilesCount = 130;

    BarGoLink bar(DBCFilesCount);

    StoreProblemList bad_dbc_files;

    LocalData availableDbcLocales(build,defaultLocaleNameStr->locale);

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sAreaStore,                dbcPath,"AreaTable.dbc");

    // must be after sAreaStore loading
    for (uint32 i = 0; i < sAreaStore.GetNumRows(); ++i)    // areaflag numbered from 0
    {
        if(AreaTableEntry* area = (AreaTableEntry*)sAreaStore.LookupEntry(i))
        {
            //switch (area->ID)
            //{
            //    //case 720:       // Fray Island
            //    //    area->flags = AREA_FLAG_ARENA | AREA_FLAG_OUTLAND | AREA_FLAG_OUTLAND2;
            //    //    break;
            //    //case 3320:      // Warsong Lumber Mill
            //    //case 3321:      // Silverwing Hold
            //    //case 4571:      // Silverwing Flag Room
            //    //case 4572:      // Warsong Flag Room
            //    //    area->flags &= ~AREA_FLAG_INSIDE;
            //    //    area->flags |= AREA_FLAG_OUTSIDE;
            //    //    break;
            //    default:
            //        break;
            //}

            // fill AreaId->DBC records
            sAreaFlagByAreaID.insert(AreaFlagByAreaID::value_type(uint16(area->ID),area->exploreFlag));

            // fill MapId->DBC records ( skip sub zones and continents )
            if(area->zone==0 && area->mapid != 0 && area->mapid != 1 && area->mapid != 530 && area->mapid != 571 )
                sAreaFlagByMapID.insert(AreaFlagByMapID::value_type(area->mapid,area->exploreFlag));
        }
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sAchievementStore,         dbcPath,"Achievement.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sAchievementCriteriaStore, dbcPath,"Achievement_Criteria.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sAreaTriggerStore,         dbcPath,"AreaTrigger.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sAreaGroupStore,           dbcPath,"AreaGroup.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sArmorLocationStore,       dbcPath,"ArmorLocation.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sAuctionHouseStore,        dbcPath,"AuctionHouse.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sBankBagSlotPricesStore,   dbcPath,"BankBagSlotPrices.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sBattlemasterListStore,    dbcPath,"BattlemasterList.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sBarberShopStyleStore,     dbcPath,"BarberShopStyle.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCharStartOutfitStore,     dbcPath,"CharStartOutfit.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCharTitlesStore,          dbcPath,"CharTitles.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sChatChannelsStore,        dbcPath,"ChatChannels.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sChrClassesStore,          dbcPath,"ChrClasses.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sChrPowerTypesStore,       dbcPath,"ChrClassesXPowerTypes.dbc");
    for (uint32 i = 0; i < MAX_CLASSES; ++i)
    {
        for (uint32 j = 0; j < MAX_POWERS; ++j)
            sChrClassXPowerTypesStore[i][j] = INVALID_POWER_INDEX;
        for (uint32 j = 0; j < MAX_STORED_POWERS; ++j)
            sChrClassXPowerIndexStore[i][j] = INVALID_POWER;
    }
    for (uint32 i = 0; i < sChrPowerTypesStore.GetNumRows(); ++i)
    {
        ChrPowerTypesEntry const* entry = sChrPowerTypesStore.LookupEntry(i);
        if (!entry)
            continue;

        MANGOS_ASSERT(entry->classId < MAX_CLASSES && "MAX_CLASSES not updated");
        MANGOS_ASSERT(entry->power < MAX_POWERS && "MAX_POWERS not updated");

        uint32 index = 0;

        for (uint32 j = 0; j < MAX_POWERS; ++j)
        {
            if (sChrClassXPowerTypesStore[entry->classId][j] != INVALID_POWER_INDEX)
                ++index;
        }

        MANGOS_ASSERT(index < MAX_STORED_POWERS && "MAX_STORED_POWERS not updated");

        sChrClassXPowerTypesStore[entry->classId][entry->power] = index;
        sChrClassXPowerIndexStore[entry->classId][index] = entry->power;
    }
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sChrRacesStore,            dbcPath,"ChrRaces.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCinematicSequencesStore,  dbcPath,"CinematicSequences.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCreatureDisplayInfoStore, dbcPath,"CreatureDisplayInfo.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCreatureDisplayInfoExtraStore,dbcPath,"CreatureDisplayInfoExtra.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCreatureFamilyStore,      dbcPath,"CreatureFamily.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCreatureModelDataStore,   dbcPath,"CreatureModelData.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sDestructibleModelDataStore,dbcPath,"DestructibleModelData.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCreatureSpellDataStore,   dbcPath,"CreatureSpellData.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCreatureTypeStore,        dbcPath,"CreatureType.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sCurrencyTypesStore,       dbcPath,"CurrencyTypes.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sDungeonEncounterStore,    dbcPath,"DungeonEncounter.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sDurabilityCostsStore,     dbcPath,"DurabilityCosts.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sDurabilityQualityStore,   dbcPath,"DurabilityQuality.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sEmotesStore,              dbcPath,"Emotes.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sEmotesTextStore,          dbcPath,"EmotesText.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sFactionStore,             dbcPath,"Faction.dbc");
    for (uint32 i=0;i<sFactionStore.GetNumRows(); ++i)
    {
        FactionEntry const * faction = sFactionStore.LookupEntry(i);
        if (faction && faction->team)
        {
            SimpleFactionsList &flist = sFactionTeamMap[faction->team];
            flist.push_back(i);
        }
    }

    // Hero of the Shattrath
    // Aldor Part
    AchievementCriteriaEntry* achentry = (AchievementCriteriaEntry*)sAchievementCriteriaStore.LookupEntry(13427);
    if (achentry)
    {
        achentry->requiredType = ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION;
        achentry->gain_reputation.factionID = 932;
        achentry->gain_reputation.reputationAmount = 42000;
    }
    // Scryers Part
    achentry = (AchievementCriteriaEntry*)sAchievementCriteriaStore.LookupEntry(13428);
    if (achentry)
    {
        achentry->requiredType = ACHIEVEMENT_CRITERIA_TYPE_GAIN_REPUTATION;
        achentry->gain_reputation.factionID = 934;
        achentry->gain_reputation.reputationAmount = 42000;
    }

    // The Turkinator
    achentry = (AchievementCriteriaEntry*)sAchievementCriteriaStore.LookupEntry(11128);
    if (achentry)
    {
        achentry->requiredType = ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE;
        achentry->kill_creature.creatureID = 32820;
        achentry->kill_creature.creatureCount = 40;
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sFactionTemplateStore,     dbcPath,"FactionTemplate.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGameObjectDisplayInfoStore,dbcPath,"GameObjectDisplayInfo.dbc");
    for (uint32 i = 0; i < sGameObjectDisplayInfoStore.GetNumRows(); ++i)
    {
        if (GameObjectDisplayInfoEntry *info = const_cast<GameObjectDisplayInfoEntry*>(sGameObjectDisplayInfoStore.LookupEntry(i)))
        {
            if (info->geoBoxMaxX < info->geoBoxMinX)
                std::swap(info->geoBoxMaxX, info->geoBoxMinX);
            if (info->geoBoxMaxY < info->geoBoxMinY)
                std::swap(info->geoBoxMaxY, info->geoBoxMinY);
            if (info->geoBoxMaxZ < info->geoBoxMinZ)
                std::swap(info->geoBoxMaxZ, info->geoBoxMinZ);
        }
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGemPropertiesStore,       dbcPath,"GemProperties.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGlyphPropertiesStore,     dbcPath,"GlyphProperties.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGlyphSlotStore,           dbcPath,"GlyphSlot.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtBarberShopCostBaseStore,dbcPath,"gtBarberShopCostBase.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtCombatRatingsStore,     dbcPath,"gtCombatRatings.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtChanceToMeleeCritBaseStore, dbcPath,"gtChanceToMeleeCritBase.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtChanceToMeleeCritStore, dbcPath,"gtChanceToMeleeCrit.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtChanceToSpellCritBaseStore, dbcPath,"gtChanceToSpellCritBase.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtChanceToSpellCritStore, dbcPath,"gtChanceToSpellCrit.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtOCTClassCombatRatingScalarStore,dbcPath,"gtOCTClassCombatRatingScalar.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtOCTHpPerStaminaStore,   dbcPath,"gtOCTHpPerStamina.dbc");
    //LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtOCTRegenMPStore,        dbcPath,"gtOCTRegenMP.dbc");       -- not used currently
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtRegenMPPerSptStore,     dbcPath,"gtRegenMPPerSpt.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtSpellScalingStore,      dbcPath,"gtSpellScaling.dbc");     // 15595
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtOCTBaseHPByClassStore,  dbcPath,"gtOCTBaseHPByClass.dbc"); // 15595
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGtOCTBaseMPByClassStore,  dbcPath,"gtOCTBaseMPByClass.dbc"); // 15595
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sGuildPerkSpellsStore,     dbcPath,"GuildPerkSpells.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sHolidaysStore,            dbcPath,"Holidays.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sImportPriceArmorStore,    dbcPath,"ImportPriceArmor.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sImportPriceQualityStore,  dbcPath,"ImportPriceQuality.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sImportPriceShieldStore,   dbcPath,"ImportPriceShield.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sImportPriceWeaponStore,   dbcPath,"ImportPriceWeapon.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemPriceBaseStore,       dbcPath,"ItemPriceBase.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemArmorQualityStore,    dbcPath,"ItemArmorQuality.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemArmorShieldStore,     dbcPath,"ItemArmorShield.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemArmorTotalStore,      dbcPath,"ItemArmorTotal.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemBagFamilyStore,       dbcPath,"ItemBagFamily.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemReforgeStore,         dbcPath, "ItemReforge.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sLFGDungeonStore,          dbcPath,"LFGDungeons.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemClassStore,           dbcPath,"ItemClass.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageAmmoStore,      dbcPath,"ItemDamageAmmo.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageOneHandStore,   dbcPath,"ItemDamageOneHand.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageOneHandCasterStore,dbcPath,"ItemDamageOneHandCaster.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageRangedStore,    dbcPath,"ItemDamageRanged.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageThrownStore,    dbcPath,"ItemDamageThrown.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageTwoHandStore,   dbcPath,"ItemDamageTwoHand.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageTwoHandCasterStore,dbcPath,"ItemDamageTwoHandCaster.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDamageWandStore,      dbcPath,"ItemDamageWand.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDisenchantLootStore,  dbcPath,"ItemDisenchantLoot.dbc");
    //LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemDisplayInfoStore,     dbcPath,"ItemDisplayInfo.dbc");     -- not used currently
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemLimitCategoryStore,   dbcPath,"ItemLimitCategory.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemRandomPropertiesStore,dbcPath,"ItemRandomProperties.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemRandomSuffixStore,    dbcPath,"ItemRandomSuffix.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sItemSetStore,             dbcPath,"ItemSet.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sLiquidTypeStore,          dbcPath,"LiquidType.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sLockStore,                dbcPath,"Lock.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sMailTemplateStore,        dbcPath,"MailTemplate.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sMapStore,                 dbcPath,"Map.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sMapDifficultyStore,       dbcPath,"MapDifficulty.dbc");
    // fill data
    for(uint32 i = 1; i < sMapDifficultyStore.GetNumRows(); ++i)
    {
        if(MapDifficultyEntry const* entry = sMapDifficultyStore.LookupEntry(i))
            sMapDifficultyMap[MAKE_PAIR32(entry->MapId,entry->Difficulty)] = entry;
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sMountCapabilityStore,     dbcPath,"MountCapability.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sMountTypeStore,           dbcPath,"MountType.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sMovieStore,               dbcPath,"Movie.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sNumTalentsAtLevelStore,   dbcPath,"NumTalentsAtLevel.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sOverrideSpellDataStore,   dbcPath,"OverrideSpellData.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sQuestFactionRewardStore,  dbcPath,"QuestFactionReward.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sQuestPOIPointStore,       dbcPath,"QuestPOIPoint.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sQuestSortStore,           dbcPath,"QuestSort.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sQuestXPLevelStore,        dbcPath,"QuestXP.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sPhaseStore,               dbcPath,"Phase.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sPvPDifficultyStore,       dbcPath,"PvpDifficulty.dbc");
    for(uint32 i = 0; i < sPvPDifficultyStore.GetNumRows(); ++i)
        if (PvPDifficultyEntry const* entry = sPvPDifficultyStore.LookupEntry(i))
            if (entry->bracketId > MAX_BATTLEGROUND_BRACKETS)
                MANGOS_ASSERT(false && "Need update MAX_BATTLEGROUND_BRACKETS by DBC data");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sRandomPropertiesPointsStore, dbcPath,"RandPropPoints.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sResearchBranchStore,      dbcPath,"ResearchBranch.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sResearchProjectStore,     dbcPath,"ResearchProject.dbc");
    for (uint32 i = 0; i < sResearchProjectStore.GetNumRows(); ++i)
    {
        ResearchProjectEntry const* rp = sResearchProjectStore.LookupEntry(i);
        if (!rp || !rp->IsVaid())
            continue;

        sResearchProjectSet.insert(rp);
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sResearchSiteStore,        dbcPath,"ResearchSite.dbc");
    for (uint32 i = 0; i < sResearchSiteStore.GetNumRows(); ++i)
    {
        ResearchSiteEntry const* rs = sResearchSiteStore.LookupEntry(i);
        if (!rs || !rs->IsValid())
            continue;

        ResearchSiteData& data = sResearchSiteDataMap[rs->ID];

        data.entry = rs;

        for (uint32 i = 0; i < sQuestPOIPointStore.GetNumRows(); ++i)
            if (QuestPOIPointEntry const* poi = sQuestPOIPointStore.LookupEntry(i))
                if (poi->POIId == rs->POIid)
                    data.points.push_back(ResearchPOIPoint(poi->x, poi->y));

        if (data.points.size() == 0)
            sLog.outDebug("Research site %u POI %u map %u has 0 POI points in DBC!", rs->ID, rs->POIid, rs->mapId);
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sScalingStatDistributionStore, dbcPath,"ScalingStatDistribution.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sScalingStatValuesStore,   dbcPath,"ScalingStatValues.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSkillLineStore,           dbcPath,"SkillLine.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSkillLineAbilityStore,    dbcPath,"SkillLineAbility.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSkillRaceClassInfoStore,  dbcPath,"SkillRaceClassInfo.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSoundEntriesStore,        dbcPath,"SoundEntries.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellCategoryStore,       dbcPath,"SpellCategory.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellCategoriesStore,     dbcPath,"SpellCategories.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellStore,               dbcPath,"Spell.dbc", &CustomSpellEntryfmt, &CustomSpellEntryIndex);
    for(uint32 i = 1; i < sSpellStore.GetNumRows(); ++i)
    {
        SpellEntry* spell = (SpellEntry*)sSpellStore.LookupEntry(i);
        if (!spell)
            continue;
        if (SpellCategoriesEntry const* category = spell->GetSpellCategories())
            if(uint32 cat = category->Category)
                sSpellCategoryMap[cat].insert(i);

        // DBC not support uint64 fields but SpellEntry have SpellFamilyFlags mapped at 2 uint32 fields
        // uint32 field already converted to bigendian if need, but must be swapped for correct uint64 bigendian view
        #if MANGOS_ENDIAN == MANGOS_BIGENDIAN
        std::swap(*((uint32*)(&spell->SpellFamilyFlags)),*(((uint32*)(&spell->SpellFamilyFlags))+1));
        #endif

    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellAuraOptionsStore,    dbcPath,"SpellAuraOptions.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellAuraRestrictionsStore, dbcPath,"SpellAuraRestrictions.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellCastingRequirementsStore, dbcPath,"SpellCastingRequirements.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellClassOptionsStore,   dbcPath,"SpellClassOptions.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellCooldownsStore,      dbcPath,"SpellCooldowns.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellEffectStore,         dbcPath,"SpellEffect.dbc");

    for(uint32 i = 1; i < sSpellEffectStore.GetNumRows(); ++i)
    {
        if (SpellEffectEntry const *spellEffect = sSpellEffectStore.LookupEntry(i))
        {
            switch (spellEffect->EffectApplyAuraName)
            {
                case SPELL_AURA_MOD_INCREASE_ENERGY:
                case SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT:
                case SPELL_AURA_PERIODIC_MANA_LEECH:
                case SPELL_AURA_PERIODIC_ENERGIZE:
                case SPELL_AURA_POWER_BURN_ENERGY:
                    MANGOS_ASSERT(spellEffect->EffectMiscValue >= 0 && spellEffect->EffectMiscValue < MAX_POWERS);
                    break;
            }

            sSpellEffectMap[spellEffect->EffectSpellId].effects[spellEffect->EffectIndex] = spellEffect;
        }
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellEquippedItemsStore,  dbcPath,"SpellEquippedItems.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellInterruptsStore,     dbcPath,"SpellInterrupts.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellLevelsStore,         dbcPath,"SpellLevels.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellPowerStore,          dbcPath,"SpellPower.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellReagentsStore,       dbcPath,"SpellReagents.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellScalingStore,        dbcPath,"SpellScaling.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellShapeshiftStore,     dbcPath,"SpellShapeshift.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellTargetRestrictionsStore, dbcPath,"SpellTargetRestrictions.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellTotemsStore,         dbcPath,"SpellTotems.dbc");

    for (uint32 j = 0; j < sSkillLineAbilityStore.GetNumRows(); ++j)
    {
        SkillLineAbilityEntry const *skillLine = sSkillLineAbilityStore.LookupEntry(j);

        if(!skillLine)
            continue;

        SpellEntry const* spellInfo = sSpellStore.LookupEntry(skillLine->spellId);

        if (spellInfo && (spellInfo->Attributes & (SPELL_ATTR_UNK4 | SPELL_ATTR_PASSIVE | SPELL_ATTR_UNK7 | SPELL_ATTR_UNK8)) == (SPELL_ATTR_UNK4 | SPELL_ATTR_PASSIVE | SPELL_ATTR_UNK7 | SPELL_ATTR_UNK8))
        {
            for (unsigned int i = 1; i < sCreatureFamilyStore.GetNumRows(); ++i)
            {
                CreatureFamilyEntry const* cFamily = sCreatureFamilyStore.LookupEntry(i);
                if(!cFamily)
                    continue;

                if(skillLine->skillId != cFamily->skillLine[0] && skillLine->skillId != cFamily->skillLine[1])
                    continue;

                sPetFamilySpellsStore[i].insert(spellInfo->Id);
            }
        }
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellCastTimesStore,      dbcPath,"SpellCastTimes.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellDurationStore,       dbcPath,"SpellDuration.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellDifficultyStore,     dbcPath,"SpellDifficulty.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellFocusObjectStore,    dbcPath,"SpellFocusObject.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellItemEnchantmentStore,dbcPath,"SpellItemEnchantment.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellItemEnchantmentConditionStore,dbcPath,"SpellItemEnchantmentCondition.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellRadiusStore,         dbcPath,"SpellRadius.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellRangeStore,          dbcPath,"SpellRange.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellRuneCostStore,       dbcPath,"SpellRuneCost.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSpellShapeshiftFormStore, dbcPath,"SpellShapeshiftForm.dbc");
    //LoadDBC(availableDbcLocales,bar,bad_dbc_files,sStableSlotPricesStore,    dbcPath,"StableSlotPrices.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sSummonPropertiesStore,    dbcPath,"SummonProperties.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sTalentStore,              dbcPath,"Talent.dbc");

    // create talent spells set
    for (unsigned int i = 0; i < sTalentStore.GetNumRows(); ++i)
    {
        TalentEntry const *talentInfo = sTalentStore.LookupEntry(i);
        if (!talentInfo) continue;
        for (int j = 0; j < MAX_TALENT_RANK; j++)
            if(talentInfo->RankID[j])
                sTalentSpellPosMap[talentInfo->RankID[j]] = TalentSpellPos(i,j);
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sTalentTabStore,           dbcPath,"TalentTab.dbc");

    // prepare fast data access to bit pos of talent ranks for use at inspecting
    {
        // now have all max ranks (and then bit amount used for store talent ranks in inspect)
        for(uint32 talentTabId = 1; talentTabId < sTalentTabStore.GetNumRows(); ++talentTabId)
        {
            TalentTabEntry const *talentTabInfo = sTalentTabStore.LookupEntry( talentTabId );
            if(!talentTabInfo)
                continue;

            for (uint32 i = 0; i < MAX_MASTERY_SPELLS; ++i)
                if (uint32 spellid = talentTabInfo->masterySpells[i])
                    if (sSpellStore.LookupEntry(spellid))
                        sTalentTreeMasterySpellsMap[talentTabId].push_back(spellid);

            // prevent memory corruption; otherwise cls will become 12 below
            if ((talentTabInfo->ClassMask & CLASSMASK_ALL_PLAYABLE)==0)
                continue;

            // store class talent tab pages
            for (uint32 cls = 1; cls < MAX_CLASSES; ++cls)
                if (talentTabInfo->ClassMask & (1 << (cls - 1)))
                    sTalentTabPages[cls][talentTabInfo->tabpage] = talentTabId;

            sTalentTreeRolesMap[talentTabId] = talentTabInfo->rolesMask;
        }
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files, sTalentTreePrimarySpellsStore, dbcPath, "TalentTreePrimarySpells.dbc");
    for (uint32 i = 0; i < sTalentTreePrimarySpellsStore.GetNumRows(); ++i)
        if (TalentTreePrimarySpellsEntry const* talentSpell = sTalentTreePrimarySpellsStore.LookupEntry(i))
            if (sSpellStore.LookupEntry(talentSpell->SpellId))
                sTalentTreePrimarySpellsMap[talentSpell->TalentTree].push_back(talentSpell->SpellId);
    sTalentTreePrimarySpellsStore.Clear();

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sTaxiNodesStore,           dbcPath,"TaxiNodes.dbc");

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sTaxiPathStore,            dbcPath,"TaxiPath.dbc");
    for(uint32 i = 1; i < sTaxiPathStore.GetNumRows(); ++i)
        if(TaxiPathEntry const* entry = sTaxiPathStore.LookupEntry(i))
            sTaxiPathSetBySource[entry->from][entry->to] = TaxiPathBySourceAndDestination(entry->ID,entry->price);
    uint32 pathCount = sTaxiPathStore.GetNumRows();

    //## TaxiPathNode.dbc ## Loaded only for initialization different structures
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sTaxiPathNodeStore,        dbcPath,"TaxiPathNode.dbc");
    // Calculate path nodes count
    std::vector<uint32> pathLength;
    pathLength.resize(pathCount);                           // 0 and some other indexes not used
    for(uint32 i = 1; i < sTaxiPathNodeStore.GetNumRows(); ++i)
        if(TaxiPathNodeEntry const* entry = sTaxiPathNodeStore.LookupEntry(i))
        {
            if (pathLength[entry->path] < entry->index + 1)
                pathLength[entry->path] = entry->index + 1;
        }
    // Set path length
    sTaxiPathNodesByPath.resize(pathCount);                 // 0 and some other indexes not used
    for(uint32 i = 1; i < sTaxiPathNodesByPath.size(); ++i)
        sTaxiPathNodesByPath[i].resize(pathLength[i]);
    // fill data (pointers to sTaxiPathNodeStore elements
    for(uint32 i = 1; i < sTaxiPathNodeStore.GetNumRows(); ++i)
        if(TaxiPathNodeEntry const* entry = sTaxiPathNodeStore.LookupEntry(i))
            sTaxiPathNodesByPath[entry->path].set(entry->index, entry);

    // Initialize global taxinodes mask
    // include existing nodes that have at least single not spell base (scripted) path
    {
        std::set<uint32> spellPaths;
        for(uint32 i = 1; i < sSpellStore.GetNumRows (); ++i)
            if(SpellEntry const* sInfo = sSpellStore.LookupEntry (i))
                for(int j=0; j < MAX_EFFECT_INDEX; ++j)
                    if(SpellEffectEntry const* effect = sInfo->GetSpellEffect(SpellEffectIndex(j)))
                        if(effect->Effect==123 /*SPELL_EFFECT_SEND_TAXI*/)
                            spellPaths.insert(effect->EffectMiscValue);

        memset(sTaxiNodesMask,0,sizeof(sTaxiNodesMask));
        memset(sOldContinentsNodesMask,0,sizeof(sTaxiNodesMask));
        memset(sHordeTaxiNodesMask, 0, sizeof(sHordeTaxiNodesMask));
        memset(sAllianceTaxiNodesMask, 0, sizeof(sAllianceTaxiNodesMask));
        memset(sDeathKnightTaxiNodesMask, 0, sizeof(sDeathKnightTaxiNodesMask));
        for(uint32 i = 1; i < sTaxiNodesStore.GetNumRows(); ++i)
        {
            TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(i);
            if(!node)
                continue;

            TaxiPathSetBySource::const_iterator src_i = sTaxiPathSetBySource.find(i);
            if(src_i!=sTaxiPathSetBySource.end() && !src_i->second.empty())
            {
                bool ok = false;
                for(TaxiPathSetForSource::const_iterator dest_i = src_i->second.begin();dest_i != src_i->second.end(); ++dest_i)
                {
                    // not spell path
                    if(spellPaths.find(dest_i->second.ID)==spellPaths.end())
                    {
                        ok = true;
                        break;
                    }
                }

                if(!ok)
                    continue;
            }

            // valid taxi network node
            uint8  field   = (uint8)((i - 1) / 8);
            uint32 submask = 1 << ((i-1) % 8);
            sTaxiNodesMask[field] |= submask;

            if (node->MountCreatureID[0] && node->MountCreatureID[0] != 32981)
                sHordeTaxiNodesMask[field] |= submask;
            if (node->MountCreatureID[1] && node->MountCreatureID[1] != 32981)
                sAllianceTaxiNodesMask[field] |= submask;
            if (node->MountCreatureID[0] == 32981 || node->MountCreatureID[1] == 32981)
                sDeathKnightTaxiNodesMask[field] |= submask;

            // old continent node (+ nodes virtually at old continents, check explicitly to avoid loading map files for zone info)
            if (node->map_id < 2 || i == 82 || i == 83 || i == 93 || i == 94)
                sOldContinentsNodesMask[field] |= submask;

            // fix DK node at Ebon Hold
            if (i == 315)
                ((TaxiNodesEntry*)node)->MountCreatureID[1] = 32981;
        }
    }

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sTotemCategoryStore,       dbcPath,"TotemCategory.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sTransportAnimationStore,  dbcPath,"TransportAnimation.dbc");
    for (uint32 i = 0; i < sTransportAnimationStore.GetNumRows(); ++i)
        if (TransportAnimationEntry const* entry = sTransportAnimationStore.LookupEntry(i))
            sTransportAnimationsByEntry[entry->transportEntry][entry->timeFrame] = entry;

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sVehicleStore,             dbcPath,"Vehicle.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sVehicleSeatStore,         dbcPath,"VehicleSeat.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sWorldMapAreaStore,        dbcPath,"WorldMapArea.dbc");
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sWMOAreaTableStore,        dbcPath,"WMOAreaTable.dbc");
    for(uint32 i = 0; i < sWMOAreaTableStore.GetNumRows(); ++i)
    {
        if(WMOAreaTableEntry const* entry = sWMOAreaTableStore.LookupEntry(i))
        {
            sWMOAreaInfoByTripple.insert(WMOAreaInfoByTripple::value_type(WMOAreaTableTripple(entry->rootId, entry->adtId, entry->groupId), entry));
        }
    }
    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sWorldMapOverlayStore,     dbcPath,"WorldMapOverlay.dbc");
    // Undercity -> Ruins of Lordaeron
    if (WorldMapOverlayEntry* entry = (WorldMapOverlayEntry*)(uint32*)sWorldMapOverlayStore.LookupEntry(228))
        entry->areatableID[1] = 153;

    LoadDBC(availableDbcLocales,bar,bad_dbc_files,sWorldSafeLocsStore,       dbcPath,"WorldSafeLocs.dbc");

    // error checks
    if (bad_dbc_files.size() >= DBCFilesCount )
    {
        sLog.outError("\nIncorrect DataDir value in mangosd.conf or ALL required *.dbc files (%d) not found by path: %sdbc",DBCFilesCount,dataPath.c_str());
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }
    else if (!bad_dbc_files.empty() )
    {
        std::string str;
        for(std::list<std::string>::iterator i = bad_dbc_files.begin(); i != bad_dbc_files.end(); ++i)
            str += *i + "\n";

        sLog.outError("\nSome required *.dbc files (%u from %d) not found or not compatible:\n%s",(uint32)bad_dbc_files.size(),DBCFilesCount,str.c_str());
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    for (uint32 i = 0; i < sSpellStore.GetNumRows(); ++i)
    {
        SpellEntry* spell = const_cast<SpellEntry*>(sSpellStore.LookupEntry(i));
        if (!spell)
            continue;

        switch (spell->Id)
        {
            case 122:                           // Frost Nova
            case 8122:                          // Psychic Scream
            {
                if (SpellInterruptsEntry* inter = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                    inter->AuraInterruptFlags |= 0x08000000; // AURA_INTERRUPT_FLAG_DAMAGE2
                break;
            }
            case 1776:                          // Gouge
            {
                spell->Attributes &= ~SPELL_ATTR_UNK11;
                break;
            }
            case 2643:                          // Multi-Shot
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->Effect = TARGET_CHAIN_DAMAGE;
                break;
            }
            case 2654:                          // Summon Tamed (TEST) -> Premium NPC Summoner
            {
                spell->Attributes = 0x10000110;
                spell->CastingTimeIndex = 0;
                spell->DurationIndex = 21;
                spell->rangeIndex = 1;

                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                {
                    eff->Effect = 28;
                    eff->EffectDieSides = 1;
                    eff->EffectImplicitTargetA = 32;
                    eff->EffectRadiusIndex = 15;
                    eff->EffectMultipleValue = 1000;
                    eff->EffectMiscValue = 88004;
                    eff->EffectMiscValueB = 41;
                }

                spell->SpellVisual[0] = 6744;
                spell->SpellIconID = 597;

                spell->SpellInterruptsId = 14290;
                //intr->AuraInterruptFlags = 0x1F;
                break;
            }
            case 5420:                          // Tree of Life (Passive)
            {
                spell->SpellShapeshiftId = 700;
                break;
            }
            case 6358:                          // Seduction
            {
                spell->SchoolMask = SPELL_SCHOOL_MASK_MAGIC;
                break;
            }
            case 8145:                          // Tremor Totem Passive
            {
                spell->AttributesEx5 |= SPELL_ATTR_EX5_START_PERIODIC_AT_APPLY;
                break;
            }
            case 11327:                         // Vanish
            {
                if (SpellInterruptsEntry* intr = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                    intr->InterruptFlags &= ~0x8000000; //AURA_INTERRUPT_FLAG_DAMAGE2
                break;
            }
            case 12051:                         // Evocation
            {
                if (SpellInterruptsEntry* intr = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                    intr->InterruptFlags = 0x15;
                break;
            }
            case 12880:                         // Enrage (Rank 1)
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectBasePoints = 2;
                break;
            }
            case 14201:                         // Enrage (Rank 2)
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectBasePoints = 4;
                break;
            }
            case 14202:                         // Enrage (Rank 3)
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectBasePoints = 6;
                break;
            }
            case 15290:                         // Improved Vampiric Embrace heal part
            {
                spell->SpellClassOptionsId = 859;
                //spell->SpellFamilyFlags.Flags |= UI64LIT(0x0000000000000004);
                break;
            }
            case 18754:                         // Improved Succubus
            case 18755:
            case 18756:
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectImplicitTargetA = TARGET_SELF;
                break;
            }
            case 20224:                         // Seals of the Pure
            case 20225:
            case 20330:
            case 20331:
            case 20332:
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectSpellClassMask.Flags |= UI64LIT(0x2000000000000000); // Seal of Righteoussness proc
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_1))
                    eff->EffectSpellClassMask.Flags |= UI64LIT(0x2000000000000000); // Seal of Righteoussness proc
                break;
            }
            case 24706:                         // Stinking Up Southshore Hack
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_2))
                {
                    eff->Effect = SPELL_EFFECT_QUEST_COMPLETE;
                    eff->EffectMiscValue = 1657;
                }
                break;
            }
            case 25771:                         // Ranger: Forbearance mechanic immune
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectMiscValue = 29;
                break;
            }
            case 27792:                         // megai2: set interrupt and castingtimeindex to 0 for spirit of redemption
            case 27827:
            {
                if (SpellInterruptsEntry* intr = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                    intr->AuraInterruptFlags = 0;
                spell->CastingTimeIndex = 0;
                break;
            }
            case 27795:                         // interupt only for spirit of redemption
            {
                // no interrupts as of 4.3.4+
                //spell->AuraInterruptFlags = 0;
                break;
            }
            case 30526:                         // Flame Turret
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->Effect = 0;
                break;
            }
            case 31117:                         // Unstable Affliction dispel damage
            {
                spell->AttributesEx6 &= ~SPELL_ATTR_EX6_NO_DMG_MODS;
                break;
            }
            case 32592:                         // Mass Dispel
            {
                spell->Attributes |= SPELL_ATTR_UNAFFECTED_BY_INVULNERABILITY;
                break;
            }
            case 36032:                         // Arcance Blast Debuff
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectSpellClassMask.Flags |= UI64LIT(0x800000200000);
                break;
            }
            case 37212:                         // Flametongue Weapon (Passive)
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectSpellClassMask.Flags = UI64LIT(0x0000000000200000);
                break;
            }
            case 40851:                         // Disgruntled
            {
                for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(SpellEffectIndex(i)))
                    {
                        eff->EffectImplicitTargetA = TARGET_SELF;
                        eff->EffectImplicitTargetB = TARGET_NONE;
                    }
                }
                break;
            }
            case 43660:                         // Brewfest - Throw Keg - DND
            {
                spell->AttributesEx &= ~SPELL_ATTR_EX_NEGATIVE;
                break;
            }
            case 43999:                         // Corpse Explosion
            {
                spell->AttributesEx3 |= SPELL_ATTR_EX3_CANT_MISS;
                break;
            }
            case 44544:                         // Fingers of Frost
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectSpellClassMask.Flags |= UI64LIT(0x20000);    // Ice Lance mask
                break;
            }
            case 44807:                         // Crazed Rage
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                {
                    eff->EffectImplicitTargetA = TARGET_SELF;
                    eff->EffectImplicitTargetB = TARGET_SELF;
                }
                break;
            }
            case 45284:                         // Lightning (Elemental Overload)
            case 45297:                         // Chain Lightning (Elemental Overload)
            case 77451:                         // Lava Burst (Elemental Overload)
            {
                spell->AttributesEx2 |= SPELL_ATTR_EX2_CANT_CRIT;
                break;
            }
            case 45440:                         // Steam Tonk Controller
            {
                for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(SpellEffectIndex(i)))
                        eff->Effect = 0;
                }
                break;
            }
            case 45732:                         // Torch Toss
            case 46054:                         // Torch Toss (land)
            {
                if (SpellCastingRequirementsEntry* req = (SpellCastingRequirementsEntry*)spell->GetSpellCastingRequirements())
                    req->RequiresSpellFocus = 0;

                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectImplicitTargetA = TARGET_SCRIPT_COORDINATES;
                break;
            }
            case 46747:                         // Fling Torch
            {
                if (SpellCastingRequirementsEntry* req = (SpellCastingRequirementsEntry*)spell->GetSpellCastingRequirements())
                    req->RequiresSpellFocus = 0;
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectImplicitTargetA = TARGET_SELF;
                break;
            }
            case 46924:                         // Bladestorm
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_1))
                    eff->EffectMiscValue &= ~(1 << (MECHANIC_DISARM-1));
                break;
            }
            case 47094:                         // No Man's Land
            case 58101:                         // No Man's Land
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                {
                    eff->Effect = SPELL_EFFECT_APPLY_AREA_AURA_ENEMY;
                    eff->EffectRadiusIndex = 12;   // 100 yards
                }
                break;
            }
            case 48650:                         // Haxx: disable Fetch Ball
            {
                for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(SpellEffectIndex(i)))
                        eff->Effect = 0;
                }
                break;
            }
            case 48743:                         // Death Pact
            {
                spell->AttributesEx &= ~SPELL_ATTR_EX_CANT_TARGET_SELF;
                break;
            }
            case 50025:                         // Plague Barrel
            {
                spell->rangeIndex = 164;
            }
            case 50026:                         // Plague Slime
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_2))
                    eff->Effect = 0;
                break;
            }
            case 50317:                         // D.I.S.C.O
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                {
                    eff->Effect = SPELL_EFFECT_TRANS_DOOR; // was SPELL_EFFECT_SUMMON
                    eff->EffectMiscValue = 190351;
                }
                break;
            }
            case 50421:                         // Scent of Blood
            {
                if (SpellAuraOptionsEntry* opt = (SpellAuraOptionsEntry*)spell->GetSpellAuraOptions())
                    opt->procCharges = 1;
                break;
            }
            case 50536:                         // Unholy Blight
            {
                spell->speed = 24.0f;
                break;
            }
            case 51698:                         // Honor Among Thieves (Rank 1)
            {
                spell->SpellClassOptionsId = 12892;
                break;
            }
            case 51755:                         // Camouflage
            {
                if (SpellInterruptsEntry* inter = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                {
                    inter->AuraInterruptFlags &= ~4;    // AURA_INTERRUPT_FLAG_CAST
                    inter->AuraInterruptFlags |= 8192;  // AURA_INTERRUPT_FLAG_SPELL_ATTACK
                }
                break;
            }
            case 51804:                         // Power Siphon disable
            {
                for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(SpellEffectIndex(i)))
                        eff->Effect = 0;
                }
                break;
            }
            //case 52481:                       // Disable Gift of the Harvester
            //{
            //    spell->Effect[0] = 0;
            //    spell->EffectTriggerSpell[0] = 0;
            //    break;
            //}
            case 52752:                         // Ancestral Awakening can crit
            {
                spell->AttributesEx2 &= ~SPELL_ATTR_EX2_CANT_CRIT;
                break;
            }
            case 54408:                         // Conjure Mana Gem triggered
            case 54409:
            case 54410:
            case 54411:
            case 54414:
            case 54412:
            case 54413:
            {
                if (SpellPowerEntry* pow = (SpellPowerEntry*)spell->GetSpellPower())
                    pow->manaCost = 0;
                break;
            }
            case 54831:                         // Glyph of Hurricane
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectApplyAuraName = SPELL_AURA_ADD_FLAT_MODIFIER;
                break;
            }
            case 54833:                         // Innervate and Glyph of Innervate
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectApplyAuraName = SPELL_AURA_PERIODIC_ENERGIZE;
                break;
            }
            case 54861:                         // Nitro Boosts
            {
                spell->SpellInterruptsId = 9309; // AURA_INTERRUPT_FLAG_DIRECT_DAMAGE;
                break;
            }
            case 55078:                         // Blood Fever
            {
                if (SpellCategoriesEntry* cat = (SpellCategoriesEntry*)spell->GetSpellCategories())
                    cat->DmgClass = SPELL_DAMAGE_CLASS_NONE;
                break;
            }
            case 55098:                         // Transformation
            {
                if (SpellInterruptsEntry* intr = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                    intr->InterruptFlags |= 0x04;
                break;
            }
            case 55277:                         // Effect of Glyph of Stoneclaw Totem
            {
                if (SpellClassOptionsEntry* opt = (SpellClassOptionsEntry*)spell->GetSpellClassOptions())
                    opt->SpellFamilyFlags.Flags &= ~(UI64LIT(0x1));
                break;
            }
            case 56244:                         // Glyph of Fear
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                {
                    eff->EffectBasePoints = 5000;
                    eff->EffectApplyAuraName = SPELL_AURA_ADD_FLAT_MODIFIER;
                    eff->EffectMiscValue = 11;  // SPELLMOD_COOLDOWN
                }
                break;
            }
            case 56641:                         // Steady Shot
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_2))
                    eff->EffectImplicitTargetA = TARGET_CHAIN_DAMAGE;
                break;
            }
            case 57529:                         // Arcane potency
            case 57531:
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectSpellClassMask.Flags |= UI64LIT(0x800000000802);    // Arcane Missiles + Fire Blast + Arcane Barrage
                break;
            }
            case 58530:                         // Return to Stormwind
            case 58551:                         // Return to Orgrimmar
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->Effect = 0;
                break;
            }
            case 59887:                         // Borrowed Time proc
            case 59888:
            case 59889:
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectSpellClassMask.Flags |= UI64LIT(0x142490);
                break;
            }
            case 60661:                         // LK Arena 4 Gladiator's Libram of Justice
            case 60662:                         // LK Arena 5 Gladiator's Libram of Justice
            case 60664:                         // LK Arena 6 Gladiator's Libram of Justice
            case 60800:                         // Libram of Souls Redeemed
            {
                if (SpellClassOptionsEntry* opt = (SpellClassOptionsEntry*)spell->GetSpellClassOptions())
                    opt->SpellFamilyFlags.Flags &= ~UI64LIT(0x0000000040000000);
                break;
            }
            case 61719:                         // Easter Lay Noblegarden Egg Aura
            {
                if (SpellInterruptsEntry* intr = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                    intr->AuraInterruptFlags = 1 | 2; //AURA_INTERRUPT_FLAG_UNK0 | AURA_INTERRUPT_FLAG_DAMAGE;
                break;
            }
            case 62991:                         // Bonked!
            {
                spell->Attributes |= SPELL_ATTR_CANT_CANCEL;
                break;
            }
            case 63675:                         // Improved Devouring Plague
            {
                spell->AttributesEx6 |= SPELL_ATTR_EX6_NO_DMG_MODS;
                break;
            }
            case 64380:                         // megai2: set 0 index to Shattering throw triggered spell
            {
                spell->CastingTimeIndex = 0;
                break;
            }
            case 64568:                         // Blood Reserve
            {
                if (SpellAuraOptionsEntry* opt = (SpellAuraOptionsEntry*)spell->GetSpellAuraOptions())
                    opt->procCharges = 1;
                break;
            }
            case 64844:                         // Divine Hymn trigger
            {
                spell->AttributesEx &= ~SPELL_ATTR_EX_NEGATIVE;
                if (SpellCategoriesEntry* cat = (SpellCategoriesEntry*)spell->GetSpellCategories())
                    cat->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;
                break;
            }
            case 64904:                         // Hymn of Hope trigger
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_1))
                    eff->EffectApplyAuraName = SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT;
                spell->AttributesEx &= ~SPELL_ATTR_EX_NEGATIVE;
                break;
            }
            case 66532:                         // Fel Fireball
            case 66963:
            case 66964:
            case 66965:
            {
                if (SpellInterruptsEntry* intr = (SpellInterruptsEntry*)spell->GetSpellInterrupts())
                    intr->InterruptFlags |= 0x04;
                break;
            }
            case 67712:                         // Item - Coliseum 25 Normal Caster Trinket
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectTriggerSpell = 67713;
                break;
            }
            case 67758:                         // Item - Coliseum 25 Heroic Caster Trinket
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectTriggerSpell = 67759;
                break;
            }
            case 70157:                         // megai2: sindragosa icetomb
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_2))
                    eff->EffectTriggerSpell = 43979;
                break;
            }
            case 70873:                         // Valithria Emerald Vigor
            {
                for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(SpellEffectIndex(i)))
                    {
                        eff->EffectImplicitTargetA = TARGET_SELF;
                        eff->EffectImplicitTargetB = TARGET_NONE;
                    }
                }
                break;
            }
            case 71880:                         // Heartpiece dummy aura
            case 71892:
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectTriggerSpell = 0;
                break;
            }
            case 70346:                         // megai2: set 8yd range for slime puddle of proffesor putricide
            case 72868:
            case 72869:
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectRadiusIndex = 14;
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_1))
                    eff->EffectRadiusIndex = 14;
                break;
            }
            case 72706:                         // Achievement Check (Valithria Dreamwalker)
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectRadiusIndex = EFFECT_RADIUS_200_YARDS;   // 200yd
                break;
            }
            case 72968:                         // Precious's Ribbon
            {
                spell->AttributesEx3 |= SPELL_ATTR_EX3_DEATH_PERSISTENT;
                break;
            }
            case 26573:
            {
                spell->SpellVisual[0] = 5600;
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_2))
                {
                    eff->Effect = 27;
                    eff->EffectApplyAuraName = 4;
                    eff->EffectImplicitTargetA = 18;
                    eff->EffectImplicitTargetB = 16;
                }
                break;
            }
            case 82690:                         // Flame Orb
            case 84717:                         // Frostfire Orb
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectAmplitude = 1000;
                break;
            }
            case 83154:                         // Piercing Chill
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_2))
                    eff->EffectImplicitTargetA = TARGET_CURRENT_ENEMY_COORDINATES;
                break;
            }
            case 687:                           // Demon Armor
            case 28176:                         // Fel Armor
            case 48517:                         // Eclipse (Solar)
            case 48518:                         // Eclipse (Lunar)
            case 67483:                         // Eclipse Marker
            case 67484:                         // Eclipse Marker
            case 81206:                         // Chakra: Sanctuary
            case 81208:                         // Chakra: Serenity
            case 81209:                         // Chakra: Chastice
            case 83582:                         // Pyromaniac
            case 94338:                         // Eclipse (Solar)
            case 96206:                         // Nature's Bounty
            {
                spell->AttributesEx6 |= SPELL_ATTR_EX6_REMOVED_AT_SPEC_SWITCH;
                break;
            }
            case 85547:                         // Jinx: Curse of the Elements
            case 86105:                         // Jinx: Curse of the Elements
            {
                spell->AttributesEx |= SPELL_ATTR_EX_NOT_BREAK_STEALTH;
                spell->AttributesEx3 |= SPELL_ATTR_EX3_NO_INITIAL_AGGRO;
                break;
            }
            case 86150:                         // Guardian of Ancient Kings
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectImplicitTargetA = TARGET_SELF;
                break;
            }
            case 87178:                         // Mind Spike
            {
                if (SpellAuraOptionsEntry* opts = (SpellAuraOptionsEntry*)spell->GetSpellAuraOptions())
                    opts->procCharges = 1;
                break;
            }
            case 88611:                         // Smoke Bomb
            {
                spell->Attributes |= SPELL_ATTR_CANT_CANCEL;
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                {
                    eff->EffectImplicitTargetA = TARGET_AREAEFFECT_CUSTOM;
                    eff->EffectImplicitTargetB = TARGET_NONE;
                }
                break;
            }
            case 97985:                         // Feral Swiftness Clear
            {
                spell->AttributesEx |= SPELL_ATTR_EX_NOT_BREAK_STEALTH;
                break;
            }
            case 99009:                         // Item - Druid T12 Feral 4P Bonus
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectApplyAuraName = SPELL_AURA_ADD_TARGET_TRIGGER;
                break;
            }
            case 101626:                        // Pulverize
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectMiscValue = 15;
                break;
            }
            //case 101815:                        // Pulverize
            //{
            //    if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
            //        eff->EffectRadiusIndex = 50;    // 15 yd
            //    break;
            //}
            case 105722:                        // Item - Druid T13 Balance 2P Bonus (Insect Swarm)
            {
                if (SpellEffectEntry* eff = (SpellEffectEntry*)spell->GetSpellEffect(EFFECT_INDEX_0))
                    eff->EffectSpellClassMask.Flags &= ~UI64LIT(0x10);
                break;
            }
            case 108464:                        // Night Elf Illusion
            {
                spell->Attributes |= SPELL_ATTR_CASTABLE_WHILE_DEAD;
                break;
            }
         }

        // Lightwell, enable healing spd scale
        if (spell->IsFitToFamily(SPELLFAMILY_PRIEST, UI64LIT(0x0), 0x4000))
            if (SpellCategoriesEntry* cat = (SpellCategoriesEntry*)spell->GetSpellCategories())
                cat->DmgClass = SPELL_DAMAGE_CLASS_MAGIC;

        // Blind and Sap speed
        if (spell->Id == 2094 || spell->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x80)))
            spell->speed = 50;

        // Envenom
        if (spell->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x800000000)))
            spell->AttributesEx6 |= SPELL_ATTR_EX6_NO_DMG_MODS;

        // Penance heal start trigger
        if (spell->IsFitToFamily(SPELLFAMILY_PRIEST, UI64LIT(0x1000000000000), 0x80))
            spell->AttributesEx &= ~SPELL_ATTR_EX_NEGATIVE;
    }

    // Check loaded DBC files proper version
    if (!sAreaStore.LookupEntry(4713)              ||       // last area (areaflag) added in 4.3.4
        !sCharTitlesStore.LookupEntry(287)         ||       // last char title added in 4.3.4
        !sGemPropertiesStore.LookupEntry(2250)     ||       // last gem property added in 4.3.4
        !sMapStore.LookupEntry(980)                ||       // last map added in 4.3.4
        !sSpellStore.LookupEntry(121820)           )        // last added spell in 4.3.4
    {
        sLog.outError("\nYou have mixed version DBC files. Please re-extract DBC files for one from client build: %s",AcceptableClientBuildsListStr().c_str());
        Log::WaitBeforeContinueIfNeed();
        exit(1);
    }

    sLog.outString();
    sLog.outString( ">> Initialized %d data stores", DBCFilesCount );
}

SimpleFactionsList const* GetFactionTeamList(uint32 faction)
{
    FactionTeamMap::const_iterator itr = sFactionTeamMap.find(faction);
    if(itr==sFactionTeamMap.end())
        return NULL;
    return &itr->second;
}

char const* GetPetName(uint32 petfamily, uint32 dbclang)
{
    if(!petfamily)
        return NULL;
    CreatureFamilyEntry const *pet_family = sCreatureFamilyStore.LookupEntry(petfamily);
    if(!pet_family)
        return NULL;
    return pet_family->Name[dbclang]?pet_family->Name[dbclang]:NULL;
}

TalentSpellPos const* GetTalentSpellPos(uint32 spellId)
{
    TalentSpellPosMap::const_iterator itr = sTalentSpellPosMap.find(spellId);
    if(itr==sTalentSpellPosMap.end())
        return NULL;

    return &itr->second;
}

SpellEffectEntry const* GetSpellEffectEntry(uint32 spellId, SpellEffectIndex effect)
{
    SpellEffectMap::const_iterator itr = sSpellEffectMap.find(spellId);
    if(itr == sSpellEffectMap.end())
        return NULL;

    return itr->second.effects[effect];
}

uint32 GetTalentSpellCost(TalentSpellPos const* pos)
{
    if (pos)
        return pos->rank+1;

    return 0;
}

uint32 GetTalentSpellCost(uint32 spellId)
{
    return GetTalentSpellCost(GetTalentSpellPos(spellId));
}

int32 GetAreaFlagByAreaID(uint32 area_id)
{
    AreaFlagByAreaID::iterator i = sAreaFlagByAreaID.find(area_id);
    if(i == sAreaFlagByAreaID.end())
        return -1;

    return i->second;
}

WMOAreaTableEntry const* GetWMOAreaTableEntryByTripple(int32 rootid, int32 adtid, int32 groupid)
{
    WMOAreaInfoByTripple::iterator i = sWMOAreaInfoByTripple.find(WMOAreaTableTripple(rootid, adtid, groupid));
    if(i == sWMOAreaInfoByTripple.end())
        return NULL;
    return i->second;
}

AreaTableEntry const* GetAreaEntryByAreaID(uint32 area_id)
{
    int32 areaflag = GetAreaFlagByAreaID(area_id);
    if(areaflag < 0)
        return NULL;

    return sAreaStore.LookupEntry(areaflag );
}

AreaTableEntry const* GetAreaEntryByAreaFlagAndMap(uint32 area_flag,uint32 map_id)
{
    if(area_flag)
        return sAreaStore.LookupEntry(area_flag);

    if(MapEntry const* mapEntry = sMapStore.LookupEntry(map_id))
        return GetAreaEntryByAreaID(mapEntry->linked_zone);

    return NULL;
}

uint32 GetAreaFlagByMapId(uint32 mapid)
{
    AreaFlagByMapID::iterator i = sAreaFlagByMapID.find(mapid);
    if(i == sAreaFlagByMapID.end())
        return 0;
    else
        return i->second;
}

std::string GetAreaNameById(uint32 areaid, LocaleConstant locale)
{
    AreaTableEntry const * area = GetAreaEntryByAreaID(areaid);
    if (!area)
        return "";

    if (std::string(area->area_name[locale]) != "")
        return area->area_name[locale];
    else
        return std::string(area->area_name[0]);
}

uint32 GetVirtualMapForMapAndZone(uint32 mapid, uint32 zoneId)
{
    if (mapid != 530 && mapid != 571)               // speed for most cases
        return mapid;

    if (WorldMapAreaEntry const* wma = sWorldMapAreaStore.LookupEntry(zoneId))
        return wma->virtual_map_id >= 0 ? wma->virtual_map_id : wma->map_id;

    return mapid;
}

ContentLevels GetContentLevelsForMapAndZone(uint32 mapId, uint32 zoneId)
{
    MapEntry const* mapEntry = sMapStore.LookupEntry(mapId);
    if (!mapEntry)
        return CONTENT_1_60;

    if (mapEntry->rootPhaseMap != -1)
        mapId = mapEntry->rootPhaseMap;

    switch (mapId)
    {
        case 648:   // Lost Islands
        case 654:   // Gilneas
            return CONTENT_1_60;
        default:
            break;
    }

    switch (zoneId)
    {
        case 616:   // Mount Hyjal
        case 4922:  // Twilight Highlands
        case 5034:  // Uldum
        case 5042:  // Deepholm
            return CONTENT_81_85;
        default:
            break;
    }

    switch (mapEntry->Expansion())
    {
        default: return CONTENT_1_60;
        case 1:  return CONTENT_61_70;
        case 2:  return CONTENT_71_80;
        case 3:  return CONTENT_81_85;
    }
}

ChatChannelsEntry const* GetChannelEntryFor(uint32 channel_id)
{
    // not sorted, numbering index from 0
    for(uint32 i = 0; i < sChatChannelsStore.GetNumRows(); ++i)
    {
        ChatChannelsEntry const* ch = sChatChannelsStore.LookupEntry(i);
        if(ch && ch->ChannelID == channel_id)
            return ch;
    }
    return NULL;
}

bool IsTotemCategoryCompatiableWith(uint32 itemTotemCategoryId, uint32 requiredTotemCategoryId)
{
    if(requiredTotemCategoryId==0)
        return true;
    if(itemTotemCategoryId==0)
        return false;

    TotemCategoryEntry const* itemEntry = sTotemCategoryStore.LookupEntry(itemTotemCategoryId);
    if(!itemEntry)
        return false;
    TotemCategoryEntry const* reqEntry = sTotemCategoryStore.LookupEntry(requiredTotemCategoryId);
    if(!reqEntry)
        return false;

    if(itemEntry->categoryType!=reqEntry->categoryType)
        return false;

    return (itemEntry->categoryMask & reqEntry->categoryMask)==reqEntry->categoryMask;
}

bool Zone2MapCoordinates(float& x,float& y,uint32 zone)
{
    WorldMapAreaEntry const* maEntry = sWorldMapAreaStore.LookupEntry(zone);

    // if not listed then map coordinates (instance)
    if (!maEntry || maEntry->x2 == maEntry->x1 || maEntry->y2 == maEntry->y1)
        return false;

    std::swap(x,y);                                         // at client map coords swapped
    x = x*((maEntry->x2-maEntry->x1)/100)+maEntry->x1;
    y = y*((maEntry->y2-maEntry->y1)/100)+maEntry->y1;      // client y coord from top to down

    return true;
}

bool Map2ZoneCoordinates(float& x,float& y,uint32 zone)
{
    WorldMapAreaEntry const* maEntry = sWorldMapAreaStore.LookupEntry(zone);

    // if not listed then map coordinates (instance)
    if (!maEntry || maEntry->x2 == maEntry->x1 || maEntry->y2 == maEntry->y1)
        return false;

    x = (x-maEntry->x1)/((maEntry->x2-maEntry->x1)/100);
    y = (y-maEntry->y1)/((maEntry->y2-maEntry->y1)/100);    // client y coord from top to down
    std::swap(x,y);                                         // client have map coords swapped

    return true;
}

MapDifficultyEntry const* GetMapDifficultyData(uint32 mapId, Difficulty difficulty)
{
    MapDifficultyMap::const_iterator itr = sMapDifficultyMap.find(MAKE_PAIR32(mapId,difficulty));
    return itr != sMapDifficultyMap.end() ? itr->second : NULL;
}

PvPDifficultyEntry const* GetBattlegroundBracketByLevel( uint32 mapid, uint32 level )
{
    PvPDifficultyEntry const* maxEntry = NULL;              // used for level > max listed level case
    for(uint32 i = 0; i < sPvPDifficultyStore.GetNumRows(); ++i)
    {
        if (PvPDifficultyEntry const* entry = sPvPDifficultyStore.LookupEntry(i))
        {
            // skip unrelated and too-high brackets
            if (entry->mapId != mapid || entry->minLevel > level)
                continue;

            // exactly fit
            if (entry->maxLevel >= level)
                return entry;

            // remember for possible out-of-range case (search higher from existed)
            if (!maxEntry || maxEntry->maxLevel < entry->maxLevel)
                maxEntry = entry;
        }
    }

    return maxEntry;
}

PvPDifficultyEntry const* GetBattlegroundBracketById(uint32 mapid, BattleGroundBracketId id)
{
    for(uint32 i = 0; i < sPvPDifficultyStore.GetNumRows(); ++i)
        if (PvPDifficultyEntry const* entry = sPvPDifficultyStore.LookupEntry(i))
            if (entry->mapId == mapid && entry->GetBracketId() == id)
                return entry;

    return NULL;
}

uint32 const* GetTalentTabPages(uint32 cls)
{
    return sTalentTabPages[cls];
}

std::vector<uint32> const* GetTalentTreeMasterySpells(uint32 talentTree)
{
    TalentTreeSpellsMap::const_iterator itr = sTalentTreeMasterySpellsMap.find(talentTree);
    if (itr == sTalentTreeMasterySpellsMap.end())
        return NULL;

    return &itr->second;
}

std::vector<uint32> const* GetTalentTreePrimarySpells(uint32 talentTree)
{
    TalentTreeSpellsMap::const_iterator itr = sTalentTreePrimarySpellsMap.find(talentTree);
    if (itr == sTalentTreePrimarySpellsMap.end())
        return NULL;

    return &itr->second;
}

uint32 GetTalentTreeRolesMask(uint32 talentTree)
{
    TalentTreeRolesMap::const_iterator itr = sTalentTreeRolesMap.find(talentTree);
    if (itr == sTalentTreeRolesMap.end())
        return 0;

    return itr->second;
}

bool IsPointInAreaTriggerZone(AreaTriggerEntry const* atEntry, uint32 mapid, float x, float y, float z, float delta)
{
    if (mapid != atEntry->mapid)
        return false;

    if (atEntry->radius > 0)
    {
        // if we have radius check it
        float dist2 = (x-atEntry->x)*(x-atEntry->x) + (y-atEntry->y)*(y-atEntry->y) + (z-atEntry->z)*(z-atEntry->z);
        if(dist2 > (atEntry->radius + delta)*(atEntry->radius + delta))
            return false;
    }
    else
    {
        // we have only extent

        // rotate the players position instead of rotating the whole cube, that way we can make a simplified
        // is-in-cube check and we have to calculate only one point instead of 4

        // 2PI = 360, keep in mind that ingame orientation is counter-clockwise
        double rotation = 2*M_PI-atEntry->box_orientation;
        double sinVal = sin(rotation);
        double cosVal = cos(rotation);

        float playerBoxDistX = x - atEntry->x;
        float playerBoxDistY = y - atEntry->y;

        float rotPlayerX = float(atEntry->x + playerBoxDistX * cosVal - playerBoxDistY*sinVal);
        float rotPlayerY = float(atEntry->y + playerBoxDistY * cosVal + playerBoxDistX*sinVal);

        // box edges are parallel to coordiante axis, so we can treat every dimension independently :D
        float dz = z - atEntry->z;
        float dx = rotPlayerX - atEntry->x;
        float dy = rotPlayerY - atEntry->y;
        if( (fabs(dx) > atEntry->box_x/2 + delta) ||
            (fabs(dy) > atEntry->box_y/2 + delta) ||
            (fabs(dz) > atEntry->box_z/2 + delta) )
        {
            return false;
        }
    }

    return true;
}

uint32 GetCreatureModelRace(uint32 model_id)
{
    CreatureDisplayInfoEntry const* displayEntry = sCreatureDisplayInfoStore.LookupEntry(model_id);
    if (!displayEntry)
        return 0;
    CreatureDisplayInfoExtraEntry const* extraEntry = sCreatureDisplayInfoExtraStore.LookupEntry(displayEntry->ExtendedDisplayInfoID);
    return extraEntry ? extraEntry->Race : 0;
}

float GetCurrencyPrecision(uint32 currencyId)
{
    CurrencyTypesEntry const * entry = sCurrencyTypesStore.LookupEntry(currencyId);

    return entry ? entry->GetPrecision() : 1.0f;
}

ResearchSiteEntry const* GetResearchSiteEntryById(uint32 id)
{
    ResearchSiteDataMap::const_iterator itr = sResearchSiteDataMap.find(id);
    if (itr == sResearchSiteDataMap.end())
        return NULL;

    return itr->second.entry;
}

// script support functions
MANGOS_DLL_SPEC DBCStorage <SoundEntriesEntry>  const* GetSoundEntriesStore()   { return &sSoundEntriesStore;   }
MANGOS_DLL_SPEC DBCStorage <SpellEntry>         const* GetSpellStore()          { return &sSpellStore;          }
MANGOS_DLL_SPEC DBCStorage <SpellRangeEntry>    const* GetSpellRangeStore()     { return &sSpellRangeStore;     }
MANGOS_DLL_SPEC DBCStorage <FactionEntry>       const* GetFactionStore()        { return &sFactionStore;        }
MANGOS_DLL_SPEC DBCStorage <CreatureDisplayInfoEntry> const* GetCreatureDisplayStore() { return &sCreatureDisplayInfoStore; }
MANGOS_DLL_SPEC DBCStorage <EmotesEntry>        const* GetEmotesStore()         { return &sEmotesStore;         }
MANGOS_DLL_SPEC DBCStorage <EmotesTextEntry>    const* GetEmotesTextStore()     { return &sEmotesTextStore;     }
MANGOS_DLL_SPEC DBCStorage <AchievementEntry>   const* GetAchievementStore()    { return &sAchievementStore;    }
MANGOS_DLL_SPEC DBCStorage <CreatureModelDataEntry> const* GetCreatureModelDataStore() { return &sCreatureModelDataStore; }
MANGOS_DLL_SPEC DBCStorage <CreatureDisplayInfoEntry> const* GetCreatureDisplayInfoStore() { return &sCreatureDisplayInfoStore; }
