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
SDName: Boss_Sjonnir
SD%Complete: 20%
SDComment:
SDCategory: Halls of Stone
EndScriptData */

#include "precompiled.h"
#include "halls_of_stone.h"

enum
{
    SAY_AGGRO                   = -1599000,
    SAY_SLAY_1                  = -1599001,
    SAY_SLAY_2                  = -1599002,
    SAY_SLAY_3                  = -1599003,
    SAY_DEATH                   = -1599004,
    EMOTE_GENERIC_FRENZY        = -1000002,

    SPELL_CHAIN_LIGHTING        = 50830,
    SPELL_CHAIN_LIGHTING_H      = 59844,
    SPELL_FRENZY                = 28747,
    SPELL_LIGHTING_SHIELD       = 50831,
    SPELL_LIGHTING_SHIELD_H     = 59845,
    SPELL_STATIC_CHARGE         = 50834, //Periodic Trigger 2s interval, spell =50835
    SPELL_STATIC_CHARGE_H       = 59846, //Periodic Trigger 2s interval, spell =50847

    SPELL_LIGHTING_RING         = 51849,
    SPELL_LIGHTING_RING_H       = 59861,
    SPELL_LIGHTING_RING1        = 50840,
    SPELL_LIGHTING_RING1_H      = 59848,

    NPC_FORGED_IRON_TROGG       = 27979,
    NPC_MALFORMED_OOZE          = 27981,
    NPC_FORGED_IRON_DWARF       = 27982,
};

struct Locations
{
    float x, y, z;
    uint32 id;
};

static Locations PipeLoc[]=
{
    {1295.44f, 734.07f, 200.3f}, // left
    {1297.7f,  595.6f,  199.9f}, // right
};

/*######
## boss_sjonnir
######*/

struct MANGOS_DLL_DECL boss_sjonnirAI : public ScriptedAI
{
    boss_sjonnirAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsFrenzy;

    std::list<ObjectGuid> m_lDwarfGUIDList;
    uint32 m_uiChainLightning_Timer;
    uint32 m_uiLightningShield_Timer;
    uint32 m_uiStaticCharge_Timer;
    uint32 m_uiLightningRing_Timer;
    uint32 m_uiSummon_Timer;
    uint32 m_uiFrenzy_Timer;

    void Reset()
    {
        m_bIsFrenzy = false;

        m_uiChainLightning_Timer = 3000 + rand()%5000;
        m_uiLightningShield_Timer = 20000 + rand()%5000;
        m_uiStaticCharge_Timer = 20000 + rand()%5000;
        m_uiLightningRing_Timer = 30000 + rand()%5000;
        m_uiSummon_Timer = 5000;
        m_uiFrenzy_Timer = 300000;

        DespawnDwarf();

        if(m_pInstance)
            m_pInstance->SetData(TYPE_GRIEF, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if(m_pInstance)
            m_pInstance->SetData(TYPE_GRIEF, IN_PROGRESS);
//            pSummoned->SetWalk(false);
//            pSummoned->GetMotionMaster()->MovePoint(0, fX, fY, fZ);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(rand()%3)
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
            m_pInstance->SetData(TYPE_GRIEF, DONE);
    }

    void DespawnDwarf()
    {
        if (m_lDwarfGUIDList.empty())
            return;

        for(std::list<ObjectGuid>::iterator itr = m_lDwarfGUIDList.begin(); itr != m_lDwarfGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pTemp->isAlive())
                    pTemp->ForcedDespawn();
            }
        }

        m_lDwarfGUIDList.clear();
    }

    void JustSummoned(Creature* pSummoned)
    {
        m_lDwarfGUIDList.push_back(pSummoned->GetObjectGuid());

        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
        {
            pSummoned->AddThreat(pTarget, 0.0f);
            pSummoned->AI()->AttackStart(pTarget);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiChainLightning_Timer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTING_H : SPELL_CHAIN_LIGHTING);
            m_uiChainLightning_Timer = 10000 + rand()%5000;
        }
        else
            m_uiChainLightning_Timer -= uiDiff;

        if (m_uiLightningShield_Timer < uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTING_SHIELD_H : SPELL_LIGHTING_SHIELD);
            m_uiLightningShield_Timer = 20000 + rand()%5000;
        }
        else
            m_uiLightningShield_Timer -= uiDiff;

        if (m_uiStaticCharge_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_STATIC_CHARGE_H : SPELL_STATIC_CHARGE);
            m_uiStaticCharge_Timer = 20000 + rand()%5000;
        }
        else
            m_uiStaticCharge_Timer -= uiDiff;

        if (m_uiLightningRing_Timer < uiDiff)
        {
            if (m_creature->IsNonMeleeSpellCasted(false))
                m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_LIGHTING_RING_H : SPELL_LIGHTING_RING);
            m_uiLightningRing_Timer = 30000 + rand()%5000;
        }
        else
            m_uiLightningRing_Timer -= uiDiff;

        if (m_uiSummon_Timer < uiDiff)
        {
            uint32 SummonPipe = rand()%2;
            uint32 SummonEntry = 0;
            switch(rand()%3)
            {
                case 0: SummonEntry = NPC_FORGED_IRON_TROGG; break;
                case 1: SummonEntry = NPC_MALFORMED_OOZE;    break;
                case 2: SummonEntry = NPC_FORGED_IRON_DWARF; break;
            }
            m_creature->SummonCreature(SummonEntry, PipeLoc[SummonPipe].x, PipeLoc[SummonPipe].y, PipeLoc[SummonPipe].z, 0.0f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000);
            m_uiSummon_Timer = 20000;
        }
        else
            m_uiSummon_Timer -= uiDiff;

        if (!m_bIsFrenzy && m_uiFrenzy_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_FRENZY);
            m_bIsFrenzy = true;
            m_uiFrenzy_Timer = 0;
        }
        else
            m_uiFrenzy_Timer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sjonnir(Creature* pCreature)
{
    return new boss_sjonnirAI(pCreature);
}

void AddSC_boss_sjonnir()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sjonnir";
    newscript->GetAI = &GetAI_boss_sjonnir;
    newscript->RegisterSelf();
}
