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
SDName: boss_lavanthor
SDAuthor: ckegg
SD%Complete: 0
SDComment: 
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

enum
{
    SPELL_CAUTERIZING_FLAMES                  = 59466,
    SPELL_FIREBOLT                            = 54235,
    SPELL_FIREBOLT_H                          = 59468,
    SPELL_FLAME_BREATH                        = 54282,
    SPELL_FLAME_BREATH_H                      = 59469,
    SPELL_LAVA_BURN                           = 54249,
    SPELL_LAVA_BURN_H                         = 59594,
};

struct MANGOS_DLL_DECL boss_lavanthorAI : public ScriptedAI
{
    boss_lavanthorAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance *m_pInstance;

    bool m_bIsRegularMode;
    bool MovementStarted;

    uint32 m_uiCauterizingFlames_Timer;
    uint32 m_uiFlameBreath_Timer;
    uint32 m_uiFirebolt_Timer;

    void Reset()
    {
        if (!m_pInstance) return;
        m_uiCauterizingFlames_Timer = urand(40000, 41000);
        m_uiFlameBreath_Timer = urand(15000, 16000);
        m_uiFirebolt_Timer = urand(10000, 11000);
        MovementStarted = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_LAVANTHOR, FAIL);
            m_pInstance->SetData(TYPE_EVENT, FAIL);
            m_pInstance->SetData(TYPE_RIFT, FAIL);
            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, NOT_STARTED);}
            else {m_pInstance->SetData(TYPE_PORTAL12, NOT_STARTED);}
            }
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;
        m_pInstance->SetData(TYPE_LAVANTHOR, IN_PROGRESS);
        m_creature->GetMotionMaster()->MovementExpired();
        SetCombatMovement(true);
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_LAVANTHOR) != SPECIAL && m_pInstance->GetData(TYPE_LAVANTHOR) != IN_PROGRESS)
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
        if (id == 0 )
        {
            MovementStarted = false;
            m_creature->GetMotionMaster()->MovementExpired();
            SetCombatMovement(true);
            m_creature->SetInCombatWithZone();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_LAVANTHOR) == SPECIAL && !MovementStarted)
            StartMovement(0);

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCauterizingFlames_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_CAUTERIZING_FLAMES);
            m_uiCauterizingFlames_Timer = urand(40000, 41000);
        }
        else m_uiCauterizingFlames_Timer -= uiDiff;

        if (m_uiFirebolt_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FIREBOLT: SPELL_FIREBOLT_H );
            m_uiFirebolt_Timer = urand(10000, 11000);
        }
        else m_uiFirebolt_Timer -= uiDiff;

        if (m_uiFlameBreath_Timer < uiDiff)
        {
            switch (urand(0, 1))
            {
                case 0:
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H);
                    break;
                case 1:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_LAVA_BURN : SPELL_LAVA_BURN_H);
                    break;
            }
            m_uiFlameBreath_Timer = urand(15000, 16000);
        }
        else m_uiFlameBreath_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance){
            m_pInstance->SetData(TYPE_LAVANTHOR, DONE);
            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, DONE);}
            else {m_pInstance->SetData(TYPE_PORTAL12, DONE);}
        }
    }
};

CreatureAI* GetAI_boss_lavanthor(Creature* pCreature)
{
    return new boss_lavanthorAI (pCreature);
}

void AddSC_boss_lavanthor()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lavanthor";
    newscript->GetAI = &GetAI_boss_lavanthor;
    newscript->RegisterSelf();
}
