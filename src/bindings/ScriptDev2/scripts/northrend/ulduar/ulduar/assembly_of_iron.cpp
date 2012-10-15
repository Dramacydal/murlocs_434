/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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

/* ScriptData
SDName: boss_iron_council
SD%Complete: 
SDComment: Achievment(Can't Do That While Stunned,
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //yells
    SAY_MOLGEIM_AGGRO       = -1603060,
    SAY_MOLGEIM_DEATH1      = -1603061,
    SAY_MOLGEIM_DEATH2      = -1603062,
    SAY_MOLGEIM_DEATH_RUNE  = -1603063,
    SAY_MOLGEIM_SUMMON      = -1603064,
    SAY_MOLGEIM_SLAY1       = -1603065,
    SAY_MOLGEIM_SLAY2       = -1603066,
    SAY_MOLGEIM_BERSERK     = -1603067,

    SAY_STEEL_AGGRO         = -1603070,
    SAY_STEEL_DEATH1        = -1603071,
    SAY_STEEL_DEATH2        = -1603072,
    SAY_STEEL_SLAY1         = -1603073,
    SAY_STEEL_SLAY2         = -1603074,
    SAY_STEEL_OVERWHELMING  = -1603075,
    SAY_STEEL_BERSERK       = -1603076,

    SAY_BRUNDIR_AGGR0       = -1603080,
    SAY_BRUNDIR_WHIRL       = -1603081,
    SAY_BRUNDIR_DEATH1      = -1603082,
    SAY_BRUNDIR_DEATH2      = -1603083,
    SAY_BRUNDIR_SLAY1       = -1603084,
    SAY_BRUNDIR_SLAY2       = -1603085,
    SAY_BRUNDIR_BERSERK     = -1603086,
    SAY_BRUNDIR_FLY         = -1603087,

    //all
    SPELL_BERSERK               = 47008,
    SPELL_SUPERCHARGE           = 61920,
    SPELL_IRON_COUNCIL_CREDIT   = 65195,    // custom spell in spell_dbc.sql

    //steelbreaker
    SPELL_HIGH_VOLTAGE          = 61890,
    SPELL_HIGH_VOLTAGE_H        = 63498,
    SPELL_FUSION_PUNCH          = 61903,
    SPELL_FUSION_PUNCH_H        = 63493,
    SPELL_STATIC_DISRUPTION     = 44008,
    SPELL_STATIC_DISRUPTION_H   = 63494,
    SPELL_POWER                 = 64637,
    SPELL_POWER_H               = 61888,
    SPELL_ELECTRICAL_CHARGE     = 61902,

    //runemaster molgeim
    SPELL_SHIELD                = 62274,
    SPELL_SHIELD_H              = 63489,
    SPELL_RUNE_OF_POWER         = 63513,
    SPELL_RUNE_OF_DEATH         = 62269,
    SPELL_RUNE_OF_DEATH_H       = 63490,
    SPELL_RUNE_OF_SUMMONING     = 62273,
    //rune of power
    AURA_RUNE_OF_POWER          = 61974,
    //rune of summoning
    AURA_RUNE_OF_SUMMONING      = 62019,
    //lightning elemental
    SPELL_LIGHTNING_BLAST       = 62054,
    SPELL_LIGHTNING_BLAST_H     = 63491,

    //stormcaller brundir
    SPELL_CHAIN_LIGHTNING       = 61879,
    SPELL_CHAIN_LIGHTNING_H     = 63479,
    SPELL_OVERLOAD              = 61869,
    SPELL_LIGHTNING_WHIRL       = 61915,
    SPELL_LIGHTNING_WHIRL_H     = 63483,
    SPELL_STORMSHIELD           = 64187,
    SPELL_LIGHTNING_TENDRILS    = 61887,
    SPELL_LIGHTNING_TENDRILS_H  = 63486,
    LIGHTNING_TENDRILS_VISUAL   = 61883,
    //NPC ids
    MOB_LIGHTNING_ELEMENTAL     = 32958,
};

enum BrundirPhase
{
    PHASE_NORMAL        = 1,
    PHASE_UP            = 2,
    PHASE_TENDRILS      = 3,
    PHASE_DOWN          = 4,
};

// Rune of Power
struct MANGOS_DLL_DECL mob_rune_of_powerAI : public ScriptedAI
{
    mob_rune_of_powerAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        SetCombatMovement(false);
        Reset();
    }

    uint32 m_uiDeath_Timer;

    void Reset()
    {
        m_uiDeath_Timer = 60000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature,  AURA_RUNE_OF_POWER);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiDeath_Timer < diff)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }else m_uiDeath_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_rune_of_power(Creature* pCreature)
{
    return new mob_rune_of_powerAI(pCreature);
}

// Lightning Elemental
struct MANGOS_DLL_DECL mob_ulduar_lightning_elementalAI : public ScriptedAI
{
    mob_ulduar_lightning_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDeath_Timer;
    uint32 m_uiCheck_Timer;
    bool m_bWillExplode;

    void Reset()
    {
        m_bWillExplode  = false;
        m_uiCheck_Timer = 1000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiDeath_Timer < diff && m_bWillExplode)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else m_uiDeath_Timer -= diff;

        if (m_uiCheck_Timer < diff)
        {
            if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 15))
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_BLAST : SPELL_LIGHTNING_BLAST_H);
                m_bWillExplode  = true;
                m_uiDeath_Timer = 500;
                m_uiCheck_Timer = 5000;
            }
            m_uiCheck_Timer = 1000;
        }else m_uiCheck_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_ulduar_lightning_elemental(Creature* pCreature)
{
    return new mob_ulduar_lightning_elementalAI(pCreature);
}

// Rune of Summoning
struct MANGOS_DLL_DECL mob_rune_of_summoningAI : public ScriptedAI
{
    mob_rune_of_summoningAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint32 m_uiDeath_Timer;
    uint32 m_uiSummon_Timer;
    uint32 m_uiSummonNum;

    void Reset()
    {
        m_uiDeath_Timer     = 0;
        m_uiSummon_Timer    = 5000;
        m_uiSummonNum       = 0;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature,  AURA_RUNE_OF_SUMMONING);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetInCombatWithZone();
        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            pSummoned->AddThreat(pTarget, 100.0f);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiSummon_Timer < diff)
        {
            if (/*Creature* pTemp = */m_creature->SummonCreature(MOB_LIGHTNING_ELEMENTAL, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                ++m_uiSummonNum;

            if (m_uiSummonNum > 9)
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

            m_uiSummon_Timer = 500;
        } else m_uiSummon_Timer -= diff;
    }
};

