/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * Copyright (C) 2011 MangosR2
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
SDName: instance_culling_of_stratholme
SD%Complete: %
SDComment:
EndScriptData */

#include "precompiled.h"
#include "culling_of_stratholme.h"

/*###
## npc_cs_gnoul
###*/

enum
{
  SPELL_FLESH        = 52352
};

struct MANGOS_DLL_DECL npc_cs_gnoulAI : public ScriptedAI
{
   npc_cs_gnoulAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
       m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
       m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
       m_creature->SetActiveObjectState(true);
       Reset();
   }

   ScriptedInstance* m_pInstance;
   bool m_bIsHeroic;

   uint32 m_uiFleshTimer;

   uint32 WaypointId;
   uint32 MoveTimer;

   void Reset()
   {
     m_creature->SetWalk(false);
     MoveTimer = (urand(100, 5000));
     m_uiFleshTimer = (urand(3000, 10000));
     WaypointId = 1;
   }

   void MoveToPoint(float X, float Y, float Z)
   {
       m_creature->GetMotionMaster()->MovementExpired(false);
       m_creature->GetMotionMaster()->MovePoint(0, X, Y, Z);
   }

   void AttackStart(Unit* pWho)
   {
      if (!pWho)
          return;

      if (m_creature->Attack(pWho, true))
      {
        m_creature->AddThreat(pWho);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);

        if (IsCombatMovement())
            m_creature->GetMotionMaster()->MoveChase(pWho);
      }
   }

   void EnterEvadeMode()
   {
      m_creature->RemoveAllAuras();
      m_creature->DeleteThreatList();
      m_creature->CombatStop(true);
      m_creature->LoadCreatureAddon(false);
      m_uiFleshTimer = (urand(3000, 10000));

      m_creature->SetLootRecipient(NULL);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!m_creature->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_creature->CanFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(pWho);
            if (m_creature->IsWithinDistInMap(pWho, attackRadius) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    AttackStart(pWho);
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho, 0.0f);
                }
            }
        }
    }

   void JumpNextStep(uint32 Time)
   {
     MoveTimer = Time;
     WaypointId++;
   }

   void UpdateAI(const uint32 uiDiff)
   {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() && m_creature->IsTemporarySummon())
        {
          if(MoveTimer < uiDiff)
          {   
             if(m_pInstance->GetData(TYPE_WING) == RIGHT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2356.659f, 1185.501f, 130.636f);
                     JumpNextStep(10000);
                     break;
                  case 2:
                     MoveToPoint(2301.735f, 1179.265f, 136.944f);
                     JumpNextStep(8000);
                     break;
                  case 3:
                     MoveToPoint(2234.787f, 1180.638f, 136.344f);
                     JumpNextStep(9000);
                     break;
                  case 4:
                     MoveToPoint(2178.313f, 1244.350f, 136.107f);
                     JumpNextStep(12000);
                     break; 
                  case 5:
                     MoveToPoint(2163.553f, 1277.814f, 133.444f);
                     JumpNextStep(5000);
                     break; 
                  case 6:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(5000);
                     break; 
                }
             }

             if(m_pInstance->GetData(TYPE_WING) == LEFT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2188.318f, 1331.410f, 130.003f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2165.351f, 1279.156f, 133.388f);    
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);    
                     JumpNextStep(9000);
                     break; 
                }
             }

           } else MoveTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiFleshTimer < uiDiff)
        {
           DoCast(m_creature->getVictim(), SPELL_FLESH);
           m_uiFleshTimer = (urand(3000, 10000));
        }
        else m_uiFleshTimer -= uiDiff;

        DoMeleeAttackIfReady();

        return;
   }
};

/*###
## npc_cs_necromancer
###*/

enum
{
  SPELL_SHADOW_BOLT        = 15472,
  SPELL_COURSE             = 20812,
  SPELL_DRAIN_MANA         = 58770
};

