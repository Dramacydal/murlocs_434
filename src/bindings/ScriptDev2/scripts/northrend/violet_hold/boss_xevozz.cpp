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
SDName: boss_xevozz
SDAuthor: ckegg
SD%Complete: 60%
SDComment: 
SDCategory: The Violet Hold
EndScriptData */

#include "precompiled.h"
#include "violet_hold.h"

enum
{
    SAY_AGGRO                                 = -1608027,
    SAY_SLAY_1                                = -1608028,
    SAY_SLAY_2                                = -1608029,
    SAY_SLAY_3                                = -1608030,
    SAY_DEATH                                 = -1608031,
    SAY_SPAWN                                 = -1608032,
    SAY_CHARGED                               = -1608033,
    SAY_REPEAT_SUMMON_1                       = -1608034,
    SAY_REPEAT_SUMMON_2                       = -1608035,
    SAY_SUMMON_ENERGY                         = -1608036,

    SPELL_ARCANE_BARRAGE_VOLLEY               = 54202,
    SPELL_ARCANE_BARRAGE_VOLLEY_H             = 59483,
    SPELL_ARCANE_BUFFET                       = 54226,
    SPELL_ARCANE_BUFFET_H                     = 59485,
    SPELL_SUMMON_ETHEREAL_SPHERE_1            = 54102,
    SPELL_SUMMON_ETHEREAL_SPHERE_2            = 54137,
    SPELL_SUMMON_ETHEREAL_SPHERE_3            = 54138,

    NPC_ETHEREAL_SPHERE                       = 29271,
    //NPC_ETHEREAL_SPHERE2                      = 32582, // heroic only?
    SPELL_ARCANE_POWER                        = 54160,
    SPELL_ARCANE_POWER_H                      = 59474,
    SPELL_SUMMON_PLAYERS                      = 54164,
    SPELL_ARCANE_TEMPEST                      = 35845,
    SPELL_ARCANE_TEMPEST_H                    = 49366,
    SPELL_ETHEREAL_BEACON_VISUAL              = 32368
};

struct MANGOS_DLL_DECL boss_xevozzAI : public ScriptedAI
{
    boss_xevozzAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance *m_pInstance;

    bool m_bIsRegularMode;
    bool MovementStarted;

    uint32 m_uiSummonEtherealSphere_Timer;
    uint32 m_uiArcaneBarrageVolley_Timer;
    uint32 m_uiArcaneBuffet_Timer;
    uint32 m_uiDespawn_Timer;

