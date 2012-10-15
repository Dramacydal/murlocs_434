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
#include "escort_ai.h"
#include "WorldPacket.h"

/*###
## npc_chromi_start
###*/

struct MANGOS_DLL_DECL npc_chromi_startAI : public ScriptedAI
{
    npc_chromi_startAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        m_bCounterHere = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bCounterHere;

    void Reset()
    {
        m_bCounterHere = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_pInstance || !pWho || pWho->GetTypeId() != TYPEID_PLAYER || ((Player*)pWho)->isGameMaster() ||
            m_creature->GetDistance2d(pWho) > 15)
            return;

        if (!m_bCounterHere)
        {
            m_pInstance->DoUpdateWorldState(WORLD_STATE_COS_CRATE_ON, 1);
            m_pInstance->SetData(TYPE_QUEST, IN_PROGRESS);
            m_bCounterHere = true;
        }

        if (m_pInstance->GetData(TYPE_QUEST) != DONE && !((Player*)pWho)->HasItemCount(ITEM_ARCANE_DISRUPTOR, 1))
        {
            ItemPosCountVec dest;
            if (((Player*)pWho)->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_ARCANE_DISRUPTOR, 1) == EQUIP_ERR_OK)
                if (Item* pItem = ((Player*)pWho)->StoreNewItem(dest, ITEM_ARCANE_DISRUPTOR, true))
                    ((Player*)pWho)->SendNewItem(pItem, 1, true, true);
        }
    }
};

/*###
## npc_mike
###*/

enum
{
   SAY_MIKE01                = -1597270,
   SAY_FORRESTER02           = -1597271,
   SAY_JAMES03               = -1597272,
   SAY_SIABI04               = -1597273,
   SAY_MIKE05                = -1597274,
   SAY_CORICKS06             = -1597275,
   SAY_GRIAN07               = -1597276,
   SAY_CORICKS08             = -1597277,
   SAY_JAMES09               = -1597278,
   SAY_FORRESTER10           = -1597279,

   EMOTE_SHOT                = 5,
   EMOTE_TALK                = 1,
   EMOTE_POINT               = 25,
   EMOTE_NO                  = 274,
   EMOTE_LAUGH               = 11
};

struct MANGOS_DLL_DECL npc_mikeAI : public ScriptedAI
{
    npc_mikeAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiPhase;

    void Reset()
    {
       if (m_uiPhase != 2)
       {
          m_uiStep = 0;
          m_uiStepTimer = 100;
          m_uiPhase = 0;
       }
    }

    void MoveInLineOfSight(Unit* who)
    {
      if (!who)
          return;

      if (!m_pInstance) return;

           if (who->GetTypeId() == TYPEID_PLAYER && m_creature->GetDistance2d(who) <= 15 && who->GetPositionZ() > 99.50f && m_uiPhase == 0)
           {
             m_creature->SetGuidValue(UNIT_FIELD_TARGET, who->GetObjectGuid());
             m_uiPhase = 1;
           }
    }

