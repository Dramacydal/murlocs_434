DROP TABLE IF EXISTS `character_renames`;
CREATE TABLE `character_renames` (
  `guid` int(11) NOT NULL,
  `oldname` text,
  `newname` text,
  `date` int(11) NOT NULL,
   PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='';