CREATE TABLE `character_account_log` (
  `guid` int(11) unsigned NOT NULL default '0' COMMENT 'Global Unique Identifier',
  `oldacc` int(11) unsigned NOT NULL default '0' COMMENT 'Old Account Identifier',
  `newacc` int(11) unsigned NOT NULL default '0' COMMENT 'New Account Identifier',
  `date` timestamp NOT NULL default CURRENT_TIMESTAMP
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Character transfer logs';