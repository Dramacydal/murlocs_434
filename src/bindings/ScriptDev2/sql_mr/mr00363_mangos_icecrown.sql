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

-- Icecrown buff
DELETE FROM `spell_area` WHERE `spell` IN
(73762, 73824, 73825, 73826, 73827, 73828, -- Alliance
73816, 73818, 73819, 73820, 73821, 73822);
-- +5%
-- INSERT INTO `spell_area` (`spell`, `area`, `racemask`, `gender`, `autocast`) VALUES
-- (73762, 4812, 1101, 2, 1), -- Alliance
-- (73816, 4812, 690, 2, 1); -- Horde
-- +10%
-- INSERT INTO `spell_area` (`spell`, `area`, `racemask`, `gender`, `autocast`) VALUES
-- (73824, 4812, 1101, 2, 1), -- Alliance
-- (73818, 4812, 690, 2, 1); -- Horde
-- +15%
-- INSERT INTO `spell_area` (`spell`, `area`, `racemask`, `gender`, `autocast`) VALUES
-- (73825, 4812, 1101, 2, 1), -- Alliance
-- (73819, 4812, 690, 2, 1); -- Horde
-- +20%
-- INSERT INTO `spell_area` (`spell`, `area`, `racemask`, `gender`, `autocast`) VALUES
-- (73826, 4812, 1101, 2, 1), -- Alliance
-- (73820, 4812, 690, 2, 1); -- Horde
-- +25%
-- INSERT INTO `spell_area` (`spell`, `area`, `racemask`, `gender`, `autocast`) VALUES
-- (73827, 4812, 1101, 2, 1), -- Alliance
-- (73821, 4812, 690, 2, 1); -- Horde
-- +30%
-- INSERT INTO `spell_area` (`spell`, `area`, `racemask`, `gender`, `autocast`) VALUES
-- (73828, 4812, 1101, 2, 1), -- Alliance
-- (73822, 4812, 690, 2, 1); -- Horde

-- --------
-- Saurfang
-- --------

UPDATE `creature_template` SET `vehicle_id` = 639, `AIName`='', `PowerType` = 3, `ScriptName`='boss_deathbringer_saurfang' WHERE `entry`=37813;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_highlord_saurfang_icc' WHERE `entry`=37187;
UPDATE `creature_template` SET `AIName`='', `ScriptName`='npc_deathbringer_event_guards_iccAI' WHERE `entry` IN (37920, 37902);
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

DELETE FROM `spell_proc_event` WHERE entry IN (72178, 72256);
INSERT INTO `spell_proc_event` VALUES
(72256, 0x7F,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0),
(72178, 0x7F,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0);

-- Rune of Blood (Saurfang)
DELETE FROM `spell_proc_event` WHERE `entry` = 72408;
INSERT INTO `spell_proc_event` () VALUES
(72408, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000, 0x00000000, 0x00000000, 0x00000014, 0x00000003, 0, 0, 0);

-- Blood Link (Saurfang, Blood Beast)
DELETE FROM `spell_proc_event` WHERE `entry` = 72176;
INSERT INTO `spell_proc_event` () VALUES
(72176, 0x00, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000000, 0x00000000, 0x00000000, 0x00000014, 0x00000003, 0, 0, 0);

-- Scent of Blood
DELETE FROM `spell_script_target` WHERE `entry` = 72771;
INSERT INTO `spell_script_target` VALUES (72771, 1, 38508);

-- update attack speed for Saurfang
UPDATE `creature_template` SET baseattacktime = 1000 WHERE `entry` IN (37813, 38402, 38582, 38583);


-- ------------
-- Deathwhisper
-- ------------

UPDATE `creature_template` SET `ScriptName`='boss_lady_deathwhisper', `AIName`='' WHERE `entry`=36855;
UPDATE `creature_template` SET `flags_extra` = `flags_extra` | 256 WHERE `entry` IN (38296, 38297); -- can't taunt on heroic mode
UPDATE `creature_template` SET `faction_A`=14, `faction_H`=14,`ScriptName`='', `AIName`='' WHERE `entry`= 38222;
UPDATE `creature_template` SET `ScriptName`='mob_cult_adherent', `AIName`='' WHERE `entry`= 37949;
UPDATE `creature_template` SET `ScriptName`='mob_cult_fanatic', `AIName`='' WHERE `entry`= 37890;

