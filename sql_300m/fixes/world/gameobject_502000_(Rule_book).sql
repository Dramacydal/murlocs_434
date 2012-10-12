/*

Заполнение БД для GO - Книга правил
id книги - 502000

SQL-скрипт сначала УДАЛЯЕТ GO с id 502000 из мира, далее удаляет его структуру, создает его структуру, 
чистит данные локализации и снова их создает, также чистится и заполняется таблица непосредственно под сам текст

*/

SET NAMES 'utf8';

DELETE FROM gameobject WHERE id = 502000;

DELETE FROM gameobject_template WHERE entry = 502000;

INSERT INTO gameobject_template
   (`entry`, `type`, `displayId`, `name`, `IconName`, `castBarCaption`, `unk1`, `faction`, `flags`, `size`, `questItem1`, `questItem2`, `questItem3`, `questItem4`, `questItem5`, `questItem6`, `data0`, `data1`, `data2`, `data3`, `data4`, `data5`, `data6`, `data7`, `data8`, `data9`, `data10`, `data11`, `data12`, `data13`, `data14`, `data15`, `data16`, `data17`, `data18`, `data19`, `data20`, `data21`, `data22`, `data23`, `mingold`, `maxgold`, `ScriptName`)
VALUES
   (502000, 9, 8520, 'Rule book', '', '', '', 0, 0, 1.5, 0, 0, 0, 0, 0, 0, 52000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');

DELETE FROM locales_gameobject WHERE entry = 502000;

INSERT INTO locales_gameobject
   (`entry`, `name_loc1`, `name_loc2`, `name_loc3`, `name_loc4`, `name_loc5`, `name_loc6`, `name_loc7`, `name_loc8`, `castbarcaption_loc1`, `castbarcaption_loc2`, `castbarcaption_loc3`, `castbarcaption_loc4`, `castbarcaption_loc5`, `castbarcaption_loc6`, `castbarcaption_loc7`, `castbarcaption_loc8`)
VALUES
   (502000, '', '', '', '', '', '', '', 'Книга правил', '', '', '', '', '', '', '', '');

DELETE FROM page_text WHERE entry BETWEEN 52000 AND 52001;

INSERT INTO page_text
   (`entry`, `text`, `next_page`)
VALUES
   (52000, 'Привет, $N!\n\nЗдесь ты можешь узнать о правилах сервера. Рекомендуем их выполнять, в противном случае может наступить незамедлительное возмездие. И не говори потом, что мы не предупреждали!\n\n\n\n\n\n\n\n\n\nПродолжение на второй странице...', 52001);


INSERT INTO page_text
   (`entry`, `text`, `next_page`)
VALUES
   (52001, 'Запрещено:\nМат, капс, флуд, цветной текст в чате (блокировка чата вплоть до 2 (двух) часов);\nОскорбления игроков и администрации, участие в баго-рейдах (бан до 1 (одной) недели);\nПерелив рейтинга арены (бан на 1 (один) месяц);\nОбмен/торговля аккаунтами, обман пользователей проекта с целью мошенничества, использование читов и стороннего ПО, реклама сторонних ресурсов (бан навсегда).\n\nСоблюдение этих простых правил приведёт к комфортной игре на нашем сервере. Приятной игры!\n\nПолезные ссылки:\nhttp://300Murlocs.com - наш сервер;\nwww.vk.com/murlocs - наша группа ВКонтакте;\nwww.bit.ly/tehpod - форум техподдержки;\nwww.bit.ly/pravill - полные правила нашего сервера;\nwww.bit.ly/konkyrs - форумные конкурсы;\nwww.bit.ly/vakansii - вакансии.', 0);

