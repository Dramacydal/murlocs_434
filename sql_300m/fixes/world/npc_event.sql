
DELETE FROM `npc_text` WHERE `ID` IN (888000);
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(888000, 'Greetings $N! Are you ready to play our event?');

DELETE FROM `locales_npc_text` WHERE `entry` IN (888000);
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc2`, `Text0_0_loc3`, `Text0_0_loc4`, `Text0_0_loc5`, `Text0_0_loc6`, `Text0_0_loc7`, `Text0_0_loc8`) VALUES
(888000, 'Greetings $N! Are you ready to play our event?', NULL, NULL, NULL, NULL, NULL, NULL, 'Приветствую, $N! Готов ли ты отправиться исследовать будущее со мной? Я могу взять тебя на великое состязание. \n \n Веди себя хорошо и не хулигань, с правилами поведения можешь ознакомиться на форуме проекта 300murlocs.com, раздел World of Warcraft -> Игровые события. После эвента не забудь завершить сессию игры. Если случится дисконнект, обратись ко мне ещё раз. Всего доброго.');

DELETE FROM `creature_template` WHERE `entry` IN(88000);
INSERT INTO `creature_template` VALUES 
(88000,0,0,0,0,0,15639,0,0,0,'Эру Илуватар','The Great Maker',NULL,0,99,99,5100,5100,5751,5751,3000,35,35,3,1.25,1,1.14286,1,70,105,0,26,1,1500,1500,8,0,0,0,0,0,0,0,56,84,21,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1.2,1,0,0,0,0,0,0,0,0,1,0,0,2,'npc_event');



DELETE FROM `npc_text` WHERE `ID` IN (889000);
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(889000, 'Ascension on Stalgorn? Hah, this hard business, friend. The parachute that though is?
From here you can not continue. You can continue from a runway. The way there will be longer, but is easier. Help you with this business?');

DELETE FROM `locales_npc_text` WHERE `entry` IN (889000);
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc2`, `Text0_0_loc3`, `Text0_0_loc4`, `Text0_0_loc5`, `Text0_0_loc6`, `Text0_0_loc7`, `Text0_0_loc8`) VALUES
(889000, 'Ascension on Stalgorn? Hah, this hard business, friend. The parachute that though is?
From here you can not continue. You can continue from a runway. The way there will be longer, but is easier. Help you with this business?', NULL, NULL, NULL, NULL, NULL, NULL, 'Восхождение на Стальгорн? Хах, нелёгкое это дело, приятель. Парашют то хоть есть?
Отсюда не получится продолжить путь, уклон слишком крутой. Гораздо проще это сделать со взлётной полосы. Путь там будет длиннее, но проще. Помочь тебе в этом деле?');

DELETE FROM `creature_template` WHERE `entry` IN(89000);
INSERT INTO `creature_template` VALUES 
(89000,0,0,0,0,0,26353,0,0,0,'Бранн','',NULL,0,99,99,5100,5100,5751,5751,3000,35,35,3,1.25,1,1.14286,1,70,105,0,26,1,1500,1500,8,0,0,0,0,0,0,0,56,84,21,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1.2,1,0,0,0,0,0,0,0,0,1,0,0,2,'npc_event_1');




DELETE FROM `npc_text` WHERE `ID` IN (888001, 888002);
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(888001, 'A festival comes to us!'),
(888002, 'Want to join our festival? Easily! Donate 5 gold and the murlocs will be eager to invite you to their family!');

DELETE FROM `locales_npc_text` WHERE `entry` IN (888001, 888002);
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc8`) VALUES
(888001, 'A festival comes to us!', 'Праздник к нам приходит!'),
(888002, 'Want to join our festival? Easily! Donate 5 gold and the murlocs will be eager to invite you to their family!', 'Хочешь праздник? Без проблем! Уплати взнос в пять золотых монет и мурлоки с радостью примут тебя в свою семью!');

DELETE FROM `creature_template` WHERE `entry` IN(88001,88002);
INSERT INTO `creature_template` VALUES 
(88001,0,0,0,0,0,22984,0,0,0,'Мурчаль','',NULL,0,99,99,5100,5100,5751,5751,3000,35,35,3,1.25,1,1.14286,1,70,105,0,26,1,1500,1500,8,0,0,0,0,0,0,0,56,84,21,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1.2,1,0,0,0,0,0,0,0,0,1,0,0,2,'npc_event_2'),
(88002,0,0,0,0,0,22984,0,0,0,'Мурчаль','',NULL,0,99,99,5100,5100,5751,5751,3000,35,35,3,1.25,1,1.14286,1,70,105,0,26,1,1500,1500,8,0,0,0,0,0,0,0,56,84,21,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,'',0,3,1.2,1,0,0,0,0,0,0,0,0,1,0,0,2,'npc_event_2');