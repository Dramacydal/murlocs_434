# ICC trash mobs from Xfurry
-- ###########
-- LOWER SPIRE
-- ###########

-- The Damned
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37011;
DELETE FROM `creature_ai_scripts` WHERE `id`=3701101;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3701101,37011,0,0,100,30,10000,15000,10000,15000,11,70960,0,0,0,0,0,0,0,0,0,0, 'The Dammed  - cast Bone Flurry');
DELETE FROM `creature_ai_scripts` WHERE `id`=3701102;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3701102,37011,6,0,100,30,0,0,0,0,11,70961,0,0,0,0,0,0,0,0,0,0, 'The Dammed  - cast Shattred Bones');
-- Ancient Skeletal Soldier
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37012;
DELETE FROM `creature_ai_scripts` WHERE `id`=3701201;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3701201,37012,0,0,100,31,5000,7000,5000,7000,11,70964,1,0,0,0,0,0,0,0,0,0, 'Ancient skeletal soldier - shield bash');
-- Servant of the Throne
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36724;
DELETE FROM `creature_ai_scripts` WHERE `id`=3672401;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3672401,36724,0,0,100,31,3000,5000,7000,9000,11,71029,0,0,0,0,0,0,0,0,0,0, 'Servant of the throne - glacial blast');
-- Nerub'ar Broodkeeper
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36725;
DELETE FROM `creature_ai_scripts` WHERE `id`=3672501;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3672501,36725,10,0,100,31,1,60,0,0,38,0,0,0,0,0,0,0,0,0,0,0, 'Nerubar broodkeeper - set aggressive');
DELETE FROM `creature_ai_scripts` WHERE `id`=3672502;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3672502,36725,0,0,100,31,5000,7000,5000,7000,11,70965,4,0,0,0,0,0,0,0,0,0, 'Nerubar Broodkeeper - crypt scarabs');
DELETE FROM `creature_ai_scripts` WHERE `id`=3672503;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3672503,36725,14,0,100,31,50,30,8000,13000,11,36725,6,0,0,0,0,0,0,0,0,0, 'Nerubar broodkeeper - dark mending');
DELETE FROM `creature_ai_scripts` WHERE `id`=3672504;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3672504,36725,0,0,100,31,7000,11000,7000,11000,11,70980,4,0,0,0,0,0,0,0,0,0, 'Nerubar broodkeeper - web wrap');
-- Deathbound Ward
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37007;
DELETE FROM `creature_ai_scripts` WHERE `id`=3700701;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3700701,37007,10,0,100,31,1,80,0,0,38,0,0,0,0,0,0,0,0,0,0,0, 'Deathbound ward - set aggressive');
DELETE FROM `creature_ai_scripts` WHERE `id`=3700702;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3700702,37007,0,0,100,31,5000,7000,5000,7000,11,71021,1,0,0,0,0,0,0,0,0,0, 'Deathbound ward - saber lash');
DELETE FROM `creature_ai_scripts` WHERE `id`=3700703;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3700703,37007,0,0,100,31,15000,20000,20000,30000,11,71022,0,0,0,0,0,0,0,0,0,0, 'Deathbound ward - disrupting shout');

-- #####################
-- ORATORY OF THE DAMMED
-- #####################

