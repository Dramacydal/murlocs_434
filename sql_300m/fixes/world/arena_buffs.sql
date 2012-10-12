UPDATE `gameobject` SET `animprogress` = 100, `spawntimesecs` = 60 WHERE `id` IN (184663, 184664);
UPDATE `gameobject_template` SET `data5` = 60, `data6` = 60000 WHERE `entry` IN (184663, 184664);