-- The Endless Hunger
UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 29519, `ReqCreatureOrGOCount1` = 1 WHERE `entry` = 12848;
UPDATE `creature` SET `spawntimesecs` = 25 WHERE `id` IN (29519, 29520, 29565, 29566, 29567);
UPDATE `gameobject` SET `spawntimesecs` = 25 WHERE `id` IN (191577, 191580, 191581, 191582, 191583, 191584, 191585, 191586, 191587, 191588, 191589, 191590);

-- An End To All Things...
UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 29150, `ReqCreatureOrGOId2` = 29104, `ReqCreatureOrGOCount1` = 150, `ReqCreatureOrGOCount2` = 10 WHERE `entry` = 12779;
UPDATE `creature` SET `spawntimesecs` = 25 WHERE `id` IN (29102, 29103, 29104);

-- Massacre At Light's Point
-- UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 28849, `ReqCreatureOrGOCount1` = 100 WHERE `entry` = 12701;
UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 28849, `ReqCreatureOrGOCount1` = 100 WHERE `entry` = 12701;
UPDATE `creature` SET `spawntimesecs` = 15 WHERE `id` IN (28833, 28834, 28850);
UPDATE `creature_template` SET `unit_flags` = `unit_flags` & ~0x100 WHERE `entry` = 28833;

-- The Gift That Keeps On Giving
UPDATE `quest_template` SET `Method` = 2, `SrcItemId` = 39253, `SrcItemCount` = 1, `ReqCreatureOrGOId1` = 28845, `ReqCreatureOrGOCount1` = 5 WHERE `entry` = 12698;
UPDATE `creature_template` SET `faction_A` = 2095, `faction_H` = 2095 WHERE `entry` = 28846;

-- Lambs To The Slaughter
UPDATE `creature` SET `spawntimesecs` = 15 WHERE `id` IN (28941);

-- Tonight We Dine In Havenshire
UPDATE `gameobject` SET `spawntimesecs` = 25 WHERE `id` = 190691;

-- Grand Theft Palomino
UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 28767, `ReqCreatureOrGOCount1` = 1 WHERE `entry` = 12680;
UPDATE `creature_template` SET `ScriptName` = '' WHERE `entry` IN (28605, 28608);

-- Into the Realm of Shadows
UPDATE `quest_template` SET `Method` = 2, `SpecialFlags` = 2, `SrcSpell` = 52357, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0, `ReqSpellCast1` = 0, `CompleteScript` = 12687 WHERE `entry` = 12687;
UPDATE `creature` SET `phasemask` = 32, `spawntimesecs` = 25 WHERE `id` = 28654 AND `map` = 609;
UPDATE `creature_template` SET `ScriptName` = 'mob_acherus_deathcharger', `flags_extra` =  `flags_extra` | 0x1000 WHERE `entry` = 28782;
UPDATE `creature_template` SET `faction_A` = 7, `faction_H` = 7, `AIName` = 'EventAI' WHERE `entry` = 28654;
DELETE FROM `creature_involvedrelation` WHERE `quest` = 12687;
INSERT INTO `creature_involvedrelation` VALUE (28788, 12687);
UPDATE `npc_spellclick_spells` SET `quest_start` = 12687, `quest_start_active` = 1 WHERE `npc_entry` = 28782;
DELETE FROM `creature_ai_scripts` WHERE `id` = 28654100;
INSERT INTO `creature_ai_scripts` (`id`, `creature_id`, `event_type`, `event_param1`, `event_param2`, `action1_type`, `action1_param1`, `action1_param2`, `action1_param3`, `comment`) VALUE
(28654100, 28654, 6, 0, 0, 12, 28782, 0, 60000, 'Dark Rider of Acherus - Summon Acherus Deathcharger on death');
DELETE FROM `quest_end_scripts` WHERE `id` = 12687;
INSERT INTO `quest_end_scripts` (`id`, `command`, `datalong`) VALUE
(12687, 14, 52275);

-- The Plaguebringer's Request
UPDATE `gameobject` SET `spawntimesecs` = 25 WHERE `id` = 190937;
UPDATE `gameobject` SET `spawntimesecs` = 25 WHERE `id` = 190938;

-- Death Comes From On High
UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 28525, `ReqCreatureOrGOId2` = 28543, `ReqCreatureOrGOId3` = 28542, `ReqCreatureOrGOId4` = 28544, `ReqCreatureOrGOCount1` = 1, `ReqCreatureOrGOCount2` = 1, `ReqCreatureOrGOCount3` = 1, `ReqCreatureOrGOCount4` = 1, `ReqSpellCast1` = 0, `ReqSpellCast2` = 0, `ReqSpellCast3` = 0, `ReqSpellCast4` = 0 WHERE `entry` = 12641;
UPDATE `creature_template` SET `flags_extra` = `flags_extra` & ~128 WHERE `entry` IN (28525, 28543, 28542, 28544);

-- The Light of Dawn
-- UPDATE `quest_template` SET `Method` = 2, `ReqCreatureOrGOId1` = 29245, `ReqCreatureOrGOCount1` = 1 WHERE `entry` = 12801;
UPDATE `quest_template` SET `Method` = 0, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0 WHERE `entry` = 12801;

-- Ambush At The Overlook
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 39646, `ReqItemId2` = 39647, `ReqItemCount1` = 1, `ReqItemCount1` = 1 WHERE `entry` = 12754;

-- The Battle For The Ebon Hold
UPDATE `quest_template` SET `Method` = 2, ReqCreatureOrGOId1 = 31099, ReqCreatureOrGOId2 = 31100, ReqCreatureOrGOCount1 = 1, ReqCreatureOrGOCount2 = 10 WHERE `entry` = 13166;
UPDATE `creature` SET `spawntimesecs` = 25, `spawndist` = 1 WHERE `id` IN (31095, 31096, 31098, 31099);

-- Bloody Breakout
-- UPDATE `gameobject` SET `phaseMask` = 4 WHERE `id` = 191092;
UPDATE `creature` SET `spawntimesecs` = 25 WHERE `id` IN (28912);

-- Warchief's Blessing & Where Kings Walk
UPDATE `gameobject` SET `phaseMask` = 257 WHERE `id` IN (193052, 193053);
-- Horde
UPDATE `gameobject_template` SET `faction` = 1801 WHERE `entry` = 193052;
UPDATE `creature_template` SET `faction_A` = 1801, `faction_H` = 1801 WHERE `entry` = 4949;
-- Alliance
UPDATE `gameobject_template` SET `faction` = 1802 WHERE `entry` = 193053;
UPDATE `creature_template` SET `faction_A` = 1802, `faction_H` = 1802 WHERE `entry` = 29611;