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
SDName: Hall Of Reflection
SD%Complete: Who knows? :D
SDComment: event script!
SDErrors: They have, but i dont know were it! :D
SDCategory: hall_of_reflection
SDAuthor: MaxXx2021 aka Mioka
EndScriptData */

#include "precompiled.h"
#include "halls_of_reflection.h"
#include "escort_ai.h"

enum
{
  /*INTRO - Pre Uther*/
  SAY_JAINA_INTRO_01                 = -1668433,
  SAY_SYLVANA_INTRO_01               = -1668434,
  SAY_JAINA_INTRO_02                 = -1668435,
  SAY_JAINA_INTRO_03                 = -1668436,
  SAY_SYLVANA_INTRO_03               = -1668437,
  SAY_JAINA_INTRO_04                 = -1668438,
  SAY_SYLVANA_INTRO_04               = -1668439,

  /*INTRO - Uther Dialog*/
  SAY_UTHER_A_01                     = -1668440,
  SAY_UTHER_H_01                     = -1668441,
  SAY_JAINA_02                       = -1668442,
  SAY_SYLVANA_02                     = -1668443,
  SAY_UTHER_A_03                     = -1668444,
  SAY_UTHER_H_03                     = -1668445,
  SAY_JAINA_04                       = -1668446,
  SAY_SYLVANA_04                     = -1668447,
  SAY_UTHER_A_05                     = -1668448,
  SAY_UTHER_H_05                     = -1668449,
  SAY_JAINA_06                       = -1668450,
  SAY_SYLVANA_06                     = -1668451,
  SAY_UTHER_A_07                     = -1668452,
  SAY_UTHER_H_07                     = -1668453,
  SAY_JAINA_08                       = -1668454,
  SAY_SYLVANA_08                     = -1668455,
  SAY_UTHER_A_09                     = -1668456,
  SAY_UTHER_H_09                     = -1668457,
  SAY_JAINA_10                       = -1668458,
  SAY_UTHER_A_11                     = -1668459,
  SAY_UTHER_H_11                     = -1668660,
  SAY_JAINA_12                       = -1668461,
  SAY_SYLVANA_12                     = -1668462,
  SAY_UTHER_A_13                     = -1668463,
  SAY_UTHER_A_14                     = -1668464,
  SAY_JAINA_15                       = -1668465,

  /*INTRO - Lich King Arrive*/
  SAY_UTHER_A_16                     = -1668466,
  SAY_UTHER_H_16                     = -1668467,
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

  /*INTRO - Pre Escape*/
  SAY_LICH_KING_AGGRO_A              = -1668477,
  SAY_LICH_KING_AGGRO_H              = -1668478,
  SAY_JAINA_AGGRO                    = -1668479,
  SAY_SYLVANA_AGGRO                  = -1668480,

  /*ESCAPE*/
  SAY_JAINA_WALL_01                  = -1668487,
  SAY_SYLVANA_WALL_01                = -1668488,
  SAY_JAINA_WALL_02                  = -1668489,
  SAY_SYLVANA_WALL_02                = -1668490,
  SAY_LICH_KING_WALL_02              = -1668491,
  SAY_LICH_KING_WALL_03              = -1668492,
  SAY_LICH_KING_WALL_04              = -1668493,
  SAY_JAINA_WALL_03                  = -1668494,
  SAY_JAINA_WALL_04                  = -1668495,
  SAY_SYLVANA_WALL_03                = -1668496,
  SAY_SYLVANA_WALL_04                = -1668497,
  SAY_JAINA_ESCAPE_01                = -1668498,
  SAY_JAINA_ESCAPE_02                = -1668499,
  SAY_SYLVANA_ESCAPE_01              = -1668500,
  SAY_SYLVANA_ESCAPE_02              = -1668501,
  SAY_JAINA_TRAP                     = -1668502,
  SAY_SYLVANA_TRAP                   = -1668503,
  SAY_LICH_KING_END_01               = -1668506,
  SAY_LICH_KING_END_02               = -1668507,
  SAY_LICH_KING_END_03               = -1668508,

  SAY_ESCAPE_01                      = -1668531,
  SAY_ESCAPE_02                      = -1668532,
  SAY_ESCAPE_03                      = -1668533,


  /*SPELLS AND VISUAL EFFECTS*/
  SPELL_TAKE_FROSTMOURNE             = 72729,
  SPELL_FROSTMOURNE_DESPAWN          = 72726,
  SPELL_FROSTMOURNE_SOUNDS           = 70667,
  SPELL_CAST_VISUAL                  = 65633,  //Jaina And Sylavana cast this when summon uther.
  SPELL_BOSS_SPAWN_AURA              = 72712,  //Falric and Marwyn
  SPELL_UTHER_DESPAWN                = 70693,
  SPELL_WINTER                       = 69780,
  SPELL_FURY_OF_FROSTMOURNE          = 70063,
  SPELL_SOUL_REAPER                  = 73797,
  SPELL_RAISE_DEAD                   = 69818,
  SPELL_ICE_PRISON                   = 69708,
  SPELL_DARK_ARROW                   = 70194,
  SPELL_ICE_BARRIER                  = 69787,
  SPELL_DESTROY_ICE_WALL_01          = 69784, //Jaina
  SPELL_DESTROY_ICE_WALL_02          = 70224,
  SPELL_DESTROY_ICE_WALL_03          = 70225, //Sylvana
  SPELL_DESTRUCT_ICE_WALL            = 69784,
  SPELL_SUMMON_ICE_WALL              = 69768,
  SPELL_SYLVANA_JUMP                 = 68339,
  SPELL_SYLVANA_STEP                 = 69087,
  SPELL_SILENCE                      = 69413,
  SPELL_LICH_KING_CAST               = 57561,
  SPELL_FROSTMOURNE_VISUAL           = 73220,
  SPELL_SHIELD_DISRUPTION            = 58291,

