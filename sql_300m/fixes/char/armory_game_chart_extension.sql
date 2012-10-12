ALTER TABLE armory_game_chart
  ADD COLUMN `personalRating` int(11) NOT NULL AFTER `end`,
  ADD COLUMN `ip` text AFTER `personalRating`;