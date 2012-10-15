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
SDName: boss_moragg
SDAuthor: ckegg
SD%Complete: 50%
SDComment:
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

enum
{
    SPELL_CORROSICE_SALIVA                = 54527,
    SPELL_OPTIC_LINK                      = 54396,
    SPELL_RAY_PAIN                        = 59525,
    SPELL_RAY_SUFFERING                   = 54417,
};

struct MANGOS_DLL_DECL boss_moraggAI : public ScriptedAI
{
    boss_moraggAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance *m_pInstance;

    bool m_bIsRegularMode;
    uint32 m_uiCorrosiveSaliva_Timer;
    uint32 m_uiOpticLink_Timer;
    uint32 m_uiRay_Timer;

    bool MovementStarted;

    void Reset()
    {
        MovementStarted = false;
        m_uiCorrosiveSaliva_Timer = urand(10000, 11000);
        m_uiOpticLink_Timer = urand(25000, 30000);
        m_uiRay_Timer = urand(2000, 7000);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    }
    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_MORAGG, FAIL);
            m_pInstance->SetData(TYPE_EVENT, FAIL);
            m_pInstance->SetData(TYPE_RIFT, FAIL);
            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, NOT_STARTED);}
            else {m_pInstance->SetData(TYPE_PORTAL12, NOT_STARTED);}
            }
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;
        m_pInstance->SetData(TYPE_MORAGG, IN_PROGRESS);
        m_creature->GetMotionMaster()->MovementExpired();
        SetCombatMovement(true);
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_MORAGG) != SPECIAL && m_pInstance->GetData(TYPE_MORAGG) != IN_PROGRESS)
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
        if (m_pInstance->GetData(TYPE_MORAGG) == SPECIAL && !MovementStarted)
            StartMovement(0);

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCorrosiveSaliva_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CORROSICE_SALIVA);
            m_uiCorrosiveSaliva_Timer = urand(10000, 11000);
        }
        else m_uiCorrosiveSaliva_Timer -= uiDiff;

        if (m_uiOpticLink_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_OPTIC_LINK);
            m_uiOpticLink_Timer = urand(25000, 30000);
        }
        else m_uiOpticLink_Timer -= uiDiff;

        if (m_uiRay_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, urand(0, 1) ? SPELL_RAY_PAIN : SPELL_RAY_SUFFERING);
            m_uiRay_Timer = urand(2000, 7000);
        }
        else m_uiRay_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance){
            m_pInstance->SetData(TYPE_MORAGG, DONE);
        if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, DONE);}
            else {m_pInstance->SetData(TYPE_PORTAL12, DONE);}
        }
    }
};

CreatureAI* GetAI_boss_moragg(Creature* pCreature)
{
    return new boss_moraggAI (pCreature);
}

void AddSC_boss_moragg()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_moragg";
    newscript->GetAI = &GetAI_boss_moragg;
    newscript->RegisterSelf();
}