  SPELL_ESCAPED_FROM_ARTHAS          = 72830,

  FACTION                            = 2076,
};

struct MANGOS_DLL_DECL npc_jaina_and_sylvana_HRintroAI : public ScriptedAI
{
    npc_jaina_and_sylvana_HRintroAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
        m_pInstance = (BSWScriptedInstance*)pCreature->GetInstanceData();
        Reset();
   }

   BSWScriptedInstance* m_pInstance;

   Creature* pUther;
   bool Small;

   void Reset()
   {
      m_creature->SetPhaseMask(65535, true);
      m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
      m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
      Small = false;
   }

   void Event()
   {
         switch(m_pInstance->GetEvent(m_creature->GetEntry()))
         {
            case 1:
                if (m_pInstance->GetData(TYPE_EVENT) == 2)
                    Small = true;
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_pInstance->SetNextEvent(2,m_creature->GetEntry(),2000);
                break;
            case 2:
                if(m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_INTRO_01, m_creature);
                    m_pInstance->SetNextEvent(3,m_creature->GetEntry(),5000);
                }
                else if(m_creature->GetEntry() == NPC_SYLVANA)
                {
                    DoScriptText(SAY_SYLVANA_INTRO_01, m_creature);
                    m_pInstance->SetNextEvent(3,m_creature->GetEntry(),8000);
                }
                break;
            case 3:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_INTRO_02, m_creature);
                    m_pInstance->SetNextEvent(4,m_creature->GetEntry(),5000);
                }
                else if(m_creature->GetEntry() == NPC_SYLVANA)
                    m_pInstance->SetNextEvent(4,m_creature->GetEntry(),500);
                break;
            case 4:
                m_creature->SetWalk(false);
                m_creature->GetMotionMaster()->MovePoint(0, 5307.031f, 1997.920f, 709.341f);
                m_pInstance->SetNextEvent(5,m_creature->GetEntry(),10000);
                break;
            case 5:
                if(Creature* pTarget = m_creature->SummonCreature(NPC_ALTAR_TARGET,5309.374f,2006.788f,711.615f,1.37f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,360000))
                {
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTarget->GetObjectGuid());
                    pTarget->SetCreatorGuid(ObjectGuid());
                }
                m_pInstance->SetNextEvent(6,m_creature->GetEntry(),1000);
                break;
            case 6:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_INTRO_03, m_creature);
                    m_pInstance->SetNextEvent(7,m_creature->GetEntry(),5000);
                }
                if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    DoScriptText(SAY_SYLVANA_INTRO_03, m_creature);
                    m_pInstance->SetNextEvent(7,m_creature->GetEntry(),5000);
                }
                break;
            case 7:
                DoCast(m_creature, SPELL_CAST_VISUAL);
                if (m_creature->GetEntry() == NPC_JAINA)
                    DoScriptText(SAY_JAINA_INTRO_04, m_creature);
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                    DoScriptText(SAY_SYLVANA_INTRO_04, m_creature);
                m_pInstance->SetNextEvent(8,m_creature->GetEntry(),3000);
                break;
            case 8:
                DoCast(m_creature, SPELL_FROSTMOURNE_SOUNDS);
                if(GameObject* pFrostmourne = m_pInstance->GetSingleGameObjectFromStorage(GO_FROSTMOURNE))
                    pFrostmourne->SetGoState(GO_STATE_ACTIVE);
                if(m_creature->GetEntry() == NPC_JAINA)
                    m_pInstance->SetNextEvent(9,m_creature->GetEntry(),12000);
                if(m_creature->GetEntry() == NPC_SYLVANA)
                    m_pInstance->SetNextEvent(9,m_creature->GetEntry(),8000);
                break;
            case 9:
                if(Creature* Uther = m_creature->SummonCreature(NPC_UTHER,5308.228f,2003.641f,709.341f,4.17f,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,360000))
                {
                    pUther = Uther;
                    Uther->SetCreatorGuid(ObjectGuid());
                    Uther->SetRespawnDelay(DAY);
                    Uther->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, Uther->GetObjectGuid());
                    if (m_creature->GetEntry() == NPC_JAINA)
                    {
                        DoScriptText(SAY_UTHER_A_01, Uther);
                        m_pInstance->SetNextEvent(10,m_creature->GetEntry(),3000);
                    }
                    else if (m_creature->GetEntry() == NPC_SYLVANA)
                    {
                        DoScriptText(SAY_UTHER_H_01, Uther);
                        m_pInstance->SetNextEvent(10,m_creature->GetEntry(),10000);
                    }
                }
                break;
            case 10:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_02, m_creature);
                    m_pInstance->SetNextEvent(11,m_creature->GetEntry(),5000);
                }
                else if(m_creature->GetEntry() == NPC_SYLVANA)
                {
                    DoScriptText(SAY_SYLVANA_02, m_creature);
                    m_pInstance->SetNextEvent(11,m_creature->GetEntry(),3000);
                }
                break;
            case 11:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                {
                    DoScriptText(SAY_UTHER_A_03, pUther);
                    m_pInstance->SetNextEvent(Small ? 24 : 12, m_creature->GetEntry(),7000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA && pUther)
                {
                    DoScriptText(SAY_UTHER_H_03, pUther);
                    m_pInstance->SetNextEvent(Small ? 24 : 12, m_creature->GetEntry(),6000);
                }
                break;
            case 12:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_04, m_creature);
                    m_pInstance->SetNextEvent(13,m_creature->GetEntry(),2000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    DoScriptText(SAY_SYLVANA_04, m_creature);
                    m_pInstance->SetNextEvent(13,m_creature->GetEntry(),5000);
                }
                break;
            case 13:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                {
                    DoScriptText(SAY_UTHER_A_05, pUther);
                    m_pInstance->SetNextEvent(14,m_creature->GetEntry(),10000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA && pUther)
                {
                    DoScriptText(SAY_UTHER_H_05, pUther);
                    m_pInstance->SetNextEvent(14,m_creature->GetEntry(),19000);
                }
                break;
            case 14:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_06, m_creature);
                    m_pInstance->SetNextEvent(15,m_creature->GetEntry(),6000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    DoScriptText(SAY_SYLVANA_06, m_creature);
                    m_pInstance->SetNextEvent(15,m_creature->GetEntry(),2000);
                }
                break;
            case 15:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                {
                    DoScriptText(SAY_UTHER_A_07, pUther);
                    m_pInstance->SetNextEvent(16,m_creature->GetEntry(),12000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA && pUther)
                {
                    DoScriptText(SAY_UTHER_H_07, pUther);
                    m_pInstance->SetNextEvent(16,m_creature->GetEntry(),6000);
                }
                break;
            case 16:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_08, m_creature);
                    m_pInstance->SetNextEvent(17,m_creature->GetEntry(),6000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    DoScriptText(SAY_SYLVANA_08, m_creature);
                    m_pInstance->SetNextEvent(17,m_creature->GetEntry(),3000);
                }
                break;
            case 17:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                {
                    DoScriptText(SAY_UTHER_A_09, pUther);
                    m_pInstance->SetNextEvent(18,m_creature->GetEntry(),12000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA && pUther)
                {
                    DoScriptText(SAY_UTHER_H_09, pUther);
                    m_pInstance->SetNextEvent(18,m_creature->GetEntry(),11000);
                }
                break;
            case 18:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_10, m_creature);
                    m_pInstance->SetNextEvent(19,m_creature->GetEntry(),12000);
                }
                else if(m_creature->GetEntry() == NPC_SYLVANA)
                {
                    m_pInstance->SetNextEvent(19,m_creature->GetEntry(),500);
                }
                break;
            case 19:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                {
                    DoScriptText(SAY_UTHER_A_11, pUther);
                    m_pInstance->SetNextEvent(20,m_creature->GetEntry(),24000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA && pUther)
                {
                    DoScriptText(SAY_UTHER_H_11, pUther);
                    m_pInstance->SetNextEvent(20,m_creature->GetEntry(),9000);
                }
                break;
            case 20:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_12, m_creature);
                    m_pInstance->SetNextEvent(21,m_creature->GetEntry(),2000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    DoScriptText(SAY_SYLVANA_12, m_creature);
                    m_pInstance->SetNextEvent(21,m_creature->GetEntry(),2100);
                }
                break;
            case 21:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                {
                    DoScriptText(SAY_UTHER_A_13, pUther);
                    m_pInstance->SetNextEvent(22,m_creature->GetEntry(),5000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    m_pInstance->SetNextEvent(22,m_creature->GetEntry(),500);
                }
                break;
            case 22:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                {
                    DoScriptText(SAY_UTHER_A_14, pUther);
                    m_pInstance->SetNextEvent(23,m_creature->GetEntry(),12000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    m_pInstance->SetNextEvent(23,m_creature->GetEntry(),500);
                }
                break;
            case 23:
                if (m_creature->GetEntry() == NPC_JAINA)
                {
                    DoScriptText(SAY_JAINA_15, m_creature);
                    m_pInstance->SetNextEvent(24,m_creature->GetEntry(),2000);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA)
                {
                    m_pInstance->SetNextEvent(24,m_creature->GetEntry(),500);
                }
                break;
            case 24:
                if (m_creature->GetEntry() == NPC_JAINA && pUther)
                   DoScriptText(SAY_UTHER_A_16, pUther);
                else if (m_creature->GetEntry() == NPC_SYLVANA && pUther)
                   DoScriptText(SAY_UTHER_H_16, pUther);
                if(Creature* LichKing = m_creature->SummonCreature(NPC_LICH_KING,5362.469f,2062.342f,707.695f,3.97f,TEMPSUMMON_MANUAL_DESPAWN,0,true))
                {
                   LichKing->SetRespawnDelay(DAY);
                   LichKing->SetCreatorGuid(ObjectGuid());
                }
                m_pInstance->SetNextEvent(24,NPC_LICH_KING,2000);
                break;
            case 25:
                if (pUther)
                   pUther->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);
                m_pInstance->SetNextEvent(25,NPC_LICH_KING,2000);
                break;
            case 31:
                m_creature->RemoveAurasDueToSpell(SPELL_FROSTMOURNE_SOUNDS);
                m_pInstance->SetNextEvent(32,NPC_LICH_KING,4500);
                break;
            case 36:
                if (m_creature->GetEntry() == NPC_JAINA)
                   DoScriptText(SAY_JAINA_20, m_creature);
                else if(m_creature->GetEntry() == NPC_SYLVANA)
                   DoScriptText(SAY_SYLVANA_20, m_creature);
                m_creature->GetMotionMaster()->MovePoint(0, 5443.880f, 2147.095f, 707.695f);
                m_pInstance->SetNextEvent(37,NPC_LICH_KING,4000);
                break;
            case 39:
                m_pInstance->SetNextEvent(40,NPC_LICH_KING,1000);
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

         return;
     }
};

