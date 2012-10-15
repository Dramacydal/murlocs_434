/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
// Gormok - Firebomb not implemented, timers need correct
// Snakes - Underground phase not worked, timers need correct
// Icehowl - Trample&Crash event not implemented, timers need correct

/* ScriptData
SDName: northrend_beasts
SD%Complete: 90% 
SDComment: by /dev/rsa
SDCategory:
EndScriptData */

// Snobols have to jump into players. Need core support
// Some spells have wrong target, invalid.

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Emotes
{
    EMOTE_SUMMON_SNOBOLD        = -1713601,
    EMOTE_ENRAGE                = -1713504,
    EMOTE_TRAMPLE               = -1713506,
    EMOTE_STAGGERED             = -1713507,
    EMOTE_CRASH                 = -1713508,
};
enum Equipment
{
    EQUIP_MAIN                  = 47505,
    EQUIP_DONE                  = EQUIP_NO_CHANGE,
};

enum Summons
{
    NPC_SNOBOLD_VASSAL          = 34800,
    NPC_SLIME_POOL              = 35176,
    NPC_FIRE_BOMB               = 34854,
};

enum BossSpells
{
    // Gormok
    SPELL_IMPALE                = 66331,
    SPELL_STAGGERING_STOMP      = 67648,
    SPELL_RISING_ANGER          = 66636,
    SPELL_SNOBOLLED             = 66406,
    SPELL_BATTER                = 66408,
    SPELL_FIRE_BOMB             = 66313,
    SPELL_FIRE_BOMB_DOT         = 66318,
    SPELL_HEAD_CRACK            = 66407,

    // Acidmaw & Dreadscale
    SPELL_EMERGE                = 66947,
    SPELL_SUBMERGE              = 66948,
    SPELL_ENRAGE                = 68335,
    SPELL_ACID_SPIT             = 66880,
    SPELL_PARALYTIC_SPRAY       = 66901,
    SPELL_PARALYTIC_TOXIN       = 66823,
    SPELL_ACID_SPEW             = 66819,
    SPELL_PARALYTIC_BITE        = 66824,
    SPELL_SWEEP                 = 66794,
    SPELL_FIRE_SPIT             = 66796,
    SPELL_MOLTEN_SPEW           = 66821,
    SPELL_BURNING_BITE          = 66879,
    SPELL_BURNING_SPRAY         = 66902,
    SPELL_BURNING_BILE          = 66869,
    SPELL_AUTO_GROW             = 62559,
    SPELL_SLIME_POOL            = 66883,
    SPELL_SLIME_POOL_AURA       = 66882,
    SPELL_SLIME_POOL_VISUAL     = 63084,
    SPELL_CHECK_ACHIEV          = 68523,

    // Icehowl
    SPELL_FEROCIOUS_BUTT        = 66770,
    SPELL_MASSIVE_CRASH         = 66683,
    SPELL_WHIRL                 = 67345,
    SPELL_ARCTIC_BREATH         = 66689,
    SPELL_TRAMPLE               = 66734,
    SPELL_ADRENALINE            = 68667,
    SPELL_FROTHING_RAGE         = 66759,
    SPELL_STAGGERED_DAZE        = 66758,
};

enum Models
{
    MODEL_ACIDMAW_STATIONARY    = 29815,
    MODEL_ACIDMAW_MOBILE        = 29816,
    MODEL_DREADSCALE_STATIONARY = 26935,
    MODEL_DREADSCALE_MOBILE     = 24564,
};

enum Phases
{
    // Snakes
    PHASE_EMERGED               = 0,
    PHASE_SUBMERGED             = 1,

    // Icehowl
    PHASE_NORMAL                = 0,
    PHASE_MOVING                = 1,
    PHASE_TRAMPLE               = 2,
};

enum Points
{
    POINT_CENTER                = 0,
    POINT_TARGET                = 1,
};

