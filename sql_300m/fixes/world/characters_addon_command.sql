DELETE FROM `command` WHERE `name` IN ('character addon set', 'character addon remove');
INSERT INTO `command` VALUE
('character addon set', 3, 'Syntax: .character addon set $name $modelid $phase $scale $faction \r\n Sets and addon to specified character with specified parameters.'),
('character addon remove', 3, 'Syntax: .character addon remove $name \r\n Removes an addon from a player. Character must re-log to drop model id.');