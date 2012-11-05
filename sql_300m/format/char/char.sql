CREATE TABLE IF NOT EXISTS `anticheat_log` (
  `entry`          bigint(20) NOT NULL auto_increment,
  `acctid`         int(11) NOT NULL,
  `guid`           int(11) unsigned NOT NULL,
  `playername`     varchar(32) NOT NULL,
  `checktype`      mediumint(8) unsigned NOT NULL,
  `alarm_time`     datetime NOT NULL,
  `action1`        mediumint(8) NOT NULL default '0',
  `action2`        mediumint(8) NOT NULL default '0',
  `reason`         varchar(255) NOT NULL DEFAULT 'Unknown',
  `Map`            smallint(5) NOT NULL default '-1',
  `Pos`            varchar(255) NOT NULL default '0',
  `Level`          mediumint(9) NOT NULL default '0',
   PRIMARY KEY  (`entry`),
   KEY `idx_Player` (`guid`)
)  ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Anticheat log table';

CREATE TABLE `armory_character_stats` (
  `guid` int(11) NOT NULL,
  `data` longtext NOT NULL,
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='World of Warcraft Armory table';

CREATE TABLE `armory_game_chart` (
  `gameid` int(11) NOT NULL,
  `teamid` int(11) NOT NULL,
  `guid` int(11) NOT NULL,
  `changeType` int(11) NOT NULL,
  `ratingChange` int(11) NOT NULL,
  `teamRating` int(11) NOT NULL,
  `damageDone` int(11) NOT NULL,
  `deaths` int(11) NOT NULL,
  `healingDone` int(11) NOT NULL,
  `damageTaken` int(11) NOT NULL,
  `healingTaken` int(11) NOT NULL,
  `killingBlows` int(11) NOT NULL,
  `mapId` int(11) NOT NULL,
  `start` int(11) NOT NULL,
  `end` int(11) NOT NULL,
  `personalRating` int(11) NOT NULL,
  `ip` text,
  `type` int(11) NOT NULL,
  PRIMARY KEY (`gameid`,`teamid`,`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='WoWArmory Game Chart';

 CREATE TABLE IF NOT EXISTS `auction_log` (
	`guid` int(11) unsigned NOT NULL DEFAULT '0',
	`seller` int(11) unsigned NOT NULL DEFAULT '0',
	`item` int(11) unsigned NOT NULL DEFAULT '0',
	`itemGuid` int(11) unsigned NOT NULL DEFAULT '0',
	`money` int(11) NOT NULL DEFAULT '0',
	`type` tinyint(3) NOT NULL DEFAULT '0',
	`date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
)
ENGINE=InnoDb DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Auction log';

CREATE TABLE `item_soulbound_trade_data` (
  `itemGuid` int(16) unsigned NOT NULL DEFAULT '0',
  `allowedPlayers` varchar(255) NOT NULL DEFAULT '',
  PRIMARY KEY (`itemGuid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='BOP item trade cache';

CREATE TABLE  `characters_visuals` (
  `guid` int(11) unsigned NOT NULL,
  `head` int(11) unsigned NOT NULL,
  `shoulders` int(11) unsigned NOT NULL,
  `chest` int(11) unsigned NOT NULL,
  `waist` int(11) unsigned NOT NULL,
  `legs` int(11) unsigned NOT NULL,
  `feet` int(11) unsigned NOT NULL,
  `wrists` int(11) unsigned NOT NULL,
  `hands` int(11) unsigned NOT NULL,
  `back` int(11) unsigned NOT NULL,
  `main` int(11) unsigned NOT NULL,
  `off` int(11) unsigned NOT NULL,
  `ranged` int(11) unsigned NOT NULL,
  `type` int(11) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Player extra visuality items';

CREATE TABLE `character_account_log` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `oldacc` int(11) unsigned NOT NULL default '0' COMMENT 'Old Account Identifier',
  `newacc` int(11) unsigned NOT NULL default '0' COMMENT 'New Account Identifier',
  `date` timestamp NOT NULL default CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Character transfer logs';

CREATE TABLE `character_banned` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Character guid',
  `bandate` bigint(40) NOT NULL default '0',
  `unbandate` bigint(40) NOT NULL default '0',
  `bannedby` varchar(50) NOT NULL,
  `banreason` varchar(255) NOT NULL,
  `active` tinyint(4) NOT NULL default '1',
  PRIMARY KEY  (`guid`,`bandate`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='Character Ban List';

CREATE TABLE IF NOT EXISTS `character_mmr` (
  `guid` int(10) NOT NULL,
  `slot` tinyint(3) NOT NULL,
  `mmr` smallint(5) NOT NULL,
  PRIMARY KEY (`guid`,`slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE `character_pet`
	CHANGE `Reactstate` `Reactstate` INT(10) UNSIGNED NOT NULL DEFAULT '0';

CREATE TABLE `character_race_change` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `oldrace` int(11) unsigned NOT NULL default '0' COMMENT 'Old Race',
  `newrace` int(11) unsigned NOT NULL default '0' COMMENT 'New Race',
  `oldname` text,
  `newname` text,
  `date` timestamp NOT NULL default CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Character race/faction change logs';

CREATE TABLE `character_battleground_random` (
  `guid` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

ALTER TABLE `saved_variables` ADD COLUMN `NextRandomBGResetTime` bigint(40) unsigned NOT NULL default '0' AFTER `NextWeeklyQuestResetTime`;

CREATE TABLE `character_renames` (
  `guid` int(11) NOT NULL,
  `oldname` text,
  `newname` text,
  `date` int(11) NOT NULL,
   PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='';

CREATE TABLE `console` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `done` timestamp NULL DEFAULT NULL,
  `command` text,
  `user` varchar(50) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=17 DEFAULT CHARSET=utf8;

CREATE TABLE `queuedGifts` (
  `qIdx` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `charGuid` int(10) unsigned NOT NULL,
  `itemId` int(10) unsigned NOT NULL,
  `itemStackCnt` int(10) unsigned DEFAULT '1',
  `itemGuid` int(10) unsigned DEFAULT NULL,
  `sentFlag` int(10) unsigned DEFAULT '0',
  `text` text,
  `subj` varchar(255) DEFAULT '',
  `gold` int(10) unsigned DEFAULT '0',
  `timestamp` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`qIdx`)
) ENGINE=InnoDB AUTO_INCREMENT=84071 DEFAULT CHARSET=utf8;

CREATE TABLE `gm_played_time` (
  `account` int(11) unsigned NOT NULL default '0' COMMENT 'Account Identifier',
  `playedTime` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='GM played time tracking system';

CREATE TABLE `gmlog_commands` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `command` varchar(255) NOT NULL,
  `account` int(10) unsigned NOT NULL,
  `player` varchar(30) NOT NULL,
  `posX` float NOT NULL,
  `posY` float NOT NULL,
  `posZ` float NOT NULL,
  `selected_type` varchar(255) DEFAULT NULL,
  `selected_guid` bigint(20) unsigned DEFAULT NULL,
  `map` int(10) unsigned NOT NULL,
  `ip` varchar(20) NOT NULL DEFAULT '0.0.0.0',
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1351458 DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='GM logs';

CREATE TABLE `item_instance_text` (
`guid` INT UNSIGNED  NOT NULL,
`text` LONGTEXT,
 PRIMARY KEY ( `guid` )
);

INSERT INTO item_instance_text SELECT guid, text FROM item_instance as it WHERE it.text != '';
ALTER TABLE item_instance DROP `text`;

CREATE TABLE `item_refund` (
  `item_guid` int(10) unsigned NOT NULL COMMENT 'Item GUID',
  `player_guid` int(10) unsigned NOT NULL COMMENT 'Player GUID',
  `paidMoney` int(10) unsigned NOT NULL DEFAULT '0',
  `paidExtendedCost` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`item_guid`,`player_guid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='Item Refund System';

ALTER TABLE `group_member`
	ADD COLUMN `roles` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member roles bit mask' AFTER `subgroup`;
ALTER TABLE `group_member`
	CHANGE `assistant` `memberFlags` TINYINT(1) UNSIGNED NOT NULL DEFAULT '0' COMMENT 'Member flags bit mask';

ALTER TABLE `groups` DROP `mainTank`,
	DROP `mainAssistant`;

ALTER TABLE `characters` ADD COLUMN `grantableLevels` tinyint(3) unsigned NOT NULL default '0' AFTER `actionBars`;
-- for conversion from 335
ALTER TABLE `characters` ADD COLUMN `arenaPoints` int(11) unsigned NOT NULL default '0' AFTER `deleteDate`;
ALTER TABLE `characters` ADD COLUMN `totalHonorPoints` int(11) unsigned NOT NULL default '0' AFTER `arenaPoints`;
-- just 335 compatibility
ALTER TABLE `characters` ADD COLUMN `todayHonorPoints` int(11) unsigned NOT NULL default '0' AFTER `totalHonorPoints`;
ALTER TABLE `characters` ADD COLUMN `yesterdayHonorPoints` int(11) unsigned NOT NULL default '0' AFTER `todayHonorPoints`;
ALTER TABLE `characters` ADD COLUMN `knownCurrencies` int(11) unsigned NOT NULL default '0' AFTER `yesterdayHonorPoints`;
ALTER TABLE `characters` ADD COLUMN `power6` int(11) unsigned NOT NULL default '0' AFTER `knownCurrencies`;
ALTER TABLE `characters` ADD COLUMN `power7` int(11) unsigned NOT NULL default '0' AFTER `power6`;
ALTER TABLE `characters` ADD COLUMN `ammoId` int(11) unsigned NOT NULL default '0' AFTER `power7`;

CREATE TABLE `account_forcepermission` (
  `id` int(11) unsigned NOT NULL DEFAULT '0',
  `security` int(3) unsigned DEFAULT '0',
  `comment` varchar(255) DEFAULT '',
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

CREATE TABLE `character_feed_log` (
  `guid` int(11) NOT NULL,
  `type` smallint(1) NOT NULL,
  `data` int(11) NOT NULL,
  `date` int(11) DEFAULT NULL,
  `counter` int(11) NOT NULL,
  `difficulty` smallint(6) DEFAULT '-1',
  PRIMARY KEY (`guid`,`type`,`data`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- optimizations
ALTER TABLE `character_achievement_progress`
	MODIFY COLUMN `date`  int(4) UNSIGNED NOT NULL AFTER `counter`;

ALTER TABLE `character_queststatus` 
	MODIFY COLUMN `status` TINYINT UNSIGNED NOT NULL,
	MODIFY COLUMN `mobcount1` SMALLINT UNSIGNED NOT NULL,
	MODIFY COLUMN `mobcount2` SMALLINT UNSIGNED NOT NULL AFTER `mobcount1`,
	MODIFY COLUMN `mobcount3` SMALLINT UNSIGNED NOT NULL AFTER `mobcount2`,
	MODIFY COLUMN `mobcount4` SMALLINT UNSIGNED NOT NULL AFTER `mobcount3`,
	MODIFY COLUMN `itemcount1` SMALLINT UNSIGNED NOT NULL,
	MODIFY COLUMN `itemcount2` SMALLINT UNSIGNED NOT NULL AFTER `itemcount1`,
	MODIFY COLUMN `itemcount3` SMALLINT UNSIGNED NOT NULL AFTER `itemcount2`,
	MODIFY COLUMN `itemcount4` SMALLINT UNSIGNED NOT NULL AFTER `itemcount3`,
	MODIFY COLUMN `itemcount5` SMALLINT UNSIGNED NOT NULL AFTER `itemcount4`,
	MODIFY COLUMN `itemcount6` SMALLINT UNSIGNED NOT NULL AFTER `itemcount5`,
	MODIFY COLUMN `timer`  int(4) UNSIGNED NOT NULL AFTER `explored`;

ALTER TABLE `character_reputation` CHANGE `faction` `faction` SMALLINT UNSIGNED NOT NULL;
ALTER TABLE `character_reputation` CHANGE `flags` `flags` SMALLINT UNSIGNED NOT NULL;

ALTER TABLE `pet_spell` CHANGE `active` `active` TINYINT UNSIGNED NOT NULL;

