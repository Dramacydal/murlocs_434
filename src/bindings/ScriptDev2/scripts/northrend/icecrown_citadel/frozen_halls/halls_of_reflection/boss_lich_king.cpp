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
SDName: boss_lich_king
SD%Complete: 100%
SDComment:
SDAuthor: MaxXx2021 aka Mioka
SDCategory: Halls of Reflection
EndScriptData */

#include "precompiled.h"
#include "halls_of_reflection.h"
#include "escort_ai.h"

enum
{
  SPELL_WINTER                       = 69780,
  SPELL_FURY_OF_FROSTMOURNE          = 70063,
  SPELL_SOUL_REAPER                  = 73797,
  SPELL_ICE_PRISON                   = 69708,
  SPELL_DARK_ARROW                   = 70194,
  SPELL_EMERGE_VISUAL                = 50142,
  SPELL_DESTROY_ICE_WALL_02          = 70224,
  SPELL_SILENCE                      = 69413,
  SPELL_LICH_KING_CAST               = 57561,
  SPELL_GNOUL_JUMP                   = 70150,
  SPELL_ABON_STRIKE                  = 40505,

  // summon spells
  SPELL_WITCH_DOCTOR                 = 69836,
  SPELL_SUMMON_ABOM                  = 69835,
  SPELL_RAISE_DEAD                   = 69818,

  /*SPELLS - Witch Doctor, 36941 */
  SPELL_COURSE_OF_DOOM               = 70144,
  SPELL_SHADOW_BOLT_VALLEY           = 70145,
  SPELL_SHADOW_BOLT                  = 70080,

  /*SPELLS - abomination, 37069 */
  SPELL_CLEAVE                       = 40505,

  /*SPELLS - raging ghoul, 36940 */
  SPELL_LEAP                         = 70150,

  SAY_LICH_KING_WALL_01              = -1594486,
  SAY_LICH_KING_WALL_02              = -1594491,
  SAY_LICH_KING_GNOUL                = -1594482,
  SAY_LICH_KING_ABON                 = -1594483,
  SAY_LICH_KING_WINTER               = -1594481,
  SAY_LICH_KING_END_DUN              = -1594504,
  SAY_LICH_KING_WIN                  = -1594485,

  /*INTRO - Lich King Arrive*/
  SAY_LICH_KING_17                   = -1668468,
  SAY_LICH_KING_18                   = -1668469,
  SAY_LICH_KING_19                   = -1668470,
  SAY_JAINA_20                       = -1668471,
  SAY_SYLVANA_20                     = -1668472,
  SAY_LICH_KING_A_21                 = -1668473,
  SAY_LICH_KING_H_21                 = -1668474,
  SAY_FALRIC_INTRO                   = -1668475,
  SAY_MARWYN_INTRO                   = -1668476,
  SAY_FALRIC_INTRO2                  = -1668505,

  SPELL_TAKE_FROSTMOURNE             = 72729,
  SPELL_FROSTMOURNE_DESPAWN          = 72726,
  SPELL_FROSTMOURNE_SOUNDS           = 70667,
  SPELL_CAST_VISUAL                  = 65633,  //Jaina And Sylavana cast this when summon uther.
  SPELL_BOSS_SPAWN_AURA              = 72712,  //Falric and Marwyn
  SPELL_UTHER_DESPAWN                = 70693,
  SPELL_FROSTMOURNE_VISUAL           = 73220,

  /*INTRO - Pre Escape*/
  SAY_LICH_KING_AGGRO_A              = -1668477,
  SAY_LICH_KING_AGGRO_H              = -1668478,
  SAY_JAINA_AGGRO                    = -1668479,
  SAY_SYLVANA_AGGRO                  = -1668480,

};

