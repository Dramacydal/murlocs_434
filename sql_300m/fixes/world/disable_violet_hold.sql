-- Disable Violet Hold (was 70, 80)
UPDATE `instance_template` SET `levelMin` = 70, `levelMax` = 80 WHERE `map` = 608;
UPDATE `areatrigger_teleport` SET `required_level` = '99' WHERE `id` = 5209;