DROP TABLE IF EXISTS `warden_data_result_cata`;

CREATE TABLE `warden_data_result_cata` (
  `id` int(4) NOT NULL auto_increment,
  `check` int(3) default NULL,
  `data` tinytext,
  `str` tinytext,
  `address` int(8) default NULL,
  `length` int(2) default NULL,
  `result` tinytext,
  `comment` text,
  PRIMARY KEY  (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COMMENT='Warden data checks (cata)';

-- Warden data from TOM_RUS, R2 and TrinityCore
INSERT INTO `warden_data_result_cata` (`id`, `check`, `data`, `str`, `address`, `length`, `result`, `comment`) VALUES (1, 217, '', 'WPESPY.DLL', 0, 0, '', 'WPE PRO - injected dll');
INSERT INTO `warden_data_result_cata` (`id`, `check`, `data`, `str`, `address`, `length`, `result`, `comment`) VALUES (2, 217, '', 'RPE.DLL', 0, 0, '', 'rEdoX Packet Editor - injected dll');

-- Warden data from 300murlocs
INSERT INTO `warden_data_result_cata` (`id`, `check`, `data`, `str`, `address`, `length`, `result`, `comment`) VALUES (3, 178, 'A444519C9C7A9FC73FF1E41094FDBEDE716DC068B82952BE', '', 40815, 81, '', 'WPE PRO: HackTool.Win32.Sniffer.WpePro.w');
INSERT INTO `warden_data_result_cata` (`id`, `check`, `data`, `str`, `address`, `length`, `result`, `comment`) VALUES (4, 178, 'A444519C9C7A9FC73FF1E41094FDBEDE716DC068B82952BE', '', 40623, 81, '', 'WPE PRO: HackTool.Win32.Sniffer.WpePro.y');
INSERT INTO `warden_data_result_cata` (`id`, `check`, `data`, `str`, `address`, `length`, `result`, `comment`) VALUES (5, 178, 'A444519CE7D2730A09B1BFE6F7EBCC62B46FC702D2875BB4', '', 361000, 84, '', 'rEdoX Packet Editor');
INSERT INTO `warden_data_result_cata` (`id`, `check`, `data`, `str`, `address`, `length`, `result`, `comment`) VALUES (6, 178, '33F11A8CA5BF8903F67BDF7A8D8C570CFEB62687610662BB', '', 16948, 27, '', 'AntiWarden tools: WardenNinja');