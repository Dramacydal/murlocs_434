DELETE FROM `gameobject` WHERE `map` = 617;
INSERT INTO `gameobject` VALUES
-- buffs
('702350','184663','617','1','1','1291.7','813.424','7.11472','4.64562','0','0','0','0','60','100','1'),
('702351','184664','617','1','1','1291.7','768.911','7.11472','1.55194','0','0','0','0','60','100','1'),
-- doors
('702352','192642','617','1','1','1350.95','817.2','21.4096','3.15','0','0','0','0','86400','100','1'),
('702353','192643','617','1','1','1232.65','764.913','21.4729','6.3','0','0','0.','0','86400','100','1'),
-- waterfalls
('702354','194395','617','1','1','1291.56','790.837','7.1','3.14238','0','0','0','0','120','100','1'),
('702355','191877','617','1','1','1291.56','790.837','7.1','3.14238','0','0','0','0','120','100','1');

DELETE FROM `creature` WHERE `map` = 617;
INSERT INTO `creature` VALUES
(702100, 28567, 617, 1, 1, 0, 0, 1358.977, 817.2882, 16.08718, 3.106686, 25, 0, 0, 1, 1, 0, 0),
(702101, 28567, 617, 1, 1, 0, 0, 1224.833, 765.3871, 16.09484, 0.0, 25, 0, 0, 1, 1, 0, 0),
(702102, 28567, 617, 1, 1, 0, 0, 1292.587, 790.2205, 7.19796, 3.054326, 25, 0, 0, 1, 1, 0, 0);

DELETE FROM `gameobject_battleground` WHERE `guid` BETWEEN 702350 AND 702355;
INSERT INTO `gameobject_battleground` VALUES
-- buffs
('702350','253','0'),
('702351','253','0'),
-- doors
('702352','254','0'),
('702353','254','0'),
-- waterfalls
('702354','6','0'),
('702355','5','0');

-- water sprouts
DELETE FROM `creature_battleground` WHERE `guid` BETWEEN 702100 AND 702102;
INSERT INTO `creature_battleground` VALUES
(702100, 7, 0),
(702101, 7, 0),
(702102, 7, 0);

DELETE FROM `battleground_events` WHERE `map` = 617;
INSERT INTO `battleground_events` VALUES
('617','253','0','buffs'),
('617','254','0','doors'),
('617','5','0','waterfall visual'),
('617','6','0','waterfall collision'),
('617','7','0','water sprouts');

-- doors
UPDATE `gameobject_template` set `faction` = 114, `flags` = 32, `size` = 1.5 WHERE `entry` IN (192642,192643);

-- Waterfall
UPDATE `gameobject_template` SET `size` = 1.25 WHERE `entry` IN (191877, 194395);

-- Water Spout
UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 0x02000000 WHERE `entry` = 28567;