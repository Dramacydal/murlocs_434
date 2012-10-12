-- Pilgrims' Bounty
-- Now We're Cookin' (Horde)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (11123, 11124, 11125, 11126, 11127);
INSERT INTO `achievement_criteria_requirement` VALUES
(11123, 0, 0, 0),
(11124, 0, 0, 0),
(11125, 0, 0, 0),
(11126, 0, 0, 0),
(11127, 0, 0, 0);

-- Now We're Cookin' (Alliance)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (11118, 11119, 11120, 11121, 11122);
INSERT INTO `achievement_criteria_requirement` VALUES
(11118, 0, 0, 0),
(11119, 0, 0, 0),
(11120, 0, 0, 0),
(11121, 0, 0, 0),
(11122, 0, 0, 0);

-- Terokkar Turkey Time
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 11142;
INSERT INTO `achievement_criteria_requirement` VALUES
(11142, 5, 66303, 0),
(11142, 5, 66304, 0);

-- Spirit of Sharing (Alliance)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (11078, 11079, 11080, 11081);
INSERT INTO `achievement_criteria_requirement` VALUES
(11078, 6, 1657, 0),
(11079, 6, 1, 0),
(11080, 6, 3557, 0),
(11081, 6, 12, 0);

-- Spirit of Sharing (Horde)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (11082, 11083, 11084, 11085);
INSERT INTO `achievement_criteria_requirement` VALUES
(11082, 6, 14, 0),
(11083, 6, 3430, 0),
(11084, 6, 1638, 0),
(11085, 6, 1497, 0);

-- Pilgrim's Peril (for Alliance)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (11134, 11135, 11136, 11137);
INSERT INTO `achievement_criteria_requirement` VALUES
(11134, 6, 14, 0),
(11134, 5, 66303, 0),
(11135, 6, 3430, 0),
(11135, 5, 66303, 0),
(11136, 6, 1638, 0),
(11136, 5, 66303, 0),
(11137, 6, 1497, 0),
(11137, 5, 66303, 0);

-- Pilgrim's Peril (for Horde)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (11138, 11139, 11140, 11141);
INSERT INTO `achievement_criteria_requirement` VALUES
(11138, 6, 3557, 0),
(11138, 5, 66303, 0),
(11139, 6, 1657, 0),
(11139, 5, 66303, 0),
(11140, 6, 1, 0),
(11140, 5, 66303, 0),
(11141, 6, 12, 0),
(11141, 5, 66303, 0);

-- Turkey Lurkey
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` BETWEEN 11158 AND 11165;
INSERT INTO `achievement_criteria_requirement` VALUES
(11158, 2, 4, 1),
(11158, 16, 404, 0),
(11159, 2, 4, 4),
(11159, 16, 404, 0),
(11160, 2, 4, 2),
(11160, 16, 404, 0),
(11161, 2, 4, 8),
(11161, 16, 404, 0),
(11162, 2, 4, 5),
(11162, 16, 404, 0),
(11163, 2, 4, 10),
(11163, 16, 404, 0),
(11164, 2, 4, 3),
(11164, 16, 404, 0),
(11165, 2, 4, 7),
(11165, 16, 404, 0);

-- Bountiful Table
DELETE FROM `creature_template` WHERE `entry` = 32823;
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `modelid_3`, `modelid_4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `powerType`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `spell5`, `spell6`, `spell7`, `spell8`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `health_mod`, `power_mod`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `vehicle_id`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUE
(32823, 0, 0, 0, 0, 0, 28293, 0, 0, 0, 'Bountiful Table', '', '', 0, 1, 1, 1, 1, 0, 0, 0, 17, 35, 35, 0, 1, 1.14286, 1, 0, 1, 2, 0, 0, 1, 2000, 2000, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, '');

-- Wild Turkey
UPDATE `creature` SET `spawntimesecs` = 25 WHERE `id` = 32820;

