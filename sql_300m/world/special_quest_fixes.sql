SET NAMES 'utf8';

-- ----------------------------------------
-- Quest 13343 ----------------------------
-- ----------------------------------------

-- Really fix quest: Mystery of the Infinite, Redux (http://wh.300murlocs.com/?quest=13343)
-- WARNING! Scripts support NEEDED!
UPDATE `creature_template` SET `AIName`='EventAI', `ScriptName`='' WHERE `entry`='32352';

DELETE FROM `creature_ai_scripts` WHERE `id` BETWEEN 3235201 AND 3235202;

INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
 (3235201, 32352, 0, 0, 100, 1, 5000, 8000, 11000, 15000, 11, 51020, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Infinite Timerender - Cast Time Lapse'),
 (3235202, 32352, 6, 0, 100, 0, 0, 0, 0, 0, 26, 13343, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

UPDATE creature_template SET ScriptName='npc_hourglass_redux', AIName = '' WHERE entry = 32327;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 32327;
UPDATE `creature_template` SET `unit_flags` = 516, `dynamicflags` = 8 WHERE `entry` = 32327; -- added root, disable movement, passive



-- ----------------------------------------
-- Quest's 10545, 10512 -------------------
-- ----------------------------------------

-- Really fix quest's: Bladespire Kegger (http://wh.300murlocs.com/?quest=10545) & Getting the Bladespire Tanked (http://wh.300murlocs.com/?quest=10512)
-- WARNING! Scripts support NEEDED!

-- UPDATE `creature_template` SET `ScriptName` = 'generic_creature' WHERE entry = 19995;
-- UPDATE `creature_template` SET `AIName`='EventAI', `ScriptName`='' WHERE entry = 21241;
UPDATE `creature_template` SET `ScriptName` = 'npc_ogre_brute' WHERE entry = 19995;
UPDATE `creature_template` SET `AIName`='', `ScriptName` = 'npc_bloodmaul_brutebane' WHERE entry = 21241;



-- ----------------------------------------
-- Quest 10584 ----------------------------
-- ----------------------------------------

-- Temp fix for quest: Picking Up Some Power Converters (http://wh.300murlocs.com/?quest=10584)
-- WARNING! Scripts support NEEDED!

-- UPDATE `gameobject_template` SET `ScriptName`='' WHERE `entry`='184906';
UPDATE `gameobject_template` SET `ScriptName`='go_power_converter' WHERE `entry`='184906';



-- ----------------------------------------
-- Quest's 10859, 10674 -------------------
-- ----------------------------------------

-- Fix for quest's: Gather the Orbs (http://wh.300murlocs.com/?quest=10859) and Trapping the Light Fantastic (http://wh.300murlocs.com/quest=10674)
-- WARNING! Scripts support NEEDED!

-- UPDATE `creature_template` SET `unit_flags` = 0, `dynamicflags` = 0, `MovementType`='1', `InhabitType`='3' WHERE `entry` IN ('22333', '21926');
UPDATE `creature_template` SET `unit_flags` = 33554948, `dynamicflags` = 8, `MovementType`='0', `InhabitType`='0'  WHERE `entry` IN ('22333', '21926');

UPDATE `creature_template` SET `ScriptName`='razaani_light_orb' WHERE `entry` IN ('20635', '20771');



-- ----------------------------------------
-- Quest 12832 ----------------------------
-- ----------------------------------------

-- Fix for quest: Bitter Departure (http://wh.300murlocs.com/?quest=12832)
-- WARNING! Scripts support NEEDED!

-- spawn data backup
-- DELETE FROM `creature` WHERE `id`=29434;
-- INSERT INTO `creature` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`modelid`,`equipment_id`,`position_x`,`position_y`,`position_z`,`orientation`,`spawntimesecs`,`spawndist`,`currentwaypoint`,`curhealth`,`curmana`,`DeathState`,`MovementType`) VALUES
-- (105017, 29434, 571, 1, 1, 0, 0, 6690.49, -1167.41, 398.915, 3.76991, 300, 0, 0, 9103, 0, 0, 0),
-- (105018, 29434, 571, 1, 1, 0, 0, 6646.89, -1110.96, 427.438, 3.22886, 300, 0, 0, 9103, 0, 0, 0);

DELETE FROM `creature` WHERE `id`= 29434;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUES
(105017, 29434, 571, 1, 1, 0, 254, 6629.44, -1251.64, 395.942, 5.38386, 25, 0, 0, 9416, 0, 0, 2);

UPDATE `creature_template` SET `MovementType` = '2', `ScriptName` = 'npc_injured_goblin' WHERE `entry`=29434;

DELETE FROM `script_texts` WHERE `entry` BETWEEN -1800001 AND -1800000;
INSERT INTO `script_texts` (`entry`, `content_default`, `content_loc1`, `content_loc2`, `content_loc3`, `content_loc4`, `content_loc5`, `content_loc6`, `content_loc7`, `content_loc8`, `sound`, `type`, `language`, `emote`, `comment`) VALUES 
('-1800000', 'Let me know when you''re ready. I''d prefer sooner than later... what with the slowly dying from poison and all. ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'injured goblin SAY_QUEST_START'), 
('-1800001', 'I''m going to bring the venom sack to Ricket... and then... you know... collapse. Thank you for helping me! ', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, '0', '0', '0', '0', 'injured goblin SAY_END_WP_REACHED');

DELETE FROM `script_waypoint` WHERE `entry` = 29434;
INSERT INTO `script_waypoint` (`entry`, `pointid`, `location_x`, `location_y`, `location_z`, `waittime`, `point_comment`) VALUES
(29434, 1, 6645.47, -1263.66, 396.938, 0, 0),
(29434, 2, 6662.06, -1274.13, 397.319, 0, 0),
(29434, 3, 6666.29, -1254.59, 396.11, 0, 0),
(29434, 4, 6669.33, -1240.06, 397.789, 0, 0),
(29434, 5, 6654.31, -1205.01, 399.823, 0, 0),
(29434, 6, 6657.03, -1170.15, 398.755, 0, 0),
(29434, 7, 6667.33, -1146.28, 399.014, 0, 0),
(29434, 8, 6670.9, -1138.68, 397.125, 0, 0),
(29434, 9, 6685.09, -1103.95, 396.828, 0, 0),
(29434, 10, 6688.71, -1094.32, 395.019, 0, 0),
(29434, 11, 6684.75, -1084.31, 396.916, 0, 0),
(29434, 12, 6679.3, -1074.4, 404.284, 0, 0),
(29434, 13, 6691.91, -1051.96, 407.983, 0, 0),
(29434, 14, 6696.28, -1016.54, 414.823, 0, 0),
(29434, 15, 6675.32, -1005.06, 414.844, 0, 0),
(29434, 16, 6661.01, -1007.41, 414.946, 0, 0),
(29434, 17, 6643.75, -1010.24, 420.508, 0, 0),
(29434, 18, 6620.76, -1012.59, 424.378, 0, 0),
(29434, 19, 6610.33, -996.992, 428.116, 0, 0),
(29434, 20, 6581.3, -1005.9, 434.257, 0, 0),
(29434, 21, 6570.74, -1013, 434.076, 0, 0),
(29434, 22, 6551.68, -1012.38, 433.218, 0, 0),
(29434, 23, 6530.83, -1024.99, 433.04, 0, 0),
(29434, 24, 6510.05, -1031.23, 435.824, 0, 0),
(29434, 25, 6491.5, -1032.46, 434.226, 0, 0),
(29434, 26, 6475.58, -1023.13, 434.759, 0, 0),
(29434, 27, 6451.81, -1025.43, 431.502, 10000, 0);



-- ----------------------------------------
-- support fo gameobject Wind Stone -------
-- ----------------------------------------
UPDATE `gameobject_template` SET `ScriptName`='go_wind_stone' WHERE `entry` IN (180456,180461,180466,180518,180529,180534,180539,180544,180549,180554,180559,180564);



-- ----------------------------------------
-- Quest 12843 ----------------------------
-- ----------------------------------------

-- Fix for quest: They Took Our Men! (http://wh.300murlocs.com/?quest=12843)
-- UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` = 29466;
-- UPDATE `gameobject_template` SET ScriptName = '' WHERE `entry` = 191544;

UPDATE `creature_template` SET `ScriptName` = 'npc_goblin_prisioner' WHERE `entry` = 29466;
UPDATE `gameobject_template` SET ScriptName = 'go_rusty_cage' WHERE `entry` = 191544;



-- ----------------------------------------
-- Quest 11587 ----------------------------
-- ----------------------------------------

-- Fix for quest: Prison Break (http://wh.300murlocs.com/?quest=11587)
-- UPDATE `gameobject_template` SET ScriptName = '' WHERE `entry` = 187561;
UPDATE `gameobject_template` SET `ScriptName`='go_arcane_prison' WHERE `entry` = 187561;



-- ----------------------------------------
-- Quest 25229 ----------------------------
-- ----------------------------------------

-- A Few Good Gnomes (http://wh.300murlocs.com/?quest=25229)
-- UPDATE `creature_template` SET `AIName`='', `ScriptName`='' WHERE `entry` = '39253';
-- UPDATE `creature_template` SET `AIName`='', `ScriptName`='' WHERE `entry` = '39466';
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_gnome_citizen' WHERE `entry` = '39253';
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_gnome_citizen_motivated' WHERE `entry` = '39466';
DELETE FROM `item_required_target` WHERE `entry` = '52566';
INSERT INTO `item_required_target` (`entry`, `type`, `targetEntry`) VALUES ('52566', '1', '39253');
-- UPDATE `creature_template` SET `modelid_1`='11686' WHERE `entry`='39466';
UPDATE `creature_template` SET `modelid_1`='2590' WHERE `entry`='39466';



-- ----------------------------------------
-- Quest 25229 & 13261---------------------
-- ----------------------------------------

-- Volatility (http://wh.300murlocs.com/?quest=13239) and Volatility daily (http://wh.300murlocs.com/?quest=13261)
-- UPDATE `creature_template` SET `AIName` = '', `ScriptName`='' WHERE `entry` = '31578';
-- UPDATE `creature_template` SET `AIName` = '', `ScriptName`='' WHERE `entry` = '31137';
UPDATE `creature_template` SET `AIName` = '', `ScriptName`='npc_armored_decoy' WHERE `entry` = '31578';
UPDATE `creature_template` SET `AIName` = '', `ScriptName`='npc_frostbrood_skytalon' WHERE `entry` = '31137';

