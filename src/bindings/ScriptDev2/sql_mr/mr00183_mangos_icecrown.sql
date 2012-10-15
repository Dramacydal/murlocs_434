-- --------
-- Entrance
-- --------
UPDATE `areatrigger_teleport` SET `required_level` = '80' WHERE `areatrigger_teleport`.`id` =5670;

UPDATE `creature` SET `spawntimesecs` = 7200 WHERE `map` = 631 AND (`spawntimesecs` BETWEEN 200 AND 7100 );

DELETE FROM `gameobject` WHERE `guid` = 913334;
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(913334, 202244, 631, 3, 1, -209.5, 2211.91, 199.97, 3.07661, 0, 0, 0.999472, 0.0324833, 0, 0, 1);

UPDATE `gameobject_template` SET `flags` = 0, `ScriptName` = 'go_icecrown_teleporter' WHERE `entry`  IN (202242,202243,202244,202245,202235,202223,202246);
UPDATE `gameobject` SET `phaseMask` = 1 WHERE `id`  IN (202242,202243,202244,202245,202235,202223,202246);
DELETE FROM `areatrigger_teleport` WHERE `id` = 5718 AND `target_map` = 631;

DELETE FROM `creature` WHERE `id` = 99322;
DELETE FROM `creature_template` WHERE `entry` = 99322;
DELETE FROM `locales_npc_text` WHERE `entry` = 99322;
DELETE FROM `npc_text` WHERE `ID` = 99322;
DELETE FROM `locales_npc_text` WHERE `entry` = 99323;
DELETE FROM `npc_text` WHERE `ID` = 99323;
DELETE FROM `gameobject` WHERE `guid` IN (913334);

UPDATE `instance_template` SET `ScriptName`='instance_icecrown_spire' WHERE `map`=631;

-- fix for ytdb data on frostwing door/puddle

DELETE FROM `gameobject` WHERE `id`=201919;
INSERT INTO `gameobject` (`guid`,`id`,`map`,`spawnMask`,`phaseMask`,`position_x`,`position_y`,`position_z`,`orientation`,`rotation0`,`rotation1`,`rotation2`,`rotation3`,`spawntimesecs`,`animprogress`,`state`) VALUES
(7807, 201919, 631, 1, 128, 4356.52, 2651.74, 351.1, 1.61378, 0, 0, 0.722138, 0.691749, 25, 255, 1);

-- --------
-- Saurfang
-- --------

UPDATE `creature_template` SET `vehicle_id` = 639, `AIName`='', `PowerType` = 3, `ScriptName`='boss_deathbringer_saurfang' WHERE `entry`=37813;
UPDATE `creature_template` SET `vehicle_id` = 639, `AIName`='', `PowerType` = 3 WHERE `entry` IN (38402,38582,38583);
UPDATE `creature` SET `position_x` = -476.621,`position_y` = 2211.11,`position_z` = 541.197, `spawntimesecs` = 604800 WHERE `id` = 37813;
UPDATE `creature_template` SET `ScriptName`='mob_blood_beast', `AIName`='' WHERE `entry`= 38508;
DELETE FROM `spell_script_target` WHERE `entry` IN (72260, 72202, 72278,72279,72280);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
('72260', '1', '37813'),
('72278', '1', '37813'),
('72279', '1', '37813'),
('72280', '1', '37813'),
('72202', '1', '37813');

DELETE FROM `spell_proc_event` WHERE entry IN (72176,72178, 72256);
INSERT INTO `spell_proc_event` VALUES
(72256, 0x7F,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0),
(72178, 0x7F,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0),
(72176, 0x7F,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0);

-- ------------
-- Deathwhisper
-- ------------

UPDATE `creature_template` SET `ScriptName`='boss_lady_deathwhisper', `AIName`='' WHERE `entry`=36855;
UPDATE `creature_template` SET `faction_A`=14, `faction_H`=14,`ScriptName`='mob_vengeful_shade', `AIName`='' WHERE `entry`= 38222;
UPDATE `creature_template` SET `ScriptName`='mob_cult_adherent', `AIName`='' WHERE `entry`= 37949;
UPDATE `creature_template` SET `ScriptName`='mob_cult_fanatic', `AIName`='' WHERE `entry`= 37890;

-- ---------
-- Marrowgar
-- ---------

