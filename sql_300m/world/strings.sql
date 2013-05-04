-- NPC_TELEPORT
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006000 AND 2000006017;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006000, 'Stormwind', 'Штормград'),
(2000006001, 'Ironforge', 'Стальгорн'),
(2000006002, 'Darnassus', 'Дарнас'),
(2000006003, 'The Exodar', 'Эксодар'),

(2000006004, 'Orgrimmar', 'Оргриммар'),
(2000006005, 'Thunder Bluff', 'Громовой Утес'),
(2000006006, 'Undercity', 'Подгород'),
(2000006007, 'Silvermoon City', 'Луносвет'),

(2000006008, 'Dalaran', 'Даларан'),
(2000006009, 'Shattrath City', 'Шаттрат'),
(2000006010, 'Booty Bay', 'Пиратская Бухта'),
(2000006011, 'Gadgetzan', 'Прибамбасск'),
(2000006012, 'Ratchet', 'Кабестан'),
(2000006013, 'Valiance Keep', 'Крепость Отваги'),
(2000006014, 'Warsong Hold', 'Крепость Песни Войны'),
(2000006015, 'Vengeance Landing', 'Лагерь Возмездия'),
(2000006016, 'Valgarde', 'Валгард'),

(2000006017, 'Are you sure you want to donate?', 'Вы уверены, что готовы заплатить?');

-- Pilgrim's Bounty
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006100 AND 2000006103;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006100, 'Pilgrim''s Paunch', 'Присоединиться к застолью'),
(2000006101, 'Throw Food', 'Метнуть явство'),
(2000006102, 'Share Food', 'Передать сотрапезникам еду'),
(2000006103, 'Sit at the table', 'Сесть за стол');

-- NPC_EVENT_2
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006200 AND 2000006205;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006200, 'A festival?', 'Праздник?'),
(2000006201, 'I''m a murloc too and I want to celebrate!', 'Я тоже мурлок и хочу себе праздник!'),
(2000006202, 'I am ready!', 'Я готов!'),
(2000006203, 'I am ready!', 'Я готова!'),
(2000006204, 'Are you sure you want to make a donate?', 'Вы действительно хотите сделать взнос?'),
(2000006205, 'Welcome to the festival!', 'Добро пожаловать на праздник!');

-- NPC_EVENT
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006300 AND 2000006301;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006300, 'Take me to the event!', 'Отправиться на ивент.'),
(2000006301, 'Sorry, can''t stay. I should leave.', 'Простите, у меня неотложные дела. Мне нужно уйти.');

-- NPC_EVENT_1
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006400 AND 2000006401;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006400, 'Yes, go!', 'Давай на полосу, только быстрее, тороплюсь я!'),
(2000006401, 'Disappear pudge, i will do this without you!', 'Исчезни, коротышка, без тебя справлюсь!');

-- Strand of the Ancients
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006500 AND 2000006514;

-- Titanium Seal of Dalaran
DELETE FROM `mangos_string` WHERE `entry` BETWEEN 20100 AND 20102;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(20100, 'casually flips his [Titanium Seal of Dalaran]', 'небрежно подкидывает в воздух свою [Титановая печать Даларана].'),
(20101, 'catches the coin heads up!', 'обнаруживает, что монета упала орлом!'),
(20102, 'finds the coin face down for tails!', 'обнаруживает, что монета упала решкой!');

-- Orgrimmar Guard
UPDATE `db_script_string` SET `content_loc8` = 'Когда же прийдет моя смена... у меня жена и дети...' WHERE `entry` = 2000000138;

-- npc_experience
DELETE FROM `npc_text` WHERE `ID` = 890000;
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(890000, 'There are some who desire to stop gaining experience from their actions; to forever remain at the level of strength they are now, and never grow stronger.$B$BI can make this possible... for a fee, of course.$B$BBut understand this: Once you pay me, you will NO LONGER GAIN EXPERIENCE. None. Not one whit.$B$BShould you change your mind afterwards and want to start gaining experience again, I can undo the process for the same fee.');

