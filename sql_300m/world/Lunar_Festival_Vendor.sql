-- http://www.wowhead.com/npc=15898 on map 530
DELETE FROM `game_event_creature` WHERE guid in (
SELECT guid 
FROM `creature` 
WHERE `id` = 15898 AND `map` = 530
);

DELETE FROM `creature` WHERE `id` = 15898 AND `map` = 530;
INSERT INTO `creature` (
`id` ,
`map` ,
`spawnMask` ,
`phaseMask` ,
`modelid` ,
`equipment_id` ,
`position_x` ,
`position_y` ,
`position_z` ,
`orientation` ,
`spawntimesecs` ,
`spawndist` ,
`currentwaypoint` ,
`curhealth` ,
`curmana` ,
`DeathState` ,
`MovementType` 
) VALUES (
'15898', '530', '1', '1', '0', '0', '-1752.040527', '5459.849609', '-12.428048', '0.861977', '600', '0', '0', '1500', '5013', '0', '0'
);

INSERT INTO `game_event_creature` (`guid`, `event`) 
SELECT guid, 7 
FROM `creature` 
WHERE `id` = 15898 AND `map` = 530 LIMIT 0, 1;