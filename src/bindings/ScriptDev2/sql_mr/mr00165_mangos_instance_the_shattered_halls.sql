-- Instance The Shattered Halls
-- ------------------------

-- ------------
-- Shattered Hand Executioner
-- ------------
UPDATE `creature_template` SET `unit_flags` = 3 WHERE `entry` = 20585; -- adding not attackable flag, removed by script on boss death

-- ------------
-- Prisoner Officers
-- ------------
UPDATE `creature_template` SET `npcflag` = 2 WHERE `entry` = 17296; -- gossip flag added after death of Shattered Hand Executioner
UPDATE `creature_template` SET `npcflag` = 2 WHERE `entry` = 17290;

-- adding Shattered Hand Executioner
DELETE FROM creature WHERE id IN (17301);

-- scripted areatriggers
DELETE FROM scripted_areatrigger WHERE entry IN (4182, 4183, 4524);
INSERT INTO scripted_areatrigger (`entry`, `ScriptName`) VALUES 
('4524', 'at_shattered_halls');
-- ---------------------------------------------
-- InstanceFixes and Related Data --------------
-- ---------------------------------------------