DELETE FROM `locales_npc_text` WHERE `entry` = 890000;
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc8`) VALUES
(890000, 'There are some who desire to stop gaining experience from their actions; to forever remain at the level of strength they are now, and never grow stronger.$B$BI can make this possible... for a fee, of course.$B$BBut understand this: Once you pay me, you will NO LONGER GAIN EXPERIENCE. None. Not one whit.$B$BShould you change your mind afterwards and want to start gaining experience again, I can undo the process for the same fee.', 'Некоторые искатели приключений не желают получать опыт за свои подвиги, чтобы навсегда закрепиться на том уровне, на котором они находятся сегодня.$B$BЯ могу оказать тебе такую услугу... не за просто так, разумеется.$B$BОднако запомни - как только деньги окажутся у меня в руках, ты больше не будешь получать опыт. Никакой. Никогда.$B$BВпрочем, если ты передумаешь, я могу вернуть все назад - за определенную плату.');

DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006600 AND 2000006610;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006600, 'I no longer wish to gain experience.', 'Я больше не хочу получать опыт.'),
(2000006601, 'I wish to start gaining experience again.', 'Я хочу снова начать получать опыт.'),
(2000006602, 'Are you sure?', 'Вы уверены?'),

(2000006603, 'Slahtz daydreams of Lucky Fishing Hats.', 'Слотц мечтает о Счастливой шляпе рыболова.'),
(2000006604, 'Slahtz bemoans the loss of his Nethercleft Leg Armor.', 'Слотц оплакивает потерю своих накладок для поножей из кожи копытня.'),
(2000006605, 'Slahtz realizes he needs some Leggings of the Fang.', 'Слотц понимает, что ему нужны Поножи Клыка.'),
(2000006606, 'Slahtz plans a Stranglethorn Vale fishing trip.', 'Слотц планирует отправиться на рыбалку по Тернистой долине.'),

(2000006607, 'Behsten daydreams of Lucky Fishing Hats.', 'Бестин мечтает о Счастливой шляпе рыболова.'),
(2000006608, 'Behsten bemoans the loss of his Nethercleft Leg Armor.', 'Бестин оплакивает потерю своих накладок для поножей из кожи копытня.'),
(2000006609, 'Behsten realizes he needs some Leggings of the Fang.', 'Бестин понимает, что ему нужны Поножи Клыка.'),
(2000006610, 'Behsten plans a Stranglethorn Vale fishing trip.', 'Бестин планирует отправиться на рыбалку по Тернистой долине.');

DELETE FROM `mangos_string` WHERE `entry` = 11003;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11003, 'You must return to ''%s'' in %u seconds, or you will be dismissed from your vehicle.', 'Вы должны вернуться в ''%s'' в течение %u секунд, иначе ваше средство передвижения исчезнет.');

-- characters addon
DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11004 AND 11006;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11004, 'Character %s(%u) was set addon with ModelId: %u Phasemask: %u Scale: %f Faction: %u.', 'Персонажу %s(%u) была назначена надстройка, Модель: %u Маска фаз: %u Размер: %f Фракция: %u.'),
(11005, 'Character %s(%u) addon was removed. Character must relog.', 'С персонажа %s(%u) была снята надстройка. Персонажу необходимо сделать лог.'),
(11006, 'Character addon feature is disabled on this realm.', 'На данном рилме отключена возможность создания надстроек персонажей.');

-- fix arathi basin texts
UPDATE `mangos_string` SET `content_loc8` = 'стойла' WHERE `entry` = 652;
UPDATE `mangos_string` SET `content_loc8` = 'кузницу' WHERE `entry` = 653;
UPDATE `mangos_string` SET `content_loc8` = 'ферму' WHERE `entry` = 654;
UPDATE `mangos_string` SET `content_loc8` = 'лесопилку' WHERE `entry` = 655;
UPDATE `mangos_string` SET `content_loc8` = 'золотой рудник' WHERE `entry` = 656;
UPDATE `mangos_string` SET `content_loc8` = '%s захватывает %s' WHERE `entry` = 657;
UPDATE `mangos_string` SET `content_loc8` = '$n защитил(а) %s' WHERE `entry` = 658;
UPDATE `mangos_string` SET `content_loc8` = '$n нападает на %s' WHERE `entry` = 659;
UPDATE `mangos_string` SET `content_loc8` = '$n атакует %s! Если не помешать, %s захватит эту точку через 1 минуту!' WHERE `entry` = 660;

-- autobroadcast
DELETE FROM `mangos_string` WHERE `entry` = '11000';
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11000, '|cffffcc00[Autobroadcast]: |cff00ff00%s|r', '|cffffcc00[Авторассылка]: |cff00ff00%s|r');

-- .character checkdkquests
DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11007 AND 11010;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11007, 'Selected player\'s %s class is not Death Knight!', 'Класс персонажа %s не Рыцарь Смерти!'),
(11008, 'Quests from Death Knight questline, that were not completed by %s:', 'Квесты цепочки Рыцарей Смерти, который не выполнил %s:'),
(11009, '%d - |cffffffff|Hquest:%d:%d|h[%s]|h|r %s', '%d - |cffffffff|Hquest:%d:%d|h[%s]|h|r %s'),
(11010, '%s has already completed all the quests!', 'Вы квесты персонажем %s были выполнены!');

-- pinfo improvements
UPDATE `mangos_string` SET `content_default` = 'Player%s %s (guid: %u) Account: %s (id: %u) GMLevel: %u Race: %s Class: %s Gender: %s Last IP: %s Last login: %s Latency: %ums',
`content_loc8` = 'Игрок%s %s (GUID: %u) Аккаунт: %s (id: %u) Уровень доступа: %u Раса: %s Класс: %s Пол: %s Последний IP: %s Последний вход: %s Задержка: %ums'
WHERE `entry` = 548;

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11017 AND 11038;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11017, 'Human', 'Человек'),
(11018, 'Orc', 'Орк'),
(11019, 'Dwarf', 'Дворф'),
(11020, 'Night Elf', 'Ночной Эльф'),
(11021, 'Undead', 'Нежить'),
(11022, 'Tauren', 'Таурен'),
(11023, 'Gnome', 'Гном'),
(11024, 'Troll', 'Тролль'),
(11025, 'Blood Elf', 'Эльф Крови'),
(11026, 'Draenei', 'Дреней'),

(11027, 'Warrior', 'Воин'),
(11028, 'Paladin', 'Паладин'),
(11029, 'Hunter', 'Охотник'),
(11030, 'Rogue', 'Разбойник'),
(11031, 'Priest', 'Жрец'),
(11032, 'Death Knight', 'Рыцарь Смерти'),
(11033, 'Shaman', 'Шаман'),
(11034, 'Mage', 'Маг'),
(11035, 'Warlock', 'Колдун'),
(11036, 'Druid', 'Друид'),

(11037, 'Male', 'Мужской'),
(11038, 'Female', 'Женский');

-- ginfo command
DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11039 AND 11042;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11039, '%s is in group of %u members, leader: %s, group guid: %u', '%s состоит в группе из %u игроков, лидер: %s, group guid: %u'),
(11040, '%u) %s %s (%u) %s%s', '%u) %s %s (%u) %s%s'),
(11041, 'Leader ', 'Лидер '),
(11042, 'Is Taxi Flying', 'Находится на такси');

-- cheat encounter notification
DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11011 AND 11012;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11011, '|cffffcc00[Сообщение античита]:|cff00ff00|r |c0000FF00Внимание! Игрок |r|cffffffff|Hplayer:%s|h[%s]|h|r|c0000FF00 бьет босса %s, но условия доступности этого босса не выполнены!|r', '|cffffcc00[Сообщение античита]:|cff00ff00|r |c0000FF00Внимание! Игрок |r|cffffffff|Hplayer:%s|h[%s]|h|r|c0000FF00 бьет босса %s, но условия доступности этого босса не выполнены!|r'),
(11012, '|cffffcc00[Сообщение античита]:|cff00ff00|r |c0000FF00Внимание! Игроки в подземелье бьют босса %s, но условия доступности этого босса не выполнены! Игрок для телепорта, находящийся в этом подеземелье: |r|cffffffff|Hplayer:%s|h[%s]|h|r|c0000FF00 %s|r', '|cffffcc00[Сообщение античита]:|cff00ff00|r |c0000FF00Внимание! Игроки в подземелье бьют босса %s, но условия доступности этого босса не выполнены! Игрок для телепорта, находящийся в этом подеземелье: |r|cffffffff|Hplayer:%s|h[%s]|h|r|c0000FF00 %s|r');

-- Anticheat message
DELETE FROM `mangos_string` WHERE `entry` = 11013;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11013, '|cffffcc00[Anticheat message]:|cff00ff00|r |c0000FF00Warning! Maybe found a cheater, nickname: |r|cffffffff|Hplayer:%s|h[%s]|h|r|c0000FF00, reason: %s.|r', '|cffffcc00[Сообщение античита]:|cff00ff00|r |c0000FF00Внимание! Возможно обнаружен читер, ник: |r|cffffffff|Hplayer:%s|h[%s]|h|r|c0000FF00, причина: %s.|r');

-- Anticrash guard message
DELETE FROM `mangos_string` WHERE `entry` = 11014;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11014, '|cffff0000[Anticrash guard]:|cff00ff00|r |cffff0000ATTENTION! Violation in server operation is revealed. To all players the request to make logout for saving of the data. In 2 minutes the server will be forcedly rebooted.|r', '|cffff0000[Anticrash guard]:|cff00ff00|r |cffff0000ВНИМАНИЕ! Обнаружено нарушение в работе сервера. Всем игрокам просьба сделать логаут для сохранения данных. Через 2 минуты сервер будет принудительно перезагружен.|r');

-- Pirate's Day
DELETE FROM `npc_text` WHERE `ID` = 890001;
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(890001, 'Ye\'ve got some stones comin\' up here, $g lad : lass;. It\'s Pirate\'s Day, ye know... and \'til the sun rises on the morrow, Booty Bay is MINE. \nSo! What do ye say? Raise a drink ta me and me crew, and join the celebration?');

-- DELETE FROM `locales_npc_text` WHERE `entry` = 890001;
-- INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc8`) VALUES
-- (890001, 'Ye\'ve got some stones comin\' up here, $g lad : lass;. It\'s Pirate\'s Day, ye know... and \'til the sun rises on the morrow, Booty Bay is MINE. \nSo! What do ye say? Raise a drink ta me and me crew, and join the celebration?');

