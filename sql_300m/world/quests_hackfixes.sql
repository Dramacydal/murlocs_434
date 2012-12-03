-- FOR ALL FOLLOWING QUESTS IT IS REQUIRED SCRIPTS. AT CORRECTION THROUGH SCRIPTS THESE HACKS NEED TO REVERT!

-- 28.04.2012

-- The Storm King's Vengeance (hack! Need vechile and aura 55430 support)
-- UPDATE `creature` SET `phaseMask`='256' WHERE id IN (29934, 29897, 29887, 29892, 29935, 29889, 29894, 37976, 29893, 29890, 29891, 37893, 29872, 29895, 29821);
UPDATE `creature` SET `phaseMask`='1' WHERE id IN (29934, 29897, 29887, 29892, 29935, 29889, 29894, 37976, 29893, 29890, 29891, 37893, 29872, 29895, 29821);

-- Slim Pickings (hack! drop http://wh.300murlocs.com/?item=36765 from http://wh.300murlocs.com/?npc=26358)
DELETE FROM `creature_loot_template` WHERE `entry` = 26358 AND `item` = 36765;
INSERT INTO `creature_loot_template` (`entry`, `item`, `ChanceOrQuestChance`, `groupid`, `mincountOrRef`, `maxcount`, `lootcondition`, `condition_value1`, `condition_value2`) VALUES (26358, 36765, -100, 0, 1, 1, 0, 0, 0);

-- The Chain Gun And You (http://wh.300murlocs.com/?quest=12457)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='27788', `ReqCreatureOrGOCount1`='8' WHERE `entry`='12457';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='12457';

-- I've Got a Flying Machine! (http://wh.300murlocs.com/?quest=11390)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='24439', `ReqCreatureOrGOCount1`='3' WHERE `entry`='11390';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='11390';

-- Putting Olakin Back Together Again (http://wh.300murlocs.com/?quest=13220)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='31235', `ReqCreatureOrGOCount1`='1' WHERE `entry`='13220';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13220';

-- Tirion's Gambit (http://wh.300murlocs.com/?quest=13364 & http://wh.300murlocs.com/?quest=13403)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId2`='32648', `ReqCreatureOrGOCount2`='1', `ReqSourceId1`='44784', `ReqSourceCount1`='1' WHERE `entry` IN (13364, 13403);
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOCount2`='0', `ReqSourceId1`='0', `ReqSourceCount1`='0' WHERE `entry` IN (13364, 13403);

-- Generosity Abounds (http://wh.300murlocs.com/?quest=13146)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='31075', `ReqCreatureOrGOCount1`='4' WHERE `entry`='13146';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13146';

-- Killing Two Scourge With One Skeleton (http://wh.300murlocs.com/?quest=13144)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='30995', `ReqCreatureOrGOCount1`='3' WHERE `entry`='13144';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13144';

-- Sniffing Out the Perpetrator (http://wh.300murlocs.com/?quest=12910)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='29677', `ReqCreatureOrGOCount1`='1', `ReqCreatureOrGOId2`='29695', `ReqCreatureOrGOCount2`='1' WHERE `entry`='12910';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOCount2`='0' WHERE `entry`='12910';

-- The Heart of the Storm (http://wh.300murlocs.com/?quest=12998)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='30299', `ReqCreatureOrGOCount1`='1' WHERE `entry`='12998';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='12998';

-- King of the Mountain (http://wh.300murlocs.com/?quest=13283)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='31766', `ReqCreatureOrGOCount1`='1' WHERE `entry`='13283';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13283';

-- The Reckoning (http://wh.300murlocs.com/?quest=13047)
-- UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='2', `ReqSourceId1`='42840', `ReqSourceCount1`='1' WHERE `entry`='13047';
UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='0', `ReqSourceId1`='0', `ReqSourceCount1`='0' WHERE `entry`='13047';

-- Feeding Arngrim (http://wh.300murlocs.com/?quest=13046)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='30421', `ReqCreatureOrGOCount1`='5' WHERE `entry`='13046';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13046';

-- Find the Ancient Hero (http://wh.300murlocs.com/?quest=13133)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='30880', `ReqCreatureOrGOCount1`='1', `ReqSourceId1`='43166', `ReqSourceCount1`='1' WHERE `entry`='13133';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0', `ReqSourceId1`='0', `ReqSourceCount1`='0' WHERE `entry`='13133';

-- Changing the Wind's Course (http://wh.300murlocs.com/?quest=13058)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='-194123', `ReqCreatureOrGOCount1`='1' WHERE `entry`='13058';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13058';

-- 29.04.2012

-- You, Robot (http://wh.300murlocs.com/?quest=10248)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='19851', `ReqCreatureOrGOCount1`='1' WHERE `entry`='10248';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='10248';

-- Teleport This! (http://wh.300murlocs.com/?quest=10857)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='22348', `ReqCreatureOrGOId2`='22350', `ReqCreatureOrGOId3`='22351', `ReqCreatureOrGOCount1`='1', `ReqCreatureOrGOCount2`='1', `ReqCreatureOrGOCount3`='1' WHERE `entry`='10857';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOId3`='0', `ReqCreatureOrGOCount1`='0', `ReqCreatureOrGOCount2`='0', `ReqCreatureOrGOCount3`='0' WHERE `entry`='10857';

-- Delivering the Message (http://wh.300murlocs.com/?quest=10406)
-- UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='2' WHERE `entry`='10406';
UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='0' WHERE `entry`='10406';

-- Torching Sunfury Hold (http://wh.300murlocs.com/?quest=10233)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='19723', `ReqCreatureOrGOId2`='19724', `ReqCreatureOrGOCount1`='4', `ReqCreatureOrGOCount2`='4', WHERE `entry`='10233';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOCount1`='0', `ReqCreatureOrGOCount2`='0' WHERE `entry`='10233';

-- Seeker of Truth (http://wh.300murlocs.com/?quest=11082)
-- UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='2' WHERE `entry`='11082';
UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='0' WHERE `entry`='11082';

-- Subdue the Subduer (http://wh.300murlocs.com/?quest=11090)
-- UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='2' WHERE `entry`='11090';
UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='0' WHERE `entry`='11090';

-- A Necessary Distraction (http://wh.300murlocs.com/?quest=10637)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='21892', `ReqCreatureOrGOCount1`='1' WHERE `entry`='10637';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='10637';

-- A Necessary Distraction (http://wh.300murlocs.com/?quest=10688)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='21892', `ReqCreatureOrGOCount1`='1' WHERE `entry`='10688';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='10688';

-- The Deadliest Trap Ever Laid (http://wh.300murlocs.com/?quest=11097)
-- UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='3' WHERE `entry`='11097';
UPDATE `quest_template` SET `Method`='2', `SpecialFlags`='1' WHERE `entry`='11097';

-- The Deadliest Trap Ever Laid (http://wh.300murlocs.com/?quest=11101)
-- UPDATE `quest_template` SET `Method`='2' WHERE `entry`='11101';
UPDATE `quest_template` SET `Method`='0' WHERE `entry`='11101';

-- To Legion Hold (http://wh.300murlocs.com/?quest=10596)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId3`='21502', `ReqCreatureOrGOCount3`='1' WHERE `entry`='10596';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId3`='0', `ReqCreatureOrGOCount3`='0' WHERE `entry`='10596';

-- 05.05.2012

-- That's Abominable! (http://wh.300murlocs.com/?quest=13264 & http://wh.300murlocs.com/?quest=13288)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='31743', `ReqCreatureOrGOId2`='32168', `ReqCreatureOrGOId3`='32167', `ReqCreatureOrGOCount1`='15', `ReqCreatureOrGOCount2`='15', `ReqCreatureOrGOCount3`='15' WHERE `entry` IN (13264, 13288);
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOId3`='0', `ReqCreatureOrGOCount1`='0', `ReqCreatureOrGOCount2`='0', `ReqCreatureOrGOCount3`='0' WHERE `entry` IN (13264, 13288);

-- 26.05.2012

-- King of the Mountain (http://wh.300murlocs.com/?quest=13280)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='31766', `ReqCreatureOrGOCount1`='1' WHERE `entry`='13280';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13280';

-- 24.06.2012

-- That's Abominable! (daily) (http://wh.300murlocs.com/?quest=13276 & http://wh.300murlocs.com/?quest=13289)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='31743', `ReqCreatureOrGOId2`='32168', `ReqCreatureOrGOId3`='32167', `ReqCreatureOrGOCount1`='15', `ReqCreatureOrGOCount2`='15', `ReqCreatureOrGOCount3`='15' WHERE `entry` IN (13276, 13289);
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOId3`='0', `ReqCreatureOrGOCount1`='0', `ReqCreatureOrGOCount2`='0', `ReqCreatureOrGOCount3`='0' WHERE `entry` IN (13276, 13289);

-- 16.09.2012

-- It Rolls Downhill (http://wh.300murlocs.com/?quest=12673)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='28740', `ReqCreatureOrGOCount1`='7' WHERE `entry`='12673';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='12673';

-- Song of Wind and Water (http://wh.300murlocs.com/?quest=12726)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='29008', `ReqCreatureOrGOCount1`='3', `ReqCreatureOrGOId2`='29009', `ReqCreatureOrGOCount2`='3', `ReqSourceId3`='39571', `ReqSourceCount3`='1' WHERE `entry` IN (12726);
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0', `ReqSourceId3`='0', `ReqSourceCount3`='0' WHERE `entry` IN (12726);

-- Mission: Plague This! (http://wh.300murlocs.com/?quest=11332)
-- UPDATE `quest_template` SET `Method`='2', `ReqItemId1`='38083', `ReqItemCount1`='1' WHERE `entry`='11332';
UPDATE `quest_template` SET `Method`='2', `ReqItemId1`='0', `ReqItemCount1`='0' WHERE `entry`='11332';

-- Iron Rune Constructs and You: Collecting Data (http://wh.300murlocs.com/?quest=11489)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='24820', `ReqCreatureOrGOCount1`='1' WHERE `entry`='11489';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='11489';

-- Valkyrion Must Burn (http://wh.300murlocs.com/?quest=12953)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='30096', `ReqCreatureOrGOCount1`='6' WHERE `entry`='12953';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='12953';

-- Sniffing Out the Perpetrator (http://wh.300murlocs.com/?quest=12855)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='29677', `ReqCreatureOrGOCount1`='1', `ReqCreatureOrGOId3`='29695', `ReqCreatureOrGOCount3`='1', `ReqSourceId1`='40971', `ReqSourceCount1`='1', `ReqSourceId3`='41430', `ReqSourceCount3`='1' WHERE `entry` IN (12855);
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0', `ReqCreatureOrGOId3`='0', `ReqCreatureOrGOCount3`='0', `ReqSourceId1`='0', `ReqSourceCount1`='0', `ReqSourceId3`='0', `ReqSourceCount3`='0' WHERE `entry` IN (12855);

-- Fire At Will! (http://wh.300murlocs.com/?quest=10911)
-- UPDATE `quest_template` SET `Method`='2', `ReqItemId3`='38083', `ReqItemCount3`='1', `ReqCreatureOrGOId1`='22504', `ReqCreatureOrGOId2`='22503', `ReqCreatureOrGOCount1`='1', `ReqCreatureOrGOCount2`='1' WHERE `entry`='10911';
UPDATE `quest_template` SET `Method`='2', `ReqItemId3`='0', `ReqItemCount3`='0', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOCount1`='0', `ReqCreatureOrGOCount2`='0' WHERE `entry`='10911';

-- An Undead's Best Friend (http://wh.300murlocs.com/?quest=13169)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='31119', `ReqCreatureOrGOCount1`='18' WHERE `entry`='13169';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='13169';

-- Egg Freezing (http://wh.300murlocs.com/?quest=4734)
-- UPDATE `quest_template` SET `Method`='2', `ReqItemId1`='12286', `ReqItemCount1`='1' WHERE `entry`='4734';
UPDATE `quest_template` SET `Method`='2', `ReqItemId1`='0', `ReqItemCount1`='0' WHERE `entry`='4734';

-- Across Transborea (http://wh.300murlocs.com/?quest=11930)
-- UPDATE `quest_template` SET `Method`='2' WHERE `entry`='11930';
UPDATE `quest_template` SET `Method`='0' WHERE `entry`='11930';

-- Coward Delivery... Under 30 Minutes or it's Free (http://wh.300murlocs.com/?quest=11711)
-- UPDATE `quest_template` SET `Method`='2' WHERE `entry`='11711';
UPDATE `quest_template` SET `Method`='0' WHERE `entry`='11711';

-- Frogs Away! (http://wh.300murlocs.com/?quest=25446)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='40218', `ReqCreatureOrGOCount1`='12' WHERE `entry`='25446';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='25446';

-- Da Perfect Spies (http://wh.300murlocs.com/?quest=25444)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId2`='40188', `ReqCreatureOrGOCount2`='5' WHERE `entry`='25444';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOCount2`='0' WHERE `entry`='25444';

-- The Brothers Bronzebeard (http://wh.300murlocs.com/?quest=13417)
-- UPDATE `quest_template` SET `Method`='2' WHERE `entry`='13417';
UPDATE `quest_template` SET `Method`='0' WHERE `entry`='13417';

-- Escape from Silverbrook (http://wh.300murlocs.com/?quest=12308)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='-188706', `ReqCreatureOrGOCount1`='1' WHERE `entry`='12308';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId1`='0', `ReqCreatureOrGOCount1`='0' WHERE `entry`='12308';

-- What Illidan Wants, Illidan Gets... (http://wh.300murlocs.com/?quest=10577)
-- UPDATE `quest_template` SET `Method`='2' WHERE `entry`='10577';
UPDATE `quest_template` SET `Method`='0' WHERE `entry`='10577';

-- 18.09.2012

-- A Visit to the Doctor (http://wh.300murlocs.com/?quest=13152)
-- UPDATE `quest_template` SET `ReqCreatureOrGOId2`='30993' WHERE `entry`='13152';
UPDATE `quest_template` SET `ReqCreatureOrGOId2`='30992' WHERE `entry`='13152';

-- The Admiral Revealed (http://wh.300murlocs.com/?quest=13152)
-- UPDATE `quest_template` SET `ReqCreatureOrGOId1`='29627' WHERE `entry`='12852';
UPDATE `quest_template` SET `ReqCreatureOrGOId1`='29621' WHERE `entry`='12852';

-- 23.09.2012

-- Forging an Alliance (http://wh.300murlocs.com/?quest=12924)
-- UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId2`='30126', `ReqCreatureOrGOCount2`='1' WHERE `entry`='12924';
UPDATE `quest_template` SET `Method`='2', `ReqCreatureOrGOId2`='0', `ReqCreatureOrGOCount2`='0' WHERE `entry`='12924';
