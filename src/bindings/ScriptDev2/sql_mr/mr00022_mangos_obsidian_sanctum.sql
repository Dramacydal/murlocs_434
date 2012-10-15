-- Instance Obsidian Sanctum

-- ScriptNames
UPDATE `creature_template` SET `ScriptName` = "boss_sartharion" WHERE `entry` = 28860;
UPDATE `creature_template` SET `ScriptName` = "mob_vesperon" WHERE `entry` = 30449;
UPDATE `creature_template` SET `ScriptName` = "mob_shadron" WHERE `entry` = 30451;
UPDATE `creature_template` SET `ScriptName` = "mob_tenebron" WHERE `entry` = 30452;
UPDATE `creature_template` SET `ScriptName` = "mob_acolyte_of_shadron" WHERE `entry` IN (30688, 31218);
UPDATE `creature_template` SET `ScriptName` = "mob_acolyte_of_vesperon" WHERE `entry` IN (30858, 31219);
UPDATE `creature_template` SET `ScriptName` = "mob_twilight_eggs" WHERE `entry` = 30882;
UPDATE `creature_template` SET `ScriptName` = "mob_twilight_whelp" WHERE `entry` IN (30890, 31214);
UPDATE `creature_template` SET `ScriptName` = "mob_fire_cyclone" WHERE `entry` = 30648;
UPDATE `creature_template` SET `ScriptName` = "mob_flame_tsunami" WHERE `entry` = 30616;

-- Drakes' templates - instance binding dependent on instance script; set lootid.
UPDATE `creature_template` SET `lootid` = `entry`, `flags_extra` = `flags_extra`&~1 WHERE `entry` IN (30449, 30451, 30452, 31535, 31520, 31534);

-- fix speed of tsunami
UPDATE `creature_template` SET speed_walk = 23 WHERE entry = 30616;

-- fly, walk and chase in lava
UPDATE creature_template SET InhabitType = 7 WHERE entry IN (30451, 31520, 30452, 31534, 30449, 31535);

-- Shadow Fissure
UPDATE `creature_template` SET `AIName` = "EventAI" WHERE entry = 30641;
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 30641;
INSERT INTO `creature_ai_scripts`VALUES
(3064101, 30641, 1, 0, 100, 2, 5000, 5000, 0, 0, 11, 57581, 0, 2, 37, 0, 0, 0, 0, 0, 0, 0, "Twilight Fissure - Cast Void Blast (Sartharion)"),
(3064102, 30641, 1, 0, 100, 4, 5000, 5000, 0, 0, 11, 59128, 0, 2, 37, 0, 0, 0, 0, 0, 0, 0, "Twilight Fissure - Cast Void Blast heroic (Sartharion)");

-- Warden of The Chamber - make them appear as dead after failing to defend the sanctum
UPDATE `creature` SET `DeathState` = 1 WHERE `guid` IN (131063, 131064);

-- Hatch Eggs - target Twilight Egg Controller
DELETE FROM `spell_script_target` WHERE `entry` IN (58542, 59189);
INSERT INTO `spell_script_target` VALUES
(58542, 1, 31138),
(59189, 1, 31138);

-- handling Twilight Eggs
UPDATE `creature_template` SET `ScriptName` = "mob_twilight_eggs" WHERE `entry` = 31204;
UPDATE `creature_template` SET `ScriptName` = "mob_twilight_egg_controller" WHERE `entry` = 31138;

-- Twilight Portal
UPDATE `gameobject_template` SET `ScriptName` = "obsidian_sanctum_portals" WHERE `entry` = 193988;

-- Safe Area dummy creature - mark targets in 40yd radius to be not targeted by Sartharion's Pyrobuffet
UPDATE `creature_template` SET `spell1` = 56911 WHERE `entry` = 30494;

-- TRASH ACID
UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '30680';
UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '30681';
UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '30682';
UPDATE `creature_template` SET `AIName` = 'EventAI', `ScriptName` = '' WHERE `entry` = '30453';
DELETE FROM `creature_ai_scripts` WHERE `creature_id` IN (30680, 30681, 30682, 30453);
INSERT INTO `creature_ai_scripts` VALUES
(3068001, 30680, 0, 0, 100, 7, 5000, 10000, 5000, 10000, 11, 13737, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Brood General - Cast Mortal Strike"),
(3068002, 30680, 4, 0, 100, 2, 0, 0, 0, 0, 11, 57740, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Brood General - Cast Devotion Aura (10man)"),
(3068003, 30680, 4, 0, 100, 4, 0, 0, 0, 0, 11, 58944, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Brood General - Cast Devotion Aura (25man)"),
(3068004, 30680, 2, 0, 100, 6, 25, 1, 0, 0, 11, 57742, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Brood General - Cast Avenging Fury at 25% HP"),
(3068005, 30680, 0, 0, 100, 3, 5000, 10000, 15000, 20000, 11, 57733, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Brood General - Cast Draconic Rage (10man)"),
(3068006, 30680, 0, 0, 100, 5, 5000, 10000, 15000, 20000, 11, 58942, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Brood General - Cast Draconic Rage (25man)"),

(3068101, 30681, 0, 0, 100, 3, 5000, 10000, 10000, 15000, 11, 57757, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Blaze Mistress - Cast Rain of Fire (10man)"),
(3068102, 30681, 0, 0, 100, 5, 5000, 10000, 10000, 15000, 11, 58936, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Blaze Mistress - Cast Rain of Fire (25man)"),
(3068103, 30681, 2, 0, 100, 7, 50, 1, 20000, 25000, 11, 57753, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Blaze Mistress - Cast Conjure Flame Orb at 50% HP"),
(3068104, 30681, 0, 0, 100, 3, 5000, 10000, 10000, 15000, 11, 39529, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Blaze Mistress - Cast Flame Shock (10man)"),
(3068105, 30681, 0, 0, 100, 5, 5000, 10000, 10000, 15000, 11, 58940, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Blaze Mistress - Cast Flame Shock (25man)"),

(3068201, 30682, 0, 0, 100, 7, 7000, 12000, 7000, 12000, 11, 57759, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Flight Captain - Cast Hammer Drop"),
(3068202, 30682, 0, 0, 100, 7, 5000, 10000, 5000, 10000, 11, 58953, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Flight Captain - Cast Pummel"),

(3045301, 30453, 0, 0, 100, 3, 5000, 10000, 10000, 15000, 11, 57728, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Sanctum Guardian - Cast Shockwave (10man)"),
(3045302, 30453, 0, 0, 100, 5, 5000, 10000, 10000, 15000, 11, 58947, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Sanctum Guardian - Cast Shockwave (25man)"),
(3045303, 30453, 0, 0, 100, 6, 5000, 7000, 0, 0, 11, 58948, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Sanctum Guardian - Cast Curse of Mending"),
(3045304, 30453, 2, 0, 100, 6, 25, 1, 0, 0, 11, 53801, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Onyx Sanctum Guardian - Cast Frenzy at 25% HP");

-- Achievements
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` BETWEEN 7326 AND 7333;
INSERT INTO `achievement_criteria_requirement` VALUES
(7326, 18, 0, 0),
(7327, 18, 0, 0),
(7328, 18, 0, 0),
(7331, 18, 0, 0),
(7329, 18, 0, 0),
(7332, 18, 0, 0),
(7330, 18, 0, 0),
(7333, 18, 0, 0);
