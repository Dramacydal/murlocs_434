/* Copyright (C) 2009 - 2010 by /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */
#include "precompiled.h"
#include "BSW_ai.h"
#ifdef DEF_BOSS_SPELL_WORKER_H
#include "ace/Process_Mutex.h"

BSWScriptedAI::BSWScriptedAI(Creature* pCreature) : ScriptedAI(pCreature)
{
    doReset();
    debug_log("BSW: Initialized BSWScriptedAI structure for creature %u difficulty %u",m_creature->GetEntry(),currentDifficulty);
};

BSWScriptedAI::~BSWScriptedAI()
{
     m_BSWRecords.clear();
     debug_log("BSW: Removing BSWScriptedAI structure for creature %u",m_creature->GetEntry());
};

void BSWScriptedAI::doReset()
{
    Map* pMap = m_creature->GetMap();
    if (pMap) currentDifficulty = pMap->GetDifficulty();
        else currentDifficulty = RAID_DIFFICULTY_10MAN_NORMAL;
    m_BSWRecords.clear();
    setStage(0);
    _loadFromDB();
    _fillEmptyDataField();
    resetTimers();
};

void BSWScriptedAI::_resetTimer(uint8 m_uiSpellIdx)
{
    if (m_BSWRecords[m_uiSpellIdx].m_uiSpellTimerMin[currentDifficulty] == 0
        && m_BSWRecords[m_uiSpellIdx].m_uiSpellTimerMax[currentDifficulty] >= HOUR*IN_MILLISECONDS)
    {
            m_BSWRecords[m_uiSpellIdx].m_SpellTimer = 0;
    }
    else if (m_BSWRecords[m_uiSpellIdx].m_uiSpellTimerMin[currentDifficulty] != m_BSWRecords[m_uiSpellIdx].m_uiSpellTimerMax[currentDifficulty])
    {
            m_BSWRecords[m_uiSpellIdx].m_SpellTimer = urand(0,m_BSWRecords[m_uiSpellIdx].m_uiSpellTimerMax[currentDifficulty]);
    }
    else m_BSWRecords[m_uiSpellIdx].m_SpellTimer = m_BSWRecords[m_uiSpellIdx].m_uiSpellTimerMin[currentDifficulty];

};

void BSWScriptedAI::_loadFromDB()
{
    // mutex block for process-safe request execute
    ACE_Process_Mutex mMutex = ACE_Process_Mutex("BSW_Lock");

    debug_log("BSW: Loading table of creature %u spell on difficulty %u", m_creature->GetEntry(), currentDifficulty);

    char query[MAX_QUERY_LEN];

    sprintf(query, "SELECT entry, spellID_N10, spellID_N25, spellID_H10, spellID_H25, timerMin_N10, timerMin_N25, timerMin_H10, timerMin_H25, timerMax_N10, timerMax_N25, timerMax_H10, timerMax_H25, data1, data2, data3, data4, locData_x, locData_y, locData_z, varData, StageMask_N, StageMask_H, CastType, isVisualEffect, isBugged, textEntry FROM `boss_spell_table` WHERE entry = %u;\r\n", m_creature->GetEntry());

    mMutex.acquire();
       QueryResult* Result = strSD2Pquery(query);
    mMutex.release();

    if (Result)
    {
        uint32 uiCount = 0;
        do
        {
            BSWRecord m_BSWRecord;

            Field* pFields = Result->Fetch();

            m_BSWRecord.id  = uiCount;

            uint32 m_creatureEntry          = pFields[0].GetUInt32();

            for (uint8 j = 0; j < DIFFICULTY_LEVELS; ++j)
                 m_BSWRecord.m_uiSpellEntry[j]  = pFields[1+j].GetUInt32();

            for (uint8 j = 0; j < DIFFICULTY_LEVELS; ++j)
                 m_BSWRecord.m_uiSpellTimerMin[j]  = pFields[1+DIFFICULTY_LEVELS+j].GetUInt32();

            for (uint8 j = 0; j < DIFFICULTY_LEVELS; ++j)
                 m_BSWRecord.m_uiSpellTimerMax[j]  = pFields[1+DIFFICULTY_LEVELS*2+j].GetUInt32();

            for (uint8 j = 0; j < DIFFICULTY_LEVELS; ++j)
                 m_BSWRecord.m_uiSpellData[j]  = pFields[1+DIFFICULTY_LEVELS*3+j].GetUInt32();

            m_BSWRecord.LocData.x  = pFields[1+DIFFICULTY_LEVELS*4].GetFloat();
            m_BSWRecord.LocData.y  = pFields[2+DIFFICULTY_LEVELS*4].GetFloat();
            m_BSWRecord.LocData.z  = pFields[3+DIFFICULTY_LEVELS*4].GetFloat();

            m_BSWRecord.varData    = pFields[4+DIFFICULTY_LEVELS*4].GetInt32();

            m_BSWRecord.StageMaskN = pFields[5+DIFFICULTY_LEVELS*4].GetUInt32();
            m_BSWRecord.StageMaskH = pFields[6+DIFFICULTY_LEVELS*4].GetUInt32();

            m_BSWRecord.m_CastTarget = _getBSWCastType(pFields[7+DIFFICULTY_LEVELS*4].GetUInt8());

            m_BSWRecord.m_IsVisualEffect = (pFields[8+DIFFICULTY_LEVELS*4].GetUInt8() == 0) ? false : true ;

            m_BSWRecord.m_IsBugged = (pFields[9+DIFFICULTY_LEVELS*4].GetUInt8() == 0) ? false : true ;

            m_BSWRecord.textEntry = pFields[10+DIFFICULTY_LEVELS*4].GetInt32();

            m_BSWRecords.push_back(m_BSWRecord);

            if (m_creatureEntry != m_creature->GetEntry()) error_log("BSW: Unknown error while load boss_spell_table");
               else ++uiCount;
        } while (Result->NextRow());

        delete Result;

        debug_log("BSW: Loaded %u spell data records for creature %u", bossSpellCount(), m_creature->GetEntry());
    }
    else
    {
        error_log("BSW: BSW table for creature %u is empty.", m_creature->GetEntry());
    };
}