DELETE FROM `db_script_string` WHERE `entry` = 2000006611;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006611, 'Here\'s to you, Dread Captain! Make me a pirate for the day.', 'Это тебе, Жуткий Капитан! Я хочу быть пиратом в этот день.');

-- 
UPDATE `mangos_string` SET `content_loc8` = 'Аккаунт %s никогда не был в бане' WHERE `entry` = 416;
UPDATE `mangos_string` SET `content_loc8` = 'История банов аккаунта %s:' WHERE `entry` = 417;
UPDATE `mangos_string` SET `content_loc8` = 'Навечно' WHERE `entry` = 419;

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11001 AND 11002;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11001, 'Account %s has never been banned', 'Персонаж %s никогда не был в бане'),
(11002, 'Ban history for character %s:', 'История банов персонажа %s:');

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11043 AND 11046;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11043, 'The following characters match your query:', 'Следующие персонажи соответствуют Вашему запросу:'),
(11044, 'Currently Banned Characters:', 'Текущие забаненные персонажи:'),
(11045, '|    Nickname   |   BanDate    |   UnbanDate  |  Banned By    |   Ban Reason  |', '|     Имя       |   Дата бана    |   Дата разбана  |  Кем забанен    |   Причина  |'),
(11046, 'There is no banned character matching this name part.', 'Нет забаненных персонажей, содержащих такую часть имени.');

