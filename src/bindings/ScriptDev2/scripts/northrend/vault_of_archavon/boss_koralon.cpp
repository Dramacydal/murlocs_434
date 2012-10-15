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
SDName: boss_koralon
SD%Complete: 50%
SDComment: 
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

#define SP_BURNING_FURY_AURA    66895
#define SP_BURNING_FURY_AURA2   68168
#define SP_BURNING_FURY_EFFECT  66721

#define SP_BURNING_BREATH       66665
#define H_SP_BURNING_BREATH     67328 //DBM
#define SP_BB_EFFECT            66670
#define H_SP_BB_EFFECT          67329

#define SP_METEOR_FISTS         66725 //DBM
#define H_SP_METEOR_FISTS       68161       
#define SP_METEOR_FISTS_EFF     66765
#define H_SP_METEOR_FISTS_EFF   67333

#define SP_CINDER       66684
#define H_SP_CINDER     67332

struct MANGOS_DLL_DECL boss_koralonAI : public ScriptedAI
{
    boss_koralonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Regular = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool Regular;
    uint32 BurningBreathTimer;
    uint32 MeteorFistsTimer;
    uint32 FlamesTimer;

    uint32 BBTickTimer;
    uint32 BBTicks;
    bool BB;

    void Reset()
    {
        BurningBreathTimer = 25000;
        MeteorFistsTimer = 47000;
        FlamesTimer = 15000;

        BB = false;

        if(pInstance) pInstance->SetData(TYPE_KORALON, NOT_STARTED);
    }

    void Aggro(Unit *who)
    {
        DoCastSpellIfCan(m_creature, SP_BURNING_FURY_AURA);

        if(pInstance) pInstance->SetData(TYPE_KORALON, IN_PROGRESS);
    };

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_KORALON, DONE);
    };

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(BurningBreathTimer < diff)
        {
            DoCastSpellIfCan(m_creature, Regular ? SP_BURNING_BREATH : H_SP_BURNING_BREATH);
            BurningBreathTimer = 45000;

            BB = true;
            BBTickTimer = 1000;
            BBTicks = 0;
        }
        else BurningBreathTimer -= diff;


        if(FlamesTimer < diff)
        {
            int flames = Regular ? 3 : 5;
            int i;
            for(i=0; i< flames; ++i)
            {
                Unit *target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);
                if(target) DoCastSpellIfCan(target, Regular ? SP_CINDER : H_SP_CINDER);
            }
            FlamesTimer = 20000;
        }
        else FlamesTimer -= diff;

        if(MeteorFistsTimer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SP_METEOR_FISTS_EFF);
            MeteorFistsTimer = 45000;
        }
        else MeteorFistsTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_koralonAI(Creature* pCreature)
{
    return new boss_koralonAI(pCreature);
}

void AddSC_boss_koralon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_koralon";
    newscript->GetAI = &GetAI_boss_koralonAI;
    newscript->RegisterSelf();
}
