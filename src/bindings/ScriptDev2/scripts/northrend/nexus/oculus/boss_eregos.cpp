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
SDName: Boss_Eregos
SD%Complete: 60%
SDComment:
SDAuthor: originally from TC, reworked by MaxXx2021 Aka Mioka, corrected by /dev/rsa
SDCategory: Oculus
EndScriptData */

#include "precompiled.h"
#include "oculus.h"

enum Spells
{
    SAY_AGGRO            = -1578030,
    SAY_KILL_1           = -1578031,
    SAY_KILL_2           = -1578032,
    SAY_KILL_3           = -1578033,
    SAY_DEATH            = -1578034,
    SAY_ARCANE_SHIELD    = -1578035,
    SAY_FIRE_SHIELD      = -1578036,
    SAY_NATURE_SHIELD    = -1578037,
    SAY_FRENZY           = -1578038,
    SAY_SPAWN            = -1578039,

    SPELL_ARCANE_BARRAGE_N                        = 50804,
    SPELL_ARCANE_BARRAGE_H                        = 59381,
    SPELL_ARCANE_VOLLEY_N                         = 51153,
    SPELL_ARCANE_VOLLEY_H                         = 59382,
    SPELL_ENRAGED_ASSAULT                         = 51170,
    SPELL_SUMMON_DRAKE                            = 51175,
    SPELL_PLANAR_ANOMALIES                        = 57959,
    SPELL_PLANAR_ANOMALIES_SUMMON                 = 57963, //need spell script on m_creature 27656
    SPELL_PLANAR_ANOMALIES_VISUAL                 = 57971,
    SPELL_PLANAR_SHIFT                            = 51162,
    SPELL_PLANAR_ANOMALIES_DMG                    = 57976,

    NPC_PLANAR_ANOMALY                            = 30879,
    NPC_DRAGON                                    = 28276
};

struct MANGOS_DLL_DECL boss_eregosAI : public ScriptedAI
{
    boss_eregosAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 uiArcaneBarrageTimer;
    uint32 uiArcaneVolleyTimer;
    uint32 uiEnragedAssaultTimer;
    uint32 uiSummonTimer;

    uint8 m_uiSummonCount;

    bool m_bIsMove;

    void Reset()
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        if (m_pInstance)
        {
           m_pInstance->SetData(TYPE_EREGOS, NOT_STARTED);
           if(m_pInstance->GetData(TYPE_UROM) == DONE)
           {
              m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           }
           else
           {
              m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
           }
        }

