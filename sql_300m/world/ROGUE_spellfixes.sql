-- TODO: update for 4.x

/*--------------------------------------------------------------------*/
/*------------------------Main ROGUE spellfixes-----------------------*/
/*--------------------------------------------------------------------*/


-- Hemorrhage
-- DELETE FROM `spell_proc_event` WHERE `entry` = 16511;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`) VALUE
-- (16511, 127);

-- Waylay
-- DELETE FROM `spell_proc_event` WHERE `entry` IN (51692,51696);
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
-- ('51692','0','8','516','0','0','0','0','0','0','0'),
-- ('51696','0','8','516','0','0','0','0','0','0','0');

-- Ruthlessness & Envenom (SpellFamilyMaskB not set)
-- DELETE FROM `spell_proc_event` WHERE `entry` = 14156;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `SpellFamilyMaskB0`, `SpellFamilyMaskB1`, `SpellFamilyMaskB2`) VALUE
-- (14156, 8, 4063232, 4063232, 4063232, 8, 8, 8);

-- Seal Fate
-- DELETE FROM `spell_proc_event` WHERE `entry` = 14186;
-- INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `SpellFamilyMaskB0`, `SpellFamilyMaskB1`, `SpellFamilyMaskB2`, `procEx`)  VALUE 
-- (14186, 8, 1082131720, 1082131720, 1082131720, 6, 6, 6, 2);

-- Aura remove procs (Vanish & Invisibility)
-- DELETE FROM spell_proc_event WHERE entry IN (26888, 32612);
-- INSERT INTO spell_proc_event (entry, procFlags) VALUES
-- Vanish
-- (26888, 664232),
-- Invisibility
-- (32612, 664232);

-- Setup
-- UPDATE `spell_proc_event` SET `Cooldown` = 1 WHERE `entry` = 13983;

/*--------------------------------------------------------------------*/
/*----------Secondary ROGUE spellfixes (spell coef data etc.)---------*/
/*--------------------------------------------------------------------*/