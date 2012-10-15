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
SDName: boss_falric
SD%Complete: 70%
SDComment:
SDAuthor: /dev/rsa, changed by MaxXx2021 aka Mioka
SDCategory: Halls of Reflection
EndScriptData */

#include "precompiled.h"
#include "halls_of_reflection.h"

enum
{
    SAY_FALRIC_AGGRO                        = -1668507,
    SAY_FALRIC_DEATH                        = -1668508,
    SAY_FALRIC_SLAY01                       = -1668509,
    SAY_FALRIC_SLAY02                       = -1668510,
    SAY_FALRIC_SP01                         = -1668511,
    SAY_FALRIC_SP02                         = -1668512,

    SPELL_HOPELESSNESS                      = 72395,
    SPELL_IMPENDING_DESPAIR                 = 72426,
    SPELL_DEFILING_HORROR                   = 72435,
    //SPELL_DEFILING_HORROR_H                 = 72452,
    SPELL_QUIVERING_STRIKE                  = 72422,
    //SPELL_QUIVERING_STRIKE_H                = 72453,

    SPELL_BERSERK                           = 47008
};

struct MANGOS_DLL_DECL boss_falricAI : public BSWScriptedAI
{
    boss_falricAI(Creature *pCreature) : BSWScriptedAI(pCreature)
   {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
   }

   ScriptedInstance* m_pInstance;
   bool m_bIsCall;

   uint32 m_uiSummonTimer;
   uint32 m_uiLocNo;
   ObjectGuid m_uiSummonGUID[16];
   uint32 m_uiCheckSummon;

   uint8 SummonCount;

   uint32 pSummon;

    void Reset()
    {
      SummonCount = 0;
      m_bIsCall = false;
      m_uiSummonTimer = 11000;
      m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
      m_creature->SetVisibility(VISIBILITY_OFF);
    }

    void Aggro(Unit* pVictim)
    {
      m_creature->SetWalk(false);
      DoScriptText(SAY_FALRIC_AGGRO, m_creature);
      DoCast(m_creature, SPELL_HOPELESSNESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0,1))
        {
            case 0: DoScriptText(SAY_FALRIC_SLAY01, m_creature); break;
            case 1: DoScriptText(SAY_FALRIC_SLAY02, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
      if(!m_pInstance) return;
      m_pInstance->SetData(TYPE_MARWYN, SPECIAL);
      DoScriptText(SAY_FALRIC_DEATH, m_creature);
    }

    void AttackStart(Unit* who)
    {
        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_FALRIC) != IN_PROGRESS)
             return;

        ScriptedAI::AttackStart(who);
    }

    void Summon()
    {
         m_uiLocNo = 0;

         for(uint8 i = 0; i < 14; i++)
         {
            switch(urand(0,3))
            {
               case 0:
                   switch(urand(1, 3))
                   {
                     case 1: pSummon = NPC_DARK_1; break;
                     case 2: pSummon = NPC_DARK_3; break;
                     case 3: pSummon = NPC_DARK_6; break;
                   }
                   break;
               case 1:
                   switch(urand(1, 3))
                   {
                     case 1: pSummon = NPC_DARK_2; break;
                     case 2: pSummon = NPC_DARK_3; break;
                     case 3: pSummon = NPC_DARK_4; break;
                   }
                   break;
               case 2:
                   switch(urand(1, 3))
                   {
                     case 1: pSummon = NPC_DARK_2; break;
                     case 2: pSummon = NPC_DARK_5; break;
                     case 3: pSummon = NPC_DARK_6; break;
                   }
                   break;
               case 3:
                   switch(urand(1, 3))
                   {
                     case 1: pSummon = NPC_DARK_1; break;
                     case 2: pSummon = NPC_DARK_5; break;
                     case 3: pSummon = NPC_DARK_4; break;
                   }
                   break;
             }

             m_uiCheckSummon = 0;

             if(Creature* Summon = m_creature->SummonCreature(pSummon, SpawnLoc[m_uiLocNo].x, SpawnLoc[m_uiLocNo].y, SpawnLoc[m_uiLocNo].z, SpawnLoc[m_uiLocNo].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 30000))
             {
                m_uiSummonGUID[i] = Summon->GetObjectGuid();
                Summon->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                Summon->setFaction(974);
             }
             m_uiLocNo++;
         }
    }

    void CallFallSoldier()
    {
         for(uint8 i = 0; i < 4; i++)
         {
            if(Creature* Summon = m_pInstance->instance->GetCreature(m_uiSummonGUID[m_uiCheckSummon]))
            {
               Summon->setFaction(14);
               Summon->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
               Summon->SetInCombatWithZone();
            }
            m_uiCheckSummon++;
         }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_EVENT) == 5)
        {
            m_pInstance->SetData(TYPE_EVENT, 6);
            Summon();
        }

        if (m_pInstance->GetData(TYPE_FALRIC) == SPECIAL)
        {

            if (m_uiSummonTimer < uiDiff)
            {
                    ++SummonCount;
                    m_pInstance->SetData(DATA_WAVE_COUNT,SummonCount);
                    if(SummonCount > 4)
                    {
                        m_pInstance->SetData(TYPE_FALRIC, IN_PROGRESS);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->SetInCombatWithZone();
                    }
                    else CallFallSoldier();
                    m_uiSummonTimer = 60000;
            } else m_uiSummonTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        timedCast(SPELL_QUIVERING_STRIKE, uiDiff);
        timedCast(SPELL_IMPENDING_DESPAIR, uiDiff);
        timedCast(SPELL_DEFILING_HORROR, uiDiff);

        timedCast(SPELL_BERSERK, uiDiff);

        DoMeleeAttackIfReady();

    }
};

CreatureAI* GetAI_boss_falric(Creature* pCreature)
{
    return new boss_falricAI(pCreature);
}

void AddSC_boss_falric()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_falric";
    newscript->GetAI = &GetAI_boss_falric;
    newscript->RegisterSelf();
}
