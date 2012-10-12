UPDATE `gameobject_template` SET `data16` = 1 WHERE `entry` = 180418;

DELETE FROM `creature_battleground` WHERE `guid` IN (150383, 150390);
INSERT INTO `creature_battleground` VALUES
(150383, 3, 1),
(150390, 3, 3);