CreatureAI* GetAI_mob_rune_of_summoning(Creature* pCreature)
{
    return new mob_rune_of_summoningAI(pCreature);
}

//Stormcaller Brundir
struct MANGOS_DLL_DECL boss_brundirAI : public ScriptedAI
{
    boss_brundirAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty(); 
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiChain_Lightning_Timer;
    uint32 m_uiOverload_Timer;
    uint32 m_uiWhirl_Timer;
    uint32 m_uiTendrilsTimer;
    uint32 m_uiTendrilsEndTimer;
    uint32 m_uiEnrage_Timer;

    bool m_bHasSupercharge1;
    bool m_bHasSupercharge2;
    bool m_bIsEnrage;

    BrundirPhase brundirPhase;

    void Reset()
    {
        m_creature->SetLevitate(false);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_uiChain_Lightning_Timer = 0;
        brundirPhase        = PHASE_NORMAL;
        m_uiOverload_Timer      = 35000;
        m_uiEnrage_Timer        = 900000;
        m_bIsEnrage             = false;
        m_bHasSupercharge1      = false;
        m_bHasSupercharge2      = false;
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_SUPERCHARGE)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            if (m_bHasSupercharge1)
            {
                m_bHasSupercharge2 = true;
                m_uiTendrilsTimer = 40000;
            }
            else
            {
                m_bHasSupercharge1 = true;
                m_uiWhirl_Timer = 10000;
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (m_bHasSupercharge2)
        {
            m_pInstance->SetData(TYPE_ASSEMBLY, DONE);  
            DoCast(m_creature, SPELL_IRON_COUNCIL_CREDIT, true);
        }
        else
        {
            if (m_pInstance)
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_YOU_BRUNDIR, false);
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            DoCast(m_creature, SPELL_SUPERCHARGE, true);
        }