-- Vengeful Blast aura for shades
DELETE FROM `creature_template_addon` WHERE `entry` = 38222;
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES (38222, "71494");

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


-- -----------
-- Plague wing
-- -----------

-- -------
-- Rotface
-- -------

UPDATE `creature_template` SET `ScriptName`='boss_rotface', `AIName`=''  WHERE `entry`= 36627;
UPDATE `gameobject_template` SET `faction` = '114' WHERE `gameobject_template`.`entry` IN (201370);
UPDATE `gameobject` SET `state` = '0' WHERE `id` IN (201370);
UPDATE `creature_template` SET `ScriptName`='mob_little_ooze', `AIName`='', `flags_extra` = `flags_extra`|0x100 WHERE `entry`= 36897;
UPDATE `creature_template` SET `ScriptName`='mob_big_ooze', `AIName`='' WHERE `entry`= 36899;
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 14, `faction_H` = 14, `ScriptName`='mob_rotface_ooze_dummy', `AIName`='', unit_flags = 524288, flags_extra = flags_extra | 2 WHERE `entry` IN (37013, 37986, 38548);
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 2212, `faction_H` = 2212, `ScriptName`='mob_sticky_ooze', `AIName`='' WHERE `entry`= 37006;
UPDATE `creature_template` SET `minlevel` = 80, `maxlevel` = 80, `AIName` ='', `faction_A`= 2212, `faction_H` = 2212, `ScriptName`='mob_ooze_explosion_stalker', `AIName`='' WHERE `entry` = 38107;

DELETE FROM `spell_script_target` WHERE `entry` IN (69508, 70881, 69782, 69783, 70079);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
('69508', '1', '37986'), -- Slime Spray
('70881', '1', '37986'), -- Slime Spray dummy
('69783', '1', '37013'), -- Ooze Flood
('70079', '1', '36627'); -- Ooze Flood remove

-- fix rotface combat reach was set to 40 he could hit u from across the room
DELETE FROM `creature_model_info` WHERE (`modelid`=31005);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31005, 5, 4, 2, 0, 0);

-- fix stinky modelID info
DELETE FROM `creature_model_info` WHERE (`modelid`=30483);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (30483, 5, 1, 2, 0, 0);

-- fix flags of Puddle Stalkers
UPDATE `creature_template` SET `unit_flags` = 524288 WHERE `entry` = 37013;

-- ---------
-- Festergut
-- ---------

UPDATE `creature_template` SET `ScriptName`='boss_festergut', `AIName`=''  WHERE `entry`= 36626;
UPDATE `gameobject_template` SET `faction` = '114' WHERE `gameobject_template`.`entry` IN (201371);
UPDATE `gameobject` SET `state` = '0' WHERE `id` IN (201371);
UPDATE `creature_template` SET `ScriptName`='mob_vile_gas_malleable_goo', unit_flags = 0, `AIName`='', `flags_extra` = `flags_extra` | 2 | 128  WHERE `entry` IN (38548, 38556);
-- orange gas stalker
UPDATE `creature_template` SET `unit_flags` = `unit_flags` & ~33554432 & ~2, `faction_A` = 2212, `faction_H` = 2212,  `ScriptName`='', `AIName`='' WHERE `entry`= 36659;
DELETE FROM `spell_script_target` WHERE `entry` IN (69157, 69162, 69164);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(69157, 1, 36659),
(69162, 1, 36659),
(69164, 1, 36659);

-- original auras from YTDB
DELETE FROM `creature_template_addon` WHERE `entry` = 36659;
-- INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `bytes2`, `emote`, `moveflags`, `auras`) VALUES
-- (36659, 0, 0, 1, 0, 0, '69126 69152 69154');

-- gas from valves
UPDATE `creature` SET `spawnMask` = 15, `modelid` = 11686, `spawntimesecs` = 300 WHERE `id`=37013;
DELETE FROM `spell_script_target` WHERE `entry` = 69125;
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES (69125, 1, 37013);

