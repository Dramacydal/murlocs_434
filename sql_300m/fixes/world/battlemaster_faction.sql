UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35 WHERE `npcflag` & 0x140000 <> 0 AND
(`faction_A` = 1615 OR `faction_H` = 1615);