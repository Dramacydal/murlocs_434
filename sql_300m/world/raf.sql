-- Refer-a-friend system by MasOn
DELETE FROM `command` WHERE `name` IN
('account friend add', 'account friend delete', 'account friend list');

INSERT INTO `command` (`name`, `security`, `help`) VALUES
('account friend add', 4, 'Syntax: .account friend add [#accountId|$accountName] [#friendaccountId|$friendaccountName]\r\n\r\nSet friend account.'),
('account friend delete', 4, 'Syntax: .account friend delete [#accountId|$accountName] [#friendaccountId|$friendaccountName]\r\n\r\nDelete friend account.'),
('account friend list', 4, 'Syntax: .account friend list [#accountId|$accountName]\r\n\r\nList friends for account.');

DELETE FROM `mangos_string` WHERE `entry` IN (11059, 11060);
INSERT INTO `mangos_string` VALUES
(11059,'RAF system ok.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
(11060,'RAF system error.', NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);