-- first clean 
-- YTDB cleanup
DELETE FROM `creature` WHERE `map` = 609 AND `guid` IN (116863);
DELETE FROM `creature` WHERE `map` = 609 AND `id` IN (29219,29206,29190);
UPDATE `creature_template` SET `unit_flags`=0,`type_flags`=8 WHERE entry IN (29174,29182,29186,29190,29206,29176,29177,29181);
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'generic_creature' WHERE `entry` = 29178;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'generic_creature' WHERE `entry` = 29179;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'generic_creature' WHERE `entry` = 29180;

UPDATE `gameobject_template` SET ScriptName ="go_org_portal", data0 = 0 WHERE `entry` = 193052;
UPDATE `gameobject_template` SET ScriptName ="go_sw_portal", data0 = 0 WHERE `entry` = 193053;

-- ------------
-- Special npcs
-- ------------

UPDATE `creature_template` SET `ScriptName` = 'npc_valkyr_battle_maiden' WHERE `entry` = '28534';

-- ------------------------
-- QUEST::DAWN OF THE LIGHT
-- ------------------------
-- remove the four static uneeded spawn around chapel --  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29174;

-- remove static rayne--  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29181;

-- remove static rimblat--  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29182;

-- remove static krofax--  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29176;

-- remove static eligor--  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29177;

-- remove static tyrosus--  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29178;

-- remove static leon--  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29179;

-- remove stativ duke--  spawned during event not static plus they respawn during event
DELETE FROM `creature` WHERE `id`=29180;

UPDATE `creature_template` SET `AIName` = '', `ScriptName`='npc_highlord_darion_mograine' WHERE `entry`='29173';
UPDATE `creature_template` SET `AIName` = '', `ScriptName`='npc_the_lich_king_tirion_dawn' WHERE `entry` in (29183,29175);
UPDATE `creature_template` SET `AIName` = '', `ScriptName`='npc_minibosses_dawn_of_light' WHERE `entry` IN (29199,29204,29200);
UPDATE `creature_template` SET `AIName` = '', `ScriptName`='mob_acherus_ghoul' WHERE `entry`='29219';
UPDATE `creature_template` SET `AIName` = '', `ScriptName`='mob_warrior_of_the_frozen_wastes' WHERE `entry`='29206';

DELETE FROM `spell_script_target` WHERE `entry` in (53658, 53679, 53701, 53705, 53706, 53677, 53685);
INSERT INTO `spell_script_target` VALUES (53679, 1, 29183);
INSERT INTO `spell_script_target` VALUES (53701, 1, 29175);
INSERT INTO `spell_script_target` VALUES (53705, 1, 29183);
INSERT INTO `spell_script_target` VALUES (53706, 1, 29183);
INSERT INTO `spell_script_target` VALUES (53677, 1, 29227);
INSERT INTO `spell_script_target` VALUES (53685, 1, 29175);

-- ----------------------------
-- Quest:: An End To All Things
-- ----------------------------

-- Frostbrood Vanquisher
UPDATE creature_template SET
    spell1 = 53114,
    spell2 = 53110,
    spell3 = 0,
    spell4 = 0,
    spell5 = 0,
    spell6 = 0,
    vehicle_id = 156
WHERE entry IN (28670);

UPDATE creature_template SET maxhealth = 133525, minhealth = 133525, maxmana = 51360, minmana = 51360, InhabitType = 3 WHERE entry = 28670;

REPLACE INTO creature_template_addon (entry, mount, bytes1, b2_0_sheath, emote, moveflags, auras) VALUES
(28670, 0, 50331648, 1, 0, 1024, '53112');

-- -----------------------------
-- Quest::Ambush at the overlook
-- -----------------------------
UPDATE `creature_template` SET `ScriptName`='mob_scarlet_courier' WHERE `entry`='29076';

-- ----------------------
-- Quest::Bloody Breakout
-- ----------------------

UPDATE `creature_template` SET `mechanic_immune_mask` = 12582928 WHERE `entry` = 28912; 
UPDATE `creature_template` SET `mechanic_immune_mask` = 12582928 WHERE `entry` = 28447;

