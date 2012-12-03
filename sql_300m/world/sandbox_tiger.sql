DELETE FROM npc_spellclick_spells WHERE npc_entry IN (33357);
INSERT INTO npc_spellclick_spells VALUES
(33357, 67373, 0, 0, 0, 1);

UPDATE `creature_template` SET `vehicle_id` = 354, IconName = 'vehicleCursor' WHERE `entry` = 33357;