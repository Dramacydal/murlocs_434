SET NAMES utf8;

-- Sniffing Out the Perpetrator
UPDATE `quest_template` SET `ReqSourceId1`='40971',`ReqSourceCount1`='1' WHERE `entry`='12855';

-- Long Forgotten Memories issue 750
UPDATE `quest_template` SET `Method` = 0 WHERE entry = 8305;

-- The Valiant's Challenge issue 743
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = 33707 WHERE `entry` IN (13699,13713,13723,13724,13725,13726,13727,13728,13729,12731);
UPDATE `creature` SET `spawntimesecs` = 25 WHERE `id` = 33707;
UPDATE `creature_template` SET `faction_A` = 7, `faction_H` = 7, `unit_flags` = 0 WHERE `entry` = 33707;

-- The Aspirant's Challenge
UPDATE `quest_template` SET `ReqCreatureOrGOId1` = 33447 WHERE `entry` IN (13679, 13680);

-- Kindness
UPDATE `quest_template` SET `Method` = 2, `ReqSourceId2` = 31372, `ReqSourceId3` = 31373, `ReqSourceCount2` = 31372, `ReqSourceCount3` = 5, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0, ReqItemId1 = 31372, ReqItemCount1 = 8  WHERE `entry` = 10804;

-- Skybreaker Leutenant
UPDATE `creature_template` SET `difficulty_entry_2` = 38492, `difficulty_entry_3` = 38496 WHERE `entry` = 38492;

-- Still At It
UPDATE `quest_template` SET `ReqItemId1` = 0, `ReqItemCount1` = 0, `ReqCreatureOrGOId1` = 28566, `ReqCreatureOrGOCount1` = 1 WHERE `entry` = 12644;

-- Redeeming the Dead
UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0 WHERE `entry` = 9685;

-- The Tome of Divinity
UPDATE `quest_template` SET `Method` = 2 WHERE `entry` = 1786;

-- Personal Property
UPDATE `quest_template` SET `ReqItemId1` = 51315, `ReqItemCount1` = 1, `ReqSourceId1` = 0, `ReqSourceCount1` = 0 WHERE `entry` = 24914;

-- Titanium Powder (obsolete)
DELETE FROM `creature_questrelation` WHERE `quest` = 14103;
DELETE FROM `creature_involvedrelation` WHERE `quest` = 14103;

