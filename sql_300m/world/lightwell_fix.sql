-- TODO: update for 4.x

-- DELETE FROM npc_spellclick_spells WHERE npc_entry IN (31897, 31896, 31895, 31894, 31893, 31883);
-- INSERT INTO npc_spellclick_spells (npc_entry, spell_id, cast_flags) VALUES (31897, 7001, 2), (31896, 27873, 2), (31895, 27874, 2), (31894, 28276, 2), (31893, 48084, 2), (31883, 48085, 2);
-- DELETE FROM creature_template_addon WHERE entry IN (31897, 31896, 31895, 31894, 31893, 31883);
-- INSERT INTO creature_template_addon (entry, auras) VALUES (31897, "59907 0"), (31896, "59907 0"), (31895, "59907 0"), (31894, "59907 0"), (31893, "59907 0"), (31883, "59907 0");
-- delete from spell_bonus_data where entry in (7001, 27873, 27874, 28276, 48084, 48085);
-- insert into spell_bonus_data (entry, direct_bonus, dot_bonus) VALUES (7001, 0, 0.63), (27873, 0, 0.63), (27874, 0, 0.63), (28276, 0, 0.63), (48084, 0, 0.63), (48085, 0, 0.63);