bool GossipHello_npc_jaina_and_sylvana_HRintro(Player* pPlayer, Creature* pCreature)
{
    BSWScriptedInstance* m_pInstance = (BSWScriptedInstance*)pCreature->GetInstanceData();

    if(pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu( pCreature->GetObjectGuid());

    switch(pCreature->GetEntry())
    {
       case NPC_JAINA:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, -3594536, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, -3594537, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            break;
       case NPC_SYLVANA:
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, -3594538, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, -3594539, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            break;
    }

    pPlayer->PlayerTalkClass->SendGossipMenu(907,pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_jaina_and_sylvana_HRintro(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    BSWScriptedInstance* m_pInstance = (BSWScriptedInstance*)pCreature->GetInstanceData();

    if (!m_pInstance) return false;

    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
               pPlayer->CLOSE_GOSSIP_MENU();
               m_pInstance->SetData(TYPE_EVENT, 1);
               m_pInstance->SetData(TYPE_PHASE, 1);
               break;
        case GOSSIP_ACTION_INFO_DEF+2:
               pPlayer->CLOSE_GOSSIP_MENU();
               m_pInstance->SetData(TYPE_EVENT, 2);
               m_pInstance->SetData(TYPE_PHASE, 1);
               break;
    }

    m_pInstance->SetNextEvent(1, pCreature->GetEntry(), 500);

    m_pInstance->SetData(DATA_ESCAPE_LIDER,pCreature->GetEntry());

    return true;
}

struct MANGOS_DLL_DECL npc_jaina_and_sylvana_HRextroAI : public npc_escortAI
{
   npc_jaina_and_sylvana_HRextroAI(Creature *pCreature) : npc_escortAI(pCreature)
   {
        m_pInstance = (BSWScriptedInstance*)pCreature->GetInstanceData();
        Reset();
   }

    BSWScriptedInstance* m_pInstance;

    uint32 CastTimer;
    uint32 HoldTimer;
    uint8 m_wallNum;
    bool Fight;
    ObjectGuid wallTarget;
    uint32    m_chestID;

    void Reset()
    {
        if(!m_pInstance) return;

        if(m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS) return;

        HoldTimer = 10000;
        Fight = true;
        m_wallNum = 0;
        wallTarget = ObjectGuid();
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        m_creature->CastSpell(m_creature, SPELL_ICE_BARRIER, false);
        if(m_creature->GetEntry() == NPC_JAINA_OUTRO)
        {
            m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2HL);
        }

    }

    void AttackStart(Unit* who)
    {
       if (!who)
           return;

       if (m_creature->GetEntry() != NPC_SYLVANA_OUTRO)
           return;

       if (m_pInstance->GetData(TYPE_LICH_KING) == IN_PROGRESS || Fight != true)
           return;

       npc_escortAI::AttackStart(who);

    }

    void JustDied(Unit* pKiller)
    {
        if(!m_pInstance)
            return;
        DoDestructWall();
    }

    void DoSummonWall(uint8 wallNum)
    {
        if(!m_pInstance || wallNum > 3)
            return;

        m_wallNum = wallNum+1;
        HoldTimer = 20000;

        switch (wallNum)
        {
            case 0: m_pInstance->SetNextEvent(200,BOSS_LICH_KING,500); break;
            case 1: m_pInstance->SetNextEvent(300,BOSS_LICH_KING,500); break;
            case 2: m_pInstance->SetNextEvent(400,BOSS_LICH_KING,500); break;
            case 3: m_pInstance->SetNextEvent(500,BOSS_LICH_KING,500); break;
            default:
                break;
        }

        if (Creature* pWallTarget = m_creature->SummonCreature(NPC_ICE_WALL,WallLoc[wallNum].x,WallLoc[wallNum].y,WallLoc[wallNum].z,WallLoc[wallNum].o,TEMPSUMMON_MANUAL_DESPAWN,0, true))
        {
            pWallTarget->SetPhaseMask(65535, true);
            pWallTarget->setFaction(114);
            pWallTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            pWallTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            wallTarget = pWallTarget->GetObjectGuid();
            pWallTarget->CastSpell(pWallTarget, SPELL_SUMMON_ICE_WALL, false);
        }
    }

    void DoDestructWall()
    {
        m_pInstance->DoOpenDoor(GO_ICE_WALL);
        if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
        {
            pWallTarget->ForcedDespawn();
        }
        wallTarget =  ObjectGuid();
        m_wallNum = 0;
    }

    void WaypointReached(uint32 i)
    {
        switch(i)
        {
            case 0:
                break;
            case 1:
                break;
            case 2:
                DoSummonWall(0);
                break;
            case 3:
                break;
            case 4:
                if (m_creature->GetEntry() == NPC_JAINA_OUTRO)
                    DoScriptText(SAY_JAINA_WALL_01, m_creature);
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                    DoScriptText(SAY_SYLVANA_WALL_01, m_creature);
                CastTimer = 1000;
                SetEscortPaused(true);
                if(m_creature->GetEntry() == NPC_JAINA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_01, false);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_02, false);
                }
                break;
            case 5:
                break;
            case 6:
                DoSummonWall(1);
                break;
            case 7:
                break;
            case 8:
                if (m_creature->GetEntry() == NPC_JAINA_OUTRO)
                    DoScriptText(SAY_JAINA_WALL_02, m_creature);
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                    DoScriptText(SAY_SYLVANA_WALL_02, m_creature);
                CastTimer = 1000;
                SetEscortPaused(true);
                if(m_creature->GetEntry() == NPC_JAINA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_01, false);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_02, false);
                }
                break;
            case 9:
                if (m_creature->GetEntry() == NPC_JAINA_OUTRO)
                   DoScriptText(SAY_JAINA_ESCAPE_01, m_creature);
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                   DoScriptText(SAY_SYLVANA_ESCAPE_01, m_creature);
                DoSummonWall(2);
                break;
            case 10:
                break;
            case 11:
                break;
            case 12:
                if(m_creature->GetEntry() == NPC_JAINA_OUTRO)
                   DoScriptText(SAY_JAINA_WALL_03, m_creature);
                if(m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                   DoScriptText(SAY_SYLVANA_WALL_03, m_creature);
                CastTimer = 1000;
                SetEscortPaused(true);
                if(m_creature->GetEntry() == NPC_JAINA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_01, false);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_02, false);
                }
                break;
            case 13:
                if (m_creature->GetEntry() == NPC_JAINA_OUTRO)
                    DoScriptText(SAY_JAINA_ESCAPE_02, m_creature);
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                    DoScriptText(SAY_SYLVANA_ESCAPE_02, m_creature);
                break;
            case 14:
                break;
            case 15:
                DoSummonWall(3);
                break;
            case 16:
                if (m_creature->GetEntry() == NPC_JAINA_OUTRO)
                    DoScriptText(SAY_JAINA_WALL_04, m_creature);
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                    DoScriptText(SAY_SYLVANA_WALL_04, m_creature);
                CastTimer = 1000;
                SetEscortPaused(true);
                if(m_creature->GetEntry() == NPC_JAINA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_01, false);
                }
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
                        m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_02, false);
                }
                break;
            case 17:
                break;
            case 18:
                break;
            case 19:
                if (m_creature->GetEntry() == NPC_JAINA_OUTRO)
                    DoScriptText(SAY_JAINA_TRAP, m_creature);
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                    DoScriptText(SAY_SYLVANA_TRAP, m_creature);
                break;
            case 20:
                SetEscortPaused(true);
                if (m_creature->GetEntry() == NPC_JAINA_OUTRO)
                   m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY2HL);
                else if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                   m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                break;
            default:
                break;
        }
    }

    void Intro()
    {
        switch(m_pInstance->GetEvent(m_creature->GetEntry()))
        {
            case 100:
                m_creature->SetWalk(false);
                if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                {
                    if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                        AttackStart(pLichKing);
                }
                m_pInstance->SetNextEvent(101,BOSS_LICH_KING,500);
                break;
           case 102:
                if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    Fight = false;
                    m_creature->GetMotionMaster()->MovePoint(0, (m_creature->GetPositionX()-5)+rand()%10, (m_creature->GetPositionY()-5)+rand()%10, m_creature->GetPositionZ());
                    m_pInstance->SetNextEvent(103,m_creature->GetEntry(),3000);
                }
                else
                    m_pInstance->SetNextEvent(103,m_creature->GetEntry(),500);
              break;
           case 103:
                if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                   Fight = true;
                m_pInstance->SetNextEvent(104,m_creature->GetEntry(),500);
                break;
           case 104:
                if(m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                        m_creature->CastSpell(pLichKing, SPELL_SYLVANA_STEP, false);
                    m_pInstance->SetNextEvent(105,m_creature->GetEntry(),3000);
                }
                else
                    m_pInstance->SetNextEvent(105,m_creature->GetEntry(),500);
              break;
           case 105:
                if(m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    Fight = false;
                    m_creature->GetMotionMaster()->MovePoint(0, (m_creature->GetPositionX()-5)+rand()%10, (m_creature->GetPositionY()-5)+rand()%10, m_creature->GetPositionZ());
                    m_pInstance->SetNextEvent(106,m_creature->GetEntry(),3000);
                }
                else
                    m_pInstance->SetNextEvent(106,m_creature->GetEntry(),12000);
                break;
           case 106:
                Fight = true;
                if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                    m_creature->CastSpell(pLichKing,(m_creature->GetEntry() == NPC_JAINA_OUTRO ? SPELL_ICE_PRISON : SPELL_DARK_ARROW),true);
                m_pInstance->SetNextEvent(107,m_creature->GetEntry(),2500);
                break;
           case 107:
                m_creature->RemoveAurasDueToSpell(SPELL_ICE_BARRIER);
                if(m_creature->GetEntry() == NPC_JAINA_OUTRO)
                {
                    if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                        if (!pLichKing->HasAura(SPELL_ICE_PRISON))
                            pLichKing->CastSpell(pLichKing,SPELL_ICE_PRISON,true);
                    DoScriptText(SAY_JAINA_AGGRO, m_creature);
                }
                else if(m_creature->GetEntry() == NPC_SYLVANA_OUTRO)
                {
                    if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                        if (!pLichKing->HasAura(SPELL_DARK_ARROW))
                            pLichKing->CastSpell(pLichKing,SPELL_DARK_ARROW,true);
                    DoScriptText(SAY_SYLVANA_AGGRO, m_creature);
                }
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                m_creature->AttackStop();
                m_pInstance->SetNextEvent(108,m_creature->GetEntry(),3000);
                break;
           case 108:
                m_creature->GetMotionMaster()->MovePoint(0, 5577.187f, 2236.003f, 733.012f);
                if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pLichKing->GetObjectGuid());
                m_pInstance->SetNextEvent(109,m_creature->GetEntry(),10000);
                break;
           case 109:
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
//              Count = 1;
                m_pInstance->SetNextEvent(110,m_creature->GetEntry(),10000);
                break;
        }
   }

    void Outro()
    {
        switch(m_pInstance->GetEvent(m_creature->GetEntry()))
        {
            case 610:
              m_creature->CastSpell(m_creature, SPELL_SHIELD_DISRUPTION,false);
              m_pInstance->SetData(DATA_ESCAPE_LIDER,m_creature->GetEntry());
              m_creature->SummonGameobject(GO_CAVE, 5275.28f, 1694.23f, 786.147f, 0.981225f, 0);
              m_pInstance->SetNextEvent(611,m_creature->GetEntry(),6000);
              break;
            case 611:
              if (GameObject* pCave = m_pInstance->GetSingleGameObjectFromStorage(GO_CAVE))
                  pCave->SetGoState(GO_STATE_READY);
              m_creature->RemoveAurasDueToSpell(SPELL_SILENCE);
              m_creature->SetLevitate(false);
              m_creature->CastSpell(m_creature, SPELL_SHIELD_DISRUPTION,false);
              m_creature->SetWalk(false);
              m_creature->GetMotionMaster()->MovePoint(0, 5258.911328f,1652.112f,784.295166f);
              DoScriptText(SAY_ESCAPE_01, m_creature);
              m_pInstance->SetNextEvent(612,m_creature->GetEntry(),10000);
              break;
           case 612:
              m_creature->CastSpell(m_creature,SPELL_ESCAPED_FROM_ARTHAS,false);
              m_pInstance->SetData(TYPE_LICH_KING, DONE);
              DoScriptText(SAY_ESCAPE_02, m_creature);
              m_pInstance->SetNextEvent(613,m_creature->GetEntry(),10000);
              break;
           case 613:
              DoScriptText(SAY_ESCAPE_03, m_creature);
              m_pInstance->SetNextEvent(614,m_creature->GetEntry(),10000);
              break;
           case 614:
              m_creature->GetMotionMaster()->MovePoint(0, 5240.66f, 1646.93f, 784.302f);
              m_pInstance->SetNextEvent(615,m_creature->GetEntry(),5000);
              break;
           case 615:
              m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
              m_creature->SetOrientation(0.68f);
              m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
              m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
              m_pInstance->SetNextEvent(616,m_creature->GetEntry(),5000);
              break;
        }
   }

    void UpdateEscortAI(const uint32 diff)
    {
        if(!m_pInstance || m_pInstance->GetData(TYPE_PHASE) < 4)
            return;

        if (m_pInstance->GetData(TYPE_PHASE) == 6 && m_pInstance->GetEventTimer(m_creature->GetEntry(),diff))
        {
            Outro();
            return;
        }

        DoMeleeAttackIfReady();

        if (m_pInstance->GetData(TYPE_PHASE) == 4 && m_pInstance->GetEventTimer(m_creature->GetEntry(),diff))
        {
            Intro();
            return;
        }

        if (m_creature->GetEntry() == NPC_SYLVANA_OUTRO && !wallTarget.IsEmpty() && m_wallNum && CastTimer < diff)
        {
            if (Creature* pWallTarget = m_creature->GetMap()->GetCreature(wallTarget))
               m_creature->CastSpell(pWallTarget, SPELL_DESTROY_ICE_WALL_03, false);
            CastTimer = 1000;
        }
        else
            CastTimer -= diff;

        if (!wallTarget.IsEmpty() && m_wallNum && m_pInstance->GetData(DATA_SUMMONS) == 0 && HoldTimer < 1000)
        {
            m_creature->InterruptNonMeleeSpells(false);
            SetEscortPaused(false);

            switch(m_wallNum)
            {
                case 1:
                    DoDestructWall();
                    if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                        DoScriptText(SAY_LICH_KING_WALL_02, pLichKing);
                    break;
                case 2:
                    DoDestructWall();
                    if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                        DoScriptText(SAY_LICH_KING_WALL_03, pLichKing);
                    break;
                case 3:
                    DoDestructWall();
                    if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                        DoScriptText(SAY_LICH_KING_WALL_04, pLichKing);
                    break;
                case 4:
                    DoDestructWall();
                    if (Creature* pLichKing = m_pInstance->GetSingleCreatureFromStorage(BOSS_LICH_KING))
                    {
                        pLichKing->RemoveAurasDueToSpell(SPELL_WINTER);
                        pLichKing->SetSpeedRate(MOVE_WALK, 2.5f, true);
                    }
                    break;
            }
            HoldTimer = 10000;
         }
         else
         {
             if (HoldTimer <= diff)
                 HoldTimer = 0;
             else
                 HoldTimer -= diff;
         }
    }
};

