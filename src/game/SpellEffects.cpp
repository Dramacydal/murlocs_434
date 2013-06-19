/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "SkillExtraItems.h"
#include "Unit.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "SpellAuras.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "Pet.h"
#include "GameObject.h"
#include "GossipDef.h"
#include "Creature.h"
#include "Totem.h"
#include "CreatureAI.h"
#include "BattleGround/BattleGroundMgr.h"
#include "BattleGround/BattleGround.h"
#include "BattleGround/BattleGroundEY.h"
#include "BattleGround/BattleGroundTP.h"
#include "BattleGround/BattleGroundWS.h"
#include "BattleGround/BattleGroundSA.h"
#include "OutdoorPvP/OutdoorPvPMgr.h"
#include "Language.h"
#include "SocialMgr.h"
#include "VMapFactory.h"
#include "Util.h"
#include "Vehicle.h"
#include "TemporarySummon.h"
#include "ThreatManager.h"
#include "ScriptMgr.h"
#include "SkillDiscovery.h"
#include "Formulas.h"
#include "GridNotifiers.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "CellImpl.h"
#include "OutdoorPvP/OutdoorPvP.h"
#include "BattleField/BattleFieldWG.h"
#include "Guild.h"
#include "GuildMgr.h"
#include <G3D/Vector3.h>
#include "PhaseMgr.h"

pEffect SpellEffects[TOTAL_SPELL_EFFECTS]=
{
    &Spell::EffectNULL,                                     //  0
    &Spell::EffectInstaKill,                                //  1 SPELL_EFFECT_INSTAKILL
    &Spell::EffectSchoolDMG,                                //  2 SPELL_EFFECT_SCHOOL_DAMAGE
    &Spell::EffectDummy,                                    //  3 SPELL_EFFECT_DUMMY
    &Spell::EffectUnused,                                   //  4 SPELL_EFFECT_PORTAL_TELEPORT          unused from pre-1.2.1
    &Spell::EffectTeleportUnits,                            //  5 SPELL_EFFECT_TELEPORT_UNITS
    &Spell::EffectApplyAura,                                //  6 SPELL_EFFECT_APPLY_AURA
    &Spell::EffectEnvironmentalDMG,                         //  7 SPELL_EFFECT_ENVIRONMENTAL_DAMAGE
    &Spell::EffectPowerDrain,                               //  8 SPELL_EFFECT_POWER_DRAIN
    &Spell::EffectHealthLeech,                              //  9 SPELL_EFFECT_HEALTH_LEECH
    &Spell::EffectHeal,                                     // 10 SPELL_EFFECT_HEAL
    &Spell::EffectBind,                                     // 11 SPELL_EFFECT_BIND
    &Spell::EffectUnused,                                   // 12 SPELL_EFFECT_PORTAL                   unused from pre-1.2.1, exist 2 spell, but not exist any data about its real usage
    &Spell::EffectUnused,                                   // 13 SPELL_EFFECT_RITUAL_BASE              unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 14 SPELL_EFFECT_RITUAL_SPECIALIZE        unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 15 SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL   unused from pre-1.2.1
    &Spell::EffectQuestComplete,                            // 16 SPELL_EFFECT_QUEST_COMPLETE
    &Spell::EffectWeaponDmg,                                // 17 SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL
    &Spell::EffectResurrect,                                // 18 SPELL_EFFECT_RESURRECT
    &Spell::EffectAddExtraAttacks,                          // 19 SPELL_EFFECT_ADD_EXTRA_ATTACKS
    &Spell::EffectEmpty,                                    // 20 SPELL_EFFECT_DODGE                    one spell: Dodge
    &Spell::EffectEmpty,                                    // 21 SPELL_EFFECT_EVADE                    one spell: Evade (DND)
    &Spell::EffectParry,                                    // 22 SPELL_EFFECT_PARRY
    &Spell::EffectBlock,                                    // 23 SPELL_EFFECT_BLOCK                    one spell: Block
    &Spell::EffectCreateItem,                               // 24 SPELL_EFFECT_CREATE_ITEM
    &Spell::EffectEmpty,                                    // 25 SPELL_EFFECT_WEAPON                   spell per weapon type, in ItemSubclassmask store mask that can be used for usability check at equip, but current way using skill also work.
    &Spell::EffectEmpty,                                    // 26 SPELL_EFFECT_DEFENSE                  one spell: Defense
    &Spell::EffectPersistentAA,                             // 27 SPELL_EFFECT_PERSISTENT_AREA_AURA
    &Spell::EffectSummonType,                               // 28 SPELL_EFFECT_SUMMON
    &Spell::EffectLeapForward,                              // 29 SPELL_EFFECT_LEAP
    &Spell::EffectEnergize,                                 // 30 SPELL_EFFECT_ENERGIZE
    &Spell::EffectWeaponDmg,                                // 31 SPELL_EFFECT_WEAPON_PERCENT_DAMAGE
    &Spell::EffectTriggerMissileSpell,                      // 32 SPELL_EFFECT_TRIGGER_MISSILE
    &Spell::EffectOpenLock,                                 // 33 SPELL_EFFECT_OPEN_LOCK
    &Spell::EffectSummonChangeItem,                         // 34 SPELL_EFFECT_SUMMON_CHANGE_ITEM
    &Spell::EffectApplyAreaAura,                            // 35 SPELL_EFFECT_APPLY_AREA_AURA_PARTY
    &Spell::EffectLearnSpell,                               // 36 SPELL_EFFECT_LEARN_SPELL
    &Spell::EffectEmpty,                                    // 37 SPELL_EFFECT_SPELL_DEFENSE            one spell: SPELLDEFENSE (DND)
    &Spell::EffectDispel,                                   // 38 SPELL_EFFECT_DISPEL
    &Spell::EffectEmpty,                                    // 39 SPELL_EFFECT_LANGUAGE                 misc store lang id
    &Spell::EffectDualWield,                                // 40 SPELL_EFFECT_DUAL_WIELD
    &Spell::EffectJump,                                     // 41 SPELL_EFFECT_JUMP
    &Spell::EffectJump,                                     // 42 SPELL_EFFECT_JUMP2
    &Spell::EffectTeleUnitsFaceCaster,                      // 43 SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER
    &Spell::EffectLearnSkill,                               // 44 SPELL_EFFECT_SKILL_STEP
    &Spell::EffectPlayMovie,                                // 45 SPELL_EFFECT_PLAY_MOVIE
    &Spell::EffectNULL,                                     // 46 SPELL_EFFECT_SPAWN                    spawn/login animation, expected by spawn unit cast, also base points store some dynflags
    &Spell::EffectTradeSkill,                               // 47 SPELL_EFFECT_TRADE_SKILL
    &Spell::EffectUnused,                                   // 48 SPELL_EFFECT_STEALTH                  one spell: Base Stealth
    &Spell::EffectUnused,                                   // 49 SPELL_EFFECT_DETECT                   one spell: Detect
    &Spell::EffectTransmitted,                              // 50 SPELL_EFFECT_TRANS_DOOR
    &Spell::EffectUnused,                                   // 51 SPELL_EFFECT_FORCE_CRITICAL_HIT       unused from pre-1.2.1
    &Spell::EffectUnused,                                   // 52 SPELL_EFFECT_GUARANTEE_HIT            unused from pre-1.2.1
    &Spell::EffectEnchantItemPerm,                          // 53 SPELL_EFFECT_ENCHANT_ITEM
    &Spell::EffectEnchantItemTmp,                           // 54 SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY
    &Spell::EffectTameCreature,                             // 55 SPELL_EFFECT_TAMECREATURE
    &Spell::EffectSummonPet,                                // 56 SPELL_EFFECT_SUMMON_PET
    &Spell::EffectLearnPetSpell,                            // 57 SPELL_EFFECT_LEARN_PET_SPELL
    &Spell::EffectWeaponDmg,                                // 58 SPELL_EFFECT_WEAPON_DAMAGE
    &Spell::EffectCreateRandomItem,                         // 59 SPELL_EFFECT_CREATE_RANDOM_ITEM       create item base at spell specific loot
    &Spell::EffectProficiency,                              // 60 SPELL_EFFECT_PROFICIENCY
    &Spell::EffectSendEvent,                                // 61 SPELL_EFFECT_SEND_EVENT
    &Spell::EffectPowerBurn,                                // 62 SPELL_EFFECT_POWER_BURN
    &Spell::EffectThreat,                                   // 63 SPELL_EFFECT_THREAT
    &Spell::EffectTriggerSpell,                             // 64 SPELL_EFFECT_TRIGGER_SPELL
    &Spell::EffectApplyAreaAura,                            // 65 SPELL_EFFECT_APPLY_AREA_AURA_RAID
    &Spell::EffectRestoreItemCharges,                       // 66 SPELL_EFFECT_RESTORE_ITEM_CHARGES     itemtype - is affected item ID
    &Spell::EffectHealMaxHealth,                            // 67 SPELL_EFFECT_HEAL_MAX_HEALTH
    &Spell::EffectInterruptCast,                            // 68 SPELL_EFFECT_INTERRUPT_CAST
    &Spell::EffectDistract,                                 // 69 SPELL_EFFECT_DISTRACT
    &Spell::EffectPull,                                     // 70 SPELL_EFFECT_PULL                     one spell: Distract Move
    &Spell::EffectPickPocket,                               // 71 SPELL_EFFECT_PICKPOCKET
    &Spell::EffectAddFarsight,                              // 72 SPELL_EFFECT_ADD_FARSIGHT
    &Spell::EffectNULL,                                     // 73 SPELL_EFFECT_UNTRAIN_TALENTS          one spell: Trainer: Untrain Talents
    &Spell::EffectApplyGlyph,                               // 74 SPELL_EFFECT_APPLY_GLYPH
    &Spell::EffectHealMechanical,                           // 75 SPELL_EFFECT_HEAL_MECHANICAL          one spell: Mechanical Patch Kit
    &Spell::EffectSummonObjectWild,                         // 76 SPELL_EFFECT_SUMMON_OBJECT_WILD
    &Spell::EffectScriptEffect,                             // 77 SPELL_EFFECT_SCRIPT_EFFECT
    &Spell::EffectUnused,                                   // 78 SPELL_EFFECT_ATTACK
    &Spell::EffectSanctuary,                                // 79 SPELL_EFFECT_SANCTUARY
    &Spell::EffectAddComboPoints,                           // 80 SPELL_EFFECT_ADD_COMBO_POINTS
    &Spell::EffectUnused,                                   // 81 SPELL_EFFECT_CREATE_HOUSE             one spell: Create House (TEST)
    &Spell::EffectNULL,                                     // 82 SPELL_EFFECT_BIND_SIGHT
    &Spell::EffectDuel,                                     // 83 SPELL_EFFECT_DUEL
    &Spell::EffectStuck,                                    // 84 SPELL_EFFECT_STUCK
    &Spell::EffectSummonPlayer,                             // 85 SPELL_EFFECT_SUMMON_PLAYER
    &Spell::EffectActivateObject,                           // 86 SPELL_EFFECT_ACTIVATE_OBJECT
    &Spell::EffectWMODamage,                                // 87 SPELL_EFFECT_WMO_DAMAGE (57 spells in 3.3.2)
    &Spell::EffectWMORepair,                                // 88 SPELL_EFFECT_WMO_REPAIR (2 spells in 3.3.2)
    &Spell::EffectWMOChange,                                // 89 SPELL_EFFECT_WMO_CHANGE (7 spells in 3.3.2)
    &Spell::EffectKillCreditPersonal,                       // 90 SPELL_EFFECT_KILL_CREDIT_PERSONAL     Kill credit but only for single person
    &Spell::EffectUnused,                                   // 91 SPELL_EFFECT_THREAT_ALL               one spell: zzOLDBrainwash
    &Spell::EffectEnchantHeldItem,                          // 92 SPELL_EFFECT_ENCHANT_HELD_ITEM
    &Spell::EffectBreakPlayerTargeting,                     // 93 SPELL_EFFECT_BREAK_PLAYER_TARGETING
    &Spell::EffectSelfResurrect,                            // 94 SPELL_EFFECT_SELF_RESURRECT
    &Spell::EffectSkinning,                                 // 95 SPELL_EFFECT_SKINNING
    &Spell::EffectCharge,                                   // 96 SPELL_EFFECT_CHARGE
    &Spell::EffectSummonAllTotems,                          // 97 SPELL_EFFECT_SUMMON_ALL_TOTEMS
    &Spell::EffectKnockBack,                                // 98 SPELL_EFFECT_KNOCK_BACK
    &Spell::EffectDisEnchant,                               // 99 SPELL_EFFECT_DISENCHANT
    &Spell::EffectInebriate,                                //100 SPELL_EFFECT_INEBRIATE
    &Spell::EffectFeedPet,                                  //101 SPELL_EFFECT_FEED_PET
    &Spell::EffectDismissPet,                               //102 SPELL_EFFECT_DISMISS_PET
    &Spell::EffectReputation,                               //103 SPELL_EFFECT_REPUTATION
    &Spell::EffectSummonObject,                             //104 SPELL_EFFECT_SUMMON_OBJECT_SLOT
    &Spell::EffectSurvey,                                   //105 SPELL_EFFECT_SURVEY
    &Spell::EffectSummonRaidMarker,                         //106 SPELL_EFFECT_SUMMON_RAID_MARKER
    &Spell::EffectNULL,                                     //107 SPELL_EFFECT_LOOT_CORPSE
    &Spell::EffectDispelMechanic,                           //108 SPELL_EFFECT_DISPEL_MECHANIC
    &Spell::EffectSummonDeadPet,                            //109 SPELL_EFFECT_SUMMON_DEAD_PET
    &Spell::EffectDestroyAllTotems,                         //110 SPELL_EFFECT_DESTROY_ALL_TOTEMS
    &Spell::EffectDurabilityDamage,                         //111 SPELL_EFFECT_DURABILITY_DAMAGE
    &Spell::EffectUnused,                                   //112 SPELL_EFFECT_112 (old SPELL_EFFECT_SUMMON_DEMON)
    &Spell::EffectResurrectNew,                             //113 SPELL_EFFECT_RESURRECT_NEW
    &Spell::EffectTaunt,                                    //114 SPELL_EFFECT_ATTACK_ME
    &Spell::EffectDurabilityDamagePCT,                      //115 SPELL_EFFECT_DURABILITY_DAMAGE_PCT
    &Spell::EffectSkinPlayerCorpse,                         //116 SPELL_EFFECT_SKIN_PLAYER_CORPSE       one spell: Remove Insignia, bg usage, required special corpse flags...
    &Spell::EffectSpiritHeal,                               //117 SPELL_EFFECT_SPIRIT_HEAL              one spell: Spirit Heal
    &Spell::EffectSkill,                                    //118 SPELL_EFFECT_SKILL                    professions and more
    &Spell::EffectApplyAreaAura,                            //119 SPELL_EFFECT_APPLY_AREA_AURA_PET
    &Spell::EffectUnused,                                   //120 SPELL_EFFECT_TELEPORT_GRAVEYARD       one spell: Graveyard Teleport Test
    &Spell::EffectWeaponDmg,                                //121 SPELL_EFFECT_NORMALIZED_WEAPON_DMG
    &Spell::EffectUnused,                                   //122 SPELL_EFFECT_122                      unused
    &Spell::EffectSendTaxi,                                 //123 SPELL_EFFECT_SEND_TAXI                taxi/flight related (misc value is taxi path id)
    &Spell::EffectPullTowards,                              //124 SPELL_EFFECT_PLAYER_PULL              opposite of knockback effect (pulls player twoard caster)
    &Spell::EffectModifyThreatPercent,                      //125 SPELL_EFFECT_MODIFY_THREAT_PERCENT
    &Spell::EffectStealBeneficialBuff,                      //126 SPELL_EFFECT_STEAL_BENEFICIAL_BUFF    spell steal effect?
    &Spell::EffectProspecting,                              //127 SPELL_EFFECT_PROSPECTING              Prospecting spell
    &Spell::EffectApplyAreaAura,                            //128 SPELL_EFFECT_APPLY_AREA_AURA_FRIEND
    &Spell::EffectApplyAreaAura,                            //129 SPELL_EFFECT_APPLY_AREA_AURA_ENEMY
    &Spell::EffectRedirectThreat,                           //130 SPELL_EFFECT_REDIRECT_THREAT
    &Spell::EffectPlaySound,                                //131 SPELL_EFFECT_PLAY_SOUND               sound id in misc value (SoundEntries.dbc)
    &Spell::EffectPlayMusic,                                //132 SPELL_EFFECT_PLAY_MUSIC               sound id in misc value (SoundEntries.dbc)
    &Spell::EffectUnlearnSpecialization,                    //133 SPELL_EFFECT_UNLEARN_SPECIALIZATION   unlearn profession specialization
    &Spell::EffectKillCreditGroup,                          //134 SPELL_EFFECT_KILL_CREDIT_GROUP        misc value is creature entry
    &Spell::EffectNULL,                                     //135 SPELL_EFFECT_CALL_PET
    &Spell::EffectHealPct,                                  //136 SPELL_EFFECT_HEAL_PCT
    &Spell::EffectEnergisePct,                              //137 SPELL_EFFECT_ENERGIZE_PCT
    &Spell::EffectLeapBack,                                 //138 SPELL_EFFECT_LEAP_BACK                Leap back
    &Spell::EffectClearQuest,                               //139 SPELL_EFFECT_CLEAR_QUEST              (misc - is quest ID)
    &Spell::EffectForceCast,                                //140 SPELL_EFFECT_FORCE_CAST
    &Spell::EffectForceCastWithValue,                       //141 SPELL_EFFECT_FORCE_CAST_WITH_VALUE    
    &Spell::EffectTriggerSpellWithValue,                    //142 SPELL_EFFECT_TRIGGER_SPELL_WITH_VALUE
    &Spell::EffectApplyAreaAura,                            //143 SPELL_EFFECT_APPLY_AREA_AURA_OWNER
    &Spell::EffectKnockBack,                                //144 SPELL_EFFECT_KNOCK_BACK_DEST          Spectral Blast
    &Spell::EffectSuspendGravity,                           //145 SPELL_EFFECT_PULL_TOWARDS_DEST        Black Hole Effect
    &Spell::EffectActivateRune,                             //146 SPELL_EFFECT_ACTIVATE_RUNE
    &Spell::EffectQuestFail,                                //147 SPELL_EFFECT_QUEST_FAIL               quest fail
    &Spell::EffectTriggerMissileSpell,                      //148 SPELL_EFFECT_148                      single spell: Inflicts Fire damage to an enemy.
    &Spell::EffectCharge2,                                  //149 SPELL_EFFECT_CHARGE2                  swoop
    &Spell::EffectQuestOffer,                               //150 SPELL_EFFECT_QUEST_OFFER
    &Spell::EffectTriggerRitualOfSummoning,                 //151 SPELL_EFFECT_TRIGGER_SPELL_2
    &Spell::EffectFriendSummon,                             //152 SPELL_EFFECT_FRIEND_SUMMON            summon Refer-a-Friend
    &Spell::EffectNULL,                                     //153 SPELL_EFFECT_CREATE_PET               misc value is creature entry
    &Spell::EffectTeachTaxiNode,                            //154 SPELL_EFFECT_TEACH_TAXI_NODE          single spell: Teach River's Heart Taxi Path
    &Spell::EffectTitanGrip,                                //155 SPELL_EFFECT_TITAN_GRIP Allows you to equip two-handed axes, maces and swords in one hand, but you attack $49152s1% slower than normal.
    &Spell::EffectEnchantItemPrismatic,                     //156 SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC
    &Spell::EffectCreateItem2,                              //157 SPELL_EFFECT_CREATE_ITEM_2            create item or create item template and replace by some randon spell loot item
    &Spell::EffectMilling,                                  //158 SPELL_EFFECT_MILLING                  milling
    &Spell::EffectRenamePet,                                //159 SPELL_EFFECT_ALLOW_RENAME_PET         allow rename pet once again
    &Spell::EffectNULL,                                     //160 SPELL_EFFECT_160                      single spell: Nerub'ar Web Random Unit
    &Spell::EffectSpecCount,                                //161 SPELL_EFFECT_TALENT_SPEC_COUNT        second talent spec (learn/revert)
    &Spell::EffectActivateSpec,                             //162 SPELL_EFFECT_TALENT_SPEC_SELECT       activate primary/secondary spec
    &Spell::EffectUnused,                                   //163 unused in 3.3.5a
    &Spell::EffectCancelAura,                               //164 SPELL_EFFECT_CANCEL_AURA
    &Spell::EffectDamageFromMaxHealthPct,                   //165 SPELL_EFFECT_DAMAGE_FROM_MAX_HEALTH_PCT 82 spells in 4.3.4
    &Spell::EffectRewardCurrency,                           //166 SPELL_EFFECT_REWARD_CURRENCY          56 spells in 4.3.4
    &Spell::EffectNULL,                                     //167 SPELL_EFFECT_167                      42 spells in 4.3.4
    &Spell::EffectNULL,                                     //168 SPELL_EFFECT_168                      2 spells in 4.3.4 Allows give commands to controlled pet
    &Spell::EffectDestroyItem,                              //169 SPELL_EFFECT_DESTROY_ITEM             9 spells in 4.3.4 removes something
    &Spell::EffectNULL,                                     //170 SPELL_EFFECT_170                      70 spells in 4.3.4
    &Spell::EffectNULL,                                     //171 SPELL_EFFECT_171                      19 spells in 4.3.4 related to GO summon
    &Spell::EffectResurrectWithAura,                        //172 SPELL_EFFECT_RESURRECT_WITH_AURA      Mass Ressurection (Guild Perk)
    &Spell::EffectBuyGuildBankSlot,                         //173 SPELL_EFFECT_BUY_GUILD_BANKSLOT       4 spells in 4.3.4 basepoints - slot
    &Spell::EffectNULL,                                     //174 SPELL_EFFECT_174                      13 spells some sort of area aura apply effect
    &Spell::EffectUnused,                                   //175 SPELL_EFFECT_175                      unused in 4.3.4
    &Spell::EffectSanctuary,                                //176 SPELL_EFFECT_SANCTUARY_2              4 spells in 4.3.4
    &Spell::EffectNULL,                                     //177 SPELL_EFFECT_177                      2 spells in 4.3.4 Deluge(100757) and test spell
    &Spell::EffectUnused,                                   //178 SPELL_EFFECT_178                      unused in 4.3.4
    &Spell::EffectNULL,                                     //179 SPELL_EFFECT_179                      15 spells in 4.3.4
    &Spell::EffectUnused,                                   //180 SPELL_EFFECT_180                      unused in 4.3.4
    &Spell::EffectUnused,                                   //181 SPELL_EFFECT_181                      unused in 4.3.4
    &Spell::EffectNULL,                                     //182 SPELL_EFFECT_182                      3 spells 4.3.4
};

void Spell::DoSummonSpecialPets(SpellEffectEntry const * effect)
{
    if (!unitTarget)
        return;

    uint32 pet_entry = effect->EffectMiscValue;
    if (!pet_entry)
        return;

    CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(pet_entry);
    if (!cinfo)
    {
        sLog.outErrorDb("Spell::DoSummonSpecialPets: Creature entry %u does not exist but used in spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    uint32 level = m_caster->getLevel();
    int32 amount = damage > 0 ? damage : 1;

    if( amount > 10 )
        amount = 1;

    for(uint8 count = 0; count < amount; ++count)
    {
        Pet* spawnCreature = new Pet(SUMMON_PET);

        Map *map = m_caster->GetMap();
        uint32 pet_number = sObjectMgr.GeneratePetNumber();

        // Summon in dest location
        float x, y, z;
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            x = m_targets.m_destX;
            y = m_targets.m_destY;
            z = m_targets.m_destZ;
        }
        else
            m_caster->GetClosePoint(x, y, z, spawnCreature->GetObjectBoundingRadius());

        CreatureCreatePos pos (map, x, y, z, -m_caster->GetOrientation(), m_caster->GetPhaseMask());
        if (!spawnCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cinfo, pet_number))
        {
            sLog.outErrorDb("Spell::DoSummonSpecialPets:: can't creature creature entry %u for spell %u", pet_entry, m_spellInfo->Id);
            delete spawnCreature;
            return;
        }

        spawnCreature->SetRespawnCoord(pos);

        if (m_duration > 0)
            spawnCreature->SetDuration(m_duration);

        spawnCreature->SetOwnerGuid(m_caster->GetObjectGuid());
        spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        spawnCreature->setPowerType(POWER_MANA);
        spawnCreature->SetUInt32Value(UNIT_FIELD_FLAGS, 0);
        spawnCreature->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
        spawnCreature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
        spawnCreature->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
        spawnCreature->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
        spawnCreature->SetCreatorGuid(m_caster->GetObjectGuid());
        spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

        spawnCreature->setFaction(m_caster->getFaction());
        spawnCreature->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        if(m_caster->IsPvP())
            spawnCreature->SetPvP(true);

        spawnCreature->InitStatsForLevel(level, m_caster);
        spawnCreature->InitPetCreateSpells();
        spawnCreature->InitLevelupSpellsForLevel();
        spawnCreature->GetCharmInfo()->SetPetNumber(pet_number, false);
        m_caster->SetPet(spawnCreature);
        spawnCreature->AIM_Initialize();

        // Spirit Wolves
        if (pet_entry == 29264)
            spawnCreature->SetFollowAngle((count + 1) * 4 * PET_FOLLOW_ANGLE / amount);

        map->Add((Creature*)spawnCreature);
        
        if (count == 0 && m_caster->GetTypeId() == TYPEID_PLAYER)
            ((Player*)m_caster)->PetSpellInitialize();
    }
}

void Spell::EffectEmpty(SpellEffectEntry const* /*effect*/)
{
    // NOT NEED ANY IMPLEMENTATION CODE, EFFECT POSISBLE USED AS MARKER OR CLIENT INFORM
}

void Spell::EffectNULL(SpellEffectEntry const* /*effect*/)
{
    DEBUG_LOG("WORLD: Spell Effect DUMMY");
}

void Spell::EffectUnused(SpellEffectEntry const* /*effect*/)
{
    // NOT USED BY ANY SPELL OR USELESS OR IMPLEMENTED IN DIFFERENT WAY IN MANGOS
}

void Spell::EffectResurrectNew(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->isAlive())
        return;

    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(!unitTarget->IsInWorld())
        return;

    Player* pTarget = ((Player*)unitTarget);

    if(pTarget->isRessurectRequested())       // already have one active request
        return;

    uint32 health = damage;
    uint32 mana = effect->EffectMiscValue;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (((Player*)m_caster)->GetGuildId() == pTarget->GetGuildId())
            health = uint32(health * m_caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_RESURRECTED_HEALTH_BY_GUILD_MEMBER));
    }

    pTarget->setResurrectRequestData(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana, 0);
    SendResurrectRequest(pTarget);
}

void Spell::EffectInstaKill(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget || !unitTarget->isAlive())
        return;

    if (m_caster == unitTarget)                             // prevent interrupt message
        finish();

    WorldObject* caster = GetCastingObject();               // we need the original casting object

    WorldPacket data(SMSG_SPELLINSTAKILLLOG, (8+8+4));
    data << (caster && caster->GetTypeId() != TYPEID_GAMEOBJECT ? m_caster->GetObjectGuid() : ObjectGuid()); // Caster GUID
    data << unitTarget->GetObjectGuid();                    // Victim GUID
    data << uint32(m_spellInfo->Id);
    m_caster->SendMessageToSet(&data, true);

    m_caster->DealDamage(unitTarget, unitTarget->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
}

void Spell::EffectEnvironmentalDMG(SpellEffectEntry const* effect)
{
    uint32 absorb = 0;
    uint32 resist = 0;

    // Note: this hack with damage replace required until GO casting not implemented
    // environment damage spells already have around enemies targeting but this not help in case nonexistent GO casting support
    // currently each enemy selected explicitly and self cast damage, we prevent apply self casted spell bonuses/etc
    damage = effect->CalculateSimpleValue();

    m_caster->CalculateDamageAbsorbAndResist(m_caster, GetSpellSchoolMask(m_spellInfo), SPELL_DIRECT_DAMAGE, damage, &absorb, &resist, m_spellInfo);

    m_caster->SendSpellNonMeleeDamageLog(m_caster, m_spellInfo->Id, damage, GetSpellSchoolMask(m_spellInfo), absorb, resist, false, 0, false);
    if(m_caster->GetTypeId() == TYPEID_PLAYER)
        ((Player*)m_caster)->EnvironmentalDamage(DAMAGE_FIRE, damage);
}

void Spell::EffectSchoolDMG(SpellEffectEntry const* effect)
{
    if( unitTarget && unitTarget->isAlive())
    {
        SpellClassOptionsEntry const* classOptions = m_spellInfo->GetSpellClassOptions();

        switch(m_spellInfo->GetSpellFamilyName())
        {
            case SPELLFAMILY_GENERIC:
            {
                switch(m_spellInfo->Id)                     // better way to check unknown
                {
                    // Meteor like spells (divided damage to targets)
                    case 24340: case 26558: case 28884:     // Meteor
                    case 36837: case 38903: case 41276:     // Meteor
                    case 57467:                             // Meteor
                    case 26789:                             // Shard of the Fallen Star
                    case 31436:                             // Malevolent Cleave
                    case 35181:                             // Dive Bomb
                    case 40810: case 43267: case 43268:     // Saber Lash
                    case 42384:                             // Brutal Swipe
                    case 45150:                             // Meteor Slash
                    case 64422: case 64688:                 // Sonic Screech
                    case 70492: case 72505:                 // Ooze Eruption
                    case 71904:                             // Chaos Bane
                    case 71386:                             // Rimefang's Frost Breath
                    case 72624: case 72625:                 // Ooze Eruption
                    case 88942: case 95172:                 // Meteor Slash
                    case 96913: case 101007:                // Searing Shadows
                    case 105069: case 108094:               // Seething Hate
                    {
                        uint32 count = 0;
                        for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                            if(ihit->effectMask & (1<<effect->EffectIndex))
                                ++count;

                        damage /= count;                    // divide to all targets
                        break;
                    }
                    // AoE spells, which damage is reduced with distance from the initial hit point
                    case 51673:                 // Rocket Blast (SotA cannons)
                    case 52339:                 // Hurl Boulder (SotA Demolishers)
                    case 62311: case 64596:     // Cosmic Smash
                    case 62598: case 62937:     // Detonate
                    case 65279:                 // Lightning Nova
                    {
                        float distance = unitTarget->GetDistance2d(m_targets.m_destX, m_targets.m_destY);
                        damage *= exp(-distance / 15.0f);
                        break; 
                    }
                    // percent from health with min
                    case 25599:                             // Thundercrash
                    {
                        damage = unitTarget->GetHealth() / 2;
                        if(damage < 200)
                            damage = 200;
                        break;
                    }
                    // Mana Detonation
                    case 27820:
                    {
                        if (unitTarget == m_caster)
                            damage = 0;
                        else
                            damage = m_caster->GetMaxPower(POWER_MANA);
                        break;
                    }
                    case 28375:     // Decimate (Gluth encounter)
                    {
                        // leave only 5% HP
                        if (unitTarget)
                        {
                            // damage of this spell is very odd so we're setting HP manually
                            damage = 0;
                            unitTarget->SetHealthPercent(5.0f);
                        }
                    }
                    // percent max target health
                    case 29142:                             // Eyesore Blaster
                    case 35139:                             // Throw Boom's Doom
                    case 49882:                             // Leviroth Self-Impale
                    case 55269:                             // Deathly Stare
                    {
                        damage = damage * unitTarget->GetMaxHealth() / 100;
                        break;
                    }
                    // Thaddius' charges, don't deal dmg to units with the same charge but give them the buff:
                    // Positive Charge
                    case 28062:
                    {
                        // remove pet from damage and buff list
                        if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                        {
                              damage = 0;
                              break;
                        }
                        // If target is not (+) charged, then just deal dmg
                        if (!unitTarget->HasAura(28059))
                            break;

                        if (m_caster != unitTarget)
                            m_caster->CastSpell(m_caster, 29659, true);

                        damage = 0;
                        break;
                    }
                    // Negative Charge
                    case 28085:
                    {
                        // remove pet from damage and buff list
                        if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                        {
                              damage = 0;
                              break;
                        }
                        // If target is not (-) charged, then just deal dmg
                        if (!unitTarget->HasAura(28084))
                            break;

                        if (m_caster != unitTarget)
                            m_caster->CastSpell(m_caster, 29660, true);

                        damage = 0;
                        break;
                    }
                    // Cataclysmic Bolt
                    case 38441:
                    {
                        damage = unitTarget->GetMaxHealth() / 2;
                        break;
                    }
                    // Explode
                    case 47496:
                    {
                        if (SpellEffectIndex(effect->EffectIndex) == EFFECT_INDEX_1)
                        {
                            unitTarget->CastSpell(unitTarget, 43999, true);
                            unitTarget->SetDisplayId(25537 + urand(0, 3));
                            return;
                        }
                        break;
                    }
                    case 48292:                             // Dark Slash
                    {
                        damage = unitTarget->GetHealth() / 2;
                        break;
                    }
                    // Touch the Nightmare
                    case 50341:
                    {
                        if (SpellEffectIndex(effect->EffectIndex) == EFFECT_INDEX_2)
                            damage = int32(unitTarget->GetMaxHealth() * 0.3f);
                        break;
                    }
                    // Shatter (Krystallus)
                    case 50811:
                    case 61547:
                    {
                        float dist = unitTarget->GetDistance(m_caster);
                        float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));

                        damage = damage / radius * (radius - dist);
                        break;
                    }
                    case 51132: // Urom Clocking Bomb Damage
                    {
                        damage = m_caster->GetMaxHealth() - m_caster->GetHealth();
                        break;
                    }
                    // Flame Tsunami (Sartharion encounter)
                    case 57491:
                    {
                        // knock back only once
                        if (unitTarget->HasAura(60241))
                            return;

                        unitTarget->SetOrientation(m_caster->GetOrientation()+M_PI_F);
                        unitTarget->CastSpell(unitTarget, 60241, true);
                        break;
                    }
                    // Biting Cold
                    case 62188:
                    {
                        if (!unitTarget)
                            return;

                        // 200 * 2 ^ stack_amount
                        if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(62039))
                        {
                            damage = 200 << holder->GetStackAmount();
                        }
                        break;
                    }
                    // Tympanic Tantrum
                    case 62775:
                    {
                        damage = unitTarget->GetMaxHealth() / 10;
                        break;
                    }
                    // Hand of Rekoning (name not have typos ;) )
                    case 67485:
                        damage += uint32(0.5f * m_caster->GetTotalAttackPowerValue(BASE_ATTACK));
                        break;
                    //Magic Bane normal (Forge of Souls - Bronjahm)
                    case 68921:
                    //Magic Bane heroic (Forge of Souls - Bronjahm)
                    case 69049:
                    {
                        float dist = m_caster->GetDistance2d(unitTarget);
                        if (dist < 12.0f)
                            damage = 0.0f;
                        else
                        {
                            damage += uint32(unitTarget->GetMaxPower(POWER_MANA) / 2);
                            damage = std::min(damage, m_spellInfo->Id == 68921 ? 10000 : 15000);
                        }
                        break;
                    }
                    // Defile damage depending from scale.
                    case 72754:
                    case 73708:
                    case 73709:
                    case 73710:
                        damage = damage * m_caster->GetObjectScale();
                        break;
                    // Growling ooze puddle
                    case 70346:
                    case 72456:
                    case 72868:
                    case 72869:
                    {
                        float distance = unitTarget->GetDistance2d(m_caster); 
                        damage *= exp(-distance/(5.0f*m_caster->GetObjectScale()));
                        break;
                    }
                    // Bone Storm
                    case 69075:
                    case 70834:
                    case 70835:
                    case 70836:
                    {
                        float distance = unitTarget->GetDistance2d(m_caster);
                        damage *= exp(-distance/(27.5f));
                        break;
                    }
                    case 74607:
                    // SPELL_FIERY_COMBUSTION_EXPLODE - Ruby sanctum boss Halion,
                    // damage proportional number of mark (74567, dummy)
                    {
                        if (Aura* aura = m_caster->GetAura(74567, EFFECT_INDEX_0))
                        {
                            if (aura->GetStackAmount() > 0)
                                damage = 1000 * aura->GetStackAmount();
                            m_caster->RemoveAurasDueToSpell(74567);
                        }
                        else damage = 0;
                        break;
                    }
                    // Blade of Twilight
                    case 74769:
                    case 77844:
                    case 77845:
                    case 77846:
                    {
                        float distance = unitTarget->GetDistance2d(m_caster);
                        damage *= exp(-distance/(10.0f));
                        break;
                    }
                    case 74799:
                    // SPELL_SOUL_CONSUMPTION_EXPLODE - Ruby sanctum boss Halion,
                    // damage proportional number of mark (74795, dummy)
                    {
                        if (Aura* aura = m_caster->GetAura(74795, EFFECT_INDEX_0))
                        {
                            if (aura->GetStackAmount() > 0)
                                damage = 1000 * aura->GetStackAmount();
                            m_caster->RemoveAurasDueToSpell(74795);
                        }
                        else damage = 0;
                        break;
                    }
                    case 86704:                             // Ancient Fury
                    {
                        int32 stacks = 0;
                        // Ancient Power
                        if (SpellAuraHolder* holder = m_caster->GetSpellAuraHolder(86700, m_caster->GetObjectGuid()))
                            stacks = holder->GetStackAmount();

                        uint32 count = 0;
                        bool last = false;

                        for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                            if(ihit->effectMask & (1<<effect->EffectIndex))
                            {
                                ++count;
                                last = ihit->targetGUID == unitTarget->GetObjectGuid();
                            }

                        damage += damage * stacks;
                        damage /= count;                    // divide to all targets

                        // remove Ancient Power buff after last target is processed
                        if (last)
                            m_caster->RemoveAurasDueToSpell(86700);
                        break;
                    }
                    case 92601:                             // Detonate Mana
                    {
                        // Recaptured Mana
                        if (Aura* aura = m_caster->GetAura(92596, EFFECT_INDEX_0))
                            damage += aura->GetModifier()->m_amount;
                        break;
                    }
                }
                break;
            }
            case SPELLFAMILY_WARRIOR:
            {
                // Bloodthirst
                if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x40000000000))
                {
                    damage = uint32(damage * (m_caster->GetTotalAttackPowerValue(BASE_ATTACK)) / 100);
                }
                // Victory Rush
                else if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x10000000000))
                {
                    damage = uint32(damage * m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100);
                }
                // Revenge ${$m1+$AP*0.310} to ${$M1+$AP*0.310}
                else if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000400))
                {
                    m_caster->ModifyAuraState(AURA_STATE_DEFENSE, false);
                }
                // Shockwave ${$m3/100*$AP}
                else if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000800000000000))
                {
                    int32 pct = m_caster->CalculateSpellDamage(unitTarget, m_spellInfo, EFFECT_INDEX_2);
                    if (pct > 0)
                        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * pct / 100);
                    break;
                }
                // Thunder Clap
                else if (m_spellInfo->Id == 6343)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        // find Blood and Thunder
                        if (SpellEntry const * bat = ((Player*)m_caster)->GetKnownTalentRankById(10480))
                        {
                            // check if Blood and Thunder chance is already calculated
                            if (!m_currentBasePoints[EFFECT_INDEX_2])
                            {
                                m_currentBasePoints[EFFECT_INDEX_2] = 1;
                                m_currentBasePoints[EFFECT_INDEX_1] = 0;

                                if (roll_chance_i(bat->CalculateSimpleValue(EFFECT_INDEX_0)))
                                {
                                    for (TargetList::iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
                                    {
                                        if (Unit* unit = m_caster->GetMap()->GetUnit(itr->targetGUID))
                                            if (unit->GetSpellAuraHolder(94009, m_caster->GetObjectGuid()))
                                            {
                                                m_currentBasePoints[EFFECT_INDEX_1] = 1;
                                                break;
                                            }
                                    }
                                }
                            }

                            // check if we should cast rend
                            if (m_currentBasePoints[EFFECT_INDEX_1])
                                m_caster->CastSpell(unitTarget, 94009, true);
                        }
                    }
                }
                // Execute
                else if (m_spellInfo->Id == 5308)
                {
                    if (!unitTarget)
                        return;

                    // megai2: may be i'm crazy
                    if ((unitTarget->GetHealth() * 100 / unitTarget->GetMaxHealth() > 20))
                        return;

                    int32 rage = m_caster->GetPower(POWER_RAGE);

                    // up to max 20 rage cost
                    int32 maxRage = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1);
                    if (rage > maxRage * 10)
                        rage = maxRage * 10;

                    // ${10+$AP*0.437*$m1/100}
                    int32 apBonus = int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.437f);
                    int32 addBonus = int32(apBonus * rage / 100);
                    damage += apBonus + addBonus;

                    // Sudden Death
                    Unit::AuraList const& auras = m_caster->GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
                    for (Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        // Only Sudden Death have this SpellIconID with SPELL_AURA_PROC_TRIGGER_SPELL
                        if ((*itr)->GetSpellProto()->SpellIconID == 1989)
                        {
                            // saved rage top stored in next affect
                            uint32 lastrage = (*itr)->GetModifier()->m_amount * 10;
                            if (lastrage < rage)
                                rage -= lastrage;
                            break;
                        }
                    }

                    m_caster->ModifyPower(POWER_RAGE, -rage);
                    break;
                }
                break;
            }
            case SPELLFAMILY_MAGE:
            {
                switch (m_spellInfo->Id)
                {
                    case 11113:         // Blast Wave
                    {
                        // Search Improved Firestarter talent
                        int32 ifsChance = 0;

                        Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                        for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                        {
                            if ((*itr)->GetSpellProto()->SpellIconID == 37 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_MAGE &&
                                (*itr)->GetEffIndex() == EFFECT_INDEX_0)
                            {
                                ifsChance = (*itr)->GetModifier()->m_amount;
                                break;
                            }
                        }

                        if (roll_chance_i(ifsChance))
                        {
                            int32 count = 0;
                            bool needBreak = false;

                            // count affected targets, only for first affected
                            for (TargetList::iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
                            {
                                if ((itr->effectMask & (1 << effect->EffectIndex)) == 0)
                                    continue;

                                if (itr->targetGUID == unitTarget->GetObjectGuid() && count > 0)
                                {
                                    needBreak = true;
                                    break;
                                }

                                ++count;
                            }

                            if (needBreak)
                                break;

                            // cast Flame Strike
                            if (count >= 2)
                                m_caster->CastSpell(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, 2120, true);
                        }
                        break;
                    }
                    case 33395:         // Freeze
                    {
                        Unit* owner = m_caster->GetOwner();
                        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                            break;

                        bool needBreak = false;
                        // only first target
                        for (TargetList::iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
                        {
                            if ((itr->effectMask & (1 << effect->EffectIndex)) == 0)
                                continue;

                            if (itr->targetGUID != unitTarget->GetObjectGuid())
                                needBreak = true;

                            break;
                        }

                        if (needBreak)
                            break;

                        int32 chance = 0;
                        // Search Improved Freeze
                        Unit::AuraList const& mDummyAuras = owner->GetAurasByType(SPELL_AURA_DUMMY);
                        for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                        {
                            if ((*itr)->GetSpellProto()->SpellIconID == 94 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_MAGE &&
                                (*itr)->GetEffIndex() == EFFECT_INDEX_0)
                            {
                                chance = (*itr)->GetModifier()->m_amount;
                                break;
                            }
                        }

                        if (chance && roll_chance_i(chance))
                        {
                            // Fingers of Frost
                            owner->CastSpell(owner, 44544, true);
                            owner->CastSpell(owner, 44544, true);
                        }

                        break;
                    }
                }
                break;
            }
            case SPELLFAMILY_WARLOCK:
            {
                // Shadowflame
                if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0001000000000000))
                {
                    // Apply DOT part
                    switch(m_spellInfo->Id)
                    {
                        case 47897: m_caster->CastSpell(unitTarget, 47960, true); break;
                        case 61290: m_caster->CastSpell(unitTarget, 61291, true); break;
                        default:
                            ERROR_LOG("Spell::EffectDummy: Unhandeled Shadowflame spell rank %u",m_spellInfo->Id);
                        break;
                    }
                }
                // Shadow Bite
                else if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0040000000000000))
                {
                    Unit *owner = m_caster->GetOwner();
                    if (!owner)
                        break;

                    uint32 counter = 0;
                    Unit::AuraList const& dotAuras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraList::const_iterator itr = dotAuras.begin(); itr!=dotAuras.end(); ++itr)
                        if ((*itr)->GetCasterGuid() == owner->GetObjectGuid())
                            ++counter;

                    if (counter)
                        damage += (counter * owner->CalculateSpellDamage(unitTarget, m_spellInfo, EFFECT_INDEX_2) * damage) / 100.0f;
                }
                // Conflagrate
                else if (m_spellInfo->GetTargetAuraState() == AURA_STATE_CONFLAGRATE)
                {
                    Unit* unitTarget = m_targets.getUnitTarget();
                    if (!unitTarget)
                        break;

                    // Find Immoplate
                    SpellAuraHolder* im = unitTarget->GetSpellAuraHolder(348, m_caster->GetObjectGuid());
                    if (!im)
                        break;

                    // find req. aura for damage calculation
                    if (Aura* aura = im->GetAuraByEffectIndex(EFFECT_INDEX_2))
                    {
                        damage += int32(aura->GetModifier()->m_amount * aura->GetAuraMaxTicks() * m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100.0f);
                        damage = unitTarget->SpellDamageBonusTaken(m_caster, im->GetSpellProto(), damage, DOT);
                        break;
                    }
                }
                // Firebolt (imp)
                else if (m_spellInfo->Id == 3110)
                {
                    // search Burning Embers
                    Unit::AuraList const& dummyAuras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                    for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                        if ((*itr)->GetSpellProto()->SpellIconID == 5116 && (*itr)->GetEffIndex() == EFFECT_INDEX_0 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_WARLOCK)
                        {
                            int32 bp = int32(damage * (*itr)->GetModifier()->m_amount / 100.0f / 7);
                            m_caster->CastCustomSpell(unitTarget, 85421, &bp, NULL, NULL, true);
                            break;
                        }
                }
                // Seed of Corruption detonate
                else if (m_spellInfo->Id == 27285)
                {
                    if (Unit* caster = GetAffectiveCaster())
                        if (caster->soulburnMarker)           // marked as affected by Soulburn
                        {
                            caster->m_nextCustomSpellData.SetFlag(CUSTOM_SPELL_FLAG_NO_COST);
                            caster->CastSpell(unitTarget, 172, true);   // cast Corruption
                        }
                }
                // Shadow Cleave
                else if (m_spellInfo->Id == 50581)
                {
                    int32 mod = int32(m_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + SPELL_SCHOOL_SHADOW) - m_caster->GetUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + SPELL_SCHOOL_SHADOW));
                    damage += mod > 0 ? int32(mod * 0.2143f) : 0;
                }
                // Soul Swap
                else if (m_spellInfo->Id == 86121)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* pCaster = (Player*)m_caster;
                        pCaster->m_soulSwapData.spells.clear();
                        pCaster->m_soulSwapData.swapTarget = unitTarget->GetObjectGuid();

                        if (SpellEntry const * affecter = sSpellStore.LookupEntry(92794))
                        {
                            if (SpellEffectEntry const* effect = affecter->GetSpellEffect(EFFECT_INDEX_0))
                            {
                                Unit::SpellAuraHolderMap const& holders = unitTarget->GetSpellAuraHolderMap();
                                for (Unit::SpellAuraHolderMap::const_iterator itr = holders.begin(); itr != holders.end(); ++itr)
                                {
                                    if (itr->second->IsDeleted() ||
                                        itr->second->IsPositive() ||
                                        itr->second->IsPassive() ||
                                        itr->second->GetCasterGuid() != m_caster->GetObjectGuid())
                                        continue;

                                    bool isPeriodic = false;
                                    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                                    {
                                        if (Aura* aura = itr->second->GetAuraByEffectIndex(SpellEffectIndex(i)))
                                        {
                                            if (aura->IsPeriodic())
                                            {
                                                isPeriodic = true;
                                                break;
                                            }
                                        }
                                    }

                                    if (!isPeriodic)
                                        continue;

                                    if (itr->second->GetSpellProto()->IsFitToFamily(SPELLFAMILY_WARLOCK, effect->EffectSpellClassMask))
                                        pCaster->m_soulSwapData.spells.push_back(itr->second->GetId());
                                }
                            }
                        }

                        // Glyph of Soul Swap
                        if (m_caster->HasAura(56226))
                            m_caster->CastSpell(m_caster, 94229, true);
                        else
                        {
                            for (uint32 i = 0; i < pCaster->m_soulSwapData.spells.size(); ++i)
                                unitTarget->RemoveAurasByCasterSpell(pCaster->m_soulSwapData.spells[i], m_caster->GetObjectGuid());
                        }

                        // aura 332 spell
                        m_caster->CastSpell(m_caster, 86211, true);
                    }
                }
                // Soul Swap Exhale
                else if (m_spellInfo->Id == 86213)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* pCaster = (Player*)m_caster;
                        for (uint32 i = 0; i < pCaster->m_soulSwapData.spells.size(); ++i)
                            m_caster->CastSpell(unitTarget, pCaster->m_soulSwapData.spells[i], true);

                        m_caster->RemoveAurasDueToSpell(86211);
                    }
                }
                break;
            }
            case SPELLFAMILY_PRIEST:
            {
                // Shadow Word: Death - deals damage equal to damage done to caster
                if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000200000000))
                {
                    if (unitTarget->GetHealthPercent() <= 25.0f)
                    {
                        // three times as much damage
                        damage *= 3;

                        // Search Mind Melt
                        Unit::AuraList const& mindMelt = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                        for (Unit::AuraList::const_iterator i = mindMelt.begin(); i != mindMelt.end(); ++i)
                        {
                            if ((*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_PRIEST &&
                                ((*i)->GetSpellProto()->SpellIconID == 3139))
                            {
                                damage += int32(damage * (*i)->GetModifier()->m_amount / 100.0f);
                                break;
                            }
                        }
                    }

                    int32 back_damage = m_caster->SpellDamageBonusDone(unitTarget,m_spellInfo,uint32(damage),SPELL_DIRECT_DAMAGE);
                    // Item - Priest T13 Shadow 2P Bonus (Shadow Word: Death)
                    if (Aura* aura = m_caster->GetAura(105843, EFFECT_INDEX_1))
                        back_damage = int32(back_damage * (100.0f - aura->GetModifier()->m_amount) / 100.0f);
                    m_caster->CastCustomSpell(m_caster, 32409, &back_damage, 0, 0, true);
                }
                // Improved Mind Blast (Mind Blast in shadow form bonus)
                else if (m_caster->GetShapeshiftForm() == FORM_SHADOW && (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x00002000)))
                {
                    Unit::AuraList const& ImprMindBlast = m_caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
                    for (Unit::AuraList::const_iterator i = ImprMindBlast.begin(); i != ImprMindBlast.end(); ++i)
                    {
                        if ((*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_PRIEST &&
                            ((*i)->GetSpellProto()->SpellIconID == 95))
                        {
                            int chance = (*i)->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1);
                            if (roll_chance_i(chance))
                                // Mind Trauma
                                m_caster->CastSpell(unitTarget, 48301, true);
                            break;
                        }
                    }
                }
                // Mind Spike
                else if (m_spellInfo->Id == 73510)
                {
                    if (!unitTarget)
                        return;

                    std::set<uint32> toRemoveSpellList;
                    Unit::SpellAuraHolderMap const& holders = unitTarget->GetSpellAuraHolderMap();
                    for (Unit::SpellAuraHolderMap::const_iterator itr = holders.begin(); itr != holders.end(); ++itr)
                    {
                        if (itr->second->IsPositive() ||
                            itr->second->IsPassive() ||
                            itr->second->IsDeathPersistent())
                            continue;

                        if (itr->second->GetCasterGuid() != m_caster->GetObjectGuid())
                            continue;

                        SpellEntry const * spellProto = itr->second->GetSpellProto();

                        if ((GetSpellSchoolMask(spellProto) & SPELL_SCHOOL_MASK_SHADOW) == 0)
                            continue;

                        // Except Shadow Word: Death periodic
                        if (IsSpellHaveAura(spellProto, SPELL_AURA_PERIODIC_DAMAGE) && spellProto->Id != 32409 ||
                            IsSpellHaveAura(spellProto, SPELL_AURA_PERIODIC_DAMAGE_PERCENT) ||
                            IsSpellHaveAura(spellProto, SPELL_AURA_PERIODIC_LEECH))
                            toRemoveSpellList.insert(spellProto->Id);
                    }

                    for (std::set<uint32>::iterator i = toRemoveSpellList.begin(); i != toRemoveSpellList.end(); ++i)
                        unitTarget->RemoveAurasByCasterSpell(*i, m_caster->GetObjectGuid());
                }

                // Mind Blast or Mind Spike
                if (m_spellInfo->Id == 8092 || m_spellInfo->Id == 73510)
                {
                    // Shadow Orb dummy buff
                    if (SpellAuraHolder* orb = m_caster->GetSpellAuraHolder(77487, m_caster->GetObjectGuid()))
                    {
                        int32 mod = 10;
                        if (SpellAuraHolder* talent = m_caster->GetSpellAuraHolder(77486, m_caster->GetObjectGuid()))
                            if (Aura* aur = talent->GetAuraByEffectIndex(EFFECT_INDEX_0))
                                mod += aur->GetModifier()->m_amount;
                        // Empowered Shadow
                        m_caster->CastCustomSpell(m_caster, 95799, &mod, &mod, NULL, true);
                        mod *= orb->GetStackAmount();
                        damage = int32(damage * (100.0f + mod) / 100.0f);

                        m_caster->RemoveSpellAuraHolder(orb);
                    }
                }
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                SpellEffectEntry const* rakeSpellEffect = m_spellInfo->GetSpellEffect(EFFECT_INDEX_2);
                // Ferocious Bite
                if (classOptions && (classOptions->SpellFamilyFlags & UI64LIT(0x000800000)) && m_spellInfo->SpellVisual[0] == 6587)
                {
                    // converts up to 35 points to up to 100% damage
                    if (int32 energy = std::min(m_caster->GetPower(POWER_ENERGY), 35))
                    {
                        // Glyph of Ferocious Bite
                        if (Aura* glyph = m_caster->GetAura(67598, EFFECT_INDEX_0))
                            if (int32 bp = glyph->GetModifier()->m_amount * energy / 10)
                                m_caster->CastCustomSpell(m_caster, 101024, &bp, NULL, NULL, true);

                        damage += int32(damage * energy / 35);

                        m_caster->ModifyPower(POWER_ENERGY, -energy);
                    }
                }

                // Maul
                if (m_spellInfo->Id == 6807)
                {
                    // Maul does 1/2 damage to secondary target (Glyph of Maul)
                    if (m_targets.getUnitTarget() != unitTarget)
                        damage /= 2;
                }
                break;
            }
            case SPELLFAMILY_ROGUE:
            {
                // Envenom
                if (m_caster->GetTypeId()==TYPEID_PLAYER && (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x800000000)))
                {
                    // consume from stack dozes not more that have combo-points
                    if(uint32 combo = m_caster->GetComboPoints())
                    {
                        Aura *poison = 0;
                        // Lookup for Deadly poison (only attacker applied)
                        Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                        for(Unit::AuraList::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
                        {
                            SpellClassOptionsEntry const* poisonClassOptions = (*itr)->GetSpellProto()->GetSpellClassOptions();
                            if(!poisonClassOptions)
                                continue;
                            if( poisonClassOptions->SpellFamilyName==SPELLFAMILY_ROGUE &&
                                (poisonClassOptions->SpellFamilyFlags & UI64LIT(0x10000)) &&
                                (*itr)->GetCasterGuid() == m_caster->GetObjectGuid())
                            {
                                poison = *itr;
                                break;
                            }
                        }
                        // count consumed deadly poison doses at target
                        if (poison)
                        {
                            bool needConsume = true;
                            uint32 spellId = poison->GetId();
                            uint32 doses = poison->GetStackAmount();
                            if (doses > combo)
                                doses = combo;

                            // Master Poisoner
                            if (!m_caster->HasAura(58410))
                                unitTarget->RemoveAuraHolderFromStack(spellId, doses, m_caster->GetObjectGuid());

                            damage *= doses;
                            damage += int32(((Player*)m_caster)->GetTotalAttackPowerValue(BASE_ATTACK) * 0.09f * doses);
                        }
                        // Eviscerate and Envenom Bonus Damage (item set effect)
                        if (Aura* aura = m_caster->GetDummyAura(37169))
                            damage += combo * aura->GetModifier()->m_amount;

                        float pctBonus = 1.0f;
                        // Vile Poisons
                        Unit::AuraList const& mPctModifierAuras = m_caster->GetAurasByType(SPELL_AURA_ADD_PCT_MODIFIER);
                        for (Unit::AuraList::const_iterator i = mPctModifierAuras.begin(); i != mPctModifierAuras.end(); ++i)
                        {
                            if ((*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_ROGUE && (*i)->GetSpellProto()->SpellIconID == 857 && (*i)->GetEffIndex() == EFFECT_INDEX_0)
                            {
                                pctBonus *= (100.0f + (*i)->GetModifier()->m_amount) / 100;
                                break;
                            }
                        }

                        damage = int32(pctBonus * damage);
                    }
                }
                // Eviscerate
                else if ((classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x00020000)) && m_caster->GetTypeId()==TYPEID_PLAYER)
                {
                    if (uint32 combo = m_caster->GetComboPoints())
                    {
                        float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
                        damage += irand(int32(ap * combo * 0.03f), int32(ap * combo * 0.07f));

                        // Eviscerate and Envenom Bonus Damage (item set effect)
                        if (Aura* aura = m_caster->GetDummyAura(37169))
                            damage += combo * aura->GetModifier()->m_amount;

                        Unit::AuraList const& dummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                        for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                        {
                            // Serrated Blades
                            if ((*itr)->GetSpellProto()->SpellIconID == 2004 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_ROGUE)
                            {
                                if (roll_chance_i((*itr)->GetModifier()->m_amount * combo))
                                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(1943, m_caster->GetObjectGuid()))
                                        holder->RefreshHolder();
                                break;
                            }
                        }

                        // Apply spell mods
                        if (Player* modOwner = m_caster->GetSpellModOwner())
                            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_DAMAGE, damage);
                    }
                }
                // Thrash (Raise ally ghoul spell)
                else if (m_spellInfo->Id == 47480)
                {
                    if (Aura* aura = m_caster->GetAura(62218, EFFECT_INDEX_0))
                    {
                        if (aura->GetStackAmount() > 0)
                            damage += m_caster->GetTotalAttackPowerValue(BASE_ATTACK) *
                                     m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0)/100 +
                                     m_caster->GetTotalAttackPowerValue(BASE_ATTACK) /10 *
                                     aura->GetStackAmount();
                        m_caster->RemoveAurasDueToSpell(62218);

                        if (Unit* owner = m_caster->GetCharmerOrOwner())
                           owner->RemoveAurasDueToSpell(62218);
                    }
                    else
                        damage += m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0)/100;
                }
                break;
            }
            case SPELLFAMILY_HUNTER:
            {
                //Gore
                if (m_spellInfo->SpellIconID == 1578)
                {
                    if (m_caster->HasAura(57627))           // Charge 6 sec post-affect
                        damage *= 2;
                }
                // Kill Command
                if (m_spellInfo->Id == 83381)
                {
                    float rap = m_caster->GetTotalAttackPowerValue(RANGED_ATTACK);
                    damage = int32((damage + (rap * 0.516f)));
                }
                break;
            }
            case SPELLFAMILY_PALADIN:
            {
                // Exorcism
                if (m_spellInfo->Id == 879)
                {
                    damage += int32(0.344f * std::max(int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)), m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo))));
                }
                // Judgement of Righteousness - receive benefit from Spell Damage and Attack power
                // ${1+0.2*$AP+0.32*$SPH}
                else if (m_spellInfo->Id == 20187)
                {
                    float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo));
                    if (holy < 0)
                        holy = 0;
                    damage = 1 + int32(ap * 0.20f) + int32(holy * 0.32f);
                }
                // Judgement of Truth ${1+0.223*$SPH+0.142*$AP} + 20% for each application of Censure on the target
                else if (m_spellInfo->Id == 31804)
                {
                    float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo));
                    if (holy < 0)
                        holy = 0;
                    damage = 1 + int32(ap * 0.142f) + int32(holy * 0.223f);
                    // Get stack of Holy Vengeance on the target added by caster

                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(31803, m_caster->GetObjectGuid()))
                        if (uint32 stacks = holder->GetStackAmount())
                            // + 20% for each application of Censure on the target
                            damage += damage * stacks * 20 /100;
                }
                // Avenger's Shield ($m1+0.21*$SPH+0.419*$AP)
                else if (m_spellInfo->Id == 31935)
                {
                    float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo));
                    if (holy < 0)
                        holy = 0;
                    damage += int32(ap * 0.419f) + int32(0.21 * holy);
                }
                // Hammer of Wrath ($m1+0.15*$SPH+0.15*$AP) - ranged type sdb future fix
                else if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000008000000000))
                {
                    float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo));
                    if (holy < 0)
                        holy = 0;
                    damage += int32(ap * 0.15f) + int32(holy * 15 / 100);
                }
                // Shield of the Righteous
                else if (m_spellInfo->Id == 53600)
                {
                    switch (m_usedHolyPower)
                    {
                        case 1:
                            damage -= 1;
                            break;
                        case 2:
                            damage = damage * 3 - 3;
                            break;
                        case 3:
                            damage = damage * 6 - 6;
                            break;
                    }
                    break;
                }
                // Judgement
                else if (m_spellInfo->Id == 54158)
                {
                    // ${1+0.25*$SPH+0.16*$AP}
                    float ap = m_caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo));
                    if (holy < 0)
                        holy = 0;
                    damage = 1 + int32(ap * 0.16f) + int32(holy * 0.25f);
                }
                break;
            }
            case SPELLFAMILY_DEATHKNIGHT:
            {
                // Blood Boil - bonus for diseased targets or with Burning Blood
                if (m_spellInfo->IsFitToFamilyMask(0x00040000) && (unitTarget->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DEATHKNIGHT, 0, 0x00000002, m_caster->GetObjectGuid()) || unitTarget->GetSpellAuraHolder(98957, m_caster->GetObjectGuid())))
                {
                    damage += damage / 2;
                    damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK)* 0.035f);
                }
                // Chains of Ice
                else if (m_spellInfo->Id == 45524)
                {
                    // Glyph of Chains of Ice
                    if (m_caster->HasAura(58620))
                        damage += int32(m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 0.08f);
                    break;
                }
                // Howling Blast
                else if (m_spellInfo->Id == 49143)
                {
                    // half damage to secondary targets
                    if (unitTarget != m_targets.getUnitTarget())
                        damage /= 2;
                }
                break;
            }
            case SPELLFAMILY_SHAMAN:
            {
                // Attack (Searing Totem effect)
                if (unitTarget && m_spellInfo->Id == 22048)
                {
                    if (Unit* owner = m_caster->GetOwner())
                    {
                        if (owner->GetTypeId() == TYPEID_PLAYER)
                        {
                            Player* plrOwner = (Player*)owner;
                            // Searing Flames
                            if (SpellEntry const * spellInfo = plrOwner->GetKnownTalentRankById(2083))
                            {
                                if (roll_chance_i(spellInfo->CalculateSimpleValue(EFFECT_INDEX_0)))
                                {
                                    int32 bp = damage >= 0 ? m_damage + damage : damage;
                                    // Cast Searing Flames
                                    m_caster->CastCustomSpell(unitTarget, 77661, &bp, NULL, NULL, true, NULL, NULL, owner->GetObjectGuid());
                                }
                            }
                        }
                    }
                }
                break;
            }
        }

        if(damage >= 0)
            m_damage += damage;
    }
}

void Spell::EffectDummy(SpellEffectEntry const* effect)
{
    if (!unitTarget && !gameObjTarget && !itemTarget)
        return;

    // selection by spell family
    switch(m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id)
            {
                case 3360:                                  // Curse of the Eye
                {
                    if(!unitTarget)
                        return;

                    uint32 spell_id = (unitTarget->getGender() == GENDER_MALE) ? 10651: 10653;

                    m_caster->CastSpell(unitTarget, spell_id, true);
                    return;
                }
                case 7671:                                  // Transformation (human<->worgen)
                {
                    if (!unitTarget)
                        return;

                    // Transform Visual
                    unitTarget->CastSpell(unitTarget, 24085, true);
                    return;
                }
                case 8063:                                  // Deviate Fish
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1,5))
                    {
                        case 1: spell_id = 8064; break;     // Sleepy
                        case 2: spell_id = 8065; break;     // Invigorate
                        case 3: spell_id = 8066; break;     // Shrink
                        case 4: spell_id = 8067; break;     // Party Time!
                        case 5: spell_id = 8068; break;     // Healthy Spirit
                    }
                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
                case 8213:                                  // Savory Deviate Delight
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1,2))
                    {
                        // Flip Out - ninja
                        case 1: spell_id = (m_caster->getGender() == GENDER_MALE ? 8219 : 8220); break;
                        // Yaaarrrr - pirate
                        case 2: spell_id = (m_caster->getGender() == GENDER_MALE ? 8221 : 8222); break;
                    }

                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 9976:                                  // Polly Eats the E.C.A.C.
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Summon Polly Jr.
                    unitTarget->CastSpell(unitTarget, 9998, true);

                    ((Creature*)unitTarget)->ForcedDespawn(100);
                    return;
                }
                case 10254:                                 // Stone Dwarf Awaken Visual
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    // see spell 10255 (aura dummy)
                    m_caster->clearUnitState(UNIT_STAT_ROOT);
                    m_caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    return;
                }
                case 13120:                                 // net-o-matic
                {
                    if (!unitTarget)
                        return;

                    uint32 spell_id = 0;

                    uint32 roll = urand(0, 99);

                    if (roll < 2)                           // 2% for 30 sec self root (off-like chance unknown)
                        spell_id = 16566;
                    else if (roll < 4)                      // 2% for 20 sec root, charge to target (off-like chance unknown)
                        spell_id = 13119;
                    else                                    // normal root
                        spell_id = 13099;

                    m_caster->CastSpell(unitTarget,spell_id,true,NULL);
                    return;
                }
                case 13280:                                 // Gnomish Death Ray
                {
                    if (!unitTarget)
                        return;

                    if (urand(0, 99) < 15)
                        m_caster->CastSpell(m_caster, 13493, true, NULL);
                    else
                        m_caster->CastSpell(unitTarget, 13279, true, NULL);

                    return;
                }
                case 13567:                                 // Dummy Trigger
                {
                    // can be used for different aura triggering, so select by aura
                    if (!m_triggeredByAuraSpell || !unitTarget)
                        return;

                    switch(m_triggeredByAuraSpell->Id)
                    {
                        case 26467:                         // Persistent Shield
                            m_caster->CastCustomSpell(unitTarget, 26470, &damage, NULL, NULL, true);
                            break;
                        default:
                            ERROR_LOG("EffectDummy: Non-handled case for spell 13567 for triggered aura %u",m_triggeredByAuraSpell->Id);
                            break;
                    }
                    return;
                }
                case 14537:                                 // Six Demon Bag
                {
                    if (!unitTarget)
                        return;

                    Unit* newTarget = unitTarget;
                    uint32 spell_id = 0;
                    uint32 roll = urand(0, 99);
                    if (roll < 25)                          // Fireball (25% chance)
                        spell_id = 15662;
                    else if (roll < 50)                     // Frostbolt (25% chance)
                        spell_id = 11538;
                    else if (roll < 70)                     // Chain Lighting (20% chance)
                        spell_id = 21179;
                    else if (roll < 77)                     // Polymorph (10% chance, 7% to target)
                        spell_id = 14621;
                    else if (roll < 80)                     // Polymorph (10% chance, 3% to self, backfire)
                    {
                        spell_id = 14621;
                        newTarget = m_caster;
                    }
                    else if (roll < 95)                     // Enveloping Winds (15% chance)
                        spell_id = 25189;
                    else                                    // Summon Felhund minion (5% chance)
                    {
                        spell_id = 14642;
                        newTarget = m_caster;
                    }

                    m_caster->CastSpell(newTarget, spell_id, true, m_CastItem);
                    return;
                }
                case 15998:                                 // Capture Worg Pup
                case 29435:                                 // Capture Female Kaliri Hatchling
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (m_spellInfo->Id == 29435 && unitTarget->GetEntry() != 17034) // Female Kaliri
                        return;
                    else
                    if (m_spellInfo->Id == 15998 && unitTarget->GetEntry() != 10221) // Bloodaxe Worg Pup
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;

                    creatureTarget->ForcedDespawn();
                    return;
                }
                case 16589:                                 // Noggenfogger Elixir
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1, 3))
                    {
                        case 1: spell_id = 16595; break;
                        case 2: spell_id = 16593; break;
                        default:spell_id = 16591; break;
                    }

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
                case 17251:                                 // Spirit Healer Res
                {
                    if (!unitTarget)
                        return;

                    Unit* caster = GetAffectiveCaster();

                    if (caster && caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        WorldPacket data(SMSG_SPIRIT_HEALER_CONFIRM, 8);
                        data << unitTarget->GetObjectGuid();
                        ((Player*)caster)->GetSession()->SendPacket( &data );
                    }
                    return;
                }
                case 17271:                                 // Test Fetid Skull
                {
                    if (!itemTarget && m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    uint32 spell_id = roll_chance_i(50)
                        ? 17269                             // Create Resonating Skull
                        : 17270;                            // Create Bone Dust

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
                case 17770:                                 // Wolfshead Helm Energy
                {
                    m_caster->CastSpell(m_caster, 29940, true, NULL);
                    return;
                }
                case 17950:                                 // Shadow Portal
                {
                    if (!unitTarget)
                        return;

                    // Shadow Portal
                    const uint32 spell_list[6] = {17863, 17939, 17943, 17944, 17946, 17948};

                    m_caster->CastSpell(unitTarget, spell_list[urand(0, 5)], true);
                    return;
                }
                case 19411:                                 // Lava Bomb
                case 20474:                                 // Lava Bomb
                {
                    if (!unitTarget)
                        return;

                    // Hack alert!
                    // This dummy are expected to cast spell 20494 to summon GO entry 177704
                    // Spell does not exist client side, so we have to make a hack, creating the GO (SPELL_EFFECT_SUMMON_OBJECT_WILD)
                    // Spell should appear in both SMSG_SPELL_START/GO and SMSG_SPELLLOGEXECUTE

                    // For later, creating custom spell
                    // _START: packguid: target, cast flags: 0xB, TARGET_FLAG_NONE
                    // _GO: packGuid: target, cast flags: 0x4309, TARGET_FLAG_DEST_LOCATION
                    // LOG: spell: 20494, effect, pguid: goguid

                    GameObject* pGameObj = new GameObject;

                    Map *map = unitTarget->GetMap();

                    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), 177704,
                        map, m_caster->GetPhaseMask(),
                        unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(),
                        unitTarget->GetOrientation()))
                    {
                        delete pGameObj;
                        return;
                    }

                    DEBUG_LOG("Gameobject, create custom in SpellEffects.cpp EffectDummy");

                    // Expect created without owner, but with level from _template
                    pGameObj->SetRespawnTime(MINUTE/2);
                    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, pGameObj->GetGOInfo()->trap.level);
                    pGameObj->SetSpellId(m_spellInfo->Id);

                    map->Add(pGameObj);

                    return;
                }
                case 20577:                                 // Cannibalize
                {
                    if (unitTarget)
                        m_caster->CastSpell(m_caster, 20578, false, NULL);

                    return;
                }
                case 21147:                                 // Arcane Vacuum
                case 58694:                                 // Arcane Vacuum (Cyanigosa)
                {
                    if (!unitTarget)
                        return;

                    // Spell used by Azuregos to teleport all the players to him
                    // This also resets the target threat
                    if (m_caster->getThreatManager().getThreat(unitTarget))
                        m_caster->getThreatManager().modifyThreatPercent(unitTarget, -100);

                    // cast summon player
                    m_caster->CastSpell(unitTarget, 21150, true);

                    return;
                }
                case 23019:                                 // Crystal Prison Dummy DND
                {
                    if (!unitTarget || !unitTarget->isAlive() || unitTarget->GetTypeId() != TYPEID_UNIT || ((Creature*)unitTarget)->IsPet())
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;
                    if (creatureTarget->IsPet())
                        return;

                    GameObject* pGameObj = new GameObject;

                    Map *map = creatureTarget->GetMap();

                    // create before death for get proper coordinates
                    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), 179644, map, m_caster->GetPhaseMask(),
                        creatureTarget->GetPositionX(), creatureTarget->GetPositionY(), creatureTarget->GetPositionZ(),
                        creatureTarget->GetOrientation()) )
                    {
                        delete pGameObj;
                        return;
                    }

                    pGameObj->SetRespawnTime(creatureTarget->GetRespawnTime()-time(NULL));
                    pGameObj->SetOwnerGuid(m_caster->GetObjectGuid() );
                    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel() );
                    pGameObj->SetSpellId(m_spellInfo->Id);

                    creatureTarget->ForcedDespawn();

                    DEBUG_LOG("AddObject at SpellEfects.cpp EffectDummy");
                    map->Add(pGameObj);

                    return;
                }
                case 23074:                                 // Arcanite Dragonling
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 19804, true, m_CastItem);
                    return;
                }
                case 23075:                                 // Mithril Mechanical Dragonling
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 12749, true, m_CastItem);
                    return;
                }
                case 23076:                                 // Mechanical Dragonling
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 4073, true, m_CastItem);
                    return;
                }
                case 23133:                                 // Gnomish Battle Chicken
                {
                    if (!m_CastItem)
                        return;

                    m_caster->CastSpell(m_caster, 13166, true, m_CastItem);
                    return;
                }
                case 23138:                                 // Gate of Shazzrah
                {
                    if (!unitTarget)
                        return;

                    // Effect probably include a threat change, but it is unclear if fully
                    // reset or just forced upon target for teleport (SMSG_HIGHEST_THREAT_UPDATE)

                    // Gate of Shazzrah
                    m_caster->CastSpell(unitTarget, 23139, true);
                    return;
                }
                case 23448:                                 // Transporter Arrival - Ultrasafe Transporter: Gadgetzan - backfires
                {
                    int32 r = irand(0, 119);
                    if (r < 20)                             // Transporter Malfunction - 1/6 polymorph
                        m_caster->CastSpell(m_caster, 23444, true);
                    else if (r < 100)                       // Evil Twin               - 4/6 evil twin
                        m_caster->CastSpell(m_caster, 23445, true);
                    else                                    // Transporter Malfunction - 1/6 miss the target
                        m_caster->CastSpell(m_caster, 36902, true);

                    return;
                }
                case 23453:                                 // Gnomish Transporter - Ultrasafe Transporter: Gadgetzan
                {
                    if (roll_chance_i(50))                  // Gadgetzan Transporter         - success
                        m_caster->CastSpell(m_caster, 23441, true);
                    else                                    // Gadgetzan Transporter Failure - failure
                        m_caster->CastSpell(m_caster, 23446, true);

                    return;
                }
                case 23645:                                 // Hourglass Sand
                    m_caster->RemoveAurasDueToSpell(23170); // Brood Affliction: Bronze
                    return;
                case 23725:                                 // Gift of Life (warrior bwl trinket)
                    m_caster->CastSpell(m_caster, 23782, true);
                    m_caster->CastSpell(m_caster, 23783, true);
                    return;
                case 24930:                                 // Hallow's End Treat
                {
                    uint32 spell_id = 0;

                    switch(urand(1,4))
                    {
                        case 1: spell_id = 24924; break;    // Larger and Orange
                        case 2: spell_id = 24925; break;    // Skeleton
                        case 3: spell_id = 24926; break;    // Pirate
                        case 4: spell_id = 24927; break;    // Ghost
                    }

                    m_caster->CastSpell(m_caster, spell_id, true);
                    return;
                }
                case 25860:                                 // Reindeer Transformation
                {
                    if (!m_caster->HasAuraType(SPELL_AURA_MOUNTED))
                        return;

                    float flyspeed = m_caster->GetSpeedRate(MOVE_FLIGHT);
                    float speed = m_caster->GetSpeedRate(MOVE_RUN);

                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    // 5 different spells used depending on mounted speed and if mount can fly or not
                    if (flyspeed >= 4.1f)
                        // Flying Reindeer
                        m_caster->CastSpell(m_caster, 44827, true); // 310% flying Reindeer
                    else if (flyspeed >= 3.8f)
                        // Flying Reindeer
                        m_caster->CastSpell(m_caster, 44825, true); // 280% flying Reindeer
                    else if (flyspeed >= 1.6f)
                        // Flying Reindeer
                        m_caster->CastSpell(m_caster, 44824, true); // 60% flying Reindeer
                    else if (speed >= 2.0f)
                        // Reindeer
                        m_caster->CastSpell(m_caster, 25859, true); // 100% ground Reindeer
                    else
                        // Reindeer
                        m_caster->CastSpell(m_caster, 25858, true); // 60% ground Reindeer

                    return;
                }
                case 26074:                                 // Holiday Cheer
                    // implemented at client side
                    return;
                case 26374:                                 // Elune's Candle
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->GetEntry() == 15467)
                    {
                        uint32 spellIds[5] = {26622, 26623, 26624, 26625, 26649};
                        m_caster->CastSpell(unitTarget, spellIds[urand(0,4)], true);
                    }
                    else
                        m_caster->CastSpell(unitTarget, 26636, true);
                    return;
                }
                case 28006:                                 // Arcane Cloaking
                {
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER )
                        // Naxxramas Entry Flag Effect DND
                        m_caster->CastSpell(unitTarget, 29294, true);

                    return;
                }
                case 28089:                                 // Polarity Shift (Thaddius - Naxxramas)
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // neutralize the target
                    if (unitTarget->HasAura(28059)) unitTarget->RemoveAurasDueToSpell(28059);
                    if (unitTarget->HasAura(29659)) unitTarget->RemoveAurasDueToSpell(29659);
                    if (unitTarget->HasAura(28084)) unitTarget->RemoveAurasDueToSpell(28084);
                    if (unitTarget->HasAura(29660)) unitTarget->RemoveAurasDueToSpell(29660);

                    unitTarget->CastSpell(unitTarget, roll_chance_i(50) ? 28059 : 28084, true, 0, 0, m_caster->GetObjectGuid());
                    break;
                }
                case 29126:                                 // Cleansing Flames (Darnassus)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(unitTarget, 29099, true);
                    return;
                }
                case 29135:                                 // Cleansing Flames (Ironforge)
                case 29136:                                 // Cleansing Flames (Orgrimmar)
                case 29137:                                 // Cleansing Flames (Stormwind)
                case 29138:                                 // Cleansing Flames (Thunder Bluff)
                case 29139:                                 // Cleansing Flames (Undercity)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spellIDs[] = {29102, 29130, 29101, 29132, 29133};
                    unitTarget->CastSpell(unitTarget, spellIDs[m_spellInfo->Id - 29135], true);
                    return;
                }
                case 29200:                                 // Purify Helboar Meat
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = roll_chance_i(50)
                        ? 29277                             // Summon Purified Helboar Meat
                        : 29278;                            // Summon Toxic Helboar Meat

                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 29858:                                 // Soulshatter
                {
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->IsHostileTo(m_caster))
                        m_caster->CastSpell(unitTarget,32835,true);

                    return;
                }
                case 29969:                                 // Summon Blizzard
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 29952, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 29979:                                 // Massive Magnetic Pull
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 30010, true);
                    return;
                }
                case 30004:                                 // Flame Wreath
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 29946, true);
                    return;
                }
                case 30458:                                 // Nigh Invulnerability
                {
                    if (!m_CastItem)
                        return;

                    if (roll_chance_i(86))                  // Nigh-Invulnerability   - success
                        m_caster->CastSpell(m_caster, 30456, true, m_CastItem);
                    else                                    // Complete Vulnerability - backfire in 14% casts
                        m_caster->CastSpell(m_caster, 30457, true, m_CastItem);

                    return;
                }
                case 30507:                                 // Poultryizer
                {
                    if (!m_CastItem)
                        return;

                    if (roll_chance_i(80))                  // Poultryized! - success
                        m_caster->CastSpell(unitTarget, 30501, true, m_CastItem);
                    else                                    // Poultryized! - backfire 20%
                        m_caster->CastSpell(unitTarget, 30504, true, m_CastItem);

                    return;
                }
                case 32146:                                 // Liquid Fire
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)m_caster)->KilledMonsterCredit(unitTarget->GetEntry(), unitTarget->GetObjectGuid());
                    ((Creature*)unitTarget)->ForcedDespawn();
                    return;
                }
                case 32300:                                 // Focus Fire
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, unitTarget->GetMap()->IsRegularDifficulty() ? 32302 : 38382, true);
                    return;
                }
                case 33060:                                 // Make a Wish
                {
                    if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;

                    switch(urand(1,5))
                    {
                        case 1: spell_id = 33053; break;    // Mr Pinchy's Blessing
                        case 2: spell_id = 33057; break;    // Summon Mighty Mr. Pinchy
                        case 3: spell_id = 33059; break;    // Summon Furious Mr. Pinchy
                        case 4: spell_id = 33062; break;    // Tiny Magical Crawdad
                        case 5: spell_id = 33064; break;    // Mr. Pinchy's Gift
                    }

                    m_caster->CastSpell(m_caster, spell_id, true, NULL);
                    return;
                }
                case 34803:                                 // Summon Reinforcements
                {
                    m_caster->CastSpell(m_caster, 34810, true); // Summon 20083 behind of the caster
                    m_caster->CastSpell(m_caster, 34817, true); // Summon 20078 right of the caster
                    m_caster->CastSpell(m_caster, 34818, true); // Summon 20078 left of the caster
                    m_caster->CastSpell(m_caster, 34819, true); // Summon 20078 front of the caster
                    return;
                }
                case 36677:                                 // Chaos Breath
                {
                    if (!unitTarget)
                        return;

                    uint32 possibleSpells[] = {36693, 36694, 36695, 36696, 36697, 36698, 36699, 36700} ;
                    std::vector<uint32> spellPool(possibleSpells, possibleSpells + countof(possibleSpells));
                    std::random_shuffle(spellPool.begin(), spellPool.end());

                    for (uint8 i = 0; i < (m_caster->GetMap()->IsRegularDifficulty() ? 2 : 4); ++i)
                        m_caster->CastSpell(m_caster, spellPool[i], true);

                    return;
                }
                case 33923:                                 // Sonic Boom
                case 38796:                                 // Sonic Boom (heroic)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, m_spellInfo->Id == 33923 ? 33666 : 38795, true);
                    return;
                }
                case 35745:                                 // Socrethar's Stone
                {
                    uint32 spell_id;
                    switch(m_caster->GetAreaId())
                    {
                        case 3900: spell_id = 35743; break; // Socrethar Portal
                        case 3742: spell_id = 35744; break; // Socrethar Portal
                        default: return;
                    }

                    m_caster->CastSpell(m_caster, spell_id, true);
                    return;
                }
                case 37473:                                 // Detect Whispers (related to quest 10607 - Whispers of the Raven God_Whispers of the Raven God)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, damage, true);
                    break;
                }
                case 37674:                                 // Chaos Blast
                {
                    if (!unitTarget)
                        return;

                    int32 basepoints0 = 100;
                    m_caster->CastCustomSpell(unitTarget, 37675, &basepoints0, NULL, NULL, true);
                    return;
                }
                case 38173:                                 // Rexxar's Whistle (related to quest 10714)
                {
                    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER || SpellEffectIndex(effect->EffectIndex) != EFFECT_INDEX_0)
                        break;

                    if (m_caster->GetMapId() != 530 && m_caster->GetZoneId() != 3522 && m_caster->GetAreaId() != 3826)
                        break;
                
                    Creature *pTaskmasterTemp = unitTarget->GetClosestCreature(22160, 10.0f);
                    Creature *pSoothsayerrTemp = unitTarget->GetClosestCreature(22384, 10.0f);
                
                    if (m_caster->GetTransport() || !pTaskmasterTemp || !pSoothsayerrTemp || ((Player*)m_caster)->GetQuestStatus(10714) != QUEST_STATUS_INCOMPLETE)
                        break;

                    ((Player*)m_caster)->KilledMonsterCredit(22383);

                    return;
                }
                case 38782:                                 // Druid Signal
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || SpellEffectIndex(effect->EffectIndex) != EFFECT_INDEX_0)
                        break;

                    m_caster->SummonCreature(22423, m_caster->GetPositionX()+2, m_caster->GetPositionY()+2, unitTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000);

                    // change to right displayId
                    if (Creature* EvergroveDruid = m_caster->GetClosestCreature(22423, 3.0f))
                    {
                        // expected for players
                        switch(m_caster->getRace())
                        {
                            case RACE_NIGHTELF:
                                EvergroveDruid->SetDisplayId(m_caster->getGender() == GENDER_MALE ? 20991 : 20992);
                                break;
                            case RACE_TAUREN:
                                EvergroveDruid->SetDisplayId(m_caster->getGender() == GENDER_MALE ? 20993 : 20994);
                                break;
                            default:
                                break;
                        }
                    }
                    return;
                }
                case 39189:                                 // Sha'tari Torch
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Flames
                    if (unitTarget->HasAura(39199))
                        return;

                    unitTarget->CastSpell(unitTarget, 39199, true);
                    ((Player*)m_caster)->KilledMonsterCredit(unitTarget->GetEntry(), unitTarget->GetObjectGuid());
                    ((Creature*)unitTarget)->ForcedDespawn(10000);
                    return;
                }
                case 39635:                                 // Throw Glaive (first)
                case 39849:                                 // Throw Glaive (second)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 41466, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 39992:                                 // High Warlord Naj'entus: Needle Spine Targeting
                {
                    if (!unitTarget)
                        return;

                    // TODO - Cone Targeting; along wowwiki this spell should target "three random targets in a cone"
                    m_caster->CastSpell(unitTarget, 39835, true);
                    return;
                }
                case 40109:                                 // Knockdown Fel Cannon: The Bolt
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 40075, true);
                    return;
                }
                case 40160:                                 // Throw Bomb
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        break;

                    // Hack...
                    ((Player*)m_caster)->KilledMonsterCredit(23118);

                    break;
                }
                case 40802:                                 // Mingo's Fortune Generator (Mingo's Fortune Giblets)
                {
                    // selecting one from Bloodstained Fortune item
                    uint32 newitemid;
                    switch(urand(1, 20))
                    {
                        case 1:  newitemid = 32688; break;
                        case 2:  newitemid = 32689; break;
                        case 3:  newitemid = 32690; break;
                        case 4:  newitemid = 32691; break;
                        case 5:  newitemid = 32692; break;
                        case 6:  newitemid = 32693; break;
                        case 7:  newitemid = 32700; break;
                        case 8:  newitemid = 32701; break;
                        case 9:  newitemid = 32702; break;
                        case 10: newitemid = 32703; break;
                        case 11: newitemid = 32704; break;
                        case 12: newitemid = 32705; break;
                        case 13: newitemid = 32706; break;
                        case 14: newitemid = 32707; break;
                        case 15: newitemid = 32708; break;
                        case 16: newitemid = 32709; break;
                        case 17: newitemid = 32710; break;
                        case 18: newitemid = 32711; break;
                        case 19: newitemid = 32712; break;
                        case 20: newitemid = 32713; break;
                        default:
                            return;
                    }

                    DoCreateItem(effect, newitemid);
                    return;
                }
                case 40834:                                 // Agonizing Flames
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 40932, true);
                    return;
                }
                case 40962:                                 // Blade's Edge Terrace Demon Boss Summon Branch
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch (urand(0,3))
                    {
                        case 0: spell_id = 40957; break;    // Blade's Edge Terrace Demon Boss Summon 1
                        case 1: spell_id = 40959; break;    // Blade's Edge Terrace Demon Boss Summon 2
                        case 2: spell_id = 40960; break;    // Blade's Edge Terrace Demon Boss Summon 3
                        case 3: spell_id = 40961; break;    // Blade's Edge Terrace Demon Boss Summon 4
                    }
                    unitTarget->CastSpell(unitTarget, spell_id, true);
                    return;
                }
                case 42287:                                 // Salvage Wreckage
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (roll_chance_i(66))
                        m_caster->CastSpell(m_caster, 42289, true, m_CastItem);
                    else
                        m_caster->CastSpell(m_caster, 42288, true);

                    return;
                }
                case 42339:                                 // Bucket Lands
                {
                     // remove aura Has Bucket from caster
                   m_caster->RemoveAurasDueToSpell(42336);
                    if (!unitTarget)
                        return;
                    // if target has aura Has Bucket do nothing
                    if (unitTarget->HasAura(42336))
                         return;

                    // apply aura Has Bucket
                    unitTarget->CastSpell(unitTarget, 42336, true);
                    // create new bucket for target
                    m_caster->CastSpell(unitTarget, 42349, true);
                    return;
                }
                case 42628:                                 // Fire Bomb (throw)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 42629, true);
                    return;
                }
                case 42631:                                 // Fire Bomb (explode)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    unitTarget->RemoveAurasDueToSpell(42629);
                    unitTarget->CastSpell(unitTarget, 42630, true);

                    // despawn the bomb after exploding
                    ((Creature*)unitTarget)->ForcedDespawn(3000);
                    return;
                }
                case 42793:                                 // Burn Body
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Creature* pCreature = (Creature*)unitTarget;

                    // Spell can be used in combat and may affect different target than the expected.
                    // If target is not the expected we need to prevent this effect.
                    if (pCreature->HasLootRecipient() || pCreature->isInCombat())
                        return;

                    // set loot recipient, prevent re-use same target
                    pCreature->SetLootRecipient(m_caster);

                    pCreature->ForcedDespawn(m_duration);

                    // EFFECT_INDEX_2 has 0 miscvalue for effect 134, doing the killcredit here instead (only one known case exist where 0)
                    ((Player*)m_caster)->KilledMonster(pCreature->GetCreatureInfo(), pCreature->GetObjectGuid());
                    return;
                }
                case 43014:                                 // Despawn Self
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    ((Creature*)unitTarget)->ForcedDespawn(2000);
                    float x, y, z;
                    unitTarget->GetClosePoint(x, y, z, unitTarget->GetObjectBoundingRadius(), 10.0f, unitTarget->GetOrientation());
                    unitTarget->MonsterMoveWithSpeed(x, y, z, 28);
                    return;
                }
                case 43036:                                 // Dismembering Corpse
                {
                    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (unitTarget->HasAura(43059, EFFECT_INDEX_0))
                        return;

                    unitTarget->CastSpell(m_caster, 43037, true);
                    unitTarget->CastSpell(unitTarget, 43059, true);
                    return;
                }
                case 43069:                                 // Towers of Certain Doom: Skorn Cannonfire
                {
                    // Towers of Certain Doom: Tower Caster Instakill
                    m_caster->CastSpell(m_caster, 43072, true);
                    return;
                }
                case 43096:                                 // Summon All Players
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 43097, true);
                    return;
                }
                case 43144:                                 // Hatch All Eggs
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 42493, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 43209:                                 // Place Ram Meat
                {
                    if (!unitTarget)
                        return;

                    // Self Visual - Sleep Until Cancelled (DND)
                    unitTarget->RemoveAurasDueToSpell(6606);
                    return;
                }
                case 43498:                                 // Siphon Soul
                {
                    // This spell should cast the next spell only for one (player)target, however it should hit multiple targets, hence this kind of implementation
                    if (!unitTarget || m_UniqueTargetInfo.rbegin()->targetGUID != unitTarget->GetObjectGuid())
                        return;

                    std::vector<Unit*> possibleTargets;
                    possibleTargets.reserve(m_UniqueTargetInfo.size());
                    for (TargetList::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
                    {
                        // Skip Non-Players
                        if (!itr->targetGUID.IsPlayer())
                            continue;

                        if (Unit* target = m_caster->GetMap()->GetPlayer(itr->targetGUID))
                            possibleTargets.push_back(target);
                    }

                    // Cast Siphon Soul channeling spell
                    if (!possibleTargets.empty())
                        m_caster->CastSpell(possibleTargets[urand(0, possibleTargets.size()-1)], 43501, false);

                    return;
                }
                case 43572:                                 // Send Them Packing: On /Raise Emote Dummy to Player
                {
                    if (!unitTarget)
                        return;

                    // m_caster (creature) should start walking back to it's "home" here, no clear way how to do that

                    // Send Them Packing: On Successful Dummy Spell Kill Credit
                    m_caster->CastSpell(unitTarget, 42721, true);
                    return;
                }
                // Demon Broiled Surprise
                case 43723:
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)m_caster)->CastSpell(unitTarget, 43753, true, m_CastItem, NULL, m_originalCasterGUID, m_spellInfo);
                    return;
                }
                case 43882:                                 // Scourging Crystal Controller Dummy
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // see spell dummy 50133
                    unitTarget->RemoveAurasDueToSpell(43874);
                    return;
                }
                case 44454:                                 // Tasty Reef Fish
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    m_caster->CastSpell(unitTarget, 44455, true, m_CastItem);
                    return;
                }
                case 44845:                                 // Spectral Realm
                {
                    if (!unitTarget)
                        return;

                    // teleport all targets which have the spectral realm aura
                    if (unitTarget->HasAura(46021))
                    {
                        unitTarget->RemoveAurasDueToSpell(46021);
                        unitTarget->CastSpell(unitTarget, 46020, true);
                        unitTarget->CastSpell(unitTarget, 44867, true);
                    }

                    return;
                }
                case 44869:                                 // Spectral Blast
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // If target has spectral exhaustion or spectral realm aura return
                    if (unitTarget->HasAura(44867) || unitTarget->HasAura(46021))
                        return;

                    // Cast the spectral realm effect spell, visual spell and spectral blast rift summoning
                    unitTarget->CastSpell(unitTarget, 44866, true, NULL, NULL, m_caster->GetObjectGuid());
                    unitTarget->CastSpell(unitTarget, 46648, true, NULL, NULL, m_caster->GetObjectGuid());
                    unitTarget->CastSpell(unitTarget, 44811, true);
                    return;
                }
                case 44875:                                 // Complete Raptor Capture
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;

                    creatureTarget->ForcedDespawn();

                    //cast spell Raptor Capture Credit
                    m_caster->CastSpell(m_caster, 42337, true, NULL);
                    return;
                }
                case 44997:                                 // Converting Sentry
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* creatureTarget = (Creature*)unitTarget;

                    creatureTarget->ForcedDespawn();

                    // Converted Sentry Credit
                    m_caster->CastSpell(m_caster, 45009, true);
                    return;
                }
                case 45030:                                 // Impale Emissary
                {
                    // Emissary of Hate Credit
                    m_caster->CastSpell(m_caster, 45088, true);
                    return;
                }
                case 45449:                                // Arcane Prisoner Rescue
                {
                    uint32 spellId=0;
                    switch(rand() % 2)
                    {
                        case 0: spellId = 45446; break;    // Summon Arcane Prisoner - Male
                        case 1: spellId = 45448; break;    // Summon Arcane Prisoner - Female
                    }
                    //Spawn
                    m_caster->CastSpell(m_caster, spellId, true);
                    
                    if (!unitTarget) return;
                    //Arcane Prisoner Kill Credit
                    unitTarget->CastSpell(m_caster, 45456, true);

                    break;
                }
                case 45583:                                 // Throw Gnomish Grenade
                {
                    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)m_caster)->KilledMonsterCredit(unitTarget->GetEntry(), unitTarget->GetObjectGuid());

                    // look for gameobject within max spell range of unitTarget, and respawn if found

                    // big fire
                    GameObject* pGo = NULL;

                    float fMaxDist = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

                    MaNGOS::NearestGameObjectEntryInPosRangeCheck go_check_big(*unitTarget, 187675, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), fMaxDist);
                    MaNGOS::GameObjectSearcher<MaNGOS::NearestGameObjectEntryInPosRangeCheck> checker1(pGo, go_check_big);

                    Cell::VisitGridObjects(unitTarget, checker1, fMaxDist);

                    if (pGo && !pGo->isSpawned())
                    {
                        pGo->SetRespawnTime(MINUTE/2);
                        pGo->Refresh();
                    }

                    // small fire
                    std::list<GameObject*> lList;

                    MaNGOS::GameObjectEntryInPosRangeCheck go_check_small(*unitTarget, 187676, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), fMaxDist);
                    MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectEntryInPosRangeCheck> checker2(lList, go_check_small);

                    Cell::VisitGridObjects(unitTarget, checker2, fMaxDist);

                    for(std::list<GameObject*>::iterator iter = lList.begin(); iter != lList.end(); ++iter)
                    {
                        if (!(*iter)->isSpawned())
                        {
                            (*iter)->SetRespawnTime(MINUTE/2);
                            (*iter)->Refresh();
                        }
                    }

                    return;
                }
                case 45644:                                 // Juggle Torch (Catch)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 45280, true);
                    break;
                }
                case 45819:                                 // Throw Torch
                {
                    if ((m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) == 0)
                    {
                        ERROR_LOG("Spell::EffectDummy %u unknown target location", m_spellInfo->Id);
                        return;
                    }

                    m_caster->CastSpell(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, 45638, true);
                    //if (m_caster->GetTypeId() != TYPEID_PLAYER || !m_caster->IsInWorld())
                    //    return;

                    //((Player*)m_caster)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2, 45280, 0, m_caster);
                    return;
                }
                case 45958:                                 // Signal Alliance
                {
                    m_caster->CastSpell(m_caster, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 45980:                                 // Re-Cursive Transmatter Injection
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER && unitTarget)
                    {
                        if (const SpellEntry *pSpell = sSpellStore.LookupEntry(46022))
                        {
                            m_caster->CastSpell(unitTarget, pSpell, true);
                            SpellEffectEntry const* killSpellEffect = pSpell->GetSpellEffect(EFFECT_INDEX_0);
                            ((Player*)m_caster)->KilledMonsterCredit(killSpellEffect ? killSpellEffect->EffectMiscValue : 0);
                        }

                        if (unitTarget->GetTypeId() == TYPEID_UNIT)
                            ((Creature*)unitTarget)->ForcedDespawn();
                    }

                    return;
                }
                case 45685:                                 // Magnataur On Death 2
                {
                    m_caster->RemoveAurasDueToSpell(45673);
                    m_caster->RemoveAurasDueToSpell(45672);
                    m_caster->RemoveAurasDueToSpell(45677);
                    m_caster->RemoveAurasDueToSpell(45681);
                    m_caster->RemoveAurasDueToSpell(45683);
                    return;
                }
                case 45976:                                 // Open Portal
                case 46177:                                 // Open All Portals
                {
                    if (!unitTarget)
                        return;

                    // portal visual
                    unitTarget->CastSpell(unitTarget, 45977, true);

                    // break in case additional procressing in scripting library required
                    break;
                }
                case 45989:                                 // Summon Void Sentinel Summoner Visual
                {
                    if (!unitTarget)
                        return;

                    // summon void sentinel
                    unitTarget->CastSpell(unitTarget, 45988, true);

                    return;
                }
                case 45990:                                 // Collect Oil
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (const SpellEntry *pSpell = sSpellStore.LookupEntry(45991))
                    {
                        unitTarget->CastSpell(unitTarget, pSpell, true);
                        ((Creature*)unitTarget)->ForcedDespawn(m_duration);
                    }

                    return;
                }
                case 46167:                                 // Planning for the Future: Create Snowfall Glade Pup Cover
                case 50918:                                 // Gluttonous Lurkers: Create Basilisk Crystals Cover
                case 50926:                                 // Gluttonous Lurkers: Create Zul'Drak Rat Cover
                case 51026:                                 // Create Drakkari Medallion Cover
                case 51592:                                 // Pickup Primordial Hatchling
                case 51961:                                 // Captured Chicken Cover
                case 55364:                                 // Create Ghoul Drool Cover
                case 61832:                                 // Rifle the Bodies: Create Magehunter Personal Effects Cover
                case 74904:                                 // Pickup Sen'jin Frog
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spellId = 0;

                    switch(m_spellInfo->Id)
                    {
                        case 46167: spellId = 46773; break;
                        case 50918: spellId = 50919; break;
                        case 50926: spellId = 50927; break;
                        case 51026: spellId = 50737; break;
                        case 51592: spellId = 51593; break;
                        case 51961: spellId = 51037; break;
                        case 55364: spellId = 55363; break;
                        case 61832: spellId = 47096; break;
                        case 74904: spellId = 74905; break;
                    }

                    if (const SpellEntry *pSpell = sSpellStore.LookupEntry(spellId))
                    {
                        unitTarget->CastSpell(m_caster, spellId, true);

                        Creature* creatureTarget = (Creature*)unitTarget;

                        if (const SpellCastTimesEntry *pCastTime = sSpellCastTimesStore.LookupEntry(pSpell->CastingTimeIndex))
                            creatureTarget->ForcedDespawn(pCastTime->CastTime + 1);
                    }
                    return;
                }
                case 46171:                                 // Scuttle Wrecked Flying Machine
                {
                    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)m_caster)->KilledMonsterCredit(unitTarget->GetEntry(), unitTarget->GetObjectGuid());

                    // look for gameobject within max spell range of unitTarget, and respawn if found
                    GameObject* pGo = NULL;

                    float fMaxDist = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

                    MaNGOS::NearestGameObjectEntryInPosRangeCheck go_check(*unitTarget, 187675, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), fMaxDist);
                    MaNGOS::GameObjectSearcher<MaNGOS::NearestGameObjectEntryInPosRangeCheck> checker(pGo, go_check);

                    Cell::VisitGridObjects(unitTarget, checker, fMaxDist);

                    if (pGo && !pGo->isSpawned())
                    {
                        pGo->SetRespawnTime(MINUTE/2);
                        pGo->Refresh();
                    }

                    return;
                }
                case 46485:                                 // Greatmother's Soulcatcher
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (const SpellEntry *pSpell = sSpellStore.LookupEntry(46486))
                    {
                        m_caster->CastSpell(unitTarget, pSpell, true);

                        if (SpellEffectEntry const* pSpellEffect = pSpell->GetSpellEffect(EFFECT_INDEX_0))
                            if (const SpellEntry *pSpellCredit = sSpellStore.LookupEntry(pSpellEffect->EffectTriggerSpell))
                                if(SpellEffectEntry const* pSpellCreditEffect = pSpellCredit->GetSpellEffect(EFFECT_INDEX_0))
                                    ((Player*)m_caster)->KilledMonsterCredit(pSpellCreditEffect->EffectMiscValue);

                        ((Creature*)unitTarget)->ForcedDespawn();
                    }

                    return;
                }
                case 46606:                                 // Plague Canister Dummy
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    unitTarget->CastSpell(m_caster, 43160, true);
                    unitTarget->SetDeathState(JUST_DIED);
                    unitTarget->SetHealth(0);
                    return;
                }
                case 46671:                                 // Cleansing Flames (Exodar)
                case 46672:                                 // Cleansing Flames (Silvermoon)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(unitTarget, m_spellInfo->Id == 46671 ? 46690 : 46689, true);
                    return;
                }
                case 46747:                                 // Fling torch
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(m_caster, 45638, true);
                    ((Player*)unitTarget)->KilledMonsterCredit(25218);
                    break;
                }
                case 46797:                                 // Quest - Borean Tundra - Set Explosives Cart
                {
                    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)m_caster)->KilledMonsterCredit(unitTarget->GetEntry(), unitTarget->GetObjectGuid());

                    // Quest - Borean Tundra - Summon Explosives Cart
                    unitTarget->CastSpell(unitTarget, 46798, true);
                    return;
                }
                case 47110:                                 // Summon Drakuru's Image
                {
                    uint32 spellId = 0;

                    // Spell 47117,47149,47316,47405,50439 exist, are these used to check area/meet requirement
                    // and to cast correct spell in correct area?

                    switch(m_caster->GetAreaId())
                    {
                        case 4255: spellId = 47381; break;  // Reagent Check (Frozen Mojo)
                        case 4209: spellId = 47386; break;  // Reagent Check (Zim'Bo's Mojo)
                        case 4270: spellId = 47389; break;  // Reagent Check (Desperate Mojo)
                        case 4216: spellId = 47408; break;  // Reagent Check (Sacred Mojo)
                        case 4196: spellId = 50441; break;  // Reagent Check (Survival Mojo)
                    }

                    // The additional castspell arguments are needed here to remove reagents for triggered spells
                    if (spellId)
                        m_caster->CastSpell(m_caster, spellId, true, m_CastItem, NULL, m_caster->GetObjectGuid(), m_spellInfo);

                    return;
                }
                case 47170:                                 // Impale Leviroth
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    unitTarget->SetHealthPercent(8.0f);

                    // Cosmetic - Underwater Blood (no sound)
                    unitTarget->CastSpell(unitTarget, 47172, true);

                    ((Creature*)unitTarget)->AI()->AttackStart(m_caster);
                    return;
                }
                case 47176:                                 // Infect Ice Troll
                {
                    // Spell has wrong areaGroupid, so it can not be casted where expected.
                    // TODO: research if spells casted by NPC, having TARGET_SCRIPT, can have disabled area check
                    if (!unitTarget)
                        return;

                    // Plague Effect Self
                    unitTarget->CastSpell(unitTarget, 47178, true);
                    return;
                }
                case 47305:                                 // Potent Explosive Charge
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // only if below 80% hp
                    if (unitTarget->GetHealthPercent() > 80.0f)
                        return;

                    // Issues with explosion animation (remove insta kill spell resolves the issue)

                    // Quest - Jormungar Explosion Spell Spawner
                    unitTarget->CastSpell(unitTarget, 47311, true);

                    // Potent Explosive Charge
                    unitTarget->CastSpell(unitTarget, 47306, true);

                    return;
                }
                case 47381:                                 // Reagent Check (Frozen Mojo)
                case 47386:                                 // Reagent Check (Zim'Bo's Mojo)
                case 47389:                                 // Reagent Check (Desperate Mojo)
                case 47408:                                 // Reagent Check (Sacred Mojo)
                case 50441:                                 // Reagent Check (Survival Mojo)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    switch(m_spellInfo->Id)
                    {
                        case 47381:
                            // Envision Drakuru
                            m_caster->CastSpell(m_caster, 47118, true);
                            break;
                        case 47386:
                            m_caster->CastSpell(m_caster, 47150, true);
                            break;
                        case 47389:
                            m_caster->CastSpell(m_caster, 47317, true);
                            break;
                        case 47408:
                            m_caster->CastSpell(m_caster, 47406, true);
                            break;
                        case 50441:
                            m_caster->CastSpell(m_caster, 50440, true);
                            break;
                    }

                    return;
                }
                case 47468:                                 // Claw
                {
                    if (!unitTarget)
                        return;

                    // check for Dark Transformation
                    m_caster->CastSpell(unitTarget, m_caster->HasAura(63560) ? 91778 : 91776, true);
                    return;
                }
                case 47482:                                 // Leap
                {
                    if (!unitTarget)
                        return;

                    // check for Dark Transformation
                    m_caster->CastSpell(unitTarget, m_caster->HasAura(63560) ? 91802 : 91809, true);
                    return;
                }
                case 47484:                                 // Huddle
                {
                    if (!unitTarget)
                        return;

                    // check for Dark Transformation
                    m_caster->CastSpell(unitTarget, m_caster->HasAura(63560) ? 91837 : 91838, true);
                    return;
                }
                case 48046:                                 // Use Camera
                {
                    if (!unitTarget)
                        return;

                    // No despawn expected, nor any change in dynamic flags/other flags.
                    // Need internal way to track if credit has been given for this object.

                    // Iron Dwarf Snapshot Credit
                    m_caster->CastSpell(m_caster, 48047, true, m_CastItem, NULL, unitTarget->GetObjectGuid());
                    return;
                }
                case 48386:                                 // Ymiron Summon Fountain
                {
                    m_caster->CastSpell(m_caster, 48385, true);
                    return;
                }
                case 48593:                                 // Summon Avenging Spirit
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        m_caster->CastSpell(m_caster, 48592, true);
                    }
                    return;
                }
                case 48790:                                 // Neltharion's Flame
                {
                    if (!unitTarget)
                        return;

                    // Neltharion's Flame Fire Bunny: Periodic Fire Aura
                    unitTarget->CastSpell(unitTarget, 48786, false);
                    return;
                }
                case 49075:                                 // Quest - Smoke 'Em Out - Smoked Bomb
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (unitTarget->GetEntry() != 27570)
                        return;

                    unitTarget->SetPanic(m_caster, 6000);

                    // Venture Straggler Credit
                    m_caster->CastSpell(m_caster, 49093, true, m_CastItem, NULL, unitTarget->GetObjectGuid());

                    ((Creature*)unitTarget)->ForcedDespawn(6100);

                    return;
                }
                case 49357:                                 // Brewfest Mount Transformation
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (!m_caster->HasAuraType(SPELL_AURA_MOUNTED))
                        return;

                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    // Ram for Alliance, Kodo for Horde
                    if (((Player *)m_caster)->GetTeam() == ALLIANCE)
                    {
                        if (m_caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                            // 100% Ram
                            m_caster->CastSpell(m_caster, 43900, true);
                        else
                            // 60% Ram
                            m_caster->CastSpell(m_caster, 43899, true);
                    }
                    else
                    {
                        if (((Player *)m_caster)->GetSpeedRate(MOVE_RUN) >= 2.0f)
                            // 100% Kodo
                            m_caster->CastSpell(m_caster, 49379, true);
                        else
                            // 60% Kodo
                            m_caster->CastSpell(m_caster, 49378, true);
                    }
                    return;
                }
                case 49625:                                 // Brave's Flare
                {
                    // Trigger Brave's Flare Effect (with EffectTarget)
                    m_caster->CastSpell(m_caster, 43106, true);
                    return;
                }
                case 49634:                                 // Sergeant's Flare
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Towers of Certain Doom: Tower Bunny Smoke Flare Effect
                    // TODO: MaNGOS::DynamicObjectUpdater::VisitHelper prevent aura to be applied to dummy creature (see HandleAuraDummy for effect of aura)
                    m_caster->CastSpell(unitTarget, 56511, true);

                    static uint32 const spellCredit[4] =
                    {
                        43077,                              // E Kill Credit
                        43067,                              // NW Kill Credit
                        43087,                              // SE Kill Credit
                        43086,                              // SW Kill Credit
                    };

                    // for sizeof(spellCredit)
                    for (int i = 0; i < 4; ++i)
                    {
                        const SpellEntry *pSpell = sSpellStore.LookupEntry(spellCredit[i]);

                        if (pSpell->GetEffectMiscValue(EFFECT_INDEX_0) == unitTarget->GetEntry())
                        {
                            m_caster->CastSpell(m_caster, spellCredit[i], true);
                            break;
                        }
                    }

                    return;
                }
                case 49859:                                 // Rune of Command
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Captive Stone Giant Kill Credit
                    unitTarget->CastSpell(m_caster, 43564, true);
                    // Is it supposed to despawn?
                    ((Creature*)unitTarget)->ForcedDespawn();
                    return;
                }
                case 50133:                                 // Scourging Crystal Controller
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Scourge Mur'gul Camp: Force Shield Arcane Purple x3
                    if (unitTarget->HasAura(43874))
                    {
                        // someone else is already channeling target
                        if (unitTarget->HasAura(43878))
                            return;

                        // Scourging Crystal Controller
                        m_caster->CastSpell(unitTarget, 43878, true, m_CastItem);
                    }

                    return;
                }
                case 50243:                                 // Teach Language
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // spell has a 1/3 chance to trigger one of the below
                    if (roll_chance_i(66))
                        return;

                    if (((Player*)m_caster)->GetTeam() == ALLIANCE)
                    {
                        // 1000001 - gnomish binary
                        m_caster->CastSpell(m_caster, 50242, true);
                    }
                    else
                    {
                        // 01001000 - goblin binary
                        m_caster->CastSpell(m_caster, 50246, true);
                    }

                    return;
                }
                case 50440:                                 // Envision Drakuru
                {
                    if (!unitTarget)
                        return;

                    // Script Cast Summon Image of Drakuru 05
                    unitTarget->CastSpell(unitTarget, 50439, true);
                    return;
                }
                case 50546:                                 // Ley Line Focus Control Ring Effect
                case 50547:                                 // Ley Line Focus Control Amulet Effect
                case 50548:                                 // Ley Line Focus Control Talisman Effect
                {
                    if (!m_originalCaster || !unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    switch(m_spellInfo->Id)
                    {
                        case 50546: unitTarget->CastSpell(m_originalCaster, 47390, true); break;
                        case 50547: unitTarget->CastSpell(m_originalCaster, 47472, true); break;
                        case 50548: unitTarget->CastSpell(m_originalCaster, 47635, true); break;
                    }

                    if (m_originalCaster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    switch(m_spellInfo->Id)
                    {
                        case 50546: ((Player*)m_originalCaster)->KilledMonsterCredit(26773); break;
                        case 50547: ((Player*)m_originalCaster)->KilledMonsterCredit(26831); break;
                        case 50548: ((Player*)m_originalCaster)->KilledMonsterCredit(26887); break;
                    }

                    return;
                }
                case 51276:                                 // Incinerate Corpse
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    unitTarget->CastSpell(unitTarget, 51278, true);
                    unitTarget->CastSpell(m_caster, 51279, true);

                    unitTarget->SetDeathState(JUST_DIED);
                    return;
                }
                case 51330:                                 // Shoot RJR
                {
                    if (!unitTarget)
                        return;

                    // guessed chances
                    if (roll_chance_i(75))
                        m_caster->CastSpell(unitTarget, roll_chance_i(50) ? 51332 : 51366, true, m_CastItem);
                    else
                        m_caster->CastSpell(unitTarget, 51331, true, m_CastItem);

                    return;
                }
                case 51333:                                 // Dig For Treasure
                {
                    if (!unitTarget)
                        return;

                    if (roll_chance_i(75))
                        m_caster->CastSpell(unitTarget, 51370, true, m_CastItem);
                    else
                        m_caster->CastSpell(m_caster, 51345, true);

                    return;
                }
                case 51336:                                 // Drakos Magic Pull
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 50770, true);
                    return;
                }
                case 51420:                                 // Digging for Treasure Ping
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // only spell related protector pets exist currently
                    Pet* pPet = m_caster->GetProtectorPet();
                    if (!pPet)
                        return;

                    pPet->SetFacingToObject(unitTarget);

                    // Digging for Treasure
                    pPet->CastSpell(unitTarget, 51405, true);

                    ((Creature*)unitTarget)->ForcedDespawn(1);
                    return;
                }
                case 51582:                                 // Rocket Boots Engaged (Rocket Boots Xtreme and Rocket Boots Xtreme Lite)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (BattleGround* bg = ((Player*)m_caster)->GetBattleGround())
                        bg->EventPlayerDroppedFlag((Player*)m_caster);

                    m_caster->CastSpell(m_caster, 30452, true, NULL);
                    return;
                }
                case 51840:                                 // Despawn Fruit Tosser
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (roll_chance_i(20))
                    {
                        // summon NPC, or...
                        unitTarget->CastSpell(m_caster, 52070, true);
                    }
                    else
                    {
                        // ...drop banana, orange or papaya
                        switch(urand(0,2))
                        {
                            case 0: unitTarget->CastSpell(m_caster, 51836, true); break;
                            case 1: unitTarget->CastSpell(m_caster, 51837, true); break;
                            case 2: unitTarget->CastSpell(m_caster, 51839, true); break;
                        }
                    }

                    ((Creature*)unitTarget)->ForcedDespawn(5000);
                    return;
                }
                case 51858: // Siphon of Acherus
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                    return;

                    static uint32 const spellCredit[4] =
                    {
                        51974,                              // Forge Credit
                        51980,                              // Scarlet Hold Credit
                        51977,                              // Town Hall Credit
                        51982,                              // Chapel Credit
                    };
                    for (int i = 0; i < 4; ++i)
                    {
                        SpellEntry const * pSpell = sSpellStore.LookupEntry(spellCredit[i]);
                        if (!pSpell)
                            continue;

                        SpellEffectEntry const * effect = pSpell->GetSpellEffect(EFFECT_INDEX_0);
                        if (!effect)
                            continue;

                        if (effect->EffectMiscValue == (int32)unitTarget->GetEntry())
                        {
                            m_caster->RemoveAurasDueToSpell(52006);   // Remove Stealth from Eye of Acherus upon cast
                            m_caster->CastSpell(unitTarget, spellCredit[i], true);
                            break;
                        }
                    }
                    return;
                }
                case 51866:                                 // Kick Nass
                {
                    // It is possible that Nass Heartbeat (spell id 61438) is involved in this
                    // If so, unclear how it should work and using the below instead (even though it could be a bit hack-ish)

                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Only own guardian pet
                    if (m_caster != unitTarget->GetOwner())
                        return;

                    // This means we already set state (see below) and need to wait.
                    if (unitTarget->hasUnitState(UNIT_STAT_ROOT))
                        return;

                    // Expecting pTargetDummy to be summoned by AI at death of target creatures.

                    Creature* pTargetDummy = NULL;
                    float fRange = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*m_caster, 28523, true, false, fRange*2);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pTargetDummy, u_check);

                    Cell::VisitGridObjects(m_caster, searcher, fRange*2);

                    if (pTargetDummy)
                    {
                        if (unitTarget->hasUnitState(UNIT_STAT_FOLLOW | UNIT_STAT_FOLLOW_MOVE))
                            unitTarget->GetMotionMaster()->MovementExpired();

                        unitTarget->MonsterMoveWithSpeed(pTargetDummy->GetPositionX(), pTargetDummy->GetPositionY(), pTargetDummy->GetPositionZ(), 24.f);

                        // Add state to temporarily prevent follow
                        unitTarget->addUnitState(UNIT_STAT_ROOT);

                        // Collect Hair Sample
                        unitTarget->CastSpell(pTargetDummy, 51870, true);
                    }

                    return;
                }
                case 51872:                                 // Hair Sample Collected
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // clear state to allow follow again
                    m_caster->clearUnitState(UNIT_STAT_ROOT);

                    // Nass Kill Credit
                    m_caster->CastSpell(m_caster, 51871, true);

                    // Despawn dummy creature
                    ((Creature*)unitTarget)->ForcedDespawn();

                    return;
                }
                case 51964:                                 // Tormentor's Incense
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // This might not be the best way, and effect may need some adjustment. Removal of any aura from surrounding dummy creatures?
                    if (((Creature*)unitTarget)->AI())
                        ((Creature*)unitTarget)->AI()->AttackStart(m_caster);

                    return;
                }
                case 52238:                                 // Volkhan Temper, Summon Golems
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 52405, true);   // Summon Golem
                    m_caster->CastSpell(m_caster, 52661, true);     // Summon Temper
                    m_caster->CastSpell(unitTarget, 52654, false);  // Temper 2 Vokhan Deal Two Strike

                    return;
                }
                case 52654:                                  // Volkhan Temper 2, Summon Golems
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 52405, true);   // Summon Golem
                    m_caster->CastSpell(m_caster, 52661, true);     // Summon Temper

                    return;
                }
                case 52429:                                 // Volkhan Golem Shattered
                case 59527:
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    ((Creature*)m_caster)->ForcedDespawn(500);
                    return;
                }
                case 52264:                                 // Deliver Stolen Horse
                {
                    if (m_caster->GetObjectGuid().IsVehicle())
                        m_caster->GetVehicleKit()->RemoveAllPassengers();
                    return;
                }
                case 52308:                                 // Take Sputum Sample
                {
                    switch(effect->EffectIndex)
                    {
                        case EFFECT_INDEX_0:
                        {
                            uint32 spellID = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
                            uint32 reqAuraID = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1);

                            if (m_caster->HasAura(reqAuraID, EFFECT_INDEX_0))
                                m_caster->CastSpell(m_caster, spellID, true, NULL);
                            return;
                        }
                        case EFFECT_INDEX_1:                // additional data for dummy[0]
                        case EFFECT_INDEX_2:
                            return;
                    }
                    return;
                }
                case 52369:                                 // Detonate Explosives
                case 52371:                                 // Detonate Explosives
                {
                    if (!unitTarget)
                        return;

                    // Cosmetic - Explosion
                    unitTarget->CastSpell(unitTarget, 46419, true);

                    // look for gameobjects within max spell range of unitTarget, and respawn if found
                    std::list<GameObject*> lList;

                    float fMaxDist = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

                    MaNGOS::GameObjectEntryInPosRangeCheck go_check(*unitTarget, 182071, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), fMaxDist);
                    MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectEntryInPosRangeCheck> checker(lList, go_check);

                    Cell::VisitGridObjects(unitTarget, checker, fMaxDist);

                    for(std::list<GameObject*>::iterator iter = lList.begin(); iter != lList.end(); ++iter)
                    {
                        if (!(*iter)->isSpawned())
                        {
                            (*iter)->SetRespawnTime(MINUTE/2);
                            (*iter)->Refresh();
                        }
                    }

                    return;
                }
                case 52596:                                 // Medallion of Mam'toth
                {
                    if (!unitTarget || unitTarget->GetEntry() != 28851)
                        return;

                    unitTarget->setFaction(35);
                    unitTarget->CombatStop();
                    return;
                }
                case 52759:                                 // Ancestral Awakening
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastCustomSpell(unitTarget, 52752, &damage, NULL, NULL, true);
                    return;
                }
                case 52781:                                 // Persuasive Strike
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player* pCaster = (Player*)m_caster;
                    Creature* cTarget = (Creature*)unitTarget;
                    if (pCaster->GetQuestStatus(12720) != QUEST_STATUS_INCOMPLETE) 
                        return;         
                    // TODO: Need correct chance!
                    if (roll_chance_i(25))
                    {
                        cTarget->MonsterSay(-555, LANG_UNIVERSAL, 0);
                        cTarget->MonsterSay(-556, LANG_UNIVERSAL, 0);
                        cTarget->MonsterSay(-557, LANG_UNIVERSAL, 0);
                        cTarget->MonsterSay(-558, LANG_UNIVERSAL, 0);
                        pCaster->Say("LIES! The pain you are about to endure will be talked about for years to come!", LANG_UNIVERSAL);
                        cTarget->MonsterSay(-559, LANG_UNIVERSAL, 0);
                        pCaster->CompleteQuest(12720);
                        return;
                    }
                    else
                    {
                        // TODO: Add this texts do database. 
                        static const char* playerTexts[] =
                        {
                            "I'll tear the secrets from your soul! Tell me about the \"Crimson Dawn\" and your life may be spared!", 
                            "Tell me what you know about \"Crimson Dawn\" or the beatings will continue!",
                            "I'm through being courteous with your kind, human! What is the \"Crimson Dawn?\"",
                            "Is your life worth so little? Just tell me what I need to know about \"Crimson Dawn\" and I'll end your suffering quickly.",
                            "I can keep this up for a very long time, Scarlet dog! Tell me about the \"Crimson Dawn!\"",
                            "What is the \"Crimson Dawn?\"",
                            "\"Crimson Dawn!\" What is it! Speak!",
                        };
                        static const char* npcTexts[] =
                        {
                            "You'll be hanging in the gallows shortly, Scourge fiend!",
                            "You'll have to kill me, monster! I will tell you NOTHING!",
                            "You hit like a girl. Honestly. Is that the best you can do?",
                            "ARGH! You burned my last good tabard!",
                            "Argh... The pain... The pain is almost as unbearable as the lashings I received in grammar school when I was but a child.",
                            "I used to work for Grand Inquisitor Isillien! Your idea of pain is a normal mid-afternoon for me!",
                        };
                        uint32 plText = urand(0, 6);
                        uint32 npcText = urand(0, 5);
                        pCaster->Say(playerTexts[plText], LANG_UNIVERSAL);
                        cTarget->MonsterSay(npcTexts[npcText], LANG_UNIVERSAL, 0);
                    }     
                    return;
                }
                case 52845:                                 // Brewfest Mount Transformation (Faction Swap)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (!m_caster->HasAuraType(SPELL_AURA_MOUNTED))
                        return;

                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    // Ram for Horde, Kodo for Alliance
                    if (((Player *)m_caster)->GetTeam() == HORDE)
                    {
                        if (m_caster->GetSpeedRate(MOVE_RUN) >= 2.0f)
                            // Swift Brewfest Ram, 100% Ram
                            m_caster->CastSpell(m_caster, 43900, true);
                        else
                            // Brewfest Ram, 60% Ram
                            m_caster->CastSpell(m_caster, 43899, true);
                    }
                    else
                    {
                        if (((Player *)m_caster)->GetSpeedRate(MOVE_RUN) >= 2.0f)
                            // Great Brewfest Kodo, 100% Kodo
                            m_caster->CastSpell(m_caster, 49379, true);
                        else
                            // Brewfest Riding Kodo, 60% Kodo
                            m_caster->CastSpell(m_caster, 49378, true);
                    }
                    return;
                }
                case 53341:                                 // Rune of Cinderglacier
                case 53343:                                 // Rune of Razorice
                {
                    // Runeforging Credit
                    m_caster->CastSpell(m_caster, 54586, true);
                    return;
                }
                case 53475:                                 // Set Oracle Faction Friendly
                case 53487:                                 // Set Wolvar Faction Honored
                case 54015:                                 // Set Oracle Faction Honored
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (effect->EffectIndex == EFFECT_INDEX_0)
                    {
                        Player* pPlayer = (Player*)m_caster;

                        uint32 faction_id = m_currentBasePoints[effect->EffectIndex];
                        int32  rep_change = m_currentBasePoints[EFFECT_INDEX_1];

                        FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

                        if (!factionEntry)
                            return;

                        // Set rep to baserep + basepoints (expecting spillover for oposite faction -> become hated)
                        // Not when player already has equal or higher rep with this faction
                        if (pPlayer->GetReputationMgr().GetBaseReputation(factionEntry) < rep_change)
                            pPlayer->GetReputationMgr().SetReputation(factionEntry, rep_change);

                        // EFFECT_INDEX_2 most likely update at war state, we already handle this in SetReputation
                    }

                    return;
                }
                case 53808:                                 // Pygmy Oil
                {
                    const uint32 spellShrink = 53805;
                    const uint32 spellTransf = 53806;

                    if (SpellAuraHolder* holder = m_caster->GetSpellAuraHolder(spellShrink))
                    {
                        // chance to become pygmified (5, 10, 15 etc)
                        if (roll_chance_i(holder->GetStackAmount() * 5))
                        {
                            m_caster->RemoveAurasDueToSpell(spellShrink);
                            m_caster->CastSpell(m_caster, spellTransf, true);
                            return;
                        }
                    }

                    if (m_caster->HasAura(spellTransf))
                        return;

                    m_caster->CastSpell(m_caster, spellShrink, true);
                    return;
                }
                case 54092:                                 // Monster Slayer's Kit
                {
                    if (!unitTarget)
                        return;

                    uint32 spellIds[] = {51853, 54063, 54071, 54086};
                    m_caster->CastSpell(unitTarget, spellIds[urand(0, 3)], true);
                    return;
                }
                case 54148:                                 // Svala - Ritual Of Sword
                {
                    unitTarget->CastSpell(unitTarget, 48267, true);    // Teleport Player
                    return;
                }
                case 54171:                                 // Divine Storm
                {
                    // split between targets
                    int32 bp = damage / m_UniqueTargetInfo.size();
                    m_caster->CastCustomSpell(unitTarget, 54172, &bp, NULL, NULL, true);
                    return;
                }
                case 54209:                                 // Portal Missile - Drakuru Overlord
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 51807, true);    // Cast Portal Visual
                    return;
                }
                case 54245:                                 // Enough - Drakuru Overlord, Kill Trolls
                {
                    if (!unitTarget)
                        return;

                    m_caster->DealDamage(unitTarget, unitTarget->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    return;
                }
                case 54250:                                 // Skull Missile - Drakuru Overlord
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 54253, true);    // Summon Skull
                    return;
                }
                case 54517:                                 // Magnetic Pull
                {
                    // Feugen casts on Stalagg
                    if (m_caster->GetTypeId() != TYPEID_UNIT || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (m_caster->GetEntry() == 15930 && unitTarget->GetEntry() == 15929)
                    {
                        Unit *pFeugenVictim = m_caster->getVictim();
                        Unit *pStalaggVictim = unitTarget->getVictim();

                        if (pFeugenVictim && pStalaggVictim)
                        {
                            pStalaggVictim->CastSpell(m_caster, 54485, true);
                            pFeugenVictim->CastSpell(unitTarget, 54485, true);

                            // threat swap
                            m_caster->AddThreat(pStalaggVictim, m_caster->getThreatManager().getThreat(pFeugenVictim));
                            unitTarget->AddThreat(pFeugenVictim, m_caster->getThreatManager().getThreat(pStalaggVictim));
                            m_caster->getThreatManager().modifyThreatPercent(pFeugenVictim, -101);
                            unitTarget->getThreatManager().modifyThreatPercent(pStalaggVictim, -101);

                            // stop moving for a moment
                            m_caster->GetMotionMaster()->Clear();
                            m_caster->GetMotionMaster()->MoveIdle();
                            unitTarget->GetMotionMaster()->Clear();
                            unitTarget->GetMotionMaster()->MoveIdle();
                        }
                    }
                    return;
                }
                case 54577:                                 // Throw U.D.E.D.
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Sometimes issues with explosion animation. Unclear why
                    // but possibly caused by the order of spells.

                    // Permanent Feign Death
                    unitTarget->CastSpell(unitTarget, 29266, true);

                    // need to despawn later
                    ((Creature*)unitTarget)->ForcedDespawn(2000);

                    // Mammoth Explosion Spell Spawner
                    unitTarget->CastSpell(unitTarget, 54581, true, m_CastItem);
                    return;
                }
                case 54850:                                 // Emerge
                {
                    // Cast Emerge summon
                    m_caster->CastSpell(m_caster, 54851, true);
                    return;
                }
                case 55004:                                 // Nitro Boosts
                {
                    if (!m_CastItem)
                        return;

                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (BattleGround* bg = ((Player*)m_caster)->GetBattleGround())
                        bg->EventPlayerDroppedFlag((Player*)m_caster);

                    if (roll_chance_i(95))                  // Nitro Boosts - success
                        m_caster->CastSpell(m_caster, 54861, true, m_CastItem);
                    else                                    // Knocked Up   - backfire 5%
                        m_caster->CastSpell(m_caster, 46014, true, m_CastItem);

                    return;
                }
                case 55818:                                 // Hurl Boulder
                {
                    // unclear how many summon min/max random, best guess below
                    uint32 random = urand(3,5);

                    for(uint32 i = 0; i < random; ++i)
                        m_caster->CastSpell(m_caster, 55528, true);

                    return;
                }
                case 56430:                                 // Arcane Bomb
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 56431, true, NULL, NULL, m_caster->GetObjectGuid());
                    unitTarget->CastSpell(unitTarget, 56432, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 57385:                                 // Argent Cannon
                case 57412:                                 // Reckoning Bomb
                {
                    if (!unitTarget || gameObjTarget)
                        return;

                    SpellEntry const* spellInfo = sSpellStore.LookupEntry(effect->CalculateSimpleValue());

                    // Init dest coordinates
                    float x,y,z;
                    x = m_targets.m_destX;
                    y = m_targets.m_destY;
                    z = m_targets.m_destZ;

                    MaNGOS::NormalizeMapCoord(x);
                    MaNGOS::NormalizeMapCoord(y);
                    m_caster->UpdateGroundPositionZ(x,y,z);

                    m_caster->CastSpell(x, y, z, spellInfo, false, NULL, NULL, m_originalCasterGUID);

                    return;
                }
                case 57496:                                 // Volazj - Insanity
                {
                    m_caster->CastSpell(m_caster, 57561, true);
                    return;
                }
                case 57908:                                 // Stain Cloth
                {
                    // nothing do more
                    finish();

                    m_caster->CastSpell(m_caster, 57915, false, m_CastItem);

                    // cast item deleted
                    ClearCastItem();
                    break;
                }
                case 57930:                                 // Arcane Lightning
                {
                    m_caster->CastSpell(m_caster, 57912, true);
                    return;
                }
                case 58418:                                 // Portal to Orgrimmar
                case 58420:                                 // Portal to Stormwind
                    return;                                 // implemented in EffectScript[0]
                case 58601:                                 // Remove Flight Auras
                {
                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_FLY);
                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED);
                    return;
                }
                case 58689:                                 // Rock Shards (Vault of Archavon, Archavon)
                {
                    m_caster->CastSpell(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_caster->GetMap()->IsRegularDifficulty() ? 58696 : 60884, true);
                    return;
                }
                case 58692:                                 // Rock Shards (Vault of Archavon, Archavon)
                {
                    m_caster->CastSpell(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_caster->GetMap()->IsRegularDifficulty() ? 58695 : 60883, true);
                    return;
                }
                case 59640:                                 // Underbelly Elixir
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1,3))
                    {
                        case 1: spell_id = 59645; break;
                        case 2: spell_id = 59831; break;
                        case 3: spell_id = 59843; break;
                    }

                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 60038:                                 // Arcane Lightning
                {
                    m_caster->CastSpell(m_caster, 58152, true);
                    return;
                }
                case 60932:                                 // Disengage (one from creature versions)
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget,60934,true,NULL);
                    return;
                }
                case 61698:
                {
                    if (!unitTarget)
                        return;

                    float horizontalSpeed = 20.0f + (40.0f - m_caster->GetDistance(unitTarget));
                    float verticalSpeed = 8.0f;
                    float angle = m_caster->GetAngle(unitTarget->GetPositionX(), m_caster->GetPositionY());
                    if (m_caster->GetPositionY() > 790.0f)
                        angle += M_PI_F;
                    // This method relies on the Dalaran Sewer map disposition and Water Spout position
                    // What we do is knock the player from a position exactly behind him and at the end of the pipe
                    m_caster->KnockBackWithAngle(angle, horizontalSpeed, verticalSpeed);
                }
                case 61784:                                 // Feast On Turkey
                case 61785:                                 // Feast On Cranberries
                case 61786:                                 // Feast On Sweet Potato
                case 61787:                                 // Feast On Pie
                case 61788:                                 // Feast On Stuffing
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(effect->CalculateSimpleValue());

                    VehicleKit* vehicle = unitTarget->GetVehicleKit();
                    if (!vehicle)
                        return;

                    Unit* unit = vehicle->GetPassenger(0);
                    if (!unit)
                        return;

                    unit->CastSpell(unit, 61826, true);     // On Plate - Eat Visual

                    uint32 buffId1; // Helpings buff
                    uint32 buffId2; // Stuffed buff
                    switch(m_spellInfo->Id)
                    {
                        case 61784: buffId1 = 61842; buffId2 = 66291; break;
                        case 61785: buffId1 = 61841; buffId2 = 66295; break;
                        case 61786: buffId1 = 61844; buffId2 = 66292; break;
                        case 61787: buffId1 = 61845; buffId2 = 66294; break;
                        case 61788: buffId1 = 61843; buffId2 = 66293; break;
                    }

                    unit->CastSpell(unit, buffId1, true);
                    if (SpellAuraHolder* holder = unit->GetSpellAuraHolder(buffId1))
                        if (holder->GetStackAmount() >= 5)
                            unit->CastSpell(unit, buffId2, true);

                    return;
                }
                case 62105:                                 // To'kini's Blowgun
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Sleeping Sleep
                    unitTarget->CastSpell(unitTarget, 62248, true);

                    // Although not really correct, it's needed to have access to m_caster later,
                    // to properly process spell 62110 (cast from gossip).
                    // Can possibly be replaced with a similar function that doesn't set any dynamic flags.
                    ((Creature*)unitTarget)->SetLootRecipient(m_caster);

                    unitTarget->setFaction(190);            // Ambient (neutral)
                    unitTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                    return;
                }
                case 62301:                                 // Cosmic Smash (Ulduar - Algalon)
                case 64598:
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 62295, true);
                    return;
                }
                case 62994:                                 // Foam Sword Defeat
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (Item* pItem = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                    {
                        uint32 itemId[5] = {45179, 45178, 45176, 45061, 45177};

                        for (uint8 i = 0; i < 5; ++i)
                        {
                            if (pItem->GetEntry() == itemId[i])
                            {
                                ((Player*)unitTarget)->DestroyItem(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND, true);
                                break;
                            }
                        }
                    }

                    return;
                }
                case 63820:                                 // Summon Scrap Bot Trigger (Ulduar - Mimiron) for Scrap Bots
                case 64425:                                 // Summon Scrap Bot Trigger (Ulduar - Mimiron) for Assault Bots
                case 64620:                                 // Summon Fire Bot Trigger  (Ulduar - Mimiron) for Fire Bots
                {
                    if (!unitTarget)
                        return;

                    uint32 triggerSpell = 0;
                    switch (m_spellInfo->Id)
                    {
                        case 63820: triggerSpell = 64398; break;
                        case 64425: triggerSpell = 64426; break;
                        case 64620: triggerSpell = 64621; break;
                    }
                    unitTarget->CastSpell(unitTarget, triggerSpell, false);
                    return;
                }
                case 63984:                                 // Hate to Zero (Ulduar - Yogg Saron)
                {
                    if (!unitTarget)
                        return;

                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    unitTarget->getHostileRefManager().deleteReferences();
                    return;
                }
                case 64172:                                 // Titanic Storm (Ulduar - Yogg Saron)
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->HasAura(64162))
                        unitTarget->DealDamage(unitTarget, unitTarget->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    return;
                }
                case 64385:                                 // Spinning (from Unusual Compass)
                {
                    m_caster->SetFacingTo(frand(0, M_PI_F*2));
                    return;
                }
                case 64981:                                 // Summon Random Vanquished Tentacle
                {
                    uint32 spell_id = 0;

                    switch(urand(0, 2))
                    {
                        case 0: spell_id = 64982; break;    // Summon Vanquished Crusher Tentacle
                        case 1: spell_id = 64983; break;    // Summon Vanquished Constrictor Tentacle
                        case 2: spell_id = 64984; break;    // Summon Vanquished Corruptor Tentacle
                    }

                    m_caster->CastSpell(m_caster, spell_id, true);
                    return;
                }
                case 65357:                                 // Throw Paper Zeppelin
                {
                    m_caster->CastSpell(unitTarget, 65351, true);
                    break;
                }
                case 66218:                                 // Launch - set position
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Amaru: update this
                    /*if (VehicleKit* vehicleKit = unitTarget->GetVehicleKit())
                    {
                        vehicleKit->SetDestination(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, unitTarget->GetOrientation(),  m_targets.GetSpeed(), m_targets.GetElevation());
                    }*/
                    return;
                }
                case 66250:                                 // Pass The Turkey
                case 66259:                                 // Pass The Stuffing
                case 66260:                                 // Pass The Pie
                case 66261:                                 // Pass The Cranberries
                case 66262:                                 // Pass The Sweet Potatoes
                {
                    if (!unitTarget)
                        return;

                    uint32 auraCheckSpell, throwSpell, achSpell;
                    switch(m_spellInfo->Id)
                    {
                        case 66250: auraCheckSpell = 61807; throwSpell = 61928; achSpell = 66373; break;
                        case 66259: auraCheckSpell = 61806; throwSpell = 61927; achSpell = 66375; break;
                        case 66260: auraCheckSpell = 61805; throwSpell = 61926; achSpell = 66374; break;
                        case 66261: auraCheckSpell = 61804; throwSpell = 61925; achSpell = 66372; break;
                        case 66262: auraCheckSpell = 61808; throwSpell = 61929; achSpell = 66376; break;
                    }

                    m_caster->CastSpell(unitTarget, auraCheckSpell, true);

                    if (VehicleKit* sourceVehicle = m_caster->GetVehicleKit())
                    {
                        if (Unit* source = sourceVehicle->GetPassenger(0))
                        {
                            source->CastSpell(source, achSpell, true);

                            if (VehicleKit* targetVehicle = unitTarget->GetVehicleKit())
                            {
                                if (Unit* target = targetVehicle->GetPassenger(0))
                                {
                                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(auraCheckSpell))
                                        if (holder->GetStackAmount() >= 5)
                                            source->CastSpell(target, throwSpell, true);
                                }
                            }
                        }
                    }

                    break;
                }
                case 66390:                                 // Read Last Rites
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Summon Tualiq Proxy
                    // Not known what purpose this has
                    unitTarget->CastSpell(unitTarget, 66411, true);

                    // Summon Tualiq Spirit
                    // Offtopic note: the summoned has aura from spell 37119 and 66419. One of them should
                    // most likely make summoned "rise", hover up/sideways in the air (MOVEFLAG_LEVITATING + MOVEFLAG_HOVER)
                    unitTarget->CastSpell(unitTarget, 66412, true);

                    ((Player*)m_caster)->KilledMonsterCredit(unitTarget->GetEntry(), unitTarget->GetObjectGuid());

                    // Must have a delay for proper spell animation
                    ((Creature*)unitTarget)->ForcedDespawn(1000);
                    return;
                }
                case 62653:                                 // Tidal Wave
                {
                     if (!unitTarget)
                        return;

                     m_caster->CastSpell(unitTarget, 62654, true);
                     return;
                }
                case 62935:                                 // Tidal Wave (H)
                {
                   if (!unitTarget)
                      return;

                   m_caster->CastSpell(unitTarget, 62936, true);
                   return;
                }
                case 67019:                                 // Flask of the North
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(m_caster->getClass())
                    {
                        case CLASS_WARRIOR:
                        case CLASS_DEATH_KNIGHT:
                            spell_id = 67018;               // STR for Warriors, Death Knights
                            break;
                        case CLASS_ROGUE:
                        case CLASS_HUNTER:
                            spell_id = 67017;               // AP for Rogues, Hunters
                            break;
                        case CLASS_PRIEST:
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            spell_id = 67016;               // SPD for Priests, Mages, Warlocks
                            break;
                        case CLASS_SHAMAN:
                            // random (SPD, AP)
                            spell_id = roll_chance_i(50) ? 67016 : 67017;
                            break;
                        case CLASS_PALADIN:
                        case CLASS_DRUID:
                        default:
                            // random (SPD, STR)
                            spell_id = roll_chance_i(50) ? 67016 : 67018;
                            break;
                    }
                    m_caster->CastSpell(m_caster, spell_id, true);
                    return;
                }
                case 67366:                                 // C-14 Gauss Rifle
                {
                    if (!unitTarget)
                        return;

                    Unit* pZerg = unitTarget->GetMiniPet();
                    if (pZerg && pZerg->isAlive() && pZerg->GetEntry() == 11327)
                    {
                        pZerg->GetMotionMaster()->MovementExpired();
                        m_caster->DealDamage(pZerg, unitTarget->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        ((Creature*)pZerg)->ForcedDespawn(5000);
                    }
                    return;
                }
                case 67400:                                 // Zergling Attack (on Grunty companion)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || !((Creature*)unitTarget)->IsPet())
                        return;

                    m_caster->DealDamage(unitTarget, unitTarget->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    ((Pet*)unitTarget)->Unsummon(PET_SAVE_AS_DELETED);
                    m_caster->GetMotionMaster()->MovementExpired();
                    return;
                }
                case 68576:                                 // Eject All Passengers (also used in encounters Lich King, Jaraxxus?)
                {
                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
                    return;
                }
                case 69922:                                 // Temper Quel'Delar
                {
                    if (!unitTarget)
                        return;

                    // Return Tempered Quel'Delar
                    unitTarget->CastSpell(m_caster, 69956, true);
                    return;
                }
                case 70769:                                 // Divine Storm! - Paladin T10 2P Bonus proc
                {
                    if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    ((Player*)m_caster)->RemoveSpellCooldown(53385, true);
                    return;
                }
                case 70895:                                 // Dark Transformation (Icecrown Citadel, Lady Deathwhisper encounter)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 70900, true);
                    break;
                }
                case 70896:                                 // Dark Empowerment (Icecrown Citadel, Lady Deathwhisper encounter)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 70901, true);
                    break;
                }
                case 70897:                                 // Dark Martyrdom (Icecrown Citadel, Lady Deathwhisper encounter)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    switch (unitTarget->GetEntry())
                    {
                        case 37949:                         // Cult Adherent
                            unitTarget->CastSpell(unitTarget, 70903, false);
                            break;
                        case 37890:                         // Cult Fanatic
                            unitTarget->CastSpell(unitTarget, 71236, false);
                            break;
                    }
                    break;
                }
                case 71390:                                 // Pact of darkfallen
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 71341, true);
                    break;
                }
                case 71445:                                 // Twilight Bloodbolt
                case 71471:                                 // Twilight Bloodbolt
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 71818, true);
                    return;
                }
                case 71718:                                 // Conjure Flame
                case 72040:                                 // Conjure Empowered Flame
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    return;
                }
                //case 71837:                                 // Vampiric Bite
                //{
                //    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                //        return;

                //    m_caster->CastSpell(unitTarget, 71726, true);
                //    return;
                //}
                // check scriptdev
                //case 71861:                                 // Swarming Shadows
                //{
                //    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                //        return;

                //    m_caster->CastSpell(unitTarget, 71264, true);
                //    return;
                //}
                case 72202:                                 // Blade power
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 72195, true);
                    break;
                }
                case 72261:                                 // Delirious Slash
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, m_caster->CanReachWithMeleeAttack(unitTarget) ? 71623 : 72264, true);
                    return;
                }
                case 72752:                                 // Will of the Forsaken Cooldown Trigger
                case 72757:                                 // Will of the Forsaken Cooldown Trigger (WotF)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player* target = (Player*)unitTarget;
                    target->AddSpellCooldown(m_spellInfo->Id, 0, time(NULL) + m_spellInfo->GetCategoryRecoveryTime() / IN_MILLISECONDS);
                    WorldPacket data(SMSG_SPELL_COOLDOWN, 8 + 1 + 4 + 4);
                    data << target->GetObjectGuid();
                    data << uint8(0);
                    data << uint32(m_spellInfo->Id);
                    data << uint32(0);
                    target->GetSession()->SendPacket(&data);
                    return;
                }
                case 74452:                                 // Conflagration
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, 74453, true);
                    m_caster->CastSpell(unitTarget, 74454, true, NULL, NULL, m_caster->GetObjectGuid(), m_spellInfo);
                    return;
                }
                case 77762:                                 // Lava Surge!
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                        ((Player*)unitTarget)->RemoveSpellCooldown(51505, true);

                    // Item - Shaman T12 Elemental 4P Bonus
                    if (unitTarget->HasAura(99206))
                        unitTarget->CastSpell(unitTarget, 99207, true);
                    return;
                }
                case 79637:                                 // Flask of Enhancement
                {
                    uint32 spell =79638;                    // Enhanced Strength
                    Stats stat = STAT_STRENGTH;
                    if (m_caster->GetTotalStatValue(STAT_AGILITY) > m_caster->GetTotalStatValue(stat))
                    {
                        stat = STAT_AGILITY;
                        spell = 79639;                      // Enhanced Agility
                    }
                    if (m_caster->GetTotalStatValue(STAT_INTELLECT) > m_caster->GetTotalStatValue(stat))
                    {
                        stat = STAT_INTELLECT;
                        spell = 79640;                      // Enhanced Intellect
                    }

                    m_caster->CastSpell(m_caster, spell, true);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch(m_spellInfo->Id)
            {
                case 1459:                                  // Arcane Brilliance
                {
                    Unit* target = unitTarget;
                    if (!target)
                        target = m_caster;

                    uint32 spellId;
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || target->GetTypeId() != TYPEID_PLAYER
                        || ((Player*)m_caster)->GetGroup() != ((Player*)target)->GetGroup())
                        spellId = 79057;
                    else
                        spellId = 79058;

                    m_caster->CastSpell(target, spellId, true);
                    return;
                }
                case 11958:                                 // Cold Snap
                {
                    if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    // immediately finishes the cooldown on Frost spells
                    const SpellCooldowns& cm = ((Player *)m_caster)->GetSpellCooldownMap();
                    for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
                    {
                        SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);

                        if (spellInfo->GetSpellFamilyName() == SPELLFAMILY_MAGE &&
                            (GetSpellSchoolMask(spellInfo) & SPELL_SCHOOL_MASK_FROST) &&
                            spellInfo->Id != 11958 && GetSpellRecoveryTime(spellInfo) > 0)
                        {
                            ((Player*)m_caster)->RemoveSpellCooldown((itr++)->first, true);
                        }
                        else
                            ++itr;
                    }
                    return;
                }
                /// Adonai, bug #34 spell 3
                case 55342:                                 // Mirror Image
                {
                    m_caster->CastSpell(m_caster, 58831, true); // Image 1
                    m_caster->CastSpell(m_caster, 58833, true); // Image 2
                    m_caster->CastSpell(m_caster, 58834, true); // Image 3
                    return;
                }
                case 32826:                                 // Polymorph Cast Visual
                {
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
                    {
                        //Polymorph Cast Visual Rank 1
                        const uint32 spell_list[6] =
                        {
                            32813,                          // Squirrel Form
                            32816,                          // Giraffe Form
                            32817,                          // Serpent Form
                            32818,                          // Dragonhawk Form
                            32819,                          // Worgen Form
                            32820                           // Sheep Form
                        };
                        unitTarget->CastSpell( unitTarget, spell_list[urand(0, 5)], true);
                    }
                    return;
                }
                case 38194:                                 // Blink
                {
                    // Blink
                    if (unitTarget)
                        m_caster->CastSpell(unitTarget, 38203, true);

                    return;
                }
                case 42955:                                 // Conjure Refreshment
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (m_caster->getLevel() > 84)
                        m_caster->CastSpell(unitTarget, 92727, true); // conjure refreshment, rank 7
                    else if (m_caster->getLevel() > 79)
                        m_caster->CastSpell(unitTarget, 42956, true); // conjure refreshment, rank 6
                    else if (m_caster->getLevel() > 74)
                        m_caster->CastSpell(unitTarget, 74625, true); // conjure refreshment, rank 5
                    else if (m_caster->getLevel() > 64)
                        m_caster->CastSpell(unitTarget, 92805, true); // conjure refreshment, rank 4
                    else if (m_caster->getLevel() > 54)
                        m_caster->CastSpell(unitTarget, 92802, true); // conjure refreshment, rank 3
                    else if (m_caster->getLevel() > 44)
                        m_caster->CastSpell(unitTarget, 92799, true); // conjure refreshment, rank 2
                    else if (m_caster->getLevel() > 38)
                        m_caster->CastSpell(unitTarget, 92739, true); // conjure refreshment, rank 1
                    return;
                }
                case 43987:                                 // Ritual of Refreshment
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (m_caster->getLevel() < 80)
                        m_caster->CastSpell(unitTarget, 74650, true);   // Ritual of Refreshment (Rank 1)
                    else if (m_caster->getLevel() < 85)
                        m_caster->CastSpell(unitTarget, 92824, true);   // Ritual of Refreshment (Rank 2)
                    else
                        m_caster->CastSpell(unitTarget, 92827, true);   // Ritual of Refreshment (Rank 3)
                    return;
                }
                case 61316:                                 // Dalaran Brilliance
                {
                    Unit* target = unitTarget;
                    if (!target)
                        target = m_caster;

                    uint32 spellId;
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || target->GetTypeId() != TYPEID_PLAYER
                        || ((Player*)m_caster)->GetGroup() != ((Player*)target)->GetGroup())
                        spellId = 79039;
                    else
                        spellId = 79038;

                    m_caster->CastSpell(target, spellId, true);
                    return;
                }
                case 82731:                                 // Flame Orb (summon)
                {
                    m_caster->CastSpell(m_caster, 84765, true);         // Flame Orb
                    return;
                }
                case 82734:                                 // Flame Orb Periodic Trigger Dummy
                {
                    if (unitTarget)
                    {
                        m_caster->CastSpell(unitTarget, 86719, true, NULL, NULL, m_caster->GetCreatorGuid());   // orb visual
                        m_caster->CastSpell(unitTarget, 82739, true, NULL, NULL, m_caster->GetCreatorGuid());   // Frostfire Orb DMG spell
                    }
                    return;
                }
                case 84718:                                 // Frostfire Orb Periodic Trigger Dummy
                {
                    if (unitTarget)
                    {
                        m_caster->CastSpell(unitTarget, 86719, true, NULL, NULL, m_caster->GetCreatorGuid());   // orb visual

                        Unit* creator = m_caster->GetCreator();
                        // Frostfire Orb (Rank 2)
                        if (creator && creator->HasAura(84727))
                            m_caster->CastSpell(unitTarget, 84721, true, NULL, NULL, m_caster->GetCreatorGuid()); // Frostfire Orb DMG spell with chill
                        else
                            m_caster->CastSpell(unitTarget, 95969, true, NULL, NULL, m_caster->GetCreatorGuid()); // Frostfire Orb DMG spell
                    }
                    return;
                }
                case 92283:                                 // Frostfire Orb (summon)
                {
                    m_caster->CastSpell(m_caster, 84714, true);         // Frostfire Orb
                    return;
                }
                case 96934:                                 // Blessing of Khaz'goroth
                case 97127:                                 // Blessing of Khaz'goroth
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player* player = (Player*)unitTarget;

                    // Titanic Power
                    SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(96923);
                    if (!holder)
                        return;

                    int32 bp = damage * holder->GetStackAmount();

                    uint32 spells[] = { 96927, 96928, 96929 };
                    for (int i = 0; i < 3; ++i)
                        unitTarget->RemoveAurasDueToSpell(spells[i]);

                    uint32 triggered_spell_id = 96927;
                    CombatRating cr = CR_HASTE_MELEE;
                    if (player->GetRatingBonusValue(CR_CRIT_MELEE) > player->GetRatingBonusValue(cr))
                    {
                        cr = CR_CRIT_MELEE;
                        triggered_spell_id = 96928;
                    }
                    if (player->GetRatingBonusValue(CR_MASTERY) > player->GetRatingBonusValue(cr))
                        triggered_spell_id = 96929;

                    unitTarget->CastCustomSpell(unitTarget, triggered_spell_id, &bp, NULL, NULL, true, m_CastItem);
                    return;
                }
                case 105065:                                // Seething Hate
                case 108090:                                // Seething Hate (H)
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 105067, true);
                    return;
                }
                case 106248:                                // Blade Dance
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 105726, true);
                    return;
                }
            }

            // Conjure Mana Gem
            if (effect->EffectIndex == EFFECT_INDEX_1 && m_spellInfo->GetSpellEffectIdByIndex(EFFECT_INDEX_0) == SPELL_EFFECT_CREATE_ITEM)
            {
                if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                    return;

                // checked in create item check, avoid unexpected
                if (Item* item = ((Player*)m_caster)->GetItemByLimitedCategory(ITEM_LIMIT_CATEGORY_MANA_GEM))
                    if (item->HasMaxCharges())
                        return;

                unitTarget->CastSpell( unitTarget, effect->CalculateSimpleValue(), true, m_CastItem);
                return;
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            SpellClassOptionsEntry const* warClassOptions = m_spellInfo->GetSpellClassOptions();
            // Charge
            if (warClassOptions && (warClassOptions->SpellFamilyFlags & UI64LIT(0x1)) && m_spellInfo->SpellVisual[0] == 867)
            {
                int32 chargeBasePoints0 = damage;
                m_caster->CastCustomSpell(m_caster, 34846, &chargeBasePoints0, NULL, NULL, true);
                return;
            }
            // Slam
            if (warClassOptions && warClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000200000))
            {
                if (!unitTarget)
                    return;

                // dummy cast itself ignored by client in logs
                m_caster->CastCustomSpell(unitTarget, 50782, &damage, NULL, NULL, true);
                return;
            }

            switch(m_spellInfo->Id)
            {
                // Warrior's Wrath
                case 21977:
                {
                    if (!unitTarget)
                        return;
                    m_caster->CastSpell(unitTarget, 21887, true); // spell mod
                    return;
                }
                // Last Stand
                case 12975:
                {
                    int32 healthModSpellBasePoints0 = int32(m_caster->GetMaxHealth()*0.3);
                    m_caster->CastCustomSpell(m_caster, 12976, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
                    return;
                }
                // Bloodthirst
                case 23881:
                {
                    m_caster->CastCustomSpell(unitTarget, 23885, &damage, NULL, NULL, true, NULL);
                    return;
                }
                // Rallying Cry
                case 97462:
                {
                    int32 bp = int32(damage * unitTarget->GetMaxHealth() / 100.0f);
                    m_caster->CastCustomSpell(unitTarget, 97463, &bp, NULL, NULL, true);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            SpellClassOptionsEntry const* wrlClassOptions = m_spellInfo->GetSpellClassOptions();
            // Life Tap
            if (wrlClassOptions && wrlClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000040000))
            {
                damage = (unitTarget->GetMaxHealth() * m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_2) / 100.0f);
                if (unitTarget && (int32(unitTarget->GetHealth()) > damage))
                {
                    // Shouldn't Appear in Combat Log
                    unitTarget->ModifyHealth(-damage);

                    int32 mana = int32(damage * m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100.0f);

                    // Improved Life Tap mod
                    Unit::AuraList const& auraDummy = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = auraDummy.begin(); itr != auraDummy.end(); ++itr)
                        if((*itr)->GetSpellProto()->GetSpellFamilyName()==SPELLFAMILY_WARLOCK && (*itr)->GetSpellProto()->SpellIconID == 208)
                            mana = (((*itr)->GetModifier()->m_amount + 100) * mana / 100.0f);

                    m_caster->CastCustomSpell(unitTarget, 31818, &mana, NULL, NULL, true);

                    // Mana Feed
                    int32 manaFeedVal = 0;
                    Unit::AuraList const& mod = m_caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
                    for (Unit::AuraList::const_iterator itr = mod.begin(); itr != mod.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->GetSpellFamilyName()==SPELLFAMILY_WARLOCK && (*itr)->GetSpellProto()->SpellIconID == 1982 && (*itr)->GetEffIndex() == EFFECT_INDEX_0)
                        {
                            manaFeedVal += (*itr)->GetModifier()->m_amount;
                            break;
                        }
                    }
                    if (manaFeedVal > 0)
                    {
                        manaFeedVal = manaFeedVal * mana / 100;
                        m_caster->CastCustomSpell(m_caster, 32553, &manaFeedVal, NULL, NULL, true, NULL);
                    }
                }
                else
                    SendCastResult(SPELL_FAILED_FIZZLE);

                return;
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            SpellClassOptionsEntry const* prtsClassOptions = m_spellInfo->GetSpellClassOptions();
            // Penance
            if (prtsClassOptions && prtsClassOptions->SpellFamilyFlags & UI64LIT(0x0080000000000000))
            {
                if (!unitTarget)
                    return;

                int hurt = 0;
                int heal = 0;
                switch(m_spellInfo->Id)
                {
                    case 47540: hurt = 47758; heal = 47757; break;
                    default:
                        ERROR_LOG("Spell::EffectDummy: Spell %u Penance need set correct heal/damage spell", m_spellInfo->Id);
                        return;
                }

                // prevent interrupted message for main spell
                finish(true);

                // replace cast by selected spell, this also make it interruptible including target death case
                if (m_caster->IsFriendlyTo(unitTarget))
                    m_caster->CastSpell(unitTarget, heal, false);
                else
                    m_caster->CastSpell(unitTarget, hurt, false);

                return;
            }
            // Dispel Magic
            if (m_spellInfo->Id == 527)
            {
                if (!unitTarget)
                    return;

                uint32 spellId = 97691;
                if (!unitTarget->IsHostileTo(m_caster))
                {
                    spellId = 97690;
                    // check Absolution Talent
                    if (unitTarget != m_caster && !m_caster->HasAura(33167))
                        unitTarget = m_caster;
                }

                // cast actual dispel spell
                m_caster->CastSpell(unitTarget, spellId, true);
            }
            // Leap of Faith
            else if (m_spellInfo->Id == 73325)
            {
                if (!unitTarget)
                    return;

                // cast jump spell
                unitTarget->CastSpell(m_caster, 92832, true, NULL, NULL, m_caster->GetObjectGuid());
            }
            // Power Word: Fortitude
            else if (m_spellInfo->Id == 21562)
            {
                Unit* target = unitTarget;
                if (!target)
                    target = m_caster;

                uint32 spellId;
                if (m_caster->GetTypeId() != TYPEID_PLAYER || target->GetTypeId() != TYPEID_PLAYER
                    || ((Player*)m_caster)->GetGroup() != ((Player*)target)->GetGroup())
                    spellId = 79104;
                else
                    spellId = 79105;

                m_caster->CastSpell(target, spellId, true);
                return;
            }
            // Shadow Protection
            else if (m_spellInfo->Id == 27683)
            {
                Unit* target = unitTarget;
                if (!target)
                    target = m_caster;

                uint32 spellId;
                if (m_caster->GetTypeId() != TYPEID_PLAYER || target->GetTypeId() != TYPEID_PLAYER
                    || ((Player*)m_caster)->GetGroup() != ((Player*)target)->GetGroup())
                    spellId = 79106;
                else
                    spellId = 79107;

                m_caster->CastSpell(target, spellId, true);
                return;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Starfall
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000000000000000), 0x00000100))
            {
                //Shapeshifting into an animal form or mounting cancels the effect.
                if(m_caster->GetCreatureType() == CREATURE_TYPE_BEAST || m_caster->IsMounted())
                {
                    if(m_triggeredByAuraSpell)
                        m_caster->RemoveAurasDueToSpell(m_triggeredByAuraSpell->Id);
                    return;
                }

                //Any effect which causes you to lose control of your character will supress the starfall effect.
                if (m_caster->hasUnitState(UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL))
                    return;

                switch (m_spellInfo->Id)
                {
                    case 50286: m_caster->CastSpell(unitTarget, 50288, true); return;
                    default:
                        ERROR_LOG("Spell::EffectDummy: Unhandeled Starfall spell rank %u",m_spellInfo->Id);
                        return;
                }
            }

            switch(m_spellInfo->Id)
            {
                // Mark of the Wild
                case 1126:
                {
                    Unit* target = unitTarget;
                    if (!target)
                        target = m_caster;
    
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->CastSpell(unitTarget, ((Player*)m_caster)->GetGroup() != ((Player*)unitTarget)->GetGroup() ? 79060 : 79061, true);
                    return;
                }
                // Skull Bash
                case 80964:
                case 80965:
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 93983, true);   // charge
                    m_caster->CastSpell(unitTarget, 93985, true);   // interrupt

                    // search Brutal Impact
                    Unit::AuraList const& flatModAuras = m_caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
                    for (Unit::AuraList::const_iterator itr = flatModAuras.begin(); itr != flatModAuras.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->SpellIconID == 473 && (*itr)->GetEffIndex() == EFFECT_INDEX_0 &&
                            (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DRUID)
                        {
                            // cast mana cost increase spell
                            m_caster->CastSpell(unitTarget, (*itr)->GetId() == 16940 ? 82364 : 82365, true);
                            break;
                        }
                    }
                    return;
                }
                // Wild Mushroom : Detonate
                case 88751:
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 fungal = 0;
                    if (m_caster->HasAura(78788)) // Fungal Growth Rank 1
                        fungal = 81291;
                    else if (m_caster->HasAura(78789)) // Fungal Growth Rank 2
                        fungal = 81283;

                    SummonUnitList& mushroomList = ((Player*)m_caster)->GetSummonUnitList();
                    SummonUnitList tempList;

                    float spellRange = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex), true);
    
                    for (SummonUnitList::const_iterator i = mushroomList.begin(); i != mushroomList.end(); ++i)
                    {
                        if ((*i)->isAlive() && (*i)->GetEntry() == 47649)
                        {
                            tempList.push_back(*i);
                            if (tempList.size() >= 3) // max 3 mushroom (buggers?)
                                break;
                        }
                    }

                    for (SummonUnitList::const_iterator i = tempList.begin(); i != tempList.end(); ++i)
                    {
                        if (!m_caster->IsWithinDist3d((*i)->GetPositionX(), (*i)->GetPositionY(), (*i)->GetPositionZ(), spellRange))
                            continue;

                        (*i)->SetVisibility(VISIBILITY_ON);
                        (*i)->CastSpell((*i), 92701, true); // Detonate Death Visual
                        (*i)->CastSpell((*i), 92853, true); // Explosion visual and suicide
                        m_caster->CastSpell((*i)->GetPositionX(), (*i)->GetPositionY(), (*i)->GetPositionZ(), 78777, true); // damage

                        if (fungal)
                            m_caster->CastSpell((*i)->GetPositionX(), (*i)->GetPositionY(), (*i)->GetPositionZ(), fungal, true); // Summoning fungal growth
                    }
                }
            }

            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch(m_spellInfo->Id)
            {
                case 5938:                                  // Shiv
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player *pCaster = ((Player*)m_caster);

                    Item *item = pCaster->GetWeaponForAttack(OFF_ATTACK);
                    if (!item)
                        return;

                    m_caster->CastSpell(unitTarget, 5940, true);

                    // all poison enchantments is temporary
                    uint32 enchant_id = item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT);
                    if (!enchant_id)
                        return;

                    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if (!pEnchant)
                        return;

                    for (int s = 0; s < 3; ++s)
                    {
                        if (pEnchant->type[s] != ITEM_ENCHANTMENT_TYPE_COMBAT_SPELL)
                            continue;

                        SpellEntry const* combatEntry = sSpellStore.LookupEntry(pEnchant->spellid[s]);
                        if (!combatEntry || combatEntry->GetDispel() != DISPEL_POISON)
                            continue;

                        m_caster->CastSpell(unitTarget, combatEntry, true, item);
                    }

                    return;
                }
                case 14185:                                 // Preparation
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Glyph of Preparation
                    Aura* glyph = m_caster->GetDummyAura(56819);

                    // immediately finishes the cooldown on certain Rogue abilities
                    const SpellCooldowns& cm = ((Player *)m_caster)->GetSpellCooldownMap();
                    for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end(); )
                    {
                        switch (itr->first)
                        {
                            case 1856:  // Vanish
                            case 2983:  // Sprint
                            case 36554: // Shadowstep
                                ((Player*)m_caster)->RemoveSpellCooldown((itr++)->first, true);
                                break;
                            case 1766:  // Kick
                            case 51722: // Dismantle
                            case 76577: // Smoke Bomb
                                if (glyph)
                                    ((Player*)m_caster)->RemoveSpellCooldown((itr++)->first, true);
                                else
                                    ++itr;
                                break;
                            default:
                                ++itr;
                        }
                    }
                    return;
                }
                case 31231:                                 // Cheat Death
                {
                    // Cheating Death
                    m_caster->CastSpell(m_caster, 45182, true);
                    return;
                }
                case 51662:                                 // Hunger for Blood
                {
                    m_caster->CastSpell(m_caster, 63848, true);
                    return;
                }
                case 51690:                                 // Killing Spree
                {
                    m_caster->CastSpell(m_caster, 61851, true);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            SpellClassOptionsEntry const* huntClassOptions = m_spellInfo->GetSpellClassOptions();
            // Disengage
            if (huntClassOptions && huntClassOptions->SpellFamilyFlags & UI64LIT(0x0000400000000000))
            {
                Unit* target = unitTarget;
                uint32 spellid;
                switch(m_spellInfo->Id)
                {
                    case 57635: spellid = 57636; break;     // one from creature cases
                    case 61507: spellid = 61508; break;     // one from creature cases
                    default:
                        ERROR_LOG("Spell %u not handled propertly in EffectDummy(Disengage)",m_spellInfo->Id);
                        return;
                }
                if (!target || !target->isAlive())
                    return;
                m_caster->CastSpell(target,spellid,true,NULL);
            }

            switch(m_spellInfo->Id)
            {
                case 23989:                                 // Readiness talent
                {
                    if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    //immediately finishes the cooldown for hunter abilities
                    const SpellCooldowns& cm = ((Player*)m_caster)->GetSpellCooldownMap();
                    for (SpellCooldowns::const_iterator itr = cm.begin(); itr != cm.end();)
                    {
                        SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);

                        if (spellInfo->GetSpellFamilyName() == SPELLFAMILY_HUNTER && spellInfo->Id != 23989 && GetSpellRecoveryTime(spellInfo) > 0 )
                            ((Player*)m_caster)->RemoveSpellCooldown((itr++)->first,true);
                        else
                            ++itr;
                    }
                    return;
                }
                case 37506:                                 // Scatter Shot
                {
                    if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    // break Auto Shot and autohit
                    m_caster->InterruptSpell(CURRENT_AUTOREPEAT_SPELL);
                    m_caster->AttackStop();
                    ((Player*)m_caster)->SendAttackSwingCancelAttack();
                    return;
                }
                case 51753:                                 // Camouflage
                {
                    m_caster->CastSpell(m_caster, 51755, true);
                    if (Pet* pet = m_caster->GetPet())
                        pet->CastSpell(pet, 51755, true);
                    return;
                }
                case 53478:                                 // Last Stand
                {
                    if (!unitTarget)
                        return;
                    int32 healthModSpellBasePoints0 = int32(unitTarget->GetMaxHealth() * 0.3);
                    unitTarget->CastCustomSpell(unitTarget, 53479, &healthModSpellBasePoints0, NULL, NULL, true, NULL);
                    return;
                }
                case 53271:                                 // Master's Call
                {
                    Pet* pet = m_caster->GetPet();
                    if (!pet || !unitTarget)
                        return;

                    pet->CastSpell(unitTarget, damage, true);
                    return;
                }
                case 56641:                                 // Steady Shot
                {
                    if (SpellEntry const* spellEntry = sSpellStore.LookupEntry(77443))
                    {
                        int32 bp = spellEntry->CalculateSimpleValue(EFFECT_INDEX_0);
                        // Item - Hunter T13 2P Bonus (Steady Shot and Cobra Shot)
                        if (m_caster->HasAura(105732))
                            bp *= 2;

                        if (unitTarget)
                        {
                            // search Termination
                            Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                            for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                            {
                                if ((*itr)->GetSpellProto()->SpellIconID == 2008 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_HUNTER)
                                {
                                    if (unitTarget->GetHealthPercent() < (*itr)->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1))
                                        bp += (*itr)->GetModifier()->m_amount;
                                    break;
                                }
                            }

                            // Glyph of Dazzled Prey
                            if (Aura* glyph = m_caster->GetAura(56856, EFFECT_INDEX_0))
                            {
                                // check dazed affect
                                Unit::AuraList const& decSpeedList = unitTarget->GetAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
                                for(Unit::AuraList::const_iterator iter = decSpeedList.begin(); iter != decSpeedList.end(); ++iter)
                                {
                                    if ((*iter)->GetSpellProto()->SpellIconID==15 && (*iter)->GetSpellProto()->GetDispel() == 0)
                                    {
                                        bp += glyph->GetModifier()->m_amount;
                                        break;
                                    }
                                }
                            }
                        }

                        // energize
                        m_caster->CastCustomSpell(m_caster, spellEntry, &bp, NULL, NULL, true);
                    }
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Judgement (seal trigger)
            if (m_spellInfo->GetCategory() == SPELLCATEGORY_JUDGEMENT)
            {
                if (!unitTarget || !unitTarget->isAlive())
                    return;

                uint32 damageSpell;
                // Seal of Righteousness
                if (m_caster->HasAura(20154))
                    damageSpell = 20187;        // Judgement of Righteousness
                // Seal of Truth
                else if (m_caster->HasAura(31801))
                    damageSpell = 31804;        // Judgement of Truth
                else
                    damageSpell = 54158;        // Judgement

                m_caster->CastSpell(unitTarget, damageSpell, true, NULL, NULL, m_caster->GetObjectGuid(), m_spellInfo);

                // Communion
                if (m_caster->HasAura(31876))
                    m_caster->CastSpell(m_caster, 57669, true); // Replenishment

                // Long Arm of the Law
                Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->SpellIconID == 3013 &&
                        (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_PALADIN)
                    {
                        if (roll_chance_i((*itr)->GetModifier()->m_amount) && m_caster->GetDistance2d(unitTarget) >= 15.0f)
                            m_caster->CastSpell(m_caster, 87173, true);
                        break;
                    }
                }

                return;
            }

            switch (m_spellInfo->SpellIconID)
            {
                case 156:                                   // Holy Shock
                {
                    if (!unitTarget)
                        return;

                    int hurt = 0;
                    int heal = 0;

                    switch (m_spellInfo->Id)
                    {
                        case 20473: hurt = 25912; heal = 25914; break;
                        default:
                            ERROR_LOG("Spell::EffectDummy: Spell %u not handled in HS",m_spellInfo->Id);
                            return;
                    }

                    if (m_caster->IsFriendlyTo(unitTarget))
                        m_caster->CastSpell(unitTarget, heal, true);
                    else
                        m_caster->CastSpell(unitTarget, hurt, true);

                    return;
                }
                case 561:                                   // Judgement of command
                {
                    if (!unitTarget)
                        return;

                    uint32 spell_id = m_currentBasePoints[effect->EffectIndex];
                    SpellEntry const* spell_proto = sSpellStore.LookupEntry(spell_id);
                    if (!spell_proto)
                        return;

                    m_caster->CastSpell(unitTarget, spell_proto, true, NULL);
                    return;
                }
            }

            switch(m_spellInfo->Id)
            {
                case 19740:                                 // Blessing of Might
                {
                    Unit* target = unitTarget;
                    if (!target)
                        target = m_caster;

                    uint32 spellId;
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || target->GetTypeId() != TYPEID_PLAYER
                        || ((Player*)m_caster)->GetGroup() != ((Player*)target)->GetGroup())
                        spellId = 79101;
                    else
                        spellId = 79102;

                    m_caster->CastSpell(target, spellId, true);
                    return;
                }
                case 20217:                                 // Blessing of Kings
                {
                    Unit* target = unitTarget;
                    if (!target)
                        target = m_caster;

                    uint32 spellId;
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || target->GetTypeId() != TYPEID_PLAYER
                        || ((Player*)m_caster)->GetGroup() != ((Player*)target)->GetGroup())
                        spellId = 79062;
                    else
                        spellId = 79063;

                    m_caster->CastSpell(target, spellId, true);
                    return;
                }
                case 31789:                                 // Righteous Defense (step 1)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    {
                        SendCastResult(SPELL_FAILED_TARGET_AFFECTING_COMBAT);
                        return;
                    }

                    // 31989 -> dummy effect (step 1) + dummy effect (step 2) -> 31709 (taunt like spell for each target)
                    Unit* friendTarget = !unitTarget || unitTarget->IsFriendlyTo(m_caster) ? unitTarget : unitTarget->getVictim();
                    if (friendTarget)
                    {
                        Player* player = friendTarget->GetCharmerOrOwnerPlayerOrPlayerItself();
                        if (!player || !player->IsInSameRaidWith((Player*)m_caster))
                            friendTarget = NULL;
                    }

                    // non-standard cast requirement check
                    if (!friendTarget || friendTarget->getAttackers().empty())
                    {
                        ((Player*)m_caster)->RemoveSpellCooldown(m_spellInfo->Id,true);
                        SendCastResult(SPELL_FAILED_TARGET_AFFECTING_COMBAT);
                        return;
                    }

                    // Righteous Defense (step 2) (in old version 31980 dummy effect)
                    // Clear targets for eff 1
                    for(TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        ihit->effectMask &= ~(1<<1);

                    // not empty (checked), copy
                    Unit::AttackerSet attackers = friendTarget->getAttackers();

                    // selected from list 3
                    for(uint32 i = 0; i < std::min(size_t(3),attackers.size()); ++i)
                    {
                        Unit::AttackerSet::iterator aItr = attackers.begin();
                        std::advance(aItr, rand() % attackers.size());
                        AddUnitTarget((*aItr), EFFECT_INDEX_1);
                        attackers.erase(aItr);
                    }

                    // now let next effect cast spell at each target.
                    return;
                }
                case 37877:                                 // Blessing of Faith
                {
                    if (!unitTarget)
                        return;

                    uint32 spell_id = 0;
                    switch(unitTarget->getClass())
                    {
                        case CLASS_DRUID:   spell_id = 37878; break;
                        case CLASS_PALADIN: spell_id = 37879; break;
                        case CLASS_PRIEST:  spell_id = 37880; break;
                        case CLASS_SHAMAN:  spell_id = 37881; break;
                        default: return;                    // ignore for not healing classes
                    }

                    m_caster->CastSpell(m_caster, spell_id, true);
                    return;
                }
                case 86150:                                 // Guardian of Ancient Kings
                {
                    if (!unitTarget)
                        return;

                    if (m_caster->HasSpell(20473))          // Holy Shock
                        m_caster->CastSpell(m_caster, 86669, true);
                    else if (m_caster->HasSpell(31935))     // Avenger's Shield
                        m_caster->CastSpell(m_caster, 86659, true);
                    else if (m_caster->HasSpell(85256))     // Templar's Verdict
                        m_caster->CastSpell(m_caster, 86698, true);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            SpellClassOptionsEntry const* shamClassOptions = m_spellInfo->GetSpellClassOptions();
            // Cleansing Totem
            if (shamClassOptions && (shamClassOptions->SpellFamilyFlags & UI64LIT(0x0000000004000000)) && m_spellInfo->SpellIconID==1673)
            {
                if (unitTarget)
                    m_caster->CastSpell(unitTarget, 52025, true);
                return;
            }
            // Healing Stream Totem
            if (shamClassOptions && shamClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000002000))
            {
                if (unitTarget)
                {
                    if (Unit *owner = m_caster->GetOwner())
                    {
                        // spell have SPELL_DAMAGE_CLASS_NONE and not get bonuses from owner, use main spell for bonuses
                        if (m_triggeredBySpellInfo)
                        {
                            damage = int32(owner->SpellHealingBonusDone(unitTarget, m_triggeredBySpellInfo, damage, HEAL));
                            damage = int32(unitTarget->SpellHealingBonusTaken(owner, m_triggeredBySpellInfo, damage, HEAL));
                        }
                    }
                    m_caster->CastCustomSpell(unitTarget, 52042, &damage, NULL, NULL, true, 0, 0, m_originalCasterGUID);
                }
                return;
            }
            // Flametongue Weapon Proc, Ranks
            if (shamClassOptions && shamClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000200000))
            {
                if (!m_CastItem)
                {
                    ERROR_LOG("Spell::EffectDummy: spell %i requires cast Item", m_spellInfo->Id);
                    return;
                }
                // found spelldamage coefficients of 0.381% per 0.1 speed and 15.244 per 4.0 speed
                // but own calculation say 0.385 gives at most one point difference to published values
                int32 spellDamage = m_caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(m_spellInfo));
                float weaponSpeed = (1.0f/IN_MILLISECONDS) * m_CastItem->GetProto()->Delay;
                int32 totalDamage = int32((damage + 3.85f * spellDamage) * 0.01 * weaponSpeed);

                m_caster->CastCustomSpell(unitTarget, 10444, &totalDamage, NULL, NULL, true, m_CastItem);
                return;
            }
            // Unleash Elements
            if (m_spellInfo->Id == 73680)
            {
                if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER)
                    return;

                uint8 slots[2]= { EQUIPMENT_SLOT_MAINHAND, EQUIPMENT_SLOT_OFFHAND };
                for (uint8 i = 0; i < 2; ++i)
                {
                    if (Item* item = ((Player*)m_caster)->GetItemByPos(INVENTORY_SLOT_BAG_0, slots[i]))
                        if (uint32 ench = item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
                        {
                            uint32 triggered_spell = 0;
                            switch (ench)
                            {
                                case 2:         // Frostbrand Weapon
                                    triggered_spell = 73682;
                                    break;
                                case 5:         // Flametongue Weapon
                                    triggered_spell = 73683;
                                    break;
                                case 283:       // Windfury Weapon
                                    triggered_spell = 73681;
                                    break;
                                case 3021:      // Rockbiter Weapon
                                    triggered_spell =73684;
                                    break;
                                case 3345:      // Earthliving Weapon
                                    triggered_spell =73685;
                                    if (!unitTarget->IsFriendlyTo(m_caster))
                                        unitTarget = m_caster;
                                    break;
                                default:
                                    break;
                            }
                            if (triggered_spell)
                            {
                                m_caster->CastSpell(unitTarget, triggered_spell, true);
                                break;
                            }
                        }
                }
                return;
            }
            // Lava Lash
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000000000000000), 0x00000004))
            {
                if (m_caster->GetTypeId()!=TYPEID_PLAYER)
                    return;

                Player* plrCaster = (Player*)m_caster;

                Item *item = plrCaster->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                if (item)
                {
                    // Damage is increased if your off-hand weapon is enchanted with Flametongue.
                    Unit::AuraList const& auraDummy = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = auraDummy.begin(); itr != auraDummy.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->IsFitToFamily(SPELLFAMILY_SHAMAN, UI64LIT(0x0000000000200000)) &&
                            (*itr)->GetCastItemGuid() == item->GetObjectGuid())
                        {
                            m_damage += m_damage * damage / 100;
                            break;
                        }
                    }
                }
                return;
            }
            // Spirit Link
            if (m_spellInfo->Id == 98020)
            {
                if (!unitTarget)
                    return;

                if (m_UniqueTargetInfo.size() < 2)
                    return;

                if (m_currentBasePoints[1] == 0)
                {
                    int32 sumHealth = 0;
                    int32 sumMaxHealth = 0;
                    for (TargetList::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
                    {
                        if (Unit* unit = m_caster->GetMap()->GetUnit(itr->targetGUID))
                        {
                            if (unit->isDead())
                                continue;

                            sumHealth += unit->GetHealth();
                            sumMaxHealth += unit->GetMaxHealth();
                        }
                    }

                    m_currentBasePoints[1] = int32(float(sumHealth) / sumMaxHealth * 10000);
                }

                int32 diff = int32(unitTarget->GetHealth() - unitTarget->GetMaxHealth() / 100.0f * m_currentBasePoints[1] / 100.0f);
                // need damage
                if (diff > 0)
                    m_caster->CastCustomSpell(unitTarget, 98021, &diff, NULL, NULL, true);
                // need heal
                else if (diff < 0)
                {
                    diff *= -1;
                    m_caster->CastCustomSpell(unitTarget, 98021, NULL, &diff, NULL, true);
                }
                return;
            }
            // Fire Nova
            if (m_spellInfo->Id == 1535)
            {
                if (unitTarget && unitTarget->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, UI64LIT(0x10000000), 0, m_caster->GetObjectGuid()))
                    m_caster->CastSpell(unitTarget, 8349, true);
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            SpellClassOptionsEntry const* dkClassOptions = m_spellInfo->GetSpellClassOptions();
            switch(m_spellInfo->Id)
            {
                case 46584:                                 // Raise Dead
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || effect->EffectIndex != EFFECT_INDEX_0)
                        return;

                    Player* p_caster = (Player*)m_caster;

                    // do nothing if ghoul summon already exsists (in fact not possible, but...)
                    if (p_caster->GetPet())
                    {
                        p_caster->RemoveSpellCooldown(m_spellInfo->Id, true);
                        SendCastResult(SPELL_FAILED_ALREADY_HAVE_SUMMON);
                        finish(false);
                        return;
                    }

                    // check for "Master of Ghouls", id's stored in basepoints
                    if (p_caster->HasAura(52143))
                        p_caster->CastSpell(m_caster,m_currentBasePoints[1], true);
                    else
                        p_caster->CastSpell(m_caster,m_currentBasePoints[0], true);
                    break;
                }
                // Gnaw
                case 47481:
                {
                    if (!unitTarget)
                        return;

                    // check for Dark Transformation
                    m_caster->CastSpell(unitTarget, m_caster->HasAura(63560) ? 91797 : 91800, true);
                    return;
                }
                // Death Grip
                case 49576:
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 49560, true);
                    return;
                }
                // Death Grip
                case 49560:
                {
                    if (!unitTarget || unitTarget == m_caster)
                        return;

                    uint32 spellId = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
                    unitTarget->CastSpell(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), spellId, true);
                    return;
                }
            }
            // Death Coil
            if (dkClassOptions && dkClassOptions->SpellFamilyFlags & UI64LIT(0x002000))
            {
                if (m_caster->IsFriendlyTo(unitTarget))
                {
                    if (!unitTarget || unitTarget->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                        return;

                    int32 bp = int32(damage * 3.5f);
                    m_caster->CastCustomSpell(unitTarget, 47633, &bp, NULL, NULL, true);
                }
                else
                {
                    int32 bp = damage;
                    m_caster->CastCustomSpell(unitTarget, 47632, &bp, NULL, NULL, true);
                }
                return;
            }
            // Death Strike
            else if (m_spellInfo->Id == 49998 || m_spellInfo->Id == 66188)
            {
                // minimum of at least $s3% of your maximum health
                int32 minHeal = damage * m_caster->GetMaxHealth() / 100;
                // TODO: replace 20% magic number
                int32 heal = int32(m_caster->GetDamageCounterInPastSecs(5, DAMAGE_TAKEN_COUNTER) * 0.2f);

                // Improved Death Strike
                Unit::AuraList const& dummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->SpellIconID == 2751 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DEATHKNIGHT &&
                        (*itr)->GetEffIndex() == EFFECT_INDEX_2)
                    {
                        heal = int32(heal * (*itr)->GetModifier()->m_amount / 100.0f);
                        break;
                    }
                }

                // Dark Succor
                if (Aura* succor = m_caster->GetAura(101568, EFFECT_INDEX_0))
                {
                    // Frost or Unholy Presence
                    if (m_caster->HasAura(48266) || m_caster->HasAura(48265))
                        minHeal = succor->GetModifier()->m_amount;
                }

                heal = std::max(heal, minHeal);

                // correct for threat of Tassarian
                if (m_caster->haveOffhandWeapon())
                    heal /= 2;

                if (heal)
                {
                    // Blood Shield
                    if (Aura* bloodShield = m_caster->GetAura(77513, EFFECT_INDEX_0))
                    {
                        // only in Blood Presence
                        if (m_caster->HasAura(48263))
                        {
                            int32 bp = int32(heal * bloodShield->GetModifier()->m_amount / 100.0f);
                            if (Aura* oldShield = m_caster->GetAura(77535, EFFECT_INDEX_0))
                                bp += oldShield->GetModifier()->m_amount;

                            m_caster->CastCustomSpell(m_caster, 77535, &bp, NULL, NULL, true);
                        }
                    }
                    m_caster->CastCustomSpell(m_caster, 45470, &heal, NULL, NULL, true);
                }
                return;
            }
            // Corpse Explosion. Execute for Effect1 only
            else if (m_spellInfo->SpellIconID == 1737 && SpellEffectIndex(effect->EffectIndex) == EFFECT_INDEX_1)
            {
                if (!unitTarget)
                    return;

                // casting on a ghoul-pet makes it explode! :D
                // target validation is done in Spell:SetTargetMap
                if (unitTarget->GetEntry() == 26125 && unitTarget->GetObjectGuid().IsPet() && unitTarget->isAlive())
                {
                    int32 bp0 = int32(unitTarget->GetMaxHealth() * 0.25f); // AoE dmg
                    unitTarget->InterruptNonMeleeSpells(false);
                    unitTarget->CastCustomSpell(unitTarget, 50444, &bp0, NULL, NULL, false);
                    ((Creature*)unitTarget)->ForcedDespawn(500);
                    //((Pet*)unitTarget)->Unsummon(PET_SAVE_AS_DELETED);
                }
                else
                {
                    unitTarget->SetDeathState(DEAD);
                    int32 damage = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
                    uint32 spell = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1);

                    m_caster->CastCustomSpell(unitTarget, spell, &damage, NULL, NULL, true);
                    unitTarget->SetDisplayId(25537 + urand(0, 3));

                    //if (unitTarget->GetTypeId() == TYPEID_UNIT && unitTarget->getDeathState() == CORPSE)
                    //    ((Creature*)unitTarget)->RemoveCorpse();
                }
                return;
            }
            break;
        }
        case SPELLFAMILY_PET:
        {
            if (m_spellInfo->SpellIconID == 3748 && unitTarget->GetTypeId() == TYPEID_UNIT && ((Creature*)unitTarget)->IsPet())
                ((Pet*)unitTarget)->UpdateScalingAuras();
            break;
        }
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr.GetPetAura(m_spellInfo->Id, SpellEffectIndex(effect->EffectIndex)))
    {
        m_caster->AddPetAura(petSpell);
        return;
    }

    // Script based implementation. Must be used only for not good for implementation in core spell effects
    // So called only for not processed cases
    bool libraryResult = false;
    if (gameObjTarget)
        libraryResult = sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, SpellEffectIndex(effect->EffectIndex), gameObjTarget);
    else if (unitTarget && unitTarget->GetTypeId() == TYPEID_UNIT)
        libraryResult = sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, SpellEffectIndex(effect->EffectIndex), (Creature*)unitTarget);
    else if (itemTarget)
        libraryResult = sScriptMgr.OnEffectDummy(m_caster, m_spellInfo->Id, SpellEffectIndex(effect->EffectIndex), itemTarget);

    if (libraryResult || !unitTarget)
        return;

    // Previous effect might have started script
    if (!ScriptMgr::CanSpellEffectStartDBScript(m_spellInfo, SpellEffectIndex(effect->EffectIndex)))
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart spellid %u in EffectDummy", m_spellInfo->Id);
    m_caster->GetMap()->ScriptsStart(sSpellScripts, m_spellInfo->Id, m_caster, unitTarget);
}

void Spell::EffectTriggerSpellWithValue(SpellEffectEntry const* effect)
{
    uint32 triggered_spell_id = effect->EffectTriggerSpell;

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        ERROR_LOG("EffectTriggerSpellWithValue of spell %u: triggering unknown spell id %i", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    int32 bp = damage;
    m_caster->CastCustomSpell(unitTarget, triggered_spell_id, &bp, &bp, &bp, true, m_CastItem , NULL, m_originalCasterGUID, m_spellInfo);
}

void Spell::EffectTriggerRitualOfSummoning(SpellEffectEntry const* effect)
{
    uint32 triggered_spell_id = effect->EffectTriggerSpell;
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        ERROR_LOG("EffectTriggerRitualOfSummoning of spell %u: triggering unknown spell id %i", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    finish();

    m_caster->CastSpell(unitTarget,spellInfo,false);
}

void Spell::EffectClearQuest(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)m_caster;

    uint32 quest_id = effect->EffectMiscValue;

    if (!sObjectMgr.GetQuestTemplate(quest_id))
    {
        sLog.outError("Spell::EffectClearQuest spell entry %u attempt clear quest entry %u but this quest does not exist.", m_spellInfo->Id, quest_id);
        return;
    }

    // remove quest possibly in quest log (is that expected?)
    for(uint16 slot = 0; slot < MAX_QUEST_LOG_SIZE; ++slot)
    {
        uint32 quest = player->GetQuestSlotQuestId(slot);

        if (quest == quest_id)
        {
            player->SetQuestSlot(slot, 0);
            // ignore unequippable quest items in this case, it will still be equipped
            player->TakeQuestSourceItem(quest_id, false);
        }
    }

    player->SetQuestStatus(quest_id, QUEST_STATUS_NONE);
    player->getQuestStatusMap()[quest_id].m_rewarded = false;

    PhaseUpdateData phaseUdateData;
    phaseUdateData.AddQuestUpdate(quest_id);

    player->GetPhaseMgr()->NotifyConditionChanged(phaseUdateData);
}

void Spell::EffectForceCast(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    uint32 triggered_spell_id = effect->EffectTriggerSpell;

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(triggered_spell_id);

    if (!spellInfo)
    {
        ERROR_LOG("EffectForceCast of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    // if triggered spell has SPELL_AURA_CONTROL_VEHICLE, it must be casted on caster
    for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (spellInfo->GetEffectApplyAuraNameByIndex(SpellEffectIndex(i)) == SPELL_AURA_CONTROL_VEHICLE)
        {
            unitTarget->CastSpell(m_caster, spellInfo, true, NULL, NULL, ObjectGuid(), m_spellInfo);
            return;
        }
    }

    unitTarget->CastSpell(unitTarget, spellInfo, true, NULL, NULL, m_originalCasterGUID, m_spellInfo);
}

void Spell::EffectForceCastWithValue(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    uint32 triggered_spell_id = effect->EffectTriggerSpell;

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(triggered_spell_id);

    if (!spellInfo)
    {
        ERROR_LOG("EffectForceCastWithValue of spell %u: triggering unknown spell id %i", m_spellInfo->Id,triggered_spell_id);
        return;
    }

    int32 bp = damage;
    unitTarget->CastCustomSpell(unitTarget, spellInfo, &bp, NULL, NULL, true, NULL, NULL, m_originalCasterGUID);
}

void Spell::EffectTriggerSpell(SpellEffectEntry const* effect)
{
    // only unit case known
    if (!unitTarget)
    {
        if(gameObjTarget || itemTarget)
            ERROR_LOG("Spell::EffectTriggerSpell (Spell: %u): Unsupported non-unit case!",m_spellInfo->Id);
        return;
    }

    uint32 triggered_spell_id = effect->EffectTriggerSpell;

    // special cases
    switch (triggered_spell_id)
    {
        // Vanish (not exist)
        case 18461:
        {
            unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
            unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
            unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_STALKED);

            // if this spell is given to NPC it must handle rest by it's own AI
            if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            return;
        }
        // Intimidating Shout
        case 20511:
        {
            // Glyph of Intimidating Shout
            if (!m_caster->HasAura(63327))
                return;
            break;
        }
        // just skip
        case 23770:                                         // Sayge's Dark Fortune of *
            // not exist, common cooldown can be implemented in scripts if need.
            return;
        // Brittle Armor - (need add max stack of 24575 Brittle Armor)
        case 29284:
            m_caster->CastSpell(unitTarget, 24575, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        // Mercurial Shield - (need add max stack of 26464 Mercurial Shield)
        case 29286:
            m_caster->CastSpell(unitTarget, 26464, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        // Righteous Defense
        case 31980:
        {
            m_caster->CastSpell(unitTarget, 31790, true, m_CastItem, NULL, m_originalCasterGUID);
            return;
        }
        // Cold Flame
        case 33801:
        {
            // Does not exist in DB, so don't flood
            return;
        }
        // Cloak of Shadows
        case 35729:
        {
            std::set<uint32> toRemoveSpellList;
            Unit::SpellAuraHolderMap& Auras = unitTarget->GetSpellAuraHolderMap();
            for (Unit::SpellAuraHolderMap::iterator iter = Auras.begin(); iter != Auras.end(); ++iter)
            {
                // Remove all harmful spells on you except positive/passive/physical auras
                if (!iter->second->IsPositive() &&
                    !iter->second->IsPassive() &&
                    !iter->second->IsDeathPersistent() &&
                    (GetSpellSchoolMask(iter->second->GetSpellProto()) & SPELL_SCHOOL_MASK_NORMAL) == 0)
                {
                    toRemoveSpellList.insert(iter->first);
                }
            }

            for (std::set<uint32>::iterator i = toRemoveSpellList.begin(); i != toRemoveSpellList.end(); ++i)
                m_caster->RemoveAurasDueToSpell(*i);
            return;
        }
        // Priest Shadowfiend (34433) need apply mana gain trigger aura on pet
        case 41967:
        {
            if (Unit *pet = unitTarget->GetPet())
            {
                pet->CastSpell(pet, 28305, true);
                // Item - Priest T12 Shadow 2P Bonus
                if (unitTarget->HasAura(99154))
                    pet->CastSpell(pet, 99155, true);
            }
            return;
        }
        // Demonic Leap
        case 54786:
        {
            if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
            {
                float x = m_targets.m_destX;
                float y = m_targets.m_destY;
                float z = m_targets.m_destZ;

                MaNGOS::NormalizeMapCoord(x);
                MaNGOS::NormalizeMapCoord(y);
                m_caster->UpdateGroundPositionZ(x, y, z);

                m_caster->CastSpell(x, y, z, triggered_spell_id, true, NULL, NULL, m_originalCasterGUID);
                return;
            }
        }
        // Coldflame (Lord Marrowgar - Icecrown Citadel) - have casting time 0.2s, must be casted with triggered=false
        case 69147:
        {
            m_caster->CastSpell(m_caster, triggered_spell_id, false);
            return;
        }
        // Feral Swiftness Clear
        case 97985:
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER)
                // search Feral Swiftness
                if (SpellEntry const* talent = ((Player*)m_caster)->GetKnownTalentRankById(8295))
                    if (roll_chance_i(talent->CalculateSimpleValue(EFFECT_INDEX_1)))
                        break;

            return;
        }
    }

    // normal case
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(triggered_spell_id);
    if (!spellInfo)
    {
        // No previous Effect might have started a script
        bool startDBScript = unitTarget && ScriptMgr::CanSpellEffectStartDBScript(m_spellInfo, SpellEffectIndex(effect->EffectIndex));
        if (startDBScript)
        {
            DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart spellid %u in EffectTriggerSpell", m_spellInfo->Id);
            startDBScript = m_caster->GetMap()->ScriptsStart(sSpellScripts, m_spellInfo->Id, m_caster, unitTarget);
        }

        if (!startDBScript)
            sLog.outError("EffectTriggerSpell of spell %u: triggering unknown spell id %i", m_spellInfo->Id, triggered_spell_id);
        return;
    }

    // select formal caster for triggered spell
    Unit* caster = m_caster;

    // some triggered spells require specific equipment
    if (spellInfo->GetEquippedItemClass() >=0 && m_caster->GetTypeId()==TYPEID_PLAYER)
    {
        // main hand weapon required
        if (spellInfo->HasAttribute(SPELL_ATTR_EX3_MAIN_HAND))
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(BASE_ATTACK, true, false);

            // skip spell if no weapon in slot or broken
            if (!item)
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }

        // offhand hand weapon required
        if (spellInfo->HasAttribute(SPELL_ATTR_EX3_REQ_OFFHAND))
        {
            Item* item = ((Player*)m_caster)->GetWeaponForAttack(OFF_ATTACK, true, false);

            // skip spell if no weapon in slot or broken
            if (!item)
                return;

            // skip spell if weapon not fit to triggered spell
            if (!item->IsFitToSpellRequirements(spellInfo))
                return;
        }
    }
    /*else if (spellInfo->Targets & TARGET_FLAG_DEST_LOCATION &&
         m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        // Init dest coordinates
        float x,y,z;

        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            ((Player*)m_caster)->RemoveSpellCooldown(triggered_spell_id);

        MaNGOS::NormalizeMapCoord(x);
        MaNGOS::NormalizeMapCoord(y);
        m_caster->UpdateGroundPositionZ(x,y,z);

        m_caster->CastSpell(x, y, z, spellInfo, true, NULL, NULL, m_originalCasterGUID);
        return;
    }*/
    else
    {
        // Note: not exist spells with weapon req. and IsSpellHaveCasterSourceTargets == true
        // so this just for speedup places in else
        caster = IsSpellWithCasterSourceTargetsOnly(spellInfo) ? unitTarget : m_caster;
    }

    caster->CastSpell(unitTarget, spellInfo, true, m_CastItem, NULL, m_originalCasterGUID, m_spellInfo);
}

void Spell::EffectTriggerMissileSpell(SpellEffectEntry const* effect)
{
    uint32 triggered_spell_id = effect->EffectTriggerSpell;

    // normal case
    SpellEntry const *spellInfo = sSpellStore.LookupEntry( triggered_spell_id );

    if(!spellInfo)
    {
        sLog.outError("EffectTriggerMissileSpell of spell %u (eff: %u): triggering unknown spell id %u",
            m_spellInfo->Id,effect->EffectIndex,triggered_spell_id);
        return;
    }

    // Throw Totem
    if (m_spellInfo->Id == 101603)
    {
        m_caster->RemoveAurasDueToSpell(101601);    // Throw Totem
        m_caster->RemoveAurasDueToSpell(107837);    // Throw Totem
    }

    if (m_CastItem)
        DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "WORLD: cast Item spellId - %i", spellInfo->Id);

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        ((Player*)m_caster)->RemoveSpellCooldown(triggered_spell_id);

    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION || m_caster == unitTarget)
    {
        // Init dest coordinates
        float x,y,z;
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;

        MaNGOS::NormalizeMapCoord(x);
        MaNGOS::NormalizeMapCoord(y);
        m_caster->UpdateAllowedPositionZ(x, y, z);

        m_caster->CastSpell(x, y, z, spellInfo, true, m_CastItem, NULL, m_originalCasterGUID, m_spellInfo);
    }
    else
    {
        Unit* caster = IsSpellWithCasterSourceTargetsOnly(spellInfo) ? unitTarget : m_caster;
        caster->CastSpell(unitTarget, spellInfo, true, m_CastItem, NULL, m_originalCasterGUID, m_spellInfo);
    }
}

void Spell::EffectJump(SpellEffectEntry const* effect)
{
    if(m_caster->IsTaxiFlying())
        return;

    // Init dest coordinates
    float x,y,z,o;
    if(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        m_targets.getDestination(x, y, z);

        if(effect->EffectImplicitTargetA == TARGET_BEHIND_VICTIM)
        {
            // explicit cast data from client or server-side cast
            // some spell at client send caster
            Unit* pTarget = NULL;
            if(m_targets.getUnitTarget() && m_targets.getUnitTarget()!=m_caster)
                pTarget = m_targets.getUnitTarget();
            else if(m_caster->getVictim())
                pTarget = m_caster->getVictim();
            else if(m_caster->GetTypeId() == TYPEID_PLAYER)
                pTarget = m_caster->GetMap()->GetUnit(((Player*)m_caster)->GetSelectionGuid());

            o = pTarget ? pTarget->GetOrientation() : m_caster->GetOrientation();
        }
        else
            o = m_caster->GetOrientation();
    }
    else if(unitTarget)
    {
        unitTarget->GetContactPoint(m_caster,x,y,z,CONTACT_DISTANCE);
        o = m_caster->GetOrientation();
    }
    else if(gameObjTarget)
    {
        gameObjTarget->GetContactPoint(m_caster,x,y,z,CONTACT_DISTANCE);
        o = m_caster->GetOrientation();
    }
    else
    {
        ERROR_LOG( "Spell::EffectJump - unsupported target mode for spell ID %u", m_spellInfo->Id );
        return;
    }

    m_caster->UpdateGroundPositionZ(x, y, z, 7.0f);
    uint32 speed_z = effect->EffectMiscValue;
    if (!speed_z)
        speed_z = 10;
    uint32 speed_xy = effect->EffectMiscValueB;
    if (!speed_xy)
        speed_xy = 150;
    m_caster->MonsterMoveJump(x, y, z, float(speed_xy) / 2, float(speed_z) / 10);
}

void Spell::EffectTeleportUnits(SpellEffectEntry const* effect)   // TODO - Use target settings for this effect!
{
    if(!unitTarget || unitTarget->IsTaxiFlying())
        return;

    // Target dependend on TargetB, if there is none provided, decide dependend on A
    uint32 targetType = effect->EffectImplicitTargetB;
    if (!targetType)
        targetType = effect->EffectImplicitTargetA;

    switch (targetType)
    {
        case TARGET_INNKEEPER_COORDINATES:
        {
            // Only players can teleport to innkeeper
            if (unitTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            ((Player*)unitTarget)->TeleportToHomebind(unitTarget==m_caster ? TELE_TO_SPELL : 0);
            return;
        }
        case TARGET_AREAEFFECT_INSTANT:                     // in all cases first TARGET_TABLE_X_Y_Z_COORDINATES
        case TARGET_TABLE_X_Y_Z_COORDINATES:
        case TARGET_SELF2:
        {
            SpellTargetPosition const* st = sSpellMgr.GetSpellTargetPosition(m_spellInfo->Id);
            if(!st)
            {
                ERROR_LOG( "Spell::EffectTeleportUnits - unknown Teleport coordinates for spell ID %u", m_spellInfo->Id );
                return;
            }

            if(st->target_mapId==unitTarget->GetMapId())
                unitTarget->NearTeleportTo(st->target_X,st->target_Y,st->target_Z,st->target_Orientation,unitTarget==m_caster);
            else if(unitTarget->GetTypeId()==TYPEID_PLAYER)
                ((Player*)unitTarget)->TeleportTo(st->target_mapId,st->target_X,st->target_Y,st->target_Z,st->target_Orientation,unitTarget==m_caster ? TELE_TO_SPELL : 0);
            break;
        }
        case TARGET_EFFECT_SELECT:
        {
            // m_destN filled, but sometimes for wrong dest and does not have TARGET_FLAG_DEST_LOCATION

            float x = unitTarget->GetPositionX();
            float y = unitTarget->GetPositionY();
            float z = unitTarget->GetPositionZ();
            float orientation = m_caster->GetOrientation();

            m_caster->NearTeleportTo(x, y, z, orientation, unitTarget == m_caster);
            return;
        }
        case TARGET_BEHIND_VICTIM:
        {
            Unit *pTarget = NULL;

            // explicit cast data from client or server-side cast
            // some spell at client send caster
            if(m_targets.getUnitTarget() && m_targets.getUnitTarget()!=unitTarget)
                pTarget = m_targets.getUnitTarget();
            else if(unitTarget->getVictim())
                pTarget = unitTarget->getVictim();
            else if(unitTarget->GetTypeId() == TYPEID_PLAYER)
                pTarget = unitTarget->GetMap()->GetUnit(((Player*)unitTarget)->GetSelectionGuid());

            // Init dest coordinates
            float x = m_targets.m_destX;
            float y = m_targets.m_destY;
            float z = m_targets.m_destZ;
            float orientation = pTarget ? pTarget->GetOrientation() : unitTarget->GetOrientation();
            unitTarget->NearTeleportTo(x,y,z,orientation,unitTarget==m_caster);
            return;
        }
        default:
        {
            // If not exist data for dest location - return
            if(!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
            {
                sLog.outError( "Spell::EffectTeleportUnits - unknown EffectImplicitTargetB[%u] = %u for spell ID %u", effect->EffectIndex, effect->EffectImplicitTargetB, m_spellInfo->Id );
                return;
            }
            // Init dest coordinates
            float x = m_targets.m_destX;
            float y = m_targets.m_destY;
            float z = m_targets.m_destZ;
            float orientation = unitTarget->GetOrientation();
            // Teleport
            unitTarget->NearTeleportTo(x,y,z,orientation,unitTarget==m_caster);
            return;
        }
    }

    // post effects for TARGET_TABLE_X_Y_Z_COORDINATES
    switch ( m_spellInfo->Id )
    {
        // Dimensional Ripper - Everlook
        case 23442:
        {
            int32 r = irand(0, 119);
            if ( r >= 70 )                                  // 7/12 success
            {
                if ( r < 100 )                              // 4/12 evil twin
                    m_caster->CastSpell(m_caster, 23445, true);
                else                                        // 1/12 fire
                    m_caster->CastSpell(m_caster, 23449, true);
            }
            return;
        }
        // Ultrasafe Transporter: Toshley's Station
        case 36941:
        {
            if ( roll_chance_i(50) )                        // 50% success
            {
                int32 rand_eff = urand(1, 7);
                switch ( rand_eff )
                {
                    case 1:
                        // soul split - evil
                        m_caster->CastSpell(m_caster, 36900, true);
                        break;
                    case 2:
                        // soul split - good
                        m_caster->CastSpell(m_caster, 36901, true);
                        break;
                    case 3:
                        // Increase the size
                        m_caster->CastSpell(m_caster, 36895, true);
                        break;
                    case 4:
                        // Decrease the size
                        m_caster->CastSpell(m_caster, 36893, true);
                        break;
                    case 5:
                    // Transform
                    {
                        if (((Player*)m_caster)->GetTeam() == ALLIANCE )
                            m_caster->CastSpell(m_caster, 36897, true);
                        else
                            m_caster->CastSpell(m_caster, 36899, true);
                        break;
                    }
                    case 6:
                        // chicken
                        m_caster->CastSpell(m_caster, 36940, true);
                        break;
                    case 7:
                        // evil twin
                        m_caster->CastSpell(m_caster, 23445, true);
                        break;
                }
            }
            return;
        }
        // Dimensional Ripper - Area 52
        case 36890:
        {
            if ( roll_chance_i(50) )                        // 50% success
            {
                int32 rand_eff = urand(1, 4);
                switch ( rand_eff )
                {
                    case 1:
                        // soul split - evil
                        m_caster->CastSpell(m_caster, 36900, true);
                        break;
                    case 2:
                        // soul split - good
                        m_caster->CastSpell(m_caster, 36901, true);
                        break;
                    case 3:
                        // Increase the size
                        m_caster->CastSpell(m_caster, 36895, true);
                        break;
                    case 4:
                    // Transform
                    {
                        if (((Player*)m_caster)->GetTeam() == ALLIANCE )
                            m_caster->CastSpell(m_caster, 36897, true);
                        else
                            m_caster->CastSpell(m_caster, 36899, true);
                        break;
                    }
                }
            }
            return;
        }
    }
}

void Spell::EffectApplyAura(SpellEffectEntry const* effect)
{
    // Hack: Aura has zero duration
    switch (m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id)
            {
                case 49590:         // Arcane Disruptor (quest support http://www.wowhead.com/quest=13149)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        ((Player*)m_caster)->KilledMonsterCredit(30996);
                    break;
                }
                case 56278:         // Read Pronouncement (quest support http://www.wowhead.com/quest=12987)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                         ((Player*)m_caster)->KilledMonsterCredit(30210);
                     break;
                }
            }
            break;
        }
    }

    if(!unitTarget)
        return;

    // ghost spell check, allow apply any auras at player loading in ghost mode (will be cleanup after load)
    if ( (!unitTarget->isAlive() && !(IsDeathOnlySpell(m_spellInfo) || IsDeathPersistentSpell(m_spellInfo))) &&
        (unitTarget->GetTypeId() != TYPEID_PLAYER || !((Player*)unitTarget)->GetSession()->PlayerLoading()) )
        return;

    Unit* caster = GetAffectiveCaster();
    if(!caster)
    {
        // FIXME: currently we can't have auras applied explicitly by gameobjects
        // so for auras from wild gameobjects (no owner) target used
        if (m_originalCasterGUID.IsGameObject())
            caster = unitTarget;
        else
            return;
    }

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell: Aura is: %u", effect->EffectApplyAuraName);

    Aura* aur = CreateAura(m_spellInfo, SpellEffectIndex(effect->EffectIndex), &m_currentBasePoints[effect->EffectIndex], m_spellAuraHolder, unitTarget, caster, m_CastItem);
    m_spellAuraHolder->AddAura(aur, SpellEffectIndex(effect->EffectIndex));
}

void Spell::EffectUnlearnSpecialization(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)unitTarget;
    uint32 spellToUnlearn = effect->EffectTriggerSpell;

    _player->removeSpell(spellToUnlearn);

    DEBUG_LOG( "Spell: Player %u has unlearned spell %u from NpcGUID: %u", _player->GetGUIDLow(), spellToUnlearn, m_caster->GetGUIDLow() );
}

void Spell::EffectPowerDrain(SpellEffectEntry const* effect)
{
    if(effect->EffectMiscValue < 0 || effect->EffectMiscValue >= MAX_POWERS)
        return;

    Powers drain_power = Powers(effect->EffectMiscValue);

    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;
    if(unitTarget->getPowerType() != drain_power)
        return;
    if(damage < 0)
        return;

    uint32 curPower = unitTarget->GetPower(drain_power);

    //add spell damage bonus
    damage = m_caster->SpellDamageBonusDone(unitTarget,m_spellInfo,uint32(damage),SPELL_DIRECT_DAMAGE);
    damage = unitTarget->SpellDamageBonusTaken(m_caster, m_spellInfo, uint32(damage),SPELL_DIRECT_DAMAGE);

    // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
    uint32 power = damage;
    if (drain_power == POWER_MANA)
        power -= unitTarget->GetCritDamageReduction(power);

    int32 new_damage;
    if(curPower < power)
        new_damage = curPower;
    else
        new_damage = power;

    unitTarget->ModifyPower(drain_power,-new_damage);

    // Don`t restore from self drain
    if(drain_power == POWER_MANA && m_caster != unitTarget)
    {
        float manaMultiplier = effect->EffectMultipleValue;
        if(manaMultiplier==0)
            manaMultiplier = 1;

        if(Player *modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, manaMultiplier);

        int32 gain = int32(new_damage * manaMultiplier);

        m_caster->EnergizeBySpell(m_caster, m_spellInfo->Id, gain, POWER_MANA);
    }
}

void Spell::EffectSendEvent(SpellEffectEntry const* effect)
{
    /*
    we do not handle a flag dropping or clicking on flag in battleground by sendevent system
    TODO: Actually, why not...
    */
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart %u for spellid %u in EffectSendEvent ", effect->EffectMiscValue, m_spellInfo->Id);
    StartEvents_Event(m_caster->GetMap(), effect->EffectMiscValue, m_caster, focusObject, true, m_caster);
}

void Spell::EffectPowerBurn(SpellEffectEntry const* effect)
{
    if (effect->EffectMiscValue < 0 || effect->EffectMiscValue >= MAX_POWERS)
        return;

    Powers powertype = Powers(effect->EffectMiscValue);

    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;
    if (unitTarget->getPowerType()!=powertype)
        return;
    if (damage < 0)
        return;

    // burn x% of target's mana, up to maximum of 2x% of caster's mana (Mana Burn)
    if (m_spellInfo->Id == 8129)
    {
        int32 maxdamage = m_caster->GetMaxPower(powertype) * damage * 2 / 100;
        damage = unitTarget->GetMaxPower(powertype) * damage / 100;
        if(damage > maxdamage)
            damage = maxdamage;
    }

    int32 curPower = int32(unitTarget->GetPower(powertype));

    // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
    int32 power = damage;
    if (m_caster->GetTypeId() == TYPEID_PLAYER && powertype == POWER_MANA)
        power -= unitTarget->GetCritDamageReduction(power);

    int32 new_damage = (curPower < power) ? curPower : power;

    unitTarget->ModifyPower(powertype, -new_damage);
    float multiplier = effect->EffectMultipleValue;

    if (Player *modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    unitTarget->RemoveAurasByMechanicMask(1 << (MECHANIC_FEAR - 1));

    new_damage = int32(new_damage * multiplier);
    m_damage += new_damage;
}

void Spell::EffectHeal(SpellEffectEntry const* /*effect*/)
{
    if (unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = GetAffectiveCaster();
        if (!caster)
            return;

        int32 addhealth = damage;

        // Healthstone
        if (m_spellInfo->Id == 6262)
            addhealth = int32(m_caster->GetCreateHealth() * m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0) / 100.0f);
        // Seal of Light proc
        if (m_spellInfo->Id == 20167)
        {
            float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
            int32 holy = caster->SpellBaseHealingBonusDone(GetSpellSchoolMask(m_spellInfo));
            if (holy < 0)
                holy = 0;
            addhealth += int32(ap * 0.15f) + int32(holy * 0.15f);
        }
        // Vessel of the Naaru (Vial of the Sunwell trinket)
        else if (m_spellInfo->Id == 45064)
        {
            // Amount of heal - depends from stacked Holy Energy
            int damageAmount = 0;
            Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
                if ((*i)->GetId() == 45062)
                    damageAmount+=(*i)->GetModifier()->m_amount;
            if (damageAmount)
                m_caster->RemoveAurasDueToSpell(45062);

            addhealth += damageAmount;
        }
        // Death Pact (percent heal)
        else if (m_spellInfo->Id==48743)
            addhealth = addhealth * unitTarget->GetMaxHealth() / 100;
        // Swiftmend - consumes Regrowth or Rejuvenation
        else if (m_spellInfo->GetTargetAuraState() == AURA_STATE_SWIFTMEND && unitTarget->HasAuraState(AURA_STATE_SWIFTMEND))
        {
            Unit::AuraList const& RejorRegr = unitTarget->GetAurasByType(SPELL_AURA_PERIODIC_HEAL);
            // find most short by duration
            Aura* targetAura = NULL;
            for(Unit::AuraList::const_iterator i = RejorRegr.begin(); i != RejorRegr.end(); ++i)
            {
                SpellClassOptionsEntry const* smClassOptions = (*i)->GetSpellProto()->GetSpellClassOptions();
                if (smClassOptions && smClassOptions->SpellFamilyName == SPELLFAMILY_DRUID &&
                    // Regrowth or Rejuvenation 0x40 | 0x10
                    (smClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000050)))
                {
                    if (!targetAura || (*i)->GetAuraDuration() < targetAura->GetAuraDuration())
                        targetAura = *i;
                }
            }

            if (!targetAura)
            {
                ERROR_LOG("Target (GUID: %u TypeId: %u) has aurastate AURA_STATE_SWIFTMEND but no matching aura.", unitTarget->GetGUIDLow(), unitTarget->GetTypeId());
                return;
            }

            // Glyph of Swiftmend
            if (!caster->HasAura(54824))
                unitTarget->RemoveAurasByCasterSpell(targetAura->GetId(), targetAura->GetCasterGuid());
        }
        // Runic Healing Injector & Healing Potion Injector effect increase for engineers
        else if ((m_spellInfo->Id == 67486 || m_spellInfo->Id == 67489) && unitTarget->GetTypeId() == TYPEID_PLAYER)
        {
            Player* player = (Player*)unitTarget;
            if (player->HasSkill(SKILL_ENGINEERING))
                addhealth += int32(addhealth * 0.25);
        }
        // Light of Dawn, Word of Glory
        else if (m_spellInfo->Id == 85222 || m_spellInfo->Id == 85673)
        {
            if (m_caster->GetPowerIndex(POWER_HOLY_POWER) != INVALID_POWER_INDEX)
                addhealth *= m_usedHolyPower;

            // World of Glory
            if (m_spellInfo->Id == 85673)
            {
                Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_PALADIN)
                    {
                        // Eternal Glory
                        if ((*itr)->GetSpellProto()->SpellIconID == 2944)
                        {

                            if (roll_chance_i((*itr)->GetModifier()->m_amount))
                            {
                                int32 bp = m_usedHolyPower;
                                // Restore power cost
                                m_caster->CastCustomSpell(m_caster, 88676, &bp, NULL, NULL, true);
                            }
                        }
                        // Guarded by the Light
                        else if ((*itr)->GetSpellProto()->SpellIconID == 3026)
                        {
                            if (unitTarget == m_caster)
                                addhealth = int32(addhealth * (100.0f + (*itr)->GetModifier()->m_amount) / 100.0f);
                        }
                    }
                }
            }
        }
        // Atonement
        else if (m_spellInfo->Id == 94472)
        {
            if (unitTarget == m_caster)
                addhealth /= 2;

            m_healing += addhealth;
            return;
        }
        // Tipping of the Scales
        else if (m_spellInfo->Id == 96880)
        {
            // Weight of a Feather
            if (Aura* aura = unitTarget->GetAura(96881, EFFECT_INDEX_0))
            {
                addhealth += aura->GetModifier()->m_amount;
                unitTarget->RemoveSpellAuraHolder(aura->GetHolder());
            }
        }
        // Spirit Mend (Exotic Ability)
        else if (m_spellInfo->Id == 90361)
        {
            if (Unit* owner = m_caster->GetOwner())
            {
                float rap = owner->GetTotalAttackPowerValue(RANGED_ATTACK);
                addhealth += int32((rap * 0.35f) * 0.5f);
            }
        }

        // Chain Healing
        SpellClassOptionsEntry const* chClassOptions = m_spellInfo->GetSpellClassOptions();
        if (chClassOptions && chClassOptions->SpellFamilyName == SPELLFAMILY_SHAMAN && chClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000100))
        {
            if (unitTarget == m_targets.getUnitTarget())
            {
                // check for Riptide
                Aura* riptide = unitTarget->GetAura(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_SHAMAN, UI64LIT(0x0), 0x00000010, caster->GetObjectGuid());
                if (riptide)
                {
                    addhealth += addhealth/4;
                    // Item - Shaman T12 Restoration 4P Bonus
                    if (!m_caster->HasAura(99195))
                        unitTarget->RemoveSpellAuraHolder(riptide->GetHolder());
                }
            }
        }

        addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, addhealth, HEAL);
        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, addhealth, HEAL);

        m_healing += addhealth;
    }
}

void Spell::EffectHealPct(SpellEffectEntry const* /*effect*/)
{
    if (unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = GetAffectiveCaster();
        if (!caster)
            return;

        // Victory Rush
        if (m_spellInfo->Id == 34428)
        {
            // Impending Victory
            if (SpellAuraHolder* holder = m_caster->GetSpellAuraHolder(82368, m_caster->GetObjectGuid()))
            {
                damage = 5;
                m_caster->RemoveSpellAuraHolder(holder);
            }
            else
                m_caster->RemoveAurasDueToSpell(32216);
        }

        uint32 addhealth = unitTarget->GetMaxHealth() * damage / 100;

        addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, addhealth, HEAL);
        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, addhealth, HEAL);

        uint32 absorb = 0;
        unitTarget->CalculateHealAbsorb(addhealth, &absorb);

        int32 gain = caster->DealHeal(unitTarget, addhealth - absorb, m_spellInfo, false, absorb);

        unitTarget->getHostileRefManager().threatAssist(caster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(m_spellInfo), m_spellInfo);
    }
}

void Spell::EffectHealMechanical(SpellEffectEntry const* /*effect*/)
{
    // Mechanic creature type should be correctly checked by targetCreatureType field
    if (unitTarget && unitTarget->isAlive() && damage >= 0)
    {
        // Try to get original caster
        Unit *caster = GetAffectiveCaster();
        if (!caster)
            return;

        uint32 addhealth = caster->SpellHealingBonusDone(unitTarget, m_spellInfo, damage, HEAL);
        addhealth = unitTarget->SpellHealingBonusTaken(caster, m_spellInfo, addhealth, HEAL);

        uint32 absorb = 0;
        unitTarget->CalculateHealAbsorb(addhealth, &absorb);

        caster->DealHeal(unitTarget, addhealth - absorb, m_spellInfo, false, absorb);
    }
}

void Spell::EffectHealthLeech(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;

    if (damage < 0)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "HealthLeech :%i", damage);

    uint32 curHealth = unitTarget->GetHealth();
    damage = m_caster->SpellNonMeleeDamageLog(unitTarget, m_spellInfo->Id, damage );
    if ((int32)curHealth < damage)
        damage = curHealth;

    float multiplier = effect->EffectMultipleValue;

    if (Player *modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_MULTIPLE_VALUE, multiplier);

    int32 heal = int32(damage*multiplier);
    if (m_caster->isAlive())
    {
        heal = m_caster->SpellHealingBonusTaken(m_caster, m_spellInfo, heal, HEAL);

        uint32 absorb = 0;
        m_caster->CalculateHealAbsorb(heal, &absorb);

        m_caster->DealHeal(m_caster, heal - absorb, m_spellInfo, false, absorb);
    }
}

void Spell::DoCreateItem(SpellEffectEntry const* effect, uint32 itemtype)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    uint32 newitemid = itemtype;
    ItemPrototype const *pProto = ObjectMgr::GetItemPrototype( newitemid );
    if(!pProto)
    {
        player->SendEquipError( EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL );
        return;
    }

    // bg reward have some special in code work
    bool bg_mark = false;
    switch(m_spellInfo->Id)
    {
        case SPELL_WG_MARK_VICTORY:
        case SPELL_WG_MARK_DEFEAT:
            bg_mark = true;
            break;
        default:
            break;
    }

    uint32 num_to_add = damage;

    if (num_to_add < 1)
        num_to_add = 1;
    if (num_to_add > pProto->GetMaxStackSize())
        num_to_add = pProto->GetMaxStackSize();

    // init items_count to 1, since 1 item will be created regardless of specialization
    int items_count=1;
    // the chance to create additional items
    float additionalCreateChance=0.0f;
    // the maximum number of created additional items
    uint8 additionalMaxNum=0;
    // get the chance and maximum number for creating extra items
    if ( canCreateExtraItems(player, m_spellInfo->Id, additionalCreateChance, additionalMaxNum) )
    {
        // roll with this chance till we roll not to create or we create the max num
        while ( roll_chance_f(additionalCreateChance) && items_count<=additionalMaxNum )
            ++items_count;
    }

    // really will be created more items
    num_to_add *= items_count;

    // can the player store the new item?
    ItemPosCountVec dest;
    uint32 no_space = 0;
    InventoryResult msg = player->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, newitemid, num_to_add, &no_space );
    if (msg != EQUIP_ERR_OK)
    {
        // convert to possible store amount
        if (msg == EQUIP_ERR_INVENTORY_FULL || msg == EQUIP_ERR_CANT_CARRY_MORE_OF_THIS)
            num_to_add -= no_space;
        else
        {
            // ignore mana gem case (next effect will recharge existing example)
            if (effect->EffectIndex == EFFECT_INDEX_0 && m_spellInfo->GetSpellEffectIdByIndex(EFFECT_INDEX_1) == SPELL_EFFECT_DUMMY )
                return;

            // if not created by another reason from full inventory or unique items amount limitation
            player->SendEquipError( msg, NULL, NULL, newitemid );
            return;
        }
    }

    if(num_to_add)
    {
        // create the new item and store it
        Item* pItem = player->StoreNewItem( dest, newitemid, true, Item::GenerateItemRandomPropertyId(newitemid));

        // was it successful? return error if not
        if(!pItem)
        {
            player->SendEquipError( EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL );
            return;
        }

        // set the "Crafted by ..." property of the item
        if (pItem->GetProto()->Class != ITEM_CLASS_CONSUMABLE && pItem->GetProto()->Class != ITEM_CLASS_QUEST)
            pItem->SetGuidValue(ITEM_FIELD_CREATOR, player->GetObjectGuid());

        // send info to the client
        player->SendNewItem(pItem, num_to_add, true, !bg_mark);

        if (pProto->Quality > ITEM_QUALITY_EPIC || (pProto->Quality == ITEM_QUALITY_EPIC && pProto->ItemLevel >= MinNewsItemLevel[sWorld.getConfig(CONFIG_UINT32_EXPANSION)]))
            if (Guild* guild = sGuildMgr.GetGuildById(player->GetGuildId()))
                guild->LogNewsEvent(GUILD_NEWS_ITEM_CRAFTED, time(NULL), player->GetGUID(), 0, pProto->ItemId);

        // we succeeded in creating at least one item, so a levelup is possible
        if(!bg_mark)
            player->UpdateCraftSkill(m_spellInfo->Id);
    }
}

void Spell::EffectCreateItem(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() == TYPEID_PLAYER && sSpellMgr.IsAbilityOfSkillType(m_spellInfo, SKILL_ARCHAEOLOGY))
        if (!((Player*)m_caster)->SolveResearchProject(m_spellInfo->Id, m_targets))
            return;

    DoCreateItem(effect, effect->EffectItemType);
}

void Spell::EffectCreateItem2(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId()!=TYPEID_PLAYER)
        return;
    Player* player = (Player*)m_caster;

    // explicit item (possible fake)
    uint32 item_id = effect->EffectItemType;
    // Random Volatile Element
    if (item_id == 54464)
        item_id = 0;

    if (item_id)
        DoCreateItem(effect, item_id);

    // not explicit loot (with fake item drop if need)
    if (IsLootCraftingSpell(m_spellInfo))
    {
        if(item_id)
        {
            if (!player->HasItemCount(item_id, 1))
                return;

            // remove reagent
            uint32 count = 1;
            player->DestroyItemCount(item_id, count, true);
        }

        // create some random items
        player->AutoStoreLoot(NULL, m_spellInfo->Id, LootTemplates_Spell);
    }
}

void Spell::EffectCreateRandomItem(SpellEffectEntry const* /*effect*/)
{
    if (m_caster->GetTypeId()!=TYPEID_PLAYER)
        return;
    Player* player = (Player*)m_caster;

    // create some random items
    player->AutoStoreLoot(NULL, m_spellInfo->Id, LootTemplates_Spell);
}

void Spell::EffectPersistentAA(SpellEffectEntry const* effect)
{
    Unit* pCaster = GetAffectiveCaster();
    // FIXME: in case wild GO will used wrong affective caster (target in fact) as dynobject owner
    if (!pCaster)
        pCaster = m_caster;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));

    if (Player* modOwner = pCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius);

    float x, y, z;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.getDestination(x, y, z);
    else if (Unit* target = m_targets.getUnitTarget())
        target->GetPosition(x, y, z);
    else
        pCaster->GetPosition(x, y, z);

    DynamicObject* dynObj = new DynamicObject;
    if (!dynObj->Create(pCaster->GetMap()->GenerateLocalLowGuid(HIGHGUID_DYNAMICOBJECT), pCaster, m_spellInfo->Id, SpellEffectIndex(effect->EffectIndex), x, y, z, m_duration, radius, DYNAMIC_OBJECT_AREA_SPELL))
    {
        delete dynObj;
        return;
    }

    pCaster->AddDynObject(dynObj);
    pCaster->GetMap()->Add(dynObj);
}

void Spell::EffectEnergize(SpellEffectEntry const* effect)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    if(effect->EffectMiscValue < 0 || effect->EffectMiscValue >= MAX_POWERS)
        return;

    Powers power = Powers(effect->EffectMiscValue);

    // Some level depends spells
    int level_multiplier = 0;
    int level_diff = 0;
    switch (m_spellInfo->Id)
    {
        case 9512:                                          // Restore Energy
            level_diff = m_caster->getLevel() - 40;
            level_multiplier = 2;
            break;
        case 24571:                                         // Blood Fury
            level_diff = m_caster->getLevel() - 60;
            level_multiplier = 10;
            break;
        case 24532:                                         // Burst of Energy
            level_diff = m_caster->getLevel() - 60;
            level_multiplier = 4;
            break;
        case 20167:                                         // Seal of Insight
        case 63375:                                         // Improved Stormstrike
            damage = damage * unitTarget->GetCreateMana() / 100;
            break;
        case 53385:                                         // Divine Storm
        {
            // Divine Storm grant holy power only if it;s damage part hit 4+ targets
            uint32 targetCount = 0;
            for (TargetList::iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
                if (itr->effectMask & (1 << EFFECT_INDEX_2))
                    ++targetCount;
            if (targetCount < 4)
                return;
            break;
        }
        case 67487:                                         // Mana Potion Injector
        case 67490:                                         // Runic Mana Injector
        {
            if (unitTarget->GetTypeId() == TYPEID_PLAYER)
            {
                Player* player = (Player*)unitTarget;
                if (player->HasSkill(SKILL_ENGINEERING))
                    damage += int32(damage * 0.25);
            }
            break;
        }
        case 91322:                                         // Blind Spot
        {
            // remove Inner Eye
            unitTarget->RemoveAurasDueToSpell(91320);
            break;
        }
        case 92331:                                         // Blind Spot
        {
            // remove Inner Eye
            unitTarget->RemoveAurasDueToSpell(92329);
            break;
        }
        case 92601:                                         // Detonate Mana
        {
            // Recaptured Mana
            if (Aura* aura = m_caster->GetAura(92596, EFFECT_INDEX_0))
            {
                damage += aura->GetModifier()->m_amount;
                m_caster->RemoveSpellAuraHolder(aura->GetHolder());
            }
            break;
        }
        case 99007:                                         // Heartfire
        case 99069:                                         // Fires of Heaven
        case 99131:                                         // Divine Fire
        case 99189:                                         // Flametide
        {
            damage = m_caster->GetCreateMana() * damage / 100;
            break;
        }
        case 101033:                                        // Resurgence
        {
            // spell by wich Resurgence was procced is stored in unexistent effect
            uint32 proccedBySpell = m_currentBasePoints[1];
            if (!m_currentBasePoints[1])
                break;

            float mod = 1.0f;
            switch (proccedBySpell)
            {
                case 331:   // Healing Wave
                case 77472: // Greater Healing Wave
                    mod = 1.0f;
                    break;
                case 8004:  // Healing Surge
                case 61295: // Riptide
                case 73685: // Unleash Life
                    mod = 0.6f;
                    break;
                case 1064:  // Chain Heal
                    mod = 0.333f;
                    break;
                default:
                    break;
            }
            damage = int32(damage * mod);
            break;
        }
        default:
            break;
    }

    if (level_diff > 0)
        damage -= level_multiplier * level_diff;

    if (damage < 0 && power != POWER_ECLIPSE && power != POWER_ALTERNATIVE)
        return;

    if(unitTarget->GetMaxPower(power) == 0)
        return;

    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, damage, power);

    // Mad Alchemist's Potion
    if (m_spellInfo->Id == 45051)
    {
        // find elixirs on target
        uint32 elixir_mask = 0;
        Unit::SpellAuraHolderMap& Auras = unitTarget->GetSpellAuraHolderMap();
        for(Unit::SpellAuraHolderMap::iterator itr = Auras.begin(); itr != Auras.end(); ++itr)
        {
            uint32 spell_id = itr->second->GetId();
            if(uint32 mask = sSpellMgr.GetSpellElixirMask(spell_id))
                elixir_mask |= mask;
        }

        // get available elixir mask any not active type from battle/guardian (and flask if no any)
        elixir_mask = (elixir_mask & ELIXIR_FLASK_MASK) ^ ELIXIR_FLASK_MASK;

        // get all available elixirs by mask and spell level
        std::vector<uint32> elixirs;
        SpellElixirMap const& m_spellElixirs = sSpellMgr.GetSpellElixirMap();
        for(SpellElixirMap::const_iterator itr = m_spellElixirs.begin(); itr != m_spellElixirs.end(); ++itr)
        {
            if (itr->second & elixir_mask)
            {
                if (itr->second & (ELIXIR_UNSTABLE_MASK | ELIXIR_SHATTRATH_MASK))
                    continue;

                SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
                if (spellInfo && (spellInfo->GetSpellLevel() < m_spellInfo->GetSpellLevel() || spellInfo->GetSpellLevel() > unitTarget->getLevel()))
                    continue;

                elixirs.push_back(itr->first);
            }
        }

        if (!elixirs.empty())
        {
            // cast random elixir on target
            uint32 rand_spell = urand(0,elixirs.size()-1);
            m_caster->CastSpell(unitTarget,elixirs[rand_spell],true,m_CastItem);
        }
    }
    // Crazy Alchemist Potion
    else if (m_spellInfo->Id == 53750)
    {
        if (roll_chance_i(30))
        {
            uint32 spellIds[12] = { 43185, 43186, 53753, 53761, 53762, 53908,
                                    53909, 53910, 53911, 53913, 53914, 53915};

            m_caster->CastSpell(unitTarget, spellIds[urand(0,11)], true, m_CastItem);
        }
    }
}

void Spell::EffectEnergisePct(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;

    if (effect->EffectMiscValue < 0 || effect->EffectMiscValue >= MAX_POWERS)
        return;

    Powers power = Powers(effect->EffectMiscValue);

    uint32 maxPower = unitTarget->GetMaxPower(power);
    if (maxPower == 0)
        return;

    uint32 gain = damage * maxPower / 100;
    m_caster->EnergizeBySpell(unitTarget, m_spellInfo->Id, gain, power);

    // Rolling Thunder
    if (m_spellInfo->Id == 88765 && m_triggeredByAuraSpell)
    {
        int32 charges = 0;
        // Lightning Shield
        if (SpellAuraHolder* holder = m_caster->GetSpellAuraHolder(324))
        {
            charges = holder->GetAuraCharges();
            if (charges < m_triggeredByAuraSpell->CalculateSimpleValue(EFFECT_INDEX_0))
            {
                holder->SetAuraCharges(charges + 1);
                holder->RefreshHolder();
            }
        }

        // Fulmination
        if (Aura* fulmination = m_caster->GetAura(88766, EFFECT_INDEX_0))
            if (charges && charges + 1 > fulmination->GetModifier()->m_amount)
                m_caster->CastSpell(m_caster, 95774, true);     // Fulmination marker
    }
}

void Spell::SendLoot(ObjectGuid guid, LootType loottype, LockType lockType)
{
    if (gameObjTarget)
    {
        switch (gameObjTarget->GetGoType())
        {
            case GAMEOBJECT_TYPE_DOOR:
            case GAMEOBJECT_TYPE_BUTTON:
            case GAMEOBJECT_TYPE_QUESTGIVER:
            case GAMEOBJECT_TYPE_SPELL_FOCUS:
            case GAMEOBJECT_TYPE_GOOBER:
                gameObjTarget->Use(m_caster);
                return;

            case GAMEOBJECT_TYPE_CHEST:
                gameObjTarget->Use(m_caster);
                // Don't return, let loots been taken
                break;

            case GAMEOBJECT_TYPE_TRAP:
                if (lockType == LOCKTYPE_DISARM_TRAP || m_spellInfo->Id == 1843 && gameObjTarget->GetEntry() == 190752)
                {
                    gameObjTarget->SetLootState(GO_JUST_DEACTIVATED);
                    return;
                }
                ERROR_LOG("Spell::SendLoot unhandled locktype %u for GameObject trap (entry %u) for spell %u.", lockType, gameObjTarget->GetEntry(), m_spellInfo->Id);
                return;
            default:
                ERROR_LOG("Spell::SendLoot nhandled GameObject type %u (entry %u) for spell %u.", gameObjTarget->GetGoType(), gameObjTarget->GetEntry(), m_spellInfo->Id);
                return;
        }
    }

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // Send loot
    ((Player*)m_caster)->SendLoot(guid, loottype);
}

void Spell::EffectOpenLock(SpellEffectEntry const* effect)
{
    if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
    {
        DEBUG_LOG( "WORLD: Open Lock - No Player Caster!");
        return;
    }

    Player* player = (Player*)m_caster;

    uint32 lockId = 0;
    ObjectGuid guid;

    // Get lockId
    if (gameObjTarget)
    {
        GameObjectInfo const* goInfo = gameObjTarget->GetGOInfo();
        // Arathi Basin banner opening !
        if (goInfo->type == GAMEOBJECT_TYPE_BUTTON && goInfo->button.noDamageImmune ||
            goInfo->type == GAMEOBJECT_TYPE_GOOBER && goInfo->goober.losOK)
        {
            //CanUseBattleGroundObject() already called in CheckCast()
            // in battleground check
            if (BattleGround *bg = player->GetBattleGround())
            {
                // check if it's correct bg
                if (bg->GetTypeID(true) == BATTLEGROUND_AB || bg->GetTypeID(true) == BATTLEGROUND_AV || bg->GetTypeID(true) == BATTLEGROUND_SA || bg->GetTypeID(true) == BATTLEGROUND_IC || bg->GetTypeID(true) == BATTLEGROUND_BG)
                    bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (goInfo->type == GAMEOBJECT_TYPE_FLAGSTAND)
        {
            //CanUseBattleGroundObject() already called in CheckCast()
            // in battleground check
            if (BattleGround *bg = player->GetBattleGround())
            {
                if (bg->GetTypeID(true) == BATTLEGROUND_EY)
                    bg->EventPlayerClickedOnFlag(player, gameObjTarget);
                return;
            }
        }
        else if (goInfo->type == GAMEOBJECT_TYPE_GOOBER)
        {
            // Check if object is handled by outdoor pvp
            // GameObject is handling some events related to world battleground events
            if (OutdoorPvP* opvp = sOutdoorPvPMgr.GetScript(player->GetCachedZoneId()))
                if (opvp->HandleGameObjectUse(player, gameObjTarget))
                    return;
        }
        lockId = goInfo->GetLockId();
        guid = gameObjTarget->GetObjectGuid();
    }
    else if (itemTarget)
    {
        lockId = itemTarget->GetProto()->LockID;
        guid = itemTarget->GetObjectGuid();
    }
    else
    {
        DEBUG_LOG( "WORLD: Open Lock - No GameObject/Item Target!");
        return;
    }

    SkillType skillId = SKILL_NONE;
    int32 reqSkillValue = 0;
    int32 skillValue;

    SpellCastResult res = CanOpenLock(SpellEffectIndex(effect->EffectIndex), lockId, skillId, reqSkillValue, skillValue);
    if (res != SPELL_CAST_OK)
    {
        SendCastResult(res);
        return;
    }

    // mark item as unlocked
    if (itemTarget)
        itemTarget->SetFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_UNLOCKED);

    SendLoot(guid, LOOT_SKINNING, LockType(effect->EffectMiscValue));

    // not allow use skill grow at item base open
    if (!m_CastItem && skillId != SKILL_NONE)
    {
        // update skill if really known
        if (uint32 pureSkillValue = player->GetPureSkillValue(skillId))
        {
            if (gameObjTarget)
            {
                // Allow one skill-up until respawned
                if (!gameObjTarget->IsInSkillupList(player) &&
                    player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue))
                    gameObjTarget->AddToSkillupList(player);
            }
            else if (itemTarget)
            {
                // Do one skill-up
                player->UpdateGatherSkill(skillId, pureSkillValue, reqSkillValue);
            }
        }
    }
}

void Spell::EffectSummonChangeItem(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)m_caster;

    // applied only to using item
    if (!m_CastItem)
        return;

    // ... only to item in own inventory/bank/equip_slot
    if (m_CastItem->GetOwnerGuid() != player->GetObjectGuid())
        return;

    uint32 newitemid = effect->EffectItemType;
    if (!newitemid)
        return;

    Item* oldItem = m_CastItem;

    // prevent crash at access and unexpected charges counting with item update queue corrupt
    ClearCastItem();

    player->ConvertItem(oldItem, newitemid);
}

void Spell::EffectProficiency(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    Player *p_target = (Player*)unitTarget;

    SpellEquippedItemsEntry const* eqItems = m_spellInfo->GetSpellEquippedItems();
    if (!eqItems)
        return;

    if (eqItems->EquippedItemClass == ITEM_CLASS_WEAPON && !(p_target->GetWeaponProficiency() & eqItems->EquippedItemSubClassMask))
    {
        p_target->AddWeaponProficiency(eqItems->EquippedItemSubClassMask);
        p_target->SendProficiency(ITEM_CLASS_WEAPON, p_target->GetWeaponProficiency());
    }
    if (eqItems->EquippedItemClass == ITEM_CLASS_ARMOR && !(p_target->GetArmorProficiency() & eqItems->EquippedItemSubClassMask))
    {
        p_target->AddArmorProficiency(eqItems->EquippedItemSubClassMask);
        p_target->SendProficiency(ITEM_CLASS_ARMOR, p_target->GetArmorProficiency());
    }
}

void Spell::EffectApplyAreaAura(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;
    if (!unitTarget->isAlive())
        return;

    AreaAura* Aur = new AreaAura(m_spellInfo, SpellEffectIndex(effect->EffectIndex), &m_currentBasePoints[effect->EffectIndex], m_spellAuraHolder, unitTarget, m_caster, m_CastItem);
    m_spellAuraHolder->AddAura(Aur, SpellEffectIndex(effect->EffectIndex));
}

void Spell::EffectSummonType(SpellEffectEntry const* effect)
{
    uint32 prop_id = effect->EffectMiscValueB;
    SummonPropertiesEntry const *summon_prop = sSummonPropertiesStore.LookupEntry(prop_id);
    if(!summon_prop)
    {
        ERROR_LOG("EffectSummonType: Unhandled summon type %u", prop_id);
        return;
    }

    DETAIL_LOG("Prop ID: %u, PropGroup: %u, PropTitle: %u", prop_id, summon_prop->Group, summon_prop->Title);

    // Ranger: in 4.3.4 two spells: 81283 & 81291
    // need research summon metod, DoSummonWild by default?
    if ((int32)summon_prop->Title == -1)
    {
        DoSummonWild(effect, m_caster->getFaction(), true);
        return;
    }

    switch(summon_prop->Group)
    {
        // faction handled later on, or loaded from template
        case SUMMON_PROP_GROUP_WILD:
        case SUMMON_PROP_GROUP_FRIENDLY:
        {
            switch(summon_prop->Title)                      // better from known way sorting summons by AI types
            {
                case UNITNAME_SUMMON_TITLE_NONE:
                {
                    // those are classical totems - effectbasepoints is their hp and not summon ammount!
                    // 121: 23035, battlestands
                    // 647: 52893, Anti-Magic Zone (npc used)
                    // 3149: 62618, Power Word: Barrier
                    if (prop_id == 121 || prop_id == 647 || prop_id == 3149)
                        DoSummonTotem(effect);
                    // Snake trap exception
                    else if (effect->EffectMiscValueB == 2301)
                        DoSummonSnakes(effect);
                    // Mirror Image
                    else if (prop_id == 1021)
                        DoSummonGuardian(effect, summon_prop->FactionId);
                    else
                        DoSummonWild(effect, summon_prop->FactionId);
                    break;
                }
                case UNITNAME_SUMMON_TITLE_PET:
                case UNITNAME_SUMMON_TITLE_MINION:
                case UNITNAME_SUMMON_TITLE_RUNEBLADE:
                    DoSummonGuardian(effect, summon_prop->FactionId);
                    break;
                case UNITNAME_SUMMON_TITLE_GUARDIAN:
                {
                    if (prop_id == 61)                      // mixed guardians, totems, statues
                    {
                        // * Stone Statue, etc  -- fits much better totem AI
                        if (m_spellInfo->SpellIconID == 2056)
                            DoSummonTotem(effect);
                        else
                        {
                            // possible sort totems/guardians only by summon creature type
                            CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(effect->EffectMiscValue);

                            if (!cInfo)
                                return;

                            // FIXME: not all totems and similar cases selected by this check...
                            if (cInfo->type == CREATURE_TYPE_TOTEM)
                                DoSummonTotem(effect);
                            else
                                DoSummonGuardian(effect, summon_prop->FactionId);
                        }
                    }
                    else
                        DoSummonGuardian(effect, summon_prop->FactionId);
                    break;
                }
                case UNITNAME_SUMMON_TITLE_CONSTRUCT:
                {
                    if (prop_id == 2913)                    // Scrapbot
                        DoSummonWild(effect, summon_prop->FactionId);
                    else if (prop_id == 829)                // DK summon
                        DoSummonSpecialPets(effect);
                    else
                        DoSummonGuardian(effect, summon_prop->FactionId);
                    break;
                }
                case UNITNAME_SUMMON_TITLE_TOTEM:
                    DoSummonTotem(effect, summon_prop->Slot);
                    break;
                case UNITNAME_SUMMON_TITLE_COMPANION:
                    // slot 6 set for critters that can help to player in fighting
                    if (summon_prop->Slot == 6)
                        DoSummonGuardian(effect, summon_prop->FactionId);
                    else
                        DoSummonCritter(effect, summon_prop->FactionId);
                    break;
                case UNITNAME_SUMMON_TITLE_OPPONENT:
                case UNITNAME_SUMMON_TITLE_LIGHTWELL:
                case UNITNAME_SUMMON_TITLE_BUTLER:
                    DoSummonWild(effect, summon_prop->FactionId);
                    break;
                case UNITNAME_SUMMON_TITLE_VEHICLE:
                case UNITNAME_SUMMON_TITLE_MOUNT:
                    // TODO
                    // EffectSummonVehicle(i);
                    DoSummonVehicle(effect, summon_prop->FactionId);
                    break;
                default:
                    ERROR_LOG("EffectSummonType: Unhandled summon title %u", summon_prop->Title);
                break;
            }
            break;
        }
        case SUMMON_PROP_GROUP_PETS:
        {
            // FIXME : multiple summons -  not yet supported as pet
            //1562 - force of nature  - sid 33831
            //1161 - feral spirit - sid 51533
            if(prop_id == 3097) // 3 uncontrolable instead of one controllable :/
                DoSummonGuardian(effect, summon_prop->FactionId);
            else if(prop_id == 1161)
                DoSummonSpecialPets(effect);
            else
                DoSummonPet(effect);
            break;
        }
        case SUMMON_PROP_GROUP_CONTROLLABLE:
        {
            switch(prop_id)
            {
                case 65:
                case 427:
                case 428:
                    DoSummonPossessed(effect);
                    break;
                default:
                    DoSummonGuardian(effect, summon_prop->FactionId);
                break;
            }
            break;
        }
        case SUMMON_PROP_GROUP_VEHICLE:
        case SUMMON_PROP_GROUP_UNCONTROLLABLE_VEHICLE:
        {
            DoSummonVehicle(effect, summon_prop->FactionId);
            break;
        }
        default:
            ERROR_LOG("EffectSummonType: Unhandled summon group type %u", summon_prop->Group);
            break;
    }
}

void Spell::DoSummonPet(SpellEffectEntry const * effect)
{
    if (m_caster->GetPetGuid())
        return;

    if (!unitTarget)
        return;

    uint32 pet_entry = effect->EffectMiscValue;
    if (!pet_entry)
        return;

    CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonPet: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    uint32 level = m_caster->getLevel();
    Pet* spawnCreature = new Pet(SUMMON_PET);

    if (m_caster->GetTypeId()==TYPEID_PLAYER && spawnCreature->LoadPetFromDB((Player*)m_caster, pet_entry))
    {
        // Summon in dest location
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
            spawnCreature->Relocate(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation());

        // set timer for unsummon
        if (m_duration > 0)
            spawnCreature->SetDuration(m_duration);

        return;
    }

    // Summon in dest location
    CreatureCreatePos pos (m_caster->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation(), m_caster->GetPhaseMask());

    if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        pos = CreatureCreatePos(m_caster, -m_caster->GetOrientation());

    Map *map = m_caster->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!spawnCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        sLog.outErrorDb("Spell::DoSummonPet: can't create creature with entry %u for spell %u", cInfo->Entry, m_spellInfo->Id);
        delete spawnCreature;
        return;
    }

    spawnCreature->SetRespawnCoord(pos);

    // set timer for unsummon
    if (m_duration > 0)
        spawnCreature->SetDuration(m_duration);

    spawnCreature->SetOwnerGuid(m_caster->GetObjectGuid());
    spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    spawnCreature->setPowerType(POWER_MANA);
    spawnCreature->setFaction(m_caster->getFaction());
    spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
    spawnCreature->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    spawnCreature->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    spawnCreature->SetCreatorGuid(m_caster->GetObjectGuid());
    spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    spawnCreature->InitStatsForLevel(level, m_caster);

    spawnCreature->GetCharmInfo()->SetPetNumber(pet_number, false);

    spawnCreature->AIM_Initialize();
    spawnCreature->InitPetCreateSpells();
    spawnCreature->InitLevelupSpellsForLevel();
    spawnCreature->SetHealth(spawnCreature->GetMaxHealth());
    spawnCreature->SetPower(POWER_MANA, spawnCreature->GetMaxPower(POWER_MANA));

    //spawnCreature->SetName("");                           // generated by client

    map->Add((Creature*)spawnCreature);

    m_caster->SetPet(spawnCreature);

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        spawnCreature->GetCharmInfo()->SetState(CHARM_STATE_REACT, REACT_DEFENSIVE);
        spawnCreature->SavePetToDB(PET_SAVE_AS_CURRENT);
        ((Player*)m_caster)->PetSpellInitialize();
    }

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned((Creature*)spawnCreature);
    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
        ((Creature*)m_originalCaster)->AI()->JustSummoned((Creature*)spawnCreature);
}

void Spell::DoSummonPossessed(SpellEffectEntry const * effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 creature_entry = effect->EffectMiscValue;
    if (!creature_entry)
        return;

    CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(creature_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonPossessed: creature entry %u not found for spell %u.", creature_entry, m_spellInfo->Id);
        return;
    }

    int32 duration = GetSpellDuration(m_spellInfo);

    float px, py, pz;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        // Summon 1 unit in dest location
        px = m_targets.m_destX;
        py = m_targets.m_destY;
        pz = m_targets.m_destZ;
    }
    // Summon if dest location not present near caster
    else
        m_caster->GetClosePoint(px,py,pz,1.0f);


    TempSummonType summonType = (duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN;
    Creature* summon = m_caster->SummonCreature(creature_entry, px, py, pz, m_caster->GetOrientation(), summonType, duration, true);
    if (!summon)
    {
        sLog.outError("Spell::DoSummonPossessed: creature entry %u for spell %u could not be summoned.", creature_entry, m_spellInfo->Id);
        return;
    }

    summon->SetLevel(m_caster->getLevel());
    summon->SetOwnerGuid(m_caster->GetObjectGuid());
    summon->SetCreatorGuid(m_caster->GetObjectGuid());
    summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    // Internal changes
    summon->addUnitState(UNIT_STAT_CONTROLLED);

    // Changes to owner
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Player* player = (Player*)m_caster;

        player->GetCamera().SetView(summon);

        player->SetCharm(summon);
        player->SetClientControl(summon, 1);
        player->SetMover(summon);

        if (CharmInfo* charmInfo = summon->InitCharmInfo(summon))
            charmInfo->InitPossessCreateSpells();
        player->PossessSpellInitialize();

        summon->SetWalk(m_caster->IsWalking(), true);
        // TODO: Set Fly (ie glyph dependend)
    }

    // Notify Summoner
    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
        ((Creature*)m_originalCaster)->AI()->JustSummoned(summon);

    DEBUG_LOG("Possessed creature %s summoned. Owner is %s ", summon->GetGuidStr().c_str(), m_caster->GetGuidStr().c_str());
}

void Spell::EffectLearnSpell(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            EffectLearnPetSpell(effect);

        return;
    }

    Player *player = (Player*)unitTarget;

    uint32 spellToLearn = ((m_spellInfo->Id==SPELL_ID_GENERIC_LEARN) || (m_spellInfo->Id==SPELL_ID_GENERIC_LEARN_PET)) ? damage : effect->EffectTriggerSpell;
    player->learnSpell(spellToLearn, false);

    DEBUG_LOG( "Spell: Player %u has learned spell %u from NpcGUID=%u", player->GetGUIDLow(), spellToLearn, m_caster->GetGUIDLow() );
}

void Spell::EffectDispel(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    // Fill possible dispel list
    std::list <std::pair<SpellAuraHolder* ,uint32> > dispel_list;

    // Create dispel mask by dispel type
    uint32 dispel_type = effect->EffectMiscValue;
    uint32 dispelMask  = GetDispelMask( DispelType(dispel_type) );

    if (dispelMask & (1 << DISPEL_DISEASE) && unitTarget->HasAura(50536))
        dispelMask &= ~(1 << DISPEL_DISEASE);

    // Cleanse powered by Acts of Sacrifice
    if (effect->EffectIndex == EFFECT_INDEX_0 && m_caster->GetTypeId() == TYPEID_PLAYER &&
        m_spellInfo->Id == 4987 && unitTarget == m_caster)
    {
        Unit::AuraList const& mPctAuras = m_caster->GetAurasByType(SPELL_AURA_ADD_PCT_MODIFIER);
        for (Unit::AuraList::const_iterator itr = mPctAuras.begin(); itr != mPctAuras.end(); ++itr)
        {
            // Acts of Sacrifice Ranks 1, 2
            if ((*itr)->GetId() == 85446 || (*itr)->GetId() == 85795)
            {
                m_caster->RemoveAurasByMechanicMask(IMMUNE_TO_ROOT_AND_SNARE_MASK, false, 1);
                break;
            }
        }
    }

    bool friendly = unitTarget->IsFriendlyTo(m_caster);
    bool immune = unitTarget->IsImmunedToDamage(GetSpellSchoolMask(m_spellInfo));

    Unit::SpellAuraHolderMap const& auras = unitTarget->GetSpellAuraHolderMap();
    for(Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        SpellAuraHolder* holder = itr->second;
        if ((1<<holder->GetSpellProto()->GetDispel()) & dispelMask)
        {
            // spells with that mechanic are not dispeled
            if (GetAllSpellMechanicMask(holder->GetSpellProto()) & (1 << (MECHANIC_IMMUNE_SHIELD - 1)))
                continue;

            bool dispelCharges = holder->GetSpellProto()->HasAttribute(SPELL_ATTR_EX7_DISPEL_CHARGES);
            if (holder->GetSpellProto()->GetDispel() == DISPEL_MAGIC)
            {
                bool positive = true;
                if (!holder->IsPositive())
                    positive = false;
                else
                    positive = !holder->GetSpellProto()->HasAttribute(SPELL_ATTR_EX_NEGATIVE);

                // do not remove positive auras if friendly target
                //               negative auras if non-friendly target
                if (positive == friendly)
                    continue;
            }

            dispel_list.push_back(std::pair<SpellAuraHolder* ,uint32>(holder, dispelCharges ? holder->GetAuraCharges() : holder->GetStackAmount()));
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!dispel_list.empty())
    {
        std::list<std::pair<SpellAuraHolder* ,uint32> > success_list;// (spell_id,casterGuid)
        std::list < uint32 > fail_list;                     // spell_id

        // some spells have effect value = 0 and all from its by meaning expect 1
        if(!damage && m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_GENERIC)
            damage = 1;

        // Dispel N = damage buffs (or while exist buffs for dispel)
        for (int32 count=0; count < damage && !dispel_list.empty(); ++count)
        {
            // Random select buff for dispel
            std::list<std::pair<SpellAuraHolder* ,uint32> >::iterator dispel_itr = dispel_list.begin();
            std::advance(dispel_itr,urand(0, dispel_list.size()-1));

            SpellAuraHolder *holder = dispel_itr->first;

            dispel_itr->second -= 1;

            // remove entry from dispel_list if nothing left in stack
            if (dispel_itr->second == 0)
                dispel_list.erase(dispel_itr);

            SpellEntry const* spellInfo = holder->GetSpellProto();
            bool success = false;
            // Try dispel
            if (!friendly && immune || !GetDispelChance(holder->GetCaster(), unitTarget, holder->GetId(), !unitTarget->IsFriendlyTo(m_caster), &success))
                fail_list.push_back(spellInfo->Id);
            else if (success)
            {
                bool foundDispelled = false;
                for (std::list<std::pair<SpellAuraHolder* ,uint32> >::iterator success_iter = success_list.begin(); success_iter != success_list.end(); ++success_iter)
                {
                    if (success_iter->first->GetId() == holder->GetId() && success_iter->first->GetCasterGuid() == holder->GetCasterGuid())
                    {
                        success_iter->second += 1;
                        foundDispelled = true;
                        break;
                    }
                }
                if (!foundDispelled)
                    success_list.push_back(std::pair<SpellAuraHolder* ,uint32>(holder, 1));
            }
            else
                fail_list.push_back(spellInfo->Id);
        }
        // Send success log and really remove auras
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLDISPELLOG, 8+8+4+1+4+count*5);
            data << unitTarget->GetPackGUID();              // Victim GUID
            data << m_caster->GetPackGUID();                // Caster GUID
            data << uint32(m_spellInfo->Id);                // Dispel spell id
            data << uint8(0);                               // not used
            data << uint32(count);                          // count
            for (std::list<std::pair<SpellAuraHolder* ,uint32> >::iterator j = success_list.begin(); j != success_list.end(); ++j)
            {
                SpellAuraHolder* dispelledHolder = j->first;
                data << uint32(dispelledHolder->GetId());   // Spell Id
                data << uint8(0);                           // 0 - dispelled !=0 cleansed
                unitTarget->RemoveAuraHolderDueToSpellByDispel(dispelledHolder->GetId(), j->second, dispelledHolder->GetCasterGuid(), m_caster);
            }
            m_caster->SendMessageToSet(&data, true);

            // On success dispel
            // Devour Magic
            if (m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_WARLOCK && m_spellInfo->GetCategory() == SPELLCATEGORY_DEVOUR_MAGIC)
            {
                int32 heal_amount = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1);
                m_caster->CastCustomSpell(m_caster, 19658, &heal_amount, NULL, NULL, true);

                // Glyph of Felhunter
                if (Unit *owner = m_caster->GetOwner())
                    if (owner->HasAura(56249))
                        m_caster->CastCustomSpell(owner, 19658, &heal_amount, NULL, NULL, true);
            }
            // Cleanse Spirit
            else if (m_spellInfo->Id == 51886)
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    // search Cleansing Waters
                    Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                    {
                        if ((*itr)->GetEffIndex() == EFFECT_INDEX_0 && (*itr)->GetSpellProto()->SpellIconID == 2020 &&
                            (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_SHAMAN)
                        {
                            uint32 triggered_spell = (*itr)->GetId() == 86959 ? 86961 : 86958;

                            if (((Player*)m_caster)->HasSpellCooldown(triggered_spell))
                                break;

                            m_caster->CastSpell(unitTarget, triggered_spell, true);
                            ((Player*)m_caster)->AddSpellCooldown(triggered_spell, 0, time(NULL) + 6);
                            break;
                        }
                    }
                }
            }
            // Dispel Magic
            else if (m_spellInfo->Id == 97690)
            {
                // Glyph of Dispel Magic
                if (Aura* glyph = m_caster->GetAura(55677, EFFECT_INDEX_0))
                {
                    int32 bp = 0;
                    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                        bp = int32(unitTarget->GetMaxHealth() * glyph->GetModifier()->m_amount / 100);
                    else
                        bp = int32(m_caster->GetMaxHealth() * glyph->GetModifier()->m_amount / 100);

                    m_caster->CastCustomSpell(unitTarget, glyph->GetSpellEffect()->EffectTriggerSpell, &bp, NULL, NULL, true);
                }
            }
        }
        // Send fail log to client
        if (!fail_list.empty())
        {
            // Failed to dispel
            WorldPacket data(SMSG_DISPEL_FAILED, 8+8+4+4*fail_list.size());
            data << m_caster->GetObjectGuid();              // Caster GUID
            data << unitTarget->GetObjectGuid();            // Victim GUID
            data << uint32(m_spellInfo->Id);                // Dispel spell id
            for (std::list< uint32 >::iterator j = fail_list.begin(); j != fail_list.end(); ++j)
                data << uint32(*j);                         // Spell Id
            m_caster->SendMessageToSet(&data, true);
        }
    }
}

void Spell::EffectDualWield(SpellEffectEntry const* /*effect*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanDualWield(true);
}

void Spell::EffectPull(SpellEffectEntry const* /*effect*/)
{
    // TODO: create a proper pull towards distract spell center for distract
    DEBUG_LOG("WORLD: Spell Effect DUMMY");
}

void Spell::EffectDistract(SpellEffectEntry const* /*effect*/)
{
    // Check for possible target
    if (!unitTarget || unitTarget->isInCombat())
        return;

    // target must be OK to do this
    if (unitTarget->hasUnitState(UNIT_STAT_CAN_NOT_REACT))
        return;

    unitTarget->SetFacingTo(unitTarget->GetAngle(m_targets.m_destX, m_targets.m_destY));
    unitTarget->clearUnitState(UNIT_STAT_MOVING);

    if (unitTarget->GetTypeId() == TYPEID_UNIT)
        unitTarget->GetMotionMaster()->MoveDistract(damage * IN_MILLISECONDS);
}

void Spell::EffectPickPocket(SpellEffectEntry const* /*effect*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // victim must be creature and attackable
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->IsFriendlyTo(unitTarget))
        return;

    // victim have to be alive and humanoid or undead
    if (unitTarget->isAlive() && (unitTarget->GetCreatureTypeMask() & CREATURE_TYPEMASK_HUMANOID_OR_UNDEAD) != 0)
    {
        int32 chance = 10 + int32(m_caster->getLevel()) - int32(unitTarget->getLevel());

        if (chance > irand(0, 19))
        {
            // Stealing successful
            //DEBUG_LOG("Sending loot from pickpocket");
            ((Player*)m_caster)->SendLoot(unitTarget->GetObjectGuid(),LOOT_PICKPOCKETING);
        }
        else
        {
            // Reveal action + get attack
            m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            unitTarget->AttackedBy(m_caster);
        }
    }
}

void Spell::EffectAddFarsight(SpellEffectEntry const* effect)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 duration = GetSpellDuration(m_spellInfo);
    DynamicObject* dynObj = new DynamicObject;

    // set radius to 0: spell not expected to work as persistent aura
    if(!dynObj->Create(m_caster->GetMap()->GenerateLocalLowGuid(HIGHGUID_DYNAMICOBJECT), m_caster, m_spellInfo->Id, SpellEffectIndex(effect->EffectIndex), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, duration, 0, DYNAMIC_OBJECT_FARSIGHT_FOCUS))
    {
        delete dynObj;
        return;
    }

    m_caster->AddDynObject(dynObj);
    m_caster->GetMap()->Add(dynObj);

    ((Player*)m_caster)->GetCamera().SetView(dynObj);
}

void Spell::DoSummonWild(SpellEffectEntry const * effect, uint32 forceFaction, bool unlimited)
{
    uint32 creature_entry = effect->EffectMiscValue;
    if (!creature_entry)
        return;

    uint32 level = m_caster->getLevel();

    // level of creature summoned using engineering item based at engineering skill level
    if (m_caster->GetTypeId()==TYPEID_PLAYER && m_CastItem)
    {
        ItemPrototype const *proto = m_CastItem->GetProto();
        if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
        {
            uint16 skill202 = ((Player*)m_caster)->GetSkillValue(SKILL_ENGINEERING);
            if (skill202)
                level = skill202/5;
        }
    }

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));
    TempSummonType summonType = (m_duration == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_DESPAWN;

    int32 amount = damage > 0 ? damage : 1;

    if (!unlimited && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        SummonUnitList& summonList = ((Player*)m_caster)->GetSummonUnitList();
        SummonUnitList list;

        for (SummonUnitList::const_iterator i = summonList.begin(); i != summonList.end(); ++i)
        {
            if ((*i)->GetTypeId() != TYPEID_UNIT)
                continue;

            Creature* creature = (Creature*)*i;

            if (creature->IsTemporarySummon() && (*i)->isAlive() &&
                creature->GetEntry() == creature_entry)
                list.push_back(*i);
        }

        while (list.size() > amount - 1)
        {
            ((TemporarySummon*)list.front())->UnSummon();
            list.remove(list.front());
        }
    }

    float px, py, pz;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        // Summon in random point all other units if location present
        m_caster->GetRandomPoint(center_x, center_y, center_z, radius, px, py, pz);
    }
    // Summon if dest location not present near caster
    else
    {
        if (radius > 0.0f)
        {
            // not using bounding radius of caster here
            m_caster->GetClosePoint(px, py, pz, 0.0f, radius);
        }
        else
        {
            // EffectRadiusIndex 0 or 36
            px = m_caster->GetPositionX();
            py = m_caster->GetPositionY();
            pz = m_caster->GetPositionZ();
        }
    }

    if (Creature* summon = m_caster->SummonCreature(creature_entry, px, py, pz, m_caster->GetOrientation(), summonType, m_duration, false, level))
    {
        summon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

        // UNIT_FIELD_CREATEDBY are not set for these kind of spells.
        // Does exceptions exist? If so, what are they?
        // Amaru: unforunately, commenting this break vehicles...
        summon->SetCreatorGuid(m_caster->GetObjectGuid());
        
        if(m_caster->GetTypeId() == TYPEID_PLAYER)
            ((Player *) m_caster)->AddSummonUnit(summon);

        if(forceFaction)
            summon->setFaction(forceFaction);

        if (m_spellInfo->Id == 73332)
            summon->PlayDistanceSound(10896); // Lament of the Highbourne

        // Notify original caster if not done already
        if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
            ((Creature*)m_originalCaster)->AI()->JustSummoned(summon);

        // Ghoul emote
        if (summon->GetEntry() == 24207 || summon->GetEntry() == 26125 || summon->GetEntry() == 28528)
            summon->HandleEmote(EMOTE_ONESHOT_EMERGE);

        // Pack Hobgoblin (Racial)
        if (m_spellInfo->Id == 69046)
            summon->GetMotionMaster()->MoveFollow(m_caster, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);

        DEBUG_LOG("DoSummonWild: summoned npc %u from spell %u at %f %f %f map %u summonType %u duration %i",
            creature_entry, m_spellInfo->Id, px, py, pz, m_caster->GetMapId(), summonType, m_duration);
    }
    else
        sLog.outError("DoSummonWild: failed to summon npc %u from spell %u at %f %f %f map %u summonType %u duration %i",
            creature_entry, m_spellInfo->Id, px, py, pz, m_caster->GetMapId(), summonType, m_duration);
}

void Spell::DoSummonGuardian(SpellEffectEntry const * effect, uint32 forceFaction)
{
    uint32 pet_entry = effect->EffectMiscValue;
    if (!pet_entry)
        return;

    SummonPropertiesEntry const* propEntry = sSummonPropertiesStore.LookupEntry(effect->EffectMiscValueB);
    if (!propEntry)
        return;

    CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonGuardian: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    PetType petType = propEntry->Title == UNITNAME_SUMMON_TITLE_COMPANION ? PROTECTOR_PET : GUARDIAN_PET;

    // second direct cast unsummon guardian(s) (guardians without like functionality have cooldown > spawn time)
    if (!m_IsTriggeredSpell && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        bool found = false;
        // including protector
        while (Pet* old_summon = m_caster->FindGuardianWithEntry(pet_entry))
        {
            old_summon->Unsummon(PET_SAVE_AS_DELETED, m_caster);
            found = true;
        }

        if (found)
            return;
    }

    // protectors allowed only in single amount
    if (petType == PROTECTOR_PET && m_CastItem)
        if (Pet* old_protector = m_caster->GetProtectorPet())
            old_protector->Unsummon(PET_SAVE_AS_DELETED, m_caster);

    // in another case summon new
    uint32 level = m_caster->getLevel();

    // level of pet summoned using engineering item based at engineering skill level
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_CastItem)
    {
        ItemPrototype const *proto = m_CastItem->GetProto();
        if (proto && proto->RequiredSkill == SKILL_ENGINEERING)
        {
            uint16 skill202 = ((Player*)m_caster)->GetSkillValue(SKILL_ENGINEERING);
            if (skill202)
            {
                level = skill202 / 5;
            }
        }
    }

    // select center of summon position
    float center_x = m_targets.m_destX;
    float center_y = m_targets.m_destY;
    float center_z = m_targets.m_destZ;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));

    int32 amount = damage > 0 ? damage : 1;

    if (amount > 10)
        amount = 1;

    for(int32 count = 0; count < amount; ++count)
    {
        Pet* spawnCreature = new Pet(petType);

        // If dest location if present
        // Summon 1 unit in dest location
        CreatureCreatePos pos(m_caster->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, -m_caster->GetOrientation(), m_caster->GetPhaseMask());

        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        {
            // Summon in random point all other units if location present
            // do not randomize for Ring of Frost
            if (count > 0 && m_spellInfo->Id != 82676)
            {
                float x, y, z;
                m_caster->GetRandomPoint(center_x, center_y, center_z, radius, x, y, z);
                pos = CreatureCreatePos(m_caster->GetMap(), x, y, z, m_caster->GetOrientation(), m_caster->GetPhaseMask());
            }
        }
        // Summon if dest location not present near caster
        else
            pos = CreatureCreatePos(m_caster, m_caster->GetOrientation());

        Map *map = m_caster->GetMap();
        uint32 pet_number = sObjectMgr.GeneratePetNumber();
        if (!spawnCreature->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
        {
            sLog.outError("Spell::DoSummonGuardian: can't create creature entry %u for spell %u.", pet_entry, m_spellInfo->Id);
            delete spawnCreature;
            return;
        }

        spawnCreature->SetRespawnCoord(pos);

        if (m_duration > 0)
            spawnCreature->SetDuration(m_duration);

        //spawnCreature->SetName("");                       // generated by client
        spawnCreature->SetOwnerGuid(m_caster->GetObjectGuid());
        spawnCreature->setPowerType(POWER_MANA);
        spawnCreature->SetUInt32Value(UNIT_NPC_FLAGS, spawnCreature->GetCreatureInfo()->npcflag);
        spawnCreature->setFaction(forceFaction ? forceFaction : m_caster->getFaction());
        spawnCreature->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, 0);
        spawnCreature->SetCreatorGuid(m_caster->GetObjectGuid());
        spawnCreature->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

        spawnCreature->InitStatsForLevel(level, m_caster);
        spawnCreature->GetCharmInfo()->SetPetNumber(pet_number, false);

        // Mirror Image
        if (pet_entry == 31216)
        {
            switch (effect->EffectImplicitTargetA)      //setting follow angles for mirrors
            {
            case 49:
                spawnCreature->SetFollowAngle(2*PET_FOLLOW_ANGLE);
                break;
            case 50:
                spawnCreature->SetFollowAngle(3*PET_FOLLOW_ANGLE);
                break;
            default:
                spawnCreature->SetFollowAngle(PET_FOLLOW_ANGLE);
                break;
            }
        }

        spawnCreature->AIM_Initialize();

        m_caster->AddGuardian(spawnCreature);

        map->Add((Creature*)spawnCreature);

        // Notify Summoner
        if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
            ((Creature*)m_caster)->AI()->JustSummoned(spawnCreature);
        if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
            ((Creature*)m_originalCaster)->AI()->JustSummoned(spawnCreature);

        // Ghoul emote
        if (spawnCreature->GetEntry() == 24207 || spawnCreature->GetEntry() == 26125 || spawnCreature->GetEntry() == 28528)
            spawnCreature->HandleEmote(EMOTE_ONESHOT_EMERGE);

        // reload creature addon for Moonwel because of creator-dependent auras
        if (spawnCreature->GetEntry() == 53883)
            spawnCreature->LoadCreatureAddon(true);
    }
}

// Used only for snake trap
void Spell::DoSummonSnakes(SpellEffectEntry const * effect)
{
    uint32 creature_entry = effect->EffectMiscValue;
    if (!creature_entry || !m_caster)
        return;

    // Find trap GO and get it coordinates to spawn snakes
    GameObject* pTrap = m_caster->GetMap()->GetGameObject(m_originalCasterGUID);
    if (!pTrap)
    {
        sLog.outError("DoSummonSnakes failed to find trap for caster %s", m_caster->GetGuidStr().c_str());
        return;
    }

    float position_x, position_y, position_z;
    pTrap->GetPosition(position_x, position_y, position_z);

    int32 amount = damage > 0 ? damage : 1;
    if (amount > 10) amount = 10;

    for(int32 count = 0; count < amount; ++count)
    {
        // Summon snakes
        Creature *pSummon = m_caster->SummonCreature(creature_entry, position_x, position_y, position_z, m_caster->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, m_duration);
        if (!pSummon)
            return;
        // Valid position
        if (!pSummon->IsPositionValid())
        {
            sLog.outError("DoSummonSnakes failed to summon snakes for Unit %s bacause of invalid position (x = %f, y = %f, z = %f map = %u)",
                m_caster->GetGuidStr().c_str(), position_x, position_y, position_z, m_caster->GetMapId());
            delete pSummon;
            continue;
        }

        // Apply stats
        pSummon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
        pSummon->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE | UNIT_FLAG_PET_IN_COMBAT | UNIT_FLAG_PVP);
        pSummon->SetCreatorGuid(m_caster->GetObjectGuid());
        pSummon->SetOwnerGuid(m_caster->GetObjectGuid());
        pSummon->setFaction(m_caster->getFaction());
        pSummon->SetLevel(m_caster->getLevel());
        pSummon->SetMaxHealth(m_caster->getLevel()+ urand(20,30));

        if(m_caster->IsPvP())
            pSummon->SetPvP(true);
        if(m_caster->IsFFAPvP())
            pSummon->SetFFAPvP(true);
    }
}

void Spell::DoSummonVehicle(SpellEffectEntry const * effect, uint32 forceFaction)
{
    if (m_caster->hasUnitState(UNIT_STAT_ON_VEHICLE))
    {
        if (m_spellInfo->Attributes & SPELL_ATTR_UNK7)
            m_caster->RemoveSpellsCausingAura(SPELL_AURA_CONTROL_VEHICLE);
        else
            return;
    }

    uint32 vehicle_entry = effect->EffectMiscValue;

    if (!vehicle_entry)
        return;

    SpellEntry const* m_mountspell = sSpellStore.LookupEntry(effect->EffectBasePoints != 0 ? effect->CalculateSimpleValue() : VEHICLE_SPELL_RIDE_HARDCODED);

    if (!m_mountspell || !IsSpellHaveAura(m_mountspell, SPELL_AURA_CONTROL_VEHICLE))
        m_mountspell = sSpellStore.LookupEntry(VEHICLE_SPELL_RIDE_HARDCODED);
    // Used BasePoint mount spell, if not present - hardcoded (by Blzz).

    float px, py, pz;
    // If dest location present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        px = m_targets.m_destX;
        py = m_targets.m_destY;
        pz = m_targets.m_destZ;
    }
    // Summon if dest location not present near caster
    else
        m_caster->GetClosePoint(px, py, pz,m_caster->GetObjectBoundingRadius());

    TempSummonType summonType = (GetSpellDuration(m_spellInfo) == 0) ? TEMPSUMMON_DEAD_DESPAWN : TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN;

    if (Creature* vehicle = m_caster->SummonCreature(vehicle_entry,px,py,pz,m_caster->GetOrientation(),summonType,GetSpellDuration(m_spellInfo),true))
    {
        if (!vehicle->GetObjectGuid().IsVehicle())
        {
            sLog.outError("DoSommonVehicle: Creature (guid %s, entry %u) summoned, but this is not vehicle. Correct vehicleId in creature_template.", vehicle->GetGuidStr().c_str(), vehicle->GetEntry());
            vehicle->ForcedDespawn();
            return;
        }

        vehicle->setFaction(forceFaction ? forceFaction : m_caster->getFaction());
        vehicle->SetUInt32Value(UNIT_CREATED_BY_SPELL,m_spellInfo->Id);
        vehicle->SetCreatorGuid(m_caster->GetObjectGuid());
        m_caster->CastSpell(vehicle, m_mountspell, true);
        DEBUG_LOG("Caster (guid %s) summon vehicle (guid %s, entry %u) and mounted with spell %u ", m_caster->GetGuidStr().c_str(), vehicle->GetGuidStr().c_str(), vehicle->GetEntry(), m_mountspell->Id);

        // Notify Summoner
        if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
            ((Creature*)m_caster)->AI()->JustSummoned(vehicle);
    }
    else
        sLog.outError("Vehicle (caster guid %s, spellid %u, vehicle entry %u) NOT summoned by undefined reason. ", m_caster->GetGuidStr().c_str(), m_spellInfo->Id, vehicle_entry);
}

void Spell::EffectTeleUnitsFaceCaster(SpellEffectEntry const * effect)
{
    if (!unitTarget || unitTarget->IsTaxiFlying())
        return;

    float fx, fy, fz;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.getDestination(fx, fy, fz);
    else
    {
        float dis = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));
        m_caster->GetClosePoint(fx, fy, fz, unitTarget->GetObjectBoundingRadius(), dis);
    }

    unitTarget->NearTeleportTo(fx, fy, fz, -m_caster->GetOrientation(), unitTarget == m_caster);
}

void Spell::EffectLearnSkill(SpellEffectEntry const* effect)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (damage < 0)
        return;

    uint32 skillid =  effect->EffectMiscValue;
    uint16 skillval = ((Player*)unitTarget)->GetPureSkillValue(skillid);
    ((Player*)unitTarget)->SetSkill(skillid, skillval ? skillval : 1, damage * 75, damage);

    // Archaeology
    if (skillid == SKILL_ARCHAEOLOGY && sWorld.getConfig(CONFIG_BOOL_ARCHAEOLOGY_ENABLED))
    {
        ((Player*)unitTarget)->GenerateResearchSites();
        ((Player*)unitTarget)->GenerateResearchProjects();
    }
}

void Spell::EffectPlayMovie(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 movieId = effect->EffectMiscValue;
    if (!sMovieStore.LookupEntry(movieId))
        return;

    ((Player*)unitTarget)->SendMovieStart(movieId);
}

void Spell::EffectTradeSkill(SpellEffectEntry const* /*effect*/)
{
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    // uint32 skillid =  m_spellInfo->EffectMiscValue[i];
    // uint16 skillmax = ((Player*)unitTarget)->(skillid);
    // ((Player*)unitTarget)->SetSkill(skillid,skillval?skillval:1,skillmax+75);
}

void Spell::EffectEnchantItemPerm(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    uint32 enchant_id = effect->EffectMiscValue;
    if (!enchant_id)
        return;

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
        return;

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    Player* p_caster = (Player*)m_caster;
    // Enchanting a vellum requires special handling, as it creates a new item
    // instead of modifying an existing one.
    ItemPrototype const* targetProto = itemTarget->GetProto();
    if (targetProto->IsVellum() && effect->EffectItemType)
    {
        unitTarget = m_caster;
        DoCreateItem(effect, effect->EffectItemType);
        // Vellum target case: Target becomes additional reagent, new scroll item created instead in Spell::EffectEnchantItemPerm()
        // cannot already delete in TakeReagents() unfortunately
        p_caster->DestroyItemCount(targetProto->ItemId, 1, true);
        return;
    }

    // Using enchant stored on scroll does not increase enchanting skill! (Already granted on scroll creation)
    if (!(m_CastItem && m_CastItem->GetProto()->Flags & ITEM_FLAG_ENCHANT_SCROLL))
        p_caster->UpdateCraftSkill(m_spellInfo->Id);

    if (item_owner!=p_caster && p_caster->GetSession()->GetSecurity() > SEC_CURATOR && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE) )
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(),"GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
            p_caster->GetName(),p_caster->GetSession()->GetAccountId(),
            itemTarget->GetProto()->Name1,itemTarget->GetEntry(),
            item_owner->GetName(),item_owner->GetSession()->GetAccountId());
    }

    bool dontApply = 
        itemTarget->IsEquipped() 
        && (itemTarget->GetSlot() == EQUIPMENT_SLOT_MAINHAND || itemTarget->GetSlot() == EQUIPMENT_SLOT_OFFHAND || itemTarget->GetSlot() == EQUIPMENT_SLOT_RANGED)
        && !item_owner->CanUseEquippedWeapon(Player::GetAttackBySlot(itemTarget->GetSlot()));

    // remove old enchanting before applying new if equipped
    if (!dontApply) item_owner->ApplyEnchantment(itemTarget,PERM_ENCHANTMENT_SLOT,false);

    itemTarget->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchant_id, 0, 0, m_caster->GetObjectGuid());

    // add new enchanting if equipped
    if (!dontApply) item_owner->ApplyEnchantment(itemTarget,PERM_ENCHANTMENT_SLOT,true);

    itemTarget->SetSoulboundTradeable(NULL, item_owner, false);
}

void Spell::EffectEnchantItemPrismatic(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    uint32 enchant_id = effect->EffectMiscValue;
    if (!enchant_id)
        return;

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if (!pEnchant)
        return;

    // support only enchantings with add socket in this slot
    {
        bool add_socket = false;
        for(int i = 0; i < 3; ++i)
        {
            if (pEnchant->type[i]==ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET)
            {
                add_socket = true;
                break;
            }
        }
        if (!add_socket)
        {
            ERROR_LOG("Spell::EffectEnchantItemPrismatic: attempt apply enchant spell %u with SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC (%u) but without ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET (%u), not suppoted yet.",
                m_spellInfo->Id,SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC,ITEM_ENCHANTMENT_TYPE_PRISMATIC_SOCKET);
            return;
        }
    }

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if (!item_owner)
        return;

    if (item_owner!=p_caster && p_caster->GetSession()->GetSecurity() > SEC_CURATOR && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE) )
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(),"GM %s (Account: %u) enchanting(perm): %s (Entry: %d) for player: %s (Account: %u)",
            p_caster->GetName(),p_caster->GetSession()->GetAccountId(),
            itemTarget->GetProto()->Name1,itemTarget->GetEntry(),
            item_owner->GetName(),item_owner->GetSession()->GetAccountId());
    }

    bool dontApply = 
        itemTarget->IsEquipped() 
        && (itemTarget->GetSlot() == EQUIPMENT_SLOT_MAINHAND || itemTarget->GetSlot() == EQUIPMENT_SLOT_OFFHAND || itemTarget->GetSlot() == EQUIPMENT_SLOT_RANGED)
        && !item_owner->CanUseEquippedWeapon(Player::GetAttackBySlot(itemTarget->GetSlot()));

    // remove old enchanting before applying new if equipped
    if (!dontApply) item_owner->ApplyEnchantment(itemTarget,PRISMATIC_ENCHANTMENT_SLOT,false);

    itemTarget->SetEnchantment(PRISMATIC_ENCHANTMENT_SLOT, enchant_id, 0, 0, m_caster->GetObjectGuid());

    // add new enchanting if equipped
    if (!dontApply) item_owner->ApplyEnchantment(itemTarget,PRISMATIC_ENCHANTMENT_SLOT,true);

    itemTarget->SetSoulboundTradeable(NULL, item_owner, false);
}

void Spell::EffectEnchantItemTmp(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;

    // Rockbiter Weapon
    SpellClassOptionsEntry const* classOptions = m_spellInfo->GetSpellClassOptions();
    if (classOptions && classOptions->SpellFamilyName == SPELLFAMILY_SHAMAN && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000400000))
    {
        uint32 spell_id = 0;

        // enchanting spell selected by calculated damage-per-sec stored in Effect[1] base value
        // Note: damage calculated (correctly) with rounding int32(float(v)) but
        // RW enchantments applied damage int32(float(v)+0.5), this create  0..1 difference sometime
        switch(damage)
        {
            // Rank 1
            case  2: spell_id = 36744; break;               //  0% [ 7% ==  2, 14% == 2, 20% == 2]
            // Rank 2
            case  4: spell_id = 36753; break;               //  0% [ 7% ==  4, 14% == 4]
            case  5: spell_id = 36751; break;               // 20%
            // Rank 3
            case  6: spell_id = 36754; break;               //  0% [ 7% ==  6, 14% == 6]
            case  7: spell_id = 36755; break;               // 20%
            // Rank 4
            case  9: spell_id = 36761; break;               //  0% [ 7% ==  6]
            case 10: spell_id = 36758; break;               // 14%
            case 11: spell_id = 36760; break;               // 20%
            default:
                ERROR_LOG("Spell::EffectEnchantItemTmp: Damage %u not handled in S'RW",damage);
                return;
        }

        SpellEntry const *spellInfo = sSpellStore.LookupEntry(spell_id);
        if (!spellInfo)
        {
            ERROR_LOG("Spell::EffectEnchantItemTmp: unknown spell id %i", spell_id);
            return;
        }

        Spell *spell = new Spell(m_caster, spellInfo, true);
        SpellCastTargets targets;
        targets.setItemTarget( itemTarget );
        spell->prepare(&targets);
        return;
    }

    if (!itemTarget)
        return;

    uint32 enchant_id = effect->EffectMiscValue;

    if (!enchant_id)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have 0 as enchanting id",m_spellInfo->Id,effect->EffectIndex);
        return;
    }

    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
    if(!pEnchant)
    {
        sLog.outError("Spell %u Effect %u (SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY) have nonexistent enchanting id %u ",m_spellInfo->Id,effect->EffectIndex,enchant_id);
        return;
    }

    // select enchantment duration
    uint32 duration;

    // rogue family enchantments exception by duration
    if(m_spellInfo->Id == 38615)
        duration = 1800;                                    // 30 mins
    // other rogue family enchantments always 1 hour (some have spell damage=0, but some have wrong data in EffBasePoints)
    else if(classOptions && classOptions->SpellFamilyName == SPELLFAMILY_ROGUE)
        duration = 3600;                                    // 1 hour
    // shaman family enchantments
    else if(classOptions && classOptions->SpellFamilyName == SPELLFAMILY_SHAMAN)
        duration = 1800;                                    // 30 mins
    // other cases with this SpellVisual already selected
    else if(m_spellInfo->SpellVisual[0] == 215)
        duration = 1800;                                    // 30 mins
    // some fishing pole bonuses except Glow Worm
    else if(m_spellInfo->SpellVisual[0] == 563 && m_spellInfo->Id != 64401)
        duration = 600;                                     // 10 mins
    // shaman rockbiter enchantments
    else if(m_spellInfo->SpellVisual[0] == 0)
        duration = 1800;                                    // 30 mins
    else if(m_spellInfo->Id == 29702)
        duration = 300;                                     // 5 mins
    else if(m_spellInfo->Id == 37360)
        duration = 300;                                     // 5 mins
    // default case
    else
        duration = 3600;                                    // 1 hour

    // item can be in trade slot and have owner diff. from caster
    Player* item_owner = itemTarget->GetOwner();
    if(!item_owner)
        return;

    if(item_owner!=p_caster && p_caster->GetSession()->GetSecurity() > SEC_CURATOR && sWorld.getConfig(CONFIG_BOOL_GM_LOG_TRADE) )
    {
        sLog.outCommand(p_caster->GetSession()->GetAccountId(),"GM %s (Account: %u) enchanting(temp): %s (Entry: %d) for player: %s (Account: %u)",
            p_caster->GetName(), p_caster->GetSession()->GetAccountId(),
            itemTarget->GetProto()->Name1, itemTarget->GetEntry(),
            item_owner->GetName(), item_owner->GetSession()->GetAccountId());
    }

    bool dontApply = 
        itemTarget->IsEquipped() 
        && (itemTarget->GetSlot() == EQUIPMENT_SLOT_MAINHAND || itemTarget->GetSlot() == EQUIPMENT_SLOT_OFFHAND || itemTarget->GetSlot() == EQUIPMENT_SLOT_RANGED)
        && !item_owner->CanUseEquippedWeapon(Player::GetAttackBySlot(itemTarget->GetSlot()));

    // remove old enchanting before applying new if equipped
    if (!dontApply) item_owner->ApplyEnchantment(itemTarget,TEMP_ENCHANTMENT_SLOT, false);

    itemTarget->SetEnchantment(TEMP_ENCHANTMENT_SLOT, enchant_id, duration * 1000, 0, m_caster->GetObjectGuid());

    // add new enchanting if equipped
    if (!dontApply) item_owner->ApplyEnchantment(itemTarget, TEMP_ENCHANTMENT_SLOT, true);
}

void Spell::EffectTameCreature(SpellEffectEntry const* /*effect*/)
{
    // Caster must be player, checked in Spell::CheckCast
    // Spell can be triggered, we need to check original caster prior to caster
    Player* plr = (Player*)GetAffectiveCaster();

    Creature* creatureTarget = (Creature*)unitTarget;

    // cast finish successfully
    //SendChannelUpdate(0);
    finish();

    PetSaveMode slot = plr->GetFreeStableSlot();
    if (slot == PET_SAVE_NOT_IN_SLOT)
        return;

    Pet* pet = new Pet(HUNTER_PET);

    if(!pet->CreateBaseAtCreature(creatureTarget))
    {
        delete pet;
        return;
    }

    pet->SetOwnerGuid(plr->GetObjectGuid());
    pet->SetCreatorGuid(plr->GetObjectGuid());
    pet->setFaction(plr->getFaction());
    pet->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    if (plr->IsPvP())
        pet->SetPvP(true);

    if (plr->IsFFAPvP())
        pet->SetFFAPvP(true);

    // level of hunter pet can't be less owner level at 5 levels
    uint32 level = creatureTarget->getLevel() + 5 < plr->getLevel() ? (plr->getLevel() - 5) : creatureTarget->getLevel();

    if (!pet->InitStatsForLevel(level))
    {
        sLog.outError("Pet::InitStatsForLevel() failed for creature (Entry: %u)!", creatureTarget->GetEntry());
        delete pet;
        return;
    }

    pet->GetCharmInfo()->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);
    // this enables pet details window (Shift+P)
    pet->AIM_Initialize();
    pet->InitPetCreateSpells();
    pet->InitLevelupSpellsForLevel();
    pet->InitTalentForLevel();
    pet->SetHealth(pet->GetMaxHealth());

    // "kill" original creature
    creatureTarget->ForcedDespawn();

    // prepare visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level - 1);

    // add to world
    pet->GetMap()->Add((Creature*)pet);

    // visual effect for levelup
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, level);

    pet->m_actualSlot = slot;

    // caster have pet now
    plr->SetPet(pet);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    plr->PetSpellInitialize();
}

void Spell::EffectSummonPet(SpellEffectEntry const* effect)
{
    PetSaveMode slot = PET_SAVE_NOT_IN_SLOT;
    uint32 petentry = effect->EffectMiscValue;
    if (!petentry && m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_GENERIC)
        slot = PetSaveMode(damage);

    Pet *OldSummon = m_caster->GetPet();

    // if pet requested type already exist
    if( OldSummon )
    {
        if(petentry == 0 || OldSummon->GetEntry() == petentry)
        {
            // pet in corpse state can't be summoned
            if( OldSummon->isDead() )
                return;

            OldSummon->GetMap()->Remove((Creature*)OldSummon,false);

            float px, py, pz;
            m_caster->GetClosePoint(px, py, pz, OldSummon->GetObjectBoundingRadius());

            OldSummon->Relocate(px, py, pz, OldSummon->GetOrientation());
            m_caster->GetMap()->Add((Creature*)OldSummon);

            if(m_caster->GetTypeId() == TYPEID_PLAYER && OldSummon->isControlled() )
            {
                ((Player*)m_caster)->PetSpellInitialize();
            }
            return;
        }

        if(m_caster->GetTypeId() == TYPEID_PLAYER)
            OldSummon->Unsummon(OldSummon->getPetType() == HUNTER_PET ? PET_SAVE_AS_DELETED : PET_SAVE_NOT_IN_SLOT, m_caster);
        else
            return;
    }

    CreatureInfo const* cInfo = petentry ? ObjectMgr::GetCreatureTemplate(petentry) : NULL;

    // == 0 in case call current pet, check only real summon case
    if (petentry && !cInfo)
    {
        sLog.outErrorDb("EffectSummonPet: creature entry %u not found for spell %u.", petentry, m_spellInfo->Id);
        return;
    }

    Pet* NewSummon = new Pet;

    // petentry==0 for hunter "call pet" (current pet summoned if any)
    if (m_caster->GetTypeId() == TYPEID_PLAYER && NewSummon->LoadPetFromDB((Player*)m_caster, petentry, 0, false, slot))
        return;

    // not error in case fail hunter call pet
    if (!petentry)
    {
        delete NewSummon;
        return;
    }

    CreatureCreatePos pos(m_caster, m_caster->GetOrientation());

    Map *map = m_caster->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!NewSummon->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        delete NewSummon;
        return;
    }

    NewSummon->SetRespawnCoord(pos);

    uint32 petlevel = m_caster->getLevel();
    NewSummon->setPetType(SUMMON_PET);

    uint32 faction = m_caster->getFaction();
    if (m_caster->GetTypeId() == TYPEID_UNIT)
    {
        if (((Creature*)m_caster)->IsTotem())
            NewSummon->GetCharmInfo()->SetState(CHARM_STATE_REACT, REACT_AGGRESSIVE);
        else
            NewSummon->GetCharmInfo()->SetState(CHARM_STATE_REACT, REACT_DEFENSIVE);
    }

    NewSummon->SetOwnerGuid(m_caster->GetObjectGuid());
    NewSummon->SetCreatorGuid(m_caster->GetObjectGuid());
    NewSummon->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
    NewSummon->setFaction(faction);
    NewSummon->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL)));
    NewSummon->SetUInt32Value(UNIT_FIELD_PETEXPERIENCE, 0);
    NewSummon->SetUInt32Value(UNIT_FIELD_PETNEXTLEVELEXP, 1000);
    NewSummon->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);

    NewSummon->GetCharmInfo()->SetPetNumber(pet_number, true);
    // this enables pet details window (Shift+P)

    if(m_caster->IsPvP())
        NewSummon->SetPvP(true);

    if(m_caster->IsFFAPvP())
        NewSummon->SetFFAPvP(true);

    NewSummon->InitStatsForLevel(petlevel, m_caster);
    NewSummon->InitPetCreateSpells();
    NewSummon->InitLevelupSpellsForLevel();
    NewSummon->InitTalentForLevel();

    if (m_caster->GetTypeId() == TYPEID_PLAYER && NewSummon->getPetType() == SUMMON_PET)
    {
        // generate new name for summon pet
        std::string new_name = sObjectMgr.GeneratePetName(petentry);
        if (!new_name.empty())
            NewSummon->SetName(new_name);
    }

    if (NewSummon->getPetType() == HUNTER_PET)
    {
        NewSummon->RemoveByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED);
        NewSummon->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_ABANDONED);
    }

    NewSummon->AIM_Initialize();
    NewSummon->SetHealth(NewSummon->GetMaxHealth());
    NewSummon->SetPower(POWER_MANA, NewSummon->GetMaxPower(POWER_MANA));

    map->Add((Creature*)NewSummon);

    m_caster->SetPet(NewSummon);
    DEBUG_LOG("New Pet has guid %u", NewSummon->GetGUIDLow());

    if(m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        NewSummon->SavePetToDB(PET_SAVE_AS_CURRENT);
        ((Player*)m_caster)->PetSpellInitialize();
    }

    // Ghoul emote
    if (NewSummon->GetEntry() == 24207 || NewSummon->GetEntry() == 26125 ||NewSummon-> GetEntry() == 28528)
        NewSummon->HandleEmote(EMOTE_ONESHOT_EMERGE);
}

void Spell::EffectLearnPetSpell(SpellEffectEntry const* effect)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    Pet *pet = _player->GetPet();
    if(!pet)
        return;
    if(!pet->isAlive())
        return;

    SpellEntry const *learn_spellproto = sSpellStore.LookupEntry(effect->EffectTriggerSpell);
    if(!learn_spellproto)
        return;

    pet->learnSpell(learn_spellproto->Id);

    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    _player->PetSpellInitialize();
}

void Spell::EffectTaunt(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget)
        return;

    // this effect use before aura Taunt apply for prevent taunt already attacking target
    // for spell as marked "non effective at already attacking target"
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (unitTarget->getVictim()==m_caster)
        {
            SendCastResult(SPELL_FAILED_DONT_REPORT);
            return;
        }
    }

    // Also use this effect to set the taunter's threat to the taunted creature's highest value
    if (unitTarget->CanHaveThreatList() && unitTarget->getThreatManager().getCurrentVictim())
        unitTarget->getThreatManager().addThreat(m_caster,unitTarget->getThreatManager().getCurrentVictim()->getThreat());
}

void Spell::EffectWeaponDmg(SpellEffectEntry const* effect)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    // multiple weapon dmg effect workaround
    // execute only the last weapon damage
    // and handle all effects at once
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        switch(m_spellInfo->GetSpellEffectIdByIndex(SpellEffectIndex(j)))
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                if (j < int(effect->EffectIndex))           // we must calculate only at last weapon effect
                    return;
            break;
        }
    }

    // some spell specific modifiers
    bool spellBonusNeedWeaponDamagePercentMod = false;      // if set applied weapon damage percent mode to spell bonus

    float weaponDamagePercentMod = 1.0f;                    // applied to weapon damage and to fixed effect damage bonus
    float totalDamagePercentMod  = 1.0f;                    // applied to final bonus+weapon damage
    bool normalized = false;

    int32 spell_bonus = 0;                                  // bonus specific for spell

    SpellClassOptionsEntry const* classOptions = m_spellInfo->GetSpellClassOptions();

    switch(m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id)
            {
                // for spells with divided damage to targets
                case 66765: case 66809: case 67331:         // Meteor Fists
                case 67333:                                 // Meteor Fists
                case 69055:                                 // Bone Slice (Icecrown Citadel, Lord Marrowgar, normal)
                case 70814:                                 // Bone Slice (Icecrown Citadel, Lord Marrowgar, heroic)
                case 71021:                                 // Saber Lash
                {
                    uint32 count = 0;
                    for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        if(ihit->effectMask & (1<<effect->EffectIndex))
                            ++count;

                    totalDamagePercentMod /= float(count);  // divide to all targets
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Whirlwind
            if (m_spellInfo->Id == 1680)
            {
                if (effect->EffectIndex != EFFECT_INDEX_0)
                    break;

                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    break;

                if (!m_currentBasePoints[EFFECT_INDEX_2])
                    break;

                uint32 count = 0;
                for (TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    if (ihit->effectMask & (1 << effect->EffectIndex))
                        ++count;

                if (count >= 4)
                {
                    int32 secs = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_2);
                    ((Player*)m_caster)->SendModifyCooldown(m_spellInfo->Id, - secs * IN_MILLISECONDS);
                    m_currentBasePoints[EFFECT_INDEX_2] = 0;    // mark as decreased cd
                }
            }
            // Devastate
            else if (m_spellInfo->SpellVisual[0] == 12295 && m_spellInfo->SpellIconID == 1508)
            {
                // Sunder Armor
                Aura* sunder = unitTarget->GetAura(SPELL_AURA_MOD_RESISTANCE_PCT, SPELLFAMILY_WARRIOR, UI64LIT(0x0000000000004000), 0x00000000, m_caster->GetObjectGuid());

                // Devastate bonus and sunder armor refresh
                if (sunder)
                {
                    sunder->GetHolder()->RefreshHolder();
                    spell_bonus += sunder->GetStackAmount() * CalculateDamage(EFFECT_INDEX_1, unitTarget);
                }

                // Devastate causing Sunder Armor Effect
                // and no need to cast over max stack amount
                if (!sunder || sunder->GetStackAmount() < sunder->GetSpellProto()->GetStackAmount())
                    m_caster->CastSpell(unitTarget, 58567, true);
            }
            // Amaru: remove combopoints on overpower cast
            else if (m_spellInfo->Id == 7384)
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)m_caster)->ClearComboPoints();
                break;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Ambush
            if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->Id == 8676)
            {
                Item* weapon = ((Player*)m_caster)->GetWeaponForAttack(m_attackType,true,true);
                if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                    totalDamagePercentMod *= 1.447f;         // 144.7% to daggers
            }
            // Fan of Knives
            else if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->Id == 51723)
            {
                // search Vile Poisons
                if (SpellEntry const* talent = ((Player*)m_caster)->GetKnownTalentRankById(682))
                {
                    for (int i = BASE_ATTACK; i <= OFF_ATTACK; ++i)
                        if (Item* weapon = ((Player*)m_caster)->GetWeaponForAttack(WeaponAttackType(i), true, true))
                            if (roll_chance_i(talent->CalculateSimpleValue(EFFECT_INDEX_0)))
                                ((Player*)m_caster)->CastItemCombatSpell(unitTarget, WeaponAttackType(i));
                }
            }
            // Hemorrhage
            else if (m_caster->GetTypeId() == TYPEID_PLAYER && classOptions && (classOptions->SpellFamilyFlags & UI64LIT(0x2000000)))
            {
                Item* weapon = ((Player*)m_caster)->GetWeaponForAttack(m_attackType,true,true);
                if (weapon && weapon->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_DAGGER)
                    totalDamagePercentMod *= 1.45f;         // 145% to daggers
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Templar's Verdict
            if (m_spellInfo->Id == 85256)
            {
                switch (m_usedHolyPower)
                {
                    case 2:
                        totalDamagePercentMod *= 2.0f;
                        break;
                    case 3:
                        totalDamagePercentMod *= 47.0f / 6.0f;
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            switch (m_spellInfo->Id)
            {
                case 3044:              // Arcane Shot
                {
                    spell_bonus += int32(0.0483f * m_caster->GetTotalAttackPowerValue(RANGED_ATTACK));
                    break;
                }
                case 2643:              // Multi-Shot
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        break;

                    // Serpent Spread
                    if (SpellEntry const* talent = ((Player*)m_caster)->GetKnownTalentRankById(11698))
                    {
                        uint32 spellId = talent->Id == 87934 ? 88453 : 88466;
                        m_caster->CastSpell(unitTarget, spellId, true);
                    }
                    break;
                }
                case 19434:             // Aimed Shot
                case 82928:
                {
                    // "A powerful aimed shot that deals % ranged weapon damage plus (RAP * 0.724)+776."
                    spell_bonus += int32(0.724f * m_caster->GetTotalAttackPowerValue(RANGED_ATTACK));
                    break;
                }
                case 53351:             // Kill Shot
                {
                    spell_bonus += int32(0.45f * m_caster->GetTotalAttackPowerValue(RANGED_ATTACK));
                    break;
                }
                case 53209:             // Chimera Shot
                {
                    // "An instant shot that causes ranged weapon damage plus RAP*0.732+1620, refreshing the duration of  your Serpent Sting and healing you for 5% of your total health."
                    spell_bonus += int32(0.732f * m_caster->GetTotalAttackPowerValue(RANGED_ATTACK));
                    break;
                }
                case 56641:             // Steady Shot
                {
                    // "A steady shot that causes % weapon damage plus RAP*0.021+280. Generates 9 Focus."
                    spell_bonus += int32(0.021f * m_caster->GetTotalAttackPowerValue(RANGED_ATTACK));
                    break;
                }
                case 77767:             // Cobra Shot
                {
                    // "Deals weapon damage plus (276 + (RAP * 0.017)) in the form of Nature damage and increases the duration of your Serpent Sting on the target by 6 sec. Generates 9 Focus."
                    spell_bonus += int32(0.017f * m_caster->GetTotalAttackPowerValue(RANGED_ATTACK));
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Skyshatter Harness item set bonus
            // Stormstrike
            if(classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x001000000000))
            {
                Unit::AuraList const& m_OverrideClassScript = m_caster->GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for(Unit::AuraList::const_iterator citr = m_OverrideClassScript.begin(); citr != m_OverrideClassScript.end(); ++citr)
                {
                    // Stormstrike AP Buff
                    if ( (*citr)->GetModifier()->m_miscvalue == 5634 )
                    {
                        m_caster->CastSpell(m_caster, 38430, true, NULL, *citr);
                        break;
                    }
                }
            }
            // Lava Lash
            if (m_spellInfo->Id == 60103)
            {
                if (unitTarget && m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    // Improved Lava Lash
                    if (SpellEntry const * spellInfo = ((Player*)m_caster)->GetKnownTalentRankById(5563))
                    {
                        // Searing Flames
                        if (SpellAuraHolder* flames = unitTarget->GetSpellAuraHolder(77661, m_caster->GetObjectGuid()))
                        {
                            // Item - Shaman T12 Enhancement 2P Bonus
                            Aura* aura = m_caster->GetAura(99209, EFFECT_INDEX_0);

                            totalDamagePercentMod *= (flames->GetStackAmount() * (spellInfo->CalculateSimpleValue(EFFECT_INDEX_1) + (aura ? aura->GetModifier()->m_amount : 0)) + 100.0f) / 100.0f;
                            unitTarget->RemoveSpellAuraHolder(flames);
                        }

                        // Flame Shock
                        if (unitTarget->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, UI64LIT(0x10000000), 0, m_caster->GetObjectGuid()))
                        {
                            UnitList nearUnits;
                            MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(unitTarget, unitTarget, 12.0f);
                            MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck > searcher(nearUnits, u_check);
                            Cell::VisitAllObjects(unitTarget, searcher, 12.0f);
                            int targets = 4;
                            for (UnitList::iterator itr = nearUnits.begin(); itr != nearUnits.end() && targets >= 0; ++itr)
                            {
                                // does not already has Flame Shock from shaman
                                if (!(*itr)->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, UI64LIT(0x10000000), 0, m_caster->GetObjectGuid()))
                                {
                                    // cast Flame Shock
                                    m_caster->CastSpell(*itr, 8050, true);
                                    --targets;
                                }
                            }
                        }
                    }
                }
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Blood Strike, Heart Strike, Obliterate
            // Blood-Caked Strike
            if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0002000001400000) ||
                m_spellInfo->SpellIconID == 1736)
            {
                uint32 count = 0;
                Unit::SpellAuraHolderMap const& auras = unitTarget->GetSpellAuraHolderMap();
                for(Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
                {
                    if(itr->second->GetSpellProto()->GetDispel() == DISPEL_DISEASE &&
                        itr->second->GetCasterGuid() == m_caster->GetObjectGuid())
                        ++count;
                }

                // Burning Blood
                // Item - Death Knight T12 Blood 2P Bonus
                if (count < 2 && unitTarget->GetSpellAuraHolder(98957, m_caster->GetObjectGuid()))
                    count = 2;

                if (count)
                {
                    float bonus;
                    if (m_spellInfo->SpellIconID != 1736) // Blood Strike, Heart Strike, Obliterate
                    {
                        // Effect 3 damage is bonus
                         bonus = count * CalculateDamage(EFFECT_INDEX_2, unitTarget) / 100.0f;
                         // Obliterate store bonus * 2
                         if (m_spellInfo->Id == 49020)
                            bonus /= 2.0f;
                         // Blood Strike store bonus * 10
                         else if (m_spellInfo->Id == 45902)
                             bonus /= 10.0f;

                        // Item - Death Knight T8 Melee 4P Bonus
                        if (Aura* dummy = m_caster->GetDummyAura(64736))
                            bonus *= ((float)dummy->GetModifier()->m_amount+100.0f)/100.0f;
                    }
                    else // Blood-Caked Blade damage info taken from http://www.wowhead.com/forums&topic=54152.2 and Dr.Damage addon.
                       bonus = count * 0.5f;//Blood-Caked Blade damage = (0.25(base) + diseaseCount * 0.125)= 0.25*(1+diseaseCount * 0.5)

                    totalDamagePercentMod *= 1.0f + bonus;
                }

                // Heart Strike secondary target
                if (m_spellInfo->SpellIconID == 3145)
                    if (m_targets.getUnitTarget() != unitTarget)
                    {
                        int8 mod = 0;
                        for (TargetList::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
                        {
                            if ((itr->effectMask & (1 << effect->EffectIndex)) == 0)
                                continue;

                            if (itr->targetGUID == unitTarget->GetObjectGuid())
                            {
                                mod += 1;
                                break;
                            }
                        }

                        while (mod--)
                            weaponDamagePercentMod *= 0.75f ;
                    }
            }
            // Rune Strike
            if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x2000000000000000))
            {
                // Unlike other reactive dependent spells, Rune Strike has no cooldown, so reactive must be cleared here to prevent >1 Rune Strike per dodge/parry
                m_caster->ClearAllReactives();
                int32 count = CalculateDamage(EFFECT_INDEX_2, unitTarget);
                spell_bonus += int32(count * m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 100.0f);
            }
            // Glyph of Death Strike
            if( classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000010) &&
                m_caster->HasAura(59336))
            {
                int32 rp = m_caster->GetPower(POWER_RUNIC_POWER) / 10;
                if (rp > 100)
                    rp = 100;
                rp /= 5;

                totalDamagePercentMod *= 1.0f + 2.0f * rp / 100.0f;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Rend and Tear on Maul and Shred
            if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x00000000008800))
            {
                if (unitTarget && unitTarget->HasAuraState(AURA_STATE_BLEEDING))
                {
                    Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
                        if ((*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DRUID && (*i)->GetSpellProto()->SpellIconID == 2859 && (*i)->GetEffIndex() == EFFECT_INDEX_0)
                        {
                            totalDamagePercentMod *= ((*i)->GetModifier()->m_amount + 100.0f) / 100.0f;
                            break;
                        }
                }
            }
            break;
        }
    }

    int32 fixed_bonus = 0;
    for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
        if(!spellEffect)
            continue;

        switch(spellEffect->Effect)
        {
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
                fixed_bonus += CalculateDamage(SpellEffectIndex(j), unitTarget);
                break;
            case SPELL_EFFECT_NORMALIZED_WEAPON_DMG:
            {
                // Pulverize
                if (m_spellInfo->Id == 80313)
                {
                    int32 normalizedMod = 1;
                    if (unitTarget)
                    {
                        normalizedMod = 0;
                        if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(33745, m_caster->GetObjectGuid()))
                        {
                            normalizedMod = holder->GetStackAmount();
                            unitTarget->RemoveSpellAuraHolder(holder);

                            // crit bonus
                            if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(80951))
                            {
                                int32 bp =  spellInfo->CalculateSimpleValue(EFFECT_INDEX_0) * normalizedMod;
                                m_caster->CastCustomSpell(m_caster, spellInfo, &bp, NULL, NULL, true);
                            }
                        }
                    }

                    fixed_bonus += CalculateDamage(SpellEffectIndex(j), unitTarget) * normalizedMod * 6 / 10;
                }
                else
                    fixed_bonus += CalculateDamage(SpellEffectIndex(j), unitTarget);

                
                normalized = true;
                break;
            }
            case SPELL_EFFECT_WEAPON_PERCENT_DAMAGE:
                weaponDamagePercentMod *= float(CalculateDamage(SpellEffectIndex(j), unitTarget)) / 100.0f;

                // applied only to prev.effects fixed damage
                fixed_bonus = int32(fixed_bonus*weaponDamagePercentMod);
                break;
            default:
                break;                                      // not weapon damage effect, just skip
        }
    }

    // apply weaponDamagePercentMod to spell bonus also
    if(spellBonusNeedWeaponDamagePercentMod)
        spell_bonus = int32(spell_bonus*weaponDamagePercentMod);

    // non-weapon damage
    int32 bonus = spell_bonus + fixed_bonus;

    // apply to non-weapon bonus weapon total pct effect, weapon total flat effect included in weapon damage
    if(bonus)
    {
        UnitMods unitMod;
        switch(m_attackType)
        {
            default:
            case BASE_ATTACK:   unitMod = UNIT_MOD_DAMAGE_MAINHAND; break;
            case OFF_ATTACK:    unitMod = UNIT_MOD_DAMAGE_OFFHAND;  break;
            case RANGED_ATTACK: unitMod = UNIT_MOD_DAMAGE_RANGED;   break;
        }

        float weapon_total_pct  = m_caster->GetModifierValue(unitMod, TOTAL_PCT);
        bonus = int32(bonus*weapon_total_pct);
    }

    // + weapon damage with applied weapon% dmg to base weapon damage in call
    bonus += int32(m_caster->CalculateDamage(m_attackType, normalized)*weaponDamagePercentMod);

    // total damage
    bonus = int32(bonus*totalDamagePercentMod);

    // prevent negative damage
    m_damage+= uint32(bonus > 0 ? bonus : 0);

    // Hemorrhage
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x0000000002000000)))
        m_caster->AddComboPoints(unitTarget, 1);
    // Mangle (Cat): CP
    else if (m_spellInfo->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x0000040000000000)))
        AddTriggeredSpell(34071);

    // take ammo
    if(m_attackType == RANGED_ATTACK && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Item *pItem = ((Player*)m_caster)->GetWeaponForAttack(RANGED_ATTACK, true, false);

        // wands don't have ammo
        if (!pItem || pItem->GetProto()->SubClass == ITEM_SUBCLASS_WEAPON_WAND)
            return;

        if (pItem->GetProto()->InventoryType == INVTYPE_THROWN)
        {
            if(pItem->GetMaxStackCount()==1)
            {
                // decrease durability for non-stackable throw weapon
                ((Player*)m_caster)->DurabilityPointLossForEquipSlot(EQUIPMENT_SLOT_RANGED);
            }
            else
            {
                // decrease items amount for stackable throw weapon
                uint32 count = 1;
                ((Player*)m_caster)->DestroyItemCount( pItem, count, true);
            }
        }
    }
}

void Spell::EffectThreat(SpellEffectEntry const* /*effect*/)
{
    if(!unitTarget || !unitTarget->isAlive() || !m_caster->isAlive())
        return;

    if(!unitTarget->CanHaveThreatList())
        return;

    int32 bonus=0;
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        //Sunder Armor bonus threat
        if (m_spellInfo->IsFitToFamily(SPELLFAMILY_WARRIOR, UI64LIT(0x4000)))
            bonus += m_caster->GetTotalAttackPowerValue(BASE_ATTACK) / 20;
    }

    unitTarget->AddThreat(m_caster, float(damage+bonus), false, GetSpellSchoolMask(m_spellInfo), m_spellInfo);
}

void Spell::EffectHealMaxHealth(SpellEffectEntry const* /*effect*/)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    uint32 heal = m_caster->GetMaxHealth();

    m_healing += heal;
}

void Spell::EffectInterruptCast(SpellEffectEntry const* effect)
{
    if(!unitTarget)
        return;
    if(!unitTarget->isAlive())
        return;

    uint32 spellSchoolMask = 0;
    // TODO: not all spells that used this effect apply cooldown at school spells
    // also exist case: apply cooldown to interrupted cast only and to all spells
    for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
    {
        if (Spell* spell = unitTarget->GetCurrentSpell(CurrentSpellTypes(i)))
        {
            SpellEntry const* curSpellInfo = spell->m_spellInfo;
            // check if we can interrupt spell
            if ((spell->getState() == SPELL_STATE_CASTING || spell->getState() == SPELL_STATE_PREPARING && spell->GetCastTime() > 0.0f) &&
                (curSpellInfo->GetInterruptFlags() & SPELL_INTERRUPT_FLAG_INTERRUPT) && curSpellInfo->GetPreventionType() == SPELL_PREVENTION_TYPE_SILENCE)
            {
                unitTarget->ProhibitSpellSchool(GetSpellSchoolMask(curSpellInfo), unitTarget->CalculateAuraDuration(m_spellInfo, (1 << effect->EffectIndex), GetSpellDuration(m_spellInfo), m_caster, spell));
                unitTarget->InterruptSpell(CurrentSpellTypes(i), false);
                spellSchoolMask = GetSpellSchoolMask(curSpellInfo);
            }
        }
    }

    if (spellSchoolMask && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        // Kick
        if (m_spellInfo->Id == 1766)
        {
            // Glyph of Kick
            if (Aura* glyph = m_caster->GetAura(56805, EFFECT_INDEX_1))
                ((Player*)m_caster)->SendModifyCooldown(m_spellInfo->Id, -glyph->GetModifier()->m_amount);
        }
        // Counterspell
        else if (m_spellInfo->Id == 2139)
        {
            // Invocation
            if (SpellEntry const * spellProto = ((Player*)m_caster)->GetKnownTalentRankById(10864))
                if (SpellEffectEntry const * effect = spellProto->GetSpellEffect(EFFECT_INDEX_0))
                {
                    int32 bp = effect->CalculateSimpleValue();
                    m_caster->CastCustomSpell(m_caster, effect->EffectTriggerSpell, &bp, NULL, NULL, true);
                }
        }
        // Pummel
        else if (m_spellInfo->Id == 6552)
        {
            // Rude Interruption
            if (SpellEntry const * spellProto = ((Player*)m_caster)->GetKnownTalentRankById(11415))
                if (SpellEffectEntry const * effect = spellProto->GetSpellEffect(EFFECT_INDEX_0))
                    m_caster->CastSpell(m_caster, effect->EffectTriggerSpell, true);
        }
        // Silencing Shot
        else if (m_spellInfo->Id == 34490)
        {
            // Glyph of Silencing Shot
            if (Aura* aura = m_caster->GetAura(56836, EFFECT_INDEX_0))
                m_caster->CastSpell(m_caster, aura->GetSpellEffect()->EffectTriggerSpell, true);
        }
        // Wind Shear
        else if (m_spellInfo->Id == 57994)
        {
            Player* plrCaster = (Player*)m_caster;
            // Frozen Power
            if (SpellEntry const * spellProto = plrCaster->GetKnownTalentRankById(11220))
            {
                uint32 triggeredSpell = 0;
                if (spellSchoolMask & SPELL_SCHOOL_MASK_FIRE)
                    triggeredSpell = 97618;
                else if (spellSchoolMask & SPELL_SCHOOL_MASK_FROST)
                    triggeredSpell = 97619;
                else if (spellSchoolMask & SPELL_SCHOOL_MASK_NATURE)
                    triggeredSpell = 97620;
                else if (spellSchoolMask & SPELL_SCHOOL_MASK_ARCANE)
                    triggeredSpell = 97621;
                else if (spellSchoolMask & SPELL_SCHOOL_MASK_SHADOW)
                    triggeredSpell = 97622;
                if (triggeredSpell)
                {
                    int32 bp = GetResistancesAtLevel(m_caster->getLevel()) * (spellProto->Id == 16086 ? 0.5f : 1.0f);
                    m_caster->CastCustomSpell(m_caster, triggeredSpell, &bp, NULL, NULL, true);
                }
            }
        }
    }
}

void Spell::EffectSummonObjectWild(SpellEffectEntry const* effect)
{
    uint32 gameobject_id = effect->EffectMiscValue;

    GameObject* pGameObj = new GameObject;

    WorldObject* target = focusObject;
    if( !target )
        target = m_caster;

    float x, y, z;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.getDestination(x, y, z);
    else
        m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);

    Map *map = target->GetMap();

    if(!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map,
        m_caster->GetPhaseMask(), x, y, z, target->GetOrientation()))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetRespawnTime(m_duration > 0 ? m_duration/IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    // Wild object not have owner and check clickable by players
    map->Add(pGameObj);

    // Store the GO to the caster
    m_caster->AddWildGameObject(pGameObj);

    if(pGameObj->GetGoType() == GAMEOBJECT_TYPE_FLAGDROP && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Player *pl = (Player*)m_caster;
        BattleGround* bg = ((Player *)m_caster)->GetBattleGround();

        switch(pGameObj->GetMapId())
        {
            case 489:                                       //WS
            {
                if(bg && bg->GetTypeID(true)==BATTLEGROUND_WS && bg->GetStatus() == STATUS_IN_PROGRESS)
                {
                    Team team = pl->GetTeam() == ALLIANCE ? HORDE : ALLIANCE;

                    ((BattleGroundWS*)bg)->SetDroppedFlagGuid(pGameObj->GetObjectGuid(), team);
                }
                break;
            }
            case 566:                                       //EY
            {
                if(bg && bg->GetTypeID(true)==BATTLEGROUND_EY && bg->GetStatus() == STATUS_IN_PROGRESS)
                {
                    ((BattleGroundEY*)bg)->SetDroppedFlagGuid(pGameObj->GetObjectGuid());
                }
                break;
            }
            case 726:                                       //TP
            {
                if(bg && bg->GetTypeID(true)==BATTLEGROUND_TP && bg->GetStatus() == STATUS_IN_PROGRESS)
                {
                    Team team = pl->GetTeam() == ALLIANCE ? HORDE : ALLIANCE;

                    ((BattleGroundTP*)bg)->SetDroppedFlagGuid(pGameObj->GetObjectGuid(), team);
                }
                break;
            }
        }
    }

    // Drop Seaforium Charge
    if (m_spellInfo->Id == 52417 && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (BattleGround* bg = ((Player*)m_caster)->GetBattleGround())
            if (bg->GetTypeID(true) == BATTLEGROUND_SA)
                ((BattleGroundSA*)bg)->HandleSummonedBomb(pGameObj);
    }

    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);
    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
        ((Creature*)m_originalCaster)->AI()->JustSummoned(pGameObj);
}

void Spell::EffectScriptEffect(SpellEffectEntry const* effect)
{
    // TODO: we must implement hunter pet summon at login there (spell 6962)
    switch(m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id)
            {
                case 8856:                                  // Bending Shinbone
                {
                    if (!itemTarget && m_caster->GetTypeId()!=TYPEID_PLAYER)
                        return;

                    uint32 spell_id = 0;
                    switch(urand(1, 5))
                    {
                        case 1:  spell_id = 8854; break;
                        default: spell_id = 8855; break;
                    }

                    m_caster->CastSpell(m_caster,spell_id,true,NULL);
                    return;
                }
                case 12355:                                     // Impact
                {
                    if (!unitTarget)
                        return;

                    Unit* mainTarget = m_targets.getUnitTarget();
                    if (!mainTarget)
                        return;

                    // Pyroblast
                    if (SpellAuraHolder* holder = mainTarget->GetSpellAuraHolder(11366, m_caster->GetObjectGuid()))
                        if (Aura* aura = holder->GetAuraByEffectIndex(EFFECT_INDEX_1))
                        {
                            int32 bp = aura->GetModifier()->m_amount;
                            m_caster->m_nextCustomSpellData.SetFlag(CUSTOM_SPELL_FLAG_NO_COST);
                            m_caster->CastCustomSpell(unitTarget, holder->GetId(), NULL, &bp, NULL, true);
                        }

                    // Ignite
                    if (SpellAuraHolder* holder = mainTarget->GetSpellAuraHolder(12654, m_caster->GetObjectGuid()))
                        if (Aura* aura = holder->GetAuraByEffectIndex(EFFECT_INDEX_0))
                        {
                            int32 bp = aura->GetModifier()->m_amount;
                            m_caster->m_nextCustomSpellData.SetFlag(CUSTOM_SPELL_FLAG_NO_COST);
                            m_caster->CastCustomSpell(unitTarget, holder->GetId(), &bp, NULL, NULL, true);
                        }
                    // Living Bomb
                    if (SpellAuraHolder* holder = mainTarget->GetSpellAuraHolder(44457, m_caster->GetObjectGuid()))
                        if (Aura* aura = holder->GetAuraByEffectIndex(EFFECT_INDEX_0))
                        {
                            int32 bp = aura->GetModifier()->m_amount;
                            m_caster->m_nextCustomSpellData.SetFlag(CUSTOM_SPELL_FLAG_NO_COST);
                            m_caster->CastCustomSpell(unitTarget, holder->GetId(), &bp, NULL, NULL, true);
                        }
                    // Combustion
                    if (SpellAuraHolder* holder = mainTarget->GetSpellAuraHolder(83853, m_caster->GetObjectGuid()))
                        if (Aura* aura = holder->GetAuraByEffectIndex(EFFECT_INDEX_0))
                        {
                            int32 bp = aura->GetModifier()->m_amount;
                            m_caster->m_nextCustomSpellData.SetFlag(CUSTOM_SPELL_FLAG_NO_COST);
                            m_caster->CastCustomSpell(unitTarget, holder->GetId(), &bp, NULL, NULL, true);
                        }
                    return;
                }
                case 17512:                                 // Piccolo of the Flaming Fire
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->HandleEmote(EMOTE_STATE_DANCE);
                    return;
                }
                case 20589:                                 // Escape artist
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
                    return;
                }
                case 22539:                                 // Shadow Flame (All script effects, not just end ones to
                case 22972:                                 // prevent player from dodging the last triggered spell)
                case 22975:
                case 22976:
                case 22977:
                case 22978:
                case 22979:
                case 22980:
                case 22981:
                case 22982:
                case 22983:
                case 22984:
                case 22985:
                {
                    if (!unitTarget || !unitTarget->isAlive())
                        return;

                    // Onyxia Scale Cloak
                    if (unitTarget->GetDummyAura(22683))
                        return;

                    // Shadow Flame
                    m_caster->CastSpell(unitTarget, 22682, true);
                    return;
                }
                case 24194:                                 // Uther's Tribute
                case 24195:                                 // Grom's Tribute
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint8 race = m_caster->getRace();
                    uint32 spellId = 0;

                    switch(m_spellInfo->Id)
                    {
                        case 24194:
                            switch(race)
                            {
                                case RACE_HUMAN:            spellId = 24105; break;
                                case RACE_DWARF:            spellId = 24107; break;
                                case RACE_NIGHTELF:         spellId = 24108; break;
                                case RACE_GNOME:            spellId = 24106; break;
                                case RACE_DRAENEI:          spellId = 69533; break;
                            }
                            break;
                        case 24195:
                            switch(race)
                            {
                                case RACE_ORC:              spellId = 24104; break;
                                case RACE_UNDEAD:           spellId = 24103; break;
                                case RACE_TAUREN:           spellId = 24102; break;
                                case RACE_TROLL:            spellId = 24101; break;
                                case RACE_BLOODELF:         spellId = 69530; break;
                            }
                            break;
                    }

                    if (spellId)
                        m_caster->CastSpell(m_caster, spellId, true);

                    return;
                }
                case 24320:                                 // Poisonous Blood
                {
                    unitTarget->CastSpell(unitTarget, 24321, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 24590:                                 // Brittle Armor - need remove one 24575 Brittle Armor aura
                    unitTarget->RemoveAuraHolderFromStack(24575);
                    return;
                case 24714:                                 // Trick
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (roll_chance_i(14))                  // Trick (can be different critter models). 14% since below can have 1 of 6
                        m_caster->CastSpell(m_caster, 24753, true);
                    else                                    // Random Costume, 6 different (plus add. for gender)
                        m_caster->CastSpell(m_caster, 24720, true);

                    return;
                }
                case 24717:                                 // Pirate Costume
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Pirate Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24708 : 24709, true);
                    return;
                }
                case 24718:                                 // Ninja Costume
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Ninja Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24711 : 24710, true);
                    return;
                }
                case 24719:                                 // Leper Gnome Costume
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Leper Gnome Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24712 : 24713, true);
                    return;
                }
                /*case 24720:                                 // Hallowed Wand - Random
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spellId = 0;
                    switch(urand(0,6))
                    {
                        case 0: spellId = 24733; break;    // Bat
                        case 1: spellId = 24737; break;    // Ghost
                        case 2: spellId = 24719; break;    // Leper Gnome
                        case 3: spellId = 24718; break;    // Ninja
                        case 4: spellId = 24717; break;    // Pirate
                        case 5: spellId = 24724; break;    // Skeleton
                        case 6: spellId = 24741; break;    // Wisp
                    }

                    m_caster->CastSpell(unitTarget, spellId, false);
                    return;
                }*/
                case 24720:                                 // Random Costume
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spellId = 0;

                    switch(urand(0, 6))
                    {
                        case 0:
                            spellId = unitTarget->getGender() == GENDER_MALE ? 24708 : 24709;
                            break;
                        case 1:
                            spellId = unitTarget->getGender() == GENDER_MALE ? 24711 : 24710;
                            break;
                        case 2:
                            spellId = unitTarget->getGender() == GENDER_MALE ? 24712 : 24713;
                            break;
                        case 3:
                            spellId = 24723;
                            break;
                        case 4:
                            spellId = 24732;
                            break;
                        case 5:
                            spellId = unitTarget->getGender() == GENDER_MALE ? 24735 : 24736;
                            break;
                        case 6:
                            spellId = 24740;
                            break;
                    }

                    m_caster->CastSpell(unitTarget, spellId, true);
                    return;
                }
                case 24737:                                 // Ghost Costume
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Ghost Costume (male or female)
                    m_caster->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 24735 : 24736, true);
                    return;
                }
                case 24751:                                 // Trick or Treat
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Tricked or Treated
                    unitTarget->CastSpell(unitTarget, 24755, true);

                    // Treat / Trick
                    unitTarget->CastSpell(unitTarget, roll_chance_i(50) ? 24714 : 24715, true);
                    return;
                }
                case 25140:                                 // Orb teleport spells
                case 25143:
                case 25650:
                case 25652:
                case 29128:
                case 29129:
                case 35376:
                case 35727:
                {
                    if (!unitTarget)
                        return;

                    uint32 spellid;
                    switch(m_spellInfo->Id)
                    {
                        case 25140: spellid =  32568; break;
                        case 25143: spellid =  32572; break;
                        case 25650: spellid =  30140; break;
                        case 25652: spellid =  30141; break;
                        case 29128: spellid =  32571; break;
                        case 29129: spellid =  32569; break;
                        case 35376: spellid =  25649; break;
                        case 35727: spellid =  35730; break;
                        default:
                            return;
                    }

                    unitTarget->CastSpell(unitTarget,spellid,false);
                    return;
                }
                case 26004:                                 // Mistletoe
                {
                    if (!unitTarget)
                        return;

                    unitTarget->HandleEmote(EMOTE_ONESHOT_CHEER);
                    return;
                }
                case 26137:                                 // Rotate Trigger
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, urand(0, 1) ? 26009 : 26136, true);
                    return;
                }
                case 26218:                                 // Mistletoe
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spells[3] = {26206, 26207, 45036};

                    m_caster->CastSpell(unitTarget, spells[urand(0, 2)], true);
                    return;
                }
                case 26393:                                 // Elune's Blessing
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(unitTarget, 26394, true);
                    return;
                }
                case 26275:                                 // PX-238 Winter Wondervolt TRAP
                {
                    uint32 spells[4] = {26272, 26157, 26273, 26274};

                    // check presence
                    for (int j = 0; j < 4; ++j)
                        if(unitTarget->HasAura(spells[j], EFFECT_INDEX_0))
                            return;

                    // cast
                    unitTarget->CastSpell(unitTarget, spells[urand(0,3)], true);
                    return;
                }
                case 26465:                                 // Mercurial Shield - need remove one 26464 Mercurial Shield aura
                    unitTarget->RemoveAuraHolderFromStack(26464);
                    return;
                case 26656:                                 // Summon Black Qiraji Battle Tank
                {
                    if (!unitTarget)
                        return;

                    // Prevent stacking of mounts
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    // Two separate mounts depending on area id (allows use both in and out of specific instance)
                    if (unitTarget->GetAreaId() == 3428)
                        unitTarget->CastSpell(unitTarget, 25863, false);
                    else
                        unitTarget->CastSpell(unitTarget, 26655, false);

                    return;
                }
                case 26678:                                 // Heart Candy
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 items[8] = {21816, 21817, 21818, 21819, 21820, 21821, 21822, 21823};

                    DoCreateItem(effect, items[urand(0,7)]);
                    break;
                }
                case 27687:                                 // Summon Bone Minions
                {
                    if (!unitTarget)
                        return;

                    // Spells 27690, 27691, 27692, 27693 are missing from DBC
                    // So we need to summon creature 16119 manually
                    float x, y, z;
                    float angle = unitTarget->GetOrientation();
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        unitTarget->GetNearPoint(unitTarget, x, y, z, unitTarget->GetObjectBoundingRadius(), 5.0f, angle + i*M_PI_F/2);
                        unitTarget->SummonCreature(16119, x, y, z, angle, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10*MINUTE*IN_MILLISECONDS);
                    }
                    return;
                }
                case 27695:                                 // Summon Bone Mages
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 27696, true);
                    unitTarget->CastSpell(unitTarget, 27697, true);
                    unitTarget->CastSpell(unitTarget, 27698, true);
                    unitTarget->CastSpell(unitTarget, 27699, true);
                    return;
                }
                case 28374:                                 // Decimate (Naxxramas: Gluth)
                case 54426:                                 // Decimate (Naxxramas: Gluth (spells are identical))
                case 71123:                                 // Decimate (ICC: Precious / Stinky)
                {
                    if (!unitTarget)
                        return;

                    float downToHealthPercent = (m_spellInfo->Id != 71123 ? 5 : effect->CalculateSimpleValue()) * 0.01f;

                    int32 damage = unitTarget->GetHealth() - unitTarget->GetMaxHealth() * downToHealthPercent;
                    if (damage > 0)
                        m_caster->CastCustomSpell(unitTarget, 28375, &damage, NULL, NULL, true);
                    return;
                }
                case 28560:                                 // Summon Blizzard
                {
                    if (!unitTarget)
                        return;

                    m_caster->SummonCreature(16474, unitTarget->GetPositionX(), unitTarget->GetPositionY(), unitTarget->GetPositionZ(), 0.0f, TEMPSUMMON_TIMED_DESPAWN, 30000);
                    return;
                }
                case 28859:                                 // Cleansing Flames
                case 29126:                                 // Cleansing Flames (Darnassus)
                case 29135:                                 // Cleansing Flames (Ironforge)
                case 29136:                                 // Cleansing Flames (Orgrimmar)
                case 29137:                                 // Cleansing Flames (Stormwind)
                case 29138:                                 // Cleansing Flames (Thunder Bluff)
                case 29139:                                 // Cleansing Flames (Undercity)
                case 46671:                                 // Cleansing Flames (Exodar)
                case 46672:                                 // Cleansing Flames (Silvermoon)
                {
                    // Cleanse all magic, curse, disease and poison
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasWithDispelType(DISPEL_MAGIC);
                    unitTarget->RemoveAurasWithDispelType(DISPEL_CURSE);
                    unitTarget->RemoveAurasWithDispelType(DISPEL_DISEASE);
                    unitTarget->RemoveAurasWithDispelType(DISPEL_POISON);

                    return;
                }
                case 29395:                                 // Break Kaliri Egg
                {
                    uint32 creature_id = 0;
                    uint32 rand = urand(0, 99);

                    if (rand < 10)
                        creature_id = 17034;
                    else if (rand < 60)
                        creature_id = 17035;
                    else
                        creature_id = 17039;

                    if (WorldObject* pSource = GetAffectiveCasterObject())
                        pSource->SummonCreature(creature_id, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 120*IN_MILLISECONDS);
                    return;
                }
                case 29830:                                 // Mirren's Drinking Hat
                {
                    uint32 item = 0;
                    switch ( urand(1, 6) )
                    {
                        case 1:
                        case 2:
                        case 3:
                            item = 23584; break;            // Loch Modan Lager
                        case 4:
                        case 5:
                            item = 23585; break;            // Stouthammer Lite
                        case 6:
                            item = 23586; break;            // Aerie Peak Pale Ale
                    }

                    if (item)
                        DoCreateItem(effect,item);

                    break;
                }
                case 30541:                                 // Blaze
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 30542, true);
                    break;
                }
                case 30769:                                 // Pick Red Riding Hood
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // cast Little Red Riding Hood
                    m_caster->CastSpell(unitTarget, 30768, true);
                    break;
                }
                case 30835:                                 // Infernal Relay
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 30836, true, NULL, NULL, m_caster->GetObjectGuid());
                    break;
                }
                case 30918:                                 // Improved Sprint
                {
                    if (!unitTarget)
                        return;

                    // Removes snares and roots.
                    unitTarget->RemoveAurasAtMechanicImmunity(IMMUNE_TO_ROOT_AND_SNARE_MASK, 0, true);
                    break;
                }
                case 32301:                                 // Ping Shirrak
                {
                    if (!unitTarget)
                        return;

                    // Cast Focus fire on caster
                    unitTarget->CastSpell(m_caster, 32300, true);
                    return;
                }
                case 35865:                                 // Summon Nether Vapor
                {
                    if (!unitTarget)
                        return;

                    float x, y, z;
                    for (uint8 i = 0; i < 4; ++i)
                    {
                        m_caster->GetNearPoint(m_caster, x, y, z, 0, 5.0f, M_PI_F*.5f*i + M_PI_F*.25f);
                        m_caster->SummonCreature(21002, x, y, z, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 30000);
                    }
                    return;
                }
                case 37431:                                 // Spout
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, urand(0, 1) ? 37429 : 37430, true);
                    return;
                }
                case 37473:                                 // Detect Whispers (related to quest 10607 - Whispers of the Raven God_Whispers of the Raven God)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, damage, true);
                    break;
                }
                case 37834:                                 // Unbansih Azaloth
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER || !unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* pTemp = (Creature*)unitTarget;
                    if (pTemp->AI())
                        pTemp->AI()->AttackStart(m_caster);

                    ((Player*)m_caster)->KilledMonsterCredit(21892);
                    break;
                }
                case 38358:                                 // Tidal Surge
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 38353, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 39681:                                 // Summon Goblin Tonk
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(unitTarget, 39682, true);
                    break;
                }
                case 39684:                                 // Summon Gnomish Tonk
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(unitTarget, 39683, true);
                    break;
                }
                case 39835:                                 // Needle Spine (Warlord Najentus)
                {
                    if (!unitTarget)
                        return;

                    // TODO likely that this spell should have m_caster as Original caster, but conflicts atm with TARGET_ALL_FRIENDLY_UNITS_AROUND_CASTER
                    unitTarget->CastSpell(unitTarget, 39968, true);
                    return;
                }
                case 40904:                                 // Draw Soul
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, 40903, true);
                    break;
                }
                case 41055:                                 // Copy Weapon
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT || !unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (Item* pItem = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                    {
                        ((Creature*)m_caster)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, pItem->GetEntry());

                        // Unclear what this spell should do
                        unitTarget->CastSpell(m_caster, effect->CalculateSimpleValue(), true);
                    }

                    return;
                }
                case 41126:                                 // Flame Crash
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 41131, true);
                    break;
                }
                case 42281:                                 // Sprout (Headless Horsemann spell)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 42285, true);
                    return;
                }
                case 42436:                                 // Drink! Complimentary Brewfest Sampler
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;
                    m_caster->CastSpell(m_caster, 47173, false);
                    return;
                }
                case 42577:                                 // Zap
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->getVictim())
                    {
                        // Cast Zap damage spell
                        unitTarget->CastSpell(unitTarget->getVictim(), 43137, true);

                        // Attack a new target
                        if (unitTarget->GetTypeId() == TYPEID_UNIT)
                        {
                            Creature* cre = (Creature*)unitTarget;
                            if (cre && cre->AI())
                            {
                                if (Unit* target = cre->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                    cre->AI()->AttackStart(target);
                            }
                        }
                    }
                    return;
                }
                case 42924:                                 // Giddyup!
                {
                    if (!m_caster->HasAura(43880) && !m_caster->HasAura(43883))
                        return;

                    // Exhausted Ram
                    if (m_caster->HasAura(43332))
                        return;

                    // Giddyup!
                    if (SpellAuraHolder* holder = m_caster->GetSpellAuraHolder(42924))
                    {
                        int8 spellIndex = -1;
                        uint32 spellIds[4] = { 43310, 42992, 42993, 42994 };
                        switch(holder->GetStackAmount())
                        {
                            case 1: spellIndex = 0; break; // Ram Level - Neutral
                            case 2: spellIndex = 1; break; // Ram - Trot
                            case 6: spellIndex = 2; break; // Ram - Canter
                            case 9: spellIndex = 3; break; // Ram - Gallop
                        }

                        if (spellIndex != -1)
                        {
                            if (spellIndex > 0)
                                m_caster->RemoveAurasDueToSpell(spellIds[spellIndex - 1]);
                            m_caster->CastSpell(m_caster, spellIds[spellIndex], true);
                        }
                    }
                    return;
                }
                case 43365:                                 // The Cleansing: Shrine Cast
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Script Effect Player Cast Mirror Image
                    m_caster->CastSpell(m_caster, 50217, true);
                    return;
                }
                case 43375:                                 // Mixing Vrykul Blood
                case 43972:                                 // Mixing Blood for Quest 11306
                {
                    if (!unitTarget)
                        return;

                    uint32 triggeredSpell[] = {43376, 43378, 43970, 43377};

                    unitTarget->CastSpell(unitTarget, triggeredSpell[urand(0, 3)], true);
                    return;
                }
                case 43876:                                 // Dismount Ram
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(43880);   // Ramstein's Swift Work Ram
                    unitTarget->RemoveAurasDueToSpell(43883);   // Rental Racing Ram
                    return;
                }
                case 44364:                                 // Rock Falcon Primer
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Are there anything special with this, a random chance or condition?
                    // Feeding Rock Falcon
                    unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true, NULL, NULL, unitTarget->GetObjectGuid(), m_spellInfo);
                    return;
                }
                case 44436:                                 // Tricky Treat
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;
                    if (unitTarget->HasAura(42966))         // Upset Tummy
                        return;
                    if (Aura* boost = unitTarget->GetAura(42919, EFFECT_INDEX_0))
                        if (boost->GetStackAmount() == boost->GetSpellProto()->GetStackAmount())
                        {
                            m_caster->CastSpell(unitTarget, 42966,true);
                            return;
                        }
                    m_caster->CastSpell(unitTarget, 42919, true);

                    return;
                }
                case 44455:                                 // Character Script Effect Reverse Cast
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* pTarget = (Creature*)unitTarget;

                    if (const SpellEntry *pSpell = sSpellStore.LookupEntry(effect->CalculateSimpleValue()))
                    {
                        // if we used item at least once...
                        if (pTarget->IsTemporarySummon() && int32(pTarget->GetEntry()) == pSpell->GetEffectMiscValue(SpellEffectIndex(effect->EffectIndex)))
                        {
                            TemporarySummon* pSummon = (TemporarySummon*)pTarget;

                            // can only affect "own" summoned
                            if (pSummon->GetSummonerGuid() == m_caster->GetObjectGuid())
                            {
                                if (pTarget->hasUnitState(UNIT_STAT_ROAMING | UNIT_STAT_ROAMING_MOVE))
                                    pTarget->GetMotionMaster()->MovementExpired();

                                // trigger cast of quest complete script (see code for this spell below)
                                pTarget->CastSpell(pTarget, 44462, true);

                                pTarget->GetMotionMaster()->MovePoint(0, m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ());
                            }

                            return;
                        }

                        // or if it is first time used item, cast summon and despawn the target
                        m_caster->CastSpell(pTarget, pSpell, true);
                        pTarget->ForcedDespawn();

                        // TODO: here we should get pointer to the just summoned and make it move.
                        // without, it will be one extra use of quest item
                    }

                    return;
                }
                case 44462:                                 // Cast Quest Complete on Master
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    Creature* pQuestCow = NULL;

                    float range = 20.0f;

                    // search for a reef cow nearby
                    MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*m_caster, 24797, true, false, range);
                    MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pQuestCow, u_check);

                    Cell::VisitGridObjects(m_caster, searcher, range);

                    // no cows found, so return
                    if (!pQuestCow)
                        return;

                    if (!((Creature*)m_caster)->IsTemporarySummon())
                        return;

                    if (const SpellEntry *pSpell = sSpellStore.LookupEntry(effect->CalculateSimpleValue()))
                    {
                        TemporarySummon* pSummon = (TemporarySummon*)m_caster;

                        // all ok, so make summoner cast the quest complete
                        if (Unit* pSummoner = pSummon->GetSummoner())
                            pSummoner->CastSpell(pSummoner, pSpell, true);
                    }

                    return;
                }
                case 44811:                                 // Spectral Realm
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // If the player can't be teleported, send him a notification
                    if (unitTarget->HasAura(44867))
                    {
                        ((Player*)unitTarget)->GetSession()->SendNotification(LANG_FAIL_ENTER_SPECTRAL_REALM);
                        return;
                    }

                    // Teleport target to the spectral realm, add debuff and force faction
                    //unitTarget->CastSpell(unitTarget, 44852, true);
                    unitTarget->CastSpell(unitTarget, 46019, true);
                    unitTarget->CastSpell(unitTarget, 46021, true);
                    return;
                }
                case 44876:                                 // Force Cast - Portal Effect: Sunwell Isle
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 44870, true);
                    break;
                }
                case 45141:                                 // Burn
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 46394, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 45151:                                 // Burn
                {
                    if (!unitTarget || unitTarget->HasAura(46394))
                        return;

                    // Make the burn effect jump to another friendly target
                    unitTarget->CastSpell(unitTarget, 46394, true);
                    return;
                }
                case 45185:                                 // Stomp
                {
                    if (!unitTarget)
                        return;

                    // Remove the burn effect
                    unitTarget->RemoveAurasDueToSpell(46394);
                    return;
                }
                case 45204:                                 // Clone Me!
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 45206:                                 // Copy Off-hand Weapon
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT || !unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (Item* pItem = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                    {
                        ((Creature*)m_caster)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, pItem->GetEntry());

                        // Unclear what this spell should do
                        unitTarget->CastSpell(m_caster, effect->CalculateSimpleValue(), true);
                    }

                    return;
                }
                case 45625:                                 // Arcane Chains: Character Force Cast
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, 45626, true);
                    break;
                }
                case 45668:                                 // Ultra-Advanced Proto-Typical Shortening Blaster
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (roll_chance_i(25))                  // chance unknown, using 25
                        return;

                    static uint32 const spellPlayer[5] =
                    {
                        45674,                              // Bigger!
                        45675,                              // Shrunk
                        45678,                              // Yellow
                        45682,                              // Ghost
                        45684                               // Polymorph
                    };

                    static uint32 const spellTarget[5] =
                    {
                        45673,                              // Bigger!
                        45672,                              // Shrunk
                        45677,                              // Yellow
                        45681,                              // Ghost
                        45683                               // Polymorph
                    };

                    m_caster->CastSpell(m_caster, spellPlayer[urand(0,4)], true);
                    unitTarget->CastSpell(unitTarget, spellTarget[urand(0,4)], true);

                    return;
                }
                case 45691:                                 // Magnataur On Death 1
                {
                    // assuming caster is creature, if not, then return
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    Player* pPlayer = ((Creature*)m_caster)->GetOriginalLootRecipient();

                    if (!pPlayer)
                        return;

                    if (pPlayer->HasAura(45674) || pPlayer->HasAura(45675) || pPlayer->HasAura(45678) || pPlayer->HasAura(45682) || pPlayer->HasAura(45684))
                        pPlayer->CastSpell(pPlayer, 45686, true);

                    m_caster->CastSpell(m_caster, 45685, true);

                    return;
                }
                case 45713:                                 // Naked Caravan Guard - Master Transform
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    const CreatureInfo* cTemplate = NULL;

                    switch(m_caster->GetEntry())
                    {
                        case 25342: cTemplate = ObjectMgr::GetCreatureTemplate(25340); break;
                        case 25343: cTemplate = ObjectMgr::GetCreatureTemplate(25341); break;
                    }

                    if (!cTemplate)
                        return;

                    uint32 display_id = 0;

                    // Spell is designed to be used in creature addon.
                    // This makes it possible to set proper model before adding to map.
                    // For later, spell is used in gossip (with following despawn,
                    // so addon can reload the default model and data again).

                    // It should be noted that additional spell id's have been seen in relation to this spell, but
                    // those does not exist in client (45701 (regular spell), 45705-45712 (auras), 45459-45460 (auras)).
                    // We can assume 45705-45712 are transform auras, used instead of hard coded models in the below code.

                    // not in map yet OR no npc flags yet (restored after LoadCreatureAddon for respawn cases)
                    if (!m_caster->IsInMap(m_caster) || m_caster->GetUInt32Value(UNIT_NPC_FLAGS) == UNIT_NPC_FLAG_NONE)
                    {
                        display_id = Creature::ChooseDisplayId(cTemplate);
                        ((Creature*)m_caster)->LoadEquipment(((Creature*)m_caster)->GetEquipmentId());
                    }
                    else
                    {
                        m_caster->SetUInt32Value(UNIT_NPC_FLAGS, cTemplate->npcflag);
                        ((Creature*)m_caster)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 0);
                        ((Creature*)m_caster)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);

                        switch(m_caster->GetDisplayId())
                        {
                            case 23246: display_id = 23245; break;
                            case 23247: display_id = 23250; break;
                            case 23248: display_id = 23251; break;
                            case 23249: display_id = 23252; break;
                            case 23124: display_id = 23253; break;
                            case 23125: display_id = 23254; break;
                            case 23126: display_id = 23255; break;
                            case 23127: display_id = 23256; break;
                        }
                    }

                    m_caster->SetDisplayId(display_id);
                    return;
                }
                case 45958:                                 // Signal Alliance
                {
                    // "escort" aura not present, so let nothing happen
                    if (!m_caster->HasAura(effect->CalculateSimpleValue()))
                        return;
                    // "escort" aura is present so break; and let DB table dbscripts_on_spell be used and process further.
                    else
                        break;
                }
                case 46054:                                 // Torch Toss (land)
                {
                    if (!gameObjTarget || gameObjTarget->GetEntry() != 187708)
                        return;

                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)m_caster)->KilledMonsterCredit(30771);
                    break;
                }
                case 46203:                                 // Goblin Weather Machine
                {
                    if (!unitTarget)
                        return;

                    uint32 spellId = 0;
                    switch(rand() % 4)
                    {
                        case 0: spellId = 46740; break;
                        case 1: spellId = 46739; break;
                        case 2: spellId = 46738; break;
                        case 3: spellId = 46736; break;
                    }
                    unitTarget->CastSpell(unitTarget, spellId, true);
                    break;
                }
                case 46289:                                 // Negative Energy
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 46285, true);
                    break;
                }
                case 46642:                                 // 5,000 Gold
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)unitTarget)->ModifyMoney(50000000);
                    break;
                }
                case 47097:                                 // Surge Needle Teleporter
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (unitTarget->GetAreaId() == 4157)
                        unitTarget->CastSpell(unitTarget, 47324, true);
                    else if (unitTarget->GetAreaId() == 4156)
                        unitTarget->CastSpell(unitTarget, 47325, true);

                    break;
                }
                case 47311:                                 // Quest - Jormungar Explosion Spell Spawner
                {
                    // Summons npc's. They are expected to summon GO from 47315
                    // but there is no way to get the summoned, to trigger a spell
                    // cast (workaround can be done with ai script).

                    // Quest - Jormungar Explosion Summon Object
                    for(int i = 0; i < 2; ++i)
                        m_caster->CastSpell(m_caster, 47309, true);

                    for(int i = 0; i < 2; ++i)
                        m_caster->CastSpell(m_caster, 47924, true);

                    for(int i = 0; i < 2; ++i)
                        m_caster->CastSpell(m_caster, 47925, true);

                    return;
                }
                case 47393:                                 // The Focus on the Beach: Quest Completion Script
                {
                    if (!unitTarget)
                        return;

                    // Ley Line Information
                    unitTarget->RemoveAurasDueToSpell(47391);

                    return;
                }
                case 47615:                                 // Atop the Woodlands: Quest Completion Script
                {
                    if (!unitTarget)
                        return;

                    // Ley Line Information
                    unitTarget->RemoveAurasDueToSpell(47473);
                    return;
                }
                case 47638:                                 // The End of the Line: Quest Completion Script
                {
                    if (!unitTarget)
                        return;

                    // Ley Line Information
                    unitTarget->RemoveAurasDueToSpell(47636);
                    return;
                }
                case 47770:                                 // Roll Dice
                case 47776:                                 // Roll 'dem Bones
                {
                    if (unitTarget == NULL || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;
                    
                    Player *player = (Player*)unitTarget;
                    uint32 roll = urand(1, 100);
                    WorldPacket data(MSG_RANDOM_ROLL, 4+4+4+8);
                    data << uint32(1);
                    data << uint32(100);
                    data << uint32(roll);
                    data << player->GetObjectGuid();
                    if(player->GetGroup() != NULL)
                        player->GetGroup()->BroadcastPacket(&data, false);
                    else
                        player->GetSession()->SendPacket(&data);
                    return;
                }
                case 47703:                                 // Unholy Union
                {
                    m_caster->CastSpell(m_caster, 50254, true);
                    return;
                }
                case 47724:                                 // Frost Draw
                {
                    m_caster->CastSpell(m_caster, 50239, true);
                    return;
                }
                case 47958:                                 // Crystal Spikes
                {
                    // Summon Crystal Spike
                    m_caster->CastSpell(m_caster, 47954, true);
                    m_caster->CastSpell(m_caster, 47955, true);
                    m_caster->CastSpell(m_caster, 47956, true);
                    m_caster->CastSpell(m_caster, 47957, true);
                    return;
                }
                case 48590:                                 // Avenging Spirits (summon Avenging Spirit Summoners)
                {
                    if (!unitTarget)
                        return;

                    // Summon 4 spirits summoners
                    unitTarget->CastSpell(unitTarget, 48586, true);
                    unitTarget->CastSpell(unitTarget, 48587, true);
                    unitTarget->CastSpell(unitTarget, 48588, true);
                    unitTarget->CastSpell(unitTarget, 48589, true);
                    return;
                }
                case 48603:                                 // High Executor's Branding Iron
                    // Torture the Torturer: High Executor's Branding Iron Impact
                    unitTarget->CastSpell(unitTarget, 48614, true);
                    return;
                case 48724:                                 // The Denouncement: Commander Jordan On Death
                case 48726:                                 // The Denouncement: Lead Cannoneer Zierhut On Death
                case 48728:                                 // The Denouncement: Blacksmith Goodman On Death
                case 48730:                                 // The Denouncement: Stable Master Mercer On Death
                {
                    // Compelled
                    if (!unitTarget || !m_caster->HasAura(48714))
                        return;

                    unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 48679:                                 // Banshee's Magic Mirror
                {
                    if (!unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    unitTarget->CastSpell(m_caster, 48648, true);
                    return;
                }
                // Gender spells
                case 48762:                                 // A Fall from Grace: Scarlet Raven Priest Image - Master
                case 45759:                                 // Warsong Orc Disguise
                case 69672:                                 // Sunreaver Disguise
                case 69673:                                 // Silver Covenant Disguise
                {
                    if (!unitTarget)
                        return;

                    uint8 gender = unitTarget->getGender();
                    uint32 spellId;
                    switch (m_spellInfo->Id)
                    {
                        case 48762: spellId = (gender == GENDER_MALE ? 48763 : 48761); break;
                        case 45759: spellId = (gender == GENDER_MALE ? 45760 : 45762); break;
                        case 69672: spellId = (gender == GENDER_MALE ? 70974 : 70973); break;
                        case 69673: spellId = (gender == GENDER_MALE ? 70972 : 70971); break;
                        default: return;
                    }
                    unitTarget->CastSpell(unitTarget, spellId, true);
                    return;
                }
                case 48810:                                 // Death's Door
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Spell effect order will summon creature first and then apply invisibility to caster.
                    // This result in that summoner/summoned can not see each other and that is not expected.
                    // Aura from 48814 can be used as a hack from creature_addon, but we can not get the
                    // summoned to cast this from this spell effect since we have no way to get pointer to creature.
                    // Most proper would be to summon to same visibility mask as summoner, and not use spell at all.

                    // Binding Life
                    m_caster->CastSpell(m_caster, 48809, true);

                    // After (after: meaning creature does not have auras at creation)
                    // creature is summoned and visible for player in map, it is expected to
                    // gain two auras. First from 29266(aura slot0) and then from 48808(aura slot1).
                    // We have no pointer to summoned, so only 48808 is possible from this spell effect.

                    // Binding Death
                    m_caster->CastSpell(m_caster, 48808, true);
                    return;
                }
                case 48811:                                 // Despawn Forgotten Soul
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (!((Creature*)unitTarget)->IsTemporarySummon())
                        return;

                    TemporarySummon* pSummon = (TemporarySummon*)unitTarget;

                    Unit::AuraList const& images = unitTarget->GetAurasByType(SPELL_AURA_MIRROR_IMAGE);

                    if (images.empty())
                        return;

                    Unit* pCaster = images.front()->GetCaster();
                    Unit* pSummoner = unitTarget->GetMap()->GetUnit(pSummon->GetSummonerGuid());

                    if (pSummoner && pSummoner == pCaster)
                        pSummon->UnSummon();

                    return;
                }
                case 48917:                                 // Who Are They: Cast from Questgiver
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Male Shadowy Disguise / Female Shadowy Disguise
                    unitTarget->CastSpell(unitTarget, unitTarget->getGender() == GENDER_MALE ? 38080 : 38081, true);
                    // Shadowy Disguise
                    unitTarget->CastSpell(unitTarget, 32756, true);
                    return;
                }
                case 49380:                                 // Consume
                case 59803:                                 // Consume (heroic)
                {
                    if (!unitTarget)
                        return;

                    // Each target hit buffs the caster
                    unitTarget->CastSpell(m_caster, m_spellInfo->Id == 49380 ? 49381 : 59805, true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 49405:                                 // Invader Taunt Trigger
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 49899:                                 // Activate Robotic Arms
                {
                    if (!unitTarget)
                        return;

                    unitTarget->HandleEmote(EMOTE_ONESHOT_CUSTOMSPELL05);
                    return;
                }
                case 50217:                                 // The Cleansing: Script Effect Player Cast Mirror Image
                {
                    // Summon Your Inner Turmoil
                    m_caster->CastSpell(m_caster, 50167, true);

                    // Spell 50218 has TARGET_SCRIPT, but other wild summons near may exist, and then target can become wrong
                    // Only way to make this safe is to get the actual summoned by m_caster

                    // Your Inner Turmoil's Mirror Image Aura
                    m_caster->CastSpell(m_caster, 50218, true);

                    return;
                }
                case 50218:                                 // The Cleansing: Your Inner Turmoil's Mirror Image Aura
                {
                    if (!m_originalCaster || m_originalCaster->GetTypeId() != TYPEID_PLAYER || !unitTarget)
                        return;

                    // determine if and what weapons can be copied
                    switch(effect->EffectIndex)
                    {
                        case EFFECT_INDEX_1:
                            if (((Player*)m_originalCaster)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                                unitTarget->CastSpell(m_originalCaster, effect->CalculateSimpleValue(), true);

                            return;
                        case EFFECT_INDEX_2:
                            if (((Player*)m_originalCaster)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
                                unitTarget->CastSpell(m_originalCaster, effect->CalculateSimpleValue(), true);

                            return;
                        default:
                            return;
                    }
                    return;
                }
                case 50238:                                 // The Cleansing: Your Inner Turmoil's On Death Cast on Master
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (((Creature*)m_caster)->IsTemporarySummon())
                    {
                        TemporarySummon* pSummon = (TemporarySummon*)m_caster;

                        if (pSummon->GetSummonerGuid().IsPlayer())
                        {
                            if (Player* pSummoner = sObjectMgr.GetPlayer(pSummon->GetSummonerGuid()))
                                pSummoner->CastSpell(pSummoner, effect->CalculateSimpleValue(), true);
                        }
                    }

                    return;
                }
                case 50252:                                 // Blood Draw for Quest In Service of Blood
                {
                    m_caster->CastSpell(m_caster, 50250, true);
                    return;
                }
                case 50255:                                 // Poisoned Spear
                case 59331:                                 // Poisoned Spear (heroic)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true, NULL, NULL, m_originalCasterGUID);
                    return;
                }
                case 50439:                                 // Script Cast Summon Image of Drakuru 05
                {
                    // TODO: check if summon already exist, if it does in this instance, return.

                    // Summon Drakuru
                    m_caster->CastSpell(m_caster, 50446, true);
                    return;
                }
                case 50499: // Listening to Music (Parent)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;
                    m_caster->CastSpell(m_caster, 50493, true);
                    return;
                }
                case 50725:                                 // Vigilance - remove cooldown on Taunt
                {
                    Unit* caster = GetAffectiveCaster();
                    if (!caster || !caster->IsInWorld() || caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)caster)->RemoveSpellCategoryCooldown(82, true);
                    return;
                }
                case 50742:                                 // Ooze Combine
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    m_caster->CastSpell(unitTarget, 50747, true);
                    ((Creature*)m_caster)->ForcedDespawn();
                    return;
                }
                case 50810:                                 // Shatter
                case 61546:                                 // Shatter (h)
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (!unitTarget->HasAura(50812))
                        return;

                    unitTarget->RemoveAurasDueToSpell(50812);
                    unitTarget->CastSpell(unitTarget, m_spellInfo->Id == 50810 ? 50811 : 61547 , true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 50894:                                 // Zul'Drak Rat
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    if (SpellAuraHolder* pHolder = unitTarget->GetSpellAuraHolder(m_spellInfo->Id))
                    {
                        if (pHolder->GetStackAmount() + 1 >= m_spellInfo->GetStackAmount())
                        {
                            // Gluttonous Lurkers: Summon Gorged Lurking Basilisk
                            unitTarget->CastSpell(m_caster, 50928, true);
                            ((Creature*)unitTarget)->ForcedDespawn(1);
                        }
                    }

                    return;
                }
                case 51519:                                 // Death Knight Initiate Visual
                {
                    if (!unitTarget)
                        return;

                    uint32 spellId = 0;

                    bool isMale = unitTarget->getGender() == GENDER_MALE;
                    switch (unitTarget->getRace())
                    {
                        case RACE_HUMAN:    spellId = isMale ? 51520 : 51534; break;
                        case RACE_DWARF:    spellId = isMale ? 51538 : 51537; break;
                        case RACE_NIGHTELF: spellId = isMale ? 51535 : 51536; break;
                        case RACE_GNOME:    spellId = isMale ? 51539 : 51540; break;
                        case RACE_DRAENEI:  spellId = isMale ? 51541 : 51542; break;
                        case RACE_ORC:      spellId = isMale ? 51543 : 51544; break;
                        case RACE_UNDEAD:   spellId = isMale ? 51549 : 51550; break;
                        case RACE_TAUREN:   spellId = isMale ? 51547 : 51548; break;
                        case RACE_TROLL:    spellId = isMale ? 51546 : 51545; break;
                        case RACE_BLOODELF: spellId = isMale ? 51551 : 51552; break;
                        default:
                            return;
                    }

                    unitTarget->CastSpell(unitTarget, spellId, true);

                    Creature* npc = (Creature*)unitTarget;
                    npc->LoadEquipment(npc->GetEquipmentId());
                    return;
                }
                case 51770:                                 // Emblazon Runeblade
                {
                    Unit* caster = GetAffectiveCaster();
                    if (!caster)
                        return;

                    caster->CastSpell(caster, damage, false);
                    break;
                }
                case 51854:                                 // Deathbolt from Thalgran Blightbringer
                {
                    // Retribution by Sevenfold Retribution
                    if (!m_caster || !unitTarget)
                        return;

                    // reflected by Freya's Ward
                    if (unitTarget->HasAura(51845))
                        unitTarget->CastSpell(m_caster, 51856, true);
                    else
                        m_caster->CastSpell(unitTarget, 51855, true);
                    break;
                }
                case 51864:                                 // Player Summon Nass
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Summon Nass
                    if (const SpellEntry* pSpell = sSpellStore.LookupEntry(51865))
                    {
                        // Only if he is not already there
                        if (!m_caster->FindGuardianWithEntry(pSpell->GetEffectMiscValue(EFFECT_INDEX_0)))
                        {
                            m_caster->CastSpell(m_caster, pSpell, true);

                            if (Pet* pPet = m_caster->FindGuardianWithEntry(pSpell->GetEffectMiscValue(EFFECT_INDEX_0)))
                            {
                                // Nass Periodic Say aura
                                pPet->CastSpell(pPet, 51868, true);
                            }
                        }
                    }
                    return;
                }
                case 51889:                                 // Quest Accept Summon Nass
                {
                    // This is clearly for quest accept, is spell 51864 then for gossip and does pretty much the same thing?
                    // Just "jumping" to what may be the "gossip-spell" for now, doing the same thing
                    m_caster->CastSpell(m_caster, 51864, true);
                    return;
                }
                case 51904:                                 // Summon Ghouls Of Scarlet Crusade
                {
                    if(!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 54522, true);
                    break;
                }
                case 51910:                                 // Kickin' Nass: Quest Completion
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (const SpellEntry* pSpell = sSpellStore.LookupEntry(51865))
                    {
                        // Is this all to be done at completion?
                        if (Pet* pPet = m_caster->FindGuardianWithEntry(pSpell->GetEffectMiscValue(EFFECT_INDEX_0)))
                            pPet->Unsummon(PET_SAVE_AS_DELETED, m_caster);
                    }
                    return;
                }
                case 52124:                                 // Sky Darkener Assault
                {
                    if (unitTarget && unitTarget != m_caster)
                        m_caster->CastSpell(unitTarget, 52125, false);
                    break;
                }
                case 52173:                                 // Coyote Spirit Despawn
                case 60243:                                 // Blood Parrot Despawn
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->GetTypeId() == TYPEID_UNIT && ((Creature*)unitTarget)->IsTemporarySummon())
                        ((TemporarySummon*)unitTarget)->UnSummon();
                    return;
                }
                case 52357:                                 // Into the realm of shadows
                {
                    if (!unitTarget)
                        return;
                    unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    break;
                }
                case 52459:                                 // End of Round
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Player* plr = (Player*)unitTarget;
                    plr->SetHealth(plr->GetMaxHealth());
                    plr->SetPower(POWER_MANA, plr->GetMaxPower(POWER_MANA));
                    plr->CombatStopWithPets(true);
                    return;
                }
                case 52479:                                 // The Gift That Keeps On Giving
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(m_caster, roll_chance_i(75) ? 52505 : effect->CalculateSimpleValue(), true);
                    ((Creature*)unitTarget)->ForcedDespawn();
                    break;
                }
                case 52694:                                 // Recall Eye of Acherus
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;
                    m_caster->RemoveAurasDueToSpell(530);
                    return;
                }
                case 52751:                                 // Death Gate
                {
                    if (!unitTarget || unitTarget->getClass() != CLASS_DEATH_KNIGHT)
                        return;

                    // triggered spell is stored in m_spellInfo->EffectBasePoints[0]
                    unitTarget->CastSpell(unitTarget, damage, false);
                    break;
                }
                case 52941:                                 // Song of Cleansing
                {
                    uint32 spellId = 0;

                    switch(m_caster->GetAreaId())
                    {
                        case 4385: spellId = 52954; break;  // Bittertide Lake
                        case 4290: spellId = 52958; break;  // River's Heart
                        case 4388: spellId = 52959; break;  // Wintergrasp River
                    }

                    if (spellId)
                        m_caster->CastSpell(m_caster, spellId, true);

                    break;
                }
                case 53110:                                 // Devour Humanoid
                {
                    unitTarget->CastSpell(m_caster, effect->CalculateSimpleValue(), true, NULL, NULL, m_caster->GetObjectGuid());
                    return;
                }
                case 53242:                                 // Clear Gift of Tharonja
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(52509);
                    return;
                }
                case 54182:                                 // An End to the Suffering: Quest Completion Script
                {
                    if (!unitTarget)
                        return;

                    // Remove aura (Mojo of Rhunok) given at quest accept / gossip
                    unitTarget->RemoveAurasDueToSpell(51967);
                    return;
                }
                case 54248:                                 // Drakuru Overlord Death
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 52578, true); // Meat
                    unitTarget->CastSpell(unitTarget, 52580, true); // Bones
                    unitTarget->CastSpell(unitTarget, 52575, true); // Bones II
                    unitTarget->CastSpell(unitTarget, 52578, true); // Meat
                    unitTarget->CastSpell(unitTarget, 52580, true); // Bones
                    unitTarget->CastSpell(unitTarget, 52575, true); // Bones II
                    unitTarget->CastSpell(unitTarget, 54250, true); // Skull Missile
                    return;
                }
                case 54269:                                 // Drakkari Colossus, Elemental Despawn
                {
                    if (!unitTarget || m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    ((Creature*)m_caster)->ForcedDespawn(3000);
                    m_caster->CastSpell(unitTarget, 54878, true); // Set Scale 0.1 And Stun
                    return;
                }
                case 54581:                                 // Mammoth Explosion Spell Spawner
                {
                    if (m_caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    // Summons misc npc's. They are expected to summon GO from 54625
                    // but there is no way to get the summoned, to trigger a spell
                    // cast (workaround can be done with ai script).

                    // Quest - Mammoth Explosion Summon Object
                    for(int i = 0; i < 2; ++i)
                        m_caster->CastSpell(m_caster, 54623, true);

                    for(int i = 0; i < 2; ++i)
                        m_caster->CastSpell(m_caster, 54627, true);

                    for(int i = 0; i < 2; ++i)
                        m_caster->CastSpell(m_caster, 54628, true);

                    // Summon Main Mammoth Meat
                    m_caster->CastSpell(m_caster, 57444, true);
                    return;
                }
                case 54436:                                 // Demonic Empowerment (succubus Vanish effect)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_STALKED);
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOD_STUN);
                    return;
                }
                // Ranger: Stoneclaw Totem & Glyph - patch by laise: Stoneclaw Totem absorb
                case 55328:
                {
                    if(!unitTarget)
                        return;

                    for(int slot = 0;  slot < MAX_TOTEM_SLOT; ++slot)
                    {
                        if (Totem* totem = unitTarget->GetTotem(TotemSlot(slot)))
                            totem->CastCustomSpell(totem, 55277, &damage, NULL, NULL, true);
                    }
                    
                    // Glyph of Stoneclaw Totem
                    if (Aura *aur = unitTarget->GetAura(63298, EFFECT_INDEX_0))
                    {
                        int32 totalAbsorb = aur->GetModifier()->m_amount * damage;
                        if (totalAbsorb)
                            unitTarget->CastCustomSpell(unitTarget, 55277, &totalAbsorb, NULL, NULL, true);
                    }
                    return;
                }
                case 55299:                                 // Galdarah Transform to Troll!
                {
                    m_caster->RemoveAurasDueToSpell(55297);
                    return;
                }
                case 55693:                                 // Remove Collapsing Cave Aura
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(effect->CalculateSimpleValue());
                    break;
                }
                case 56659:                                 // Build Demolisher (Force)
                case 56662:                                 // Build Siege Vehicle (Force)
                case 56664:                                 // Build Catapult (Force)
                case 61409:                                 // Build Siege Vehicle (Force)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), false);
                    return;
                }
                case 57337:                                 // Great Feast
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 58067, true);
                    break;
                }
                case 57397:                                 // Fish Feast
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 45548, true);
                    unitTarget->CastSpell(unitTarget, 57073, true);
                    unitTarget->CastSpell(unitTarget, 57398, true);
                    break;
                }
                case 58466:                                 // Gigantic Feast
                case 58475:                                 // Small Feast
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 57085, true);
                    unitTarget->CastSpell(unitTarget, m_spellInfo->Id == 58475 ? 58477 : 58467, true);
                    break;
                }
                case 58418:                                 // Portal to Orgrimmar
                case 58420:                                 // Portal to Stormwind
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER || effect->EffectIndex != EFFECT_INDEX_0)
                        return;

                    uint32 spellID = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0);
                    uint32 questID = m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1);

                    if (((Player*)unitTarget)->GetQuestStatus(questID) == QUEST_STATUS_COMPLETE && !((Player*)unitTarget)->GetQuestRewardStatus (questID))
                        unitTarget->CastSpell(unitTarget, spellID, true);

                    return;
                }
                case 58622:                                 // Teleport to Lake Wintergrasp
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    BattleFieldWG* opvp = (BattleFieldWG*)sOutdoorPvPMgr.GetScript(ZONE_ID_WINTERGRASP);
                    if (!opvp)
                        return;

                    TeamIndex teamIdx = GetTeamIndex(((Player*)unitTarget)->GetTeam());
                    if (opvp->GetState() == BF_STATE_IN_PROGRESS || opvp->GetTimer() < opvp->GetStartInviteDelay())
                        unitTarget->CastSpell(unitTarget, teamIdx == opvp->GetDefender() ? 60035 : 59096, true);
                    else if (opvp->GetState() == BF_STATE_COOLDOWN)
                        if (opvp->GetDefender() == teamIdx)
                            unitTarget->CastSpell(unitTarget, 60035, true);
                        else
                            unitTarget->CastSpell(unitTarget, teamIdx == TEAM_INDEX_ALLIANCE ? 58633 : 58632, true);

                    return;
                }
                case 58916:                                 // Gift of the Lich King
                {
                    if (!unitTarget || unitTarget->isAlive())
                        return;

                    m_caster->CastSpell(unitTarget, 58915, true);
                    if (unitTarget->GetTypeId() == TYPEID_UNIT)
                        ((Creature*)unitTarget)->RemoveCorpse();

                    if (Unit* master = m_caster->GetCharmerOrOwner())
                        master->CastSpell(master, 58987, true);
                    return;
                }
                case 58917:                                 // Consume minions
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT)
                        return;

                    m_caster->CastSpell(unitTarget, 58919, true);
                    return;
                }
                case 58941:                                 // Rock Shards (Vault of Archavon, Archavon)
                {
                    if (Unit* pTarget = m_caster->GetMap()->GetUnit(m_caster->GetChannelObjectGuid()))
                    {
                        for (uint8 i = 0; i < 3; ++i)   // Trigger three spikes from each hand
                        {
                            m_caster->CastSpell(pTarget, 58689, true);
                            m_caster->CastSpell(pTarget, 58692, true);
                        }
                    }
                    return;
                }
                case 58983:                                  // Big Blizzard Bear
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Prevent stacking of mounts
                    unitTarget->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);

                    if (uint16 skillval = ((Player*)unitTarget)->GetSkillValue(SKILL_RIDING))
                    {
                        switch (skillval)
                        {
                            case 75: 
                                unitTarget->CastSpell(unitTarget, 58997, true); 
                                break;
                            case 150: 
                            case 225:
                            case 300:
                                unitTarget->CastSpell(unitTarget, 58999, true); 
                                break;
                        }
                    }
                    return;
                }
                case 59317:                                 // Teleporting
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // return from top
                    if (((Player*)unitTarget)->GetAreaId() == 4637)
                        unitTarget->CastSpell(unitTarget, 59316, true);
                    // teleport atop
                    else
                        unitTarget->CastSpell(unitTarget, 59314, true);

                    return;
                }
                case 59789:                                 // Oracle Ablutions
                {
                    if (!unitTarget)
                        return;

                    switch (unitTarget->getPowerType())
                    {
                        case POWER_RUNIC_POWER:
                        {
                            unitTarget->CastSpell(unitTarget, 59812, true);
                            break;
                        }
                        case POWER_MANA:
                        {
                            int32 manapool = unitTarget->GetMaxPower(POWER_MANA) * 0.05;
                            unitTarget->CastCustomSpell(unitTarget, 59813, &manapool, NULL, NULL, true);
                            break;
                        }
                        case POWER_RAGE:
                        {
                            unitTarget->CastSpell(unitTarget, 59814, true);
                            break;
                        }
                        case POWER_ENERGY:
                        {
                            unitTarget->CastSpell(unitTarget, 59815, true);
                            break;
                        }
                        // These are not restored
                        case POWER_FOCUS:
                        case POWER_RUNE:
                        case POWER_HEALTH:
                            break;
                    }
                    return;
                }
                case 60123: // Lightwell
                {
                   if (m_caster->GetTypeId() != TYPEID_UNIT)
                       return;

                    uint32 spellID;
                    uint32 entry  = m_caster->GetEntry();

                    switch(entry)
                    {
                        case 31897: spellID = 7001; break;   // Lightwell Renew Rank 1
                        case 31896: spellID = 27873; break;  // Lightwell Renew Rank 2
                        case 31895: spellID = 27874; break;  // Lightwell Renew Rank 3
                        case 31894: spellID = 28276; break;  // Lightwell Renew Rank 4
                        case 31893: spellID = 48084; break;  // Lightwell Renew Rank 5
                        case 31883: spellID = 48085; break;  // Lightwell Renew Rank 6
                        default:
                            sLog.outError("Unknown Lightwell spell caster %u", m_caster->GetEntry());
                            return;
                    }
                    Aura* chargesaura = m_caster->GetAura(59907, EFFECT_INDEX_0);
                    if (chargesaura && chargesaura->GetHolder() && chargesaura->GetHolder()->GetAuraCharges() >= 1)
                    {
                        chargesaura->GetHolder()->SetAuraCharges(chargesaura->GetHolder()->GetAuraCharges() - 1);
                        m_caster->CastSpell(unitTarget, spellID, false, NULL, NULL);
                    }
                    else
                        ((TemporarySummon*)m_caster)->UnSummon();

                    return;
                }
                case 60603:                                 // Eject Passenger 1
                {
                    if (!unitTarget)
                        return;

                    VehicleKit* vehicle = unitTarget->GetVehicleKit();
                    if (!vehicle)
                        return;

                    Unit* passenger = vehicle->GetPassenger(int8(damage));
                    if (!passenger)
                        return;

                    passenger->ExitVehicle();
                    return;
                }
                                                            // random spell learn instead placeholder
                case 60893:                                 // Northrend Alchemy Research
                case 61177:                                 // Northrend Inscription Research
                case 61288:                                 // Minor Inscription Research
                case 61756:                                 // Northrend Inscription Research (FAST QA VERSION)
                case 64323:                                 // Book of Glyph Mastery
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // learn random explicit discovery recipe (if any)
                    if (uint32 discoveredSpell = GetExplicitDiscoverySpell(m_spellInfo->Id, (Player*)m_caster))
                    {
                        ((Player*)m_caster)->learnSpell(discoveredSpell, false);
                        ((Player*)m_caster)->UpdateCraftSkill(m_spellInfo->Id);
                    }

                    return;
                }
                case 61263:                                 // Intravenous Healing Potion
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 61828, true);
                    break;
                }
                case 61551:                                 // Toy Train Set
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 soundId = 0;
                    uint8 gender = unitTarget->getGender();
                    switch (unitTarget->getRace())
                    {
                        case RACE_BLOODELF: soundId = gender == GENDER_MALE ? 9672 : 9644; break;
                        case RACE_DRAENEI: soundId = gender == GENDER_MALE ? 9722 : 9697; break;
                        case RACE_DWARF: soundId = gender == GENDER_MALE ? 7636 : 7637; break;
                        case RACE_GNOME: soundId = gender == GENDER_MALE ? 7641 : 7640; break;
                        case RACE_HUMAN: soundId = gender == GENDER_MALE ? 7634 : 7635; break;
                        case RACE_NIGHTELF: soundId = gender == GENDER_MALE ? 7643 : 7642; break;
                        case RACE_ORC: soundId = gender == GENDER_MALE ? 7638 : 7639; break;
                        case RACE_TAUREN: soundId = gender == GENDER_MALE ? 7646 : 7647; break;
                        case RACE_TROLL: soundId = gender == GENDER_MALE ? 7648 : 7649; break;
                        case RACE_UNDEAD: soundId = gender == GENDER_MALE ? 7644 : 7645; break;
                        default:
                            return;
                    }

                    unitTarget->HandleEmote(EMOTE_ONESHOT_TRAIN);
                    unitTarget->PlayDistanceSound(soundId);

                    return;
                }
                case 62003:                                 // Algalon - Black Hole Spawn
                {
                    if (!unitTarget)
                        return;

                    // Apply aura which causes black hole phase/1 sec to hostile targets
                    unitTarget->CastSpell(m_caster, 62185, true);
                    return;
                }
                case 62168:                                 // Algalon - Black Hole Damage
                {
                    if (!unitTarget)
                        return;
                    unitTarget->CastSpell(unitTarget, 62169, true);
                    return;
                }
                case 62217:                                 // Unstable Energy (Ulduar: Freya's elder)
                {
                    uint32 spellId = effect->CalculateSimpleValue();
                    if (unitTarget && unitTarget->HasAura(spellId))
                        unitTarget->RemoveAurasDueToSpell(spellId);
                    return;
                }
                case 62262:                                 // Brightleaf Flux (Ulduar: Freya's elder)
                {
                    uint32 buffId = roll_chance_i(50) ? 62251 : 62252;

                    m_caster->CastSpell(m_caster, buffId, true);
                    if (buffId == 62252)
                    {
                        if (SpellAuraHolder* holder = m_caster->GetSpellAuraHolder(62239))
                            if (holder->ModStackAmount(-1))
                                m_caster->RemoveSpellAuraHolder(holder);
                    }
                    else
                        m_caster->CastSpell(m_caster, 62239, true);
                    return;
                }
                case 62428:                                 // Load into Catapult
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    m_caster->CastSpell(m_caster, 62340, true);
                    return;
                }
                case 62536:                                 // Frog Kiss (quest Blade fit for a champion)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(62581); // remove Warts!
                    if (!unitTarget->HasAura(62574)) // if not protected by potion cast Warts!
                        m_caster->CastSpell(unitTarget, 62581, true);
                                                            
                    unitTarget->RemoveAurasDueToSpell(62574); // remove protective aura

                    m_caster->GetMotionMaster()->MoveFollow(unitTarget, PET_FOLLOW_DIST, unitTarget->GetAngle(m_caster));
                    break;
                }
                case 62524:                                 // Attuned to Nature 2 Dose Reduction
                case 62525:                                 // Attuned to Nature 10 Dose Reduction
                case 62521:                                 // Attuned to Nature 25 Dose Reduction
                {
                    if (!unitTarget)
                        return;

                    uint32 numStacks = 0;

                    switch(m_spellInfo->Id)
                    {
                        case 62524: numStacks = 2;  break;
                        case 62525: numStacks = 10; break;
                        case 62521: numStacks = 25; break;
                    };

                    uint32 spellId = effect->CalculateSimpleValue();
                    unitTarget->RemoveAuraHolderFromStack(spellId, numStacks);
                    return;
                }
                case 62678:                                 // Summon Allies of Nature
                {
                    const uint32 randSpells[] =
                    {
                        62685,  // Summon Wave - 1 Mob
                        62686,  // Summon Wave - 3 Mob
                        62688,  // Summon Wave - 10 Mob
                    };

                    m_caster->CastSpell(m_caster, randSpells[urand(0, countof(randSpells)-1)], true);
                    return;
                }
                case 62688:                                 // Summon Wave - 10 Mob
                {
                    uint32 spellId = effect->CalculateSimpleValue();

                    for (uint32 i = 0; i < 10; ++i)
                        m_caster->CastSpell(m_caster, spellId, true);

                    return;
                }
                case 62707:                                 // Grab (Ulduar: Ignis)
                case 63535:                                 // Grab heroic
                {
                    if (!unitTarget || !m_caster)
                        return;

                    unitTarget->CastSpell(m_caster, 62708, true); // Control Vehicle aura
                    m_caster->CastSpell(unitTarget, (m_spellInfo->Id == 62707) ? 62717 : 63477, true); // DoT/Immunity
                    break;
                }
                case 62991:                                 // Bonked!
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(62991, m_caster->GetObjectGuid()))
                    {
                        if (holder->GetStackAmount() == m_spellInfo->GetStackAmount() - 1)
                            unitTarget->CastSpell(unitTarget, 62994, true);
                    }
                    return;
                }
                case 63027:                                 // Proximity Mines for Mimiron Encounter
                {
                    if (!unitTarget)
                        return;

                    for(uint8 i = 0; i < urand(8, 10); ++i)
                    {
                        unitTarget->CastSpell(unitTarget, 65347, true);
                    }
                    break;
                }
                case 63122:                                 // Clear Insane (Ulduar - Yogg Saron)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(63050);
                    unitTarget->RemoveAurasDueToSpell(63120);
                    return;
                }
                case 63795:                                 // Psychosis normal (Ulduar - Yogg Saron)
                case 63803:                                 // Brain Link
                case 63830:                                 // Malady of the Mind
                case 63881:
                case 64059:                                 // Induce Madness
                case 64164:                                 // Lunatic Gaze spell from Yogg Saron
                case 64168:                                 // Lunatic Gaze spell from Laughing Skull
                case 65301:                                 // Psychosis heroic (Ulduar - Yogg Saron)                              
                {
                    if (!unitTarget)
                        return;

                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(63050))
                    {
                        int32 stacks = 0;
                        switch (m_spellInfo->Id)
                        {
                            case 63795: stacks = -9;
                                break;                      // Psychosis; remove!?, more script Effect basepoints 63988
                            case 65301: stacks = -12;
                                break;                      // Psychosis; remove!?, more script Effect basepoints 63988
                            case 64164: stacks = -4;
                                break;                      // Lunatic Gaze
                            case 64168:
                            case 63803: stacks = -2;
                                break;                      // Brain Link
                            case 63830:                     // Induce Madness; 65201 (Crush) as basepoints !?
                            case 63881: stacks = -3;
                                break;
                            case 64059:                     // remove!?, more script Effect basepoints 63988
                            {
                                if (unitTarget->GetPositionZ() > 245.0f)
                                    return;
                                stacks = -100; break;
                            }
                        }
                        int32 stackAmount = holder->GetStackAmount() + stacks;

                        if (stackAmount > 100)
                            stackAmount = 100;

                        else if (stackAmount <=0)           // Last aura from stack removed
                        {
                            stackAmount = 0;
                        }
                        holder->SetStackAmount(stackAmount);
                    }
                    return;
                }
                case 63845:                                 // Create lance
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 spellid;

                    if (((Player*)unitTarget)->GetTeam() == HORDE)
                       spellid = 63919;
                    else
                       spellid = 63914;

                    unitTarget->CastSpell(unitTarget, spellid, true);
                    return;
                }
                case 64104:                                 // Quest Credit - Trigger - Dummy - 01
                case 64107:                                 // Quest Credit - Trigger - Dummy - 02
                {
                    if (!unitTarget)
                        return;

                    if (Unit* charmer = unitTarget->GetCharmer())
                        charmer->CastSpell(charmer, damage, true);
                    return;
                }
                case 64122:
                case 65108:                                 // Algalon - Collapsing start explosion to summon black hole
                {
                    if (!unitTarget)
                        return;

                    // Cast Black hole spawn
                    m_caster->CastSpell(m_caster, 62189, true);
                    return;
                }
                case 64123:                                 // Lunge (Ulduar - Yogg Saron)
                {
                    if (!unitTarget)
                        return;

                    uint32 spellid = 0;
                    unitTarget->GetMap()->IsRegularDifficulty() ? spellid = 64125 : spellid = 64126;
                    unitTarget->CastSpell(unitTarget, spellid, true);
                    break;
                }
                case 64142:                                 // Upper Deck - Create Foam Sword
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    uint32 itemId[5] = {45179, 45178, 45176, 45061, 45177};

                    for (uint8 i = 0; i < 5; ++i)
                        if (((Player*)unitTarget)->HasItemCount(itemId[i], 1, true))
                            return;

                    unitTarget->CastSpell(unitTarget, 63380, true); // Create Foam Sword
                    DoCreateItem(effect, itemId[urand(0,4)]);
                    return;
                }
                case 64456:                                 // Feral Essence Application Removal
                {
                    if (!unitTarget)
                        return;

                    uint32 spellId = effect->CalculateSimpleValue();
                    unitTarget->RemoveAuraHolderFromStack(spellId);
                    return;
                }
                case 64466:                                 // Empowering Shadows (Ulduar - Yogg Saron)
                {
                    if (!unitTarget)
                        return;

                    // effect back to caster (Immortal Guardian)
                    unitTarget->CastSpell(m_caster, 64467, true);
                    break;
                }
                case 64467:                                 // Empowering Shadows (Ulduar - Yogg Saron)
                {
                    if (!unitTarget)
                        return;

                    uint32 spellid = 0;
                    unitTarget->GetMap()->IsRegularDifficulty() ? spellid = 64468 : spellid = 64469;
                    unitTarget->CastSpell(unitTarget, spellid, true);
                    break;
                }
                case 65044:                                 // Flames Ulduar
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->HasAura(62297))
                        unitTarget->RemoveAurasDueToSpell(62297);   // Remove Hodir's Fury
                    break;
                }
                case 65238:                                 // Shattered Illusion (Ulduar - Yogg Saron)
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveAurasDueToSpell(effect->EffectBasePoints);
                    return;
                }
                case 65576:                                 // Pelted!
                {
                    if (!unitTarget)
                        return;

                    if (unitTarget->GetTypeId() == TYPEID_PLAYER)
                        m_caster->CastSpell(m_caster, 65591, true);
                    return;
                }
                case 65917:                                 // Magic Rooster
                {
                    if(!m_caster || !m_caster->IsInWorld() || m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    m_caster->RemoveAurasDueToSpell(65917);

                    uint32 spellId = 66122; // common case

                    if (((Player*)unitTarget)->getGender() == GENDER_MALE)
                    {
                        switch (((Player*)unitTarget)->getRace())
                        {
                            case RACE_TAUREN: spellId = 66124; break;
                            case RACE_DRAENEI: spellId = 66123; break;
                        }
                    }

                    unitTarget->CastSpell(unitTarget, spellId, true);
                    return;
                }
                case 66291:                                 // Stuffed with Turkey
                case 66292:                                 // Stuffed with Sweet Potatoes
                case 66293:                                 // Stuffed with Stuffing
                case 66294:                                 // Stuffed with Pie
                case 66295:                                 // Stuffed with Cranberries
                {
                    if (!unitTarget)
                        return;

                    uint32 spellIds[5] = { 66291, 66292, 66293, 66294, 66295 };
                    uint32 buffId;  // Well Fed
                    switch(m_spellInfo->Id)
                    {
                        case 66291: buffId = 65414; break;
                        case 66292: buffId = 65410; break;
                        case 66293: buffId = 65416; break;
                        case 66294: buffId = 65415; break;
                        case 66295: buffId = 65412; break;
                    }

                    unitTarget->RemoveAurasDueToSpell(effect->CalculateSimpleValue());
                    unitTarget->CastSpell(unitTarget, buffId, true);

                    bool ok = true;
                    for (int i = 0; i < 5; ++i)
                        if (!unitTarget->HasAura(spellIds[i]) && spellIds[i] != m_spellInfo->Id)
                        {
                            ok = false;
                            break;
                        }

                    if (ok)
                        unitTarget->CastSpell(unitTarget, 61849, true); // The Spirit of Sharing

                    return;
                }
                case 66336:                                 // Mistress' Kiss (Trial of the Crusader, ->
                case 67076:                                 // -> Lord Jaraxxus encounter, all difficulties)
                case 67077:                                 // ----- // -----
                case 67078:                                 // ----- // -----
                {
                    if (unitTarget)
                        unitTarget->CastSpell(unitTarget, 66334, true);
                    return;
                }
                case 66477:                                 // Bountiful Feast
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 65422, true);
                    unitTarget->CastSpell(unitTarget, 66622, true);
                    break;
                }
                //case 66545:                                 // Summon Memory
                //{
                //    uint8 uiRandom = urand(0,25);
                //    uint32 uiSpells[26] = {66704,66705,66706,66707,66709,66710,66711,66712,66713,66714,66715,66708,66708,66691,66692,66694,66695,66696,66697,66698,66699,66700,66701,66702,66703,66543};

                //    m_caster->CastSpell(m_caster, uiSpells[uiRandom], true);
                //    break;
                //}
                case 66741:                                 // Chum the Water
                {
                    // maybe this check should be done sooner?
                    if (!m_caster->IsInWater())
                        return;

                    uint32 spellId = 0;

                    // too low/high?
                    if (roll_chance_i(33))
                        spellId = 66737;                    // angry
                    else
                    {
                        switch(rand() % 3)
                        {
                            case 0: spellId = 66740; break; // blue
                            case 1: spellId = 66739; break; // tresher
                            case 2: spellId = 66738; break; // mako
                        }
                    }

                    if (spellId)
                        m_caster->CastSpell(m_caster, spellId, true);

                    return;
                }
                case 66744:                                 // Make Player Destroy Totems
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Totem of the Earthen Ring does not really require or take reagents.
                    // Expecting RewardQuest() to already destroy them or we need additional code here to destroy.
                    unitTarget->CastSpell(unitTarget, 66747, true);
                    return;
                }
                case 67009:                                 // Nether Power (ToC25: Lord Jaraxxus)
                {
                    if (!unitTarget)
                        return;

                    for (uint8 i = 0; i < 11; ++i)
                        unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);

                    return;
                }
                case 67369:                                 // Grunty Periodic usable only on Zergling companion
                {
                    if (!unitTarget)
                        return;

                    Unit* pZerg = unitTarget->GetMiniPet(); // Only usable on Grunty companion
                    if (pZerg && pZerg->isAlive() && pZerg->GetEntry() == 11327)
                    {
                        m_caster->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                        m_caster->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_ATTACK_UNARMED);
                        return;
                    }
                    return;
                }
                case 67398:                                 // Zergling Periodic Effect (Called by Zergling Passive)
                {
                    if (!unitTarget || !unitTarget->isAlive())
                        return;
                                                            // Only usable on Grunty companion
                    Unit* pGrunty = unitTarget->GetMiniPet();
                    if (pGrunty && pGrunty->GetEntry() == 34694)
                    {
                        if (m_caster->IsWithinDist(pGrunty, 2.0f))
                            m_caster->CastSpell(pGrunty, 67400, true); //zerg attack
                        else
                        {
                            m_caster->CastSpell(pGrunty, 67397, true); // zerg rush dummy aura
                            m_caster->GetMotionMaster()->MoveFollow(pGrunty,0,0);
                        }
                    }
                    return;
                }
                case 67533:                                 // Shoot Air Rifle
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 67532, true);
                    m_caster->CastSpell(unitTarget, 65576, true);
                    break;
                }
                case 68861:                                 // Consume Soul (ICC FoS: Bronjahm)
                    if (unitTarget)
                        unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    return;
                case 68871:                                 // Wailing Souls
                    // Left or Right direction?
                    m_caster->CastSpell(m_caster, urand(0, 1) ? 68875 : 68876, false);
                    // Clear TargetGuid for sweeping
                    m_caster->SetTargetGuid(ObjectGuid());
                    return;
                case 69048:                                 // Mirrored Soul
                {
                    if (!unitTarget)
                        return;

                    // This is extremely strange!
                    // The spell should send SMSG_CHANNEL_START, SMSG_SPELL_START
                    // However it has cast time 2s, but should send SMSG_SPELL_GO instantly.
                    m_caster->CastSpell(unitTarget, 69051, true);
                    return;
                }
                case 69051:                                 // Mirrored Soul
                {
                    if (!unitTarget)
                        return;

                    // Actually this spell should be sent with SMSG_SPELL_START
                    unitTarget->CastSpell(m_caster, 69023, true);
                    return;
                }
                case 69140:                                 // Coldflame (random target selection)
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 69147:                                 // Coldflame
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 69200:                                 // Raging Spirit
                {
                    //if (!unitTarget)
                    //    return;

                    //unitTarget->CastSpell(unitTarget, 69201, true);
                    return;
                }
                case 69377:                                 // Fortitude
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 72590, true);
                    return;
                }
                case 69378:                                 // Blessing of Forgotten Kings
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 72586, true);
                    return;
                }
                case 69381:                                 // Gift of the Wild
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 72588, true);
                    return;
                }
                case 70117:                                 // Ice grip (Sindragosa pull effect)
                {
                    if (!unitTarget)
                        return;
                    float fPosX, fPosY, fPosZ;
                    m_caster->GetPosition(fPosX, fPosY, fPosZ);
                    m_caster->GetRandomPoint(fPosX, fPosY, fPosZ, m_caster->GetObjectBoundingRadius(), fPosX, fPosY, fPosZ);
                    unitTarget->NearTeleportTo(fPosX, fPosY, fPosZ+1.0f, -unitTarget->GetOrientation(), false);
                    return;
                }
                case 71446:                                 // Twilight Bloodbolt 10N
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 71447, true);
                    return;
                }
                case 71478:                                 // Twilight Bloodbolt 25N
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 71481, true);
                    return;
                }
                case 71479:                                 // Twilight Bloodbolt 10H
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 71482, true);
                    return;
                }
                case 71480:                                 // Twilight Bloodbolt 25H
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 71483, true);
                    return;
                }
                case 71806:                                 // Glittering Sparks
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 71899:                                 // Bloodbolt Whirl 10N
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 71446, true);
                    return;
                }
                case 71900:                                 // Bloodbolt Whirl 25N
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 71478, true);
                    return;
                }
                case 71901:                                 // Bloodbolt Whirl 10H
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 71479, true);
                    return;
                }
                case 71902:                                 // Bloodbolt Whirl 25H
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastSpell(unitTarget, 71480, true);
                    return;
                }
                case 72034:                                 // Whiteout
                case 72096:                                 // Whiteout (heroic)
                {
                    // cast Whiteout visual
                    m_caster->CastSpell(unitTarget, 72036, true);
                    return;
                }
                case 72195:                                 // Blood link
                {
                    if (!unitTarget)
                        return;
                    if (unitTarget->HasAura(72371))
                    {
                        unitTarget->RemoveAurasDueToSpell(72371);
                        int32 power = unitTarget->GetPower(unitTarget->getPowerType());
                        unitTarget->CastCustomSpell(unitTarget, 72371, &power, &power, NULL, true);
                    }
                    return;
                }
                case 72705:                                 // Coldflame (summon around the caster)
                {
                    if (!unitTarget)
                        return;

                    // Cast summon spells 72701, 72702, 72703, 72704
                    for (uint32 triggeredSpell = effect->CalculateSimpleValue(); triggeredSpell < m_spellInfo->Id; ++triggeredSpell)
                        unitTarget->CastSpell(unitTarget, triggeredSpell, true);

                    return;
                }
                case 74455:                                 // Conflagration
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, effect->CalculateSimpleValue(), true);
                    return;
                }
                case 80863:                                 // Blood in the Water
                {
                    if (!unitTarget)
                        return;

                    // get Rip
                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(1079, m_caster->GetObjectGuid()))
                        holder->RefreshHolder();
                    return;
                }
                case 86507:                                 // Fortitude
                {
                    if (!unitTarget)
                        return;

                    m_caster->CastCustomSpell(unitTarget, 72590, &damage, NULL, NULL, true);
                    return;
                }
                case 97985:                                 // Feral Swiftness Clear
                {
                    if (!unitTarget)
                        return;

                    unitTarget->RemoveRootsAndSnares();
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            switch(m_spellInfo->Id)
            {
                case  6201:                                 // Conjure Healthstone
                {
                    if (!unitTarget)
                        return;

                    uint32 item = 5512;
                    DoCreateItem(effect, item);
                    return;
                }
                case 47193:                                 // Demonic Empowerment
                {
                    if (!unitTarget)
                        return;

                    uint32 entry = unitTarget->GetEntry();
                    uint32 spellID;
                    switch(entry)
                    {
                        case   416: spellID = 54444; break; // imp
                        case   417: spellID = 54509; break; // fellhunter
                        case  1860: spellID = 54443; break; // void
                        case  1863: spellID = 54435; break; // succubus
                        case 17252: spellID = 54508; break; // fellguard
                        default:
                            return;
                    }
                    unitTarget->CastSpell(unitTarget, spellID, true);
                    return;
                }
                case 47422:                                 // Everlasting Affliction
                {
                    if (!unitTarget)
                        return;

                    // Need refresh caster corruption auras on target
                    if (SpellAuraHolder* corruption = unitTarget->GetSpellAuraHolder(172, m_caster->GetObjectGuid()))
                        corruption->RefreshHolder();
                    return;
                }
                case 77799:                                 // Fel Flame
                {
                    if (!unitTarget)
                        return;

                    // get Immolate
                    SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(348, m_caster->GetObjectGuid());
                    if (holder)
                    {
                        int32 newDuration = holder->GetAuraDuration() + damage * IN_MILLISECONDS;
                        if (newDuration > holder->GetAuraMaxDuration())
                            newDuration = holder->GetAuraMaxDuration();
                        holder->SetAuraDuration(newDuration);
                        holder->SendAuraUpdate(false);
                    }

                    // get Unstable Affliction
                    holder = unitTarget->GetSpellAuraHolder(30108, m_caster->GetObjectGuid());
                    if (holder)
                    {
                        int32 newDuration = holder->GetAuraDuration() + damage * IN_MILLISECONDS;
                        if (newDuration > holder->GetAuraMaxDuration())
                            newDuration = holder->GetAuraMaxDuration();
                        holder->SetAuraDuration(newDuration);
                        holder->SendAuraUpdate(false);
                    }

                    return;
                }
                case 77801:                                 // Demon Soul
                {
                    if (!unitTarget)
                        return;

                    uint32 entry = unitTarget->GetEntry();
                    uint32 spellID;
                    switch (entry)
                    {
                        case   416: spellID = 79459; break; // imp
                        case   417: spellID = 79460; break; // fellhunter
                        case  1860: spellID = 79464; break; // void
                        case  1863: spellID = 79463; break; // succubus
                        case 17252: spellID = 79462; break; // fellguard
                        default:
                            return;
                    }
                    m_caster->CastSpell(m_caster, spellID, true);
                    return;
                }
                case 89603:                                 // Cremation
                {
                    if (!unitTarget)
                        return;

                    // get Immolate
                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(348, m_caster->GetObjectGuid()))
                        holder->RefreshHolder();

                    return;
                }
                case 92931:                                 // Pandemic
                {
                    if (!unitTarget)
                        return;

                    // get Unstable Affliction
                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(30108, m_caster->GetObjectGuid()))
                        holder->RefreshHolder();

                    return;
                }
                case 93036:                                 // Regrowth Refresh
                {
                    if (!unitTarget)
                        return;

                    // get Regrowth
                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(8936, m_caster->GetObjectGuid()))
                        if (holder->GetAuraDuration() < damage * IN_MILLISECONDS)
                        {
                            holder->SetAuraDuration(damage * IN_MILLISECONDS);
                            holder->SendAuraUpdate(false);
                        }

                    return;
                }
                case 96931:                                 // Eyes of Occu'thar
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Unit* caster = m_caster->GetMap()->GetUnit(m_originalCasterGUID);
                    if (!caster)
                        return;

                    m_caster->CastSpell(m_caster, 96932, true);

                    if (Creature* eye = caster->SummonCreature(52389, m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), m_caster->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))
                        eye->CastSpell(m_caster, 96942, true);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            switch (m_spellInfo->Id)
            {
                case 47948:                                 // Pain and Suffering
                {
                    if (!unitTarget)
                        return;

                    // Refresh Shadow Word: Pain on target
                    Unit::SpellAuraHolderMap& auras = unitTarget->GetSpellAuraHolderMap();
                    for(Unit::SpellAuraHolderMap::iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        SpellEntry const *spellInfo = (*itr).second->GetSpellProto();
                        SpellClassOptionsEntry const* swpClassOptions = spellInfo->GetSpellClassOptions();
                        if (swpClassOptions && swpClassOptions->SpellFamilyName == SPELLFAMILY_PRIEST &&
                            (swpClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000008000)) &&
                            (*itr).second->GetCasterGuid() == m_caster->GetObjectGuid())
                        {
                            (*itr).second->RefreshHolder();
                            return;
                        }
                    }
                    return;
                }
                case 81585:                                 // Chakra: Serenity
                {
                    if (!unitTarget)
                        return;

                    // Search Renew
                    if (SpellAuraHolder* renew = unitTarget->GetSpellAuraHolder(139, m_caster->GetObjectGuid()))
                        renew->RefreshHolder();
                    return;
                }
                case 87151:                                 // Archangel
                {
                    if (!unitTarget)
                        return;

                    // Evangelism
                    SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(81661, m_caster->GetObjectGuid());
                    if (!holder)
                        // Dark Evangelism
                        holder = unitTarget->GetSpellAuraHolder(87118, m_caster->GetObjectGuid());
                    if (!holder)
                        return;

                    int32 stackAmt = holder->GetStackAmount();

                    if (holder->GetId() == 81661)
                    {
                        // Archangel
                        // healing done
                        SpellEntry const* spellProto = sSpellStore.LookupEntry(81700);
                        int32 bp = stackAmt * spellProto->CalculateSimpleValue(EFFECT_INDEX_0);
                        m_caster->CastCustomSpell(unitTarget, spellProto, &bp, NULL, NULL, true);

                        // mana regen
                        spellProto = sSpellStore.LookupEntry(87152);
                        bp = stackAmt * spellProto->CalculateSimpleValue(EFFECT_INDEX_0);
                        m_caster->CastCustomSpell(unitTarget, spellProto, &bp, NULL, NULL, true);
                    }
                    else
                    {
                        // Dark Archangel, shadow damage done
                        SpellEntry const* spellProto = sSpellStore.LookupEntry(87153);
                        int32 bp0 = stackAmt * spellProto->CalculateSimpleValue(EFFECT_INDEX_0);
                        int32 bp1 = stackAmt * spellProto->CalculateSimpleValue(EFFECT_INDEX_1);
                        m_caster->CastCustomSpell(unitTarget, spellProto, &bp0, &bp1, NULL, true);

                        // mana regen
                        SpellEntry const* spellProto2 = sSpellStore.LookupEntry(87152);
                        bp0 = stackAmt * spellProto2->CalculateSimpleValue(EFFECT_INDEX_0) * spellProto->CalculateSimpleValue(EFFECT_INDEX_2);
                        m_caster->CastCustomSpell(unitTarget, spellProto2, &bp0, NULL, NULL, true);
                    }

                    unitTarget->RemoveSpellAuraHolder(holder);
                    return;
                }
                case 89490:                                 // Strength of Soul
                {
                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                        return;

                    // Weakened Soul
                    SpellAuraHolder* ws = unitTarget->GetSpellAuraHolder(6788, m_caster->GetObjectGuid());
                    if (!ws)
                        return;

                    int32 durMod = damage * IN_MILLISECONDS;
                    int32 oldDur = ws->GetAuraDuration();
                    if (oldDur <= durMod)
                    {
                        unitTarget->RemoveSpellAuraHolder(ws);
                        return;
                    }

                    int32 newDur = oldDur - durMod;
                    ws->SetAuraDuration(newDur);
                    ws->SendAuraUpdate(false);
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            switch (m_spellInfo->Id)
            {
                case 34026:                                 // Kill Command
                {
                    if (!unitTarget)
                        return;

                    Unit* victim = unitTarget->getVictim();
                    if (!victim)
                        return;

                    m_caster->CastSpell(victim, damage, true, NULL, NULL, unitTarget->GetObjectGuid(), NULL);
                    return;
                }
                case 53209:                                 // Chimera Shot
                {
                    // heal caster
                    m_caster->CastSpell(m_caster, 53353, true);

                    if (!unitTarget)
                        return;

                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(1978, m_caster->GetObjectGuid()))
                        holder->RefreshHolder();
                    return;
                }
                case 53271:                                 // Master's Call
                {
                    if (!unitTarget)
                        return;

                    // script effect have in value, but this outdated removed part
                    unitTarget->CastSpell(unitTarget, 62305, true);
                    return;
                }
                case 53412:                                 // Invigoration (pet triggered script, master targeted)
                {
                    if (!unitTarget)
                        return;

                    Unit::AuraList const& auras = unitTarget->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator i = auras.begin();i != auras.end(); ++i)
                    {
                        // Invigoration (master talent)
                        if ((*i)->GetSpellProto()->SpellIconID == 3487 && (*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_HUNTER)
                        {
                            int32 bp = (*i)->GetModifier()->m_amount;
                            unitTarget->CastCustomSpell(unitTarget, 53398, &bp, NULL, NULL, true, NULL, *i, m_caster->GetObjectGuid());
                            break;
                        }
                    }
                    return;
                }
                case 55709:                                 // Heart of the Phoenix
                {
                    if (!unitTarget || !unitTarget->GetObjectGuid().IsPet())
                        return;

                    if (!unitTarget->HasAura(55711))
                    {
                        ((Pet*)unitTarget)->GetOwner()->CastSpell(unitTarget, 54114, true);
                        int32 pct = 100;
                        unitTarget->CastCustomSpell(unitTarget, 54114, &pct, NULL, NULL, true);
                    }
                    else
                        SendCastResult(SPELL_FAILED_CASTER_AURASTATE);
                    return;
                }
                case 77767:                                 // Cobra Shot
                {
                    if (SpellEntry const* spellEntry = sSpellStore.LookupEntry(77443))
                    {
                        int32 bp = spellEntry->CalculateSimpleValue(EFFECT_INDEX_0);
                        // Item - Hunter T13 2P Bonus (Steady Shot and Cobra Shot)
                        if (m_caster->HasAura(105732))
                            bp *= 2;

                        if (unitTarget)
                        {
                            // search Termination
                            Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                            for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                            {
                                if ((*itr)->GetSpellProto()->SpellIconID == 2008 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_HUNTER)
                                {
                                    if (unitTarget->GetHealthPercent() < (*itr)->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1))
                                        bp += (*itr)->GetModifier()->m_amount;
                                    break;
                                }
                            }

                            // Glyph of Dazzled Prey
                            if (Aura* glyph = m_caster->GetAura(56856, EFFECT_INDEX_0))
                            {
                                // check dazed affect
                                Unit::AuraList const& decSpeedList = unitTarget->GetAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
                                for(Unit::AuraList::const_iterator iter = decSpeedList.begin(); iter != decSpeedList.end(); ++iter)
                                {
                                    if ((*iter)->GetSpellProto()->SpellIconID == 15 && (*iter)->GetSpellProto()->GetDispel() == 0)
                                    {
                                        bp += glyph->GetModifier()->m_amount;
                                        break;
                                    }
                                }
                            }
                        }

                        // energize
                        m_caster->CastCustomSpell(m_caster, spellEntry, &bp, NULL, NULL, true);
                    }

                    if (!unitTarget)
                        return;

                    // get Serpent Sting
                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(1978, m_caster->GetObjectGuid()))
                    {
                        int32 maxDuration = holder->GetAuraMaxDuration();
                        int32 newDuration = std::min(holder->GetAuraDuration() + damage * IN_MILLISECONDS, maxDuration);

                        holder->SetAuraDuration(newDuration);
                        holder->SendAuraUpdate(false);
                    }
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_POTION:
        {
            switch(m_spellInfo->Id)
            {
                case 28698:                                 // Dreaming Glory
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(unitTarget, 28694, true);
                    break;
                }
                case 28702:                                 // Netherbloom
                {
                    if (!unitTarget)
                        return;

                    // 25% chance of casting a random buff
                    if (roll_chance_i(75))
                        return;

                    // triggered spells are 28703 to 28707
                    // Note: some sources say, that there was the possibility of
                    //       receiving a debuff. However, this seems to be removed by a patch.
                    const uint32 spellid = 28703;

                    // don't overwrite an existing aura
                    for(uint8 i = 0; i < 5; ++i)
                        if (unitTarget->HasAura(spellid + i, EFFECT_INDEX_0))
                            return;

                    unitTarget->CastSpell(unitTarget, spellid+urand(0, 4), true);
                    break;
                }
                case 28720:                                 // Nightmare Vine
                {
                    if (!unitTarget)
                        return;

                    // 25% chance of casting Nightmare Pollen
                    if (roll_chance_i(75))
                        return;

                    unitTarget->CastSpell(unitTarget, 28721, true);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            switch(m_spellInfo->Id)
            {
                case 50842:                                 // Pestilence
                {
                    if (!unitTarget)
                        return;

                    Unit* mainTarget = m_targets.getUnitTarget();
                    if (!mainTarget)
                        return;

                    if (mainTarget == unitTarget)
                        return;

                    // Blood Plague
                    if (mainTarget->HasAura(55078))
                        m_caster->CastSpell(unitTarget, 55078, true);

                    // Frost Fever
                    if (mainTarget->HasAura(55095))
                        m_caster->CastSpell(unitTarget, 55095, true);

                    break;
                }
                // Raise ally
                case 61999:
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER || unitTarget->isAlive())
                    {
                        SendCastResult(SPELL_FAILED_TARGET_NOT_DEAD);
                        finish(true);
                        CancelGlobalCooldown();
                        return;
                    }

                    // hack remove death
                    unitTarget->CastSpell(unitTarget, m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_0), true);
                    CancelGlobalCooldown();
                    return;
                }
            }
            // Festering Strike
            case 85948:
            {
                // Blood Plague, Frost Fever, Chains of Ice
                uint32 spells[3] = { 55078, 55095, 45524 };
                
                for (int i = 0; i < 3; ++i)
                {
                    if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(spells[i]))
                    {
                        int32 newDuration = holder->GetAuraDuration() + damage * IN_MILLISECONDS;
                        if (newDuration > holder->GetAuraMaxDuration())
                            newDuration = holder->GetAuraMaxDuration();
                        holder->SetAuraDuration(newDuration);
                        holder->SendAuraUpdate(false);
                    }
                }
                return;
            }
            // Glyph of Resilient Grip
            case 90289:
            {
                if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
                    return;

                ((Player*)unitTarget)->RemoveSpellCooldown(49576, true);
                return;
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            switch(m_spellInfo->Id)
            {
                case 47962:                                     // Resque inquired soldier
                {
                    if (!unitTarget)
                        return;

                    unitTarget->CastSpell(m_caster, effect->CalculateSimpleValue(), true);
                    m_caster->CastSpell(m_caster, 47967, true);
                    return;
                }
                case 64380:                                     // Shattering Throw
                {
                    if (!unitTarget || !unitTarget->isAlive())
                        return;

                    // remove immunity effects
                    unitTarget->RemoveAurasByMechanicMask(1 << (MECHANIC_IMMUNE_SHIELD - 1), true);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            switch (m_spellInfo->Id)
            {
                case 73921:                                     // Earthquake
                {
                    // Cast Stun
                    if (unitTarget && roll_chance_i(damage))
                        m_caster->CastSpell(unitTarget, 77505, true);
                    return;
                }
                case 99202:                                     // Taming the Flames
                {
                    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
                        ((Player*)unitTarget)->SendModifyCooldown(2894, -4 * IN_MILLISECONDS);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch (m_spellInfo->Id)
            {
                case 11129:                                     // Combustion
                {
                    if (!unitTarget)
                        return;

                    // Ignite, Pyroblast, Living Bomb
                    uint32 spellIds[3] = { 12654, 11366, 44457 };

                    int32 bp = 0;
                    for (uint8 i = 0; i < 3; ++i)
                    {
                        if (SpellAuraHolder* holder = unitTarget->GetSpellAuraHolder(spellIds[i], m_caster->GetObjectGuid()))
                        {
                            for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
                                if (Aura* aura = holder->GetAuraByEffectIndex(SpellEffectIndex(j)))
                                {
                                    if (aura->GetModifier()->m_auraname != SPELL_AURA_PERIODIC_DAMAGE)
                                        continue;

                                    int32 mod = aura->GetModifier()->m_amount;
                                    uint32 dmgClass = holder->GetSpellProto()->GetDmgClass();
                                    if (dmgClass == SPELL_DAMAGE_CLASS_NONE || dmgClass == SPELL_DAMAGE_CLASS_MAGIC)
                                        mod = unitTarget->SpellDamageBonusTaken(m_caster, holder->GetSpellProto(), mod, DOT, holder->GetStackAmount());

                                    bp += mod * IN_MILLISECONDS / aura->GetModifier()->periodictime;
                                    break;
                                }
                        }
                    }

                    if (bp)
                        m_caster->CastCustomSpell(unitTarget, 83853, &bp, NULL, NULL, true);
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch (m_spellInfo->Id)
            {
                case 88433:                                     // Empowered Touch
                {
                    if (!unitTarget)
                        return;

                    SpellAuraHolder* holder = NULL;
                    // Lifebloom
                    if (holder = unitTarget->GetSpellAuraHolder(33763, m_caster->GetObjectGuid()))
                        holder->RefreshHolder();
                    // Lifebloom
                    else if (holder = unitTarget->GetSpellAuraHolder(94447, m_caster->GetObjectGuid()))
                        holder->RefreshHolder();

                    if (holder)
                    {
                        // Revitalize
                        if (m_caster->GetTypeId() == TYPEID_PLAYER && ((Player*)m_caster)->GetKnownTalentRankById(8269))
                        {
                            // Replenishment
                            m_caster->CastSpell(m_caster, 57669, true);
                        }
                    }
                    return;
                }
            }
            break;
        }
    }

    // normal DB scripted effect
    if (!unitTarget)
        return;

    // Previous effect might have started script
    if (!ScriptMgr::CanSpellEffectStartDBScript(m_spellInfo, SpellEffectIndex(effect->EffectIndex)))
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell ScriptStart spellid %u in EffectScriptEffect", m_spellInfo->Id);
    m_caster->GetMap()->ScriptsStart(sSpellScripts, m_spellInfo->Id, m_caster, unitTarget);
}

void Spell::EffectSanctuary(SpellEffectEntry const* /*effect*/)
{
    if(!unitTarget)
        return;

    unitTarget->m_lastSanctuaryTime = WorldTimer::getMSTime();

    unitTarget->CombatStop();
    unitTarget->getHostileRefManager().deleteReferences();  // stop all fighting

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        UnitList targets;
        {
            MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(m_caster, m_caster, m_caster->GetMap()->GetVisibilityDistance());
            MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
            Cell::VisitAllObjects(m_caster, searcher, m_caster->GetMap()->GetVisibilityDistance());
        }

        for(UnitList::iterator tIter = targets.begin(); tIter != targets.end(); ++tIter)
        {
            for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
            {
                if ((*tIter)->GetCurrentSpell(CurrentSpellTypes(i))
                    && (*tIter)->GetCurrentSpell(CurrentSpellTypes(i))->m_targets.getUnitTargetGuid() == unitTarget->GetObjectGuid())
                {
                    (*tIter)->InterruptSpell(CurrentSpellTypes(i), false);
                    (*tIter)->AttackStop();
                }
            }

            //if ((*tIter)->getHostileRefManager().isEmpty())
            //    (*tIter)->CombatStop();
        }
    }

    // Vanish allows to remove all threat and cast regular stealth so other spells can be used
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x0000000000000800)))
        ((Player *)m_caster)->RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
}

void Spell::EffectAddComboPoints(SpellEffectEntry const* effect /*effect*/)
{
    if (!unitTarget)
        return;

    if (m_spellInfo->Id == 73981)   // Redirect
    {
        damage = m_caster->GetComboPoints();

        int32 bp = 0;
        // Search Shallow, Moderate and Deep Insight auras
        Unit::AuraList const& dummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
        for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
        {
            if ((*itr)->GetId() == 84745 || (*itr)->GetId() == 84746 || (*itr)->GetId() == 84747)
            {
                bp = (*itr)->GetModifier()->m_amount;
                if (SpellAuraHolder* holder = (*itr)->GetHolder())
                    holder->RefreshHolder();
                break;
            }
        }

        // cast damage bonus aura
        if (bp)
            m_caster->CastCustomSpell(unitTarget, 84748, &bp, NULL, NULL, true);
    }

    if (damage <= 0)
        return;

    m_caster->AddComboPoints(unitTarget, damage);
}

void Spell::EffectDuel(SpellEffectEntry const* effect)
{
    if(!m_caster || !unitTarget || m_caster->GetTypeId() != TYPEID_PLAYER || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *caster = (Player*)m_caster;
    Player *target = (Player*)unitTarget;

    // caster or target already have requested duel
    if (caster->duel || target->duel || !target->GetSocial() || target->GetSocial()->HasIgnore(caster->GetObjectGuid()))
        return;

    // Players can only fight a duel with each other outside (=not inside dungeons and not in capital cities)
    AreaTableEntry const* casterAreaEntry = GetAreaEntryByAreaID(caster->GetAreaId());
    if (casterAreaEntry && !(casterAreaEntry->flags & AREA_FLAG_DUEL))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    AreaTableEntry const* targetAreaEntry = GetAreaEntryByAreaID(target->GetAreaId());
    if (targetAreaEntry && !(targetAreaEntry->flags & AREA_FLAG_DUEL))
    {
        SendCastResult(SPELL_FAILED_NO_DUELING);            // Dueling isn't allowed here
        return;
    }

    //CREATE DUEL FLAG OBJECT
    GameObject* pGameObj = new GameObject;

    uint32 gameobject_id = effect->EffectMiscValue;

    Map* map = m_caster->GetMap();
    float x = (m_caster->GetPositionX() + unitTarget->GetPositionX()) * 0.5f;
    float y = (m_caster->GetPositionY() + unitTarget->GetPositionY()) * 0.5f;
    float z = m_caster->GetPositionZ();
    m_caster->UpdateAllowedPositionZ(x, y, z);
    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), gameobject_id, map, m_caster->GetPhaseMask(), x, y, z, m_caster->GetOrientation()))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_FACTION, m_caster->getFaction() );
    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel()+1 );
    pGameObj->SetRespawnTime(m_duration > 0 ? m_duration/IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);

    m_caster->AddGameObject(pGameObj);
    map->Add(pGameObj);
    //END

    // Send request
    WorldPacket data(SMSG_DUEL_REQUESTED, 8 + 8);
    data << pGameObj->GetObjectGuid();
    data << caster->GetObjectGuid();
    caster->GetSession()->SendPacket(&data);
    target->GetSession()->SendPacket(&data);

    // create duel-info
    DuelInfo *duel   = new DuelInfo;
    duel->initiator  = caster;
    duel->opponent   = target;
    duel->startTime  = 0;
    duel->startTimer = 0;
    caster->duel     = duel;

    DuelInfo *duel2   = new DuelInfo;
    duel2->initiator  = caster;
    duel2->opponent   = caster;
    duel2->startTime  = 0;
    duel2->startTimer = 0;
    target->duel      = duel2;

    caster->SetGuidValue(PLAYER_DUEL_ARBITER, pGameObj->GetObjectGuid());
    target->SetGuidValue(PLAYER_DUEL_ARBITER, pGameObj->GetObjectGuid());
}

void Spell::EffectStuck(SpellEffectEntry const* effect /*effect*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if(!sWorld.getConfig(CONFIG_BOOL_CAST_UNSTUCK))
        return;

    Player* pTarget = (Player*)unitTarget;

    DEBUG_LOG("Spell Effect: Stuck");
    DETAIL_LOG("Player %s (guid %u) used auto-unstuck future at map %u (%f, %f, %f)", pTarget->GetName(), pTarget->GetGUIDLow(), m_caster->GetMapId(), m_caster->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());

    if(pTarget->IsTaxiFlying())
        return;

    // homebind location is loaded always
    pTarget->TeleportToHomebind(unitTarget==m_caster ? TELE_TO_SPELL : 0);
    if (pTarget->isDead())
    {
        pTarget->ResurrectPlayer(0.5f);
        pTarget->SpawnCorpseBones();
    }
    // Stuck spell trigger Hearthstone cooldown
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(8690);
    if(!spellInfo)
        return;
    Spell spell(pTarget, spellInfo, true);
    spell.SendSpellCooldown();
}

void Spell::EffectSummonPlayer(SpellEffectEntry const* /*effect*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // Evil Twin (ignore player summon, but hide this for summoner)
    if(unitTarget->GetDummyAura(23445))
        return;

    float x, y, z;
    m_caster->GetClosePoint(x, y, z, unitTarget->GetObjectBoundingRadius());

    ((Player*)unitTarget)->SetSummonPoint(m_caster->GetMapId(),x,y,z);

    WorldPacket data(SMSG_SUMMON_REQUEST, 8+4+4);
    data << m_caster->GetObjectGuid();                      // summoner guid
    data << uint32(m_caster->GetZoneId());                  // summoner zone
    data << uint32(MAX_PLAYER_SUMMON_DELAY*IN_MILLISECONDS); // auto decline after msecs
    ((Player*)unitTarget)->GetSession()->SendPacket(&data);
}

static ScriptInfo generateActivateCommand()
{
    ScriptInfo si;
    si.id = 0;
    si.buddyEntry = 0;
    si.searchRadius = 0;
    si.data_flags = 0x00;
    return si;
}

void Spell::EffectActivateObject(SpellEffectEntry const* effect)
{
    if(!gameObjTarget)
        return;

    // Ranger: some hack's for quest's...
    switch (m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id)
            {
                case 39844:                                                            // Fires Over Skettis (http://www.wowhead.com/quest=11008)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (gameObjTarget->GetTypeId() != TYPEID_GAMEOBJECT || gameObjTarget->GetEntry() != 185549 || !gameObjTarget->isSpawned())
                        return;

                    ((Player*)m_caster)->KilledMonsterCredit(22991);

                    gameObjTarget->SetRespawnTime(MINUTE*5);

                    break;
                }
            }
            break;
        }
    }

    static ScriptInfo activateCommand = generateActivateCommand();

    int32 delay_secs = effect->CalculateSimpleValue();

    gameObjTarget->GetMap()->ScriptCommandStart(activateCommand, delay_secs, m_caster, gameObjTarget);
}

void Spell::EffectApplyGlyph(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER || m_glyphIndex >= MAX_GLYPH_SLOT_INDEX)
        return;

    Player *player = (Player*)m_caster;

        // glyph sockets level requirement
    uint8 minLevel = 0;
    switch (m_glyphIndex)
    {
        case 0:
        case 1:
        case 6: minLevel = 25; break;
        case 2:
        case 3:
        case 7: minLevel = 50; break;
        case 4:
        case 5:
        case 8: minLevel = 75; break;
    }

    if (minLevel && m_caster->getLevel() < minLevel)
    {
        SendCastResult(SPELL_FAILED_GLYPH_SOCKET_LOCKED);
        return;
    }

    // apply new one
    if (uint32 glyph = effect->EffectMiscValue)
    {
        if(GlyphPropertiesEntry const *gp = sGlyphPropertiesStore.LookupEntry(glyph))
        {
            if(GlyphSlotEntry const *gs = sGlyphSlotStore.LookupEntry(player->GetGlyphSlot(m_glyphIndex)))
            {
                if(gp->TypeFlags != gs->TypeFlags)
                {
                    SendCastResult(SPELL_FAILED_INVALID_GLYPH);
                    return;                                 // glyph slot mismatch
                }
            }

            // remove old glyph
            player->ApplyGlyph(m_glyphIndex, false);
            player->SetGlyph(m_glyphIndex, glyph);
            player->ApplyGlyph(m_glyphIndex, true);
            player->SendTalentsInfoData(false);
        }
    }
    else if (player->GetGlyph(m_glyphIndex))                // Removing the glyph, get the old one
    {
        player->ApplyGlyph(m_glyphIndex, false);
        player->SetGlyph(m_glyphIndex, 0);
        player->SendTalentsInfoData(false);
    }
}

void Spell::DoSummonTotem(SpellEffectEntry const * effect, uint8 slot_dbc)
{
    // DBC store slots starting from 1, with no slot 0 value)
    int slot = slot_dbc ? slot_dbc - 1 : TOTEM_SLOT_NONE;

    // unsummon old totem
    if (slot < MAX_TOTEM_SLOT)
        if (Totem *OldTotem = m_caster->GetTotem(TotemSlot(slot)))
            OldTotem->UnSummon();

    // FIXME: Setup near to finish point because GetObjectBoundingRadius set in Create but some Create calls can be dependent from proper position
    // if totem have creature_template_addon.auras with persistent point for example or script call
    float angle = slot < MAX_TOTEM_SLOT ? M_PI_F/MAX_TOTEM_SLOT - (slot*2*M_PI_F/MAX_TOTEM_SLOT) : 0;

    CreatureCreatePos pos;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        DEBUG_LOG("Spell::DoSummonTotem: TARGET_FLAG_DEST_LOCATION is used for spell %u effIdx %u", m_spellInfo->Id, effect->EffectIndex);
        float x, y, z;
        m_targets.getDestination(x, y, z);
        m_caster->UpdateGroundPositionZ(x, y, z);
        pos = CreatureCreatePos(m_caster->GetMap(), x, y, z, m_caster->GetOrientation(), m_caster->GetPhaseMask());
    }
    else
        pos = CreatureCreatePos(m_caster, m_caster->GetOrientation(), 2.0f, angle);

    CreatureInfo const *cinfo = ObjectMgr::GetCreatureTemplate(effect->EffectMiscValue);
    if (!cinfo)
    {
        sLog.outErrorDb("Creature entry %u does not exist but used in spell %u totem summon.", m_spellInfo->Id, effect->EffectMiscValue);
        return;
    }

    Totem* pTotem = new Totem;

    if (!pTotem->Create(m_caster->GetMap()->GenerateLocalLowGuid(HIGHGUID_UNIT), pos, cinfo, m_caster))
    {
        delete pTotem;
        return;
    }

    pTotem->SetRespawnCoord(pos);

    if (slot < MAX_TOTEM_SLOT)
        m_caster->_AddTotem(TotemSlot(slot),pTotem);

    pTotem->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
    //pTotem->SetName("");                                  // generated by client
    pTotem->SetOwner(m_caster);
    pTotem->SetTypeBySummonSpell(m_spellInfo);              // must be after Create call where m_spells initialized

    pTotem->SetDuration(m_duration);

    // Ranger: Mana Tide Totem - health based on caster's health
    if (m_spellInfo->Id == 16190)
        damage = m_caster->GetMaxHealth() * m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100;
    // Anti-Magic Zone AP bonus
    else if (m_spellInfo->Id == 51052)
        damage += m_caster->GetTotalAttackPowerValue(BASE_ATTACK) * 2;

    if (damage)                                             // if not spell info, DB values used
    {
        pTotem->SetMaxHealth(damage);
        pTotem->SetHealth(damage);
    }

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        pTotem->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

    if (m_caster->IsPvP())
        pTotem->SetPvP(true);

    if (m_caster->IsFFAPvP())
        pTotem->SetFFAPvP(true);

    // sending SMSG_TOTEM_CREATED before add to map (done in Summon)
    if (slot >= 0 && slot < MAX_TOTEM_SLOT && m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        // set display id depending on race
        SummonPropertiesEntry const *properties = sSummonPropertiesStore.LookupEntry(effect->EffectMiscValueB);
        if(properties)
        {
            uint32 displayId = m_caster->GetModelForTotem(PlayerTotemType(properties->Id));
            if(displayId)
            {
                pTotem->SetNativeDisplayId(displayId);
                pTotem->SetDisplayId(displayId);
            }
        }

        WorldPacket data(SMSG_TOTEM_CREATED, 1 + 8 + 4 + 4);
        data << uint8(slot);
        data << pTotem->GetObjectGuid();
        data << uint32(m_duration);
        data << uint32(m_spellInfo->Id);
        ((Player*)m_caster)->SendDirectMessage(&data);
    }

    pTotem->Summon(m_caster);
}

void Spell::EffectEnchantHeldItem(SpellEffectEntry const* effect)
{
    // this is only item spell effect applied to main-hand weapon of target player (players in area)
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* item_owner = (Player*)unitTarget;
    Item* item = item_owner->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);

    if(!item )
        return;

    // must be equipped
    if(!item ->IsEquipped())
        return;

    if (effect->EffectMiscValue)
    {
        uint32 enchant_id = effect->EffectMiscValue;
        int32 duration = m_duration;                        // Try duration index first...
        if (!duration)
            duration = m_currentBasePoints[SpellEffectIndex(effect->EffectIndex)];        // Base points after...
        if (!duration)
            duration = 10;                                  // 10 seconds for enchants which don't have listed duration

        SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
        if(!pEnchant)
            return;

        // Always go to temp enchantment slot
        EnchantmentSlot slot = TEMP_ENCHANTMENT_SLOT;

        // Enchantment will not be applied if a different one already exists
        if(item->GetEnchantmentId(slot) && item->GetEnchantmentId(slot) != enchant_id)
            return;

        bool dontApply = 
            item->IsEquipped() 
            && (item->GetSlot() == EQUIPMENT_SLOT_MAINHAND || item->GetSlot() == EQUIPMENT_SLOT_OFFHAND || item->GetSlot() == EQUIPMENT_SLOT_RANGED)
            && !item_owner->CanUseEquippedWeapon(Player::GetAttackBySlot(item->GetSlot()));

        // Apply the temporary enchantment
        item->SetEnchantment(slot, enchant_id, duration*IN_MILLISECONDS, 0, m_caster->GetObjectGuid());
        if (!dontApply) item_owner->ApplyEnchantment(item, slot, true);
    }
}

void Spell::EffectDisEnchant(SpellEffectEntry const* /*effect*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* p_caster = (Player*)m_caster;
    if(!itemTarget || !itemTarget->GetProto()->DisenchantID)
        return;

    p_caster->UpdateCraftSkill(m_spellInfo->Id);

    ((Player*)m_caster)->SendLoot(itemTarget->GetObjectGuid(),LOOT_DISENCHANTING);

    // item will be removed at disenchanting end
}

void Spell::EffectInebriate(SpellEffectEntry const* /*effect*/)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *player = (Player*)unitTarget;
    uint16 currentDrunk = player->GetDrunkValue();
    uint16 drunkMod = damage * 256;
    if (currentDrunk + drunkMod > 0xFFFF)
        currentDrunk = 0xFFFF;
    else
        currentDrunk += drunkMod;
    player->SetDrunkValue(currentDrunk, m_CastItem ? m_CastItem->GetEntry() : 0);
}

void Spell::EffectFeedPet(SpellEffectEntry const* effect)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)m_caster;

    Item* foodItem = m_targets.getItemTarget();
    if(!foodItem)
        return;

    Pet *pet = _player->GetPet();
    if(!pet)
        return;

    if(!pet->isAlive())
        return;

    int32 benefit = pet->GetCurrentFoodBenefitLevel(foodItem->GetProto()->ItemLevel);
    if(benefit <= 0)
        return;

    uint32 count = 1;
    _player->DestroyItemCount(foodItem,count,true);
    // TODO: fix crash when a spell has two effects, both pointed at the same item target

    SpellEntry const* triggeredSpell = sSpellStore.LookupEntry(effect->EffectTriggerSpell);
    if (!triggeredSpell)
        return;

    if (IsSpellHaveAura(triggeredSpell, SPELL_AURA_PERIODIC_ENERGIZE))
        m_caster->CastCustomSpell(pet, triggeredSpell, &benefit, NULL, NULL, true);
    else
        m_caster->CastSpell(pet, triggeredSpell, true);
}

void Spell::EffectDismissPet(SpellEffectEntry const* /*effect*/)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Pet* pet = m_caster->GetPet();

    // not let dismiss dead pet
    if(!pet||!pet->isAlive())
        return;

    pet->Unsummon(PET_SAVE_NOT_IN_SLOT, m_caster);
}

void Spell::EffectSummonObject(SpellEffectEntry const* effect)
{
    uint32 go_id = effect->EffectMiscValue;
    if (!go_id)
        return;

    uint8 slot = effect->EffectMiscValueB;
    if (slot >= MAX_OBJECT_SLOT)
        return;

    if (ObjectGuid guid = m_caster->m_ObjectSlotGuid[slot])
    {
        if (GameObject* obj = m_caster ? m_caster->GetMap()->GetGameObject(guid) : NULL)
            obj->SetLootState(GO_JUST_DEACTIVATED);

        m_caster->m_ObjectSlotGuid[slot].Clear();
    }

    GameObject* pGameObj = new GameObject;

    float x, y, z;
    // If dest location if present
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.getDestination(x, y, z);
    // Summon in random point all other units if location present
    else
    {
        // Demonic Circle: Summon
        if(m_spellInfo->Id == 48018)
        {
            x = m_caster->GetPositionX();
            y = m_caster->GetPositionY();
            z = m_caster->GetPositionZ();
        }
        else
            m_caster->GetClosePoint(x, y, z, DEFAULT_WORLD_OBJECT_SIZE);
    }

    Map* map = m_caster->GetMap();
    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), go_id, map,
        m_caster->GetPhaseMask(), x, y, z, m_caster->GetOrientation()))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel());
    pGameObj->SetRespawnTime(m_duration > 0 ? m_duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);
    m_caster->AddGameObject(pGameObj);

    map->Add(pGameObj);

    m_caster->m_ObjectSlotGuid[slot] = pGameObj->GetObjectGuid();

    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);
    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
        ((Creature*)m_originalCaster)->AI()->JustSummoned(pGameObj);
}

void Spell::EffectSummonRaidMarker(SpellEffectEntry const* effect)
{
    Unit* caster = GetAffectiveCaster();
    // FIXME: in case wild GO will used wrong affective caster (target in fact) as dynobject owner
    if (!caster)
        caster = m_caster;

    if (caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* pCaster = (Player*)caster;

    Group* group = pCaster->GetGroup();
    if (!group)
        return;

    if (!group->IsAssistant(pCaster->GetObjectGuid()) && !group->IsLeader(pCaster->GetObjectGuid()))
        return;

    uint32 go_id = effect->EffectMiscValue;

    uint8 slot = damage;

    float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));

    if (Player* modOwner = pCaster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius);

    float x, y, z;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.getDestination(x, y, z);
    else if (Unit* target = m_targets.getUnitTarget())
        target->GetPosition(x, y, z);
    else
        pCaster->GetPosition(x, y, z);

    DynamicObject* dynObj = new DynamicObject;
    if (!dynObj->Create(pCaster->GetMap()->GenerateLocalLowGuid(HIGHGUID_DYNAMICOBJECT), pCaster, m_spellInfo->Id, SpellEffectIndex(effect->EffectIndex), x, y, z, m_duration, radius, DYNAMIC_OBJECT_RAID_MARKER))
    {
        delete dynObj;
        return;
    }

    group->SetRaidMarker(slot, pCaster, dynObj->GetObjectGuid());
    pCaster->GetMap()->Add(dynObj);
}

void Spell::EffectResurrect(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (unitTarget->isAlive() || !unitTarget->IsInWorld())
        return;

    switch (m_spellInfo->Id)
    {
        case 8342:                                          // Defibrillate (Goblin Jumper Cables) has 33% chance on success
        case 22999:                                         // Defibrillate (Goblin Jumper Cables XL) has 50% chance on success
        case 54732:                                         // Defibrillate (Gnomish Army Knife) has 67% chance on success
        {
            uint32 failChance = 0;
            uint32 failSpellId = 0;
            switch (m_spellInfo->Id)
            {
                case 8342:  failChance=67; failSpellId = 8338;  break;
                case 22999: failChance=50; failSpellId = 23055; break;
                case 54732: failChance=33; failSpellId = 0; break;
            }

            if (roll_chance_i(failChance))
            {
                if (failSpellId)
                    m_caster->CastSpell(m_caster, failSpellId, true, m_CastItem);
                return;
            }
            break;
        }
        default:
            break;
    }

    Player* pTarget = ((Player*)unitTarget);

    if(pTarget->isRessurectRequested())       // already have one active request
        return;
    float healthPct = damage / 100.0f;
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (((Player*)m_caster)->GetGuildId() == pTarget->GetGuildId())
            healthPct *= m_caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_RESURRECTED_HEALTH_BY_GUILD_MEMBER);
    }
    uint32 health = uint32(pTarget->GetMaxHealth() * std::min(healthPct, 1.0f));
    uint32 mana   = pTarget->GetMaxPower(POWER_MANA) * damage / 100;

    pTarget->setResurrectRequestData(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana, 0);
    SendResurrectRequest(pTarget);
}

void Spell::EffectAddExtraAttacks(SpellEffectEntry const* /*effect*/)
{
    if(!unitTarget || !unitTarget->isAlive())
        return;

    if( unitTarget->m_extraAttacks )
        return;

    unitTarget->m_extraAttacks = damage;
}

void Spell::EffectParry(SpellEffectEntry const* /*effect*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanParry(true);
}

void Spell::EffectBlock(SpellEffectEntry const* /*effect*/)
{
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
        ((Player*)unitTarget)->SetCanBlock(true);
}

void Spell::EffectLeapForward(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->IsTaxiFlying())
        return;

    if (m_spellInfo->rangeIndex == 1)
    {
        TerrainInfo const* map = unitTarget->GetTerrain();
        float distance = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));
        // For glyph of blink
        if (Player* modOwner = unitTarget->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, distance, this);

        // Start Info //
        float cx, cy, cz;
        float dx, dy, dz;
        float angle = unitTarget->GetOrientation();
        unitTarget->GetPosition(cx, cy, cz);

        float _dx = 0.5f * cos(angle);
        float _dy = 0.5f * sin(angle);
        dx = cx;
        dy = cy;
        std::list<G3D::Vector3> result;
        // Going foward 0.5f until max distance
        for (float i = 0.5f; i < distance; i += 0.5f)
        {
            dx += _dx;
            dy += _dy;
            MaNGOS::NormalizeMapCoord(dx);
            MaNGOS::NormalizeMapCoord(dy);
            dz = cz;
             
            // Prevent climbing and go around object maybe 2.0f is to small? use 3.0f?
            if (map->IsNextZcoordOK(dx, dy, dz, 3.0f) && unitTarget->IsWithinLOS(dx, dy, dz))
            {
                // No climb, the z differenze between this and prev step is ok. Store this destination for future use or check.
                cx = dx;
                cy = dy;
                unitTarget->UpdateGroundPositionZ(cx, cy, cz, 2.0f);
                bool ok = false;
                if (unitTarget->GetPathLength(cx, cy, cz, false, &ok) <= distance && ok)
                    result.push_back(G3D::Vector3(cx, cy, cz));
            }
        }

        if (result.size() > 0)
        {
            cx = result.back().x;
            cy = result.back().y;
            cz = result.back().z;
        }

        // Prevent Falling during swap building/outerspace
        unitTarget->UpdateGroundPositionZ(cx, cy, cz);

        if(unitTarget->GetTypeId() == TYPEID_PLAYER)
            ((Player*)unitTarget)->TeleportTo(map->GetMapId(), cx, cy, cz, unitTarget->GetOrientation(), TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET | (unitTarget==m_caster ? TELE_TO_SPELL : 0));
        else
            unitTarget->GetMap()->CreatureRelocation((Creature*)unitTarget, cx, cy, cz, unitTarget->GetOrientation());
    }
}

void Spell::EffectLeapBack(SpellEffectEntry const* effect)
{
    if (unitTarget->IsTaxiFlying())
        return;

    // Rocket Jump (Racial)
    if (m_spellInfo->Id == 69070)
        m_caster->KnockBackWithAngle(m_caster->GetOrientation(), float(effect->EffectMiscValue) / 10, float(damage) / 10);
    else
        m_caster->KnockBackFrom(unitTarget, float(effect->EffectMiscValue) / 10, float(damage) / 10);
}

void Spell::EffectReputation(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *_player = (Player*)unitTarget;

    int32  rep_change = m_currentBasePoints[effect->EffectIndex];
    uint32 faction_id = effect->EffectMiscValue;

    FactionEntry const* factionEntry = sFactionStore.LookupEntry(faction_id);

    if(!factionEntry)
        return;

    rep_change = _player->CalculateReputationGain(REPUTATION_SOURCE_SPELL, rep_change, faction_id);

    _player->GetReputationMgr().ModifyReputation(factionEntry, rep_change);
}

void Spell::EffectQuestComplete(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    // A few spells has additional value from basepoints, check condition here.
    switch(m_spellInfo->Id)
    {
        case 43458:                                         // Secrets of Nifflevar
        {
            if (!unitTarget->HasAura(effect->CalculateSimpleValue()))
                return;

            break;
        }
        // TODO: implement these!
        // "this spell awards credit for the entire raid (all spell targets as this is area target) if just ONE member has both auras (yes, both effect's basepoints)"
        //case 72155:                                         // Harvest Blight Specimen
        //case 72162:                                         // Harvest Blight Specimen
            //break;
        default:
            break;
    }

    uint32 quest_id = effect->EffectMiscValue;
    ((Player*)unitTarget)->AreaExploredOrEventHappens(quest_id);
}

void Spell::EffectSelfResurrect(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->isAlive())
        return;
    if(unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if(!unitTarget->IsInWorld())
        return;

    uint32 health = 0;
    uint32 mana = 0;

    // flat case
    if(damage < 0)
    {
        health = uint32(-damage);
        mana = effect->EffectMiscValue;
    }
    // percent case
    else
    {
        health = uint32(damage/100.0f*unitTarget->GetMaxHealth());
        if(unitTarget->GetMaxPower(POWER_MANA) > 0)
            mana = uint32(damage/100.0f*unitTarget->GetMaxPower(POWER_MANA));
    }

    Player *plr = ((Player*)unitTarget);
    plr->ResurrectPlayer(0.0f);

    plr->SetHealth( health );
    plr->SetPower(POWER_MANA, mana );
    plr->SetPower(POWER_RAGE, 0 );
    plr->SetPower(POWER_ENERGY, plr->GetMaxPower(POWER_ENERGY) );

    plr->SpawnCorpseBones();
}

void Spell::EffectSkinning(SpellEffectEntry const* /*effect*/)
{
    if(unitTarget->GetTypeId() != TYPEID_UNIT )
        return;
    if(!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Creature* creature = (Creature*) unitTarget;
    int32 targetLevel = creature->getLevel();

    uint32 skill = creature->GetCreatureInfo()->GetRequiredLootSkill();

    if (!creature->loot.empty())
        creature->loot.clear();

    ((Player*)m_caster)->SendLoot(creature->GetObjectGuid(),LOOT_SKINNING);
    creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);

    int32 reqValue = targetLevel < 10 ? 0 : targetLevel < 20 ? (targetLevel-10)*10 : targetLevel*5;

    int32 skillValue = ((Player*)m_caster)->GetPureSkillValue(skill);

    // Double chances for elites
    ((Player*)m_caster)->UpdateGatherSkill(skill, skillValue, reqValue, creature->IsElite() ? 2 : 1 );
}

void Spell::EffectCharge(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget)
        return;

    // TODO: research more ContactPoint/attack distance.
    // 3.666666 instead of ATTACK_DISTANCE(5.0f) in below seem to give more accurate result.
    float x, y, z;
    unitTarget->GetContactPoint(m_caster, x, y, z, CONTACT_DISTANCE);

    // Try to normalize Z coord cuz GetContactPoint do nothing with Z axis
    unitTarget->UpdateGroundPositionZ(x, y, z, 7.0f);

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        ((Creature *)unitTarget)->StopMoving();
    else
    {   // Delay attack, otherwise player makes instant attack after cast
        if (m_caster->GetAttackTime(BASE_ATTACK) < 300)
            m_caster->setAttackTimer(BASE_ATTACK, m_caster->GetAttackTime(BASE_ATTACK) + 20 * m_caster->GetDistance(unitTarget));
        if (m_caster->GetAttackTime(OFF_ATTACK) < 300)
            m_caster->setAttackTimer(OFF_ATTACK,  m_caster->GetAttackTime(OFF_ATTACK)  + 20 * m_caster->GetDistance(unitTarget));
    }

    // Only send MOVEMENTFLAG_WALK_MODE, client has strange issues with other move flags
    m_caster->MonsterMoveWithSpeed(x, y, z, 24.f, true, true);

    // not all charge effects used in negative spells
    if (unitTarget != m_caster && !IsPositiveSpell(m_spellInfo->Id))
        m_caster->Attack(unitTarget, true);
}

void Spell::EffectCharge2(SpellEffectEntry const* /*effect*/)
{
    float x, y, z;
    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        m_targets.getDestination(x, y, z);

        if (unitTarget->GetTypeId() != TYPEID_PLAYER)
            ((Creature *)unitTarget)->StopMoving();
    }
    else if (unitTarget && unitTarget != m_caster)
        unitTarget->GetContactPoint(m_caster, x, y, z, CONTACT_DISTANCE);
    else
        return;

    // Try to normalize Z coord cuz GetContactPoint do nothing with Z axis
    unitTarget->UpdateGroundPositionZ(x, y, z, 7.0f);

    // Only send MOVEMENTFLAG_WALK_MODE, client has strange issues with other move flags
    m_caster->MonsterMoveWithSpeed(x, y, z, 24.f, true, true);

    // not all charge effects used in negative spells
    if (unitTarget && unitTarget != m_caster && !IsPositiveSpell(m_spellInfo->Id))
        m_caster->Attack(unitTarget, true);
}

void Spell::DoSummonCritter(SpellEffectEntry const * effect, uint32 forceFaction)
{
    uint32 pet_entry = effect->EffectMiscValue;
    if(!pet_entry)
        return;

    CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(pet_entry);
    if (!cInfo)
    {
        sLog.outErrorDb("Spell::DoSummonCritter: creature entry %u not found for spell %u.", pet_entry, m_spellInfo->Id);
        return;
    }

    Pet* old_critter = m_caster->GetMiniPet();

    // for same pet just despawn (player unsummon command)
    if (m_caster->GetTypeId() == TYPEID_PLAYER && old_critter && old_critter->GetEntry() == pet_entry)
    {
        m_caster->RemoveMiniPet();
        return;
    }

    // despawn old pet before summon new
    if (old_critter)
        m_caster->RemoveMiniPet();

    CreatureCreatePos pos(m_caster->GetMap(), m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, m_caster->GetOrientation(), m_caster->GetPhaseMask());
    if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
        pos = CreatureCreatePos(m_caster, m_caster->GetOrientation());

    // summon new pet
    Pet* critter = new Pet(MINI_PET);

    Map *map = m_caster->GetMap();
    uint32 pet_number = sObjectMgr.GeneratePetNumber();
    if (!critter->Create(map->GenerateLocalLowGuid(HIGHGUID_PET), pos, cInfo, pet_number))
    {
        ERROR_LOG("Spell::EffectSummonCritter, spellid %u: no such creature entry %u", m_spellInfo->Id, pet_entry);
        delete critter;
        return;
    }

    critter->SetRespawnCoord(pos);

    //critter->SetName("");                                 // generated by client
    critter->SetOwnerGuid(m_caster->GetObjectGuid());
    critter->SetCreatorGuid(m_caster->GetObjectGuid());

    critter->SetUInt32Value(UNIT_CREATED_BY_SPELL, m_spellInfo->Id);
    critter->setFaction(forceFaction ? forceFaction : m_caster->getFaction());
    critter->AIM_Initialize();
    critter->InitPetCreateSpells();                         // e.g. disgusting oozeling has a create spell as critter...
    //critter->InitLevelupSpellsForLevel();                 // none?
    critter->SelectLevel(critter->GetCreatureInfo());       // some summoned creaters have different from 1 DB data for level/hp
    critter->SetUInt32Value(UNIT_NPC_FLAGS, critter->GetCreatureInfo()->npcflag);
                                                            // some mini-pets have quests

    // set timer for unsummon
    if(m_duration > 0)
        critter->SetDuration(m_duration);

    m_caster->SetMiniPet(critter);

    map->Add((Creature*)critter);

    // Notify Summoner
    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(critter);
    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
        ((Creature*)m_originalCaster)->AI()->JustSummoned(critter);
}

void Spell::EffectKnockBack(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    if (unitTarget->hasUnitState(UNIT_STAT_ROOT))
        return;

    // Typhoon
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x100000000000000)))
        if (m_caster->HasAura(62135))         // Glyph of Typhoon
            return;

    // Thunderstorm
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_SHAMAN, UI64LIT(0x00200000000000)))
        if (m_caster->HasAura(62132))         // Glyph of Thunderstorm
            return;

    // Instantly interrupt non melee spells being casted
    if (unitTarget->IsNonMeleeSpellCasted(true))
        unitTarget->InterruptNonMeleeSpells(true);

    if (effect->Effect == SPELL_EFFECT_KNOCK_BACK_DEST)
    {
        if ((m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) == 0)
            return;
        float x, y;
        x = m_targets.m_destX;
        y = m_targets.m_destY;

        float angle = unitTarget->GetAngle(x, y);
        unitTarget->KnockBackWithAngle(angle, float(effect->EffectMiscValue) / 10, float(damage) / 10);
    }
    else  //if (m_spellInfo->Effect[i] == SPELL_EFFECT_KNOCK_BACK)
        unitTarget->KnockBackFrom(m_caster, float(effect->EffectMiscValue) / 10, float(damage) / 10);
}

void Spell::EffectSendTaxi(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->ActivateTaxiPathTo(effect->EffectMiscValue, m_spellInfo->Id);
}

void Spell::EffectPullTowards(SpellEffectEntry const * effect)
{
    if(!unitTarget)
        return;

    float height = damage / 10.0f;
    float speedXY = effect->EffectMiscValue / 10;
    unitTarget->MonsterMoveJump(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), speedXY, height);
}

void Spell::EffectSuspendGravity(SpellEffectEntry const * effect)
{
    if (!unitTarget)
        return;

    float x,y,z;

    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        x = m_targets.m_destX;
        y = m_targets.m_destY;
        z = m_targets.m_destZ;
    }
    else
        m_caster->GetClosePoint(x, y, z, m_caster->GetObjectBoundingRadius(), 0.0f, m_caster->GetAngle(unitTarget));

    unitTarget->UpdateAllowedPositionZ(x, y, z);

    float speed  = effect->EffectMiscValue / 2.0f;
    float height = unitTarget->GetDistance(x,y,z) / 10.0f;

    unitTarget->MonsterMoveJump(x, y, z + 0.1f, speed, height);
}

void Spell::EffectDispelMechanic(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    int32 count = damage;

    if (!count && m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_GENERIC)
        count = 1;

    uint32 mechanic = effect->EffectMiscValue;

    std::set<uint32> toRemoveSpellList;
    std::set<uint32> failSpellList;
    Unit::SpellAuraHolderMap& Auras = unitTarget->GetSpellAuraHolderMap();
    for(Unit::SpellAuraHolderMap::iterator iter = Auras.begin(); iter != Auras.end() && count > 0; ++iter)
    {
        SpellAuraHolder* holder = iter->second;
        if (holder->HasMechanic(mechanic))
        {
            bool success = false;
            GetDispelChance(holder->GetCaster(), unitTarget, holder->GetId(), !unitTarget->IsFriendlyTo(m_caster), &success);

            if (success)
                toRemoveSpellList.insert(holder->GetId());
            else
                failSpellList.insert(holder->GetId());
            --count;
        }
    }

    if (!toRemoveSpellList.empty())
    {
        uint32 count = toRemoveSpellList.size();

        WorldPacket data(SMSG_SPELLDISPELLOG, 8 + 8 + 4 + 1 + 4 + 5 * count);
        data << unitTarget->GetPackGUID();              // Victim GUID
        data << m_caster->GetPackGUID();                // Caster GUID
        data << uint32(m_spellInfo->Id);                // Dispel spell id
        data << uint8(0);                               // not used
        data << uint32(count);                          // count

        for (std::set<uint32>::iterator itr = toRemoveSpellList.begin(); itr != toRemoveSpellList.end(); ++itr)
        {
            data << uint32(*itr);                       // Spell Id
            data << uint8(0);                           // 0 - dispelled !=0 cleansed

            unitTarget->RemoveAurasDueToSpell(*itr, NULL, AURA_REMOVE_BY_DISPEL);
        }

        m_caster->SendMessageToSet(&data, true);
    }

    if (!failSpellList.empty())
    {
        WorldPacket data(SMSG_DISPEL_FAILED, 8+8+4+4*failSpellList.size());
        data << m_caster->GetObjectGuid();              // Caster GUID
        data << unitTarget->GetObjectGuid();            // Victim GUID
        data << uint32(m_spellInfo->Id);                // Dispel spell id
        for (std::set<uint32>::iterator itr = failSpellList.begin(); itr != failSpellList.end(); ++itr)
            data << uint32(*itr);                       // Spell Id

        m_caster->SendMessageToSet(&data, true);
    }
}

void Spell::EffectSummonDeadPet(SpellEffectEntry const* /*effect*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;
    Player* _player = (Player*)m_caster;
    Pet* pet = _player->GetPet();
    if (!pet)
        return;
    if (pet->isAlive())
        return;
    if (damage < 0)
        return;

    pet->SetUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_NONE);
    pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE);
    pet->SetDeathState( ALIVE );
    pet->clearUnitState(UNIT_STAT_ALL_STATE);
    pet->SetHealth( uint32(pet->GetMaxHealth()*(float(damage)/100)));

    pet->AIM_Initialize();

    // _player->PetSpellInitialize(); -- action bar not removed at death and not required send at revive
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
}

void Spell::EffectSummonAllTotems(SpellEffectEntry const* effect)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 start_button = ACTION_BUTTON_SHAMAN_TOTEMS_BAR + effect->EffectMiscValue;
    int32 amount_buttons = effect->EffectMiscValueB;

    for(int32 slot = 0; slot < amount_buttons; ++slot)
        if (ActionButton const* actionButton = ((Player*)m_caster)->GetActionButton(start_button+slot))
            if (actionButton->GetType()==ACTION_BUTTON_SPELL)
                if (uint32 spell_id = actionButton->GetAction())
                    if (((Player*)m_caster)->HasSpell(spell_id) && !((Player*)m_caster)->HasSpellCooldown(spell_id))
                        m_caster->CastSpell(unitTarget,spell_id,true);
}

void Spell::EffectDestroyAllTotems(SpellEffectEntry const* /*effect*/)
{
    int32 mana = 0;
    for(int slot = 0;  slot < MAX_TOTEM_SLOT; ++slot)
    {
        if (Totem* totem = m_caster->GetTotem(TotemSlot(slot)))
        {
            if (damage)
            {
                uint32 spell_id = totem->GetUInt32Value(UNIT_CREATED_BY_SPELL);
                if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(spell_id))
                {
                    uint32 manacost = m_caster->GetCreateMana() * spellInfo->GetManaCostPercentage() / 100;
                    mana += manacost * damage / 100;
                }
            }
            totem->UnSummon();
        }
    }

    if (mana)
        m_caster->CastCustomSpell(m_caster, 39104, &mana, NULL, NULL, true);
}

void Spell::EffectBreakPlayerTargeting (SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget)
        return;

    WorldPacket data(SMSG_CLEAR_TARGET, 8);
    data << unitTarget->GetObjectGuid();
    unitTarget->SendMessageToSet(&data, false);
}

void Spell::EffectDurabilityDamage(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = effect->EffectMiscValue;

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if(slot < 0)
    {
        ((Player*)unitTarget)->DurabilityPointsLossAll(damage, (slot < -1));
        return;
    }

    // invalid slot value
    if(slot >= INVENTORY_SLOT_BAG_END)
        return;

    if(Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        ((Player*)unitTarget)->DurabilityPointsLoss(item, damage);
}

void Spell::EffectDurabilityDamagePCT(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    int32 slot = effect->EffectMiscValue;

    // FIXME: some spells effects have value -1/-2
    // Possibly its mean -1 all player equipped items and -2 all items
    if(slot < 0)
    {
        ((Player*)unitTarget)->DurabilityLossAll(double(damage)/100.0f, (slot < -1), false);
        return;
    }

    // invalid slot value
    if(slot >= INVENTORY_SLOT_BAG_END)
        return;

    if(damage <= 0)
        return;

    if(Item* item = ((Player*)unitTarget)->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
        ((Player*)unitTarget)->DurabilityLoss(item, double(damage)/100.0f);
}

void Spell::EffectModifyThreatPercent(SpellEffectEntry const* /*effect*/)
{
    if(!unitTarget)
        return;

    unitTarget->getThreatManager().modifyThreatPercent(m_caster, damage);
}

void Spell::EffectTransmitted(SpellEffectEntry const* effect)
{
    uint32 name_id = effect->EffectMiscValue;

    GameObjectInfo const* goinfo = ObjectMgr::GetGameObjectInfo(name_id);

    if (!goinfo)
    {
        sLog.outErrorDb("Gameobject (Entry: %u) not exist and not created at spell (ID: %u) cast",name_id, m_spellInfo->Id);
        return;
    }

    float fx, fy, fz;

    // Place seaforium charges
    if (m_spellInfo->Id == 52410 || m_spellInfo->Id == 66268 || m_spellInfo->Id == 66674)
    {
        fx = m_caster->GetPositionX();
        fy = m_caster->GetPositionY();
        fz = m_caster->GetPositionZ();
    }
    else if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
        m_targets.getDestination(fx, fy, fz);
    // FIXME: this can be better check for most objects but still hack
    else if (effect->GetRadiusIndex() && m_spellInfo->speed == 0)
    {
        float dis = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));
        m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }
    else
    {
        float min_dis = GetSpellMinRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float max_dis = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
        float dis = rand_norm_f() * (max_dis - min_dis) + min_dis;

        // special code for fishing bobber (TARGET_SELF_FISHING), should not try to avoid objects
        // nor try to find ground level, but randomly vary in angle
        if (goinfo->type == GAMEOBJECT_TYPE_FISHINGNODE)
        {
            // calculate angle variation for roughly equal dimensions of target area
            float max_angle = (max_dis - min_dis)/(max_dis + m_caster->GetObjectBoundingRadius());
            float angle_offset = max_angle * (rand_norm_f() - 0.5f);
            m_caster->GetNearPoint2D(fx, fy, dis, m_caster->GetOrientation() + angle_offset);

            GridMapLiquidData liqData;
            if (!m_caster->GetTerrain()->IsInWater(fx, fy, m_caster->GetPositionZ() + 1.f, &liqData))
            {
                SendCastResult(SPELL_FAILED_NOT_FISHABLE);
                SendChannelUpdate(0);
                return;
            }

            fz = liqData.level;
            // finally, check LoS
            if (!m_caster->IsWithinLOS(fx, fy, fz))
            {
                SendCastResult(SPELL_FAILED_LINE_OF_SIGHT);
                SendChannelUpdate(0);
                return;
            }
        }
        else
            m_caster->GetClosePoint(fx, fy, fz, DEFAULT_WORLD_OBJECT_SIZE, dis);
    }

    Map* cMap = m_caster->GetMap();

    // if gameobject is summoning object, it should be spawned right on caster's position
    if (goinfo->type == GAMEOBJECT_TYPE_SUMMONING_RITUAL)
    {
        m_caster->GetPosition(fx, fy, fz);
    }

    GameObject* pGameObj = new GameObject;

    if(!pGameObj->Create(cMap->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), name_id, cMap,
        m_caster->GetPhaseMask(), fx, fy, fz, m_caster->GetOrientation()))
    {
        delete pGameObj;
        return;
    }

    int32 duration = m_duration;

    switch(goinfo->type)
    {
        case GAMEOBJECT_TYPE_FISHINGNODE:
        {
            m_caster->SetChannelObjectGuid(pGameObj->GetObjectGuid());
            m_caster->AddGameObject(pGameObj);              // will removed at spell cancel

            // end time of range when possible catch fish (FISHING_BOBBER_READY_TIME..GetDuration(m_spellInfo))
            // start time == fish-FISHING_BOBBER_READY_TIME (0..GetDuration(m_spellInfo)-FISHING_BOBBER_READY_TIME)
            int32 lastSec = 0;
            switch(urand(0, 3))
            {
                case 0: lastSec =  3; break;
                case 1: lastSec =  7; break;
                case 2: lastSec = 13; break;
                case 3: lastSec = 17; break;
            }

            duration = duration - lastSec*IN_MILLISECONDS + FISHING_BOBBER_READY_TIME*IN_MILLISECONDS;
            break;
        }
        case GAMEOBJECT_TYPE_SUMMONING_RITUAL:
        {
            if(m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                pGameObj->AddUniqueUse((Player*)m_caster);
                m_caster->AddGameObject(pGameObj);          // will removed at spell cancel
            }
            break;
        }
        case GAMEOBJECT_TYPE_FISHINGHOLE:
        case GAMEOBJECT_TYPE_CHEST:
        default:
            break;
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration/IN_MILLISECONDS : 0);

    pGameObj->SetOwnerGuid(m_caster->GetObjectGuid());

    pGameObj->SetUInt32Value(GAMEOBJECT_LEVEL, m_caster->getLevel());
    pGameObj->SetSpellId(m_spellInfo->Id);

    DEBUG_LOG("AddObject at SpellEfects.cpp EffectTransmitted");
    //m_caster->AddGameObject(pGameObj);
    //m_ObjToDel.push_back(pGameObj);

    cMap->Add(pGameObj);

    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);

    if (m_spellInfo->Id == 50317)
        pGameObj->PlayDistanceSound(12924); // Disco Ball Music

    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
        ((Creature*)m_originalCaster)->AI()->JustSummoned(pGameObj);
}

void Spell::EffectProspecting(SpellEffectEntry const* /*effect*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER || !itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    if (sWorld.getConfig(CONFIG_BOOL_SKILL_PROSPECTING))
    {
        uint32 SkillValue = p_caster->GetPureSkillValue(SKILL_JEWELCRAFTING);
        uint32 reqSkillValue = itemTarget->GetProto()->RequiredSkillRank;
        p_caster->UpdateGatherSkill(SKILL_JEWELCRAFTING, SkillValue, reqSkillValue);
    }

    ((Player*)m_caster)->SendLoot(itemTarget->GetObjectGuid(), LOOT_PROSPECTING);
}

void Spell::EffectMilling(SpellEffectEntry const* /*effect*/)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER || !itemTarget)
        return;

    Player* p_caster = (Player*)m_caster;

    if( sWorld.getConfig(CONFIG_BOOL_SKILL_MILLING))
    {
        uint32 SkillValue = p_caster->GetPureSkillValue(SKILL_INSCRIPTION);
        uint32 reqSkillValue = itemTarget->GetProto()->RequiredSkillRank;
        p_caster->UpdateGatherSkill(SKILL_INSCRIPTION, SkillValue, reqSkillValue);
    }

    ((Player*)m_caster)->SendLoot(itemTarget->GetObjectGuid(), LOOT_MILLING);
}

void Spell::EffectSkill(SpellEffectEntry const* /*effect*/)
{
    DEBUG_LOG("WORLD: SkillEFFECT");
}

void Spell::EffectSpiritHeal(SpellEffectEntry const* /*effect*/)
{
    // TODO player can't see the heal-animation - he should respawn some ticks later
    if (!unitTarget || unitTarget->isAlive())
        return;
    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;
    if (!unitTarget->IsInWorld())
        return;
    if (m_spellInfo->Id == 22012 && !unitTarget->HasAura(2584))
        return;

    ((Player*)unitTarget)->ResurrectPlayer(1.0f);
    ((Player*)unitTarget)->SpawnCorpseBones();
}

// remove insignia spell effect
void Spell::EffectSkinPlayerCorpse(SpellEffectEntry const* /*effect*/)
{
    DEBUG_LOG("Effect: SkinPlayerCorpse");
    if ( (m_caster->GetTypeId() != TYPEID_PLAYER) || (unitTarget->GetTypeId() != TYPEID_PLAYER) || (unitTarget->isAlive()) )
        return;

    ((Player*)unitTarget)->RemovedInsignia( (Player*)m_caster );
}

void Spell::EffectStealBeneficialBuff(SpellEffectEntry const* effect)
{
    DEBUG_LOG("Effect: StealBeneficialBuff");

    if(!unitTarget || unitTarget==m_caster)                 // can't steal from self
        return;

    typedef std::vector<SpellAuraHolder*> StealList;
    StealList steal_list;
    // Create dispel mask by dispel type
    uint32 dispelMask  = GetDispelMask( DispelType(effect->EffectMiscValue) );
    Unit::SpellAuraHolderMap const& auras = unitTarget->GetSpellAuraHolderMap();
    for(Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
    {
        SpellAuraHolder *holder = itr->second;
        if (holder && (1<<holder->GetSpellProto()->GetDispel()) & dispelMask)
        {
            // Need check for passive? this
            if (holder->IsPositive() && !holder->IsPassive() && !holder->GetSpellProto()->HasAttribute(SPELL_ATTR_EX4_NOT_STEALABLE))
                steal_list.push_back(holder);
        }
    }
    // Ok if exist some buffs for dispel try dispel it
    if (!steal_list.empty())
    {
        typedef std::list < std::pair<uint32, ObjectGuid> > SuccessList;
        SuccessList success_list;
        typedef std::list < uint32 > FailList;
        FailList fail_list;
        // Dispell N = damage buffs (or while exist buffs for dispel)
        for (int32 count=0; count < damage && !steal_list.empty(); ++count)
        {
            // Random select buff for dispel
            std::vector<SpellAuraHolder*>::iterator steal_itr = steal_list.begin();
            std::advance(steal_itr,urand(0, steal_list.size()-1));

            SpellAuraHolder *holder = *steal_itr;
            // remove entry from steal_list
            steal_list.erase(steal_itr);

            SpellEntry const* spellInfo = holder->GetSpellProto();
            bool success = false;

            // Try dispel
            if (!GetDispelChance(holder->GetCaster(), unitTarget, holder->GetId(), !unitTarget->IsFriendlyTo(m_caster), &success))
                fail_list.push_back(spellInfo->Id);
            else if (success)
                success_list.push_back(SuccessList::value_type(holder->GetId(), holder->GetCasterGuid()));
            else
                fail_list.push_back(spellInfo->Id);
        }
        // Really try steal and send log
        if (!success_list.empty())
        {
            int32 count = success_list.size();
            WorldPacket data(SMSG_SPELLSTEALLOG, 8+8+4+1+4+count*5);
            data << unitTarget->GetPackGUID();       // Victim GUID
            data << m_caster->GetPackGUID();         // Caster GUID
            data << uint32(m_spellInfo->Id);         // Dispell spell id
            data << uint8(0);                        // not used
            data << uint32(count);                   // count
            for (SuccessList::iterator j = success_list.begin(); j != success_list.end(); ++j)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(j->first);
                data << uint32(spellInfo->Id);       // Spell Id
                data << uint8(0);                    // 0 - steals !=0 transfers
                unitTarget->RemoveAurasDueToSpellBySteal(spellInfo->Id, j->second, m_caster);
            }
            m_caster->SendMessageToSet(&data, true);
        }
        // Send fail log to client
        if (!fail_list.empty())
        {
            // Failed to steal
            WorldPacket data(SMSG_DISPEL_FAILED, 8+8+4+4*fail_list.size());
            data << m_caster->GetObjectGuid();              // Caster GUID
            data << unitTarget->GetObjectGuid();            // Victim GUID
            data << uint32(m_spellInfo->Id);                // Steal spell id
            for (FailList::iterator j = fail_list.begin(); j != fail_list.end(); ++j)
                data << uint32(*j);                         // Spell Id
            m_caster->SendMessageToSet(&data, true);
        }
    }
}

void Spell::EffectKillCreditPersonal(SpellEffectEntry const* effect)
{
    if(!unitTarget)
        return;

    Player* player = NULL;

    if (unitTarget->GetTypeId() != TYPEID_PLAYER)
    {
        if (unitTarget->GetObjectGuid().IsVehicle())
            if (Unit* unit = unitTarget->GetVehicleKit()->GetPassenger(0))
                if (unit->GetTypeId() == TYPEID_PLAYER)
                    player = (Player*)unit;
    }

    if (!player)
        if (unitTarget->GetTypeId() == TYPEID_PLAYER)
            player = (Player*)unitTarget;
        else 
            return;

    player->KilledMonsterCredit(effect->EffectMiscValue);
}

void Spell::EffectKillCreditGroup(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->RewardPlayerAndGroupAtEvent(effect->EffectMiscValue, unitTarget);
}

void Spell::EffectQuestFail(SpellEffectEntry const* effect)
{
    if(!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->FailQuest(effect->EffectMiscValue);
}

void Spell::EffectActivateRune(SpellEffectEntry const* effect)
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player *plr = (Player*)m_caster;

    if (plr->getClass() != CLASS_DEATH_KNIGHT)
        return;

    int32 count = damage;           // max amount of reset runes
    if (!count)
        return;

    m_runesState = plr->GetRunesState();

    for (uint32 j = 0; j < MAX_RUNES && count > 0; ++j)
    {
        if (plr->GetRuneCooldown(j))
        {
            // Energize Blood, Frost, Unholy Rune
            // these spells activate fully depleted runes depending on base rune
            if (m_spellInfo->Id == 81166 || m_spellInfo->Id == 81168 || m_spellInfo->Id == 81169)
            {
                if (plr->GetBaseRune(j) != RuneType(effect->EffectMiscValue))
                    continue;

                if (plr->GetRuneCooldown(j) != plr->GetBaseRuneCooldown(j))
                    continue;
            }
            else if (plr->GetCurrentRune(j) != RuneType(effect->EffectMiscValue))
                continue;

            // Blood Tap
            if (m_spellInfo->Id == 45529)
                if (plr->GetBaseRune(j) != RuneType(effect->EffectMiscValueB))
                    continue;
            plr->SetRuneCooldown(j, 0);
            --count;
        }
    }

    // Blood Tap
    if (m_spellInfo->Id == 45529 && count > 0)
    {
        for (uint32 l = 0; l < MAX_RUNES && count > 0; ++l)
        {
            // Check if both runes are on cd as that is the only time when this needs to come into effect
            if ((plr->GetRuneCooldown(l) && plr->GetCurrentRune(l) == RuneType(effect->EffectMiscValueB)) && (plr->GetRuneCooldown(l + 1) && plr->GetCurrentRune(l+1) == RuneType(effect->EffectMiscValueB)))
            {
                // Should always update the rune with the lowest cd
                if (plr->GetRuneCooldown(l) >= plr->GetRuneCooldown(l+1))
                    ++l;
                plr->SetRuneCooldown(l, 0);
                --count;
            }
            else
                break;
        }
    }

    plr->ResyncRunes();
}

void Spell::EffectTitanGrip(SpellEffectEntry const* effect)
{
    // Make sure "Titan's Grip" (49152) penalty spell does not silently change
    if (effect->EffectMiscValue != 49152)
        sLog.outError("Spell::EffectTitanGrip: Spell %u has unexpected EffectMiscValue '%u'", m_spellInfo->Id, effect->EffectMiscValue);
    if (unitTarget && unitTarget->GetTypeId() == TYPEID_PLAYER)
    {
        Player *plr = (Player*)m_caster;
        plr->SetCanTitanGrip(true);
        if (plr->HasTwoHandWeaponInOneHand() && !plr->HasAura(49152))
            plr->CastSpell(plr, 49152, true);
    }
}

void Spell::EffectRenamePet(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_UNIT ||
        !((Creature*)unitTarget)->IsPet() || ((Pet*)unitTarget)->getPetType() != HUNTER_PET)
        return;

    unitTarget->RemoveByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED);
}

void Spell::EffectPlaySound(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 soundId = effect->EffectMiscValue;
    if (!sSoundEntriesStore.LookupEntry(soundId))
    {
        sLog.outError("EffectPlaySound: Sound (Id: %u) in spell %u does not exist.", soundId, m_spellInfo->Id);
        return;
    }

    unitTarget->PlayDirectSound(soundId, (Player*)unitTarget);
}

void Spell::EffectPlayMusic(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 soundId = effect->EffectMiscValue;
    if (!sSoundEntriesStore.LookupEntry(soundId))
    {
        ERROR_LOG("EffectPlayMusic: Sound (Id: %u) in spell %u does not exist.", soundId, m_spellInfo->Id);
        return;
    }

    WorldPacket data(SMSG_PLAY_MUSIC, 4);
    data << uint32(soundId);
    data << ObjectGuid();
    ((Player*)unitTarget)->GetSession()->SendPacket(&data);
}

void Spell::EffectSpecCount(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->UpdateSpecCount(damage);
}

void Spell::EffectActivateSpec(SpellEffectEntry const* /*effect*/)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 spec = damage-1;

    ((Player*)unitTarget)->ActivateSpec(spec);
}

void Spell::EffectBind(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    uint32 area_id;
    WorldLocation loc;
    if (effect->EffectImplicitTargetA == TARGET_TABLE_X_Y_Z_COORDINATES ||
        effect->EffectImplicitTargetB == TARGET_TABLE_X_Y_Z_COORDINATES)
    {
        SpellTargetPosition const* st = sSpellMgr.GetSpellTargetPosition(m_spellInfo->Id);
        if (!st)
        {
            ERROR_LOG( "Spell::EffectBind - unknown Teleport coordinates for spell ID %u", m_spellInfo->Id );
            return;
        }

        loc.mapid       = st->target_mapId;
        loc.coord_x     = st->target_X;
        loc.coord_y     = st->target_Y;
        loc.coord_z     = st->target_Z;
        loc.orientation = st->target_Orientation;
        area_id = sTerrainMgr.GetAreaId(loc.mapid, loc.coord_x, loc.coord_y, loc.coord_z);
    }
    else
    {
        player->GetPosition(loc);
        area_id = player->GetAreaId();
    }

    player->SetHomebindToLocation(loc,area_id);

    // binding
    WorldPacket data( SMSG_BINDPOINTUPDATE, (4+4+4+4+4) );
    data << float(loc.coord_x);
    data << float(loc.coord_y);
    data << float(loc.coord_z);
    data << uint32(loc.mapid);
    data << uint32(area_id);
    player->SendDirectMessage( &data );

    DEBUG_LOG("New Home Position X is %f", loc.coord_x);
    DEBUG_LOG("New Home Position Y is %f", loc.coord_y);
    DEBUG_LOG("New Home Position Z is %f", loc.coord_z);
    DEBUG_LOG("New Home MapId is %u", loc.mapid);
    DEBUG_LOG("New Home AreaId is %u", area_id);

    // zone update
    data.Initialize(SMSG_PLAYERBOUND, 8+4);
    data << player->GetObjectGuid();
    data << uint32(area_id);
    player->SendDirectMessage( &data );
}

void Spell::EffectRestoreItemCharges(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(effect->EffectItemType);
    if (!itemProto)
        return;

    // In case item from limited category recharge any from category, is this valid checked early in spell checks
    Item* item;
    if (itemProto->ItemLimitCategory)
        item = player->GetItemByLimitedCategory(itemProto->ItemLimitCategory);
    else
        item = player->GetItemByEntry(effect->EffectItemType);

    if (!item)
        return;

    item->RestoreCharges();
}

void Spell::EffectRedirectThreat(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    // Misdirection with Glyph of Misdirection
    if (m_spellInfo->Id == 34477 && m_caster->GetTypeId() == TYPEID_PLAYER)
        if (unitTarget->GetObjectGuid().IsPet() && unitTarget->GetOwnerGuid() == m_caster->GetObjectGuid() && m_caster->HasAura(56829))
            ((Player*)m_caster)->RemoveSpellCooldown(m_spellInfo->Id, true);

    m_caster->getHostileRefManager().SetThreatRedirection(unitTarget->GetObjectGuid(), uint32(damage));
}

void Spell::EffectTeachTaxiNode(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* player = (Player*)unitTarget;

    uint32 taxiNodeId = effect->EffectMiscValue;
    if (!sTaxiNodesStore.LookupEntry(taxiNodeId))
        return;

    if (player->m_taxi.SetTaximaskNode(taxiNodeId))
    {
        WorldPacket data(SMSG_NEW_TAXI_PATH, 0);
        player->SendDirectMessage( &data );

        data.Initialize( SMSG_TAXINODE_STATUS, 9 );
        data << m_caster->GetObjectGuid();
        data << uint8( 1 );
        player->SendDirectMessage( &data );
    }
}

void Spell::EffectQuestOffer(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (Quest const* quest = sObjectMgr.GetQuestTemplate(effect->EffectMiscValue))
    {
        Player* player = (Player*)unitTarget;

        if (player->CanTakeQuest(quest, false))
            player->PlayerTalkClass->SendQuestGiverQuestDetails(quest, player->GetObjectGuid(), true);
    }
}

void Spell::EffectCancelAura(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    uint32 spellId = effect->EffectTriggerSpell;

    if (!sSpellStore.LookupEntry(spellId))
    {
        sLog.outError("Spell::EffectCancelAura: spell %u doesn't exist", spellId);
        return;
    }

    unitTarget->RemoveAurasDueToSpell(spellId);
}

void Spell::EffectWMODamage(SpellEffectEntry const* effect)
{
    if (!gameObjTarget)
    {
        DEBUG_LOG("Spell::EffectWMODamage called, but no valid targets. Spell ID %u", m_spellInfo->Id);
        return;
    }

    if (gameObjTarget->GetGoType() != GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        DEBUG_LOG("Spell::EffectWMODamage called, but no valid targets. Spell ID %u, gameObjTarget: %s", m_spellInfo->Id, gameObjTarget->GetGuidStr().c_str());
        return;
    }

    if (!gameObjTarget->GetHealth())
    {
        DEBUG_LOG("Spell::EffectWMODamage called, but no valid targets. Spell ID %u, target %s is destroyed", m_spellInfo->Id, gameObjTarget->GetGuidStr().c_str());
        return;
    }

    Unit *caster = m_originalCaster;

    if (!caster)
    {
        DEBUG_LOG("Spell::EffectWMODamage called, but no original caster. Spell ID %u", m_spellInfo->Id);
        return;
    }

    Player* pWho = NULL;
    if (caster->GetTypeId() == TYPEID_PLAYER)
        pWho = (Player*)caster;
    else if (((Creature*)caster)->GetVehicleKit())
        pWho = caster->GetCharmerOrOwnerPlayerOrPlayerItself();

    if (OutdoorPvP* opvp = sOutdoorPvPMgr.GetScript(pWho ? pWho->GetCachedZoneId() : caster->GetZoneId()))
        if (!opvp->CanDamageGO(gameObjTarget, pWho))
            return;

    if (pWho)
    {
        if (BattleGround* bg = pWho->GetBattleGround())
            if (!bg->CanDamageGO(gameObjTarget, pWho))
                return;

        float mod = 1.0f;
        Unit::AuraList const& mModDamagePercentDone = pWho->GetAurasByType(SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
        for (Unit::AuraList::const_iterator i = mModDamagePercentDone.begin(); i != mModDamagePercentDone.end(); ++i)
        {
            SpellEquippedItemsEntry const* spellEquip = (*i)->GetSpellProto()->GetSpellEquippedItems();
            if (((*i)->GetModifier()->m_miscvalue & GetSpellSchoolMask(m_spellInfo)) &&
                (!spellEquip || spellEquip->EquippedItemClass == -1 &&
                                                                // -1 == any item class (not wand then)
                spellEquip->EquippedItemInventoryTypeMask == 0))
                                                                // 0 == any inventory type (not wand then)
            {
                mod *= ((*i)->GetModifier()->m_amount + 100.0f) / 100.0f;
            }
        }
        damage *= mod;
    }

    DEBUG_LOG( "Spell::EffectWMODamage, spell ID %u, object %s, damage %u", m_spellInfo->Id, gameObjTarget->GetGuidStr().c_str(), uint32(damage));

    gameObjTarget->DamageTaken(caster, uint32(damage), m_spellInfo->Id);

    WorldPacket data(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, 8+8+8+4+4);
    data << gameObjTarget->GetPackGUID();
    data << caster->GetPackGUID();

    if (Unit* who = caster->GetCharmerOrOwner()) //check for pet / vehicle
        data << who->GetPackGUID();
    else
        data << caster->GetPackGUID();

    data << uint32(damage);
    data << uint32(m_spellInfo->Id);

    gameObjTarget->SendMessageToSet(&data, false);
}

void Spell::EffectWMORepair(SpellEffectEntry const* effect)
{
    if (gameObjTarget && gameObjTarget->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        DEBUG_LOG( "Spell::EffectWMORepair,  spell ID %u, object %u", m_spellInfo->Id,gameObjTarget->GetEntry());
        gameObjTarget->Rebuild(m_caster);
    }
    else
        DEBUG_LOG( "Spell::EffectWMORepair called, but no valid targets. Spell ID %u", m_spellInfo->Id);
}

void Spell::EffectWMOChange(SpellEffectEntry const* effect)
{
    if (gameObjTarget && gameObjTarget->GetGoType() == GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING)
    {
        DEBUG_LOG( "Spell::EffectWMOChange,  spell ID %u, object %u, command %u", m_spellInfo->Id, gameObjTarget->GetEntry(), effect->EffectMiscValue);

        Unit* caster = m_originalCaster;

        if (!caster)
            return;

        switch (effect->EffectMiscValue)
        {
            case 0: // intact
                if (gameObjTarget->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED))
                    gameObjTarget->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
                if (gameObjTarget->HasFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED))
                    gameObjTarget->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                break;
            case 1: // damaged
                gameObjTarget->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DAMAGED);
                break;
            case 2: // destroyed
                gameObjTarget->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
                break;
            case 3: // rebuild
                gameObjTarget->Rebuild(caster);
                break;
            default:
                DEBUG_LOG( "Spell::EffectWMOChange,  spell ID %u with defined change value %u", m_spellInfo->Id, effect->EffectMiscValue);
                break;
        }
    }
    else
        DEBUG_LOG( "Spell::EffectWMORepair called, but no valid targets. Spell ID %u", m_spellInfo->Id);
}

void Spell::EffectFriendSummon(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (((Player*)m_caster)->GetSelectionGuid().IsEmpty() || !((Player*)m_caster)->GetSelectionGuid().IsPlayer())
    {
        DEBUG_LOG( "Spell::EffectFriendSummon is called, but no selection or selection is not player");
        return;
    }

    DEBUG_LOG("Spell::EffectFriendSummon called for player %u", ((Player*)m_caster)->GetSelectionGuid().GetCounter());

    m_caster->CastSpell(m_caster, effect->EffectTriggerSpell, true);
}

void Spell::EffectRewardCurrency(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)unitTarget)->ModifyCurrencyCount(effect->EffectMiscValue, damage);
}

void Spell::EffectDamageFromMaxHealthPct(SpellEffectEntry const* effect)
{
    if (!unitTarget)
        return;

    m_damage += int32(unitTarget->GetMaxHealth() * damage / 100.0f);
}

void Spell::EffectResurrectWithAura(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    if (unitTarget->isAlive() || !unitTarget->IsInWorld())
        return;

    Player* pTarget = (Player*)unitTarget;

    if (pTarget->isRessurectRequested())       // already have one active request
        return;

    float healthPct = damage / 100.0f;
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (((Player*)m_caster)->GetGuildId() == pTarget->GetGuildId())
            healthPct *= m_caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_RESURRECTED_HEALTH_BY_GUILD_MEMBER);
    }
    uint32 health = uint32(pTarget->GetMaxHealth() * std::min(healthPct, 1.0f));
    uint32 mana   = pTarget->GetMaxPower(POWER_MANA) * std::min(healthPct, 1.0f);

    uint32 resurrectAuraSpell = effect->EffectTriggerSpell;
    SpellEntry const* spellInfo = sSpellStore.LookupEntry(resurrectAuraSpell);
    if (spellInfo && unitTarget->HasAura(resurrectAuraSpell))
        return;

    pTarget->setResurrectRequestData(m_caster->GetObjectGuid(), m_caster->GetMapId(), m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), health, mana, spellInfo ? resurrectAuraSpell : 0);
    SendResurrectRequest(pTarget);
}

void Spell::EffectDestroyItem(SpellEffectEntry const* effect)
{
    if (!unitTarget || unitTarget->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 itemId = effect->EffectItemType;
    uint32 count = uint32(damage);

    ((Player*)unitTarget)->DestroyItemCount(itemId, count, true);
}

void Spell::EffectBuyGuildBankSlot(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!damage)
        return;

    uint8 TabId = damage - 1;
    Player* player = (Player*)m_caster;

    uint32 GuildId = player->GetGuildId();
    if (!GuildId)
        return;

    Guild* pGuild = sGuildMgr.GetGuildById(GuildId);
    if (!pGuild)
        return;

    if (pGuild->GetLeaderGuid() != player->GetObjectGuid())
        return;

    // m_PurchasedTabs = 0 when buying Tab 0, that is why this check can be made
    // also don't allow buy tabs that are obtained through guild perks
    if (TabId != pGuild->GetPurchasedTabs() || TabId >= GUILD_BANK_MAX_TABS)
        return;

    // Go on with creating tab
    pGuild->CreateNewBankTab();
    pGuild->SetBankRightsAndSlots(player->GetRank(), TabId, GUILD_BANK_RIGHT_FULL, WITHDRAW_SLOT_UNLIMITED, true);
    pGuild->Roster();                                       // broadcast for tab rights update
    pGuild->DisplayGuildBankTabsInfo(player->GetSession());
}

void Spell::EffectSurvey(SpellEffectEntry const* effect)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (!sWorld.getConfig(CONFIG_BOOL_ARCHAEOLOGY_ENABLED))
        return;

    uint32 go_id = 0;
    uint8 slot = 4;

    float x, y, z, o;
    x = m_caster->GetPositionX();
    y = m_caster->GetPositionY();
    z = m_caster->GetPositionZ();
    o = m_caster->GetOrientation();

    int32 duration;
    if (!((Player*)m_caster)->OnSurvey(go_id, x, y, z, o))
        duration = 10000;
    else
        duration = 60000;

    if (!go_id)
        return;

    if (ObjectGuid guid = m_caster->m_ObjectSlotGuid[slot])
    {
        if (GameObject* obj = m_caster ? m_caster->GetMap()->GetGameObject(guid) : NULL)
            obj->SetLootState(GO_JUST_DEACTIVATED);

        m_caster->m_ObjectSlotGuid[slot].Clear();
    }

    GameObject* pGameObj = new GameObject;

    Map* map = m_caster->GetMap();
    if (!pGameObj->Create(map->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), go_id, map,
        m_caster->GetPhaseMask(), x, y, z, o))
    {
        delete pGameObj;
        return;
    }

    pGameObj->SetRespawnTime(duration > 0 ? duration / IN_MILLISECONDS : 0);
    pGameObj->SetSpellId(m_spellInfo->Id);
    m_caster->AddGameObject(pGameObj);

    map->Add(pGameObj);

    m_caster->m_ObjectSlotGuid[slot] = pGameObj->GetObjectGuid();

    pGameObj->SummonLinkedTrapIfAny();

    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->JustSummoned(pGameObj);
    if (m_originalCaster && m_originalCaster != m_caster && m_originalCaster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_originalCaster)->AI())
        ((Creature*)m_originalCaster)->AI()->JustSummoned(pGameObj);
}