bool BSWScriptedAI::_QuerySpellPeriod(uint8 m_uiSpellIdx, uint32 diff, bool ignorecast)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    if (m_creature->IsNonMeleeSpellCasted(false) && !ignorecast) return false;

    if (pSpell->m_SpellTimer <= diff)
    {
        if (pSpell->m_uiSpellTimerMax[currentDifficulty] >= HOUR*IN_MILLISECONDS) pSpell->m_SpellTimer=HOUR*IN_MILLISECONDS;
            else pSpell->m_SpellTimer = urand(pSpell->m_uiSpellTimerMin[currentDifficulty],pSpell->m_uiSpellTimerMax[currentDifficulty]);
        return true;
    }
    else
    {
        pSpell->m_SpellTimer -= diff;
            return false;
    };
};

CanCastResult BSWScriptedAI::_BSWSpellSelector(uint8 m_uiSpellIdx, Unit* pTarget)
{

    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    Unit* pSummon = NULL;

    CanCastResult result = CAST_FAIL_OTHER;

    debug_log("BSW: Casting spell number %u type %u",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);

        if (pSpell->m_uiSpellTimerMax[currentDifficulty] >= HOUR*IN_MILLISECONDS)
            m_creature->InterruptNonMeleeSpells(true);

        switch (pSpell->m_CastTarget) {

            case DO_NOTHING:
                   result = CAST_OK;
                   break;

            case CAST_ON_SELF:
                   result = _BSWCastOnTarget(m_creature, m_uiSpellIdx);
                   break;

            case CAST_ON_SUMMONS:
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case CAST_ON_VICTIM:
                   pTarget = m_creature->getVictim();
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case CAST_ON_RANDOM:
                   pTarget = _doSelect(0, false, 60.0f);
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case CAST_ON_BOTTOMAGGRO:
                   pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_BOTTOMAGGRO,0);
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case CAST_ON_TARGET:
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case APPLY_AURA_SELF:
                       if (_doAura(m_uiSpellIdx, m_creature, EFFECT_INDEX_0))
                           result = CAST_OK;
                       else result = CAST_FAIL_OTHER;
                   break;

            case APPLY_AURA_TARGET:
                   if (!pTarget || !pTarget->IsInMap(m_creature))
                   {
                       result = CAST_FAIL_OTHER;
                       break;
                   }
                   if (_doAura(m_uiSpellIdx, pTarget, EFFECT_INDEX_0))
                       result = CAST_OK;
                   else result = CAST_FAIL_OTHER;
                   break;

            case SUMMON_NORMAL:
                   pSummon = _doSummon(m_uiSpellIdx, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 5000);
                   if(pSummon) result = CAST_OK;
                       else result = CAST_FAIL_OTHER;
                   break;

            case SUMMON_TEMP:
                   pSummon = _doSummon(m_uiSpellIdx, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN,
                                        urand(pSpell->m_uiSpellTimerMin[currentDifficulty],pSpell->m_uiSpellTimerMax[currentDifficulty]));
                   if(pSummon) result = CAST_OK;
                       else result = CAST_FAIL_OTHER;
                   break;

            case SUMMON_INSTANT:
                   pSummon = _doSummon(m_uiSpellIdx, TEMPSUMMON_MANUAL_DESPAWN,0);
                   if(pSummon) result = CAST_OK;
                       else result = CAST_FAIL_OTHER;
                   break;

            case CAST_ON_ALLPLAYERS:
                   {
                       Map* pMap = m_creature->GetMap();
                       Map::PlayerList const& pPlayers = pMap->GetPlayers();
                       if (!pPlayers.isEmpty())
                       {
                           for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                           {
                               pTarget = itr->getSource();
                               if (pTarget && pTarget->isAlive() && pTarget->IsWithinDistInMap(m_creature, pSpell->LocData.x))
                               {
                                   if (!pSpell->m_IsBugged)
                                   {
                                       m_creature->CastSpell(pTarget, pSpell->m_uiSpellEntry[currentDifficulty], false);
                                   }
                                   else
                                   {
                                       result = _BSWDoCast(m_uiSpellIdx, pTarget);
                                   };

                                   debug_log("BSW: Casted spell number %u type %u on target %s bugged: %u result: %u", pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget, pTarget->GetGuidStr().c_str(), pSpell->m_IsBugged, result);
                                   result = CAST_OK;
                               };
                           }
                       } else
                       {
                           debug_log("BSW: Casted spell number %u has type = %u, but pMap->GetPlayers() list is empty", pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);
                           result = CAST_FAIL_OTHER;
                       }
                   }
                   break;

            case CAST_ON_FRENDLY:
                   pTarget = DoSelectLowestHpFriendly(pSpell->LocData.x,0);
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case CAST_ON_FRENDLY_LOWHP:
                   pTarget = DoSelectLowestHpFriendly(pSpell->LocData.x,1);
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case CAST_ON_RANDOM_POINT:
                   if (!pTarget) pTarget = m_creature;
                   if (pSpell->LocData.z <= 1.0f) 
                   {
                         float fPosX, fPosY, fPosZ;
                         if (!pTarget->IsPositionValid() || !pTarget->IsInMap(m_creature))
                         {
                             if (pTarget->GetTypeId() == TYPEID_PLAYER)
                                 error_log("BSW: CAST_ON_RANDOM_POINT FAILED: player has invalid position. SpellID is %u",pSpell->m_uiSpellEntry[currentDifficulty]);
                             else error_log("BSW: CAST_ON_RANDOM_POINT FAILED: creature has invalid position. SpellID is %u",pSpell->m_uiSpellEntry[currentDifficulty]);
                             result = CAST_FAIL_OTHER;
                             break;
                         }

                         pTarget->GetPosition(fPosX, fPosY, fPosZ);
                         pTarget->GetRandomPoint(fPosX, fPosY, fPosZ, urand((uint32)pSpell->LocData.x, (uint32)pSpell->LocData.y), fPosX, fPosY, fPosZ);
                         if ((int)fPosZ == 0)
                         {
                             error_log("BSW: CAST_ON_RANDOM_POINT FAILED: Positon Z is NULL. Strange bug");
                             result = CAST_FAIL_OTHER;
                             break;
                         }

                         if (SpellEntry const *spell = (SpellEntry *)GetSpellStore()->LookupEntry(pSpell->m_uiSpellEntry[currentDifficulty]))
                           if (SpellRangeEntry const *pSpellRange = GetSpellRangeStore()->LookupEntry(spell->rangeIndex))
                              if (m_creature->GetDistance(fPosX, fPosY, fPosZ) <= pSpellRange->maxRange)
                              {
                                   m_creature->CastSpell(fPosX, fPosY, fPosZ, pSpell->m_uiSpellEntry[currentDifficulty], false);
                                   result = CAST_OK;
                                   break;
                               };
                         result = CAST_FAIL_TOO_FAR;
                   } else  result = CAST_FAIL_OTHER;
                   break;

            case CAST_ON_RANDOM_PLAYER:
                   if ( pSpell->LocData.x < 1 ) pTarget = _doSelect(0, false, 60.0f);
                       else pTarget = _doSelect(0, false, (float)pSpell->LocData.x);
                   result = _BSWCastOnTarget(pTarget, m_uiSpellIdx);
                   break;

            case APPLY_AURA_ALLPLAYERS:
                   {
                       Map* pMap = m_creature->GetMap();
                       Map::PlayerList const& pPlayers = pMap->GetPlayers();
                       for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                       {
                           pTarget = itr->getSource();
                           if (pTarget && pTarget->isAlive() && pTarget->IsWithinDistInMap(m_creature, pSpell->LocData.x))
                           {
                               _doAura(m_uiSpellIdx, pTarget, EFFECT_INDEX_0);
                               result = CAST_OK;
                           }
                       }
                   }
                   break;

            case FORCE_CAST:
                   result = _BSWDoForceCast(m_uiSpellIdx, pTarget);
                   break;

            case SPELLTABLEPARM_NUMBER:
            default:
                   error_log("BSW: FAILED casting spell number %u type %u - type not exists",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);
                   result = CAST_FAIL_OTHER;
                   break;
            };

        if (pSpell->textEntry && result == CAST_OK)
        {
            if (pTarget)
                DoScriptText(pSpell->textEntry,m_creature,pTarget);
            else
                DoScriptText(pSpell->textEntry,m_creature);
        };

    debug_log("BSW: Casted spell number %u, result = %u",pSpell->m_uiSpellEntry[currentDifficulty], result);

    return result;
};