DELETE FROM `command` WHERE `name` IN ('ban char', 'unban char', 'banlist char', 'baninfo char',
'ban characc', 'unban characc', 'banlist characc', 'baninfo characc',
'ban character', 'unban character', 'banlist character', 'baninfo character');
INSERT INTO `command` VALUE
('ban char', 2, 'Syntax: .ban char $Name $bantime $reason\r\nBan character and kick player. Only this characters will be banned.\r\n$bantime: negative value leads to permban, otherwise use a timestring like \"4d20h3s\".'),
('baninfo char', 2, 'Syntax: .baninfo char $charactername \r\nWatch full information about a specific ban.'),
('banlist char', 2, 'Syntax: .banlist char $Name\r\nSearches the banlist for a character name pattern. Pattern required.'),
('unban char', 3, 'Syntax: .unban char $Name\r\nUnban character.'),

('ban characc', 2, 'Syntax: .ban characc $Name $bantime $reason\r\nBan account and kick player.\r\n$bantime: negative value leads to permban, otherwise use a timestring like \"4d20h3s\".'),
('baninfo characc', 2, 'Syntax: .baninfo characc $charactername \r\nWatch full information about a specific ban.'),
('banlist characc', 2, 'Syntax: .banlist characc $Name\r\nSearches the banlist for account by a character name pattern. Pattern required.'),
('unban characc', 3, 'Syntax: .unban characc $Name\r\nUnban accounts for character name pattern.');

