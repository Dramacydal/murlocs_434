/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
SDName: Boss_Taldaram
SD%Complete: 80%
SDAuthor: Tassadar
SDComment: Timers, maybe wrong orb behavior
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"

enum
{
    SPELL_BEAM_VISUAL               = 60342,      // Used when taldram levitates before encounter
    SPELL_CONJURE_FLAME_ORB         = 55931,      // Dummy spell, dont do anything except cast
    SPELL_BLOODTHIRST               = 55968,
    SPELL_VANISH                    = 55964,      // Does not work...?
    SPELL_EMBRACE_OF_THE_VAMPYR     = 55959,
    SPELL_EMBRACE_OF_THE_VAMPYR_H   = 59513,

    SPELL_FLAME_ORB_SPAWN_EFFECT    = 55891, // Orb Grow up
    SPELL_FLAME_ORB_VISUAL          = 55928, // Flame orb effect
    SPELL_FLAME_ORB_DEATH           = 55947, // Despawn effect
    SPELL_FLAME_ORB                 = 57750, // Flame orb damage    
    SPELL_FLAME_ORB_H               = 58937,

    NPC_FLAME_ORB                   = 30702,

    SAY_AGGRO                       = -1619008,
    SAY_VANISH_1                    = -1619009,
    SAY_VANISH_2                    = -1619010,
    SAY_FEED_1                      = -1619011,
    SAY_FEED_2                      = -1619012,
    SAY_SLAY_1                      = -1619013,
    SAY_SLAY_2                      = -1619014,
    SAY_SLAY_3                      = -1619015,
    SAY_DEATH                       = -1619016,

    FLAME_ORB_Z                     = 17,

    FLAME_ORB_UP_X                  = 383,
    FLAME_ORB_UP_Y                  = -984,

    FLAME_ORB_DOWN_X                = 632,
    FLAME_ORB_DOWN_Y                = -684,

    FLAME_ORB_RIGHT_X               = 350,
    FLAME_ORB_RIGHT_Y               = -705,

    FLAME_ORB_LEFT_X                = 613,
    FLAME_ORB_LEFT_Y                = -966,
};

/*######
## boss_taldaram
######*/

struct MANGOS_DLL_DECL boss_taldaramAI : public ScriptedAI
{
    boss_taldaramAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiVanishPhase;
    uint32 m_uiDamageTaken;
    Unit* m_uEmbraceTarget;

