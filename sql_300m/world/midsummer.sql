-- DELETE FROM `character_queststatus` WHERE `quest` = 11972;

-- zoram strand and quest npc's
UPDATE `creature` SET `spawntimesecs` = 25 WHERE `guid` IN 
(84758, 84789, 82191, 58064,
78657, 78662, 78658, 78659, 78661, 92537);

UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35, `scriptname` = 'npc_ice_caller' WHERE `entry` = 25949;

-- ahune heroic
UPDATE `creature_template` SET `minhealth` = 748000, `maxhealth` = 748000, `minlevel` = 85, `maxlevel` = 85, `armor` = 15000, `ScriptName` = 'generic_creature'
WHERE `entry` = 26338;

-- Ahune's loot
DELETE FROM `creature_loot_template` WHERE `entry` = 25740;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`, `lootcondition`, `condition_value1`, `condition_value2`) VALUES
(25740, 34955, 2, 0, 1, 1, 0, 0, 0),
(25740, 53641, 2, 0, 1, 1, 0, 0, 0),
(25740, 35498, 7, 0, 1, 1, 0, 0, 0),
(25740, 35557, 30, 0, 2, 2, 0, 0, 0),
(25740, 35720, 5, 0, 1, 6, 0, 0, 0),
(25740, 35723, 45, 0, 1, 1, 0, 0, 0),
(25740, 54801, 0, 1, 1, 1, 0, 0, 0),
(25740, 54802, 0, 1, 1, 1, 0, 0, 0),
(25740, 54803, 0, 1, 1, 1, 0, 0, 0),
(25740, 54804, 0, 1, 1, 1, 0, 0, 0),
(25740, 54805, 0, 1, 1, 1, 0, 0, 0),
(25740, 54806, 0, 1, 1, 1, 0, 0, 0);

UPDATE `creature_template` SET `lootid` = 25740 WHERE `entry` IN (25740, 26338);

--
DELETE FROM `creature_questrelation` WHERE `quest` = 11976;

UPDATE `gameobject_template` SET `data3` = 25000 WHERE `entry` IN (
181561, -- Flame of Ashenvale
181563, -- Flame of Darkshore
181560, -- Flame of the Barrens
181562, -- Flame of Stonetalon
181565, -- Flame of Westfall
181567, -- Flame of Wetlands
181564, -- Flame of Silverpine
181566); -- Flame of Hillsbrad

UPDATE `gameobject_template` SET `data10` = 45406 WHERE `entry` = 181605;
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 6804;
INSERT INTO `achievement_criteria_requirement` VALUES
(6804, 5, 58933, 0);

-- Torch Tossing
UPDATE `quest_template` SET `SpecialFlags` = 0, `ReqCreatureOrGOId1` = 30771, `ReqCreatureOrGOCount1` = 8 WHERE `entry` IN (11731, 11922);
UPDATE `quest_template` SET `SpecialFlags` = 0, `ReqCreatureOrGOId1` = 30771, `ReqCreatureOrGOCount1` = 20 WHERE `entry` IN (11921, 11926);

DELETE FROM `spell_script_target` WHERE `entry` IN (45732, 46054);
INSERT INTO `spell_script_target` VALUES
(45732, 0, 187708),
(46054, 0, 187708);

-- Torch Catching
DELETE FROM `spell_script_target` WHERE `entry` = 46747;
-- INSERT INTO `spell_script_target` VALUES
-- (46747, 1, 25218);

UPDATE `quest_template` SET `SpecialFlags` = 0, `ReqCreatureOrGOId1` = 25218, `ReqCreatureOrGOCount1` = 4 WHERE `entry` IN (11657, 11923);
UPDATE `quest_template` SET `SpecialFlags` = 0, `ReqCreatureOrGOId1` = 25218, `ReqCreatureOrGOCount1` = 10 WHERE `entry` IN (11924, 11925);

-- Torch Juggler
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 6937;
INSERT INTO `achievement_criteria_requirement` VALUES
(6937, 6, 4613, 0),
(6937, 16, 341, 0);