struct MANGOS_DLL_DECL boss_lich_king_hrAI : public npc_escortAI
{
    boss_lich_king_hrAI(Creature *pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (BSWScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    BSWScriptedInstance* m_pInstance;
    bool StartEscort;
    bool NonFight;
    bool Finish;

    void Reset()
    {
        if(!m_pInstance) return;
        NonFight    = false;
        StartEscort = false;
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, 36942);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_pInstance->GetData(TYPE_LICH_KING) == DONE)
            m_creature->SetVisibility(VISIBILITY_OFF);
    }

    void JustDied(Unit* pKiller)
    {
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (!who || !m_pInstance)
            return;

        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        Player* pPlayer = (Player *)who;

        if (pPlayer->isGameMaster())
            return;

        if (m_pInstance->GetData(TYPE_PHASE) != 3
           || m_pInstance->GetData(TYPE_LICH_KING) == DONE
           || !m_creature->IsWithinDistInMap(who, 50.0f)
           || m_pInstance->GetData(TYPE_FROST_GENERAL) != DONE)
           return;

        m_pInstance->DoOpenDoor(GO_ICECROWN_DOOR_2);

        Team team;
        if (Group* pGroup = pPlayer->GetGroup())
        {
            ObjectGuid LeaderGuid = pGroup->GetLeaderGuid();
            if (!LeaderGuid.IsEmpty())
                if (Player* pLeader =m_creature->GetMap()->GetPlayer(LeaderGuid))
                    team = pLeader->GetTeam();
        }
        else
             team = ((Player*)who)->GetTeam();

        uint32 newLeader;

        if (team == ALLIANCE)
            newLeader = NPC_JAINA_OUTRO;
        else
            newLeader = NPC_SYLVANA_OUTRO;

        if (Creature* pNewLeader = m_creature->SummonCreature(newLeader,WallLoc[6].x,WallLoc[6].y,WallLoc[6].z,WallLoc[6].o,TEMPSUMMON_MANUAL_DESPAWN,0,true))
        {
             pNewLeader->SetWalk(false);
             pNewLeader->SetSpeedRate(MOVE_RUN, 1.0f, true);
             pNewLeader->SetRespawnDelay(DAY);
             pNewLeader->SetHealth(pNewLeader->GetMaxHealth()/10);
             m_pInstance->SetData(DATA_ESCAPE_LIDER, pNewLeader->GetEntry());
        }
        m_pInstance->DoOpenDoor(GO_ICECROWN_DOOR_2);
        m_pInstance->SetData(TYPE_PHASE, 4);
        m_pInstance->SetNextEvent(100,GetLeader(),500);
   }

