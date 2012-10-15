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

/* ScriptData
SDName: Boss_Gortok
SD%Complete: 20%
SDComment:
SDCategory: Utgarde Pinnacle
EndScriptData */

/* TODO
implement Orb - a red ball-> this Orb activate the mobs
*/


#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_AGGRO               = -1575015,
    SAY_SLAY_1              = -1575016,
    SAY_SLAY_2              = -1575017,
    SAY_DEATH               = -1575018,

    SPELL_FREEZE_ANIM       = 16245,

    SPELL_IMPALE            = 48261,
    SPELL_IMPALE_H          = 59268,

    SPELL_WITHERING_ROAR    = 48256,
    SPELL_WITHERING_ROAR_H  = 59267,

    SPELL_ARCING_SMASH      = 48260,

    SPELL_ORB_VISUAL        = 48044, // not used
};

enum Phase
{
    PHASE_BEGIN             = 0,
    PHASE_FRENZIED_WORGEN   = 1,
    PHASE_RAVENOUS_FURLBORG = 2,
    PHASE_MASSIVE_JORMUNGAR = 3,
    PHASE_FEROCIOUS_RHINO   = 4,
    PHASE_GORTOK_PALEHOOF   = 5,
};

enum SubBossSteps
{
    PHASE_SUB_BOSS_BEGIN    = 0,
    PHASE_SUB_BOSS_INFIGHT  = 1,
    PHASE_SUB_BOSS_END      = 2,
};

/*######
## boss_gortok
######*/

struct MANGOS_DLL_DECL boss_gortokAI : public ScriptedAI
{
    boss_gortokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pinnacle* m_pInstance;
    bool m_bIsRegularMode;

    Phase m_uiPhase;
    uint8 m_uiSubBossSteps;
    uint8 m_uiSubBossCount;
    uint8 m_uiSubBossMax;

    uint32 m_uiCheckForDeathSubBoss;

    uint32 m_uiWitheringRoar;
    uint32 m_uiImpale;
    uint32 m_uiArcingSmash;

    void Reset()
    {
        m_uiPhase = PHASE_BEGIN;
        m_uiSubBossSteps =  PHASE_SUB_BOSS_BEGIN;
        m_uiSubBossCount = 0;
        m_uiSubBossMax = m_bIsRegularMode ? 2 : 4;
        m_uiCheckForDeathSubBoss = 2000;

        m_uiWitheringRoar = urand(7000,11000);
        m_uiImpale = 9000;
        m_uiArcingSmash = 5000;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        ScriptedAI::MoveInLineOfSight(pWho);
        if (m_creature->IsWithinDistInMap(pWho, m_creature->GetAttackDistance(pWho)) && m_creature->IsWithinLOSInMap(pWho))
        {
            if (m_uiPhase==PHASE_BEGIN)
            {
                if (m_pInstance)
                {
                    switch(urand(0,3))
                    {
                        case 0: m_uiPhase = PHASE_FRENZIED_WORGEN; break;
                        case 1: m_uiPhase = PHASE_RAVENOUS_FURLBORG; break;
                        case 2: m_uiPhase = PHASE_MASSIVE_JORMUNGAR; break;
                        case 3: m_uiPhase = PHASE_FEROCIOUS_RHINO; break;
                        default: m_uiPhase = PHASE_FRENZIED_WORGEN; break;
                    }
                }
            }
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (m_uiPhase==PHASE_GORTOK_PALEHOOF)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_FREEZE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            ScriptedAI::AttackStart(pWho);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, DONE);
    }

