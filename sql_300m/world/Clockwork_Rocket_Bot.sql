
-- removed in r564 (remark)
-- restoring them...

UPDATE `creature_template` SET `npcflag`='128' WHERE `entry`='24968';

REPLACE INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
(24968, 34498, 0, 0, 0),
(24968, 44481, 0, 0, 0);