-- proper way for Gastric Bloat, but cooldowns for creatures not implemented yet
-- -- Gastric Bloat
DELETE FROM `spell_proc_event` WHERE `entry` = 72214;
-- INSERT INTO `spell_proc_event` VALUES
-- (72214, 0x00,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 8);

-- fixed modelID info
DELETE FROM `creature_model_info` WHERE (`modelid`=31006);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31006, 5, 4, 2, 0, 0);

-- fix peroicus modelID info
DELETE FROM `creature_model_info` WHERE (`modelid`=30483);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (30483, 5, 1, 2, 0, 0);

-- make triggers not visible
UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 33554432 | 2 WHERE entry IN (37013, 37986, 38107, 38548, 37006, 38107, 38548, 38556, 36659);

-- -------------------
-- Professor putricide
-- -------------------

UPDATE `creature_template` SET `ScriptName`='boss_professor_putricide', `AIName`='' WHERE `entry`= 36678;
UPDATE `creature_template` SET `PowerType` = 0, `vehicle_id` = 587 WHERE `entry` IN (36678, 38431, 38585, 38586);
UPDATE `gameobject_template` SET `faction` = '114',`data0` = '0' WHERE `gameobject_template`.`entry` IN (201372,201614,201613, 201612);
UPDATE `gameobject` SET `state` = '1' WHERE `id` IN (201612,201614,201613);
UPDATE `gameobject` SET `state` = '0' WHERE `id` IN (201372);
UPDATE `creature_template` SET `ScriptName`='mob_icc_gas_cloud', `AIName`='' WHERE `entry`= 37562;
UPDATE `creature_template` SET `ScriptName`='mob_icc_volatile_ooze', `AIName`='' WHERE `entry`= 37697;
UPDATE `creature_template` SET `ScriptName`='mob_choking_gas_bomb', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14, `scale` = 0.5 WHERE `entry`= 38159;
UPDATE `creature_template` SET `ScriptName`='mob_ooze_puddle',`scale` = '1.0', `AIName`='', `minlevel` = 82, `maxlevel` = 82, `modelid_1` = 11686, `modelid_2` = 11686, `modelid_3` = 11686, `modelid_4` = 11686, `faction_A` = 14, `faction_H` = 14  WHERE `entry`= 37690;
UPDATE `gameobject_template` SET `faction` = '0', `ScriptName` = 'go_plague_sigil' WHERE `gameobject_template`.`entry` IN (202182);

DELETE FROM `spell_script_target` WHERE `entry` IN (71412, 71415, 71617);
INSERT INTO `spell_script_target` VALUES
(71412, 1, 37824), -- Green Ooze
(71415, 1, 37824), -- Orange Ooze
(71617, 1, 38317); -- Tear Gas

-- delete Grow Stacker - handle in script
DELETE FROM `creature_template_addon` WHERE `entry` = 37690;

-- remove proc from Mutated Strength - currently cooldown for creatures not handled in core
DELETE FROM `spell_proc_event` WHERE `entry` IN (71604, 72673, 72674, 72675);
INSERT INTO `spell_proc_event` (`entry`, `procFlags`) VALUES
(71604, 256),
(72673, 256),
(72674, 256),
(72675, 256);

-- -----------
-- Abomination
-- -----------

DELETE FROM `creature_template_addon` WHERE `entry` IN (37672, 38285);
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES
(37672, '70385'),
(38285, '70385');

UPDATE `creature_template` SET `PowerType` = 3, `vehicle_id`=591 WHERE `entry` IN (37672, 38605, 38786, 38787, 38285, 38788, 38789, 38790);

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

UPDATE `creature_template` SET `minhealth` = 5647725, `maxhealth` = 5647725, `ScriptName`='npc_blood_orb_control', `AIName`='' WHERE `entry`= 38008;
UPDATE `creature_template` SET `minhealth` = 22590900, `maxhealth` = 22590900 WHERE `entry` = 38641;
UPDATE `creature_template` SET `minhealth` = 7600025, `maxhealth` = 7600025 WHERE `entry` = 38773;
UPDATE `creature_template` SET `minhealth` = 30469825, `maxhealth` = 30469825 WHERE `entry` = 38774;

