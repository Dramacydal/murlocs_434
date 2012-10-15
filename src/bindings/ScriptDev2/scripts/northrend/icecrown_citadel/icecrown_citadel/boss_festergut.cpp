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
SDName: boss_festergut
SD%Complete: 90%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */
// Need correct timers
#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_GASEOUS_BLIGHT_1   = 69157,
    SPELL_GASEOUS_BLIGHT_2   = 69162,
    SPELL_GASEOUS_BLIGHT_3   = 69164,
    SPELL_BLIGHT_VISUAL_1    = 69126,
    SPELL_BLIGHT_VISUAL_2    = 69152,
    SPELL_BLIGHT_VISUAL_3    = 69154,
    SPELL_INHALE_BLIGHT      = 69165,
    SPELL_INHALED_BLIGHT     = 69166,
    SPELL_PUNGENT_BLIGHT     = 69195,
    SPELL_GAS_SPORE          = 69278,
    SPELL_SPORE_AURA_0       = 69279,
    SPELL_SPORE_AURA_1       = 69290,
    SPELL_INOCULATE          = 69291,
    SPELL_REMOVE_UNOCULATE   = 69298,
    SPELL_GASTRIC_BLOAT      = 72219,
    SPELL_VILE_GAS           = 72272,
    SPELL_VILE_GAS_AURA      = 69244,
    SPELL_VILE_GAS_AURA_0    = 69248,
    SPELL_BERSERK            = 47008,

    SPELL_GASEOUS_SPIGOT     = 71379,

    SPELL_SUMMON_VILE_STALKER       = 72287,

    NPC_VILE_GAS_STALKER     = 38548,
    NPC_BLIGHT_STALKER       = 36659,
    NPC_PUDDLE_STALKER       = 37013,
    MAX_SPORE_TARGETS        = 6,
};

static Locations SpawnLoc[]=
{
    {4267.9399f, 3137.32f, 360.385986f, 0.0f},  // 0 (start point)
    {4317.067383f, 3136.99f, 360.385590f, 3.21f},  // 1 right
    {4220.801758f, 3136.99f, 360.385590f, 0.39f},  // 2 left
    {4269.084473f, 3186.306641f, 360.385590f, 4.72f},  // 3 rear
};

struct MANGOS_DLL_DECL boss_festergutAI : public BSWScriptedAI
{
    boss_festergutAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    bool intro;
    bool pet;
    uint64 blightTargetGUID;
//    uint64 pPuddleStalkerGUID[3];

