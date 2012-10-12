DROP TABLE IF EXISTS `boss_cost`;
CREATE TABLE IF NOT EXISTS `boss_cost` (
  `boss` int(10) unsigned NOT NULL default '0',
  `flag` int(10) unsigned NOT NULL default '0',
  `cost` int(10) unsigned NOT NULL default '0',
  UNIQUE KEY `boss` (`boss`,`flag`,`cost`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

DROP TABLE IF EXISTS `boss_kills`;
CREATE TABLE IF NOT EXISTS `boss_kills` (
  `guild` int(10) unsigned NOT NULL default '0',
  `boss` int(10) unsigned NOT NULL default '0',
  `flag` int(10) unsigned NOT NULL default '0',
  `count` int(10) unsigned NOT NULL default '0',
  `time` timestamp NOT NULL default CURRENT_TIMESTAMP on update CURRENT_TIMESTAMP,
  UNIQUE KEY `guild` (`guild`,`boss`,`flag`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
        

/* by Pashtet

- statistics

SELECT guild, sum( cost * count ) AS total_points
FROM `boss_kills` 
INNER JOIN boss_cost
USING ( boss, flag ) 
GROUP BY guild
ORDER BY total_points DESC 
LIMIT 0 , 10

*/
