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
SDName: Boss_Colossus
SD%Complete: 80%
SDComment: Some visual stuff missing
SDCategory: Gundrak
EndScriptData */

#include "precompiled.h"
#include "gundrak.h"
#include "MapPersistentStateMgr.h"

enum
{
    EMOTE_SURGE                 = -1604008,
    EMOTE_SEEP                  = -1604009,
    EMOTE_GLOW                  = -1604010,

    // Drakkari Collosus' abilities
    SPELL_EMERGE                = 54850,
    SPELL_MIGHTY_BLOW           = 54719,
    SPELL_MORTAL_STRIKES        = 54715,
    SPELL_MORTAL_STRIKES_H      = 59454,
    SPELL_FREEZE_ANIM           = 16245,

    // Drakkari Elemental's abilities
    SPELL_MERGE                 = 54878, // not used
    SPELL_SURGE                 = 54801,
    SPELL_MOJO_VOLLEY_H         = 59453,
    SPELL_MOJO_VOLLEY           = 54849,

    // Living Mojo spells
    SPELL_MOJO_WAVE             = 55626,
    SPELL_MOJO_WAVE_H           = 58993,
    SPELL_MOJO_PUDDLE           = 55627,
    SPELL_MOJO_PUDDLE_H         = 58994,

    PHASE_BEFORE_1ST_SPLIT      = 0,
    PHASE_1ST_ELEMENTAL         = 1,
    PHASE_BEFORE_2ND_SPLIT      = 2,
    PHASE_2ND_ELEMENTAL         = 3,
    PHASE_BEFORE_DEATH          = 4,
    POINT_AT_COLOSSUS_COORDS    = 0
};

/*######
## boss_colossus
######*/

struct MANGOS_DLL_DECL boss_colossusAI : public ScriptedAI
{
    boss_colossusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPhase;
    uint32 m_uiMightyBlowTimer;

    void Reset()
    {
        m_uiPhase = PHASE_BEFORE_1ST_SPLIT;
        m_uiMightyBlowTimer = 10000;
    }

    void Aggro(Unit* pWho)
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MORTAL_STRIKES : SPELL_MORTAL_STRIKES_H);
        m_creature->SetInCombatWithZone();
        m_creature->RemoveAurasDueToSpell(SPELL_FREEZE_ANIM);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_COLOSSUS, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_COLOSSUS, FAIL);

        SetCreature(false);
    }

    void JustDied(Unit* pKiller)
    {
        SetCreature(false);

        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_COLOSSUS, DONE);

            #ifndef WIN32
            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_KILL_CREATURE, NPC_ELEMENTAL, m_creature);
            #endif
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ELEMENTAL)
        {
            DoScriptText(EMOTE_SEEP, pSummoned);
            if (m_uiPhase == PHASE_2ND_ELEMENTAL)
                pSummoned->SetMaxHealth(pSummoned->GetMaxHealth()/2);
        }
    }

    void SetCreature(bool stoned)
    {
        m_creature->RemoveAllAuras();
        if (stoned)
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_COLOSSUS) == IN_PROGRESS)
                DoCastSpellIfCan(m_creature, SPELL_EMERGE, CAST_INTERRUPT_PREVIOUS);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            SetCombatMovement(false);
        }
        else
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_MOJO_VOLLEY : SPELL_MOJO_VOLLEY_H);
            SetCombatMovement(true);
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ELEMENTAL)
        {
            if (m_uiPhase == PHASE_1ST_ELEMENTAL)
            {
                ++m_uiPhase;
                SetCreature(false);
            }
            else if (m_uiPhase == PHASE_2ND_ELEMENTAL)
                m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        switch(m_uiPhase)
        {
            case PHASE_BEFORE_2ND_SPLIT:
                if (m_creature->GetHealth() > uiDamage)
                    break;
            case PHASE_BEFORE_1ST_SPLIT:
                if (m_creature->GetHealthPercent() > 50.0f)
                    break;

                ++m_uiPhase;
                SetCreature(true);                    
                uiDamage = 0;
            default: break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPhase == PHASE_1ST_ELEMENTAL || m_uiPhase == PHASE_2ND_ELEMENTAL)
            return;

        if (m_uiMightyBlowTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_MIGHTY_BLOW);
            m_uiMightyBlowTimer = 10000;
        }
        else
            m_uiMightyBlowTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_colossus(Creature* pCreature)
{
    return new boss_colossusAI(pCreature);
};

