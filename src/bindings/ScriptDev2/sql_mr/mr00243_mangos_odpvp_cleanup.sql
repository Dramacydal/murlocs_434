-- Old odpvp removal
DELETE FROM `world_template` WHERE `map` = 0;	  	
DELETE FROM `world_template` WHERE `map` = 1;	  		  	
DELETE FROM `world_template` WHERE `map` = 530;	  	
DELETE FROM `world_template` WHERE `map` = 571;
	  	
UPDATE gameobject_template SET ScriptName='' WHERE entry IN (181597,181598);  	
DELETE FROM scripted_areatrigger WHERE entry IN (4162, 4168);

-- restore off sd2 world_map_scripts

DELETE FROM world_template WHERE map IN (0, 1, 530, 571);
INSERT INTO world_template VALUES
(0, 'world_map_eastern_kingdoms'),
(1, 'world_map_kalimdor'),
(530, 'world_map_outland'),
(571, 'world_map_northrend');