struct MANGOS_DLL_DECL boss_gormokAI : public ScriptedAI
{
    boss_gormokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_uiMapDifficulty;
    Unit* pFocus;

    bool m_bIsHeroic;
    bool m_bIs25Man;

    uint8 SnoboldsCount;

    uint32 m_uiImpaleTimer;
    uint32 m_uiStaggeringStompTimer;
    uint32 m_uiSummonSnoboldTimer;

    std::list<ObjectGuid> mobsGUIDList;

    void Reset() 
    {
        if (!m_pInstance) 
            return;

        SetEquipmentSlots(false, EQUIP_MAIN, -1, -1);
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetInCombatWithZone();

        SnoboldsCount = m_bIs25Man? 5 : 4;

        pFocus = NULL;

        m_uiImpaleTimer           = urand(15000, 30000);
        m_uiStaggeringStompTimer  = urand(20000, 25000);
        m_uiSummonSnoboldTimer    = urand(20000, 30000);

        mobsGUIDList.clear();
    }

    void SummonAdds()
    {
        if (!m_pInstance)
            return;

        for (uint32 i = 0; i < SnoboldsCount; i++)
        {
            Unit *pTemp = m_creature->SummonCreature(NPC_SNOBOLD_VASSAL, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
            pTemp->EnterVehicle(m_creature->GetVehicleKit(), i);
            mobsGUIDList.push_back(pTemp->GetObjectGuid());
        }
    }

    void ThrowAdd(Unit *pTarget)
    {
        if (!m_pInstance || mobsGUIDList.empty())
            return;

        for(std::list<ObjectGuid>::iterator itr = mobsGUIDList.begin(); itr != mobsGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->isAlive())
                {
                    pTemp->ExitVehicle();
                    pTemp->SetInCombatWithZone();
                    pTemp->AddThreat(pTarget, 10000.0f);
                    pTemp->AI()->AttackStart(pTarget);
                    mobsGUIDList.remove(pTemp->GetObjectGuid());
                    break;
                }
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_DONE);
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, FAIL);

        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, GORMOK_IN_PROGRESS);
        SummonAdds();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiImpaleTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_IMPALE) == CAST_OK)
                m_uiImpaleTimer = 10000;
        }
        else
            m_uiImpaleTimer -= uiDiff;

        if (m_uiStaggeringStompTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STAGGERING_STOMP) == CAST_OK)
                m_uiStaggeringStompTimer = 20000;
        }
        else
            m_uiStaggeringStompTimer -= uiDiff;

        if (m_uiSummonSnoboldTimer <= uiDiff && SnoboldsCount > 0)
        {
            DoScriptText(EMOTE_SUMMON_SNOBOLD, m_creature);
            ThrowAdd(pFocus = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0));
            pFocus->_AddAura(SPELL_SNOBOLLED);
            m_creature->_AddAura(SPELL_RISING_ANGER);
            --SnoboldsCount;
            m_uiSummonSnoboldTimer = 20000;
        }
        else
            m_uiSummonSnoboldTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_gormok(Creature* pCreature)
{
    return new boss_gormokAI(pCreature);
}

struct MANGOS_DLL_DECL mob_snobold_vassalAI : public ScriptedAI
{
    mob_snobold_vassalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Unit *pFocus;

    uint32 m_uiBatterTimer;
    uint32 m_uiFireBombTimer;
    uint32 m_uiHeadCrackTimer;

