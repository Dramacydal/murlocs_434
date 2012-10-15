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
SDName: Boss_Tharonja
SD%Complete: 80%
SDComment: Timers
SDCategory: Drak'Tharon Keep
EndScriptData */

#include "precompiled.h"
#include "draktharon_keep.h"

enum
{
    SAY_KING_DRED_TALON        = -1600020,
    SAY_CALL_FOR_RAPTOR        = -1600021,
    
    SPELL_BELLOWING_ROAR       = 22686,
    SPELL_FEARSOME_ROAR        = 48849,
    SPELL_FEARSOME_ROAR_H      = 59422,
    SPELL_GRIEVOUS_BITE        = 48920,
    SPELL_MANGLING_SLASH       = 48873,
    SPELL_PIERCING_SLASH       = 48878,
    SPELL_RAPTOR_CALL          = 59416,            //not yet implemented

};

const float PosSummon1[3] = {-528.8f, -690.58f, 30.25f};
/*######
## boss_dred
######*/

struct MANGOS_DLL_DECL boss_dredAI : public ScriptedAI
{
    boss_dredAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_draktharon_keep*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_draktharon_keep* m_pInstance;
    bool m_bIsRegularMode;
    
    uint32 m_uiFearsomeRoar;
    uint32 m_uiManglingSlash;
    uint32 m_uiPiercingSlash;
    uint32 m_uiGrievousBite;
    uint32 m_uiBellowingRoar;
    uint32 m_uiCallForRaptor;
    uint32 m_uiCallForRaptorSpawn;

    bool m_bCallForRaptorSpawn;
    
    void Reset()
    {
        m_uiFearsomeRoar       = 15000;
        m_uiManglingSlash      = urand(5000, 10000);
        m_uiPiercingSlash      = urand(10000, 15000);
        m_uiGrievousBite       = urand(15000, 20000);
        m_uiBellowingRoar      = 60000;
        m_uiCallForRaptor      = 25000;
        m_bCallForRaptorSpawn  = false;
    }
    
    void CallForRaptorSpawnCheck()
    {
        m_uiCallForRaptorSpawn = 1000;
        m_uiCallForRaptorSpawn = true;
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Fearsome Roar
        if (m_uiFearsomeRoar < uiDiff)
        {
            if (DoCastSpellIfCan (m_creature, m_bIsRegularMode ? SPELL_FEARSOME_ROAR : SPELL_FEARSOME_ROAR_H) == CAST_OK)
                m_uiFearsomeRoar = 15000;
        }
        else
            m_uiFearsomeRoar -= uiDiff;

        //Piercing Slash
        if (m_uiPiercingSlash < uiDiff)
        {    
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_PIERCING_SLASH) == CAST_OK)
                m_uiPiercingSlash = urand(20000, 25000);
        }
        else
            m_uiPiercingSlash -= uiDiff;

        //Mangling Slash
        if (m_uiManglingSlash < uiDiff)
        {    
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_MANGLING_SLASH) == CAST_OK)
                m_uiManglingSlash = urand(20000, 25000);
        }
        else
            m_uiManglingSlash -= uiDiff;

        //Grievous Bite
        if (m_uiGrievousBite < uiDiff)
        {    
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_GRIEVOUS_BITE) == CAST_OK)
                m_uiGrievousBite = urand(20000, 25000);
        }
        else
            m_uiGrievousBite -= uiDiff;

        //Bellowing Roar
        if (m_uiBellowingRoar < uiDiff)
        {    
            if (DoCastSpellIfCan(m_creature, SPELL_BELLOWING_ROAR) == CAST_OK)
                m_uiBellowingRoar = 60000;
        }
        else
            m_uiBellowingRoar -= uiDiff;

        //Call For Raptor - spell
        if (m_uiCallForRaptor < uiDiff)
        {    
            if (DoCastSpellIfCan(m_creature, SPELL_RAPTOR_CALL) == CAST_OK)
            {
                DoScriptText(SAY_CALL_FOR_RAPTOR, m_creature);
                m_uiCallForRaptor = 25000;
                CallForRaptorSpawnCheck();
            }
        }
        else
            m_uiCallForRaptor -= uiDiff;

        //Call For Raptor - spawn
        if (m_uiCallForRaptorSpawn < uiDiff && m_bCallForRaptorSpawn)
        {    
            switch(urand(0, 1))
            {
                case 0:
                {
                    if (Creature* pRaptor1 = m_creature->SummonCreature(NPC_DRAKKARI_GUTRIPPER, PosSummon1[0], PosSummon1[1], PosSummon1[2], 0 , TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240000))
                        pRaptor1->SetInCombatWithZone();
                }
                case 1:
                {
                    if (Creature* pRaptor2 = m_creature->SummonCreature(NPC_DRAKKARI_SCYTHECLAW, PosSummon1[0], PosSummon1[1], PosSummon1[2], 0 , TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 240000))
                        pRaptor2->SetInCombatWithZone();
                }
            }
            m_bCallForRaptorSpawn = false;
        }
        else
            m_uiCallForRaptorSpawn -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_dred(Creature* pCreature)
{
    return new boss_dredAI(pCreature);
}

void AddSC_boss_dred()
{
    Script *pNewscript;

    pNewscript = new Script;
    pNewscript->Name = "boss_dred";
    pNewscript->GetAI = &GetAI_boss_dred;
    pNewscript->RegisterSelf();
}
