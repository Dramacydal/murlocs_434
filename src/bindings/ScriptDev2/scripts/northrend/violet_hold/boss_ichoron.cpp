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
SDName: boss_ichoron
SDAuthor: ckegg
SD%Complete: 30%
SDComment: 
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"


enum
{
    SAY_AGGRO                                 = -1608018,
    SAY_SLAY_1                                = -1608019,
    SAY_SLAY_2                                = -1608020,
    SAY_SLAY_3                                = -1608021,
    SAY_DEATH                                 = -1608022,
    SAY_SPAWN                                 = -1608023,
    SAY_ENRAGE                                = -1608024,
    SAY_SHATTER                               = -1608025,
    SAY_BUBBLE                                = -1608026,
    EMOTE_ICHORON_PROTECTIVE_BUBBLE           = -1608008,

    SPELL_DRAINED                             = 59820,
    SPELL_FRENZY                              = 54312,
    SPELL_FRENZY_H                            = 59522,
    SPELL_PROTECTIVE_BUBBLE                   = 54306,
    SPELL_WATER_BLAST                         = 54237,
    SPELL_WATER_BLAST_H                       = 59520,
    SPELL_WATER_BOLT_VOLLEY                   = 54241,
    SPELL_WATER_BOLT_VOLLEY_H                 = 59521,

    NPC_ICHOR_GLOBULE                         = 29321,
    SPELL_SPLASH                              = 59516,
    SPELL_WATER_GLOBULE                       = 54268,
    SPELL_WATER_GLOBULE_2                     = 54260,
    GLOBULE_HEAL_H                            = 9000,
    GLOBULE_HEAL                              = 5000
};

struct MANGOS_DLL_DECL boss_ichoronAI : public ScriptedAI
{
    boss_ichoronAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance *m_pInstance;
    std::list<ObjectGuid> m_lWaterElementsGUIDList;

    bool m_bIsRegularMode;
    bool m_bIsExploded;
    bool m_bIsFrenzy;
    bool MovementStarted;

    uint32 m_uiBuubleChecker_Timer;
    uint32 m_uiWaterBoltVolley_Timer;
    uint32 m_uiShowup_Counter;
    uint32 m_uiVisible_Timer;
    uint32 m_uiHealth;

