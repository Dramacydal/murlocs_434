-- ----------------------------------------
-- mangosR2 EAI CleanUp for Quests && SD2 -
-- ----------------------------------------

DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 25316;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 26127;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 27483;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 27600;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 28610;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 28939;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 29199;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 29200;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 29204;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 29206;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 29219;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 32149;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 32257;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 36626;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 36678;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 29395;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30660;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30661;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30662;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30663;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30664;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30666;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30667;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30668;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 32191;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30695;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 31011;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 31079;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 32191;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 31118;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 29395;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 31513;


-- fixes for YTDB DB update
UPDATE creature_template SET ScriptName='' WHERE entry IN (35035, 35770, 35771, 35766);
UPDATE creature_template SET ScriptName='' WHERE entry=36549;
UPDATE `creature_template` SET `npcflag`=1, `scriptname`='npc_toc_announcer' WHERE `entry`=34816;  -- needed to be rerun
UPDATE `creature_template` SET `pickpocketloot` = 0 WHERE `entry` = 37444;
UPDATE `creature_template` SET `pickpocketloot` = 0 WHERE `entry` = 31818;
UPDATE `creature_template` SET `pickpocketloot` = 0 WHERE `entry` = 37283;
UPDATE `creature_template` SET `pickpocketloot` = 0 WHERE `entry` = 31818;
UPDATE `creature_template` SET `pickpocketloot` = 0 WHERE `entry` = 37283;
UPDATE `creature_template` SET `pickpocketloot` = 0 WHERE `entry` = 37444;
UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = 20159;


DELETE FROM `gameobject` WHERE `id`=190643;
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
(511000, 190643, 571, 1, 1, 5545.45, 5767.53, -77.8042, 5.39307, 0, 0, 0.959937, 0.280215, -25, 0, 1),
(47391, 190643, 571, 1, 1, 5547.61, 5767.75, -78.0231, 4.08966, 0, 0, 0.889734, -0.456479, -120, 100, 1);

-- EAI Text clean up for quests/sd2
DELETE FROM `creature_ai_texts` WHERE (`entry`='-1120') OR (`entry`='-1121') OR (`entry`='-1122') OR (`entry`='-1123') OR (`entry`='-1124') OR (`entry`='-1125') OR (`entry`='-1126') OR (`entry`='-1127') OR (`entry`='-1128');
DELETE FROM `creature_ai_texts` WHERE (`entry`='-555') OR (`entry`='-556') OR (`entry`='-557') OR (`entry`='-558') OR (`entry`='-559') OR (`entry`='-560') OR (`entry`='-561') OR (`entry`='-562') OR (`entry`='-563') OR (`entry`='-564') OR (`entry`='-565');
DELETE FROM `creature_ai_texts` WHERE (`entry`='-696') OR (`entry`='-697');
DELETE FROM `creature_ai_texts` WHERE (`entry`='-312491') OR (`entry`='-312492') OR (`entry`='-312493');
DELETE FROM `creature_ai_texts` WHERE  `entry`= -1137;

-- Official Sd2 Clean up
DELETE FROM scripted_event_id WHERE id = 9735;
UPDATE creature_template SET ScriptName='' WHERE entry IN (30679,32174);
UPDATE gameobject_template SET ScriptName='' WHERE entry = 193611;

DELETE FROM scripted_areatrigger WHERE entry=3626;	  	
INSERT INTO scripted_areatrigger VALUES (3626, 'at_vaelastrasz');