-- tweak Valroth EAI
UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29001';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900112');
INSERT INTO `creature_ai_scripts` VALUES ('2900112', '29001', '0', '0', '100', '1', '2000', '10000', '7500', '10000', '11', '52922', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', 'High Inquisitor Valroth - Cast Inquisitor_penance');

UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29001';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900101');
INSERT INTO `creature_ai_scripts` VALUES ('2900101', '29001', '1', '0', '100', '0', '0', '0', '0', '0', '21', '1', '1', '0', '22', '0', '0', '0', '0', '0', '0', '0', 'High Inquisitor Valroth - Set Phase to 0 on Spawn');

UPDATE `gameobject_template` SET `castBarCaption` = 'Valroth\'s Remains' WHERE `entry` = 191092;  -- caption bar message when looting remains

UPDATE `creature_template` SET `equipment_id` = 0 WHERE `entry` = 28912;  -- tabled half dead version shouldnt be carrying a axe
UPDATE `creature_template` SET `equipment_id` = 488 WHERE `entry` = 28447; -- this is model with axe

DELETE FROM `creature_equip_template` WHERE (`entry`=488);
INSERT INTO `creature_equip_template` (`entry`, `equipentry1`, `equipentry2`, `equipentry3`) VALUES (488, 38633, 0, 0); -- make sure axe is right 

DELETE FROM `creature_template_addon` WHERE (`entry`=28912);   -- spawned entry should nt have anti zone magic field 
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`, `auras`) VALUES (28912, 0, 7, 1, 0, 0, 0, ''); 

DELETE FROM `creature_template_addon` WHERE (`entry`=28447);  -- this entry is the one that needs antimagic zone
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`, `auras`) VALUES (28447, 0, 0, 1, 0, 0, 0, 52894);

-- tweaked crimsons EAIs
DELETE FROM `creature` WHERE `id`=29007;
UPDATE `creature_template` SET `AIName`='EventAI',minmana=1020,maxmana=1058,unit_flags=32768 WHERE (`entry`='29007');

UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29007';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900702');
INSERT INTO `creature_ai_scripts` VALUES ('2900702', '29007', '0', '0', '100', '1', '1000', '4000', '1000', '4000', '11', '15498', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '');

UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29007';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900701');
INSERT INTO `creature_ai_scripts` VALUES ('2900701', '29007', '4', '0', '100', '0', '0', '0', '0', '0', '11', '15498', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '');

UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '29007';
DELETE FROM `creature_ai_scripts` WHERE (`id`='2900703');
INSERT INTO `creature_ai_scripts` VALUES ('2900703', '29007', '11', '0', '100', '1', '0', '0', '0', '0', '11', '34809', '0', '1', '0', '0', '0', '0', '0', '0', '0', '0', '');

-- -----------------------------------------------------
-- Quest how to win friends and how to influence enemies
-- -----------------------------------------------------
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_crusade_persuaded' WHERE `entry` IN (28939,28610);   -- 28640 scarlet crusader needs to be added

-- ---------------------------------------------------------------
-- Quest Death comes From a High
-- ---------------------------------------------------------------
-- Eye of acherus
UPDATE `creature_template` SET `InhabitType` = 3, `ScriptName` = 'npc_eye_of_acherus' WHERE `entry` = 28511;
REPLACE INTO `creature_template_addon` (`entry`,`moveflags`,`auras`) VALUES (28511,33562624,''),(28525,0,'64328'),(28542,0,'64328'),(28543,0,'64328'),(28544,0,'64328');
REPLACE INTO `spell_script_target` (`entry`,`type`,`targetEntry`) VALUES (51859,1,28525),(51859,1,28542),(51859,1,28543),(51859,1,28544);
DELETE FROM `creature_addon` WHERE `guid` IN (SELECT guid FROM `creature` WHERE `id` IN (28511,28525,28542,28543,28544));
UPDATE `npc_spellclick_spells` SET `quest_start` = 0, `quest_start_active` = 0 WHERE `npc_entry` = 29501;