struct MANGOS_DLL_DECL npc_cs_necromancerAI : public ScriptedAI
{
   npc_cs_necromancerAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
       m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
       m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
       m_creature->SetActiveObjectState(true);
       SetCombatMovement(false);
       Reset();
   }

   ScriptedInstance* m_pInstance;
   bool m_bIsHeroic;

   uint32 m_uiShadowBoltTimer;
   uint32 m_uiCourseTimer;

   uint32 WaypointId;
   uint32 MoveTimer;

   void Reset() 
   { 
     m_creature->SetWalk(false);
     MoveTimer = (urand(100, 5000));
     m_uiCourseTimer = (urand(7000, 17000));
     m_uiShadowBoltTimer = (urand(3000, 10000));
     WaypointId = 1;
   }

   void Aggro(Unit* pWho)
   {
     m_creature->GetMotionMaster()->MovementExpired(false);
     m_creature->StopMoving();
   }

   void MoveToPoint(float X, float Y, float Z)
   {
       m_creature->GetMotionMaster()->MovementExpired(false);
       m_creature->GetMotionMaster()->MovePoint(0, X, Y, Z);
   }

   void AttackStart(Unit* pWho)
   {
      if (!pWho)
          return;

      if (m_creature->Attack(pWho, true))
      {
        m_creature->AddThreat(pWho);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);
      }
      m_creature->GetMotionMaster()->MovementExpired(false);
   }

   void EnterEvadeMode()
   {
      m_creature->RemoveAllAuras();
      m_creature->DeleteThreatList();
      m_creature->CombatStop(true);
      m_creature->LoadCreatureAddon(false);
      m_uiShadowBoltTimer = (urand(3000, 10000));
      m_uiCourseTimer = (urand(7000, 17000));

      m_creature->SetLootRecipient(NULL);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!m_creature->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_creature->CanFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(pWho);
            if (m_creature->IsWithinDistInMap(pWho, attackRadius) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    AttackStart(pWho);
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho, 0.0f);
                }
            }
        }
    }

   void JumpNextStep(uint32 Time)
   {
     MoveTimer = Time;
     WaypointId++;
   }

   void UpdateAI(const uint32 uiDiff)
   {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() && m_creature->IsTemporarySummon())
        {
          if(MoveTimer < uiDiff)
          {
             if(m_pInstance->GetData(TYPE_WING) == RIGHT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2356.659f, 1185.501f, 130.636f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2301.735f, 1179.265f, 136.944f);
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2234.787f, 1180.638f, 136.344f);
                     JumpNextStep(9000);
                     break; 
                  case 4:
                     MoveToPoint(2178.313f, 1244.350f, 136.107f);
                     JumpNextStep(12000);
                     break; 
                  case 5:
                     MoveToPoint(2163.553f, 1277.814f, 133.444f);
                     JumpNextStep(5000);
                     break; 
                  case 6:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(5000);
                     break; 
                }
             }

             if(m_pInstance->GetData(TYPE_WING) == LEFT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2188.318f, 1331.410f, 130.003f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2165.351f, 1279.156f, 133.388f);    
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);    
                     JumpNextStep(9000);
                     break; 
                }
             }

           } else MoveTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiShadowBoltTimer < uiDiff)
        {
           DoCast(m_creature->getVictim(), SPELL_SHADOW_BOLT);
           m_uiShadowBoltTimer  = (urand(3000, 5000));
        }
        else m_uiShadowBoltTimer -= uiDiff;

        if(m_uiCourseTimer < uiDiff)
        {
           m_creature->InterruptNonMeleeSpells(false);
           if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))   
              DoCast(target, SPELL_COURSE);
           m_uiCourseTimer  = (urand(7000, 17000));
        }
        else m_uiCourseTimer -= uiDiff;

        DoMeleeAttackIfReady();

        return;
   }
};

/*###
## npc_cs_field
###*/

enum
{
  SPELL_BLOW              = 52491,
  SPELL_SCARAB            = 52496
};

