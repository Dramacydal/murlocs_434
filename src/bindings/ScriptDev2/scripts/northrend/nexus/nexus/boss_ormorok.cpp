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
SDName: Boss_Ormorok
SD%Complete: 95%
SDComment: Playable Normal Heroic Support
SDCategory: Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

enum
{
    // Spells
    SPELL_CRYSTAL_SPIKES_N                     = 47958, //Don't work, using walkaround
    SPELL_CRYSTAL_SPIKES_H                     = 57082, //Don't work, using walkaround
    SPELL_CRYSTALL_SPIKE_DAMAGE_N              = 47944,
    SPELL_CRYSTALL_SPIKE_DAMAGE_H              = 57067,
    SPELL_CRYSTAL_SPIKE_PREVISUAL              = 50442,
    SPELL_SPELL_REFLECTION                     = 35399, //47981,
    SPELL_TRAMPLE_N                            = 48016,
    SPELL_TRAMPLE_H                            = 57066,
    SPELL_FRENZY_H                             = 48017,
    SPELL_FRENZY_N                             = 57086,
    SPELL_SUMMON_CRYSTALLINE_TANGLER           = 61564, //summons npc 32665
    SPELL_ROOTS                                = 28858, //proper spell id is unknown

    // NPC's
    MOB_CRYSTAL_SPIKE                          = 27099,
    MOB_CRYSTALLINE_TANGLER                    = 32665,

    // Texts
    SAY_AGGRO                                  = -1576020,
    SAY_DEATH                                  = -1576021,
    SAY_REFLECT                                = -1576022,
    SAY_CRYSTAL_SPIKES                         = -1576023,
    SAY_KILL                                   = -1576024,

    // Others
    SPIKE_DISTANCE                             = 5
};

struct MANGOS_DLL_DECL boss_ormorokAI : public ScriptedAI
{
    boss_ormorokAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsFrenzy;
    bool m_bIsCrystalSpikes;
    uint8  m_uiCrystalSpikesCount;
    uint32 m_uiSpellCrystalSpikesTimer;
    uint32 m_uiCrystalSpikesTimer;
    uint32 m_uiTrampleTimer;
    uint32 m_uiFrenzyTimer;
    uint32 m_uiReflectionTimer;
    uint32 m_uiSummonTanglerTimer;
    float m_fBaseX;
    float m_fBaseY;
    float m_fBaseZ;
    float m_fBaseO;
    float m_fSpikeXY[4][2];

