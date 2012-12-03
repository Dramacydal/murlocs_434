REPLACE INTO `creature_questrelation` (`id`, `quest`) VALUES (16818, 9365);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry` = 16817;
REPLACE INTO `creature_involvedrelation` (`id`, `quest`) VALUES (16817, 9365);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=16817;
UPDATE `item_template` SET `StartQuest`=0 WHERE `StartQuest` = 9319;
REPLACE INTO `creature_questrelation` (`id`, `quest`) VALUES (16818, 9319);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry` = 16818;
REPLACE INTO `creature_involvedrelation` (`id`, `quest`) VALUES (16818, 9319);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=16818;
REPLACE INTO `creature_questrelation` (`id`, `quest`) VALUES (16818, 9339);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry` = 16818;
REPLACE INTO `creature_involvedrelation` (`id`, `quest`) VALUES (16818, 9339);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=16818;
UPDATE `item_template` SET `StartQuest`=0 WHERE `StartQuest` = 9386;
REPLACE INTO `creature_questrelation` (`id`, `quest`) VALUES (16818, 9386);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry` = 16818;
REPLACE INTO `creature_involvedrelation` (`id`, `quest`) VALUES (16818, 9386);
UPDATE `creature_template` SET `npcflag`=`npcflag`|2 WHERE `entry`=16818;


-- Rut'threan Village
UPDATE `spell_area` SET `area` = 702 WHERE `spell` = 29126;