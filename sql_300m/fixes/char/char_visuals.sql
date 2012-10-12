DROP TABLE IF EXISTS `characters_visuals`;
CREATE TABLE  `characters_visuals` (
  `guid` int(11) unsigned NOT NULL,
  `head` int(11) unsigned NOT NULL,
  `shoulders` int(11) unsigned NOT NULL,
  `chest` int(11) unsigned NOT NULL,
  `waist` int(11) unsigned NOT NULL,
  `legs` int(11) unsigned NOT NULL,
  `feet` int(11) unsigned NOT NULL,
  `wrists` int(11) unsigned NOT NULL,
  `hands` int(11) unsigned NOT NULL,
  `back` int(11) unsigned NOT NULL,
  `main` int(11) unsigned NOT NULL,
  `off` int(11) unsigned NOT NULL,
  `ranged` int(11) unsigned NOT NULL,
  `type` int(11) unsigned NOT NULL,
  PRIMARY KEY (`guid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Player extra visuality items';