    void EnterEvadeMode()
    {
        ScriptedAI::EnterEvadeMode();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GORTOK, FAIL);
    }

    void BeginSubBossPhase()
    {
        if (m_pInstance)
        {
            Creature* pTemp;
            switch(m_uiPhase)
            {
                case PHASE_FRENZIED_WORGEN:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_WORGEN);
                    break;
                }
                case PHASE_RAVENOUS_FURLBORG:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_FURBOLG);
                    break;
                }
                case PHASE_MASSIVE_JORMUNGAR:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JORMUNGAR);
                    break;
                }
                case PHASE_FEROCIOUS_RHINO:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_RHINO);
                    break;
                }
                default:
                    break;
            }
            if (pTemp)
            {
                pTemp->RemoveAurasDueToSpell(SPELL_FREEZE);
                pTemp->SetInCombatWithZone();
                pTemp->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            }
        }
    }

    Phase getValityPhase()
    {
        uint32 result = m_uiPhase;
        ++result;
        if (result > PHASE_FEROCIOUS_RHINO)
            result = PHASE_FRENZIED_WORGEN;
        return (Phase)result;
    }

    bool IsSubBossDead()
    {
        if (m_pInstance)
        {
            Creature* pTemp;
            switch(m_uiPhase)
            {
                case PHASE_FRENZIED_WORGEN:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_WORGEN);
                    break;
                }
                case PHASE_RAVENOUS_FURLBORG:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_FURBOLG);
                    break;
                }
                case PHASE_MASSIVE_JORMUNGAR:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JORMUNGAR);
                    break;
                }
                case PHASE_FEROCIOUS_RHINO:
                {
                    pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_RHINO);
                    break;
                }
                default:
                    break;
            }
            if (pTemp)
            {
                pTemp->RemoveAurasDueToSpell(SPELL_FREEZE);
                return pTemp->isDead();
            }
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        switch(m_uiPhase)
        {
            case PHASE_BEGIN:
                // Nothing
                break;
            case PHASE_FRENZIED_WORGEN:
            case PHASE_RAVENOUS_FURLBORG:
            case PHASE_MASSIVE_JORMUNGAR:
            case PHASE_FEROCIOUS_RHINO:
                switch(m_uiSubBossSteps)
                {
                    case PHASE_SUB_BOSS_BEGIN:
                    {
                        ++m_uiSubBossCount;
                        BeginSubBossPhase();
                        m_uiSubBossSteps = PHASE_SUB_BOSS_INFIGHT;
                        break;
                    }
                    case PHASE_SUB_BOSS_INFIGHT:
                    {
                        if (m_uiCheckForDeathSubBoss < uiDiff)
                        {
                            if (IsSubBossDead())
                            {
                                m_uiSubBossSteps = PHASE_SUB_BOSS_END;
                            }
                            m_uiCheckForDeathSubBoss = 2000;
                        }
                        else
                            m_uiCheckForDeathSubBoss -= uiDiff;

                        break;                        
                    }
                    case PHASE_SUB_BOSS_END:
                    {
                        if (m_uiSubBossCount >= m_uiSubBossMax)
                        {
                            m_uiPhase = PHASE_GORTOK_PALEHOOF;
                            m_creature->SetInCombatWithZone();
                            return;
                        }
                        m_uiPhase = getValityPhase();
                        m_uiSubBossSteps = PHASE_SUB_BOSS_BEGIN;
                        break;
                    }
                    default:
                        m_uiSubBossSteps = PHASE_SUB_BOSS_BEGIN;

                }
                break;
            case PHASE_GORTOK_PALEHOOF:

                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                if (m_uiWitheringRoar < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WITHERING_ROAR : SPELL_WITHERING_ROAR_H) == CAST_OK)
                        m_uiWitheringRoar = urand(7000,11000);
                }
                else
                    m_uiWitheringRoar -= uiDiff;
                
                if (m_uiImpale < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, 0.0f, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H) == CAST_OK)
                            m_uiImpale = 9000;
                    }
                }
                else
                    m_uiImpale -= uiDiff;

                if (m_uiArcingSmash < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_ARCING_SMASH) == CAST_OK)
                        m_uiArcingSmash = 5000;
                }
                else
                    m_uiArcingSmash -= uiDiff;
                        
                DoMeleeAttackIfReady();
                break;
            default:
                m_uiPhase = PHASE_BEGIN;
        }
    }
};

CreatureAI* GetAI_boss_gortok(Creature* pCreature)
{
    return new boss_gortokAI(pCreature);
}

void AddSC_boss_gortok()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_gortok";
    pNewScript->GetAI = &GetAI_boss_gortok;
    pNewScript->RegisterSelf();
}