    void Reset()
    {
        m_uiSpellCrystalSpikesTimer = 12*IN_MILLISECONDS;
        m_uiTrampleTimer = 10*IN_MILLISECONDS;
        m_uiReflectionTimer = 30*IN_MILLISECONDS;
        m_uiSummonTanglerTimer = 17*IN_MILLISECONDS;
        m_bIsFrenzy = false;
        m_bIsCrystalSpikes = false;

        if(m_pInstance)
            m_pInstance->SetData(TYPE_ORMOROK, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ORMOROK, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bIsCrystalSpikes)
        {
            if (m_uiCrystalSpikesTimer < uiDiff)
            {
                m_fSpikeXY[0][0] = m_fBaseX+(SPIKE_DISTANCE*m_uiCrystalSpikesCount*cos(m_fBaseO));
                m_fSpikeXY[0][1] = m_fBaseY+(SPIKE_DISTANCE*m_uiCrystalSpikesCount*sin(m_fBaseO));
                m_fSpikeXY[1][0] = m_fBaseX-(SPIKE_DISTANCE*m_uiCrystalSpikesCount*cos(m_fBaseO));
                m_fSpikeXY[1][1] = m_fBaseY-(SPIKE_DISTANCE*m_uiCrystalSpikesCount*sin(m_fBaseO));
                m_fSpikeXY[2][0] = m_fBaseX+(SPIKE_DISTANCE*m_uiCrystalSpikesCount*cos(m_fBaseO-(M_PI/2)));
                m_fSpikeXY[2][1] = m_fBaseY+(SPIKE_DISTANCE*m_uiCrystalSpikesCount*sin(m_fBaseO-(M_PI/2)));
                m_fSpikeXY[3][0] = m_fBaseX-(SPIKE_DISTANCE*m_uiCrystalSpikesCount*cos(m_fBaseO-(M_PI/2)));
                m_fSpikeXY[3][1] = m_fBaseY-(SPIKE_DISTANCE*m_uiCrystalSpikesCount*sin(m_fBaseO-(M_PI/2)));

                for (uint8 i = 0; i < 4; i++)
                    Creature* Spike = m_creature->SummonCreature(MOB_CRYSTAL_SPIKE, m_fSpikeXY[i][0], m_fSpikeXY[i][1], m_fBaseZ, 0, TEMPSUMMON_TIMED_DESPAWN, 7000);

                if (++m_uiCrystalSpikesCount >= 13)
                    m_bIsCrystalSpikes = false;

                m_uiCrystalSpikesTimer = 0.2*IN_MILLISECONDS;
            }
            else
                m_uiCrystalSpikesTimer -= uiDiff;
        }

        if (!m_bIsFrenzy && (m_creature->GetHealthPercent() < 25.0f))
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_FRENZY_N : SPELL_FRENZY_H);
            m_bIsFrenzy = true;
        }

        if (m_uiTrampleTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_TRAMPLE_N : SPELL_TRAMPLE_H);
            m_uiTrampleTimer = urand(10*IN_MILLISECONDS, 35*IN_MILLISECONDS);
        }
        else
            m_uiTrampleTimer -= uiDiff;

        if (m_uiReflectionTimer < uiDiff)
        {
            DoScriptText(SAY_REFLECT, m_creature);
            DoCast(m_creature, SPELL_SPELL_REFLECTION);
            m_uiReflectionTimer = 15*IN_MILLISECONDS;
        }
        else
            m_uiReflectionTimer -= uiDiff;

        if (m_uiSpellCrystalSpikesTimer < uiDiff)
        {
            DoScriptText(SAY_CRYSTAL_SPIKES, m_creature);
            m_bIsCrystalSpikes = true;
            m_uiCrystalSpikesCount = 1;
            m_uiCrystalSpikesTimer = 0;
            m_fBaseX = m_creature->GetPositionX();
            m_fBaseY = m_creature->GetPositionY();
            m_fBaseZ = m_creature->GetPositionZ();
            m_fBaseO = m_creature->GetOrientation();
            m_uiSpellCrystalSpikesTimer = 20*IN_MILLISECONDS;
        }
        else
            m_uiSpellCrystalSpikesTimer -= uiDiff;

        if (!m_bIsRegularMode && (m_uiSummonTanglerTimer < uiDiff))
        {
            Creature* CrystallineTangler = m_creature->SummonCreature(MOB_CRYSTALLINE_TANGLER, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
            if (CrystallineTangler)
                if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    CrystallineTangler->AI()->AttackStart(target);

            m_uiSummonTanglerTimer = 17*IN_MILLISECONDS;
        }
        else
            m_uiSummonTanglerTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ormorok(Creature* pCreature)
{
    return new boss_ormorokAI (pCreature);
}

struct MANGOS_DLL_DECL mob_crystal_spikeAI : public Scripted_NoMovementAI
{
    mob_crystal_spikeAI(Creature *pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    uint32 m_uiCrystallSpikeDamageTimer;
    uint32 m_uiCrystalSpikePreVisualTimer;

    void Reset()
    {
        m_uiCrystallSpikeDamageTimer = 3.7*IN_MILLISECONDS;
        m_uiCrystalSpikePreVisualTimer = 1*IN_MILLISECONDS;
        m_creature->SetLevel(80);                                        //
        m_creature->setFaction(16);                                      //Walkaround to be independent from data in DB
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); //
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE); //
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCrystalSpikePreVisualTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_CRYSTAL_SPIKE_PREVISUAL);
            m_uiCrystalSpikePreVisualTimer = 10*IN_MILLISECONDS;
        }
        else
            m_uiCrystalSpikePreVisualTimer -= uiDiff;

        if (m_uiCrystallSpikeDamageTimer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_CRYSTALL_SPIKE_DAMAGE_N : SPELL_CRYSTALL_SPIKE_DAMAGE_H);
            m_uiCrystallSpikeDamageTimer = 10*IN_MILLISECONDS;
        }
        else
            m_uiCrystallSpikeDamageTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_crystal_spike(Creature* pCreature)
{
    return new mob_crystal_spikeAI (pCreature);
}

struct MANGOS_DLL_DECL mob_crystalline_tanglerAI : public ScriptedAI
{
    mob_crystalline_tanglerAI(Creature *pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiRootsTimer;

    void Reset()
    {
        m_uiRootsTimer = 1*IN_MILLISECONDS;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiRootsTimer < uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_ROOTS, true);
            m_uiRootsTimer = 15*IN_MILLISECONDS;
        }
        else
            m_uiRootsTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_crystalline_tangler(Creature* pCreature)
{
    return new mob_crystalline_tanglerAI (pCreature);
}

void AddSC_boss_ormorok()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name= "boss_ormorok";
    pNewScript->GetAI = &GetAI_boss_ormorok;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name= "mob_crystal_spike";
    pNewScript->GetAI = &GetAI_mob_crystal_spike;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name= "mob_crystalline_tangler";
    pNewScript->GetAI = &GetAI_mob_crystalline_tangler;
    pNewScript->RegisterSelf();
}