    void Reset()
    {
        m_uiSummonEtherealSphere_Timer = 10000;
        m_uiDespawn_Timer  = 35000;
        m_uiArcaneBarrageVolley_Timer = urand(20000, 22000);
        m_uiArcaneBuffet_Timer = m_uiSummonEtherealSphere_Timer + urand(5000, 6000);
        DespawnSphere();
        MovementStarted = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;

        DoScriptText(SAY_AGGRO, m_creature);
        m_pInstance->SetData(TYPE_XEVOZZ, IN_PROGRESS);
        m_creature->GetMotionMaster()->MovementExpired();
        SetCombatMovement(true);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_XEVOZZ, FAIL);
            m_pInstance->SetData(TYPE_EVENT, FAIL);
            m_pInstance->SetData(TYPE_RIFT, FAIL);
            if(m_pInstance->GetData(TYPE_PORTAL6) == IN_PROGRESS) {m_pInstance->SetData(TYPE_PORTAL6, NOT_STARTED);}
            else {m_pInstance->SetData(TYPE_PORTAL12, NOT_STARTED);}
            }
    }

    void AttackStart(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_XEVOZZ) != SPECIAL && m_pInstance->GetData(TYPE_XEVOZZ) != IN_PROGRESS)
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

    void DespawnSphere()
    {
        std::list<Creature*> assistList;
        GetCreatureListWithEntryInGrid(assistList,m_creature, NPC_ETHEREAL_SPHERE ,150.0f);

        if (assistList.empty())
            return;

        for(std::list<Creature*>::iterator iter = assistList.begin(); iter != assistList.end(); ++iter)
            (*iter)->DealDamage((*iter), (*iter)->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void JustSummoned(Creature* pSummoned)
    {
        pSummoned->SetSpeedRate(MOVE_RUN, 0.5f);
        pSummoned->StopMoving();
        pSummoned->CastSpell(pSummoned, SPELL_ETHEREAL_BEACON_VISUAL, false);
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
        if (m_pInstance->GetData(TYPE_XEVOZZ) == SPECIAL && !MovementStarted)
            StartMovement(0);

        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcaneBarrageVolley_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BARRAGE_VOLLEY : SPELL_ARCANE_BARRAGE_VOLLEY_H);
            m_uiArcaneBarrageVolley_Timer = urand(20000, 22000);
        }
        else m_uiArcaneBarrageVolley_Timer -= uiDiff;

        if (m_uiArcaneBuffet_Timer)
            if (m_uiArcaneBuffet_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_BUFFET : SPELL_ARCANE_BUFFET_H);
                m_uiArcaneBuffet_Timer = 0;
            }
            else m_uiArcaneBuffet_Timer -= uiDiff;

        if (m_uiSummonEtherealSphere_Timer < uiDiff)
        {
            DoScriptText(SAY_SPAWN, m_creature);
            DoCast(m_creature, SPELL_SUMMON_ETHEREAL_SPHERE_1);
            if (!m_bIsRegularMode) // extra one for heroic
                m_creature->SummonCreature(NPC_ETHEREAL_SPHERE, m_creature->GetPositionX()-5+rand()%10, m_creature->GetPositionY()-5+rand()%10, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 40000);

            m_uiSummonEtherealSphere_Timer = 30000;
            m_uiArcaneBuffet_Timer = urand(5000, 6000);
            m_uiDespawn_Timer= 30000;
        }
        else m_uiSummonEtherealSphere_Timer -= uiDiff;

        if (m_uiDespawn_Timer < uiDiff)
        {
            DespawnSphere();
        }
        else m_uiDespawn_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnSphere();

        if (m_pInstance){
            m_pInstance->SetData(TYPE_XEVOZZ, DONE);
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

struct MANGOS_DLL_DECL mob_ethereal_sphereAI : public ScriptedAI
{
    mob_ethereal_sphereAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }
    ScriptedInstance *m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonPlayers_Timer;
    uint32 m_uiRangeCheck_Timer;
    uint32 m_uiPhaseCheck_Timer;
    bool bNormalPhase;


    void Reset()
    {
        m_uiSummonPlayers_Timer = 5000;
        m_uiRangeCheck_Timer = 1000;
        m_uiPhaseCheck_Timer = 10000;
        bNormalPhase = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        //Return since we have no target
       /* if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;*/ 
        if(!bNormalPhase)
        {
            if (m_uiPhaseCheck_Timer < uiDiff )
            {
            bNormalPhase = true;
            m_uiPhaseCheck_Timer=3000000;
            Creature* pXevozz = m_pInstance->GetSingleCreatureFromStorage(DATA_XEVOZZ);
            m_creature->GetMotionMaster()->MoveFollow(pXevozz, 0, 0);
            }
            else m_uiPhaseCheck_Timer -= uiDiff;
        }
        if(bNormalPhase)
        {

            if (m_uiRangeCheck_Timer < uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_TEMPEST: SPELL_ARCANE_TEMPEST_H);
                if (m_pInstance)
                {
                    if (Creature* pXevozz = m_pInstance->GetSingleCreatureFromStorage(DATA_XEVOZZ))
                    {
                        float fDistance = m_creature->GetDistance2d(pXevozz);
                        if (fDistance <= 3)
                        {
                            m_creature->CastSpell(pXevozz, SPELL_ARCANE_POWER_H, false);
                        // DoCast(pXevozz,SPELL_ARCANE_POWER_H);
                        }
                    }
                }
                m_uiRangeCheck_Timer = 1000;
            }
            else m_uiRangeCheck_Timer -= uiDiff;

            if (m_uiSummonPlayers_Timer < uiDiff)
            {
                if (Creature* pXevozz = (m_pInstance->GetSingleCreatureFromStorage(DATA_XEVOZZ)))
                {
                    float fDistance = m_creature->GetDistance2d(pXevozz);

                    if(fDistance<=20 && fDistance >6)
                    {
                        DoCast(m_creature, SPELL_SUMMON_PLAYERS); // not working right

                        Map* pMap = m_creature->GetMap();
                        if (pMap && pMap->IsDungeon())
                        {
                            Map::PlayerList const &PlayerList = pMap->GetPlayers();

                            if (!PlayerList.isEmpty())
                                for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                                    if (i->getSource()->isAlive())
                                        DoTeleportPlayer(i->getSource(), m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), i->getSource()->GetOrientation());
                        }

                       m_uiSummonPlayers_Timer = 15000;
                     }
                  }
            }
            else m_uiSummonPlayers_Timer -= uiDiff; 
        }
    }
};

CreatureAI* GetAI_boss_xevozz(Creature* pCreature)
{
    return new boss_xevozzAI (pCreature);
}

CreatureAI* GetAI_mob_ethereal_sphere(Creature* pCreature)
{
    return new mob_ethereal_sphereAI (pCreature);
}

void AddSC_boss_xevozz()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_xevozz";
    newscript->GetAI = &GetAI_boss_xevozz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ethereal_sphere";
    newscript->GetAI = &GetAI_mob_ethereal_sphere;
    newscript->RegisterSelf();
}
