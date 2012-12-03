-- Tricky Treat
DELETE FROM `creature_loot_template` WHERE `entry` = 23682 AND `item` = 33226;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`, `lootcondition`, `condition_value1`, `condition_value2`) VALUE
(23682, 33226, 100, 0, 20, 20, 0, 0, 0);

-- Stinking Up Southshore
-- ReqCreatureOrGOId1 = 15415, ReqCreatureOrGOCount1 = 3
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0 WHERE `entry` = 1657;

-- "Let the Fires Come!", Stop the Fires!
UPDATE `quest_template` SET `method` = 0 WHERE `entry` IN (11131, 11219, 12135, 12139);

DELETE FROM `gameobject_involvedrelation` WHERE `id` IN (190072, 190073);
INSERT INTO `gameobject_involvedrelation` VALUE
(190072, 12369),
(190073, 12370);

DELETE FROM `gameobject_questrelation` WHERE `id` IN (190072, 190073);
INSERT INTO `gameobject_questrelation` VALUE
(190072, 12369),
(190073, 12370);