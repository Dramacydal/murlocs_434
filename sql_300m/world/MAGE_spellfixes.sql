-- TODO: update for 4.x
/*--------------------------------------------------------------------*/
/*-------------------------Main MAGE spellfixes-----------------------*/
/*--------------------------------------------------------------------*/


-- Impact
-- DELETE FROM spell_proc_event WHERE entry IN (11103, 12357, 12358, 64343);
-- INSERT INTO `spell_proc_event` (`entry`,  `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
-- (11103, 0x00000000, 3, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.0, 0.0, 0),
-- (64343, 0x00000000, 3, 0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.0, 0.0, 0);

-- Fingers of Frost proc chance
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (44543, 44545);
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`,  `SpellFamilyMaskA0`, `SpellFamilyMaskB0`, `SpellFamilyMaskC0`, `CustomChance`) VALUES
-- (44543, 3, 0x1012E0, 0x1000, 0x1, 7),
-- (44545, 3, 0x1012E0, 0x1000, 0x1, 15);

-- 0        0        1  -- Winter's chill
-- 0x20     0        0  -- Frost bolt
-- 0        0x10000  0  -- Frostfire
-- 0x100000 0        0  -- chills
-- 0x200    0        0  -- cone of cold
-- 0x40     0        0  -- frost nova
-- 0x80     0        0  -- blizzard

-- Fingers of Frost proc remove
-- DELETE FROM `spell_proc_event` WHERE `entry` = 74396;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procFlags`, `CustomChance`) VALUES
-- (74396, 126, 0x10000, 100);

-- Arcane Blast debuff
-- DELETE FROM `spell_proc_event` WHERE `entry` = 36032;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`, `SpellFamilyMaskA1`, `SpellFamilyMaskB1`) VALUE
-- (36032, 3, 0x200000, 0x8000, 0x200000, 0x8000);

-- Missile Barrage proc remove
-- DELETE FROM `spell_proc_event` WHERE `entry` = 44401;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `procFlags`, `procEx`) VALUES
-- (44401, 3, 65536, 524288);

/*--------------------------------------------------------------------*/
/*----------Secondary MAGE spellfixes (spell coef data etc.)----------*/
/*--------------------------------------------------------------------*/

-- Deep Freeze dmg
-- DELETE FROM `spell_bonus_data` WHERE `entry` = 71757;
-- INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUE
-- (71757, 2.143, 0, 0, 0, 'Deep Freeze');

-- Flaming Orb
UPDATE `creature_template` SET `minlevel` = 85, `maxlevel` = 85, `speed_run` = 0.714286, `unit_flags` = 33554432 + 2, `unit_flags2` = 2048, type_flags = 0, `InhabitType` = 3, movementId = 90, `ScriptName` = 'npc_mage_flame_orb'  where `entry` = 44214;