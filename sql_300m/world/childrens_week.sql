-- Missing starting quests
DELETE FROM `creature_questrelation` WHERE `id` = 14451 AND `quest` = 172;
DELETE FROM `creature_questrelation` WHERE `id` = 14450 AND `quest` = 1468;
INSERT INTO `creature_questrelation` VALUES
(14451, 172),
(14450, 1468);

-- Dark Portal
DELETE FROM `scripted_areatrigger` WHERE `entry` = 4356;
INSERT INTO `scripted_areatrigger` VALUE
(4356, 'at_childrens_week_spot');

-- Auchindoun and the Ring of Observance
-- A Trip to the Dark Portal (H)
-- A Trip to the Dark Portal (A)
UPDATE `quest_template` SET `SpecialFlags` = 2 WHERE `entry` IN (10952, 10951, 10950);

UPDATE `creature_template` SET `ScriptName` = 'npc_child_event' WHERE `entry` IN (14305, 14444, 22817, 22818, 33532, 33533);

-- Back To The Orphanage
UPDATE `quest_template` SET `RewMailDelaySecs` = 60 WHERE `entry` IN (13959, 13960);

-- Home Alone
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 10391;
INSERT INTO `achievement_criteria_requirement` VALUE
(10391, 16, 201, 0);

-- Hail to the King, Baby
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 6659;
INSERT INTO `achievement_criteria_requirement` VALUE
(6659, 16, 201, 0);