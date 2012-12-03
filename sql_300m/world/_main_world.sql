ALTER TABLE creature_template
  ADD COLUMN `spell5` mediumint(8) unsigned NOT NULL default '0' AFTER `spell4`,
  ADD COLUMN `spell6` mediumint(8) unsigned NOT NULL default '0' AFTER `spell5`,
  ADD COLUMN `spell7` mediumint(8) unsigned NOT NULL default '0' AFTER `spell6`,
  ADD COLUMN `spell8` mediumint(8) unsigned NOT NULL default '0' AFTER `spell7`,
  ADD COLUMN `powerType` mediumint(8) unsigned NOT NULL default '0' AFTER `maxhealth`,
  CHANGE COLUMN unk16 health_mod float,
  CHANGE COLUMN unk17 power_mod float;
  
DROP TABLE IF EXISTS `autobroadcast`;
CREATE TABLE `autobroadcast` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `text` longtext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `creature_transport`;
CREATE TABLE IF NOT EXISTS `creature_transport` (
 `guid` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
 `map` mediumint(8) unsigned NOT NULL DEFAULT '0',
 `position_x` float DEFAULT '0',
 `position_y` float DEFAULT '0',
 `position_z` float DEFAULT '0',
 `orientation` float DEFAULT '0',
 `emote` mediumint(8) unsigned DEFAULT '0',
 PRIMARY KEY (`guid`,`map`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Creature on transports';

DROP TABLE IF EXISTS `custom_areaflags`;
CREATE TABLE `custom_areaflags` (
 `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
 `flags` int(10) unsigned NOT NULL DEFAULT '0',
 `comment` char(100) NOT NULL DEFAULT '0',
 PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Custom area flags table';

DROP TABLE IF EXISTS `custom_worldsafelocs`;
CREATE TABLE `custom_worldsafelocs` (
 `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
 `map` int(10) unsigned,
 `x` float DEFAULT '0',
 `y` float DEFAULT '0',
 `z` float DEFAULT '0',
 `team` int(10) unsigned,
 `comment` text,
 PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Custom world safe locs';

ALTER TABLE `areatrigger_teleport` CHANGE `required_quest_done` `required_quest_done_A` int(11) UNSIGNED NOT NULL DEFAULT "0"
COMMENT "Alliance quest";

ALTER TABLE `areatrigger_teleport` CHANGE `required_quest_done_heroic` `required_quest_done_heroic_A` int(11) UNSIGNED NOT NULL DEFAULT "0"
COMMENT "Alliance heroic quest";

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`required_quest_done_H` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Horde quest"
AFTER `required_quest_done_A`;

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`required_quest_done_heroic_H` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Horde heroic quest"
AFTER `required_quest_done_heroic_A`;

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`minGS` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Min player gear score";

ALTER TABLE `areatrigger_teleport` ADD COLUMN
`maxGS` int(11) UNSIGNED NOT NULL DEFAULT "0" COMMENT "Max player gear score";

UPDATE `areatrigger_teleport` SET `required_quest_done_H` = `required_quest_done_A`
WHERE `required_quest_done_A` > 0;

UPDATE `areatrigger_teleport` SET `required_quest_done_heroic_H` = `required_quest_done_heroic_A`
WHERE `required_quest_done_heroic_A` > 0;

-- LFG dungeon reward structure from TC

DROP TABLE IF EXISTS `lfg_dungeon_rewards`;
CREATE TABLE `lfg_dungeon_rewards` (
  `dungeonId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Dungeon entry from dbc',
  `maxLevel` tinyint(3) unsigned NOT NULL DEFAULT '0' COMMENT 'Max level at which this reward is rewarded',
  `firstQuestId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest id with rewards for first dungeon this day',
  `firstMoneyVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Money multiplier for completing the dungeon first time in a day with less players than max',
  `firstXPVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Experience multiplier for completing the dungeon first time in a day with less players than max',
  `otherQuestId` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Quest id with rewards for Nth dungeon this day',
  `otherMoneyVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Money multiplier for completing the dungeon Nth time in a day with less players than max',
  `otherXPVar` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Experience multiplier for completing the dungeon Nth time in a day with less players than max',
  PRIMARY KEY (`dungeonId`,`maxLevel`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `player_factionchange_spells`;
CREATE TABLE `player_factionchange_spells` (
    `alliance_id` int(8) NOT NULL,
    `commentA` varchar(255) DEFAULT NULL,
    `horde_id` int(8) NOT NULL,
    `commentH` varchar(255) DEFAULT NULL,
    PRIMARY KEY (`alliance_id`, `horde_id`)
) DEFAULT CHARSET=UTF8;

DROP TABLE IF EXISTS `player_factionchange_reputations`;
CREATE TABLE `player_factionchange_reputations` (
    `alliance_id` int(8) NOT NULL,
    `commentA` varchar(255) DEFAULT NULL,
    `horde_id` int(8) NOT NULL,
    `commentH` varchar(255) DEFAULT NULL,
    PRIMARY KEY (`alliance_id`, `horde_id`)
) DEFAULT CHARSET=UTF8;

DROP TABLE IF EXISTS `player_factionchange_achievements`;
CREATE TABLE `player_factionchange_achievements` (
    `alliance_id` int(8) NOT NULL,
    `horde_id` int(8) NOT NULL,
    `commentA` varchar(255) NOT NULL,
    `commentH` varchar(255) NOT NULL,
    PRIMARY KEY (`alliance_id`,`horde_id`)
) DEFAULT CHARSET=UTF8;

DROP TABLE IF EXISTS `player_factionchange_items`;
CREATE TABLE `player_factionchange_items` (
    `alliance_id` int(8) NOT NULL,
    `commentA` varchar(255) DEFAULT NULL,
    `horde_id` int(8) NOT NULL,
    `commentH` varchar(255) DEFAULT NULL,
    PRIMARY KEY (`alliance_id`,`horde_id`)
) DEFAULT CHARSET=UTF8;

CREATE TABLE `player_factionchange_titles` (
    `alliance_id` int(8) NOT NULL,
    `commentA` varchar(255) DEFAULT NULL,
    `horde_id` int(8) NOT NULL,
    `commentH` varchar(255) DEFAULT NULL,
    PRIMARY KEY (`alliance_id`, `horde_id`)
) DEFAULT CHARSET=UTF8;

ALTER TABLE `creature_onkill_reputation` 
    ADD COLUMN `ChampioningAura` int(11) unsigned NOT NULL default '0' AFTER `TeamDependent`;
	
CREATE TABLE `pet_levelstats` (
  `creature_entry` mediumint(8) unsigned NOT NULL,
  `level` tinyint(3) unsigned NOT NULL COMMENT 'Level',
  `hp` smallint(5) unsigned NOT NULL COMMENT 'Health',
  `mana` smallint(5) unsigned NOT NULL COMMENT 'Mana',
  `armor` int(10) unsigned NOT NULL DEFAULT '0' COMMENT 'Armor',
  `mindmg` mediumint(11) NOT NULL DEFAULT '0' COMMENT 'Min base damage',
  `maxdmg` mediumint(11) NOT NULL DEFAULT '0' COMMENT 'Max base damage',
  `attackpower` mediumint(11) NOT NULL DEFAULT '0' COMMENT 'Attack power',
  `str` smallint(5) unsigned NOT NULL COMMENT 'Strength',
  `agi` smallint(5) unsigned NOT NULL COMMENT 'Agility',
  `sta` smallint(5) unsigned NOT NULL COMMENT 'Stamina',
  `inte` smallint(5) unsigned NOT NULL  COMMENT 'Intellect',
  `spi` smallint(5) unsigned NOT NULL  COMMENT 'Spirit',
  PRIMARY KEY (`creature_entry`,`level`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 PACK_KEYS=0 COMMENT='Stores pet levels stats.';