   void WaypointReached(uint32 i)
   {
        switch(i)
        {
            case 20:
                SetEscortPaused(true);
                Finish = true;
                DoCast(m_creature, SPELL_LICH_KING_CAST);
                m_pInstance->SetData(TYPE_LICH_KING, SPECIAL);
                DoScriptText(SAY_LICH_KING_END_DUN, m_creature);
                if (Creature* pLider = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(DATA_ESCAPE_LIDER)))
                {
                    pLider->CastSpell(pLider, SPELL_SILENCE, false);
                    pLider->SetLevitate(true);
                    pLider->MonsterMoveWithSpeed(pLider->GetPositionX(), pLider->GetPositionY(), pLider->GetPositionZ() + 4, 26);
                }
                m_pInstance->SetData(TYPE_PHASE, 6);
                m_pInstance->SetNextEvent(610,GetLeader(),5000);
                m_creature->SetActiveObjectState(false);
                break;
        }
   }

   void AttackStart(Unit* who)
   {
      if (!m_pInstance || !who || NonFight)
          return;

     if (m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS || who->GetTypeId() == TYPEID_PLAYER) return;

     npc_escortAI::AttackStart(who);
   }

    void SummonedCreatureJustDied(Creature* summoned)
    {
         if(!m_pInstance || !summoned)
             return;

        if (summoned->GetEntry() == NPC_JAINA_OUTRO
            || summoned->GetEntry() == NPC_SYLVANA_OUTRO)
        {
            m_creature->NearTeleportTo(5572.077f, 2283.1f, 734.976f, 3.89f);
            if (boss_lich_king_hrAI* pEscortAI = dynamic_cast<boss_lich_king_hrAI*>(m_creature->AI()))
                pEscortAI->EnterEvadeMode();
            StartEscort = false;
        }
        else
            m_pInstance->SetData(DATA_SUMMONS, 0);
    }

    void JustSummoned(Creature* summoned)
    {
        if (!m_pInstance || !summoned)
            return;

        if (summoned->GetEntry() == NPC_JAINA_OUTRO
            || summoned->GetEntry() == NPC_SYLVANA_OUTRO)
        {
            summoned->SetCreatorGuid(ObjectGuid());
            summoned->setFaction(2076);
            summoned->SetPhaseMask(65535, true);
            m_creature->SetInCombatWith(summoned);
            summoned->SetInCombatWith(m_creature);
        }
        else
        {
            summoned->SetPhaseMask(65535, true);
            summoned->SetInCombatWithZone();
            summoned->SetActiveObjectState(true);

            m_pInstance->SetData(DATA_SUMMONS, 1);

            if (Creature* pLider = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(DATA_ESCAPE_LIDER)))
            {
                summoned->GetMotionMaster()->MoveChase(pLider);
                summoned->AddThreat(pLider, 100.0f);
            }
        }
    }

    uint32 GetLeader()
    {
         if (Creature* pLider = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(DATA_ESCAPE_LIDER)))
             return pLider->GetEntry();
         else
             return 0;
    }

    void Event()
    {
        switch (m_pInstance->GetEvent(m_creature->GetEntry()))
        {
// Intro phase
        case 101:
            if(GetLeader() == NPC_JAINA_OUTRO)
            DoScriptText((GetLeader() == NPC_JAINA_OUTRO ? SAY_LICH_KING_AGGRO_A : SAY_LICH_KING_AGGRO_H), m_creature);
            if (Creature* pLider = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(DATA_ESCAPE_LIDER)))
                AttackStart(pLider);
            m_pInstance->SetNextEvent(102,GetLeader(),3000);
            break;

// Active phase
        case 200:
            SetEscortPaused(true);
            m_pInstance->SetData(DATA_SUMMONS, 3);
            DoScriptText(SAY_LICH_KING_WALL_01, m_creature);
            m_pInstance->SetNextEvent(210,m_creature->GetEntry(),2000);
            break;
        case 210:
            m_pInstance->SetNextEvent(220,m_creature->GetEntry(),2000);
            break;
        case 220:
            DoCast(m_creature, SPELL_RAISE_DEAD);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            m_pInstance->SetNextEvent(230,m_creature->GetEntry(),7000);
            break;
        case 230:
            DoScriptText(SAY_LICH_KING_WINTER, m_creature);
            DoCast(m_creature, SPELL_WINTER);
            m_creature->SetSpeedRate(MOVE_WALK, 1.3f, true);
            m_pInstance->SetNextEvent(240,m_creature->GetEntry(),1000);
            break;
        case 240:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            SetEscortPaused(false);
            m_pInstance->SetNextEvent(250,m_creature->GetEntry(),2000);
            break;
        case 250:
            m_pInstance->SetNextEvent(290,GetLeader(),1000);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            break;

        case 300:
            SetEscortPaused(true);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            m_pInstance->SetData(DATA_SUMMONS, 3);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            m_pInstance->SetNextEvent(310,m_creature->GetEntry(),500);
            break;
        case 310:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(320,m_creature->GetEntry(),500);
            break;
        case 320:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(330,m_creature->GetEntry(),5000);
            break;
        case 330:
            DoCast(m_creature, SPELL_RAISE_DEAD);
            SetEscortPaused(false);
            m_pInstance->SetNextEvent(390,GetLeader(),1000);
            break;

        case 400:
            SetEscortPaused(true);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            m_pInstance->SetData(DATA_SUMMONS, 3);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            m_pInstance->SetNextEvent(410,m_creature->GetEntry(),500);
            break;
        case 410:
            DoScriptText(SAY_LICH_KING_ABON, m_creature);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            m_pInstance->SetNextEvent(420,m_creature->GetEntry(),500);
            break;
        case 420:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(430,m_creature->GetEntry(),500);
            break;
        case 430:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(440,m_creature->GetEntry(),500);
            break;
        case 440:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(450,m_creature->GetEntry(),500);
            break;
        case 450:
            DoCast(m_creature, SPELL_RAISE_DEAD);
            m_pInstance->SetNextEvent(490,GetLeader(),5000);
            SetEscortPaused(false);
            break;

        case 500:
            m_pInstance->SetData(DATA_SUMMONS, 3);
            DoScriptText(SAY_LICH_KING_GNOUL, m_creature);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            m_pInstance->SetNextEvent(510,m_creature->GetEntry(),10000);
            break;
        case 510:
            m_creature->SetSpeedRate(MOVE_WALK, 1.3f, true);
            DoCast(m_creature, SPELL_SUMMON_ABOM);
            m_pInstance->SetNextEvent(520,m_creature->GetEntry(),500);
            break;
        case 520:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(530,m_creature->GetEntry(),500);
            break;
        case 530:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(540,m_creature->GetEntry(),500);
            break;
        case 540:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(550,m_creature->GetEntry(),500);
            break;
        case 550:
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(570,m_creature->GetEntry(),2000);
            break;
        case 570:
            DoScriptText(SAY_LICH_KING_ABON, m_creature);
            DoCast(m_creature, SPELL_WITCH_DOCTOR);
            m_pInstance->SetNextEvent(580,m_creature->GetEntry(),5000);
            break;
        case 580:
            DoScriptText(SAY_LICH_KING_ABON, m_creature);
            DoCast(m_creature, SPELL_RAISE_DEAD);
            m_pInstance->SetNextEvent(590,GetLeader(),1000);
            break;

        case 900:
            if (Creature* pLider = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(DATA_ESCAPE_LIDER)))
                m_creature->DealDamage(pLider, pLider->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_pInstance->SetData(TYPE_LICH_KING, FAIL);
            m_creature->SetActiveObjectState(false);
            m_pInstance->SetData(TYPE_PHASE, 3);
            m_pInstance->SetNextEvent(0,0);
            break;

        default:
            break;
        }
    }

     void UpdateEscortAI(const uint32 diff)
     {
        if(!m_pInstance)
            return;

        if ( (m_pInstance->GetData(TYPE_PHASE) == 4 || m_pInstance->GetData(TYPE_PHASE) == 5 || m_pInstance->GetData(TYPE_PHASE) == 7)
             && m_pInstance->GetEventTimer(m_creature->GetEntry(),diff))
            Event();

        if(m_pInstance->GetData(TYPE_LICH_KING) != IN_PROGRESS)
        {
           if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
               return;

           DoMeleeAttackIfReady();
           return;
        }

        if(m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS && !StartEscort)
        {
           StartEscort = true;
           if(m_creature->HasAura(SPELL_ICE_PRISON))
               m_creature->RemoveAurasDueToSpell(SPELL_ICE_PRISON);
           if(m_creature->HasAura(SPELL_DARK_ARROW))
               m_creature->RemoveAurasDueToSpell(SPELL_DARK_ARROW);

           m_creature->SetActiveObjectState(true);

           NonFight = true;
           m_creature->AttackStop();
           m_creature->SetWalk(true);
           m_creature->SetSpeedRate(MOVE_WALK, 2.7f, true);
           if (boss_lich_king_hrAI* pEscortAI = dynamic_cast<boss_lich_king_hrAI*>(m_creature->AI()))
               pEscortAI->Start();
        }
        if (m_pInstance->GetData(TYPE_PHASE) == 5)
            if (Creature* pLider = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(DATA_ESCAPE_LIDER)))
                if (pLider->isAlive() && pLider->IsWithinDistInMap(m_creature, 2.0f))
                {
                    m_pInstance->SetData(TYPE_PHASE,7);
                    SetEscortPaused(true);
                    m_creature->GetMotionMaster()->Clear();
                    DoScriptText(SAY_LICH_KING_WIN, m_creature);
                    m_pInstance->SetNextEvent(900,m_creature->GetEntry(),4000);
                    m_creature->CastSpell(m_creature, SPELL_FURY_OF_FROSTMOURNE, false);
            };

        return;
    }
};