-- Deathspeaker High Priest
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36829;
DELETE FROM `creature_ai_scripts` WHERE `id`=3682901;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3682901,36829,4,0,100,30,0,0,0,0,11,69491,0,0,0,0,0,0,0,0,0,0, 'Deathspeaker high priest - aura of darkness');
DELETE FROM `creature_ai_scripts` WHERE `id`=3682902;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3682902,36829,0,0,100,31,5000,7000,10000,12000,11,69483,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker High Priest - dark reckoning');
-- Deathspeaker Attendant
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36811;
DELETE FROM `creature_ai_scripts` WHERE `id`=3681101;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3681101,36811,0,0,100,31,3000,5000,6000,8000,11,69387,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker attendant - shadow bolt');
DELETE FROM `creature_ai_scripts` WHERE `id`=3681102;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3681102,36811,0,0,100,11,10000,15000,10000,15000,11,69355,0,0,0,0,0,0,0,0,0,0, 'Deathspeaker attendant - shadow nova 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3681103;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3681103,36811,0,0,100,21,10000,15000,10000,15000,11,71106,0,0,0,0,0,0,0,0,0,0, 'Deathspeaker attendant - shadow nova 25');
-- Deathspeaker Disciple
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36807;
DELETE FROM `creature_ai_scripts` WHERE `id`=3680701;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680701,36807,0,0,100,31,3000,6000,7000,10000,11,69387,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker disciple - shadow bolt');
DELETE FROM `creature_ai_scripts` WHERE `id`=3680702;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680702,36807,16,0,100,31,69391,30,5000,9000,11,69391,6,0,0,0,0,0,0,0,0,0, 'Deathspeaker disciple - dark blessing');
DELETE FROM `creature_ai_scripts` WHERE `id`=3680703;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680703,36807,14,0,100,11,50,50,7000,11000,11,69389,6,0,0,0,0,0,0,0,0,0, 'Deathspeaker disciple - shadow mend 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3680704;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680704,36807,14,0,100,21,50,50,7000,11000,11,71107,6,0,0,0,0,0,0,0,0,0, 'Deathspeaker disciple - shadow mend 25');
-- Deathspeaker Servant
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36805;
DELETE FROM `creature_ai_scripts` WHERE `id`=3680501;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680501,36805,0,0,100,31,7000,10000,7000,10000,11,69405,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker Servant - consuming shadows');
DELETE FROM `creature_ai_scripts` WHERE `id`=3680502;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680502,36805,0,0,100,11,4000,7000,4000,7000,11,69576,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker servant - chaos bolt 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3680503;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680503,36805,0,0,100,21,4000,7000,4000,7000,11,71108,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker servant - chaos bolt 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3680504;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680504,36805,0,0,100,11,7000,13000,7000,13000,11,69404,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker servant - curse of agony 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3680505;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680505,36805,0,0,100,21,7000,13000,7000,13000,11,71112,4,0,0,0,0,0,0,0,0,0, 'Deathspeaker servant - curse of agony 25');
-- Deathspeaker Zealot
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36808;
DELETE FROM `creature_ai_scripts` WHERE `id`=3680801;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3680801,36808,0,0,100,31,3000,6000,3000,6000,11,69492,1,0,0,0,0,0,0,0,0,0, 'Deathspeaker zealot - shadow cleave');

-- ################
-- RAMPART OF SKULL
-- ################

-- Frenzied Abomination
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37546;
DELETE FROM `creature_ai_scripts` WHERE `id`=3754601;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3754601,37546,0,0,100,31,3000,5000,3000,5000,11,70191,1,0,0,0,0,0,0,0,0,0, 'Frenzied abomination - abomination cleave');
DELETE FROM `creature_ai_scripts` WHERE `id`=3754602;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3754602,37546,2,0,100,30,10,9,0,0,11,70371,0,0,0,0,0,0,0,0,0,0, 'Frenzied abomination - enrage');
-- Spire Gargoyle
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37544;
DELETE FROM `creature_ai_scripts` WHERE `id`=3754401;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3754401,37544,0,0,100,31,3000,5000,3000,5000,11,70189,4,0,0,0,0,0,0,0,0,0, 'Spire gargoyle - poison spit');
-- Spire Minion
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37545;
DELETE FROM `creature_ai_scripts` WHERE `id`=3754501;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3754501,37545,0,0,100,31,3000,6000,3000,6000,11,70396,1,0,0,0,0,0,0,0,0,0, 'Spire minion - ghoul slash');
DELETE FROM `creature_ai_scripts` WHERE `id`=3754502;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3754502,37545,0,0,100,31,30000,30000,30000,30000,11,70363,0,0,0,0,0,0,0,0,0,0, 'Spire minion - cannibalize');

-- ###########
-- PLAGUEWORKS
-- ###########

