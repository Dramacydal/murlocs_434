-- TODO: update for 4.x
/*--------------------------------------------------------------------*/
/*--------------------------Main PET spellfixes-----------------------*/
/*--------------------------------------------------------------------*/

-- DELETE FROM spell_proc_event WHERE entry IN (53178, 53179, 62764, 62765);
-- INSERT INTO spell_proc_event (entry, SchoolMask, SpellFamilyName, SpellFamilyMaskB0, SpellFamilyMaskB1, SpellFamilyMaskB2, procFlags, procEx, ppmRate, CustomChance, Cooldown) VALUES
--  (53178, 0x00, 9, 0x00000000, 0x10000000, 0x00000000, 0x00010000, 0x00000000, 0, 100, 0), -- Guard Dog
--  (62764, 0x00, 9, 0x00000000, 0x10000000, 0x00000000, 0x00010000, 0x00000000, 0, 100, 0); -- Silverback



/*--------------------------------------------------------------------*/
/*-----------Secondary PET spellfixes (spell pet auras etc.)----------*/
/*--------------------------------------------------------------------*/

-- Kindred Spirits
-- DELETE FROM spell_pet_auras WHERE spell IN (56314, 56315, 56316, 56317, 56318);
-- INSERT INTO spell_pet_auras VALUES
-- (56314, 0, 0, 57447),
-- (56314, 1, 0, 57485),
-- (56315, 0, 0, 57452),
-- (56315, 1, 0, 57484),
-- (56316, 0, 0, 57453),
-- (56316, 1, 0, 57483),
-- (56317, 0, 0, 57457),
-- (56317, 1, 0, 57482),
-- (56318, 0, 0, 57458),
-- (56318, 1, 0, 57475);

-- Ferocious Inspiration
-- DELETE FROM spell_pet_auras WHERE spell IN (34460, 34459, 34455);
-- INSERT INTO spell_pet_auras VALUES
-- (34460, 0, 0, 75447),
-- (34459, 0, 0, 75446),
-- (34455, 0, 0, 75593);

-- Animal Handler
-- DELETE FROM spell_pet_auras WHERE spell IN (34453, 34454);
-- INSERT INTO spell_pet_auras VALUES
-- (34453, 1, 0, 68361),
-- (34454, 1, 0, 68361);
