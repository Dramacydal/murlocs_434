SET NAMES 'utf8';

-- Chug and Chuck! mugs resp time
UPDATE `gameobject` SET `spawntimesecs` = 30 WHERE `id` = 186189;
UPDATE `item_template` set `spellid_1` = 47173 where `entry` = 33096;

-- Pink Elleks
UPDATE `creature_template` SET `gossip_menu_id` = 24657 WHERE `entry` = 24657;
UPDATE `creature_template` SET `gossip_menu_id` = 24657 WHERE `entry` = 23486;

DELETE FROM gossip_menu_option WHERE menu_id = 24657;
INSERT INTO gossip_menu_option (menu_id, id, option_icon, option_text, option_id, npc_option_npcflag, action_script_id) VALUES
(24657, 0, 0, "GOSSIP_OPTION_QUESTGIVER", 2, 2, 0),
(24657, 1, 0, "I need a pair of goggles.", 1, 1, 24657);
DELETE FROM `locales_gossip_menu_option` WHERE `menu_id` = 24657;
INSERT INTO `locales_gossip_menu_option` (menu_id, id, option_text_loc8) VALUES
(24657, 1, "Мне нужны очки!");
UPDATE `creature` SET `spawntimesecs` = 30 WHERE `id` IN (23528, 23507, 23531, 53529, 23527, 23530);

DELETE FROM `gossip_scripts` WHERE `id` = 24657;
INSERT INTO `gossip_scripts` (id, command, datalong, datalong2, comments) VALUE
(24657, 15, 66592, 2, '');

-- Wolpertingers
UPDATE `quest_template` SET `ReqItemId1` = 32906, `ReqItemId2` = 32907, `ReqItemCount1` = 5, `ReqItemCount2` = 0,
`ReqSpellCast1` = 0 WHERE `entry` IN (11431, 11117);
UPDATE `creature` SET `spawntimesecs` = 30 WHERE `id` = 23487;

-- Brew of the Year
DELETE FROM `npc_vendor` WHERE `entry` = 27819 AND `item` IN (37497, 37907);
INSERT INTO `npc_vendor` VALUES
(27819, 37497, 0, 0, 0),
(27819, 37907, 0, 0, 0);

--
UPDATE `quest_template` SET `QuestFlags` = `QuestFlags` | 0x1000, SpecialFlags = 0 WHERE `entry` IN (11122, 11412);
UPDATE `quest_template` SET `SrcSpell` = 43883 WHERE `entry` IN (11407, 11408, 11409, 11412);
UPDATE `quest_template` SET `SrcSpell` = 43880 WHERE `entry` IN (11318, 11122, 11293, 11294);
UPDATE `quest_template` SET `SpecialFlags` = 1 WHERE `entry` IN (11293, 11294, 11407, 11408);

UPDATE `creature_template` SET `ScriptName` = 'npc_relayrace_dealer' WHERE `entry` IN (24364, 24527);
UPDATE `creature_template` SET `ScriptName` = 'npc_relayrace_master' WHERE `entry` IN (24468, 24510);

DELETE FROM `spell_scripts` WHERE `id` = 49466;
INSERT INTO `spell_scripts` (id, delay, command, datalong, datalong2, datalong3, datalong4, data_flags, dataint, dataint2, dataint3, dataint4, x, y, z, o, comments) VALUE
(49466, 1, 15, 47523, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

UPDATE `creature_template` SET `ScriptName` = 'npc_brewfest_maiden' WHERE `entry` IN (36024, 36021);


DELETE FROM `creature_questrelation` WHERE `id` = 24657 AND `quest` = 11120;
DELETE FROM `creature_questrelation` WHERE `id` = 24657 AND `quest` = 11431;
DELETE FROM `creature_questrelation` WHERE `id` = 23486 AND `quest` = 11118;
DELETE FROM `creature_questrelation` WHERE `id` = 23486 AND `quest` = 11117;
INSERT INTO `creature_questrelation` VALUES
(24657, 11120),
(24657, 11431),
(23486, 11118),
(23486, 11117);

DELETE FROM `creature_involvedrelation` WHERE `id` = 24657 AND `quest` = 11120;
DELETE FROM `creature_involvedrelation` WHERE `id` = 24657 AND `quest` = 11431;
DELETE FROM `creature_involvedrelation` WHERE `id` = 23486 AND `quest` = 11118;
DELETE FROM `creature_involvedrelation` WHERE `id` = 23486 AND `quest` = 11117;
INSERT INTO `creature_involvedrelation` VALUES
(24657, 11120),
(24657, 11431),
(23486, 11118),
(23486, 11117);

UPDATE `quest_template` SET `SrcSpell` = 66592 WHERE `entry` IN (11117, 11118, 11120, 11431);
UPDATE `creature_template` SET `unit_flags` = 512, `IconName` = 'LootAll', `faction_A` = 35, `faction_H` = 35 WHERE `entry` = 23487;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` = 23487;
INSERT INTO `npc_spellclick_spells` VALUE
(23487, 41622, 0, 0, 0, 3);

-- Disturbing The Peace
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 4112 AND `type` = 15;
INSERT INTO `achievement_criteria_requirement` VALUES
(4112, 15, 3, 0);