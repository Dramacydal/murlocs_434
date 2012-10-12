CREATE TABLE `character_race_change` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `oldrace` int(11) unsigned NOT NULL default '0' COMMENT 'Old Race',
  `newrace` int(11) unsigned NOT NULL default '0' COMMENT 'New Race',
  `oldname` text,
  `newname` text,
  `date` timestamp NOT NULL default CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Character race/faction change logs';