/*######
## boss_drakari_elemental
######*/
struct MANGOS_DLL_DECL boss_drakari_elementalAI : public ScriptedAI
{
    boss_drakari_elementalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSurgeTimer;

    void Reset()
    {
        m_uiSurgeTimer      = 5000;
    }

    void JustReachedHome()
    {
        // reset colossus if wiped party
        if(Creature* pColossus = m_pInstance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
            ((boss_colossusAI*)pColossus->AI())->EnterEvadeMode();
                m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        m_creature->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_MOJO_VOLLEY : SPELL_MOJO_VOLLEY_H);
        DoScriptText(EMOTE_GLOW, m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (Creature* pColossus = m_pInstance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
        {
            if (boss_colossusAI* pColossusAI = dynamic_cast<boss_colossusAI*>(pColossus->AI()))
                if (pColossusAI->m_uiPhase == PHASE_1ST_ELEMENTAL && m_creature->GetHealthPercent() < 50.0f)
                {
                    uiDamage = 0;
                    SetCombatMovement(false);
                    float fColoX, fColoY, fColoZ;
                    pColossus->GetPosition(fColoX, fColoY, fColoZ);
                    m_creature->GetMotionMaster()->MovementExpired(true);
                    m_creature->GetMotionMaster()->MovePoint(POINT_AT_COLOSSUS_COORDS, fColoX, fColoY, fColoZ);
                }
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_AT_COLOSSUS_COORDS)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void Aggro(Unit* pWho)
    {
        DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_MOJO_VOLLEY : SPELL_MOJO_VOLLEY_H, CAST_INTERRUPT_PREVIOUS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSurgeTimer < uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                DoScriptText(EMOTE_SURGE, m_creature, pTarget);
                DoResetThreat();
                m_creature->AddThreat(pTarget, 1000.0f);
                m_creature->CastSpell(pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), SPELL_SURGE, false);
            }
            m_uiSurgeTimer = urand(5000, 15000);
        }
        else
            m_uiSurgeTimer -= uiDiff;
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_drakari_elemental(Creature* pCreature)
{
    return new boss_drakari_elementalAI(pCreature);
};

/*######
## npc_living_mojo
######*/

struct MANGOS_DLL_DECL npc_living_mojoAI : public ScriptedAI
{
    npc_living_mojoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (instance_gundrak*)pCreature->GetInstanceData();
        Reset();
    }

    instance_gundrak* m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiMojoPuddleTimer;
    uint32 m_uiMojoWaveTimer;

    void Reset()
    {
        m_uiMojoPuddleTimer = 2000;
        m_uiMojoWaveTimer   = 4000;
    }
 
    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_AT_COLOSSUS_COORDS)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
            m_creature->DealDamage(m_creature, m_creature->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void EnterEvadeMode()
    {
        ScriptedAI::EnterEvadeMode();
        if (m_pInstance && m_pInstance->IsValidLivingMojo(m_creature->GetObjectGuid()))
        {
            if (Creature* pColossus = m_pInstance->GetSingleCreatureFromStorage(NPC_COLOSSUS))
            {
                float fColoX, fColoY, fColoZ;
                pColossus->GetPosition(fColoX, fColoY, fColoZ);
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MovementExpired();
                m_creature->GetMotionMaster()->MovePoint(POINT_AT_COLOSSUS_COORDS, fColoX, fColoY, fColoZ);
            }
            else // this should not happen!
                MovementInform(POINT_MOTION_TYPE, POINT_AT_COLOSSUS_COORDS);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiMojoPuddleTimer < uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_MOJO_PUDDLE : SPELL_MOJO_PUDDLE_H);
            m_uiMojoPuddleTimer = urand(4000, 8000);
        }
        else m_uiMojoPuddleTimer -= uiDiff;

        if (m_uiMojoWaveTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MOJO_WAVE : SPELL_MOJO_WAVE_H);
            m_uiMojoWaveTimer = urand(4000, 7000);
        }
        else m_uiMojoWaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_living_mojo(Creature* pCreature)
{
    return new npc_living_mojoAI(pCreature);
};

void AddSC_boss_colossus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_colossus";
    pNewScript->GetAI = &GetAI_boss_colossus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_drakari_elemental";
    pNewScript->GetAI = &GetAI_boss_drakari_elemental;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_living_mojo";
    pNewScript->GetAI = &GetAI_npc_living_mojo;
    pNewScript->RegisterSelf();
}