    uint32 m_uiBloodthirst_Timer;
    uint32 m_uiSummonOrb_Timer;
    uint32 m_uiVanish_Timer;
    uint32 m_uiVanishPhase_Timer;
    uint32 m_uiEmbrace_Timer;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        m_uiBloodthirst_Timer = 4000;
        m_uiSummonOrb_Timer = 13000;
        m_uiVanish_Timer = 17000;
        m_uiVanishPhase_Timer = 0;
        m_uiEmbrace_Timer = 0;
        m_uiVanishPhase = 0;
        m_uiDamageTaken = 0;
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TALDARAM, NOT_STARTED);

        m_creature->SetVisibility(VISIBILITY_ON);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->RemoveAurasDueToSpell(SPELL_BEAM_VISUAL);
        m_creature->SetLevitate(false);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_TALDARAM, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TALDARAM, DONE);

        m_creature->SetVisibility(VISIBILITY_ON);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if(m_creature->IsNonMeleeSpellCasted(false))
        {
            m_uiDamageTaken += uiDamage;
            uint32 m_uiMinDamage = m_bIsRegularMode ? 20000 : 40000;
            if(m_uiDamageTaken >= m_uiMinDamage)
            {
                m_uiVanishPhase = 0; 
                m_creature->InterruptNonMeleeSpells(false);
            }
        }
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!m_pInstance) return;

        if(m_uiVanishPhase != 0)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            if(m_uiVanishPhase_Timer <= uiDiff)
            {
                m_creature->InterruptNonMeleeSpells(false);
                m_uiVanishPhase = 0;
            }else m_uiVanishPhase_Timer -= uiDiff;

            if(m_uiVanishPhase != 1)
                return;

            // Embrace of the Vampyr
            if(m_uiEmbrace_Timer <= uiDiff)
            {
                switch(urand(0, 1))
                {
                    case 0: DoScriptText(SAY_FEED_1, m_creature); break;
                    case 1: DoScriptText(SAY_FEED_2, m_creature); break;
                }
                m_creature->SetVisibility(VISIBILITY_ON);
                if(m_uEmbraceTarget && m_uEmbraceTarget->isAlive())
                    DoCast(m_uEmbraceTarget, m_bIsRegularMode ? SPELL_EMBRACE_OF_THE_VAMPYR : SPELL_EMBRACE_OF_THE_VAMPYR_H);
                m_uiDamageTaken = 0;
                m_uiVanishPhase = 2;
            }else m_uiEmbrace_Timer -= uiDiff;    
            return;
        }

        // Bloodthirst
        if(m_uiBloodthirst_Timer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_BLOODTHIRST);
            m_uiBloodthirst_Timer = 8000 + rand()%6000;
        }else m_uiBloodthirst_Timer -= uiDiff;

        // Summon Flame Orb
        if(m_uiSummonOrb_Timer <= uiDiff)
        {
            for(int i = 0; i <= 3; ++i)
            {
                m_creature->SummonCreature(NPC_FLAME_ORB, m_creature->GetPositionX(), m_creature->GetPositionY(), FLAME_ORB_Z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                if(m_bIsRegularMode)
                    break;
            }
            DoCast(m_creature, SPELL_CONJURE_FLAME_ORB);
            m_uiSummonOrb_Timer = 16000 + rand()%10000;
            m_uiVanish_Timer = 16000;
        }else m_uiSummonOrb_Timer -= uiDiff;

        // Vanish
        if(m_uiVanish_Timer <= uiDiff)
        {
            switch(urand(0, 1))
            {
                case 0: DoScriptText(SAY_VANISH_1, m_creature); break;
                case 1: DoScriptText(SAY_VANISH_2, m_creature); break;
            }

            //DoCast(m_creature, SPELL_VANISH); We dont want to drop aggro
            m_uiVanishPhase = 1;
            bool stop = false;
            while(!stop)
            {
                m_uEmbraceTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if(m_uEmbraceTarget && m_uEmbraceTarget->isAlive() && m_uEmbraceTarget->GetTypeId() == TYPEID_PLAYER)
                    stop = true;
                else
                    continue;

                m_creature->GetMotionMaster()->MoveChase(m_uEmbraceTarget);
                break;
            }

            m_creature->SetVisibility(VISIBILITY_OFF);
            m_uiVanish_Timer = 10000 + rand()%10000;
            m_uiEmbrace_Timer = 3500;
            m_uiVanishPhase_Timer = 22500;
            return;
        }else m_uiVanish_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_taldaram(Creature* pCreature)
{
    return new boss_taldaramAI(pCreature);
}

/*######
## mob_flame_orb
######*/

struct MANGOS_DLL_DECL mob_flame_orbAI : public ScriptedAI
{
    mob_flame_orbAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsFlying;
    int8 direction;

    uint32 m_uiDespawn_Timer;
    uint32 m_uiCast_Timer;

    void Reset()
    {
        m_uiDespawn_Timer = 13000;
        m_uiCast_Timer = 3000;
        direction = -1;
        m_bIsFlying = false;
        DoCast(m_creature, SPELL_FLAME_ORB_VISUAL);
        DoCast(m_creature, SPELL_FLAME_ORB_SPAWN_EFFECT);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }
    void AttackStart(Unit* pWho)
    {
        return;
    }
    void UpdateAI(const uint32 uiDiff)
    {
        // Despawn Timer
        if(m_uiDespawn_Timer <= uiDiff)
        {
            DoCast(m_creature, SPELL_FLAME_ORB_DEATH);
            m_creature->ForcedDespawn();
        }else m_uiDespawn_Timer -= uiDiff;

        // Fly timer
        if(m_uiCast_Timer <= uiDiff)
        {
            if(m_bIsFlying)
                return;

            DoCast(m_creature, m_bIsRegularMode ? SPELL_FLAME_ORB : SPELL_FLAME_ORB_H);
            direction = rand()%4;
            switch(direction)
            {
                case 0: // Up
                    m_creature->GetMotionMaster()->MovePoint(0, FLAME_ORB_UP_X, FLAME_ORB_UP_Y, FLAME_ORB_Z);
                    break;
                case 1: // Down
                    m_creature->GetMotionMaster()->MovePoint(0, FLAME_ORB_DOWN_X, FLAME_ORB_DOWN_Y, FLAME_ORB_Z);
                    break;
                case 2: // Right
                    m_creature->GetMotionMaster()->MovePoint(0, FLAME_ORB_RIGHT_X, FLAME_ORB_RIGHT_Y, FLAME_ORB_Z);
                    break;
                case 3: // Left
                    m_creature->GetMotionMaster()->MovePoint(0, FLAME_ORB_LEFT_X, FLAME_ORB_LEFT_Y, FLAME_ORB_Z);
                    break;
                default:
                    m_creature->GetMotionMaster()->MovePoint(0, FLAME_ORB_UP_X, FLAME_ORB_UP_Y, FLAME_ORB_Z);
                    break;

            }
            m_bIsFlying = true;
            m_creature->SetLevitate(true);
        }else m_uiCast_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_flame_orb(Creature* pCreature)
{
    return new mob_flame_orbAI(pCreature);
}

/*######
## go_nerubian_device
######*/

bool GOUse_go_nerubian_device(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    pInstance->SetData(TYPE_TALDARAM, SPECIAL);
    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    return false;
}

void AddSC_boss_taldaram()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_taldaram";
    pNewScript->GetAI = &GetAI_boss_taldaram;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_nerubian_device";
    pNewScript->pGOUse = &GOUse_go_nerubian_device;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_flame_orb";
    pNewScript->GetAI = &GetAI_mob_flame_orb;
    pNewScript->RegisterSelf();
}
