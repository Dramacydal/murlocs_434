-- TODO: update for 4.x
/*--------------------------------------------------------------------*/
/*------------------------Main PALADIN spellfixes---------------------*/
/*--------------------------------------------------------------------*/

-- Infusion of Light
-- DELETE FROM `spell_proc_event` WHERE `entry` = 58597;
-- DELETE FROM `spell_proc_event` WHERE `entry` = 53601;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procFlags`, `Cooldown`) VALUE
-- (53601, 127, 0xA82A8, 6);

-- Judgements of the Just
-- DELETE FROM `spell_proc_event` WHERE `entry` = 53695;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, SpellFamilyName, `CustomChance`) VALUE
-- (53695, 2, 10, 100);

-- Seal of Wisdom
-- DELETE FROM `spell_proc_event` WHERE `entry` = 20166;
-- INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `ppmRate`) VALUE
-- (20166, 127, 15);

-- Judgement of Light
-- DELETE FROM `spell_proc_event` WHERE `entry` = 20185;
-- INSERT INTO `spell_proc_event` (entry, SchoolMask, ppmrate) VALUES
-- (20185, 1, 15);

-- Judgement of Wisdom
-- DELETE FROM `spell_proc_event` WHERE `entry` = 20186;
-- INSERT INTO `spell_proc_event` (entry, SchoolMask, ppmrate) VALUES
-- (20186, 1, 15);

-- Seal of Light
-- UPDATE `spell_proc_event` SET `ppmRate` = 10 WHERE `entry` = 20165;

/*--------------------------------------------------------------------*/
/*---------Secondary PALADIN spellfixes (spell coef data etc.)--------*/
/*--------------------------------------------------------------------*/