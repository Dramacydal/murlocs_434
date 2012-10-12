-- Remove Corruption (has uneven TARGET) TARGET_SCRIPT and NULL TriggerSpell
-- UPDATE `creature_template` SET `spell1` = 70602, `spell2` = 70759 WHERE `entry` = 37868;
UPDATE `creature_template` SET `spell1` = 70759, `spell2` = 0 WHERE `entry` = 37868;
-- UPDATE `creature_template` SET `spell1` =70602, `spell2` = 70759, `spell3` = 71179 WHERE `entry` IN (38725, 38167, 38735);
UPDATE `creature_template` SET `spell1` = 70759, `spell2` = 71179, `spell3` = 0 WHERE `entry` IN (38725, 38167, 38735);