-- Blighted Abomination
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37022;
DELETE FROM `creature_ai_scripts` WHERE `id`=3702201;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3702201,37022,0,0,100,31,2000,5000,2000,5000,11,40504,1,0,0,0,0,0,0,0,0,0, 'Blighted abomination - cleave');
DELETE FROM `creature_ai_scripts` WHERE `id`=3702202;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3702202,37022,0,0,100,31,7000,15000,20000,30000,11,71150,0,0,0,0,0,0,0,0,0,0, 'Blighted abomination - plague cloud');
DELETE FROM `creature_ai_scripts` WHERE `id`=3702203;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3702203,37022,0,0,100,31,5000,10000,10000,15000,11,71140,4,0,0,0,0,0,0,0,0,0, 'Blighted abomination - scourge hook');
-- Plague Scientist
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37023;
DELETE FROM `creature_ai_scripts` WHERE `id`=3702301;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3702301,37023,0,0,100,31,10000,10000,10000,10000,11,71103,4,0,0,0,0,0,0,0,0,0, 'Plague scientist - combobulating spray');
DELETE FROM `creature_ai_scripts` WHERE `id`=3702302;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3702302,37023,0,0,100,31,8000,11000,8000,11000,11,73079,4,0,0,0,0,0,0,0,0,0, 'Plague scientist - plague blast');
DELETE FROM `creature_ai_scripts` WHERE `id`=3702303;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3702303,37023,0,0,100,31,15000,20000,15000,20000,11,69871,6,0,0,0,0,0,0,0,0,0, 'Plague scientist - plague stream');

-- Vengefull fleshreaper
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37038;
DELETE FROM `creature_ai_scripts` WHERE `id`=3703801;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3703801,37038,0,0,100,31,5000,10000,5000,10000,11,71164,5,0,0,0,0,0,0,0,0,0, 'Vengefull fleshreaper - leaping face maul');
-- Decaying colossus
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36880;
DELETE FROM `creature_ai_scripts` WHERE `id`=3688001;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3688001,36880,0,0,100,11,5000,9000,10000,15000,11,71114,0,0,0,0,0,0,0,0,0,0, 'Decaying colossus - massive stomp 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3688002;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3688002,36880,0,0,100,21,5000,9000,10000,15000,11,71115,0,0,0,0,0,0,0,0,0,0, 'Decaying colossus - massive stomp 25');

-- #############
-- CRIMSON HALLS
-- #############

