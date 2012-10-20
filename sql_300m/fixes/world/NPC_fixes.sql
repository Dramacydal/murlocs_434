
-- http://www.wowhead.com/npc=24133#english-comments не должен приручаться
UPDATE `creature_template` SET `type_flags`=`type_flags` & ~1 WHERE `entry`='24133';

-- правка фракции для Argent Confessor Paletress (http://wh.300murlocs.com/?npc=34928)
UPDATE `creature_template` SET `faction_A`='14', `faction_H`='14' WHERE `entry`='34928';
UPDATE `creature_template` SET `faction_A`='14', `faction_H`='14' WHERE `entry`='35517';

-- http://www.wowhead.com/npc=21657 Neltharaku (speed correction)
-- UPDATE `creature_template` SET `speed_run`='2.85714' WHERE `entry`='21657';
UPDATE `creature_template` SET `speed_run`='1.2' WHERE `entry`='21657';

-- chillmaw
DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 33687 AND `action1_type` = 12;