    void Reset()
    {
        pFocus = NULL;

        m_uiBatterTimer         = 10000;
        m_uiFireBombTimer       = urand(10000, 20000);
        m_uiHeadCrackTimer      = urand(10000, 30000);

        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);
    }

    void Aggro(Unit *pWho)
    {
        if (!m_pInstance) 
            return;

        pFocus = pWho;
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (pFocus && pFocus->isAlive())
            pFocus->RemoveAurasDueToSpell(SPELL_SNOBOLLED);

        m_creature->ForcedDespawn(5000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL)
            m_creature->ForcedDespawn();

        if (m_uiBatterTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BATTER) == CAST_OK)
                m_uiBatterTimer = 15000;
        }
        else
            m_uiBatterTimer -= uiDiff;

        if (m_uiFireBombTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FIRE_BOMB) == CAST_OK)
            {
                float x, y, z;
                m_creature->GetPosition(x, y, z);

                if (Creature *pBomb = m_creature->SummonCreature(NPC_FIRE_BOMB, x, y, z, 0, TEMPSUMMON_TIMED_DESPAWN, 60000))
                    pBomb->CastSpell(pBomb, SPELL_FIRE_BOMB_DOT, true);
            }
            m_uiFireBombTimer = urand(10000, 20000);
        }
        else
            m_uiFireBombTimer -= uiDiff;

        if (m_uiHeadCrackTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_HEAD_CRACK) == CAST_OK)
                m_uiHeadCrackTimer = urand(10000, 30000);
        }
        else
            m_uiHeadCrackTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_snobold_vassal(Creature* pCreature)
{
    return new mob_snobold_vassalAI(pCreature);
}

struct MANGOS_DLL_DECL mob_fire_bombAI : public ScriptedAI
{
    mob_fire_bombAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        m_creature->SetInCombatWithZone();
        SetCombatMovement(false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == FAIL)
            m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_mob_fire_bomb(Creature* pCreature)
{
    return new mob_fire_bombAI(pCreature);
}

struct MANGOS_DLL_DECL boss_acidmawAI : public ScriptedAI
{
    boss_acidmawAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_crusader*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiAcidSpewTimer;
    uint32 m_uiParalyticBiteTimer;
    uint32 m_uiAcidSpitTimer;
    uint32 m_uiParalyticSprayTimer;
    uint32 m_uiSweepTimer;
    uint32 m_uiSlimePoolTimer;
    uint32 m_uiAchievTimer;

    bool m_bIsEnraged;
    bool m_bAchievFailed;

    void Reset()
    {
        m_uiPhaseTimer                  = 65000;
        m_uiAcidSpewTimer               = urand(10000, 20000);
        m_uiParalyticBiteTimer          = 15000;
        m_uiAcidSpitTimer               = 0;
        m_uiParalyticSprayTimer         = 15000;
        m_uiSweepTimer                  = 20000;
        m_uiSlimePoolTimer              = 10000;

        m_uiPhase                       = PHASE_SUBMERGED;

        m_uiAchievTimer                 = 10000;

        m_bIsEnraged                    = false;
        m_bAchievFailed                 = false;

        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetDisplayId(MODEL_ACIDMAW_STATIONARY);
        m_creature->SetSpeedRate(MOVE_WALK, 0);
        m_creature->SetSpeedRate(MOVE_RUN, 0);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) 
            return;