bool GossipHello_npc_jaina_and_sylvana_HRextro(Player* pPlayer, Creature* pCreature)
{

    ScriptedInstance*   m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();

    if(!m_pInstance) return false;

    if(m_pInstance->GetData(TYPE_LICH_KING) == DONE) return false;

    if(pCreature->isQuestGiver())
       pPlayer->PrepareQuestMenu( pCreature->GetObjectGuid());

    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, -3594540, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_jaina_and_sylvana_HRextro(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
           pPlayer->CLOSE_GOSSIP_MENU();
           ((npc_jaina_and_sylvana_HRextroAI*)pCreature->AI())->Start(true);
           pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
           pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
           pCreature->SetUInt64Value(UNIT_FIELD_TARGET, 0);

           if(m_pInstance)
           {
              m_pInstance->SetData(DATA_ESCAPE_LIDER, pCreature->GetEntry());
              m_pInstance->SetData(TYPE_LICH_KING, IN_PROGRESS);
              m_pInstance->SetData(TYPE_PHASE, 5);
           }
           return true;
           break;
       default:  return false;
    }
}

CreatureAI* GetAI_npc_jaina_and_sylvana_HRintro(Creature* pCreature)
{
    return new npc_jaina_and_sylvana_HRintroAI(pCreature);
}