UPDATE `quest_template` SET `ReqSpellCast1` = 0, `ReqSpellCast2` = 0, `ReqSpellCast3` = 0, `ReqSpellCast4` = 0 WHERE `entry` = 12641;

DELETE FROM `creature_template` WHERE (`entry`=28511);
INSERT INTO `creature_template` (`entry`, `difficulty_entry_1`, `difficulty_entry_2`, `difficulty_entry_3`, `KillCredit1`, `KillCredit2`, `modelid_1`, `modelid_2`, `modelid_3`, `modelid_4`, `name`, `subname`, `IconName`, `gossip_menu_id`, `minlevel`, `maxlevel`, `minhealth`, `maxhealth`, `minmana`, `maxmana`, `armor`, `faction_A`, `faction_H`, `npcflag`, `speed_walk`, `speed_run`, `scale`, `rank`, `mindmg`, `maxdmg`, `dmgschool`, `attackpower`, `dmg_multiplier`, `baseattacktime`, `rangeattacktime`, `unit_class`, `unit_flags`, `dynamicflags`, `family`, `trainer_type`, `trainer_spell`, `trainer_class`, `trainer_race`, `minrangedmg`, `maxrangedmg`, `rangedattackpower`, `type`, `type_flags`, `lootid`, `pickpocketloot`, `skinloot`, `resistance1`, `resistance2`, `resistance3`, `resistance4`, `resistance5`, `resistance6`, `spell1`, `spell2`, `spell3`, `spell4`, `PetSpellDataId`, `mingold`, `maxgold`, `AIName`, `MovementType`, `InhabitType`, `unk16`, `unk17`, `RacialLeader`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `movementId`, `RegenHealth`, `equipment_id`, `trainer_id`, `vendor_id`, `mechanic_immune_mask`, `flags_extra`, `ScriptName`) VALUES (28511, 0, 0, 0, 0, 0, 26320, 25499, 0, 0, 'Eye of Acherus', '', '', 0, 55, 55, 2614, 2614, 0, 0, 9730, 35, 35, 0, 1, 1.14286, 1, 0, 420, 630, 0, 157, 1, 2000, 2000, 1, 0, 0, 0, 0, 0, 0, 0, 336, 504, 126, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52694, 52006, 51859, 51904, 0, 0, 0, '', 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 'npc_eye_of_acherus');
-- -----------------------------------------------------------------------------
-- Quest  Death's Changelle --
-- -----------------------------------------------------------------------------
 
-- bug olrun - is out of interaction reach for smaller races  --
UPDATE `creature` SET `position_x` = 2371.77, `position_y` = -5788.7, `position_z` = 155.61 WHERE `guid` = 96309;

