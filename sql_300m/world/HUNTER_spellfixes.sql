-- TODO: update for 4.x

/*--------------------------------------------------------------------*/
/*-----------------------Main HUNTER spellfixes-----------------------*/
/*--------------------------------------------------------------------*/

-- Lock and Load
-- DELETE FROM `spell_proc_event` WHERE entry = 56342;
-- INSERT INTO `spell_proc_event` VALUES (56342, 0x00, 0x09, 0x00000018, 0x00000018, 0x00000018, 0x08000000, 0x08000000, 0x08000000, 0x00064000, 0x00064000, 0x00064000, 0x00000000, 0x00000000, 0, 0, 0);

-- Lock and load triggered
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (56453);
-- INSERT INTO `spell_proc_event` VALUES (56453, 0x00, 0x09, 0x00000800, 0x00000000, 0x00000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00080000, 0, 0, 0);

-- Improved Mend Pet
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (19572);
-- INSERT INTO `spell_proc_event` VALUES (19572, 0, 0x09, 0x00800000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00040000, 0x0040000, 0, 101, 0);

-- Improved Kill Command
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (37483);
-- INSERT INTO `spell_proc_event` VALUES (37483, 0x00, 0x09, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00000000, 0, 101, 0);

-- Entrapment
-- DELETE FROM spell_proc_event WHERE entry = 19184;
-- INSERT INTO spell_proc_event VALUES (19184, 0x00, 0x09, 0x00000000, 0x00000000, 0x00000000, 0x00002000, 0x00002000, 0x00002000, 0x00040000, 0x00040000, 0x00040000, 0x00000000, 0x00000000, 0, 0, 0);

-- Cobra Strikes
-- DELETE FROM `spell_proc_event` WHERE `entry` = 53257;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `procFlags`, `procEx`) VALUES
-- (53257, 9, 0x11010, 0x2);

-- Dust Cloud
-- DELETE FROM `spell_proc_event` WHERE `entry` = 54404;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procEx`) VALUES
-- (54404, 127, 65536);

-- Deep Jormungar is not tameable
-- UPDATE `creature_template` SET `type_flags` = `type_flags` &~1 WHERE `entry` = 34920;
-- DELETE FROM `allowed_pets` WHERE `entry` = 34920;

/*--------------------------------------------------------------------*/
/*---------Secondary HUNTER spellfixes (spell coef data etc.)---------*/
/*--------------------------------------------------------------------*/