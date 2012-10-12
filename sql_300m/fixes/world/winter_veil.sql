-- The Abominable Greench
UPDATE `creature` SET `spawntimesecs` = 25 WHERE `id` = 13602;
UPDATE `creature_template` SET `rank` = 0 WHERE `entry` = 13602;

-- Winter Reveler AI Script
UPDATE `creature_template` SET `ScriptName` = 'npc_winter_reveler' WHERE `name` LIKE "%Winter Reveler%";

-- Fel Cannonball Stack
UPDATE `gameobject` SET `spawntimesecs` = 25 WHERE `id` = 185861;

-- An Apexis Relic, The Relic's Emanation
UPDATE `quest_template` SET `Method` = 0 WHERE `entry` IN (11058, 11080);

-- Our Boy Wants To Be A Skyguard Ranger
UPDATE `quest_template` SET `PrevQuestId` = 11058 WHERE `entry` = 11030;

-- Quest with a gift providing Red Rider Air Rifle
DELETE FROM `gameobject_questrelation` WHERE `id` = 187236 AND `quest` = 13966;
INSERT INTO `gameobject_questrelation` (`id`, `quest`) VALUE 
(187236, 13966);
DELETE FROM `gameobject_involvedrelation` WHERE `id` = 187236 AND `quest` = 13966;
INSERT INTO `gameobject_involvedrelation` (`id`, `quest`) VALUE 
(187236, 13966);

-- BB King
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (12663, 12664, 12665, 12666, 12667,
12658, 12659, 12660, 12661, 12662);
INSERT INTO `achievement_criteria_requirement` (criteria_id, type, value1, value2) VALUES
(12663, 16, 141, 0),
(12663, 1, 29611, 0),

(12664, 16, 141, 0),
(12664, 1, 2784, 0),

(12665, 16, 141, 0),
(12665, 1, 7937, 0),

(12666, 16, 141, 0),
(12666, 1, 7999, 0),

(12667, 16, 141, 0),
(12667, 1, 17468, 0),

(12658, 16, 141, 0),
(12658, 1, 4949, 0),

(12659, 16, 141, 0),
(12659, 1, 10540, 0),

(12660, 16, 141, 0),
(12660, 1, 16802, 0),

(12661, 16, 141, 0),
(12661, 1, 10181, 0),

(12662, 16, 141, 0),
(12662, 1, 3057, 0);

-- Wulmort Jinglepocket
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35 WHERE `entry` = 13433;

-- 'Tis the Season
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 4230;
INSERT INTO `achievement_criteria_requirement` VALUE
(4230, 5, 55000, 0);

-- Crashin Trashin Racer
-- old 25332
UPDATE `creature_template` SET `modelid_1` = 27829, `spell1` = 49297, `speed_walk` = 1.1, `speed_run` = 1.1 WHERE `entry` IN (27664, 40281);
UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 0x2000000, `modelid_1` = 23767, `modelid_2` = 23767 WHERE `entry` = 27674;
UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 0x2000000 WHERE `entry` = 27697;

-- Crashin' & Thrashin'
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 4090;
INSERT INTO `achievement_criteria_requirement` VALUES
(4090, 16, 141, 0);