-- Darkfallen Archmage
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37664;
DELETE FROM `creature_ai_scripts` WHERE `id`=3766401;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766401,37664,0,0,100,11,5000,10000,30000,35000,11,70408,0,0,0,0,0,0,0,0,0,0, 'Darkfallen archmage - amplify magic 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766402;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766402,37664,0,0,100,21,5000,10000,30000,35000,11,72336,0,0,0,0,0,0,0,0,0,0, 'Darkfallen archmage - amplify magic 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766403;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766403,37664,0,0,100,11,7000,11000,7000,11000,11,70407,0,0,0,0,0,0,0,0,0,0, 'Darkfallen archmage - blast wave 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766404;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766404,37664,0,0,100,21,7000,11000,7000,11000,11,71151,0,0,0,0,0,0,0,0,0,0, 'Darkfallen archmage - blast wave 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766405;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766405,37664,0,0,100,11,9000,13000,9000,13000,11,70409,4,0,0,0,0,0,0,0,0,0, 'Darkfallen archmage - fireball 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766406;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766406,37664,0,0,100,21,9000,13000,9000,13000,11,71153,4,0,0,0,0,0,0,0,0,0, 'Darkfallen archmage - fireball 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766407;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766407,37664,0,0,100,31,10000,10000,10000,10000,11,70410,5,0,0,0,0,0,0,0,0,0, 'Darkfallen archmage - polymorph spider');
-- Darkfallen Blood Knight
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37595;
DELETE FROM `creature_ai_scripts` WHERE `id`=3759501;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3759501,37595,4,0,100,6,0,0,0,0,11,71736,0,0,0,0,0,0,0,0,0,0, 'Darkfallen blood knight - vampiric aura');
DELETE FROM `creature_ai_scripts` WHERE `id`=3759502;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3759502,37595,0,0,100,31,5000,9000,5000,9000,11,70450,1,0,0,0,0,0,0,0,0,0, 'Darkfallen blood knight - blood mirror');
DELETE FROM `creature_ai_scripts` WHERE `id`=3759503;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3759503,37595,0,0,100,31,3000,6000,3000,6000,11,70437,1,0,0,0,0,0,0,0,0,0, 'Darkfallen blood knight - unholy strike');
-- Darkfallen Noble
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37663;
DELETE FROM `creature_ai_scripts` WHERE `id`=3766301;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766301,37663,0,0,100,31,7000,11000,7000,11000,11,70645,4,0,0,0,0,0,0,0,0,0, 'Darkfallen noble - chains of shadow');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766302;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766302,37663,0,0,100,11,3000,6000,3000,6000,11,72960,4,0,0,0,0,0,0,0,0,0, 'Darkfallen noble - shadow bolt 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766303;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766303,37663,0,0,100,21,3000,6000,3000,6000,11,72961,4,0,0,0,0,0,0,0,0,0, 'Darkfallen noble - shadow bolt 25');
-- Darkfallen Advisor
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37571;
DELETE FROM `creature_ai_scripts` WHERE `id`=3757101;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3757101,37571,0,0,100,11,5000,9000,5000,9000,11,72057,4,0,0,0,0,0,0,0,0,0, 'Darkfallen advisor - lich slap 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3757102;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3757102,37571,0,0,100,21,5000,9000,5000,9000,11,72421,4,0,0,0,0,0,0,0,0,0, 'Darkfallen advisor - lich slap 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3757103;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3757103,37571,16,0,100,31,72066,40,10000,20000,11,72066,6,0,0,0,0,0,0,0,0,0, 'Darkfallen advisor - shroud of spell warding');
DELETE FROM `creature_ai_scripts` WHERE `id`=3757104;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3757104,37571,16,0,100,31,72065,50,10000,20000,11,72065,6,0,0,0,0,0,0,0,0,0, 'Darkfallen advisor - shroud of protection');
-- Darkfallen Lieutenant
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37665;
DELETE FROM `creature_ai_scripts` WHERE `id`=3766501;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766501,37665,0,0,100,11,5000,7000,5000,7000,11,70435,1,0,0,0,0,0,0,0,0,0, 'Darkfallen lieutenant - rend flesh 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766502;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766502,37665,0,0,100,21,5000,7000,5000,7000,11,71154,1,0,0,0,0,0,0,0,0,0, 'Darkfallen lieutenant - rend flesh 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766503;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766503,37665,0,0,100,31,5000,7000,20000,25000,11,70423,1,0,0,0,0,0,0,0,0,0, 'Darkfallen lieutenant - vampiric curse');
-- Darkfallen Tactician
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37666;
DELETE FROM `creature_ai_scripts` WHERE `id`=3766601;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766601,37666,0,0,100,31,5000,7000,10000,13000,11,70432,1,0,0,0,0,0,0,0,0,0, 'Darkfallen tactitian - blood sap');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766602;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766602,37666,0,0,100,31,7000,10000,7000,10000,11,70437,1,0,0,0,0,0,0,0,0,0, 'Darkfallen tactitian - unholy strike');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766603;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766603,37666,4,0,100,6,0,0,0,0,11,70431,0,0,0,0,0,0,0,0,0,0, 'Darkfallen tactitian - shadowstep');
-- Darkfallen Commander
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37662;
DELETE FROM `creature_ai_scripts` WHERE `id`=3766201;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766201,37662,4,0,100,6,0,0,0,0,11,70750,0,0,0,0,0,0,0,0,0,0, 'Darkfallen commander - battle shout');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766202;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766202,37662,0,0,100,11,3000,5000,5000,7000,11,70449,5,0,0,0,0,0,0,0,0,0, 'Darkfallen commander - vampire rush 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3766203;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3766203,37662,0,0,100,21,3000,5000,5000,7000,11,71155,5,0,0,0,0,0,0,0,0,0, 'Darkfallen commander - vampire rush 25');

-- #########
-- FROSTWING
-- #########

