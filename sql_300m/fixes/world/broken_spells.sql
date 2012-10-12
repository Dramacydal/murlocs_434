-- Strangely broken after 3.3.5 spells

-- Mage Impact
DELETE FROM `spell_proc_event` WHERE `entry` = 64343;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE
(64343, 3, 2);

-- Improved Scorch (was missing SpellFamilyMask)
DELETE FROM `spell_proc_event` WHERE `entry` = 11095;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE
(11095, 3, 16);

-- Borrowed Time (was missing SpellFamilyMask)
DELETE FROM `spell_proc_event` WHERE `entry` = 52795;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE
(52795, 6, 1);

-- Renewed Hope (was missing SpellFamilyMask)
DELETE FROM `spell_proc_event` WHERE `entry` = 57470;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA1`) VALUE
(57470, 6, 1);

-- Grace (was missing SpellFamilyMask)
DELETE FROM `spell_proc_event` WHERE `entry` = 47516;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`) VALUE
(47516, 6, 6144, 65536);

-- Enduring Winter
DELETE FROM `spell_proc_event` WHERE `entry` = 44557;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA1`) VALUE
(44557, 3, 32);

-- Inner Fire
-- DELETE FROM `spell_proc_event` WHERE `entry` = 588;
-- INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `procEx`) VALUE
-- (588, 0x2A8, 3);

