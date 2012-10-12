-- Icecrown citadel spelltable

-- Lord Marrowgar
DELETE FROM `boss_spell_table` WHERE `entry` = 36612;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36612, 71021, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36612, 69138, 0, 0, 0, 6000, 0, 0, 0, 12000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 71580, 0, 0, 0, 6000, 0, 0, 0, 12000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 69146, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 69057, 0, 0, 0, 17000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36612, 69076, 0, 0, 0, 60000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36612, 69075, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 5, 0, 0, 12, 0, 1),
(36612, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36612, 38711, 20000, 40000, 1, 1, 2, 2, 1, 5, 0, 9),
(36612, 36672, 45000, 45000, 1, 1, 2, 2, 75, 100, 0, 9);
-- Cold flame
DELETE FROM `boss_spell_table` WHERE `entry` = 36672;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36672, 69145, 0, 0, 0, 15000, 0, 0, 0, 15000, 0, 0, 0, 15, 15, 15, 15, 0, 0, 0, 1, 0, 0),
(36672, 69147, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 15000, 15000, 30000, 30000, 0, 0, 0, 1, 0, 0),
(36672, 69146, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36672, 36672, 30000, 30000, 1, 1, 1, 1, 0, 0, 0, 11);
-- Bone spike
DELETE FROM `boss_spell_table` WHERE `entry` = 38711;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38711, 46598, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 18, 0, 0),
(38711, 69065, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 6, 0, 0);

-- Lady Deathwhisper
DELETE FROM `boss_spell_table` WHERE `entry` = 36855;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36855, 70842, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 1, 0, 0),
(36855, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 1, 0, 0),
(36855, 71254, 72008, 72008, 72504, 5000, 0, 0, 0, 8000, 0, 0, 0, 4, 0, 0),
(36855, 71420, 72501, 72007, 72502, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36855, 71001, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36855, 71204, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(36855, 70901, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 6, 0, 0),
(36855, 71289, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36855, 71494, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36855, 37890, 45000, 75000, 1, 1, 2, 2, 75, 100, 0, 11),
(36855, 37949, 45000, 75000, 1, 1, 2, 2, 75, 100, 0, 11),
(36855, 38010, 45000, 45000, 1, 1, 2, 2, 75, 100, 0, 9),
(36855, 38222, 8000,  15000, 1, 1, 1, 1, 75, 100, 0, 9),
(36855, 38009, 45000, 45000, 1, 1, 2, 2, 75, 100, 0, 9);
-- Vengeful shade
DELETE FROM `boss_spell_table` WHERE `entry` = 38222;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38222, 71494, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 1, 0, 0),
(38222, 71544, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 3, 0, 0);
-- Adherent
DELETE FROM `boss_spell_table` WHERE `entry` = 37949;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37949, 71129, 0, 0, 0, 15000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37949, 70594, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 4, 0, 0),
(37949, 71254, 0, 0, 0, 5000, 0, 0, 0, 15000, 0, 0, 0, 4, 0, 0),
(37949, 70906, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 4, 0, 0),
(37949, 70903, 0, 0, 0, 1000, 0, 0, 0, 2000, 0, 0, 0, 1, 0, 0),
(37949, 71237, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37949, 70768, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 4, 0, 0),
(37949, 41236, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 1, 0, 0),
(37949, 71234, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 4, 0, 0);
-- Fanatic
DELETE FROM `boss_spell_table` WHERE `entry` = 37890;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37890, 70659, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 3, 0, 0),
(37890, 70670, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 3, 0, 0),
(37890, 70674, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 1, 0, 0);

-- Gunship battle
-- Frost wyrm
DELETE FROM `boss_spell_table` WHERE `entry` = 37230;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37230, 70116, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(37230, 70362, 0, 0, 0, 20000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(37230, 70361, 0, 0, 0, 3000, 0, 0, 0, 5000, 0, 0, 0, 3, 0, 0),
(37230, 47008, 0, 0, 0, 180000, 0, 0, 0, 180000, 0, 0, 0, 1, 0, 0);

-- Rotted frost giant
DELETE FROM `boss_spell_table` WHERE `entry` IN (38490, 38494);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38490, 64652, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 4, 0, 0),
(38490, 72865, 0, 0, 0, 30000, 0, 0, 0, 45000, 0, 0, 0, 4, 0, 0),
(38490, 47008, 0, 0, 0, 300000, 0, 0, 0, 300000, 0, 0, 0, 1, 0, 0);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38494, 64652, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 4, 0, 0),
(38494, 72865, 0, 0, 0, 25000, 0, 0, 0, 40000, 0, 0, 0, 4, 0, 0),
(38494, 47008, 0, 0, 0, 300000, 0, 0, 0, 300000, 0, 0, 0, 1, 0, 0);

