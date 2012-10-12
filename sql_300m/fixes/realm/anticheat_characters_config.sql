-- Anticheat
-- Config

/*Table structure for table `anticheat_config` */

DROP TABLE IF EXISTS `anticheat_config`;

CREATE TABLE `anticheat_config` (
  `checktype` mediumint(8) unsigned NOT NULL COMMENT 'Type of check',
  `description` varchar(255) default NULL,
  `check_period` int(11) unsigned NOT NULL default '0' COMMENT 'Time period of check, in ms, 0 - always',
  `alarmscount` int(11) unsigned NOT NULL default '1' COMMENT 'Count of alarms before action',
  `disableoperation` tinyint(3) unsigned NOT NULL default '0' COMMENT 'Anticheat disable operations in main core code after check fail',
  `messagenum` int(11) NOT NULL default '0' COMMENT 'Number of system message',
  `intparam1` mediumint(8) NOT NULL default '0' COMMENT 'Int parameter 1',
  `intparam2` mediumint(8) NOT NULL default '0' COMMENT 'Int parameter 2',
  `floatparam1` float NOT NULL default '0' COMMENT 'Float parameter 1',
  `floatparam2` float NOT NULL default '0' COMMENT 'Float parameter 2',
  `action1` mediumint(8) NOT NULL default '0' COMMENT 'Action 1',
  `actionparam1` mediumint(8) NOT NULL default '0' COMMENT 'Action parameter 1',
  `action2` mediumint(8) NOT NULL default '0' COMMENT 'Action 1',
  `actionparam2` mediumint(8) NOT NULL default '0' COMMENT 'Action parameter 1',
  `disabledzones` varchar(255) NOT NULL DEFAULT '' COMMENT 'List of zones, in which check disabled.',
  PRIMARY KEY  (`checktype`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Anticheat configuration';



-- Main checks
INSERT INTO `anticheat_config` VALUES ('0', 'Null check', '0', '1', '0', '11013', '0', '0', '0', '0', '1', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('1', 'Movement cheat', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('2', 'Spell cheat', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('3', 'Quest cheat', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('4', 'Transport cheat', '0', '3', '0', '11013', '0', '0', '60', '0', '0', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('5', 'Damage cheat', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('6', 'Item cheat', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('7', 'Warden check', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');

-- Subchecks
INSERT INTO `anticheat_config` VALUES ('101', 'Speed hack', '500', '2', '0', '11013', '10000', '0', '0.005', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('102', 'Fly hack', '500', '2', '0', '11013', '20000', '0', '10', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('103', 'Wall climb hack', '500', '3', '0', '11013', '10000', '0', '0.0015', '2.37', '0', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('104', 'Waterwalking hack', '1000', '3', '0', '11013', '20000', '0', '0', '0', '0', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('105', 'Teleport to plane hack', '500', '1', '1', '11013', '0', '0', '0.0001', '0.1', '0', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('106', 'AirJump hack', '1000', '3', '0', '11013', '30000', '0', '10', '25', '0', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('107', 'Teleport hack', '0', '1', '0', '11013', '0', '0', '81', '0', '2', '1', '9', '0', '');
INSERT INTO `anticheat_config` VALUES ('108', 'Fall hack', '0', '3', '0', '11013', '10000', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('109', 'Z Axis hack', '1000', '2', '0', '11013', '0', '0', '0.00001', '10', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('201', 'Spell invalid', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('202', 'Spellcast in dead state', '0', '1', '0', '11013', '0', '0', '0', '0', '0', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('203', 'Spell not valid for player', '0', '1', '0', '11013', '0', '0', '0', '0', '5', '86400', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('204', 'Spell not in player book', '0', '1', '0', '11013', '0', '0', '0', '0', '0', '0', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('501', 'Spell damage hack', '0', '1', '0', '11013', '0', '180000', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('502', 'Melee damage hack', '0', '1', '0', '11013', '0', '180000', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('601', 'Item dupe hack', '0', '1', '0', '11013', '0', '0', '0', '0', '2', '1', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('701', 'Warden memory check', '0', '1', '0', '11013', '0', '0', '0', '0', '5', '86400', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('702', 'Warden key check', '0', '1', '0', '11013', '0', '0', '0', '0', '5', '86400', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('703', 'Warden checksum check', '0', '2', '0', '11013', '0', '0', '0', '0', '5', '86400', '0', '0', '');
INSERT INTO `anticheat_config` VALUES ('704', 'Warden timeout check', '0', '1', '0', '11013', '0', '0', '0', '0', '0', '0', '0', '0', '');