    bool IsAttackerPositionOk(Unit* attacker)
    {
        if (!attacker)
            return true;

        float dfX = attacker->GetPositionX() - 4268.05f;
        float dfY = attacker->GetPositionY() - 3137.28f;
        float dfZ = attacker->GetPositionZ() - 360.38f;

        return sqrt(dfX * dfX + dfY * dfY + dfZ * dfZ) <= 55.2f && attacker->GetPositionZ() < 370.0f;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!pDoneBy || !pInstance)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_FESTERGUT))
        {
            pInstance->SendGMEncounterNotification(pDoneBy, TYPE_FESTERGUT);
            EnterEvadeMode();
            uiDamage = 0;
            return;
        }

        if (!IsAttackerPositionOk(pDoneBy))
        {
            uiDamage = 0;
            EnterEvadeMode();
        }
    }

    void Reset()
    {
        if(!pInstance) return;
        resetTimers();
        if (m_creature->isAlive()) 
            pInstance->SetData(TYPE_FESTERGUT, NOT_STARTED);
        setStage(0);
        intro = false;
        pet = false;
/*        for(uint8 i = 0; i < 3; ++i)
             if (!pPuddleStalkerGUID[i])
             {
                 Unit* pTemp = doSummon(NPC_PUDDLE_STALKER,SpawnLoc[i].x, SpawnLoc[i].y, SpawnLoc[i].z, TEMPSUMMON_MANUAL_DESPAWN);
                 if (pTemp)
                 {
                     pPuddleStalkerGUID[i] = pTemp->GetGUID();
                     pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                     pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                 }
             }
*/
        Creature* pBlightTarget = doSelectNearestCreature(NPC_BLIGHT_STALKER,60.0f);

        if (pBlightTarget && !pBlightTarget->isAlive())
            pBlightTarget->Respawn();
        if (pBlightTarget)
        {
             blightTargetGUID = pBlightTarget->GetGUID();
             pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
             pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
             doCast(SPELL_BLIGHT_VISUAL_1,pBlightTarget);
             doCast(SPELL_BLIGHT_VISUAL_2,pBlightTarget);
             doCast(SPELL_BLIGHT_VISUAL_3,pBlightTarget);
        }
    }

    void MoveInLineOfSight(Unit* pWho) 
    {
        ScriptedAI::MoveInLineOfSight(pWho);
        if(!pInstance || intro) return;
        if (pWho->GetTypeId() != TYPEID_PLAYER) return;

        pInstance->SetData(TYPE_EVENT, 500);
        debug_log("EventMGR: creature %u send signal %u ",m_creature->GetEntry(),pInstance->GetData(TYPE_EVENT));
        intro = true;
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0, 1))
        {
        case 0:
           DoScriptText(-1631204,m_creature,pVictim);
           break;
        case 1:
           DoScriptText(-1631205,m_creature,pVictim);
           break;
        }
    }

    void JustReachedHome()
    {
        if (!pInstance)
            return;
        pInstance->SetData(TYPE_FESTERGUT, FAIL);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_1);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_2);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_3);
        if (Creature* pBlightTarget = m_creature->GetMap()->GetCreature(blightTargetGUID))
            doCast(SPELL_BLIGHT_VISUAL_1,pBlightTarget);
    }

    void EnterEvadeMode()
    {
        if (!pInstance) 
            return;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(false);
        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(NULL);
        m_creature->SetHealth(m_creature->GetMaxHealth());

        Reset();
    }


    void Aggro(Unit *pWho)
    {
        if (!pInstance)
            return;
        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_FESTERGUT))
        {
            pInstance->SendGMEncounterNotification(pWho, TYPE_FESTERGUT);
            EnterEvadeMode();
            return;
        }

        Creature* pBlightTarget = m_creature->GetMap()->GetCreature(blightTargetGUID);

        pInstance->SetData(TYPE_FESTERGUT, IN_PROGRESS);
        DoScriptText(-1631203,m_creature,pWho);
        if (pBlightTarget && !pBlightTarget->isAlive())
            pBlightTarget->Respawn();
        if (pBlightTarget)
        {
            pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pBlightTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            doRemove(SPELL_BLIGHT_VISUAL_1,pBlightTarget);
            doRemove(SPELL_BLIGHT_VISUAL_2,pBlightTarget);
            doRemove(SPELL_BLIGHT_VISUAL_3,pBlightTarget);
            doCast(SPELL_BLIGHT_VISUAL_1,pBlightTarget);
        }
        doCast(SPELL_GASEOUS_BLIGHT_1);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_3);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_2);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_1);
/*        for(uint8 i = 0; i < 3; ++i)
             if (pPuddleStalkerGUID[i])
                 doCast(SPELL_GASEOUS_SPIGOT, m_creature->GetMap()->GetCreature(pPuddleStalkerGUID[i]));
*/
    }

    void JustDied(Unit *killer)
    {
        if (!pInstance)
            return;
        pInstance->SetData(TYPE_FESTERGUT, DONE);
        pInstance->SetData(TYPE_EVENT, 550);
        DoScriptText(-1631206,m_creature);
        Creature* pBlightTarget = m_creature->GetMap()->GetCreature(blightTargetGUID);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_1);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_2);
        doRemoveFromAll(SPELL_BLIGHT_VISUAL_3);
        if (pBlightTarget)
            doCast(SPELL_BLIGHT_VISUAL_1,pBlightTarget);
