/*--------------------------------------------------------------------*/
/*----------------Item spellfixes (gems, weapons etc.)----------------*/
/*--------------------------------------------------------------------*/


-- Stormchops
DELETE FROM spell_bonus_data where entry in (43731, 43733);
INSERT INTO spell_bonus_data (entry, direct_bonus, dot_bonus, ap_bonus) VALUES
(43731, 0, 0, 0), (43733, 0, 0, 0);

--  Zod's Repeating Longbow
DELETE FROM `spell_proc_event` WHERE `entry` IN (71835, 71836);
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `CustomChance`) VALUES
(71835, 320, 4),
(71836, 320, 5);

-- Val'anyr, Hammer of Ancient Kings
DELETE FROM `spell_proc_event` WHERE `entry` = 64415;
INSERT INTO `spell_proc_event` (`entry`, `Cooldown`) VALUE
(64415, 45);

-- Insightful Earthsiege Diamond
DELETE FROM `spell_proc_event` WHERE `entry` = 55381;
INSERT INTO `spell_proc_event` (`entry`, `Cooldown`, `procFlags`, `procEx`) VALUE 
(55381, 15, 0x54000, 0x40003);

-- Invigorating Earthsiege Diamond
DELETE FROM `spell_proc_event` WHERE `entry` = 61356;
INSERT INTO `spell_proc_event` (`entry`, `procFlags`, `procEx`, `Cooldown`) VALUE 
('61356', '69972', '2', '45');

-- Shadowmourne
DELETE FROM `spell_proc_event` WHERE `entry` = 71903;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procFlags`, `CustomChance`) VALUE 
(71903, 127, 0x51014, 20);

DELETE FROM `spell_bonus_data` WHERE `entry` IN (7712, 7714, 10577, 16614, 18798, 27655, 28788, 38395, 55756, 6297, 13897, 20004, 28005, 20006, 44525, 28715, 38616, 43731, 43733, 46579);
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES
-- Items
(7712, 0, 0, 0, 0, 'Item - Blazefury Medallion & Fiery Retributor (Fire Strike)'),
(7714, 0, 0, 0, 0, 'Item - Fiery Plate Gauntlets (Fire Strike)'),
(10577, 0, 0, 0, 0, 'Item - Gauntlets of the Sea (Heal)'),
(16614, 0, 0, 0, 0, 'Item - Storm Gauntlets (Lightning Strike)'),
(18798, 0, 0, 0, 0, 'Item - Freezing Band (Freeze)'),
(27655, 0, 0, 0, 0, 'Item - Heart of Wyrmthalak (Flame Lash)'),
(28788, 0, 0, 0, 0, 'Item - Paladin T3 (8)'),
(38395, 0, 0, 0, 0, 'Item - Warlock T5 (2)'),
(55756, 0, 0, 0, 0, 'Item - Brunnhildar weapons (Chilling Blow)'),
-- Enchants
(6297, 0, 0, 0, 0, 'Enchant - Fiery Blaze'),
(13897, 0, 0, 0, 0, 'Enchant - Fiery Weapon'),
(20004, 0, 0, 0, 0, 'Enchant - Lifestealing'),
(28005, 0, 0, 0, 0, 'Enchant - Battlemaster'),
(20006, 0, 0, 0, 0, 'Enchant - Unholy Weapon'),
(44525, 0, 0, 0, 0, 'Enchant - Icebreaker'),
(46579, 0, 0, 0, 0, 'Enchant - Deathfrost'),
-- Consumables
(28715, 0, 0, 0, 0, 'Consumable - Flamecap (Flamecap Fire)'),
(38616, 0, 0, 0, 0, 'Poison - Bloodboil Poison'),
(43731, 0, 0, 0, 0, 'Consumable - Stormchops (Lightning Zap)'),
(43733, 0, 0, 0, 0, 'Consumable - Stormchops (Lightning Zap)');

-- Item - Icecrown 25 Normal/Heroic Healer Weapon Proc
DELETE FROM `spell_proc_event` WHERE `entry` IN (71865, 71868);
INSERT INTO `spell_proc_event` VALUES
(71865, 0x7F, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0, 0, 0, 0, 0x00010000, 0, 0, 0),
(71868, 0x7F, 0x00, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0, 0, 0, 0, 0x00010000, 0, 0, 0);
