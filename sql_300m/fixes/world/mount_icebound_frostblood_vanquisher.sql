-- Icebound Frostbrood Vanquisher
DELETE FROM `creature_template` WHERE `entry` IN (38695);
INSERT INTO `creature_template` (`entry`, `name`, `modelid_1`,`faction_A`,`faction_H`,`unit_class`) VALUES
(38695, 'Icebound Frostbrood Vanquisher', 31155, 35, 35, 1);

DELETE FROM `creature_model_info` WHERE `modelid` = 31155;
INSERT INTO `creature_model_info` (`modelid`) VALUE (31155);

DELETE FROM `locales_creature` WHERE `entry` = 38695;
INSERT INTO `locales_creature` (`entry`, `name_loc8`) VALUE
('38695', 'Неуклонный ледяной покоритель');