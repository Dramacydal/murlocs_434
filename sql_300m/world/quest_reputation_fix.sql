-- Exodar rep fix (not all)
UPDATE `quest_template` SET `RewRepFaction1` = '930', `RewRepValueId1` = '5' WHERE `entry` IN ('9568', '9574', '9703', '9557', '9700', '9748', '9649', '10065', '10067', '10066', '9759', '9746', '9741', '9740');
UPDATE `quest_template` SET `RewRepFaction2` = '0', `RewRepValueId2` = '0', `RewRepFaction3` = '0', `RewRepValueId3` = '0', `RewRepFaction4` = '0', `RewRepValueId4` = '0',`RewRepFaction5` = '0', `RewRepValueId5` = '0' 
WHERE `entry` IN ('9567', '9643', '9629', '9574', '9641', '9648', '9703', '9698', '9699', '9549', '9548', '9550', '9557', '9700', '9748', '9649', '9620', '9561', '9628', '10065', '10067', '10066', '9759', '9746', '9741', '9706', '9740');
UPDATE `quest_template` SET `PrevQuestId` = '9700' WHERE entry = '9703';
UPDATE `quest_template` SET `RewRepValueId1` = '4' WHERE `entry` IN ('9741', '9706');
UPDATE `quest_template` SET `RewRepFaction1` = '69', `RewRepValueId1` = '5' WHERE `entry` = '9561';

-- Ravenholdt quest fix
UPDATE `quest_template` SET `ZoneOrSort` = '0' WHERE entry = '8249';