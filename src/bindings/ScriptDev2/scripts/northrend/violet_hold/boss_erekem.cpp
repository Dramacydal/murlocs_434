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
SDName: boss_erekem
SDAuthor: ckegg
SD%Complete: 50%
SDComment: 
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

enum
{
    SAY_AGGRO                                 = -1608010,
    SAY_SLAY_1                                = -1608011,
    SAY_SLAY_2                                = -1608012,
    SAY_SLAY_3                                = -1608013,
    SAY_DEATH                                 = -1608014,
    SAY_SPAWN                                 = -1608015,
    SAY_ADD_KILED                             = -1608016,
    SAY_BOTH_ADDS_KILED                       = -1608017,

    SPELL_BLOODLUST                           = 54516,
    SPELL_BREAK_BONDS                         = 59463,
    SPELL_CHAIN_HEAL                          = 54481,
    SPELL_CHAIN_HEAL_H                        = 59473,
    SPELL_EARTH_SHIELD                        = 54479,
    SPELL_EARTH_SHIELD_H                      = 59471,
    SPELL_EARTH_SHOCK                         = 54511,
    SPELL_LIGHTNING_BOLT                      = 53044,
    SPELL_STORMSTRIKE                         = 51876,

    SPELL_GUSHING_WOUND                       = 39215,
    SPELL_HOWLING_SCREECH                     = 54462,
    SPELL_STRIKE                              = 14516,
};

struct MANGOS_DLL_DECL boss_erekemAI : public ScriptedAI
{
    boss_erekemAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance *m_pInstance;

    bool m_bIsRegularMode;
    bool m_bIsAddDead;
    bool MovementStarted;

    uint32 m_uiBloodlust_Timer;
    uint32 m_uiBreakBonds_Timer;
    uint32 m_uiChainHeal_Timer;
    uint32 m_uiEarthShield_Timer;
    uint32 m_uiEarthShock_Timer;
    uint32 m_uiLightningBolt_Timer;
    uint32 m_uiStormstrike_Timer;

    void Reset()
    {
        if (!m_pInstance) return;

        m_bIsAddDead = false;
        MovementStarted = false;
        m_uiLightningBolt_Timer = 2000;
        m_uiEarthShield_Timer = urand(10000, 15000);
        m_uiEarthShock_Timer = urand(12000, 17000);
        m_uiChainHeal_Timer = urand(5000, 25000);
        m_uiBreakBonds_Timer = urand(25000, 30000);
        m_uiBloodlust_Timer = urand(60000, 65000);
        m_uiStormstrike_Timer = urand(1000, 2000);

        std::list<Creature*> lUnitList;
        GetCreatureListWithEntryInGrid(lUnitList, m_creature, NPC_EREKEM_GUARD, 100.0f);
        if (!lUnitList.empty())
            for(std::list<Creature*>::iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
                if ((*iter))
                    if ((*iter)->isDead())
                        (*iter)->Respawn();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_EREKEM, FAIL);
            m_pInstance->SetData(TYPE_EVENT, FAIL);
            m_pInstance->SetData(TYPE_RIFT, FAIL);
            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, NOT_STARTED);}
            else {m_pInstance->SetData(TYPE_PORTAL12, NOT_STARTED);}
        }
    }
    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;
        DoScriptText(SAY_AGGRO, m_creature);
        m_pInstance->SetData(TYPE_EREKEM, IN_PROGRESS);
        m_creature->GetMotionMaster()->MovementExpired();
        SetCombatMovement(true);
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_EREKEM) != SPECIAL && m_pInstance->GetData(TYPE_EREKEM) != IN_PROGRESS)
            return;

        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 0.0f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);

            std::list<Creature*> lUnitList;
            GetCreatureListWithEntryInGrid(lUnitList, m_creature, NPC_EREKEM_GUARD, 100.0f);
            if (!lUnitList.empty())
                for(std::list<Creature*>::iterator iter = lUnitList.begin(); iter != lUnitList.end(); ++iter)
                    if (*iter)
                        if ((*iter)->isAlive())
                        {
                            (*iter)->AddThreat(pWho, 0.0f);
                      /*      (*iter)->AI()->AttackStart(pWho);*/ 
                        }
        }
    }

    void StartMovement(uint32 id)
    {
        m_creature->GetMotionMaster()->MovePoint(id, PortalLoc[id].x, PortalLoc[id].y, PortalLoc[id].z);
        m_creature->SetWalk(true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetInCombatWithZone();
        MovementStarted = true;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE || !MovementStarted) return;
        if (id == 0)
        {
            MovementStarted = false;
            m_creature->GetMotionMaster()->MovementExpired();
            SetCombatMovement(true);
            m_creature->SetInCombatWithZone();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_EREKEM) == SPECIAL && !MovementStarted)
           StartMovement(0);

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEarthShield_Timer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_EARTH_SHIELD : SPELL_EARTH_SHIELD_H);
            m_uiEarthShield_Timer = urand(25000, 30000);
        }
        else m_uiEarthShield_Timer -= uiDiff;

        if (m_uiEarthShock_Timer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_EARTH_SHOCK);
            m_uiEarthShock_Timer = urand(12000, 17000);
        }
        else m_uiEarthShock_Timer -= uiDiff;

        if (m_uiChainHeal_Timer < uiDiff)
        {
            //m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_CHAIN_HEAL : SPELL_CHAIN_HEAL_H);
            m_uiChainHeal_Timer = urand(5000, 25000);
        }
        else m_uiChainHeal_Timer -= uiDiff;

        if (m_uiBreakBonds_Timer < uiDiff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, SPELL_BREAK_BONDS);
            m_uiBreakBonds_Timer = urand(25000, 30000);
        }
        else m_uiBreakBonds_Timer -= uiDiff;

        if (!m_bIsAddDead)
        {
            if (m_uiLightningBolt_Timer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_LIGHTNING_BOLT);
                m_uiLightningBolt_Timer = 2000;
            }
            else m_uiLightningBolt_Timer -= uiDiff;
        }
        else
        {
            if (m_uiStormstrike_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), SPELL_STORMSTRIKE);
                m_uiStormstrike_Timer = 1000;
            }
            else m_uiStormstrike_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance) {
            m_pInstance->SetData(TYPE_EREKEM, DONE);
            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, DONE);}
            else {m_pInstance->SetData(TYPE_PORTAL12, DONE);}

        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature);break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature);break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature);break;
        }
    }
};

