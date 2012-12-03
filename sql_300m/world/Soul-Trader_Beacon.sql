
-- support for Soul-Trader Beacon (http://old.wowhead.com/item=38050) and Ethereal Credit (http://old.wowhead.com/item=38186)
-- WARNING! Script support NEEDED!

UPDATE `creature_template` SET `npcflag`='129', `scale`='1.5', `ScriptName`='pet_soul_trader_beacon' WHERE `entry`='27914';

DELETE FROM `npc_vendor` WHERE `entry` = 27914;

INSERT INTO `npc_vendor` (`entry`, `item`, `maxcount`, `incrtime`, `ExtendedCost`) VALUES
 (27914, 38308, 0, 0, 2411),
 (27914, 38300, 0, 0, 2411),
 (27914, 38294, 0, 0, 2412),
 (27914, 38291, 0, 0, 2408),
 (27914, 38160, 0, 0, 2410),
 (27914, 38161, 0, 0, 2409),
 (27914, 38162, 0, 0, 2409),
 (27914, 38163, 0, 0, 2408),
 (27914, 38285, 0, 0, 2408),
 (27914, 38286, 0, 0, 2407);