        DoScriptText(urand(0,1) ? SAY_BRUNDIR_DEATH1 : SAY_BRUNDIR_DEATH2 , m_creature);
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_STEELBREAKER))
        {
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();
        }
        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MOLGEIM))
        {
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();
        }
        if (m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_ASSEMBLY) != IN_PROGRESS)
                m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);
        }

        DoScriptText(SAY_BRUNDIR_AGGR0, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_ASSEMBLY) != FAIL)
                m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
        }
    }

    void KilledUnit(Unit *pWho)
    {
        DoScriptText(urand(0,1) ? SAY_BRUNDIR_SLAY1 : SAY_BRUNDIR_SLAY2 , m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEnrage_Timer < uiDiff && !m_bIsEnrage)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                m_bIsEnrage = true;
                DoScriptText(SAY_BRUNDIR_BERSERK, m_creature);
            }
        }
        else
            m_uiEnrage_Timer -= uiDiff;

        switch (brundirPhase)
        {
            case PHASE_NORMAL:
            {
                // level 1 spells
                if (m_uiChain_Lightning_Timer < uiDiff)
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (DoCastSpellIfCan(target, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H) == CAST_OK)
                            m_uiChain_Lightning_Timer = 2000;
                }
                else
                    m_uiChain_Lightning_Timer -= uiDiff;   

                if (m_uiOverload_Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_OVERLOAD) == CAST_OK)
                        m_uiOverload_Timer = 40000;
                }
                else
                    m_uiOverload_Timer -= uiDiff;  

                // level 2 spells
                if (m_bHasSupercharge1)
                {
                    if (m_uiWhirl_Timer < uiDiff)
                    {
                        if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_WHIRL : SPELL_LIGHTNING_WHIRL_H) == CAST_OK)
                        {
                            DoScriptText(SAY_BRUNDIR_WHIRL, m_creature);
                            m_uiWhirl_Timer = 15000;
                        }
                    }
                    else
                        m_uiWhirl_Timer -= uiDiff;

                    // level 3 spells
                    if (m_bHasSupercharge2)
                    {
                        // boss doesn't fly during tendrils, needs fixing!
                        if (m_uiTendrilsTimer < uiDiff)
                        {
                            brundirPhase = PHASE_UP;
                            DoScriptText(SAY_BRUNDIR_FLY, m_creature);

                            m_creature->SetLevitate(true);
                            m_creature->SetWalk(false);
                            m_creature->GetMotionMaster()->MovePoint(1, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ() + 5);
                            m_creature->SetSpeedRate(MOVE_RUN, 0.8f);
                            m_uiTendrilsTimer = 4000;
                        }
                        else
                            m_uiTendrilsTimer -= uiDiff;

                        DoCastSpellIfCan(m_creature, SPELL_STORMSHIELD, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                    }
                }
                break;

            }
            case PHASE_UP:
                if (m_uiTendrilsTimer < uiDiff)
                {
                    brundirPhase = PHASE_TENDRILS;
                    DoCast(m_creature, LIGHTNING_TENDRILS_VISUAL, true);
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_TENDRILS : SPELL_LIGHTNING_TENDRILS_H,true);
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
                        m_creature->GetMotionMaster()->MovePoint(1, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ() + 5);
                    m_uiTendrilsTimer = 4000;
                    m_uiTendrilsEndTimer = 40000;
                }
                else
                    m_uiTendrilsTimer -= uiDiff;

                break;
            case PHASE_TENDRILS:
                if (m_uiTendrilsTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
                        m_creature->GetMotionMaster()->MovePoint(1, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ() + 5);
                    m_uiTendrilsTimer = 4000;
                }
                else 
                    m_uiTendrilsTimer -= uiDiff;

                if (m_uiTendrilsEndTimer < uiDiff)
                {
                    brundirPhase = PHASE_DOWN;

                    m_uiTendrilsTimer = 2000;
                    m_creature->SetLevitate(false);
                    m_creature->SetWalk(true);
                    float x = m_creature->GetPositionX();
                    float y = m_creature->GetPositionY();
                    float z = m_creature->GetTerrain()->GetWaterOrGroundLevel(x,y,m_creature->GetPositionZ());
                    m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
                    m_creature->SetSpeedRate(MOVE_RUN, 1.8f);
                    m_uiChain_Lightning_Timer = 5000;
                    m_uiOverload_Timer = 35000;
                    m_uiWhirl_Timer = 10000;
                }
                else
                    m_uiTendrilsEndTimer -= uiDiff;
                break;
            case PHASE_DOWN:
                if (m_uiTendrilsTimer < uiDiff)
                {
                    brundirPhase = PHASE_NORMAL;
                    m_uiTendrilsTimer = 90000;
                    if (m_creature->HasAura(SPELL_LIGHTNING_TENDRILS))
                        m_creature->RemoveAurasDueToSpell(SPELL_LIGHTNING_TENDRILS);
                    if (m_creature->HasAura(SPELL_LIGHTNING_TENDRILS_H))
                        m_creature->RemoveAurasDueToSpell(SPELL_LIGHTNING_TENDRILS_H);
                    if (m_creature->HasAura(LIGHTNING_TENDRILS_VISUAL))
                        m_creature->RemoveAurasDueToSpell(LIGHTNING_TENDRILS_VISUAL);
                }
                else
                    m_uiTendrilsTimer -= uiDiff;
                break;                
        }
    }
};