CanCastResult BSWScriptedAI::_BSWCastOnTarget(Unit* pTarget, uint8 m_uiSpellIdx)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    if (!pTarget || !pTarget->IsInMap(m_creature) || !pTarget->isAlive() && pTarget != (Unit*)m_creature)
    {
        debug_log("BSW: warning - failed casting (on target) spell number %u - no target or target not in map",pSpell->m_uiSpellEntry[currentDifficulty]);
        return CAST_FAIL_OTHER;
    }

    debug_log("BSW: Casting (on target) spell %u type %u",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);

    if (!pSpell->m_IsBugged) return _DoCastSpellIfCan(pTarget, pSpell->m_uiSpellEntry[currentDifficulty]);
        else if (pSpell->m_IsBugged) return _BSWDoCast(m_uiSpellIdx, pTarget);
             else  return CAST_FAIL_OTHER;
};

bool BSWScriptedAI::_hasAura(uint8 m_uiSpellIdx, Unit* pTarget)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    return _hasAura(pSpell->m_uiSpellEntry[currentDifficulty], pTarget);

};

bool BSWScriptedAI::_hasAura(uint32 SpellID, Unit* pTarget)
{
    if (!pTarget || !pTarget->IsInMap(m_creature))
    {
        error_log("BSW: FAILED Query aura for spell %u - no target or target not in map",SpellID);
        return false;
    }

    return (pTarget->HasAura(SpellID));

};

