UPDATE `item_template` SET `ExtraFlags` = `ExtraFlags` | 4 WHERE `entry` IN (39878, 44717);
DELETE FROM `item_loot_template` WHERE `entry` IN (39878, 44717);
INSERT INTO `item_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`, `lootcondition`, `condition_value1`, `condition_value2`) VALUES
('39878', '39883', '100', '0', '1', '1', '0', '0', '0'),
('44717', '44718', '100', '0', '1', '1', '0', '0', '0');