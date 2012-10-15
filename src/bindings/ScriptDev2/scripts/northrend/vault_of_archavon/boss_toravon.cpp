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
SDName: Toravon the Ice Watcher
SDAuthor: Lutik
SD%Complete: 0%
SDComment:
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

enum
{
    SP_WHITEOUT         = 72034,
    H_SP_WHITEOUT       = 72096,
    SP_FREEZING_GROUND  = 72090,
    H_SP_FREEZING_GROUND = 72104,
    SP_FROZEN_MALLET    = 71993,
    FROZEN_ORB_AURA     = 72081,

    CR_FROZEN_ORB       = 38456
};


struct MANGOS_DLL_DECL boss_toravonAI : public ScriptedAI
{
    boss_toravonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        orbsNum = m_bIsRegularMode ? 1 : 3;
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    bool m_bIsRegularMode;
    ScriptedInstance *pInstance;

    int orbsNum;
    uint32 WhiteoutTimer;
    uint32 OrbsTimer;
    uint32 FreezeTimer;

    void Reset()
    {
        WhiteoutTimer = 40000;
        OrbsTimer = 15000;
        FreezeTimer = 20000 + rand()%5000;

        if(pInstance)
            pInstance->SetData(TYPE_TORAVON, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoCastSpellIfCan(m_creature, SP_FROZEN_MALLET);

        if(pInstance)
            pInstance->SetData(TYPE_TORAVON, IN_PROGRESS);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance)
            pInstance->SetData(TYPE_TORAVON, DONE);
    }

    void JustSummoned(Creature *orb)
    {
        orb->CastSpell(orb, FROZEN_ORB_AURA, false);
        orb->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(WhiteoutTimer < diff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SP_WHITEOUT : H_SP_WHITEOUT);
            WhiteoutTimer = 40000;
        }
        else
            WhiteoutTimer -= diff;

        if(OrbsTimer < diff)
        {
            for(int i=0; i<orbsNum; ++i)
            {
                if(Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->SummonCreature(CR_FROZEN_ORB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 3000);
            }
            OrbsTimer = 40000;
        }
        else
            OrbsTimer -= diff;

        if(FreezeTimer < diff)
        {
            DoCastSpellIfCan(m_creature, SP_FREEZING_GROUND);
            FreezeTimer = 20000 + rand()%5000;
        }
        else
            FreezeTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_toravon(Creature *pCreature)
{
    return new boss_toravonAI (pCreature);
};

void AddSC_boss_toravon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_toravon";
    newscript->GetAI = &GetAI_boss_toravon;
    newscript->RegisterSelf();
};
