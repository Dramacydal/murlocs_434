-- http://ru.wowhead.com/npc=40041
-- flags_extra 128 -> 0
-- UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry` = 40029;
-- UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry` = 40041;
-- UPDATE `creature_template` SET `flags_extra` = 128 WHERE `entry` = 40042;

UPDATE `creature_template` SET `flags_extra` = 0 WHERE `entry` = 40029;
UPDATE `creature_template` SET `flags_extra` = 0 WHERE `entry` = 40041;
UPDATE `creature_template` SET `flags_extra` = 0 WHERE `entry` = 40042;