uint8 BSWScriptedAI::_auraCount(uint8 m_uiSpellIdx, Unit* pTarget, SpellEffectIndex index)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    return _auraCount(pSpell->m_uiSpellEntry[currentDifficulty], pTarget, index);

};

uint8 BSWScriptedAI::_auraCount(uint32 SpellID, Unit* pTarget, SpellEffectIndex index)
{
    if (!_hasAura(SpellID,pTarget)) return 0;

    if (Aura* aura = pTarget->GetAura(SpellID, index))
        if (aura->GetStackAmount() > 0)
            return aura->GetStackAmount();
    return 0;

};

uint8 BSWScriptedAI::_findSpellIDX(uint32 SpellID)
{
    if (bossSpellCount() > 0)
        for(uint8 i = 0; i < bossSpellCount(); ++i)
            if (m_BSWRecords[i].m_uiSpellEntry[RAID_DIFFICULTY_10MAN_NORMAL] == SpellID) return i;

    error_log("BSW: spell %u not found  in m_creature %u spelltable. Memory or database error?", SpellID, m_creature->GetEntry());

    return SPELL_INDEX_ERROR;
}

BSWRecord* BSWScriptedAI::_getRecord(uint32 SpellID)
{
    if (!m_BSWRecords.empty())
    {
        for(uint8 i = 0; i < m_BSWRecords.size(); ++i)
            if (m_BSWRecords[i].m_uiSpellEntry[RAID_DIFFICULTY_10MAN_NORMAL] == SpellID) return &m_BSWRecords[i];
    }
    error_log("BSW: spell %u not found  in m_creature %u spelltable. Memory or database error?", SpellID, m_creature->GetEntry());
    return NULL;
}

