/*--------------------------------------------------------------------*/
/*-----------------Trinkets & Rings item spellfixes-------------------*/
/*--------------------------------------------------------------------*/


-- (67653) Coliseum 5 Tank Trinket ()
DELETE FROM `spell_proc_event` WHERE `entry` IN (67653);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (67653, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000008, 0x00000000, 0.000000, 0.000000, 45);

-- (67670) Coliseum 5 CasterTrinket ()
DELETE FROM `spell_proc_event` WHERE `entry` IN (67670);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (67670, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00010000, 0x00000000, 0.000000, 0.000000, 45);

-- (67672) Coliseum 5 Melee Trinket ()
DELETE FROM `spell_proc_event` WHERE `entry` IN (67672);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (67672, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000044, 0x00000000, 0.000000, 0.000000, 45);

-- (67667) Coliseum 5 Healer Trinket ()
DELETE FROM `spell_proc_event` WHERE `entry` IN (67667);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (67667, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00004000, 0x00000000, 0.000000, 0.000000, 45);

-- some rings proc
DELETE FROM `spell_proc_event` WHERE `entry` IN (72413, 72415, 72417, 72419);
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`) VALUES (72415, 10, 60); 
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUES (72417, 10, 60, 0x00010000); 
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUES (72413, 10, 60, 0x00000154); 
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUES (72419, 10, 60, 0x00004000);

-- Nevermelting Ice Crystal
DELETE FROM `spell_proc_event` WHERE `entry` = 71563;
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `procEx`) VALUES ('71563', '65536', '2');

-- Needle-Encrusted Scorpion
DELETE FROM `spell_proc_event` WHERE `entry` = 71404;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`, `procEx`) VALUE 
('71404', '10', '45', '340', '2');

-- Pyrite Infuser
DELETE FROM `spell_proc_event` WHERE `entry` = 65013;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`, `procEx`) VALUE 
('65013', '10', '45', '340', '2');

-- Mjolnir Runestone
DELETE FROM `spell_proc_event` WHERE `entry` = 65020;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUE 
('65020', '15', '45', '340');

-- Elemental Focus Stone
DELETE FROM `spell_proc_event` WHERE `entry` = 65005;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUE 
('65005', '10', '45', '65536');

-- Dark Matter
DELETE FROM `spell_proc_event` WHERE `entry` = 65025;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUE 
('65025', '10', '45', '340');

-- Comet's Trail
DELETE FROM `spell_proc_event` WHERE `entry` = 64786;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUE 
('64786', '10', '45', '340');

-- Corpse Tongue Coin
DELETE FROM `spell_proc_event` WHERE `entry` = 71634;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUE 
('71634', '0', '30', '8');

-- Corpse Tongue Coin (Heroic)
DELETE FROM `spell_proc_event` WHERE `entry` = 71640;
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUE 
('71640', '0', '30', '8');

-- Phylactery of the Nameless Lich
DELETE FROM `spell_proc_event` WHERE `entry` = 71606;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`) VALUE 
('71606', '127', '100');

-- Phylactery of the Nameless Lich (Heroic)
DELETE FROM `spell_proc_event` WHERE `entry` = 71637;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`) VALUE 
('71637', '127', '100');

-- Dislodged Foreign Object
DELETE FROM `spell_proc_event` WHERE `entry` = 71602;
INSERT INTO `spell_proc_event` (`entry`, `cooldown`, `procFlags`) VALUES (71602, 45, 0x00010000);

-- Dislodged Foreign Object (Heroic)
DELETE FROM `spell_proc_event` WHERE `entry` = 71645;
INSERT INTO `spell_proc_event` (`entry`, `cooldown`, `procFlags`) VALUES (71645, 45, 0x00010000);

-- Pandora's Plea
DELETE FROM `spell_proc_event` WHERE `entry` = 64742;
INSERT INTO `spell_proc_event` (`entry`, `Cooldown`) VALUE 
('64742', '45');

-- Charred Twilight Scale
DELETE FROM `spell_proc_event` WHERE `entry` = 75465;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`) VALUE 
('75465', '127', '45');

-- Charred Twilight Scale (Heroic)
DELETE FROM `spell_proc_event` WHERE `entry` = 75474;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`) VALUE 
('75474', '127', '45');

-- Sharpened Twilight Scale
DELETE FROM `spell_proc_event` WHERE `entry` = 75455;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`) VALUE 
('75455', '127', '45');

-- Sharpened Twilight Scale  (Heroic)
DELETE FROM `spell_proc_event` WHERE `entry` = 75457;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`) VALUE 
('75457', '127', '45');

-- Althor's Abacus (Normal & Heroic)
DELETE FROM `spell_proc_event` WHERE `entry` IN (71642, 71611);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`, `procFlags`) VALUES (71642, 127, 45, 0x00004000);
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `Cooldown`, `procFlags`) VALUES (71611, 127, 45, 0x00004000);

-- (71585) Item - Icecrown 25 Emblem Healer Trinket (Purified Lunar Dust)
DELETE FROM spell_proc_event WHERE entry = 71585;
INSERT INTO spell_proc_event VALUES (71585, 127, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0, 0, 45);

-- Whispering Fanged Skull (Normal & Heroic)
DELETE FROM `spell_proc_event` WHERE `entry` IN (71540, 71402);
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUES (71540, 35, 45, 0x00000154); 
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUES (71402, 35, 45, 0x00000154);

-- Item - Coliseum 25 Normal Caster Trinket
-- Item - Coliseum 25 Heroic Caster Trinket
DELETE FROM `spell_proc_event` WHERE `entry` IN (67712, 67758);
INSERT INTO `spell_proc_event` (`entry`, `procEx`, `Cooldown`) VALUES
(67712, 0x2, 2),
(67758, 0x2, 2);

-- Solace of the Defeated
-- Solace of the Fallen
DELETE FROM `spell_proc_event` WHERE `entry` IN (67698, 67752);
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `procEx`) VALUES
(67698, 0x54000, 0x40003),
(67752, 0x54000, 0x40003);

-- Deathbringer's Will, Normal & Heroic
DELETE FROM `spell_proc_event` WHERE `entry` IN ('71562', '71519');
INSERT INTO `spell_proc_event` (`entry`, `CustomChance`, `Cooldown`, `procFlags`) VALUES 
('71562', '28', '90', '340'),
('71519', '28', '90', '340');