-- Deathbringer Saurfang
DELETE FROM `boss_spell_table` WHERE `entry` = 37813;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`,`data1`, `data2`, `data3`, `data4`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37813, 72178, 0, 0, 0, 20000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72371, 0, 0, 0, 3000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72256, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72293, 0, 0, 0, 35000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37813, 72737, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72385, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72380, 0, 0, 0, 25000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72408, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72172, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 2, 5, 2, 5, 1, 0, 0),
(37813, 72173, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 2, 5, 2, 5, 1, 0, 0),
(37813, 72356, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72357, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72358, 0, 0, 0, 45000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72769, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37813, 72723, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 72242, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37813, 47008, 0, 0, 0, 480000, 0, 0, 0, 480000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(37813, 38508, 45000, 45000, 1, 1, 2, 2, 15, 25, 0, 9);
-- Blood beast
DELETE FROM `boss_spell_table` WHERE `entry` = 38508;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38508, 72176, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0),
(38508, 72723, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0),
(38508, 72769, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0),
(38508, 21150, 0, 0, 0, 15000, 0, 0, 0, 27000, 0, 0, 0, 1, 0, 0);


-- Festergut
DELETE FROM `boss_spell_table` WHERE `entry` = 36626;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
( 36626, 69157, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69162, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69164, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69126, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69152, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69154, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69165, 0, 0, 0, 10000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69195, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69278, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69279, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69290, 0, 0, 0, 20000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 69291, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
( 36626, 72219, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
( 36626, 72227, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
( 36626, 72272, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69244, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 69248, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
( 36626, 72287, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
( 36626, 71379, 0, 0, 0, 20000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
( 36626, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36626, 38548, 12000, 12000, 1, 1, 1, 1, 10, 20, 0, 11);

-- Rotface
DELETE FROM `boss_spell_table` WHERE `entry` = 36627;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`,`data1`, `data2`, `data3`, `data4`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36627, 69508, 0, 0, 0, 15000, 0, 0, 0, 20000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36627, 69674, 0, 0, 0, 1000, 0, 0, 0,  1000, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36627, 70003, 0, 0, 0, 15000, 0, 0, 0, 15000, 0, 0, 0, 1, 2, 1, 3, 6, 0, 0),
(36627, 69788, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36627, 69783, 0, 0, 0, 30000, 0, 0, 0, 45000, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36627, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36627, 69789, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 0, 6, 0, 1);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36627, 36897, 12000, 12000, 1, 1, 1, 1, 1, 5, 0, 9),
(36627, 37986, 15000, 15000, 1, 1, 1, 1, 10, 20, 0, 11),
(36627, 37013, 15000, 15000, 1, 1, 1, 1, 0, 0, 0, 11);
-- Small ooze
DELETE FROM `boss_spell_table` WHERE `entry` = 36897;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36897,69774, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 4, 0, 0),
(36897,69750, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36897,69644, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 6, 0, 0),
(36897,69889, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 6, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36897, 36899, 12000, 12000, 1, 1, 1, 1, 1, 5, 0, 9);
-- Big ooze
DELETE FROM `boss_spell_table` WHERE `entry` = 36899;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36899,69774, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 4, 0, 0),
(36899,69839, 0, 0, 0, 6000, 0, 0, 0, 6000, 0, 0, 0, 1, 0, 0),
(36899,69760, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36899,69644, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36899,69558, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 1, 0, 0),
(36899,69889, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 6, 0, 0);
-- Ooze explode stalker
DELETE FROM `boss_spell_table` WHERE `entry` = 38107;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38107,69840, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 1, 0, 0);

-- Professor Putricide
DELETE FROM `boss_spell_table` WHERE `entry` = 36678;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36678,70351, 0, 0, 0, 40000, 0, 0, 0, 60000, 0, 0, 0, 0, 1, 0, 0),
(36678,71617, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71615, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 100, 12, 0, 0),
(36678,71618, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 100, 12, 0, 0),
(36678,71621, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71278, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71279, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71893, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,71273, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 1, 0, 0),
(36678,71275, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 1, 0, 0),
(36678,71276, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 1, 0, 0),
(36678,71702, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 1, 0, 0),
(36678,71703, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 1, 0, 0),
(36678,71603, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 3, 0, 0),
(36678,70311, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 0, 1, 0, 0),
(36678,47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 1, 0, 0),
(36678,71518, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 6, 0, 1),
(36678,72672, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 3, 0, 0);
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36678,70342, 0, 0, 0, 30000, 0, 0, 0, 60000, 0, 0, 0, 10, 40, 0, 15, 0, 0),
(36678,70852, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 10, 30, 0, 15, 0, 0);