    void Reset()
    {
        if (!m_pInstance) return;
        DoCastSpellIfCan(m_creature, SPELL_PROTECTIVE_BUBBLE);
        m_bIsExploded = false;
        m_bIsFrenzy = false;
        MovementStarted = false;
        m_uiBuubleChecker_Timer = 1000;
       // m_uiDrained_Timer = 1000;
        m_uiVisible_Timer = 0;
        m_uiWaterBoltVolley_Timer = urand(5000, 10000);
       // m_uiShowup_Counter = 0;

        m_creature->SetVisibility(VISIBILITY_ON);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        DespawnWaterElements();
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_ICHORON, FAIL);
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
        m_pInstance->SetData(TYPE_ICHORON, IN_PROGRESS);
        SetCombatMovement(true);
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_ICHORON) != SPECIAL && m_pInstance->GetData(TYPE_ICHORON) != IN_PROGRESS)
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

    void WaterElementHit()
    {
       if (Creature* pIchoron = m_pInstance->GetSingleCreatureFromStorage(DATA_ICHORON))
       {
          if(pIchoron->isAlive())
          {
            pIchoron->ModifyHealth( m_bIsRegularMode ? GLOBULE_HEAL : GLOBULE_HEAL_H);
            if (m_bIsExploded)
            {
                if(m_creature->HasAura(SPELL_DRAINED))
                {
                    m_creature->RemoveAurasByCasterSpell(SPELL_DRAINED,m_creature->GetObjectGuid());
                }
                m_creature->SetVisibility(VISIBILITY_ON);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
           }
       }
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetSpeedRate(MOVE_RUN, 0.2f);
        pSummoned->GetMotionMaster()->MoveFollow(m_creature, 0, 0);
        pSummoned->CastSpell(pSummoned, SPELL_WATER_GLOBULE, false);
        m_lWaterElementsGUIDList.push_back(pSummoned->GetObjectGuid());
    }
   void DespawnWaterElements()
    {
        if (m_lWaterElementsGUIDList.empty())
            return;

        for(std::list<ObjectGuid>::iterator itr = m_lWaterElementsGUIDList.begin(); itr != m_lWaterElementsGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->isAlive())
                    //pTemp->ForcedDespawn();
                    pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }

        m_lWaterElementsGUIDList.clear();
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
        if (type != POINT_MOTION_TYPE || !MovementStarted || m_creature->GetVisibility() == VISIBILITY_OFF ) return;
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
        if (m_pInstance->GetData(TYPE_ICHORON) == SPECIAL && !MovementStarted)
           StartMovement(0);

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_bIsFrenzy)
        {
            if (m_uiBuubleChecker_Timer < uiDiff)
            {
                if(!m_creature->HasAura(SPELL_PROTECTIVE_BUBBLE) && m_creature->GetVisibility() == VISIBILITY_ON && m_bIsExploded)
                {
                    DoCastSpellIfCan(m_creature, SPELL_PROTECTIVE_BUBBLE);
                    DoScriptText(SAY_BUBBLE, m_creature);
                    m_bIsExploded = false;
                    m_uiBuubleChecker_Timer = 3000;
                }
                if(!m_creature->HasAura(SPELL_PROTECTIVE_BUBBLE) && m_creature->GetVisibility() == VISIBILITY_ON && !m_bIsExploded)
                {
                    DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_WATER_BLAST : SPELL_WATER_BLAST_H);

                    for(uint8 i = 0; i < 10; i++)
                        {
                            int tmp = urand(0, 5);
                            m_creature->SummonCreature(NPC_ICHOR_GLOBULE, PortalLoc[tmp].x, PortalLoc[tmp].y, PortalLoc[tmp].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                        }
                    DoScriptText(EMOTE_ICHORON_PROTECTIVE_BUBBLE, m_creature);
                    //DoScriptText(SAY_SHATTER, m_creature);
                    m_creature->DealDamage(m_creature, m_creature->GetMaxHealth()*0.25, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    m_creature->SetVisibility(VISIBILITY_OFF);
                    m_bIsExploded = true;
                    m_uiVisible_Timer = 15000;
                }

            }
            else m_uiBuubleChecker_Timer -= uiDiff;

            if(m_creature->GetVisibility() == VISIBILITY_OFF && m_uiVisible_Timer < uiDiff)
            {
                m_creature->SetVisibility(VISIBILITY_ON);
                m_uiVisible_Timer = 3000;
            }
            else m_uiVisible_Timer -= uiDiff;

        }
        if (m_creature->GetVisibility() == VISIBILITY_OFF && !m_creature->HasAura(SPELL_DRAINED) )
        {
            DoCastSpellIfCan(m_creature, SPELL_DRAINED);
        }

        if (m_creature->GetVisibility() == VISIBILITY_ON )
        {
            if (m_uiWaterBoltVolley_Timer < uiDiff)
            {
                DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WATER_BOLT_VOLLEY : SPELL_WATER_BOLT_VOLLEY_H);
                m_uiWaterBoltVolley_Timer = urand(15000, 20000);
            }
            else m_uiWaterBoltVolley_Timer -= uiDiff;

        }
        if (!m_bIsFrenzy && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 25)
            {
                DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FRENZY : SPELL_FRENZY_H);
                DoScriptText(SAY_ENRAGE, m_creature);
                if(m_creature->HasAura(SPELL_DRAINED))
                {
                    m_creature->RemoveAurasByCasterSpell(SPELL_DRAINED,m_creature->GetObjectGuid());
                }
                if(m_creature->GetVisibility() == VISIBILITY_OFF){m_creature->SetVisibility(VISIBILITY_ON);}
                m_bIsFrenzy = true;
            }
        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnWaterElements();

        if (m_pInstance){
            m_pInstance->SetData(TYPE_ICHORON, DONE);
            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, DONE);}
            else {m_pInstance->SetData(TYPE_PORTAL12, DONE);}
        }

        if(m_creature->GetVisibility() == VISIBILITY_OFF)
            m_creature->SetVisibility(VISIBILITY_ON);
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


struct MANGOS_DLL_DECL mob_ichor_globuleAI : public ScriptedAI
{
    mob_ichor_globuleAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }
    ScriptedInstance *m_pInstance;

    uint32 m_uiRangeCheck_Timer;

    void Reset()
    {
        m_uiRangeCheck_Timer = 1000;
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiRangeCheck_Timer < uiDiff)
        {
            if (m_pInstance)
            {
                if (Creature* pIchoron = m_pInstance->GetSingleCreatureFromStorage(DATA_ICHORON))
                {
                    float fDistance = m_creature->GetDistance2d(pIchoron);
                    if (fDistance <= 2)
                    {
                        ((boss_ichoronAI*)pIchoron->AI())->WaterElementHit();

                        m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                }
            }
            m_uiRangeCheck_Timer = 1000;
        }
        else m_uiRangeCheck_Timer -= uiDiff;
    }

    void JustDied(Unit* pKiller)
    {
        DoCast(m_creature, SPELL_SPLASH);
        DoCast(m_creature, SPELL_WATER_GLOBULE_2);

    }
};

CreatureAI* GetAI_boss_ichoron(Creature* pCreature)
{
    return new boss_ichoronAI (pCreature);
}

CreatureAI* GetAI_mob_ichor_globule(Creature* pCreature)
{
    return new mob_ichor_globuleAI (pCreature);
}

void AddSC_boss_ichoron()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_ichoron";
    newscript->GetAI = &GetAI_boss_ichoron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ichor_globule";
    newscript->GetAI = &GetAI_mob_ichor_globule;
    newscript->RegisterSelf();
}