-- --------
-- Taldaram
-- --------

UPDATE `creature_template` SET `ScriptName`='boss_taldaram_icc', `AIName`='' WHERE `entry`= 37973;
UPDATE `creature_template` SET `ScriptName`='mob_ball_of_flames', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry` IN (38332, 38451);

-- -------
-- Valanar
-- -------

UPDATE `creature_template` SET `ScriptName`='boss_valanar_icc', `AIName`='' WHERE `entry`= 37970;
UPDATE `creature_template` SET `ScriptName`='mob_kinetic_bomb', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry`= 38454;
UPDATE `creature_template` SET `ScriptName`='mob_shock_vortex', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry`= 38422;
UPDATE `creature_template` SET `ScriptName`='mob_kinetic_bomb_target', `AIName`='' WHERE `entry`= 38458;

DELETE FROM `spell_proc_event` WHERE `entry` = 72059;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procFlags`, `procEx`) VALUES
(72059, 127, 0x00000008|0x00000020|0x00000080|0x00000200|0x00002000|0x00020000|0x00400000, 0x0010000);

-- --------
-- Keleseth
-- --------

UPDATE `creature_template` SET `ScriptName`='boss_keleseth_icc', `AIName`='' WHERE `entry`= 37972;
UPDATE `creature_template` SET `ScriptName`='mob_dark_nucleus', `AIName`='',`minlevel` = 82, `maxlevel` = 82, `faction_A` = 14, `faction_H` = 14 WHERE `entry`= 38369;

DELETE FROM  `creature_template_addon` WHERE `entry` IN (37972,37973,37970,38401,38784,38785,38399,38769,38770,38400,38771,38772);


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
UPDATE `gameobject_template` SET `faction` = '0', `ScriptName` = 'go_frostwing_sigil' WHERE `gameobject_template`.`entry` IN (202181);

-- Essence of the Blood Queen
DELETE FROM `spell_proc_event` WHERE entry IN (70871);
INSERT INTO `spell_proc_event` VALUES
(70871, 0x7F,  0, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0);

-- Presence of the Darkfallen
DELETE FROM spell_script_target WHERE entry IN (70995, 71952);
INSERT INTO spell_script_target VALUES
(71952, 1, 37955),
(70995, 1, 37955);

-- by default Pact of the Darkfallen is getting spell power coeff, so set it to 0
DELETE FROM `spell_bonus_data` WHERE `entry` = 71341;
INSERT INTO `spell_bonus_data` VALUES
(71341, 0, 0, 0, 0, 'Pact of the Darkfallen (Lanathel)');

DELETE FROM `creature_model_info` WHERE (`modelid`=31165);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31165, 3, 5, 2, 0, 0);

DELETE FROM `creature_model_info` WHERE (`modelid`=31093);
INSERT INTO `creature_model_info` (`modelid`, `bounding_radius`, `combat_reach`, `gender`, `modelid_other_gender`, `modelid_alternative`) VALUES (31093, 1.24, 2, 1, 0, 0);