-- Frostwarden handler
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37531;
DELETE FROM `creature_ai_scripts` WHERE `id`=3753101;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3753101,37531,0,0,100,11,6000,11000,6000,11000,11,71337,0,0,0,0,0,0,0,0,0,0, 'Frostwarde handler - concussive shock 10');
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37531;
DELETE FROM `creature_ai_scripts` WHERE `id`=3753102;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3753102,37531,0,0,100,21,6000,11000,6000,11000,11,71338,0,0,0,0,0,0,0,0,0,0, 'Frostwarde handler - concussive shock 10');
-- Frostwing whelp
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37532;
DELETE FROM `creature_ai_scripts` WHERE `id`=3753201;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3753201,37532,0,0,100,31,5000,7000,10000,13000,11,71350,4,0,0,0,0,0,0,0,0,0, 'Frostwing whelp - focus fire');
DELETE FROM `creature_ai_scripts` WHERE `id`=3753202;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3753202,37532,0,0,100,11,4000,5000,4000,5000,11,71361,4,0,0,0,0,0,0,0,0,0, 'Frostwing whelp - frost blast 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3753203;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3753203,37532,0,0,100,21,4000,5000,4000,5000,11,71362,4,0,0,0,0,0,0,0,0,0, 'Frostwing whelp - frost blast 25');
-- Ymirjar battle maiden
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37132;
DELETE FROM `creature_ai_scripts` WHERE `id`=3713201;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713201,37132,0,0,100,31,4000,6000,10000,11000,11,71258,0,0,0,0,0,0,0,0,0,0, 'Ymirjar battle maiden - adrenaline rush');
DELETE FROM `creature_ai_scripts` WHERE `id`=3713202;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713202,37132,0,0,100,31,5000,7000,5000,7000,11,71257,3,0,0,0,0,0,0,0,0,0, 'Ymirjar battle maiden - barbaric strike');
-- Ymirjar deathbringer
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=38125;
DELETE FROM `creature_ai_scripts` WHERE `id`=3812501;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3812501,38125,0,0,100,31,4000,6000,6000,8000,11,71298,4,0,0,0,0,0,0,0,0,0, 'Ymirjar deathbringer - banish');
DELETE FROM `creature_ai_scripts` WHERE `id`=3812502;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3812502,38125,0,0,100,11,7000,13000,15000,20000,11,71299,0,0,0,0,0,0,0,0,0,0, 'Ymirjar deathbringer - deaths embrace 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3812503;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3812503,38125,0,0,100,21,7000,13000,15000,20000,11,71300,0,0,0,0,0,0,0,0,0,0, 'Ymirjar deathbringer - deaths embrace 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3812504;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3812504,38125,0,0,100,11,4000,7000,4000,7000,11,71296,4,0,0,0,0,0,0,0,0,0, 'Ymirjar deathbringer - shadow bolt 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3812505;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3812505,38125,0,0,100,21,4000,7000,4000,7000,11,71297,4,0,0,0,0,0,0,0,0,0, 'Ymirjar deathbringer - shadow bolt 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3812506;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3812506,38125,16,0,100,31,69929,30,35000,50000,11,69929,6,0,0,0,0,0,0,0,0,0, 'Ymirjar deathbringer - spirit steam');
DELETE FROM `creature_ai_scripts` WHERE `id`=3812507;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3812507,38125,0,0,100,31,20000,25000,25000,30000,11,71303,0,0,0,0,0,0,0,0,0,0, 'Ymirjar deathbringer - summon ymirjar');
-- Ymirjar warlord
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37133;
DELETE FROM `creature_ai_scripts` WHERE `id`=3713301;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713301,37133,0,0,100,31,8000,13000,8000,13000,11,41056,0,0,0,0,0,0,0,0,0,0, 'Ymirjar warlord - whirlwind');
-- Ymirjar huntress
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37134;
DELETE FROM `creature_ai_scripts` WHERE `id`=3713401;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713401,37134,0,0,100,31,5000,10000,7000,14000,11,71249,0,0,0,0,0,0,0,0,0,0, 'Ymirjar huntress - ice trap');
DELETE FROM `creature_ai_scripts` WHERE `id`=3713402;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713402,37134,0,0,100,31,8000,13000,10000,15000,11,71251,0,0,0,0,0,0,0,0,0,0, 'Ymirjar huntress - rapid shot');
DELETE FROM `creature_ai_scripts` WHERE `id`=3713403;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713403,37134,0,0,100,31,2000,4000,2000,4000,11,71253,4,0,0,0,0,0,0,0,0,0, 'Ymirjar huntress - shoot');
DELETE FROM `creature_ai_scripts` WHERE `id`=3713404;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713404,37134,0,0,100,31,6000,8000,7000,10000,11,71252,4,0,0,0,0,0,0,0,0,0, 'Ymirjar huntress - volley');
DELETE FROM `creature_ai_scripts` WHERE `id`=3713405;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3713405,37134,4,0,100,6,0,0,0,0,11,71705,0,0,0,0,0,0,0,0,0,0, 'Ymirjar huntress - summon warhawk');
-- Ymirjar frostbinder
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37127;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712701;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712701,37127,0,0,100,31,7000,10000,7000,10000,11,71306,5,0,0,0,0,0,0,0,0,0, 'Ymirjar frostbinder - twisted winds');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712702;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712702,37127,0,0,100,31,15000,20000,15000,20000,11,71274,5,0,0,0,0,0,0,0,0,0, 'Ymirjar frostbinder - frozen orb');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712703;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712703,37127,4,0,100,6,0,0,0,0,11,71270,0,0,0,0,0,0,0,0,0,0, 'Ymirjar frostbinder - arctic chill');