    void TavernEvent()
    {
        switch(m_uiStep)
        {
           case 0:
              DoScriptText(SAY_MIKE01, m_creature);
              m_uiStepTimer = 4000;
              break;
           case 1:
              m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
              m_uiStepTimer = 5000;
              break;
           case 2:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_FORRESTER))
                 DoScriptText(SAY_FORRESTER02, pTemp);
              m_uiStepTimer = 6000;
              break;
           case 3:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JAMES))
                 DoScriptText(SAY_JAMES03, pTemp);
              m_uiStepTimer = 5000;
              break;
           case 4:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_FRAS_FRASIABI))
                 DoScriptText(SAY_SIABI04, pTemp);
              m_uiStepTimer = 2000;
              break;
           case 5:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_FRAS_FRASIABI))
                 pTemp->HandleEmoteCommand(EMOTE_SHOT);
              m_creature->SetStandState(UNIT_STAND_STATE_STAND);
              m_uiStepTimer = 5000;
              break;
           case 6:
              m_creature->GetMotionMaster()->MovePoint(0, 1554.849f, 588.465f, 99.775f);
              m_uiStepTimer = 3000;
              break;
           case 7:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_FRAS_FRASIABI))
                 pTemp->HandleEmoteCommand(EMOTE_LAUGH);
              m_uiStepTimer = 3000;
              break;
           case 8:
              DoScriptText(SAY_MIKE05, m_creature);
              m_uiStepTimer = 2000;
              break;
           case 9:
              m_creature->HandleEmoteCommand(EMOTE_SHOT);
              m_uiStepTimer = 1000;
              break;
           case 10:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MAL_CORICS))
                 DoScriptText(SAY_CORICKS06, pTemp);
              m_uiStepTimer = 4000;
              break;
           case 11:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MAL_CORICS))
                 pTemp->HandleEmoteCommand(EMOTE_TALK);
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_GRIAN_STONE))
                 DoScriptText(SAY_GRIAN07, pTemp);
              m_uiStepTimer = 11000;
              break;
           case 12:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MAL_CORICS))
                 DoScriptText(SAY_CORICKS08, pTemp);
              m_creature->GetMotionMaster()->MovePoint(0, 1549.609f, 575.544f, 100.052f);
              m_uiStepTimer = 2000;
              break;
           case 13:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JAMES))
                 DoScriptText(SAY_JAMES09, pTemp);
              m_uiStepTimer = 2000;
              break;
           case 14:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_JAMES))
                 pTemp->HandleEmoteCommand(EMOTE_TALK);
              m_uiStepTimer = 5000;
              break;
           case 15:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_FORRESTER))
                 DoScriptText(SAY_FORRESTER10, pTemp);
              m_uiPhase = 2;
              break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_uiPhase == 1)
       {
            if (m_uiStepTimer < uiDiff)
            {
               TavernEvent();
               m_uiStep++;
            }
            else m_uiStepTimer -= uiDiff;
       }

       return;
    }
};

/*###
## npc_roger
###*/

enum
{
  SAY_ROGER01                     = -1597280,
  SAY_ROGER02                     = -1597281,
  SAY_ROGER03                     = -1597282,
  SAY_ROGER04                     = -1597283,
};

struct MANGOS_DLL_DECL npc_rogerAI : public ScriptedAI
{
    npc_rogerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiPhase;

    void Reset()
    {
       if (m_uiPhase != 2)
       {
          m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
          m_uiStep = 0;
          m_uiStepTimer = 100;
          m_uiPhase = 0;
       }
    }

    void StartRoger()
    {
      m_uiPhase = 1;
    }

