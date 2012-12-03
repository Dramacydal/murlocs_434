SET NAMES 'utf8';

DELETE FROM `creature_template` WHERE `entry` = 88004;
INSERT INTO `creature_template` 
(`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `modelid_3`, `modelid_4`,     `name`,       `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `powerType`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `vehicle_id`,`mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `health_mod`, `power_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUE
(88004,            0,                     0,                   0,                  0,            0,           15394,        15395,      15398,       15397,    'Murloc Helper',      '',        NULL,       0,                  5,        5,         100,          100,         0,            0,         0,        91,       35,         35,   0x620001,          1,           1.14,       1,       0,      5,         6,        0,              2,             1,                2000,            2000,              1,             0,           0,            0,          0,             0,                 0,                 0,            4,           5,                2,             12,   0x800400,         0,           0,              0,          0,              0,              0,             0,               0,           0,          0,         0,       0,        0,        0,        0,        0,        0,         0,               0,            0,         0,        '',        0,                3,           1,           1,              0,              0,          0,              0,             0,          0,           0,            0,               1,            0,             0,           0,            0,                      2,     'npc_premium_pet');

-- DELETE FROM `item_template` WHERE `entry` = 15;
-- Soulbound Test Gem
DELETE FROM `item_template` WHERE `entry` = 33060;
DELETE FROM `locales_item` WHERE `entry` = 33060;
INSERT INTO `item_template`
(`entry`, `class`, `subclass`, `unk0`,    `name`,               `displayid`, `Quality`, `Flags`, `Flags2`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `delay`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`,                                 `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`,   `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `Duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `ExtraFlags`) VALUE
(33060,      15,       0,        -1,     'Murloc Premium Item',    43108,         5,         1,      0,         1,         0,            0,           0,                -1,              -1,              1,             0,               0,               0,                  0,                 0,                  0,                    0,                             0,                  1,          1,            0,               0,           0,              0,           0,               0,            0,            0,            0,              0,            0,              0,            0,             0,           0,              0,           0,             0,            0,             0,             0,                0,                   0,           0,           2654,            0,                 0,                0,            1500,                 0,                     -1,                   0,               0,             0,                0,                 -1,                 0,                   -1,                  0,              0,               0,                0,                   -1,               0,              -1,                        0,              0,              0,                0,                 -1,                0,                 -1,                       0,                0,             0,               0,             -1,                 0,                  -1,                       1,     'Item for 300Murlocs Premium Account',             0,            0,           0,              0,            0,         4,         0,          0,              0,              0,           0,           0,      0,       4096,         0,                0,                0,                   0,              0,                0,               0,               0,                0,                 -1,                   0,                   0,                0,                0,           '',         0,              0,              0,            0,             0);

/*
backup of item 33060 (Soulbound Test Gem)

REPLACE INTO item_template
   (`entry`, `class`, `subclass`, `unk0`, `name`, `displayid`, `Quality`, `Flags`, `Flags2`, `BuyCount`, `BuyPrice`, `SellPrice`, `InventoryType`, `AllowableClass`, `AllowableRace`, `ItemLevel`, `RequiredLevel`, `RequiredSkill`, `RequiredSkillRank`, `requiredspell`, `requiredhonorrank`, `RequiredCityRank`, `RequiredReputationFaction`, `RequiredReputationRank`, `maxcount`, `stackable`, `ContainerSlots`, `StatsCount`, `stat_type1`, `stat_value1`, `stat_type2`, `stat_value2`, `stat_type3`, `stat_value3`, `stat_type4`, `stat_value4`, `stat_type5`, `stat_value5`, `stat_type6`, `stat_value6`, `stat_type7`, `stat_value7`, `stat_type8`, `stat_value8`, `stat_type9`, `stat_value9`, `stat_type10`, `stat_value10`, `ScalingStatDistribution`, `ScalingStatValue`, `dmg_min1`, `dmg_max1`, `dmg_type1`, `dmg_min2`, `dmg_max2`, `dmg_type2`, `armor`, `holy_res`, `fire_res`, `nature_res`, `frost_res`, `shadow_res`, `arcane_res`, `delay`, `ammo_type`, `RangedModRange`, `spellid_1`, `spelltrigger_1`, `spellcharges_1`, `spellppmRate_1`, `spellcooldown_1`, `spellcategory_1`, `spellcategorycooldown_1`, `spellid_2`, `spelltrigger_2`, `spellcharges_2`, `spellppmRate_2`, `spellcooldown_2`, `spellcategory_2`, `spellcategorycooldown_2`, `spellid_3`, `spelltrigger_3`, `spellcharges_3`, `spellppmRate_3`, `spellcooldown_3`, `spellcategory_3`, `spellcategorycooldown_3`, `spellid_4`, `spelltrigger_4`, `spellcharges_4`, `spellppmRate_4`, `spellcooldown_4`, `spellcategory_4`, `spellcategorycooldown_4`, `spellid_5`, `spelltrigger_5`, `spellcharges_5`, `spellppmRate_5`, `spellcooldown_5`, `spellcategory_5`, `spellcategorycooldown_5`, `bonding`, `description`, `PageText`, `LanguageID`, `PageMaterial`, `startquest`, `lockid`, `Material`, `sheath`, `RandomProperty`, `RandomSuffix`, `block`, `itemset`, `MaxDurability`, `area`, `Map`, `BagFamily`, `TotemCategory`, `socketColor_1`, `socketContent_1`, `socketColor_2`, `socketContent_2`, `socketColor_3`, `socketContent_3`, `socketBonus`, `GemProperties`, `RequiredDisenchantSkill`, `ArmorDamageModifier`, `Duration`, `ItemLimitCategory`, `HolidayId`, `ScriptName`, `DisenchantID`, `FoodType`, `minMoneyLoot`, `maxMoneyLoot`, `ExtraFlags`)
VALUES
   (33060, 3, 8, -1, 'Soulbound Test Gem', 43108, 3, 0, 0, 1, 120000, 30000, 0, -1, -1, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 0, 0, 0, 0, -1, 0, -1, 1, 'Matches a Red, Blue, or Yellow Socket.', 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 512, 0, 0, 0, 0, 0, 0, 0, 0, 681, -1, 0, 0, 0, 0, '', 0, 0, 0, 0, 0);

REPLACE INTO locales_item
   (`entry`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `name_loc7`, `name_loc8`, `description_loc1`, `description_loc2`, `description_loc3`, `description_loc4`, `description_loc5`, `description_loc6`, `description_loc7`, `description_loc8`)
VALUES
   (33060, '', '', '', '', '', '', '', 'Неотчуждаемый тестовый самоцвет', '', '', '', '', '', '', '', 'Для красного, минего или желтого гнезда.');
*/

DELETE FROM `locales_creature` WHERE `entry` = 88004;
INSERT INTO `locales_creature` (`entry`, `name_loc8`) VALUE 
(88004, 'Мурлок - помошник');

DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006700 AND 2000006709;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006700, 'I''d like to use my bank slot.', 'Я бы хотел воспользоваться своей банковской ячейкой.'),
(2000006701, 'I''d like to use the auction house .', 'Я бы хотел воспользоваться аукционом.'),
(2000006702, 'I''d like to see the stables.', 'Я бы хотел воспользоваться стойлами.'),
(2000006703, 'I want you show me the way to some place...', 'Я бы хотел, чтобы ты показал мне дорогу кое-куда...'),
(2000006704, 'I''d like to check my mailbox.', 'Я бы хотел проверить свою почту.'),

(2000006705, 'I''d like to use my bank slot.', 'Я бы хотела воспользоваться своей банковской ячейкой.'),
(2000006706, 'I''d like to use the auction house .', 'Я бы хотела воспользоваться аукционом.'),
(2000006707, 'I''d like to see the stables.', 'Я бы хотела воспользоваться стойлами.'),
(2000006708, 'I want you show me the way to some place...', 'Я бы хотела, чтобы ты показал мне дорогу кое-куда...'),
(2000006709, 'I''d like to check my mailbox.', 'Я бы хотела проверить свою почту.');

DELETE FROM `npc_text` WHERE `ID` BETWEEN 888004 AND 888008;
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(888004, 'Greetings, $N!$BWhat can I help you with?'),
(888005, 'Greetings, $N!$BSorry, but I feel that I can''t help you at the moment. Maybe you should put your deals in order?'),
(888006, 'Sorry, but my master forbid me to talk with strangers.'),
(888007, 'Where do you want to travel with me?'),
(888008, 'Sorry, but I am too tired since last journey...');

DELETE FROM `locales_npc_text` WHERE `entry` BETWEEN 888004 AND 888008;
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc8`) VALUES
(888004, 'Greetings, $N!$BWhat can I help you with?', 'Приветствую, $N!$BЧем я могу тебе помочь?'),
(888005, 'Greetings, $N!$BSorry, but I feel that I can''t help you at the moment. Maybe you should put your deals in order?', 'Приветствую, $N!$BИзвини, но я чувствую, что ничем не могу помочь тебе сейчас. Может, тебе стоит привести свои дела в порядок?'),
(888006, 'Sorry, but my master forbid me to talk with strangers.', 'Прости, но мой хозяин запретил мне разговаривать с незнакомцами.'),
(888007, 'Where do you want to travel with me?', 'Куда бы ты $gхотел:хотела; отправиться со мной?'),
(888008, 'Sorry, but I am too tired since last journey...', 'Извини, но я слишком устал после прошлого путешествия...');

DELETE FROM `mangos_string` WHERE `entry` IN (11015, 11016);
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11015, 'This feature is available only to Premium accounts.', 'Эта функция доступна только для Премиум аккаунтов.'),
(11016, 'This feature is available only to Premium accounts with Extended type.', 'Эта функция доступна только для Премиум аккаунтов Расширенного типа.');