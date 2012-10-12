CREATE TABLE `db_stat` (
  `guid` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `queryAvgLifetime` INT UNSIGNED NOT NULL,
  `longQueryes` INT  NOT NULL,
  `queryPerSecond` INT UNSIGNED NOT NULL,
  `fetchAvgTime` INT UNSIGNED NOT NULL,
  `fetchPerSecond` INT UNSIGNED NOT NULL,
  `realQueryAvgPerformTime` INT UNSIGNED NOT NULL,
  `online` SMALLINT UNSIGNED NOT NULL,
  `realmId` TINYINT UNSIGNED NOT NULL,
  `dbName` TINYINT UNSIGNED NOT NULL,
  PRIMARY KEY (`guid`)
)
ENGINE = MyISAM
COMMENT = 'stat info for this db, dbName: 0 = char 1 = world 2 = login';

