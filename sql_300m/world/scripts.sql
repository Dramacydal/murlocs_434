-- Eye of Kilrogg
UPDATE `creature_template` SET `ScriptName` = 'npc_eye_of_kilrogg' WHERE `entry` = '4277';

-- npc_experience
UPDATE `creature_template` SET `ScriptName` = 'npc_experience' WHERE `entry` IN ('35364', '35365');

-- npc_acherus_portal
UPDATE `creature_template` SET `ScriptName` = 'npc_acherus_portal' WHERE `entry` IN ('29588', '29589');

-- Pirate's Day
UPDATE `creature_template` SET `ScriptName` = 'npc_de_meza' WHERE `entry` = 28048;

--
UPDATE `creature_template` SET `gossip_menu_id` = 0, `ScriptName` = 'npc_wormhole' WHERE `entry` = 35646;

-- Aronen ICC ring restorer
UPDATE `creature_template` SET `ScriptName` = 'npc_ormus' WHERE `entry` = 38316;
