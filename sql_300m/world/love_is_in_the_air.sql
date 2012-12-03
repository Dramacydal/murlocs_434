-- TODO: update 4.x

-- Handful of Rose Petals on Jeremiah Payson
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 4227;
INSERT INTO `achievement_criteria_requirement` VALUES
(4227, 1, 8403, 0),
(4227, 5, 26682, 0),
(4227, 15, 3, 0);

-- Kissed Jeremiah Payson
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 3929;
INSERT INTO `achievement_criteria_requirement` VALUES
(3929, 1, 8403, 0),
(3929, 5, 26682, 0),
(3929, 15, 3, 0);

-- Handful of Rose Petals on Sraaz
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 12859;
INSERT INTO `achievement_criteria_requirement` VALUES
(12859, 1, 9099, 0),
(12859, 5, 26682, 0),
(12859, 15, 3, 0);

-- Kissed Sraaz
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 3931;
INSERT INTO `achievement_criteria_requirement` VALUES
(3931, 1, 9099, 0),
(3931, 5, 26682, 0),
(3931, 15, 3, 0);

-- Buttermilk Delight at Romantic Picnic
-- Romantic Picnic
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (5787, 4071);
INSERT INTO `achievement_criteria_requirement` VALUES
(5787, 16, 335, 0),
(5787, 6, 4613, 0),
(4071, 16, 335, 0),
(4071, 6, 4613, 0);

-- Romantic Basket
-- backup UPDATE gameobject_template SET type = 10, data3 = 1000, data10 = 45102 WHERE entry = 187267;
UPDATE `gameobject_template` SET `type` = 22, `data0` = 45123, `data3` = 0, `data10` = 0 WHERE `entry` = 187267;

-- Pilfering Perfume
UPDATE `quest_template` SET `Method` = 0 WHERE `entry` IN (24541, 24656);

-- Hot On The Trail
UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOId2` = 0, `ReqCreatureOrGOId3` = 0, `ReqCreatureOrGOCount1` = 0, `ReqCreatureOrGOCount2` = 0, `ReqCreatureOrGOCount3` = 0 WHERE `entry` IN (24849, 24851); 

-- Crushing the Crown spawns
UPDATE `creature` SET `spawntimesecs` = 15 WHERE `id` IN (37214, 37917, 37984, 38006, 38016, 38023, 38030, 38032, 38035);
UPDATE `creature_template` SET `modelid_2` = 0 WHERE `entry` = 38035;

-- Add perfumes to innkeepers
DELETE FROM `npc_vendor` WHERE `item` IN (21829, 21833);
INSERT INTO `npc_vendor` (`entry`, `item`) VALUES
-- Orgrimmar
(6929, 21829), (6929, 21833),
-- Thunder Bluff
(6746, 21829), (6746, 21833),
-- Undercity
(6741, 21829), (6741, 21833),
-- Silvermoon
(16618, 21829), (16618, 21833),
-- Stormwind
(6740, 21829), (6740, 21833),
-- Ironforge
(5111, 21829), (5111, 21833),
-- Darnassus
(6735, 21829), (6735, 21833),
-- The Exodar
(16739, 21829), (16739, 21833);

-- Lovely Charm Collector's Kit
UPDATE `item_template` SET `unk0` = -1, `Flags` = 0, `spellcharges_1` = -1, `spellcooldown_1` = -1, `spellcategorycooldown_1` = -1, `spelltrigger_1` = 5, `bonding` = 0, `maxcount` = 1, `Material` = 4 WHERE `entry` = 49661;

-- Buycounts
UPDATE `item_template` SET `BuyCount` = 5 WHERE `entry` IN (49859, 49857, 49858, 49861, 49856, 49860, 22218, 34258, 50163, 22200);

-- I Pitied The Fool
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (6343, 6344, 6345, 6346, 6347);
INSERT INTO `achievement_criteria_requirement` VALUES
(6343, 6, 4197, 0),
(6343, 1, 16111, 0),
(6344, 6, 2177, 0),
(6344, 1, 16111, 0),
(6345, 6, 3421, 0),
(6345, 1, 16111, 0),
(6346, 6, 4100, 0),
(6346, 1, 16111, 0),
(6347, 6, 3456, 0),
(6347, 1, 16111, 0);

UPDATE `creature_template` SET `ScriptName` = 'boss_hummel' WHERE `entry` = 36296;
UPDATE `quest_template` SET `CompleteScript` = 0 WHERE `entry` = 14488;


update creature_template set faction_A = 35,faction_H=35 where entry in (36296, 36272, 36565);