-- Alexstrasza's Gift. different db versions have different guids
UPDATE `gameobject` SET `position_z` = 269.17, `spawntimesecs` = -604800  WHERE `id` IN (193905, 193967) AND `map` = 616;

-- Heart of Magic
UPDATE `gameobject` SET `position_z` = 268.17, `spawntimesecs` = -604800 WHERE `id` IN (194158, 194159) AND `map` = 616;