-- ---------------------
-- Valithria dreamwalker
-- ---------------------
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35, `AIName` = '', `ScriptName` = 'boss_valithria_dreamwalker' WHERE `entry` = 36789;
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35 WHERE `entry` = 38174; -- 25man difficulty
UPDATE `creature_template` SET `faction_A` = 14, `faction_H` = 14, `ScriptName`='mob_valithria_combat_trigger', `AIName`='', `flags_extra` = `flags_extra` &~2 WHERE `entry`= 38752;
UPDATE `creature` SET `phaseMask` = `phaseMask` | 16 WHERE `id` = 38752; -- phaseMask for Combat Trigger
UPDATE `creature_template` SET `unit_flags` = `unit_flags` &~ 33554432, `AIName` = '', `ScriptName` = 'mob_valithria_dream_phase' WHERE `entry` = 37950; -- Valithria in dream phase
REPLACE INTO `creature_template_addon` (`entry`, `emote`) VALUES (`entry` = 37950, `emote` = 453); -- flying state of dream Valithria
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35, `ScriptName` = 'mob_dream_portal_pre', `AIName` = '' WHERE `entry`= 38186;
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35, `IconName` = '', `npcflag` = `npcflag` | 1, `ScriptName` = 'mob_dream_portal', `AIName` = '' WHERE `entry`= 37945;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16, `ScriptName` = 'mob_dream_cloud', `AIName` = '', `flags_extra` = `flags_extra` &~128 WHERE `entry`= 37985;
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35, `ScriptName` = 'mob_nightmare_portal_pre', `AIName` = '' WHERE `entry` = 38429;
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35, `IconName` = '', `npcflag` = `npcflag` | 1, `ScriptName` = 'mob_nightmare_portal', `AIName` = '' WHERE `entry` = 38430;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16, `ScriptName` = 'mob_nightmare_cloud', `AIName` = '', `flags_extra` = `flags_extra` &~128 WHERE `entry` = 38421;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16, `ScriptName` = 'mob_gluttonous_abomination', `AIName` = '' WHERE `entry`= 37886;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16, `ScriptName` = 'mob_blistering_zombie', `AIName` = '' WHERE `entry`= 37934;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16, `ScriptName` = 'mob_risen_archmage', `AIName` = '' WHERE `entry`= 37868;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16, `ScriptName` = 'mob_blazing_skeleton', `AIName` = '', `mechanic_immune_mask` = `mechanic_immune_mask` | 2048 WHERE `entry`= 36791;
UPDATE `creature_template` SET `faction_A` = 16, `faction_H` = 16, `ScriptName` = 'mob_suppresser', `AIName` = '' WHERE `entry`= 37863;
UPDATE `creature_template` SET `minlevel` = 82, `maxlevel` = 82, `ScriptName`='mob_mana_void', `AIName`='', `flags_extra` = `flags_extra` &~128 WHERE `entry`= 38068;
UPDATE `creature_template` SET `ScriptName`='mob_column_of_frost', `AIName`='', `flags_extra` = `flags_extra` &~128 WHERE `entry`= 37918;

DELETE FROM `spell_script_target` WHERE `entry` IN (71946, 72031, 72032, 72033);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(71946, 1, 37950),
(72031, 1, 37950),
(72032, 1, 37950),
(72033, 1, 37950);

