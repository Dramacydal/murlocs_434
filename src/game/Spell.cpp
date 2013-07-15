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
#include "WorldSession.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Pet.h"
#include "Unit.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "Group.h"
#include "UpdateData.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "CellImpl.h"
#include "Policies/Singleton.h"
#include "SharedDefines.h"
#include "LootMgr.h"
#include "VMapFactory.h"
#include "BattleGround/BattleGround.h"
#include "BattleGround/BattleGroundDS.h"
#include "Util.h"
#include "Vehicle.h"
#include "Chat.h"
#include "Language.h"
#include "DB2Stores.h"
#include "SQLStorages.h"
#include "TemporarySummon.h"
#include "GuildMgr.h"
#include "Guild.h"
#include "SQLStorages.h"

extern pEffect SpellEffects[TOTAL_SPELL_EFFECTS];

class PrioritizeManaUnitWraper
{
    public:
        explicit PrioritizeManaUnitWraper(Unit* unit) : i_unit(unit)
        {
            int32 maxmana = unit->GetMaxPower(POWER_MANA);
            i_percent = maxmana ? unit->GetPower(POWER_MANA) * 100 / maxmana : 101;
        }
        Unit* getUnit() const { return i_unit; }
        uint32 getPercent() const { return i_percent; }
    private:
        Unit* i_unit;
        uint32 i_percent;
};

struct PrioritizeMana
{
    int operator()( PrioritizeManaUnitWraper const& x, PrioritizeManaUnitWraper const& y ) const
    {
        return x.getPercent() > y.getPercent();
    }
};

typedef std::priority_queue<PrioritizeManaUnitWraper, std::vector<PrioritizeManaUnitWraper>, PrioritizeMana> PrioritizeManaUnitQueue;

class PrioritizeHealthUnitWraper
{
public:
    explicit PrioritizeHealthUnitWraper(Unit* unit) : i_unit(unit)
    {
        uint32 maxhp = unit->GetMaxHealth();
        i_percent = maxhp ? (unit->GetHealth() * 100 / maxhp) : 101;
    }
    Unit* getUnit() const { return i_unit; }
    uint32 getPercent() const { return i_percent; }
private:
    Unit* i_unit;
    uint32 i_percent;
};

struct PrioritizeHealth
{
    int operator()( PrioritizeHealthUnitWraper const& x, PrioritizeHealthUnitWraper const& y ) const
    {
        return x.getPercent() > y.getPercent();
    }
};

typedef std::priority_queue<PrioritizeHealthUnitWraper, std::vector<PrioritizeHealthUnitWraper>, PrioritizeHealth> PrioritizeHealthUnitQueue;

bool IsQuestTameSpell(uint32 spellId)
{
    SpellEntry const *spellproto = sSpellStore.LookupEntry(spellId);
    if (!spellproto)
        return false;

    SpellEffectEntry const* spellEffect0 = spellproto->GetSpellEffect(EFFECT_INDEX_0);
    SpellEffectEntry const* spellEffect1 = spellproto->GetSpellEffect(EFFECT_INDEX_1);
    return spellEffect0 && spellEffect0->Effect == SPELL_EFFECT_THREAT &&
        spellEffect1 && spellEffect1->Effect == SPELL_EFFECT_APPLY_AURA && spellEffect1->EffectApplyAuraName == SPELL_AURA_DUMMY;
}

SpellCastTargets::SpellCastTargets()
{
    m_unitTarget = NULL;
    m_itemTarget = NULL;
    m_GOTarget   = NULL;

    m_itemTargetEntry  = 0;

    m_srcX = m_srcY = m_srcZ = m_destX = m_destY = m_destZ = 0.0f;
    m_strTarget = "";
    m_targetMask = 0;

    m_srcTransportGUID.Clear();
    m_destTransportGUID.Clear();

    m_srcTransOffsetX = m_srcTransOffsetY = m_srcTransOffsetZ = 0.0f;
    m_destTransOffsetX = m_destTransOffsetY = m_destTransOffsetZ = 0.0f;

    m_elevation = 0.0f;
    m_speed = 0.0f;
}

SpellCastTargets::~SpellCastTargets()
{
}

void SpellCastTargets::setUnitTarget(Unit *target)
{
    if (!target)
        return;

    m_destX = target->GetPositionX();
    m_destY = target->GetPositionY();
    m_destZ = target->GetPositionZ();
    m_unitTarget = target;
    m_unitTargetGUID = target->GetObjectGuid();
    m_targetMask |= TARGET_FLAG_UNIT;
}

void SpellCastTargets::setDestination(float x, float y, float z)
{
    m_destX = x;
    m_destY = y;
    m_destZ = z;
    m_targetMask |= TARGET_FLAG_DEST_LOCATION;
}

void SpellCastTargets::setSource(float x, float y, float z)
{
    m_srcX = x;
    m_srcY = y;
    m_srcZ = z;
    m_targetMask |= TARGET_FLAG_SOURCE_LOCATION;
}

void SpellCastTargets::setGOTarget(GameObject *target)
{
    m_GOTarget = target;
    m_GOTargetGUID = target->GetObjectGuid();
    //    m_targetMask |= TARGET_FLAG_OBJECT;
}

void SpellCastTargets::setItemTarget(Item* item)
{
    if(!item)
        return;

    m_itemTarget = item;
    m_itemTargetGUID = item->GetObjectGuid();
    m_itemTargetEntry = item->GetEntry();
    m_targetMask |= TARGET_FLAG_ITEM;
}

void SpellCastTargets::setTradeItemTarget(Player* caster)
{
    m_itemTargetGUID = ObjectGuid(uint64(TRADE_SLOT_NONTRADED));
    m_itemTargetEntry = 0;
    m_targetMask |= TARGET_FLAG_TRADE_ITEM;

    Update(caster);
}

void SpellCastTargets::setCorpseTarget(Corpse* corpse)
{
    m_CorpseTargetGUID = corpse->GetObjectGuid();
}

void SpellCastTargets::Update(Unit* caster)
{
    m_GOTarget   = m_GOTargetGUID ? caster->GetMap()->GetGameObject(m_GOTargetGUID) : NULL;
    m_unitTarget = m_unitTargetGUID ?
        ( m_unitTargetGUID == caster->GetObjectGuid() ? caster : ObjectAccessor::GetUnit(*caster, m_unitTargetGUID) ) :
        NULL;

    m_itemTarget = NULL;
    if (caster->GetTypeId() == TYPEID_PLAYER)
    {
        Player *player = ((Player*)caster);

        if (m_targetMask & TARGET_FLAG_ITEM)
            m_itemTarget = player->GetItemByGuid(m_itemTargetGUID);
        else if (m_targetMask & TARGET_FLAG_TRADE_ITEM)
        {
            if (TradeData* pTrade = player->GetTradeData())
                if (m_itemTargetGUID.GetRawValue() < TRADE_SLOT_COUNT)
                    m_itemTarget = pTrade->GetTraderData()->GetItem(TradeSlots(m_itemTargetGUID.GetRawValue()));
        }

        if (m_itemTarget)
            m_itemTargetEntry = m_itemTarget->GetEntry();
    }

    if (m_srcTransportGUID && (m_targetMask & TARGET_FLAG_SOURCE_LOCATION))
    {
        if (GameObject* transport = m_srcTransportGUID.IsMOTransport() ? caster->GetMap()->GetTransport(m_srcTransportGUID) : caster->GetMap()->GetGameObject(m_srcTransportGUID))
        {
            m_srcX = transport->GetPositionX() + m_srcTransOffsetX * cos(transport->GetOrientation()) - m_srcTransOffsetY * sin(transport->GetOrientation());
            m_srcY = transport->GetPositionY() + m_srcTransOffsetY * cos(transport->GetOrientation()) + m_srcTransOffsetX * sin(transport->GetOrientation());
            m_srcZ = transport->GetPositionZ() + m_srcTransOffsetZ;
        }
    }

    if (m_destTransportGUID && (m_targetMask & TARGET_FLAG_DEST_LOCATION))
    {
        if (GameObject* transport = m_destTransportGUID.IsMOTransport() ? caster->GetMap()->GetTransport(m_destTransportGUID) : caster->GetMap()->GetGameObject(m_destTransportGUID))
        {
            m_destX = transport->GetPositionX() + m_destTransOffsetX * cos(transport->GetOrientation()) - m_destTransOffsetY * sin(transport->GetOrientation());
            m_destY = transport->GetPositionY() + m_destTransOffsetY * cos(transport->GetOrientation()) + m_destTransOffsetX * sin(transport->GetOrientation());
            m_destZ = transport->GetPositionZ() + m_destTransOffsetZ;
        }
    }
}

void SpellCastTargets::read( ByteBuffer& data, Unit *caster )
{
    data >> m_targetMask;

    if (m_targetMask == TARGET_FLAG_NONE)
        return;

    // TARGET_FLAG_UNK2 is used for non-combat pets, maybe other?
    if( m_targetMask & ( TARGET_FLAG_UNIT | TARGET_FLAG_UNK2 ))
        data >> m_unitTargetGUID.ReadAsPacked();

    if( m_targetMask & ( TARGET_FLAG_OBJECT ))
        data >> m_GOTargetGUID.ReadAsPacked();

    if(( m_targetMask & ( TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM )) && caster->GetTypeId() == TYPEID_PLAYER)
        data >> m_itemTargetGUID.ReadAsPacked();

    if( m_targetMask & (TARGET_FLAG_CORPSE | TARGET_FLAG_PVP_CORPSE ) )
        data >> m_CorpseTargetGUID.ReadAsPacked();

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        data >> m_srcTransportGUID.ReadAsPacked();
        data >> m_srcX >> m_srcY >> m_srcZ;
        if (m_srcTransportGUID)
        {
            m_srcTransOffsetX = m_srcX;
            m_srcTransOffsetY = m_srcY;
            m_srcTransOffsetZ = m_srcZ;
        }
        else if(!MaNGOS::IsValidMapCoord(m_srcX, m_srcY, m_srcZ))
            throw ByteBufferException(false, data.rpos(), 0, data.size());
    }
    else
    {
        m_srcTransportGUID = caster->GetTransportGuid();
        if (m_srcTransportGUID)
        {
            m_srcTransOffsetX = caster->GetTransOffsetX();
            m_srcTransOffsetY = caster->GetTransOffsetY();
            m_srcTransOffsetZ = caster->GetTransOffsetZ();
        }
        else
        {
            m_srcX = caster->GetPositionX();
            m_srcY = caster->GetPositionY();
            m_srcZ = caster->GetPositionZ();
        }
    }

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data >> m_destTransportGUID.ReadAsPacked();
        data >> m_destX >> m_destY >> m_destZ;
        if (m_destTransportGUID)
        {
            m_destTransOffsetX = m_destX;
            m_destTransOffsetY = m_destY;
            m_destTransOffsetZ = m_destZ;
        }
        else if (!MaNGOS::IsValidMapCoord(m_destX, m_destY, m_destZ))
            throw ByteBufferException(false, data.rpos(), 0, data.size());
    }
    else
    {
        m_destTransportGUID = caster->GetTransportGuid();
        if (m_destTransportGUID)
        {
            m_destTransOffsetX = caster->GetTransOffsetX();
            m_destTransOffsetY = caster->GetTransOffsetY();
            m_destTransOffsetZ = caster->GetTransOffsetZ();
        }
        else
        {
            m_destX = caster->GetPositionX();
            m_destY = caster->GetPositionY();
            m_destZ = caster->GetPositionZ();
        }
    }

    if( m_targetMask & TARGET_FLAG_STRING )
        data >> m_strTarget;

    // find real units/GOs / update trasport coordinates
    Update(caster);
}

void SpellCastTargets::write( ByteBuffer& data ) const
{
    data << uint32(m_targetMask);

    if( m_targetMask & ( TARGET_FLAG_UNIT | TARGET_FLAG_PVP_CORPSE | TARGET_FLAG_OBJECT | TARGET_FLAG_CORPSE | TARGET_FLAG_UNK2 ) )
    {
        if(m_targetMask & TARGET_FLAG_UNIT)
        {
            if(m_unitTarget)
                data << m_unitTarget->GetPackGUID();
            else
                data << uint8(0);
        }
        else if( m_targetMask & TARGET_FLAG_OBJECT )
        {
            if(m_GOTarget)
                data << m_GOTarget->GetPackGUID();
            else
                data << uint8(0);
        }
        else if( m_targetMask & ( TARGET_FLAG_CORPSE | TARGET_FLAG_PVP_CORPSE ) )
            data << m_CorpseTargetGUID.WriteAsPacked();
        else
            data << uint8(0);
    }

    if( m_targetMask & ( TARGET_FLAG_ITEM | TARGET_FLAG_TRADE_ITEM ) )
    {
        if(m_itemTarget)
            data << m_itemTarget->GetPackGUID();
        else
            data << uint8(0);
    }

    if (m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
    {
        data << m_srcTransportGUID.WriteAsPacked();
        if (m_srcTransportGUID)
            data << m_srcTransOffsetX << m_srcTransOffsetY << m_srcTransOffsetZ;
        else
            data << m_srcX << m_srcY << m_srcZ;
    }

    if (m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data << m_destTransportGUID.WriteAsPacked();
        if (m_destTransportGUID)
            data << m_destTransOffsetX << m_destTransOffsetY << m_destTransOffsetZ;
        else
            data << m_destX << m_destY << m_destZ;
    }

    if( m_targetMask & TARGET_FLAG_STRING )
        data << m_strTarget;
}

void SpellCastTargets::ReadAdditionalData(WorldPacket& data, uint8& cast_flags)
{
    if (cast_flags & 0x02)              // has trajectory data
    {
        data >> m_elevation >> m_speed;

        bool hasMovementData;
        data >> hasMovementData;
        if (hasMovementData)
        {
            MovementInfo mi;
            data >> mi;
            setSource(mi.GetPos()->x, mi.GetPos()->y, mi.GetPos()->z);
        }
    }
    else if (cast_flags & 0x08)         // has archaeology weight
    {
        uint32 count;
        uint8 type;
        data >> count;
        for (uint32 i = 0; i < count; ++i)
        {
            ArchaeologyWeight w;
            data >> type;
            w.type = type;

            switch (type)
            {
                case WEIGHT_FRAGMENT:                   // Fragments
                    data >> w.fragment.currencyId;      // Currency entry
                    data >> w.fragment.currencyCount;   // Currency count
                    break;
                case WEIGHT_KEYSTONE:                   // Keystones
                    data >> w.keystone.itemId;          // Item entry
                    data >> w.keystone.itemCount;       // Item count
                    break;
            }

            DEBUG_LOG("SpellCastTargets::ReadAdditionalData: type %u id %u count %u", w.type, w.raw.id, w.raw.count);
            m_weights.push_back(w);
        }
    }
}

Spell::Spell(Unit* caster, SpellEntry const* info, bool triggered, ObjectGuid originalCasterGUID, SpellEntry const* triggeredBy)
{
    MANGOS_ASSERT( caster != NULL && info != NULL );
    MANGOS_ASSERT( info == sSpellStore.LookupEntry( info->Id ) && "`info` must be pointer to sSpellStore element");

    if (info->SpellDifficultyId && caster->IsInWorld() && caster->GetMap()->IsDungeon())
    {
        if (SpellEntry const* spellEntry = GetSpellEntryByDifficulty(info->SpellDifficultyId, caster->GetMap()->GetDifficulty(), caster->GetMap()->IsRaid()))
            m_spellInfo = spellEntry;
        else
            m_spellInfo = info;
    }
    else
        m_spellInfo = info;

    m_triggeredBySpellInfo = triggeredBy;

    m_spellInterrupts = m_spellInfo->GetSpellInterrupts();

    m_caster = caster;
    m_selfContainer = NULL;
    m_referencedFromCurrentSpell = false;
    m_executedCurrently = false;
    m_delayStart = 0;
    m_delayAtDamageCount = 0;
    m_damage = 0;

    m_applyMultiplierMask = 0;

    // Get data for type of attack
    m_attackType = GetWeaponAttackType(m_spellInfo);

    m_spellSchoolMask = GetSpellSchoolMask(info);           // Can be override for some spell (wand shoot for example)

    if(m_attackType == RANGED_ATTACK)
    {
        // wand case
        if((m_caster->getClassMask() & CLASSMASK_WAND_USERS) != 0 && m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (Item* pItem = ((Player*)m_caster)->GetWeaponForAttack(RANGED_ATTACK))
                m_spellSchoolMask = SpellSchoolMask(1 << pItem->GetProto()->DamageType);
        }
    }
    // Set health leech amount to zero
    m_healthLeech = 0;

    m_originalCasterGUID = originalCasterGUID ? originalCasterGUID : m_caster->GetObjectGuid();

    UpdateOriginalCasterPointer();

    for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
        m_currentBasePoints[i] = m_spellInfo->CalculateSimpleValue(SpellEffectIndex(i));

    m_spellState = SPELL_STATE_PREPARING;

    m_castPositionX = m_castPositionY = m_castPositionZ = 0;
    m_TriggerSpells.clear();
    m_preCastSpells.clear();
    m_IsTriggeredSpell = triggered;
    m_customSpellData.Clear();
    //m_AreaAura = false;
    m_CastItem = NULL;

    unitTarget = NULL;
    itemTarget = NULL;
    gameObjTarget = NULL;
    focusObject = NULL;
    m_cast_count = 0;
    m_glyphIndex = 0;
    m_triggeredByAuraSpell  = NULL;

    //Auto Shot & Shoot (wand)
    m_autoRepeat = IsAutoRepeatRangedSpell(m_spellInfo);

    m_runesState = 0;
    m_powerCost = 0;                                        // setup to correct value in Spell::prepare, don't must be used before.
    m_usedHolyPower = 0;
    m_casttime = 0;                                         // setup to correct value in Spell::prepare, don't must be used before.
    m_timer = 0;                                            // will set to cast time in prepare
    m_duration = 0;

    m_needAliveTargetMask = 0;

    // determine reflection
    m_canReflect = CanReflect(m_spellInfo);

    m_spellFlags = SPELL_FLAG_NORMAL;

    CleanupTargetList();
}

bool Spell::CanReflect(SpellEntry const * spellInfo)
{
    if (!spellInfo)
        return false;

    // AoE spells, spells with non-magic DmgClass or SchoolMask or with SPELL_ATTR_EX_CANT_REFLECTED cannot be reflected
    if (spellInfo->GetDmgClass() == SPELL_DAMAGE_CLASS_MAGIC &&
        spellInfo->SchoolMask != SPELL_SCHOOL_MASK_NORMAL &&
        !spellInfo->HasAttribute(SPELL_ATTR_EX_CANT_REFLECTED) &&
        !IsAreaOfEffectSpell(spellInfo))
    {
        for(int j = 0; j < MAX_EFFECT_INDEX; ++j)
        {
            SpellEffectEntry const* spellEffect = spellInfo->GetSpellEffect(SpellEffectIndex(j));
            if(!spellEffect)
                continue;

            if (spellEffect->Effect == 0)
                continue;

            if (IsPositiveTarget(spellEffect->EffectImplicitTargetA, spellEffect->EffectImplicitTargetB)
                && !spellInfo->HasAttribute(SPELL_ATTR_EX_NEGATIVE))
                continue;
            else
                return true;

            break;
        }
    }

    return false;
}

Spell::~Spell() { }

template<typename T>
WorldObject* Spell::FindCorpseUsing()
{
    // non-standard target selection
    SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex);
    float max_range = GetSpellMaxRange(srange);

    WorldObject* result = NULL;

    T u_check((Player*)m_caster, max_range);
    MaNGOS::WorldObjectSearcher<T> searcher(result, u_check);

    Cell::VisitGridObjects(m_caster, searcher, max_range);

    if (!result)
        Cell::VisitWorldObjects(m_caster, searcher, max_range);

    return result;
}

void Spell::FillTargetMap()
{
    // TODO: ADD the correct target FILLS!!!!!!

    UnitList tmpUnitLists[MAX_EFFECT_INDEX];                // Stores the temporary Target Lists for each effect
    uint8 effToIndex[MAX_EFFECT_INDEX] = {0, 1, 2};         // Helper array, to link to another tmpUnitList, if the targets for both effects match

    for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
        if(!spellEffect)
            continue;

        // not call for empty effect.
        // Also some spells use not used effect targets for store targets for dummy effect in triggered spells
        if(spellEffect->Effect == SPELL_EFFECT_NONE)
            continue;

        // targets for TARGET_SCRIPT_COORDINATES (A) and TARGET_SCRIPT
        // for TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT (A) all is checked in Spell::CheckCast and in Spell::CheckItem
        // filled in Spell::CheckCast call
        if(spellEffect->EffectImplicitTargetA == TARGET_SCRIPT_COORDINATES ||
           spellEffect->EffectImplicitTargetA == TARGET_SCRIPT ||
           spellEffect->EffectImplicitTargetA == TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT ||
           (spellEffect->EffectImplicitTargetB == TARGET_SCRIPT && spellEffect->EffectImplicitTargetA != TARGET_SELF))
            continue;

        // TODO: find a way so this is not needed?
        // for area auras always add caster as target (needed for totems for example)
        if (IsAreaAuraEffect(spellEffect->Effect))
            AddUnitTarget(m_caster, SpellEffectIndex(i));

        // no double fill for same targets
        for (int j = 0; j < i; ++j)
        {
            SpellEffectEntry const* spellEffect1 = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
            if (!spellEffect1)
                continue;

            // Check if same target, but handle i.e. AreaAuras different
            if (spellEffect->EffectImplicitTargetA == spellEffect1->EffectImplicitTargetA && spellEffect->EffectImplicitTargetB == spellEffect1->EffectImplicitTargetB
                && spellEffect1->Effect != SPELL_EFFECT_NONE
                && !IsAreaAuraEffect(spellEffect->Effect) && !IsAreaAuraEffect(spellEffect1->Effect))
                // Add further conditions here if required
            {
                effToIndex[i] = j;                          // effect i has same targeting list as effect j
                break;
            }
        }

        if (effToIndex[i] == i)                             // New target combination
        {
            if (!FillCustomTargetMap(SpellEffectIndex(i), tmpUnitLists[i /*==effToIndex[i]*/]))
            // TargetA/TargetB dependent from each other, we not switch to full support this dependences
            // but need it support in some know cases
            switch(spellEffect->EffectImplicitTargetA)
            {
                case TARGET_NONE:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case TARGET_NONE:
                            if (m_caster->GetObjectGuid().IsPet())
                                SetTargetMap(SpellEffectIndex(i), TARGET_SELF, tmpUnitLists[i /*==effToIndex[i]*/]);
                            else
                                SetTargetMap(SpellEffectIndex(i), TARGET_EFFECT_SELECT, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
                case TARGET_SELF:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case TARGET_NONE:
                        case TARGET_EFFECT_SELECT:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        case TARGET_AREAEFFECT_INSTANT:         // use B case that not dependent from from A in fact
                            if((m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) == 0)
                                m_targets.setDestination(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ());
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        case TARGET_BEHIND_VICTIM:              // use B case that not dependent from from A in fact
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
                case TARGET_EFFECT_SELECT:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case TARGET_NONE:
                        case TARGET_EFFECT_SELECT:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        // dest point setup required
                        case TARGET_AREAEFFECT_INSTANT:
                        case TARGET_ALL_ENEMY_IN_AREA:
                        case TARGET_ALL_ENEMY_IN_AREA_CHANNELED:
                        case TARGET_ALL_FRIENDLY_UNITS_IN_AREA:
                        case TARGET_AREAEFFECT_GO_AROUND_DEST:
                        case TARGET_RANDOM_NEARBY_DEST:
                            // triggered spells get dest point from default target set, ignore it
                            if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) || m_IsTriggeredSpell)
                                if (WorldObject* castObject = GetCastingObject())
                                    m_targets.setDestination(castObject->GetPositionX(), castObject->GetPositionY(), castObject->GetPositionZ());
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        // target pre-selection required
                        case TARGET_INNKEEPER_COORDINATES:
                        case TARGET_TABLE_X_Y_Z_COORDINATES:
                        case TARGET_CASTER_COORDINATES:
                        case TARGET_SCRIPT_COORDINATES:
                        case TARGET_CURRENT_ENEMY_COORDINATES:
                        case TARGET_DUELVSPLAYER_COORDINATES:
                        case TARGET_DYNAMIC_OBJECT_COORDINATES:
                        case TARGET_POINT_AT_NORTH:
                        case TARGET_POINT_AT_SOUTH:
                        case TARGET_POINT_AT_EAST:
                        case TARGET_POINT_AT_WEST:
                        case TARGET_POINT_AT_NE:
                        case TARGET_POINT_AT_NW:
                        case TARGET_POINT_AT_SE:
                        case TARGET_POINT_AT_SW:
                            // need some target for processing
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        case TARGET_AREAEFFECT_CUSTOM:
                        case TARGET_ALL_ENEMY_IN_AREA_INSTANT:
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
                case TARGET_CASTER_COORDINATES:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case TARGET_ALL_ENEMY_IN_AREA:
                            // Note: this hack with search required until GO casting not implemented
                            // environment damage spells already have around enemies targeting but this not help in case nonexistent GO casting support
                            // currently each enemy selected explicitly and self cast damage
                            if (spellEffect->Effect == SPELL_EFFECT_ENVIRONMENTAL_DAMAGE)
                            {
                                if(m_targets.getUnitTarget())
                                    tmpUnitLists[i /*==effToIndex[i]*/].push_back(m_targets.getUnitTarget());
                            }
                            else
                            {
                                SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                                SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            }
                            break;
                        case 0:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            tmpUnitLists[i /*==effToIndex[i]*/].push_back(m_caster);
                            break;
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
                case TARGET_TABLE_X_Y_Z_COORDINATES:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case 0:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);


                        // need some target for processing
                            SetTargetMap(SpellEffectIndex(i), TARGET_EFFECT_SELECT, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        case TARGET_AREAEFFECT_INSTANT:         // All 17/7 pairs used for dest teleportation, A processed in effect code
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
                case TARGET_ALLY_IN_FRONT_OF_CASTER_30:
                    SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                    break;
                case TARGET_SELF2:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case TARGET_NONE:
                        case TARGET_EFFECT_SELECT:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        case TARGET_AREAEFFECT_CUSTOM:
                            // triggered spells get dest point from default target set, ignore it
                            if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) || m_IsTriggeredSpell)
                                if (WorldObject* castObject = GetCastingObject())
                                    m_targets.setDestination(castObject->GetPositionX(), castObject->GetPositionY(), castObject->GetPositionZ());
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        case TARGET_RANDOM_NEARBY_DEST: 
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        // most A/B target pairs is self->negative and not expect adding caster to target list
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
                case TARGET_DUELVSPLAYER_COORDINATES:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case TARGET_NONE:
                        case TARGET_EFFECT_SELECT:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            if (Unit* currentTarget = m_targets.getUnitTarget())
                                tmpUnitLists[i /*==effToIndex[i]*/].push_back(currentTarget);
                            break;
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
                case TARGET_AREAEFFECT_CUSTOM:
                default:
                    switch(spellEffect->EffectImplicitTargetB)
                    {
                        case TARGET_NONE:
                        case TARGET_EFFECT_SELECT:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                        case TARGET_SCRIPT_COORDINATES:         // B case filled in CheckCast but we need fill unit list base at A case
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                           break;
                        default:
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetA, tmpUnitLists[i /*==effToIndex[i]*/]);
                            SetTargetMap(SpellEffectIndex(i), spellEffect->EffectImplicitTargetB, tmpUnitLists[i /*==effToIndex[i]*/]);
                            break;
                    }
                    break;
            }
        }

        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            Player* me = (Player*)m_caster;
            for (UnitList::const_iterator itr = tmpUnitLists[effToIndex[i]].begin(); itr != tmpUnitLists[effToIndex[i]].end(); ++itr)
            {
                Player *targetOwner = (*itr)->GetCharmerOrOwnerPlayerOrPlayerItself();
                if (targetOwner && targetOwner != me && targetOwner->IsPvP() && !me->IsInDuelWith(targetOwner))
                {
                    me->UpdatePvP(true);
                    me->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_ENTER_PVP_COMBAT);
                    break;
                }
            }
        }

        DEBUG_LOG("Spell::FillTargetMap: %u targets for spell %u effIdx %u targetpair", tmpUnitLists[effToIndex[i]].size(), m_spellInfo->Id, i);
        for (UnitList::iterator itr = tmpUnitLists[effToIndex[i]].begin(); itr != tmpUnitLists[effToIndex[i]].end();)
        {
            if (!CheckTarget (*itr, SpellEffectIndex(i)))
            {
                itr = tmpUnitLists[effToIndex[i]].erase(itr);
                continue;
            }
            else
                ++itr;
        }
        DEBUG_LOG("After target check: %u", tmpUnitLists[effToIndex[i]].size());

        for (UnitList::const_iterator iunit = tmpUnitLists[effToIndex[i]].begin(); iunit != tmpUnitLists[effToIndex[i]].end(); ++iunit)
            AddUnitTarget((*iunit), SpellEffectIndex(i));
    }
}

void Spell::prepareDataForTriggerSystem()
{
    //==========================================================================================
    // Now fill data for trigger system, need know:
    // an spell trigger another or not ( m_canTrigger )
    // Create base triggers flags for Attacker and Victim ( m_procAttacker and  m_procVictim)
    //==========================================================================================
    // Fill flag can spell trigger or not
    // TODO: possible exist spell attribute for this
    m_canTrigger = false;

    if (m_CastItem)
        m_canTrigger = false;                               // Do not trigger from item cast spell
    else if (!m_IsTriggeredSpell)
        m_canTrigger = true;                                // Normal cast - can trigger
    else if (!m_triggeredByAuraSpell)
        m_canTrigger = true;                                // Triggered from SPELL_EFFECT_TRIGGER_SPELL - can trigger

    switch (m_spellInfo->GetSpellFamilyName())              // Exceptions (some periodic triggers)
    {
        case SPELLFAMILY_MAGE:
            // Arcane Missiles / Blizzard triggers need do it
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000000000200080)))
                m_canTrigger = true;
            // Clearcasting trigger need do it
            else if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000000200000000), 0x8))
                m_canTrigger = true;
            // Replenish Mana, item spell with triggered cases (Mana Agate, etc mana gems)
            else if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000010000000000)))
                m_canTrigger = true;
            // Living Bomb needs Ignite, Hot Streak etc
            else if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x1000000000000)))
                m_canTrigger = true;
            // Winter's Chill
            else if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0), 0x1))
                m_canTrigger = true;
            // Chilled debuffs
            else if (m_spellInfo->Id == 6136 || m_spellInfo->Id == 6136 || m_spellInfo->Id == 18101 || m_spellInfo->Id == 31257)
                m_canTrigger = true;
            break;
        case SPELLFAMILY_WARRIOR:
            // Deep Wounds
            if (m_spellInfo->Id == 12721)
                m_canTrigger = true;
            // Whirlwind (Bladestorm)
            else if (m_spellInfo->Id == 50622)
                m_canTrigger = true;
            // Bloodsurge
            else if (m_spellInfo->Id == 46916)
                m_canTrigger = true;
            break;
        case SPELLFAMILY_WARLOCK:
            // For Hellfire Effect / Rain of Fire / Seed of Corruption triggers need do it
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000800000000060)))
                m_canTrigger = true;
            break;
        case SPELLFAMILY_PRIEST:
            // For Penance, Mind Sear,Mind Flay, Improved Devouring Plague heal/damage triggers need do it
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0001800000800000), 0x00000048))
                m_canTrigger = true;
            // Empowered Renew
            else if (m_spellInfo->Id == 63544)
                m_canTrigger = true;
            break;
        case SPELLFAMILY_DRUID:
            // Clearcasting
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x20000000000000)))
                m_canTrigger = true;
            break;
        case SPELLFAMILY_ROGUE:
            // For poisons need do it
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x000000101001E000)))
                m_canTrigger = true;
            // Mutilate triggers should trigger. Main spell should not trigger
            else if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x20000000000000)))
                m_canTrigger = false;
            else if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x600000000)))
                m_canTrigger = true;
            // Main Gauche
            else if (m_spellInfo->Id == 86392)
                m_canTrigger = true;
            break;
        case SPELLFAMILY_HUNTER:
            // Hunter Explosive Trap Effect/Immolation Trap Effect/Frost Trap Aura/Snake Trap Effect/Explosive Shot
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0100200000000214)))
                m_canTrigger = true;
            // Rapid Killing
            else if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0), 0x1000000))
                m_canTrigger = true;
            break;
        case SPELLFAMILY_PALADIN:
            // For Judgements (all) / Holy Shock triggers need do it
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0001000900B80400)))
                m_canTrigger = true;
            // Seal of Righteousness proc
            else if (m_spellInfo->Id == 25742)
                m_canTrigger = true;
            // Seal of Justice proc
            else if (m_spellInfo->Id == 20170)
                m_canTrigger = true;
            // Censure
            else if (m_spellInfo->Id == 31803)
                m_canTrigger = true;
            break;
        case SPELLFAMILY_DEATHKNIGHT:
            // Death Coil (damage)
            if (m_spellInfo->Id == 47632)
                m_canTrigger = true;
            // Death Coil (heal)
            else if (m_spellInfo->Id == 47633)
                m_canTrigger = true;
        default:
            break;
    }

    // Get data for type of attack and fill base info for trigger
    switch (m_spellInfo->GetDmgClass())
    {
        case SPELL_DAMAGE_CLASS_MELEE:
            m_procAttacker = PROC_FLAG_SUCCESSFUL_MELEE_SPELL_HIT;
            if (m_attackType == OFF_ATTACK)
                m_procAttacker |= PROC_FLAG_SUCCESSFUL_OFFHAND_HIT;
            m_procVictim   = PROC_FLAG_TAKEN_MELEE_SPELL_HIT;
            break;
        case SPELL_DAMAGE_CLASS_RANGED:
            // Auto attack
            if (m_spellInfo->HasAttribute(SPELL_ATTR_EX2_AUTOREPEAT_FLAG))
            {
                m_procAttacker = PROC_FLAG_SUCCESSFUL_RANGED_HIT;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_HIT;
            }
            else // Ranged spell attack
            {
                m_procAttacker = PROC_FLAG_SUCCESSFUL_RANGED_SPELL_HIT;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_SPELL_HIT;
            }
            break;
        default:
            if (IsPositiveSpell(m_spellInfo->Id))                                 // Check for positive spell
            {
                m_procAttacker = PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL;
                m_procVictim   = PROC_FLAG_TAKEN_POSITIVE_SPELL;
            }
            else if (m_spellInfo->HasAttribute(SPELL_ATTR_EX2_AUTOREPEAT_FLAG))   // Wands auto attack
            {
                m_procAttacker = PROC_FLAG_SUCCESSFUL_RANGED_HIT;
                m_procVictim   = PROC_FLAG_TAKEN_RANGED_HIT;
            }
            else                                           // Negative spell
            {
                m_procAttacker = PROC_FLAG_SUCCESSFUL_NEGATIVE_SPELL_HIT;
                m_procVictim   = PROC_FLAG_TAKEN_NEGATIVE_SPELL_HIT;
            }
            break;
    }

    // some negative spells have positive effects to another or same targets
    // avoid triggering negative hit for only positive targets
    m_negativeEffectMask = 0x0;
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (!IsPositiveEffect(m_spellInfo, SpellEffectIndex(i)))
            m_negativeEffectMask |= (1<<i);

    // Hunter traps spells: Immolation Trap Effect, Frost Trap (triggering spell!!),
    // Freezing Trap Effect(+ Freezing Arrow Effect), Explosive Trap Effect, Snake Trap Effect
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_HUNTER, UI64LIT(0x000020000000001C), 0x64000))
        m_procAttacker |= PROC_FLAG_ON_TRAP_ACTIVATION;
}

void Spell::CleanupTargetList()
{
    m_UniqueTargetInfo.clear();
    m_UniqueGOTargetInfo.clear();
    m_UniqueItemInfo.clear();
    m_delayMoment = 0;
}

void Spell::AddUnitTarget(Unit* pVictim, SpellEffectIndex effIndex)
{
    SpellEffectEntry const *spellEffect = m_spellInfo->GetSpellEffect(effIndex);
    if (!spellEffect || spellEffect->Effect == 0)
        return;

    // Check for effect immune skip if immuned
    bool immuned = pVictim->IsImmuneToSpellEffect(m_spellInfo, effIndex, pVictim == m_caster);

    if (pVictim->GetTypeId() == TYPEID_UNIT && ((Creature*)pVictim)->IsTotem() && (m_spellFlags & SPELL_FLAG_REDIRECTED))
        immuned = false;

    ObjectGuid targetGUID = pVictim->GetObjectGuid();

    // Lookup target in already in list
    for(TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (targetGUID == ihit->targetGUID)                 // Found in list
        {
            if (!immuned)
                ihit->effectMask |= 1 << effIndex;          // Add only effect mask if not immuned
            return;
        }
    }

    // This is new target calculate data for him

    // Get spell hit result on target
    TargetInfo target;
    target.targetGUID = targetGUID;                         // Store target GUID
    target.effectMask = immuned ? 0 : (1 << effIndex);      // Store index of effect if not immuned
    target.processed  = false;                              // Effects not applied on target

    // Calculate hit result

    // Procs can miss, weapon enchants can miss, triggered spells and effects cannot miss (miss already calculated in triggering spell)
    bool canMiss = (m_triggeredByAuraSpell || !m_IsTriggeredSpell);
    target.missCondition = m_caster->SpellHitResult(pVictim, m_spellInfo, m_canReflect, canMiss);

    // spell fly from visual cast object
    WorldObject* affectiveObject = GetAffectiveCasterObject();

    // Spell has trajectory - need calculate incoming time
    if (affectiveObject && m_targets.GetSpeed() > 0.0f)
    {
        float dist;
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
            dist = affectiveObject->GetDistance(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ);
        else
            dist = affectiveObject->GetDistance(pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ());
        float speed = m_targets.GetSpeed() * cos(m_targets.GetElevation());

        target.timeDelay = (uint64) floor(dist / speed * 1000.0f);

        // Calculate minimum incoming time
        if (m_delayMoment == 0 || m_delayMoment>target.timeDelay)
            m_delayMoment = target.timeDelay;
    }
    // Spell has speed - need calculate incoming time
    else if (m_spellInfo->speed > 0.0f && affectiveObject && pVictim != affectiveObject)
    {
        // calculate spell incoming interval
        float dist = affectiveObject->GetDistance(pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ());
        if (dist < 0.0001f) dist = 0.0001f;
        target.timeDelay = (uint64) floor(dist / m_spellInfo->speed * 1000.0f);

        // Calculate minimum incoming time
        if (m_delayMoment == 0 || m_delayMoment>target.timeDelay)
            m_delayMoment = target.timeDelay;
    }
    // Spell casted on self - mostly TRIGGER_MISSILE code
    else if (m_spellInfo->speed > 0.0f && affectiveObject && pVictim == affectiveObject)
    {
        float dist = 0.0f;
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
            dist = affectiveObject->GetDistance(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ);

        target.timeDelay = (uint64) floor(dist / m_spellInfo->speed * 1000.0f);
    }
    else
        target.timeDelay = UI64LIT(0);

    // If target reflect spell back to caster
    if (target.missCondition == SPELL_MISS_REFLECT)
    {
        // Calculate reflected spell result on caster
        target.reflectResult =  m_caster->SpellHitResult(m_caster, m_spellInfo, m_canReflect);

        if (target.reflectResult == SPELL_MISS_REFLECT)     // Impossible reflect again, so simply deflect spell
            target.reflectResult = SPELL_MISS_PARRY;

        // Increase time interval for reflected spells by 1.5
        target.timeDelay += target.timeDelay >> 1;

        m_spellFlags |= SPELL_FLAG_REFLECTED;
    }
    else
        target.reflectResult = SPELL_MISS_NONE;

    // Add target to list
    m_UniqueTargetInfo.push_back(target);
}

void Spell::AddUnitTarget(ObjectGuid unitGuid, SpellEffectIndex effIndex)
{
    if (Unit* unit = m_caster->GetObjectGuid() == unitGuid ? m_caster : ObjectAccessor::GetUnit(*m_caster, unitGuid))
        AddUnitTarget(unit, effIndex);
}

void Spell::AddGOTarget(GameObject* pVictim, SpellEffectIndex effIndex)
{
    SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(effIndex);
    if (!spellEffect || spellEffect->Effect == 0)
        return;

    ObjectGuid targetGUID = pVictim->GetObjectGuid();

    // Lookup target in already in list
    for(GOTargetList::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
    {
        if (targetGUID == ihit->targetGUID)                 // Found in list
        {
            ihit->effectMask |= (1 << effIndex);            // Add only effect mask
            return;
        }
    }

    // This is new target calculate data for him

    GOTargetInfo target;
    target.targetGUID = targetGUID;
    target.effectMask = (1 << effIndex);
    target.processed  = false;                              // Effects not apply on target

    // spell fly from visual cast object
    WorldObject* affectiveObject = GetAffectiveCasterObject();

    // Spell has trajectory - need calculate incoming time
    if (affectiveObject && m_targets.GetSpeed() > 0.0f)
    {
        float dist;
        if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
            dist = affectiveObject->GetDistance(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ);
        else
            dist = affectiveObject->GetDistance(pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ());

        float speed = m_targets.GetSpeed() * cos(m_targets.GetElevation());

        target.timeDelay = (uint64) floor(dist / speed * 1000.0f);

        if (m_delayMoment == 0 || m_delayMoment > target.timeDelay)
            m_delayMoment = target.timeDelay;
    }
    // Spell has speed - need calculate incoming time
    else if (m_spellInfo->speed > 0.0f && affectiveObject && pVictim != affectiveObject)
    {
        // calculate spell incoming interval
        float dist = affectiveObject->GetDistance(pVictim->GetPositionX(), pVictim->GetPositionY(), pVictim->GetPositionZ());
        if (dist < 0.0001f)
            dist = 0.0001f;
        target.timeDelay = (uint64) floor(dist / m_spellInfo->speed * 1000.0f);
        if (m_delayMoment == 0 || m_delayMoment > target.timeDelay)
            m_delayMoment = target.timeDelay;
    }
    else
        target.timeDelay = UI64LIT(0);

    // Add target to list
    m_UniqueGOTargetInfo.push_back(target);
}

void Spell::AddGOTarget(ObjectGuid goGuid, SpellEffectIndex effIndex)
{
    if (GameObject* go = m_caster->GetMap()->GetGameObject(goGuid))
        AddGOTarget(go, effIndex);
}

void Spell::AddItemTarget(Item* pitem, SpellEffectIndex effIndex)
{
    SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(effIndex);
    if (!spellEffect || spellEffect->Effect == 0)
        return;

    // Lookup target in already in list
    for(ItemTargetList::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
    {
        if (pitem == ihit->item)                            // Found in list
        {
            ihit->effectMask |= (1 << effIndex);            // Add only effect mask
            return;
        }
    }

    // This is new target add data

    ItemTargetInfo target;
    target.item       = pitem;
    target.effectMask = (1 << effIndex);
    m_UniqueItemInfo.push_back(target);
}

void Spell::DoAllEffectOnTarget(TargetInfo *target)
{
    if (!target || target->processed)   // Check target
        return;

    target->processed = true;                               // Target checked in apply effects procedure

    // Get mask of effects for target
    uint32 mask = target->effectMask;

    Unit* unit = m_caster->GetObjectGuid() == target->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target->targetGUID);
    if (!unit)
        return;

    // Get original caster (if exist) and calculate damage/healing from him data
    Unit *real_caster = GetAffectiveCaster();
    // FIXME: in case wild GO heal/damage spells will be used target bonuses
    Unit *caster = real_caster ? real_caster : m_caster;

    SpellMissInfo missInfo = target->missCondition;
    // Need init unitTarget by default unit (can changed in code on reflect)
    // Or on missInfo!=SPELL_MISS_NONE unitTarget undefined (but need in trigger subsystem)
    unitTarget = unit;

    // Reset damage/healing counter
    ResetEffectDamageAndHeal();

    // Fill base trigger info
    uint32 procAttacker = m_procAttacker;
    uint32 procVictim   = m_procVictim;
    uint32 procEx       = PROC_EX_NONE;

    // drop proc flags in case target not affected negative effects in negative spell
    // for example caster bonus or animation,
    // except miss case where will assigned PROC_EX_* flags later
    if (((procAttacker | procVictim) & NEGATIVE_TRIGGER_MASK) &&
        !(target->effectMask & m_negativeEffectMask) && missInfo == SPELL_MISS_NONE)
    {
        procAttacker = PROC_FLAG_NONE;
        procVictim   = PROC_FLAG_NONE;
    }

    if (getState() == SPELL_STATE_DELAYED && !IsPositiveSpell(m_spellInfo->Id) && (WorldTimer::getMSTime() - target->timeDelay) <= unit->m_lastSanctuaryTime)
        return;

    // obsolete since 3.3.5
    //bool offensive = !unitTarget->IsFriendlyTo(caster);
    //for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    //{
    //    SpellEffectEntry const * effect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
    //    if (!effect)
    //        continue;

    //    uint32 dispelMask;
    //    if (effect->Effect == SPELL_EFFECT_DISPEL)
    //    {
    //        dispelMask = GetDispelMask(DispelType(effect->EffectMiscValue));
    //        if (!(offensive && unitTarget->HasPositiveAuraByDispelMask(dispelMask) || !offensive && unitTarget->HasNegativeAuraByDispelMask(dispelMask)))
    //        {
    //            mask &= ~(1 << i);
    //            target->effectMask &= ~(1 << i);
    //        }
    //    }
    //    else if (offensive && effect->Effect == SPELL_EFFECT_STEAL_BENEFICIAL_BUFF)
    //    {
    //        dispelMask = GetDispelMask(DispelType(effect->EffectMiscValue));
    //        if (!unitTarget->HasPositiveAuraByDispelMask(dispelMask))
    //        {
    //            mask &= ~(1 << i);
    //            target->effectMask &= ~(1 << i);
    //        }
    //    }
    //}

    if (m_spellInfo->speed > 0)
    {
        // mark effects that were already handled in Spell::HandleDelayedSpellLaunch on spell launch as processed
        for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (IsEffectHandledOnDelayedSpellLaunch(m_spellInfo, SpellEffectIndex(i)))
                mask &= ~(1<<i);

        // maybe used in effects that are handled on hit
        m_damage += target->damage;

        // Amaru: do recheck for delayed spells on players only
        if (unit && unit->GetTypeId() == TYPEID_PLAYER && missInfo == SPELL_MISS_NONE)
        {
            missInfo = m_caster->SpellHitResult(unit, m_spellInfo, m_canReflect);

            switch (missInfo)
            {
                //Ice Block, Divine Shield, etc
                case SPELL_MISS_IMMUNE:
                //Deterrence
                case SPELL_MISS_DEFLECT:
                //Cloak of Shadows, etc
                case SPELL_MISS_MISS:
                    target->missCondition = missInfo;
                    caster->SendSpellMiss(unit, m_spellInfo->Id, missInfo);
                    return;
                case SPELL_MISS_REFLECT:
                    target->missCondition = missInfo;
                    // Calculate reflected spell result on caster
                    target->reflectResult =  m_caster->SpellHitResult(m_caster, m_spellInfo, m_canReflect);

                    if (target->reflectResult == SPELL_MISS_REFLECT)     // Impossible reflect again, so simply deflect spell
                        target->reflectResult = SPELL_MISS_PARRY;

                    // Increase time interval for reflected spells by 1.5
                    target->timeDelay += target->timeDelay >> 1;
                    caster->SendSpellMiss(unit, m_spellInfo->Id, missInfo);
                    break;
                default:
                    missInfo = target->missCondition;
                    break;
            }
        }
    }

    if (missInfo==SPELL_MISS_NONE)                          // In case spell hit target, do all effect on that target
        DoSpellHitOnUnit(unit, mask);
    else if (missInfo == SPELL_MISS_REFLECT && target->reflectResult == SPELL_MISS_NONE)                // In case spell reflect from target, do all effect on caster (if hit)
                                                                                                        // If reflected spell hit caster -> do all effect on him
    {
        DoSpellHitOnUnit(m_caster, mask);
        unitTarget = m_caster;
    }
    else if (real_caster && real_caster != unit && missInfo != SPELL_MISS_EVADE && target->reflectResult != SPELL_MISS_EVADE)
    {
        // can cause back attack (if detected)
        if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX3_NO_INITIAL_AGGRO) && (!IsPositiveSpell(m_spellInfo->Id) || IsDispelSpell(m_spellInfo) && real_caster->IsHostileTo(unit)) &&
            m_caster->isVisibleForOrDetect(unit, unit, false))
        {
            if (!unit->IsStandState() && !unit->hasUnitState(UNIT_STAT_STUNNED))
                unit->SetStandState(UNIT_STAND_STATE_STAND);

            if (!unit->isInCombat() && unit->GetTypeId() != TYPEID_PLAYER && ((Creature*)unit)->AI())
                ((Creature*)unit)->AI()->AttackedBy(real_caster);

            unit->AddThreat(real_caster);
            unit->SetInCombatWith(real_caster);
            real_caster->SetInCombatWith(unit);

            if (Player *attackedPlayer = unit->GetCharmerOrOwnerPlayerOrPlayerItself())
                real_caster->SetContestedPvP(attackedPlayer);
        }

        // Glyph of Resilient Grip
        if (m_spellInfo->Id == 49576 && missInfo == SPELL_MISS_IMMUNE)
            if (real_caster->HasAura(59309))
                real_caster->CastSpell(real_caster, 90289, true);
    }

    // All calculated do it!
    // Do healing and triggers
    if (m_healing)
    {
        bool crit = real_caster && real_caster->IsSpellCrit(unitTarget, m_spellInfo, m_spellSchoolMask);
        uint32 addhealth = m_healing;
        if (crit)
        {
            procEx |= PROC_EX_CRITICAL_HIT;
            addhealth = caster->SpellCriticalHealingBonus(m_spellInfo, addhealth, NULL);
        }
        else
            procEx |= PROC_EX_NORMAL_HIT;

        uint32 absorb = 0;
        unitTarget->CalculateHealAbsorb(addhealth, &absorb);
        addhealth -= absorb;

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (m_canTrigger && missInfo != SPELL_MISS_REFLECT)
        {
            caster->ProcDamageAndSpell(unitTarget, real_caster ? procAttacker : uint32(PROC_FLAG_NONE), procVictim, procEx, addhealth, absorb, m_attackType, m_spellInfo);
        }

        int32 gain = caster->DealHeal(unitTarget, addhealth, m_spellInfo, crit, absorb);

        if (real_caster)
            unitTarget->getHostileRefManager().threatAssist(real_caster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(m_spellInfo), m_spellInfo);
    }
    // Do damage and triggers
    else if (m_damage)
    {
        // Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage damageInfo(m_originalCaster ? m_originalCaster : caster, unitTarget, m_spellInfo->Id, m_spellSchoolMask);

        if (m_spellInfo->speed > 0)
        {
            damageInfo.damage = m_damage;
            damageInfo.HitInfo = target->HitInfo;
        }
        // Add bonuses and fill damageInfo struct
        else
            caster->CalculateSpellDamage(&damageInfo, m_damage, m_spellInfo, m_attackType);

        unitTarget->CalculateAbsorbResistBlock(caster, &damageInfo, m_spellInfo);

        caster->DealDamageMods(damageInfo.target, damageInfo.damage, &damageInfo.absorb);

        // Send log damage message to client
        caster->SendSpellNonMeleeDamageLog(&damageInfo);

        procEx = createProcExtendMask(&damageInfo, missInfo);
        procVictim |= PROC_FLAG_TAKEN_ANY_DAMAGE;

        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (m_canTrigger && missInfo != SPELL_MISS_REFLECT)
            caster->ProcDamageAndSpell(unitTarget, real_caster ? procAttacker : uint32(PROC_FLAG_NONE), procVictim, procEx, damageInfo.damage, damageInfo.absorb, m_attackType, m_spellInfo);

        // Haunt (NOTE: for avoid use additional field damage stored in dummy value (replace unused 100%)
        // apply before deal damage because aura can be removed at target kill
        SpellClassOptionsEntry const *classOpt = m_spellInfo->GetSpellClassOptions();
        if (classOpt && classOpt->SpellFamilyName == SPELLFAMILY_WARLOCK && m_spellInfo->SpellIconID == 3172 &&
            (classOpt->SpellFamilyFlags & UI64LIT(0x0004000000000000)))
            if(Aura* dummy = unitTarget->GetDummyAura(m_spellInfo->Id))
                dummy->GetModifier()->m_amount = damageInfo.damage;

        // Divine Storm (use m_healthLeech to store damage for all targets)
        if (m_spellInfo->Id == 53385)
            m_healthLeech += damageInfo.damage / 4;

        /* process anticheat check */
        if (caster->GetObjectGuid().IsPlayer())
            ((Player*)caster)->GetAntiCheat()->DoAntiCheatCheck(CHECK_DAMAGE_SPELL, m_spellInfo->Id, 0, damageInfo.damage);

        caster->DealSpellDamage(&damageInfo, true);

        // Scourge Strike, here because needs to use final damage in second part of the spell
        if (classOpt && classOpt->SpellFamilyName == SPELLFAMILY_DEATHKNIGHT && classOpt->SpellFamilyFlags & UI64LIT(0x0800000000000000))
        {
            uint32 count = 0;
            Unit::SpellAuraHolderMap const& auras = unitTarget->GetSpellAuraHolderMap();
            for(Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
            {
                if(itr->second->GetSpellProto()->GetDispel() == DISPEL_DISEASE &&
                    itr->second->GetCasterGuid() == caster->GetObjectGuid())
                    ++count;
            }

            if (count)
            {
                int32 bp = count * CalculateDamage(EFFECT_INDEX_2, unitTarget) * damageInfo.damage / 100;
                if (bp)
                    caster->CastCustomSpell(unitTarget, 70890, &bp, NULL, NULL, true);
            }
        }
    }
    // Passive spell hits/misses or active spells only misses (only triggers if proc flags set)
    else if (procAttacker || procVictim)
    {
        // Fill base damage struct (unitTarget - is real spell target)
        SpellNonMeleeDamage damageInfo(caster, unitTarget, m_spellInfo->Id, m_spellSchoolMask);
        procEx = createProcExtendMask(&damageInfo, missInfo);
        // Do triggers for unit (reflect triggers passed on hit phase for correct drop charge)
        if (m_canTrigger && missInfo != SPELL_MISS_REFLECT)
            caster->ProcDamageAndSpell(unitTarget, real_caster ? procAttacker : uint32(PROC_FLAG_NONE), procVictim, procEx, 0, 0, m_attackType, m_spellInfo);
    }

    // trigger weapon enchants for weapon based spells; exclude spells that stop attack, because may break CC
    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->GetEquippedItemClass() == ITEM_CLASS_WEAPON &&
        !m_spellInfo->HasAttribute(SPELL_ATTR_STOP_ATTACK_TARGET))
        ((Player*)m_caster)->CastItemCombatSpell(unitTarget, m_attackType);

    // Call scripted function for AI if this spell is casted upon a creature
    if (unit->GetTypeId() == TYPEID_UNIT)
    {
        // cast at creature (or GO) quest objectives update at successful cast finished (+channel finished)
        // ignore pets or autorepeat/melee casts for speed (not exist quest for spells (hm... )
        if (real_caster && !((Creature*)unit)->IsPet() && !IsAutoRepeat() && !IsNextMeleeSwingSpell() && !IsChannelActive())
            if (Player* p = real_caster->GetCharmerOrOwnerPlayerOrPlayerItself())
                p->RewardPlayerAndGroupAtCast(unit, m_spellInfo->Id);

        if(((Creature*)unit)->AI())
            ((Creature*)unit)->AI()->SpellHit(m_caster, m_spellInfo);
    }

    // Call scripted function for AI if this spell is casted by a creature
    if (m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->AI())
        ((Creature*)m_caster)->AI()->SpellHitTarget(unit, m_spellInfo);
    if (real_caster && real_caster != m_caster && real_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)real_caster)->AI())
        ((Creature*)real_caster)->AI()->SpellHitTarget(unit, m_spellInfo);
}

void Spell::DoSpellHitOnUnit(Unit *unit, uint32 effectMask)
{
    if (!unit || !effectMask)
        return;

    Unit* realCaster = GetAffectiveCaster();

    // Recheck effect immune (only for delayed spells)
    if (m_spellInfo->speed > 0.0f)
    {
        for (int effIdx = 0; effIdx < MAX_EFFECT_INDEX; ++effIdx)
        {
            if (effectMask & (1 << effIdx))
                if (unit->IsImmuneToSpellEffect(m_spellInfo, SpellEffectIndex(effIdx), unit == realCaster))
                    effectMask &= ~(1 << effIdx);
        }
    }

    if (realCaster)
    {
        // Exorcism
        if (m_spellInfo->Id == 879)
        {
            // Glyph of Exorcism
            if (!realCaster->HasAura(54934))
                effectMask &= ~(1 << 1);
        }
        // Concussive Shot
        else if (m_spellInfo->Id == 5116)
        {
            // Glyph of Concussive Shot
            if (!realCaster->HasAura(56851))
                effectMask &= ~(1 << 1);
        }
        // Fear
        else if (m_spellInfo->Id == 5782)
        {
            // Glyph of Fear
            if (!realCaster->HasAura(56244))
                effectMask &= ~(1 << 2);
        }
        // Psyhic Scream
        else if (m_spellInfo->Id == 8122)
        {
            // Glyph of Psyhic Scream
            if (!realCaster->HasAura(55676))
                effectMask &= ~(1 << 2);
        }
        // Frostfire Bolt
        else if (m_spellInfo->Id == 44614)
        {
            // Glyph of Frostfire
            if (!realCaster->HasAura(61205))
                effectMask &= ~(1 << 2);
            else
                effectMask &= ~(1 << 0);
        }
        // Word of Glory
        else if (m_spellInfo->Id == 85673)
        {
            // Glyph of the Long Word
            if (!realCaster->HasAura(93466))
                effectMask &= ~(1 << 1);
        }
    }

    // Recheck immune (only for delayed spells)
    if (m_spellInfo->speed > 0.0f && !IsPositiveSpell(m_spellInfo->Id) &&
                (unit->IsImmunedToDamage(GetSpellSchoolMask(m_spellInfo)) ||
                unit->IsImmuneToSpell(m_spellInfo, unit == realCaster)))
    {
        if (realCaster)
            realCaster->SendSpellMiss(unit, m_spellInfo->Id, SPELL_MISS_IMMUNE);

        ResetEffectDamageAndHeal();
        return;
    }

    if (unit->GetTypeId() == TYPEID_PLAYER && unit->IsInWorld())
    {
        ((Player*)unit)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, m_spellInfo->Id);
        ((Player*)unit)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET2, m_spellInfo->Id);
    }

    if (realCaster && realCaster->GetTypeId() == TYPEID_PLAYER && unit->IsInWorld())
        ((Player*)realCaster)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL2, m_spellInfo->Id, 0, unit);

    if (realCaster && realCaster != unit)
    {
        // Recheck  UNIT_FLAG_NON_ATTACKABLE for delayed spells
        if (m_spellInfo->speed > 0.0f &&
            unit->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) &&
            unit->GetCharmerOrOwnerGuid() != m_caster->GetObjectGuid())
        {
            realCaster->SendSpellMiss(unit, m_spellInfo->Id, SPELL_MISS_EVADE);
            ResetEffectDamageAndHeal();
            return;
        }

        if (!realCaster->IsFriendlyTo(unit))
        {
            // not break stealth by cast targeting
            // Amaru: Sap always breaks stealth
            if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX_NOT_BREAK_STEALTH) || m_spellInfo->GetMechanic() == MECHANIC_SAPPED)
            {
                //Amaru: check for spells that never break stealth
                if (m_spellInfo->Id != 32375 && m_spellInfo->Id != 32592 && m_spellInfo->Id != 72734 && m_spellInfo->Id != 39897 //Mass Dispels
                    && m_spellInfo->Id != 3600 //Earthbind
                    && !m_spellInfo->IsFitToFamily(SPELLFAMILY_WARLOCK, UI64LIT(0x0000000002000000)) // Voidwalker Suffering
                    )
                    unit->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            }

            // can cause back attack (if detected), stealth removed at Spell::cast if spell break it
            if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX3_NO_INITIAL_AGGRO) && !m_spellInfo->HasAttribute(SPELL_ATTR_EX_NO_THREAT) &&
                (!IsPositiveSpell(m_spellInfo->Id) || IsDispelSpell(m_spellInfo) && m_caster->IsHostileTo(unit)) && m_caster->isVisibleForOrDetect(unit, unit, false))
            {
                // use speedup check to avoid re-remove after above lines
                if (m_spellInfo->HasAttribute(SPELL_ATTR_EX_NOT_BREAK_STEALTH) && !IsDispelSpell(m_spellInfo))
                    unit->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                // caster can be detected but have stealth aura
                m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);

                if (!unit->IsStandState() && !unit->hasUnitState(UNIT_STAT_STUNNED))
                    unit->SetStandState(UNIT_STAND_STATE_STAND);

                if (!unit->isInCombat())
                    unit->AttackedBy(realCaster);

                unit->AddThreat(realCaster);
                unit->SetInCombatWith(realCaster);
                realCaster->SetInCombatWith(unit);

                if (Player* attackedPlayer = unit->GetCharmerOrOwnerPlayerOrPlayerItself())
                    realCaster->SetContestedPvP(attackedPlayer);
            }
        }
        else
        {
            // for delayed spells ignore negative spells (after duel end) for friendly targets
            if (m_spellInfo->speed > 0.0f && !IsPositiveSpell(m_spellInfo->Id))
            {
                realCaster->SendSpellMiss(unit, m_spellInfo->Id, SPELL_MISS_EVADE);
                ResetEffectDamageAndHeal();
                return;
            }

            // assisting case, healing and resurrection
            if (unit->hasUnitState(UNIT_STAT_ATTACK_PLAYER))
                realCaster->SetContestedPvP();

            if (unit->isInCombat() && !m_spellInfo->HasAttribute(SPELL_ATTR_EX3_NO_INITIAL_AGGRO) && !m_spellInfo->HasAttribute(SPELL_ATTR_EX_NO_THREAT))
            {
                realCaster->SetInCombatState(unit->GetCombatTimer() > 0);
                unit->getHostileRefManager().threatAssist(realCaster, 0.0f, m_spellInfo);
            }
        }
    }

    // Get Data Needed for Diminishing Returns, some effects may have multiple auras, so this must be done on spell hit, not aura add
    m_diminishGroup = GetDiminishingReturnsGroupForSpell(m_spellInfo,m_triggeredByAuraSpell);
    m_diminishLevel = unit->GetDiminishing(m_diminishGroup);
    // Increase Diminishing on unit, current informations for actually casts will use values above
    if ((GetDiminishingReturnsGroupType(m_diminishGroup) == DRTYPE_PLAYER && unit->GetCharmerOrOwnerPlayerOrPlayerItself()) ||
        GetDiminishingReturnsGroupType(m_diminishGroup) == DRTYPE_ALL)
        unit->IncrDiminishing(m_diminishGroup);

    // Apply additional spell effects to target
    CastPreCastSpells(unit);

    //SpellEntry const * originalSpellInfo = m_spellInfo;
    if (IsSpellAppliesAura(m_spellInfo, effectMask))
    {
        //if (m_caster->GetTypeId() == TYPEID_PLAYER)
        //{
        //    if (SpellEntry const * actualSpellInfo = sSpellMgr.SelectAuraRankForLevel(m_spellInfo, unit->getLevel()))
        //        m_spellInfo = actualSpellInfo;
        //    else
        //        return;
        //}

        m_spellAuraHolder = CreateSpellAuraHolder(m_spellInfo, unit, realCaster, m_CastItem);
        m_spellAuraHolder->setDiminishGroup(m_diminishGroup);
    }
    else
        m_spellAuraHolder = NULL;

    for(int effectNumber = 0; effectNumber < MAX_EFFECT_INDEX; ++effectNumber)
    {
        if (effectMask & (1 << effectNumber))
        {
            SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(effectNumber));
            HandleEffects(unit, NULL, NULL, SpellEffectIndex(effectNumber), m_damageMultipliers[effectNumber]);
            if ( m_applyMultiplierMask & (1 << effectNumber) )
            {
                // Get multiplier
                float multiplier = spellEffect ? spellEffect->DmgMultiplier : 1.0f;
                // Apply multiplier mods
                if (realCaster)
                    if(Player* modOwner = realCaster->GetSpellModOwner())
                        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_EFFECT_PAST_FIRST, multiplier, this);
                m_damageMultipliers[effectNumber] *= multiplier;
            }
        }
    }

    // now apply all created auras
    if (m_spellAuraHolder)
    {
        // normally shouldn't happen
        if (!m_spellAuraHolder->IsEmptyHolder())
        {
            int32 duration = m_spellAuraHolder->GetAuraMaxDuration();
            int32 originalDuration = duration;

            if (duration > 0)
            {
                int32 limitduration = GetDiminishingReturnsLimitDuration(m_diminishGroup, m_spellInfo);
                unit->ApplyDiminishingToDuration(m_diminishGroup, duration, m_caster, m_diminishLevel, limitduration, m_spellFlags & SPELL_FLAG_REFLECTED);

                // Fully diminished
                if (duration == 0)
                {
                    delete m_spellAuraHolder;
                    //m_spellInfo = originalSpellInfo;
                    return;
                }
            }

            duration = unit->CalculateAuraDuration(m_spellInfo, effectMask, duration, m_caster, this);

            if (duration != originalDuration)
            {
                m_spellAuraHolder->SetAuraMaxDuration(duration);
                m_spellAuraHolder->SetAuraDuration(duration);
            }

            unit->AddSpellAuraHolder(m_spellAuraHolder);
        }
        else
            delete m_spellAuraHolder;
    }
    //m_spellInfo = originalSpellInfo;
}

void Spell::DoAllEffectOnTarget(GOTargetInfo *target)
{
    if (target->processed)                                  // Check target
        return;

    target->processed = true;                               // Target checked in apply effects procedure

    uint32 effectMask = target->effectMask;
    if(!effectMask)
        return;

    GameObject* go = m_caster->GetMap()->GetGameObject(target->targetGUID);
    if(!go)
        return;

    for(int effectNumber = 0; effectNumber < MAX_EFFECT_INDEX; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(NULL, NULL, go, SpellEffectIndex(effectNumber));

    // cast at creature (or GO) quest objectives update at successful cast finished (+channel finished)
    // ignore autorepeat/melee casts for speed (not exist quest for spells (hm... )
    if( !IsAutoRepeat() && !IsNextMeleeSwingSpell() && !IsChannelActive() )
    {
        if ( Player* p = m_caster->GetCharmerOrOwnerPlayerOrPlayerItself() )
            p->RewardPlayerAndGroupAtCast(go, m_spellInfo->Id);
    }
}

void Spell::DoAllEffectOnTarget(ItemTargetInfo *target)
{
    uint32 effectMask = target->effectMask;
    if(!target->item || !effectMask)
        return;

    for(int effectNumber = 0; effectNumber < MAX_EFFECT_INDEX; ++effectNumber)
        if (effectMask & (1 << effectNumber))
            HandleEffects(NULL, target->item, NULL, SpellEffectIndex(effectNumber));
}

void Spell::HandleDelayedSpellLaunch(TargetInfo *target)
{
     // Get mask of effects for target
    uint32 mask = target->effectMask;

    Unit* unit = m_caster->GetObjectGuid() == target->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target->targetGUID);
    if (!unit)
        return;

    // Get original caster (if exist) and calculate damage/healing from him data
    Unit *real_caster = GetAffectiveCaster();
    // FIXME: in case wild GO heal/damage spells will be used target bonuses
    Unit *caster = real_caster ? real_caster : m_caster;

    SpellMissInfo missInfo = target->missCondition;
    // Need init unitTarget by default unit (can changed in code on reflect)
    // Or on missInfo!=SPELL_MISS_NONE unitTarget undefined (but need in trigger subsystem)
    unitTarget = unit;

    // Reset damage/healing counter
    m_damage = 0;
    m_healing = 0; // healing maybe not needed at this point

    if (missInfo == SPELL_MISS_REFLECT)
        unitTarget = caster;

    // Fill base damage struct (unitTarget - is real spell target)
    SpellNonMeleeDamage damageInfo(caster, unitTarget, m_spellInfo->Id, m_spellSchoolMask);

    // keep damage amount for reflected spells
    if (missInfo == SPELL_MISS_NONE || (missInfo == SPELL_MISS_REFLECT && target->reflectResult == SPELL_MISS_NONE))
    {
        for (int32 effectNumber = 0; effectNumber < MAX_EFFECT_INDEX; ++effectNumber)
        {
            if (mask & (1 << effectNumber) && IsEffectHandledOnDelayedSpellLaunch(m_spellInfo, SpellEffectIndex(effectNumber)))
            {
                SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(effectNumber));
                HandleEffects(unit, NULL, NULL, SpellEffectIndex(effectNumber), m_damageMultipliers[effectNumber]);
                if ( m_applyMultiplierMask & (1 << effectNumber) )
                {
                    // Get multiplier
                    float multiplier = spellEffect ? spellEffect->DmgMultiplier : 1.0f;
                    // Apply multiplier mods
                    if (real_caster)
                        if(Player* modOwner = real_caster->GetSpellModOwner())
                            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_EFFECT_PAST_FIRST, multiplier, this);
                    m_damageMultipliers[effectNumber] *= multiplier;
                }
            }
        }

        if (m_damage > 0)
            caster->CalculateSpellDamage(&damageInfo, m_damage, m_spellInfo, m_attackType);
    }

    target->damage = damageInfo.damage;
    target->HitInfo = damageInfo.HitInfo;
}

void Spell::InitializeDamageMultipliers()
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
        if(!spellEffect)
            continue;
        if (spellEffect->Effect == 0)
            continue;

        uint32 EffectChainTarget = spellEffect->EffectChainTarget;
        if (Unit* realCaster = GetAffectiveCaster())
            if(Player* modOwner = realCaster->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_JUMP_TARGETS, EffectChainTarget, this);

        m_damageMultipliers[i] = 1.0f;
        if( (spellEffect->EffectImplicitTargetA == TARGET_CHAIN_DAMAGE || spellEffect->EffectImplicitTargetA == TARGET_CHAIN_HEAL) &&
            (EffectChainTarget > 1) )
            m_applyMultiplierMask |= (1 << i);
    }
}

bool Spell::IsAliveUnitPresentInTargetList()
{
    // Not need check return true
    if (m_needAliveTargetMask == 0)
        return true;

    uint8 needAliveTargetMask = m_needAliveTargetMask;

    for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition == SPELL_MISS_NONE && (needAliveTargetMask & ihit->effectMask))
        {
            Unit *unit = m_caster->GetObjectGuid() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);

            // either unit is alive and normal spell, or unit dead and deathonly-spell
            if (unit && (unit->isAlive() != IsDeathOnlySpell(m_spellInfo)))
                needAliveTargetMask &= ~ihit->effectMask;   // remove from need alive mask effect that have alive target
        }
    }

    // is all effects from m_needAliveTargetMask have alive targets
    return needAliveTargetMask == 0;
}

// Helper for Chain Healing
// Spell target first
// Raidmates then descending by injury suffered (MaxHealth - Health)
// Other players/mobs then descending by injury suffered (MaxHealth - Health)
struct ChainHealingOrder : public std::binary_function<const Unit*, const Unit*, bool>
{
    const Unit* MainTarget;
    ChainHealingOrder(Unit const* Target) : MainTarget(Target) {};
    // functor for operator ">"
    bool operator()(Unit const* _Left, Unit const* _Right) const
    {
        return (ChainHealingHash(_Left) < ChainHealingHash(_Right));
    }
    int32 ChainHealingHash(Unit const* Target) const
    {
        if (Target == MainTarget)
            return 0;
        else if (Target->GetTypeId() == TYPEID_PLAYER && MainTarget->GetTypeId() == TYPEID_PLAYER &&
            ((Player const*)Target)->IsInSameRaidWith((Player const*)MainTarget))
        {
            if (Target->GetHealth() == Target->GetMaxHealth())
                return 40000;
            else
                return 20000 - Target->GetMaxHealth() + Target->GetHealth();
        }
        else
            return 40000 - Target->GetMaxHealth() + Target->GetHealth();
    }
};

class ChainHealingFullHealth: std::unary_function<const Unit*, bool>
{
    public:
        const Unit* MainTarget;
        ChainHealingFullHealth(const Unit* Target) : MainTarget(Target) {};

        bool operator()(const Unit* Target)
        {
            return (Target != MainTarget && Target->GetHealth() == Target->GetMaxHealth());
        }
};

// Helper for targets nearest to the spell target
// The spell target is always first unless there is a target at _completely_ the same position (unbelievable case)
struct TargetDistanceOrderNear : public std::binary_function<const Unit, const Unit, bool>
{
    const Unit* MainTarget;
    TargetDistanceOrderNear(const Unit* Target) : MainTarget(Target) {};
    // functor for operator ">"
    bool operator()(const Unit* _Left, const Unit* _Right) const
    {
        return MainTarget->GetDistanceOrder(_Left, _Right);
    }
};

// Helper for targets furthest away to the spell target
// The spell target is always first unless there is a target at _completely_ the same position (unbelievable case)
struct TargetDistanceOrderFarAway : public std::binary_function<const Unit, const Unit, bool>
{
    const Unit* MainTarget;
    TargetDistanceOrderFarAway(const Unit* Target) : MainTarget(Target) {};
    // functor for operator "<"
    bool operator()(const Unit* _Left, const Unit* _Right) const
    {
        return !MainTarget->GetDistanceOrder(_Left, _Right);
    }
};

void Spell::SetTargetMap(SpellEffectIndex effIndex, uint32 targetMode, UnitList& targetUnitMap)
{
    DEBUG_LOG("Spell::SetTargetMap for spell %u eff %u targetMode %u",
        m_spellInfo->Id, effIndex, targetMode);

    SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(effIndex);
    SpellClassOptionsEntry const* classOpt = m_spellInfo->GetSpellClassOptions();

    if (!spellEffect)
        return;

    float radius;
    if (uint32 radiusIndex = spellEffect->GetRadiusIndex())
        radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(radiusIndex));
    else
        radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

    uint32 EffectChainTarget = spellEffect ? spellEffect->EffectChainTarget : 0;

    if (Unit* realCaster = GetAffectiveCaster())
    {
        if(Player* modOwner = realCaster->GetSpellModOwner())
        {
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius, this);
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_JUMP_TARGETS, EffectChainTarget, this);
        }
    }

    // Get spell max affected targets
    uint32 unMaxTargets = m_spellInfo->GetMaxAffectedTargets();

    // custom target amount cases
    switch (m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (m_spellInfo->Id)
            {
                case 802:                                   // Mutate Bug (AQ40, Emperor Vek'nilash)
                case 804:                                   // Explode Bug (AQ40, Emperor Vek'lor)
                case 23138:                                 // Gate of Shazzrah (MC, Shazzrah)
                case 28560:                                 // Summon Blizzard (Naxx, Sapphiron)
                case 30541:                                 // Blaze (Magtheridon)
                case 30572:                                 // Quake (Magtheridon)
                case 30769:                                 // Pick Red Riding Hood (Karazhan, Big Bad Wolf)
                case 30835:                                 // Infernal Relay (Karazhan, Prince Malchezaar)
                case 31347:                                 // Doom (Hyjal Summit, Azgalor)
                case 33711:                                 // Murmur's Touch (Shadow Labyrinth, Murmur)
                case 38794:                                 // Murmur's Touch (h) (Shadow Labyrinth, Murmur)
                case 40834:                                 // Agonizing Flames (BT, Illidan Stormrage)
                case 41537:                                 // Summon Enslaved Soul (BT, Reliquary of Souls)
                case 44869:                                 // Spectral Blast (SWP, Kalecgos)
                case 45892:                                 // Sinister Reflection (SWP, Kil'jaeden)
                case 45976:                                 // Open Portal (SWP, M'uru)
                case 47669:                                 // Awaken Subboss (Utgarde Pinnacle)
                case 48278:                                 // Paralyze (Utgarde Pinnacle)
                case 50742:                                 // Ooze Combine (Halls of Stone)
                case 50988:                                 // Glare of the Tribunal (Halls of Stone)
                case 51003:                                 // Summon Dark Matter Target (Halls of Stone)
                case 51146:                                 // Summon Searing Gaze Target (Halls Of Stone)
                case 52438:                                 // Summon Skittering Swarmer (Azjol Nerub,  Krik'thir the Gatewatcher)
                case 52449:                                 // Summon Skittering Infector (Azjol Nerub,  Krik'thir the Gatewatcher)
                case 53457:                                 // Impale (Azjol Nerub,  Anub'arak)
                case 54148:                                 // Ritual of the Sword (Utgarde Pinnacle, Svala)
                case 55479:                                 // Forced Obedience (Naxxramas, Razovius)
                case 56140:                                 // Summon Power Spark (Eye of Eternity, Malygos)
                case 59870:                                 // Glare of the Tribunal (h) (Halls of Stone)
                case 62016:                                 // Charge Orb (Ulduar, Thorim)
                case 62042:                                 // Stormhammer (Ulduar, Thorim)
                case 62166:                                 // Stone Grip (Ulduar, Kologarn)
                case 62301:                                 // Cosmic Smash (Ulduar, Algalon)
                case 62488:                                 // Activate Construct (Ulduar, Ignis)
                case 63018:                                 // Searing Light (Ulduar, XT-002)
                case 63024:                                 // Gravity Bomb (Ulduar, XT-002)
                case 63387:                                 // Rapid Burst
                case 63545:                                 // Icicle Hodir(trigger spell from 62227)
                case 63795:                                 // Psychosis (Ulduar, Yogg-Saron)
                case 63820:                                 // Summon Scrap Bot Trigger (Ulduar, Mimiron) use for Scrap Bots, hits npc 33856
                case 64218:                                 // Overcharge (VoA, Emalon)
                case 64234:                                 // Gravity Bomb (h) (Ulduar, XT-002)
                case 64425:                                 // Summon Scrap Bot Trigger (Ulduar, Mimiron) use for Assault Bots, hits npc 33856
                case 64531:                                 // Rapid Burst (h)
                case 65121:                                 // Searing Light (h) (Ulduar, XT-002)
                case 65301:                                 // Psychosis (Ulduar, Yogg-Saron)
                case 65872:                                 // Pursuing Spikes (ToCrusader, Anub'arak)
                case 65950:                                 // Touch of Light (ToCrusader, Val'kyr Twins)
                case 66001:                                 // Touch of Darkness (ToCrusader, Val'kyr Twins)
                case 66152:                                 // Bullet Controller Summon Periodic Trigger Light (ToCrusader)
                case 66153:                                 // Bullet Controller Summon Periodic Trigger Dark (ToCrusader)
                case 66332:                                 // Nerubian Burrower (Mode 0) (ToCrusader, Anub'arak)
                case 66336:                                 // Mistress' Kiss (ToCrusader, Jaraxxus)
                case 66339:                                 // Summon Scarab (ToCrusader, Anub'arak)
                case 67077:                                 // Mistress' Kiss (Mode 2) (ToCrusader, Jaraxxus)
                case 67281:                                 // Touch of Darkness (Mode 1)
                case 67282:                                 // Touch of Darkness (Mode 2)
                case 67283:                                 // Touch of Darkness (Mode 3)
                case 67296:                                 // Touch of Light (Mode 1)
                case 67297:                                 // Touch of Light (Mode 2)
                case 67298:                                 // Touch of Light (Mode 3)
                case 68912:                                 // Wailing Souls (FoS)
                case 68950:                                 // Fear (FoS)
                case 69048:                                 // Mirrored Soul (FoS)
                case 69057:                                 // Bone Spike Graveyard (Icecrown Citadel, Lord Marrowgar) 10 man
                case 72088:
                case 73142:
                case 73144:
                case 69140:                                 // Coldflame (ICC, Marrowgar)
                case 69674:                                 // Mutated Infection (ICC, Rotface)
                case 70450:                                 // Blood Mirror
                case 70837:                                 // Blood Mirror
                case 70882:                                 // Slime Spray Summon Trigger (ICC, Rotface)
                case 70920:                                 // Unbound Plague Search Effect (ICC, Putricide)
                case 71224:                                 // Mutated Infection (Mode 1)
                case 71445:                                 // Twilight Bloodbolt
                case 71471:                                 // Twilight Bloodbolt
                case 71610:                                 // Echoes of Light
                case 71641:                                 // Echoes of Light
                case 71837:                                 // Vampiric Bite
                case 71861:                                 // Swarming Shadows
                case 72091:                                 // Frozen Orb (Vault of Archavon, Toravon)
                case 73022:                                 // Mutated Infection (Mode 2)
                case 73023:                                 // Mutated Infection (Mode 3)
                case 105065:                                // Seething Hate
                case 106248:                                // Blade Dance
                case 108090:                                // Seething Hate (H)
                    unMaxTargets = 1;
                    break;
                case 28542:                                 // Life Drain (Naxx, Sapphiron)
                case 62476:                                 // Icicle (Hodir 10man)
                case 63476:                                 // Icicle (Hodir - 10)
                case 66013:                                 // Penetrating Cold (10 man) (ToCrusader, Anub'arak)
                case 67755:                                 // Nerubian Burrower (Mode 1) (ToCrusader, Anub'arak)
                case 67756:                                 // Nerubian Burrower (Mode 2) (ToCrusader, Anub'arak)
                case 68509:                                 // Penetrating Cold (10 man heroic)
                case 69195:                                 // Pungent Blight (10 man)
                case 69278:                                 // Gas spore (ICC, Festergut)
                case 70341:                                 // Slime Puddle (ICC, Putricide)
                case 71219:                                 // Pungent Blight (10 man heroic)
                case 71336:                                 // Pact of the Darkfallen
                case 71390:                                 // Pact of the Darkfallen
                case 73031:                                 // Pungent Blight (25 man)
                case 73032:                                 // Pungent Blight (25 man heroic)
                    unMaxTargets = 2;
                    break;
                case 28796:                                 // Poison Bolt Volley (Naxx, Faerlina)
                case 29213:                                 // Curse of the Plaguebringer (Naxx, Noth the Plaguebringer)
                case 30004:                                 // Flame Wreath (Karazhan, Shade of Aran)
                case 31298:                                 // Sleep (Hyjal Summit, Anetheron)
                case 39992:                                 // Needle Spine Targeting (BT, Warlord Najentus)
                case 41303:                                 // Soul Drain (BT, Reliquary of Souls)
                case 41376:                                 // Spite (BT, Reliquary of Souls)
                case 51904:                                 // Summon Ghouls On Scarlet Crusade
                case 54522:                                 // Summon Ghouls On Scarlet Crusade
                case 60936:                                 // Surge of Power (h) (Malygos)
                case 61693:                                 // Arcane Storm (Malygos)
                case 62477:                                 // Icicle (Hodir 25man)
                case 63981:                                 // StoneGrip (h) (Ulduar, Kologarn)
                case 64598:                                 // Cosmic Smash (h) (Ulduar, Algalon)
                case 64620:                                 // Summon Fire Bot Trigger (Ulduar, Mimiron) hits npc 33856
                case 69055:                                 // Bone Slice (Icecrown Citadel, Lord Marrowgar, 10)
                case 70814:                                 // Bone Slice (ICC, Lord Marrowgar, heroic)
                case 72095:                                 // Frozen Orb (h) (Vault of Archavon, Toravon)
                case 72089:                                 // Bone Spike Graveyard (Icecrown Citadel, Lord Marrowgar) 25 man
                case 70826:
                case 73143:
                case 73145:
                    unMaxTargets = 3;
                    break;
                case 61916:                                 // Lightning Whirl (Stormcaller Brundir - Ulduar)
                    unMaxTargets = urand(2, 3);
                    break;
                case 46771:                                 // Flame Sear (SWP, Grand Warlock Alythess)
                    unMaxTargets = urand(3, 5);
                    break;
                case 63482:                                 // Lightning Whirl (h) (Stormcaller Brundir - Ulduar)
                    unMaxTargets = urand(3, 6);
                    break;
                case 37676:                                 // Insidious Whisper (SSC, Leotheras the Blind)
                case 38028:                                 // Watery Grave (SSC, Morogrim Tidewalker)
                case 67757:                                 // Nerubian Burrower (Mode 3) (ToCrusader, Anub'arak)
                case 71221:                                 // Gas spore (Mode 1) (ICC, Festergut)
                    unMaxTargets = 4;
                    break;
                case 30843:                                 // Enfeeble (Karazhan, Prince Malchezaar)
                case 40243:                                 // Crushing Shadows (BT, Teron Gorefiend)
                case 42005:                                 // Bloodboil (BT, Gurtogg Bloodboil)
                case 45641:                                 // Fire Bloom (SWP, Kil'jaeden)
                case 55665:                                 // Life Drain (h) (Naxx, Sapphiron)
                case 58917:                                 // Consume Minions
                case 64604:                                 // Nature Bomb Freya
                case 67076:                                 // Mistress' Kiss (Trial of the Crusader, ->
                case 67078:                                 // -> Lord Jaraxxus encounter, 25 and 25 heroic)
                case 67700:                                 // Penetrating Cold (25 man)
                case 68510:                                 // Penetrating Cold (25 man, heroic)
                    unMaxTargets = 5;
                    break;
                case 61694:                                 // Arcane Storm(H) (25 man) (Malygos)
                    unMaxTargets = 7;
                    break;
                case 38054:                                 // Random Rocket Missile
                    unMaxTargets = 8;
                    break;
                case 54098:                                 // Poison Bolt Volley (h) (Naxx, Faerlina)
                case 54835:                                 // Curse of the Plaguebringer (h) (Naxx, Noth the Plaguebringer)
                    unMaxTargets = 10;
                    break;
                case 25991:                                 // Poison Bolt Volley (AQ40, Pincess Huhuran)
                case 85547:                                 // Jinx: Curse of the Elements
                case 86105:                                 // Jinx: Curse of the Elements
                    unMaxTargets = 15;
                    break;
                case 71340:                                 // Pact of darkfallen (hack for script work)
                    unMaxTargets = 1;
                    break;
                case 74452:                                 // Conflagration (Saviana, Ruby Sanctum)
                {
                    if (m_caster)
                    {
                        switch (m_caster->GetMap()->GetDifficulty())
                        {
                            case RAID_DIFFICULTY_10MAN_NORMAL:
                            case RAID_DIFFICULTY_10MAN_HEROIC:
                                unMaxTargets = 2;
                                break;
                            case RAID_DIFFICULTY_25MAN_NORMAL:
                            case RAID_DIFFICULTY_25MAN_HEROIC:
                                unMaxTargets = 5;
                                break;
                        }
                    }
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch(m_spellInfo->Id)
            {
                case 38194:                     // Blink
                case 82734:                     // Flame Orb
                case 84718:                     // Frostfire Orb
                    unMaxTargets = 1;
                    break;
                case 83154:                     // Piercing Chill
                    unMaxTargets = m_caster->HasAura(83157) ? 2 : 1;
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Sunder Armor (main spell)
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000000000004000), 0x00000000) && m_spellInfo->SpellVisual[0] == 406)
                if (m_caster->HasAura(58387))               // Glyph of Sunder Armor
                    EffectChainTarget = 2;
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Starfall
            if (m_spellInfo->IsFitToFamilyMask(UI64LIT(0x0000000000000000), 0x00000100))
                unMaxTargets = 2;
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Multi-Shot
            if (m_spellInfo->Id == 2643)
                unMaxTargets = 3;
            break;
        }
        case SPELLFAMILY_PALADIN:
            if (m_spellInfo->Id == 20424)                   // Seals of Command (2 more targets for Seal of Righteousness Proc)
            {
                // Seal of Righteousness
                if (m_caster->HasAura(20154))
                    EffectChainTarget = 3;
                else
                    EffectChainTarget = 1;
            }
            // Light of Dawn
            else if (m_spellInfo->Id == 85222)
            {
                unMaxTargets = 6;
                // Glyph of Light of Dawn
                if (Aura* aura = m_caster->GetDummyAura(54940))
                    unMaxTargets -= aura->GetModifier()->m_amount;
            }
            break;
        case SPELLFAMILY_DEATHKNIGHT:
        {
            if (m_spellInfo->SpellIconID == 1737)           // Corpse Explosion
                unMaxTargets = 1;
            break;
        }
        default:
            break;
    }

    // custom radius cases
    switch (m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(m_spellInfo->Id)
            {
                case 24811:                                 // Draw Spirit (Lethon)
                {
                    if (effIndex == EFFECT_INDEX_0)         // Copy range from EFF_1 to 0
                        radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellEffect->GetRadiusIndex()));
                    break;
                }
                case 28241:                                 // Grobbulus Slime pool
                case 54363:                                 // Grobbulus Slime pool H
                {
                    uint32 auraId = (m_spellInfo->Id == 28241 ? 28158 : 54362);
                    if (SpellAuraHolder* auraHolder = m_caster->GetSpellAuraHolder(auraId))
                        radius = 0.5f * (60000 - auraHolder->GetAuraDuration()) * 0.001f;
                    break;
                }
                case 56438:                                 // Arcane Overload
                    if (Unit* realCaster = GetAffectiveCaster())
                        radius = radius * realCaster->GetObjectScale();
                    break;
                case 69075:                                 // Bone Storm
                case 69832:                                 // Unstable Ooze Explosion (Rotface)
                case 70341:                                 // Slime Puddle (Putricide)
                case 70541:                                 // Infest (Lich King)
                case 73779:
                case 73780:
                case 73781:
                case 70834:                                 // Bone Storm
                case 70835:                                 // Bone Storm
                case 70836:                                 // Bone Storm
                case 71518:                                 // Unholy infusion credit
                case 72289:                                 // Frost infusion credit
                case 72706:                                 // Valithria event credit
                case 72769:                                 // Scent of Blood (Saurfang)
                case 72771:
                case 72934:                                 // Blood infusion credit
                    radius = DEFAULT_VISIBILITY_INSTANCE;
                    break;
                case 69845:                                 // Sindragosa Frost bomb (hack!)
                case 71053:
                case 71054:
                case 71055:
                    radius = 50;
                    break;
                case 72350:                                 // Fury of Frostmourne
                case 72351:                                 // Fury of Frostmourne
                    radius = 300;
                    break;
                case 72754:                                 // Defile. Radius depended from scale.
                case 73708:                                 // Defile 25
                case 73709:                                 // Defile 10H
                case 73710:                                 // Defile 25H
                    if (Unit* realCaster = GetAffectiveCaster())
                        radius = realCaster->GetObjectScale() * 6;
                    break;
                case 66881:                                 // Slime Pool (ToCrusader, Acidmaw & Dreadscale)
                case 67638:                                 // Slime Pool (ToCrusader, Acidmaw & Dreadscale) (Mode 1)
                case 67639:                                 // Slime Pool (ToCrusader, Acidmaw & Dreadscale) (Mode 2)
                case 67640:                                 // Slime Pool (ToCrusader, Acidmaw & Dreadscale) (Mode 3)
                    if (SpellAuraHolder* auraHolder = m_caster->GetSpellAuraHolder(66882))
                        radius = 0.5f * (60000 - auraHolder->GetAuraDuration()) * 0.001f;
                    break;
                case 67732:                                 // Destroy all Frost Patches (Trial of the Crusader, Anub'arak)
                    radius = 9.0f;
                    break;
                case 74641:                                 // Meteor Strike
                    radius = 3.0f;
                    break;
                case 69057:                                 // Bone Spike Graveyard (Icecrown Citadel, Lord Marrowgar encounter, 10N)
                case 70826:                                 // Bone Spike Graveyard (Icecrown Citadel, Lord Marrowgar encounter, 25N)
                case 72088:                                 // Bone Spike Graveyard (Icecrown Citadel, Lord Marrowgar encounter, 10H)
                case 72089:                                 // Bone Spike Graveyard (Icecrown Citadel, Lord Marrowgar encounter, 25H)
                case 73142:                                 // Bone Spike Graveyard (during Bone Storm) (Icecrown Citadel, Lord Marrowgar encounter, 10N)
                case 73143:                                 // Bone Spike Graveyard (during Bone Storm) (Icecrown Citadel, Lord Marrowgar encounter, 25N)
                case 73144:                                 // Bone Spike Graveyard (during Bone Storm) (Icecrown Citadel, Lord Marrowgar encounter, 10H)
                case 73145:                                 // Bone Spike Graveyard (during Bone Storm) (Icecrown Citadel, Lord Marrowgar encounter, 25H)
                     radius = DEFAULT_VISIBILITY_INSTANCE;
                     break;
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }

    std::list<GameObject*> tempTargetGOList;

    switch (targetMode)
    {
        case TARGET_RANDOM_NEARBY_LOC:
            // Get a random point in circle. Use sqrt(rand) to correct distribution when converting polar to Cartesian coordinates.
            radius *= sqrtf(rand_norm_f());
            // no 'break' expected since we use code in case TARGET_RANDOM_CIRCUMFERENCE_POINT!!!
        case TARGET_RANDOM_CIRCUMFERENCE_POINT:
        {
            // Get a random point AT the circumference
            float angle = 2.0f * M_PI_F * rand_norm_f();
            float dest_x, dest_y, dest_z;
            m_caster->GetClosePoint(dest_x, dest_y, dest_z, 0.0f, radius, angle);
            m_targets.setDestination(dest_x, dest_y, dest_z);

            // This targetMode is often used as 'last' implicitTarget for positive spells, that just require coordinates
            // and no unitTarget (e.g. summon effects). As MaNGOS always needs a unitTarget we add just the caster here.
            if (IsPositiveSpell(m_spellInfo))
                targetUnitMap.push_back(m_caster);
            break;
        }
        case TARGET_91:
        case TARGET_RANDOM_NEARBY_DEST:
        {
            // Get a random point IN the CIRCEL around current M_TARGETS COORDINATES(!).
            if (radius > 0.0f)
           {
                // Use sqrt(rand) to correct distribution when converting polar to Cartesian coordinates.
                radius *= sqrtf(rand_norm_f());
                float angle = 2.0f * M_PI_F * rand_norm_f();
                float dest_x = m_targets.m_destX + cos(angle) * radius;
                float dest_y = m_targets.m_destY + sin(angle) * radius;
                float dest_z = m_caster->GetPositionZ();
                if (!MapManager::IsValidMapCoord(m_caster->GetMapId(), dest_x, dest_y, dest_z))
                {
                    sLog.outError("Spell::SetTargetMap: invalid map coordinates for spell %u eff_idx %u target mode %u: mapid %u x %f y %f z %f\n" 
                        "spell radius: %f caster position: x %f y %f z %f\n"
                        "base dest position: x %f y %f z %f",
                        m_spellInfo->Id, effIndex, targetMode, m_caster->GetMapId(), dest_x, dest_y, dest_z,
                        radius, m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(),
                        m_targets.m_destX, m_targets.m_destY, m_caster->GetPositionZ());
                    m_targets.setDestination(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ());
                }
                else
                {
                    m_caster->UpdateGroundPositionZ(dest_x, dest_y, dest_z);
                    m_targets.setDestination(dest_x, dest_y, dest_z);
                }
            }

            // This targetMode is often used as 'last' implicitTarget for positive spells, that just require coordinates
            // and no unitTarget (e.g. summon effects). As MaNGOS always needs a unitTarget we add just the caster here.
            if (targetMode == TARGET_91 || IsPositiveSpell(m_spellInfo))
                targetUnitMap.push_back(m_caster);
            // This targetMode is often used as 'last' implicitTarget for positive spells, that just require coordinates
            // and no unitTarget (e.g. summon effects). As MaNGOS always needs a unitTarget we add just the caster here.

            // Prevent multiple summons
            if (spellEffect->Effect == SPELL_EFFECT_SUMMON || spellEffect->Effect == SPELL_EFFECT_SUMMON_OBJECT_WILD)
                unMaxTargets = 1;
            break;
        }
        case TARGET_TOTEM_EARTH:
        case TARGET_TOTEM_WATER:
        case TARGET_TOTEM_AIR:
        case TARGET_TOTEM_FIRE:
        {
            float angle = m_caster->GetOrientation();
            switch (targetMode)
            {
                case TARGET_TOTEM_EARTH:                         break;
                case TARGET_TOTEM_WATER: angle += M_PI_F;        break;
                case TARGET_TOTEM_AIR:   angle += M_PI_F * 0.5f; break;
                case TARGET_TOTEM_FIRE:  angle += M_PI_F * 1.5f; break;
            }

            float x, y;
            float z = m_caster->GetPositionZ();
            // Ignore the BOUNDING_RADIUS for spells with radius (add a small value to prevent < 0 rounding errors)
            m_caster->GetNearPoint2D(x, y, radius > 0.001f ? radius - m_caster->GetObjectBoundingRadius() + 0.01f : 2.0f, angle);
            m_caster->UpdateAllowedPositionZ(x, y, z);
            m_targets.setDestination(x, y, z);

            // Add Summoner
            targetUnitMap.push_back(m_caster);
            break;
        }
        case TARGET_SELF:
        case TARGET_SELF2:
            targetUnitMap.push_back(m_caster);
            break;
        case TARGET_RANDOM_ENEMY_CHAIN_IN_AREA:
        {
            m_targets.m_targetMask = 0;
            unMaxTargets = EffectChainTarget;
            float max_range = radius + unMaxTargets * CHAIN_SPELL_JUMP_RADIUS;

            UnitList tempTargetUnitMap;

            {
                MaNGOS::AnyAoETargetUnitInObjectRangeCheck u_check(m_caster, max_range);
                MaNGOS::UnitListSearcher<MaNGOS::AnyAoETargetUnitInObjectRangeCheck> searcher(tempTargetUnitMap, u_check);
                Cell::VisitAllObjects(m_caster, searcher, max_range);
            }

            if(tempTargetUnitMap.empty())
                break;

            tempTargetUnitMap.sort(TargetDistanceOrderNear(m_caster));

            //Now to get us a random target that's in the initial range of the spell
            uint32 t = 0;
            UnitList::iterator itr = tempTargetUnitMap.begin();
            while(itr!= tempTargetUnitMap.end() && (*itr)->IsWithinDist(m_caster, radius))
                ++t, ++itr;

            if(!t)
                break;

            itr = tempTargetUnitMap.begin();
            std::advance(itr, rand() % t);
            Unit *pUnitTarget = *itr;
            targetUnitMap.push_back(pUnitTarget);

            tempTargetUnitMap.erase(itr);

            tempTargetUnitMap.sort(TargetDistanceOrderNear(pUnitTarget));

            t = unMaxTargets - 1;
            Unit *prev = pUnitTarget;
            UnitList::iterator next = tempTargetUnitMap.begin();

            while(t && next != tempTargetUnitMap.end())
            {
                if(!prev->IsWithinDist(*next, CHAIN_SPELL_JUMP_RADIUS))
                    break;

                if(!m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && !prev->IsWithinLOSInMap(*next) || ((m_spellInfo->AttributesEx6 & SPELL_ATTR_EX6_IGNORE_CCED_TARGETS) && !(*next)->CanFreeMove()))
                {
                    ++next;
                    continue;
                }

                prev = *next;
                targetUnitMap.push_back(prev);
                tempTargetUnitMap.erase(next);
                tempTargetUnitMap.sort(TargetDistanceOrderNear(prev));
                next = tempTargetUnitMap.begin();

                --t;
            }
            break;
        }
        case TARGET_RANDOM_FRIEND_CHAIN_IN_AREA:
        {
            m_targets.m_targetMask = 0;
            unMaxTargets = EffectChainTarget;
            float max_range = radius + unMaxTargets * CHAIN_SPELL_JUMP_RADIUS;
            UnitList tempTargetUnitMap;
            {
                MaNGOS::AnyFriendlyUnitInObjectRangeCheck u_check(m_caster, max_range);
                MaNGOS::UnitListSearcher<MaNGOS::AnyFriendlyUnitInObjectRangeCheck> searcher(tempTargetUnitMap, u_check);
                Cell::VisitAllObjects(m_caster, searcher, max_range);
            }

            if(tempTargetUnitMap.empty())
                break;

            tempTargetUnitMap.sort(TargetDistanceOrderNear(m_caster));

            //Now to get us a random target that's in the initial range of the spell
            uint32 t = 0;
            UnitList::iterator itr = tempTargetUnitMap.begin();
            while(itr != tempTargetUnitMap.end() && (*itr)->IsWithinDist(m_caster, radius))
                ++t, ++itr;

            if(!t)
                break;

            itr = tempTargetUnitMap.begin();
            std::advance(itr, rand() % t);
            Unit *pUnitTarget = *itr;
            targetUnitMap.push_back(pUnitTarget);

            tempTargetUnitMap.erase(itr);

            tempTargetUnitMap.sort(TargetDistanceOrderNear(pUnitTarget));

            t = unMaxTargets - 1;
            Unit *prev = pUnitTarget;
            UnitList::iterator next = tempTargetUnitMap.begin();

            while(t && next != tempTargetUnitMap.end())
            {
                if(!prev->IsWithinDist(*next, CHAIN_SPELL_JUMP_RADIUS))
                    break;

                if(!!m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && prev->IsWithinLOSInMap(*next))
                {
                    ++next;
                    continue;
                }
                prev = *next;
                targetUnitMap.push_back(prev);
                tempTargetUnitMap.erase(next);
                tempTargetUnitMap.sort(TargetDistanceOrderNear(prev));
                next = tempTargetUnitMap.begin();
                --t;
            }
            break;
        }
        case TARGET_PET:
        {
            Pet* tmpUnit = m_caster->GetPet();
            if (!tmpUnit) break;
            targetUnitMap.push_back(tmpUnit);
            break;
        }
        case TARGET_CHAIN_DAMAGE:
        {
            if (EffectChainTarget <= 1)
            {
                if (Unit* pUnitTarget = m_caster->SelectMagnetTarget(m_targets.getUnitTarget(), this, effIndex))
                {
                    if (m_targets.getUnitTarget() != pUnitTarget)
                    {
                        m_targets.setUnitTarget(pUnitTarget);
                        m_spellFlags |= SPELL_FLAG_REDIRECTED;
                    }
                    targetUnitMap.push_back(pUnitTarget);
                }
            }
            else
            {
                Unit* pUnitTarget = m_targets.getUnitTarget();
                WorldObject* originalCaster = GetAffectiveCasterObject();
                if (!pUnitTarget || !originalCaster)
                    break;

                unMaxTargets = EffectChainTarget;

                float max_range;
                if(m_spellInfo->GetDmgClass() == SPELL_DAMAGE_CLASS_MELEE)
                    max_range = radius;
                else
                    //FIXME: This very like horrible hack and wrong for most spells
                    max_range = radius + unMaxTargets * CHAIN_SPELL_JUMP_RADIUS;

                UnitList tempTargetUnitMap;
                {
                    MaNGOS::AnyAoEVisibleTargetUnitInObjectRangeCheck u_check(pUnitTarget, originalCaster, max_range);
                    MaNGOS::UnitListSearcher<MaNGOS::AnyAoEVisibleTargetUnitInObjectRangeCheck> searcher(tempTargetUnitMap, u_check);
                    Cell::VisitAllObjects(m_caster, searcher, max_range);
                }

                if (tempTargetUnitMap.empty())
                    break;

                tempTargetUnitMap.sort(TargetDistanceOrderNear(pUnitTarget));

                if (*tempTargetUnitMap.begin() == pUnitTarget)
                    tempTargetUnitMap.erase(tempTargetUnitMap.begin());

                targetUnitMap.push_back(pUnitTarget);
                uint32 t = unMaxTargets - 1;
                Unit *prev = pUnitTarget;
                UnitList::iterator next = tempTargetUnitMap.begin();

                while (t && next != tempTargetUnitMap.end())
                {
                    if (!prev->IsWithinDist(*next, CHAIN_SPELL_JUMP_RADIUS))
                        break;

                    if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && !prev->IsWithinLOSInMap (*next) || ((m_spellInfo->AttributesEx6 & SPELL_ATTR_EX6_IGNORE_CCED_TARGETS) && !(*next)->CanFreeMove()))
                    {
                        ++next;
                        continue;
                    }

                    prev = *next;
                    targetUnitMap.push_back(prev);
                    tempTargetUnitMap.erase(next);
                    tempTargetUnitMap.sort(TargetDistanceOrderNear(prev));
                    next = tempTargetUnitMap.begin();

                    --t;
                }
            }
            break;
        }
        case TARGET_ALL_ENEMY_IN_AREA:
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            switch (m_spellInfo->Id)
            {
                case 30843:                             // Enfeeble
                case 31347:                             // Doom
                case 37676:                             // Insidious Whisper
                case 38028:                             // Watery Grave
                case 40618:                             // Insignificance
                case 41376:                             // Spite
                case 62166:                                 // Stone Grip
                case 63981:                                 // Stone Grip (h)
                    // Do not target current victim
                    if (Unit* pVictim = m_caster->getVictim())
                        targetUnitMap.remove(pVictim);
                    break;
                case 42005:                             // Bloodboil (spell hits only the 5 furthest away targets)
                    // manually cuting, because the spell hits only the 5 furthest away targets
                    if (targetUnitMap.size() > unMaxTargets)
                    {
                        targetUnitMap.sort(TargetDistanceOrderFarAway(m_caster));
                        targetUnitMap.resize(unMaxTargets);
                    }
                    break;
                case 62240:                             // Solar Flare
                case 62920:
                    if (SpellAuraHolder* holder = m_caster->GetSpellAuraHolder(62239))
                        unMaxTargets = holder->GetStackAmount();
                    else
                        unMaxTargets = 1;
                    break;
                default:
                    break;
            }
            break;
        case TARGET_AREAEFFECT_INSTANT:
        {
            SpellTargets targetB = SPELL_TARGETS_AOE_DAMAGE;
            switch (spellEffect->Effect)
            {
                case SPELL_EFFECT_QUEST_COMPLETE:
                case SPELL_EFFECT_KILL_CREDIT_PERSONAL:
                case SPELL_EFFECT_KILL_CREDIT_GROUP:
                    targetB = SPELL_TARGETS_ALL;
                default:
                    // Select friendly targets for positive effect
                    if (IsPositiveEffect(m_spellInfo, effIndex))
                        targetB = SPELL_TARGETS_FRIENDLY;
                    break;
            }

            UnitList tempTargetUnitMap;
            SQLMultiStorage::SQLMSIteratorBounds<SpellTargetEntry> bounds = sSpellScriptTargetStorage.getBounds<SpellTargetEntry>(m_spellInfo->Id);

            // fill real target list if no spell script target defined
            FillAreaTargets(bounds.first != bounds.second ? tempTargetUnitMap : targetUnitMap,
                radius, PUSH_DEST_CENTER, bounds.first != bounds.second ? SPELL_TARGETS_ALL : targetB);

            if (!tempTargetUnitMap.empty())
            {
                for (UnitList::const_iterator iter = tempTargetUnitMap.begin(); iter != tempTargetUnitMap.end(); ++iter)
                {
                    if ((*iter)->GetTypeId() != TYPEID_UNIT)
                        continue;

                    for (SQLMultiStorage::SQLMultiSIterator<SpellTargetEntry> i_spellST = bounds.first; i_spellST != bounds.second; ++i_spellST)
                    {
                        if (i_spellST->CanNotHitWithSpellEffect(effIndex))
                            continue;

                        // only creature entries supported for this target type
                        if (i_spellST->type == SPELL_TARGET_TYPE_GAMEOBJECT)
                            continue;

                        if ((*iter)->GetEntry() == i_spellST->targetEntry)
                        {
                            if (i_spellST->type == SPELL_TARGET_TYPE_DEAD && ((Creature*)(*iter))->IsCorpse())
                            {
                                targetUnitMap.push_back((*iter));
                            }
                            else if (i_spellST->type == SPELL_TARGET_TYPE_CREATURE && (*iter)->isAlive())
                            {
                                targetUnitMap.push_back((*iter));
                            }

                            break;
                        }
                    }
                }
            }

            // exclude caster
            targetUnitMap.remove(m_caster);
            break;
        }
        case TARGET_AREAEFFECT_CUSTOM:
        {
            if (spellEffect && spellEffect->Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA)
                break;
            else if (spellEffect && spellEffect->Effect == SPELL_EFFECT_SUMMON)
            {
                targetUnitMap.push_back(m_caster);
                break;
            }

            DEBUG_LOG("SetTargetMap for TARGET_AREAEFFECT_CUSTOM spell %u eff %u x: %f y: %f z: %f",
                m_spellInfo->Id, m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ);
            UnitList tempTargetUnitMap;
            SQLMultiStorage::SQLMSIteratorBounds<SpellTargetEntry> bounds = sSpellScriptTargetStorage.getBounds<SpellTargetEntry>(m_spellInfo->Id);
            // fill real target list if no spell script target defined
            FillAreaTargets(bounds.first != bounds.second ? tempTargetUnitMap : targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_ALL);

            if (!tempTargetUnitMap.empty())
            {
                for (UnitList::const_iterator iter = tempTargetUnitMap.begin(); iter != tempTargetUnitMap.end(); ++iter)
                {
                    if ((*iter)->GetTypeId() != TYPEID_UNIT)
                        continue;

                    for (SQLMultiStorage::SQLMultiSIterator<SpellTargetEntry> i_spellST = bounds.first; i_spellST != bounds.second; ++i_spellST)
                    {
                        if (i_spellST->CanNotHitWithSpellEffect(effIndex))
                            continue;

                        // only creature entries supported for this target type
                        if (i_spellST->type == SPELL_TARGET_TYPE_GAMEOBJECT)
                            continue;

                        if ((*iter)->GetEntry() == i_spellST->targetEntry)
                        {
                            if (i_spellST->type == SPELL_TARGET_TYPE_DEAD && ((Creature*)(*iter))->IsCorpse())
                            {
                                targetUnitMap.push_back((*iter));
                            }
                            else if (i_spellST->type == SPELL_TARGET_TYPE_CREATURE && (*iter)->isAlive())
                            {
                                targetUnitMap.push_back((*iter));
                            }

                            break;
                        }
                    }
                }
            }
            else
            {
                // remove not targetable units if spell has no script targets
                for (UnitList::iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); )
                {
                    if (!(*itr)->isTargetableForAttack(m_spellInfo->HasAttribute(SPELL_ATTR_EX3_CAST_ON_DEAD)))
                        targetUnitMap.erase(itr++);
                    else
                        ++itr;
                }
            }
            break;
        }
        case TARGET_AREAEFFECT_GO_AROUND_SOURCE:
        case TARGET_AREAEFFECT_GO_AROUND_DEST:
        case TARGET_GO_IN_FRONT_OF_CASTER_90:
        {
            // It may be possible to fill targets for some spell effects
            // automatically (SPELL_EFFECT_WMO_REPAIR(88) for example) but
            // for some/most spells we clearly need/want to limit with spell_target_script

            // Some spells untested, for affected GO type 33. May need further adjustments for spells related.

            DEBUG_LOG("Target Mode: %u Spell: %u EffIdx: %u TargetMask: %u",
                targetMode, m_spellInfo->Id, effIndex, m_targets.m_targetMask);
            float x, y, z;
            if (targetMode == TARGET_AREAEFFECT_GO_AROUND_SOURCE)
            {
                if (m_targets.m_targetMask & TARGET_FLAG_SOURCE_LOCATION)
                    m_targets.getSource(x, y, z);
                else
                    m_caster->GetPosition(x, y, z);
            }
            else if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
                m_targets.getDestination(x, y, z);
            else
                m_caster->GetPosition(x, y, z);

            std::list<GameObject*> tempTargetGOList2;
            float fSearchDistance = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

            SQLMultiStorage::SQLMSIteratorBounds<SpellTargetEntry> bounds = sSpellScriptTargetStorage.getBounds<SpellTargetEntry>(m_spellInfo->Id);
            for (SQLMultiStorage::SQLMultiSIterator<SpellTargetEntry> i_spellST = bounds.first; i_spellST != bounds.second; ++i_spellST)
            {
                    if (i_spellST->CanNotHitWithSpellEffect(effIndex))
                        continue;

                    if (i_spellST->type == SPELL_TARGET_TYPE_GAMEOBJECT)
                    {
                        // search all GO's with entry, within range of m_destN
                        MaNGOS::GameObjectEntryInPosRangeCheck go_check(*m_caster, i_spellST->targetEntry, x, y, z, radius);
                        MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectEntryInPosRangeCheck> checker(tempTargetGOList2, go_check);
                        Cell::VisitGridObjects(m_caster, checker, radius + fSearchDistance);
                    }
            }

            if (bounds.first == bounds.second)
            {
                MaNGOS::GameObjectInRangeCheck check(m_caster, x, y, z, radius);
                MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectInRangeCheck> searcher(tempTargetGOList2, check);
                Cell::VisitAllObjects(m_caster, searcher, radius + fSearchDistance);
            }

            for (std::list<GameObject*>::iterator iter = tempTargetGOList2.begin(); iter != tempTargetGOList2.end(); ++iter)
                if (targetMode != TARGET_GO_IN_FRONT_OF_CASTER_90 || m_caster->HasInArc(M_PI_F, *iter))
                    tempTargetGOList.push_back(*iter);
            break;
        }
        case TARGET_ALL_ENEMY_IN_AREA_INSTANT:
        {
            // targets the ground, not the units in the area
            if (!spellEffect)
                break;

            switch (spellEffect->Effect)
            {
                case SPELL_EFFECT_PERSISTENT_AREA_AURA:
                    break;
                case SPELL_EFFECT_SUMMON:
                    targetUnitMap.push_back(m_caster);
                    break;
                default:
                {
                    // War Stomp & Blood Boil
                    if (m_spellInfo->Id == 20549 || m_spellInfo->Id == 48721)
                        m_targets.setDestination(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ());

                    FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
                    // Jinx: Curse of the Elements, Piercing Chill
                    if (m_spellInfo->Id == 85547 || m_spellInfo->Id == 86105 || m_spellInfo->Id == 83154)
                        targetUnitMap.remove(m_targets.getUnitTarget());

                    // Flame Orb and Frostfire Orb, Piercing Chill
                    if (m_spellInfo->Id == 82734 || m_spellInfo->Id == 84718 || m_spellInfo->Id == 83154)
                        targetUnitMap.sort(TargetDistanceOrderNear(m_caster));
                    break;
                }
            }
            break;
        }
        case TARGET_DUELVSPLAYER_COORDINATES:
        {
            if(Unit* currentTarget = m_targets.getUnitTarget())
                m_targets.setDestination(currentTarget->GetPositionX(), currentTarget->GetPositionY(), currentTarget->GetPositionZ());
            break;
        }
        case TARGET_ALL_PARTY_AROUND_CASTER:
        {
            if (m_caster->GetObjectGuid().IsPet())
            {
                // only affect pet and owner
                targetUnitMap.push_back(m_caster);
                if (Unit* owner = m_caster->GetOwner())
                    targetUnitMap.push_back(owner);
            }
            else
            {
                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, false, true, true);
            }
            break;
        }
        case TARGET_ALL_PARTY_AROUND_CASTER_2:
        case TARGET_ALL_PARTY:
        {
            FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, false, true, true);
            break;
        }
        case TARGET_ALL_RAID_AROUND_CASTER:
        {
            if (m_spellInfo->Id == 57669)                   // Replenishment (special target selection)
            {
                // in arena, target should be only caster
                if (m_caster->GetMap()->IsBattleArena())
                    targetUnitMap.push_back(m_caster);
                else
                    FillRaidOrPartyManaPriorityTargets(targetUnitMap, m_caster, m_caster, radius, 10, true, false, true);
            }
            else if (m_spellInfo->Id==52759)                // Ancestral Awakening (special target selection)
                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, m_caster, radius, 1, true, false, true);
            else if (m_spellInfo->Id == 54171)              // Divine Storm
                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, m_caster, radius, 3, true, false, true);
            else if (m_spellInfo->Id == 59725)              // Improved Spell Reflection
            {
                if (m_caster->HasAura(23920, EFFECT_INDEX_0) )
                    m_caster->RemoveAurasDueToSpell(23920); // will be replaced by imp. spell refl. aura

                Unit::AuraList const& lDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                for(Unit::AuraList::const_iterator i = lDummyAuras.begin(); i != lDummyAuras.end(); ++i)
                {
                    if((*i)->GetSpellProto()->SpellIconID == 1935)
                    {
                        unMaxTargets = (*i)->GetModifier()->m_amount + 1;   // +1 because we are also applying this to the caster
                        break;
                    }
                }

                radius = 20.0f;     // as mentioned in the spell's tooltip (data doesn't appear in dbc)

                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, false, false, true);
                targetUnitMap.sort(TargetDistanceOrderNear(m_caster));
                if (targetUnitMap.size() > unMaxTargets)
                    targetUnitMap.resize(unMaxTargets);
                break;
            }
            else if (m_spellInfo->Id == 70940)              // Divine Guardian
                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, true, false);
            else if (m_spellInfo->Id == 105588)             // Vampiric Blood
                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, true, false);
            else if (m_spellInfo->Id == 105737)             // Mass Regeneration (Bear Form)
                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, true, false);
            else if (m_spellInfo->Id == 105739)             // Mass Regeneration
                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, true, false);
            else if (m_spellInfo->Id == 105914)             // Shield Fortress
                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, true, false);
            else
                FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, true, IsPositiveSpell(m_spellInfo->Id));
            break;
        }
        case TARGET_119:
        {
            FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, false, false);
            break;
        }
        case TARGET_SINGLE_FRIEND:
        case TARGET_SINGLE_FRIEND_2:
            if(m_targets.getUnitTarget())
                targetUnitMap.push_back(m_targets.getUnitTarget());
            break;
        case TARGET_NONCOMBAT_PET:
            if(Unit* target = m_targets.getUnitTarget())
                if( target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->IsPet() && ((Pet*)target)->getPetType() == MINI_PET)
                    targetUnitMap.push_back(target);
            break;
        case TARGET_UNIT_CREATOR:
            if (Unit* target = m_caster->GetCreator())
                targetUnitMap.push_back(target);
            break;
        case TARGET_OWNED_VEHICLE:
            if (VehicleKit* vehicle = m_caster->GetVehicle())
                if (Unit* target = vehicle->GetBase())
                    targetUnitMap.push_back(target);
            break;
        case TARGET_UNIT_PASSENGER_0:
        case TARGET_UNIT_PASSENGER_1:
        case TARGET_UNIT_PASSENGER_2:
        case TARGET_UNIT_PASSENGER_3:
        case TARGET_UNIT_PASSENGER_4:
        case TARGET_UNIT_PASSENGER_5:
        case TARGET_UNIT_PASSENGER_6:
        case TARGET_UNIT_PASSENGER_7:
            if (m_caster->GetTypeId() == TYPEID_UNIT && m_caster->GetObjectGuid().IsVehicle())
                if (Unit *unit = m_caster->GetVehicleKit()->GetPassenger(targetMode - TARGET_UNIT_PASSENGER_0))
                    targetUnitMap.push_back(unit);
            break;
        case TARGET_CASTER_COORDINATES:
        {
            // Check original caster is GO - set its coordinates as src cast
            if (WorldObject *caster = GetCastingObject())
                m_targets.setSource(caster->GetPositionX(), caster->GetPositionY(), caster->GetPositionZ());
            break;
        }
        case TARGET_ALL_HOSTILE_UNITS_AROUND_CASTER:
            FillAreaTargets(targetUnitMap, radius, PUSH_SELF_CENTER, SPELL_TARGETS_HOSTILE);
            break;
        case TARGET_ALL_FRIENDLY_UNITS_AROUND_CASTER:
            switch (m_spellInfo->Id)
            {
                case 56153:                                 // Guardian Aura - Ahn'Kahet
                case 50811:                                 // Shatter Dmg N - Krystallus
                case 61547:                                 // Shatter Dmg H - Krystallus
                    FillAreaTargets(targetUnitMap, radius, PUSH_SELF_CENTER, SPELL_TARGETS_FRIENDLY);
                    targetUnitMap.remove(m_caster);
                    break;
                case 63881:                                 // Malady of the Mind   Ulduar Yogg Saron
                    FillAreaTargets(targetUnitMap, radius, PUSH_SELF_CENTER, SPELL_TARGETS_FRIENDLY, GetCastingObject());
                    targetUnitMap.remove(m_caster);
                    break;
                case 64844:                                 // Divine Hymn
                    // target amount stored in parent spell dummy effect but hard to access
                    FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, m_caster, radius, 3, true, false, true);
                    break;
                case 64904:                                 // Hymn of Hope
                    // target amount stored in parent spell dummy effect but hard to access
                    FillRaidOrPartyManaPriorityTargets(targetUnitMap, m_caster, m_caster, radius, 3, true, false, true);
                    break;
                case 71447:                                 // Bloodbolt Splash 10N
                case 71481:                                 // Bloodbolt Splash 25N
                case 71482:                                 // Bloodbolt Splash 10H
                case 71483:                                 // Bloodbolt Splash 25H
                    FillAreaTargets(targetUnitMap, radius, PUSH_SELF_CENTER, SPELL_TARGETS_FRIENDLY);
                    targetUnitMap.remove(m_caster);
                    break;
                default:
                    // selected friendly units (for casting objects) around casting object
                    FillAreaTargets(targetUnitMap, radius, PUSH_SELF_CENTER, SPELL_TARGETS_FRIENDLY, GetCastingObject());
                    break;
            }
            break;
        case TARGET_ALL_FRIENDLY_UNITS_IN_AREA:
            // Death Pact (in fact selection by player selection)
            if (m_spellInfo->Id == 48743)
            {
                // checked in Spell::CheckCast
                if (m_caster->GetTypeId()==TYPEID_PLAYER)
                {
                    if (Unit* target = (Unit*)m_caster->GetPet())
                        targetUnitMap.push_back(target);
                    else if (Unit* target = (Unit*)m_caster->FindGuardianWithEntry(26125, true))
                        targetUnitMap.push_back(target);
                }
            }
            // Circle of Healing
            else if (m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_PRIEST && m_spellInfo->SpellVisual[0] == 8253)
            {
                Unit* target = m_targets.getUnitTarget();
                if(!target)
                    target = m_caster;

                uint32 count = 5;
                // Glyph of Circle of Healing
                if(Aura const* glyph = m_caster->GetDummyAura(55675))
                    count += glyph->GetModifier()->m_amount;

                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, target, radius, count, true, false, true);
            }
            // Wild Growth
            else if (m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_DRUID && m_spellInfo->SpellIconID == 2864)
            {
                Unit* target = m_targets.getUnitTarget();
                if(!target)
                    target = m_caster;
                uint32 count = CalculateDamage(EFFECT_INDEX_2,m_caster); // stored in dummy effect, affected by mods

                // Tree of Life
                if (m_caster->HasAura(33891))
                    count += 2;

                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, target, radius, count, true, false, true);
            }
            // Glyph of Holy Light
            else if (m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_GENERIC && m_spellInfo->SpellIconID == 963)
            {
                Unit* target = m_targets.getUnitTarget();

                if(!target)
                    target = m_caster;

                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, target, radius, 5, true, false, true);
            }
            // Power Word: Barrier
            else if (m_spellInfo->Id == 62618)
                targetUnitMap.push_back(m_caster);
            // Item - Icecrown 25 Heroic/Normal Healer Trinket 2
            else if (m_spellInfo->Id == 71641 || m_spellInfo->Id == 71610)
                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, m_caster, radius, 1, true, false, false);
            // Efflorescence
            else if (m_spellInfo->Id == 81269)
            {
                FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_FRIENDLY);

                PrioritizeHealthUnitQueue healthQueue;
                for (UnitList::const_iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr)
                    if (!(*itr)->isDead())
                        healthQueue.push(PrioritizeHealthUnitWraper(*itr));

                targetUnitMap.clear();
                while (!healthQueue.empty() && targetUnitMap.size() < 3)
                {
                    targetUnitMap.push_back(healthQueue.top().getUnit());
                    healthQueue.pop();
                }
            }
            // Atonement
            else if (m_spellInfo->Id == 94472)
                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, m_targets.getUnitTarget(), radius, 1, true, false, true);
            // Blaze of Life
            // Nick of Time
            else if (m_spellInfo->Id == 96966 || m_spellInfo->Id == 97136 || m_spellInfo->Id == 108000 || m_spellInfo->Id == 109822 || m_spellInfo->Id == 109825)
                FillRaidOrPartyHealthPriorityTargets(targetUnitMap, m_caster, m_targets.getUnitTarget(), radius, 1, true, false, true);
            else
                FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_FRIENDLY);

            // Ancient Healer
            if (m_spellInfo->Id == 86678)
                targetUnitMap.remove(m_targets.getUnitTarget());
            // Firebloom
            // Item - Druid T12 Restoration 4P Bonus
            else if (m_spellInfo->Id == 99017)
            {
                PrioritizeHealthUnitQueue healthQueue;
                for(UnitList::const_iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr)
                    if (!(*itr)->isDead() && (*itr) != m_targets.getUnitTarget())
                        healthQueue.push(PrioritizeHealthUnitWraper(*itr));

                targetUnitMap.clear();
                if (!healthQueue.empty())
                    targetUnitMap.push_back(healthQueue.top().getUnit());
            }
            // Cauterizing Flame
            // Item - Priest T12 Healer 4P Bonus
            else if (m_spellInfo->Id == 99152)
            {
                PrioritizeHealthUnitQueue healthQueue;
                for(UnitList::const_iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr)
                    if (!(*itr)->isDead())
                        healthQueue.push(PrioritizeHealthUnitWraper(*itr));

                targetUnitMap.clear();
                while(!healthQueue.empty() && targetUnitMap.size() < 1)
                {
                    targetUnitMap.push_back(healthQueue.top().getUnit());
                    healthQueue.pop();
                }
            }
            break;
        // TARGET_SINGLE_PARTY means that the spells can only be casted on a party member and not on the caster (some seals, fire shield from imp, etc..)
        case TARGET_SINGLE_PARTY:
        {
            Unit *target = m_targets.getUnitTarget();
            // Those spells apparently can't be casted on the caster.
            if( target && target != m_caster)
            {
                // Can only be casted on group's members or its pets
                Group  *pGroup = NULL;

                Unit* owner = m_caster->GetCharmerOrOwner();
                Unit *targetOwner = target->GetCharmerOrOwner();
                if(owner)
                {
                    if(owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        if( target == owner )
                        {
                            targetUnitMap.push_back(target);
                            break;
                        }
                        pGroup = ((Player*)owner)->GetGroup();
                    }
                }
                else if (m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    if( targetOwner == m_caster && target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->IsPet())
                    {
                        targetUnitMap.push_back(target);
                        break;
                    }
                    pGroup = ((Player*)m_caster)->GetGroup();
                }

                if(pGroup)
                {
                    // Our target can also be a player's pet who's grouped with us or our pet. But can't be controlled player
                    if(targetOwner)
                    {
                        if( targetOwner->GetTypeId() == TYPEID_PLAYER &&
                            target->GetTypeId() == TYPEID_UNIT && (((Creature*)target)->IsPet()) &&
                            target->GetOwnerGuid() == targetOwner->GetObjectGuid() &&
                            pGroup->IsMember(((Player*)targetOwner)->GetObjectGuid()))
                        {
                            targetUnitMap.push_back(target);
                        }
                    }
                    // 1Our target can be a player who is on our group
                    else if (target->GetTypeId() == TYPEID_PLAYER && pGroup->IsMember(((Player*)target)->GetObjectGuid()))
                    {
                        targetUnitMap.push_back(target);
                    }
                }
            }
            break;
        }
        case TARGET_GAMEOBJECT:
            if(m_targets.getGOTarget())
                AddGOTarget(m_targets.getGOTarget(), effIndex);
            break;
        case TARGET_IN_FRONT_OF_CASTER:
        {
            SpellNotifyPushType pushType = PUSH_IN_FRONT;
            switch (m_spellInfo->SpellVisual[0])            // Some spell require a different target fill
            {
                case 3879: pushType = PUSH_IN_BACK;     break;
                case 7441: pushType = PUSH_IN_FRONT_15; break;
                case 8669: pushType = PUSH_IN_FRONT_15; break;
            }
            FillAreaTargets(targetUnitMap, radius, pushType, SPELL_TARGETS_AOE_DAMAGE);
            break;
        }
        case TARGET_LARGE_FRONTAL_CONE:
            FillAreaTargets(targetUnitMap, radius, PUSH_IN_FRONT_90, SPELL_TARGETS_AOE_DAMAGE);
            break;
        case TARGET_ALLY_IN_FRONT_OF_CASTER_30:
        {
            FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, true, false, false);

            PrioritizeHealthUnitQueue healthQueue;
            for (UnitList::const_iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr)
                if (!(*itr)->isDead())
                    healthQueue.push(PrioritizeHealthUnitWraper(*itr));

            targetUnitMap.clear();
            while (!healthQueue.empty() && targetUnitMap.size() < unMaxTargets)
            {
                if (m_caster->isInFront(healthQueue.top().getUnit(), radius, M_PI_F / 4 ))
                    targetUnitMap.push_back(healthQueue.top().getUnit());
                healthQueue.pop();
            }
            break;
        }
        case TARGET_NARROW_FRONTAL_CONE:
            // Foam Sword Attack & Bonked!
            if (m_spellInfo->Id == 62973 || m_spellInfo->Id == 62991)
            {
                FillAreaTargets(targetUnitMap, radius, PUSH_IN_FRONT_30, SPELL_TARGETS_ALL);

                UnitList::iterator caster = std::find(targetUnitMap.begin(), targetUnitMap.end(), m_caster);
                if (caster != targetUnitMap.end())
                    targetUnitMap.erase(caster);
            }
            else
                FillAreaTargets(targetUnitMap, radius, PUSH_IN_FRONT_15, SPELL_TARGETS_AOE_DAMAGE);
            break;
        case TARGET_IN_FRONT_OF_CASTER_30:
        {
            if (m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_GENERIC)
                FillAreaTargets(targetUnitMap, radius, PUSH_IN_FRONT_30, SPELL_TARGETS_AOE_DAMAGE);
            else
                FillAreaTargets(targetUnitMap, radius, PUSH_IN_FRONT, SPELL_TARGETS_AOE_DAMAGE);
            break;
        }
        case TARGET_DUELVSPLAYER:
        {
            if (Unit* target = m_targets.getUnitTarget())
            {
                if(m_caster->IsFriendlyTo(target))
                {
                    targetUnitMap.push_back(target);
                }
                else
                {
                    if (Unit* pUnitTarget = m_caster->SelectMagnetTarget(target, this, effIndex))
                    {
                        if (target != pUnitTarget)
                        {
                            m_targets.setUnitTarget(pUnitTarget);
                            m_spellFlags |= SPELL_FLAG_REDIRECTED;
                        }
                        targetUnitMap.push_back(pUnitTarget);
                    }
                }
            }
            break;
        }
        case TARGET_GAMEOBJECT_ITEM:
            if (m_targets.getGOTargetGuid())
                AddGOTarget(m_targets.getGOTarget(), effIndex);
            else if (m_targets.getItemTarget())
                AddItemTarget(m_targets.getItemTarget(), effIndex);
            break;
        case TARGET_MASTER:
            if(Unit* owner = m_caster->GetCharmerOrOwner())
                targetUnitMap.push_back(owner);
            break;
        case TARGET_ALL_ENEMY_IN_AREA_CHANNELED:
            // targets the ground, not the units in the area
            if (spellEffect && spellEffect->Effect!=SPELL_EFFECT_PERSISTENT_AREA_AURA)
                FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            break;
        case TARGET_MINION:
            if(spellEffect && spellEffect->Effect != SPELL_EFFECT_DUEL)
                targetUnitMap.push_back(m_caster);
            break;
        case TARGET_SINGLE_ENEMY:
        {
            Unit* target = m_targets.getUnitTarget();
            if (!IsPositiveEffect(m_spellInfo, effIndex))
            {
                if(Unit* pUnitTarget = m_caster->SelectMagnetTarget(target, this, effIndex))
                {
                    if (target != pUnitTarget)
                    {
                        m_targets.setUnitTarget(pUnitTarget);
                        m_spellFlags |= SPELL_FLAG_REDIRECTED;
                    }
                    targetUnitMap.push_back(pUnitTarget);
                }
                else if (target)
                    targetUnitMap.push_back(target);
            }
            else if (target)
                targetUnitMap.push_back(target);
            break;
        }
        case TARGET_AREAEFFECT_PARTY:
        {
            Unit* owner = m_caster->GetCharmerOrOwner();
            Player *pTarget = NULL;

            if(owner)
            {
                targetUnitMap.push_back(m_caster);
                if(owner->GetTypeId() == TYPEID_PLAYER)
                    pTarget = (Player*)owner;
            }
            else if (m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                if(Unit* target = m_targets.getUnitTarget())
                {
                    if( target->GetTypeId() != TYPEID_PLAYER)
                    {
                        if(((Creature*)target)->IsPet())
                        {
                            Unit *targetOwner = target->GetOwner();
                            if(targetOwner->GetTypeId() == TYPEID_PLAYER)
                                pTarget = (Player*)targetOwner;
                        }
                    }
                    else
                        pTarget = (Player*)target;
                }
            }

            Group* pGroup = pTarget ? pTarget->GetGroup() : NULL;

            if(pGroup)
            {
                uint8 subgroup = pTarget->GetSubGroup();

                for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* Target = itr->getSource();

                    // IsHostileTo check duel and controlled by enemy
                    if(Target && Target->GetSubGroup() == subgroup && !m_caster->IsHostileTo(Target))
                    {
                        if( pTarget->IsWithinDistInMap(Target, radius) )
                            targetUnitMap.push_back(Target);

                        if(Pet* pet = Target->GetPet())
                            if( pTarget->IsWithinDistInMap(pet, radius) )
                                targetUnitMap.push_back(pet);
                    }
                }
            }
            else if (owner)
            {
                if(m_caster->IsWithinDistInMap(owner, radius))
                    targetUnitMap.push_back(owner);
            }
            else if(pTarget)
            {
                targetUnitMap.push_back(pTarget);

                if(Pet* pet = pTarget->GetPet())
                    if( m_caster->IsWithinDistInMap(pet, radius) )
                        targetUnitMap.push_back(pet);
            }
            break;
        }
        case TARGET_SCRIPT:
        {
            if(m_targets.getUnitTarget())
                targetUnitMap.push_back(m_targets.getUnitTarget());
            if(m_targets.getItemTarget())
                AddItemTarget(m_targets.getItemTarget(), effIndex);
            break;
        }
        case TARGET_SELF_FISHING:
            targetUnitMap.push_back(m_caster);
            break;
        case TARGET_CHAIN_HEAL:
        {
            Unit* pUnitTarget = m_targets.getUnitTarget();
            if(!pUnitTarget)
                break;

            if (EffectChainTarget <= 1)
                targetUnitMap.push_back(pUnitTarget);
            else
            {
                unMaxTargets = EffectChainTarget;
                float max_range = radius + unMaxTargets * CHAIN_SPELL_JUMP_RADIUS;

                UnitList tempTargetUnitMap;

                FillAreaTargets(tempTargetUnitMap, max_range, PUSH_SELF_CENTER, SPELL_TARGETS_FRIENDLY);

                if (m_caster != pUnitTarget && std::find(tempTargetUnitMap.begin(), tempTargetUnitMap.end(), m_caster) == tempTargetUnitMap.end())
                    tempTargetUnitMap.push_front(m_caster);

                tempTargetUnitMap.sort(TargetDistanceOrderNear(pUnitTarget));

                if (tempTargetUnitMap.empty())
                    break;

                if (*tempTargetUnitMap.begin() == pUnitTarget)
                    tempTargetUnitMap.erase(tempTargetUnitMap.begin());

                targetUnitMap.push_back(pUnitTarget);
                uint32 t = unMaxTargets - 1;
                Unit *prev = pUnitTarget;
                UnitList::iterator next = tempTargetUnitMap.begin();

                while(t && next != tempTargetUnitMap.end())
                {
                    if(!prev->IsWithinDist(*next, CHAIN_SPELL_JUMP_RADIUS))
                        break;

                    if(!m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && !prev->IsWithinLOSInMap(*next))
                    {
                        ++next;
                        continue;
                    }

                    if((*next)->GetHealth() == (*next)->GetMaxHealth())
                    {
                        next = tempTargetUnitMap.erase(next);
                        continue;
                    }

                    prev = *next;
                    targetUnitMap.push_back(prev);
                    tempTargetUnitMap.erase(next);
                    tempTargetUnitMap.sort(TargetDistanceOrderNear(prev));
                    next = tempTargetUnitMap.begin();

                    --t;
                }
            }
            break;
        }
        case TARGET_CURRENT_ENEMY_COORDINATES:
        {
            Unit* currentTarget = m_targets.getUnitTarget();
            if(currentTarget)
            {
                targetUnitMap.push_back(currentTarget);
                m_targets.setDestination(currentTarget->GetPositionX(), currentTarget->GetPositionY(), currentTarget->GetPositionZ());
            }
            break;
        }
        case TARGET_AREAEFFECT_PARTY_AND_CLASS:
        {
            Player* targetPlayer = m_targets.getUnitTarget() && m_targets.getUnitTarget()->GetTypeId() == TYPEID_PLAYER
                ? (Player*)m_targets.getUnitTarget() : NULL;

            Group* pGroup = targetPlayer ? targetPlayer->GetGroup() : NULL;
            if(pGroup)
            {
                for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* Target = itr->getSource();

                    // IsHostileTo check duel and controlled by enemy
                    if( Target && targetPlayer->IsWithinDistInMap(Target, radius) &&
                        targetPlayer->getClass() == Target->getClass() &&
                        !m_caster->IsHostileTo(Target) )
                    {
                        targetUnitMap.push_back(Target);
                    }
                }
            }
            else if(m_targets.getUnitTarget())
                targetUnitMap.push_back(m_targets.getUnitTarget());
            break;
        }
        case TARGET_TABLE_X_Y_Z_COORDINATES:
        {
            if (SpellTargetPosition const* st = sSpellMgr.GetSpellTargetPosition(m_spellInfo->Id))
            {
                m_targets.setDestination(st->target_X, st->target_Y, st->target_Z);
                // TODO - maybe use an (internal) value for the map for neat far teleport handling

                // far-teleport spells are handled in SpellEffect, elsewise report an error about an unexpected map (spells are always locally)
                if (st->target_mapId != m_caster->GetMapId() && spellEffect && spellEffect->Effect != SPELL_EFFECT_TELEPORT_UNITS)
                    sLog.outError("SPELL: wrong map (%u instead %u) target coordinates for spell ID %u", st->target_mapId, m_caster->GetMapId(), m_spellInfo->Id);
            }
            else
                sLog.outError("SPELL: unknown target coordinates for spell ID %u", m_spellInfo->Id);
            break;
        }
        case TARGET_INFRONT_OF_VICTIM:
        case TARGET_BEHIND_VICTIM:
        case TARGET_RIGHT_FROM_VICTIM:
        case TARGET_LEFT_FROM_VICTIM:
        {
            Unit *pTarget = NULL;

            // explicit cast data from client or server-side cast
            // some spell at client send caster
            if(m_targets.getUnitTarget() && m_targets.getUnitTarget()!=m_caster)
                pTarget = m_targets.getUnitTarget();
            else if(m_caster->getVictim())
                pTarget = m_caster->getVictim();
            else if(m_caster->GetTypeId() == TYPEID_PLAYER)
                pTarget = ObjectAccessor::GetUnit(*m_caster, ((Player*)m_caster)->GetSelectionGuid());

            // hack Guardian of Ancient Kings target
            if (!pTarget && (m_spellInfo->Id == 86659 || m_spellInfo->Id == 86669 || m_spellInfo->Id == 86698))
                pTarget = m_caster;

            if(pTarget)
            {
                float angle = 0.0f;
                float dist = (radius && targetMode != TARGET_BEHIND_VICTIM) ? radius : CONTACT_DISTANCE;

                switch(targetMode)
                {
                    case TARGET_INFRONT_OF_VICTIM:                      break;
                    case TARGET_BEHIND_VICTIM:      angle = M_PI_F;       break;
                    case TARGET_RIGHT_FROM_VICTIM:  angle = -M_PI_F / 2;  break;
                    case TARGET_LEFT_FROM_VICTIM:   angle = M_PI_F / 2;   break;
                }

                float _target_x, _target_y, _target_z;
                pTarget->GetClosePoint(_target_x, _target_y, _target_z, pTarget->GetObjectBoundingRadius(), dist, angle);
                if(pTarget->IsWithinLOS(_target_x, _target_y, _target_z))
                {
                    targetUnitMap.push_back(m_caster);
                    m_targets.setDestination(_target_x, _target_y, _target_z);
                }
            }
            break;
        }
        case TARGET_DYNAMIC_OBJECT_COORDINATES:
            // if parent spell create dynamic object extract area from it
            if (DynamicObject* dynObj = m_caster->GetDynObject(m_triggeredByAuraSpell ? m_triggeredByAuraSpell->Id : m_spellInfo->Id))
                m_targets.setDestination(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ());
            // else use destination of target if no destination set (ie for Mind Sear - 53022)
            else if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) && m_targets.m_targetMask & TARGET_FLAG_UNIT)
                m_targets.setDestination(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ);
            break;
        case TARGET_DYNAMIC_OBJECT_FRONT:
        case TARGET_DYNAMIC_OBJECT_BEHIND:
        case TARGET_DYNAMIC_OBJECT_LEFT_SIDE:
        case TARGET_DYNAMIC_OBJECT_RIGHT_SIDE:
        {
            if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
            {
                // General override, we don't want to use max spell range here.
                // Note: 0.0 radius is also for index 36. It is possible that 36 must be defined as
                // "at the base of", in difference to 0 which appear to be "directly in front of".
                // TODO: some summoned will make caster be half inside summoned object. Need to fix
                // that in the below code (nearpoint vs closepoint, etc).
                if (!spellEffect || spellEffect->GetRadiusIndex() == 0)
                    radius = 0.0f;

                if (m_spellInfo->Id == 50019)               // Hawk Hunting, problematic 50K radius
                    radius = 10.0f;

                float angle = m_caster->GetOrientation();
                switch(targetMode)
                {
                    case TARGET_DYNAMIC_OBJECT_FRONT:                           break;
                    case TARGET_DYNAMIC_OBJECT_BEHIND:      angle += M_PI_F;      break;
                    case TARGET_DYNAMIC_OBJECT_LEFT_SIDE:   angle += M_PI_F / 2;  break;
                    case TARGET_DYNAMIC_OBJECT_RIGHT_SIDE:  angle -= M_PI_F / 2;  break;
                }

                float x, y;
                m_caster->GetNearPoint2D(x, y, radius, angle);
                m_targets.setDestination(x, y, m_caster->GetPositionZ());
            }

            targetUnitMap.push_back(m_caster);
            break;
        }
        case TARGET_POINT_AT_NORTH:
        case TARGET_POINT_AT_SOUTH:
        case TARGET_POINT_AT_EAST:
        case TARGET_POINT_AT_WEST:
        case TARGET_POINT_AT_NE:
        case TARGET_POINT_AT_NW:
        case TARGET_POINT_AT_SE:
        case TARGET_POINT_AT_SW:
        {
            if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
            {
                Unit* currentTarget = m_targets.getUnitTarget() ? m_targets.getUnitTarget() : m_caster;
                float angle = currentTarget != m_caster ? currentTarget->GetAngle(m_caster) : m_caster->GetOrientation();

                switch(targetMode)
                {
                    case TARGET_POINT_AT_NORTH:                         break;
                    case TARGET_POINT_AT_SOUTH: angle +=   M_PI_F;        break;
                    case TARGET_POINT_AT_EAST:  angle -=   M_PI_F / 2;    break;
                    case TARGET_POINT_AT_WEST:  angle +=   M_PI_F / 2;    break;
                    case TARGET_POINT_AT_NE:    angle -=   M_PI_F / 4;    break;
                    case TARGET_POINT_AT_NW:    angle +=   M_PI_F / 4;    break;
                    case TARGET_POINT_AT_SE:    angle -= 3*M_PI_F / 4;    break;
                    case TARGET_POINT_AT_SW:    angle += 3*M_PI_F / 4;    break;
                }

                float x, y;
                currentTarget->GetNearPoint2D(x, y, radius, angle);
                m_targets.setDestination(x, y, currentTarget->GetPositionZ());
            }
            if (spellEffect->EffectMiscValueB == TARGET_NONE)
                targetUnitMap.push_back(m_caster);
            break;
        }
        case TARGET_DIRECTLY_FORWARD:
        {
            if (!(m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION))
            {
                SpellRangeEntry const* rEntry = sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex);
                float minRange = GetSpellMinRange(rEntry);
                float maxRange = GetSpellMaxRange(rEntry);
                float dist = minRange+ rand_norm_f()*(maxRange-minRange);

                float _target_x, _target_y, _target_z;
                m_caster->GetClosePoint(_target_x, _target_y, _target_z, m_caster->GetObjectBoundingRadius(), dist);
                m_targets.setDestination(_target_x, _target_y, _target_z);
            }

            if (IsPositiveSpell(m_spellInfo->Id))
                targetUnitMap.push_back(m_caster);
            // Siege Cannon
            else if (m_spellInfo->Id == 85123)
                targetUnitMap.push_back(m_targets.getUnitTarget());
            break;
        }
        case TARGET_EFFECT_SELECT:
        {
            // add here custom effects that need default target.
            // FOR EVERY TARGET TYPE THERE IS A DIFFERENT FILL!!
            if(!spellEffect)
                break;

            switch(spellEffect->Effect)
            {
                case SPELL_EFFECT_DUMMY:
                {
                    switch(m_spellInfo->Id)
                    {
                        case 20577:                         // Cannibalize
                        {
                            WorldObject* result = FindCorpseUsing<MaNGOS::CannibalizeObjectCheck> ();

                            if(result)
                            {
                                switch(result->GetTypeId())
                                {
                                    case TYPEID_UNIT:
                                    case TYPEID_PLAYER:
                                        targetUnitMap.push_back((Unit*)result);
                                        break;
                                    case TYPEID_CORPSE:
                                        m_targets.setCorpseTarget((Corpse*)result);
                                        if (Player* owner = ObjectAccessor::FindPlayer(((Corpse*)result)->GetOwnerGuid()))
                                            targetUnitMap.push_back(owner);
                                        break;
                                }
                            }
                            else
                            {
                                // clear cooldown at fail
                                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                                    ((Player*)m_caster)->RemoveSpellCooldown(m_spellInfo->Id, true);
                                SendCastResult(SPELL_FAILED_NO_EDIBLE_CORPSES);
                                finish(false);
                            }
                            break;
                        }
                        default:
                            if (m_targets.getUnitTarget())
                                targetUnitMap.push_back(m_targets.getUnitTarget());
                            break;
                    }
                    // Add AoE target-mask to self, if no target-dest provided already
                    if ((m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) == 0)
                        m_targets.setDestination(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ());
                    break;
                }
                case SPELL_EFFECT_BIND:
                case SPELL_EFFECT_RESURRECT:
                case SPELL_EFFECT_PARRY:
                case SPELL_EFFECT_BLOCK:
                case SPELL_EFFECT_CREATE_ITEM:
                case SPELL_EFFECT_WEAPON:
                case SPELL_EFFECT_TRIGGER_SPELL:
                case SPELL_EFFECT_TRIGGER_MISSILE:
                case SPELL_EFFECT_LEARN_SPELL:
                case SPELL_EFFECT_SKILL_STEP:
                case SPELL_EFFECT_PROFICIENCY:
                case SPELL_EFFECT_SUMMON_OBJECT_WILD:
                case SPELL_EFFECT_SELF_RESURRECT:
                case SPELL_EFFECT_REPUTATION:
                case SPELL_EFFECT_SEND_TAXI:
                    if (m_targets.getUnitTarget())
                        targetUnitMap.push_back(m_targets.getUnitTarget());
                    // Triggered spells have additional spell targets - cast them even if no explicit unit target is given (required for spell 50516 for example)
                    else if (spellEffect->Effect == SPELL_EFFECT_TRIGGER_SPELL)
                        targetUnitMap.push_back(m_caster);
                    break;
                case SPELL_EFFECT_SUMMON_PLAYER:
                case SPELL_EFFECT_FRIEND_SUMMON:
                    if (m_caster->GetTypeId()==TYPEID_PLAYER && ((Player*)m_caster)->GetSelectionGuid())
                        if (Player* target = sObjectMgr.GetPlayer(((Player*)m_caster)->GetSelectionGuid()))
                            targetUnitMap.push_back(target);
                    break;
                case SPELL_EFFECT_RESURRECT_NEW:
                    if (m_targets.getUnitTarget())
                        targetUnitMap.push_back(m_targets.getUnitTarget());
                    if (m_targets.getCorpseTargetGuid())
                    {
                        if (Corpse *corpse = m_caster->GetMap()->GetCorpse(m_targets.getCorpseTargetGuid()))
                            if (Player* owner = ObjectAccessor::FindPlayer(corpse->GetOwnerGuid()))
                                targetUnitMap.push_back(owner);
                    }
                    break;
                case SPELL_EFFECT_TELEPORT_UNITS:
                case SPELL_EFFECT_SUMMON:
                case SPELL_EFFECT_SUMMON_CHANGE_ITEM:
                case SPELL_EFFECT_TRANS_DOOR:
                case SPELL_EFFECT_ADD_FARSIGHT:
                case SPELL_EFFECT_APPLY_GLYPH:
                case SPELL_EFFECT_STUCK:
                case SPELL_EFFECT_BREAK_PLAYER_TARGETING:
                case SPELL_EFFECT_SUMMON_ALL_TOTEMS:
                case SPELL_EFFECT_FEED_PET:
                case SPELL_EFFECT_DESTROY_ALL_TOTEMS:
                case SPELL_EFFECT_SKILL:
                    targetUnitMap.push_back(m_caster);
                    break;
                case SPELL_EFFECT_PERSISTENT_AREA_AURA:
                    if(Unit* currentTarget = m_targets.getUnitTarget())
                        m_targets.setDestination(currentTarget->GetPositionX(), currentTarget->GetPositionY(), currentTarget->GetPositionZ());
                    break;
                case SPELL_EFFECT_LEARN_PET_SPELL:
                    if (Pet* pet = m_caster->GetPet())
                        targetUnitMap.push_back(pet);
                    break;
                case SPELL_EFFECT_ENCHANT_ITEM:
                case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
                case SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC:
                case SPELL_EFFECT_DISENCHANT:
                case SPELL_EFFECT_PROSPECTING:
                case SPELL_EFFECT_MILLING:
                    if (m_targets.getItemTarget())
                        AddItemTarget(m_targets.getItemTarget(), effIndex);
                    break;
                case SPELL_EFFECT_APPLY_AURA:
                    switch(spellEffect->EffectApplyAuraName)
                    {
                        case SPELL_AURA_ADD_FLAT_MODIFIER:  // some spell mods auras have 0 target modes instead expected TARGET_SELF(1) (and present for other ranks for same spell for example)
                        case SPELL_AURA_ADD_PCT_MODIFIER:
                            targetUnitMap.push_back(m_caster);
                            break;
                        default:                            // apply to target in other case
                            if (m_targets.getUnitTarget())
                                targetUnitMap.push_back(m_targets.getUnitTarget());
                            break;
                    }
                    break;
                case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
                    // AreaAura
                    if ((m_spellInfo->Attributes == (SPELL_ATTR_NOT_SHAPESHIFT | SPELL_ATTR_UNK18 | SPELL_ATTR_CASTABLE_WHILE_MOUNTED | SPELL_ATTR_CASTABLE_WHILE_SITTING)) || (m_spellInfo->Attributes == SPELL_ATTR_NOT_SHAPESHIFT))
                        SetTargetMap(effIndex, TARGET_AREAEFFECT_PARTY, targetUnitMap);
                    break;
                case SPELL_EFFECT_SKIN_PLAYER_CORPSE:
                    if (m_targets.getUnitTarget())
                        targetUnitMap.push_back(m_targets.getUnitTarget());
                    else if (m_targets.getCorpseTargetGuid())
                    {
                        if (Corpse *corpse = m_caster->GetMap()->GetCorpse(m_targets.getCorpseTargetGuid()))
                            if (Player* owner = ObjectAccessor::FindPlayer(corpse->GetOwnerGuid()))
                                targetUnitMap.push_back(owner);
                    }
                    break;
                case SPELL_EFFECT_SUMMON_RAID_MARKER:
                case SPELL_EFFECT_BUY_GUILD_BANKSLOT:
                    targetUnitMap.push_back(m_caster);
                    break;
                default:
                    break;
            }
            break;
        }
        default:
            //ERROR_LOG( "SPELL: Unknown implicit target (%u) for spell ID %u", targetMode, m_spellInfo->Id );
            break;
    }

    if (unMaxTargets && targetUnitMap.size() > unMaxTargets)
    {
        // make sure one unit is always removed per iteration
        uint32 removed_utarget = 0;
        for (UnitList::iterator itr = targetUnitMap.begin(), next; itr != targetUnitMap.end(); itr = next)
        {
            next = itr;
            ++next;
            if (!*itr) continue;
            if ((*itr) == m_targets.getUnitTarget())
            {
                targetUnitMap.erase(itr);
                removed_utarget = 1;
                //        break;
            }
        }
        // remove random units from the map
        while (targetUnitMap.size() > unMaxTargets - removed_utarget)
        {
            uint32 poz = urand(0, targetUnitMap.size()-1);
            for (UnitList::iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr, --poz)
            {
                if (!*itr) continue;

                if (!poz)
                {
                    targetUnitMap.erase(itr);
                    break;
                }
            }
        }
        // the player's target will always be added to the map
        if (removed_utarget && m_targets.getUnitTarget())
            targetUnitMap.push_back(m_targets.getUnitTarget());
    }

    if (!tempTargetGOList.empty())                          // GO CASE
    {
        if (unMaxTargets && tempTargetGOList.size() > unMaxTargets)
        {
            // make sure one go is always removed per iteration
            uint32 removed_utarget = 0;
            for (std::list<GameObject*>::iterator itr = tempTargetGOList.begin(), next; itr != tempTargetGOList.end(); itr = next)
            {
                next = itr;
                ++next;
                if (!*itr) continue;
                if ((*itr) == m_targets.getGOTarget())
                {
                    tempTargetGOList.erase(itr);
                    removed_utarget = 1;
                    //        break;
                }
            }
            // remove random units from the map
            while (tempTargetGOList.size() > unMaxTargets - removed_utarget)
            {
                uint32 poz = urand(0, tempTargetGOList.size()-1);
                for (std::list<GameObject*>::iterator itr = tempTargetGOList.begin(); itr != tempTargetGOList.end(); ++itr, --poz)
                {
                    if (!*itr) continue;

                    if (!poz)
                    {
                        tempTargetGOList.erase(itr);
                        break;
                    }
                }
            }
        }
        // Add resulting GOs as GOTargets
        for (std::list<GameObject*>::iterator iter = tempTargetGOList.begin(); iter != tempTargetGOList.end(); ++iter)
            AddGOTarget(*iter, effIndex);
    }
}

void Spell::prepare(SpellCastTargets const* targets, Aura* triggeredByAura)
{
    m_targets = *targets;

    m_spellState = SPELL_STATE_PREPARING;

    m_castPositionX = m_caster->GetPositionX();
    m_castPositionY = m_caster->GetPositionY();
    m_castPositionZ = m_caster->GetPositionZ();
    m_castOrientation = m_caster->GetOrientation();

    if(triggeredByAura)
        m_triggeredByAuraSpell  = triggeredByAura->GetSpellProto();

    // create and add update event for this spell
    SpellEvent* Event = new SpellEvent(this);
    m_caster->m_Events.AddEvent(Event, m_caster->m_Events.CalculateTime(1));

    //Prevent casting at cast another spell (ServerSide check)
    if(m_caster->IsNonMeleeSpellCasted(false, true, true) && m_cast_count)
    {
        SendCastResult(SPELL_FAILED_SPELL_IN_PROGRESS);
        finish(false);
        return;
    }

    // Fill cost data
    m_powerCost = CalculatePowerCost(m_spellInfo, m_caster, this, m_CastItem);

    SpellCastResult result = CheckCast(true);
    if(result != SPELL_CAST_OK && !IsAutoRepeat())          //always cast autorepeat dummy for triggering
    {
        if(triggeredByAura && triggeredByAura->GetId() != 53563 && triggeredByAura->IsPeriodic() && !triggeredByAura->GetHolder()->IsPassive())
        {
            SendChannelUpdate(0);
            triggeredByAura->GetHolder()->SetAuraDuration(0);
        }
        SendCastResult(result);
        finish(false);
        return;
    }

    // Prepare data for triggers
    prepareDataForTriggerSystem();

    // calculate cast time (calculated after first CheckCast check to prevent charge counting for first CheckCast fail)
    m_casttime = GetSpellCastTime(m_spellInfo, this);
    m_duration = CalculateSpellDuration(m_spellInfo, m_caster);

    // set timer base at cast time
    ReSetTimer();

    // stealth must be removed at cast starting (at show channel bar)
    // skip triggered spell (item equip spell casting and other not explicit character casts/item uses)
    if ((!m_IsTriggeredSpell || m_spellInfo->Id == SPELL_ID_AUTOSHOT) && isSpellBreakStealth(m_spellInfo) && m_casttime > 0)
    {
        m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CAST);
        // clear unit emote state
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->HandleEmote(EMOTE_ONESHOT_NONE);

        for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (SpellEffectEntry const* eff = m_spellInfo->GetSpellEffect(SpellEffectIndex(i)))
                if (m_targets.getUnitTarget())
                {
                    m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_SPELL_ATTACK);
                    break;
                }
    }

    // add non-triggered (with cast time and without)
    if (!m_IsTriggeredSpell)
    {
        // add to cast type slot
        m_caster->SetCurrentCastedSpell( this );

        // will show cast bar
        SendSpellStart();

        TriggerGlobalCooldown();
    }
    // execute triggered without cast time explicitly in call point
    else if(m_timer == 0)
        cast(true);
    // else triggered with cast time will execute execute at next tick or later
    // without adding to cast type slot
    // will not show cast bar but will show effects at casting time etc

    //if (m_spellInfo->speed > 0.0f && GetCastTime())
    //{
    //    Unit* procTarget = m_targets.getUnitTarget();
    //    if (!procTarget)
    //        procTarget = m_caster;

    //    m_caster->ProcDamageAndSpell(procTarget, m_procAttacker, PROC_FLAG_NONE, PROC_EX_CAST_END, 0, 0, m_attackType, m_spellInfo);
    //}
}

void Spell::cancel()
{
    if(m_spellState == SPELL_STATE_FINISHED)
        return;

    // channeled spells don't display interrupted message even if they are interrupted, possible other cases with no "Interrupted" message
    bool sendInterrupt = IsChanneledSpell(m_spellInfo) ? false : true;

    m_autoRepeat = false;
    switch (m_spellState)
    {
        case SPELL_STATE_PREPARING:
            CancelGlobalCooldown();

            //(no break)
        case SPELL_STATE_DELAYED:
        {
            SendInterrupted(0);

            if (sendInterrupt)
                SendCastResult(SPELL_FAILED_INTERRUPTED);
        } break;

        case SPELL_STATE_CASTING:
        {
            for(TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            {
                if (ihit->missCondition == SPELL_MISS_NONE)
                {
                    Unit* unit = m_caster->GetObjectGuid() == (*ihit).targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
                    if (unit && unit->isAlive())
                        unit->RemoveAurasByCasterSpell(m_spellInfo->Id, m_caster->GetObjectGuid());

                    // prevent other effects applying if spell is already interrupted
                    // i.e. if effects have different targets and it was interrupted on one of them when 
                    // haven't yet applied to another
                    ihit->processed = true;
                }
            }

            SendChannelUpdate(0);
            SendInterrupted(0);

            if (sendInterrupt)
                SendCastResult(SPELL_FAILED_INTERRUPTED);
        } break;

        default:
        {
        } break;
    }

    finish(false);
    m_caster->RemoveDynObject(m_spellInfo->Id);
    m_caster->RemoveGameObject(m_spellInfo->Id, true);
}

void Spell::cast(bool skipCheck)
{
    SetExecutedCurrently(true);

    if (!m_caster->CheckAndIncreaseCastCounter())
    {
        if (m_triggeredByAuraSpell)
            ERROR_LOG("Spell %u triggered by aura spell %u too deep in cast chain for cast. Cast not allowed for prevent overflow stack crash.", m_spellInfo->Id, m_triggeredByAuraSpell->Id);
        else
            ERROR_LOG("Spell %u too deep in cast chain for cast. Cast not allowed for prevent overflow stack crash.", m_spellInfo->Id);

        SendCastResult(SPELL_FAILED_ERROR);
        finish(false);
        SetExecutedCurrently(false);
        return;
    }

    // update pointers base at GUIDs to prevent access to already nonexistent object
    UpdatePointers();

    // cancel at lost main target unit
    if (!m_targets.getUnitTarget() && m_targets.getUnitTargetGuid() && m_targets.getUnitTargetGuid() != m_caster->GetObjectGuid())
    {
        cancel();
        m_caster->DecreaseCastCounter();
        SetExecutedCurrently(false);
        return;
    }

    if (m_caster->GetTypeId() != TYPEID_PLAYER && m_targets.getUnitTarget() && m_targets.getUnitTarget() != m_caster)
        m_caster->SetInFront(m_targets.getUnitTarget());

    SpellCastResult castResult = CheckPower();
    if (castResult != SPELL_CAST_OK)
    {
        SendCastResult(castResult);
        finish(false);
        m_caster->DecreaseCastCounter();
        SetExecutedCurrently(false);
        return;
    }

    // triggered cast called from Spell::prepare where it was already checked
    if (!skipCheck)
    {
        castResult = CheckCast(false);
        if(castResult != SPELL_CAST_OK)
        {
            SendCastResult(castResult);
            finish(false);
            m_caster->DecreaseCastCounter();
            SetExecutedCurrently(false);
            return;
        }
    }

    // skip triggered spell (item equip spell casting and other not explicit character casts/item uses)
    if ((!m_IsTriggeredSpell || m_spellInfo->Id == SPELL_ID_AUTOSHOT) && isSpellBreakStealth(m_spellInfo) && m_casttime == 0)
    {
        m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_CAST);
        // clear unit emote state
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            m_caster->HandleEmote(EMOTE_ONESHOT_NONE);

        for (uint32 i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (SpellEffectEntry const* eff = m_spellInfo->GetSpellEffect(SpellEffectIndex(i)))
                if (m_targets.getUnitTarget())
                {
                    m_caster->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_SPELL_ATTACK);
                    break;
                }
    }

    SpellClassOptionsEntry const* classOpt = m_spellInfo->GetSpellClassOptions();

    // different triggered (for caster) and precast (casted before apply effect to target) cases
    switch(m_spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            // Bandages
            if (m_spellInfo->GetMechanic() == MECHANIC_BANDAGE)
                AddPrecastSpell(11196);                     // Recently Bandaged
            // Stoneclaw Totem
            else if (m_spellInfo->Id == 5728)
                AddTriggeredSpell(55328);
            // Stoneskin
            else if (m_spellInfo->Id == 20594)
                AddTriggeredSpell(65116);                   // Stoneskin - armor 10% for 8 sec
            else if(m_spellInfo->Id == 7744)                // Will of the Forsaken
                AddTriggeredSpell(72757);                   // PvP trinket Cooldown
            else if(m_spellInfo->Id == 42292)               // PvP trinket
                AddTriggeredSpell(72752);                   // Will of the Forsaken Cooldown
            else if (m_spellInfo->Id == 68992)              // Darkflight
            {
                AddPrecastSpell(96223);                     // Run Speed Marker
                if (m_caster->HasWorgenForm())
                    AddPrecastSpell(97709);                 // Altered Form
            }
            else if (m_spellInfo->Id == 68996)              // Two Forms
            {
                if (m_caster->IsInWorgenForm())
                    m_caster->RemoveSpellsCausingAura(SPELL_AURA_WORGEN_TRANSFORM);
                else if (m_caster->HasWorgenForm())
                    AddPrecastSpell(97709);                 // Altered Form
            }
            else if (m_spellInfo->Id == 69070)              // Rocket Jump (Racial)
                AddTriggeredSpell(79636);                   // No Feather Fall
            // Chaos Bane strength buff
            else if (m_spellInfo->Id == 71904)
                AddTriggeredSpell(73422);
            else if (m_spellInfo->Id == 74607)
                AddTriggeredSpell(74610);                  // Fiery combustion
            else if (m_spellInfo->Id == 74799)
                AddTriggeredSpell(74800);                  // Soul consumption
            else if (m_spellInfo->Id == 61968)             // Flash Freeze (Hodir: Ulduar)
                AddTriggeredSpell(62148);                  // visual effect
            else if (m_spellInfo->Id == 58672)             // Impale, damage and loose threat effect (Vault of Archavon, Archavon the Stone Watcher)
                AddPrecastSpell(m_caster->GetMap()->IsRegularDifficulty() ? 58666 : 60882);
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Ice Block
            if (classOpt && classOpt->SpellFamilyFlags & UI64LIT(0x0000008000000000))
                AddPrecastSpell(41425);                     // Hypothermia
            // Icy Veins
            // Amaru: our's better
            //else if (m_spellInfo->Id == 12472)
            //{
            //    if (m_caster->HasAura(56374))               // Glyph of Icy Veins
            //    {
            //        // not exist spell do it so apply directly
            //        m_caster->RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
            //        m_caster->RemoveSpellsCausingAura(SPELL_AURA_HASTE_SPELLS);
            //    }
            //}
            // Frostbite
            else if (m_spellInfo->Id == 12494)
            {
                if (m_caster->HasAura(44543) || m_caster->HasAura(44545))
                    AddTriggeredSpell(44544);   // Fingers of Frost
            }
            // Slow
            else if (m_spellInfo->Id == 31589)
            {
                if (IsTriggeredSpell())
                    AddPrecastSpell(86262);     // Nether Vortex
            }

            // Polymorph
            if (m_caster->GetTypeId() == TYPEID_PLAYER && m_spellInfo->IsFitToFamilyMask(UI64LIT(0x1000000)) &&
                m_spellInfo->GetEffectApplyAuraNameByIndex(EFFECT_INDEX_0) == SPELL_AURA_MOD_CONFUSE && m_spellInfo->SpellIconID == 82)
            {
                // Glyph of the Penguid
                if (m_caster->HasAura(52648))
                    AddTriggeredSpell(61635);       // Sound Effect: Polymorph Penguin
                // Glyph of the Monkey
                else if (m_caster->HasAura(57927))
                    AddTriggeredSpell(89729);       // Sound Effect: Polymorph Monkey
                else
                    AddTriggeredSpell(61634);       // Sound Effect: Polymorph Sheeps
            }

            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Bloodrage
            if (classOpt && (classOpt->SpellFamilyFlags & UI64LIT(0x0000000000000100)))
            {
                if (m_caster->HasAura(70844))               // Item - Warrior T10 Protection 4P Bonus
                    AddTriggeredSpell(70845);               // Stoicism
            }
            else if (m_spellInfo->Id == 1715)               // Hamstring
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER && m_targets.getUnitTarget() && m_targets.getUnitTarget()->HasAura(m_spellInfo->Id))
                {
                    // Search Improved Hamstring
                    Unit::AuraList const& triggerAuras = m_caster->GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
                    for (Unit::AuraList::const_iterator itr = triggerAuras.begin(); itr != triggerAuras.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->SpellIconID == 23 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_WARRIOR)
                        {
                            SpellEffectEntry const* effect = (*itr)->GetSpellEffect();
                            uint32 triggerSpell = effect->EffectTriggerSpell;
                            if (!((Player*)m_caster)->HasSpellCooldown(triggerSpell))
                            {
                                AddTriggeredSpell(triggerSpell);
                                ((Player*)m_caster)->AddSpellCooldown(triggerSpell, 0, time(NULL) + effect->EffectBasePoints);
                            }
                            break;
                        }
                    }
                }
            }
            // Shattering Throw
            else if (m_spellInfo->Id == 64382)
                AddTriggeredSpell(64380);
            // Bloodsurge (triggered)
            else if (m_spellInfo->Id == 46916)
            {
                // Item - Warrior T10 Melee 4P Bonus
                if (Aura *aur = m_caster->GetAura(70847, EFFECT_INDEX_0))
                {
                    if (roll_chance_i(aur->GetModifier()->m_amount))
                    {
                        AddTriggeredSpell(70849);           // Extra Charge!
                        // Slam! trigger Slam GCD Reduced . Sudden Death trigger Execute GCD Reduced
                        int32 gcd_spell=m_spellInfo->Id==46916 ? 71072 : 71069;
                        AddPrecastSpell(gcd_spell);
                    }
                }
            }
            // Colossus Smash
            else if (m_spellInfo->Id == 86346 || m_spellInfo->Id == 108126)
            {
                // Glyph of Colossus Smash
                if (m_caster->HasAura(89003))
                    AddTriggeredSpell(58567);
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            if (m_spellInfo->Id == 80398)                   // Dark Intent
                AddTriggeredSpell(85768);                   // caster haste part
            else if (m_spellInfo->Id == 86213)              // Soul Swap Exhale
                AddPrecastSpell(92794);
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Prayer of Mending (jump animation), we need formal caster instead original for correct animation
            if (classOpt && classOpt->SpellFamilyFlags & UI64LIT(0x0000002000000000))
                AddTriggeredSpell(41637);

            switch(m_spellInfo->Id)
            {
                case 15237: AddTriggeredSpell(23455); break;// Holy Nova
                case 81661:                                 // Evangelism
                case 87118:                                 // Dark Evangelism
                {
                    AddTriggeredSpell(87154);               // Archangel marker
                    break;
                }
                case 89485:                                 // Inner Focus
                {
                    Unit::AuraList const& vProcAuras = m_caster->GetAurasByType(SPELL_AURA_PROC_TRIGGER_SPELL);
                    for (Unit::AuraList::const_iterator itr = vProcAuras.begin(); itr != vProcAuras.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_PRIEST &&
                            (*itr)->GetSpellProto()->SpellIconID == 177)
                        {
                            if ((*itr)->GetId() == 89488)       // Strength of Soul (Rank 1)
                                AddTriggeredSpell(96266);
                            else if ((*itr)->GetId() == 89489)  // Strength of Soul (Rank 2)
                                AddTriggeredSpell(96267);
                            break;
                        }
                    }
                    break;
                }
                default:break;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Faerie Fire (Feral)
            if (m_spellInfo->Id == 16857 && m_caster->GetShapeshiftForm() != FORM_CAT)
                AddTriggeredSpell(60089);
            // Clearcasting
            else if (m_spellInfo->Id == 16870)
            {
                if (m_caster->HasAura(70718))               // Item - Druid T10 Balance 2P Bonus
                    AddPrecastSpell(70721);                 // Omen of Doom
            }
            // Lifebloom
            else if (m_spellInfo->Id == 33763 || m_spellInfo->Id == 94447)
            {
                // Revitalize
                if (m_caster->GetTypeId() == TYPEID_PLAYER && ((Player*)m_caster)->GetKnownTalentRankById(8269))
                {
                    // Replenishment
                    AddTriggeredSpell(57669);
                }
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Deterrence
            if (m_spellInfo->Id == 19263)
            {
                AddPrecastSpell(67801);
                AddPrecastSpell(114406);
            }
            // Kill Command
            else if (m_spellInfo->Id == 34026)
            {
                if (m_caster->HasAura(37483))               // Improved Kill Command - Item set bonus
                    m_caster->CastSpell(m_caster, 37482, true);// Exploited Weakness
            }
            // Lock and Load
            else if (m_spellInfo->Id == 56453)
                AddPrecastSpell(67544);                     // Lock and Load Marker
            // Fervor
            else if (m_spellInfo->Id == 82726)
                AddTriggeredSpell(82728);                   // Fervor
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Divine Illumination
            // Amaru: our's better
            //if (m_spellInfo->Id == 31842)
            //{
            //    if (m_caster->HasAura(70755))               // Item - Paladin T10 Holy 2P Bonus
            //        AddPrecastSpell(71166);                 // Divine Illumination
            //}
            // Hand of Reckoning
            if (m_spellInfo->Id == 62124)
            {
                if (m_targets.getUnitTarget() && m_targets.getUnitTarget()->getVictim() != m_caster)
                    AddPrecastSpell(67485);                 // Hand of Rekoning (no typos in name ;) )
            }
            // Divine Shield or Hand of Protection
            else if (classOpt && classOpt->SpellFamilyFlags & UI64LIT(0x0000000000000080))
                AddPrecastSpell(25771);                     // Forbearance
            // Lay on Hands
            else if (classOpt && classOpt->SpellFamilyFlags & UI64LIT(0x0000000000008000))
            {
                AddPrecastSpell(25771);                     // Forbearance
                // Glyph of Divinity
                if (m_caster->HasAura(54939))
                    AddTriggeredSpell(54986);
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(EFFECT_INDEX_0);
            // Stormstrike
            if (m_spellInfo->Id == 17364)
            {
                // Item - Shaman T12 Enhancement 4P Bonus
                if (m_caster->HasAura(99213))
                    AddTriggeredSpell(99212);
            }
            // Spirit Walk
            else if (m_spellInfo->Id == 58875)
                AddPrecastSpell(58876);
            // Spirit Link Totem
            else if (m_spellInfo->Id == 98007)
                AddTriggeredSpell(98017);
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Chains of Ice
            if (m_spellInfo->Id == 45524)
                AddTriggeredSpell(55095);                   // Frost Fever
            // Strangulate
            if (m_spellInfo->Id == 47476)
            {
                if (Unit* target = m_targets.getUnitTarget())
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        AddTriggeredSpell(32747);           // Interrupt
                break;
            }
            // Dancing Rune Weapon
            else if (m_spellInfo->Id == 49028)              // Dancing Rune Weapon (parry)
                AddTriggeredSpell(81256);
            // Pestilence
            else if (m_spellInfo->Id == 50842)
                AddPrecastSpell(76243);                     // dot damage reducer
            // Vampiric Blood
            else if (m_spellInfo->Id == 55233)
            {
                // Item - Death Knight T13 Blood 4P Bonus
                if (m_caster->HasAura(105587))
                    AddTriggeredSpell(105588);
            }
            // Explode
            //else if (m_spellInfo->Id == 47496)
            //    AddTriggeredSpell(53730);                   // Knock Back
            //// Corpse Explosion
            //else if (m_spellInfo->Id == 50444)
            //    AddTriggeredSpell(53730);                   // Knock Back
            // Corpse Explosion
            //else if (m_spellInfo->Id == 50444)
            //    AddTriggeredSpell(43999);                   // Die
            break;
        }
        default:
            break;
    }

    // traded items have trade slot instead of guid in m_itemTargetGUID
    // set to real guid to be sent later to the client
    m_targets.updateTradeSlotItem();

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (!m_IsTriggeredSpell && m_CastItem)
        {
            ((Player*)m_caster)->GetAchievementMgr().StartTimedAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM, m_CastItem->GetEntry());
            ((Player*)m_caster)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_USE_ITEM, m_CastItem->GetEntry());
        }

        ((Player*)m_caster)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_CAST_SPELL, m_spellInfo->Id);
    }

    FillTargetMap();

    if (m_spellState == SPELL_STATE_FINISHED)               // stop cast if spell marked as finish somewhere in FillTargetMap
    {
        m_caster->DecreaseCastCounter();
        SetExecutedCurrently(false);
        return;
    }

    // CAST SPELL
    SendSpellCooldown();

    TakePower();
    TakeReagents();                                         // we must remove reagents before HandleEffects to allow place crafted item in same slot

    SendCastResult(castResult);
    SendSpellGo();                                          // we must send smsg_spell_go packet before m_castItem delete in TakeCastItem()...

    InitializeDamageMultipliers();

    Unit *procTarget = m_targets.getUnitTarget();
    if (!procTarget)
        procTarget = m_caster;

    // Okay, everything is prepared. Now we need to distinguish between immediate and evented delayed spells
    if (m_spellInfo->speed > 0.0f || m_spellInfo->Id == 14157 || m_spellInfo->Id == 64380)                // hack for Ruthlessness & Shattering Throw
    {
        // Remove used for cast item if need (it can be already NULL after TakeReagents call
        // in case delayed spell remove item at cast delay start
        TakeCastItem();

        // fill initial spell damage from caster for delayed casted spells
        for(TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            HandleDelayedSpellLaunch(&(*ihit));

        // Okay, maps created, now prepare flags
        m_immediateHandled = false;
        m_spellState = SPELL_STATE_DELAYED;
        SetDelayStart(0);

        // on spell cast end proc, 
        // critical hit related part is currently done on hit so proc there, 
        // 0 damage since any damage based procs should be on hit
        // 0 victim proc since there is no victim proc dependent on successfull cast for caster
        // if m_casttime > 0  proc already made in prepare()
        //if (!GetCastTime())
            m_caster->ProcDamageAndSpell(procTarget, m_procAttacker, PROC_FLAG_NONE, PROC_EX_CAST_END, 0, 0, m_attackType, m_spellInfo);
    }
    else
    {
        if (GetCastTime())
        {
            m_caster->ProcDamageAndSpell(procTarget, m_procAttacker, PROC_FLAG_NONE, PROC_EX_CAST_END, 0, 0, m_attackType, m_spellInfo);
            // Immediate spell, no big deal
            handle_immediate();
        }
        else
        {
            handle_immediate();
            m_caster->ProcDamageAndSpell(procTarget, m_procAttacker, PROC_FLAG_NONE, PROC_EX_CAST_END, 0, 0, m_attackType, m_spellInfo);
        }
    }

    m_caster->DecreaseCastCounter();
    SetExecutedCurrently(false);
}

void Spell::handle_immediate()
{
    // start channeling if applicable
    // process immediate effects (items, ground, etc.) also initialize some variables
    _handle_immediate_phase();

    // start channeling if applicable (after _handle_immediate_phase for get persistent effect dynamic object for channel target
    if (IsChanneledSpell(m_spellInfo) && m_duration)
    {
        m_spellState = SPELL_STATE_CASTING;
        SendChannelStart(m_duration);
    }

    for(TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    for(GOTargetList::iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    // spell is finished, perform some last features of the spell here
    _handle_finish_phase();

    // Remove used for cast item if need (it can be already NULL after TakeReagents call
    TakeCastItem();

    if(m_spellState != SPELL_STATE_CASTING)
        finish(true);                                       // successfully finish spell cast (not last in case autorepeat or channel spell)
}

uint64 Spell::handle_delayed(uint64 t_offset)
{
    uint64 next_time = 0;

    if (!m_immediateHandled)
    {
        _handle_immediate_phase();
        m_immediateHandled = true;
    }

    // now recheck units targeting correctness (need before any effects apply to prevent adding immunity at first effect not allow apply second spell effect and similar cases)
    for(TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (!ihit->processed)
        {
            if (ihit->timeDelay <= t_offset)
                DoAllEffectOnTarget(&(*ihit));
            else if (next_time == 0 || ihit->timeDelay < next_time)
                next_time = ihit->timeDelay;
        }
    }

    // now recheck gameobject targeting correctness
    for(GOTargetList::iterator ighit = m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
    {
        if (!ighit->processed)
        {
            if (ighit->timeDelay <= t_offset)
                DoAllEffectOnTarget(&(*ighit));
            else if (next_time == 0 || ighit->timeDelay < next_time)
                next_time = ighit->timeDelay;
        }
    }
    // All targets passed - need finish phase
    if (next_time == 0)
    {
        // spell is finished, perform some last features of the spell here
        _handle_finish_phase();

        finish(true);                                       // successfully finish spell cast

        // return zero, spell is finished now
        return 0;
    }
    else
    {
        // spell is unfinished, return next execution time
        return next_time;
    }
}

void Spell::_handle_immediate_phase()
{
    // handle some immediate features of the spell here
    HandleThreatSpells();

    m_needSpellLog = IsNeedSendToClient();
    for(int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
        if(!spellEffect || spellEffect->Effect == 0)
            continue;

        // apply Send Event effect to ground in case empty target lists
        if( spellEffect->Effect == SPELL_EFFECT_SEND_EVENT && !HaveTargetsForEffect(SpellEffectIndex(j)) )
        {
            HandleEffects(NULL, NULL, NULL, SpellEffectIndex(j));
            continue;
        }

        // Don't do spell log, if is school damage spell
        if(spellEffect->Effect == SPELL_EFFECT_SCHOOL_DAMAGE || spellEffect->Effect == 0)
            m_needSpellLog = false;
    }

    // initialize Diminishing Returns Data
    m_diminishLevel = DIMINISHING_LEVEL_1;
    m_diminishGroup = DIMINISHING_NONE;

    // process items
    for(ItemTargetList::iterator ihit = m_UniqueItemInfo.begin(); ihit != m_UniqueItemInfo.end(); ++ihit)
        DoAllEffectOnTarget(&(*ihit));

    // process ground
    for(int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
        if(!spellEffect)
            continue;
        // persistent area auras target only the ground
        if(spellEffect->Effect == SPELL_EFFECT_PERSISTENT_AREA_AURA)
            HandleEffects(NULL, NULL, NULL, SpellEffectIndex(j));
    }
}

void Spell::_handle_finish_phase()
{
    // spell log
    if(m_needSpellLog)
        SendLogExecute();
}

void Spell::SendSpellCooldown()
{
    if(m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* _player = (Player*)m_caster;

    // mana/health/etc potions, disabled by client (until combat out as declarate)
    if (m_CastItem && m_CastItem->IsPotion())
    {
        // need in some way provided data for Spell::finish SendCooldownEvent
        _player->SetLastPotionId(m_CastItem->GetEntry());
        return;
    }

    // (1) have infinity cooldown but set at aura apply, (2) passive cooldown at triggering
    if (m_spellInfo->HasAttribute(SPELL_ATTR_DISABLED_WHILE_ACTIVE) || m_spellInfo->HasAttribute(SPELL_ATTR_PASSIVE))
        return;

    _player->AddSpellAndCategoryCooldowns(m_spellInfo, m_CastItem ? m_CastItem->GetEntry() : 0, this);
}

void Spell::update(uint32 difftime)
{
    // update pointers based at it's GUIDs
    UpdatePointers();

    if (m_targets.getUnitTargetGuid() && !m_targets.getUnitTarget())
    {
        cancel();
        return;
    }

    SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(EFFECT_INDEX_0);
    SpellInterruptsEntry const* spellInterrupts = m_spellInfo->GetSpellInterrupts();

    // check if the player caster has moved before the spell finished (exclude casts on vehicles)
    if (!m_caster->GetVehicle() && m_caster->GetTypeId() == TYPEID_PLAYER && m_timer != 0 &&
        (m_castPositionX != m_caster->GetPositionX() || m_castPositionY != m_caster->GetPositionY() || m_castPositionZ != m_caster->GetPositionZ()) &&
        (spellEffect && spellEffect->Effect != SPELL_EFFECT_STUCK || !((Player*)m_caster)->m_movementInfo.HasMovementFlag(MOVEFLAG_FALLINGFAR)) &&
        !m_caster->HasAffectedAura(SPELL_AURA_ALLOW_CAST_WHILE_MOVING, m_spellInfo))
    {
        // always cancel for channeled spells
        if( m_spellState == SPELL_STATE_CASTING )
            cancel();
        // don't cancel for melee, autorepeat, triggered and instant spells
        else if(!IsNextMeleeSwingSpell() && !IsAutoRepeat() && !m_IsTriggeredSpell && (spellInterrupts && spellInterrupts->InterruptFlags & SPELL_INTERRUPT_FLAG_MOVEMENT))
            cancel();
    }

    switch(m_spellState)
    {
        case SPELL_STATE_PREPARING:
        {
            if(m_timer)
            {
                if(difftime >= m_timer)
                    m_timer = 0;
                else
                    m_timer -= difftime;
            }

            if(m_timer == 0 && !IsNextMeleeSwingSpell() && !IsAutoRepeat())
            {
                Unit* target = m_targets.getUnitTarget();
                if (target && target != m_caster)
                {
                    if (target->IsHostileTo(m_caster) && !target->isVisibleForOrDetect(m_caster, m_caster, true))
                    {
                        cancel();
                        return;
                    }
                }

                cast();
            }
        } break;
        case SPELL_STATE_CASTING:
        {
            if(m_timer > 0)
            {
                if( m_caster->GetTypeId() == TYPEID_PLAYER )
                {
                    // check if player has jumped before the channeling finished
                    if (((Player*)m_caster)->m_movementInfo.HasMovementFlag(MOVEFLAG_FALLING) &&
                        !m_caster->HasAffectedAura(SPELL_AURA_ALLOW_CAST_WHILE_MOVING, m_spellInfo))
                        cancel();

                    // check for incapacitating player states
                    if( m_caster->hasUnitState(UNIT_STAT_CAN_NOT_REACT))
                        cancel();

                    // check if player has turned if flag is set
                    if( spellInterrupts && (spellInterrupts->ChannelInterruptFlags & CHANNEL_FLAG_TURNING) && m_castOrientation != m_caster->GetOrientation() )
                        cancel();
                }

                // check if there are alive targets left
                if (!IsAliveUnitPresentInTargetList())
                {
                    SendChannelUpdate(0);
                    finish();
                }

                if(difftime >= m_timer)
                    m_timer = 0;
                else
                    m_timer -= difftime;
            }

            if (m_timer == 0)
            {
                SendChannelUpdate(0);

                // channeled spell processed independently for quest targeting
                // cast at creature (or GO) quest objectives update at successful cast channel finished
                // ignore autorepeat/melee casts for speed (not exist quest for spells (hm... )
                if( !IsAutoRepeat() && !IsNextMeleeSwingSpell() )
                {
                    if ( Player* p = m_caster->GetCharmerOrOwnerPlayerOrPlayerItself() )
                    {
                        for (TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                        {
                            TargetInfo const &target = *ihit;
                            if (!target.targetGUID.IsCreatureOrVehicle())
                                continue;

                            Unit* unit = m_caster->GetObjectGuid() == target.targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, target.targetGUID);
                            if (unit == NULL)
                                continue;

                            p->RewardPlayerAndGroupAtCast(unit, m_spellInfo->Id);
                        }

                        for(GOTargetList::const_iterator ihit = m_UniqueGOTargetInfo.begin(); ihit != m_UniqueGOTargetInfo.end(); ++ihit)
                        {
                            GOTargetInfo const &target = *ihit;

                            GameObject* go = m_caster->GetMap()->GetGameObject(target.targetGUID);
                            if(!go)
                                continue;

                            p->RewardPlayerAndGroupAtCast(go, m_spellInfo->Id);
                        }
                    }
                }

                finish();
            }
        } break;
        default:
        {
        }break;
    }
}

void Spell::finish(bool ok)
{
    if (!m_caster)
        return;

    if (m_spellState == SPELL_STATE_FINISHED)
        return;

    m_spellState = SPELL_STATE_FINISHED;

    // other code related only to successfully finished spells
    if (!ok)
        return;

    // handle SPELL_AURA_ADD_TARGET_TRIGGER auras
    Unit* caster = NULL;
    if (m_originalCasterGUID)
        caster = m_caster->GetMap() ? m_caster->GetMap()->GetUnit(m_originalCasterGUID) : NULL;
    else
        caster = m_caster;

    if (caster)
    {
        Unit::AuraList const& targetTriggers = caster->GetAurasByType(SPELL_AURA_ADD_TARGET_TRIGGER);
        for (Unit::AuraList::const_iterator i = targetTriggers.begin(); i != targetTriggers.end(); ++i)
        {
            if (!(*i)->isAffectedOnSpell(m_spellInfo))
                continue;

            for (TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            {
                if (ihit->missCondition == SPELL_MISS_NONE)
                {
                    // check caster->GetGUID() let load auras at login and speedup most often case
                    Unit* unit = caster->GetObjectGuid() == ihit->targetGUID ? caster : ObjectAccessor::GetUnit(*caster, ihit->targetGUID);

                    if (unit && unit->isAlive())
                    {
                        SpellEntry const* auraSpellInfo = (*i)->GetSpellProto();
                        SpellEffectIndex auraSpellIdx = (*i)->GetEffIndex();
                        // Calculate chance at that moment (can be depend for example from combo points)
                        int32 auraBasePoints = (*i)->GetBasePoints();
                        int32 chance = m_caster->CalculateSpellDamage(unit, auraSpellInfo, auraSpellIdx, &auraBasePoints);
                        if (roll_chance_i(chance))
                        {
                            switch (auraSpellInfo->Id)
                            {
                                case 99009:     // Item - Druid T12 Feral 4P Bonus
                                {
                                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                                        ((Player*)m_caster)->SendModifyCooldown(50334, -(*i)->GetModifier()->m_amount * IN_MILLISECONDS);
                                    break;
                                }
                                case 50040:     // Chillblains
                                case 50041:
                                case 81327:     // Brittle Bones
                                case 81328:
                                {
                                    // don't proc on self
                                    if (unit == caster)
                                        break;
                                    // no break
                                }
                                default:
                                {
                                    if (SpellEffectEntry const* spellEffect = (*i)->GetSpellEffect())
                                        m_caster->CastSpell(unit, spellEffect->EffectTriggerSpell, true, NULL, (*i));
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Heal caster for all health leech from all targets
    if (m_healthLeech)
    {
        // Divine Storm heal calculation
        if (m_spellInfo->Id == 53385)
        {
            SpellEffectIndex healEffIndex = EFFECT_INDEX_1;
            int32 healAmount = m_caster->CalculateSpellDamage(m_caster, m_spellInfo, healEffIndex, &m_currentBasePoints[healEffIndex]);
            healAmount = int32(m_healthLeech * healAmount / 100);
            m_caster->CastCustomSpell(m_caster, 54171, &healAmount, NULL, NULL, true);
        }
        else
        {
            uint32 absorb = 0;
            m_caster->CalculateHealAbsorb(uint32(m_healthLeech), &absorb);
            m_caster->DealHeal(m_caster, uint32(m_healthLeech) - absorb, m_spellInfo, false, absorb);
        }
    }

    if (IsMeleeAttackResetSpell())
    {
        bool found = false;
        Unit::AuraList const& vIgnoreReset = m_caster->GetAurasByType(SPELL_AURA_IGNORE_MELEE_RESET);
        for (Unit::AuraList::const_iterator i = vIgnoreReset.begin(); i != vIgnoreReset.end(); ++i)
        {
            if ((*i)->isAffectedOnSpell(m_spellInfo))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            m_caster->resetAttackTimer(BASE_ATTACK);
            if (m_caster->haveOffhandWeapon())
                m_caster->resetAttackTimer(OFF_ATTACK);

            if (IsRangedSpell())
                m_caster->resetAttackTimer(RANGED_ATTACK);
        }
    }

    // Clear combo at finish state
    if(NeedsComboPoints(m_spellInfo))
    {
        // Not drop combopoints if negative spell and if any miss on enemy exist
        bool needDrop = true;
        if (!IsPositiveSpell(m_spellInfo->Id))
        {
            for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
            {
                if (ihit->missCondition != SPELL_MISS_NONE && ihit->targetGUID != m_caster->GetObjectGuid())
                {
                    needDrop = false;
                    break;
                }
            }
        }

        if (needDrop)
        {
            // Expose Armor
            if (m_spellInfo->Id == 8647)
            {
                int32 cpAmount = 0;
                Unit::AuraList const& mDummyAuras = m_caster->GetAurasByType(SPELL_AURA_DUMMY);
                // Search Improved Expose Armor
                for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->SpellIconID == 563 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_ROGUE)
                    {
                        if (roll_chance_i((*itr)->GetSpellProto()->GetProcChance()))
                            cpAmount = m_caster->GetComboPoints();
                        break;
                    }
                }

                ObjectGuid comboTargetGuid = m_caster->GetComboTargetGuid();
                m_caster->ClearComboPoints();

                // Restore combo points
                if (cpAmount)
                    if (Unit* target = m_caster->GetMap()->GetUnit(comboTargetGuid))
                        m_caster->CastCustomSpell(target, 79128, &cpAmount, NULL, NULL, true);
            }
            else
                m_caster->ClearComboPoints();
        }
    }

    // potions disabled by client, send event "not in combat" if need
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
        ((Player*)m_caster)->UpdatePotionCooldown(this);

    // call triggered spell only at successful cast (after clear combo points -> for add some if need)
    if(!m_TriggerSpells.empty())
        CastTriggerSpells();

    // Stop Attack for some spells
    if (m_spellInfo->HasAttribute(SPELL_ATTR_STOP_ATTACK_TARGET))
        m_caster->AttackStop();

    // update encounter state if needed
    Map* map = m_caster->GetMap();
    if (map && map->IsDungeon())
    {
        if (DungeonPersistentState* state = ((DungeonMap*)map)->GetPersistanceState())
            state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, m_spellInfo->Id, NULL);
    }
}

void Spell::SendCastResult(SpellCastResult result)
{
    if(result == SPELL_CAST_OK)
        return;

    DEBUG_LOG("Spell Result: %i", result);

    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if(((Player*)m_caster)->GetSession()->PlayerLoading())  // don't send cast results at loading time
        return;

    SendCastResult((Player*)m_caster, m_spellInfo, m_cast_count, result);
}

void Spell::SendCastResult(Player* caster, SpellEntry const* spellInfo, uint8 cast_count, SpellCastResult result, bool isPetCastResult /*=false*/)
{
    if(result == SPELL_CAST_OK)
        return;

    WorldPacket data(isPetCastResult ? SMSG_PET_CAST_FAILED : SMSG_CAST_FAILED, (4 + 1 + 2));
    data << uint8(cast_count);                              // single cast or multi 2.3 (0/1)
    data << uint32(spellInfo->Id);
    data << uint8(!IsPassiveSpell(spellInfo) ? result : SPELL_FAILED_DONT_REPORT); // do not report failed passive spells
    switch (result)
    {
        case SPELL_FAILED_NOT_READY:
            data << uint32(0);                              // unknown, value 1 seen for 14177 (update cooldowns on client flag)
            break;
        case SPELL_FAILED_REQUIRES_SPELL_FOCUS:
            data << uint32(spellInfo->GetRequiresSpellFocus());
            break;
        case SPELL_FAILED_REQUIRES_AREA:                    // AreaTable.dbc id
            // hardcode areas limitation case
            switch(spellInfo->Id)
            {
                case 41617:                                 // Cenarion Mana Salve
                case 41619:                                 // Cenarion Healing Salve
                    data << uint32(3905);
                    break;
                case 41618:                                 // Bottled Nethergon Energy
                case 41620:                                 // Bottled Nethergon Vapor
                    data << uint32(3842);
                    break;
                case 45373:                                 // Bloodberry Elixir
                    data << uint32(4075);
                    break;
                default:                                    // default case (don't must be)
                    data << uint32(0);
                    break;
            }
            break;
        case SPELL_FAILED_TOTEMS:
            {
                SpellTotemsEntry const* totems = spellInfo->GetSpellTotems();
                for(int i = 0; i < MAX_SPELL_TOTEMS; ++i)
                    if(totems && totems->Totem[i])
                        data << uint32(totems->Totem[i]);
            }
            break;
        case SPELL_FAILED_TOTEM_CATEGORY:
            {
                SpellTotemsEntry const* totems = spellInfo->GetSpellTotems();
                for(int i = 0; i < MAX_SPELL_TOTEM_CATEGORIES; ++i)
                    if(totems && totems->TotemCategory[i])
                        data << uint32(totems->TotemCategory[i]);
            }
            break;
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS:
            {
                SpellEquippedItemsEntry const* eqItems = spellInfo->GetSpellEquippedItems();
                data << uint32(eqItems ? eqItems->EquippedItemClass : -1);
                data << uint32(eqItems ? eqItems->EquippedItemSubClassMask : 0);
                //data << uint32(eqItems ? eqItems->EquippedItemInventoryTypeMask : 0);
            }
            break;
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND:
        case SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND:
        {
            SpellEquippedItemsEntry const* eqItems = spellInfo->GetSpellEquippedItems();
            data << uint32(eqItems ? eqItems->EquippedItemClass : -1);
            data << uint32(eqItems ? eqItems->EquippedItemSubClassMask : 0);
            break;
        }
        case SPELL_FAILED_PREVENTED_BY_MECHANIC:
            data << uint32(0);                              // SpellMechanic.dbc id
            break;
        case SPELL_FAILED_CUSTOM_ERROR:
            data << uint32(0);                              // custom error id (see enum SpellCastResultCustom)
            break;
        case SPELL_FAILED_NEED_EXOTIC_AMMO:
        {
            SpellEquippedItemsEntry const* eqItems = spellInfo->GetSpellEquippedItems();
            data << uint32(eqItems ? eqItems->EquippedItemSubClassMask : 0);// seems correct...
            break;
        }
        case SPELL_FAILED_REAGENTS:
            // normally client checks reagents, just some script effects here
            data << uint32(0);                              // item id
            break;
        case SPELL_FAILED_NEED_MORE_ITEMS:
            data << uint32(0);                              // item id
            data << uint32(0);                              // item count?
            break;
        case SPELL_FAILED_MIN_SKILL:
            switch(spellInfo->Id)
            {
                case 54732:                                 // Gnomish Army Knife
                    data << uint32(SKILL_ENGINEERING);
                    data << uint32(350);
                    break;
                default:
                    data << uint32(0);                      // SkillLine.dbc id
                    data << uint32(0);                      // required skill value
                    break;
            }
            break;
        case SPELL_FAILED_TOO_MANY_OF_ITEM:
            data << uint32(0);                              // ItemLimitCategory.dbc id
            break;
        case SPELL_FAILED_FISHING_TOO_LOW:
            data << uint32(0);                              // required fishing skill
            break;
        default:
            break;
    }
    caster->GetSession()->SendPacket(&data);
}

void Spell::SendSpellStart()
{
    if (!IsNeedSendToClient())
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Sending SMSG_SPELL_START id=%u", m_spellInfo->Id);

    uint32 castFlags = CAST_FLAG_HAS_TRAJECTORY;
    if (m_spellInfo->runeCostID)
        castFlags |= CAST_FLAG_UNKNOWN19;

    Unit* caster = m_triggeredByAuraSpell && IsChanneledSpell(m_triggeredByAuraSpell) ? GetAffectiveCaster() : m_caster;
    if (!caster)
        caster = m_caster;

    if ((m_caster->GetTypeId() == TYPEID_PLAYER ||
        m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->IsPet()) &&
        m_spellInfo->powerType != POWER_HEALTH)
        castFlags |= CAST_FLAG_PREDICTED_POWER;

    if (m_casttime && (IsSpellHaveEffect(m_spellInfo, SPELL_EFFECT_HEAL) || IsSpellHaveEffect(m_spellInfo, SPELL_EFFECT_HEAL_PCT)) ||
        IsSpellHaveAura(m_spellInfo, SPELL_AURA_PERIODIC_HEAL))
    {
        castFlags |= CAST_FLAG_HEAL_PREDICTION;
    }

    WorldPacket data(SMSG_SPELL_START, (8 + 8 + 4 + 4 + 2));
    if (m_CastItem)
        data << m_CastItem->GetPackGUID();
    else
        data << caster->GetPackGUID();

    data << caster->GetPackGUID();
    data << uint8(m_cast_count);                            // pending spell cast
    data << uint32(m_spellInfo->Id);                        // spellId
    data << uint32(castFlags);                              // cast flags
    data << uint32(m_timer);                                // delay?
    data << uint32(m_casttime);                             // m_casttime

    data << m_targets;

    if (castFlags & CAST_FLAG_PREDICTED_POWER)              // predicted power
        data << uint32(m_caster->GetPower(Powers(m_spellInfo->powerType)));

    if (castFlags & CAST_FLAG_PREDICTED_RUNES)              // predicted runes
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            Player* caster = (Player*)m_caster;

            data << uint8(m_runesState);
            data << uint8(caster->GetRunesState());
            for (uint8 i = 0; i < MAX_RUNES; ++i)
                data << uint8(caster->GetRuneCooldownFraction(i));
        }
        else
        {
            data << uint8(0);
            data << uint8(0);
            for (uint8 i = 0; i < MAX_RUNES; ++i)
                data << uint8(0);
        }
    }

    if (castFlags & CAST_FLAG_AMMO)                         // projectile info
        WriteAmmoToPacket(&data);

    if (castFlags & CAST_FLAG_IMMUNITY)                     // cast immunity
    {
        data << uint32(0);                                  // used for SetCastSchoolImmunities
        data << uint32(0);                                  // used for SetCastImmunities
    }

    if (castFlags & CAST_FLAG_HEAL_PREDICTION)
    {
        uint8 type = DOT;
        int32 amt = 0;
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (m_spellInfo->GetSpellEffectIdByIndex(SpellEffectIndex(i)) == SPELL_EFFECT_HEAL || m_spellInfo->GetSpellEffectIdByIndex(SpellEffectIndex(i)) == SPELL_EFFECT_HEAL_PCT)
            {
                type = 0;
                Unit* target = m_targets.getUnitTarget() ? m_targets.getUnitTarget() : m_caster;
                amt = CalculateDamage(SpellEffectIndex(i), target);
                amt = m_caster->SpellHealingBonusDone(target, m_spellInfo, amt, HEAL);
                break;
            }
        }

        data << uint32(amt);
        data << uint8(type);
        if (type == DOT)
            data << m_caster->GetPackGUID();
    }

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendSpellGo()
{
    // not send invisible spell casting
    if (!IsNeedSendToClient())
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Sending SMSG_SPELL_GO id=%u", m_spellInfo->Id);

    uint32 castFlags = CAST_FLAG_UNKNOWN9;

    if ((m_caster->GetTypeId() == TYPEID_PLAYER ||
        m_caster->GetTypeId() == TYPEID_UNIT && ((Creature*)m_caster)->IsPet()) &&
        m_spellInfo->powerType != POWER_HEALTH)
        castFlags |= CAST_FLAG_PREDICTED_POWER;

    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->getClass() == CLASS_DEATH_KNIGHT && m_spellInfo->runeCostID)
    {
        castFlags |= CAST_FLAG_UNKNOWN19;                   // same as in SMSG_SPELL_START
        castFlags |= CAST_FLAG_PREDICTED_RUNES;             // rune cooldowns list
    }

    if (IsSpellHaveEffect(m_spellInfo, SPELL_EFFECT_ACTIVATE_RUNE))
    {
        castFlags |= CAST_FLAG_PREDICTED_RUNES;             // rune cooldowns list
        castFlags |= CAST_FLAG_UNKNOWN19;                   // same as in SMSG_SPELL_START
    }

    Unit* caster = m_triggeredByAuraSpell && IsChanneledSpell(m_triggeredByAuraSpell) ? GetAffectiveCaster() : m_caster;
    if (!caster)
        caster = m_caster;

    WorldPacket data(SMSG_SPELL_GO, 50);                    // guess size

    if (m_CastItem)
        data << m_CastItem->GetPackGUID();
    else
        data << caster->GetPackGUID();

    data << caster->GetPackGUID();
    data << uint8(m_cast_count);                            // pending spell cast?
    data << uint32(m_spellInfo->Id);                        // spellId
    data << uint32(castFlags);                              // cast flags
    data << uint32(m_timer);
    data << uint32(WorldTimer::getMSTime());                // timestamp

    WriteSpellGoTargets(&data);

    data << m_targets;

    if (castFlags & CAST_FLAG_PREDICTED_POWER)              // predicted power
        data << uint32(m_caster->GetPower(Powers(m_spellInfo->powerType)));

    if (castFlags & CAST_FLAG_PREDICTED_RUNES)              // predicted runes
    {
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
        {
            Player* caster = (Player*)m_caster;

            data << uint8(m_runesState);
            data << uint8(caster->GetRunesState());
            for (uint8 i = 0; i < MAX_RUNES; ++i)
                data << uint8(caster->GetRuneCooldownFraction(i));
        }
        else
        {
            data << uint8(0);
            data << uint8(0);
            for (uint8 i = 0; i < MAX_RUNES; ++i)
                data << uint8(0);
        }
    }

    if (castFlags & CAST_FLAG_ADJUST_MISSILE)               // adjust missile trajectory duration
    {
        data << float(m_targets.GetElevation());
        data << uint32(m_delayMoment);
    }

    if (castFlags & CAST_FLAG_AMMO)                         // projectile info
        WriteAmmoToPacket(&data);

    if (castFlags & CAST_FLAG_VISUAL_CHAIN)                 // spell visual chain effect
    {
        data << uint32(0);                                  // SpellVisual.dbc id?
        data << uint32(0);                                  // overrides previous field if > 0 and violencelevel client cvar < 2
    }

    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
    {
        data << uint8(0);                                   // The value increase for each time, can remind of a cast count for the spell
    }

    if (m_targets.m_targetMask & TARGET_FLAG_VISUAL_CHAIN)  // probably used (or can be used) with CAST_FLAG_VISUAL_CHAIN flag
    {
        data << uint32(0);                                  // count

        //for(int = 0; i < count; ++i)
        //{
        //    // position and guid?
        //    data << float(0) << float(0) << float(0) << uint64(0);
        //}
    }

    m_caster->SendMessageToSet(&data, true);
}

void Spell::WriteAmmoToPacket(WorldPacket* data)
{
    uint32 ammoInventoryType = 0;
    uint32 ammoDisplayID = 0;

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        Item *pItem = ((Player*)m_caster)->GetWeaponForAttack( RANGED_ATTACK );
        if(pItem)
        {
            ammoInventoryType = pItem->GetProto()->InventoryType;
            if( ammoInventoryType == INVTYPE_THROWN )
                ammoDisplayID = pItem->GetProto()->DisplayInfoID;
            else
            {
                if(m_caster->GetDummyAura(46699))      // Requires No Ammo
                {
                    ammoDisplayID = 5996;                   // normal arrow
                    ammoInventoryType = INVTYPE_AMMO;
                }
            }
        }
    }
    else
    {
        for (uint8 i = 0; i < MAX_VIRTUAL_ITEM_SLOT; ++i)
        {
            if(uint32 item_id = m_caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + i))
            {
                if(ItemPrototype const* itemEntry = sItemStorage.LookupEntry<ItemPrototype>(item_id))
                //if(ItemEntry const * itemEntry = sItemStore.LookupEntry(item_id))
                {
                    if(itemEntry->Class == ITEM_CLASS_WEAPON)
                    {
                        switch(itemEntry->SubClass)
                        {
                            case ITEM_SUBCLASS_WEAPON_THROWN:
                                ammoDisplayID = itemEntry->DisplayInfoID;
                                ammoInventoryType = itemEntry->InventoryType;
                                break;
                            case ITEM_SUBCLASS_WEAPON_BOW:
                            case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                                ammoDisplayID = 5996;       // is this need fixing?
                                ammoInventoryType = INVTYPE_AMMO;
                                break;
                            case ITEM_SUBCLASS_WEAPON_GUN:
                                ammoDisplayID = 5998;       // is this need fixing?
                                ammoInventoryType = INVTYPE_AMMO;
                                break;
                        }

                        if (ammoDisplayID)
                            break;
                    }
                }
            }
        }
    }

    *data << uint32(ammoDisplayID);
    *data << uint32(ammoInventoryType);
}

void Spell::WriteSpellGoTargets(WorldPacket* data)
{
    size_t count_pos = data->wpos();
    *data << uint8(0);                                      // placeholder

    // This function also fill data for channeled spells:
    // m_needAliveTargetMask req for stop channeling if one target die
    uint32 hit  = m_UniqueGOTargetInfo.size();              // Always hits on GO
    uint32 miss = 0;

    for (TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->effectMask == 0)                         // No effect apply - all immuned add state
        {
            // possibly SPELL_MISS_IMMUNE2 for this??
            ihit->missCondition = SPELL_MISS_IMMUNE2;
            ++miss;
        }
        else if (ihit->missCondition == SPELL_MISS_NONE)    // Add only hits
        {
            ++hit;
            *data << ihit->targetGUID;
            m_needAliveTargetMask |= ihit->effectMask;
        }
        else
            ++miss;
    }

    for(GOTargetList::const_iterator ighit = m_UniqueGOTargetInfo.begin(); ighit != m_UniqueGOTargetInfo.end(); ++ighit)
        *data << ighit->targetGUID;                         // Always hits

    data->put<uint8>(count_pos, hit);

    *data << (uint8)miss;
    for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition != SPELL_MISS_NONE)         // Add only miss
        {
            *data << ihit->targetGUID;
            *data << uint8(ihit->missCondition);
            if (ihit->missCondition == SPELL_MISS_REFLECT)
                *data << uint8(ihit->reflectResult);
        }
    }
    // Reset m_needAliveTargetMask for non channeled spell
    if(!IsChanneledSpell(m_spellInfo))
        m_needAliveTargetMask = 0;
}

void Spell::SendLogExecute()
{
    Unit *target = m_targets.getUnitTarget() ? m_targets.getUnitTarget() : m_caster;

    WorldPacket data(SMSG_SPELLLOGEXECUTE, (8+4+4+4+4+8));

    if(m_caster->GetTypeId() == TYPEID_PLAYER)
        data << m_caster->GetPackGUID();
    else
        data << target->GetPackGUID();

    data << uint32(m_spellInfo->Id);
    uint32 count1 = 1;
    data << uint32(count1);                                 // count1 (effect count?)
    for(uint32 i = 0; i < count1; ++i)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(EFFECT_INDEX_0);
        data << uint32(spellEffect ? spellEffect->Effect : 0);// spell effect
        uint32 count2 = 1;
        data << uint32(count2);                             // count2 (target count?)
        for(uint32 j = 0; j < count2; ++j)
        {
            if(!spellEffect)
                continue;

            switch(spellEffect->Effect)
            {
                case SPELL_EFFECT_POWER_DRAIN:
                case SPELL_EFFECT_POWER_BURN:
                    if (Unit* unit = m_targets.getUnitTarget())
                        data << unit->GetPackGUID();
                    else
                        data << uint8(0);
                    data << uint32(0);
                    data << uint32(0);
                    data << float(0);
                    break;
                case SPELL_EFFECT_ADD_EXTRA_ATTACKS:
                    if(Unit *unit = m_targets.getUnitTarget())
                        data << unit->GetPackGUID();
                    else
                        data << uint8(0);
                    data << uint32(0);                      // count?
                    break;
                case SPELL_EFFECT_INTERRUPT_CAST:
                    if(Unit *unit = m_targets.getUnitTarget())
                        data << unit->GetPackGUID();
                    else
                        data << uint8(0);
                    data << uint32(0);                      // spellid
                    break;
                case SPELL_EFFECT_DURABILITY_DAMAGE:
                    if (Unit *unit = m_targets.getUnitTarget())
                        data << unit->GetPackGUID();
                    else
                        data << uint8(0);
                    if (Item* item = m_targets.getItemTarget())
                    {
                        data << uint32(item->GetEntry());
                        data << uint32(item->GetSlot());
                    }
                    else
                    {
                        data << uint32(0);
                        data << uint32(0);
                    }
                    break;
                case SPELL_EFFECT_OPEN_LOCK:
                    if(Item *item = m_targets.getItemTarget())
                        data << item->GetPackGUID();
                    else
                        data << uint8(0);
                    break;
                case SPELL_EFFECT_CREATE_ITEM:
                case SPELL_EFFECT_CREATE_RANDOM_ITEM:
                case SPELL_EFFECT_CREATE_ITEM_2:
                    data << uint32(spellEffect->EffectItemType);
                    break;
                case SPELL_EFFECT_SUMMON:
                case SPELL_EFFECT_TRANS_DOOR:
                case SPELL_EFFECT_SUMMON_PET:
                case SPELL_EFFECT_SUMMON_OBJECT_WILD:
                case SPELL_EFFECT_CREATE_HOUSE:
                case SPELL_EFFECT_DUEL:
                case SPELL_EFFECT_SUMMON_OBJECT_SLOT:
                case SPELL_EFFECT_171:
                    if (Unit* unit = m_targets.getUnitTarget())
                        data << unit->GetPackGUID();
                    else if (m_targets.getItemTargetGuid())
                        data << m_targets.getItemTargetGuid().WriteAsPacked();
                    else if (GameObject *go = m_targets.getGOTarget())
                        data << go->GetPackGUID();
                    else
                        data << uint8(0);                   // guid
                    break;
                case SPELL_EFFECT_FEED_PET:
                    data << uint32(m_targets.getItemTargetEntry());
                    break;
                case SPELL_EFFECT_DISMISS_PET:
                    if(Unit *unit = m_targets.getUnitTarget())
                        data << unit->GetPackGUID();
                    else
                        data << uint8(0);
                    break;
                case SPELL_EFFECT_RESURRECT:
                case SPELL_EFFECT_RESURRECT_NEW:
                case SPELL_EFFECT_RESURRECT_WITH_AURA:
                    if (Unit* unit = m_targets.getUnitTarget())
                        data << unit->GetPackGUID();
                    else
                        data << uint8(0);
                    break;
                default:
                    return;
            }
        }
    }

    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendInterrupted(uint8 result)
{
    WorldPacket data(SMSG_SPELL_FAILURE, (8+4+1));
    data << m_caster->GetPackGUID();
    data << uint8(m_cast_count);
    data << uint32(m_spellInfo->Id);
    data << uint8(result);
    m_caster->SendMessageToSet(&data, true);

    data.Initialize(SMSG_SPELL_FAILED_OTHER, (8+4));
    data << m_caster->GetPackGUID();
    data << uint8(m_cast_count);
    data << uint32(m_spellInfo->Id);
    data << uint8(result);
    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendChannelUpdate(uint32 time)
{
    if (time == 0)
    {
        // Reset farsight for some possessing auras of possessed summoned (as they might work with different aura types)
        if (m_spellInfo->HasAttribute(SPELL_ATTR_EX_FARSIGHT) && m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->GetCharmGuid()
           && !IsSpellHaveAura(m_spellInfo, SPELL_AURA_MOD_POSSESS) && !IsSpellHaveAura(m_spellInfo, SPELL_AURA_MOD_POSSESS_PET))
        {
            Player* player = (Player*)m_caster;
            // These Auras are applied to self, so get the possessed first
            Unit* possessed = player->GetCharm();

            player->SetCharm(NULL);
            if (possessed)
                player->SetClientControl(possessed, 0);
            player->SetMover(NULL);
            player->GetCamera().ResetView();
            player->RemovePetActionBar();

            if (possessed)
            {
                possessed->clearUnitState(UNIT_STAT_CONTROLLED);
                possessed->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
                possessed->SetCharmerGuid(ObjectGuid());
                // TODO - Requires more specials for target?

                // Some possessed might want to despawn?
                if (possessed->GetUInt32Value(UNIT_CREATED_BY_SPELL) == m_spellInfo->Id && possessed->GetTypeId() == TYPEID_UNIT)
                    ((Creature*)possessed)->ForcedDespawn();
            }
        }

        m_caster->RemoveAurasByCasterSpell(m_spellInfo->Id, m_caster->GetObjectGuid());

        ObjectGuid target_guid = m_caster->GetChannelObjectGuid();
        if (target_guid != m_caster->GetObjectGuid() && target_guid.IsUnit())
            if (Unit* target = ObjectAccessor::GetUnit(*m_caster, target_guid))
                target->RemoveAurasByCasterSpell(m_spellInfo->Id, m_caster->GetObjectGuid());

        // Only finish channeling when latest channeled spell finishes
        if (m_caster->GetUInt32Value(UNIT_CHANNEL_SPELL) != m_spellInfo->Id)
            return;

        m_caster->SetChannelObjectGuid(ObjectGuid());
        m_caster->SetUInt32Value(UNIT_CHANNEL_SPELL, 0);
    }

    WorldPacket data(SMSG_CHANNEL_UPDATE, 8 + 4);
    data << m_caster->GetPackGUID();
    data << uint32(time);
    m_caster->SendMessageToSet(&data, true);
}

void Spell::SendChannelStart(uint32 duration)
{
    WorldObject* target = NULL;

    // select dynobject created by first effect if any
    if (m_spellInfo->GetSpellEffectIdByIndex(EFFECT_INDEX_0) == SPELL_EFFECT_PERSISTENT_AREA_AURA)
        target = m_caster->GetDynObject(m_spellInfo->Id, EFFECT_INDEX_0);
    // select first not resisted target from target list for _0_ effect
    else if (!m_UniqueTargetInfo.empty())
    {
        for(TargetList::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
        {
            if ((itr->effectMask & (1 << EFFECT_INDEX_0)) && itr->reflectResult == SPELL_MISS_NONE &&
                itr->targetGUID != m_caster->GetObjectGuid())
            {
                target = ObjectAccessor::GetUnit(*m_caster, itr->targetGUID);
                break;
            }
        }
    }
    else if(!m_UniqueGOTargetInfo.empty())
    {
        for(GOTargetList::const_iterator itr = m_UniqueGOTargetInfo.begin(); itr != m_UniqueGOTargetInfo.end(); ++itr)
        {
            if (itr->effectMask & (1 << EFFECT_INDEX_0))
            {
                target = m_caster->GetMap()->GetGameObject(itr->targetGUID);
                break;
            }
        }
    }

    WorldPacket data(SMSG_CHANNEL_START, (8 + 4 + 4));
    data << m_caster->GetPackGUID();
    data << uint32(m_spellInfo->Id);
    data << uint32(duration);
    data << uint8(0);       // unk1
    //if (unk1)
    //{
    //    data << uint32(0);
    //    data << uint32(0);
    //}
    data << uint8(0);       // unk2
    //if (unk2)
    //{
    //    data << ObjectGuid().WriteAsPacked();
    //    data << uint32(0);
    //    data << uint8(0);   // unk3
    //    if (unk3 == 2)
    //        data << ObjectGuid().WriteAsPacked();
    //}

    m_caster->SendMessageToSet(&data, true);

    m_timer = duration;

    if (target)
        m_caster->SetChannelObjectGuid(target->GetObjectGuid());

    m_caster->SetUInt32Value(UNIT_CHANNEL_SPELL, m_spellInfo->Id);
}

void Spell::SendResurrectRequest(Player* target)
{
    // get ressurector name for creature resurrections, otherwise packet will be not accepted
    // for player resurrections the name is looked up by guid
    char const* resurrectorName = m_caster->GetTypeId() == TYPEID_PLAYER ? "" : m_caster->GetNameForLocaleIdx(target->GetSession()->GetSessionDbLocaleIndex());

    WorldPacket data(SMSG_RESURRECT_REQUEST, (8+4+strlen(resurrectorName)+1+1+1+4));

    // resurrector guid
    data << m_caster->GetObjectGuid();
    data << uint32(strlen(resurrectorName) + 1);
    data << resurrectorName;
    // null terminator
    data << uint8(0);

    data << uint8(m_caster->GetTypeId() == TYPEID_PLAYER ? 0 : 1);
    data << uint32(m_spellInfo->Id);

    target->GetSession()->SendPacket(&data);
}

void Spell::SendPlaySpellVisual(uint32 SpellID)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    WorldPacket data;
    m_caster->BuildSendPlayVisualPacket(&data, SpellID, false);

    ((Player*)m_caster)->GetSession()->SendPacket(&data);
}

void Spell::TakeCastItem()
{
    if(!m_CastItem || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    // not remove cast item at triggered spell (equipping, weapon damage, etc)
    if(m_IsTriggeredSpell && !(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM))
        return;

    ItemPrototype const *proto = m_CastItem->GetProto();

    if(!proto)
    {
        // This code is to avoid a crash
        // I'm not sure, if this is really an error, but I guess every item needs a prototype
        ERROR_LOG("Cast item (%s) has no item prototype", m_CastItem->GetGuidStr().c_str());
        return;
    }

    bool expendable = false;
    bool withoutCharges = false;

    for (int i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
    {
        if (proto->Spells[i].SpellId)
        {
            // item has limited charges
            if (proto->Spells[i].SpellCharges)
            {
                if (proto->Spells[i].SpellCharges < 0 && !(proto->ExtraFlags & ITEM_EXTRA_NON_CONSUMABLE))
                    expendable = true;

                int32 charges = m_CastItem->GetSpellCharges(i);

                // item has charges left
                if (charges)
                {
                    (charges > 0) ? --charges : ++charges;  // abs(charges) less at 1 after use
                    if (proto->Stackable == 1)
                        m_CastItem->SetSpellCharges(i, charges);
                    m_CastItem->SetState(ITEM_CHANGED, (Player*)m_caster);
                }

                // all charges used
                withoutCharges = (charges == 0);
            }
        }
    }

    if (expendable && withoutCharges)
    {
        uint32 count = 1;
        ((Player*)m_caster)->DestroyItemCount(m_CastItem, count, true);

        // prevent crash at access to deleted m_targets.getItemTarget
        ClearCastItem();
    }
}

void Spell::TakePower()
{
    if (m_CastItem || m_triggeredByAuraSpell)
        return;

    bool hit = true;
    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_spellInfo->powerType == POWER_RAGE || m_spellInfo->powerType == POWER_ENERGY || m_spellInfo->powerType == POWER_HOLY_POWER)
        {
            ObjectGuid targetGuid = m_targets.getUnitTargetGuid();
            if (!targetGuid.IsEmpty())
                for (TargetList::iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
                    if (ihit->targetGUID == targetGuid)
                    {
                        if (ihit->missCondition != SPELL_MISS_NONE && ihit->missCondition != SPELL_MISS_MISS/* && ihit->targetGUID != m_caster->GetGUID()*/)
                            hit = false;
                        if (ihit->missCondition != SPELL_MISS_NONE)
                        {
                            //lower spell cost on fail (by talent aura)
                            if (Player *modOwner = ((Player*)m_caster)->GetSpellModOwner())
                                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_SPELL_COST_REFUND_ON_FAIL, m_powerCost);
                        }
                        break;
                    }
        }
    }

    // health as power used
    if (m_spellInfo->powerType == POWER_HEALTH)
    {
        m_caster->ModifyHealth(-(int32)m_powerCost);
        return;
    }

    if (m_spellInfo->powerType >= MAX_POWERS)
    {
        ERROR_LOG("Spell::TakePower: Unknown power type '%d'", m_spellInfo->powerType);
        return;
    }

    Powers powerType = Powers(m_spellInfo->powerType);

    if (powerType == POWER_HOLY_POWER)
    {
        m_usedHolyPower = m_powerCost;

        // spells consume all holy power when successfully hit
        if (hit)
        {
            // Divine Purpose
            if (m_caster->HasAura(90174))
            {
                m_usedHolyPower = uint32(m_caster->GetMaxPower(POWER_HOLY_POWER));
                return;
            }
            else
                m_usedHolyPower = uint32(m_caster->GetPower(POWER_HOLY_POWER));
        }

        // Zealotry - does not take power
        if (m_spellInfo->Id == 85696)
            return;

        m_caster->ModifyPower(powerType, -(int32)m_usedHolyPower);
        return;
    }

    if (powerType == POWER_RUNE)
    {
        TakeRunePower(hit);
        return;
    }

    m_caster->ModifyPower(powerType, -(int32)m_powerCost);
}

SpellCastResult Spell::CheckRunePower()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return SPELL_CAST_OK;

    Player* plr = (Player*)m_caster;

    if (plr->getClass() != CLASS_DEATH_KNIGHT)
        return SPELL_CAST_OK;

    SpellRuneCostEntry const* src = sSpellRuneCostStore.LookupEntry(m_spellInfo->runeCostID);

    if (!src)
        return SPELL_CAST_OK;

    if (src->NoRuneCost())
        return SPELL_CAST_OK;

    // at this moment for rune cost exist only no cost mods, and no percent mods
    int32 runeCost[NUM_RUNE_TYPES];                         // blood, frost, unholy, death
    for (uint32 i = 0; i < RUNE_DEATH; ++i)
    {
        runeCost[i] = src->RuneCost[i];
        if (Player* modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, runeCost[i], this);
    }

    runeCost[RUNE_DEATH] = MAX_RUNES;                       // calculated later

    for (uint32 i = 0; i < MAX_RUNES; ++i)
    {
        RuneType rune = plr->GetCurrentRune(i);
        if (!plr->GetRuneCooldown(i) && runeCost[rune] > 0)
            --runeCost[rune];
    }

    for (uint32 i = 0; i < RUNE_DEATH; ++i)
        if (runeCost[i] > 0)
            runeCost[RUNE_DEATH] += runeCost[i];

    if (runeCost[RUNE_DEATH] > MAX_RUNES)
        return SPELL_FAILED_NO_POWER;                       // not sure if result code is correct

    return SPELL_CAST_OK;
}

void Spell::TakeRunePower(bool hit)
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* plr = (Player*)m_caster;

    if (plr->getClass() != CLASS_DEATH_KNIGHT)
        return;

    SpellRuneCostEntry const* src = sSpellRuneCostStore.LookupEntry(m_spellInfo->runeCostID);

    if (!src)
        return;

    if (src->NoRuneCost() && src->NoRunicPowerGain())
        return;

    m_runesState = plr->GetRunesState();                    // store previous state

    // at this moment for rune cost exist only no cost mods, and no percent mods
    int32 runeCost[NUM_RUNE_TYPES];                         // blood, frost, unholy, death
    for (uint32 i = 0; i < RUNE_DEATH; ++i)
    {
        runeCost[i] = src->RuneCost[i];
        if (Player* modOwner = m_caster->GetSpellModOwner())
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, runeCost[i], this);
    }

    runeCost[RUNE_DEATH] = 0;                               // calculated later

    plr->ClearLastUsedRuneMask();

    for (uint32 i = 0; i < MAX_RUNES; ++i)
    {
        RuneType rune = plr->GetCurrentRune(i);
        if (!plr->GetRuneCooldown(i) && runeCost[rune] > 0)
        {
            uint16 baseCd = hit ? uint16(RUNE_BASE_COOLDOWN) : uint16(RUNE_MISS_COOLDOWN);
            plr->SetBaseRuneCooldown(i, baseCd);
            plr->SetRuneCooldown(i, baseCd);
            plr->SetLastUsedRune(rune);
            --runeCost[rune];
        }
    }

    runeCost[RUNE_DEATH] = runeCost[RUNE_BLOOD] + runeCost[RUNE_UNHOLY] + runeCost[RUNE_FROST];

    if (runeCost[RUNE_DEATH] > 0)
    {
        for (uint32 i = 0; i < MAX_RUNES; ++i)
        {
            RuneType rune = plr->GetCurrentRune(i);
            if (!plr->GetRuneCooldown(i) && rune == RUNE_DEATH)
            {
                uint16 baseCd = hit ? uint16(RUNE_BASE_COOLDOWN) : uint16(RUNE_MISS_COOLDOWN);
                plr->SetBaseRuneCooldown(i, baseCd);
                plr->SetRuneCooldown(i, baseCd);
                plr->SetLastUsedRune(rune);
                --runeCost[rune];

                // keep Death Rune type if missed
                if (hit)
                    plr->RestoreBaseRune(i);

                if (runeCost[RUNE_DEATH] == 0)
                    break;
            }
        }
    }

    if (hit)
    {
        // you can gain some runic power when use runes
        int32 rp = int32(src->runePowerGain);
        if (rp)
        {
            if (Player* modOwner = m_caster->GetSpellModOwner())
                modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_COST, rp, this);

            rp = int32(sWorld.getConfig(CONFIG_FLOAT_RATE_POWER_RUNICPOWER_INCOME) * rp);
            rp += m_caster->GetTotalAuraModifier(SPELL_AURA_MOD_RUNIC_POWER_GAIN) * rp / 100;
            if (rp > 0)
                plr->ModifyPower(POWER_RUNIC_POWER, (int32)rp);
        }
    }

    return;
}

void Spell::TakeReagents()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (IgnoreItemRequirements())                           // reagents used in triggered spell removed by original spell or don't must be removed.
        return;

    Player* p_caster = (Player*)m_caster;
    if (p_caster->CanNoReagentCast(m_spellInfo))
        return;

    SpellReagentsEntry const* spellReagents = m_spellInfo->GetSpellReagents();

    for(uint32 x = 0; x < MAX_SPELL_REAGENTS; ++x)
    {
        if(!spellReagents)
            continue;
        if(spellReagents->Reagent[x] <= 0)
            continue;

        uint32 itemid = spellReagents->Reagent[x];
        uint32 itemcount = spellReagents->ReagentCount[x];

        // if CastItem is also spell reagent
        if (m_CastItem)
        {
            ItemPrototype const *proto = m_CastItem->GetProto();
            if( proto && proto->ItemId == itemid )
            {
                for(int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
                {
                    // CastItem will be used up and does not count as reagent
                    int32 charges = m_CastItem->GetSpellCharges(s);
                    if (proto->Spells[s].SpellCharges < 0 && abs(charges) < 2)
                    {
                        ++itemcount;
                        break;
                    }
                }

                m_CastItem = NULL;
            }
        }

        // if getItemTarget is also spell reagent
        if (m_targets.getItemTargetEntry() == itemid)
            m_targets.setItemTarget(NULL);

        p_caster->DestroyItemCount(itemid, itemcount, true);
    }
}

void Spell::HandleThreatSpells()
{
    if (m_UniqueTargetInfo.empty())
        return;

    SpellThreatEntry const* threatEntry = sSpellMgr.GetSpellThreatEntry(m_spellInfo->Id);

    if (!threatEntry || (!threatEntry->threat && threatEntry->ap_bonus == 0.0f))
        return;

    float threat = threatEntry->threat;
    if (threatEntry->ap_bonus != 0.0f)
        threat += threatEntry->ap_bonus * m_caster->GetTotalAttackPowerValue(GetWeaponAttackType(m_spellInfo));

    bool positive = true;
    uint8 effectMask = 0;
    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i)))
            if (spellEffect->Effect)
                effectMask |= (1<<i);

    if (m_negativeEffectMask & effectMask)
    {
        // can only handle spells with clearly defined positive/negative effect, check at spell_threat loading probably not perfect
        // so abort when only some effects are negative.
        if ((m_negativeEffectMask & effectMask) != effectMask)
        {
            DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell %u, rank %u, is not clearly positive or negative, ignoring bonus threat", m_spellInfo->Id, sSpellMgr.GetSpellRank(m_spellInfo->Id));
            return;
        }
        positive = false;
    }

    // since 2.0.1 threat from positive effects also is distributed among all targets, so the overall caused threat is at most the defined bonus
    threat /= m_UniqueTargetInfo.size();

    for (TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if (ihit->missCondition != SPELL_MISS_NONE)
            continue;

        Unit* target = m_caster->GetObjectGuid() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID);
        if (!target)
            continue;

        // positive spells distribute threat among all units that are in combat with target, like healing
        if (positive)
        {
            target->getHostileRefManager().threatAssist(m_caster /*real_caster ??*/, threat, m_spellInfo);
        }
        // for negative spells threat gets distributed among affected targets
        else
        {
            if (!target->CanHaveThreatList())
                continue;

            target->AddThreat(m_caster, threat, false, GetSpellSchoolMask(m_spellInfo), m_spellInfo);
        }
    }

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell %u added an additional %f threat for %s " SIZEFMTD " target(s)", m_spellInfo->Id, threat, positive ? "assisting" : "harming", m_UniqueTargetInfo.size());
}

void Spell::HandleEffects(Unit *pUnitTarget,Item *pItemTarget,GameObject *pGOTarget,SpellEffectIndex i, float DamageMultiplier)
{
    unitTarget = pUnitTarget;
    itemTarget = pItemTarget;
    gameObjTarget = pGOTarget;

    SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));

    damage = int32(CalculateDamage(i, unitTarget) * DamageMultiplier);

    if(spellEffect)
    {
        if(spellEffect->Effect < TOTAL_SPELL_EFFECTS)
        {
            DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell %u Effect%d : %u Targets: %s, %s, %s",
                m_spellInfo->Id, i, spellEffect->Effect,
                unitTarget ? unitTarget->GetGuidStr().c_str() : "-",
                itemTarget ? itemTarget->GetGuidStr().c_str() : "-",
                gameObjTarget ? gameObjTarget->GetGuidStr().c_str() : "-");

            (*this.*SpellEffects[spellEffect->Effect])(spellEffect);
        }
        else
        {
            sLog.outError("WORLD: Spell %u Effect%d : %u > TOTAL_SPELL_EFFECTS", m_spellInfo->Id, i, spellEffect->Effect);
        }
    }
    else
    {
        sLog.outError("WORLD: Spell %u has no effect at index %u", m_spellInfo->Id, i);
    }
}

void Spell::AddTriggeredSpell( uint32 spellId )
{
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellId );

    if(!spellInfo)
    {
        ERROR_LOG("Spell::AddTriggeredSpell: unknown spell id %u used as triggred spell for spell %u)", spellId, m_spellInfo->Id);
        return;
    }

    m_TriggerSpells.push_back(spellInfo);
}

void Spell::AddPrecastSpell( uint32 spellId )
{
    SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellId );

    if(!spellInfo)
    {
        ERROR_LOG("Spell::AddPrecastSpell: unknown spell id %u used as pre-cast spell for spell %u)", spellId, m_spellInfo->Id);
        return;
    }

    m_preCastSpells.push_back(spellInfo);
}

void Spell::CastTriggerSpells()
{
    for(SpellInfoList::const_iterator si = m_TriggerSpells.begin(); si != m_TriggerSpells.end(); ++si)
    {
        Spell* spell = new Spell(m_caster, (*si), true, m_originalCasterGUID);
        spell->prepare(&m_targets);                         // use original spell original targets
    }
}

void Spell::CastPreCastSpells(Unit* target)
{
    for(SpellInfoList::const_iterator si = m_preCastSpells.begin(); si != m_preCastSpells.end(); ++si)
        m_caster->CastSpell(target, (*si), true, m_CastItem);
}

SpellCastResult Spell::CheckCast(bool strict)
{
    if (sObjectMgr.IsSpellDisabled(m_spellInfo->Id, DISABLE_SPELL_TYPE_CAST))
        return m_IsTriggeredSpell ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

    // check cooldowns to prevent cheating (ignore passive spells, that client side visual only)
    if (m_caster->GetTypeId()==TYPEID_PLAYER && !m_spellInfo->HasAttribute(SPELL_ATTR_PASSIVE))
    {
        if (!m_IsTriggeredSpell && ((Player*)m_caster)->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_ALLOW_ONLY_ABILITY))
            return SPELL_FAILED_SPELL_IN_PROGRESS;

        if (((Player*)m_caster)->HasSpellCooldown(m_spellInfo->Id))
        {
            if(m_triggeredByAuraSpell)
                return SPELL_FAILED_DONT_REPORT;
            else if (!m_IsTriggeredSpell || m_CastItem)
                return SPELL_FAILED_NOT_READY;
        }
    }

    // check global cooldown
    if (strict && !m_IsTriggeredSpell && HasGlobalCooldown())
        return SPELL_FAILED_NOT_READY;

    // check unlimited channeled spells trying to overwrite each other
    if (m_IsTriggeredSpell)
    {
        if (uint32 channeledSpell = m_caster->GetUInt32Value(UNIT_CHANNEL_SPELL))
            if (channeledSpell == m_spellInfo->Id && GetSpellDuration(m_spellInfo) == -1)
                return SPELL_FAILED_DONT_REPORT;
    }

    // only allow triggered spells if at an ended battleground
    if (!m_IsTriggeredSpell && m_caster->GetTypeId() == TYPEID_PLAYER)
        if(BattleGround * bg = ((Player*)m_caster)->GetBattleGround())
            if(bg->GetStatus() == STATUS_WAIT_LEAVE)
                return SPELL_FAILED_DONT_REPORT;

    if (m_caster->isInCombat() && IsNonCombatSpell(m_spellInfo) && !m_caster->IsIgnoreUnitState(m_spellInfo, IGNORE_UNIT_COMBAT_STATE) && !m_IsTriggeredSpell
        && !m_spellInfo->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x0000000000400800))) // Vanish Hack
        return SPELL_FAILED_AFFECTING_COMBAT;

    if (m_caster->GetTypeId() == TYPEID_PLAYER && !((Player*)m_caster)->isGameMaster() &&
        sWorld.getConfig(CONFIG_BOOL_VMAP_INDOOR_CHECK) &&
        VMAP::VMapFactory::createOrGetVMapManager()->isLineOfSightCalcEnabled())
    {
        if (m_spellInfo->HasAttribute(SPELL_ATTR_OUTDOORS_ONLY) &&
                !m_caster->GetTerrain()->IsOutdoors(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ()))
            return SPELL_FAILED_ONLY_OUTDOORS;

        if (m_spellInfo->HasAttribute(SPELL_ATTR_INDOORS_ONLY) &&
                m_caster->GetTerrain()->IsOutdoors(m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ()))
            return SPELL_FAILED_ONLY_INDOORS;
    }
    // only check at first call, Stealth auras are already removed at second call
    // for now, ignore triggered spells
    if (strict && !m_IsTriggeredSpell)
    {
        // Ignore form req aura
        if (!m_caster->HasAffectedAura(SPELL_AURA_MOD_IGNORE_SHAPESHIFT, m_spellInfo))
        {
            // Cannot be used in this stance/form
            SpellCastResult shapeError = GetErrorAtShapeshiftedCast(m_spellInfo, m_caster->GetShapeshiftForm());
            if(shapeError != SPELL_CAST_OK)
                return shapeError;

            if (m_spellInfo->HasAttribute(SPELL_ATTR_ONLY_STEALTHED) && !m_caster->HasStealthAura())
                return SPELL_FAILED_ONLY_STEALTHED;
        }
    }

    SpellAuraRestrictionsEntry const* auraRestrictions = m_spellInfo->GetSpellAuraRestrictions();

    //if (auraRestrictions && auraRestrictions->excludeCasterAuraSpell)
    //{
    //    if (auraRestrictions->excludeCasterAuraSpell == 79636 ||    // No Feather Fall
    //        auraRestrictions->excludeCasterAuraSpell == 96223)      // Run Speed Marker
    //        AddTriggeredSpell(auraRestrictions->excludeCasterAuraSpell);
    //}

    // caster state requirements
    if (auraRestrictions && auraRestrictions->CasterAuraState && !m_caster->HasAuraState(AuraState(auraRestrictions->CasterAuraState)))
        return SPELL_FAILED_CASTER_AURASTATE;
    if (auraRestrictions && auraRestrictions->CasterAuraStateNot && m_caster->HasAuraState(AuraState(auraRestrictions->CasterAuraStateNot)))
        return SPELL_FAILED_CASTER_AURASTATE;

    // Caster aura req check if need
    if (auraRestrictions && auraRestrictions->casterAuraSpell && !m_caster->HasAura(auraRestrictions->casterAuraSpell) &&
        sSpellStore.LookupEntry(auraRestrictions->casterAuraSpell))
        return SPELL_FAILED_CASTER_AURASTATE;

    if (auraRestrictions && auraRestrictions->excludeCasterAuraSpell)
    {
        // Special cases of non existing auras handling
        if (auraRestrictions->excludeCasterAuraSpell == 61988)
        {
            // Forbearance
            if (m_caster->HasAura(25771))
                return SPELL_FAILED_CASTER_AURASTATE;
        }
        else if (m_caster->HasAura(auraRestrictions->excludeCasterAuraSpell))
            return SPELL_FAILED_CASTER_AURASTATE;
    }

    if (m_caster->GetTypeId() == TYPEID_PLAYER)
    {
        // cancel autorepeat spells if cast start when moving
        // (not wand currently autorepeat cast delayed to moving stop anyway in spell update code)
        if (((Player*)m_caster)->isMoving() && !m_caster->HasAffectedAura(SPELL_AURA_ALLOW_CAST_WHILE_MOVING, m_spellInfo))
        {
            // skip stuck spell to allow use it in falling case and apply spell limitations at movement
            if ((!((Player*)m_caster)->m_movementInfo.HasMovementFlag(MOVEFLAG_FALLINGFAR) || m_spellInfo->GetSpellEffectIdByIndex(EFFECT_INDEX_0) != SPELL_EFFECT_STUCK) &&
                (IsAutoRepeat() || (m_spellInfo->GetAuraInterruptFlags() & AURA_INTERRUPT_FLAG_NOT_SEATED) != 0))
                return SPELL_FAILED_MOVING;
        }

        // Amaru: remove mangos native check
        /*if (!m_IsTriggeredSpell && NeedsComboPoints(m_spellInfo) && !m_caster->IsIgnoreUnitState(m_spellInfo, IGNORE_UNIT_TARGET_STATE) &&
                (!m_targets.getUnitTarget() || m_targets.getUnitTarget()->GetObjectGuid() != ((Player*)m_caster)->GetComboTargetGuid()) &&
                !m_spellInfo->HasAttribute(SPELL_ATTR_EX8_IGNORE_TARGET_FOR_COMBO_POINTS))
            // warrior not have real combo-points at client side but use this way for mark allow Overpower use
            return m_caster->getClass() == CLASS_WARRIOR ? SPELL_FAILED_CASTER_AURASTATE : SPELL_FAILED_NO_COMBO_POINTS;*/
    }

    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->IsInWorld())
    {
        // Additional check for some spells
        // If 0 spell effect empty - client not send target data (need use selection)
        // TODO: check it on next client version
        if (m_targets.m_targetMask == TARGET_FLAG_NONE &&
            m_spellInfo->GetEffectImplicitTargetAByIndex(EFFECT_INDEX_1) == TARGET_CHAIN_DAMAGE
            || m_spellInfo->Id == 56190 || m_spellInfo->Id == 56191 || m_spellInfo->Id == 55346) // Focusing Lenses
        {
            if (Unit* target = m_caster->GetMap()->GetUnit(((Player *)m_caster)->GetSelectionGuid()))
                m_targets.setUnitTarget(target);
            else
                return SPELL_FAILED_BAD_TARGETS;
        }
    }

    SpellClassOptionsEntry const* classOptions = m_spellInfo->GetSpellClassOptions();

    if (Unit *target = m_targets.getUnitTarget())
    {
        // target state requirements (not allowed state), apply to self also
        if(auraRestrictions && auraRestrictions->TargetAuraStateNot && target->HasAuraState(AuraState(auraRestrictions->TargetAuraStateNot)))
            return SPELL_FAILED_TARGET_AURASTATE;

        if (!m_IsTriggeredSpell && IsDeathOnlySpell(m_spellInfo) && target->isAlive())
            return SPELL_FAILED_TARGET_NOT_DEAD;

        // Target aura req check if need
        if (auraRestrictions && auraRestrictions->targetAuraSpell && !target->HasAura(auraRestrictions->targetAuraSpell) &&
            sSpellStore.LookupEntry(auraRestrictions->targetAuraSpell))
            return SPELL_FAILED_CASTER_AURASTATE;

        if(auraRestrictions && auraRestrictions->excludeTargetAuraSpell)
        {
            // Special cases of non existing auras handling
            if (auraRestrictions->excludeTargetAuraSpell == 61988)
            {
                // Forbearance
                if (target->HasAura(25771))
                    return SPELL_FAILED_CASTER_AURASTATE;
            }
            else if (target->HasAura(auraRestrictions->excludeTargetAuraSpell))
                return SPELL_FAILED_CASTER_AURASTATE;
        }

        if (!m_IsTriggeredSpell && target == m_caster && m_spellInfo->AttributesEx & SPELL_ATTR_EX_CANT_TARGET_SELF)
            return SPELL_FAILED_BAD_TARGETS;

        // totem immunity for channeled spells(needs to be before spell cast)
        // spell attribs for player channeled spells
        if (m_spellInfo->HasAttribute(SPELL_ATTR_EX_CHANNEL_TRACKING_TARGET)
            && m_spellInfo->HasAttribute(SPELL_ATTR_EX5_AFFECTED_BY_HASTE)
            && target->GetTypeId() == TYPEID_UNIT
            && ((Creature*)target)->IsTotem())
            return SPELL_FAILED_IMMUNE;

        bool non_caster_target = target != m_caster && !IsSpellWithCasterSourceTargetsOnly(m_spellInfo);

        if (non_caster_target)
        {
            // target state requirements (apply to non-self only), to allow cast affects to self like Dirty Deeds
            if (auraRestrictions && auraRestrictions->TargetAuraState)
            {
                if (!target->HasAuraStateForCaster(AuraState(auraRestrictions->TargetAuraState), m_caster->GetObjectGuid()))
                {
                    if (auraRestrictions->TargetAuraState == AURA_STATE_FROZEN)
                    {
                        if (!m_caster->IsIgnoreUnitState(m_spellInfo, IGNORE_UNIT_TARGET_NON_FROZEN))
                            return SPELL_FAILED_TARGET_AURASTATE;
                    }
                    else if (auraRestrictions->TargetAuraState == AURA_STATE_HEALTHLESS_20_PERCENT)
                    {
                        if (!m_caster->IsIgnoreUnitState(m_spellInfo, IGNORE_UNIT_HEALTH_STATE))
                            return SPELL_FAILED_TARGET_AURASTATE;
                    }
                    else if (!m_caster->IsIgnoreUnitState(m_spellInfo, IGNORE_UNIT_TARGET_STATE))
                        return SPELL_FAILED_TARGET_AURASTATE;
                }
            }

            // Not allow casting on flying player
            if (target->IsTaxiFlying())
            {
                switch (m_spellInfo->Id)
                {
                    // Except some spells from Taxi Flying cast
                    case 36573:                             // Vision Guide
                    case 42316:                             // Alcaz Survey Credit
                    case 42385:                             // Alcaz Survey Aura
                        break;
                    default:
                        return SPELL_FAILED_BAD_TARGETS;
                }
            }

            if (!m_IsTriggeredSpell)
            {
                if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && VMAP::VMapFactory::checkSpellForLoS(m_spellInfo->Id) && !m_caster->IsWithinLOSInMap(target))
                    return SPELL_FAILED_LINE_OF_SIGHT;

                if (m_caster->IsVisionObscured(target))
                    return SPELL_FAILED_VISION_OBSCURED;
            }

            // auto selection spell rank implemented in WorldSession::HandleCastSpellOpcode
            // this case can be triggered if rank not found (too low-level target for first rank)
            if (m_caster->GetTypeId() == TYPEID_PLAYER && !m_CastItem && !m_IsTriggeredSpell)
            {
                // spell expected to be auto-downranking in cast handle, so must be same
                if (m_spellInfo != sSpellMgr.SelectAuraRankForLevel(m_spellInfo, target->getLevel()))
                    return SPELL_FAILED_LOWLEVEL;
            }

            if (strict && m_spellInfo->HasAttribute(SPELL_ATTR_EX3_TARGET_ONLY_PLAYER) && target->GetTypeId() != TYPEID_PLAYER && !IsAreaOfEffectSpell(m_spellInfo))
                return SPELL_FAILED_BAD_TARGETS;
        }
        else if (m_caster == target)
        {
            // Some special spells with non-caster only mode

            // Fire Shield
            if (classOptions && classOptions->SpellFamilyName == SPELLFAMILY_WARLOCK &&
                m_spellInfo->SpellIconID == 16)
                return SPELL_FAILED_BAD_TARGETS;

            // Focus Magic (main spell)
            if (m_spellInfo->Id == 54646)
                return SPELL_FAILED_BAD_TARGETS;

            // Lay on Hands (self cast)
            if (classOptions && classOptions->SpellFamilyName == SPELLFAMILY_PALADIN &&
                classOptions->SpellFamilyFlags & UI64LIT(0x0000000000008000))
            {
                if (target->HasAura(25771))                 // Forbearance
                    return SPELL_FAILED_CASTER_AURASTATE;
            }
        }

        // check pet presents
        for(int j = 0; j < MAX_EFFECT_INDEX; ++j)
        {
            SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
            if(!spellEffect)
                continue;

            if(spellEffect->EffectImplicitTargetA == TARGET_PET)
            {
                Pet *pet = m_caster->GetPet();
                if (!pet || (!pet->isAlive() && !IsSpellAllowDeadTarget(m_spellInfo)))
                {
                    if (m_triggeredByAuraSpell)              // not report pet not existence for triggered spells
                        return SPELL_FAILED_DONT_REPORT;
                    else
                        return SPELL_FAILED_NO_PET;
                }
                break;
            }
        }

        //check creature type
        //ignore self casts (including area casts when caster selected as target)
        if (non_caster_target)
        {
            if (!CheckTargetCreatureType(target))
            {
                if(target->GetTypeId() == TYPEID_PLAYER)
                    return SPELL_FAILED_TARGET_IS_PLAYER;
                else
                    return SPELL_FAILED_BAD_TARGETS;
            }

            // simple cases
            bool explicit_target_mode = false;
            bool target_hostile = false;
            bool target_hostile_checked = false;
            bool target_friendly = false;
            bool target_friendly_checked = false;
            for(int k = 0; k < MAX_EFFECT_INDEX;  ++k)
            {
                SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(k));
                if (!spellEffect)
                    continue;
                if (IsExplicitPositiveTarget(spellEffect->EffectImplicitTargetA))
                {
                    if (!target_hostile_checked)
                    {
                        target_hostile_checked = true;
                        target_hostile = m_caster->IsHostileTo(target);
                    }

                    if (target_hostile)
                        return SPELL_FAILED_BAD_TARGETS;

                    explicit_target_mode = true;
                }
                else if (IsExplicitNegativeTarget(spellEffect->EffectImplicitTargetA))
                {
                    if (!target_friendly_checked)
                    {
                        target_friendly_checked = true;
                        target_friendly = m_caster->IsFriendlyTo(target);
                    }

                    if (target_friendly)
                        return SPELL_FAILED_BAD_TARGETS;

                    explicit_target_mode = true;
                }
            }
            // TODO: this check can be applied and for player to prevent cheating when IsPositiveSpell will return always correct result.
            // check target for pet/charmed casts (not self targeted), self targeted cast used for area effects and etc
            if (!explicit_target_mode && m_caster->GetTypeId() == TYPEID_UNIT && m_caster->GetCharmerOrOwnerGuid() && !IsDispelSpell(m_spellInfo))
            {
                // check correctness positive/negative cast target (pet cast real check and cheating check)
                if (IsPositiveSpell(m_spellInfo->Id))
                {
                    if (!target_hostile_checked)
                    {
                        target_hostile_checked = true;
                        target_hostile = m_caster->IsHostileTo(target);
                    }

                    if (target_hostile)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                {
                    if (!target_friendly_checked)
                    {
                        target_friendly_checked = true;
                        target_friendly = m_caster->IsFriendlyTo(target);
                    }

                    if (target_friendly)
                        return SPELL_FAILED_BAD_TARGETS;
                }
            }
        }

        if (IsPositiveSpell(m_spellInfo->Id))
            if (target->IsImmuneToSpell(m_spellInfo, target == m_caster))
                return SPELL_FAILED_TARGET_AURASTATE;

        // Must be behind the target.
        if (m_spellInfo->AttributesEx2 == SPELL_ATTR_EX2_UNK20 && m_spellInfo->HasAttribute(SPELL_ATTR_EX_UNK9) && target->HasInArc(M_PI_F, m_caster))
        {
            // Exclusion for Pounce: Facing Limitation was removed in 2.0.1, but it still uses the same, old Ex-Flags
            // Exclusion for Mutilate:Facing Limitation was removed in 2.0.1 and 3.0.3, but they still use the same, old Ex-Flags
            // Exclusion for Throw: Facing limitation was added in 3.2.x, but that shouldn't be
            if (!m_spellInfo->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x0000000000020000)) &&
                !m_spellInfo->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x0020000000000000)) &&
                m_spellInfo->Id != 2764)
            {
                SendInterrupted(2);
                return SPELL_FAILED_NOT_BEHIND;
            }
        }

        // Target must be facing you.
        if ((m_spellInfo->Attributes == (SPELL_ATTR_UNK4 | SPELL_ATTR_NOT_SHAPESHIFT | SPELL_ATTR_UNK18 | SPELL_ATTR_STOP_ATTACK_TARGET)) && !target->HasInArc(M_PI_F, m_caster) &&
            (m_spellInfo->Id != 1776 || !m_caster->HasAura(56809))) // Glyph of Gouge allows cast Gouge when not in front
        {
            SendInterrupted(2);
            return SPELL_FAILED_NOT_INFRONT;
        }

        // check if target is in combat
        if (non_caster_target && m_spellInfo->HasAttribute(SPELL_ATTR_EX_NOT_IN_COMBAT_TARGET) && target->isInCombat())
            return SPELL_FAILED_TARGET_AFFECTING_COMBAT;
    }
    // zone check
    if (!m_caster->GetMap() || !m_caster->GetTerrain())
        return SPELL_FAILED_DONT_REPORT;

    uint32 zone, area;
    m_caster->GetZoneAndAreaId(zone, area);

    SpellCastResult locRes= sSpellMgr.GetSpellAllowedInLocationError(m_spellInfo, m_caster->GetMapId(), zone, area,
        m_caster->GetCharmerOrOwnerPlayerOrPlayerItself());
    if (locRes != SPELL_CAST_OK)
    {
        if (!m_IsTriggeredSpell)
            return locRes;
        else
            return SPELL_FAILED_DONT_REPORT;
    }

    bool castOnVehicleAllowed = false;

    if (m_caster->GetVehicle())
    {
        if (m_spellInfo->AttributesEx6 & SPELL_ATTR_EX6_CASTABLE_ON_VEHICLE)
            castOnVehicleAllowed = true;

        if ( VehicleSeatEntry const* seatInfo = m_caster->GetVehicle()->GetSeatInfo(m_caster))
            if (seatInfo->m_flags & SEAT_FLAG_CAN_CAST || seatInfo->m_flags & SEAT_FLAG_CAN_ATTACK)
                castOnVehicleAllowed = true;
    }

    // not let players cast spells at mount (and let do it to creatures)
    if ((m_caster->IsMounted() || (m_caster->GetVehicle() && !castOnVehicleAllowed)) && m_caster->GetTypeId() == TYPEID_PLAYER && !m_IsTriggeredSpell &&
        !IsPassiveSpell(m_spellInfo) && !m_spellInfo->HasAttribute(SPELL_ATTR_CASTABLE_WHILE_MOUNTED))
    {
        if (m_caster->IsTaxiFlying())
            return SPELL_FAILED_NOT_ON_TAXI;
        else
            return SPELL_FAILED_NOT_MOUNTED;
    }

    // always (except passive spells) check items
    if (!IsPassiveSpell(m_spellInfo))
    {
        SpellCastResult castResult = CheckItems();
        if(castResult != SPELL_CAST_OK)
            return castResult;
    }

    // check spell focus object
    if(uint32 spellFocus = m_spellInfo->GetRequiresSpellFocus())
    {
        GameObject* ok = NULL;
        MaNGOS::GameObjectFocusCheck go_check(m_caster, spellFocus);
        MaNGOS::GameObjectSearcher<MaNGOS::GameObjectFocusCheck> checker(ok, go_check);
        Cell::VisitGridObjects(m_caster, checker, m_caster->GetMap()->GetVisibilityDistance());

        if (!ok)
        {
            switch (spellFocus)
            {
                case 1:         // Anvil
                case 3:         // Forge
                case 1552:      // Runeforging
                    if (sWorld.getConfig(CONFIG_BOOL_FUN_IGNORE_SPELL_FOCUS) && m_caster->GetTypeId() == TYPEID_PLAYER &&
                        !((Player*)m_caster)->InBattleGround() && !((Player*)m_caster)->InArena())
                    {
                        ok = NULL;
                        break;
                    }
                default:
                    return SPELL_FAILED_REQUIRES_SPELL_FOCUS;
            }
        }

        focusObject = ok;                                   // game object found in range
    }

    // Database based targets from spell_target_script
    if (m_UniqueTargetInfo.empty())                         // skip second CheckCast apply (for delayed spells for example)
    {
        for(int j = 0; j < MAX_EFFECT_INDEX; ++j)
        {
            SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
            if(!spellEffect)
                continue;

            if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT ||
               (spellEffect->EffectImplicitTargetB == TARGET_SCRIPT && spellEffect->EffectImplicitTargetA != TARGET_SELF) ||
               spellEffect->EffectImplicitTargetA == TARGET_SCRIPT_COORDINATES ||
               spellEffect->EffectImplicitTargetB == TARGET_SCRIPT_COORDINATES ||
               spellEffect->EffectImplicitTargetA == TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT)
            {

                SQLMultiStorage::SQLMSIteratorBounds<SpellTargetEntry> bounds = sSpellScriptTargetStorage.getBounds<SpellTargetEntry>(m_spellInfo->Id);

                if (bounds.first == bounds.second)
                {
                    if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT || spellEffect->EffectImplicitTargetB == TARGET_SCRIPT)
                        sLog.outErrorDb("Spell entry %u, effect %i has EffectImplicitTargetA/EffectImplicitTargetB = TARGET_SCRIPT, but creature are not defined in `spell_script_target`", m_spellInfo->Id, j);

                    if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT_COORDINATES || spellEffect->EffectImplicitTargetB == TARGET_SCRIPT_COORDINATES)
                        sLog.outErrorDb("Spell entry %u, effect %i has EffectImplicitTargetA/EffectImplicitTargetB = TARGET_SCRIPT_COORDINATES, but gameobject or creature are not defined in `spell_script_target`", m_spellInfo->Id, j);

                    if (spellEffect->EffectImplicitTargetA == TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT)
                        sLog.outErrorDb("Spell entry %u, effect %i has EffectImplicitTargetA/EffectImplicitTargetB = TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT, but gameobject are not defined in `spell_script_target`", m_spellInfo->Id, j);
                }

                SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex);
                float range = GetSpellMaxRange(srange);

                Creature* targetExplicit = NULL;            // used for cases where a target is provided (by script for example)
                Creature* creatureScriptTarget = NULL;
                GameObject* goScriptTarget = NULL;

                for (SQLMultiStorage::SQLMultiSIterator<SpellTargetEntry> i_spellST = bounds.first; i_spellST != bounds.second; ++i_spellST)
                {
                    if (i_spellST->CanNotHitWithSpellEffect(SpellEffectIndex(j)))
                        continue;

                    switch (i_spellST->type)
                    {
                        case SPELL_TARGET_TYPE_GAMEOBJECT:
                        {
                            GameObject* p_GameObject = NULL;

                            if (i_spellST->targetEntry)
                            {
                                MaNGOS::NearestGameObjectEntryInObjectRangeCheck go_check(*m_caster, i_spellST->targetEntry, range);
                                MaNGOS::GameObjectLastSearcher<MaNGOS::NearestGameObjectEntryInObjectRangeCheck> checker(p_GameObject, go_check);
                                Cell::VisitGridObjects(m_caster, checker, range);

                                if (p_GameObject)
                                {
                                    // remember found target and range, next attempt will find more near target with another entry
                                    creatureScriptTarget = NULL;
                                    goScriptTarget = p_GameObject;
                                    range = go_check.GetLastRange();
                                }
                            }
                            else if (focusObject)           // Focus Object
                            {
                                float frange = m_caster->GetDistance(focusObject);
                                if (range >= frange)
                                {
                                    creatureScriptTarget = NULL;
                                    goScriptTarget = focusObject;
                                    range = frange;
                                }
                            }
                            break;
                        }
                        case SPELL_TARGET_TYPE_CREATURE:
                        case SPELL_TARGET_TYPE_DEAD:
                        default:
                        {
                            Creature *p_Creature = NULL;

                            // check if explicit target is provided and check it up against database valid target entry/state
                            if (Unit* pTarget = m_targets.getUnitTarget())
                            {
                                if (pTarget->GetTypeId() == TYPEID_UNIT && pTarget->GetEntry() == i_spellST->targetEntry)
                                {
                                    if (i_spellST->type == SPELL_TARGET_TYPE_DEAD && ((Creature*)pTarget)->IsCorpse())
                                    {
                                        // always use spellMaxRange, in case GetLastRange returned different in a previous pass
                                        if (pTarget->IsWithinDistInMap(m_caster, GetSpellMaxRange(srange)))
                                            targetExplicit = (Creature*)pTarget;
                                    }
                                    else if (i_spellST->type == SPELL_TARGET_TYPE_CREATURE && pTarget->isAlive())
                                    {
                                        // always use spellMaxRange, in case GetLastRange returned different in a previous pass
                                        if (pTarget->IsWithinDistInMap(m_caster, GetSpellMaxRange(srange)))
                                            targetExplicit = (Creature*)pTarget;
                                    }
                                }
                            }

                            // no target provided or it was not valid, so use closest in range
                            if (!targetExplicit)
                            {
                                MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck u_check(*m_caster, i_spellST->targetEntry, i_spellST->type != SPELL_TARGET_TYPE_DEAD, i_spellST->type == SPELL_TARGET_TYPE_DEAD, range);
                                MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(p_Creature, u_check);

                                // Visit all, need to find also Pet* objects
                                Cell::VisitAllObjects(m_caster, searcher, range);

                                range = u_check.GetLastRange();
                            }

                            // always prefer provided target if it's valid
                            if (targetExplicit)
                                creatureScriptTarget = targetExplicit;
                            else if (p_Creature)
                                creatureScriptTarget = p_Creature;

                            if (creatureScriptTarget)
                                goScriptTarget = NULL;

                            break;
                        }
                    }
                }

                if (creatureScriptTarget)
                {
                    // store coordinates for TARGET_SCRIPT_COORDINATES
                    if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT_COORDINATES ||
                        spellEffect->EffectImplicitTargetB == TARGET_SCRIPT_COORDINATES)
                    {
                        m_targets.setDestination(creatureScriptTarget->GetPositionX(),creatureScriptTarget->GetPositionY(),creatureScriptTarget->GetPositionZ());

                        if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT_COORDINATES && spellEffect->Effect != SPELL_EFFECT_PERSISTENT_AREA_AURA)
                            AddUnitTarget(creatureScriptTarget, SpellEffectIndex(j));
                    }
                    // store explicit target for TARGET_SCRIPT
                    else
                    {
                        if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT ||
                            spellEffect->EffectImplicitTargetB == TARGET_SCRIPT)
                            AddUnitTarget(creatureScriptTarget, SpellEffectIndex(j));
                    }
                }
                else if (goScriptTarget)
                {
                    // store coordinates for TARGET_SCRIPT_COORDINATES
                    if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT_COORDINATES ||
                        spellEffect->EffectImplicitTargetB == TARGET_SCRIPT_COORDINATES)
                    {
                        m_targets.setDestination(goScriptTarget->GetPositionX(),goScriptTarget->GetPositionY(),goScriptTarget->GetPositionZ());

                        if (spellEffect->EffectImplicitTargetA == TARGET_SCRIPT_COORDINATES && spellEffect->Effect != SPELL_EFFECT_PERSISTENT_AREA_AURA)
                            AddGOTarget(goScriptTarget, SpellEffectIndex(j));
                    }
                    // store explicit target for TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT
                    else
                    {
                        if (spellEffect->EffectImplicitTargetA == TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT ||
                            spellEffect->EffectImplicitTargetB == TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT)
                            AddGOTarget(goScriptTarget, SpellEffectIndex(j));
                    }
                }
                //Missing DB Entry or targets for this spellEffect.
                else
                {
                    /* For TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT makes DB targets optional not required for now
                     * TODO: Makes more research for this target type
                     */
                    if (spellEffect->EffectImplicitTargetA != TARGET_FOCUS_OR_SCRIPTED_GAMEOBJECT)
                    {
                        // not report target not existence for triggered spells
                        if (m_triggeredByAuraSpell || m_IsTriggeredSpell)
                            return SPELL_FAILED_DONT_REPORT;
                        else
                            return SPELL_FAILED_BAD_TARGETS;
                    }
                }
            }
        }
    }

    if(!m_IsTriggeredSpell)
    {
        SpellCastResult castResult = CheckRange(strict);
        if(castResult != SPELL_CAST_OK)
            return castResult;
    }

    {
        SpellCastResult castResult = CheckPower();
        if(castResult != SPELL_CAST_OK)
            return castResult;
    }

    if(!m_IsTriggeredSpell)                                 // triggered spell not affected by stun/etc
    {
        SpellCastResult castResult = CheckCasterAuras();
        if(castResult != SPELL_CAST_OK)
            return castResult;
    }

    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
        if(!spellEffect)
            continue;
        // for effects of spells that have only one target
        switch(spellEffect->Effect)
        {
            case SPELL_EFFECT_INSTAKILL:
                // Death Pact
                if(m_spellInfo->Id == 48743)
                {
                    if (m_caster->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_ERROR;

                    Pet* target = m_caster->GetPet();
                    if (!target)
                        target = m_caster->FindGuardianWithEntry(26125, true);

                    // alive
                    if (!target || target->isDead())
                        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
                    // undead
                    if (target->GetCreatureType() != CREATURE_TYPE_UNDEAD)
                        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
                    // owned
                    if (target->GetOwnerGuid() != m_caster->GetObjectGuid())
                        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                    float dist = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellEffect->GetRadiusIndex()));
                    if (!target->IsWithinDistInMap(m_caster,dist))
                        return SPELL_FAILED_OUT_OF_RANGE;

                    // will set in target selection code
                }
                break;
            case SPELL_EFFECT_DUMMY:
            {
                if (m_spellInfo->SpellIconID == 1648)       // Execute
                {
                    if (!m_targets.getUnitTarget() || m_targets.getUnitTarget()->GetHealth() > m_targets.getUnitTarget()->GetMaxHealth()*0.2 && !m_caster->IsIgnoreUnitState(m_spellInfo, IGNORE_UNIT_TARGET_STATE))
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else if (m_spellInfo->Id == 34026)          // Kill Command
                {
                    Pet* pet = m_caster->GetPet();
                    if (!pet)
                        return SPELL_FAILED_NO_PET;

                    Unit* target = pet->getVictim();
                    if (!target)
                        return SPELL_FAILED_NO_VALID_TARGETS;

                    if (pet->GetDistance(target) > 5.0f)
                        return SPELL_FAILED_OUT_OF_RANGE;
                    break;
                }
                else if (m_spellInfo->Id == 51582)          // Rocket Boots Engaged
                {
                    if (m_caster->IsInWater())
                        return SPELL_FAILED_ONLY_ABOVEWATER;
                }
                else if (m_spellInfo->Id == 68996)          // Two forms
                {
                    if (m_caster->isInCombat())
                        return SPELL_FAILED_AFFECTING_COMBAT;
                }
                else if (m_spellInfo->Id == 86150)          // Guardian of Ancient Kings
                {
                    // do not cast when not specced
                    // check Holy Shock, Avenger's Shield and Templar's Verdict
                    if (!m_caster->HasSpell(20473) && !m_caster->HasSpell(31935) && !m_caster->HasSpell(85256))
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                    break;
                }
                else if (m_spellInfo->SpellIconID == 156)   // Holy Shock
                {
                    // spell different for friends and enemies
                    // hart version required facing
                    if (m_targets.getUnitTarget() && !m_caster->IsFriendlyTo(m_targets.getUnitTarget()) && !m_caster->HasInArc(M_PI_F, m_targets.getUnitTarget()))
                        return SPELL_FAILED_UNIT_NOT_INFRONT;
                }
                else if (m_spellInfo->Id == 47541)          // Death Coil
                {
                    if (m_targets.getUnitTarget() && !CheckTargetCreatureType(m_targets.getUnitTarget()))
                        return SPELL_FAILED_BAD_TARGETS;
                    break;
                }
                else if(m_spellInfo->Id == 49576)           // Death Grip
                {
                    if(m_caster->m_movementInfo.HasMovementFlag(MovementFlags(MOVEFLAG_FALLING | MOVEFLAG_FALLINGFAR)))
                        return SPELL_FAILED_MOVING;
                }
                // Unleash Elements
                else if (m_spellInfo->Id == 73680)
                {
                    bool hasAnyEnch = false;
                    uint8 slots[2]= { EQUIPMENT_SLOT_MAINHAND, EQUIPMENT_SLOT_OFFHAND };
                    for (uint8 i = 0; i < 2; ++i)
                    {
                        if (Item* item = ((Player*)m_caster)->GetItemByPos(INVENTORY_SLOT_BAG_0, slots[i]))
                            if (uint32 ench = item->GetEnchantmentId(TEMP_ENCHANTMENT_SLOT))
                                switch (ench)
                                {
                                    case 2:         // Frostbrand Weapon
                                    case 5:         // Flametongue Weapon
                                    case 283:       // Windfury Weapon
                                    case 3021:      // Rockbiter
                                    case 3345:      // Earthliving
                                        hasAnyEnch = true;
                                        break;
                                    default:
                                        break;
                                }
                    }
                    if (!hasAnyEnch)
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }
                break;
            }
            case SPELL_EFFECT_SCHOOL_DAMAGE:
            {
                // Hammer of Wrath
                if (m_spellInfo->SpellVisual[0] == 7250)
                {
                    if (!m_targets.getUnitTarget())
                        return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
                }
                // Soul Swap Exhale
                else if (m_spellInfo->Id == 86213)
                {
                    Unit* target = m_targets.getUnitTarget();
                    if (!target)
                        return SPELL_FAILED_BAD_TARGETS;

                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (!((Player*)m_caster)->m_soulSwapData.swapTarget ||
                            ((Player*)m_caster)->m_soulSwapData.swapTarget == target->GetObjectGuid())
                            return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
                    }
                }
                break;
            }
            case SPELL_EFFECT_TAMECREATURE:
            {
                // Spell can be triggered, we need to check original caster prior to caster
                Unit* caster = GetAffectiveCaster();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER ||
                    !m_targets.getUnitTarget() ||
                    m_targets.getUnitTarget()->GetTypeId() == TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Player* plrCaster = (Player*)caster;

                bool gmmode = m_triggeredBySpellInfo == NULL && !sWorld.getConfig(CONFIG_BOOL_FUN_TAME_IGNORE_GM_MODE);

                if (gmmode && !ChatHandler(plrCaster).FindCommand("npc tame"))
                {
                    plrCaster->SendPetTameFailure(PETTAME_UNKNOWNERROR);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if(plrCaster->getClass() != CLASS_HUNTER && !gmmode)
                {
                    plrCaster->SendPetTameFailure(PETTAME_UNITSCANTTAME);
                    return SPELL_FAILED_DONT_REPORT;
                }

                Creature* target = (Creature*)m_targets.getUnitTarget();

                if(target->IsPet() || target->isCharmed())
                {
                    plrCaster->SendPetTameFailure(PETTAME_CREATUREALREADYOWNED);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (target->getLevel() > plrCaster->getLevel() && !gmmode)
                {
                    plrCaster->SendPetTameFailure(PETTAME_TOOHIGHLEVEL);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (target->GetCreatureInfo()->IsExotic() && !plrCaster->CanTameExoticPets() && !gmmode)
                {
                    plrCaster->SendPetTameFailure(PETTAME_CANTCONTROLEXOTIC);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (!target->GetCreatureInfo()->isTameable(plrCaster->CanTameExoticPets()))
                {
                    plrCaster->SendPetTameFailure(PETTAME_NOTTAMEABLE);
                    return SPELL_FAILED_DONT_REPORT;
                }

                if (plrCaster->GetPetGuid() || plrCaster->GetCharmGuid())
                {
                    plrCaster->SendPetTameFailure(PETTAME_ANOTHERSUMMONACTIVE);
                    return SPELL_FAILED_DONT_REPORT;
                }

                PetSaveMode slot = plrCaster->GetFreeStableSlot();
                if (slot == PET_SAVE_NOT_IN_SLOT)
                {
                    plrCaster->SendPetTameFailure(PETTAME_TOOMANY);
                    return SPELL_FAILED_DONT_REPORT;
                }

                break;
            }
            case SPELL_EFFECT_LEARN_SPELL:
            {
                if(spellEffect->EffectImplicitTargetA != TARGET_PET)
                    break;

                Pet* pet = m_caster->GetPet();

                if(!pet)
                    return SPELL_FAILED_NO_PET;

                SpellEntry const *learn_spellproto = sSpellStore.LookupEntry(spellEffect->EffectTriggerSpell);

                if(!learn_spellproto)
                    return SPELL_FAILED_NOT_KNOWN;

                if(m_spellInfo->GetSpellLevel() > pet->getLevel())
                    return SPELL_FAILED_LOWLEVEL;

                break;
            }
            case SPELL_EFFECT_LEARN_PET_SPELL:
            {
                Pet* pet = m_caster->GetPet();

                if(!pet)
                    return SPELL_FAILED_NO_PET;

                SpellEntry const *learn_spellproto = sSpellStore.LookupEntry(spellEffect->EffectTriggerSpell);
                if (!learn_spellproto)
                    return SPELL_FAILED_NOT_KNOWN;

                if(m_spellInfo->GetSpellLevel() > pet->getLevel())
                    return SPELL_FAILED_LOWLEVEL;

                break;
            }
            case SPELL_EFFECT_APPLY_GLYPH:
            {
                uint32 glyphId = spellEffect->EffectMiscValue;
                if(GlyphPropertiesEntry const *gp = sGlyphPropertiesStore.LookupEntry(glyphId))
                    if(m_caster->HasAura(gp->SpellId))
                        return SPELL_FAILED_UNIQUE_GLYPH;
                break;
            }
            case SPELL_EFFECT_FEED_PET:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                Item* foodItem = m_targets.getItemTarget();
                if (!foodItem)
                    return SPELL_FAILED_BAD_TARGETS;

                Pet* pet = m_caster->GetPet();

                if (!pet)
                    return SPELL_FAILED_NO_PET;

                if (!pet->HaveInDiet(foodItem->GetProto()))
                    return SPELL_FAILED_WRONG_PET_FOOD;

                if (!pet->GetCurrentFoodBenefitLevel(foodItem->GetProto()->ItemLevel))
                    return SPELL_FAILED_FOOD_LOWLEVEL;

                if (pet->isInCombat())
                    return SPELL_FAILED_AFFECTING_COMBAT;

                break;
            }
            case SPELL_EFFECT_POWER_BURN:
            case SPELL_EFFECT_POWER_DRAIN:
            {
                // Can be area effect, Check only for players and not check if target - caster (spell can have multiply drain/burn effects)
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (Unit* target = m_targets.getUnitTarget())
                        if (target != m_caster && int32(target->getPowerType()) != spellEffect->EffectMiscValue)
                            return SPELL_FAILED_BAD_TARGETS;
                break;
            }
            case SPELL_EFFECT_JUMP2:
            case SPELL_EFFECT_TELEPORT_UNITS:
            case SPELL_EFFECT_CHARGE:
            {
                if (m_caster->hasUnitState(UNIT_STAT_ROOT))
                {
                    // Intervene vs. Warbringer
                    if (m_spellInfo->Id == 3411 && m_caster->HasAura(57499))
                        m_caster->RemoveAurasAtMechanicImmunity(IMMUNE_TO_ROOT_AND_SNARE_MASK, 0, true);
                    else
                        return SPELL_FAILED_ROOTED;
                }

                if (!m_IsTriggeredSpell && spellEffect->Effect == SPELL_EFFECT_CHARGE)
                {
                    float destX = 0.f;
                    float destY = 0.f;
                    float destZ = 0.f;
                    if (m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION)
                    {
                        destX = m_targets.m_destX;
                        destY = m_targets.m_destY;
                        destZ = m_targets.m_destZ;
                    }
                    else if (m_targets.getUnitTarget())
                    {
                        destX = m_targets.getUnitTarget()->GetPositionX();
                        destY = m_targets.getUnitTarget()->GetPositionY();
                        destZ = m_targets.getUnitTarget()->GetPositionZ();
                    }
                    else
                        break;

                    bool ok = false;
                    float pathDist = m_caster->GetPathLength(destX, destY, destZ, false, &ok);
                    // pathfinding disabled - do nothing
                    if (!ok)
                        break;

                    float range;

                    if (uint32 radiusIndex = spellEffect->GetRadiusIndex())
                        range = GetSpellRadius(sSpellRadiusStore.LookupEntry(radiusIndex));
                    else
                        range = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

                    if (Player* modOwner = m_caster->GetSpellModOwner())
                        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, range, this);

                    if (pathDist > range)
                        return SPELL_FAILED_OUT_OF_RANGE;
                }

                break;
            }
            case SPELL_EFFECT_SKINNING:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER || !m_targets.getUnitTarget() || m_targets.getUnitTarget()->GetTypeId() != TYPEID_UNIT)
                    return SPELL_FAILED_BAD_TARGETS;

                Creature* creature = (Creature*)m_targets.getUnitTarget();

                // allow skinning non-looted creatures when not in instance
                if (!creature->GetMap()->GetInstanceId())
                {
                    if (creature->GetCreatureType() != CREATURE_TYPE_CRITTER && creature->lootForSkin && !creature->loot.empty())
                        return SPELL_FAILED_TARGET_NOT_LOOTED;
                    // loot will be cleared in spell effect
                }
                else
                {
                    if (!creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SKINNABLE))
                        return SPELL_FAILED_TARGET_UNSKINNABLE;

                    if ( creature->GetCreatureType() != CREATURE_TYPE_CRITTER && ( !creature->lootForBody || creature->lootForSkin || !creature->loot.empty() ) )
                        return SPELL_FAILED_TARGET_NOT_LOOTED;
                }

                uint32 skill = creature->GetCreatureInfo()->GetRequiredLootSkill();

                int32 skillValue = ((Player*)m_caster)->GetSkillValue(skill);
                int32 TargetLevel = m_targets.getUnitTarget()->getLevel();
                int32 ReqValue = (skillValue < 100 ? (TargetLevel-10) * 10 : TargetLevel * 5);
                if (ReqValue > skillValue)
                    return SPELL_FAILED_LOW_CASTLEVEL;

                // chance for fail at orange skinning attempt
                if( (m_selfContainer && (*m_selfContainer) == this) &&
                    skillValue < sWorld.GetConfigMaxSkillValue() &&
                    (ReqValue < 0 ? 0 : ReqValue) > irand(skillValue - 25, skillValue + 37) )
                    return SPELL_FAILED_TRY_AGAIN;

                break;
            }
            case SPELL_EFFECT_OPEN_LOCK:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)  // only players can open locks, gather etc.
                    return SPELL_FAILED_BAD_TARGETS;

                // we need a go target in case of TARGET_GAMEOBJECT (for other targets acceptable GO and items)
                if (spellEffect->EffectImplicitTargetA == TARGET_GAMEOBJECT)
                {
                    if (!m_targets.getGOTarget())
                        return SPELL_FAILED_BAD_TARGETS;
                }

                // get the lock entry
                uint32 lockId = 0;
                if (GameObject* go = m_targets.getGOTarget())
                {
                    // In BattleGround players can use only flags and banners
                    if( ((Player*)m_caster)->InBattleGround() &&
                        !((Player*)m_caster)->CanUseBattleGroundObject() )
                        return SPELL_FAILED_TRY_AGAIN;

                    lockId = go->GetGOInfo()->GetLockId();
                    if (!lockId)
                        return SPELL_FAILED_ALREADY_OPEN;
                }
                else if(Item* item = m_targets.getItemTarget())
                {
                    // not own (trade?)
                    if (item->GetOwner() != m_caster)
                        return SPELL_FAILED_ITEM_GONE;

                    lockId = item->GetProto()->LockID;

                    // if already unlocked
                    if (!lockId || item->HasFlag(ITEM_FIELD_FLAGS, ITEM_DYNFLAG_UNLOCKED))
                        return SPELL_FAILED_ALREADY_OPEN;
                }
                else
                    return SPELL_FAILED_BAD_TARGETS;

                SkillType skillId = SKILL_NONE;
                int32 reqSkillValue = 0;
                int32 skillValue = 0;

                // check lock compatibility
                SpellCastResult res = CanOpenLock(SpellEffectIndex(i), lockId, skillId, reqSkillValue, skillValue);
                if(res != SPELL_CAST_OK)
                    return res;

                // chance for fail at orange mining/herb/LockPicking gathering attempt
                // second check prevent fail at rechecks
                if (skillId != SKILL_NONE && skillId != SKILL_ARCHAEOLOGY && (!m_selfContainer || ((*m_selfContainer) != this)))
                {
                    bool canFailAtMax = skillId != SKILL_HERBALISM && skillId != SKILL_MINING;

                    // chance for failure in orange gather / lockpick (gathering skill can't fail at maxskill)
                    if ((canFailAtMax || skillValue < sWorld.GetConfigMaxSkillValue()) && reqSkillValue > irand(skillValue - 25, skillValue + 37))
                        return SPELL_FAILED_TRY_AGAIN;
                }
                break;
            }
            case SPELL_EFFECT_SUMMON_DEAD_PET:
            {
                Creature *pet = m_caster->GetPet();
                if(!pet)
                    return SPELL_FAILED_NO_PET;

                if(pet->isAlive())
                    return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                break;
            }
            // This is generic summon effect
            case SPELL_EFFECT_SUMMON:
            {
                if (SummonPropertiesEntry const *summon_prop = sSummonPropertiesStore.LookupEntry(spellEffect->EffectMiscValueB))
                {
                    if(summon_prop->Group == SUMMON_PROP_GROUP_PETS)
                    {
                        if (m_caster->GetPetGuid())
                            return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                        if (m_caster->GetCharmGuid())
                            return SPELL_FAILED_ALREADY_HAVE_CHARM;
                    }
                }

                break;
            }
            case SPELL_EFFECT_SUMMON_OBJECT_SLOT:
            case SPELL_EFFECT_SURVEY:
            case SPELL_EFFECT_SUMMON_RAID_MARKER:
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                {
                    if (!((Player*)m_caster)->GetTransportGuid().IsEmpty())
                        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                    if (BattleGround* bg = ((Player*)m_caster)->GetBattleGround())
                    {
                        if (bg->GetTypeID(true) == BATTLEGROUND_DS)
                        {
                            if (m_caster->GetPositionZ() > 11.3f || m_caster->GetPositionZ() < 3.0f)
                                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                        }
                        else  if (bg->GetTypeID(true) == BATTLEGROUND_RV)
                        {
                            if (m_caster->GetPositionZ() < 27.0f)
                                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                        }
                    }
                }
                break;
            }
            case SPELL_EFFECT_SUMMON_PET:
            {
                if (m_caster->GetPetGuid())                 // let warlock do a replacement summon
                {

                    Pet* pet = ((Player*)m_caster)->GetPet();

                    if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->getClass() == CLASS_WARLOCK)
                    {
                        if (strict)                         //Summoning Disorientation, trigger pet stun (cast by pet so it doesn't attack player)
                            pet->CastSpell(pet, 32752, true, NULL, NULL, pet->GetObjectGuid());
                    }
                    else
                        return SPELL_FAILED_ALREADY_HAVE_SUMMON;
                }

                if (m_caster->GetCharmGuid())
                    return SPELL_FAILED_ALREADY_HAVE_CHARM;

                break;
            }
            case SPELL_EFFECT_SUMMON_PLAYER:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!((Player*)m_caster)->GetSelectionGuid())
                    return SPELL_FAILED_BAD_TARGETS;

                Player* target = sObjectMgr.GetPlayer(((Player*)m_caster)->GetSelectionGuid());

                if (!target || ((Player*)m_caster) == target)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!target->IsInSameRaidWith((Player*)m_caster) && m_spellInfo->Id != 48955)
                    return SPELL_FAILED_BAD_TARGETS;

                // check if our map is dungeon
                if (sMapStore.LookupEntry(m_caster->GetMapId())->IsDungeon())
                {
                    InstanceTemplate const* instance = ObjectMgr::GetInstanceTemplate(m_caster->GetMapId());
                    if (!instance)
                        return SPELL_FAILED_TARGET_NOT_IN_INSTANCE;
                    if (instance->levelMin > target->getLevel())
                        return SPELL_FAILED_LOWLEVEL;
                    if (instance->levelMax && instance->levelMax < target->getLevel())
                        return SPELL_FAILED_HIGHLEVEL;
                }

                // can't summon dk from starting zone
                if (!target->DkQuestsCompleted())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            }
            case SPELL_EFFECT_FRIEND_SUMMON:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                if (((Player*)m_caster)->GetSelectionGuid().IsEmpty())
                    return SPELL_FAILED_BAD_TARGETS;

                Player* target = sObjectMgr.GetPlayer(((Player*)m_caster)->GetSelectionGuid());

                if (!target || !target->IsReferAFriendLinked(((Player*)m_caster)))
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }
            case SPELL_EFFECT_LEAP:
            {
                // not allow use this effect at battleground until battleground start
                if(m_caster->GetTypeId() == TYPEID_PLAYER)
                    if(BattleGround const *bg = ((Player*)m_caster)->GetBattleGround())
                        if(bg->GetStatus() != STATUS_IN_PROGRESS)
                            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (!((Player*)m_caster)->GetTransportGuid().IsEmpty())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            }
            case SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER:
            {
                float dis = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellEffect->GetRadiusIndex()));
                float fx = m_caster->GetPositionX() + dis * cos(m_caster->GetOrientation());
                float fy = m_caster->GetPositionY() + dis * sin(m_caster->GetOrientation());
                // teleport a bit above terrain level to avoid falling below it
                float fz = m_caster->GetMap()->GetHeight(m_caster->GetPhaseMask(), fx, fy, m_caster->GetPositionZ());
                if (fz <= INVALID_HEIGHT)                   // note: this also will prevent use effect in instances without vmaps height enabled
                    return SPELL_FAILED_TRY_AGAIN;

                float caster_pos_z = m_caster->GetPositionZ();
                // Control the caster to not climb or drop when +-fz > 8
                if(!(fz <= caster_pos_z + 8 && fz >= caster_pos_z - 8))
                    return SPELL_FAILED_TRY_AGAIN;

                // not allow use this effect at battleground until battleground start
                if(m_caster->GetTypeId() == TYPEID_PLAYER)
                    if(BattleGround const *bg = ((Player*)m_caster)->GetBattleGround())
                        if(bg->GetStatus() != STATUS_IN_PROGRESS)
                            return SPELL_FAILED_TRY_AGAIN;

                if (!((Player*)m_caster)->GetTransportGuid().IsEmpty())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            }
            case SPELL_EFFECT_STEAL_BENEFICIAL_BUFF:
            {
                if (m_targets.getUnitTarget() == m_caster)
                    return SPELL_FAILED_BAD_TARGETS;
                break;
            }
            case SPELL_EFFECT_LEAP_BACK:
            {
                // Disengage
                if (m_spellInfo->Id == 781 && !m_caster->isInCombat())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (m_caster->hasUnitState(UNIT_STAT_ROOT))
                    return SPELL_FAILED_ROOTED;
                break;
            }
            case SPELL_EFFECT_TRANS_DOOR:
            {
                // Ranger: Ritual of Summoning & Fishing Chair - battleground summon exploit
                if (m_spellInfo->Id == 698 || m_spellInfo->Id == 42766)
                {
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        if (BattleGround const *bg = ((Player*)m_caster)->GetBattleGround())
                            if (bg->GetStatus() != STATUS_IN_PROGRESS)
                                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                }
                break;
            }
            case SPELL_EFFECT_TALENT_SPEC_SELECT:
            {
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    if (BattleGround* bg = ((Player*)m_caster)->GetBattleGround())
                        if (bg->GetStatus() != STATUS_WAIT_JOIN)
                            return SPELL_FAILED_NOT_IN_BATTLEGROUND;
                break;
            }
            case SPELL_EFFECT_RESURRECT:
            case SPELL_EFFECT_RESURRECT_NEW:
            case SPELL_EFFECT_SELF_RESURRECT:
            {
                if (m_caster->isInCombat())
                    return SPELL_FAILED_AFFECTING_COMBAT;

                if (spellEffect->Effect == SPELL_EFFECT_SELF_RESURRECT)
                {
                    if (m_caster->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
                        return SPELL_FAILED_TARGET_CANNOT_BE_RESURRECTED;
                }
                else if (unitTarget && unitTarget->HasAuraType(SPELL_AURA_PREVENT_RESURRECTION))
                    return SPELL_FAILED_TARGET_CANNOT_BE_RESURRECTED;
                break;
            }
            case SPELL_EFFECT_ADD_COMBO_POINTS:
            {
                // Redirect
                if (m_spellInfo->Id == 73981 && (!m_caster->GetComboPoints() ||
                    m_targets.getUnitTarget() && m_targets.getUnitTarget()->GetObjectGuid() == m_caster->GetComboTargetGuid()))
                    return SPELL_FAILED_NO_COMBO_POINTS;
                break;
            }
            case SPELL_EFFECT_BUY_GUILD_BANKSLOT:
            {
                if (!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Player* player = (Player*)m_caster;

                uint32 guildId = player->GetGuildId();
                if (!guildId)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                Guild* guild = sGuildMgr.GetGuildById(guildId);
                if (!guild)
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                if (guild->GetLeaderGuid() != player->GetObjectGuid())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                uint8 slot = uint8(spellEffect->CalculateSimpleValue() - 1);
                if (slot != guild->GetPurchasedTabs())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            }
            default:
                break;
        }
    }

    for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
        if (!spellEffect)
            continue;

        switch(spellEffect->EffectApplyAuraName)
        {
            case SPELL_AURA_DUMMY:
            {
                //custom check
                switch(m_spellInfo->Id)
                {
                    case 61336:                             // Survival Instincts
                        if (m_caster->GetTypeId() != TYPEID_PLAYER || !((Player*)m_caster)->IsInFeralForm())
                            return SPELL_FAILED_ONLY_SHAPESHIFT;
                        break;
                    case 82692:                             // Focus Fire
                    {
                        Pet* pet = m_caster->GetPet();
                        if (!pet || !pet->isAlive())
                            return SPELL_FAILED_NO_PET;

                        if (!pet->HasAura(19615))
                            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
            case SPELL_AURA_MOD_POSSESS:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_UNKNOWN;

                if (m_caster->GetPetGuid())
                    return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                if (m_caster->GetCharmGuid())
                    return SPELL_FAILED_ALREADY_HAVE_CHARM;

                if (m_caster->GetCharmerGuid())
                    return SPELL_FAILED_CHARMED;

                if (!m_targets.getUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                Unit* target = m_targets.getUnitTarget();
                if (!target && !m_UniqueTargetInfo.empty())
                    target = m_caster->GetMap()->GetUnit(m_UniqueTargetInfo.front().targetGUID);

                if (!target)
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (target == m_caster)
                    return SPELL_FAILED_BAD_TARGETS;

                if (target->GetCharmerGuid())
                    return SPELL_FAILED_CHARMED;

                if (int32(target->getLevel()) > CalculateDamage(SpellEffectIndex(i), target))
                    return SPELL_FAILED_HIGHLEVEL;

                break;
            }
            case SPELL_AURA_MOD_CHARM:
            {
                if (m_caster->GetPetGuid())
                    return SPELL_FAILED_ALREADY_HAVE_SUMMON;

                if (m_caster->GetCharmGuid())
                    return SPELL_FAILED_ALREADY_HAVE_CHARM;

                if (m_caster->GetCharmerGuid())
                    return SPELL_FAILED_CHARMED;

                Unit* target = m_targets.getUnitTarget();
                if (!target && !m_UniqueTargetInfo.empty())
                    target = m_caster->GetMap()->GetUnit(m_UniqueTargetInfo.front().targetGUID);

                if (!target)
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (target == m_caster)
                    return SPELL_FAILED_BAD_TARGETS;

                if (target->GetCharmerGuid())
                    return SPELL_FAILED_CHARMED;

                if (int32(target->getLevel()) > CalculateDamage(SpellEffectIndex(i), target)) 
                    return SPELL_FAILED_HIGHLEVEL;

                break;
            }
            case SPELL_AURA_MOD_POSSESS_PET:
            {
                if (m_caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_UNKNOWN;

                if (m_caster->GetCharmGuid())
                    return SPELL_FAILED_ALREADY_HAVE_CHARM;

                if (m_caster->GetCharmerGuid())
                    return SPELL_FAILED_CHARMED;

                Pet* pet = m_caster->GetPet();
                if (!pet)
                    return SPELL_FAILED_NO_PET;

                if (pet->GetCharmerGuid())
                    return SPELL_FAILED_CHARMED;

                break;
            }
            case SPELL_AURA_MOUNTED:
            {
                if (spellEffect->EffectMiscValueB && !m_caster->GetMountCapability(spellEffect->EffectMiscValueB))
                    return SPELL_FAILED_NOT_HERE;

                // Ignore map check if spell have AreaId. AreaId already checked and this prevent special mount spells
                if (m_caster->GetTypeId() == TYPEID_PLAYER && !sMapStore.LookupEntry(m_caster->GetMapId())->IsMountAllowed() && !m_IsTriggeredSpell && !m_spellInfo->GetAreaGroupId())
                    return SPELL_FAILED_NO_MOUNTS_ALLOWED;

                if (m_caster->IsInDisallowedMountForm())
                    return SPELL_FAILED_NOT_SHAPESHIFT;

                break;
            }
            case SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS:
            {
                if(!m_targets.getUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                // can be casted at non-friendly unit or own pet/charm
                if(m_caster->IsFriendlyTo(m_targets.getUnitTarget()))
                    return SPELL_FAILED_TARGET_FRIENDLY;

                break;
            }
            case SPELL_AURA_FLY:
            case SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED:
            {
                // not allow cast fly spells if not have req. skills  (all spells is self target)
                // allow always ghost flight spells
                if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->isAlive())
                {
                    if (!((Player*)m_caster)->CanStartFlyInArea(m_caster->GetMapId(), zone, area))
                        return m_IsTriggeredSpell ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_NOT_HERE;
                }
                break;
            }
            case SPELL_AURA_PERIODIC_MANA_LEECH:
            {
                if (!m_targets.getUnitTarget())
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;

                if (m_caster->GetTypeId() != TYPEID_PLAYER || m_CastItem)
                    break;

                if (m_targets.getUnitTarget()->getPowerType() != POWER_MANA)
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }
            /*case SPELL_AURA_MIRROR_IMAGE:
            {
                Unit* pTarget = m_targets.getUnitTarget();

                // In case of TARGET_SCRIPT, we have already added a target. Use it here (and find a better solution)
                if (m_UniqueTargetInfo.size() == 1)
                    pTarget = m_caster->GetMap()->GetAnyTypeCreature(m_UniqueTargetInfo.front().targetGUID);

                if (!pTarget)
                    return SPELL_FAILED_BAD_TARGETS;

                if (pTarget->GetTypeId() != TYPEID_UNIT)    // Target must be creature. TODO: Check if target can also be player
                    return SPELL_FAILED_BAD_TARGETS;

                if (pTarget == m_caster)                    // Clone self can't be accepted
                    return SPELL_FAILED_BAD_TARGETS;

                // It is assumed that target can not be cloned if already cloned by same or other clone auras
                if (pTarget->HasAuraType(SPELL_AURA_MIRROR_IMAGE))
                    return SPELL_FAILED_BAD_TARGETS;

                break;
            }*/
            case SPELL_AURA_HASTE_ALL:
            {
                // Dark Intent
                if (m_spellInfo->Id == 80398)
                {
                    if (m_targets.getUnitTarget() && m_targets.getUnitTarget()->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
            case SPELL_AURA_WORGEN_TRANSFORM:
            {
                if (!m_caster->HasWorgenForm())
                    return m_IsTriggeredSpell ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                break;
            }
            default:
                break;
        }
    }

    // check trade slot case (last, for allow catch any another cast problems)
    if (m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
    {
        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            return SPELL_FAILED_NOT_TRADING;

        Player *pCaster = ((Player*)m_caster);
        TradeData* my_trade = pCaster->GetTradeData();

        if (!my_trade)
            return SPELL_FAILED_NOT_TRADING;

        TradeSlots slot = TradeSlots(m_targets.getItemTargetGuid().GetRawValue());
        if (slot != TRADE_SLOT_NONTRADED)
            return SPELL_FAILED_ITEM_NOT_READY;

        // if trade not complete then remember it in trade data
        if (!my_trade->IsInAcceptProcess())
        {
            // Spell will be casted at completing the trade. Silently ignore at this place
            my_trade->SetSpell(m_spellInfo->Id, m_CastItem);
            return SPELL_FAILED_DONT_REPORT;
        }
    }

    if (m_caster->GetTypeId() == TYPEID_PLAYER && ((Player*)m_caster)->IsSpectator() && m_spellInfo->Id != 39870)
        return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

    switch(m_spellInfo->Id)
    {
        case 2654:      // Summon Premium Account companion
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER && !((Player*)m_caster)->IsPremiumActive())
            {
                ChatHandler((Player*)m_caster).PSendSysMessage(LANG_ONLY_PREMIUM);
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }
            break;
        }
        case 7384:      // Overpower
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER && m_caster->GetComboPoints() == 0 &&
                !m_caster->HasAura(60503))      // Taste For Blood talent proc
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            break;
        }
        case 36554:     // Shadowstep has SPELL_EFFECT_TELEPORT_UNITS in triggered spell, so do check here
        case 51690:     // Killing Spree
        {
            if (m_caster->hasUnitState(UNIT_STAT_ROOT))
                return SPELL_FAILED_ROOTED;
            break;
        }
        case 38782:     // Druid Signal, ovveride: need Death's Door trigger?
        {
            if (!m_caster->GetClosestCreature(19747, 60.0f))
                 return SPELL_FAILED_NOT_HERE;
            break;
        }
        case 46629:     // Deathfrost
        {
            if (Unit* target = m_targets.getUnitTarget())
                if (target->getLevel() >= 73)
                    return m_IsTriggeredSpell ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_HIGHLEVEL;
            break;
        }
        case 48020:     // Demonic Circle: Teleport
        {
            GameObject* obj = m_caster->GetGameObject(48018);
            if (!obj)
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

            if (m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                if (BattleGround* bg = ((Player*)m_caster)->GetBattleGround())
                    if (bg->GetTypeID(true) == BATTLEGROUND_DS && ((BattleGroundDS*)bg)->IsCollisionSpawned())
                    {
                        float x = obj->GetPositionX();
                        float y = obj->GetPositionY();
                        float z = obj->GetPositionZ();
                        float x1 = 1291.56f;
                        float y1 = 790.837f;
                        float z1 = 7.1f;
                        if (sqrtf((x-x1)*(x-x1)+(y-y1)*(y-y1)+(z-z1)*(z-z1)) < 6.15f)
                            return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
                    }
            }
            break;
        }
        case 88751:     // Wild Mushroom : Detonate
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER)
            {
                SummonUnitList& mushroomList = ((Player*)m_caster)->GetSummonUnitList();
                SummonUnitList tempList;

                float spellRange = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex), true);

                for (SummonUnitList::const_iterator i = mushroomList.begin(); i != mushroomList.end(); ++i)
                {
                    if ((*i)->isAlive() && (*i)->GetEntry() == 47649)
                        tempList.push_back(*i);
                }

                if (tempList.empty())
                    return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

                bool inRange = false;
                for (SummonUnitList::const_iterator i = tempList.begin(); i != tempList.end(); ++i)
                {
                    if (m_caster->IsWithinDist3d((*i)->GetPositionX(), (*i)->GetPositionY(), (*i)->GetPositionZ(), spellRange))
                    {
                        inRange = true;
                        break;
                    }
                }

                if (!inRange)
                    return SPELL_FAILED_OUT_OF_RANGE;
            }
            break;
        }
        case 91041:     // Heart's Judgement
        {
            // Heart's Revelation
            if (m_targets.getUnitTarget() && !m_targets.getUnitTarget()->HasAura(91027))
                return SPELL_FAILED_CASTER_AURASTATE;
            break;
        }
        case 91836:     // Forged Fury
        {
            Unit* target = m_targets.getUnitTarget();
            if (!target)
                return SPELL_FAILED_CASTER_AURASTATE;

            // Raw Fury
            SpellAuraHolder* fury = target->GetSpellAuraHolder(91832);
            if (!fury)
                return SPELL_FAILED_CASTER_AURASTATE;

            if (fury->GetStackAmount() != fury->GetSpellProto()->GetStackAmount())
                return SPELL_FAILED_CASTER_AURASTATE;

            break;
        }
        case 92328:     // Heart's Judgement
        {
            // Heart's Revelation
            if (m_targets.getUnitTarget() && !m_targets.getUnitTarget()->HasAura(92325))
                return SPELL_FAILED_CASTER_AURASTATE;
            break;
        }
        case 92601:     // Detonate Mana
        {
            Unit* target = m_targets.getUnitTarget();
            if (!target)
                return SPELL_FAILED_CASTER_AURASTATE;

            // Recaptured Mana
            if (!target->HasAura(92596))
                return SPELL_FAILED_CASTER_AURASTATE;

            break;
        }
        case 96880:     // Tipping of the Scales
        {
            // Weight of a Feather
            if (m_targets.getUnitTarget() && !m_targets.getUnitTarget()->HasAura(96881))
                return SPELL_FAILED_CASTER_AURASTATE;
            break;
        }
        case 698:       // Ritual of Summoning
        case 3561:      // Teleport: Stormwind
        case 3562:      // Teleport: Ironforge
        case 3563:      // Teleport: Undercity
        case 3565:      // Teleport: Darnassus
        case 3566:      // Teleport: Thunder Bluff
        case 3567:      // Teleport: Orgrimmar
        case 3578:      // Teleport: Dalaran
        case 10059:     // Portal: Stormwind
        case 11416:     // Portal: Ironforge
        case 11417:     // Portal: Orgrimmar
        case 11418:     // Portal: Undercity
        case 11419:     // Portal: Darnassus
        case 11420:     // Portal: Thunder Bluff
        case 18960:     // Teleport: Moonglade
        case 19027:     // Teleport: Moonglade
        case 28148:     // Portal: Karazhan
        case 32266:     // Portal: Exodar
        case 32267:     // Portal: Silvermoon
        case 32271:     // Teleport: Exodar
        case 32272:     // Teleport: Silvermoon
        case 33690:     // Teleport: Shattrath
        case 33691:     // Portal: Shattrath
        case 35715:     // Teleport: Shattrath
        case 35717:     // Portal: Shattrath
        case 41234:     // Teleport: Black Temple
        case 46149:     // Teleport: Shattrath
        case 49358:     // Teleport: Stonard
        case 49359:     // Teleport: Theramore
        case 49360:     // Portal: Theramore
        case 49361:     // Portal: Stonard
        case 50977:     // Death Gate
        case 53097:     // Portal: Acherus
        case 53142:     // Portal: Dalaran
        case 54406:     // Teleport: Dalaran
        case 57535:     // Portal: Moonglade
        case 57654:     // Portal: Moonglade Effect
        case 57676:     // Portal: Shattrath
        case 66238:     // Teleport: Argent Tournament
        case 71436:     // Teleport: Booty Bay
        case 73324:     // Portal: Dalaran
        {
            if (m_caster->GetTypeId() == TYPEID_PLAYER && sWorld.getConfig(CONFIG_BOOL_FUN_RESTRICT_TELEPORTS))
            {
                ChatHandler((Player*)m_caster).PSendSysMessage(LANG_FUN_TELEPORT_DISABLED);
                return SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;
            }
            break;
        }
        default:
            break;
    }

    if (!m_IsTriggeredSpell && !m_caster->GetObjectGuid().IsVehicle() &&
        VMAP::VMapFactory::checkSpellForLoS(m_spellInfo->Id) &&
        !m_targets.getUnitTarget() && !m_targets.getGOTarget() && !m_targets.getItemTarget() &&
        !m_spellInfo->HasAttribute(SPELL_ATTR_EX8_RAID_MARKER))
    {
        if (m_targets.m_destX && m_targets.m_destY && m_targets.m_destZ && !m_caster->IsWithinLOS(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ))
            return SPELL_FAILED_LINE_OF_SIGHT;
    }

    // all ok
    return SPELL_CAST_OK;
}

SpellCastResult Spell::CheckPetCast(Unit* target)
{
    if (sObjectMgr.IsSpellDisabled(m_spellInfo->Id, DISABLE_SPELL_TYPE_CAST))
        return m_IsTriggeredSpell ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW;

    if(!m_caster->isAlive() && !(m_spellInfo->Attributes & SPELL_ATTR_CASTABLE_WHILE_DEAD))
        return SPELL_FAILED_CASTER_DEAD;

    if(m_caster->IsNonMeleeSpellCasted(false))              //prevent spellcast interruption by another spellcast
    {
        if(this->m_spellInfo->Id == 33395) // Water Elemental's Freeze should overcast Waterbolt
            m_caster->InterruptNonMeleeSpells(false);
        else
            return SPELL_FAILED_SPELL_IN_PROGRESS;
    }
    if(m_caster->isInCombat() && IsNonCombatSpell(m_spellInfo))
        return SPELL_FAILED_AFFECTING_COMBAT;

    if(m_caster->GetTypeId()==TYPEID_UNIT && (((Creature*)m_caster)->IsPet() || m_caster->isCharmed()))
    {
                                                            //dead owner (pets still alive when owners ressed?)
        if(m_caster->GetCharmerOrOwner() && !m_caster->GetCharmerOrOwner()->isAlive() && m_caster->GetCharmerOrOwner()->getDeathState() != GHOULED)
            return SPELL_FAILED_CASTER_DEAD;

        if(!target && m_targets.getUnitTarget())
            target = m_targets.getUnitTarget();

        bool need = false;
        for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
            if(!spellEffect)
                continue;

            if (spellEffect->EffectImplicitTargetA == TARGET_CHAIN_DAMAGE ||
                spellEffect->EffectImplicitTargetA == TARGET_SINGLE_FRIEND ||
                spellEffect->EffectImplicitTargetA == TARGET_SINGLE_FRIEND_2 ||
                spellEffect->EffectImplicitTargetA == TARGET_DUELVSPLAYER ||
                spellEffect->EffectImplicitTargetA == TARGET_SINGLE_PARTY ||
                spellEffect->EffectImplicitTargetA == TARGET_CURRENT_ENEMY_COORDINATES)
            {
                need = true;
                if(!target)
                    return SPELL_FAILED_BAD_IMPLICIT_TARGETS;
                break;
            }
        }
        if(need)
            m_targets.setUnitTarget(target);

        Unit* _target = m_targets.getUnitTarget();

        if(_target)                                         //for target dead/target not valid
        {
            if(IsPositiveSpell(m_spellInfo->Id) && !IsDispelSpell(m_spellInfo) && m_spellInfo->Id != 47482) // Ghoul Leap excluding
            {
                if (m_caster->IsHostileTo(_target))
                {
                    DEBUG_LOG("Charmed creature attempt to cast positive spell %d, but target %s is hostile", m_spellInfo->Id, target->GetGuidStr().c_str());
                    return SPELL_FAILED_BAD_TARGETS;
                }
            }
            else if (!_target->isTargetableForAttack() || (!_target->isVisibleForOrDetect(m_caster,m_caster,true) && !m_IsTriggeredSpell))
            {
                DEBUG_LOG("Charmed creature attempt to cast spell %d, but target %s is not targetable or not detectable",m_spellInfo->Id,target->GetGuidStr().c_str());
                return SPELL_FAILED_BAD_TARGETS;            // guessed error
            }
            else
            {
                bool dualEffect = false;
                for (int j = 0; j < MAX_EFFECT_INDEX; ++j)
                {
                    SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
                    if (!spellEffect)
                        continue;

                    // This effects is positive AND negative. Need for vehicles cast.
                    dualEffect |= (spellEffect->EffectImplicitTargetA == TARGET_DUELVSPLAYER
                                   || spellEffect->EffectImplicitTargetA == TARGET_IN_FRONT_OF_CASTER_30
                                   || spellEffect->EffectImplicitTargetA == TARGET_MASTER
                                   || spellEffect->EffectImplicitTargetA == TARGET_IN_FRONT_OF_CASTER
                                   || spellEffect->EffectImplicitTargetA == TARGET_EFFECT_SELECT
                                   || spellEffect->EffectImplicitTargetA == TARGET_CHAIN_DAMAGE
                                   || spellEffect->EffectImplicitTargetA == TARGET_CASTER_COORDINATES
                                   || spellEffect->EffectImplicitTargetA == TARGET_SELF2);
                }
                if (!m_caster->GetVehicleKit() && m_caster->IsFriendlyTo(_target) && !(!m_caster->GetCharmerOrOwner() || !m_caster->GetCharmerOrOwner()->IsFriendlyTo(_target))
                    && !dualEffect && !IsDispelSpell(m_spellInfo))
                {
                    DEBUG_LOG("Charmed creature attempt to cast spell %d, but target %s is not valid", m_spellInfo->Id, _target->GetGuidStr().c_str());
                    return SPELL_FAILED_BAD_TARGETS;
                }

                if (m_caster->GetObjectGuid() == _target->GetObjectGuid() && !dualEffect && !IsPositiveSpell(m_spellInfo->Id))
                {
                    DEBUG_LOG("Charmed creature %u attempt to cast negative spell %d on self",_target->GetObjectGuid().GetRawValue(),m_spellInfo->Id);
                    return SPELL_FAILED_BAD_TARGETS;
                }
            }
        }
                                                            //cooldown
        if(((Creature*)m_caster)->HasSpellCooldown(m_spellInfo->Id))
            return SPELL_FAILED_NOT_READY;
    }

    return CheckCast(true);
}

SpellCastResult Spell::CheckCasterAuras() const
{
    // Flag drop spells totally immuned to caster auras
    // FIXME: find more nice check for all totally immuned spells
    // HasAttribute(SPELL_ATTR_EX3_UNK28) ?
    if (m_spellInfo->Id == 23336 ||                         // Alliance Flag Drop
        m_spellInfo->Id == 23334 ||                         // Horde Flag Drop
        m_spellInfo->Id == 34991 || m_spellInfo->Id == 100201)  // Summon Netherstorm Flag
        return SPELL_CAST_OK;

    uint8 school_immune = 0;
    uint32 mechanic_immune = 0;
    uint32 dispel_immune = 0;

    // Check if the spell grants school or mechanic immunity.
    // We use bitmasks so the loop is done only once and not on every aura check below.
    if (m_spellInfo->HasAttribute(SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY))
    {
        for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
            if(!spellEffect)
                continue;
            if (spellEffect->EffectApplyAuraName == SPELL_AURA_SCHOOL_IMMUNITY)
                school_immune |= uint32(spellEffect->EffectMiscValue);
            else if (spellEffect->EffectApplyAuraName == SPELL_AURA_MECHANIC_IMMUNITY)
                mechanic_immune |= 1 << uint32(spellEffect->EffectMiscValue-1);
            else if (spellEffect->EffectApplyAuraName == SPELL_AURA_MECHANIC_IMMUNITY_MASK)
                mechanic_immune |= uint32(spellEffect->EffectMiscValue);
            else if (spellEffect->EffectApplyAuraName == SPELL_AURA_DISPEL_IMMUNITY)
                dispel_immune |= GetDispelMask(DispelType(spellEffect->EffectMiscValue));
        }
        // immune movement impairment and loss of control (spell data have special structure for mark this case)
        if (IsSpellRemoveAllMovementAndControlLossEffects(m_spellInfo))
            mechanic_immune = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;
    }

    // Check whether the cast should be prevented by any state you might have.
    SpellCastResult prevented_reason = SPELL_CAST_OK;
    // Have to check if there is a stun aura. Otherwise will have problems with ghost aura apply while logging out
    uint32 unitflag = m_caster->GetUInt32Value(UNIT_FIELD_FLAGS);       // Get unit state
    if (unitflag & UNIT_FLAG_STUNNED && (!m_spellInfo->HasAttribute(SPELL_ATTR_EX5_USABLE_WHILE_STUNNED)
         || (m_spellInfo->Id == 33206 || m_spellInfo->Id == 47788) && !m_caster->HasAura(63248)))     // Cast Pain Suppression and Guardian Spirit without Glyph of Desperation
        prevented_reason = SPELL_FAILED_STUNNED;
    else if (unitflag & UNIT_FLAG_CONFUSED && !m_spellInfo->HasAttribute(SPELL_ATTR_EX5_USABLE_WHILE_CONFUSED))
        prevented_reason = SPELL_FAILED_CONFUSED;
    else if (unitflag & UNIT_FLAG_FLEEING && !m_spellInfo->HasAttribute(SPELL_ATTR_EX5_USABLE_WHILE_FEARED))
        prevented_reason = SPELL_FAILED_FLEEING;
    else if (unitflag & UNIT_FLAG_SILENCED && m_spellInfo->GetPreventionType() == SPELL_PREVENTION_TYPE_SILENCE)
        prevented_reason = SPELL_FAILED_SILENCED;
    else if (unitflag & UNIT_FLAG_PACIFIED && m_spellInfo->GetPreventionType() == SPELL_PREVENTION_TYPE_PACIFY)
        prevented_reason = SPELL_FAILED_PACIFIED;

    // Attr must make flag drop spell totally immune from all effects
    if (prevented_reason != SPELL_CAST_OK)
    {
        if (school_immune || mechanic_immune || dispel_immune)
        {
            //Checking auras is needed now, because you are prevented by some state but the spell grants immunity.
            Unit::SpellAuraHolderMap const& auras = m_caster->GetSpellAuraHolderMap();
            for(Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            {
                SpellAuraHolder *holder = itr->second;
                SpellEntry const * pEntry = holder->GetSpellProto();

                if ((GetSpellSchoolMask(pEntry) & school_immune) && !pEntry->HasAttribute(SPELL_ATTR_EX_UNAFFECTED_BY_SCHOOL_IMMUNE))
                    continue;
                if ((1<<(pEntry->GetDispel())) & dispel_immune)
                    continue;

                for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    Aura *aura = holder->GetAuraByEffectIndex(SpellEffectIndex(i));
                    if (!aura)
                        continue;

                    if (GetSpellMechanicMask(pEntry, 1 << i) & mechanic_immune)
                        continue;
                    // Make a second check for spell failed so the right SPELL_FAILED message is returned.
                    // That is needed when your casting is prevented by multiple states and you are only immune to some of them.
                    switch(aura->GetModifier()->m_auraname)
                    {
                        case SPELL_AURA_MOD_STUN:
                            if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX5_USABLE_WHILE_STUNNED) || !aura->HasMechanic(MECHANIC_STUN))
                                return SPELL_FAILED_STUNNED;
                            break;
                        case SPELL_AURA_MOD_CONFUSE:
                            if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX5_USABLE_WHILE_CONFUSED))
                                return SPELL_FAILED_CONFUSED;
                            break;
                        case SPELL_AURA_MOD_FEAR:
                            if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX5_USABLE_WHILE_FEARED))
                                return SPELL_FAILED_FLEEING;
                            break;
                        case SPELL_AURA_MOD_SILENCE:
                        case SPELL_AURA_MOD_PACIFY:
                        case SPELL_AURA_MOD_PACIFY_SILENCE:
                            if( m_spellInfo->GetPreventionType() == SPELL_PREVENTION_TYPE_PACIFY)
                                return SPELL_FAILED_PACIFIED;
                            else if ( m_spellInfo->GetPreventionType() == SPELL_PREVENTION_TYPE_SILENCE)
                                return SPELL_FAILED_SILENCED;
                            break;
                        default: break;
                    }
                }
            }
        }
        // You are prevented from casting and the spell casted does not grant immunity. Return a failed error.
        else
            return prevented_reason;
    }
    return SPELL_CAST_OK;
}

bool Spell::CanAutoCast(Unit* target)
{
    ObjectGuid targetguid = target->GetObjectGuid();

    for(int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(j));
        if(!spellEffect)
            continue;
        if(spellEffect->Effect == SPELL_EFFECT_APPLY_AURA)
        {
            if( m_spellInfo->GetStackAmount() <= 1)
            {
                if( target->HasAura(m_spellInfo->Id, SpellEffectIndex(j)) )
                    return false;
            }
            else
            {
                if(Aura* aura = target->GetAura(m_spellInfo->Id, SpellEffectIndex(j)))
                    if(aura->GetStackAmount() >= m_spellInfo->GetStackAmount())
                        return false;
            }
        }
        else if ( IsAreaAuraEffect( spellEffect->Effect ))
        {
                if( target->HasAura(m_spellInfo->Id, SpellEffectIndex(j)) )
                    return false;
        }
    }

    SpellCastResult result = CheckPetCast(target);

    if(result == SPELL_CAST_OK || result == SPELL_FAILED_UNIT_NOT_INFRONT)
    {
        FillTargetMap();
        //check if among target units, our WANTED target is as well (->only self cast spells return false)
        for(TargetList::const_iterator ihit= m_UniqueTargetInfo.begin();ihit != m_UniqueTargetInfo.end();++ihit)
            if (ihit->targetGUID == targetguid)
                return true;
    }
    return false;                                           //target invalid
}

SpellCastResult Spell::CheckRange(bool strict)
{
    Unit* target = m_targets.getUnitTarget();

    DEBUG_LOG("Spell::CheckRange: id: %u rangeindex: %u, caster: %s, target: %s", m_spellInfo->Id, m_spellInfo->rangeIndex, m_caster->GetGuidStr().c_str(), target ? target->GetGuidStr().c_str() : "NULL");
    // special range cases
    switch(m_spellInfo->rangeIndex)
    {
        // self cast doesn't need range checking -- also for Starshards fix
        // spells that can be cast anywhere also need no check
        case SPELL_RANGE_IDX_SELF_ONLY:
        case SPELL_RANGE_IDX_ANYWHERE:
            return SPELL_CAST_OK;
        // combat range spells are treated differently
        //case SPELL_RANGE_IDX_COMBAT:
        //{
        //    if (target)
        //    {
        //        if (target == m_caster)
        //            return SPELL_CAST_OK;

        //        float range_mod = strict ? 1.25f : 6.25f;
        //        float base = ATTACK_DISTANCE;
        //        if (Player* modOwner = m_caster->GetSpellModOwner())
        //            range_mod += modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, base, this);

        //        // with additional 5 dist for non stricted case (some melee spells have delay in apply
        //        return m_caster->CanReachWithMeleeAttack(target, range_mod) ? SPELL_CAST_OK : SPELL_FAILED_OUT_OF_RANGE;
        //    }
        //    break;                                          // let continue in generic way for no target
        //}
    }

    //add radius of caster and ~5 yds "give" for non stricred (landing) check
    float range_mod = strict ? 1.25f : 6.25f;

    SpellRangeEntry const* srange = sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex);
    bool friendly = target ? target->IsFriendlyTo(m_caster) : false;
    float max_range = GetSpellMaxRange(srange, friendly) + range_mod;
    float min_range = GetSpellMinRange(srange, friendly);

    DEBUG_LOG("Spell::CheckRange: min: %f, max: %f, mod: %f", min_range, max_range, range_mod);

    if(Player* modOwner = m_caster->GetSpellModOwner())
        modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RANGE, max_range, this);

    DEBUG_LOG("Spell::CheckRange: max + spellmod: %f", max_range);

    if(target && target != m_caster)
    {
        // distance from target in checks
        float dist = m_caster->GetCombatDistance(target);
        DEBUG_LOG("Spell::CheckRange: combat distance: %f", dist);

        if(dist > max_range)
            return SPELL_FAILED_OUT_OF_RANGE;
        if(min_range && dist < min_range)
            return SPELL_FAILED_TOO_CLOSE;
        if( m_caster->GetTypeId() == TYPEID_PLAYER &&
            (m_spellInfo->GetFacingCasterFlags() & SPELL_FACING_FLAG_INFRONT) && !m_caster->HasInArc( M_PI_F, target ) )
            return SPELL_FAILED_UNIT_NOT_INFRONT;
    }

    // TODO verify that such spells really use bounding radius
    if(m_targets.m_targetMask == TARGET_FLAG_DEST_LOCATION && m_targets.m_destX != 0 && m_targets.m_destY != 0 && m_targets.m_destZ != 0)
    {
        if(!m_caster->IsWithinDist3d(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, max_range))
            return SPELL_FAILED_OUT_OF_RANGE;
        if(min_range && m_caster->IsWithinDist3d(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ, min_range))
            return SPELL_FAILED_TOO_CLOSE;
    }

    return SPELL_CAST_OK;
}

uint32 Spell::CalculatePowerCost(SpellEntry const* spellInfo, Unit* caster, Spell const* spell, Item* castItem)
{
    if (spell && spell->m_customSpellData.HasFlag(CUSTOM_SPELL_FLAG_NO_COST))
        return 0;

    // item cast not used power
    if (castItem)
        return 0;

    // Spell drain all exist power on cast (Only paladin lay of Hands)
    if (spellInfo->HasAttribute(SPELL_ATTR_EX_DRAIN_ALL_POWER))
    {
        // If power type - health drain all
        if (spellInfo->powerType == POWER_HEALTH)
            return caster->GetHealth();
        // Else drain all power
        if (spellInfo->powerType < MAX_POWERS)
            return caster->GetPower(Powers(spellInfo->powerType));
        ERROR_LOG("Spell::CalculateManaCost: Unknown power type '%d' in spell %d", spellInfo->powerType, spellInfo->Id);
        return 0;
    }

    // Base powerCost
    int32 powerCost = spellInfo->GetManaCost();
    // PCT cost from total amount
    if (uint32 manaCostPct = spellInfo->GetManaCostPercentage())
    {
        switch (spellInfo->powerType)
        {
            // health as power used
            case POWER_HEALTH:
                powerCost += manaCostPct * caster->GetCreateHealth() / 100;
                break;
            case POWER_MANA:
                powerCost += manaCostPct * caster->GetCreateMana() / 100;
                break;
            case POWER_RAGE:
            case POWER_FOCUS:
            case POWER_ENERGY:
                powerCost += manaCostPct * caster->GetMaxPower(Powers(spellInfo->powerType)) / 100;
                break;
            case POWER_RUNE:
            case POWER_RUNIC_POWER:
                DEBUG_LOG("Spell::CalculateManaCost: Not implemented yet!");
                break;
            default:
                ERROR_LOG("Spell::CalculateManaCost: Unknown power type '%d' in spell %d", spellInfo->powerType, spellInfo->Id);
                return 0;
        }
    }

    SpellSchoolMask schoolMask = spell ? spell->m_spellSchoolMask : GetSpellSchoolMask(spellInfo);
    // Flat mod from caster auras by spell school
    Unit::AuraList const& pwrCostAuras = caster->GetAurasByType(SPELL_AURA_MOD_POWER_COST_SCHOOL);
    for (Unit::AuraList::const_iterator itr = pwrCostAuras.begin(); itr != pwrCostAuras.end(); ++itr)
    {
        if (((*itr)->GetModifier()->m_miscvalue & schoolMask) &&
            (!(*itr)->GetSpellEffect()->EffectMiscValueB || (*itr)->GetSpellEffect()->EffectMiscValueB & (1 << spellInfo->powerType)))
            powerCost += (*itr)->GetModifier()->m_amount;
    }

    // Shiv - costs 20 + weaponSpeed*10 energy (apply only to non-triggered spell with energy cost)
    if (spellInfo->HasAttribute(SPELL_ATTR_EX4_SPELL_VS_EXTEND_COST))
        powerCost += caster->GetAttackTime(OFF_ATTACK) / 100;
    // Apply cost mod by spell
    if (spell)
        if (Player* modOwner = caster->GetSpellModOwner())
            modOwner->ApplySpellMod(spellInfo->Id, SPELLMOD_COST, powerCost, spell);

    if (spellInfo->HasAttribute(SPELL_ATTR_LEVEL_DAMAGE_CALCULATION))
        powerCost = int32(powerCost / (1.117f * spellInfo->GetSpellLevel() / caster->getLevel() - 0.1327f));

    // PCT mod from user auras by school
    float pctCostMultiplier = 1.0f;
    Unit::AuraList const& pwrCostPctAuras = caster->GetAurasByType(SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT);
    for (Unit::AuraList::const_iterator itr = pwrCostPctAuras.begin(); itr != pwrCostPctAuras.end(); ++itr)
    {
        if (((*itr)->GetModifier()->m_miscvalue & schoolMask) &&
            (!(*itr)->GetSpellEffect()->EffectMiscValueB || (*itr)->GetSpellEffect()->EffectMiscValueB & (1 << spellInfo->powerType)))
            pctCostMultiplier += (*itr)->GetModifier()->m_amount / 100.0f;
    }

    caster->wildHuntMarker = false;
    int32 powerCostMod = 0;
    if (spellInfo->powerType == POWER_FOCUS && caster->GetObjectGuid().IsPet() &&
        caster->GetPowerIndex(POWER_FOCUS) != INVALID_POWER_INDEX &&
        caster->GetPower(POWER_FOCUS) >= 50)
    {
        // search for "Wild Hunt"
        for (PetSpellMap::const_iterator itr = ((Pet*)caster)->m_spells.begin(); itr != ((Pet*)caster)->m_spells.end(); ++itr)
            if (itr->second.state != PETSPELL_REMOVED)
            {
                SpellEntry const* spellInfo = sSpellStore.LookupEntry(itr->first);
                if (!spellInfo || spellInfo->SpellIconID == 3748)
                    continue;
                caster->wildHuntMarker = true;
                pctCostMultiplier += spellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100;
                break;
            }
    }

    powerCost = int32(powerCost * pctCostMultiplier);
    if (powerCost < 0)
        powerCost = 0;

    return powerCost;
}

SpellCastResult Spell::CheckPower()
{
    // item cast not used power
    if (m_CastItem)
        return SPELL_CAST_OK;

    // health as power used - need check health amount
    if (m_spellInfo->powerType == POWER_HEALTH)
    {
        if (m_caster->GetHealth() <= m_powerCost)
            return SPELL_FAILED_CASTER_AURASTATE;
        return SPELL_CAST_OK;
    }

    // Check valid power type
    if (m_spellInfo->powerType >= MAX_POWERS)
    {
        ERROR_LOG("Spell::CheckMana: Unknown power type '%d'", m_spellInfo->powerType);
        return SPELL_FAILED_UNKNOWN;
    }

    //check rune cost only if a spell has PowerType == POWER_RUNE
    if (m_spellInfo->powerType == POWER_RUNE)
    {
        SpellCastResult failReason = CheckRunePower();
        if (failReason != SPELL_CAST_OK)
            return failReason;
    }

    // Check power amount
    Powers powerType = Powers(m_spellInfo->powerType);
    if((int32)m_caster->GetPower(powerType) < m_powerCost && !IsPassiveSpell(m_spellInfo->Id))
        return SPELL_FAILED_NO_POWER;

    return SPELL_CAST_OK;
}

bool Spell::IgnoreItemRequirements() const
{
    /// Check if it's an enchant scroll. These have no required reagents even though their spell does.
    if (m_CastItem && (m_CastItem->GetProto()->Flags & ITEM_FLAG_ENCHANT_SCROLL))
        return true;

    if (m_IsTriggeredSpell)
    {
        /// Not own traded item (in trader trade slot) req. reagents including triggered spell case
        if (Item* targetItem = m_targets.getItemTarget())
            if (targetItem->GetOwnerGuid() != m_caster->GetObjectGuid())
                return false;

        /// Some triggered spells have same reagents that have master spell
        /// expected in test: master spell have reagents in first slot then triggered don't must use own
        if (m_triggeredBySpellInfo)
        {
            SpellReagentsEntry const* spellReagents = m_triggeredBySpellInfo->GetSpellReagents();
            if (!spellReagents || !spellReagents->Reagent[0])
                return false;
        }

        return true;
    }

    return false;
}

SpellCastResult Spell::CheckItems()
{
    if (m_caster->GetTypeId() != TYPEID_PLAYER)
        return SPELL_CAST_OK;

    Player* p_caster = (Player*)m_caster;
    bool isScrollItem = false;
    bool isVellumTarget = false;

    // cast item checks
    if(m_CastItem)
    {
        if (m_CastItem->IsInTrade())
            return SPELL_FAILED_ITEM_NOT_FOUND;

        uint32 itemid = m_CastItem->GetEntry();
        if(!p_caster || !p_caster->HasItemCount(itemid, 1) )
            return SPELL_FAILED_ITEM_NOT_FOUND;

        ItemPrototype const *proto = m_CastItem->GetProto();
        if(!proto)
            return SPELL_FAILED_ITEM_NOT_FOUND;

        if (proto->Flags & ITEM_FLAG_ENCHANT_SCROLL)
            isScrollItem = true;

        for (int i = 0; i < 5; ++i)
            if (proto->Spells[i].SpellCharges)
                if(m_CastItem->GetSpellCharges(i) == 0)
                    return SPELL_FAILED_NO_CHARGES_REMAIN;

        // scroll target level checks
        if (proto->Class == ITEM_CLASS_CONSUMABLE && proto->SubClass == ITEM_SUBCLASS_SCROLL && m_targets.getUnitTarget())
        {
            if (proto->RequiredLevel > m_targets.getUnitTarget()->getLevel())
                return SPELL_FAILED_LOWLEVEL;
        }

        // Gnomish Army Knife
        if (m_spellInfo->Id == 54732)
        {
            if (p_caster->GetSkillValue(SKILL_ENGINEERING) < 350)
                return SPELL_FAILED_MIN_SKILL;
        }

        // consumable cast item checks
        if (proto->Class == ITEM_CLASS_CONSUMABLE && m_targets.getUnitTarget())
        {
            // such items should only fail if there is no suitable effect at all - see Rejuvenation Potions for example
            SpellCastResult failReason = SPELL_CAST_OK;
            for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
            {
                SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
                if(!spellEffect)
                    continue;
                // skip check, pet not required like checks, and for TARGET_PET m_targets.getUnitTarget() is not the real target but the caster
                if (spellEffect->EffectImplicitTargetA == TARGET_PET)
                    continue;

                if (spellEffect->Effect == SPELL_EFFECT_HEAL)
                {
                    if (m_targets.getUnitTarget()->GetHealth() == m_targets.getUnitTarget()->GetMaxHealth())
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_HEALTH;
                        continue;
                    }
                    else
                    {
                        failReason = SPELL_CAST_OK;
                        break;
                    }
                }

                // Mana Potion, Rage Potion, Thistle Tea(Rogue), ...
                if (spellEffect->Effect == SPELL_EFFECT_ENERGIZE)
                {
                    if(spellEffect->EffectMiscValue < 0 || spellEffect->EffectMiscValue >= MAX_POWERS)
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                        continue;
                    }

                    Powers power = Powers(spellEffect->EffectMiscValue);
                    if (m_targets.getUnitTarget()->GetPower(power) == m_targets.getUnitTarget()->GetMaxPower(power))
                    {
                        failReason = SPELL_FAILED_ALREADY_AT_FULL_POWER;
                        continue;
                    }
                    else
                    {
                        failReason = SPELL_CAST_OK;
                        break;
                    }
                }
            }
            if (failReason != SPELL_CAST_OK)
                return failReason;
        }
    }

    // check target item (for triggered case not report error)
    if (m_targets.getItemTargetGuid())
    {
        if (m_caster->GetTypeId() != TYPEID_PLAYER)
            return m_IsTriggeredSpell && !(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
                ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_BAD_TARGETS;

        if (!m_targets.getItemTarget())
            return m_IsTriggeredSpell  && !(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
                ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_ITEM_GONE;

        isVellumTarget = m_targets.getItemTarget()->GetProto()->IsVellum();
        if (!m_targets.getItemTarget()->IsFitToSpellRequirements(m_spellInfo))
            return m_IsTriggeredSpell  && !(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
                ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;

        // Do not enchant vellum with scroll
        if (isVellumTarget && isScrollItem)
            return m_IsTriggeredSpell  && !(m_targets.m_targetMask & TARGET_FLAG_TRADE_ITEM)
                ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_BAD_TARGETS;
    }
    // if not item target then required item must be equipped (for triggered case not report error)
    else
    {
        if(m_caster->GetTypeId() == TYPEID_PLAYER && !((Player*)m_caster)->HasItemFitToSpellReqirements(m_spellInfo) && m_customSpellData.HasFlag(CUSTOM_SPELL_FLAG_IGNORE_EQUIPPED_ITEM_REQ))
            return m_IsTriggeredSpell ? SPELL_FAILED_DONT_REPORT : SPELL_FAILED_EQUIPPED_ITEM_CLASS;
    }

    // check reagents (ignore triggered spells with reagents processed by original spell) and special reagent ignore case.
    if (!IgnoreItemRequirements())
    {
        if (!p_caster->CanNoReagentCast(m_spellInfo))
        {
            SpellReagentsEntry const* spellReagents = m_spellInfo->GetSpellReagents();
            if(spellReagents)
            {
                for(uint32 i = 0; i < MAX_SPELL_REAGENTS; ++i)
                {
                    if(spellReagents->Reagent[i] <= 0)
                        continue;

                    uint32 itemid    = spellReagents->Reagent[i];
                    uint32 itemcount = spellReagents->ReagentCount[i];

                    // if CastItem is also spell reagent
                    if (m_CastItem && m_CastItem->GetEntry() == itemid)
                    {
                        ItemPrototype const *proto = m_CastItem->GetProto();
                        if (!proto)
                            return SPELL_FAILED_REAGENTS;
                        for(int s = 0; s < MAX_ITEM_PROTO_SPELLS; ++s)
                        {
                            // CastItem will be used up and does not count as reagent
                            int32 charges = m_CastItem->GetSpellCharges(s);
                            if (proto->Spells[s].SpellCharges < 0 && !(proto->ExtraFlags & ITEM_EXTRA_NON_CONSUMABLE) && abs(charges) < 2)
                            {
                                ++itemcount;
                                break;
                            }
                        }
                    }

                    if (!p_caster->HasItemCount(itemid, itemcount))
                        return SPELL_FAILED_REAGENTS;
                }
            }
        }

        // check totem-item requirements (items presence in inventory)
        SpellTotemsEntry const* spellTotems = m_spellInfo->GetSpellTotems();
        if(spellTotems)
        {
            uint32 totems = MAX_SPELL_TOTEMS;
            for(int i = 0; i < MAX_SPELL_TOTEMS ; ++i)
            {
                if (spellTotems->Totem[i] != 0)
                {
                    if (p_caster->HasItemCount(spellTotems->Totem[i], 1))
                    {
                        totems -= 1;
                        continue;
                    }
                }
                else
                    totems -= 1;
            }

            if (totems != 0)
                return SPELL_FAILED_TOTEMS;
        }
    }

    // special checks for spell effects
    for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
        if(!spellEffect)
            continue;
        switch (spellEffect->Effect)
        {
            case SPELL_EFFECT_CREATE_ITEM:
            {
                if (!m_IsTriggeredSpell && spellEffect->EffectItemType)
                {
                    // Conjure Mana Gem (skip same or low level ranks for later recharge)
                    if (i == EFFECT_INDEX_0 && m_spellInfo->GetSpellEffectIdByIndex(EFFECT_INDEX_1) == SPELL_EFFECT_DUMMY)
                    {
                        if (ItemPrototype const* itemProto = ObjectMgr::GetItemPrototype(spellEffect->EffectItemType))
                        {
                            if (Item* item = p_caster->GetItemByLimitedCategory(itemProto->ItemLimitCategory))
                            {
                                if (item->GetProto()->ItemLevel <= itemProto->ItemLevel)
                                {
                                    if (item->HasMaxCharges())
                                        return SPELL_FAILED_ITEM_AT_MAX_CHARGES;

                                    // will recharge in next effect
                                    continue;
                                }
                            }
                        }
                    }

                    ItemPosCountVec dest;
                    InventoryResult msg = p_caster->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, spellEffect->EffectItemType, 1 );
                    if (msg != EQUIP_ERR_OK )
                    {
                        p_caster->SendEquipError( msg, NULL, NULL, spellEffect->EffectItemType );
                        return SPELL_FAILED_DONT_REPORT;
                    }
                }
                break;
            }
            case SPELL_EFFECT_RESTORE_ITEM_CHARGES:
            {
                if (Item* item = p_caster->GetItemByEntry(spellEffect->EffectItemType))
                    if (item->HasMaxCharges())
                        return SPELL_FAILED_ITEM_AT_MAX_CHARGES;

                break;
            }
            case SPELL_EFFECT_ENCHANT_ITEM:
            case SPELL_EFFECT_ENCHANT_ITEM_PRISMATIC:
            {
                Item* targetItem = m_targets.getItemTarget();
                if(!targetItem)
                    return SPELL_FAILED_ITEM_NOT_FOUND;

                if( targetItem->GetProto()->ItemLevel < m_spellInfo->GetBaseLevel() )
                    return SPELL_FAILED_LOWLEVEL;
                // Check if we can store a new scroll, enchanting vellum has implicit SPELL_EFFECT_CREATE_ITEM
                if (isVellumTarget && spellEffect->EffectItemType)
                {
                    ItemPosCountVec dest;
                    InventoryResult msg = p_caster->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, spellEffect->EffectItemType, 1 );
                    if (msg != EQUIP_ERR_OK)
                    {
                        p_caster->SendEquipError(msg, NULL, NULL);
                        return SPELL_FAILED_DONT_REPORT;
                    }
                }
                // Not allow enchant in trade slot for some enchant type
                if( targetItem->GetOwner() != m_caster )
                {
                    uint32 enchant_id = spellEffect->EffectMiscValue;
                    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if(!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->slot & ENCHANTMENT_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                    // cannot replace vellum with scroll in trade slot
                    if (isVellumTarget)
                        return SPELL_FAILED_BAD_TARGETS;
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY:
            {
                Item *item = m_targets.getItemTarget();
                if(!item)
                    return SPELL_FAILED_ITEM_NOT_FOUND;
                // Not allow enchant in trade slot for some enchant type
                if( item->GetOwner() != m_caster )
                {
                    uint32 enchant_id = spellEffect->EffectMiscValue;
                    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);
                    if(!pEnchant)
                        return SPELL_FAILED_ERROR;
                    if (pEnchant->slot & ENCHANTMENT_CAN_SOULBOUND)
                        return SPELL_FAILED_NOT_TRADEABLE;
                }
                break;
            }
            case SPELL_EFFECT_ENCHANT_HELD_ITEM:
                // check item existence in effect code (not output errors at offhand hold item effect to main hand for example
                break;
            case SPELL_EFFECT_DISENCHANT:
            {
                if(!m_targets.getItemTarget())
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                // prevent disenchanting in trade slot
                if( m_targets.getItemTarget()->GetOwnerGuid() != m_caster->GetObjectGuid() )
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                ItemPrototype const* itemProto = m_targets.getItemTarget()->GetProto();
                if(!itemProto)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                // must have disenchant loot (other static req. checked at item prototype loading)
                if (!itemProto->DisenchantID)
                    return SPELL_FAILED_CANT_BE_DISENCHANTED;

                // 2.0.x addon: Check player enchanting level against the item disenchanting requirements
                int32 item_disenchantskilllevel = itemProto->RequiredDisenchantSkill;
                if (item_disenchantskilllevel > int32(p_caster->GetSkillValue(SKILL_ENCHANTING)))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                break;
            }
            case SPELL_EFFECT_PROSPECTING:
            {
                if(!m_targets.getItemTarget())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                // ensure item is a prospectable ore
                if (!(m_targets.getItemTarget()->GetProto()->Flags & ITEM_FLAG_PROSPECTABLE))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                // prevent prospecting in trade slot
                if (m_targets.getItemTarget()->GetOwnerGuid() != m_caster->GetObjectGuid())
                    return SPELL_FAILED_CANT_BE_PROSPECTED;
                // Check for enough skill in jewelcrafting
                uint32 item_prospectingskilllevel = m_targets.getItemTarget()->GetProto()->RequiredSkillRank;
                if (item_prospectingskilllevel >p_caster->GetSkillValue(SKILL_JEWELCRAFTING))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                // make sure the player has the required ores in inventory
                if (int32(m_targets.getItemTarget()->GetCount()) < CalculateDamage(SpellEffectIndex(i), m_caster))
                    return SPELL_FAILED_NEED_MORE_ITEMS;

                if (!LootTemplates_Prospecting.HaveLootFor(m_targets.getItemTargetEntry()))
                    return SPELL_FAILED_CANT_BE_PROSPECTED;

                break;
            }
            case SPELL_EFFECT_MILLING:
            {
                if(!m_targets.getItemTarget())
                    return SPELL_FAILED_CANT_BE_MILLED;
                // ensure item is a millable herb
                if (!(m_targets.getItemTarget()->GetProto()->Flags & ITEM_FLAG_MILLABLE))
                    return SPELL_FAILED_CANT_BE_MILLED;
                // prevent milling in trade slot
                if (m_targets.getItemTarget()->GetOwnerGuid() != m_caster->GetObjectGuid())
                    return SPELL_FAILED_CANT_BE_MILLED;
                // Check for enough skill in inscription
                uint32 item_millingskilllevel = m_targets.getItemTarget()->GetProto()->RequiredSkillRank;
                if (item_millingskilllevel >p_caster->GetSkillValue(SKILL_INSCRIPTION))
                    return SPELL_FAILED_LOW_CASTLEVEL;
                // make sure the player has the required herbs in inventory
                if (int32(m_targets.getItemTarget()->GetCount()) < CalculateDamage(SpellEffectIndex(i), m_caster))
                    return SPELL_FAILED_NEED_MORE_ITEMS;

                if(!LootTemplates_Milling.HaveLootFor(m_targets.getItemTargetEntry()))
                    return SPELL_FAILED_CANT_BE_MILLED;

                break;
            }
            case SPELL_EFFECT_WEAPON_DAMAGE:
            case SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL:
            {
                if(m_caster->GetTypeId() != TYPEID_PLAYER) return SPELL_FAILED_TARGET_NOT_PLAYER;
                if( m_attackType != RANGED_ATTACK )
                    break;
                Item *pItem = ((Player*)m_caster)->GetWeaponForAttack(m_attackType,true,false);
                if (!pItem)
                    return SPELL_FAILED_EQUIPPED_ITEM;

                switch(pItem->GetProto()->SubClass)
                {
                    case ITEM_SUBCLASS_WEAPON_THROWN:
                    {
                        uint32 ammo = pItem->GetEntry();
                        if( !((Player*)m_caster)->HasItemCount( ammo, 1 ) )
                            return SPELL_FAILED_NO_AMMO;
                    };  break;
                    case ITEM_SUBCLASS_WEAPON_WAND:
                        break;
                    default:
                        break;
                }
                break;
            }
            default:break;
        }
    }

    return SPELL_CAST_OK;
}

void Spell::Delayed()
{
    if(!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (m_spellState == SPELL_STATE_DELAYED)
        return;                                             // spell is active and can't be time-backed

    if(isDelayableNoMore())                                 // Spells may only be delayed twice
        return;

    // spells not loosing casting time ( slam, dynamites, bombs.. )
    if(!(m_spellInfo->GetInterruptFlags() & SPELL_INTERRUPT_FLAG_DAMAGE))
        return;

    // check pushback reduce
    int32 delaytime = 500;                                  // spellcasting delay is normally 500ms
    int32 delayReduce = 100;                                // must be initialized to 100 for percent modifiers
    ((Player*)m_caster)->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, delayReduce, this);
    delayReduce += m_caster->GetTotalAuraModifier(SPELL_AURA_REDUCE_PUSHBACK) - 100;
    if(delayReduce >= 100)
        return;

    delaytime = delaytime * (100 - delayReduce) / 100;

    if(int32(m_timer) + delaytime > m_casttime)
    {
        delaytime = m_casttime - m_timer;
        m_timer = m_casttime;
    }
    else
        m_timer += delaytime;

    DETAIL_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell %u partially interrupted for (%d) ms at damage", m_spellInfo->Id, delaytime);

    WorldPacket data(SMSG_SPELL_DELAYED, 8+4);
    data << m_caster->GetPackGUID();
    data << uint32(delaytime);

    m_caster->SendMessageToSet(&data, true);
}

void Spell::DelayedChannel()
{
    if(!m_caster || m_caster->GetTypeId() != TYPEID_PLAYER || getState() != SPELL_STATE_CASTING)
        return;

    if(isDelayableNoMore())                                 // Spells may only be delayed twice
        return;

    // check pushback reduce
    int32 delaytime = GetSpellDuration(m_spellInfo) * 25 / 100;// channeling delay is normally 25% of its time per hit
    int32 delayReduce = 100;                                // must be initialized to 100 for percent modifiers
    ((Player*)m_caster)->ApplySpellMod(m_spellInfo->Id, SPELLMOD_NOT_LOSE_CASTING_TIME, delayReduce, this);
    delayReduce += m_caster->GetTotalAuraModifier(SPELL_AURA_REDUCE_PUSHBACK) - 100;
    if(delayReduce >= 100)
        return;

    delaytime = delaytime * (100 - delayReduce) / 100;

    if(int32(m_timer) < delaytime)
    {
        delaytime = m_timer;
        m_timer = 0;
    }
    else
        m_timer -= delaytime;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Spell %u partially interrupted for %i ms, new duration: %u ms", m_spellInfo->Id, delaytime, m_timer);

    for(TargetList::const_iterator ihit = m_UniqueTargetInfo.begin(); ihit != m_UniqueTargetInfo.end(); ++ihit)
    {
        if ((*ihit).missCondition == SPELL_MISS_NONE)
        {
            if (Unit* unit = m_caster->GetObjectGuid() == ihit->targetGUID ? m_caster : ObjectAccessor::GetUnit(*m_caster, ihit->targetGUID))
                unit->DelaySpellAuraHolder(m_spellInfo->Id, delaytime, unit->GetObjectGuid());
        }
    }

    for(int j = 0; j < MAX_EFFECT_INDEX; ++j)
    {
        // partially interrupt persistent area auras
        if (DynamicObject* dynObj = m_caster->GetDynObject(m_spellInfo->Id, SpellEffectIndex(j)))
            dynObj->Delay(delaytime);
    }

    SendChannelUpdate(m_timer);
}

void Spell::UpdateOriginalCasterPointer()
{
    if(m_originalCasterGUID == m_caster->GetObjectGuid())
        m_originalCaster = m_caster;
    else if (m_originalCasterGUID.IsGameObject())
    {
        GameObject* go = m_caster->IsInWorld() ? m_caster->GetMap()->GetGameObject(m_originalCasterGUID) : NULL;
        m_originalCaster = go ? go->GetOwner() : NULL;
    }
    else
    {
        Unit* unit = ObjectAccessor::GetUnit(*m_caster, m_originalCasterGUID);
        m_originalCaster = unit && unit->IsInWorld() ? unit : NULL;
    }
}

void Spell::UpdatePointers()
{
    UpdateOriginalCasterPointer();

    m_targets.Update(m_caster);
}

bool Spell::CheckTargetCreatureType(Unit* target, uint32 effMask) const
{
    uint32 spellCreatureTargetMask = m_spellInfo->GetTargetCreatureType();

    // Curse of Doom: not find another way to fix spell target check :/
    if (m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_WARLOCK && m_spellInfo->GetCategory() == 1179)
    {
        // not allow cast at player
        if(target->GetTypeId() == TYPEID_PLAYER)
            return false;

        spellCreatureTargetMask = 0x7FF;
    }
    // Holy Wrath
    else if (m_spellInfo->Id == 2812 && (effMask & (1 << EFFECT_INDEX_1)))
    {
        spellCreatureTargetMask = CREATURE_TYPEMASK_DEMON_OR_UNDEAD;
        // Glyph of Holy Wrath
        if (m_caster->HasAura(56420))
            spellCreatureTargetMask |= (1 << (CREATURE_TYPE_DRAGONKIN - 1)) | (1 << (CREATURE_TYPE_ELEMENTAL - 1));
    }
    // Death Coil
    else if (m_spellInfo->Id == 47541 && m_caster->IsFriendlyTo(target))
        spellCreatureTargetMask = 1 << (CREATURE_TYPE_UNDEAD - 1);

    // Dismiss Pet and Taming Lesson and Control Roskipped
    if (m_spellInfo->Id == 2641 || m_spellInfo->Id == 23356 || m_spellInfo->Id == 30009)
        spellCreatureTargetMask =  0;

    // skip creature type check for Grounding Totem
    if (target->GetUInt32Value(UNIT_CREATED_BY_SPELL) == 8177)
        return true;

    if (spellCreatureTargetMask)
    {
        uint32 TargetCreatureType = target->GetCreatureTypeMask();

        return !TargetCreatureType || (spellCreatureTargetMask & TargetCreatureType);
    }
    return true;
}

CurrentSpellTypes Spell::GetCurrentContainer()
{
    if (IsNextMeleeSwingSpell())
        return(CURRENT_MELEE_SPELL);
    else if (IsAutoRepeat())
        return(CURRENT_AUTOREPEAT_SPELL);
    else if (IsChanneledSpell(m_spellInfo))
        return(CURRENT_CHANNELED_SPELL);
    else
        return(CURRENT_GENERIC_SPELL);
}

bool Spell::CheckTarget(Unit* target, SpellEffectIndex eff)
{
    SpellEffectEntry const* spellEffect = m_spellInfo->GetSpellEffect(eff);
    if(!spellEffect)
        return false;

    // Check targets for creature type mask and remove not appropriate (skip explicit self target case, maybe need other explicit targets)
    // don't skip Wintergrasp Water
    if (spellEffect->EffectImplicitTargetA != TARGET_SELF || m_spellInfo->Id == 36444)
    {
        if (!CheckTargetCreatureType(target, 1 << eff))
            return false;
    }

    // Check Aura spell req (need for AoE spells)
    if (spellEffect->Effect != SPELL_EFFECT_KILL_CREDIT_PERSONAL && spellEffect->Effect != SPELL_EFFECT_KILL_CREDIT_GROUP)
    {
        SpellAuraRestrictionsEntry const* auraRestrictions = m_spellInfo->GetSpellAuraRestrictions();
        if(auraRestrictions)
        {
            if(auraRestrictions->targetAuraSpell && !target->HasAura(auraRestrictions->targetAuraSpell))
                return false;
            if (auraRestrictions->excludeTargetAuraSpell && target->HasAura(auraRestrictions->excludeTargetAuraSpell))
                return false;
        }
    }

    // Check targets for not_selectable unit flag and remove
    // A player can cast spells on his pet (or other controlled unit) though in any state
    if (target != m_caster && target->GetCharmerOrOwnerGuid() != m_caster->GetObjectGuid())
    {
        // any unattackable target skipped
        if (target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE) && target->GetObjectGuid() != m_caster->GetCharmerOrOwnerGuid())
            return false;

        // unselectable targets skipped in all cases except TARGET_SCRIPT targeting
        // in case TARGET_SCRIPT target selected by server always and can't be cheated
        if ((!m_IsTriggeredSpell || target != m_targets.getUnitTarget()) &&
            target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE) &&
            spellEffect->EffectImplicitTargetA != TARGET_SCRIPT &&
            spellEffect->EffectImplicitTargetB != TARGET_SCRIPT &&
            spellEffect->EffectImplicitTargetA != TARGET_AREAEFFECT_INSTANT &&
            spellEffect->EffectImplicitTargetB != TARGET_AREAEFFECT_INSTANT &&
            spellEffect->EffectImplicitTargetA != TARGET_AREAEFFECT_CUSTOM &&
            spellEffect->EffectImplicitTargetB != TARGET_AREAEFFECT_CUSTOM )
            return false;
    }

    if (target != m_caster && m_caster->GetCharmerOrOwnerGuid() == target->GetObjectGuid())
    {
        if (spellEffect->EffectImplicitTargetA == TARGET_MASTER ||
            spellEffect->EffectImplicitTargetB == TARGET_MASTER)
            return true;
    }

    // Check player targets and remove if in GM mode or GM invisibility (for not self casting case)
    if( target != m_caster && target->GetTypeId() == TYPEID_PLAYER)
    {
        if(((Player*)target)->GetVisibility() == VISIBILITY_OFF)
            return false;

        if(((Player*)target)->isGameMaster() && !IsPositiveSpell(m_spellInfo->Id))
            return false;
    }

    // Check sated debuffs
    // Bloodlust, Heroism, Time Warp
    if ((m_spellInfo->Id == 2825 || m_spellInfo->Id == 32182 || m_spellInfo->Id == 80353) &&
        target->HasSatedAura())
        return false;

    // Check vampiric bite
    if (m_spellInfo->Id == 70946 && target->HasAura(70867))
        return false;

    // Sindragosa frost bomb hack
    if ((m_spellInfo->Id == 69845 || m_spellInfo->Id == 71053
        || m_spellInfo->Id == 71054 || m_spellInfo->Id == 71055)
         && target->HasAura(70867))
        return false;

    // Starfall does not hit invisible targets
    if (m_spellInfo->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x80000000000000), 0x00000100)
        && !target->isVisibleForOrDetect(m_caster, m_caster, false))
        return false;

    // Foam Sword Attack & Bonked!
    if (m_spellInfo->Id == 62973 || m_spellInfo->Id == 62991)
    {
        if (!target->HasAura(62972))
            return false;

        if (SpellAuraHolder* holder = target->GetSpellAuraHolder(62991, m_caster->GetObjectGuid()))
            if (holder->GetStackAmount() == m_spellInfo->GetStackAmount())
                return false;
    }

    if (spellEffect->EffectImplicitTargetA == TARGET_ALL_RAID_AROUND_CASTER ||
        spellEffect->EffectImplicitTargetB == TARGET_ALL_RAID_AROUND_CASTER)
        return true;

    if (target->GetTypeId() != TYPEID_PLAYER && m_spellInfo->HasAttribute(SPELL_ATTR_EX3_TARGET_ONLY_PLAYER)
        && spellEffect->EffectImplicitTargetA != TARGET_SCRIPT && spellEffect->EffectImplicitTargetA != TARGET_SELF)
        return false;

    switch (m_spellInfo->Id)
    {
        // Ranger: check proc target (hacky, wrong dbc or core logic?)
        // on self
        case 60544:     // Savage Aggression
        case 60547:     // Hateful Aggression
        case 60549:     // Deadly Aggression
        case 60551:     // Furious Gladiator's Libram of Fortitude
        case 60553:     // Relentless Aggression
        case 60555:     // LK Arena 6 Aggression
        case 60565:     // Savage Magic
        case 60566:     // Hateful Magic
        case 60567:     // Deadly Magic
        case 60568:     // Furious Gladiator's Idol of Steadfastness
        case 60569:     // Relentless Survival
        case 60570:     // LK Arena 6 Magic
            if (spellEffect->EffectImplicitTargetA != TARGET_SELF)
                return false;
            break;
        // on victim
        case 45524:     // Chains of Ice
            if (spellEffect->EffectImplicitTargetA == TARGET_SELF)
                return false;
            break;

        case 37433:     // Spout (The Lurker Below), only players affected if its not in water
            if (target->GetTypeId() != TYPEID_PLAYER || target->IsInWater())
                return false;
            break;

        case 82301:     // Burning Heart
            if (target->GetEntry() != 40004)
                return false;
            break;
        case 103762:    // Engulfing Twilight
            if (target->GetCharmerOrOwnerOrOwnGuid().IsPlayer())
                return false;
            break;
        case 105739:    // Mass Regeneration
            // Mass Regeneration (Bear Form)
            if (!target->HasAura(105737))
                return false;
            break;
    }

    if (IsSpellHaveEffect(m_spellInfo, SPELL_EFFECT_HEAL) && target->GetObjectGuid().IsVehicle() && target->GetCreatureType() == CREATURE_TYPE_MECHANICAL)
        return false;

    // Check targets for LOS visibility (except spells without range limitations )
    switch(spellEffect->Effect)
    {
        case SPELL_EFFECT_SUMMON_PLAYER:                    // from anywhere
        case SPELL_EFFECT_FRIEND_SUMMON:
            break;
        case SPELL_EFFECT_DUMMY:
            if(m_spellInfo->Id != 20577)                    // Cannibalize
                break;
            // fall through
        case SPELL_EFFECT_RESURRECT_NEW:
            // player far away, maybe his corpse near?
            if (target != m_caster && !m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && !target->IsWithinLOSInMap(m_caster))
            {
                if (!m_targets.getCorpseTargetGuid())
                    return false;

                Corpse *corpse = m_caster->GetMap()->GetCorpse(m_targets.getCorpseTargetGuid());
                if(!corpse)
                    return false;

                if(target->GetObjectGuid() != corpse->GetOwnerGuid())
                    return false;

                if (!m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS) && !corpse->IsWithinLOSInMap(m_caster))
                    return false;
            }

            // all ok by some way or another, skip normal check
            break;
        default:                                            // normal case
        {
            // Get GO cast coordinates if original caster -> GO
            if (target != m_caster && !m_spellInfo->HasAttribute(SPELL_ATTR_EX2_IGNORE_LOS))
            {
                if (WorldObject* caster = GetCastingObject())
                {
                    // Exceptions
                    bool checkDest = false;
                    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
                    {
                        SpellEffectEntry const * effect = m_spellInfo->GetSpellEffect(SpellEffectIndex(i));
                        if (!effect)
                            continue;

                        if ((m_targets.m_targetMask & TARGET_FLAG_DEST_LOCATION) &&
                            (effect->EffectImplicitTargetA == TARGET_ALL_ENEMY_IN_AREA_INSTANT ||
                            effect->EffectImplicitTargetA == TARGET_ALL_ENEMY_IN_AREA_CHANNELED ||
                            effect->EffectImplicitTargetA == TARGET_DYNAMIC_OBJECT_COORDINATES))
                        {
                            checkDest = true;
                            break;
                        }
                    }

                    bool needBreak = false;
                    switch (m_spellInfo->Id)
                    {
                        case 8146:          // Tremor Totem Effect
                        case 52025:         // Cleansing Totem Effect
                        case 52752:         // Ancestral Awakening heal
                        case 52759:         // Ancestral Awakening dummy
                        case 64844:         // Divine Hymn
                        case 64904:         // Hymn of Hope
                            needBreak = true;
                            break;
                    }

                    if (spellEffect->Effect == SPELL_EFFECT_WMO_DAMAGE)
                        break;

                    if (needBreak)
                        break;

                    if (checkDest)
                    {
                        if (!target->IsInMap(caster) || !target->IsWithinLOS(m_targets.m_destX, m_targets.m_destY, m_targets.m_destZ))
                            return false;
                    }
                    else if (!target->IsWithinLOSInMap(caster))
                        return false;
                }
            }
            break;
        }
    }

    return true;
}

bool Spell::IsNeedSendToClient() const
{
    return m_spellInfo->SpellVisual[0] || m_spellInfo->SpellVisual[1] || IsChanneledSpell(m_spellInfo) ||
        m_spellInfo->speed > 0.0f || !m_triggeredByAuraSpell && !m_IsTriggeredSpell ||
        (m_spellInfo->AttributesEx8 & SPELL_ATTR_EX8_AURA_SENDS_AMOUNT);
}

bool Spell::IsTriggeredSpellWithRedundentData() const
{
    return m_triggeredByAuraSpell || m_triggeredBySpellInfo ||
        // possible not need after above check?
        m_IsTriggeredSpell && (m_spellInfo->GetManaCost() || m_spellInfo->GetManaCostPercentage());
}

bool Spell::IsTriggeredSpell() const
{
    return m_triggeredByAuraSpell || m_triggeredBySpellInfo || m_IsTriggeredSpell;
}

bool Spell::HaveTargetsForEffect(SpellEffectIndex effect) const
{
    for(TargetList::const_iterator itr = m_UniqueTargetInfo.begin(); itr != m_UniqueTargetInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    for(GOTargetList::const_iterator itr = m_UniqueGOTargetInfo.begin(); itr != m_UniqueGOTargetInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    for(ItemTargetList::const_iterator itr = m_UniqueItemInfo.begin(); itr != m_UniqueItemInfo.end(); ++itr)
        if (itr->effectMask & (1 << effect))
            return true;

    return false;
}

SpellEvent::SpellEvent(Spell* spell) : BasicEvent()
{
    m_Spell = spell;
}

SpellEvent::~SpellEvent()
{
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();

    if (m_Spell->IsDeletable())
    {
        delete m_Spell;
    }
    else
    {
        sLog.outError("~SpellEvent: %s tried to delete non-deletable spell %u. Was not deleted, causes memory leak.", m_Spell->GetCaster()->GetGuidStr().c_str(), m_Spell->m_spellInfo->Id);
    }
}

bool SpellEvent::Execute(uint64 e_time, uint32 p_time)
{
    // update spell if it is not finished
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->update(p_time);

    // check spell state to process
    switch (m_Spell->getState())
    {
        case SPELL_STATE_FINISHED:
        {
            // spell was finished, check deletable state
            if (m_Spell->IsDeletable())
            {
                // check, if we do have unfinished triggered spells
                return true;                                // spell is deletable, finish event
            }
            // event will be re-added automatically at the end of routine)
        } break;

        case SPELL_STATE_CASTING:
        {
            // this spell is in channeled state, process it on the next update
            // event will be re-added automatically at the end of routine)

            // Ranger: channeled spell with negative duration (-1)
            //         temp!
            if (m_Spell->GetActiveDuration() < 0)
            {
                return false;
            }

        } break;

        case SPELL_STATE_DELAYED:
        {
            // first, check, if we have just started
            if (m_Spell->GetDelayStart() != 0)
            {
                // no, we aren't, do the typical update
                // check, if we have channeled spell on our hands
                if (IsChanneledSpell(m_Spell->m_spellInfo))
                {
                    // evented channeled spell is processed separately, casted once after delay, and not destroyed till finish
                    // check, if we have casting anything else except this channeled spell and autorepeat
                    if (m_Spell->GetCaster()->IsNonMeleeSpellCasted(false, true, true))
                    {
                        // another non-melee non-delayed spell is casted now, abort
                        m_Spell->cancel();
                    }
                    else
                    {
                        // do the action (pass spell to channeling state)
                        m_Spell->handle_immediate();
                    }
                    // event will be re-added automatically at the end of routine)
                }
                else
                {
                    // run the spell handler and think about what we can do next
                    uint64 t_offset = e_time - m_Spell->GetDelayStart();
                    uint64 n_offset = m_Spell->handle_delayed(t_offset);
                    if (n_offset)
                    {
                        // re-add us to the queue
                        m_Spell->GetCaster()->m_Events.AddEvent(this, m_Spell->GetDelayStart() + n_offset, false);
                        return false;                       // event not complete
                    }
                    // event complete
                    // finish update event will be re-added automatically at the end of routine)
                }
            }
            else
            {
                // delaying had just started, record the moment
                m_Spell->SetDelayStart(e_time);
                // re-plan the event for the delay moment
                m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + m_Spell->GetDelayMoment(), false);
                return false;                               // event not complete
            }
        } break;

        default:
        {
            // all other states
            // event will be re-added automatically at the end of routine)
        } break;
    }

    // spell processing not complete, plan event on the next update interval
    m_Spell->GetCaster()->m_Events.AddEvent(this, e_time + 1, false);
    return false;                                           // event not complete
}

void SpellEvent::Abort(uint64 /*e_time*/)
{
    // oops, the spell we try to do is aborted
    if (m_Spell->getState() != SPELL_STATE_FINISHED)
        m_Spell->cancel();
}

bool SpellEvent::IsDeletable() const
{
    return m_Spell->IsDeletable();
}

SpellCastResult Spell::CanOpenLock(SpellEffectIndex effIndex, uint32 lockId, SkillType& skillId, int32& reqSkillValue, int32& skillValue)
{
    if(!lockId)                                             // possible case for GO and maybe for items.
        return SPELL_CAST_OK;

    // Get LockInfo
    LockEntry const *lockInfo = sLockStore.LookupEntry(lockId);

    if (!lockInfo)
        return SPELL_FAILED_BAD_TARGETS;

    bool reqKey = false;                                    // some locks not have reqs

    for(int j = 0; j < 8; ++j)
    {
        switch(lockInfo->Type[j])
        {
            // check key item (many fit cases can be)
            case LOCK_KEY_ITEM:
                if(lockInfo->Index[j] && m_CastItem && m_CastItem->GetEntry()==lockInfo->Index[j])
                    return SPELL_CAST_OK;
                reqKey = true;
                break;
                // check key skill (only single first fit case can be)
            case LOCK_KEY_SKILL:
            {
                reqKey = true;

                // wrong locktype, skip
                if(uint32(m_spellInfo->GetEffectMiscValue(effIndex)) != lockInfo->Index[j])
                    continue;

                skillId = SkillByLockType(LockType(lockInfo->Index[j]));

                if (skillId != SKILL_NONE || skillId == MAX_SKILL_TYPE)
                {
                    // skill bonus provided by casting spell (mostly item spells)
                    // add the damage modifier from the spell casted (cheat lock / skeleton key etc.) (use m_currentBasePoints, CalculateDamage returns wrong value)
                    uint32 spellSkillBonus = uint32(m_currentBasePoints[effIndex]);
                    reqSkillValue = lockInfo->Skill[j];

                    // castitem check: rogue using skeleton keys. the skill values should not be added in this case.
                    // MAX_SKILL_TYPE - skill value scales with caster level
                    if (skillId == MAX_SKILL_TYPE)
                        skillValue = m_CastItem || m_caster->GetTypeId() != TYPEID_PLAYER ? 0 : m_caster->getLevel() * 5;
                    else
                        skillValue = m_CastItem || m_caster->GetTypeId() != TYPEID_PLAYER ? 0 : ((Player*)m_caster)->GetSkillValue(skillId);

                    skillValue += spellSkillBonus;

                    if (skillValue < reqSkillValue)
                        return SPELL_FAILED_LOW_CASTLEVEL;
                }

                return SPELL_CAST_OK;
            }
        }
    }

    if (reqKey)
        return SPELL_FAILED_BAD_TARGETS;

    return SPELL_CAST_OK;
}

/**
 * Fill target list by units around (x,y) points at radius distance

 * @param targetUnitMap        Reference to target list that filled by function
 * @param x                    X coordinates of center point for target search
 * @param y                    Y coordinates of center point for target search
 * @param radius               Radius around (x,y) for target search
 * @param pushType             Additional rules for target area selection (in front, angle, etc)
 * @param spellTargets         Additional rules for target selection base at hostile/friendly state to original spell caster
 * @param originalCaster       If provided set alternative original caster, if =NULL then used Spell::GetAffectiveObject() return
 */
void Spell::FillAreaTargets(UnitList &targetUnitMap, float radius, SpellNotifyPushType pushType, SpellTargets spellTargets, WorldObject* originalCaster /*=NULL*/)
{
    MaNGOS::SpellNotifierCreatureAndPlayer notifier(*this, targetUnitMap, radius, pushType, spellTargets, originalCaster);
    Cell::VisitAllObjects(notifier.GetCenterX(), notifier.GetCenterY(), m_caster->GetMap(), notifier, radius);
}

void Spell::FillRaidOrPartyTargets(UnitList &targetUnitMap, Unit* member, Unit* center, float radius, bool raid, bool withPets, bool withcaster)
{
    Player *pMember = member->GetCharmerOrOwnerPlayerOrPlayerItself();
    Group *pGroup = pMember ? pMember->GetGroup() : NULL;

    if (pGroup)
    {
        uint8 subgroup = pMember->GetSubGroup();

        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* Target = itr->getSource();

            // IsHostileTo check duel and controlled by enemy
            if (Target && (raid || subgroup==Target->GetSubGroup())
                && !m_caster->IsHostileTo(Target))
            {
                if ((Target == center || center->IsWithinDistInMap(Target, radius)) &&
                    (withcaster || Target != m_caster))
                    targetUnitMap.push_back(Target);

                if (withPets)
                    if (Pet* pet = Target->GetPet())
                        if ((pet == center || center->IsWithinDistInMap(pet, radius)) &&
                            (withcaster || pet != m_caster))
                            targetUnitMap.push_back(pet);
            }
        }
    }
    else
    {
        Unit* ownerOrSelf = pMember ? pMember : member->GetCharmerOrOwnerOrSelf();
        if ((ownerOrSelf == center || center->IsWithinDistInMap(ownerOrSelf, radius)) &&
            (withcaster || ownerOrSelf != m_caster))
            targetUnitMap.push_back(ownerOrSelf);

        if (withPets)
            if (Pet* pet = ownerOrSelf->GetPet())
                if ((pet == center || center->IsWithinDistInMap(pet, radius)) &&
                    (withcaster || pet != m_caster))
                    targetUnitMap.push_back(pet);
    }
}

void Spell::FillRaidOrPartyManaPriorityTargets(UnitList &targetUnitMap, Unit* member, Unit* center, float radius, uint32 count, bool raid, bool withPets, bool withCaster)
{
    FillRaidOrPartyTargets(targetUnitMap, member, center, radius, raid, withPets, withCaster);

    PrioritizeManaUnitQueue manaUsers;
    for(UnitList::const_iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr)
        if ((*itr)->getPowerType() == POWER_MANA && !(*itr)->isDead())
            manaUsers.push(PrioritizeManaUnitWraper(*itr));

    targetUnitMap.clear();
    while(!manaUsers.empty() && targetUnitMap.size() < count)
    {
        targetUnitMap.push_back(manaUsers.top().getUnit());
        manaUsers.pop();
    }
}

void Spell::FillRaidOrPartyHealthPriorityTargets(UnitList &targetUnitMap, Unit* member, Unit* center, float radius, uint32 count, bool raid, bool withPets, bool withCaster)
{
    FillRaidOrPartyTargets(targetUnitMap, member, center, radius, raid, withPets, withCaster);

    PrioritizeHealthUnitQueue healthQueue;
    for(UnitList::const_iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr)
        if (!(*itr)->isDead())
            healthQueue.push(PrioritizeHealthUnitWraper(*itr));

    targetUnitMap.clear();
    while(!healthQueue.empty() && targetUnitMap.size() < count)
    {
        targetUnitMap.push_back(healthQueue.top().getUnit());
        healthQueue.pop();
    }
}

WorldObject* Spell::GetAffectiveCasterObject() const
{
    if (!m_originalCasterGUID)
        return m_caster;

    if (m_originalCasterGUID.IsGameObject() && m_caster->IsInWorld())
        return m_caster->GetMap()->GetGameObject(m_originalCasterGUID);
    return m_originalCaster;
}

WorldObject* Spell::GetCastingObject() const
{
    if (m_originalCasterGUID.IsGameObject())
        return m_caster->IsInWorld() ? m_caster->GetMap()->GetGameObject(m_originalCasterGUID) : NULL;
    else
        return m_caster;
}

void Spell::ResetEffectDamageAndHeal()
{
    m_damage = 0;
    m_healing = 0;
}

void Spell::SelectMountByAreaAndSkill(Unit* target, SpellEntry const* parentSpell, uint32 spellId75, uint32 spellId150, uint32 spellId225, uint32 spellId300, uint32 spellIdSpecial)
{
    if (!target || target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Prevent stacking of mounts
    target->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    uint16 skillval = ((Player*)target)->GetSkillValue(SKILL_RIDING);
    if (!skillval)
        return;

    if (skillval >= 225 && (spellId300 > 0 || spellId225 > 0))
    {
        uint32 spellid = skillval >= 300 ? spellId300 : spellId225;
        SpellEntry const *pSpell = sSpellStore.LookupEntry(spellid);
        if (!pSpell)
        {
            sLog.outError("SelectMountByAreaAndSkill: unknown spell id %i by caster: %s", spellid, target->GetGuidStr().c_str());
            return;
        }

        // zone check
        uint32 zone, area;
        target->GetZoneAndAreaId(zone, area);

        SpellCastResult locRes= sSpellMgr.GetSpellAllowedInLocationError(pSpell, target->GetMapId(), zone, area, target->GetCharmerOrOwnerPlayerOrPlayerItself());
        if (locRes != SPELL_CAST_OK || !((Player*)target)->CanStartFlyInArea(target->GetMapId(), zone, area))
            target->CastSpell(target, spellId150, true, NULL, NULL, ObjectGuid(), parentSpell);
        else if (spellIdSpecial > 0)
        {
            for (PlayerSpellMap::const_iterator iter = ((Player*)target)->GetSpellMap().begin(); iter != ((Player*)target)->GetSpellMap().end(); ++iter)
            {
                if (iter->second.state != PLAYERSPELL_REMOVED)
                {
                    SpellEntry const *spellInfo = sSpellStore.LookupEntry(iter->first);
                    for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
                    {
                        SpellEffectEntry const* spellEffect = spellInfo->GetSpellEffect(SpellEffectIndex(i));
                        if(!spellEffect)
                            continue;

                        if(spellEffect->EffectApplyAuraName == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
                        {
                            int32 mountSpeed = spellInfo->CalculateSimpleValue(SpellEffectIndex(i));

                            // speed higher than 280 replace it
                            if (mountSpeed > 280)
                            {
                                target->CastSpell(target, spellIdSpecial, true, NULL, NULL, ObjectGuid(), parentSpell);
                                return;
                            }
                        }
                    }
                }
            }
            target->CastSpell(target, pSpell, true, NULL, NULL, ObjectGuid(), parentSpell);
        }
        else
            target->CastSpell(target, pSpell, true, NULL, NULL, ObjectGuid(), parentSpell);
    }
    else if (skillval >= 150 && spellId150 > 0)
        target->CastSpell(target, spellId150, true, NULL, NULL, ObjectGuid(), parentSpell);
    else if (spellId75 > 0)
        target->CastSpell(target, spellId75, true, NULL, NULL, ObjectGuid(), parentSpell);

    return;
}

void Spell::ClearCastItem()
{
    if (m_CastItem==m_targets.getItemTarget())
        m_targets.setItemTarget(NULL);

    m_CastItem = NULL;
}

bool Spell::HasGlobalCooldown()
{
    // global cooldown have only player or controlled units
    if (m_caster->GetCharmInfo())
        return m_caster->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(m_spellInfo);
    else if (m_caster->GetTypeId() == TYPEID_PLAYER)
        return ((Player*)m_caster)->GetGlobalCooldownMgr().HasGlobalCooldown(m_spellInfo);
    else
        return false;
}

void Spell::TriggerGlobalCooldown()
{
    int32 gcd = m_spellInfo->GetStartRecoveryTime();
    if (!gcd)
        return;

    // global cooldown can't leave range 1..1.5 secs (if it it)
    // exist some spells (mostly not player directly casted) that have < 1 sec and > 1.5 sec global cooldowns
    // but its as test show not affected any spell mods.
    if (gcd >= 1000 && gcd <= 1500)
    {
        // gcd modifier auras applied only to self spells and only player have mods for this
        if (m_caster->GetTypeId() == TYPEID_PLAYER)
            ((Player*)m_caster)->ApplySpellMod(m_spellInfo->Id, SPELLMOD_GLOBAL_COOLDOWN, gcd, this);

        // apply haste rating
        gcd = int32(float(gcd) * m_caster->GetFloatValue(UNIT_MOD_CAST_SPEED));

        if (gcd < 1000)
            gcd = 1000;
        else if (gcd > 1500)
            gcd = 1500;
    }

    // global cooldown have only player or controlled units
    if (m_caster->GetCharmInfo())
        m_caster->GetCharmInfo()->GetGlobalCooldownMgr().AddGlobalCooldown(m_spellInfo, gcd);
    else if (m_caster->GetTypeId() == TYPEID_PLAYER)
        ((Player*)m_caster)->GetGlobalCooldownMgr().AddGlobalCooldown(m_spellInfo, gcd);
}

void Spell::CancelGlobalCooldown()
{
    if (!m_spellInfo->GetStartRecoveryTime())
        return;

    // cancel global cooldown when interrupting current cast
    if (m_caster->GetCurrentSpell(CURRENT_GENERIC_SPELL) != this)
        return;

    // global cooldown have only player or controlled units
    if (m_caster->GetCharmInfo())
        m_caster->GetCharmInfo()->GetGlobalCooldownMgr().CancelGlobalCooldown(m_spellInfo);
    else if (m_caster->GetTypeId() == TYPEID_PLAYER)
        ((Player*)m_caster)->GetGlobalCooldownMgr().CancelGlobalCooldown(m_spellInfo);
}

bool Spell::FillCustomTargetMap(SpellEffectIndex effIndex, UnitList &targetUnitMap)
{
    float radius;
    SpellEffectEntry const * spellEffect = m_spellInfo->GetSpellEffect(effIndex);
    if (!spellEffect)
        return false;

    if (uint32 radiusIndex = spellEffect->GetRadiusIndex())
        radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(radiusIndex));
    else
        radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));

    uint32 EffectChainTarget = spellEffect->EffectChainTarget;

    if (Unit* realCaster = GetAffectiveCaster())
    {
        if(Player* modOwner = realCaster->GetSpellModOwner())
        {
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_RADIUS, radius, this);
            modOwner->ApplySpellMod(m_spellInfo->Id, SPELLMOD_JUMP_TARGETS, EffectChainTarget, this);
        }
    }

    // Resulting effect depends on spell that we want to cast
    switch (m_spellInfo->Id)
    {
        case 12355: // Impact
        {
            if (effIndex != EFFECT_INDEX_1)
                return false;

            Unit* currentTarget = m_targets.getUnitTarget();
            if (!currentTarget)
                return true;

            // Ignite, Pyroblast, Living Bomb, Combustion
            uint32 spellIds[4] = { 12654, 11366, 44457, 83853 };
            SpellAuraHolder* dot = NULL;
            for (int i = 0; i < 4 && !dot; ++i)
                dot = currentTarget->GetSpellAuraHolder(spellIds[i], m_caster->GetObjectGuid());

            if (dot)
            {
                m_targets.setDestination(currentTarget->GetPositionX(), currentTarget->GetPositionY(), currentTarget->GetPositionZ());

                Spell::UnitList tempTargetUnitMap;
                FillAreaTargets(tempTargetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
                for (Spell::UnitList::iterator itr = tempTargetUnitMap.begin(); itr != tempTargetUnitMap.end(); ++itr)
                {
                    if (!(*itr)->CanFreeMove())
                        continue;

                    targetUnitMap.push_back(*itr);
                }
            }

            targetUnitMap.remove(currentTarget);
            return true;
        }
        case 19185: // Entrapment
        case 64803:
        {
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            break;
        }
        case 28374: // Decimate - Gluth encounter
        {
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_ALL);
            targetUnitMap.remove(m_caster);
            break;
        }
        case 29858: // Soulshatter
        case 44416: // Pain Suppression
        case 53055: // Hand of Salvation
        {
            UnitList tmpUnitMap;
            FillAreaTargets(tmpUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            for (UnitList::const_iterator itr = tmpUnitMap.begin(); itr != tmpUnitMap.end(); ++itr)
            {
                if (!(*itr)->GetObjectGuid().IsPlayerOrPet())
                    targetUnitMap.push_back(*itr);
            }
            break;
        }
        case 43263: // Ghoul Taunt (Army of the Dead)
        {           // exclude Player and WorldBoss targets
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            for (UnitList::iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end();)
            {
                if (!*itr || (*itr)->GetTypeId() == TYPEID_PLAYER || static_cast<Creature*>(*itr)->IsWorldBoss())
                    itr = targetUnitMap.erase(itr);
                else
                    ++itr;
            }
            if (!targetUnitMap.empty())
                return true;
            break;
        }
        case 46584: // Raise Dead
        {
            targetUnitMap.push_back(m_caster);
            break;
        }
        case 47496: // Explode
        {
            if (effIndex == EFFECT_INDEX_0)
                FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            else
                targetUnitMap.push_back(m_caster);
            break;
        }
        case 48018: // Demonic Circle: Summon
        case 60854: // Demonic Circle: Clear
        {
            targetUnitMap.push_back(m_caster);
            break;
        }
        case 48278: //Svala - Banshee Paralyze
        {
            UnitList tmpUnitMap;
            FillAreaTargets(tmpUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);

            if (tmpUnitMap.empty())
                break;

            for (UnitList::const_iterator itr = tmpUnitMap.begin(); itr != tmpUnitMap.end(); ++itr)
            {
                 if (!*itr) continue;

                 if ((*itr)->GetObjectGuid() == m_caster->GetCreatorGuid())
                     targetUnitMap.push_back(*itr);
            }
            break;
        }
        case 49158: // Corpse Explosion
        case 51325:
        case 51326:
        case 51327:
        case 51328:
        {
            Unit* unitTarget = NULL;

            targetUnitMap.remove(m_caster);
            unitTarget = m_targets.getUnitTarget();

            // Cast on corpses...
            if (unitTarget && unitTarget->getDeathState() == CORPSE &&
                (unitTarget->GetCreatureTypeMask() & CREATURE_TYPEMASK_MECHANICAL_OR_ELEMENTAL) == 0 ||
                    // ...or own Risen Ghoul pet - self explode effect
                (unitTarget && unitTarget->GetEntry() == 26125 &&
                unitTarget->GetCreatorGuid() == m_caster->GetObjectGuid()))
            {
                targetUnitMap.push_back(unitTarget);
            }
            else
            {
                Unit* pCorpseTarget = NULL;
                MaNGOS::NearestCorpseInObjectRangeCheck u_check(*m_caster, radius);
                MaNGOS::UnitLastSearcher<MaNGOS::NearestCorpseInObjectRangeCheck> searcher(pCorpseTarget, u_check);
                Cell::VisitGridObjects(m_caster, searcher, radius);

                if (pCorpseTarget)
                    targetUnitMap.push_back(pCorpseTarget);
            }

            if (targetUnitMap.empty())
            {
                // no valid targets, clear cooldown at fail
                if (m_caster->GetTypeId() == TYPEID_PLAYER)
                    ((Player*)m_caster)->RemoveSpellCooldown(m_spellInfo->Id, true);
                SendCastResult(SPELL_FAILED_NO_VALID_TARGETS);
                finish(false);
            }
            break;
        }
        case 49302: // Racer Slam Hit Destination
        {
            if (!m_targets.getUnitTarget())
                return false;
            targetUnitMap.push_back(m_targets.getUnitTarget());
            break;
        }
        case 49821: // Mind Sear
        case 53022:
        {
            Unit* unitTarget = m_targets.getUnitTarget();
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            if  (unitTarget)
                targetUnitMap.remove(unitTarget);
            break;
        }
        case 50444: // Corpse Explosion
        case 53730: // Corpse Explosion Knockback
        {
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            break;
        }
        case 50652: // Ram
        case 60206:
        case 67441:
        case 67442:
        case 67816:
        {
            switch(effIndex)
            {
                case EFFECT_INDEX_0:
                case EFFECT_INDEX_1:
                    FillAreaTargets(targetUnitMap, radius, PUSH_IN_FRONT, SPELL_TARGETS_AOE_DAMAGE);
                    break;
                case EFFECT_INDEX_2:
                {
                    float fSearchDistance = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
                    std::list<GameObject*> tempTargetGOList;
                    MaNGOS::GameObjectInRangeCheck check(m_caster, m_caster->GetPositionX(), m_caster->GetPositionY(), m_caster->GetPositionZ(), radius);
                    MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectInRangeCheck> searcher(tempTargetGOList, check);
                    Cell::VisitAllObjects(m_caster, searcher, radius + fSearchDistance);
                    for(std::list<GameObject*>::iterator iter = tempTargetGOList.begin(); iter != tempTargetGOList.end(); ++iter)
                        if (m_caster->HasInArc(M_PI_F, *iter))
                            AddGOTarget(*iter, effIndex);
                }
                    break;
            }
            break;
        }
        case 52408: // Seaforium Blast
        {
            if (!m_originalCasterGUID || !m_originalCasterGUID.IsGameObject())
                break;

            GameObject* pBomb = m_caster->GetMap()->GetGameObject(m_originalCasterGUID);
            if (!pBomb)
                break;

            DEBUG_LOG("Bomb position: %f %f %f", pBomb->GetPositionX(), pBomb->GetPositionY(), pBomb->GetPositionZ());

            m_targets.setDestination(pBomb->GetPositionX(), pBomb->GetPositionY(), pBomb->GetPositionZ());

            if (effIndex == EFFECT_INDEX_0)
                FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            else if (effIndex == EFFECT_INDEX_1)
            {
                float fSearchDistance = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellInfo->rangeIndex));
                std::list<GameObject*> tempTargetGOList;
                MaNGOS::GameObjectInRangeCheck check(m_caster, pBomb->GetPositionX(), pBomb->GetPositionY(), pBomb->GetPositionZ(), radius);
                MaNGOS::GameObjectListSearcher<MaNGOS::GameObjectInRangeCheck> searcher(tempTargetGOList, check);
                Cell::VisitAllObjects(m_caster, searcher, radius + fSearchDistance);
                for(std::list<GameObject*>::iterator iter = tempTargetGOList.begin(); iter != tempTargetGOList.end(); ++iter)
                    AddGOTarget(*iter, effIndex);
            }
            break;
        }
        case 54148: // Svala Choose Only Player
        {
            UnitList tmpUnitMap, tmpUnitMap2;
            FillAreaTargets(tmpUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);

            if (tmpUnitMap.empty())
                break;

            for (UnitList::const_iterator itr = tmpUnitMap.begin(); itr != tmpUnitMap.end(); ++itr)
            {
                 if (!*itr) continue;

                 if ((*itr)->GetTypeId() == TYPEID_PLAYER)
                     tmpUnitMap2.push_back(*itr);
            }

            if (tmpUnitMap2.empty())
                break;

            UnitList::iterator itr = tmpUnitMap2.begin();
            std::advance(itr, urand(0, tmpUnitMap2.size() - 1));
            targetUnitMap.push_back(*itr);
            break;
        }
        case 57143: // Life Burst (Wyrmrest Skytalon)
        {
            // hack - spell is AoE but implicitTargets dont match here :/
            SetTargetMap(effIndex, TARGET_ALL_FRIENDLY_UNITS_AROUND_CASTER, targetUnitMap);
            targetUnitMap.push_back(m_caster);
            break;
        }
        case 57405: // Flush
        case 58873: // Water Spout
        {
            if (effIndex != EFFECT_INDEX_0)
                return false;

            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_ALL);
            targetUnitMap.remove(m_caster);
            break;
        }
        case 57496: // Volazj Insanity
        {
            UnitList PlayerList;
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);

            if (targetUnitMap.empty())
                break;

            for (UnitList::const_iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end(); ++itr)
            {
                 if (!*itr) continue;

                 if ((*itr)->GetTypeId() == TYPEID_PLAYER)
                     PlayerList.push_back(*itr);
            }

            if (PlayerList.empty() || PlayerList.size() > 5 || PlayerList.size() < 2)
                break;

            uint32 uiPhaseIndex = 0;
            uint32 uiSummonIndex;
            for (UnitList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
            {
                if (!*itr) continue;

                (*itr)->CastSpell((*itr), 57508+uiPhaseIndex, true);

                uiSummonIndex = 0;
                for (UnitList::const_iterator iter = PlayerList.begin(); iter != PlayerList.end(); ++iter)
                {
                    if (!*iter) continue;

                    if ((*itr) != (*iter))
                        (*itr)->CastSpell((*itr), 57500+uiSummonIndex, true);
                    uiSummonIndex++;
                }
                uiPhaseIndex++;
            }
            break;
        }
        case 57557: // Pyrobuffet (Sartharion encounter) - don't target Range Markered units
        {
            UnitList tempTargetUnitMap;
            FillAreaTargets(tempTargetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_HOSTILE);
            if (!tempTargetUnitMap.empty())
            {
                for (UnitList::const_iterator iter = tempTargetUnitMap.begin(); iter != tempTargetUnitMap.end(); ++iter)
                {
                    if ((*iter) && !(*iter)->HasAura(m_spellInfo->CalculateSimpleValue(EFFECT_INDEX_2)))
                        targetUnitMap.push_back(*iter);
                }
            }

            if (!targetUnitMap.empty())
                return true;

            break;
        }
        case 58912: // Deathstorm
        {
            if (!m_caster->GetObjectGuid().IsVehicle())
                break;

            SetTargetMap(effIndex, TARGET_RANDOM_ENEMY_CHAIN_IN_AREA, targetUnitMap);
            break;
        }
        case 59754: // Rune Tap triggered from Glyph of Rune Tap
        {
            FillRaidOrPartyTargets(targetUnitMap, m_caster, m_caster, radius, false, true, false);
            break;
        }
        case 61999: // Raise ally
        {
            WorldObject* result = FindCorpseUsing<MaNGOS::RaiseAllyObjectCheck>();
            if (result)
                targetUnitMap.push_back((Unit*)result);
            else
                targetUnitMap.push_back((Unit*)m_caster);
            break;
        }
        case 62166: // Stone Grip (Kologarn)
        case 63342: // Focused Eyebeam Summon Trigger (Kologarn)
        case 63981: // Stone Grip (Kologarn)
        {
            if (m_caster->getVictim())
                targetUnitMap.push_back(m_caster->getVictim());
            else
            {
                FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
                targetUnitMap.sort(TargetDistanceOrderNear(m_caster));
                targetUnitMap.resize(1);
            }
            if (!targetUnitMap.empty())
                return true;
            break;
        }
        case 62343: // Heat (remove all except active iron constructs)
        {
            UnitList tempTargetUnitMap;
            FillAreaTargets(tempTargetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_ALL);

            for (UnitList::iterator itr = tempTargetUnitMap.begin(),next; itr != tempTargetUnitMap.end(); ++itr)
            {
                if ((*itr) && (*itr)->GetEntry() == 33121 &&
                    !(*itr)->HasAura(62468) && !(*itr)->HasAura(62373) &&
                    !(*itr)->HasAura(62382) && !(*itr)->HasAura(67114)
                    )
                {
                    targetUnitMap.push_back(*itr);
                }
            }
            break;
        }
        case 62488: // Activate Constructs (remove all except inactive constructs)
        {
            UnitList tempTargetUnitMap;
            FillAreaTargets(tempTargetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_NOT_HOSTILE);
            tempTargetUnitMap.sort(TargetDistanceOrderNear(m_caster));
            for (UnitList::iterator itr = tempTargetUnitMap.begin(),next; itr != tempTargetUnitMap.end(); ++itr)
            {
                if ((*itr) && (*itr)->GetEntry() == 33121 && (*itr)->HasAura(62468)) // check for stun aura
                {
                    targetUnitMap.push_back(*itr);
                    return true;    // only one Construct per Spell
                }
            }
            break;
        }
        case 63025:  // Gravity Bomb (XT-002 in Ulduar) - exclude caster from pull and double damage
        case 64233:
        {
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_FRIENDLY);
            targetUnitMap.remove(m_caster);
            break;
        }
        case 63278: // Mark of Faceless
        {
            if (effIndex != EFFECT_INDEX_1)
                return false;

            Unit* currentTarget = m_targets.getUnitTarget();
            if (currentTarget)
            {
                m_targets.setDestination(currentTarget->GetPositionX(), currentTarget->GetPositionY(), currentTarget->GetPositionZ());
                FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
                targetUnitMap.remove(currentTarget);
            }
            break;
        }
        case 65044: // Flames
        case 65045: // Flame of demolisher
        {
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            break;
        }
        case 65919: // Anub'arak Cast Check Ice Spell (Trial of the Crusader - Anub'arak)
        case 67858:
        case 67859:
        case 67860:
        {
            m_caster->CastSpell(m_caster, 66181, true);
            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            break;
        }
        case 66250: // Pass The Turkey
        case 66259: // Pass The Stuffing
        case 66260: // Pass The Pie
        case 66261: // Pass The Cranberries
        case 66262: // Pass The Sweet Potatoes
        {
            if (!m_caster->GetObjectGuid().IsVehicle())
                return false;

            if (Unit* target = m_targets.getUnitTarget())
            {
                if (Unit* target = m_targets.getUnitTarget())
                {
                    if (target->GetVehicle())
                        target = target->GetVehicle()->GetBase();

                    if (target)
                    {
                        if (target == m_caster)
                            return true;

                        uint32 chairIds[5] = { 34812, 34819, 34822, 34823, 34824 };
                        for (int i = 0; i < 5; ++i)
                        {
                            if (target->GetEntry() == chairIds[i])
                            {
                                targetUnitMap.push_back(target);
                                break;
                            }
                        }
                    }
                }
            }
            break;
        }
        case 66862: // Radiance (Trial of the Champion - Eadric the Pure)
        case 67681:
        {
            UnitList tmpUnitMap;
            FillAreaTargets(tmpUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            if (!tmpUnitMap.empty())
            {
                for (UnitList::const_iterator itr = tmpUnitMap.begin(); itr != tmpUnitMap.end(); ++itr)
                {
                    if (*itr && (*itr)->isInFrontInMap(m_caster, DEFAULT_VISIBILITY_DISTANCE) && (*itr)->IsWithinLOSInMap(m_caster))
                        targetUnitMap.push_back(*itr);
                }
            }
            break;
        }
        case 67470: // Pursuing Spikes (Check Aura and Summon Spikes) (Trial Of The Crusader - Anub'arak)
        {
            UnitList tmpUnitMap;
            bool m_bOneTargetHaveAura = false;

            FillAreaTargets(tmpUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            if (!tmpUnitMap.empty())
            {
                for (UnitList::const_iterator itr = tmpUnitMap.begin(); itr != tmpUnitMap.end(); ++itr)
                {
                    if ((*itr)->HasAura(67574))
                    {
                        m_bOneTargetHaveAura = true;
                        break;
                    }
                    else
                    {
                        if ((*itr)->GetTypeId() == TYPEID_PLAYER)
                            targetUnitMap.push_back(*itr);
                    }
                }
                if (!m_bOneTargetHaveAura && !targetUnitMap.empty())
                {
                    uint32 t = 0;
                    UnitList::iterator iter = targetUnitMap.begin();
                    while (iter != targetUnitMap.end() && (*iter)->IsWithinDist(m_caster, radius))
                    {
                        ++t;
                        ++iter;
                    }

                    iter = targetUnitMap.begin();
                    std::advance(iter, urand(0, t-1));
                    if (*iter)
                        (*iter)->CastSpell((*iter), 67574, true);
                }
            }
            break;
        }
        case 68921: // Soulstorm (Forge of Souls - Bronjahm)
        case 69049:
        {
            UnitList tmpUnitMap;
            FillAreaTargets(tmpUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            if (!tmpUnitMap.empty())
            {
                for (UnitList::const_iterator itr = tmpUnitMap.begin(); itr != tmpUnitMap.end(); ++itr)
                {
                    if (*itr && !(*itr)->IsWithinDistInMap(m_caster, 10.0f))
                        targetUnitMap.push_back(*itr);
                }
            }
            break;
        }
        case 68508: // Crushing Leap (IoC bosses)
        {
            UnitList tmpUnitMap;
            FillAreaTargets(tmpUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            if (!tmpUnitMap.empty())
            {
                for (UnitList::const_iterator itr = tmpUnitMap.begin(); itr != tmpUnitMap.end(); ++itr)
                {
                    if (*itr && (*itr)->GetTypeId() == TYPEID_PLAYER && !(*itr)->m_movementInfo.HasMovementFlag(MovementFlags(MOVEFLAG_FALLING)))
                        targetUnitMap.push_back(*itr);
                }
            }
            break;
        }
        case 72905: // Frostbolt Volley (Lady Deathwhisper)
        case 72906:
        case 72907:
        case 72908:
        {
            UnitList tempTargetUnitMap;
            FillAreaTargets(tempTargetUnitMap, radius, PUSH_SELF_CENTER, SPELL_TARGETS_AOE_DAMAGE);
            for (UnitList::const_iterator iter = tempTargetUnitMap.begin(); iter != tempTargetUnitMap.end(); ++iter)
            {
                if (!(*iter)->GetCharmerOrOwnerPlayerOrPlayerItself())
                    continue;

                targetUnitMap.push_back((*iter));
            }

            break;
        }
        case 74960:                                     // Infrigidate
        {
            UnitList tempTargetUnitMap;
            FillAreaTargets(tempTargetUnitMap, 20.0f, PUSH_SELF_CENTER, SPELL_TARGETS_FRIENDLY);
            tempTargetUnitMap.remove(m_caster);
            if (!tempTargetUnitMap.empty())
            {
                UnitList::iterator i = tempTargetUnitMap.begin();
                advance(i, urand(0, tempTargetUnitMap.size() - 1));
                targetUnitMap.push_back(*i);
                return true;
            }
            return false;
        }
        case 82691:                                     // Ring of Frost
        {
            Unit* target = m_targets.getUnitTarget();
            if (!target)
                return false;

            // Need to trigger this only when ring is fully deployed...
            if (target->HasAura(91264))
                return true;

            if (!m_triggeredByAuraSpell || m_triggeredByAuraSpell->Id != 82676)
                return  false;

            radius = m_triggeredByAuraSpell->CalculateSimpleValue(EFFECT_INDEX_1);

            FillAreaTargets(targetUnitMap, radius, PUSH_DEST_CENTER, SPELL_TARGETS_NOT_FRIENDLY);
            for (UnitList::iterator itr = targetUnitMap.begin(); itr != targetUnitMap.end();)
            {
                // already frozen of has 3 sec immune to freeze
                if ((*itr)->HasAura(m_spellInfo->Id) || (*itr)->GetSpellAuraHolder(91264, m_caster->GetObjectGuid()))
                    itr = targetUnitMap.erase(itr);
                else
                    ++itr;
            }

            return true;
        }
        case 85078:                                     // Leave Siege Mode
        {
            targetUnitMap.push_back(m_caster);
            return true;
        }
        case 85122:                                     // Siege Cannon
        {
            UnitList targets;
            MaNGOS::AllCreaturesOfEntryInRangeCheck u_check(m_caster, 45561, radius);
            MaNGOS::UnitListSearcher<MaNGOS::AllCreaturesOfEntryInRangeCheck> searcher(targets, u_check);
            Cell::VisitGridObjects(m_caster, searcher, radius);
            if (targets.empty())
                return true;

            UnitList::iterator itr = targets.begin();
            std::advance(itr, urand(0, targets.size() - 1));
            targetUnitMap.push_back(*itr);
            m_targets.setDestination((*itr)->GetPositionX(), (*itr)->GetPositionY(), (*itr)->GetPositionZ());
            m_targets.setUnitTarget((*itr));
            return true;
        }
        case 96931:                                     // Eyes of Occu'thar
        {
            targetUnitMap.push_back(m_caster);
            return true;
        }
        default:
        {
            // Penance & Holy Shock
            if (m_spellInfo->IsFitToFamily(SPELLFAMILY_PRIEST, UI64LIT(0x80000000000000)) ||
                m_spellInfo->IsFitToFamily(SPELLFAMILY_PALADIN, UI64LIT(0x200000)) && m_spellInfo->GetSpellEffectIdByIndex(EFFECT_INDEX_0) == SPELL_EFFECT_DUMMY)
            {
                if (Unit* target = m_targets.getUnitTarget())
                {
                    if (m_caster->IsFriendlyTo(target))
                    {
                        Player* plr = target->GetCharmerOrOwnerPlayerOrPlayerItself();
                        if (plr && (plr->duel || m_caster->GetTypeId() == TYPEID_PLAYER && plr->GetTeam() != ((Player*)m_caster)->GetTeam()))
                            m_targets.setUnitTarget(m_caster);
                    }
                    else
                    {
                        if (Unit* pUnitTarget = m_caster->SelectMagnetTarget(target, this, effIndex))
                        {
                            m_targets.setUnitTarget(pUnitTarget);
                            m_spellFlags |= SPELL_FLAG_REDIRECTED;
                        }
                    }
                    targetUnitMap.push_back(m_targets.getUnitTarget());
                    if (m_caster->GetTypeId() == TYPEID_PLAYER)
                        ((Player*)m_caster)->m_lastSpellTargetGuid = m_targets.getUnitTarget()->GetObjectGuid();
                }
                return true;
            }
            // Ranger: Stoneclaw Totem & Glyph - patch by laise: Stoneclaw Totem absorb has wrong target
            else if (m_spellInfo->GetSpellFamilyName() == SPELLFAMILY_SHAMAN && m_spellInfo->SpellIconID == 276)
            {
                targetUnitMap.push_back(m_caster);
                return true;
            }
            else
                return false;
        }
        break;
    }

    return true;
}