CreatureAI* GetAI_npc_jaina_and_sylvana_HRextro(Creature* pCreature)
{
    return new npc_jaina_and_sylvana_HRextroAI(pCreature);
}

enum GENERAL_EVENT
{
   SAY_AGGRO                    = -1594519,
   SAY_DEATH                    = -1594520,

   SPELL_SHIELD_THROWN          = 69222,
};

struct MANGOS_DLL_DECL npc_frostworn_generalAI : public ScriptedAI
{
   npc_frostworn_generalAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
   }

   ScriptedInstance* m_pInstance;

   uint32 m_uiShieldTimer;

   void Reset()
   {
        if (!m_pInstance)
            return;
        m_uiShieldTimer = 5000;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
   }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;
        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_FROST_GENERAL, DONE);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (m_creature->getVictim())
            return;

        if (pWho->GetTypeId() != TYPEID_PLAYER
            || m_pInstance->GetData(TYPE_MARWYN) != DONE
            || !m_creature->IsWithinDistInMap(pWho, 30.0f)
            ) return;

        if (Player* pPlayer = (Player*)pWho)
            if (pPlayer->isGameMaster()) return;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        AttackStart(pWho);
    }

    void Aggro(Unit* pVictim)
    {
        if (!m_pInstance)
            return;
        DoScriptText(SAY_AGGRO, m_creature);
        m_pInstance->SetData(TYPE_FROST_GENERAL, IN_PROGRESS);

        Map::PlayerList const &pList = m_creature->GetMap()->GetPlayers();
        for (Map::PlayerList::const_iterator i = pList.begin(); i != pList.end(); ++i)
        {
            if (Player* pPlayer = i->getSource())
            {
               if (pPlayer && pPlayer->isAlive() && pPlayer->IsInMap(m_creature))
               {
                   if (Creature* pMirror = m_creature->SummonCreature(NPC_SPIRITUAL_REFLECTION,0,0,0,0,TEMPSUMMON_DEAD_DESPAWN,0, true))
                   {
                      pMirror->SetPhaseMask(65535, true);
                      pMirror->SetInCombatWith(pPlayer);
                      pMirror->AddThreat(pPlayer, 1000.0f);
                   }

               }
            }
        };

    }

   void UpdateAI(const uint32 uiDiff)
   {
        if(!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiShieldTimer < uiDiff)
        {
            if(Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
               DoCast(pTarget,SPELL_SHIELD_THROWN);
            m_uiShieldTimer = urand(4000, 8000);
        }
        else m_uiShieldTimer -= uiDiff;

        DoMeleeAttackIfReady();
   }
};