/*        for(uint8 i = 0; i < 3; ++i)
             if (pPuddleStalkerGUID[i])
             {
                 Creature* pTemp = m_creature->GetMap()->GetCreature(pPuddleStalkerGUID[i]);
                 if (pTemp) pTemp->ForcedDespawn();
                 pPuddleStalkerGUID[i] = NULL;
             }
*/
    }

    void UpdateAI(const uint32 diff)
    {
        if(!pInstance)
            return;

        if (!pet) 
        {
            if (Creature* pGuard = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_STINKY)))
                if (!pGuard->isAlive())
                {
                     pet = true;
                     if (pInstance->GetData(TYPE_STINKY) == NOT_STARTED)
                     {
                         DoScriptText(-1631209,m_creature);
                         pInstance->SetData(TYPE_STINKY,DONE);
                     }
                }
        }

        if (m_creature->isInCombat())
        {
            if(!m_creature->getVictim() || (m_creature->getVictim() && m_creature->getVictim()->GetTypeId() == TYPEID_UNIT))
            {
                m_creature->DeleteThreatList();
                Unit* pTarget = doSelectRandomPlayerAtRange(20.0f);
                if (pTarget)
                {
                    m_creature->SetInCombatWith(pTarget);
                    m_creature->AddThreat(pTarget, 100.0f);
                }
                else
                    EnterEvadeMode();
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Creature* pBlightTarget = m_creature->GetMap()->GetCreature(blightTargetGUID);

        switch(getStage())
        {
            case 0:
                if (timedQuery(SPELL_GASEOUS_BLIGHT_2, diff))
                    setStage(1);
                break;
            case 1:
                switch (urand(0,2)) 
                {
                    case 0:  DoScriptText(-1631210,m_creature); break;
                    case 1:  DoScriptText(-1631211,m_creature); break;
                    case 2:  DoScriptText(-1631212,m_creature); break;
                }
                doCast(SPELL_INHALE_BLIGHT);
                setStage(2);
                break;
            case 2:
                if (m_creature->IsNonMeleeSpellCasted(false))
                    return;
                if (pBlightTarget)
                {
                    doRemove(SPELL_GASEOUS_BLIGHT_1);
                    doRemove(SPELL_BLIGHT_VISUAL_1,pBlightTarget);
                    doRemove(SPELL_BLIGHT_VISUAL_1,m_creature);
                    doCast(SPELL_GASEOUS_BLIGHT_2);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_3);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_2);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_1);
                }
                setStage(3);
                break;
            case 3:
                if (timedQuery(SPELL_GASEOUS_BLIGHT_3, diff))
                    setStage(4);
                break;
            case 4:
                switch (urand(0,2))
                {
                    case 0:  DoScriptText(-1631210,m_creature); break;
                    case 1:  DoScriptText(-1631211,m_creature); break;
                    case 2:  DoScriptText(-1631212,m_creature); break;
                }
                doCast(SPELL_INHALE_BLIGHT);
                setStage(5);
                break;
            case 5:
                if (m_creature->IsNonMeleeSpellCasted(false))
                    return;
                if (pBlightTarget)
                {
                    doRemove(SPELL_GASEOUS_BLIGHT_2);
                    doRemove(SPELL_BLIGHT_VISUAL_2,pBlightTarget);
                    doRemove(SPELL_BLIGHT_VISUAL_2,m_creature);
                    doCast(SPELL_GASEOUS_BLIGHT_3);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_3);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_2);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_1);
                }
                setStage(6);
                break;
            case 6:
                if (timedQuery(SPELL_GASEOUS_BLIGHT_3, diff))
                    setStage(7);
                break;
            case 7:
                switch (urand(0,2)) 
                {
                    case 0:  DoScriptText(-1631210,m_creature); break;
                    case 1:  DoScriptText(-1631211,m_creature); break;
                    case 2:  DoScriptText(-1631212,m_creature); break;
                }
                doCast(SPELL_INHALE_BLIGHT);
                setStage(8);
                break;
            case 8:
                if (m_creature->IsNonMeleeSpellCasted(false))
                    return;
                if (pBlightTarget)
                {
                    doRemove(SPELL_GASEOUS_BLIGHT_3);
                    doRemove(SPELL_BLIGHT_VISUAL_3,pBlightTarget);
                    doRemove(SPELL_BLIGHT_VISUAL_3,m_creature);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_3);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_2);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_1);
                }
                setStage(9);
                break;
            case 9:
                if (timedQuery(SPELL_PUNGENT_BLIGHT, diff))
                {
                    DoScriptText(-1631208,m_creature);
                    doCast(SPELL_PUNGENT_BLIGHT);
                    setStage(10);
                }
                break;
            case 10:
                if (m_creature->IsNonMeleeSpellCasted(false))
                    return;
                if (pBlightTarget)
                {
                    doCast(SPELL_BLIGHT_VISUAL_1,pBlightTarget);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_3);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_2);
                    doRemoveFromAll(SPELL_BLIGHT_VISUAL_1);
                }
                m_creature->RemoveAurasDueToSpell(SPELL_INHALED_BLIGHT);
                setStage(0);
                break;
        }


        timedCast(SPELL_GAS_SPORE, diff);

        timedCast(SPELL_GASTRIC_BLOAT, diff);

        if (timedQuery(SPELL_VILE_GAS, diff))
        {
           float fPosX, fPosY, fPosZ;
           m_creature->GetPosition(fPosX, fPosY, fPosZ);
           m_creature->GetRandomPoint(fPosX, fPosY, fPosZ, 30.0f, fPosX, fPosY, fPosZ);
           if (Unit* pTemp = doSummon(NPC_VILE_GAS_STALKER,fPosX, fPosY, fPosZ))
                doCast(SPELL_VILE_GAS, pTemp);
           DoScriptText(-1631213,m_creature);
        }

        if (timedQuery(SPELL_BERSERK, diff))
        {
            doCast(SPELL_BERSERK);
            DoScriptText(-1631207,m_creature);
        }

        DoMeleeAttackIfReady();

        if (m_creature->getVictim())
        {
            if (!IsAttackerPositionOk(m_creature->getVictim()))
                EnterEvadeMode();
        }
    }
};


