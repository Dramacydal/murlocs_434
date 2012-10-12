DROP TABLE IF EXISTS `anticheat_log`;
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