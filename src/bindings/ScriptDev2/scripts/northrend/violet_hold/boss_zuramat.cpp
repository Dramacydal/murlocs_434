/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_zuramat
SDAuthor: ckegg
SD%Complete: 60%
SDComment: 
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

enum
{
    SAY_AGGRO                                 = -1608037,
    SAY_SLAY_1                                = -1608038,
    SAY_SLAY_2                                = -1608039,
    SAY_SLAY_3                                = -1608040,
    SAY_DEATH                                 = -1608041,
    SAY_SPAWN                                 = -1608042,
    SAY_SHIELD                                = -1608043,
    SAY_WHISPER                               = -1608044,

    SPELL_SHROUD_OF_DARKNESS                  = 54524,
    SPELL_SHROUD_OF_DARKNESS_H                = 59745,
    SPELL_SUMMON_VOID_SENTRY                  = 54369,
    SPELL_VOID_SHIFT                          = 54361,
    SPELL_VOID_SHIFT_H                        = 59743,

    NPC_VOID_SENTRY                           = 29364,
    SPELL_VOID_SENTRY_AURA                    = 54341,
    SPELL_VOID_SENTRY_AURA_H                  = 54351,
    SPELL_SHADOW_BOLT_VOLLEY                  = 54358, // 54342? 54358?
    SPELL_SHADOW_BOLT_VOLLEY_H                = 59747,
    SPELL_VOID_SHIFTED                        = 54343
};

struct MANGOS_DLL_DECL boss_zuramatAI : public ScriptedAI
{
    boss_zuramatAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance *m_pInstance;

    bool m_bIsRegularMode;
    bool MovementStarted;
    std::list<ObjectGuid> m_lSentryGUIDList;

    uint32 m_uiShroudDarkness_Timer;
    uint32 m_uiVoidShift_Timer;
    uint32 m_uiSummonVoidSentry_Timer;

    void Reset()
    {
        m_uiShroudDarkness_Timer = urand(8000, 9000);
        m_uiSummonVoidSentry_Timer = 10000;
        m_uiVoidShift_Timer = 10000;
        MovementStarted = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_ZURAMAT, FAIL);
            m_pInstance->SetData(TYPE_EVENT, FAIL);
            m_pInstance->SetData(TYPE_RIFT, FAIL);

            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) 
                m_pInstance->SetData(TYPE_PORTAL6, NOT_STARTED);
            else 
                m_pInstance->SetData(TYPE_PORTAL12, NOT_STARTED);
        }
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;
        DoScriptText(SAY_AGGRO, m_creature);
        m_pInstance->SetData(TYPE_ZURAMAT, IN_PROGRESS);
        m_creature->GetMotionMaster()->MovementExpired();
        SetCombatMovement(true);
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_ZURAMAT) != SPECIAL && m_pInstance->GetData(TYPE_ZURAMAT) != IN_PROGRESS)
            return;

        if (!pWho || pWho == m_creature)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        m_lSentryGUIDList.push_back(pSummoned->GetObjectGuid());
        //pSummoned->AddThreat(m_creature);
        //pSummoned->AI()->AttackStart(m_creature);
    }

    void DespawnSentry()
    {
        if (m_lSentryGUIDList.empty())
            return;

        for(std::list<ObjectGuid>::iterator itr = m_lSentryGUIDList.begin(); itr != m_lSentryGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->isAlive())
                    //pTemp->ForcedDespawn();
                    pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        m_lSentryGUIDList.clear();
    }

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
            MovementStarted = false;
            m_creature->GetMotionMaster()->MovementExpired();
            SetCombatMovement(true);
            m_creature->SetInCombatWithZone();
        }
    }

    void UpdateAI(const uint32 uiDiff) 
    {
        if (m_pInstance->GetData(TYPE_ZURAMAT) == SPECIAL && !MovementStarted)
            StartMovement(0);

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShroudDarkness_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_SHROUD_OF_DARKNESS : SPELL_SHROUD_OF_DARKNESS_H);
            m_uiShroudDarkness_Timer = urand(7000, 8000);
        }
        else m_uiShroudDarkness_Timer -= uiDiff;

        if (m_uiVoidShift_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                DoCast(pTarget, m_bIsRegularMode ? SPELL_VOID_SHIFT : SPELL_VOID_SHIFT_H);
                pTarget->CastSpell(pTarget, SPELL_VOID_SHIFTED, true);
            }
            m_uiVoidShift_Timer = urand(10000, 11000);
        }
        else m_uiVoidShift_Timer -= uiDiff;

        if (m_uiSummonVoidSentry_Timer < uiDiff)
        {
            m_creature->SummonCreature(NPC_VOID_SENTRY, m_creature->GetPositionX()-10+rand()%20, m_creature->GetPositionY()-10+rand()%20, m_creature->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_uiSummonVoidSentry_Timer = urand(10000, 10000);
        }
        else m_uiSummonVoidSentry_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnSentry();

        if (m_pInstance){
            m_pInstance->SetData(TYPE_ZURAMAT, DONE);
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

struct MANGOS_DLL_DECL mob_zuramat_sentryAI : public ScriptedAI
{
    mob_zuramat_sentryAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        Reset();
    }
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiShadowBoltVolley_Timer;

    void Reset()
    {
        m_uiShadowBoltVolley_Timer = 3000;
//        DoCast(m_creature, m_bIsRegularMode ? SPELL_VOID_SENTRY_AURA_H : SPELL_VOID_SENTRY_AURA); 
//        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);*/ 
        DoCast(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT_VOLLEY : SPELL_SHADOW_BOLT_VOLLEY_H);
    }

    void DamageTaken(Unit *killer, uint32 &damage)
    {
        if(!killer->HasAura(SPELL_VOID_SHIFTED))
            damage=0;
    }

    void UpdateAI(const uint32 uiDiff) 
    {
        if (m_uiShadowBoltVolley_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT_VOLLEY : SPELL_SHADOW_BOLT_VOLLEY_H);
            m_uiShadowBoltVolley_Timer = 3000;
        }
        else m_uiShadowBoltVolley_Timer -= uiDiff;
    }

};

CreatureAI* GetAI_boss_zuramat(Creature* pCreature)
{
    return new boss_zuramatAI (pCreature);
}

CreatureAI* GetAI_mob_zuramat_sentry(Creature* pCreature)
{
    return new mob_zuramat_sentryAI (pCreature);
}

void AddSC_boss_zuramat()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_zuramat";
    newscript->GetAI = &GetAI_boss_zuramat;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_zuramat_sentry";
    newscript->GetAI = &GetAI_mob_zuramat_sentry;
    newscript->RegisterSelf();

}
