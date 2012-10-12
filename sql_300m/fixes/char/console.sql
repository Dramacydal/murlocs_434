-- ----------------------------
-- Table structure for console
-- ----------------------------
CREATE TABLE `console` (
  `id` int(11) NOT NULL auto_increment,
  `command` text,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

ALTER TABLE `console` ADD `user` VARCHAR( 50 ) NOT NULL AFTER `command`;