    void FirstCrateEvent()
    {
        switch(m_uiStep)
        {
           case 0:
              m_creature->SetStandState(UNIT_STAND_STATE_STAND);
              m_uiStepTimer = 7000;
              break;
           case 1:
              m_creature->GetMotionMaster()->MovePoint(0, 1590.055f, 615.727f, 99.795f);
              m_uiStepTimer = 6900;
              break;
           case 2:
              m_creature->GetMotionMaster()->MovePoint(0, 1584.039f, 622.049f, 99.907f);
              m_uiStepTimer = 3900;
              break;
           case 3:
              m_creature->GetMotionMaster()->MovePoint(0, 1578.787f, 623.924f, 99.855f);
              m_uiStepTimer = 2500;
              break;
           case 4:
              DoScriptText(SAY_ROGER01, m_creature);
              m_uiStepTimer = 6000;
              break;
           case 5:
              m_creature->GetMotionMaster()->MovePoint(0, 1579.393f, 624.018f, 99.886f);
              m_uiStepTimer = 2000;
              break;
           case 6:
              DoScriptText(SAY_ROGER02, m_creature);
              m_uiStepTimer = 6000;
              break;
           case 7:
              m_creature->GetMotionMaster()->MovePoint(0, 1579.387f, 623.198f, 99.837f);
              m_uiStepTimer = 1000;
              break;
           case 8:
              DoScriptText(SAY_ROGER03, m_creature);
              m_uiStepTimer = 4000;
              break;
           case 9:
              m_creature->GetMotionMaster()->MovePoint(0, 1575.576f, 619.935f, 99.422f);
              m_uiStepTimer = 2000;
              break;
           case 10:
              m_creature->GetMotionMaster()->MovePoint(0, 1575.833f, 620.471f, 99.466f);
              m_uiStepTimer = 1000;
              break;
           case 11:
              DoScriptText(SAY_ROGER04, m_creature);
              m_uiStepTimer = 6000;
              break;
           case 12:
              m_creature->GetMotionMaster()->MovePoint(0, 1580.215f, 624.368f, 99.924f);
              m_uiStepTimer = 1900;
              break;
           case 13:
              m_creature->GetMotionMaster()->MovePoint(0, 1587.471f, 618.181f, 99.850f);
              m_uiStepTimer = 3900;
              break;
           case 14:
              m_creature->GetMotionMaster()->MovePoint(0, 1592.646f, 590.888f, 99.151f);
              m_uiStepTimer = 13000;
              break;
           case 15:
              m_uiPhase = 2;
              m_creature->SetVisibility(VISIBILITY_OFF);
              break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_uiPhase == 1)
       {
            if (m_uiStepTimer < uiDiff)
            {
               FirstCrateEvent();
               m_uiStep++;
            }
            else m_uiStepTimer -= uiDiff;
       }

       return;
    }
};

/*###
## npc_morigan
###*/

enum
{
  SAY_MORIGAN01                     = -1597284,
  SAY_PERELLI02                     = -1597285,
  SAY_MORIGAN03                     = -1597286,
  SAY_PERELLI04                     = -1597287,
  SAY_MORIGAN05                     = -1597288,
  SAY_PERELLI06                     = -1597289,
  SAY_MORIGAN07                     = -1597290,
};

struct MANGOS_DLL_DECL npc_moriganAI : public ScriptedAI
{
    npc_moriganAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiPhase;

    void Reset()
    {
       if(m_uiPhase != 2)
       {
          m_uiStep = 0;
          m_uiStepTimer = 100;
          m_uiPhase = 0;
       }
    }

    void StartMorigan()
    {
      m_uiPhase = 1;
    }

    void SecondCrateEvent()
    {
        switch(m_uiStep)
        {
           case 0:
              DoScriptText(SAY_MORIGAN01, m_creature);
              m_uiStepTimer = 6000;
              break;
           case 1:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_PERELLI))
                 DoScriptText(SAY_PERELLI02, pTemp);
              m_uiStepTimer = 2000;
              break;
           case 2:
              m_creature->GetMotionMaster()->MovePoint(0, 1564.138f, 668.343f, 102.058f);
              m_uiStepTimer = 2000;
              break;
           case 3:
              m_creature->GetMotionMaster()->MovePoint(0, 1567.956f, 667.776f, 102.094f);
              m_uiStepTimer = 1500;
              break;
           case 4:
              m_creature->GetMotionMaster()->MovePoint(0, 1569.615f, 668.859f, 102.180f);
              m_uiStepTimer = 2000;
              break;
           case 5:
              m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
              m_uiStepTimer = 1000;
              break;
           case 6:
              DoScriptText(SAY_MORIGAN03, m_creature);
              m_uiStepTimer = 3000;
              break;
           case 7:
              m_creature->SetStandState(UNIT_STAND_STATE_STAND);
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_PERELLI))
              {
                 m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTemp->GetObjectGuid());
                 pTemp->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
              }
              m_uiStepTimer = 3000;
              break;
           case 8:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_PERELLI))
                 DoScriptText(SAY_PERELLI04, pTemp);
              m_uiStepTimer = 3000;
              break;
           case 9:
              DoScriptText(SAY_MORIGAN05, m_creature);
              m_uiStepTimer = 9000;
              break;
           case 10:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_PERELLI))
                 DoScriptText(SAY_PERELLI06, pTemp);
              m_uiStepTimer = 6000;
              break;
           case 11:
              DoScriptText(SAY_MORIGAN07, m_creature);
              m_uiStepTimer = 4000;
              break;
           case 12:
              m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
              if(Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_PERELLI))
                 pTemp->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
              m_creature->GetMotionMaster()->MovePoint(0, 1576.119f, 657.675f, 102.09f);
              m_uiStepTimer = 4900;
              break;
           case 13:
              m_creature->GetMotionMaster()->MovePoint(0, 1586.040f, 646.113f, 100.910f);
              m_uiStepTimer = 5900;
              break;
           case 14:
              m_creature->GetMotionMaster()->MovePoint(0, 1609.189f, 697.134f, 106.902f);
              m_uiStepTimer = 24900;
              break;
           case 15:
              m_uiPhase = 2;
              m_creature->SetVisibility(VISIBILITY_OFF);
              break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if(m_uiPhase == 1)
       {
            if(m_uiStepTimer < uiDiff)
            {
               SecondCrateEvent();
               m_uiStep++;
            }
            else m_uiStepTimer -= uiDiff;
       }

       return;
    }
};

