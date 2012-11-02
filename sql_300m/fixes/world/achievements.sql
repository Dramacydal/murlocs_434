-- Leeeeeeeeeeeeeroy!
-- Rookery Egg
UPDATE `gameobject_template` SET `data3` = 15745 WHERE `entry` = 175124;
-- UPDATE `gameobject_template` SET `data3` = 16474 WHERE `entry` = 175124;
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 7622;
INSERT INTO `achievement_criteria_requirement` VALUE
(7622, 0, 0, 0);

-- Pirate's Day
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 10919;
INSERT INTO `achievement_criteria_requirement` (criteria_id, type, value1, value2) VALUE
(10919, 6, 35, 0);

-- Friend or Fowl?
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` = 3821;
INSERT INTO `achievement_criteria_requirement` VALUES
(3821, 0, 0, 0);

DELETE FROM `creature_ai_scripts` WHERE `creature_id` = 24746;
UPDATE `creature_template` SET `AIName` = '', `ScriptName` = 'npc_fjord_turkey' WHERE `entry` = 24746;

-- Trial of the Champion criterias
DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN
(11420, 12310, 12298, 12311, 12299, 12312, 12300, 12313, 12301, 12314, 12302, 12318, 12303, 12319, 12304, 12320, 12305, 12321, 12306, 12322)
AND `type` IN (1, 14);

DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN
(11559, 11560, 11561, 12307, 12308, 12309, 12315, 12316, 12317, 12323, 12324, 12439)
AND `type` = 14;

INSERT INTO `achievement_criteria_requirement` (`criteria_id`, `type`, `value1`, `value2`) VALUES
(11420, 1, 34705, 0),
(12310, 1, 34705, 0),

(12298, 1, 34703, 0),
(12311, 1, 34703, 0),

(12299, 1, 34701, 0),
(12312, 1, 34701, 0),

(12300, 1, 34702, 0),
(12313, 1, 34702, 0),

(12301, 1, 34657, 0),
(12314, 1, 34657, 0),
--
(12302, 1, 35572, 0),
(12318, 1, 35572, 0),

(12303, 1, 35617, 0),
(12319, 1, 35617, 0),

(12304, 1, 35571, 0),
(12320, 1, 35571, 0),

(12305, 1, 35569, 0),
(12321, 1, 35569, 0),

(12306, 1, 35570, 0),
(12322, 1, 35570, 0);

DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN
(1242, 1803, 1804, 1805, 1806, 1807, 1808, 1809, 1810, 1825, 
1811, 1812, 1813, 1814, 1815, 1816, 1817, 1818, 1819, 1826, 
5512, 5530, 5531, 5532, 
5529, 
5533, 5534, 5535, 9165, 9166, 
5536, 5537, 5538, 5539, 5540, 13254, 
5541, 5542, 5543,
5544,
5545,
5546,
5547,
5548,
13255,
6641, 6641, 6642, 6642, 6643, 6643, 6644, 6644, 6651, 6651, 6652, 6652, 6653, 6653, 6654, 6654, 6655, 6655, 6656, 6656, 6657, 6657, 6659, 6659, 
10391,10391,
7020, 7020, 7021, 7021, 
7625, 0, 0, 0,
7628,
11497, 11498, 11500, 11501, 12178, 12179, 12181, 12182,
12398, 12398,
425, 1824,
5601, 5603,
1235,
6447,
7631);

INSERT INTO `achievement_criteria_requirement` VALUES
-- Stormpike Perfection
(1242, 11, 0, 0),
(1803, 11, 0, 0),
(1804, 11, 0, 0),
(1805, 11, 0, 0),
(1806, 11, 0, 0),
(1807, 11, 0, 0),
(1808, 11, 0, 0),
(1809, 11, 0, 0),
(1810, 11, 0, 0),
(1825, 11, 0, 0),
-- Frostwolf Perfection
(1811, 11, 0, 0),
(1812, 11, 0, 0),
(1813, 11, 0, 0),
(1814, 11, 0, 0),
(1815, 11, 0, 0),
(1816, 11, 0, 0),
(1817, 11, 0, 0),
(1818, 11, 0, 0),
(1819, 11, 0, 0),
(1826, 11, 0, 0),
-- World Killing Blows
(5512, 0, 0, 0),
(5530, 0, 0, 0),
(5531, 0, 0, 0),
(5532, 0, 0, 0),
-- Total Killing Blows
(5529, 0, 0, 0),
-- Arena Killing Blows
(5533, 0, 0, 0),
(5534, 0, 0, 0),
(5535, 0, 0, 0),
(9165, 0, 0, 0),
(9166, 0, 0, 0),
-- Battleground Killing Blows
(5536, 0, 0, 0),
(5537, 0, 0, 0),
(5538, 0, 0, 0),
(5539, 0, 0, 0),
(5540, 0, 0, 0),
(13254, 0, 0, 0),
-- 2x2, 3x2, 5x5 Killing Blows
(5541, 0, 0, 0),
(5542, 0, 0, 0),
(5543, 0, 0, 0),
-- Alterac Valley Killing Blows
(5544, 6, 2597, 0),
-- Arathi Basin Killing Blows
(5545, 6, 3358, 0),
-- Warsong Gulch Killing Blows
(5546, 6, 3277, 0),
-- Eye of the Storm Killing Blows
(5547, 6, 3820, 0),
-- Strand of the Ancients Killing Blows
(5548, 6, 4384, 0),
-- Isle of Conquest Killing Blows
(13255, 6, 4710, 0),
-- School of Hard Knocks
(6641, 5, 58818, 0),
(6641, 16, 201, 0),
(6642, 5, 58818, 0),
(6642, 16, 201, 0),
(6643, 5, 58818, 0),
(6643, 16, 201, 0),
(6644, 5, 58818, 0),
(6644, 16, 201, 0),
(6651, 5, 58818, 0),
(6651, 16, 201, 0),
(6652, 5, 58818, 0),
(6652, 16, 201, 0),
(6653, 5, 58818, 0),
(6653, 16, 201, 0),
(6654, 5, 58818, 0),
(6654, 16, 201, 0),
(6655, 5, 58818, 0),
(6655, 16, 201, 0),
(6656, 5, 58818, 0),
(6656, 16, 201, 0),
(6657, 5, 58818, 0),
(6657, 16, 201, 0),
(6659, 5, 58818, 0),
(6659, 16, 201, 0),
-- Home Alone
(10391, 5, 58818, 0),
(10391, 16, 201, 0),
-- Not In My House
(7020, 6, 4571, 0),
(7020, 7, 23335, 0),
(7021, 6, 4572, 0),
(7021, 7, 23333, 0),
-- Artillery Veteran
(7625, 0, 0, 0),
-- Artillery Expert
(7628, 0, 0, 0),
-- Demolition Derby
(11497, 6, 4710, 0),
(11498, 6, 4710, 0),
(11500, 6, 4710, 0),
(11501, 6, 4710, 0),
(12178, 6, 4710, 0),
(12179, 6, 4710, 0),
(12181, 6, 4710, 0),
(12182, 6, 4710, 0),
-- Daily Chores
(12398, 5, 58818, 0),
(12398, 16, 201, 0),
-- Kill someone in the Field of Strife
(425, 6, 3057, 0),
-- Win Warsong (Ironman)
(1824, 0, 0, 0),
-- Quick Cap
(5601, 0, 0, 0),
(5603, 0, 0, 0),
-- We Had It All Along
(1235, 17, 1590, 1599),
-- Steady Hands
(6447, 6, 4384, 0),
-- Ancient Courtyard Protector
(7631, 6, 4609, 0);

-- fix some Mal'Ganis encounter (based on ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET)
DELETE FROM `achievement_criteria_requirement` WHERE  criteria_id IN (6432, 6435, 6433, 6434, 5887);
INSERT INTO `achievement_criteria_requirement` VALUES
 (6432, 12, 0, 0),
 (6435, 12, 0, 0),
 (6433, 12, 0, 0),
 (6434, 12, 1, 0),
 (5887, 12, 1, 0);