CreatureAI* GetAI_boss_brundir(Creature* pCreature)
{
    return new boss_brundirAI(pCreature);
}

//Runemaster Molgeim
struct MANGOS_DLL_DECL boss_molgeimAI : public ScriptedAI
{
    boss_molgeimAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiShield_Timer;
    uint32 m_uiRune_Power_Timer;
    uint32 m_uiRune_Death_Timer;
    uint32 m_uiRune_Summon_Timer;
    uint32 m_uiEnrage_Timer;

    bool m_bHasSupercharge1;
    bool m_bHasSupercharge2;
    bool m_bEnrage;

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_uiShield_Timer    = 20000;
        m_uiRune_Power_Timer = 10000;
        m_uiEnrage_Timer    = 900000;
        m_bEnrage           = false;
        m_bHasSupercharge1  = false;
        m_bHasSupercharge2  = false;
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_SUPERCHARGE)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            if (m_bHasSupercharge1)
            {
                m_bHasSupercharge2 = true;                
                m_uiRune_Summon_Timer = 20000;
            }
            else
            {
                m_bHasSupercharge1 = true;
                m_uiRune_Death_Timer = 10000;
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (m_bHasSupercharge2)
        {
            m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
            DoCast(m_creature, SPELL_IRON_COUNCIL_CREDIT, true);
        }
        else
        {
            if (m_pInstance)
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_YOU_MOLGEIM, false);
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            DoCast(m_creature, SPELL_SUPERCHARGE, true);
        }

        DoScriptText(urand(0,1) ? SAY_MOLGEIM_DEATH1 : SAY_MOLGEIM_DEATH2, m_creature);
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_STEELBREAKER))
        {
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();
        }
        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_BRUNDIR))
        {
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();
        }
        if (m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_ASSEMBLY) != IN_PROGRESS)
                m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);
        }

        DoScriptText(SAY_MOLGEIM_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_ASSEMBLY) != FAIL)
                m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
        }
    }

    void KilledUnit(Unit *who)
    {
        DoScriptText(urand(0,1) ? SAY_MOLGEIM_SLAY1 : SAY_MOLGEIM_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // level 1 spells
        if (m_uiShield_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SHIELD : SPELL_SHIELD_H) == CAST_OK)
                m_uiShield_Timer = 50000;
        }
        else
            m_uiShield_Timer -= uiDiff;   

        if (m_uiRune_Power_Timer < uiDiff)
        {
            switch(urand(0, 2))
            {
                case 0:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_BRUNDIR))
                    {
                        if (pTemp->isAlive())
                            DoCast(pTemp, SPELL_RUNE_OF_POWER);
                        else
                            DoCast(m_creature, SPELL_RUNE_OF_POWER);
                    }
                    break;
                case 1:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_STEELBREAKER))
                    {
                        if (pTemp->isAlive())
                            DoCast(pTemp, SPELL_RUNE_OF_POWER);
                        else
                            DoCast(m_creature, SPELL_RUNE_OF_POWER);
                    }
                    break;
                case 2:
                    DoCast(m_creature, SPELL_RUNE_OF_POWER);
                    break;
            }
            m_uiRune_Power_Timer = 30000;
        }
        else
            m_uiRune_Power_Timer -= uiDiff;

        // level2 spells
        if (m_bHasSupercharge1)
        {
            if (m_uiRune_Death_Timer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_RUNE_OF_DEATH : SPELL_RUNE_OF_DEATH_H) == CAST_OK)
                    {
                        DoScriptText(SAY_MOLGEIM_DEATH_RUNE, m_creature);
                        m_uiRune_Death_Timer = 30000;
                    }
                }
            }
            else
                m_uiRune_Death_Timer -= uiDiff;

            // level 3 spells
            if (m_bHasSupercharge2)
            {
                if (m_uiRune_Summon_Timer < uiDiff)
                {
                    m_creature->CastStop();
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_RUNE_OF_SUMMONING) == CAST_OK)
                        {
                            DoScriptText(SAY_MOLGEIM_SUMMON, m_creature);
                            m_uiRune_Summon_Timer = 30000;
                        }
                    }
                }
                else
                    m_uiRune_Summon_Timer -= uiDiff;
            }
        }

        if (m_uiEnrage_Timer < uiDiff && !m_bEnrage)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                m_bEnrage = true;
                DoScriptText(SAY_MOLGEIM_BERSERK, m_creature);
            }
        }
        else
            m_uiEnrage_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_molgeim(Creature* pCreature)
{
    return new boss_molgeimAI(pCreature);
}

