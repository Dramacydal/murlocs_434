/*--------------------------------------------------------------------*/
/*------------------------Item sets spellfixes------------------------*/
/*--------------------------------------------------------------------*/

-- Shaman T10 2P Restoration Bonus
DELETE FROM `spell_proc_event` WHERE `entry` = 70807;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskC0`, `procFlags`, `CustomChance`) VALUE 
('70807', '11', '16', 0x4000, '100');

-- Shaman T10 4P Restoration Bonus
DELETE FROM `spell_proc_event` WHERE `entry` = 70808;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`, `procEx`, `CustomChance`) VALUE 
('70808', '11', '256', 0x4000, 2, '100');

-- Priest T10 2P Healer Bonus
DELETE FROM `spell_proc_event` WHERE `entry` = 70770;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`, `CustomChance`) VALUE 
('70770', '6', '2048', 0x4000, '33');

-- Shaman T10 2P Enhancement Bonus
DELETE FROM `spell_proc_event` WHERE `entry` = 70830;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`, `procFlags`, `CustomChance`) VALUE 
('70830', '11', '131072', 0x8000, '100');

-- Druid T10 4P Balance Bonus
DELETE FROM `spell_proc_event` WHERE `entry` = 70723;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procEx`) VALUE 
('70723', '7', '5', 2);
DELETE FROM `spell_bonus_data` WHERE `entry` = 71023;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `comments`) VALUE
(71023, 0, 0, 0, 'Item - Druid T10 4P Balance Bonus');

-- Item - Death Knight T10 Tank 4P Bonus
DELETE FROM `spell_proc_event` WHERE `entry` = 70652;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`) VALUE 
('70652', '15', '8', 17408);

-- Item - Priest T10 Healer 4P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70799;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procFlags`) VALUE
(70799, 6, 0x00000800, 0x4000);

-- Item - Shaman T10 Elemental 2P Bonus
DELETE FROM spell_proc_event WHERE entry = 70811;
INSERT INTO spell_proc_event VALUES (70811, 0x00, 0x0B, 0x00000003, 0x00000003, 0x00000003, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0, 0, 0);

-- Item - Shaman T10 Elemental 4P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70817;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA1`, `procFlags`) VALUES
('70817', '11', 0x00001000, 0x00010000);

-- Item - Paladin T10 Holy 4P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70756;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUES
('70756', '10', 2097152);

-- Item - Paladin T10 Protection 4P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70761;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskC0`) VALUES
('70761', '10', 1);

-- Item - Warlock T10 4P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70841;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`) VALUE
('70841', '5', 4, 256);

-- Item - Rogue T10 2P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70805;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB1`, `procFlags`) VALUE
('70805', '8', 131072, 0x8000);

-- Item - Warrior T10 Melee 2P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70854;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE
('70854', '4', 16);

-- Item - Warrior T10 Melee 4P Bonus
DELETE FROM `spell_proc_event` WHERE entry = 70847;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `procFlags`) VALUE
('70847', '4', 0x4000);