-- Flying to high for smaller races to interact with olrun during  --
DELETE FROM `creature_movement` WHERE `id`=96309;
INSERT INTO `creature_movement` (`id`,`point`,`position_x`,`position_y`,`position_z`,`waittime`,`script_id`,`textid1`,`textid2`,`textid3`,`textid4`,`textid5`,`emote`,`spell`,`wpguid`,`orientation`,`model1`,`model2`) VALUES
(96309, 1, 2376.5, -5781.67, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 2, 2372.28, -5788.87, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 3, 2364.96, -5780.4, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 4, 2360.76, -5774.66, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 5, 2365.82, -5770.87, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 6, 2371.81, -5767.54, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 7, 2371.6, -5775.58, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 8, 2376.5, -5781.67, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 9, 2372.28, -5788.87, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(96309, 10, 2364.96, -5780.4, 155.667, 2000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- --------------------------------------------------------------------
-- Quest Fix Noth Special Brew  (dk starting area quest fix 12716/12717
-- ---------------------------------------------------------------------
DELETE FROM `creature_questrelation` WHERE `quest` = 12716; 
DELETE FROM `gameobject_questrelation` WHERE `quest` = 12716; 
UPDATE `item_template` SET `StartQuest`=0 WHERE `StartQuest` = 12716; 
INSERT INTO `creature_questrelation` (`id`, `quest`) VALUES (28919, 12716); 
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry` = 28919; 
DELETE FROM `creature_involvedrelation` WHERE `quest` = 12716; 
DELETE FROM `gameobject_involvedrelation` WHERE `quest` = 12716; 
INSERT INTO `creature_involvedrelation` (`id`, `quest`) VALUES (28919, 12716); 
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=28919; 
UPDATE `quest_template` SET `ExclusiveGroup` = 12716 WHERE `entry` = 12716; 
UPDATE `quest_template` SET `SpecialFlags` = 0 WHERE `entry` = 12717;

-- -------------------------------
-- Quest Into the Realm of Shadows
-- -------------------------------
UPDATE creature_template SET IconName = 'vehichleCursor',
unit_flags = 0,
spell1 = 52362
WHERE entry =28782;

UPDATE quest_template SET 
SrcSpell = 52359,
SpecialFlags = 2,
ReqCreatureOrGOId1 = 28768,
ReqCreatureOrGOCount1 = 1,
ReqSpellCast1 = 0,
RewItemId1 = 39208,
RewItemCount1 = 1 WHERE entry = 12687;

DELETE FROM creature_involvedrelation WHERE quest IN (12687);
INSERT INTO creature_involvedrelation (id, quest) VALUES (28788, 12687);
UPDATE creature_template SET npcflag = 2 WHERE entry = 28788;

DELETE FROM spell_script_target WHERE entry = 52349;

-- original YTDB scripts here
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 28768;
INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_inverse_phase_mask`, `event_chance`, `event_flags`, `event_param1`, `event_param2`, `event_param3`, `event_param4`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `action2_type`, `action2_param1`, `action2_param2`, `action2_param3`, `action3_type`, `action3_param1`, `action3_param2`, `action3_param3`, `comment`) VALUES
(2876803, 28768, 0, 0, 100, 1, 3000, 7000, 6000, 9000, 11, 50688, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Dark Rider of Acherus - Cast Plague Strike'),
(2876806, 28768, 6, 0, 100, 0, 0, 0, 0, 0, 12, 28782, 0, 10000, 15, 12687, 6, 0, 43, 0, 0, 0, 'Dark Rider of Acherus - Spawn Deathcharger of Acherus and Set Quest Event Complete and Dismount on Death'),
(2876801, 28768, 4, 0, 100, 0, 0, 0, 0, 0, 1, -780, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Dark Rider of Acherus - Yell on Aggro'),
(2876804, 28768, 0, 0, 100, 1, 6000, 10000, 6000, 9000, 11, 52374, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Dark Rider of Acherus - Cast Blood Strike'),
(2876802, 28768, 0, 0, 100, 1, 0, 1500, 6000, 9000, 11, 52372, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Dark Rider of Acherus - Cast Icy Touch'),
(2876805, 28768, 9, 0, 100, 1, 5, 30, 2000, 2000, 11, 52356, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Dark Rider of Acherus - Cast Throw');

-- replace for one action
UPDATE creature_ai_scripts SET 
action1_type   = '11',
action1_param1 = '52361',
action1_param2 = '6',
action1_param3 = '16',
action2_type   = '11',
action2_param1 = '52357',
action2_param2 = '6',
action2_param3 = '16',
action3_type   = '0'
WHERE id = 2876806;

DELETE FROM creature WHERE id = 28782;

DELETE FROM creature_template_addon WHERE entry = 28782;

DELETE FROM npc_spellclick_spells WHERE npc_entry IN (28782);
INSERT INTO npc_spellclick_spells VALUES
(28782, 46598, 0, 0, 0, 1);

-- -----------------------------------
-- Quest The Gift That Keeps On Giving
-- -----------------------------------

-- item spell script targets (Scarlet Miners)
DELETE FROM spell_script_target WHERE entry = 52479;
INSERT INTO spell_script_target VALUES
(52479, 1, 28819),
(52479, 1, 28822),
(52479, 1, 28891);

UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = 28846;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_scarlet_ghoul' WHERE `entry` = 28845;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 28822;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 28819;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 28891;
UPDATE `item_template` SET `ScriptName` = 'mob_scarlet_miner' WHERE `entry` = 39253;

-- EventAI for ghost needs tweaked and double checked
DELETE FROM `creature_ai_texts` WHERE `entry` IN (-286102, -286101, -286100);
INSERT INTO `creature_ai_texts` VALUES
(-286100, "Die, Scourge filth!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Ghost SAY1"),
(-286101, "It won't be that easy, friend!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Ghost SAY2"),
(-286102, "I'll take you with me!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Ghost SAY3");
 
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 28846;
INSERT INTO `creature_ai_scripts` VALUES
(2884601, 28846, 11, 0, 100, 0, 0, 0, 0, 0, 1, -286100, -286101, -286102, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Ghost - Random say at spawn");

-- fix to take Quest Item Away at end of quest
UPDATE `quest_template` SET `ReqItemId1` = 39253, `ReqItemCount1` = 1 WHERE `entry` = 12698;

-- -------------------------------------
-- -- Massacre at Light's point quest 12701
-- -------------------------------------

UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_scarlet_miner' WHERE `entry` = 28841;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_mine_car' WHERE `entry` = 28817;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_scourge_gryphon' WHERE `entry` = 28864;
UPDATE `gameobject_template` SET `ScriptName` = 'go_inconspicous_mine_car' WHERE `entry` = 190767;
UPDATE `creature_template` SET `mechanic_immune_mask` = 0, `flags_extra` = 0 WHERE `entry` = 28864;

-- fixed dispaly of mine cart
UPDATE `creature_template` SET `modelid_2` = 25703 WHERE `entry` = 28817;

/* Scourge Gryphon */
UPDATE creature_template SET vehicle_id = 143, speed_run = 2 WHERE entry = 28864;

DELETE FROM `creature_template_addon` WHERE (`entry`=28864);
INSERT INTO `creature_template_addon` (`entry`, `mount`, `bytes1`, `b2_0_sheath`, `b2_1_pvp_state`, `emote`, `moveflags`, `auras`) VALUES (28864, 0, 0, 0, 0, 0, 0, 61453);

UPDATE creature_template SET
spell1 = 52435,
spell2 = 52576,
spell3 = 0,
spell4 = 0,
spell5 = 52588,
spell6 = 0,
vehicle_id = 79
WHERE entry IN (28833);

UPDATE creature_template SET
spell1 = 52435,
spell2 = 52576,
spell3 = 0,
spell4 = 0,
spell5 = 52588,
spell6 = 0,
vehicle_id = 68
WHERE entry IN (28887);

INSERT INTO npc_spellclick_spells VALUES ('28833', '52447', '12701', '1', '12701', '1');
INSERT INTO npc_spellclick_spells VALUES ('28887', '52447', '12701', '1', '12701', '1');
UPDATE creature_template SET minhealth = 26140, maxhealth = 26140, dynamicflags = 0, minmana = 2117, maxmana = 2117, unit_flags = 772, minlevel = 55, maxlevel = 55, unk16 = 10, unk17 = 1, InhabitType = 3, scale = 1, mindmg = 685, maxdmg = 715, armor = 3232, attackpower = 214, unit_class = 2, TYPE = 10 WHERE entry = 28833;
UPDATE creature_template SET minhealth = 26140, maxhealth = 26140, dynamicflags = 0, minmana = 0, maxmana = 0, unit_flags = 772, minlevel = 55, maxlevel = 55, unk16 = 10, unk17 = 1, InhabitType = 3, scale = 1, mindmg = 685, maxdmg = 715, armor = 3232, attackpower = 214, unit_class = 2, TYPE = 10 WHERE entry = 28887;
INSERT IGNORE INTO spell_script_target VALUES (52576, 1, 28834);
INSERT IGNORE INTO spell_script_target VALUES (52576, 1, 28886);
INSERT IGNORE INTO spell_script_target VALUES (52576, 1, 28850);

DELETE FROM spell_target_position WHERE id = 52462;
INSERT INTO spell_target_position
(id,target_map,target_position_x,target_position_y,target_position_z,target_orientation) VALUES
(52462, 609, 2390.193115, -5900.238281, 108.967354, 3.894099);


-- -------------------------------------
-- ACID scripts for Scarlet Enclave mobs
-- -------------------------------------

-- EventAI scripts name set
UPDATE `creature_template` SET `AIName` = "EventAI", `scriptname` = "" WHERE entry IN (28834, 28892, 28856, 28936, 28850, 29104, 28941, 28942, 28577, 28576, 28557);

-- script_texts
DELETE FROM `creature_ai_texts` WHERE `entry` BETWEEN -286099 AND -286092;
INSERT INTO `creature_ai_texts` VALUES
(-286099, "You don't have to do this! Nobody has to die!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear01"),
(-286098, "Let me live! I'll do whatever you say!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear02"),
(-286097, "Ugh... I... I think I pooped...", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear03"),
(-286096, "I picked the wrong day to quit drinkin'!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear04"),
(-286095, "Don't kill me! I only took this job for benefits!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Say Fear05"),
(-286094, "Have mercy, sir!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Heavenshire - Say Aggro1"),
(-286093, "You may take my life, but you won't take my freedom!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Heavenshire - Say Enrage1"),
(-286092, "DIE!", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Heavenshire - Say Enrage2");

-- EVENTAI scripts
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (28834, 28856, 28936, 28850, 29104, 28941, 28942, 28577, 28576, 28557, 28892);
INSERT INTO `creature_ai_scripts` VALUES

-- Scarlet Defender
(2883401, 28834, 0, 0, 100, 1, 0, 0, 3000, 3000, 11, 52566, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Defender - Cast Shoot"),
-- Scarlet Guardian
(2885601, 28856, 4, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Stop moving and set Phase 2 on aggro"),
(2885602, 28856, 9, 1, 100, 0, 10, 150, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Stop moving and set Phase 2 at 10yd range"),
(2885603, 28856, 0, 1, 100, 1, 3000, 3000, 3000, 3000, 11, 25710, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Cast Heroic Strike(Phase 1)"),
(2885604, 28856, 0, 2, 100, 1, 0, 0, 3000, 3000, 11, 52566, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Cast Shoot(Phase 2)"),
(2885605, 28856, 9, 2, 100, 0, 0, 10, 0, 0, 21, 1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Set Phase 1 at less then 10yd (Phase 2)"),
(2885606, 28856, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Guardian - Evade 150yd"),
-- Scarlet Commander
(2893601, 28936, 4, 0, 100, 0, 0, 0, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Stop moving and set Phase 2 on aggro"),
(2893602, 28936, 9, 1, 100, 0, 10, 150, 0, 0, 21, 0, 0, 0, 22, 2, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Stop moving and set Phase 2 at 10yd range"),
(2893603, 28936, 0, 1, 100, 1, 3000, 3000, 3000, 3000, 11, 25710, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Cast Heroic Strike(Phase 1)"),
(2893604, 28936, 0, 2, 100, 1, 0, 0, 3000, 3000, 11, 52566, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Cast Shoot(Phase 2)"),
(2893605, 28936, 9, 2, 100, 0, 0, 10, 0, 0, 21, 1, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Set Phase 1 at less then 10yd (Phase 2)"),
(2893606, 28936, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Commander - Evade 150yd"),
-- Scarlet Land Cannon
(2885001, 28850, 0, 0, 100, 1, 0, 0, 3000, 3000, 11, 52539, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Cannon - Cast Cannonball"),
(2885002, 28850, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Cannon - Evade 150yd"),
-- Scarlet Balista
(2910401, 29104, 0, 0, 100, 1, 0, 0, 3000, 3000, 11, 53117, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Balista - Cast Ballista assault"),
(2910402, 29104, 9, 0, 100, 0, 150, 300, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Balista - Evade 150yd"),
-- Citizens of New Avalon -- needs tweaked
(2894101, 28941, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of New Avalon - Spawn set Phase 1"),
(2894102, 28941, 0, 0, 80, 1, 0, 0, 10000, 10000, 11, 52716, 1, 1, 22, 2, 0, 0, 1, -286094, -286099, -286095, "Citizen of New Avalon - On Aggro: Escape in fear and set Phase 2"),
(2894103, 28941, 0, 0, 20, 1, 0, 0, 10000, 10000, 11, 52262, 1, 1, 22, 2, 0, 0, 1, -286093, -286092, 0, "Citizens of New Avalon - On Aggro: Enrage and set Phase 2"),
(2894201, 28942, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of New Avalon - Spawn set Phase 1"),
(2894202, 28942, 0, 0, 80, 1, 0, 0, 10000, 10000, 11, 52716, 1, 1, 22, 2, 0, 0, 1, -286094, -286099, -286096, "Citizen of New Avalon - On Aggro: Escape in fear and set Phase 2"),
(2894203, 28942, 0, 0, 20, 1, 0, 0, 10000, 10000, 11, 52262, 1, 1, 22, 2, 0, 0, 1, -286093, -286092, 0, "Citizens of New Avalon - On Aggro: Enrage and set Phase 2"),
-- Citizens of Havenshire -- needs tweaked
(2857601, 28576, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of New Havenshire - Spawn set Phase 1"),
(2857602, 28576, 4, 0, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286094, -286099, -286096, 0, 0, 0, 0, "Citizen of Havenshire - On Aggro: Beg for mercy"),
(2857603, 28576, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 21, 0, 0, 0, 5, 20, 0, 0, "Citizen of Havenshire - Cower (Phase 3)"),
(2857604, 28576, 4, 0, 20, 0, 0, 0, 0, 0, 11, 52262, 1, 1, 1, -286093, -286092, 0, 0, 0, 0, 0, "Citizens of Havenshire - On Aggro: Enrage"),
(2857605, 28576, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Citizens of Havenshire - Reset State"),

(2857701, 28577, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Citizen of Havenshire - Spawn set Phase 1"),
(2857702, 28577, 4, 0, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286094, -286099, -286095, 22, 3, 0, 0, "Citizen of Havenshire - On Aggro: Beg for mercy"),
(2857703, 28577, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 5, 431, 0, 0, 21, 0, 0, 0, "Citizen of Havenshire - Cower (Phase 3)"),
(2857704, 28577, 4, 0, 20, 0, 0, 0, 0, 0, 11, 52262, 1, 1, 1, -286093, -286092, 0, 0, 0, 0, 0, "Citizens of Havenshire - On Aggro: Enrage"),
(2857705, 28577, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Citizens of Havenshire - Reset State"),
-- Scarlet Peasant -- needs tweaked
(2855701, 28557, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Spawn set Phase 1"),
(2855702, 28557, 4, 1, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286099, -286098, -286097, 22, 2, 0, 0, "Scarlet Peasant - On Aggro: Beg for mercy"),
(2855703, 28557, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 5, 20, 0, 0, 21, 0, 0, 0, "Scarlet Peasant - Cower (Phase 2)"),
(2855704, 28557, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Reset State"),
(2889201, 28892, 11, 0, 100, 1, 0, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Spawn set Phase 1"),
(2889202, 28892, 4, 1, 80, 0, 0, 0, 0, 0, 11, 52716, 1, 1, 1, -286099, -286098, -286097, 22, 2, 0, 0, "Scarlet Peasant - On Aggro: Beg for mercy"),
(2889203, 28892, 0, 0, 100, 1, 0, 0, 1000, 1000, 20, 0, 0, 0, 5, 20, 0, 0, 21, 0, 0, 0, "Scarlet Peasant - Cower (Phase 2)"),
(2889204, 28892, 7, 0, 100, 0, 0, 0, 0, 0, 5, 0, 0, 0, 22, 1, 0, 0, 0, 0, 0, 0, "Scarlet Peasant - Reset State");

-- end of EventAI
-- -----------------------------------------------------------------------------------------------------------------------------------------------------