-- spawn Valithria in dream state
DELETE FROM `creature` WHERE `id` = 37950;
INSERT INTO `creature` (`id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `spawntimesecs`, `spawndist`, `curhealth`, `MovementType`) VALUES
(37950, 631, 15, 16, 4203.1416, 2484.8616, 386.4849, 6.276, 604800, 0, 1, 0);

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

UPDATE `creature_template` SET `ScriptName`='boss_the_lich_king_icc', `AIName`='' WHERE `entry` = 36597;
UPDATE `creature_template_addon` SET `auras` = '73878 73220 72846' WHERE `entry` IN (36597, 39166, 39167, 39168);
UPDATE `creature_template` SET `speed_walk` = 1.0, `speed_run` = 1.1 WHERE `entry` IN (36597, 39166, 39167, 39168);
UPDATE `creature_template` SET `ScriptName`='boss_tirion_icc', `npcflag`=1, `AIName`='' WHERE `entry`= 38995;
UPDATE `creature_template` SET `ScriptName` = 'boss_terenas_menethil_icc' WHERE entry = 38579;

UPDATE `creature_template` SET `ScriptName`='mob_ice_sphere_icc', `AIName`='' WHERE `entry`= 36633;
UPDATE `creature_template` SET `ScriptName`='mob_defiler_icc', `AIName`='' WHERE `entry`= 38757;
UPDATE `creature_template` SET `ScriptName`='mob_strangulate_vehicle', `AIName`='' WHERE `entry`= 36598;
UPDATE `creature_template` SET `ScriptName`='mob_vile_spirit', `AIName`='' WHERE `entry` IN (37799, 39190);
UPDATE `creature_template` SET `speed_walk` = 1.8, `speed_run` = 1.1 WHERE `entry` IN (37799, 39284, 39285, 39286);
UPDATE `creature_template` SET `ScriptName`='mob_raging_spirit', `AIName`='' WHERE `entry`= 36701;
UPDATE `creature_template` SET `ScriptName`='mob_drudge_ghoul', `AIName`='' WHERE `entry`= 37695;
UPDATE `creature_template` SET `ScriptName`='mob_shambling_horror', `AIName`='' WHERE `entry`= 37698;
UPDATE `creature_template` SET `ScriptName`='mob_shadow_trap', `AIName`='', `faction_A` = 14, `faction_H` = 14, `unit_flags` = `unit_flags` | 2 | 33554432 WHERE `entry`= 39137;
UPDATE `creature_template` SET `ScriptName`='mob_valkyr_shadowguard', `AIName`='' WHERE `entry`= 36609;
UPDATE `creature_template` SET `ScriptName`='npc_terenas_fm', `AIName`='' WHERE `entry`= 36823; -- normal
UPDATE `creature_template` SET `ScriptName`='npc_terenas_fm', `AIName`='' WHERE `entry`= 39217; -- heroic, different entry
UPDATE `creature_template` SET `ScriptName`='mob_spirit_warden', `AIName`='' WHERE `entry`= 36824;
UPDATE `creature_template` SET `ScriptName`='mob_spirit_bomb', `AIName`='', minlevel = 83, maxlevel = 83, `unit_flags` = `unit_flags` | 33554432, `faction_A` = 14, `faction_H` = 14 WHERE `entry`= 39189;

-- speed of Wicked Spirits
UPDATE `creature_template` SET `speed_walk` = 0.9, `speed_run` = 0.9 WHERE `entry` IN (39190, 39287, 39288, 39289);

-- damage of Terenas and Spirit Warden, they should be hitting each other for around 10-11k normal melee damage
UPDATE `creature_template` SET `dmg_multiplier` = 46 WHERE `entry` IN (36823, 36824, 39296);

-- make Ice Spheres untauntable
UPDATE `creature_template` SET `mechanic_immune_mask` = `mechanic_immune_mask` | 256 WHERE `entry` IN (36633, 39305, 39306, 39307);

DELETE FROM `spell_script_target` WHERE `entry` IN (71614, 74074, 73028, 74321, 74322, 74323, 72679, 74318, 74319, 74320);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
(71614, 1, 38995),
(74074, 1, 36597),
(73028, 1, 36597), -- Harvested Soul
(74321, 1, 36597),
(74322, 1, 36597),
(74323, 1, 36597),
(72679, 1, 36597),
(74318, 1, 36597),
(74319, 1, 36597),
(74320, 1, 36597);

-- Tirion gossip
DELETE FROM `npc_gossip` WHERE npc_guid = 115781;
INSERT INTO `npc_gossip` (`npc_guid`, `textid`) VALUES
(115781, 15290);

-- proc for Dark Hunger
DELETE FROM `spell_proc_event` WHERE `entry` = 69383;
INSERT INTO `spell_proc_event` (`entry`, `procFlags`) VALUES
(69383, 0x04 | 0x10 | 0x10000);

-- fix stats for some creatures
-- Raging Spirit
UPDATE `creature_template` SET `minhealth` = 885400, `maxhealth` = 885400 WHERE `entry` = 36701; -- 10normal
UPDATE `creature_template` SET `minhealth` = 2650000, `maxhealth` = 2650000 WHERE `entry` = 39302; -- 25normal
UPDATE `creature_template` SET `minhealth` = 1230000, `maxhealth` = 1230000 WHERE `entry` = 39303; -- 10hero
UPDATE `creature_template` SET `minhealth` = 4150000, `maxhealth` = 4150000 WHERE `entry` = 39304; -- 25hero

-- Ice Sphere
UPDATE `creature_template` SET `minhealth` = 6000, `maxhealth` = 6000 WHERE `entry` = 36633; -- 10normal
UPDATE `creature_template` SET `minhealth` = 15200, `maxhealth` = 15200 WHERE `entry` = 39305; -- 25normal
UPDATE `creature_template` SET `minhealth` = 15200, `maxhealth` = 15200 WHERE `entry` = 39306; -- 10hero
UPDATE `creature_template` SET `minhealth` = 53200, `maxhealth` = 53200 WHERE `entry` = 39307; -- 25hero

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


-- DELETE FROM `creature` WHERE `id`=37950;

-- ------
