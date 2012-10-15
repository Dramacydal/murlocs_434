-- SotA
UPDATE creature_template SET ScriptName='npc_sa_cannon' WHERE entry = 27894;
UPDATE creature_template SET ScriptName='npc_sa_demolisher' WHERE entry = 28781;
UPDATE creature_template SET ScriptName='npc_sa_vendor' WHERE entry IN (29260, 29262);
UPDATE creature_template SET ScriptName='' WHERE entry = 50000; -- delete this entry  about after a mnth
UPDATE gameobject_template SET ScriptName='go_sa_def_portal' WHERE entry = 191575;
UPDATE gameobject_template SET ScriptName='' WHERE entry IN (194086, 190753);