        m_creature->SetLevitate(true);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        uiArcaneBarrageTimer = 12000;
        uiArcaneVolleyTimer = 17000;
        uiEnragedAssaultTimer = 24000;
        uiSummonTimer = 15000;
        m_uiSummonCount = 0;
        m_bIsMove = true;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if(pWho->GetTypeId() == TYPEID_PLAYER
           && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE)
           && !((Player*)pWho)->isGameMaster()
           && m_creature->IsWithinDistInMap(pWho, 100.0f)
           && pWho->GetVehicle()
           && m_pInstance->GetData(TYPE_UROM) == DONE)
        {
              m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EREGOS, IN_PROGRESS);
    }

    void KilledUnit(Unit *victim)
    {
        uint8 uiText = urand(0, 2);
        switch (uiText)
        {
           case 0: DoScriptText(SAY_KILL_1, m_creature); break;
           case 1: DoScriptText(SAY_KILL_2, m_creature); break;
           case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* killer)
    {
        m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, 0);
        m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()-100.0f, 26);
            DoScriptText(SAY_DEATH, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EREGOS, DONE);
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
        if(m_creature->HasAura(SPELL_PLANAR_SHIFT))
           damage = 0;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_PLANAR_ANOMALY)
        {
           pSummoned->SetDisplayId(11686);
           pSummoned->SetInCombatWithZone();
           if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
              pSummoned->AI()->AttackStart(pTarget);
        }

        if(pSummoned->GetEntry() == NPC_DRAGON)
        {
           pSummoned->SetInCombatWithZone();
           if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
              pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void AttackStart(Unit* who)
    {
        if (!who)
            return;

        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            return;

        ScriptedAI::AttackStart(who);
    }

    void SummonAnomalies()
    {
        std::list<HostileReference *> t_list = m_creature->getThreatManager().getThreatList();
        if (t_list.size())
        {
            for (std::list<HostileReference *>::iterator itr = t_list.begin(); itr != t_list.end(); ++itr)
            {
                Unit *pTarget = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());
                if (pTarget && pTarget->isAlive() && pTarget->GetTypeId() == TYPEID_PLAYER)
                   DoCast(m_creature, SPELL_PLANAR_ANOMALIES_SUMMON, true);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 50.0f && m_uiSummonCount == 0)
        {
           SummonAnomalies();
           m_uiSummonCount = 1;
           m_creature->CastSpell(m_creature, SPELL_PLANAR_SHIFT,true);
        }

        if(m_creature->GetHealthPercent() < 25.0f && m_uiSummonCount == 1)
        {
            SummonAnomalies();
            m_uiSummonCount = 2;
            m_creature->CastSpell(m_creature, SPELL_PLANAR_SHIFT,true);
        }

        if(m_creature->GetDistance2d(m_creature->getVictim()) > 35.0f && !m_bIsMove)
        {
           m_bIsMove = true;
           SetCombatMovement(m_bIsMove);
           if(Unit* pTarget = m_creature->getVictim())
              m_creature->GetMotionMaster()->MoveChase(pTarget);
        }

        if(m_creature->GetDistance2d(m_creature->getVictim()) < 20.0f && m_bIsMove)
        {
           m_bIsMove = false;
           SetCombatMovement(m_bIsMove);
           m_creature->GetMotionMaster()->Clear(false);
           m_creature->GetMotionMaster()->MoveIdle();
           m_creature->StopMoving();
        }

        if(uiArcaneBarrageTimer <= diff)
        {
           if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
               DoCast(pTarget, m_bIsRegularMode ? SPELL_ARCANE_BARRAGE_N : SPELL_ARCANE_BARRAGE_H);
              uiArcaneBarrageTimer = 3000;
        } else uiArcaneBarrageTimer -= diff;

        if(uiSummonTimer <= diff)
        {
           for(uint8 i = 1; i < 3; i++)
               DoCast(m_creature, SPELL_SUMMON_DRAKE, true);
           uiSummonTimer = 15000;
        } else uiSummonTimer -= diff;

        if(uiArcaneVolleyTimer <= diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_VOLLEY_N : SPELL_ARCANE_VOLLEY_H);
            uiArcaneVolleyTimer = 17000;
        } else uiArcaneVolleyTimer -= diff;

        if(uiEnragedAssaultTimer <= diff)
        {
            DoCast(m_creature, SPELL_ENRAGED_ASSAULT, true);
            uiEnragedAssaultTimer = 44000;
        } else uiEnragedAssaultTimer -= diff;
    }
};

struct MANGOS_DLL_DECL npc_planar_anomalyAI : public ScriptedAI
{
    npc_planar_anomalyAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
       Reset();
    }

    uint32 uiPulseTimer;
    uint32 uiDeathTimer;

    void Reset()
    {
        m_creature->SetDisplayId(11686);
        m_creature->SetObjectScale(2.0f);
        m_creature->SetLevitate(true);
        m_creature->SetSpeedRate(MOVE_RUN, 1.5, true);
        m_creature->setFaction(14);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature, SPELL_PLANAR_ANOMALIES_VISUAL, true);
        uiDeathTimer = 20500;
        uiPulseTimer = 19000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiPulseTimer < diff)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_PLANAR_ANOMALIES_VISUAL);
            m_creature->CastSpell(m_creature, SPELL_PLANAR_ANOMALIES_DMG, true);
            uiPulseTimer = 6000;
        } else uiPulseTimer -= diff;

        if (uiDeathTimer < diff)
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else uiDeathTimer -= diff;
    }
};

CreatureAI* GetAI_boss_eregos(Creature* pCreature)
{
    return new boss_eregosAI (pCreature);
}

CreatureAI* GetAI_npc_planar_anomaly(Creature* pCreature)
{
    return new npc_planar_anomalyAI (pCreature);
}

void AddSC_boss_eregos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_eregos";
    newscript->GetAI = &GetAI_boss_eregos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_planar_anomaly";
    newscript->GetAI = &GetAI_npc_planar_anomaly;
    newscript->RegisterSelf();
}
