CREATE TABLE IF NOT EXISTS `character_mmr` (
  `guid` int(10) NOT NULL,
  `slot` tinyint(3) NOT NULL,
  `mmr` smallint(5) NOT NULL,
  PRIMARY KEY (`guid`,`slot`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;