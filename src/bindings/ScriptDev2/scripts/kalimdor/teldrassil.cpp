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
SDName: Teldrassil
SD%Complete: 100
SDComment: Quest support: 938
SDCategory: Teldrassil
EndScriptData */

/* ContentData
npc_mist
Tyrande Whisperwind
Archdruid Fandral Staghelm
EndContentData */

#include "precompiled.h"
#include "follower_ai.h"

/*####
# npc_mist
####*/

enum
{
    SAY_AT_HOME             = -1000323,
    EMOTE_AT_HOME           = -1000324,
    QUEST_MIST              = 938,
    NPC_ARYNIA              = 3519,
    FACTION_DARNASSUS       = 79
};

struct MANGOS_DLL_DECL npc_mistAI : public FollowerAI
{
    npc_mistAI(Creature* pCreature) : FollowerAI(pCreature) { Reset(); }

    void Reset() { }

    void MoveInLineOfSight(Unit *pWho)
    {
        FollowerAI::MoveInLineOfSight(pWho);

        if (!m_creature->getVictim() && !HasFollowState(STATE_FOLLOW_COMPLETE) && pWho->GetEntry() == NPC_ARYNIA)
        {
            if (m_creature->IsWithinDistInMap(pWho, 10.0f))
            {
                DoScriptText(SAY_AT_HOME, pWho);
                DoComplete();
            }
        }
    }

    void DoComplete()
    {
        DoScriptText(EMOTE_AT_HOME, m_creature);

        if (Player* pPlayer = GetLeaderForFollower())
        {
            if (pPlayer->GetQuestStatus(QUEST_MIST) == QUEST_STATUS_INCOMPLETE)
                pPlayer->GroupEventHappens(QUEST_MIST, m_creature);
        }

        //The follow is over (and for later development, run off to the woods before really end)
        SetFollowComplete();
    }

    //call not needed here, no known abilities
    /*void UpdateFollowerAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }*/
};

CreatureAI* GetAI_npc_mist(Creature* pCreature)
{
    return new npc_mistAI(pCreature);
}

bool QuestAccept_npc_mist(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_MIST)
    {
        if (npc_mistAI* pMistAI = dynamic_cast<npc_mistAI*>(pCreature->AI()))
            pMistAI->StartFollow(pPlayer, FACTION_DARNASSUS, pQuest);
    }

    return true;
}

/*####
# Tyrande Whisperwind
####*/

enum
{
    SPELL_CLEAVE_TYRANDE            = 20691,
    SPELL_MOONFIRE                  = 20690,
    SPELL_SEARING_ARROW             = 20688,
    SPELL_STARFALL                  = 20687,
};

struct MANGOS_DLL_DECL boss_tyrande_whisperwindAI : public ScriptedAI
{
    boss_tyrande_whisperwindAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiCleaveTimer;
    uint32 m_uiMoonfireTimer;
    uint32 m_uiStarfallTimer;
    uint32 m_uiSearingArrowTimer;

    void Reset()
    {
        m_uiCleaveTimer     = 5000;
        m_uiMoonfireTimer   = 12000;
        m_uiStarfallTimer   = 15000;
        m_uiSearingArrowTimer   = 8000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiCleaveTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_CLEAVE_TYRANDE);
            m_uiCleaveTimer = urand(5000, 8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        if (m_uiMoonfireTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                DoCast(pTarget, SPELL_MOONFIRE);
            m_uiMoonfireTimer = urand(11000, 15000);
        }
        else
            m_uiMoonfireTimer -= uiDiff;

        if (m_uiStarfallTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                DoCast(pTarget, SPELL_STARFALL);
            m_uiStarfallTimer = urand(15000, 19000);
        }
        else
            m_uiStarfallTimer -= uiDiff;

        if (m_uiSearingArrowTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                DoCast(pTarget, SPELL_SEARING_ARROW);
            m_uiSearingArrowTimer = 8000;
        }
        else
            m_uiSearingArrowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_tyrande_whisperwind(Creature* pCreature)
{
    return new boss_tyrande_whisperwindAI(pCreature);
}

/*####
# Archdruid Fandral Staghelm
####*/

enum
{
    SPELL_ENTANGLING_ROOTS          = 20699,
   // SPELL_REJUVENATION              = 20701,
    SPELL_SUMMON_TREANTS            = 20702,
    SPELL_WRATH                     = 20698,
    NPC_TREANT                      = 31802,
};

struct MANGOS_DLL_DECL boss_fandral_staghelmAI : public ScriptedAI
{
    boss_fandral_staghelmAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiEntanglingTimer;
    uint32 m_uiWrathTimer;
    uint32 m_uiSummonAlyTimer;
    //uint32 m_uiRejuvinationTimer;

    void Reset()
    {
        m_uiEntanglingTimer     = 7000;
        m_uiWrathTimer          = 5000;
        m_uiSummonAlyTimer      = 15000;
        //m_uiRejuvinationTimer   = 10000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEntanglingTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                DoCast(pTarget, SPELL_ENTANGLING_ROOTS);
            m_uiEntanglingTimer = urand(7000, 11000);
        }
        else
            m_uiEntanglingTimer -= uiDiff;

        if (m_uiWrathTimer < uiDiff)
        {
            Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0);
                DoCast(m_creature, SPELL_WRATH);
            m_uiWrathTimer = urand(5000, 8000);
        }
        else
            m_uiWrathTimer -= uiDiff;

        if (m_uiSummonAlyTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_SUMMON_TREANTS);
            m_uiSummonAlyTimer = urand(25000, 30000);
        }
        else
            m_uiSummonAlyTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_fandral_staghelm(Creature* pCreature)
{
    return new boss_fandral_staghelmAI(pCreature);
}

void AddSC_teldrassil()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_mist";
    pNewScript->GetAI = &GetAI_npc_mist;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_mist;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_tyrande_whisperwind";
    pNewScript->GetAI = &GetAI_boss_tyrande_whisperwind;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_fandral_staghelm";
    pNewScript->GetAI = &GetAI_boss_fandral_staghelm;
    pNewScript->RegisterSelf();
}
