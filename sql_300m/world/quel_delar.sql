
-- from Lorentis
UPDATE `quest_template` SET `ReqSourceCount2` = 0, `ReqCreatureOrGOId2` = 0 WHERE `entry` IN (24564, 24598);

-- The Purification of Quel'Delar
DELETE FROM `creature_questrelation` WHERE `id` = 37527 AND `quest` IN (24564, 24594, 24598, 24553, 24595);
INSERT INTO `creature_questrelation` (`id`, `quest`) VALUES (37527, 24564), (37527,24553);

UPDATE `quest_template` SET `RequiredRaces` = 690 WHERE `entry` = 24564;
UPDATE `quest_template` SET `RequiredSkill` = 0 WHERE `entry` IN (24564, 24553);

UPDATE `quest_template` SET `NextQuestInChain` = 0 WHERE `entry` = 24553;
UPDATE `quest_template` SET `PrevQuestId` = 24553 WHERE `entry` IN (24795, 24796);

-- HORDE
-- An Audience With The Arcanist
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` = 24451;
-- Reforging The Sword
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` = 24559;
-- Tempering The Blade
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` = 24560;
-- The Halls Of Reflection
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemId2` = 0, `ReqItemCount1` = 0, `ReqItemCount2` = 0, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0 WHERE `entry` = 24561;
-- Thalorien Dawnseeker
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemId2` = 0, `ReqItemCount1` = 0, `ReqItemCount2` = 0, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0 WHERE `entry` = 24563;
-- The Purification Of Quel'Delar
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` in (24596, 24598, 24594, 24564);

-- Alliance
-- A Meeting With The Magister
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` = 20439;
-- Reforging The Sword
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` = 24461;
-- Tempering The Blade
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` = 24476;
-- The Halls Of Reflection
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemId2` = 0, `ReqItemCount1` = 0, `ReqItemCount2` = 0, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0 WHERE `entry` = 24480;
-- Thalorien Dawnseeker
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemId2` = 0, `ReqItemCount1` = 0, `ReqItemCount2` = 0, `ReqCreatureOrGOId1` = 0, `ReqCreatureOrGOCount1` = 0 WHERE `entry` = 24535;
-- The Purification Of Quel'Delar
UPDATE `quest_template` SET `Method` = 2, `ReqItemId1` = 0, `ReqItemCount1` = 0 WHERE `entry` in (24595, 24553);
