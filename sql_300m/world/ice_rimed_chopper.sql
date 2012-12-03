-- Ice-Rimed Chopper to The Son's of Hodir quartermaster
DELETE FROM `npc_vendor` WHERE `entry` = 32540 AND `item` = 44191;
INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUE
(32540, 44191, 0, 0, 0);