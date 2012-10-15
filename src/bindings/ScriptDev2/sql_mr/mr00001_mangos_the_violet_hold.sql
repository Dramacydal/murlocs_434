-- --------------
-- VIOLET HOLD  -
-- --------------

UPDATE `instance_template` SET `ScriptName`='instance_violet_hold' WHERE `map`=608;
UPDATE `creature_template` SET `ScriptName`='boss_erekem' WHERE entry=29315;
UPDATE `creature_template` SET `ScriptName`='mob_erekem_guard' WHERE entry=29395;
UPDATE `creature_template` SET `ScriptName`='boss_moragg' WHERE entry=29316;
UPDATE `creature_template` SET `ScriptName`='boss_ichoron' WHERE entry=29313;
UPDATE `creature_template` SET `ScriptName`='mob_ichor_globule' WHERE entry=29321;
UPDATE `creature_template` SET `ScriptName`='boss_xevozz' WHERE entry=29266;
UPDATE `creature_template` SET `ScriptName`='mob_ethereal_sphere' WHERE entry=29271;
UPDATE `creature_template` SET `ScriptName`='boss_lavanthor' WHERE entry=29312;
UPDATE `creature_template` SET `ScriptName`='boss_zuramat'WHERE entry=29314;
UPDATE `creature_template` SET `ScriptName`='mob_zuramat_sentry' WHERE entry=29364;
UPDATE `creature_template` SET `ScriptName`='boss_cyanigosa' WHERE entry=31134;
UPDATE `creature_template` SET `ScriptName`='npc_azure_saboteur' WHERE entry=31079;
UPDATE `creature_template` SET `ScriptName`='mob_vh_dragons', AIName='' WHERE entry IN (30666,30668,30667,32191,30660,30695,30663,30661,30664,30662);
UPDATE `creature_template` SET `ScriptName`='npc_sinclari', `npcflag`=1 WHERE entry=30658;
UPDATE `creature_template` SET `ScriptName`='npc_violet_portal' WHERE entry=31011;
UPDATE `creature_template` SET `ScriptName`='npc_door_seal_vh' WHERE entry=30896;

-- Lieuntenant Sinclari (30658)
UPDATE `creature` SET `Spawntimesecs`=180 WHERE id=30658;


-- void sentry template (29364)

UPDATE `creature_template` SET faction_A=16, faction_H=16, minlevel=77, maxlevel=77, maxhealth=500, minhealth=500 WHERE entry=29364;
UPDATE `creature_template` SET `faction_A`=16,`faction_H`=16 WHERE `entry`=31518;
UPDATE `creature_template` SET `AIName`='' WHERE entry IN (31011,30695,31079,30666,30668,30667,32191,30660,30695,30663,30661,30664,30662,31118,29395,31513);

-- triggers
UPDATE `creature_template` SET flags_extra=flags_extra|128  WHERE entry IN (30857,30883,29326,30896);

DELETE FROM `creature` WHERE id=31011;

DELETE FROM `spell_script_target` WHERE `entry` =54160;
INSERT INTO `spell_script_target` VALUES (54160, 1, 29266);
DELETE FROM `spell_script_target` WHERE `entry`=59474;
INSERT INTO `spell_script_target` VALUES
(59474,1,29266),
(59474,1,31511);

-- immune masks (charm, fear, root, silence, sleep, snare, stun, freeze, knockout, polymorph, banish, shackle, horror, sapped)

UPDATE `creature_template` SET mechanic_immune_mask=mechanic_immune_mask|1|16|64|256|512|1024|2048|4096|8192|65536|131072|524288|8388608|536870912
WHERE `entry` IN (29315,29395,29316,29313,29321,29266,29271,29312,29314,31134,31511,31514,31509,31508,31512,31507,31510,31515,31513,31506);