-- Pilgrim Chairs
UPDATE `creature_template` SET `vehicle_id` = 321, `modelid_2` = 0, `unit_flags` = 0x4004, `speed_run`=1.57143 WHERE `entry` IN (34812, 34819, 34822, 34823, 34824);
UPDATE `creature_template` SET `spell1` = 66250, `spell2` = 61784, `spell3` = 61785, `spell4` = 61788, `spell5` = 61786, `spell6` = 61787 WHERE `entry` = 34812;
UPDATE `creature_template` SET `spell1` = 66259, `spell2` = 61784, `spell3` = 61785, `spell4` = 61788, `spell5` = 61786, `spell6` = 61787 WHERE `entry` = 34819;
UPDATE `creature_template` SET `spell1` = 66260, `spell2` = 61784, `spell3` = 61785, `spell4` = 61788, `spell5` = 61786, `spell6` = 61787 WHERE `entry` = 34822;
UPDATE `creature_template` SET `spell1` = 66261, `spell2` = 61784, `spell3` = 61785, `spell4` = 61788, `spell5` = 61786, `spell6` = 61787 WHERE `entry` = 34823;
UPDATE `creature_template` SET `spell1` = 66262, `spell2` = 61784, `spell3` = 61785, `spell4` = 61788, `spell5` = 61786, `spell6` = 61787 WHERE `entry` = 34824;
UPDATE `creature` SET `spawntimesecs` = 5 WHERE `id` IN (34812, 34819, 34822, 34823, 34824);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (34812, 34819, 34822, 34823, 34824);
INSERT INTO `npc_spellclick_spells` VALUES
(34824, 65403, 0, 0, 0, 1),
(34823, 65403, 0, 0, 0, 1),
(34822, 65403, 0, 0, 0, 1),
(34819, 65403, 0, 0, 0, 1),
(34812, 65403, 0, 0, 0, 1);

