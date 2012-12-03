-- Fix ironforge metro directions
UPDATE `gameobject` SET `rotation2` = 0, `rotation3` = 1 WHERE `guid` IN (5865, 5866, 5863, 16394, 5864, 15363);