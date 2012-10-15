-- Insert comments to scriptdev2 boss_spell_table from WOWD database. 
-- Change WOWD database name if you use this!

CREATE ALGORITHM = TEMPTABLE VIEW `commentlist` 
(`entry` ,`spell`, `comment`)
AS SELECT `scriptdev2`.`boss_spell_table`.`entry`,
`spellID_N10`,
CONCAT(`mangos`.`creature_template`.`name`,
' : ',
`mangos`.`wowd_spell`.`SpellName`)
FROM `scriptdev2`.`boss_spell_table`
INNER JOIN `mangos`.`creature_template` ON `mangos`.`creature_template`.`entry` = `scriptdev2`.`boss_spell_table`.`entry` 
INNER JOIN `mangos`.`wowd_spell` ON `mangos`.`wowd_spell`.`id` = `scriptdev2`.`boss_spell_table`.`spellID_N10`;

UPDATE `scriptdev2`.`boss_spell_table` SET `comment` =  (SELECT DISTINCT `commentlist`.`comment`
FROM `commentlist` WHERE `scriptdev2`.`boss_spell_table`.`entry` = `commentlist`.`entry` 
AND `scriptdev2`.`boss_spell_table`.`spellID_N10` = `commentlist`.`spell`
AND `scriptdev2`.`boss_spell_table`.`comment` IS NULL);
DROP VIEW `commentlist`;
