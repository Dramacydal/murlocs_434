CREATE TABLE `bugged_quests` (
  `entry` mediumint(8) unsigned NOT NULL DEFAULT '0',
  `comment` char(100) NOT NULL DEFAULT '0',
  PRIMARY KEY (`entry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Bugged quests table. These quests can be autocompleted by players with a command.';

DELETE FROM `mangos_string` WHERE `entry` IN (11136, 11137, 11138, 11139, 11140, 11141);
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11136, 'Such quests not exists or already marked as bugged.', ''),
(11137, 'Such quests not exists or not marked as bugged.', ''),
(11138, 'You have added quest %u to bugged quest list.', ''),
(11139, 'You have removed quest %u from bugged quest list.', ''),
(11140, 'Quest id %u not found in bugged quest list.', ''),
(11141, 'Quest %d - |cffffffff|Hquest:%d:%d|h[%s]|h|r completed.', '');