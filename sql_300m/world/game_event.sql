DELETE FROM `game_event` WHERE `entry` IN (102, 103);
INSERT INTO `game_event` (`entry`, `start_time`, `end_time`, `occurence`, `length`, `holiday`, `description`) VALUES
(102, '2012-04-01 00:00:00', '2020-01-23 00:00:00', 525600, 1440, 0, 'April Fool''s Day. Morph to goblins and worgens.'),
(103, '2012-04-01 00:00:00', '2020-01-23 00:00:00', 525600, 1440, 0, 'April Fool''s Day. Morph to other team.');