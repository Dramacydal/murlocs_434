-- ################### Idols ####################
-- Idol of Flaring Growth
DELETE FROM `spell_proc_event` WHERE `entry` = 67356;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procEx`) VALUE 
('67356', '7', '16', 0x40000);

-- Idol of the Black Willow
DELETE FROM `spell_proc_event` WHERE `entry` = 71178;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `procEx`) VALUE 
('71178', '7', '16', 0x40000);

-- Idol of the Lunar Eclipse
DELETE FROM `spell_proc_event` WHERE `entry` = 71176;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('71176', '7', '2097154');

-- Idol of the Crying Moon
DELETE FROM `spell_proc_event` WHERE `entry` = 71174;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`, `procFlags`, `CustomChance`) VALUE 
('71174', '7', '4096', 0x100, '262144', '100');

-- Idol of the Corruptor
DELETE FROM `spell_proc_event` WHERE `entry` = 64952;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('64952', '7', '1088');

-- Idol of Arcane Terror
DELETE FROM `spell_proc_event` WHERE `entry` = 57907;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('57907', '7', '2');
-- ################### Idols ####################

-- ################### Totems ###################
-- Totem of the Surging Sea
DELETE FROM `spell_proc_event` WHERE `entry` = 71217;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskC0`) VALUE 
('71217', '11', '16');

-- Totem of Calming Tides
DELETE FROM `spell_proc_event` WHERE `entry` = 67389;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('67389', '11', '256');

-- Totem of Electrifying Wind
DELETE FROM `spell_proc_event` WHERE `entry` = 67386;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('67386', '11', '1');

-- Bizuri's Totem of Shattered Ice
DELETE FROM `spell_proc_event` WHERE `entry` = 71198;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('71198', '11', '268435456');

-- Totem of Quaking Earth
DELETE FROM `spell_proc_event` WHERE `entry` = 67392;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskC0`) VALUE 
('67392', '11', '4');

-- Totem of Dueling
DELETE FROM `spell_proc_event` WHERE `entry` = 60765;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('60765', '11', '16777232');
-- ################### Totems ###################

-- ################### Librams ##################
-- Libram of Blinding Light
DELETE FROM `spell_proc_event` WHERE `entry` = 71191;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('71191', '10', '2097152');

-- Libram of the Eternal Tower
DELETE FROM `spell_proc_event` WHERE `entry` = 71194;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('71194', '10', '1048576');

-- Libram of Three Truths
DELETE FROM `spell_proc_event` WHERE `entry` = 71186;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('71186', '10', '32768');

-- Libram of Defiance
DELETE FROM `spell_proc_event` WHERE `entry` = 67379;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('67379', '10', '262144');

-- Libram of Valiance
DELETE FROM `spell_proc_event` WHERE `entry` = 67365;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('67365', '10', '2048');

-- Libram of Veracity
DELETE FROM `spell_proc_event` WHERE `entry` = 67363;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('67363', '10', '2147483648');

-- Libram of the Sacred Shield
DELETE FROM `spell_proc_event` WHERE `entry` = 64955;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('64955', '10', '64');
-- ################### Librams ##################

-- ################### Sigils ###################
-- Sigil of the Hanged Man
DELETE FROM `spell_proc_event` WHERE `entry` = 71226;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`) VALUE 
('71226', '15', '16', '134348800');

-- Sigil of the Bone Gryphon
DELETE FROM `spell_proc_event` WHERE `entry` = 71228;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('71228', '15', '536870912');

-- Sigil of Insolence
DELETE FROM `spell_proc_event` WHERE `entry` = 67381;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('67381', '15', '536870912');

-- Sigil of Virulence
DELETE FROM `spell_proc_event` WHERE `entry` = 67384;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`) VALUE 
('67384', '15', '16', '134348800');

-- Sigil of Deflection
DELETE FROM `spell_proc_event` WHERE `entry` = 64964;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskB0`) VALUE 
('64964', '15', '536870912');

-- Sigil of the Unfaltering Knight
DELETE FROM `spell_proc_event` WHERE `entry` = 62147;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`) VALUE 
('62147', '15', '2');
-- ################### Sigils ###################

-- ################### Items ####################
-- Nibelung
DELETE FROM `spell_proc_event` WHERE `entry` IN (71845, 71846);
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `CustomChance`, `Cooldown`) VALUES
('71845', 0x10000, '2', '0'),
('71846', 0x10000, '2', '0');

-- Heartpiece
DELETE FROM `spell_proc_event` WHERE `entry` IN (71880, 71892);
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `CustomChance`, `Cooldown`) VALUES
('71880', 0x14154, '20', '45'),
('71892', 0x14154, '20', '45');

-- procs of Necrotic Touch, Black Bruise procs
DELETE FROM `spell_proc_event` WHERE `entry` IN (71875, 71877);
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `CustomChance`, `Cooldown`) VALUES
('71875', 0x44, '100', '0'),
('71877', 0x44, '100', '0');

-- ################### Items ####################

