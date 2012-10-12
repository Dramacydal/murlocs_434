CREATE TABLE IF NOT EXISTS `auction_log` (
`guid` int(11) unsigned NOT NULL DEFAULT '0',
`seller` int(11) unsigned NOT NULL DEFAULT '0',
`item` int(11) unsigned NOT NULL DEFAULT '0',
`itemGuid` int(11) unsigned NOT NULL DEFAULT '0',
`money` int(11) NOT NULL DEFAULT '0',
`type` tinyint(3) NOT NULL DEFAULT '0',
`date` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP
)
ENGINE=InnoDb DEFAULT CHARSET=utf8 ROW_FORMAT=FIXED COMMENT='Auction log';