-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36678, 37562, 20000, 40000, 1, 1, 1, 1, 5, 10, 0, 9),
(36678, 38159, 8000,  20000, 1, 1, 1, 1, 20, 40, 0, 9),
(36678, 37690, 40000, 60000, 1, 1, 1, 1, 10, 40, 0, 9),
(36678, 37697, 20000, 40000, 1, 1, 1, 1, 5, 10, 0, 9);
-- Gas cloud
DELETE FROM `boss_spell_table` WHERE `entry` = 37562;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37562,70672, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0),
(37562,70215, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0),
(37562,71770, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37562,70812, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37562,70701, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0);
-- Volatile ooze
DELETE FROM `boss_spell_table` WHERE `entry` = 37697;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37697,70492, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 1, 0, 0),
(37697,70530, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37697,71770, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 1),
(37697,70447, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 6, 0, 0);
-- Mutated abomination (pet?)
DELETE FROM `boss_spell_table` WHERE `entry` = 37672;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37672,70311, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 1, 0, 0),
(37672,72527, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37672,72539, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37672,70542, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 3, 0, 0),
(37672,70405, 0, 0, 0, 10000, 0, 0, 0, 20000, 0, 0, 0, 1, 0, 0);

-- Taldaram
DELETE FROM `boss_spell_table` WHERE `entry` = 37973;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37973, 71807, 0, 0, 0, 50000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37973, 71718, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37973, 71719, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 15, 0, 0),
(37973, 72040, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37973, 72041, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 15, 0, 0),
(37973, 70952, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 70981, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 70982, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 71598, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 70983, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37973, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0);

-- Valanar
DELETE FROM `boss_spell_table` WHERE `entry` = 37970;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37970, 72053, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37970, 71598, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 38459, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 11, 0, 0),
(37970, 72037, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 38422, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 11, 0, 0),
(37970, 71945, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37970, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70983, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70981, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70982, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 70952, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37970, 72039, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0);

-- Keleseth
DELETE FROM `boss_spell_table` WHERE `entry` = 37972;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37972, 71405, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37972, 71598, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 71815, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37972, 71943, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 71822, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37972, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70952, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70981, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70982, 0, 0, 0, 30000, 0, 0, 0, 30000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37972, 70983, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 1, 0, 0);

-- Lanathel
DELETE FROM `boss_spell_table` WHERE `entry` = 37955;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37955, 72981, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37955, 71623, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37955, 70451, 70451, 71510, 71510, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
(37955, 70445, 70445, 70821, 70821, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(37955, 71726, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 70867, 71473, 71532, 71533, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 70871, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 70923, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71340, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71264, 0, 0, 0, 20000, 0, 0, 0, 35000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71446, 0, 0, 0, 7000, 0, 0, 0, 12000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37955, 71772, 0, 0, 0, 40000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37955, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37955, 72934, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 6, 0, 1),
(37955, 71952, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 1, 0, 0);

-- Valithria
DELETE FROM `boss_spell_table` WHERE `entry` = 36789;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36789, 71977, 0, 0, 0, 30000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 71987, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 1, 0),
(36789, 72481, 0, 0, 0, 10000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 70873, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 71189, 0, 0, 0, 3000, 0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 72724, 0, 0, 0, 3000, 0, 0, 0, 3000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 70904, 0, 0, 0, 5000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 1, 0, 1),
(36789, 71196, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36789, 70702, 0, 0, 0, 15000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 16, 0, 1);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36789, 38429, 30000, 40000, 1, 1, 1, 1, 5, 70, 0, 9),
(36789, 37868, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 37863, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 36791, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 37934, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11),
(36789, 37886, 30000, 45000, 1, 1, 1, 1, 1, 3, 0, 11);
-- Nightmare portal
DELETE FROM `boss_spell_table` WHERE `entry` = 38429;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(38429, 70873, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 1, 0, 1);

-- Sindragosa
DELETE FROM `boss_spell_table` WHERE `entry` = 36853;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36853, 70084, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 57764, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 1, 0),
(36853, 19983, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36853, 71077, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 69649, 0, 0, 0, 20000, 0, 0, 0, 35000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 70107, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 69762, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 69766, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36853, 69846, 0, 0, 0, 15000, 0, 0, 0, 20000, 0, 0, 0, 50.0, 100.0, 0, 15, 0, 0),
(36853, 70117, 0, 0, 0, 30000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 70123, 0, 0, 0, 20000, 0, 0, 0, 35000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 70126, 0, 0, 0, 90000, 0, 0, 0, 90000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 70157, 0, 0, 0, 6000, 0, 0, 0, 6000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36853, 71665, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36853, 69845, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36853, 36980, 0, 0, 0, 90000, 0, 0, 0, 90000, 0, 0, 0, 0, 0, 0, 9, 0, 0),
(36853, 47008, 0, 0, 0, 600000, 0, 0, 0, 600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36853, 72289, 0, 0, 0, 1000, 0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 12, 0, 1),
(36853, 70128, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 30.0, 0, 0, 12, 0, 0);