-- Bountiful Feast Hostess
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags`=33536 WHERE `entry`=34654;
-- Dokin Farplain
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags` = 33536 WHERE `entry`=34678;
-- Bountiful Feast Hostess
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `baseattacktime` = 2000, `unit_flags` = 33536 WHERE `entry`=34654;
-- Laha Farplain
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags` = 33536 WHERE `entry`=34684;
-- Bountiful Barrel
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `speed_run` = 1 WHERE `entry` = 35343;
-- Dalni Tallgrass
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `npcflag` = 640, `unit_flags` = 33536 WHERE `entry` = 34685;
-- Bountiful Barrel
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `speed_run` = 1 WHERE `entry` = 35342;
-- Ondani Greatmill
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags` = 33536 WHERE `entry` = 34713;
-- John Rigsdale
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `baseattacktime` = 2000, `npcflag` = 640, `unit_flags` = 33536 WHERE `entry` = 34787;
-- Alice Rigsdale
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags` = 33536 WHERE `entry` = 34786;
-- Miles Standish
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags` = 33536 WHERE `entry` = 34677;
-- Roberta Carter
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags` = 33536 WHERE `entry` = 34712; 
-- Rose Standish
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `unit_flags` = 33536 WHERE `entry` = 34683;
-- Bountiful Barrel
UPDATE `creature_template` SET `faction_A` = 714, `faction_H` = 714, `speed_run` = 1 WHERE `entry` = 35341; 

-- Bountiful Feast Hostess
UPDATE `creature_model_info` SET `bounding_radius` = 0.8725, `combat_reach` = 3.75, `gender` = 1 WHERE `modelid` = 29330;
-- Dokin Farplain
UPDATE `creature_model_info` SET `bounding_radius` = 0.9747, `combat_reach` = 4.05 WHERE `modelid` = 29339;
-- Bountiful Barrel
UPDATE `creature_model_info` SET `bounding_radius` = 1, `gender` = 0 WHERE `modelid` = 29775;
-- Laha Farplain
UPDATE `creature_model_info` SET `bounding_radius` = 0.8725, `combat_reach` = 3.75, `gender` = 1 WHERE `modelid` = 29342;
-- Chairs
-- UPDATE `creature_model_info` SET `bounding_radius` = 1.6, `combat_reach` = 0, `gender` = 2 WHERE `modelid` = 29205;
-- Dalni Tallgrass
UPDATE `creature_model_info` SET `bounding_radius` = 0.8725, `combat_reach`= 3.75, `gender` = 1 WHERE `modelid` = 29397;
-- Ondani Greatmill
UPDATE `creature_model_info` SET `bounding_radius` = 0.8725, `combat_reach` = 3.75, `gender` = 1 WHERE `modelid` = 29401;
-- John Rigsdale
UPDATE `creature_model_info` SET `bounding_radius` = 0.383 WHERE `modelid` = 29428;
-- Alice Rigsdale
UPDATE `creature_model_info` SET `bounding_radius` = 0.383, `gender` = 1 WHERE `modelid` = 29427;
-- Miles Standish
UPDATE `creature_model_info` SET `bounding_radius` = 0.383, `gender` = 0 WHERE `modelid` = 29338;
-- Roberta Carter
UPDATE `creature_model_info` SET `bounding_radius` = 0.383, `gender` = 1 WHERE `modelid` = 29365;
-- Rose Standish
UPDATE `creature_model_info` SET `bounding_radius` = 0.383, `gender` = 1 WHERE `modelid` = 29340;

DELETE FROM `creature_template_addon` WHERE `entry` IN (34654, 34678, 35343, 34684, 34819, 34824, 34823, 34812, 34822, 34685, 35342, 34713, 34787, 34786, 34677, 34712, 34683, 35341);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `auras`) VALUES
-- Bountiful Feast Hostess
(34654, 0, 0, 1, 1, 0, NULL), 
-- Dokin Farplain
(34678, 0, 0, 2, 2, 0, NULL),
-- Bountiful Barrel (TB)
(35343, 0, 0, 1, 1, 0, NULL),
-- Laha Farplain
(34684, 0, 0, 1, 1, 0, NULL),
-- Dalni Tallgrass
(34685, 0, 0, 1, 1, 0, NULL),
-- Bountiful Barrel (OG)
(35342, 0, 0, 1, 1, 0, NULL),
-- Ondani Greatmill
(34713, 0, 0, 2, 2, 0, NULL),
-- John Rigsdale
(34787, 0, 0, 1, 1, 0, NULL),
-- Alice Rigsdale
(34786, 0, 0, 2, 2, 0, NULL),
-- Miles Standish
(34677, 0, 0, 2, 2, 0, NULL),
-- Roberta Carter
(34712, 0, 0, 2, 2, 0, NULL),
-- Rose Standish
(34683, 0, 0, 1, 1, 0, NULL),
-- Bountiful Barrel
(35341, 0, 0, 1, 1, 0, NULL),
-- The Stuffing Chair (Stuffing Server)
(34819, 0, 0, 1, 1, 0, '61795'),
-- The Sweet Potato Chair (Sweet Potatoes Server)
(34824, 0, 0, 1, 1, 0, '61797'),
-- The Cranberry Chair (Cranberry Server)
(34823, 0, 0, 1, 1, 0, '61793'),
-- The Turkey Chair (Turkey Server)
(34812, 0, 0, 1, 1, 0, '61796'),
-- The Pie Chair (Pie Server)
(34822, 0, 0, 1, 1, 0, '61794');

-- Bountiful Table
UPDATE `creature_template` SET `unit_flags` = 33554432, `speed_run` = 1, `vehicle_id` = 320 WHERE `entry` = 32823;

-- Food Holder
UPDATE `creature_template` SET `unit_flags` = 33555200, `vehicle_id` = 323 WHERE `entry` = 32830;
-- Plate Holder
UPDATE `creature_template` SET `unit_flags` = 33555200, `vehicle_id` = 325 WHERE `entry` = 32840;

-- Sturdy Plate
UPDATE `creature_template` SET `unit_flags` = 33554432, `speed_run` = 1 WHERE `entry` = 32839;
-- [PH] Pilgrim's Bounty Table - Turkey
UPDATE `creature_template` SET `unit_flags` = 33554432 WHERE `entry` = 32824;
-- [PH] Pilgrim's Bounty Table - Cranberry Sauce
UPDATE `creature_template` SET `unit_flags` = 33554432 WHERE `entry` = 32827;
-- [PH] Pilgrim's Bounty Table - Stuffing
UPDATE `creature_template` SET `unit_flags` = 33554432 WHERE `entry` = 32831;
-- [PH] Pilgrim's Bounty Table - Yams
UPDATE `creature_template` SET `unit_flags` = 33554432 WHERE `entry` = 32825;
-- [PH] Pilgrim's Bounty Table - Pie
UPDATE `creature_template` SET `unit_flags` = 33554432 WHERE `entry` = 32829;

-- Bountiful Table
UPDATE `creature_model_info` SET `bounding_radius` = 1.8, `combat_reach` = 0, `gender` = 2 WHERE `modelid` = 28293;
-- Sturdy Plate
UPDATE `creature_model_info` SET `bounding_radius` = 1.75, `combat_reach` = 0, `gender` = 2 WHERE `modelid` = 28310;
-- [PH] Pilgrim's Bounty Table - X
UPDATE `creature_model_info` SET `bounding_radius` = 0.125, `combat_reach` = 0.25, `gender` = 2 WHERE `modelid` = 17188;

DELETE FROM `creature_template_addon` WHERE `entry` IN (32830,32840,32839,32824,32827,32831,32829,32825);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `auras`) VALUES
-- Food Holder
(32830, 0, 0, 1, 1, 0, NULL),
-- Plate Holder
(32840, 0, 0, 1, 1, 0, NULL),
-- Sturdy Plate
(32839, 0, 0, 1, 1, 0, NULL),
-- [PH] Pilgrim's Bounty Table - Turkey
(32824, 0, 0, 1, 1, 0, NULL),
-- [PH] Pilgrim's Bounty Table - Cranberry Sauce
(32827, 0, 0, 1, 1, 0, NULL),
-- [PH] Pilgrim's Bounty Table - Stuffing
(32831, 0, 0, 1, 1, 0, NULL),
-- [PH] Pilgrim's Bounty Table - Yams
(32825, 0, 0, 1, 1, 0, NULL),
-- [PH] Pilgrim's Bounty Table - Pie
(32829, 0, 0, 1, 1, 0, NULL);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (32823, 32830, 32840);
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `cast_flags`) VALUES
-- Bountiful Table (Ride Vehicle Hardcoded)
(32823, 46598, 1),
-- Food Holder (Ride Vehicle Hardcoded)
(32830, 46598, 1),
-- Plate Holder (Ride Vehicle Hardcoded)
(32840, 46598, 1);

DELETE FROM `vehicle_accessory` WHERE `entry` IN (32823, 32830, 32840);
INSERT INTO `vehicle_accessory` (`entry`, `accessory_entry`, `seat_id`, `minion`, `description`) VALUES
-- (32823, 34812, 0, 1, 'Bountiful Table - The Turkey Chair'),
-- (32823, 34823, 1, 1, 'Bountiful Table - The Cranberry Chair'),
-- (32823, 34819, 2, 1, 'Bountiful Table - The Stuffing Chair'),
-- (32823, 34824, 3, 1, 'Bountiful Table - The Sweet Potato Chair'),
-- (32823, 34822, 4, 1, 'Bountiful Table - The Pie Chair'),
(32823, 32830, 5, 1, 'Bountiful Table - Food Holder'),
(32823, 32840, 6, 1, 'Bountiful Table - Plate Holder'),
--
(32830, 32824, 0, 1, 'Food Holder - [PH] Pilgrim''s Bounty Table - Turkey'),
(32830, 32827, 1, 1, 'Food Holder - [PH] Pilgrim''s Bounty Table - Cranberry Sauce'),
(32830, 32831, 2, 1, 'Food Holder - [PH] Pilgrim''s Bounty Table - Stuffing'),
(32830, 32825, 3, 1, 'Food Holder - [PH] Pilgrim''s Bounty Table - Yams'),
(32830, 32829, 4, 1, 'Food Holder - [PH] Pilgrim''s Bounty Table - Pie'),
--
(32840, 32839, 0, 1, 'Plate Holder - Sturdy Plate'),
(32840, 32839, 1, 1, 'Plate Holder - Sturdy Plate'),
(32840, 32839, 2, 1, 'Plate Holder - Sturdy Plate'),
(32840, 32839, 3, 1, 'Plate Holder - Sturdy Plate'),
(32840, 32839, 4, 1, 'Plate Holder - Sturdy Plate');