struct MANGOS_DLL_DECL mob_erekem_guardAI : public ScriptedAI
{
    mob_erekem_guardAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance *m_pInstance;

    uint32 m_uiGushingWound_Timer;
    uint32 m_uiHowlingScreech_Timer;
    uint32 m_uiStrike_Timer;
    uint32 m_uiBloodlust_Timer;
    bool MovementStarted;

    void Reset()
    {
        m_uiGushingWound_Timer = urand(5000, 10000);
        m_uiBloodlust_Timer = urand(25000, 30000);
        m_uiHowlingScreech_Timer = urand(12000, 15000);
        m_uiStrike_Timer = urand(10000, 11000);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        MovementStarted = false;

    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->GetMotionMaster()->MovementExpired();
        SetCombatMovement(true);
    }

   /* void AttackStart(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_EREKEM) != SPECIAL && m_pInstance->GetData(TYPE_EREKEM) != IN_PROGRESS)
            return;


        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho, 0.0f);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
            m_creature->GetMotionMaster()->MovementExpired();
            SetCombatMovement(true);
        }
    }*/ 

    void StartMovement(uint32 id)
    {
        m_creature->GetMotionMaster()->MovePoint(id, PortalLoc[id].x, PortalLoc[id].y, PortalLoc[id].z);
        m_creature->SetWalk(true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        MovementStarted = true;
        m_creature->SetInCombatWithZone();
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE || !MovementStarted) return;
        if (id == 0)
        {
            m_creature->GetMotionMaster()->MovementExpired();
            SetCombatMovement(true);
            m_creature->SetInCombatWithZone();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) return;

        if (m_pInstance->GetData(TYPE_EREKEM) == IN_PROGRESS && !MovementStarted && !m_creature->getVictim())
            StartMovement(0);

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiGushingWound_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_GUSHING_WOUND);
            m_uiGushingWound_Timer = urand(30000, 32000);
        }
        else m_uiGushingWound_Timer -= uiDiff;

        if (m_uiBloodlust_Timer < uiDiff)
        {
            if (m_pInstance)
            if (Creature* pErekem = m_pInstance->GetSingleCreatureFromStorage(DATA_EREKEM))
                if (pErekem->isAlive())
                {
                    pErekem->InterruptNonMeleeSpells(false);
                    pErekem->CastSpell(pErekem, SPELL_BLOODLUST, false);
                    m_uiBloodlust_Timer = urand(25000, 30000);
                }
        }
        else m_uiBloodlust_Timer -= uiDiff;

        if (m_uiHowlingScreech_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_HOWLING_SCREECH);
            m_uiHowlingScreech_Timer = urand(24000, 30000);
        }
        else m_uiHowlingScreech_Timer -= uiDiff;

        if (m_uiStrike_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_STRIKE);
            m_uiStrike_Timer = urand(15000, 16000);
        }
        else m_uiStrike_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            if (Creature* pErekem = m_pInstance->GetSingleCreatureFromStorage(DATA_EREKEM))
                if (pErekem->isAlive())
                {
                    DoScriptText(SAY_ADD_KILED, pErekem);
                   // pErekem->InterruptNonMeleeSpells(false);
                   // pErekem->CastSpell(pErekem, SPELL_BLOODLUST, false);
                    ((boss_erekemAI*)pErekem->AI())->m_bIsAddDead = true;
                }
    }
};

CreatureAI* GetAI_boss_erekem(Creature* pCreature)
{
    return new boss_erekemAI (pCreature);
}

CreatureAI* GetAI_mob_erekem_guard(Creature* pCreature)
{
    return new mob_erekem_guardAI (pCreature);
}

void AddSC_boss_erekem()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_erekem";
    newscript->GetAI = &GetAI_boss_erekem;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_erekem_guard";
    newscript->GetAI = &GetAI_mob_erekem_guard;
    newscript->RegisterSelf();
}
