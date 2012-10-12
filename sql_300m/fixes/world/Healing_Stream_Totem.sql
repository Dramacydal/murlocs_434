

/*

Healing_Stream_Totem.sql

Отчистка данных из БД - никакого влияние на спелл http://www.wowhead.com/?spell=52042 не даёт -> рализовано через код

*/


DELETE FROM `spell_bonus_data` WHERE `entry`='52042';


/*

Бэкап:

INSERT INTO spell_bonus_data
   (`entry`, `direct_bonus`, `dot_bonus`, `ap_bonus`, `comments`)
VALUES
   (52042, 0.045, 0, 0, 'Shaman - Healing Stream Totem Triggered Heal');

*/


#EoF
