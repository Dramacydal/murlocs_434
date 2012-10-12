DELETE FROM `gameobject_loot_template` WHERE `item` = 36908 AND `entry` IN (189973, 190172, 190171, 191303, 190169, 191019, 190170);
-- Дроп с клевера
INSERT INTO `gameobject_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`, `lootcondition`, `condition_value1`, `condition_value2`) VALUES
(189973, 36908, 5, 0, 1, 1, 0, 0, 0),
-- Дроп с ледошипа
(190172, 36908, 5, 0, 1, 1, 0, 0, 0),
-- Дроп с личецвета
(190171, 36908, 5, 0, 1, 1, 0, 0, 0),
-- Дроп с огницы
(191303, 36908, 5, 0, 1, 1, 0, 0, 0),
-- Дроп с тигровой лилии
(190169, 36908, 5, 0, 1, 1, 0, 0, 0),
-- Дроп с язык аспида
(191019, 36908, 5, 0, 1, 1, 0, 0, 0),
-- Дроп с роза таландры
(190170, 36908, 5, 0, 1, 1, 0, 0, 0);