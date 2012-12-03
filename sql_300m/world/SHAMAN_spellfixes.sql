-- TODO: update for 4.x
/*--------------------------------------------------------------------*/
/*-----------------------Main SHAMAN spellfixes-----------------------*/
/*--------------------------------------------------------------------*/

-- Elemental Focus
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (16164);
-- INSERT INTO `spell_proc_event` (`entry`,  `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
-- (16164, 0x1C, 0, 0x00000000, 0x00000000, 0x00000000, 0x00011154, 0x00000002, 0.000000, 0.000000, 0);

-- Elemental Oath
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (51466, 51470);
-- INSERT INTO `spell_proc_event` (`entry`,  `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (51466, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000002, 0.000000, 0.000000, 0);

-- Maelstrom Weapon
-- DELETE FROM `spell_proc_event` WHERE `entry` = 53817;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`, `procEx`) VALUE
-- (53817, 11, 451, 32768, 65536);

-- Nature's Guardian (need rewrite with full data)
-- UPDATE `spell_proc_event` SET `Cooldown` = 30 WHERE `entry` IN (30881,30883,30884,30885,30886);

-- Lightning Shield
-- DELETE FROM `spell_proc_event` WHERE `entry` = 324;
-- INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `Cooldown`) VALUES
-- (324, 0x222A8, 3);

-- Earth Shield
-- DELETE FROM `spell_proc_event` WHERE `entry` = 974;
-- INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `Cooldown`) VALUES
-- (974, 0x222A8, 3);

-- Water Shield
-- DELETE FROM `spell_proc_event` WHERE `entry` = 52127;
-- INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `Cooldown`) VALUES
-- (52127, 0x222A8, 3);

-- Riptide (need rewrite with full data)
-- UPDATE `spell_proc_event` SET `SpellFamilyMaskC0` = 16 WHERE `entry` IN (16198, 16180, 16196, 16240, 16235, 16176);


/*--------------------------------------------------------------------*/
/*---------Secondary SHAMAN spellfixes (spell coef data etc.)---------*/
/*--------------------------------------------------------------------*/

-- Feral Spirit - Spirit Wolf spells and HP
-- first spell: Bash - http://www.wowhead.com/?spell=58861
-- UPDATE `creature_template` SET `spell1`='58861' WHERE `entry`='29264';

-- second spell: Spirit Hunt - http://www.wowhead.com/?spell=58877
-- UPDATE `creature_template` SET `spell2`='58877' WHERE `entry`='29264';

-- third spell: Spirit Walk - http://www.wowhead.com/?spell=58875
-- UPDATE `creature_template` SET `spell3`='58875' WHERE `entry`='29264';

-- fourth spell: Twin Howl - http://www.wowhead.com/?spell=58857
-- UPDATE `creature_template` SET `spell4`='58857' WHERE `entry`='29264';

-- fifth spell: Spirit Wolf Leap - http://www.wowhead.com/?spell=58867
-- UPDATE `creature_template` SET `spell5`='58867' WHERE `entry`='29264';

-- Spirit Wolf health points - blizzlike
-- UPDATE `creature_template` SET `minhealth`='12593',`maxhealth`='12593' WHERE `entry`='29264';
-- end of fixes for Feral Spirit - Spirit Wolf

-- DELETE FROM `spell_bonus_data` WHERE `entry` IN
-- (45284, 45286, 45287, 45288, 45289, 45290, 45291, 45292, 45293, 45294, 45295, 45296, 49239, 49240,
-- 45297, 45298, 45299, 45300, 45301, 45302, 49268, 49269);

-- INSERT INTO `spell_bonus_data` (`entry`, `comments`) VALUES
-- (45284, 'Shaman - Lightning Overload - Lightning (Rank 1)'),
-- (45286, 'Shaman - Lightning Overload - Lightning (Rank 2)'),
-- (45287, 'Shaman - Lightning Overload - Lightning (Rank 3)'),
-- (45288, 'Shaman - Lightning Overload - Lightning (Rank 4)'),
-- (45289, 'Shaman - Lightning Overload - Lightning (Rank 5)'),
-- (45290, 'Shaman - Lightning Overload - Lightning (Rank 6)'),
-- (45291, 'Shaman - Lightning Overload - Lightning (Rank 7)'),
-- (45292, 'Shaman - Lightning Overload - Lightning (Rank 8)'),
-- (45293, 'Shaman - Lightning Overload - Lightning (Rank 9)'),
-- (45294, 'Shaman - Lightning Overload - Lightning (Rank 10)'),
-- (45295, 'Shaman - Lightning Overload - Lightning (Rank 11)'),
-- (45296, 'Shaman - Lightning Overload - Lightning (Rank 12)'),
-- (49239, 'Shaman - Lightning Overload - Lightning (Rank 13)'),
-- (49240, 'Shaman - Lightning Overload - Lightning (Rank 14)'),
-- (45297, 'Shaman - Lightning Overload - Chain Lightning (Rank 1)'),
-- (45298, 'Shaman - Lightning Overload - Chain Lightning (Rank 2)'),
-- (45299, 'Shaman - Lightning Overload - Chain Lightning (Rank 3)'),
-- (45300, 'Shaman - Lightning Overload - Chain Lightning (Rank 4)'),
-- (45301, 'Shaman - Lightning Overload - Chain Lightning (Rank 5)'),
-- (45302, 'Shaman - Lightning Overload - Chain Lightning (Rank 6)'),
-- (49268, 'Shaman - Lightning Overload - Chain Lightning (Rank 7)'),
-- (49269, 'Shaman - Lightning Overload - Chain Lightning (Rank 8)');