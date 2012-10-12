/*--------------------------------------------------------------------*/
/*-----------------------Enchantments spellfixes----------------------*/
/*--------------------------------------------------------------------*/

-- Formula: Enchant Weapon - Icebreaker
DELETE FROM `spell_proc_item_enchant` WHERE `entry` IN(44525);
INSERT INTO `spell_proc_item_enchant` (`entry`, `ppmRate`) VALUES ('44525', '10');

-- WTF? Double? Need DELETE?
DELETE FROM `spell_proc_event` WHERE `entry` IN (64568, 64571);
INSERT INTO `spell_proc_event` (`entry`,  `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (64568, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000154, 0x00000000, 0.000000, 0.000000, 0);
INSERT INTO `spell_proc_event` (`entry`,  `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES (64571, 0x00, 0, 0x00000000, 0x00000000, 0x00000000, 0x00000154, 0x00000000, 0.000000, 0.000000, 3);

-- Formula: Enchant Weapon - Blood Draining (Blood Reserve)
DELETE FROM `spell_proc_event` WHERE `entry` = 64571;
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `CustomChance`, `Cooldown`) VALUE 
('64571', 0x850014, '50', '10');
-- Blood Reserve proc
DELETE FROM `spell_proc_event` WHERE `entry` = 64568;