/*###
## npc_jena
###*/

enum
{
  SAY_JENA01           = -1597291,
  SAY_MARTHA02         = -1597292,
  SAY_JENA03           = -1597293,
  SAY_JENA04           = -1597294,
  SAY_MARTHA05         = -1597295,

};

struct MANGOS_DLL_DECL npc_jenaAI : public ScriptedAI
{
    npc_jenaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiPhase;

    void Reset()
    {
       if(m_uiPhase != 2)
       {
          m_uiStep = 0;
          m_uiStepTimer = 100;
          m_uiPhase = 0;
       }
    }

    void StartJena()
    {
      m_uiPhase = 1;
    }

    void ThirdCrateEvent()
    {
        switch(m_uiStep)
        {
           case 0:
              m_creature->GetMotionMaster()->MovePoint(0, 1605.546f, 744.869f, 114.731f);
              m_uiStepTimer = 1900;
              break;
           case 1:
              m_creature->GetMotionMaster()->MovePoint(0, 1614.967f, 743.673f, 114.063f);
              m_uiStepTimer = 4900;
              break;
           case 2:
              m_creature->GetMotionMaster()->MovePoint(0, 1623.848f, 729.251f, 112.410f);
              m_uiStepTimer = 6900;
              break;
           case 3:
              m_creature->GetMotionMaster()->MovePoint(0, 1633.460f, 726.261f, 113.518f);
              m_uiStepTimer = 4000;
              break;
           case 4:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
                 m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTemp->GetObjectGuid());
              DoScriptText(SAY_JENA01, m_creature);
              m_uiStepTimer = 3000;
              break;
           case 5:
              if(Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
              {
                 pTemp->RemoveAurasDueToSpell(58925);
                 pTemp->GetMotionMaster()->MovePoint(0, 1635.918f, 724.357f, 113.561f);
              }
              m_uiStepTimer = 1000;
              break;
           case 6:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
              {
                 pTemp->GetMotionMaster()->MovePoint(0, 1636.721f, 725.88f, 113.561f);
                 pTemp->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
              }
              m_uiStepTimer = 1000;
              break;
           case 7:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
                 DoScriptText(SAY_MARTHA02, pTemp);
              m_uiStepTimer = 4000;
              break;
           case 8:
              DoScriptText(SAY_JENA03, m_creature);
              m_uiStepTimer = 3000;
              break;
           case 9:
              m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
              m_creature->GetMotionMaster()->MovePoint(0, 1629.278f, 727.894f, 112.636f);
              m_uiStepTimer = 1500;
              break;
           case 10:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
              {
                 pTemp->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                 pTemp->GetMap()->CreatureRelocation(pTemp, 1640.089f, 725.766f, 113.561f, 4.77f);
                 pTemp->GetMotionMaster()->MovePoint(0,1640.089f, 725.766f, 113.561f);
              }
              m_creature->GetMotionMaster()->MovePoint(0, 1629.452f, 729.416f, 112.712f);
              m_uiStepTimer = 1000;
              break;
           case 11:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
                 pTemp->GetMotionMaster()->MovePoint(0, 1640.103f, 725.522f, 113.561f);
              m_uiStepTimer = 500;
              break;
           case 12:
              if(Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
                 pTemp->CastSpell(pTemp, 58925, false);
              m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
              m_uiStepTimer = 1500;
              break;
           case 13:
              m_creature->SetStandState(UNIT_STAND_STATE_STAND);
              m_uiStepTimer = 1000;
              break;
           case 14:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
                 m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTemp->GetObjectGuid());
              DoScriptText(SAY_JENA04, m_creature);
              m_uiStepTimer = 3000;
              break;
           case 15:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_MARTHA))
              {
                 pTemp->RemoveAurasDueToSpell(58925);
                 DoScriptText(SAY_MARTHA05, pTemp);
                 pTemp->GetMotionMaster()->MovePoint(0, 1638.196f, 726.171f, 113.561f);
              }
              m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
              m_creature->GetMotionMaster()->MovePoint(0, 1615.590f, 719.509f, 110.311f);
              m_uiStepTimer = 2000;
              break;
           case 16:
              m_creature->GetMotionMaster()->MovePoint(0, 1596.436f, 670.809f, 103.747f);
              m_uiStepTimer = 6900;
              break;
           case 17:
              m_creature->GetMotionMaster()->MovePoint(0, 1571.549f, 609.837f, 99.767f);
              m_uiStepTimer = 11000;
              break;
           case 18:
              m_uiPhase = 2;
              m_creature->SetVisibility(VISIBILITY_OFF);
              break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_uiPhase == 1)
       {
            if (m_uiStepTimer < uiDiff)
            {
               ThirdCrateEvent();
               m_uiStep++;
            }
            else m_uiStepTimer -= uiDiff;
       }

       return;
    }
};