//Steelbreaker
struct MANGOS_DLL_DECL boss_steelbreakerAI : public ScriptedAI
{
    boss_steelbreakerAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFusion_Punch_Timer;
    uint32 m_uiStatic_Disruption_Timer;
    uint32 m_uiPower_Timer;
    uint32 m_uiEnrage_Timer;

    bool m_bBrundirDead;
    bool m_bMolgeimDead;
    bool m_bHasSupercharge1;
    bool m_bHasSupercharge2;
    bool m_bEnrage;

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_uiFusion_Punch_Timer = 20000;
        m_uiEnrage_Timer    = 900000;
        m_bEnrage           = false;
        m_bBrundirDead      = false;
        m_bMolgeimDead      = false;
        m_bHasSupercharge1  = false;
        m_bHasSupercharge2  = false;
    }

    void KilledUnit(Unit* pVictim)
    {
        if (m_bHasSupercharge2)
            DoCast(m_creature, SPELL_ELECTRICAL_CHARGE);

        DoScriptText(urand(0,1) ? SAY_STEEL_SLAY1 : SAY_STEEL_SLAY2, m_creature);
    }

    void SpellHit(Unit* caster, const SpellEntry* spell)
    {
        if (spell->Id == SPELL_SUPERCHARGE)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            if (m_bHasSupercharge1)
            {
                m_bHasSupercharge2 = true;                
                m_uiPower_Timer = 5000;
            }
            else
            {
                m_bHasSupercharge1 = true;
                m_uiStatic_Disruption_Timer = 12000;
            }
        }
    }


    void JustDied(Unit* pKiller)
    {
        if (m_bHasSupercharge2)
        {
            m_pInstance->SetData(TYPE_ASSEMBLY, DONE);
            DoCast(m_creature, SPELL_IRON_COUNCIL_CREDIT, true);
        }
        else
        {
            if (m_pInstance)
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_YOU_STEELBREAKER, false);
            m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
            DoCast(m_creature, SPELL_SUPERCHARGE, true);
        }
        DoScriptText(urand(0,1) ? SAY_STEEL_DEATH1 : SAY_STEEL_DEATH2, m_creature);
    }

    void Aggro(Unit* pWho)
    {
        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MOLGEIM))
        {
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();
        }
        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_BRUNDIR))
        {
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();
        }
        if (m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_ASSEMBLY) != IN_PROGRESS)
                m_pInstance->SetData(TYPE_ASSEMBLY, IN_PROGRESS);
        }

        DoScriptText(SAY_STEEL_AGGRO, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            if(m_pInstance->GetData(TYPE_ASSEMBLY) != FAIL)
                m_pInstance->SetData(TYPE_ASSEMBLY, FAIL);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HIGH_VOLTAGE : SPELL_HIGH_VOLTAGE_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        // level 1 spells
        if (m_uiFusion_Punch_Timer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FUSION_PUNCH : SPELL_FUSION_PUNCH_H))
                m_uiFusion_Punch_Timer = urand(17000,22000);
        }
        else
            m_uiFusion_Punch_Timer -= uiDiff;
        
        // level 2 spells
        if (m_bHasSupercharge1)
        {
            if (m_uiStatic_Disruption_Timer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, (uint32)0, SELECT_FLAG_PLAYER))
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_STATIC_DISRUPTION : SPELL_STATIC_DISRUPTION_H) == CAST_OK)
                        m_uiStatic_Disruption_Timer = 60000;
            }
            else
                m_uiStatic_Disruption_Timer -= uiDiff;

            // level 3 spells
            if (m_bHasSupercharge2)
            {
                if (m_uiPower_Timer < uiDiff)
                {

                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_POWER : SPELL_POWER_H) == CAST_OK)
                        {
                            DoScriptText(SAY_STEEL_OVERWHELMING, m_creature);
                            m_uiPower_Timer = m_bIsRegularMode ? 65000 : 35000;
                        }
                    }
                }
                else
                    m_uiPower_Timer -= uiDiff;
            }
        }

        if (m_uiEnrage_Timer < uiDiff && !m_bEnrage)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                m_bEnrage = true;
                DoScriptText(SAY_STEEL_BERSERK, m_creature);
            }
        }
        else
            m_uiEnrage_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_steelbreaker(Creature* pCreature)
{
    return new boss_steelbreakerAI(pCreature);
}

void AddSC_boss_iron_council()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_brundir";
    NewScript->GetAI = GetAI_boss_brundir;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_molgeim";
    NewScript->GetAI = GetAI_boss_molgeim;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "boss_steelbreaker";
    NewScript->GetAI = GetAI_boss_steelbreaker;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_rune_of_power";
    NewScript->GetAI = &GetAI_mob_rune_of_power;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_rune_of_summoning";
    NewScript->GetAI = &GetAI_mob_rune_of_summoning;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_ulduar_lightning_elemental";
    NewScript->GetAI = &GetAI_mob_ulduar_lightning_elemental;
    NewScript->RegisterSelf();
}