#-- ###############
#-- VALITHRIA EVENT
#-- ###############

#-- Grottnous abomination
#UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37886;
#DELETE FROM `creature_ai_scripts` WHERE `id`=3788601;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3788601,37886,0,0,100,3,7000,13000,7000,13000,11,70633,0,0,0,0,0,0,0,0,0,0, 'Gluttnous abomination - gut spray 10n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3788602;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3788602,37886,0,0,100,5,7000,13000,7000,13000,11,71283,0,0,0,0,0,0,0,0,0,0, 'Gluttnous abomination - gut spray 25n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3788603;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3788603,37886,0,0,100,9,7000,13000,7000,13000,11,72025,0,0,0,0,0,0,0,0,0,0, 'Gluttnous abomination - gut spray 10h');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3788604;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3788604,37886,0,0,100,17,7000,13000,7000,13000,11,72026,0,0,0,0,0,0,0,0,0,0, 'Gluttnous abomination - gut spray 25h');
#-- Blistering Zombies
#UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37934;
#DELETE FROM `creature_ai_scripts` WHERE `id`=3793401;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3793401,37934,4,0,100,31,0,0,0,0,11,70749,0,0,0,0,0,0,0,0,0,0, 'Blistering zombies - corrosion');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3793402;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3793402,37934,6,0,100,3,0,0,0,0,11,70744,0,0,0,0,0,0,0,0,0,0, 'Blistering zombies - acid burst 10n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3793403;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3793403,37934,6,0,100,5,0,0,0,0,11,71733,0,0,0,0,0,0,0,0,0,0, 'Blistering zombies - acid burst 25n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3793404;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3793404,37934,6,0,100,9,0,0,0,0,11,72017,0,0,0,0,0,0,0,0,0,0, 'Blistering zombies - acid burst 10h');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3793405;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3793405,37934,6,0,100,17,0,0,0,0,11,72018,0,0,0,0,0,0,0,0,0,0, 'Blistering zombies - acid burst 25h');
#-- Suppresser
#UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37863;
#DELETE FROM `creature_ai_scripts` WHERE `id`=3786301;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3786301,37863,4,0,100,31,0,0,0,0,11,70588,1,0,0,0,0,0,0,0,0,0, 'Suppresser - suppression');
#-- Blazing Skeleton
#UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=36791;
#DELETE FROM `creature_ai_scripts` WHERE `id`=3679101;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3679101,36791,0,0,100,3,5000,10000,5000,10000,11,70754,4,0,0,0,0,0,0,0,0,0, 'Blazin skeleton - fireball 10n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3679102;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3679102,36791,0,0,100,5,5000,10000,5000,10000,11,71748,4,0,0,0,0,0,0,0,0,0, 'Blazin skeleton - fireball 25n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3679103;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3679103,36791,0,0,100,9,5000,10000,5000,10000,11,72023,4,0,0,0,0,0,0,0,0,0, 'Blazin skeleton - fireball 10h');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3679104;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3679104,36791,0,0,100,17,5000,10000,5000,10000,11,72024,4,0,0,0,0,0,0,0,0,0, 'Blazin skeleton - fireball 25h');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3679105;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3679105,36791,0,0,100,11,10000,15000,10000,15000,11,69325,0,0,0,0,0,0,0,0,0,0, 'Blazin skeleton - lay waste 10');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3679106;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3679106,36791,0,0,100,21,10000,15000,10000,15000,11,71730,0,0,0,0,0,0,0,0,0,0, 'Blazin skeleton - lay waste 10');
-- Mana void
-- UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=38068;
-- DELETE FROM `creature_ai_scripts` WHERE `id`=3806801;
-- INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
-- (3806801,38068,4,0,100,31,0,0,0,0,11,71085,0,0,0,0,0,0,0,0,0,0, 'Mana void - mana void aura');
#UPDATE `creature_template` SET `modelid1` = 11686 WHERE `entry` = 38068;
#-- Risen archmage
#UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37868;
#DELETE FROM `creature_ai_scripts` WHERE `id`=3786801;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3786801,37868,0,0,100,3,7000,11000,7000,11000,11,70759,0,0,0,0,0,0,0,0,0,0, 'Risen archmage - frostbolt volley 10n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3786802;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3786802,37868,0,0,100,5,7000,11000,7000,11000,11,71889,0,0,0,0,0,0,0,0,0,0, 'Risen archmage - frostbolt volley 25n');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3786803;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3786803,37868,0,0,100,9,7000,11000,7000,11000,11,72015,0,0,0,0,0,0,0,0,0,0, 'Risen archmage - frostbolt volley 10h');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3786804;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3786804,37868,0,0,100,17,7000,11000,7000,11000,11,72016,0,0,0,0,0,0,0,0,0,0, 'Risen archmage - frostbolt volley 25h');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3786805;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3786805,37868,0,0,100,11,25000,30000,25000,30000,11,71179,0,0,0,0,0,0,0,0,0,0, 'Risen archmage - mana void 10');
#DELETE FROM `creature_ai_scripts` WHERE `id`=3786806;
#INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
#(3786806,37868,0,0,100,21,25000,30000,25000,30000,11,71741,0,0,0,0,0,0,0,0,0,0, 'Risen archmage - mana void 10');

