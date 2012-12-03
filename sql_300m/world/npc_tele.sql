DELETE FROM `npc_text` WHERE `ID` = 888003;
INSERT INTO `npc_text` (`ID`, `Text0_0`) VALUES
(888003, 'Donate a small amount of gold and I will send you wherever you want!');

DELETE FROM `locales_npc_text` WHERE `entry` = 888003;
INSERT INTO `locales_npc_text` (`entry`, `Text0_0_loc1`, `Text0_0_loc8`) VALUES
(888003, 'Donate a small amount of gold and I will send you wherever you want!', 'Всего за небольшую сумму я отправлю тебя туда, куда захочешь!');

DELETE FROM `creature_template` WHERE `entry` = 88003;
INSERT INTO `creature_template`
(entry, difficulty_entry_1, difficulty_entry_2, difficulty_entry_3, KillCredit1, KillCredit2, modelid_1, modelid_2, modelid_3, modelid_4, name, subname, IconName, gossip_menu_id, minlevel, maxlevel, minhealth, maxhealth, powerType, minmana, maxmana, armor, faction_A, faction_H, npcflag, speed_walk, speed_run, scale, rank, mindmg, maxdmg, dmgschool, attackpower, dmg_multiplier, baseattacktime, rangeattacktime, unit_class, unit_flags, dynamicflags, family, trainer_type, trainer_spell, trainer_class, trainer_race, minrangedmg, maxrangedmg, rangedattackpower, type, type_flags, lootid, pickpocketloot, skinloot, resistance1, resistance2, resistance3, resistance4, resistance5, resistance6, spell1, spell2, spell3, spell4, spell5, spell6, spell7, spell8, PetSpellDataId, mingold, maxgold, AIName, MovementType, InhabitType, health_mod, power_mod, RacialLeader, questItem1, questItem2, questItem3, questItem4, questItem5, questItem6, movementId, RegenHealth, vehicle_id, equipment_id, trainer_id, vendor_id, mechanic_immune_mask, flags_extra, ScriptName) VALUE
(88003, 0, 0, 0, 0, 0, 25609, 0, 0, 0, 'Мастер порталов', NULL, NULL, 0, 80, 80, 5100, 5100, 0, 5751, 5751, 3000, 35, 35, 3, 1.25, 1, 1.14286, 1, 70, 105, 0, 26, 1, 1500, 1500, 8, 0, 0, 0, 0, 0, 0, 0, 56, 84, 21, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '', 0, 3, 1.2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 2, 'npc_teleport');
