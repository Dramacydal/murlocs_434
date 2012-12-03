-- TODO: update to 4.x

/*--------------------------------------------------------------------*/
/*-----------------------Main WARRIOR spellfixes----------------------*/
/*--------------------------------------------------------------------*/

-- Furious Attacks
-- DELETE FROM  spell_proc_event WHERE entry IN (46910, 46911);
-- INSERT INTO spell_proc_event (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES 
-- ('46910', '0', '0', '0', '0', '0', '0', '0', '3', '0', '0'),
-- ('46911', '0', '0', '0', '0', '0', '0', '0', '6', '0', '0');

-- Damage Shield
-- DELETE FROM `spell_proc_event` WHERE `entry` = 58872;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procEx`) VALUE
-- (58872, 127, 67);

-- Safeguard (fucking spell with null EFF_INDEX_0)
-- DELETE FROM `spell_proc_event` WHERE `entry` = 46945;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB1`, `procFlags`, `CustomChance`) VALUE
-- (46945, 4, 65536, 1024, 100);

-- Taste For Blood proc remove
-- DELETE FROM `spell_proc_event` WHERE `entry` = 60503;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procEx`) VALUE
-- (60503, 4, 4, 0x10000);

-- Glyph of Overpower proc
-- DELETE FROM `spell_proc_event` WHERE `entry` = 68051;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procEx`) VALUE
-- (68051, 4, 4, 0x10000);

-- Sudden Death proc
-- DELETE FROM `spell_proc_event` WHERE `entry` = 52437;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`, `procEx`) VALUE
-- (52437, 4, 536870912, 16, 0x10000);

-- Sudden Death
-- DELETE FROM `spell_proc_event` WHERE `entry` = 29723;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `procFlags`) VALUE
-- (29723, 0x3F, 4, 0x51014);


/*--------------------------------------------------------------------*/
/*---------Secondary WARRIOR spellfixes (spell coef data etc.)--------*/
/*--------------------------------------------------------------------*/