CreatureAI* GetAI_boss_lich_king_hr(Creature* pCreature)
{
    return new boss_lich_king_hrAI(pCreature);
}

struct MANGOS_DLL_DECL boss_lich_king_intro_horAI : public ScriptedAI
{
    boss_lich_king_intro_horAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (BSWScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    BSWScriptedInstance* m_pInstance;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void JustDied(Unit* pKiller)
    {
    }

    void AttackStart(Unit* who)
    {
    }

    uint32 GetLeader()
    {
         if (Creature* pLider = m_pInstance->GetSingleCreatureFromStorage(m_pInstance->GetData(DATA_ESCAPE_LIDER)))
             return pLider->GetEntry();
         else
             return 0;
    }

    void Event()
    {
         switch(m_pInstance->GetEvent(m_creature->GetEntry()))
         {
            case 24:
                m_pInstance->DoOpenDoor(GO_IMPENETRABLE_DOOR);
                m_pInstance->SetNextEvent(25,GetLeader(),1000);
                break;
            case 25:
                m_creature->GetMotionMaster()->MovePoint(0, 5314.881f, 2012.496f, 709.341f);
                m_pInstance->SetNextEvent(26,m_creature->GetEntry(),3000);
                break;
            case 26:
                m_pInstance->DoCloseDoor(GO_IMPENETRABLE_DOOR);
                m_pInstance->SetNextEvent(27,m_creature->GetEntry(),7000);
                break;
            case 27:
                DoScriptText(SAY_LICH_KING_17, m_creature);
                if (Creature* pUther = (m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER)))
                    pUther->CastSpell(pUther, SPELL_UTHER_DESPAWN, false);
                m_pInstance->SetNextEvent(28,m_creature->GetEntry(),1500);
                break;
            case 28:
                if (Creature* pUther = (m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER)))
                    pUther->ForcedDespawn();
                m_pInstance->SetNextEvent(29,m_creature->GetEntry(),10000);
                break;
            case 29:
                DoScriptText(SAY_LICH_KING_18, m_creature);
                m_pInstance->SetNextEvent(30,m_creature->GetEntry(),5000);
                break;
            case 30:
                m_creature->CastSpell(m_creature, SPELL_TAKE_FROSTMOURNE, false);
                m_pInstance->DoCloseDoor(GO_FROSTMOURNE);
                m_pInstance->SetNextEvent(31,m_creature->GetEntry(),1500);
                break;
            case 31:
                if (GameObject* pFrostmourne = m_pInstance->GetSingleGameObjectFromStorage(GO_FROSTMOURNE))
                    pFrostmourne->SetPhaseMask(0, true);
                m_creature->CastSpell(m_creature, SPELL_FROSTMOURNE_VISUAL, false);
                m_pInstance->SetNextEvent(31,GetLeader(),500);
                break;
            case 32:
                DoScriptText(SAY_LICH_KING_19, m_creature);
                m_pInstance->SetNextEvent(33,m_creature->GetEntry(),9000);
                break;
            case 33:
                if (Creature* pFalric = (m_pInstance->GetSingleCreatureFromStorage(NPC_FALRIC)))
                {
                    pFalric->SetVisibility(VISIBILITY_ON);
                    pFalric->CastSpell(pFalric, SPELL_BOSS_SPAWN_AURA, false);
                    pFalric->GetMotionMaster()->MovePoint(0, 5283.309f, 2031.173f, 709.319f);
                }
                if (Creature* pMarwyn = (m_pInstance->GetSingleCreatureFromStorage(NPC_MARWYN)))
                {
                    pMarwyn->SetVisibility(VISIBILITY_ON);
                    pMarwyn->CastSpell(pMarwyn, SPELL_BOSS_SPAWN_AURA, false);
                    pMarwyn->GetMotionMaster()->MovePoint(0, 5335.585f, 1981.439f, 709.319f);
                }
                m_creature->GetMotionMaster()->MovePoint(0, 5402.286f, 2104.496f, 707.695f);
                m_pInstance->SetNextEvent(34,m_creature->GetEntry(),1000);
                break;
            case 34:
                if (Creature* pFalric = (m_pInstance->GetSingleCreatureFromStorage(NPC_FALRIC)))
                    DoScriptText(SAY_FALRIC_INTRO, pFalric);
                if (Creature* pMarwyn = (m_pInstance->GetSingleCreatureFromStorage(NPC_MARWYN)))
                    DoScriptText(SAY_MARWYN_INTRO, pMarwyn);
                m_pInstance->SetData(TYPE_EVENT, 5);
                m_pInstance->SetNextEvent(35,m_creature->GetEntry(),3000);
                break;
            case 35:
                if (GameObject* pGate = m_pInstance->GetSingleGameObjectFromStorage(GO_IMPENETRABLE_DOOR))
                     pGate->SetGoState(GO_STATE_ACTIVE);
                if (Creature* pFalric = m_pInstance->GetSingleCreatureFromStorage(NPC_FALRIC))
                    DoScriptText(SAY_FALRIC_INTRO2, pFalric);
                m_pInstance->SetData(TYPE_FALRIC, SPECIAL);
                m_pInstance->SetNextEvent(36,GetLeader(),4000);
                break;
            case 37:
                m_creature->GetMotionMaster()->MovementExpired(false);
                m_creature->SetWalk(false);
                m_creature->GetMotionMaster()->MovePoint(0, 5443.880f, 2147.095f, 707.695f);
                if (GetLeader() == NPC_JAINA)
                    DoScriptText(SAY_LICH_KING_A_21, m_creature);
                else if (GetLeader() == NPC_SYLVANA)
                    DoScriptText(SAY_LICH_KING_H_21, m_creature);
                m_pInstance->SetNextEvent(38,m_creature->GetEntry(),8000);
                break;
            case 38:
                m_pInstance->DoCloseDoor(GO_IMPENETRABLE_DOOR);
                m_pInstance->SetNextEvent(39,m_creature->GetEntry(),5000);
                break;
            case 39:
                m_creature->SetVisibility(VISIBILITY_OFF);
                m_pInstance->SetNextEvent(39,GetLeader(),1000);
                break;
            case 40:
                m_pInstance->SetData(TYPE_PHASE, 2);
                m_pInstance->SetNextEvent(0,0);
                m_creature->ForcedDespawn();
                break;
            default:
                break;
         }
    }

    void UpdateAI(const uint32 diff)
    {
         if (!m_pInstance)
             return;

         if (m_pInstance->GetEventTimer(m_creature->GetEntry(),diff) && m_pInstance->GetData(TYPE_PHASE) == 1)
            Event();

    }
};

CreatureAI* GetAI_boss_lich_king_intro_hor(Creature* pCreature)
{
    return new boss_lich_king_intro_horAI(pCreature);
}

struct MANGOS_DLL_DECL npc_undead_horAI : public BSWScriptedAI
{
    npc_undead_horAI(Creature *pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
    }

    void EnterEvadeMode()
    {
        if (!m_pInstance)
            return;

        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_LICH_KING) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        doCastAll(uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_undead_hor(Creature* pCreature)
{
    return new npc_undead_horAI(pCreature);
}

void AddSC_boss_lich_king_hr()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lich_king_hr";
    newscript->GetAI = &GetAI_boss_lich_king_hr;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_lich_king_intro_hor";
    newscript->GetAI = &GetAI_boss_lich_king_intro_hor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_undead_hor";
    newscript->GetAI = &GetAI_npc_undead_hor;
    newscript->RegisterSelf();
}
