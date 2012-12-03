-- 4.x is this still needed?

-- Taunka
-- DELETE FROM `creature_template` WHERE `entry` IN (38292,38273);
-- INSERT INTO `creature_template` (`entry`,`modelid_1`,`faction_A`,`faction_H`,`unit_class`) VALUES
-- (38292,31015,35,35,1),
-- (38273,31015,35,35,1);
-- Vrykul
-- DELETE FROM `creature_template` WHERE `entry` IN (38271);
-- INSERT INTO `creature_template` (`entry`,`modelid_1`,`faction_A`,`faction_H`,`unit_class`) VALUES
-- (38271,31182,35,35,1);
-- DELETE FROM `creature_model_info` WHERE `modelid` IN (31015, 31182);
-- INSERT INTO `creature_model_info` (`modelid`) VALUES (31015), (31182);

-- Deathbringer's Will, missing creature_template fix