BossSpellTableParameters BSWScriptedAI::_getBSWCastType(uint32 pTemp)
{
    switch (pTemp) {
                case 0:  return DO_NOTHING;
                case 1:  return CAST_ON_SELF;
                case 2:  return CAST_ON_SUMMONS;
                case 3:  return CAST_ON_VICTIM;
                case 4:  return CAST_ON_RANDOM;
                case 5:  return CAST_ON_BOTTOMAGGRO;
                case 6:  return CAST_ON_TARGET;
                case 7:  return APPLY_AURA_SELF;
                case 8:  return APPLY_AURA_TARGET;
                case 9:  return SUMMON_NORMAL;
                case 10: return SUMMON_INSTANT;
                case 11: return SUMMON_TEMP;
                case 12: return CAST_ON_ALLPLAYERS;
                case 13: return CAST_ON_FRENDLY;
                case 14: return CAST_ON_FRENDLY_LOWHP;
                case 15: return CAST_ON_RANDOM_POINT;
                case 16: return CAST_ON_RANDOM_PLAYER;
                case 17: return APPLY_AURA_ALLPLAYERS;
                case 18: return FORCE_CAST;
                case 19: return SPELLTABLEPARM_NUMBER;
     default: return DO_NOTHING;
     };
};

CanCastResult BSWScriptedAI::_BSWDoCast(uint8 m_uiSpellIdx, Unit* pTarget)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    if (!pTarget || !pTarget->IsInMap(m_creature) || !pTarget->isAlive() && pTarget != (Unit*)m_creature)
    {
       error_log("BSW: warning - failed casting bugged spell number %u - no target or target not in map",pSpell->m_uiSpellEntry[currentDifficulty]);
       return CAST_FAIL_OTHER;
    }

    debug_log("BSW: Casting bugged spell number %u type %u",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);

    pTarget->InterruptNonMeleeSpells(false);

    pTarget->CastSpell(pTarget, pSpell->m_uiSpellEntry[currentDifficulty], false);

    return CAST_OK;
};

CanCastResult BSWScriptedAI::_BSWDoForceCast(uint8 m_uiSpellIdx, Unit* pTarget)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    if (!pTarget || !pTarget->IsInMap(m_creature) || !pTarget->isAlive() && pTarget != (Unit*)m_creature)
    {
       error_log("BSW: warning - failed forced casting spell number %u - no target or target not in map",pSpell->m_uiSpellEntry[currentDifficulty]);
       return CAST_FAIL_OTHER;
    }

    debug_log("BSW: Forced casting spell number %u ",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);

    pTarget->InterruptNonMeleeSpells(false);

    pTarget->CastSpell(m_creature, pSpell->m_uiSpellEntry[currentDifficulty], true);

    return CAST_OK;
};

void BSWScriptedAI::_fillEmptyDataField()
{
    for (uint8 i = 0; i < bossSpellCount(); ++i)
        for (uint8 j = 1; j < DIFFICULTY_LEVELS; ++j)
        {
            if (m_BSWRecords[i].m_uiSpellEntry[j] == 0)
            {
                SpellEntry const* spell = GetSpellEntryByDifficulty(m_BSWRecords[i].m_uiSpellEntry[0],(Difficulty)j, true);
                if (spell)
                    m_BSWRecords[i].m_uiSpellEntry[j] = spell->Id;
                else m_BSWRecords[i].m_uiSpellEntry[j] = m_BSWRecords[i].m_uiSpellEntry[j-1];
            }

            if (m_BSWRecords[i].m_uiSpellTimerMin[j] == 0)
                   m_BSWRecords[i].m_uiSpellTimerMin[j] = m_BSWRecords[i].m_uiSpellTimerMin[j-1];

            if (m_BSWRecords[i].m_uiSpellTimerMax[j] == 0)
                   m_BSWRecords[i].m_uiSpellTimerMax[j] = m_BSWRecords[i].m_uiSpellTimerMax[j-1];

            if (m_BSWRecords[i].m_uiSpellData[j] == 0)
                   m_BSWRecords[i].m_uiSpellData[j] = m_BSWRecords[i].m_uiSpellData[j-1];
        };
};