struct MANGOS_DLL_DECL npc_cs_fieldAI : public ScriptedAI
{
   npc_cs_fieldAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
       m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
       m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
       m_creature->SetActiveObjectState(true);
       Reset();
   }

   ScriptedInstance* m_pInstance;
   bool m_bIsHeroic;

   uint32 m_uiScarabTimer;
   uint32 m_uiBlowTimer;

   uint32 WaypointId;
   uint32 MoveTimer;

   void Reset() 
   { 
     m_creature->SetWalk(false);
     MoveTimer = (urand(100, 5000));
     m_uiBlowTimer = (urand(7000, 17000));
     m_uiScarabTimer = (urand(3000, 10000));
     WaypointId = 1;
   }

   void MoveToPoint(float X, float Y, float Z)
   {
       m_creature->GetMotionMaster()->MovementExpired(false);
       m_creature->GetMotionMaster()->MovePoint(0, X, Y, Z);
   }

   void AttackStart(Unit* pWho)
   {
      if (!pWho)
          return;

      if (m_creature->Attack(pWho, true))
      {
        m_creature->AddThreat(pWho);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);

        if (IsCombatMovement())
            m_creature->GetMotionMaster()->MoveChase(pWho);
      }
   }

   void EnterEvadeMode()
   {
      m_creature->RemoveAllAuras();
      m_creature->DeleteThreatList();
      m_creature->CombatStop(true);
      m_creature->LoadCreatureAddon(false);
      m_uiScarabTimer = (urand(3000, 10000));
      m_uiBlowTimer = (urand(7000, 17000));

      m_creature->SetLootRecipient(NULL);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!m_creature->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_creature->CanFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(pWho);
            if (m_creature->IsWithinDistInMap(pWho, attackRadius) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    AttackStart(pWho);
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho, 0.0f);
                }
            }
        }
    }

   void JumpNextStep(uint32 Time)
   {
     MoveTimer = Time;
     WaypointId++;
   }

   void UpdateAI(const uint32 uiDiff)
   {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() && m_creature->IsTemporarySummon())
        {
          if(MoveTimer < uiDiff)
          {   
             if(m_pInstance->GetData(TYPE_WING) == RIGHT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2356.659f, 1185.501f, 130.636f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2301.735f, 1179.265f, 136.944f);
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2234.787f, 1180.638f, 136.344f);
                     JumpNextStep(9000);
                     break; 
                  case 4:
                     MoveToPoint(2178.313f, 1244.350f, 136.107f);
                     JumpNextStep(12000);
                     break; 
                  case 5:
                     MoveToPoint(2163.553f, 1277.814f, 133.444f);
                     JumpNextStep(5000);
                     break; 
                  case 6:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(5000);
                     break; 
                }
             }

             if(m_pInstance->GetData(TYPE_WING) == LEFT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2188.318f, 1331.410f, 130.003f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2165.351f, 1279.156f, 133.388f);
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(9000);
                     break; 
                }
             }

           } else MoveTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiScarabTimer < uiDiff)
        {
           if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))   
              DoCast(target, SPELL_SCARAB);
           m_uiScarabTimer  = (urand(3000, 5000));
        }
        else m_uiScarabTimer -= uiDiff;

        if(m_uiBlowTimer < uiDiff)
        {
           m_creature->InterruptNonMeleeSpells(false);
           if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))   
              DoCast(target, SPELL_BLOW);
           m_uiBlowTimer  = (urand(7000, 17000));
        }
        else m_uiBlowTimer -= uiDiff;

        DoMeleeAttackIfReady();

        return;
   }
};

/*###
## npc_cs_alocyte
###*/

enum
{
  SPELL_SHADOW             = 17234,
  SPELL_COLD               = 15244,
  SPELL_FIRE               = 14145,
  SPELL_COURSEA            = 39621
};

