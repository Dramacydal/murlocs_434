-- TODO: update for 4.x
/*--------------------------------------------------------------------*/
/*------------------------Main PRIEST spellfixes----------------------*/
/*--------------------------------------------------------------------*/

-- Holy Concentration
-- DELETE FROM `spell_proc_event` WHERE `entry` = 34753;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`, `SpellFamilyMaskC0`, `procEx`) VALUE
-- (34753, 6, 0x1800, 0x4, 0x1000, 2);

-- Shadow Word: Death
-- DELETE FROM `spell_proc_event` WHERE `entry`=32409;
-- INSERT INTO `spell_proc_event` (`entry`,`procFlags`) VALUES
-- ('32409', '2');

-- Divine Hymn
-- DELETE FROM `spell_bonus_data` WHERE `entry` = 64844;
-- INSERT INTO `spell_bonus_data` VALUES
-- (64844, 0.5483, 0, 0, 0, 'Priest - Divine Hymn');

-- Improved Spirit Tap
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (15337);
-- INSERT INTO `spell_proc_event` VALUES (15337, 0x20, 0x06, 0x00802000, 0x00000000, 0x00000000, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000002, 0, 0, 0);

-- 33151 Surge of Light (Rank 1)
-- DELETE FROM spell_proc_event WHERE entry = 33151;

/*--------------------------------------------------------------------*/
/*---------Secondary PRIEST spellfixes (spell coef data etc.)---------*/
/*--------------------------------------------------------------------*/

-- Shadowfiend
-- UPDATE `creature_template` SET `resistance2`='0', `resistance3`='0', `resistance4`='0', `resistance5`='0', `resistance6`='0' WHERE `entry`='19668';

-- DELETE FROM `spell_bonus_data` WHERE `entry` IN ('33110', '33076');
-- INSERT INTO spell_bonus_data (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `comments`) VALUES
-- (33076, 0, 0, 0, 'Priest - Prayer of Mending'),
-- (33110, 0, 0, 0, 'Priest - Prayer of Mending Dummy Heal');
