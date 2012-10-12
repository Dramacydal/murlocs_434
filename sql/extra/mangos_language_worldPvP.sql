DELETE FROM `mangos_string` WHERE `entry` BETWEEN 10001 AND 10050;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES

-- all messages are sent in yellow text in SMSG_DEFENSE_MESSAGE (LocalDefense channel)
-- losing control of a capture point doesnt send a text (not yet confirmed for all)
-- TODO: Switch alliance and horde position (alliance before horde)

-- silithus
(10001,'The Horde has collected 200 silithyst!', 'Орда собрала 200 силитистов!'),
(10002,'The Alliance has collected 200 silithyst!', 'Альянс собрал 200 силитистов!'),

-- eastern plaguelands
(10003,'The Horde has taken the Northpass Tower!', 'Орда захватила башню Северного перевала!'),
(10004,'The Alliance has taken the Northpass Tower!', 'Альянс захватил башню Северного перевала!'),
(10005,'The Horde has taken the Eastwall Tower!', 'Орда захватила Восточную башню!'),
(10006,'The Alliance has taken the Eastwall Tower!', 'Альянс захватил Восточную башню!'),
(10007,'The Horde has taken the Crown Guard Tower!', 'Орда захватила башню Королевской Стражи!'),
(10008,'The Alliance has taken the Crown Guard Tower!', 'Альянс захватил башню Королевской Стражи!'),
(10009,'The Horde has taken the Plaguewood Tower!', 'Орда захватила Башню Чумного леса!'),
(10010,'The Alliance has taken the Plaguewood Tower!', 'Альянс захватил Башню Чумного леса!'),
(10011,'The Horde lost the Northpass Tower!', 'Орда потеряла башню Северного перевала!'),
(10012,'The Alliance lost the Northpass Tower!', 'Альянс потерял башню Северного перевала!'),
(10013,'The Horde lost the Eastwall Tower!', 'Орда потеряла Восточную башню!'),
(10014,'The Alliance lost the Eastwall Tower!', 'Альянс потерял Восточную башню!'),
(10015,'The Horde lost the Crown Guard Tower!', 'Орда потеряла башню Королевской Стражи!'),
(10016,'The Alliance lost the Crown Guard Tower!', 'Альянс потерял башню Королевской Стражи!'),
(10017,'The Horde lost the Plaguewood Tower!', 'Орда потеряла Башню Чумного леса!'),
(10018,'The Alliance lost the Plaguewood Tower!', 'Альянс потерял Башню Чумного леса!'),

-- hellfire peninsula
(10019,'|cffffff00The Overlook has been taken by the Horde!|r', '|cffffff00Обзорная площадка захвачена Ордой!|r'),
(10020,'|cffffff00The Overlook has been taken by the Alliance!|r', '|cffffff00Обзорная площадка захвачена Альянсом!|r'),
(10021,'|cffffff00The Stadium has been taken by the Horde!|r', '|cffffff00Ристалище захвачено Ордой!|r'),
(10022,'|cffffff00The Stadium has been taken by the Alliance!|r', '|cffffff00Ристалище захвачено Альянсом!|r'),
(10023,'|cffffff00Broken Hill has been taken by the Horde!|r', '|cffffff00Изрезанных холм захвачен Ордой!|r'),
(10024,'|cffffff00Broken Hill has been taken by the Alliance!|r', '|cffffff00Изрезанных холм захвачен Альянсом!|r'),

 -- following does not exist on retail (confirmed!)
(10025,'The Horde lost The Overlook!', 'Орда потеряла Обзорную площадку!'),
(10026,'The Alliance lost The Overlook!', 'Альянс потерял Обзорную площадку!'),
(10027,'The Horde lost The Stadium!', 'Орда потеряла Ристалище!'),
(10028,'The Alliance lost The Stadium!', 'Альянс потерял Ристалище!'),
(10029,'The Horde lost Broken Hill!', 'Орда потеряла Изрезанный холм!'),
(10030,'The Alliance lost Broken Hill!', 'Альянс потерял Изрезанный холм!'),

-- zangarmarsh
(10031,'|cffffff00The Horde has taken control of the West Beacon!|r', '|cffffff00Орда захватила Западный Маяк!|r'),
(10032,'|cffffff00The Alliance has taken control of the West Beacon!|r', '|cffffff00Альянс захватил Западный Маяк!|r'),
(10033,'|cffffff00The Horde has taken control of the East Beacon!|r', '|cffffff00Орда захватила Восточный Маяк!|r'),
(10034,'|cffffff00The Alliance has taken control of the East Beacon!|r', '|cffffff00Альянс захватил Восточный Маяк!|r'),
-- '|cffffff00The Horde has taken control of both beacons!|r'
-- '|cffffff00The Alliance has taken control of both beacons!|r'
-- '|cffffff00The Horde Field Scout is now issuing battle standards.|r' -- right after 'both beacons' message
-- '|cffffff00The Alliance Field Scout is now issuing battle standards.|r'
(10035,'|cffffff00The Horde has taken control of Twin Spire Ruins!|r','|cffffff00Орда контролирует Руины Двух Башен!|r'),
(10036,'|cffffff00The Alliance has taken control of Twin Spire Ruins!|r','|cffffff00Альянс контролирует Руины Двух Башен!|r'), -- SMSG_PLAY_SOUND id 8173, from npc 15384 (npc id may be a parsing error, but unlikely)

-- following might not exist (not confirmed)
(10037,'The Horde lost the West Beacon!','Орда потеряла Западный Маяк!'),
(10038,'The Alliance lost the West Beacon!','Альянс потерял Западный Маяк!'),
(10039,'The Horde lost the East Beacon!','Орда потеряла Восточный Маяк!'),
(10040,'The Alliance lost the East Beacon!','Альянс потерял Восточный Маяк!'),
(10041,'The Horde lost the Twin Spire Graveyard!','Орда потеряла Руины Двух Башен!'),
(10042,'The Alliance lost the Twin Spire Graveyard!','Альянс потерял Руины Двух Башен!'),

-- nagrand
-- '|cffffff00The Horde is gaining control of Halaa!|r' -- at 51 % (progress event)
-- '|cffffff00The Alliance is gaining control of Halaa!|r'
(10043,'|cffffff00The Horde has taken control of Halaa!|r', '|cffffff00Орда контролирует Халаа!|r'),
(10044,'|cffffff00The Alliance has taken control of Halaa!|r', '|cffffff00Альянс контролирует Халаа!|r'),

-- following might not exist (not confirmed)
(10045,'The Horde lost Halaa!', 'Орда потеряла Халаа!'),
(10046,'The Alliance lost Halaa!', 'Орда потеряла Халаа!'),

-- terokkar forest
(10047,'|cffffff00The Horde has taken control of a Spirit Tower!|r', '|cffffff00Орда захватила Башню Духов!|r'),
(10048,'|cffffff00The Alliance has taken control of a Spirit Tower!|r', '|cffffff00Альянс захватил Башню Духов!|r'),
-- '|cffffff00The Alliance has taken control of The Bone Wastes!|r'
-- '|cffffff00The Horde has taken control of The Bone Wastes!|r'

-- following might not exist (not confirmed)
(10049,'The Horde lost a Spirit Tower!', 'Орда потеряла Башню Духов!'),
(10050,'The Alliance lost a Spirit Tower!', 'Альянс потерял Башню Духов!');