struct MANGOS_DLL_DECL npc_cs_acolyteAI : public ScriptedAI
{
   npc_cs_acolyteAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
       m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
       m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
       m_creature->SetActiveObjectState(true);
       SetCombatMovement(false);
       Reset();
   }

   ScriptedInstance* m_pInstance;
   bool m_bIsHeroic;

   uint32 m_uiColdTimer;
   uint32 m_uiFireTimer;
   uint32 m_uiCourseTimer;
   uint32 m_uiShadowTimer;

   uint32 WaypointId;
   uint32 MoveTimer;

   void Reset() 
   { 
     m_creature->SetWalk(false);
     MoveTimer = (urand(100, 5000));
     m_uiColdTimer = (urand(7000, 17000));
     m_uiFireTimer = (urand(3000, 10000));
     m_uiCourseTimer = (urand(5000, 12000));
     m_uiShadowTimer = (urand(1000, 3000));
     WaypointId = 1;
   }

   void MoveToPoint(float X, float Y, float Z)
   {
       m_creature->GetMotionMaster()->MovementExpired(false);
       m_creature->GetMotionMaster()->MovePoint(0, X, Y, Z);
   }

   void Aggro(Unit* pWho)
   {
     m_creature->GetMotionMaster()->MovementExpired(false);
     m_creature->StopMoving();
   }

   void AttackStart(Unit* pWho)
   {
      if (!pWho)
          return;

      if (m_creature->Attack(pWho, true))
      {
        m_creature->AddThreat(pWho);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);
      }
      m_creature->GetMotionMaster()->MovementExpired(false);
   }

   void EnterEvadeMode()
   {
      m_creature->RemoveAllAuras();
      m_creature->DeleteThreatList();
      m_creature->CombatStop(true);
      m_creature->LoadCreatureAddon(false);
      m_uiColdTimer = (urand(7000, 17000));
      m_uiFireTimer = (urand(3000, 10000));
      m_uiCourseTimer = (urand(5000, 12000));
      m_uiShadowTimer = (urand(1000, 3000));

      m_creature->SetLootRecipient(NULL);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!m_creature->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_creature->CanFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(pWho);
            if (m_creature->IsWithinDistInMap(pWho, attackRadius) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    AttackStart(pWho);
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho, 0.0f);
                }
            }
        }
    }

   void JumpNextStep(uint32 Time)
   {
     MoveTimer = Time;
     WaypointId++;
   }

   void UpdateAI(const uint32 uiDiff)
   {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() && m_creature->IsTemporarySummon())
        {
          if(MoveTimer < uiDiff)
          {
             if(m_pInstance->GetData(TYPE_WING) == RIGHT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2356.659f, 1185.501f, 130.636f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2301.735f, 1179.265f, 136.944f);
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2234.787f, 1180.638f, 136.344f);
                     JumpNextStep(9000);
                     break; 
                  case 4:
                     MoveToPoint(2178.313f, 1244.350f, 136.107f);
                     JumpNextStep(12000);
                     break; 
                  case 5:
                     MoveToPoint(2163.553f, 1277.814f, 133.444f);
                     JumpNextStep(5000);
                     break; 
                  case 6:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(5000);
                     break; 
                }
             }

             if(m_pInstance->GetData(TYPE_WING) == LEFT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2188.318f, 1331.410f, 130.003f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2165.351f, 1279.156f, 133.388f);
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(9000);
                     break; 
                }
             }

           } else MoveTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiShadowTimer < uiDiff)
        {
           DoCast(m_creature->getVictim(), SPELL_SHADOW);
           m_uiShadowTimer  = (urand(3000, 8000));
        }
        else m_uiShadowTimer -= uiDiff;

        if(m_uiCourseTimer < uiDiff)
        {
           if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
              DoCast(target, SPELL_COURSEA);
           m_uiCourseTimer  = (urand(7000, 13000));
        }
        else m_uiCourseTimer -= uiDiff;

        if(m_uiColdTimer < uiDiff)
        {
           if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
              DoCast(target, SPELL_COLD);
           m_uiColdTimer  = (urand(13000, 17000));
        }
        else m_uiColdTimer -= uiDiff;

        if(m_uiFireTimer < uiDiff)
        {
           if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
              DoCast(target, SPELL_FIRE);
           m_uiFireTimer  = (urand(6000, 11000));
        }
        else m_uiFireTimer -= uiDiff;

        DoMeleeAttackIfReady();

        return;
   }
};