CreatureAI* GetAI_npc_frostworn_general(Creature* pCreature)
{
    return new npc_frostworn_generalAI(pCreature);
}

enum spiritual_reflection
{
   SPELL_REFLECTION_GHOST       = 69861,
   SPELL_CLONE                  = 69828,
   SPELL_CLONE2                 = 69837,

   SPELL_BALEFUL_STRIKE         = 69933,
   SPELL_SPIRIT_BURST           = 69900,

};

struct MANGOS_DLL_DECL npc_spiritual_reflectionAI : public BSWScriptedAI
{
    npc_spiritual_reflectionAI(Creature *pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool isMirror;
    ObjectGuid victimGuid;

    void Reset()
    {
        if (!m_pInstance)
            return;
        isMirror = false;
        victimGuid = ObjectGuid();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void Aggro(Unit* pVictim)
    {
        if (!m_pInstance || !pVictim || pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        if (victimGuid.IsEmpty())
            victimGuid = pVictim->GetObjectGuid();

        DoStartMovement(pVictim);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_pInstance || !m_creature )
            return;

        if (uiDamage >= m_creature->GetHealth())
            doCast(SPELL_SPIRIT_BURST);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_FROST_GENERAL) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!isMirror)
        {
            if (Unit* pVictim = m_creature->GetMap()->GetUnit(victimGuid))
                if (m_creature->IsWithinDistInMap(pVictim, 5.0f))
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    pVictim->CastSpell(m_creature, SPELL_CLONE, true);
                    pVictim->CastSpell(m_creature, SPELL_CLONE2, true);
                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID,   pVictim->GetUInt32Value(PLAYER_VISIBLE_ITEM_16_ENTRYID));
                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+1, pVictim->GetUInt32Value(PLAYER_VISIBLE_ITEM_17_ENTRYID));
                    m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID+2, pVictim->GetUInt32Value(PLAYER_VISIBLE_ITEM_18_ENTRYID));
                    pVictim->CastSpell(m_creature, SPELL_REFLECTION_GHOST, true);
                    isMirror = true;
                }
        }

        if (!isMirror)
            return;

        timedCast(SPELL_BALEFUL_STRIKE,uiDiff);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_spiritual_reflection(Creature* pCreature)
{
    return new npc_spiritual_reflectionAI(pCreature);
}