UPDATE `creature_template` SET `ScriptName`='boss_lord_marrowgar', `AIName`='' WHERE `entry`= 36612;
UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201910,201911);
UPDATE `gameobject` SET `state` = '1' WHERE `guid` IN (72526,72525);
UPDATE `creature_template` SET `ScriptName`='mob_coldflame', `minlevel` = 82, `maxlevel` = 82, `modelid_1` = 11686, `modelid_2` = 11686, `modelid_3` = 11686, `modelid_4` = 11686, `faction_A` = 14, `faction_H` = 14, `AIName`=''  WHERE `entry`= 36672;
UPDATE `creature_template` SET `ScriptName`='mob_bone_spike', `AIName`='' WHERE `entry`= 38711;

-- --------------
-- Gunship battle
-- --------------

UPDATE `creature_template` SET `ScriptName`='mob_spire_frostwyrm', `AIName`='' WHERE `entry`= 37230;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 37230;
UPDATE `creature_template` SET `ScriptName`='mob_frost_giant', `AIName`='' WHERE `entry` IN (38490, 38494) ;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (38490, 38494);
DELETE FROM `creature` WHERE `guid` = 94094 AND `id` = 38490;

-- -------------------------
-- Gunship armory (override)
-- -------------------------
DELETE FROM `gameobject` WHERE `id` IN (201872,201873,201874,201875,202177,202178,202179,202180);
UPDATE `gameobject_template` SET `flags` = 0 WHERE `gameobject_template`.`entry` IN (201872,201873,201874,201875,202177,202178,202179,202180);
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(972541, 201872, 631, 1, 1, -428.141, 2421.34, 191.233, 3.10389, 0, 0, 0.999822, 0.0188489, -604800, 100, 1),
(972543, 201873, 631, 2, 1, -428.141, 2421.34, 191.233, 3.10389, 0, 0, 0.999822, 0.0188489, -604800, 100, 1),
(972545, 201874, 631, 4, 1, -428.141, 2421.34, 191.233, 3.10389, 0, 0, 0.999822, 0.0188489, -604800, 100, 1),
(972547, 201875, 631, 8, 1, -428.141, 2421.34, 191.233, 3.10389, 0, 0, 0.999822, 0.0188489, -604800, 100, 1),
(972551, 202177, 631, 1, 1, -447.493, 2003.5, 191.235, 0.153939, 0, 0, 0.0768933, 0.997039, -604800, 100, 1),
(972553, 202178, 631, 2, 1, -447.493, 2003.5, 191.235, 0.153939, 0, 0, 0.0768933, 0.997039, -604800, 100, 1),
(972555, 202179, 631, 4, 1, -447.493, 2003.5, 191.235, 0.153939, 0, 0, 0.0768933, 0.997039, -604800, 100, 1),
(972557, 202180, 631, 8, 1, -447.493, 2003.5, 191.235, 0.153939, 0, 0, 0.0768933, 0.997039, -604800, 100, 1);

-- -----------
-- Plague wing
-- -----------

-- -------
-- Rotface
-- -------

UPDATE `creature_template` SET `ScriptName`='boss_rotface', `AIName`=''  WHERE `entry`= 36627;
UPDATE `gameobject_template` SET `faction` = '114' WHERE `gameobject_template`.`entry` IN (201370);
UPDATE `gameobject` SET `state` = '0' WHERE `id` IN (201370);
UPDATE `creature_template` SET `ScriptName`='mob_small_ooze', `AIName`='' WHERE `entry`= 36897;
UPDATE `creature_template` SET `ScriptName`='mob_big_ooze', `AIName`='' WHERE `entry`= 36899;
UPDATE `creature_template` SET `ScriptName`='mob_ooze_spray_stalker', `AIName`='' WHERE `entry`= 37986;
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 14, `faction_H` = 14, `ScriptName`='mob_sticky_ooze', `AIName`='' WHERE `entry`= 37006;
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 14, `faction_H` = 14, `ScriptName`='', `AIName`='' WHERE `entry` IN (37013);
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 14, `faction_H` = 14, `ScriptName`='mob_ooze_explode_stalker', `AIName`='' WHERE `entry` = 38107;
DELETE FROM `spell_script_target` WHERE `entry` = 69783;
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES ('69783', '1', '37013');
DELETE FROM `spell_script_target` WHERE `entry` = 69508;
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES ('69508', '1', '37986');

-- fix rotface combat reach was set to 40 he could hit u from across the room
DELETE FROM `creature_model_info` WHERE (`modelid`=31005);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31005, 5, 4, 2, 0, 0);

-- fix stinky modelID info
DELETE FROM `creature_model_info` WHERE (`modelid`=30483);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (30483, 5, 1, 2, 0, 0);

-- ---------
-- Festergut
-- ---------

