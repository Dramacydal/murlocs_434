-- TODO: update for 4.x

/*--------------------------------------------------------------------*/
/*------------------------Main DRUID spellfixes-----------------------*/
/*--------------------------------------------------------------------*/


-- Brambles
-- DELETE FROM `spell_proc_event` WHERE entry = 22812;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES('22812','0','0','0','0','0','0','3','0','0','0');

-- Primal Precision
-- DELETE FROM `spell_proc_event` WHERE `entry` = 48409;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA1`, `SpellFamilyMaskB1`, `SpellFamilyMaskC1`, `procFlags`, `procEx`) VALUE
-- (48409, 7, 0x800000, 0x10000000, 0x200000, 0x10, 0x23B4);

-- Omen of Clarity
-- DELETE FROM `spell_proc_event` WHERE `entry` = 16864;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procFlags`, `procEx`, `CustomChance`, `Cooldown`) VALUES
-- (16864, 127, 0x54004, 0x00003, 6, 10);

-- Revitalize
-- DELETE FROM `spell_proc_event` WHERE `entry` = 48539;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `SpellFamilyMaskB0`, `SpellFamilyMaskB1`, `SpellFamilyMaskB2`, `procFlags`, `procEx`) VALUES
-- (48539, 7, 0x10, 0x10, 0x10, 0x4000000, 0x4000000, 0x4000000, 0x40000, 0x40000);

-- Owlkin Frenzy
-- DELETE FROM `spell_proc_event` WHERE `entry` = 48389;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `procFlags`) VALUES
-- (48389, 0x07, 0x000222A8);

-- DELETE FROM `spell_chain` WHERE `first_spell` IN (33876, 33878);
-- INSERT INTO `spell_chain` (`spell_id`, `prev_spell`, `first_spell`, `rank`, `req_spell`) VALUES
-- Mangle - Cat
-- (33876, 0, 33876, 1, 0),
-- (33982, 33876, 33876, 2, 0),
-- (33983, 33982, 33876, 3, 0),
-- (48565, 33983, 33876, 4, 0),
-- (48566, 48565, 33876, 5, 0),
-- Mangle - Bear
-- (33878, 0, 33878, 1, 0),
-- (33986, 33878, 33878, 2, 0),
-- (33987, 33986, 33878, 3, 0),
-- (48563, 33987, 33878, 4, 0),
-- (48564, 48563, 33878, 5, 0);

/*--------------------------------------------------------------------*/
/*----------Secondary DRUID spellfixes (spell coef data etc.)---------*/
/*--------------------------------------------------------------------*/

-- Lifebloom coef
-- DELETE FROM spell_bonus_data WHERE entry IN (33763);
-- INSERT INTO spell_bonus_data VALUES
-- (33763, 0.5163, 0.09518, 0, 0, 'Druid - Lifebloom');

-- Starfall large star
-- DELETE FROM `spell_bonus_data` WHERE `entry` = 50288;
-- INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `comments`) VALUE
-- (50288, 0.3, 0, 0, 'Druid - Starfall');

-- Starfall aoe star
-- DELETE FROM `spell_bonus_data` WHERE `entry` = 50294;
-- INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `comments`) VALUE
-- (50294, 0.13, 0, 0, 'Druid - Starfall AOE');

-- Wild mushroom damage
DELETE FROM `spell_bonus_data` WHERE `entry` = 78777;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `comments`) VALUES
(78777, 0.65, 0, 0, 'Druid: Wild mushroom damage');

-- Wild mushroom
-- UPDATE `creature_template` SET `unit_flags`=2584, `faction_A`=2203, `faction_H`=2203, `AIName`='', `ScriptName`='' WHERE `entry` = 47649;
UPDATE `creature_template` SET `unit_flags`=0x2|0x4|0x20000|0x2000000, `faction_A`=24, `faction_H`=24, `AIName`='', `ScriptName`='npc_druid_wild_mushroom' WHERE `entry` = 47649;
-- Fungal Growth I & II
-- UPDATE `creature_template` SET `unit_flags`=33556504, `dynamicflags`=0, `modelid_1`=13069, `AIName`='', `ScriptName`='' WHERE `entry` IN (43497, 43484);
UPDATE `creature_template` SET `unit_flags`=0x2|0x4|0x20000|0x2000000, `dynamicflags`=0x8, `modelid_1`=35760/*13069*/, `AIName`='', `ScriptName`='npc_druid_fungal_growth' WHERE `entry` IN (43497, 43484);
