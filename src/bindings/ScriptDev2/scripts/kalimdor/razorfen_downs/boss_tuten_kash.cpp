/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 MangosR2_ScriptDev2
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
SDName: 
SD%Complete:
SDComment: Gong event && boss w/minions Support
SDCategory:
EndScriptData */

#include "precompiled.h"

enum
{
///-> TUTEN'KASH
    SPELL_CURSE_OF_TUTEN   = 12255,
    SPELL_VIRULENT_POISON  = 12251,
    SPELL_WEB_SPARY        = 12252,
    NPC_TUTEN              = 7355,
///-> END

///-> TOMB FIEND
    SPELL_POISON   = 744,
    NPC_TOMB_FIEND = 7349,
///-> END

///->  TOMB REAVER
    /// USES VIRULENT POISON
    SPELL_WEB       = 745,
    NPC_TOMB_REAVER = 7351,
///-> END

///-> STUFF
    GO_TUTEN_GONG   = 148917,
};

enum phase
{
    PHASE_ZERO  = 0,  ///-> GOgong has been used Once
    PHASE_ZEROB = 1,
    PHASE_ONE   = 2,  ///-> GOgong has been used twice
    PHASE_ONEB  = 3,
    PHASE_TWO   = 4,  ///-> GOgong has been used thrice and final time
    PHASE_TWOB  = 5,
};

///-> phase count holder
uint8 m_uiGongPhase = PHASE_ZERO;

///-> Death count holder
uint8 m_uiFiendDead;
uint8 m_uiReaverDead;

/*****
* TUTEN'KASH
*****/

struct MANGOS_DLL_DECL boss_tuten_kashAI : public ScriptedAI
{
    boss_tuten_kashAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCurse_Of_TutenTimer;
    uint32 m_uiVirulent_Poison_Timer;
    uint32 m_uiWebSpray_Timer;

    void Reset()
    {
        m_uiCurse_Of_TutenTimer     = 10000;
        m_uiVirulent_Poison_Timer   = 7500;
        m_uiWebSpray_Timer          = 8500;
    }

    void JustDied(Unit* pKiller)
    {
        m_uiGongPhase = PHASE_TWOB;
    }

    void JustReachedHome()
    {
        m_creature->ForcedDespawn();
        m_uiGongPhase = PHASE_ZERO;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCurse_Of_TutenTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CURSE_OF_TUTEN);
            m_uiCurse_Of_TutenTimer = 10000;
        }
        else
            m_uiCurse_Of_TutenTimer -= uiDiff;

        if (m_uiVirulent_Poison_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_VIRULENT_POISON);
            m_uiVirulent_Poison_Timer = 8000;
        }
        else
            m_uiVirulent_Poison_Timer -= uiDiff;

        if (m_uiWebSpray_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WEB_SPARY);
            m_uiWebSpray_Timer = 9000;
        }
        else
            m_uiWebSpray_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_tuten_kash(Creature* pCreature)
{
    return new boss_tuten_kashAI(pCreature);
}

/*****
* TOMB FIEND
*****/