-- A Distraction for Akama (missing quest start & finish 13429 and 10985 from Xi'ri)
REPLACE INTO `creature_questrelation` (`id`, `quest`) VALUES ('18528', '13429');
REPLACE INTO `creature_questrelation` (`id`, `quest`) VALUES ('18528', '10985');
REPLACE INTO `creature_involvedrelation` (`id`, `quest`) VALUES ('18528', '13429');

-- A Feast of Souls (killcredit fill data, YTDB error?)
UPDATE `creature_template` SET `KillCredit2` = '38121' WHERE `entry` IN (10404, 36619, 36724, 36805, 36807, 36808, 36811, 36829, 36844, 37007, 37012, 37230, 37544, 37545, 37546, 37890, 37949, 38508, 37022, 37023, 37025, 37038, 37098, 37217, 37571, 37595, 37663, 37664, 38410, 38104, 37662, 37665, 37125, 37126, 37127, 37134, 37534, 36791, 37132, 37133, 37501, 37502, 37863, 37868, 37886, 37934, 38125, 38184, 38494, 38490, 38485, 38472, 38248, 37531, 37532, 37228, 37229, 37695, 36824);

-- Blacksting's Bane (http://wh.300murlocs.com/?quest=9896) vs Unfinished Business (http://wh.300murlocs.com/?quest=9901)
UPDATE `quest_template` SET `PrevQuestId`='0', `NextQuestId`='9896', `NextQuestInChain`='9896' WHERE `entry`='9901';

-- Terokk's Downfall (http://wh.300murlocs.com/?quest=11073)
-- UPDATE `creature_ai_scripts` SET `action1_param1`='40733' WHERE `creature_id` = '21838' AND `action1_param1` = '40733';
UPDATE `creature_ai_scripts` SET `action1_param1`='67251' WHERE `creature_id` = '21838' AND `action1_param1` = '40733';

-- The Art of Being a Water Terror (http://wh.300murlocs.com/?quest=13091)
-- first: repair Water Terror (http://old.wowhead.com/npc=30645)
-- UPDATE `creature_template` SET `powerType`='0' WHERE `entry`='30645';
UPDATE `creature_template` SET `powerType`='3' WHERE `entry`='30645';

-- second: setup AI scripts
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '29880';
DELETE FROM `creature_ai_scripts` WHERE (`id`='29880100');
INSERT INTO `creature_ai_scripts` VALUES ('29880100', '29880', '6', '0', '100', '0', '0', '0', '0', '0', '33', '30644', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '300murlocs-q13091');
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '30725';
DELETE FROM `creature_ai_scripts` WHERE (`id`='30725100');
INSERT INTO `creature_ai_scripts` VALUES ('30725100', '30725', '6', '0', '100', '0', '0', '0', '0', '0', '33', '30644', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '300murlocs-q13091');
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '30632';
DELETE FROM `creature_ai_scripts` WHERE (`id`='30632100');
INSERT INTO `creature_ai_scripts` VALUES ('30632100', '30632', '6', '0', '100', '0', '0', '0', '0', '0', '33', '30644', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '300murlocs-q13091');
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '30243';
DELETE FROM `creature_ai_scripts` WHERE (`id`='30243100');
INSERT INTO `creature_ai_scripts` VALUES ('30243100', '30243', '6', '0', '100', '0', '0', '0', '0', '0', '33', '30644', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '300murlocs-q13091');
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '30250';
DELETE FROM `creature_ai_scripts` WHERE (`id`='30250100');
INSERT INTO `creature_ai_scripts` VALUES ('30250100', '30250', '6', '0', '100', '0', '0', '0', '0', '0', '33', '30644', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '300murlocs-q13091');

-- Slaves to Saronite (http://wh.300murlocs.com/?quest=13300 & http://wh.300murlocs.com/?quest=13302)
-- UPDATE `gossip_menu_option` SET `action_menu_id`='0', `action_script_id`='0' WHERE `menu_id`='10137';
UPDATE `gossip_menu_option` SET `action_menu_id`='-1', `action_script_id`='10137100' WHERE `menu_id`='10137' AND `id`=0;
UPDATE `gossip_menu_option` SET `action_menu_id`='-1', `action_script_id`='10137101' WHERE `menu_id`='10137' AND `id`=1;
DELETE FROM `gossip_scripts` WHERE `id` BETWEEN 10137100 AND 10137101;
INSERT INTO `gossip_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES
 (10137100, 0, 8, 31866, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q13300 - killcredit 31866'),
 (10137100, 1, 18, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q13300 - despawn'),
 (10137101, 0, 8, 31866, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q13302 - killcredit 31866'),
 (10137101, 1, 18, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q13302 - despawn');

-- Reconnaissance Flight (http://wh.300murlocs.com/?quest=12671)
-- UPDATE `quest_template` SET `Method`='2' WHERE `entry`='12671';
UPDATE `quest_template` SET `Method`='0' WHERE `entry`='12671';

-- Defending Wyrmrest Temple (http://wh.300murlocs.com/?quest=12372)
-- UPDATE `gossip_scripts` SET `datalong2`='0' WHERE `id`='9568' AND `datalong`='49256';
UPDATE `gossip_scripts` SET `datalong2`='3' WHERE `id`='9568' AND `datalong`='49256';
UPDATE creature_template SET spell1=49161,spell2=49243,spell3=49263,spell4=49264,spell5=49367,unit_flags=0,InhabitType=3 WHERE entry=27629;
REPLACE INTO creature_ai_scripts VALUES (2769801,27698,8,0,100,1,49367,-1,0,0,33,27698,6,0,0,0,0,0,0,0,0,0,'q12372');
UPDATE creature_template SET AIName='EventAI' WHERE entry=27698;
REPLACE INTO spell_script_target VALUES (49370,1,27698),(49367,1,27698);

-- Reclusive Runemaster (http://wh.300murlocs.com/?quest=12150)
-- right fix and cleanup
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = '27003';
INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
 (2700301, 27003, 11, 0, 100, 0, 0, 0, 0, 0, 42, 20, 1, 0, 20, 1, 0, 0, 2, 0, 0, 0, 'Dregmar Runebrand - Set Invinceable at 20% HP and Enable Melee and Reset Faction on Spawn'),
 (2700302, 27003, 4, 0, 100, 0, 0, 0, 0, 0, 1, -892, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Dregmar Runebrand - Yell on Aggro'),
 (2700303, 27003, 2, 0, 100, 0, 50, 0, 0, 0, 1, -893, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Dregmar Runebrand - Yell at 50% HP'),
 (2700304, 27003, 2, 0, 100, 0, 20, 0, 0, 0, 11, 48325, 0, 7, 2, 35, 0, 0, 23, 1, 0, 0, 'Dregmar Runebrand - Yell and Cast Rune Shield and Set Faction Friendly and Set Phase 1 at 20% HP'),
 (2700305, 27003, 0, 13, 100, 1, 1000, 1000, 1000, 1000, 20, 0, 0, 0, 1, -50011, 0, 0, 23, 1, 0, 0, 'Dregmar Runebrand - Disable Melee and Yell and Set Phase 2'),
 (2700306, 27003, 0, 11, 100, 1, 1000, 1000, 1000, 1000, 15, 12150, 1, 0, 41, 8000, 0, 0, 22, 0, 0, 0, 'Dregmar Runebrand - Cast Reclusive Runemaster Quest Complete and Delayed Despawn and Set Phase 0 (Quest: 12150)');

-- only for backup: old data
-- INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
--  (2700353, 27003, 2, 0, 100, 0, 20, 0, 0, 0, 11, 48325, 0, 0, 15, 12150, 1, 0, 24, 0, 0, 0, 'q12150'),
--  (2700351, 27003, 2, 0, 100, 0, 60, 40, 0, 0, 1, -50010, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q12150'),
--  (2700352, 27003, 2, 0, 100, 0, 30, 25, 0, 0, 1, -50011, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'q12150'),
--  (2700306, 27003, 0, 11, 100, 1, 1000, 1000, 1000, 1000, 11, 48028, 1, 0, 41, 8000, 0, 0, 22, 0, 0, 0, 'Dregmar Runebrand - Cast Reclusive Runemaster Quest Complete and Delayed Despawn and Set Phase 0 (Quest: 12150)');

-- fix EventAI script for Ruby Keeper (http://old.wowhead.com/?npc=27530)
-- UPDATE `creature_ai_scripts` SET `event_flags`='0' WHERE `id`='2753051' AND `creature_id` = '27530';
UPDATE `creature_ai_scripts` SET `event_flags`='1' WHERE `id`='2753051' AND `creature_id` = '27530';

-- fix EventAI script for Plague Cauldron KC Bunny (http://old.wowhead.com/?npc=31767)
-- REPLACE INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
--  (3176751, 31767, 8, 0, 100, 1, 59655, -1, 0, 0, 11, 59876, 6, 6, 0, 0, 0, 0, 0, 0, 0, 0, 'ytdb-q13295,13279,13297,13281');
REPLACE INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
 (3176751, 31767, 8, 0, 100, 1, 59655, -1, 0, 0, 33, 31767, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'ytdb-q13295,13279,13297,13281');
REPLACE INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES ('59655', '1', '31767');

-- I Sense a Disturbance (12665)
UPDATE `gossip_scripts` SET `datalong2` = 2 WHERE `id` = 28401;
UPDATE `creature_template` SET `ScriptName` = 'npc_harkoa_kitten', `speed_run` = 1.8, `flags_extra` = `flags_extra` | 4096 WHERE `entry` = 28665;
DELETE FROM `custom_texts` WHERE `entry` BETWEEN -2000005 AND -2000000;
INSERT INTO `custom_texts` (`entry`, `content_default`, `content_loc8`, `type`, `comment`) VALUES
(-2000000, 'Thank you for saving me, $N. This is the least that I could do to return the favor. Hold on tight. Here we go.', 'Спасибо за спасение, $N. Это самое меньшее, что я мог сделать в благодарность. Держись крепко. Поехали.', 4, 'Harkoa Kitten - 1'),
(-2000001, 'This doesn''t look good. Whatever you do, don''t fall off. There''s a ton of nasty things in there!', 'Нехорошие у меня предчувствия. Что бы там ни было, не отступай. Там много опасных существ!', 4, 'Harkoa Kitten - 2'),
(-2000002, 'Oh no... Quetz''lun is dead! Stay still. We''ll sneak past the prophet.', 'О нет… Кетц''Лун мертва! Спокойно. Мы прокрадемся мимо пророка.', 4, 'Harkoa Kitten - 3'),
(-2000003, 'We''re spotted! Hang on. We have to get out of here!', 'Нас заметили! Держись крепче. Нам нужно выбираться отсюда!', 4, 'Harkoa Kitten - 4'),
(-2000004, 'I think we''re safe now. Let''s get back!', 'Думаю, теперь нам ничего не угрожает. Возвращаемся!', 4, 'Harkoa Kitten - 5'),
(-2000005, 'We made it! Take care, $N, and thanks again!', 'Все получилось! Всего тебе хорошего, $N, и еще раз спасибо!', 4, 'Harkoa Kitten - 6');

-- Foundation for Revenge (12668)
UPDATE `creature_template` SET `ScriptName` = 'npc_quetzlun_troll' WHERE `entry` IN (28747, 28748);
UPDATE `creature` SET `phaseMask` = 3 WHERE `id` = 28719;
UPDATE `creature_template` SET `unit_flags` = `unit_flags` | 0x2000000 WHERE `entry` = 28719;
UPDATE `gameobject` SET `phaseMask` = 3 WHERE `id` = 190707;

DELETE FROM `creature_template_addon` WHERE `entry` = 28719;
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUES
(28719, '52222');

-- Image of Drakuru (npc 26500)
UPDATE `creature` SET `position_x` = 3382.58, `position_y` = -1806.711426, `position_z` = 114.166374, `orientation` = 1.927345 WHERE `id` = 26500;

-- Hell Hath a Fury (12674)
UPDATE `creature_template` SET `ScriptName` = 'npc_quetzlun_priest' WHERE `entry` IN (28752, 28754, 28756);
DELETE FROM `custom_texts` WHERE `entry` BETWEEN -2000008 AND -2000006;
INSERT INTO `custom_texts` (`entry`, `content_default`, `content_loc8`, `type`, `comment`) VALUES
(-2000006, 'NOOOOOOOOOoooooooo.......!', 'НЕ-Е-ЕТ!!!', 0, 'High Priest Mufunu - death say'),
(-2000007, 'FIRE! AHHHHHHHH!', 'ОГОНЬ! АААААААА!', 0, 'High Priest Tua-Tua - death say'),
(-2000008, 'BRRRRRR!', 'БРРРРРР!', 0, 'High Priest Hawinni - death say');

-- Toxic Tolerance (13850)
DELETE FROM `creature_template_addon` WHERE `entry` = 6508;
INSERT INTO `creature_template_addon` (`entry`, `b2_0_sheath`, `b2_1_pvp_state`, `auras`) VALUES
(6508, 1, 16, '14108');

-- Wooly Justice (12707)
UPDATE `creature_template` SET `IconName` = 'vehichleCursor', `spell1` = 52603, `spell2` = 52601 WHERE `entry` = 28851;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 28851;
INSERT INTO `npc_spellclick_spells` (`npc_entry`, `spell_id`, `quest_start`, `quest_start_active`, `cast_flags`) VALUE
(28851, 52600, 12707, 1, 1);
UPDATE `creature_template` SET `ScriptName` = 'npc_mamtoth_disciple' WHERE `entry` = 28861;

-- The Key of Warlord Zol'Maz (12712)
DELETE FROM `event_scripts` WHERE `id` = 18940;
INSERT INTO `event_scripts` (`id`, `command`, `datalong`, `datalong2`) VALUE
(18940, 11, 46814, 15);
INSERT INTO `event_scripts` (`id`, `command`, `buddy_entry`, `search_radius`, `data_flags`) VALUE
(18940, 26, 28902, 50, 0x2);
UPDATE `creature` SET `spawntimesecs` = 20 WHERE `guid` = 99346;
UPDATE `creature_template` SET `modelid_1` = 25749, `modelid_2` = 0 WHERE `entry` = 28927;

UPDATE `gameobject_template` SET `data2` = 53061 WHERE `entry` = 190834;
DELETE FROM `event_scripts` WHERE `id` = 53061;
INSERT INTO `event_scripts` (`id`, `command`, `datalong`, `datalong2`) VALUE
(53061, 11, 46814, 15);

-- Pick Pocket loot for quest Klaven's Tower (http://wh.300murlocs.com/?quest=2359)
UPDATE `creature_template` SET `pickpocketloot`='7051' WHERE `entry`='7051';
DELETE FROM `pickpocketing_loot_template` WHERE `item` = '7923';
INSERT INTO `pickpocketing_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`, `lootcondition`, `condition_value1`, `condition_value2`) VALUES
 (7051, 7923, -100, 0, 1, 1, 0, 0, 0);

-- Who Are They? (http://wh.300murlocs.com/?quest=10041)
UPDATE `gossip_menu_option` SET `cond_1`='1' WHERE `menu_id`='7757' AND `id`='0';
UPDATE `gossip_menu_option` SET `cond_1`='1' WHERE `menu_id`='7759' AND `id`='0';
UPDATE `gossip_menu_option` SET `cond_1`='1' WHERE `menu_id`='7760' AND `id`='0';

-- Shred The Alliance & Shredder Repair
DELETE FROM `spell_script_target` WHERE `entry` = 48610;
INSERT INTO `spell_script_target` VALUE
(48610, 0, 300202);

-- Data Mining (http://wh.300murlocs.com/?quest=12927 & http://wh.300murlocs.com/?quest=12860)
-- UPDATE `creature_template` SET `AIName` = '' WHERE `entry` = '29746';
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '29746';
DELETE FROM `creature_ai_scripts` WHERE (`id`='29746100');
INSERT INTO `creature_ai_scripts` VALUES ('29746100', '29746', '8', '0', '100', '1', '59728', '-1', '0', '0', '33', '29746', '6', '0', '0', '0', '0', '0', '0', '0', '0', '0', '300murlocs-q12860-q12927');

-- Lazy Peons (http://wh.300murlocs.com/?quest=5441)
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` = '10556';
DELETE FROM `creature_ai_scripts` WHERE (`id`='10556100');
INSERT INTO `creature_ai_scripts` VALUES ('10556100', '10556', '8', '0', '100', '1', '19938', '-1', '10000', '10000', '33', '10556', '6', '0', '41', '10000', '0', '0', '0', '0', '0', '0', '300murlocs-q5441');

-- Still At It (http://wh.300murlocs.com/?quest=12644)
-- REPLACE INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
-- (47391, 190643, 571, 1, 1, 5547.61, 5767.75, -78.0231, 4.08966, 0, 0, 0.889735, -0.456477, -120, 100, 1);
DELETE FROM `gameobject` WHERE `guid` = '47391';
DELETE FROM `gameobject` WHERE `guid` = '511000';
UPDATE `quest_template` SET `ReqItemId1`='38688', `ReqItemCount1`='1', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='12644';

-- Banish the Demons (http://wh.300murlocs.com/?quest=11026) & Banish More Demons (http://wh.300murlocs.com/?quest=12644)
-- UPDATE `creature_template` SET `ScriptName`='generic_creature' WHERE `entry` IN (19973, 22204);
-- UPDATE `creature_template` SET `ainame`='' WHERE `entry` IN (19973,22327,22201,22195,22204,22392,20557,22291,23322);
-- DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (19973,22327,22201,22195,22204,22392,20557,22291,23322);
-- INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
-- (2229101, 22291, 0, 0, 100, 1, 6300, 12800, 6300, 12800, 11, 32736, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Furnace Guard - Cast Mortal Strike'),
-- (2219501, 22195, 0, 0, 75, 1, 2500, 7500, 14000, 18000, 11, 34017, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Wrath Speaker - Cast Rain of Chaos'),
-- (2219502, 22195, 0, 0, 100, 1, 10100, 14100, 122000, 130000, 11, 11980, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Wrath Speaker - Cast Curse of Weakness'),
-- (2055701, 20557, 4, 0, 100, 0, 0, 0, 0, 0, 11, 22911, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Wrath Hound - Cast Charge on Aggro'),
-- (2055702, 20557, 0, 0, 100, 1, 7400, 11600, 18900, 18900, 11, 36406, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 'Wrath Hound - Cast Double Breath');
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (19973,22327,22201,22195,22204,22392,20557,22291,23322);
UPDATE `creature_template` SET `ainame`='EventAI', `ScriptName`='' WHERE `entry` IN (19973,22327,22201,22195,22204,22392,20557,22291,23322);
UPDATE `creature_template` SET `unit_flags`=33554432 WHERE `entry` IN (23322,23327);
INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
 (19973100, 19973, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (19973101, 19973, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (20557100, 20557, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (20557101, 20557, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (20557102, 20557, 4, 0, 100, 0, 0, 0, 0, 0, 11, 22911, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On aggro - cast charge'),
 (20557103, 20557, 0, 0, 100, 0, 5000, 11000, 20000, 27000, 11, 36406, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'In combat - cast breath'),
 (22195100, 22195, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (22195101, 22195, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (22195102, 22195, 0, 0, 75, 0, 2500, 7500, 14000, 18000, 11, 34017, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'In combat - cast rain of chaos'),
 (22195103, 22195, 0, 0, 100, 0, 10100, 14100, 122000, 130000, 11, 11980, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'In combat - cast curse of weakness'),
 (22201100, 22201, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (22201101, 22201, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (22204100, 22204, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (22204101, 22204, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (22291100, 22291, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (22291101, 22291, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (22291102, 22291, 0, 0, 100, 0, 6300, 12800, 6300, 12800, 11, 32736, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'In combat - cast mortal strike'),
 (22327100, 22327, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (22327101, 22327, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (22392100, 22392, 8, 0, 100, 1, 40825, -1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On spell hit - Set phase 1'),
 (22392101, 22392, 6, 1, 100, 0, 0, 0, 0, 0, 33, 23327, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'On death in phase 1 - give quest credit'),
 (23322100, 23322, 11, 0, 100, 0, 0, 0, 0, 0, 11, 40849, 0, 2, 11, 40857, 0, 2, 0, 0, 0, 0, 'On spawn - cast portal');
DELETE FROM `spell_script_target` WHERE `entry` IN (40825, 40830);
INSERT INTO `spell_script_target` (`entry`, `type`, `targetEntry`) VALUES
 ('40825', '1', '19973'),
 ('40825', '1', '22327'),
 ('40825', '1', '22201'),
 ('40825', '1', '22195'),
 ('40825', '1', '22204'),
 ('40825', '1', '22392'),
 ('40825', '1', '20557'),
 ('40825', '1', '22291'),
 ('40830', '1', '19973'),
 ('40830', '1', '22327'),
 ('40830', '1', '22201'),
 ('40830', '1', '22195'),
 ('40830', '1', '22204'),
 ('40830', '1', '22392'),
 ('40830', '1', '20557'),
 ('40830', '1', '22291');

-- Fire At Will! (http://wh.300murlocs.com/?quest=10911)
-- UPDATE `creature_template` SET `unit_flags` = 0, `dynamicflags` = 0 WHERE `entry` IN (22471, 22472, 22443);
-- UPDATE `creature_template` SET `AIName` = '' WHERE `entry` IN (22471, 22472);
-- UPDATE `creature_template` SET `unit_flags`='33554432' WHERE `entry`=22443;
-- REPLACE INTO `spell_script_target` VALUES
-- ('39219', '1', '22443'),
-- ('39219', '1', '22471'),
-- ('39219', '1', '22472'),
-- ('39219', '1', '22503'),
-- ('39219', '1', '22504');
UPDATE `creature_template` SET `unit_flags` = 516, `dynamicflags` = 8 WHERE `entry` IN (22471, 22472, 22443);
UPDATE `creature_template` SET `AIName` = 'EventAI' WHERE `entry` IN (22471, 22472);
DELETE FROM `creature_ai_scripts` WHERE `id` IN (22471100, 22472100);
INSERT INTO `creature_ai_scripts` VALUES
 ('22471100', '22471', '8', '0', '100', '1', '39221', '-1', '0', '0', '33', '22503', '6', '0', '41', '0', '0', '0', '0', '0', '0', '0', 'On spell hit - give quest credit'),
 ('22472100', '22472', '8', '0', '100', '1', '39221', '-1', '0', '0', '33', '22504', '6', '0', '41', '0', '0', '0', '0', '0', '0', '0', 'On spell hit - give quest credit');
DELETE FROM `spell_script_target` WHERE `entry` = '39219';
REPLACE INTO `spell_script_target` VALUES ('39219', '1', '22443');

-- Mastery Of The Shield-Breaker (http://wh.300murlocs.com/?quest=13835 & http://wh.300murlocs.com/?quest=13838)
-- mangosR2 problem?
UPDATE `creature_template` SET `spell2`='64342' WHERE `entry`='33844';
UPDATE `creature_template` SET `spell2`='64342' WHERE `entry`='33845';

-- Ribbly Screwspigot (http://wh.300murlocs.com/?quest=4136)
UPDATE `gossip_menu_option` SET `action_menu_id`='-1', `action_script_id`='1970' WHERE (`menu_id`='1970') AND (`id`='1');
DELETE FROM `gossip_scripts` WHERE `id` = '1970';
INSERT INTO `gossip_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `search_radius`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES
(1970, 1, 22, 14, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');


-- fix quest for achievement Call to Arms! (http://wh.300murlocs.com/?achievement=11051)
REPLACE INTO `creature_questrelation` (`id`, `quest`) VALUES (15351, 11335),
 (15351, 11336),
 (15351, 11337),
 (15351, 11338),
 (15351, 13405),
 (15351, 13478),
 (15351, 14163),
 (32626, 11335),
 (32626, 11336),
 (32626, 11337),
 (32626, 11338),
 (32626, 13405),
 (32626, 13478),
 (32626, 14163);
REPLACE INTO `creature_involvedrelation` (`id`, `quest`) VALUES (32626, 14163),
 (32626, 13478),
 (32626, 13405),
 (32626, 11338),
 (32626, 11337),
 (32626, 11336),
 (32626, 11335),
 (15351, 24427),
 (15351, 24224),
 (15351, 24223),
 (15351, 24220),
 (15351, 24219),
 (15351, 24218),
 (15351, 14180),
 (15351, 14179),
 (15351, 14178),
 (15351, 14163),
 (15351, 13478),
 (15351, 13427),
 (15351, 13405),
 (15351, 11338),
 (15351, 11337),
 (15351, 11336),
 (15351, 11335),
 (15351, 8398),
 (15351, 8397),
 (15351, 8396),
 (15351, 8395),
 (15351, 8394),
 (15351, 8393),
 (15351, 8392),
 (15351, 8391),
 (15351, 8385),
 (15351, 8384),
 (15351, 8374),
 (15351, 8371);