Unit* BSWScriptedAI::_doSummon(uint8 m_uiSpellIdx, TempSummonType summontype, uint32 delay)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    debug_log("BSW: Summoning creature number %u type %u despawn delay %u",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget, delay);

    if (pSpell->LocData.z <= 1.0f)
    {
        float fPosX, fPosY, fPosZ;
        if (!m_creature->IsPositionValid())
            {
               error_log("BSW: FAILED summoning creature, creature %u has invalid position",m_creature->GetEntry());
               return NULL;
            }
        m_creature->GetPosition(fPosX, fPosY, fPosZ);
        m_creature->GetRandomPoint(fPosX, fPosY, fPosZ, urand((uint32)pSpell->LocData.x, (uint32)pSpell->LocData.y), fPosX, fPosY, fPosZ);
        return m_creature->SummonCreature(pSpell->m_uiSpellEntry[currentDifficulty], fPosX, fPosY, fPosZ+0.8f, 0, summontype, delay);
    }
    else return m_creature->SummonCreature(pSpell->m_uiSpellEntry[currentDifficulty], pSpell->LocData.x, pSpell->LocData.y, pSpell->LocData.z, 0, summontype, delay);
};

Unit* BSWScriptedAI::_doSummonAtPosition(uint8 m_uiSpellIdx, float fPosX, float fPosY, float fPosZ)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    switch (pSpell->m_CastTarget)
    {
        case SUMMON_NORMAL:
             return _doSummonAtPosition(pSpell->m_uiSpellEntry[currentDifficulty], TEMPSUMMON_CORPSE_TIMED_DESPAWN, 0, fPosX, fPosY, fPosZ);
             break;

        case SUMMON_TEMP:
             return _doSummonAtPosition(pSpell->m_uiSpellEntry[currentDifficulty], TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, urand(pSpell->m_uiSpellTimerMin[currentDifficulty],pSpell->m_uiSpellTimerMax[currentDifficulty]), fPosX, fPosY, fPosZ);
             break;

        case SUMMON_INSTANT:
             return _doSummonAtPosition(pSpell->m_uiSpellEntry[currentDifficulty], TEMPSUMMON_MANUAL_DESPAWN, urand(pSpell->m_uiSpellTimerMin[currentDifficulty],pSpell->m_uiSpellTimerMax[currentDifficulty]), fPosX, fPosY, fPosZ);
             break;

        default:
             break;
    }
    error_log("BSW: FAILED creature number %u type %u ",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);
    return NULL;
};

Unit* BSWScriptedAI::_doSummonAtPosition(uint32 guid, TempSummonType summontype, uint32 delay, float fPosX, float fPosY, float fPosZ)
{

    debug_log("BSW: Summoning creature number %u  despawn delay %u at position %f %f %f", guid, delay, fPosX, fPosY, fPosZ);

    return m_creature->SummonCreature(guid, fPosX, fPosY, fPosZ, 0, summontype, delay);
};

bool BSWScriptedAI::_doRemove(uint8 m_uiSpellIdx, Unit* pTarget, uint8 index)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    debug_log("BSW: Removing effects of spell %u type %u",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);

        switch (pSpell->m_CastTarget)
        {
                case DO_NOTHING:
                         return true;
                     break;

                case SUMMON_NORMAL:
                case SUMMON_TEMP:
                case SUMMON_INSTANT:
                         return false;
                     break;

                case CAST_ON_SELF:
                case APPLY_AURA_SELF:
                         pTarget = m_creature;
                     break;

                case CAST_ON_SUMMONS:
                case CAST_ON_VICTIM:
                case CAST_ON_BOTTOMAGGRO:
                case CAST_ON_TARGET:
                case FORCE_CAST:
                case APPLY_AURA_TARGET:
                         if (!pTarget)
                             return false;
                     break;

                case CAST_ON_RANDOM:
                case CAST_ON_RANDOM_PLAYER:
                case APPLY_AURA_ALLPLAYERS:
                case CAST_ON_ALLPLAYERS:
                         _doRemoveFromAll(m_uiSpellIdx);
                         return true;
                     break;

                  default:
                         debug_log("BSW: FAILED Removing effects of spell %u type %u - unsupported type",pSpell->m_uiSpellEntry[currentDifficulty], pSpell->m_CastTarget);
                         return false;
                     break;
        }

    return _doRemove(pSpell->m_uiSpellEntry[currentDifficulty], pTarget, index);

};

