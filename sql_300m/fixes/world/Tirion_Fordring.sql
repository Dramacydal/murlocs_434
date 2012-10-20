SET NAMES utf8;

DELETE FROM `creature_template` WHERE `entry` = 88005;
INSERT INTO `creature_template` (`entry`, `ScriptName`, `modelid_1`, `modelid_2`, `modelid_3`, `modelid_4`, `name`, `subname`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `unit_flags`, `flags_extra`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `baseattacktime`, `family`, `type`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `mingold`, `maxgold`) VALUES
('88005', 'npc_singragosa_helper', '31286', '0', '0', '0', "Tirion Fordring", "Sindragosa Encounter Helper", '80', '80', '500000', '500000', '0', '0', '1000', '35', '35', 0x1, 0x202, 0x2, '2.50', '8.00', '1', '3', '0', '0', '100', '0', '7', '0', '0', '0', '0', '0', '0', '0', '0');
DELETE FROM `locales_creature` WHERE `entry` = 88005;
INSERT INTO `locales_creature` (`entry`, `name_loc8`, `subname_loc8`) VALUE
(88005, 'Тирион Фордринг', 'Помощник в битве с Синдрагосой');

-- // 4288.604492 Y: 2504.232178 Z: 211.071732 Orientation: 5.771882

DELETE FROM `npc_text` WHERE `ID` IN (90014, 90015, 90016);
INSERT INTO `npc_text` (`ID`, `text0_0`) VALUES
(90014, 'You have already killed Sindragosa, don\'t you remember?'),
(90015, 'I can not help you, all seems okay!'),
(90016, 'Ok, I will help you.');

DELETE FROM `db_script_string` WHERE `entry` = 2000006675;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006675, 'I want drakes to respawn.', 'Я хочу, чтобы драконы реснулись.');

DELETE FROM `locales_npc_text` WHERE `entry` IN (90014, 90015, 90016);
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc8`) VALUES
(90014, 'Вы уже убили Синдрагосу, вы что, не помните?'),
(90015, 'Я ничем не могу помочь, все как-будто выглядит в порядке!'),
(90016, 'Хорошо, я помогу вам.');

DELETE FROM `creature` WHERE `guid` = 500309;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `modelid`, `equipment_id`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `currentwaypoint`, `curhealth`, `curmana`, `DeathState`, `MovementType`) VALUE
(500309, 88005, 631, 15, 1, 0, 0, 4288.604492, 2504.232178, 211.071732, 5.771882, 60, 0, 0, 500000, 0, 0, 0);
