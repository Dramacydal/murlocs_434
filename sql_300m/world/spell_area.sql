-- add spectral gryphon to dragonblight
DELETE FROM `spell_area` WHERE `area` = 65 AND `spell` IN (55173, 55164);
INSERT INTO `spell_area` (`spell`, `area`, `quest_start`, `quest_start_active`, `quest_end`, `aura_spell`, `racemask`, `gender`, `autocast`) VALUES
(55173, 65, 0, 0, 0, 8326, 8, 2, 1),
(55164, 65, 0, 0, 0, 8326, 65527, 2, 1);