bool BSWScriptedAI::_doRemove(uint32 SpellID, Unit* pTarget, uint8 index)
{

    if (!_hasAura(SpellID, pTarget)) return false;

        if (index == EFFECT_INDEX_ALL)
        {
            pTarget->RemoveAurasDueToSpell(SpellID);
        }
        else if (_auraCount(SpellID,pTarget,(SpellEffectIndex)index) > 1)
        {
            if (SpellAuraHolder* holder = pTarget->GetSpellAuraHolder(SpellID, pTarget->GetGUID()))
            {
                if (holder->ModStackAmount(-1))
                {
                    pTarget->RemoveSpellAuraHolder(holder, AURA_REMOVE_BY_DISPEL);
                } else return false;
            }
        }
        else
            pTarget->RemoveAurasDueToSpell(SpellID);

    debug_log("BSW: Removed effects of spell %u index %u",SpellID, index);
    return true;
};

bool BSWScriptedAI::_doRemoveFromAll(uint8 m_uiSpellIdx)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];
    return _doRemoveFromAll(pSpell->m_uiSpellEntry[currentDifficulty]);
};

bool BSWScriptedAI::_doRemoveFromAll(uint32 SpellID)
{
    Map* pMap = m_creature->GetMap();
    Map::PlayerList const& pPlayers = pMap->GetPlayers();
    if (!pPlayers.isEmpty())
    {
        debug_log("BSW: Removing effects of spell %u from all players",SpellID);

        for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
        {
            Unit* pTarget = itr->getSource();
            if (pTarget && pTarget->IsInWorld())
            {
                pTarget->RemoveAurasDueToSpell(SpellID);
                if (Pet* pPet = pTarget->GetPet())
                    pPet->RemoveAurasDueToSpell(SpellID);
            }
        }
        return true;
    }
    else
    {
        debug_log("BSW: Removing effects of spell %u from all players FAILED - no players in map",SpellID);
        return false;
    }
};

bool BSWScriptedAI::_doAura(uint8 m_uiSpellIdx, Unit* pTarget)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    if (!pTarget)
        pTarget = m_creature;

    bool result = true;

    for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
        result = result && _doAura(m_uiSpellIdx, pTarget, SpellEffectIndex(i), !i);

    return result;

};

bool BSWScriptedAI::_doAura(uint32 SpellID, Unit* pTarget)
{
    if (!pTarget)
        pTarget = m_creature;

    bool result = true;

    for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
        result = result && _doAura(SpellID, pTarget, SpellEffectIndex(i), 0, !i);

    return result;

};


bool BSWScriptedAI::_doAura(uint8 m_uiSpellIdx, Unit* pTarget, SpellEffectIndex index, bool isStack)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    if (!pTarget)
        pTarget = m_creature;

    return _doAura(pSpell->m_uiSpellEntry[currentDifficulty], pTarget, index, pSpell->varData, isStack);

};


bool BSWScriptedAI::_doAura(uint32 SpellID, Unit* pTarget, SpellEffectIndex index, int32 basepoint, bool isStack)
{
    if (!pTarget || !pTarget->IsInMap(m_creature) || !pTarget->isAlive())
    {
        error_log("BSW: FAILED adding aura of spell number %u - no target or target not in map or target is dead",SpellID);
        return false;
    }

    if (_hasAura(SpellID,pTarget))
         debug_log("BSW: adding aura stack from spell %u index %u",SpellID, index);
    else debug_log("BSW: adding new aura from spell %u index %u",SpellID, index);

    SpellEntry const *spell = (SpellEntry *)GetSpellStore()->LookupEntry(SpellID);

    if (spell)
    {
        if (IsSpellAppliesAura(spell, (1 << EFFECT_INDEX_0) | (1 << EFFECT_INDEX_1) | (1 << EFFECT_INDEX_2)) || IsSpellHaveEffect(spell, SPELL_EFFECT_PERSISTENT_AREA_AURA))
        {
            int32 _basepoint = basepoint ?  basepoint - 1 : spell->CalculateSimpleValue(index);

            bool addedToExisting = true;

            SpellAuraHolder* holder = pTarget->GetSpellAuraHolder(SpellID, pTarget->GetGUID());

            Aura* aura = NULL;

            if (!holder)
            {
                holder = CreateSpellAuraHolder(spell, pTarget, pTarget);
                addedToExisting = false;
            }


            if (aura = holder->GetAuraByEffectIndex(index))
            {
                if (isStack)
                    holder->ModStackAmount(1);
            }
            else
            {
                aura = CreateAura(spell, index, &_basepoint, holder, pTarget);
                holder->SetAuraDuration(aura->GetAuraMaxDuration());
                holder->AddAura(aura, index);
            }

            if (addedToExisting)
            {
                pTarget->AddAuraToModList(aura);
                holder->SetInUse(true);
                aura->ApplyModifier(true,true);
                holder->SetInUse(false);
            }
            else
                pTarget->AddSpellAuraHolder(holder);

            return true;
        }
    }

    error_log("BSW: FAILED adding aura from spell %u index %u",SpellID, index);

    return false;
};