struct MANGOS_DLL_DECL npc_tomb_fiendAI : public ScriptedAI
{
    npc_tomb_fiendAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        m_uiGongPhase = PHASE_ZEROB;
///-> using this only til when ever find out where exact spawn is
        m_creature->SetInCombatWithZone();
    }

    uint32 m_uiPoison_Timer;

    void Reset()
    {
        m_uiPoison_Timer   = 5500;
    }

    void JustDied(Unit* pKiller)
    {
        if (++m_uiFiendDead == 4)
        {
            m_uiGongPhase = PHASE_ONE;
        }
    }

    void JustReachedHome()
    {
        m_creature->ForcedDespawn();
        m_uiGongPhase = PHASE_ZERO;
    }

    void Aggro(Unit* pWho)
    {
        std::list<Creature*> lFiends;
        GetCreatureListWithEntryInGrid(lFiends, m_creature, NPC_TOMB_FIEND, 90.0f);
        if (!lFiends.empty())
        {
            for (std::list<Creature*>::iterator iter = lFiends.begin(); iter != lFiends.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                   (*iter)->AI()->AttackStart(pWho);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiPoison_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_POISON);
            m_uiPoison_Timer = 6000;
        }
        else
            m_uiPoison_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tomb_fiend(Creature* pCreature)
{
    return new npc_tomb_fiendAI(pCreature);
}

/*****
* TOMB REAVER
*****/

struct MANGOS_DLL_DECL npc_tomb_reaverAI : public ScriptedAI
{
    npc_tomb_reaverAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        Reset();
        m_uiGongPhase = PHASE_TWOB;
///-> using this only til when ever find out where exact spawn is
        m_creature->SetInCombatWithZone();
    }

    uint32 m_uiVirulent_Poison_Timer;
    uint32 m_uiWeb_Timer;

    void Reset()
    {
        m_uiVirulent_Poison_Timer   = 6500;
        m_uiWeb_Timer               = 7500;
    }

    void Aggro(Unit* pWho)
    {
        std::list<Creature*> lReavers;
        GetCreatureListWithEntryInGrid(lReavers, m_creature, NPC_TOMB_REAVER, 90.0f);
        if (!lReavers.empty())
        {
            for (std::list<Creature*>::iterator iter = lReavers.begin(); iter != lReavers.end(); ++iter)
            {
                if ((*iter) && (*iter)->isAlive())
                   (*iter)->AI()->AttackStart(pWho);
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        if (++m_uiReaverDead == 2)
        {
            m_uiGongPhase = PHASE_TWO;
        }
    }

    void JustReachedHome()
    {
        m_creature->ForcedDespawn();
        m_uiGongPhase = PHASE_ZERO;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiVirulent_Poison_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_VIRULENT_POISON);
            m_uiVirulent_Poison_Timer = 7000;
        }
        else
            m_uiVirulent_Poison_Timer -= uiDiff;

        if (m_uiWeb_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WEB);
            m_uiWeb_Timer = 6000;
        }
        else
            m_uiWeb_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_tomb_reaver(Creature* pCreature)
{
    return new npc_tomb_reaverAI(pCreature);
}

/*****
* GOgong
*****/


///->>>>  !!!!!  LOC AREN"T CORRECT need correct spawn how to !!!!!!!!!!!!!
///-> Fiends spawn
float PositionF1[4] = {2489.91f, 804.795f, 43.251f, 1.658f};
float PositionF2[4] = {2488.91f, 804.795f, 43.251f, 1.658f};
float PositionF3[4] = {2487.91f, 804.795f, 43.251f, 1.658f};
float PositionF4[4] = {2489.91f, 805.795f, 43.251f, 1.658f};
///->  reavers spawn
float PositionR1[4] = {2486.83f, 802.874f,43.198f, 2.91f};
float PositionR2[4] = {2489.91f, 804.795f, 43.251f, 1.658f};
///-> TuT spawn
float PositionTUT[4] = {2485.29f, 816.253f, 43.24f, 1.78f};

bool GOUse_go_gong_razor(Player* pPlayer, GameObject* pGo)
{
    switch(m_uiGongPhase)
    {
        case PHASE_ZERO: ///-> First time clicking
            pGo->SummonCreature(NPC_TOMB_FIEND, PositionF1[0], PositionF1[1], PositionF1[2], PositionF1[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10*MINUTE*IN_MILLISECONDS);
            pGo->SummonCreature(NPC_TOMB_FIEND, PositionF2[0], PositionF2[1], PositionF2[2], PositionF2[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10*MINUTE*IN_MILLISECONDS);
            pGo->SummonCreature(NPC_TOMB_FIEND, PositionF3[0], PositionF3[1], PositionF3[2], PositionF3[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10*MINUTE*IN_MILLISECONDS);
            pGo->SummonCreature(NPC_TOMB_FIEND, PositionF4[0], PositionF4[1], PositionF4[2], PositionF4[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10*MINUTE*IN_MILLISECONDS);
            break;
///-> empty case to keep players from multi summon the wave they are on 8P damn cheaters
        case PHASE_ZEROB:
            break;
        case PHASE_ONE:
            pGo->SummonCreature(NPC_TOMB_REAVER, PositionR1[0], PositionR1[1], PositionR1[2], PositionR1[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10*MINUTE*IN_MILLISECONDS);
            pGo->SummonCreature(NPC_TOMB_REAVER, PositionR2[0], PositionR2[1], PositionR2[2], PositionR2[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10*MINUTE*IN_MILLISECONDS);
            break;
        case PHASE_ONEB:
            break;
        case PHASE_TWO:
            pGo->SummonCreature(NPC_TUTEN, PositionTUT[0], PositionTUT[1], PositionTUT[2], PositionTUT[3], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10*MINUTE*IN_MILLISECONDS);
            break;
        case PHASE_TWOB:
            break;
    }
    return true;
}

void AddSC_boss_tuten_kash()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_tuten_kash";
    pNewScript->GetAI = &GetAI_boss_tuten_kash;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tomb_fiend";
    pNewScript->GetAI = &GetAI_npc_tomb_fiend;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tomb_reaver";
    pNewScript->GetAI = &GetAI_npc_tomb_reaver;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_gong_razor";
    pNewScript->pGOUse = &GOUse_go_gong_razor;
    pNewScript->RegisterSelf();
}