-- ############
-- SVALNA EVENT
-- ############

-- Captain Rupert
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37125;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712501;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712501,37125,0,0,100,31,5000,7000,7000,11000,11,71787,4,0,0,0,0,0,0,0,0,0, 'Captain Rupert - fel bomb');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712502;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712502,37125,0,0,100,31,7000,9000,9000,13000,11,71594,4,0,0,0,0,0,0,0,0,0, 'Captain Rupert - machine gun');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712503;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712503,37125,0,0,100,31,10000,13000,15000,17000,11,71786,4,0,0,0,0,0,0,0,0,0, 'Captain Rupert - rocket');
-- Captain Grondel
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37124;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712401;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712401,37124,0,0,100,31,10000,10000,10000,10000,11,71553,5,0,0,0,0,0,0,0,0,0, 'Captain Groundel - charge');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712402;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712402,37124,0,0,100,31,5000,10000,10000,15000,11,71554,3,0,0,0,0,0,0,0,0,0, 'Captain Groundel - sunder armor');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712403;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712403,37124,0,0,100,31,5000,7000,5000,7000,11,71552,1,0,0,0,0,0,0,0,0,0, 'Captain Groundel - mortal strike');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712404;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712404,37124,0,0,100,31,5000,9000,10000,15000,11,71785,4,0,0,0,0,0,0,0,0,0, 'Captain Groundel - conflagration');
-- Captain Brandon
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37123;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712301;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712301,37123,0,0,100,31,10000,10000,10000,10000,11,71549,1,0,0,0,0,0,0,0,0,0, 'Captain Brandon - crusader strike');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712302;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712302,37123,2,0,100,30,25,24,0,0,11,71550,0,0,0,0,0,0,0,0,0,0, 'Captain Brandon - divine shield');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712303;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712303,37123,0,0,100,31,5000,8000,7000,10000,11,71784,4,0,0,0,0,0,0,0,0,0, 'Captain Brandon - hammer of betrayal');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712304;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712304,37123,0,0,100,31,7000,10000,8000,12000,11,71551,4,0,0,0,0,0,0,0,0,0, 'Captain Brandon - judgement of command');
-- Captain Arnath
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37122;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712201;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712201,37122,14,0,100,31,50,30,5000,10000,11,71595,6,0,0,0,0,0,0,0,0,0, 'Captain Arnath - flash heal');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712202;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712202,37122,0,0,100,21,15000,20000,20000,25000,11,14515,4,0,0,0,0,0,0,0,0,0, 'Captain Arnath - dominate mind');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712203;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712203,37122,16,0,100,31,71548,30,30000,35000,11,71548,6,0,0,0,0,0,0,0,0,0, 'Captain Arnath - power shield');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712204;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712204,37122,0,0,100,3,5000,10000,9000,15000,11,71546,4,0,0,0,0,0,0,0,0,0, 'Captain Arnath - smite 10n');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712205;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712205,37122,0,0,100,5,5000,10000,9000,15000,11,71547,4,0,0,0,0,0,0,0,0,0, 'Captain Arnath - smite 25n');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712206;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712206,37122,0,0,100,9,5000,10000,9000,15000,11,71778,4,0,0,0,0,0,0,0,0,0, 'Captain Arnath - smite 10h');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712207;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712207,37122,0,0,100,17,5000,10000,9000,15000,11,71779,4,0,0,0,0,0,0,0,0,0, 'Captain Arnath - smite 25h');
-- Crok and Svalna need SD2 script
-- this is just a basic of spells
-- Crok Scourgebane
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37129;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712901;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712901,37129,0,0,100,31,5000,7000,7000,9000,11,71489,1,0,0,0,0,0,0,0,0,0, 'Crock Scrougebane - deathstrike');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712902;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712902,37129,0,0,100,31,7000,9000,10000,12000,11,71488,1,0,0,0,0,0,0,0,0,0, 'Crock Scrougebane - scourgestrike');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712903;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712903,37129,0,0,100,21,10000,15000,10000,15000,11,71490,4,0,0,0,0,0,0,0,0,0, 'Crock Scrougebane - deathcoil');
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37129;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712904;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712904,37129,4,0,100,6,0,0,0,0,39,30,0,0,0,0,0,0,0,0,0,0, 'Crock Scrougebane - call for help');
-- Svalna spear
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=38248;
DELETE FROM `creature_ai_scripts` WHERE `id`=3824801;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3824801,38248,4,0,100,30,0,0,0,0,11,71443,3,0,0,0,0,0,0,0,0,0, 'Svalna spear - impale');
DELETE FROM `creature_ai_scripts` WHERE `id`=3824802;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3824802,38248,6,0,100,30,0,0,0,0,28,1,71443,0,0,0,0,0,0,0,0,0, 'Svalna spear - remove impale');
-- Svalna
UPDATE `creature_template` SET `AIName`='EventAI' WHERE `entry`=37126;
DELETE FROM `creature_ai_scripts` WHERE `id`=3712601;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712601,37126,4,0,100,6,0,0,0,0,11,71465,0,0,0,0,0,0,0,0,0,0, 'Sister Svalna - divine surge');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712602;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712602,37126,0,0,100,11,15000,20000,20000,25000,11,71468,0,0,0,0,0,0,0,0,0,0, 'Sister Svalna - aether burst 10');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712603;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712603,37126,0,0,100,21,15000,20000,20000,25000,11,71469,0,0,0,0,0,0,0,0,0,0, 'Sister Svalna - aether burst 25');
DELETE FROM `creature_ai_scripts` WHERE `id`=3712604;
INSERT INTO `creature_ai_scripts` (`id`,`creature_id`,`event_type`,`event_inverse_phase_mask`,`event_chance`,`event_flags`,`event_param1`,`event_param2`,`event_param3`,`event_param4`,`action1_type`,`action1_param1`,`action1_param2`,`action1_param3`,`action2_type`,`action2_param1`,`action2_param2`,`action2_param3`,`action3_type`,`action3_param1`,`action3_param2`,`action3_param3`,`comment`) VALUES 
(3712604,37126,0,0,100,31,25000,30000,60000,70000,11,70196,5,0,11,71463,0,0,0,0,0,0, 'Sister Svalna - sprear');