/*###
## npc_malcolm
###*/

enum
{
  SOUND_ID_DOG_GROWL    = 1108,
  SOUND_ID_DOG_HOWL     = 1018,
  EMOTE_DOG_HOWL        = 393,

  SAY_MALCOLM01         = -1597296,
  SAY_MALCOLM02         = -1597297,
  SAY_MALCOLM03         = -1597298,
  SAY_MALCOLM04         = -1597299,
};

struct MANGOS_DLL_DECL npc_malcolmAI : public ScriptedAI
{
    npc_malcolmAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiPhase;

    void Reset()
    {
       if (m_uiPhase != 2)
       {
          m_uiStep = 0;
          m_uiStepTimer = 100;
          m_uiPhase = 0;
       }
    }

    void StartMalcolm()
    {
      m_uiPhase = 1;
    }

    void FourCrateEvent()
    {
        switch(m_uiStep)
        {
           case 0:
              m_creature->GetMotionMaster()->MovePoint(0, 1614.066f, 796.722f, 121.739f);
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 pTemp->GetMotionMaster()->MovePoint(0, 1611.459f, 793.274f, 121.928f);
              m_uiStepTimer = 5400;
              break;
           case 1:
              DoScriptText(SAY_MALCOLM01, m_creature);
              m_creature->GetMotionMaster()->MovePoint(0, 1622.820f, 798.816f, 120.570f);
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 pTemp->GetMotionMaster()->MovePoint(0, 1621.467f, 794.323f, 120.323f);
              m_uiStepTimer = 3400;
              break;
           case 2:
              m_creature->GetMotionMaster()->MovePoint(0, 1626.574f, 806.781f, 120.270f);
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 pTemp->GetMotionMaster()->MovePoint(0, 1629.232f, 803.629f, 120.011f);
              m_uiStepTimer = 3400;
              break;
           case 3:
              m_creature->GetMotionMaster()->MovePoint(0, 1622.782f, 808.533f, 121.249f);
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 pTemp->GetMotionMaster()->MovePoint(0, 1629.265f, 805.245f, 120.070f);
              m_uiStepTimer = 300;
              break;
           case 4:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 pTemp->PlayDirectSound(SOUND_ID_DOG_GROWL);
              m_uiStepTimer = 500;
              break;
           case 5:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTemp->GetObjectGuid());
              DoScriptText(SAY_MALCOLM02, m_creature);
              m_uiStepTimer = 2000;
              break;
           case 6:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 pTemp->GetMotionMaster()->MovePoint(0, 1629.163f, 809.738f, 120.369f);
              m_uiStepTimer = 2000;
              break;
           case 7:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
              {
                 pTemp->HandleEmoteCommand(EMOTE_DOG_HOWL);
                 pTemp->PlayDirectSound(SOUND_ID_DOG_HOWL);
              }
              m_uiStepTimer = 4000;
              break;
           case 8:
              m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
              m_creature->GetMotionMaster()->MovePoint(0, 1629.922f, 807.799f, 120.122f);
              m_uiStepTimer = 2900;
              break;
           case 9:
              m_creature->GetMotionMaster()->MovePoint(0, 1632.169f, 809.851f, 120.047f);
              m_uiStepTimer = 900;
              break;
           case 10:
              m_creature->GetMotionMaster()->MovePoint(0, 1630.651f, 811.149f, 120.307f);
              m_uiStepTimer = 800;
              break;
           case 11:
              m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
              DoScriptText(SAY_MALCOLM03, m_creature);
              m_uiStepTimer = 4000;
              break;
           case 12:
              m_creature->SetStandState(UNIT_STAND_STATE_STAND);
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTemp->GetObjectGuid());
              DoScriptText(SAY_MALCOLM04, m_creature);
              m_uiStepTimer = 7000;
              break;
           case 13:
              if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                 pTemp->GetMotionMaster()->MovePoint(0, 1630.692f, 808.011f, 120.083f);
              m_uiStepTimer = 600;
              break;
           case 14:
             if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_DOG))
                pTemp->SetStandState(UNIT_STAND_STATE_SIT);
              m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
              m_creature->GetMotionMaster()->MovePoint(0, 1641.452f, 812.600f, 119.948f);
              m_uiStepTimer = 3900;
              break;
           case 15:
              m_creature->GetMotionMaster()->MovePoint(0, 1657.975f, 857.352f, 119.097f);
              m_uiStepTimer = 17900;
              break;
           case 16:
              m_creature->GetMotionMaster()->MovePoint(0, 1679.852f, 912.245f, 120.533f);
              m_uiStepTimer = 22900;
              break;
           case 17:
              m_creature->GetMotionMaster()->MovePoint(0, 1699.915f, 967.110f, 121.643f);
              m_uiStepTimer = 22900;
              break;
           case 18:
              m_creature->GetMotionMaster()->MovePoint(0, 1678.393f, 1026.890f, 125.431f);
              m_uiStepTimer = 24900;
              break;
           case 19:
              m_creature->GetMotionMaster()->MovePoint(0, 1678.943f, 1093.130f, 126.899f);
              m_uiStepTimer = 25900;
              break;
           case 20:
              m_creature->GetMotionMaster()->MovePoint(0, 1700.042f, 1103.880f, 130.872f);
              m_uiStepTimer = 10900;
              break;
           case 21:
              m_uiPhase = 2;
              m_creature->SetVisibility(VISIBILITY_OFF);
              break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_uiPhase == 1)
       {
            if (m_uiStepTimer < uiDiff)
            {
               FourCrateEvent();
               m_uiStep++;
            }
            else m_uiStepTimer -= uiDiff;
       }

       return;
    }
};