-- character setaccount
DELETE FROM `command` WHERE `name` IN ('character setaccount');
INSERT INTO `command` VALUE
('character setaccount', 4, 'Syntax: .character setaccount $charactername $newaccountname \r\nTransfer character from current account to another');

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11047 AND 11050;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11047, 'Account \'%s\' for transfer not found.', 'Аккаунт \'%s\' для переноса не найден.'),
(11048, 'Characters %s is already on account %s.', 'Персонаж %s уже находится на аккаунте %s(%u).'),
(11049, 'Impossible to transfer character %s to account %s(%u). Too much characters on that account.', 'Невозможно перенести персонажа %s на аккаунт %s(%u). Слишком много персонажей на аккаунте.'),
(11050, 'Character %s successfully transfered from %s(%u) to %s(%u)', 'Персонаж %s успешно перенесен с %s(%u) на %s(%u)');

-- npc_wormhole
DELETE FROM `db_script_string` WHERE `entry` BETWEEN 2000006612 AND 2000006617;
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUES
(2000006612, 'Borean tundra', 'Борейская тундра'),
(2000006613, 'Howling Fjord', 'Ревущий фьорд'),
(2000006614, 'Sholazar Basin', 'Низина Шолазар'),
(2000006615, 'Icecrown', 'Ледяная Корона'),
(2000006616, 'The Storm Peaks', 'Грозовая Гряда'),
(2000006617, 'Under ground', 'Под землю');

DELETE FROM `gossip_menu` WHERE `entry` = 10668;
DELETE FROM `gossip_menu_option` WHERE `menu_id` = 10668;
DELETE FROM `locales_gossip_menu_option` WHERE `menu_id` = 10668;
DELETE FROM `gossip_scripts` WHERE `id` BETWEEN 50000 AND 50004;

-- icc ring restorer
DELETE FROM `db_script_string` WHERE `entry` IN (2000006618, 2000006619);
INSERT INTO `db_script_string` (`entry`, `content_default`, `content_loc8`) VALUE
(2000006618, 'I\'ve lost my ICC reputation ring.', 'Я потерял свое кольцо Пепельного союза.'),
(2000006619, 'I\'ve lost my ICC reputation ring.', 'Я потеряла свое кольцо Пепельного союза.');