UPDATE `creature_template` SET `ScriptName`='boss_festergut', `AIName`=''  WHERE `entry`= 36626;
UPDATE `gameobject_template` SET `faction` = '114' WHERE `gameobject_template`.`entry` IN (201371);
UPDATE `gameobject` SET `state` = '0' WHERE `id` IN (201371);
UPDATE `creature_template` SET `ScriptName`='mob_vile_gas_stalker', `AIName`=''  WHERE `entry`= 38548;
UPDATE `creature_template` SET `faction_A` = 14, `faction_H` = 14,  `ScriptName`='mob_orange_gas_stalker', `AIName`='' WHERE `entry`= 36659;
-- original auras from YTDB
-- INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `auras`) VALUES
-- (36659, 0, 0, 1, 0, 0, '69126 69152 69154');
-- DELETE FROM `creature_template_addon` WHERE `entry` = 36659;
-- Visual effect mobs from Timmit
UPDATE `creature` SET `spawnMask` = 15, `modelid` = 11686, `spawntimesecs` = 300 WHERE `id`=37013;

-- fixed modelID info
DELETE FROM `creature_model_info` WHERE (`modelid`=31006);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31006, 5, 4, 2, 0, 0);

-- fix peroicus modelID info
DELETE FROM `creature_model_info` WHERE (`modelid`=30483);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (30483, 5, 1, 2, 0, 0);

-- -------------------
-- Professor putricide
-- -------------------

UPDATE `creature_template` SET `ScriptName`='boss_professor_putricide', `AIName`='' WHERE `entry`= 36678;
UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201372,201614,201613, 201612);
UPDATE `gameobject` SET `state` = '1' WHERE `id` IN (201612,201614,201613);
UPDATE `gameobject` SET `state` = '0' WHERE `id` IN (201372);
UPDATE `creature_template` SET `ScriptName`='mob_icc_gas_cloud', `AIName`='' WHERE `entry`= 37562;
UPDATE `creature_template` SET `ScriptName`='mob_icc_volatile_ooze', `AIName`='' WHERE `entry`= 37697;
UPDATE `creature_template` SET `ScriptName`='mob_choking_gas_bomb', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14, `scale` = 0.5 WHERE `entry`= 38159;
UPDATE `creature_template` SET `ScriptName`='mob_ooze_puddle',`scale` = '1.0', `AIName`='', `minlevel` = 82, `maxlevel` = 82, `modelid_1` = 11686, `modelid_2` = 11686, `modelid_3` = 11686, `modelid_4` = 11686, `faction_A` = 14, `faction_H` = 14  WHERE `entry`= 37690;
UPDATE `gameobject_template` SET `faction` = '0', `ScriptName` = 'go_plague_sigil' WHERE `gameobject_template`.`entry` IN (202182);

-- -----------
-- Abomination
-- -----------

DELETE FROM `creature_template_addon` WHERE (`entry`=37672);
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES (37672, '70385 70405');
UPDATE `creature_template` SET `vehicle_id`=587 WHERE `entry` in (36678,38431,38585,38586);
UPDATE `creature_template` SET `vehicle_id`=591 WHERE `entry` in (37672,38605,38786,38787);
DELETE FROM `spell_script_target` WHERE `entry` IN (70360);
INSERT INTO `spell_script_target` VALUES (70360,1,37690);

-- ----------
-- Blood wing
-- ----------

UPDATE `gameobject_template` SET `faction` = '0', `ScriptName` = 'go_bloodwing_sigil' WHERE `gameobject_template`.`entry` IN (202183);
DELETE FROM `spell_script_target` WHERE `entry` IN (70952, 70981, 70982);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES 
('70952', '1', '37970'),
('70981', '1', '37972'),
('70982', '1', '37973');
-- --------------
-- Lanathel intro
-- --------------

UPDATE `creature_template` SET `ScriptName`='boss_blood_queen_lanathel_intro', `AIName`='' WHERE `entry`= 38004;
-- UPDATE `creature_template` SET `ScriptName`='npc_blood_orb_control', `AIName`='' WHERE `entry`= 38008;

-- --------
-- Taldaram
-- --------

UPDATE `creature_template` SET `ScriptName`='boss_taldaram_icc', `AIName`='' WHERE `entry`= 37973;
UPDATE `creature_template` SET `ScriptName`='mob_ball_of_flames', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry` IN (38332,38451);

-- -------
-- Valanar
-- -------

UPDATE `creature_template` SET `ScriptName`='boss_valanar_icc', `AIName`='' WHERE `entry`= 37970;
UPDATE `creature_template` SET `ScriptName`='mob_kinetic_bomb', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry`= 38454;
UPDATE `creature_template` SET `ScriptName`='mob_shock_vortex', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry`= 38422;
UPDATE `creature_template` SET `ScriptName`='mob_kinetic_bomb_target', `AIName`='' WHERE `entry`= 38458;

-- --------
-- Keleseth
-- --------

UPDATE `creature_template` SET `ScriptName`='boss_keleseth_icc', `AIName`='' WHERE `entry`= 37972;
UPDATE `creature_template` SET `ScriptName`='mob_dark_nucleus', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry`= 38369;

