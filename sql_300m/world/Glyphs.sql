-- TODO: update for 4.x

/*--------------------------------------------------------------------*/
/*--------------------------Glyph spellfixes--------------------------*/
/*--------------------------------------------------------------------*/


-- Glyph of Shred
-- DELETE FROM `spell_proc_event` WHERE `entry` = 54815;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`, `CustomChance`) VALUE
-- ('54815', '7', '32768', '16', '100');

-- Glyph of Starfire
-- DELETE FROM `spell_proc_event` WHERE `entry` = 54845;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`, `CustomChance`) VALUE
-- ('54845', '7', '4', 0x10000, '100');

-- Glyph of Backstab
-- DELETE FROM `spell_proc_event` WHERE `entry` = 56800;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`, `CustomChance`) VALUE
-- ('56800', '8', '8388612', '16', '100');

-- Glyph of Blocking
-- DELETE FROM `spell_proc_event` WHERE `entry` = 58375;
-- INSERT INTO spell_proc_event (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`, `procFlags`) VALUES
-- (58375, 4, 512, 0x10);

-- Glyph of Devastate
-- DELETE FROM `spell_proc_event` WHERE `entry` = 58388;
-- INSERT INTO spell_proc_event (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`, `procFlags`) VALUES
-- (58388, 4, 64, 0x10);

-- Glyph of Icy Veins
-- DELETE FROM `spell_proc_event` WHERE `entry` = 56374;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`, `procFlags`, `CustomChance`) VALUE
-- ('56374', '3', '16384', 0x4000, '100');

-- Glyph of Sunden Armor
-- DELETE FROM `spell_proc_event` WHERE `entry` = 58387;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `SpellFamilyMaskB0`, `SpellFamilyMaskB1`, `SpellFamilyMaskB2`, `procFlags`) VALUE
-- (58387, 4, 0x4000, 0x4000, 0x4000, 0x40, 0x40, 0x40, 0x10);

-- Glyph of Prayer of Healing
-- DELETE FROM `spell_proc_event` WHERE `entry` = 55680;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE
-- (55680, 6, 512);

-- Glyph of Rejuvenation
-- DELETE FROM `spell_proc_event` WHERE `entry` = 54754;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procEx`) VALUE
-- (54754, 7, 16, 0x40000);