/*###
## npc_cs_butcher
###*/

enum
{
   SPELL_CLOUD       = 52525
};

struct MANGOS_DLL_DECL npc_cs_butcherAI : public ScriptedAI
{
   npc_cs_butcherAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
       m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
       m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
       m_creature->SetActiveObjectState(true);
       Reset();
   }

   ScriptedInstance* m_pInstance;
   bool m_bIsHeroic;

   uint32 WaypointId;
   uint32 MoveTimer;

   void Reset() 
   { 
     DoCast(m_creature, SPELL_CLOUD);
     m_creature->SetWalk(false);
     MoveTimer = (urand(100, 5000));
     WaypointId = 1;
   }

   void MoveToPoint(float X, float Y, float Z)
   {
       m_creature->GetMotionMaster()->MovementExpired(false);
       m_creature->GetMotionMaster()->MovePoint(0, X, Y, Z);
   }

   void AttackStart(Unit* pWho)
   {
      if (!pWho)
          return;

      if (m_creature->Attack(pWho, true))
      {
        m_creature->AddThreat(pWho);
        m_creature->SetInCombatWith(pWho);
        pWho->SetInCombatWith(m_creature);

        if (IsCombatMovement())
            m_creature->GetMotionMaster()->MoveChase(pWho);
      }
   }

   void EnterEvadeMode()
   {
      m_creature->RemoveAllAuras();
      m_creature->DeleteThreatList();
      m_creature->CombatStop(true);
      m_creature->LoadCreatureAddon(false);

      m_creature->SetLootRecipient(NULL);
      DoCast(m_creature, SPELL_CLOUD);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (!m_creature->hasUnitState(UNIT_STAT_STUNNED) && pWho->isTargetableForAttack() &&
            m_creature->IsHostileTo(pWho) && pWho->isInAccessablePlaceFor(m_creature))
        {
            if (!m_creature->CanFly() && m_creature->GetDistanceZ(pWho) > CREATURE_Z_ATTACK_RANGE)
                return;

            float attackRadius = m_creature->GetAttackDistance(pWho);
            if (m_creature->IsWithinDistInMap(pWho, attackRadius) && m_creature->IsWithinLOSInMap(pWho))
            {
                if (!m_creature->getVictim())
                {
                    AttackStart(pWho);
                    pWho->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                }
                else if (m_creature->GetMap()->IsDungeon())
                {
                    pWho->SetInCombatWith(m_creature);
                    m_creature->AddThreat(pWho, 0.0f);
                }
            }
        }
    }

   void JumpNextStep(uint32 Time)
   {
     MoveTimer = Time;
     WaypointId++;
   }

   void UpdateAI(const uint32 uiDiff)
   {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() && m_creature->IsTemporarySummon())
        {
          if(MoveTimer < uiDiff)
          {
             if(m_pInstance->GetData(TYPE_WING) == RIGHT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2356.659f, 1185.501f, 130.636f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2301.735f, 1179.265f, 136.944f);
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2234.787f, 1180.638f, 136.344f);
                     JumpNextStep(9000);
                     break; 
                  case 4:
                     MoveToPoint(2178.313f, 1244.350f, 136.107f);
                     JumpNextStep(12000);
                     break; 
                  case 5:
                     MoveToPoint(2163.553f, 1277.814f, 133.444f);
                     JumpNextStep(5000);
                     break; 
                  case 6:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(5000);
                     break; 
                }
             }

             if(m_pInstance->GetData(TYPE_WING) == LEFT)
             {
                switch(WaypointId)
                {
                  case 1:
                     MoveToPoint(2188.318f, 1331.410f, 130.003f);
                     JumpNextStep(10000);
                     break; 
                  case 2:
                     MoveToPoint(2165.351f, 1279.156f, 133.388f);
                     JumpNextStep(8000);
                     break;  
                  case 3:
                     MoveToPoint(2083.952f, 1287.716f, 141.146f);
                     JumpNextStep(9000);
                     break; 
                }
             }

           } else MoveTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();

        return;
   }
};