DELETE FROM  `creature_template_addon` WHERE `entry` IN (37972,37973,37970,38401,38784,38785,38399,38769,38770,38400,38771,38772);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `emote`, `moveflags`, `auras`) VALUES
(37970, 0, 0, 0, 0, '71598'),
(37972, 0, 0, 0, 0, '71598'),
(37973, 0, 0, 0, 0, '71598'),
(38401, 0, 0, 0, 0, '71598'),
(38784, 0, 0, 0, 0, '71598'),
(38785, 0, 0, 0, 0, '71598'),
(38399, 0, 0, 0, 0, '71598'),
(38769, 0, 0, 0, 0, '71598'),
(38770, 0, 0, 0, 0, '71598'),
(38400, 0, 0, 0, 0, '71598'),
(38771, 0, 0, 0, 0, '71598'),
(38772, 0, 0, 0, 0, '71598');

-- -----------------
-- Loot and deathstate for blood council (correct YTDB bugs, flags - from already killed princes)
-- -----------------

UPDATE `creature_template` SET `unit_flags` = '0' WHERE `entry` IN (37972,37973,37970,38401,38784,38785,38399,38769,38770,38400,38771,38772);

UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201920,201377,201378);
UPDATE `gameobject` SET `state` = '1' WHERE `id` IN (201920,201377,201378);
UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201376);
UPDATE `gameobject` SET `state` = '0' WHERE `id` IN (201376);

-- ---------------
-- Queen Lana'thel
-- ---------------

