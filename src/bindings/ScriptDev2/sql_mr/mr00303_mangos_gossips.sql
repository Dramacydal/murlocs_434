-- NeatElves
-- Orb of Command
DELETE FROM gossip_menu WHERE entry=6001;
INSERT INTO `gossip_menu` (`entry`, `text_id`, `cond_1`, `cond_1_val_1`, `cond_1_val_2`, `cond_2`, `cond_2_val_1`, `cond_2_val_2`) VALUES
(6001, 7154, 0, 0, 0, 0, 0, 0),
(6001, 7155, 8, 7761, 0, 0, 0, 0);
DELETE FROM gossip_menu_option WHERE menu_id=6001;
INSERT INTO `gossip_menu_option` (`menu_id`, `id`, `option_icon`, `option_text`, `option_id`, `npc_option_npcflag`, `action_menu_id`, `action_poi_id`, `action_script_id`, `box_coded`, `box_money`, `box_text`, `cond_1`, `cond_1_val_1`, `cond_1_val_2`, `cond_2`, `cond_2_val_1`, `cond_2_val_2`, `cond_3`, `cond_3_val_1`, `cond_3_val_2`) VALUES
(6001, 0, 0, '<Put your hand on the sphere.>', 1, 1, -1, 0, 6001, 0, 0, NULL, 8, 7761, 0, 0, 0, 0, 0, 0, 0);
DELETE FROM gossip_scripts WHERE id=6001;
INSERT INTO `gossip_scripts` (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES
(6001, 0, 15, 23460, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '');
-- Lunaclaw Spirit
DELETE FROM gossip_scripts WHERE id IN (386201, 386202);
INSERT INTO gossip_scripts (`id`, `delay`, `command`, `datalong`, `datalong2`, `datalong3`, `datalong4`, `data_flags`, `dataint`, `dataint2`, `dataint3`, `dataint4`, `x`, `y`, `z`, `o`, `comments`) VALUES
(386201, 0, 7, 6001, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Spirit of Lunaclaw - Questcredit (Alliance)'),
(386202, 0, 7, 6002, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'Spirit of Lunaclaw - Questcredit (Horde)');
DELETE FROM gossip_menu_option WHERE menu_id=3862;
INSERT INTO gossip_menu_option VALUES
('3862', '0', '0', 'You have fought well, spirit.  I ask you to grant me the strength of your body and the strength of your heart.', '1', '1', '3863', '0', '386201', '0', '0', '', 9, 6001, 1, '0', '0', '0', '0', '0', '0'),
('3862', '1', '0', 'You have fought well, spirit.  I ask you to grant me the strength of your body and the strength of your heart.', '1', '1', '3863', '0', '386202', '0', '0', '', 9, 6002, 1, '0', '0', '0', '0', '0', '0');
-- Q3377 & npc_zamael_lunthistle
UPDATE gossip_menu_option SET cond_1=9,cond_1_val_1=3377, cond_1_val_2=1 WHERE menu_id=1285 AND id=0;
UPDATE gossip_menu_option SET action_menu_id= -1, action_script_id=1286 WHERE menu_id=1286 AND id=0;
DELETE FROM gossip_scripts WHERE id=1286;
INSERT INTO gossip_scripts (id, delay, command, datalong, datalong2, comments) VALUES
(1286, 0, 7, 3377, 5, 'Zamael Lunthistle - Questcredit (3377');

-- one fix
UPDATE quest_template SET SpecialFlags = 2 WHERE entry = 3377;
