-- Dalaran flight restriction
DELETE FROM `spell_area` WHERE `spell` = '58600';
INSERT INTO `spell_area` (`spell`, `area`, `gender`, `autocast`) VALUE
('58600', '4395', '2', '1');