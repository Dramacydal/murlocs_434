DELETE FROM `npc_text` WHERE `ID` = 888003;
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(888003, 'Donate a small amount of gold and I will send you wherever you want!');

DELETE FROM `locales_npc_text` WHERE `entry` = 888003;
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc8`) VALUES
(888003, 'Donate a small amount of gold and I will send you wherever you want!', 'Всего за небольшую сумму я отправлю тебя туда, куда захочешь!');

DELETE FROM `creature_template` WHERE `entry` = 88003;
INSERT INTO `creature_template` VALUES 
(88003,0,0,0,0,0,25609,0,0,0,'Мастер порталов','',NULL,0,80,80,5100,5100,5751,5751,3000,35,35,3,1.25,1,1.14286,1,70,105,0,26,1,1500,1500,8,0,0,0,0,0,0,0,56,84,21,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1.2,1,0,0,0,0,0,0,0,0,1,0,0,2,'npc_teleport');