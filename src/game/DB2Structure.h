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

#ifndef MANGOS_DB2STRUCTURE_H
#define MANGOS_DB2STRUCTURE_H

#include "Common.h"
#include "DBCEnums.h"
#include "Path.h"
#include "Platform/Define.h"

#include <map>
#include <set>
#include <vector>

// Structures using to access raw DB2 data and required packing to portability
struct ItemEntry
{
   uint32   ID;                                             // 0
   uint32   Class;                                          // 1
   uint32   SubClass;                                       // 2
   int32    Unk0;                                           // 3
   int32    Material;                                       // 4
   uint32   DisplayId;                                      // 5
   uint32   InventoryType;                                  // 6
   uint32   Sheath;                                         // 7
};

struct ItemCurrencyCostEntry
{
    //uint32 id;                                            // 0
    uint32 itemid;                                          // 1
};

#define MAX_EXTENDED_COST_ITEMS         5
#define MAX_EXTENDED_COST_CURRENCIES    5

enum ItemExtendedCostFlags
{
    ITEM_EXTENDED_COST_FLAG_UNK                 = 0x01, // guild related
    ITEM_EXTENDED_COST_FLAG_SEASON_IN_INDEX_0   = 0x02, // currency requirements under these indexes require season count
    ITEM_EXTENDED_COST_FLAG_SEASON_IN_INDEX_1   = 0x04,
    ITEM_EXTENDED_COST_FLAG_SEASON_IN_INDEX_2   = 0x08,
    ITEM_EXTENDED_COST_FLAG_SEASON_IN_INDEX_3   = 0x10,
    ITEM_EXTENDED_COST_FLAG_SEASON_IN_INDEX_4   = 0x20,
};

struct ItemExtendedCostEntry
{
    uint32      Id;                                         // 0
    //                                                      // 1        unk, old reqhonorpoints
    //                                                      // 2        unk, old reqarenapoints
    uint32      reqarenaslot;                               // 3        m_arenaBracket
    uint32      reqitem[MAX_EXTENDED_COST_ITEMS];           // 5-8      m_itemID
    uint32      reqitemcount[MAX_EXTENDED_COST_ITEMS];      // 9-13     m_itemCount
    uint32      reqpersonalarenarating;                     // 14       m_requiredArenaRating
    //uint32                                                // 15       m_itemPurchaseGroup
    uint32      reqcur[MAX_EXTENDED_COST_CURRENCIES];       // 16-20
    uint32      reqcurrcount[MAX_EXTENDED_COST_CURRENCIES]; // 21-25
                                                            // 26       reputation-related
                                                            // 27       reputation-related
    uint32      flags;                                      // 28
    //                                                      // 29
    //                                                      // 30

    bool IsSeasonCurrencyRequirement(uint32 i) const
    {
        MANGOS_ASSERT(i < MAX_EXTENDED_COST_CURRENCIES);

        // start from ITEM_EXTENDED_COST_FLAG_SEASON_IN_INDEX_0
        return flags & 1 << (i + 1);
    }
};

struct ItemSparseEntry
{
    uint32     ID;                                           // 0
    uint32     Quality;                                      // 1
    uint32     Flags;                                        // 2
    uint32     Flags2;                                       // 3
    float      Unk430_1;
    float      Unk430_2;
    uint32     BuyCount;
    uint32     BuyPrice;                                     // 4
    uint32     SellPrice;                                    // 5
    uint32     InventoryType;                                // 6
    int32      AllowableClass;                               // 7
    int32      AllowableRace;                                // 8
    uint32     ItemLevel;                                    // 9
    int32      RequiredLevel;                                // 10
    uint32     RequiredSkill;                                // 11
    uint32     RequiredSkillRank;                            // 12
    uint32     RequiredSpell;                                // 13
    uint32     RequiredHonorRank;                            // 14
    uint32     RequiredCityRank;                             // 15
    uint32     RequiredReputationFaction;                    // 16
    uint32     RequiredReputationRank;                       // 17
    uint32     MaxCount;                                     // 18
    uint32     Stackable;                                    // 19
    uint32     ContainerSlots;                               // 20
    int32      ItemStatType[MAX_ITEM_PROTO_STATS];           // 21 - 30
    uint32     ItemStatValue[MAX_ITEM_PROTO_STATS];          // 31 - 40
    int32      ItemStatUnk1[MAX_ITEM_PROTO_STATS];           // 41 - 50
    int32      ItemStatUnk2[MAX_ITEM_PROTO_STATS];           // 51 - 60
    uint32     ScalingStatDistribution;                      // 61
    uint32     DamageType;                                   // 62
    uint32     Delay;                                        // 63
    float      RangedModRange;                               // 64
    int32      SpellId[MAX_ITEM_PROTO_SPELLS];               // 65 - 69
    int32      SpellTrigger[MAX_ITEM_PROTO_SPELLS];          // 70 - 74
    int32      SpellCharges[MAX_ITEM_PROTO_SPELLS];          // 75 - 79
    int32      SpellCooldown[MAX_ITEM_PROTO_SPELLS];         // 80 - 84
    int32      SpellCategory[MAX_ITEM_PROTO_SPELLS];         // 85 - 89
    int32      SpellCategoryCooldown[MAX_ITEM_PROTO_SPELLS]; // 90 - 94
    uint32     Bonding;                                      // 95
    DBCString* Name;                                         // 96
    DBCString* Name2;                                        // 97
    DBCString* Name3;                                        // 98
    DBCString* Name4;                                        // 99
    DBCString* Description;                                  // 100
    uint32     PageText;                                     // 101
    uint32     LanguageID;                                   // 102
    uint32     PageMaterial;                                 // 103
    uint32     StartQuest;                                   // 104
    uint32     LockID;                                       // 105
    int32      Material;                                     // 106
    uint32     Sheath;                                       // 107
    uint32     RandomProperty;                               // 108
    uint32     RandomSuffix;                                 // 109
    uint32     ItemSet;                                      // 110
    uint32     Area;                                         // 112
    uint32     Map;                                          // 113
    uint32     BagFamily;                                    // 114
    uint32     TotemCategory;                                // 115
    uint32     Color[MAX_ITEM_PROTO_SOCKETS];                // 116 - 118
    uint32     Content[MAX_ITEM_PROTO_SOCKETS];              // 119 - 121
    int32      SocketBonus;                                  // 122
    uint32     GemProperties;                                // 123
    float      ArmorDamageModifier;                          // 124
    uint32     Duration;                                     // 125
    uint32     ItemLimitCategory;                            // 126
    uint32     HolidayId;                                    // 127
    float      StatScalingFactor;                            // 128
    int32      CurrencySubstitutionId;                       // 129
    int32      CurrencySubstitutionCount;                    // 130
};

#define KEYCHAIN_SIZE   32

struct KeyChainEntry
{
    uint32      Id;
    uint8       Key[KEYCHAIN_SIZE];
};

#endif