struct MANGOS_DLL_DECL npc_queldelar_horAI : public ScriptedAI
{
    npc_queldelar_horAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (BSWScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    BSWScriptedInstance* m_pInstance;
    bool intro;
    Team team;
    uint32 newLeader;

    void Reset()
    {
        intro = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_pInstance || intro)
            return;

        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER || !pWho->IsWithinDistInMap(m_creature, 22.0f))
            return;

        debug_log("HOR event started");

        intro = true;

        if (m_pInstance->GetData(TYPE_LICH_KING) == DONE)
            return;
        else if (m_pInstance->GetData(TYPE_FROST_GENERAL) == DONE)
        {
            m_pInstance->DoOpenDoor(GO_IMPENETRABLE_DOOR);
            m_pInstance->DoOpenDoor(GO_ICECROWN_DOOR_2);
            m_pInstance->SetData(TYPE_PHASE, 3);
            return;
        }
        else if (m_pInstance->GetData(TYPE_MARWYN) == DONE)
        {
            m_pInstance->DoOpenDoor(GO_IMPENETRABLE_DOOR);
            return;
        }
        else if (m_pInstance->GetData(TYPE_FALRIC) == DONE)
        {
            if (Creature* pMarwyn = m_pInstance->GetSingleCreatureFromStorage(NPC_MARWYN))
            {
                pMarwyn->SetVisibility(VISIBILITY_ON);
                pMarwyn->CastSpell(pMarwyn, SPELL_BOSS_SPAWN_AURA, false);
                m_pInstance->SetData(TYPE_EVENT, 7);
                m_pInstance->SetData(TYPE_MARWYN, SPECIAL);
            }
            return;
        }