-- race & faction change
DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11051 AND 11056;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11051, 'Forced race change for player %s will be requested at next login.', 'Смена расы персонажу %s будет произведена во время следующего захода им в мир.'),
(11052, 'Forced faction change for player %s (GUID #%u) will be requested at next login.', 'Смена расы персонажу %s (GUID #%u) будет произведена во время следующего захода им в мир.'),
(11053, 'Forced faction change for player %s will be requested at next login.', 'Смена фракции персонажу %s будет произведена во время следующего захода им в мир.'),
(11054, 'Forced faction change for player %s (GUID #%u) will be requested at next login.', 'Смена фракции персонажу %s (GUID #%u) будет произведена во время следующего захода им в мир.'),
(11055, 'Forced title conversion for player %s will be requested at next login.', 'Конвертация званий персонажу %s будет произведена во время следующего захода им в мир.'),
(11056, 'Forced title conversion for player %s (GUID #%u) will be requested at next login.', 'Конвертация званий персонажу %s (GUID #%u) будет произведена во время следующего захода им в мир.');

DELETE FROM `command` WHERE `name` IN ('npc setvehicleid');
INSERT INTO `command` VALUE
('npc setvehicleid', 4, 'Syntax: .npc setvehicleid $id \r\nSets vehicle id to creature. Target must be vehicle.');

DELETE FROM `command` WHERE `name` IN ('server cleangmplayedtime');
INSERT INTO `command` VALUE
('server cleangmplayedtime', 5, 'Syntax: .server cleangmplayedtime\r\nCleanups character database table `gm_played_time.`');

DELETE FROM `command` WHERE `name` IN ('character changerace', 'character changefaction');
INSERT INTO `command` VALUES
('character changerace', 3, 'Syntax: .character changerace #name'),
('character changefaction', 3, 'Syntax: .character changefaction #name');

DELETE FROM `command` WHERE `name` IN ('gm setview', 'gm unview');
INSERT INTO `command` VALUES
('gm setview', 3, 'Syntax: .gm setview [#name]\r\nSet camera to player with #name if specified. If not specified, sets camera to current selected player target. Target must be in same map.'),
('gm unview', 3, 'Syntax: .gm unview\r\nReset\'s GM\'s camera to normal state.');

DELETE FROM `mangos_string` WHERE `entry` IN (11057, 11058);
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11057, 'Your camera has been attached to %s.', 'Ваша камера прикреплена к персонажу %s.'),
(11058, 'Your camera has been reset to normal state.', 'Ваша камера возвращена в нормальное состояние.');

DELETE FROM `command` WHERE `name` = 'achievement criteria update';
INSERT INTO `command` VALUE
('achievement criteria update', 4, 'Syntax: .achievement criteria add $playername #criteriatype #value1 #value2 $useunit\r\nUpdate criterias of #type for player $playername (if specified) with values #value1 #value2 and targeted unit, if $useunit is specified');

DELETE FROM `mangos_string` WHERE `entry` = 11063;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11063, 'You will be kicked in %u seconds.', 'Вы будете кикнуты через %u секунд.');

DELETE FROM `mangos_string` WHERE `entry` IN (11064, 11065);
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11064, 'You have not enough honor points.', 'У Вас недостаточно очков чести.'),
(11065, 'You have not enough arena points.', 'У Вас недостаточно очков арены.');

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11066 AND 11077;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11066, 'You can''t do that while spectating.', 'Вы не можете делать это в режиме наблюдения.'),
(11067, 'You can''t spectate while dead.', 'Вы не можете наблюдать будуче мервты.'),
(11068, 'You can''t spectate from here.', 'Вы не можете наблюдать отсюда.'),
(11069, 'You are already spectating.', 'Вы уже находитесь в режиме наблюдения.'),
(11070, 'You are not spectating.', 'Вы не находитесь в режиме наблюдения.'),
(11071, 'You can''t spectate another spectator.', 'Вы не можете наблюдать за другим наблюдателем.'),
(11072, 'You have allowed to spectate on you.', 'Вы разрешили наблюдать за Вами.'),
(11073, 'You have forbid to spectate on you.', 'Вы запретили наблюдать за Вами.'),
(11074, 'Player %s has forbid to spectate on him.', 'Игрок %s запретил наблюдать за ним.'),
(11075, 'Spectating is disabled on this server.', 'Наблюдение за игроками отключено на данном сервере.'),
(11076, '%s is not in arena.', '%s не на арене.'),
(11077, 'You can\'t spectate while in battleground or LFG queue.', 'Вы не можете наблюдать, находять в очереди на поле боя или поиске подземелий.');