CreatureAI* GetAI_boss_festergut(Creature* pCreature)
{
    return new boss_festergutAI(pCreature);
}

struct MANGOS_DLL_DECL  mob_vile_gas_stalkerAI : public ScriptedAI
{
    mob_vile_gas_stalkerAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    uint32 m_lifetimer;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetInCombatWithZone();
        SetCombatMovement(false);
        m_creature->SetDisplayId(11686);
        m_lifetimer = 12000;
    }

    void AttackStart(Unit *pWho)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!pInstance || pInstance->GetData(TYPE_FESTERGUT) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_lifetimer <= uiDiff)
            m_creature->ForcedDespawn();
        else
            m_lifetimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_vile_gas_stalker(Creature* pCreature)
{
    return new mob_vile_gas_stalkerAI(pCreature);
}

struct MANGOS_DLL_DECL  mob_orange_gas_stalkerAI : public ScriptedAI
{
    mob_orange_gas_stalkerAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        m_creature->SetDisplayId(11686);
    }

    void AttackStart(Unit *pWho)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
//        if (!pInstance || pInstance->GetData(TYPE_FESTERGUT) != IN_PROGRESS) 
//              m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_mob_orange_gas_stalker(Creature* pCreature)
{
    return new mob_orange_gas_stalkerAI(pCreature);
}

void AddSC_boss_festergut()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_festergut";
    newscript->GetAI = &GetAI_boss_festergut;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_vile_gas_stalker";
    newscript->GetAI = &GetAI_mob_vile_gas_stalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_orange_gas_stalker";
    newscript->GetAI = &GetAI_mob_orange_gas_stalker;
    newscript->RegisterSelf();
}
