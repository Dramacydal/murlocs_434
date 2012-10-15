-- --------------------------
-- Guards around the worlds -
-- --------------------------

-- NEEDS MORE RESEARCH

-- Cenarion hold infantry
  -- adding attack spells to creature template
UPDATE `creature_template` SET `spell1` = 18328, `spell2` = 19643, `spell3` = 15618, `flags_extra` = 0 WHERE `entry` = 15184;

-- Bluff watcher ( thunderbluff )
  -- correcting guard spells
UPDATE `creature_template` SET `spell1` = 40505, `spell2` = 12024, `spell3` = 23337, `flags_extra` = 0 WHERE `entry` = 3084;

-- orc guards
UPDATE `creature_template` SET `spell1` = 8599, `spell2` = 40505, `flags_extra` = 0 WHERE `entry` = 3296;

-- Darn Guards
UPDATE `creature_template` SET `spell1` = 40505, `spell2` = 8599, `flags_extra` = 0 WHERE `entry` = 4262;

-- stormwind guards Guards
UPDATE `creature_template` SET `spell1` = 40505, `spell2` = 8599, `flags_extra` = 0 WHERE `entry` = 68;

-- Undercity Guards
UPDATE `creature_template` SET `spell1` = 59395, `spell2` = 7160, `flags_extra` = 0 WHERE `entry` = 5624;