        if (Creature* pDreadscale = m_pInstance->GetSingleCreatureFromStorage(NPC_DREADSCALE))
        {
            if (!pDreadscale->isAlive())
                m_pInstance->SetData(TYPE_NORTHREND_BEASTS, SNAKES_DONE);
            else
                m_pInstance->SetData(TYPE_NORTHREND_BEASTS, SNAKES_SPECIAL);
        }

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL && !m_bAchievFailed)
            CheckAchiev();
    }

    void CheckAchiev()
    {
        Map* pMap = m_creature->GetMap();
        Map::PlayerList const& pPlayers = pMap->GetPlayers();
        if (!pPlayers.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
            {
                Unit *pTarget = itr->getSource();
                if (pTarget)
                    m_creature->CastSpell(pTarget, SPELL_CHECK_ACHIEV, true);
            }
        }
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        if (m_pInstance->GetData(TYPE_BEASTS) == IN_PROGRESS && m_pInstance->GetData(TYPE_NORTHREND_BEASTS) != FAIL)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, FAIL);

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_uiPhase)
        {
        case PHASE_EMERGED:
             if (m_uiAcidSpewTimer <= uiDiff)
             {
                 if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ACID_SPEW) == CAST_OK)
                     m_uiAcidSpewTimer = urand(15000, 30000);
             }
             else
                 m_uiAcidSpewTimer -= uiDiff;

             if (m_uiParalyticBiteTimer <= uiDiff)
             {
                 if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PARALYTIC_BITE) == CAST_OK)
                     m_uiParalyticBiteTimer = urand(15000, 30000);
             }
             else
                 m_uiParalyticBiteTimer -= uiDiff;

             if (m_uiSlimePoolTimer <= uiDiff)
             {
                 if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLIME_POOL) == CAST_OK)
                     m_uiSlimePoolTimer = 12000;
             }
             else
                 m_uiSlimePoolTimer -= uiDiff;

             if (m_uiPhaseTimer <= uiDiff)
             {
                 m_creature->SetDisplayId(MODEL_ACIDMAW_STATIONARY);
                 m_creature->SetSpeedRate(MOVE_WALK, 0);
                 m_creature->SetSpeedRate(MOVE_RUN, 0);
                 DoCastSpellIfCan(m_creature, SPELL_EMERGE);
                 m_uiPhase = PHASE_SUBMERGED;
                 m_uiPhaseTimer = 65000;
             }
             else
                 m_uiPhaseTimer -= uiDiff;

            break;
        case PHASE_SUBMERGED:
             if (m_uiAcidSpitTimer <= uiDiff)
             {
                 if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                 {
                     if (DoCastSpellIfCan(pTarget, SPELL_ACID_SPIT) == CAST_OK)
                         m_uiAcidSpitTimer = 1200;
                 }
             }
             else
                 m_uiAcidSpitTimer -= uiDiff;

             if (m_uiParalyticSprayTimer <= uiDiff)
             {
                 if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                 {
                     if(DoCastSpellIfCan(pTarget, SPELL_PARALYTIC_SPRAY, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                        m_uiParalyticSprayTimer = 20000;
                 }
             }
             else
                 m_uiParalyticSprayTimer -= uiDiff;

             if (m_uiSweepTimer <= uiDiff)
             {
                 if (DoCastSpellIfCan(m_creature, SPELL_SWEEP, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                 {
                     m_uiSweepTimer = 20000;
                     m_uiAcidSpitTimer = 3000;
                 }
             }
             else
                 m_uiSweepTimer -= uiDiff;

             if (m_uiPhaseTimer <= uiDiff)
             {
                 m_uiPhase = PHASE_EMERGED;
                 m_creature->RemoveAurasDueToSpell(SPELL_EMERGE);
                 m_creature->SetDisplayId(MODEL_ACIDMAW_MOBILE);
                 m_creature->SetSpeedRate(MOVE_WALK, 1);
                 m_creature->SetSpeedRate(MOVE_RUN, 1);
                 SetCombatMovement(true);
                 DoResetThreat();
                 m_uiPhaseTimer = 65000;
             }
             else
                 m_uiPhaseTimer -= uiDiff;

             break;
        }

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL && !m_bIsEnraged)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
            m_creature->SetDisplayId(MODEL_ACIDMAW_MOBILE);
            m_creature->SetSpeedRate(MOVE_WALK, 1);
            m_creature->SetSpeedRate(MOVE_RUN, 1);
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE, CAST_TRIGGERED);
            DoCastSpellIfCan(m_creature, SPELL_EMERGE);

            DoScriptText(EMOTE_ENRAGE, m_creature);

            m_bIsEnraged = true;
            m_uiPhase = PHASE_EMERGED;
        }

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL)
        {
            ++m_uiAchievTimer;
            if (m_uiAchievTimer < uiDiff)
                m_bAchievFailed = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_acidmaw(Creature* pCreature)
{
    return new boss_acidmawAI(pCreature);
}

struct MANGOS_DLL_DECL boss_dreadscaleAI : public ScriptedAI
{
    boss_dreadscaleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_crusader*)pCreature->GetInstanceData();
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBurningBiteTimer;
    uint32 m_uiMoltenSpewTimer;
    uint32 m_uiFireSpitTimer;
    uint32 m_uiBurningSprayTimer;
    uint32 m_uiSweepTimer;
    uint32 m_uiSlimePoolTimer;
    uint32 m_uiAchievTimer;

    bool m_bIsEnraged;
    bool m_bAchievFailed;

    void Reset()
    {
        m_uiPhaseTimer                  = 65000;
        m_uiBurningBiteTimer            = urand(10000, 20000);
        m_uiMoltenSpewTimer             = urand(15000, 30000);
        m_uiFireSpitTimer               = 1000;
        m_uiBurningSprayTimer           = 15000;
        m_uiSweepTimer                  = 20000;
        m_uiSlimePoolTimer              = 10000;
        m_uiAchievTimer                 = 10000;

        m_uiPhase                       = PHASE_EMERGED;

        m_bIsEnraged                    = false;
        m_bAchievFailed                 = false;

        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetDisplayId(MODEL_DREADSCALE_MOBILE);
        m_creature->SetSpeedRate(MOVE_WALK, 1);
        m_creature->SetSpeedRate(MOVE_RUN, 1);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) 
            return;
            
        if (Creature *pSister = m_pInstance->GetSingleCreatureFromStorage(NPC_ACIDMAW))
        {
            if (!pSister->isAlive())
                m_pInstance->SetData(TYPE_NORTHREND_BEASTS, SNAKES_DONE);
            else 
                m_pInstance->SetData(TYPE_NORTHREND_BEASTS, SNAKES_SPECIAL);
        }

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL && !m_bAchievFailed)
            CheckAchiev();
    }

    void CheckAchiev()
    {
        Map* pMap = m_creature->GetMap();
        Map::PlayerList const& pPlayers = pMap->GetPlayers();
        if (!pPlayers.isEmpty())
        {
            for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
            {
                Unit *pTarget = itr->getSource();
                if (pTarget)
                    m_creature->CastSpell(pTarget, SPELL_CHECK_ACHIEV, true);
            }
        }
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        if (m_pInstance->GetData(TYPE_BEASTS) == IN_PROGRESS && m_pInstance->GetData(TYPE_NORTHREND_BEASTS) != FAIL)
            m_pInstance->SetData(TYPE_NORTHREND_BEASTS, FAIL);

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_uiPhase)
        {
        case PHASE_EMERGED:
             if (m_uiBurningBiteTimer <= uiDiff)
             {
                 if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BURNING_BITE) == CAST_OK)
                     m_uiBurningBiteTimer = urand(15000, 30000);
             }
             else
                 m_uiBurningBiteTimer -= uiDiff;

             if (m_uiMoltenSpewTimer <= uiDiff)
             {
                 if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MOLTEN_SPEW) == CAST_OK)
                     m_uiMoltenSpewTimer = urand(15000, 30000);
             }
             else
                 m_uiMoltenSpewTimer -= uiDiff;

             if (m_uiSlimePoolTimer <= uiDiff)
             {
                 if(DoCastSpellIfCan(m_creature->getVictim(), SPELL_SLIME_POOL) == CAST_OK)
                     m_uiSlimePoolTimer = 12000;
             }
             else
                 m_uiSlimePoolTimer -= uiDiff;

             if (m_uiPhaseTimer <= uiDiff)
             {
                 m_uiPhase = PHASE_SUBMERGED;
                 DoCastSpellIfCan(m_creature, SPELL_EMERGE);
                 m_creature->SetDisplayId(MODEL_DREADSCALE_STATIONARY);
                 m_creature->SetSpeedRate(MOVE_WALK, 0);
                 m_creature->SetSpeedRate(MOVE_RUN, 0);
                 m_uiPhaseTimer = 65000;
             }
             else
                 m_uiPhaseTimer -= uiDiff;

            break;
        case PHASE_SUBMERGED:
             if (m_uiFireSpitTimer <= uiDiff)
             {
                 if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                 {
                     if (DoCastSpellIfCan(pTarget, SPELL_FIRE_SPIT) == CAST_OK)
                         m_uiFireSpitTimer = 1200;
                 }
             }
             else
                 m_uiFireSpitTimer -= uiDiff;

             if (m_uiBurningSprayTimer <= uiDiff)
             {
                 if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                 {
                     if (DoCastSpellIfCan(pTarget, SPELL_BURNING_SPRAY, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                         m_uiBurningSprayTimer = 20000;
                 }
             }
             else
                 m_uiBurningSprayTimer -= uiDiff;

             if (m_uiSweepTimer <= uiDiff)
             {
                 if (DoCastSpellIfCan(m_creature, SPELL_SWEEP, CAST_INTERRUPT_PREVIOUS) == CAST_OK)
                 {
                     m_uiSweepTimer = 20000;
                     m_uiFireSpitTimer = 3000;
                 }
             }
             else
                 m_uiSweepTimer -= uiDiff;

             if (m_uiPhaseTimer <= uiDiff)
             {
                 m_uiPhase = PHASE_EMERGED;
                 m_creature->SetDisplayId(MODEL_DREADSCALE_MOBILE);
                 m_creature->SetSpeedRate(MOVE_WALK, 1);
                 m_creature->SetSpeedRate(MOVE_RUN, 1);
                 SetCombatMovement(true);
                 m_creature->RemoveAurasDueToSpell(SPELL_EMERGE);
                 DoResetThreat();
                 m_uiPhaseTimer = 65000;
             }
             else
                 m_uiPhaseTimer -= uiDiff;

            break;
        }

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL && !m_bIsEnraged)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_SUBMERGE);
            m_creature->SetDisplayId(MODEL_DREADSCALE_MOBILE);
            m_creature->SetSpeedRate(MOVE_WALK, 1);
            m_creature->SetSpeedRate(MOVE_RUN, 1);
            DoCastSpellIfCan(m_creature, SPELL_EMERGE, CAST_TRIGGERED);

            DoScriptText(EMOTE_ENRAGE, m_creature);
            DoCastSpellIfCan(m_creature, SPELL_ENRAGE, CAST_TRIGGERED);

            m_bIsEnraged = true;
        }

        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_SPECIAL)
        {
            ++m_uiAchievTimer;
            if (m_uiAchievTimer < uiDiff)
                m_bAchievFailed = true;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dreadscale(Creature* pCreature)
{
    return new boss_dreadscaleAI(pCreature);
}

struct MANGOS_DLL_DECL mob_slime_poolAI : public ScriptedAI
{
    mob_slime_poolAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance *m_pInstance;

    uint32 m_uiIncreaseSizeTimer;

    float m_fSize;
    bool m_bIsCloudCasted;

    void Reset()
    {
        if (!m_pInstance) 
            return;

        m_uiIncreaseSizeTimer = 0;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        //m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetInCombatWithZone();
        DoCastSpellIfCan(m_creature, SPELL_SLIME_POOL_AURA, CAST_TRIGGERED);
        DoCastSpellIfCan(m_creature, SPELL_AUTO_GROW, CAST_TRIGGERED);

        SetCombatMovement(false);

        m_fSize = m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
        m_bIsCloudCasted = false;
    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_NORTHREND_BEASTS) == SNAKES_DONE)
            m_creature->ForcedDespawn();

        if (!m_bIsCloudCasted) 
        {
            m_creature->CastSpell(m_creature, SPELL_SLIME_POOL_VISUAL, false);
            m_bIsCloudCasted = true;
        }

        if (m_fSize >= 6.0f) 
            m_creature->ForcedDespawn();
    }

};

