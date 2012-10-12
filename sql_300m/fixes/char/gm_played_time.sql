-- DROP TABLE IF EXISTS `gm_played_time`;
CREATE TABLE `gm_played_time` (
  `account` int(11) unsigned NOT NULL default '0' COMMENT 'Account Identifier',
  `playedTime` int(11) unsigned NOT NULL default '0',
  PRIMARY KEY  (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 ROW_FORMAT=DYNAMIC COMMENT='GM played time tracking system';