struct MANGOS_DLL_DECL npc_time_riftCSAI : public ScriptedAI
{
    npc_time_riftCSAI(Creature *pCreature) : ScriptedAI(pCreature) 
   {
       m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
       m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
       m_creature->SetActiveObjectState(true);
       Reset();
   }

   ScriptedInstance* m_pInstance;
   bool m_bIsHeroic;

   uint32 Step;
   uint32 m_uiStepTimer;
   Creature* Drakonian01;
   Creature* Drakonian02;
   Creature* Drakonian03;

   void Reset() 
   {
      m_uiStepTimer = 1000;
      Step = 1;
   }

   void UpdateAI(const uint32 uiDiff)
   {
       if (m_uiStepTimer < uiDiff)
       {
         switch(Step)
         {
           case 1:
              if (Creature* pArthas = m_pInstance->GetSingleCreatureFromStorage(NPC_ARTHAS))
              {
                 Drakonian01 = m_creature->SummonCreature(NPC_INFINITE_ADVERSARY,(m_creature->GetPositionX()-2)+rand()%4, (m_creature->GetPositionY()-2)+rand()%4, m_creature->GetPositionZ(),3.229f,TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN,900000);
                 Drakonian01->GetMotionMaster()->MovePoint(0, pArthas->GetPositionX(), pArthas->GetPositionY(), pArthas->GetPositionZ());
                 Drakonian02 = m_creature->SummonCreature(NPC_INFINITE_HUNTER,(m_creature->GetPositionX()-2)+rand()%4, (m_creature->GetPositionY()-2)+rand()%4, m_creature->GetPositionZ(),3.229f,TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN,900000);
                 Drakonian02->GetMotionMaster()->MovePoint(0, pArthas->GetPositionX(), pArthas->GetPositionY(), pArthas->GetPositionZ());
                 Drakonian03 = m_creature->SummonCreature(NPC_INFINITE_AGENT,(m_creature->GetPositionX()-2)+rand()%4, (m_creature->GetPositionY()-2)+rand()%4, m_creature->GetPositionZ(),3.229f,TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN,900000);
                 Drakonian03->GetMotionMaster()->MovePoint(0, pArthas->GetPositionX(), pArthas->GetPositionY(), pArthas->GetPositionZ());
              }
              m_uiStepTimer = 3000;
              Step++;
              break;
           case 2:
              m_creature->ForcedDespawn();
              Step++; 
              break;
         }
       } else m_uiStepTimer -= uiDiff;
   }
};

CreatureAI* GetAI_npc_cs_gnoul(Creature* pCreature)
{
    return new npc_cs_gnoulAI(pCreature);
}

CreatureAI* GetAI_npc_cs_necromancer(Creature* pCreature)
{
    return new npc_cs_necromancerAI(pCreature);
}

CreatureAI* GetAI_npc_cs_field(Creature* pCreature)
{
    return new npc_cs_fieldAI(pCreature);
}

CreatureAI* GetAI_npc_cs_acolyte(Creature* pCreature)
{
    return new npc_cs_acolyteAI(pCreature);
}

CreatureAI* GetAI_npc_cs_butcher(Creature* pCreature)
{
    return new npc_cs_butcherAI(pCreature);
}

CreatureAI* GetAI_npc_time_riftCS(Creature* pCreature)
{
    return new npc_time_riftCSAI(pCreature);
}

void AddSC_trash_culling_of_stratholme()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_cs_gnoul";
    pNewScript->GetAI = &GetAI_npc_cs_gnoul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_cs_necromancer";
    pNewScript->GetAI = &GetAI_npc_cs_necromancer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_cs_field";
    pNewScript->GetAI = &GetAI_npc_cs_field;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_cs_acolyte";
    pNewScript->GetAI = &GetAI_npc_cs_acolyte;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_cs_butcher";
    pNewScript->GetAI = &GetAI_npc_cs_butcher;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_time_riftCS";
    pNewScript->GetAI = &GetAI_npc_time_riftCS;
    pNewScript->RegisterSelf();
}