UPDATE `creature_template` SET `ScriptName`='boss_blood_queen_lanathel', `AIName`='' WHERE `entry`= 37955;
UPDATE `creature_template` SET  `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 14, `faction_H` = 14,`ScriptName`='mob_swarming_shadows' WHERE `entry`= 38163;
UPDATE `gameobject_template` SET `faction` = '0', `ScriptName` = 'go_frostwing_sigil' WHERE `gameobject_template`.`entry` IN (202181);
DELETE FROM `spell_proc_event` WHERE entry IN (70871);
INSERT INTO `spell_proc_event` VALUES
(70871, 0x7F,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0);

DELETE FROM `creature_model_info` WHERE (`modelid`=31165);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31165, 3, 2, 2, 0, 0);

DELETE FROM `creature_model_info` WHERE (`modelid`=31093);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31093, 1.24, 2, 1, 0, 0);
-- ---------------------
-- Valithria dreamwalker
-- ---------------------

UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35, `AIName` = '', `ScriptName` = 'boss_valithria_dreamwalker' WHERE `entry` = 36789;
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35 WHERE `entry`= 38174;
UPDATE `creature_template` SET `faction_A` = 14, `faction_H` = 14, `ScriptName`='mob_nightmare_portal', `AIName`='' WHERE `entry`= 38429;
UPDATE `creature_template` SET `ScriptName`='mob_mana_void', `AIName`='' WHERE `entry`= 38068;
DELETE FROM `creature` WHERE `guid` = 47738 AND `id` = 38589;
DELETE FROM `pool_creature` WHERE `guid`=47738;

UPDATE `gameobject_template` SET `faction` = '0',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201375,201373);
UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201374);
UPDATE `gameobject` SET `state` = '1' WHERE `id` IN (201374);
UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201380,201381,201382,201383);
UPDATE `gameobject_template` SET `faction` = '0' WHERE `entry` IN (201380,201381,201382,201383);
UPDATE `gameobject` SET `state` = '1' WHERE `id` IN (201380,201381,201382,201383);

-- ----------
-- Sindragosa
-- ----------
UPDATE `creature_template` SET `ScriptName`='boss_sindragosa', `AIName`='' WHERE `entry`= 36853;
UPDATE `creature_template` SET `ScriptName`='mob_rimefang', `AIName`='' WHERE `entry`= 37533;
UPDATE `creature_template` SET `ScriptName`='mob_spinestalker', `AIName`='' WHERE `entry`= 37534;

UPDATE `creature_template` SET `ScriptName`='mob_ice_tomb', `AIName`='' WHERE `entry`= 36980;
UPDATE `creature_template` SET `ScriptName`='mob_frost_bomb', `AIName`='' WHERE `entry`= 37186;
UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201369,201379);
UPDATE `gameobject` SET `state` = '1' WHERE `id` IN (201369,201379);

-- -------------------------------
-- frost bomb target from Lordronn
-- -------------------------------
DELETE FROM `creature_template_addon` WHERE `entry` = 37186;
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES (37186, 70022);

-- ---------
-- Lich King
-- ---------

UPDATE `creature_template` SET `ScriptName`='boss_the_lich_king_icc', `AIName`='' WHERE `entry`= 36597;
UPDATE `creature_template` SET `ScriptName`='boss_tirion_icc', `npcflag`=1, `AIName`='' WHERE `entry`= 38995;
INSERT IGNORE INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES ('71614', '1', '38995');

UPDATE `creature_template` SET `ScriptName`='mob_ice_sphere_icc', `AIName`='' WHERE `entry`= 36633;
UPDATE `creature_template` SET `ScriptName`='mob_defiler_icc', `AIName`='' WHERE `entry`= 38757;
UPDATE `creature_template` SET `ScriptName`='mob_strangulate_vehicle', `AIName`='' WHERE `entry`= 36598;
UPDATE `creature_template` SET `ScriptName`='mob_vile_spirit', `AIName`='' WHERE `entry`= 37799;
UPDATE `creature_template` SET `ScriptName`='mob_raging_spirit', `AIName`='' WHERE `entry`= 36701;

DELETE FROM `npc_text` WHERE `ID` IN (721001,721002);
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(721001, 'Greetings $N! Are you ready to battle with Lich King?'),
(721002, 'Instance script designed by /dev/rsa especially for\n http://wow.teletoria.ru\n Thanks to:\n Vladimir Mangos\n Insider42\n Wowka321\n Selector\n and many other !\n');

DELETE FROM `locales_npc_text` WHERE `entry` IN (721001,721002);
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc2`, `Text0_0_loc3`, `Text0_0_loc4`, `Text0_0_loc5`, `Text0_0_loc6`, `Text0_0_loc7`, `Text0_0_loc8`) VALUES
(721001, 'Greetings $N! Are you ready to battle with Lich King?', NULL, NULL, NULL, NULL, NULL, NULL, 'Приветствую, $N! Поможешь мне прихлопнуть главного засранца WOW?'),
(721002, 'Instance script designed by /dev/rsa especially for\n http://wow.teletoria.ru\n Thanks to:\n Vladimir Mangos\n Insider42\n Wowka321\n Selector\n and many other !\n', NULL, NULL, NULL, NULL, NULL, NULL, 'Скрипт инстанса разработан специально для\n http://wow.teletoria.ru\n Благодарности:\n Vladimir Mangos\n Insider42\n Wowka321\n Selector\n и многим другим!\n (c) /dev/rsa 2010 год');

-- -----------------
-- EAI YTDB CLEAN UP
-- -----------------
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=37973);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=37972);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=37970);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=38004);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=38112);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=37813);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=36627);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=36723);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=36789);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=36612);
DELETE FROM `creature_ai_scripts` WHERE (`creature_id`=36855);

-- -----------------------------------------------------------------------------------------------
-- Instance Fixes --------------------------------------------------------------------------------
-- -----------------------------------------------------------------------------------------------

-- The Following had the auras  ' permanet Fegien death, be undead and some other retarded aura

DELETE FROM `creature_template_addon` WHERE (`entry`=37122);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`) VALUES (37122, 0, 0, 1, 0, 333, 0);

DELETE FROM `creature_template_addon` WHERE (`entry`=37123);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`) VALUES (37123, 0, 0, 1, 0, 333, 0);

DELETE FROM `creature_template_addon` WHERE (`entry`=37124);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`) VALUES (37124, 0, 0, 1, 0, 333, 0);

DELETE FROM `creature_template_addon` WHERE (`entry`=37125);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`) VALUES (37125, 0, 0, 1, 0, 333, 0);

DELETE FROM `creature_template_addon` WHERE (`entry`=37132);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`) VALUES (37132, 0, 0, 0, 0, 333, 0);

DELETE FROM `creature_template_addon` WHERE (`entry`=37134);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`) VALUES (37134, 0, 0, 0, 0, 333, 0);

DELETE FROM `creature_template_addon` WHERE (`entry`=37133);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`) VALUES (37133, 0, 0, 0, 0, 333, 0);
-- end of weird addon fix

-- fix double spawn of dream dragon
DELETE FROM `creature` WHERE `id`=37950;

-- ------
