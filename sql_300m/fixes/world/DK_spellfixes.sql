/*--------------------------------------------------------------------*/
/*--------------------Main Death Knight spellfixes--------------------*/
/*--------------------------------------------------------------------*/


-- Bone shield (proc on damage only)
DELETE FROM `spell_proc_event` WHERE `entry` = 49222;
INSERT INTO `spell_proc_event` (`entry`, `SchoolMask`, `procFlags`, `Cooldown`) VALUE
(49222, 127, 0x100000, 3);

-- Crypt Fever (talent)
DELETE FROM `spell_chain` WHERE `first_spell` = 49032;
INSERT INTO `spell_chain` VALUES
(49032, 0, 49032, 1, 0),
(49631, 49032, 49032, 2, 0),
(49632, 49631, 49032, 3, 0);

-- Crypt Fever (procs)
DELETE FROM `spell_chain` WHERE `first_spell` = 50508;
INSERT INTO `spell_chain` VALUES
(50508, 0, 50508, 1, 0),
(50509, 50508, 50508, 2, 0),
(50510, 50509, 50508, 3, 0);

-- Ebon Plaguebringer
DELETE FROM `spell_chain` WHERE `first_spell` = 51099;
INSERT INTO `spell_chain` VALUES
(51099, 0, 51099, 1, 0),
(51160, 51099, 51099, 2, 0),
(51161, 51160, 51099, 3, 0);

-- Ebon Plague
DELETE FROM `spell_chain` WHERE `first_spell` = 51726;
INSERT INTO `spell_chain` VALUES
(51726, 0, 51726, 1, 0),
(51734, 51726, 51726, 2, 0),
(51735, 51734, 51726, 3, 0);

-- Crypt Fever
DELETE FROM `spell_proc_event` WHERE `entry` = 49032;

-- Ebon Plaguebringer
DELETE FROM `spell_proc_event` WHERE `entry` = 51099;

-- Desecration Procs
DELETE FROM `spell_proc_event` WHERE `entry` = 55666;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskB0`) VALUE
('55666', '15', '1', '134217728');

-- Dirge Rank 1
DELETE FROM `spell_proc_event` WHERE `entry`IN (49223, 49599);
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `procFlags`) VALUE
(49223, 15, 17, 134217728, 0x10);

-- Wandering Plague
DELETE FROM `spell_chain` WHERE `first_spell` = 49217;
INSERT INTO `spell_chain` VALUES
(49217, 0, 49217, 1, 0),
(49654, 49217, 49217, 2, 0),
(49655, 49654, 49217, 3, 0);
DELETE FROM `spell_proc_event` WHERE `entry` = 49217;
INSERT INTO `spell_proc_event` (`entry`, `Cooldown`) VALUE
(49217, 3);

-- Unholy Blight
DELETE FROM `spell_proc_event` WHERE `entry` IN (49194);
INSERT INTO `spell_proc_event` (`entry`,  `SchoolMask`, `SpellFamilyName`, `SpellFamilyMaskA0`, `SpellFamilyMaskA1`, `SpellFamilyMaskA2`, `procFlags`, `procEx`, `ppmRate`, `CustomChance`, `Cooldown`) VALUES
(49194, 0x00, 15, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0.000000, 0.000000, 0);

-- Killing Machine
DELETE FROM `spell_proc_event` WHERE `entry` IN (51123, 51127, 51128, 51129, 51139);
INSERT INTO `spell_proc_event` (`entry`, `ppmRate`) VALUES
(51123, 1),
(51127, 2),
(51128, 3),
(51129, 4),
(51139, 5);

DELETE FROM `spell_proc_event` WHERE `entry` = 51124;
INSERT INTO `spell_proc_event` (`entry`, `SpellFamilyName`, `procFlags`, `procEx`) VALUE
(51124, 15, 0x10010, 0x80000);

/*--------------------------------------------------------------------*/
/*------Secondary Death Knight spellfixes (spell coef data etc.)------*/
/*--------------------------------------------------------------------*/


-- Frost Fever
DELETE FROM `spell_bonus_data` WHERE `entry` = 55095;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
('55095', '0', '0', '0','0.06325', 'Death Knight - Frost Fever');

-- Blood Plague
DELETE FROM `spell_bonus_data` WHERE `entry` = 55078;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
('55078', '0', '0', '0','0.06325', 'Death Knight - Blood Plague');

-- Corspe Explosion
DELETE FROM `spell_bonus_data` WHERE `entry` = 47496;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES 
('47496', '0', '0', '0.105', '0', 'Death Knight - Corpse Explosion');

-- Anti-magic Zone
UPDATE `creature_template` SET `modelid_1` = 4590, `scale` = 1, `unit_flags` = `unit_flags` | 0x02000000 WHERE `entry` = 28306;
-- LOL? UPDATE `creature` SET `phaseMask` = 255 WHERE `id` = 28306;

-- Gargoyle Strike bonus
DELETE FROM `spell_bonus_data` WHERE `entry` = 51963;
INSERT INTO `spell_bonus_data` (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `ap_dot_bonus`, `comments`) VALUES
(51963, 1, 0, 0, 0, 'Death Knight - Gargoyle Strike');

-- Gargoyle Scaling auras
DELETE FROM `creature_template_addon` WHERE `entry` = 27829;
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUE
(27829, '54566 51996 61697');

-- Bloodworms
DELETE FROM `creature_template_addon` WHERE `entry` = 28017;
INSERT INTO `creature_template_addon` (`entry`, `auras`) VALUE
(28017, '50453');