UPDATE`boss_spell_table` SET  `data1` = 2, `data2` = 5, `data3` = 2, `data4` =5 WHERE `entry` = 36853 AND `spellID_N10` = 70126;
UPDATE`boss_spell_table` SET  `data1` = 4, `data2` = 4, `data3` = 4, `data4` =4 WHERE `entry` = 36853 AND `spellID_N10` = 69845;

-- Rimefang
DELETE FROM `boss_spell_table` WHERE `entry` = 37533;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37533, 71387, 0, 0, 0, 0, 0, 0, 0, 3600000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37533, 71386, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(37533, 71376, 0, 0, 0, 4000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 4, 0, 0);
-- Spinestalker
DELETE FROM `boss_spell_table` WHERE `entry` = 37534;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37534, 36922, 0, 0, 0, 8000, 0, 0, 0, 24000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(37534, 40505, 0, 0, 0, 5000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(37534, 71369, 0, 0, 0, 4000, 0, 0, 0, 8000, 0, 0, 0, 0, 0, 0, 4, 0, 0);
-- Ice tomb
DELETE FROM `boss_spell_table` WHERE `entry` = 36980;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36980, 70157, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 6, 0, 0);

-- Lich king
DELETE FROM `boss_spell_table` WHERE `entry` = 36597;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36597, 70541, 0, 0, 0, 6000, 0, 0, 0, 12000, 0, 0, 0, 60, 0, 0, 12, 0, 0),
(36597, 70337, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 74074, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 69409, 0, 0, 0, 10000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36597, 72762, 0, 0, 0, 20000, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 68980, 0, 0, 0, 1500, 0, 0, 0, 25000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 68981, 0, 0, 0, 60000, 0, 0, 0, 60000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72133, 0, 0, 0, 5000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72262, 0, 0, 0, 5000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69201, 0, 0, 0, 5000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 1),
(36597, 69200, 0, 0, 0, 5000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 4, 0, 0),
(36597, 69242, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69103, 0, 0, 0, 3000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69099, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69108, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70358, 0, 0, 0, 40000, 0, 0, 0, 70000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70372, 0, 0, 0, 40000, 0, 0, 0, 70000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72149, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 72143, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70503, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 69037, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 36609, 0, 0, 0, 20000, 0, 0, 0, 40000, 0, 0, 0, 0, 0, 0, 9, 0, 0),
(36597, 71769, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 70063, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 1),
(36597, 47008, 0, 0, 0, 900000, 0, 0, 0, 900000, 0, 0, 0, 0, 0, 0, 1, 0, 0),
(36597, 74352, 0, 0, 0, 8000, 0, 0, 0, 15000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- summons
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `timerMin_N10`, `timerMax_N10`,  `data1`, `data2`, `data3`, `data4`, `locData_x`, `locData_y`, `locData_z`, `CastType` ) VALUES
(36597, 37799, 60000, 60000, 8, 10, 10, 10, 15, 25, 0, 11),
(36597, 70498, 3600001, 3600001, 12, 12, 15, 15, 15, 25, 0, 1);
-- ice sphere
DELETE FROM `boss_spell_table` WHERE `entry` = 36633;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36633, 69099, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 6, 0, 0),
(36633, 69108, 0, 0, 0, 2000, 0, 0, 0, 2000, 0, 0, 0, 0, 0, 0, 3, 0, 0),
(36633, 69090, 0, 0, 0, 8000, 0, 0, 0, 24000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- vile spirit
DELETE FROM `boss_spell_table` WHERE `entry` = 37799;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(37799, 70503, 0, 0, 0, 10000, 0, 0, 0, 10000, 0, 0, 0, 0, 0, 0, 1, 0, 0);
-- raging spirit
DELETE FROM `boss_spell_table` WHERE `entry` = 36701;
INSERT INTO `boss_spell_table` (`entry`, `spellID_N10`, `spellID_N25`, `spellID_H10`, `spellID_H25`, `timerMin_N10`, `timerMin_N25`, `timerMin_H10`, `timerMin_H25`, `timerMax_N10`, `timerMax_N25`, `timerMax_H10`, `timerMax_H25`, `locData_x`, `locData_y`, `locData_z`, `CastType`, `isVisualEffect`, `isBugged`) VALUES
(36701, 69242, 0, 0, 0, 2000, 0, 0, 0, 5000, 0, 0, 0, 0, 0, 0, 3, 0, 0);
