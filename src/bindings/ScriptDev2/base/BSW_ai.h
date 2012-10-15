/* Copyright (C) 2009 - 2010 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

#ifndef DEF_BOSS_SPELL_WORKER_H
#define DEF_BOSS_SPELL_WORKER_H
#define BSW_VERSION 0.6.12

#include "precompiled.h"
#include "Player.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "Unit.h"
#include "Database/DatabaseEnv.h"
#include "../ScriptMgr.h"


enum
{
  DIFFICULTY_LEVELS    = 4,
  EFFECT_INDEX_ALL     = 255,
  SPELL_INDEX_ERROR    = 255,
};

enum BossSpellFlag
{
  CAST_NORMAL,
  CAST_REMOVE,
  CAST_OVERRIDE,
  SPELLFLAG_NUMBER
};

enum BossSpellTableParameters
{
  DO_NOTHING                =  0,
  CAST_ON_SELF              =  1,
  CAST_ON_SUMMONS           =  2,
  CAST_ON_VICTIM            =  3,
  CAST_ON_RANDOM            =  4,
  CAST_ON_BOTTOMAGGRO       =  5,
  CAST_ON_TARGET            =  6,
  APPLY_AURA_SELF           =  7,
  APPLY_AURA_TARGET         =  8,
  SUMMON_NORMAL             =  9,
  SUMMON_INSTANT            = 10,
  SUMMON_TEMP               = 11,
  CAST_ON_ALLPLAYERS        = 12,
  CAST_ON_FRENDLY           = 13,
  CAST_ON_FRENDLY_LOWHP     = 14,
  CAST_ON_RANDOM_POINT      = 15,
  CAST_ON_RANDOM_PLAYER     = 16,
  APPLY_AURA_ALLPLAYERS     = 17,
  FORCE_CAST                = 18,
  SPELLTABLEPARM_NUMBER
};

struct Locations
{
    float x, y, z, o;
    int32 id;
};

struct WayPoints
{
    WayPoints(int32 _id, float _x, float _y, float _z)
    {
        id = _id;
        x = _x;
        y = _y;
        z = _z;
    }
    int32 id;
    float x, y, z;
};

struct BSWRecord
{

    public:
    // External (loaded from database) variables
    uint32 id;
    uint32 m_uiSpellEntry[DIFFICULTY_LEVELS];          // Stores spell entry for difficulty levels
    uint32 m_uiSpellTimerMin[DIFFICULTY_LEVELS];       // The timer (min) before the next spell casting, in milliseconds
    uint32 m_uiSpellTimerMax[DIFFICULTY_LEVELS];       // The timer (max) before the next spell casting
    uint32 m_uiSpellData[DIFFICULTY_LEVELS];           // Additional data for spell casting or summon
    Locations LocData;                                 // Float data structure for locations
    int    varData;                                    // Additional data for spell
    uint32 StageMaskN;                                 // Stage mask for this spell (normal)
    uint32 StageMaskH;                                 // Stage mask for this spell (heroic)
    BossSpellTableParameters  m_CastTarget;            // Target on casting spell
    bool   m_IsVisualEffect;                           // Spellcasting is visual effect or real effect
    bool   m_IsBugged;                                 // Need override for this spell
    int32  textEntry;                                  // Text entry from script_text for this spell
    // Internal variables
    uint32 m_SpellTimer;                               // Current timer for this spell
};

struct MANGOS_DLL_DECL BSWScriptedAI : public ScriptedAI
{
    public:
        explicit BSWScriptedAI(Creature* pCreature);

        ~BSWScriptedAI();

        void doReset();

        void resetTimer(uint32 SpellID)
             {
                 if (queryIndex(_findSpellIDX(SpellID))) _resetTimer(_findSpellIDX(SpellID));
                     else return;
             };

        void resetTimers()
             {
             for (uint8 i = 0; i < bossSpellCount(); ++i)
                  _resetTimer(i);
             };

        bool timedQuery(uint32 SpellID, uint32 diff, bool ignorecast = false)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ? _QuerySpellPeriod(_findSpellIDX(SpellID), diff, ignorecast) : false;
             };

        CanCastResult timedCast(uint32 SpellID, uint32 diff, Unit* pTarget = NULL)
             {
                 if (!queryIndex(_findSpellIDX(SpellID))) return CAST_FAIL_OTHER;
                 return _QuerySpellPeriod(_findSpellIDX(SpellID), diff) ? _BSWSpellSelector(_findSpellIDX(SpellID), pTarget) : CAST_FAIL_STATE;
             };

        CanCastResult doCast(uint32 SpellID, Unit* pTarget = NULL)
             {
                  return queryIndex(_findSpellIDX(SpellID)) ? _BSWSpellSelector(_findSpellIDX(SpellID), pTarget) : CAST_FAIL_OTHER;
             };

        CanCastResult doCast(Unit* pTarget, uint32 SpellID)
             {
                  if (!pTarget) return CAST_FAIL_OTHER;
                  return queryIndex(_findSpellIDX(SpellID)) ? _BSWCastOnTarget(pTarget, _findSpellIDX(SpellID)) : CAST_FAIL_OTHER;
             };

        bool doRemove(uint32 SpellID, Unit* pTarget = NULL, uint8 index = EFFECT_INDEX_ALL)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ? _doRemove(_findSpellIDX(SpellID),pTarget,index) : _doRemove(SpellID,pTarget,index);
             };

        bool doRemoveFromAll(uint32 SpellID)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ? _doRemoveFromAll(_findSpellIDX(SpellID)) : _doRemoveFromAll(SpellID);
             };

        bool doAura(uint32 SpellID, Unit* pTarget, SpellEffectIndex index, int32 basepoint = 0, bool isStack = true)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ? _doAura(_findSpellIDX(SpellID), pTarget, index, isStack) : _doAura(SpellID, pTarget, index, basepoint, isStack);
             };

        bool doAura(uint32 SpellID, Unit* pTarget = NULL)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ? _doAura(_findSpellIDX(SpellID), pTarget) : _doAura(SpellID, pTarget);
             };

        bool hasAura(uint32 SpellID, Unit* pTarget = NULL)
             {
                 if (!pTarget) pTarget = m_creature;
                 return queryIndex(_findSpellIDX(SpellID)) ? _hasAura(_findSpellIDX(SpellID),pTarget) : _hasAura(SpellID,pTarget);
             };

        uint8 auraCount(uint32 SpellID, Unit* pTarget = NULL, SpellEffectIndex index = EFFECT_INDEX_0)
             {
                 if (!pTarget) pTarget = m_creature;
                 return queryIndex(_findSpellIDX(SpellID)) ? _auraCount(_findSpellIDX(SpellID),pTarget,index) : _auraCount(SpellID,pTarget,index);
             };

        Unit* doSummon(uint32 SpellID, TempSummonType type = TEMPSUMMON_CORPSE_TIMED_DESPAWN, uint32 delay = 60000)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ? _doSummon(_findSpellIDX(SpellID), type, delay) : NULL;
             };

        Unit* doSelectRandomPlayer(uint32 SpellID = 0, bool spellsearchtype = false, float range = 100.0f, bool includeVictim = true)
             {
                 return _doSelect(SpellID, spellsearchtype, range, includeVictim);
             };

        Unit* doSelectRandomPlayerAtRange(float range, bool includeVictim = true)
             {
                 return _doSelect(0, false, range, includeVictim);
             };

        Unit* doSummon(uint32 SpellID, float fPosX, float fPosY, float fPosZ, TempSummonType type = TEMPSUMMON_CORPSE_TIMED_DESPAWN, uint32 delay = 60000)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ? _doSummonAtPosition(_findSpellIDX(SpellID), fPosX, fPosY, fPosZ) : _doSummonAtPosition(SpellID, type, delay, fPosX, fPosY, fPosZ);
             };

        uint8 bossSpellCount()
             {
                 return m_BSWRecords.size();
             };

        bool queryIndex(uint8 m_uiSpellIdx)
             {
                 if  (m_uiSpellIdx == SPELL_INDEX_ERROR) return false;
                 else if  (m_uiSpellIdx >= 0 && m_uiSpellIdx < bossSpellCount()) return true;
                 else return false;
             };

        Creature* doSelectNearestCreature(uint32 guid, float range = 120.0f);

        uint32 getSpellData(uint32 SpellID)
             {
                 return queryIndex(_findSpellIDX(SpellID)) ?  _getSpellData(_findSpellIDX(SpellID)) : 0;
             };

        bool  doCastAll(uint32 diff);

        uint8 getStage() { return _stage; };

        void  setStage(uint8 stage) { _stage = stage; };

        bool  isHeroic() { Map* pMap = m_creature->GetMap(); return pMap->IsRaid() ? _isDifficultyInMask(12) : _isDifficultyInMask(2); };

        bool  isNormal() { return !isHeroic(); };

        bool  is25()     { Map* pMap = m_creature->GetMap(); return pMap->IsRaid() ? _isDifficultyInMask(10) : false; };

        uint32 getSpellWithDifficulty(uint32 SpellID) { return (queryIndex(_findSpellIDX(SpellID)) ? m_BSWRecords[_findSpellIDX(SpellID)].m_uiSpellEntry[currentDifficulty] : SpellID); };

    protected:

        Difficulty    currentDifficulty;

        std::vector<BSWRecord> m_BSWRecords;

    private:

        BossSpellTableParameters _getBSWCastType(uint32 pTemp);

        uint8         _findSpellIDX(uint32 SpellID);

        BSWRecord*    _getRecord(uint32 SpellID);

        void          _loadFromDB();

        void          _resetTimer(uint8 m_uiSpellIdx);

        Unit*         _doSelect(uint32 SpellID, bool spellsearchtype = false, float range = 100.0f, bool includeVictim = true);

        Unit*         _doSummon(uint8 m_uiSpellIdx, TempSummonType type = TEMPSUMMON_CORPSE_TIMED_DESPAWN, uint32 delay = 60000);

        Unit*         _doSummonAtPosition(uint8 m_uiSpellIdx, float fPosX, float fPosY, float fPosZ);

        Unit*         _doSummonAtPosition(uint32 guid, TempSummonType type, uint32 delay, float fPosX, float fPosY, float fPosZ);

        CanCastResult _BSWDoCast(uint8 m_uiSpellIdx, Unit* pTarget);

        CanCastResult _BSWDoForceCast(uint8 m_uiSpellIdx, Unit* pTarget);

        CanCastResult _BSWSpellSelector(uint8 m_uiSpellIdx, Unit* pTarget = NULL);

        CanCastResult _BSWCastOnTarget(Unit* pTarget, uint8 m_uiSpellIdx);

        bool          _QuerySpellPeriod(uint8 m_uiSpellIdx, uint32 diff, bool ignorecast = false);

        CanCastResult _DoCastSpellIfCan(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags = 0, uint64 uiOriginalCasterGUID = 0);

        bool          _doRemove(uint8 m_uiSpellIdx, Unit* pTarget = NULL, uint8 index = EFFECT_INDEX_ALL);

        bool          _doRemove(uint32 SpellID, Unit* pTarget, uint8 index = EFFECT_INDEX_ALL);

        bool          _doRemoveFromAll(uint8 m_uiSpellIdx);

        bool          _doRemoveFromAll(uint32 SpellID);

        bool          _doAura(uint8 m_uiSpellIdx, Unit* pTarget, SpellEffectIndex index, bool isStack = true);

        bool          _doAura(uint32 SpellID, Unit* pTarget, SpellEffectIndex index, int32 basepoint = 0, bool isStack = true);

        bool          _doAura(uint8 m_uiSpellIdx, Unit* pTarget);

        bool          _doAura(uint32 SpellID, Unit* pTarget);

        bool          _hasAura(uint8 m_uiSpellIdx, Unit* pTarget);

        bool          _hasAura(uint32 SpellID, Unit* pTarget);

        uint8         _auraCount(uint8 m_uiSpellIdx, Unit* pTarget = NULL, SpellEffectIndex index = EFFECT_INDEX_0);

        uint8         _auraCount(uint32 SpellID, Unit* pTarget, SpellEffectIndex index);

        void          _fillEmptyDataField();

        uint32        _getSpellData(uint8 m_uiSpellIdx);

        bool          _isDifficultyInMask(uint8 mask);

        uint8         _stage;

};

#endif