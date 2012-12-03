UPDATE `item_template` SET `Quality` = 3 WHERE `entry` IN (12529, 12264);

DELETE FROM `locales_item` WHERE `entry` IN (34518, 34519, 37298);
INSERT INTO `locales_item` (`entry`, `name_loc8`) VALUES
-- Golden Pig Coin
(34518, 'Монетка золотого поросенка'),
-- Silver Pig Coin
(34519, 'Серебряная монетка Свиньи'),
-- Competitor's Souvenir
(37298, 'Сувенир участника соревнований');

-- Shaman T10 shoulder visual
UPDATE `item_template` SET `spellid_1` = 72958, `spelltrigger_1` = 1, `spellcharges_1` = 0,
`spellppmRate_1` = 0, `spellcooldown_1` = -1, `spellcategory_1` = 0,
`spellcategorycooldown_1` = -1 WHERE `entry` IN
(50834, 50839, 50845, 51194, 51199, 51204, 51235, 51240, 51245);