DELETE FROM `command` WHERE `name` IN ('allowspectate', 'spectate', 'unspectate');
INSERT INTO `command` VALUES
('allowspectate', 0, 'Syntax: .allowspectate\r\nAllow other player to spectate on you.'),
('spectate', 0, 'Syntax: .spectate #playername\r\nSpectate after player #playername'),
('unspectate', 0, 'Syntax: .unspectate\r\nStop spectating and exit arena.');

DELETE FROM `command` WHERE `name` IN ('bg');
INSERT INTO `command` VALUE
('bg', 0, 'Syntax: .bg $num\r\nSet prefered random BG for player.\r\n0 - random, 1 - Warsong Gulch, 2 - Arathi Basin, 3 - Eye of the Storm, 4 - Alterac Valley, 5 - Strand of the Ancients');

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11122 AND 11129;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11122, 'You have selected wrong BG index!', 'Вы ввели неправильный номер поля боя!'),
(11123, 'You have selected Random BG as prefered BG.', 'Вы выбрали Случайное поле боя в качестве предпочитаемого поля боя.'),
(11124, 'You have selected Warsong Gulch as prefered BG.', 'Вы выбрали Ущелье Песни Войны в качестве предпочитаемого поля боя.'),
(11125, 'You have selected Arathi Basin as prefered BG.', 'Вы выбрали Низину Арати боя в качестве предпочитаемого поля боя.'),
(11126, 'You have selected Eye of the Storm as prefered BG.', 'Вы выбрали Око Бури в качестве предпочитаемого поля боя.'),
(11127, 'You have selected Alterac Valley as prefered BG.', 'Вы выбрали Альтеракскую Долину в качестве предпочитаемого поля боя.'),
(11128, 'You have selected Strand of the Ancients as prefered BG.', 'Вы выбрали Берег Древних в качестве предпочитаемого поля боя.'),
(11129, 'You have selected Isle of Conquest as prefered BG.', 'Вы выбрали Остров Завоеваний в качестве предпочитаемого поля боя.');

DELETE FROM `mangos_string` WHERE `entry` IN (11130, 11131);
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11130, 'You are not on vehicle!', 'Вы не находитесь на транспортном средстве!'),
(11131, 'You have successfully dismounted vehicle.', 'Вы успешно спешились с транспортного средства.');

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 11132 AND 11135;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(11132, 'You have entered wrong arena bracket.', 'Вы ввели неверный брекет арены.'),
(11133, 'You must leave %uv%u arena team before you can reset MMR for that bracket.', 'Вы должны покинуть команду арены %uv%u, перед тем как сбросить MMR для данного брекета.'),
(11134, 'Your MMR for %uv%u bracket has been reset.', 'Ваш MMR брекета %uv%u был сброшен.'),
(11135, 'Your %uv%u MMR: %u', 'Ваш %uv%u MMR: %u');

DELETE FROM `mangos_string` WHERE `entry` = 11142;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUE
(11142, 'You should play for at least %s before you can use that function', 'Вы должны отыграть как минимум %s прежде чем сможете воспользоваться данной функцией');

DELETE FROM `mangos_string` WHERE `entry` = 11143;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUE
(11143, 'You can''t do that while arena team is fighting.', 'Вы не можете сделать это, пока команда арены участвует в бою.');

DELETE FROM `mangos_string` WHERE `entry` = 11144;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUE
(11144, 'Active events:', 'Активные праздники:');