-- TODO: update to 4.x

/*--------------------------------------------------------------------*/
/*-----------------------Main WARLOCK spellfixes----------------------*/
/*--------------------------------------------------------------------*/


-- Decimation (buff proc from soul fire too)
-- DELETE FROM `spell_proc_event` WHERE `entry` = 63156;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`) VALUE
-- (63156, 5, 1, 192);

-- Everlasting Affliction
-- DELETE FROM `spell_proc_event` WHERE `entry` = 47201;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`, `procEx`) VALUE
-- ('47201', '5', '16393', '262144', '65536');

-- Health Funnel
-- DELETE FROM `spell_bonus_data` WHERE `entry` = 755;
-- INSERT INTO `spell_bonus_data` (`entry`, `dot_bonus`) VALUE
-- (755, 0.548);

-- Molten Core
-- DELETE FROM `spell_proc_event` WHERE `entry` = 47245;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE
-- ('47245', '5', '2');

-- Improved Shadow Bolt
-- DELETE FROM `spell_proc_event` WHERE `entry` = 17793;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE
-- (17793, 5, 1);

-- Pyroclasm
-- DELETE FROM `spell_proc_event` WHERE entry IN (18096, 18073, 63245);
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`, `procEx`) VALUES
-- (18096, 5, 0x100, 0x800000, 2);
-- DELETE FROM `spell_chain` WHERE `first_spell` = 18096;

-- Warlock T5 2 part set
-- DELETE FROM `spell_proc_event` WHERE `entry` = 37381;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, SpellFamilyName, `CustomChance`) VALUE
-- (37381, 127, 5, 100);
-- DELETE FROM `spell_bonus_data` WHERE `entry` = 37382;
-- INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `comments`) VALUE
-- (37382, 0, 0, 'Warlock - Health Link (T5 set)');

/*--------------------------------------------------------------------*/
/*---------Secondary WARLOCK spellfixes (spell coef data etc.)--------*/
/*--------------------------------------------------------------------*/

-- DELETE FROM `spell_bonus_data` WHERE `entry` IN (30108, 31117);
-- INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `comments`) VALUE
-- (30108, 0, 0.2, 'Warlock - Unstable Affliction dot'),
-- (31117, 1.8, 0, 'Warlock - Unstable Affliction dispel damage');