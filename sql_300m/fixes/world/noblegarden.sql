UPDATE `creature_template` SET `ScriptName` = 'npc_spring_rabbit' WHERE `entry` = 32791;
UPDATE `gameobject_template` SET `ScriptName` = 'go_spring_egg' WHERE `entry` IN (113768, 113769, 113770, 113771, 113772);

-- Achievement bugs
-- Spring Fling (Horde)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (9202, 9203, 9204, 9205);
INSERT INTO `achievement_criteria_requirement` VALUES
(9202, 6, 222, 0),
(9203, 6, 159, 0),
(9204, 6, 3665, 0),
(9205, 6, 362, 0);
-- Spring Fling (Alliance)
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (9199, 9200, 9201, 9121);
INSERT INTO `achievement_criteria_requirement` VALUES
(9121, 6, 3576, 0),
(9199, 6, 186, 0),
(9200, 6, 87, 0),
(9201, 6, 131, 0);

-- Hard Boiled
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 9118;
INSERT INTO `achievement_criteria_requirement` VALUES
-- (9118, 5, 61734, 0),
(9118, 6, 543, 0),
(9118, 16, 181, 0);

-- Noble Garden
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (9122, 9123);
INSERT INTO `achievement_criteria_requirement` VALUES
(9122, 6, 3487, 0),
(9123, 6, 1519, 0);

-- Eggs without event binding
DELETE FROM `game_event_gameobject` WHERE `guid` IN (25778, 25779, 25780, 25781, 25782, 25783, 25784);
INSERT INTO `game_event_gameobject` (`guid`, `event`) VALUES
(25778, 9),
(25779, 9),
(25780, 9),
(25781, 9),
(25782, 9),
(25783, 9),
(25784, 9);

-- Missing quests
DELETE FROM `creature_questrelation` WHERE `quest` IN (13483, 13484);
INSERT INTO `creature_questrelation` VALUES
-- Alliance Commoners
(18927, 13484), (19148, 13484), (19171, 13484), (19172, 13484), (19173, 13484),
-- Horde Commoners
(19169, 13483), (19175, 13483), (19176, 13483), (19177, 13483), (19178, 13483),
-- Goblin
(20102, 13483), (20102, 13484);