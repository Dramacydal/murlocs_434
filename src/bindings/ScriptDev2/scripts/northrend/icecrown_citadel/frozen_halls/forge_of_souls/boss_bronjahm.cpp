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
SDName: boss_bronjahm
SD%Complete: 90%
SDComment: Small unknown behaviour for his Shadow Bold use in phase 1; Soulstorm needs additional handling in core
SDCategory: The Forge of Souls
EndScriptData */

#include "precompiled.h"
#include "forge_of_souls.h"

enum BossSpells
{
        //common
        SPELL_BERSERK                           = 47008,
        //yells
        //summons
        NPC_SOUL_FRAGMENT                       = 36535,
        //Abilities
        SPELL_MAGIC_BANE                        = 68793,
        SPELL_CORRUPT_SOUL                      = 68839,
        SPELL_CONSUME_SOUL                      = 68858,
        SPELL_TELEPORT                          = 68988,
        SPELL_SOULSTORM                         = 68872,
        SPELL_SOULSTORM_2                       = 68921,
        SPELL_FEAR                              = 68950,
        SPELL_SHADOW_BOLT                       = 70043,

   /*Music*/
   Battle01                              = 6077,
   Battle02                              = 6078,
   Battle03                              = 6079,

};

struct MANGOS_DLL_DECL boss_bronjahmAI : public BSWScriptedAI
{
    boss_bronjahmAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    uint8 stage;
    uint32 BattleMusicTimer;
    uint32 Music;

    void Reset()
    {
        if(pInstance) pInstance->SetData(TYPE_BRONJAHM, NOT_STARTED);
        resetTimers();
        stage = 0;
    }

    void Aggro(Unit *who)
    {
       Music = (urand(0, 2));
       switch(Music)
       {
         case 0:
            m_creature->PlayDirectSound(Battle01);
            BattleMusicTimer = 48000;
            break;
         case 1:
            m_creature->PlayDirectSound(Battle02);
            BattleMusicTimer = 27000;
            break;
         case 2:
            m_creature->PlayDirectSound(Battle03);
            BattleMusicTimer = 36000;
            break;
        }

        if(pInstance) pInstance->SetData(TYPE_BRONJAHM, IN_PROGRESS);
            DoScriptText(-1632001,m_creature,who);
            SetCombatMovement(true);
    }

    void JustDied(Unit *killer)
    {
        if(pInstance) pInstance->SetData(TYPE_BRONJAHM, DONE);
        doRemove(SPELL_SOULSTORM);
               DoScriptText(-1632004,m_creature,killer);
    }

    void KilledUnit(Unit* pVictim)
    {
    switch (urand(0,1)) {
        case 0:
               DoScriptText(-1632002,m_creature,pVictim);
               break;
        case 1:
               DoScriptText(-1632003,m_creature,pVictim);
               break;
        };
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        switch(stage)
        {
            case 0:
                    if  (timedQuery(SPELL_CORRUPT_SOUL, diff))
                        {
                            if (Unit* pTarget= m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                {
                                if (doCast(SPELL_CORRUPT_SOUL, pTarget) == CAST_OK)
                                    {
                                    DoScriptText(-1632006,m_creature,pTarget);
                                    float fPosX, fPosY, fPosZ;
                                    pTarget->GetPosition(fPosX, fPosY, fPosZ);
                                    doSummon(NPC_SOUL_FRAGMENT,fPosX, fPosY, fPosZ);
                                    }
                                }
                        }
                    break;
            case 1:
                        if (timedCast(SPELL_TELEPORT, diff) == CAST_OK) stage = 2;
                    break;
            case 2:
                        if (timedCast(SPELL_SOULSTORM, diff) == CAST_OK) {
                               DoScriptText(-1632005,m_creature);
                               SetCombatMovement(false);
                               stage = 3;
                               }
                    break;
            case 3:
                        timedCast(SPELL_FEAR, diff);
                    break;
        }

        timedCast(SPELL_SHADOW_BOLT, diff);

        timedCast(SPELL_MAGIC_BANE, diff);

        if (m_creature->GetHealthPercent() <= 30.0f && stage == 0) stage = 1;

        DoMeleeAttackIfReady();

        if (BattleMusicTimer < diff && m_creature->isAlive())
        {
           switch(Music)
           {
             case 0:
                m_creature->PlayDirectSound(Battle01);
                BattleMusicTimer = 49000;
                break;
             case 1:
                m_creature->PlayDirectSound(Battle02);
                BattleMusicTimer = 28000;
                break;
             case 2:
                m_creature->PlayDirectSound(Battle03);
                BattleMusicTimer = 37000;
                break;
            }
        } else BattleMusicTimer -= diff;
    }
};

struct MANGOS_DLL_DECL mob_soul_fragmentAI : public ScriptedAI
{
    mob_soul_fragmentAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint32 m_uiRangeCheck_Timer;

    void Reset()
    {
        m_uiRangeCheck_Timer = 1000;
        if (!m_pInstance) return;
        Creature* pBoss = m_pInstance->GetSingleCreatureFromStorage(NPC_BRONJAHM);
        m_creature->SetSpeedRate(MOVE_RUN, 0.2f);
        if (Creature* pBoss = m_pInstance->GetSingleCreatureFromStorage(NPC_BRONJAHM))
            m_creature->GetMotionMaster()->MoveChase(pBoss);
        m_creature->SetRespawnDelay(DAY);
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_BRONJAHM) != IN_PROGRESS ) m_creature->ForcedDespawn();

        if (m_uiRangeCheck_Timer < uiDiff)
        {
            if (Creature* pBoss = m_pInstance->GetSingleCreatureFromStorage(NPC_BRONJAHM))
            {
                if (pBoss->IsWithinDistInMap(m_creature, 2.0f))
                {
                    pBoss->CastSpell(pBoss, SPELL_CONSUME_SOUL, false);
                    m_creature->ForcedDespawn();
                } else m_creature->GetMotionMaster()->MoveChase(pBoss);
            }
            else
                m_creature->ForcedDespawn();

            m_uiRangeCheck_Timer = 1000;
        }
        else m_uiRangeCheck_Timer -= uiDiff;
    }

};

CreatureAI* GetAI_boss_bronjahm(Creature* pCreature)
{
    return new boss_bronjahmAI(pCreature);
}

CreatureAI* GetAI_mob_soul_fragment(Creature* pCreature)
{
    return new mob_soul_fragmentAI (pCreature);
}


void AddSC_boss_bronjahm()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_bronjahm";
    newscript->GetAI = &GetAI_boss_bronjahm;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_soul_fragment";
    newscript->GetAI = &GetAI_mob_soul_fragment;
    newscript->RegisterSelf();

}