CanCastResult BSWScriptedAI::_DoCastSpellIfCan(Unit* pTarget, uint32 uiSpell, uint32 uiCastFlags, uint64 uiOriginalCasterGUID)
{
    if (!pTarget || !pTarget->IsInWorld() || !pTarget->IsInMap(m_creature)|| !pTarget->isAlive() && pTarget != (Unit*)m_creature) return CAST_FAIL_OTHER;

    return DoCastSpellIfCan(pTarget,uiSpell,uiCastFlags,uiOriginalCasterGUID);
}

// Not threat-based select random player function

Unit* BSWScriptedAI::_doSelect(uint32 SpellID, bool spellsearchtype, float range, bool includeVictim)
{
    Map* pMap = m_creature->GetMap();
    Map::PlayerList const &pList = pMap->GetPlayers();
        if (pList.isEmpty()) return NULL;

    std::vector<Unit*> _list;
    _list.clear();

    for(Map::PlayerList::const_iterator i = pList.begin(); i != pList.end(); ++i)
        if (Player* player = i->getSource())
        {
            if (player->isGameMaster()) continue;

            if (!player->IsInMap(m_creature)) continue;

            if (player == m_creature->getVictim() && !includeVictim) continue;

            if (player->isAlive()
                && player->IsWithinDistInMap(m_creature, range)
                && (SpellID == 0 || (player->HasAura(SpellID) == spellsearchtype))
                )
                _list.push_back((Unit*)player);
        }

    debug_log("BSW: search random player with criteria = %u, found %u players.",SpellID,_list.size());

    if (_list.empty()) return NULL;
        else return _list[urand(0,_list.size() - 1)];
};

Creature* BSWScriptedAI::doSelectNearestCreature(uint32 id, float range)
{
    Creature* pTarget = GetClosestCreatureWithEntry(m_creature, id, range);

    if (pTarget && pTarget->IsInMap(m_creature) && pTarget != m_creature && pTarget->isAlive())
    {
        debug_log("BSW: search creature %u in range %f - found it.",id,range);
        return pTarget;
    }
    else
    {
        debug_log("BSW: search creature %u in range %f - NOT found.",id,range);
        return NULL;
    }
}

uint32 BSWScriptedAI::_getSpellData(uint8 m_uiSpellIdx)
{
    BSWRecord* pSpell = &m_BSWRecords[m_uiSpellIdx];

    return pSpell->m_uiSpellData[currentDifficulty];
};

bool BSWScriptedAI::doCastAll(uint32 diff)
{

    uint8 succesfulCast = 0;

    if (bossSpellCount() > 0)
    {
        for(uint8 i = 0; i < bossSpellCount(); ++i)
            if (_QuerySpellPeriod(i, diff))
                if (_BSWSpellSelector(i) == CAST_OK)
                    ++succesfulCast;

        if (succesfulCast)
            debug_log("BSW: Casting all spells for creature %u done. Successful casted %u spells from %u.", m_creature->GetEntry(),succesfulCast,bossSpellCount());
    }
    else
    {
        error_log("BSW: Casting all spells for creature %u failed. Database has no spells.", m_creature->GetEntry());
    }

    return (succesfulCast >= 1) ? true : false;

};

bool BSWScriptedAI::_isDifficultyInMask(uint8 mask)
{
    return ((1 << (uint8)currentDifficulty) & mask);
};

#endif