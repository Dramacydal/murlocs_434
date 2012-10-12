-- DROP TABLE IF EXISTS `gmlog_commands`;

CREATE TABLE `gmlog_commands` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `command` varchar(255) NOT NULL,
  `account` int(10) unsigned NOT NULL,
  `player` varchar(12) NOT NULL,
  `posX` float NOT NULL,
  `posY` float NOT NULL,
  `posZ` float NOT NULL,
  `selected_type` varchar(255) DEFAULT NULL,
  `selected_guid` bigint(20) unsigned DEFAULT NULL,
  `map` int(10) unsigned NOT NULL,
  `ip` varchar(15) NOT NULL DEFAULT '0.0.0.0',
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='GM logs';