CreatureAI* GetAI_mob_slime_pool(Creature* pCreature)
{
    return new mob_slime_poolAI(pCreature);
}

struct MANGOS_DLL_DECL boss_icehowlAI : public ScriptedAI
{
    boss_icehowlAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_trial_of_the_crusader*)pCreature->GetInstanceData());
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;

    uint32 m_uiFerociousButtTimer;
    uint32 m_uiArcticBreathTimer;
    uint32 m_uiWhirlTimer;
    uint32 m_uiMassiveCrashTimer;
    uint32 m_uiWaitTimer;
    uint32 m_uiPhaseTimer;
    uint32 m_uiPhase;
    uint32 m_uiCheckTimer;

    float fPosX, fPosY, fPosZ;
    Unit *pFocus;
    bool m_bAchievFailed;

    std::list<Creature*> vassalsEntryList;

    void Reset() 
    {
        if (!m_pInstance) 
            return;

        m_uiFerociousButtTimer         = 15000;
        m_uiArcticBreathTimer          = 25000;
        m_uiWhirlTimer                 = 20000;
        m_uiMassiveCrashTimer          = 30000;
        m_uiPhase                      = PHASE_NORMAL;
        m_uiCheckTimer                 = 0; // if not vassals at encounter start only check once.

        m_creature->SetRespawnDelay(7*DAY);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetSpeedRate(MOVE_WALK, 3.0f);
        m_creature->SetSpeedRate(MOVE_RUN, 3.0f);

        m_bAchievFailed                = false;

        pFocus = NULL;

        vassalsEntryList.clear();
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData)
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiData == POINT_CENTER)
        {
            if (m_uiPhase == PHASE_MOVING)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_MASSIVE_CRASH) == CAST_OK)
                {
                    pFocus = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                    DoScriptText(EMOTE_TRAMPLE, m_creature, pFocus);
                    m_creature->SetSpeedRate(MOVE_WALK, 6.0f);
                    m_creature->SetSpeedRate(MOVE_RUN, 6.0f);
                    m_uiWaitTimer = 5000;
                    m_uiPhaseTimer = 8000;
                    m_uiPhase = PHASE_TRAMPLE;
                }
            }
        }
        else if (uiData == POINT_TARGET)
        {
            if (m_uiPhase == PHASE_MOVING)
            {
                if (pFocus && pFocus->isAlive() && m_creature->IsWithinDistInMap(pFocus, 10.0f))  // I think that it should be any player not only focused.
                {
                    DoCastSpellIfCan(m_creature, SPELL_TRAMPLE, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_FROTHING_RAGE, CAST_TRIGGERED);
                }
                else
                {
                    DoCastSpellIfCan(m_creature, SPELL_STAGGERED_DAZE, CAST_TRIGGERED);
                    DoScriptText(EMOTE_STAGGERED, m_creature);
                }

                if (m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetSpeedRate(MOVE_WALK, 3.0f);
                m_creature->SetSpeedRate(MOVE_RUN, 3.0f);
                SetCombatMovement(true);
                m_uiPhase = PHASE_NORMAL;
                pFocus = NULL;
            }
        }
    }

    void CheckAchiev()
    {
        vassalsEntryList.clear();
        GetCreatureListWithEntryInGrid(vassalsEntryList, m_creature, NPC_SNOBOLD_VASSAL, 250.0f);

        if (vassalsEntryList.empty())
        {
            m_pInstance->SetSpecialAchievementCriteria(TYPE_UPPER_BACK_PAIN, false);
            m_bAchievFailed = true;
            return;
        }

        if (vassalsEntryList.size()-1 >= 2)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_UPPER_BACK_PAIN, true);
        else
        {
            m_pInstance->SetSpecialAchievementCriteria(TYPE_UPPER_BACK_PAIN, false);
            m_bAchievFailed = true;
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, ICEHOWL_DONE);
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, FAIL);
        
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();
        m_pInstance->SetData(TYPE_NORTHREND_BEASTS, ICEHOWL_IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bAchievFailed)
        {
            if (m_uiCheckTimer < uiDiff)
            {
                CheckAchiev();
                m_uiCheckTimer = 500;
            }
            else
                m_uiCheckTimer -= uiDiff;
        }

        switch (m_uiPhase)
        {
            case PHASE_NORMAL:
                 if (m_uiFerociousButtTimer <= uiDiff)
                 {
                     if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FEROCIOUS_BUTT) == CAST_OK)
                         m_uiFerociousButtTimer = urand(15000, 30000);
                 }
                 else
                     m_uiFerociousButtTimer -= uiDiff;

                 if (m_uiArcticBreathTimer <= uiDiff)
                 {
                     if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                         if (DoCastSpellIfCan(pTarget, SPELL_ARCTIC_BREATH) == CAST_OK)
                             m_uiArcticBreathTimer = urand(25000, 40000);
                 }
                 else
                     m_uiArcticBreathTimer -= uiDiff;

                 if (m_uiWhirlTimer <= uiDiff)
                 {
                     if (DoCastSpellIfCan(m_creature, SPELL_WHIRL) == CAST_OK)
                         m_uiWhirlTimer = urand(20000, 30000);
                 }
                 else
                     m_uiWhirlTimer -= uiDiff;

                 if (m_uiMassiveCrashTimer <= uiDiff)
                 {
                     m_creature->AttackStop();
                     m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                     SetCombatMovement(false);
                     m_creature->GetMotionMaster()->Clear();
                     m_creature->GetMotionMaster()->MovePoint(POINT_CENTER, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                     m_uiPhase = PHASE_MOVING;
                     m_uiMassiveCrashTimer = 40000;
                 }
                 else
                     m_uiMassiveCrashTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            case PHASE_TRAMPLE:
                if (m_creature->IsNonMeleeSpellCasted(true))
                    return;

                if (m_uiWaitTimer < uiDiff)
                {
                    if (pFocus && pFocus->isAlive())
                    {
                        pFocus->GetPosition(fPosX, fPosY, fPosZ);
                        m_creature->SetFacingToObject(pFocus);
                        m_uiWaitTimer = 10000; // For count only once
                    }
                }
                else
                    m_uiWaitTimer -= uiDiff;

                if (m_uiPhaseTimer <= uiDiff)
                {
                    if (pFocus && pFocus->isAlive())
                    {
                        m_creature->GetMotionMaster()->Clear();
                        m_creature->GetMotionMaster()->MovePoint(POINT_TARGET, fPosX, fPosY, fPosZ);
                        DoScriptText(EMOTE_CRASH, m_creature);
                        m_uiPhase = PHASE_MOVING;
                    }
                }
                else
                    m_uiPhaseTimer -= uiDiff;
                break;
            case PHASE_MOVING:
                break;
        }
    }
};

CreatureAI* GetAI_boss_icehowl(Creature* pCreature)
{
    return new boss_icehowlAI(pCreature);
}

void AddSC_northrend_beasts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_gormok";
    pNewScript->GetAI = &GetAI_boss_gormok;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_acidmaw";
    pNewScript->GetAI = &GetAI_boss_acidmaw;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_dreadscale";
    pNewScript->GetAI = &GetAI_boss_dreadscale;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_icehowl";
    pNewScript->GetAI = &GetAI_boss_icehowl;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_snobold_vassal";
    pNewScript->GetAI = &GetAI_mob_snobold_vassal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fire_bomb";
    pNewScript->GetAI = &GetAI_mob_fire_bomb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_slime_pool";
    pNewScript->GetAI = &GetAI_mob_slime_pool;
    pNewScript->RegisterSelf();
}