/*###
## npc_bartleby
###*/

enum
{
  SAY_BARTLEBY01         = -1597300,
  SAY_BARTLEBY02         = -1597301,
  SAY_BARTLEBY03         = -1597302,
  SAY_BARTLEBY04         = -1597303,
  SAY_BARTLEBY05         = -1597304,
};

struct MANGOS_DLL_DECL npc_bartleby_csAI : public ScriptedAI
{
    npc_bartleby_csAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiPhase;

    void Reset()
    {
       if(m_uiPhase != 4)
       {
          m_uiStep = 0;
          m_uiStepTimer = 100;
          m_uiPhase = 0;
       }
    }

    void MoveInLineOfSight(Unit* who)
    {
      if (!who)
          return;

      if (!m_pInstance) return;

           if (who->GetTypeId() == TYPEID_PLAYER && m_creature->GetDistance2d(who) <= 20 && m_uiPhase == 0)
           {
             m_uiPhase = 1;
           }
    }

    void StartBartleby()
    {
      m_uiPhase = 3;
    }

    void FifthCrateEvent()
    {
        switch(m_uiStep)
        {
           case 0:
              DoScriptText(SAY_BARTLEBY03, m_creature);
              m_creature->GetMotionMaster()->MovePoint(0, 1672.539f, 872.277f, 120.113f);
              m_uiStepTimer = 1000;
              break;
           case 1:
              m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
              m_uiStepTimer = 3000;
              break;
           case 2:
              DoScriptText(SAY_BARTLEBY04, m_creature);
              m_uiStepTimer = 7000;
              break;
           case 3:
              m_creature->SetStandState(UNIT_STAND_STATE_STAND);
              m_uiStepTimer = 1000;
              break;
           case 4:
              DoScriptText(SAY_BARTLEBY05, m_creature);
              m_creature->GetMotionMaster()->MovePoint(0, 1663.054f, 869.959f, 119.734f);
              m_uiStepTimer = 2900;
              break;
           case 5:
              m_creature->GetMotionMaster()->MovePoint(0, 1640.732f, 812.422f, 119.933f);
              m_uiStepTimer = 23900;
              break;
           case 6:
              m_creature->GetMotionMaster()->MovePoint(0, 1623.704f, 755.741f, 115.710f);
              m_uiStepTimer = 22900;
              break;
           case 7:
              m_creature->GetMotionMaster()->MovePoint(0, 1607.108f, 699.637f, 106.971f);
              m_uiStepTimer = 22900;
              break;
           case 8:
              m_creature->GetMotionMaster()->MovePoint(0, 1587.750f, 646.929f, 100.990f);
              m_uiStepTimer = 20900;
              break;
           case 9:
              m_creature->GetMotionMaster()->MovePoint(0, 1571.103f, 660.949f, 102.084f);
              m_uiStepTimer = 10900;
              break;
           case 10:
              m_uiPhase = 4;
              m_creature->SetVisibility(VISIBILITY_OFF);
              break;
        }
    }