        if (Group* pGroup = ((Player*)pWho)->GetGroup())
        {
            ObjectGuid LeaderGuid = pGroup->GetLeaderGuid();
            if (!LeaderGuid.IsEmpty())
                if (Player* pLeader =m_creature->GetMap()->GetPlayer(LeaderGuid))
                    team = pLeader->GetTeam();
        }
        else
             team = ((Player*)pWho)->GetTeam();


        if (team == ALLIANCE)
            newLeader = NPC_JAINA;
        else
            newLeader = NPC_SYLVANA;

        debug_log("HOR event: team %u, leader %u ",team,newLeader);

        if (Creature* pNewLeader = m_creature->SummonCreature(newLeader,WallLoc[4].x,WallLoc[4].y,WallLoc[4].z,WallLoc[4].o,TEMPSUMMON_MANUAL_DESPAWN,0,true))
        {
             pNewLeader->SetCreatorGuid(ObjectGuid());
             pNewLeader->setFaction(35);
             pNewLeader->SetPhaseMask(65535, true);
             pNewLeader->GetMotionMaster()->MovePoint(0, WallLoc[5].x,WallLoc[5].y,WallLoc[5].z);
             pNewLeader->SetWalk(false);
             pNewLeader->SetSpeedRate(MOVE_RUN, 1.0f, true);
             pNewLeader->SetRespawnDelay(DAY);
        }
//        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, 63135);
    }

    void AttackStart(Unit* who)
    {
         return;
    }

    void UpdateAI(const uint32 diff)
    {
    }
};
CreatureAI* GetAI_npc_queldelar_hor(Creature* pCreature)
{
    return new npc_queldelar_horAI(pCreature);
}


void AddSC_halls_of_reflection()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_jaina_and_sylvana_HRintro";
    newscript->GetAI = &GetAI_npc_jaina_and_sylvana_HRintro;
    newscript->pGossipHello = &GossipHello_npc_jaina_and_sylvana_HRintro;
    newscript->pGossipSelect = &GossipSelect_npc_jaina_and_sylvana_HRintro;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_jaina_and_sylvana_HRextro";
    newscript->GetAI = &GetAI_npc_jaina_and_sylvana_HRextro;
    newscript->pGossipHello = &GossipHello_npc_jaina_and_sylvana_HRextro;
    newscript->pGossipSelect = &GossipSelect_npc_jaina_and_sylvana_HRextro;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_frostworn_general";
    newscript->GetAI = &GetAI_npc_frostworn_general;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_spiritual_reflection";
    newscript->GetAI = &GetAI_npc_spiritual_reflection;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_queldelar_hor";
    newscript->GetAI = &GetAI_npc_queldelar_hor;
    newscript->RegisterSelf();
}
