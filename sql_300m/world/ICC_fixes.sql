-- Временной отключение стражей статуй и пауков. Было использовано EventAI
UPDATE `creature_template` SET `unit_flags`='33554438' WHERE `entry` IN (37007, 36725, 38058, 38031);
UPDATE `creature_template` SET `AIName`='' WHERE `entry` IN (37007, 36725);


-- Mana void должен быть виден!
UPDATE `creature_template` SET `flags_extra`='0' WHERE `entry`='38068';


-- Val'kyr Shadowguard (clear unit_flags)
UPDATE `creature_template` SET `unit_flags`='0' WHERE `entry` IN ('39120', '39121', '39122', '36609');