    void SpeechEvent()
    {
        switch(m_uiStep)
        {
           case 0:
              DoScriptText(SAY_BARTLEBY01, m_creature);
              m_uiStepTimer = 5000;
              break;
           case 1:
              DoScriptText(SAY_BARTLEBY02, m_creature);
              if (m_uiPhase == 1)
                 m_uiPhase = 2;
              break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (m_uiPhase == 3)
       {
            if (m_uiStepTimer < uiDiff)
            {
               FifthCrateEvent();
               m_uiStep++;
            }
            else m_uiStepTimer -= uiDiff;
       }

       if (m_uiPhase == 1)
       {
            if(m_uiStepTimer < uiDiff)
            {
               SpeechEvent();
               m_uiStep++;
            }
            else m_uiStepTimer -= uiDiff;
       }

       return;
    }
};

/*###
## npc_crates
###*/

enum
{
    SPELL_LIGHT            = 49590
};

struct MANGOS_DLL_DECL npc_stratholme_cratesAI : public ScriptedAI
{
    npc_stratholme_cratesAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool Active;

    void Reset()
    {
        Active = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
       if (!m_pInstance) return;

       if (m_creature->HasAura(SPELL_LIGHT) && Active != true)
       {
            if (Creature* pRoger = m_pInstance->GetSingleCreatureFromStorage(NPC_ROGER))
            {
               if (m_creature->GetDistance2d(pRoger->GetPositionX(), pRoger->GetPositionY()) < 50.0f)
               {
                   ((npc_rogerAI*)pRoger->AI())->StartRoger();
               }
            }

            if (Creature* pMorigan = m_pInstance->GetSingleCreatureFromStorage(NPC_MORIGAN))
            {
               if(m_creature->GetDistance2d(pMorigan->GetPositionX(), pMorigan->GetPositionY()) < 50.0f)
               {
                  ((npc_moriganAI*)pMorigan->AI())->StartMorigan();
               }
            }

            if (Creature* pJena = m_pInstance->GetSingleCreatureFromStorage(NPC_JENA))
            {
               if (m_creature->GetDistance2d(pJena->GetPositionX(), pJena->GetPositionY()) < 50.0f)
               {
                   ((npc_jenaAI*)pJena->AI())->StartJena();
               }
            }

            if (Creature* pMalcolm = m_pInstance->GetSingleCreatureFromStorage(NPC_MALCOLM))
            {
               if (m_creature->GetDistance2d(pMalcolm->GetPositionX(), pMalcolm->GetPositionY()) < 50.0f)
               {
                   ((npc_malcolmAI*)pMalcolm->AI())->StartMalcolm();
               }
            }

            if (Creature* pBartleby = m_pInstance->GetSingleCreatureFromStorage(NPC_BARTLEBY))
            {
               if (m_creature->GetDistance2d(pBartleby->GetPositionX(), pBartleby->GetPositionY()) < 50.0f)
               {
                   ((npc_bartleby_csAI*)pBartleby->AI())->StartBartleby();
               }
            }

            m_pInstance->SetData(TYPE_CRATES_COUNT, 1);
            if (GameObject* pLight = GetClosestGameObjectWithEntry(m_creature, GO_CRATE_LIGHT, 5.0f))
               pLight->SetPhaseMask(0, true);
            //m_creature->SetPhaseMask(0, true);
            Active = true;
       }

       return;
    }
};

CreatureAI* GetAI_npc_chromi_start(Creature* pCreature)
{
    return new npc_chromi_startAI(pCreature);
}

CreatureAI* GetAI_npc_mike(Creature* pCreature)
{
    return new npc_mikeAI(pCreature);
}

CreatureAI* GetAI_npc_roger(Creature* pCreature)
{
    return new npc_rogerAI(pCreature);
}

CreatureAI* GetAI_npc_morigan(Creature* pCreature)
{
    return new npc_moriganAI(pCreature);
}

CreatureAI* GetAI_npc_jena(Creature* pCreature)
{
    return new npc_jenaAI(pCreature);
}

CreatureAI* GetAI_npc_malcolm(Creature* pCreature)
{
    return new npc_malcolmAI(pCreature);
}

CreatureAI* GetAI_npc_bartleby_cs(Creature* pCreature)
{
    return new npc_bartleby_csAI(pCreature);
}

CreatureAI* GetAI_npc_stratholme_crates(Creature* pCreature)
{
    return new npc_stratholme_cratesAI(pCreature);
}

void AddSC_culling_of_stratholmeAI()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_chromi_start";
    pNewScript->GetAI = &GetAI_npc_chromi_start;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mike";
    pNewScript->GetAI = &GetAI_npc_mike;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_roger";
    pNewScript->GetAI = &GetAI_npc_roger;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_morigan";
    pNewScript->GetAI = &GetAI_npc_morigan;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_jena";
    pNewScript->GetAI = &GetAI_npc_jena;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_malcolm";
    pNewScript->GetAI = &GetAI_npc_malcolm;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_bartleby_cs";
    pNewScript->GetAI = &GetAI_npc_bartleby_cs;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_stratholme_crates";
    pNewScript->GetAI = &GetAI_npc_stratholme_crates;
    pNewScript->RegisterSelf();
}
