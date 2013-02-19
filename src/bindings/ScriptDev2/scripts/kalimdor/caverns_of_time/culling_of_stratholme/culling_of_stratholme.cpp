/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
#include "Weather.h"

/*###
## npc_arthas
###*/

enum
{
    SAY_INTRO01                           = -1594071,    //Arthas
    SAY_INTRO02                           = -1594072,    //Uther
    SAY_INTRO03                           = -1594073,    //Arthas
    SAY_INTRO04                           = -1594074,    //Arthas
    SAY_INTRO05                           = -1594075,    //Uther
    SAY_INTRO06                           = -1594076,    //Arthas
    SAY_INTRO07                           = -1594077,    //Uther
    SAY_INTRO08                           = -1594078,    //Arthas
    SAY_INTRO09                           = -1594079,    //Arthas
    SAY_INTRO10                           = -1594080,    //Uther
    SAY_INTRO11                           = -1594081,    //Arthas
    SAY_INTRO12                           = -1594082,    //Uther
    SAY_INTRO13                           = -1594083,    //Jaina
    SAY_INTRO14                           = -1594084,    //Arthas
    SAY_INTRO15                           = -1594085,    //Uther
    SAY_INTRO16                           = -1594086,    //Arthas
    SAY_INTRO17                           = -1594087,    //Jaina
    SAY_INTRO18                           = -1594088,    //Arthas

    SAY_ENTER01                           = -1594089,    //Arthas
    SAY_ENTER02                           = -1594090,    //Cityman
    SAY_ENTER03                           = -1594091,    //Arthas
    SAY_ENTER04                           = -1594092,    //Crazyman
    SAY_ENTER05                           = -1594093,    //Crazyman2
    SAY_ENTER06                           = -1594094,    //Arthas
    SAY_ENTER07                           = -1594095,    //Malganis
    SAY_ENTER08                           = -1594096,    //Malganis
    SAY_ENTER09                           = -1594097,    //Arthas
    SAY_ENTER10                           = -1594098,    //Arthas

    SAY_SALRAMM_SPAWN                     = -1594129,
    SAY_MEATHOOK_SPAWN                    = -1594110,

    SAY_PHASE501                          = -1594142,    //Arthas
    SAY_PHASE502                          = -1594143,    //Arthas
    SAY_PHASE503                          = -1594144,    //Human
    SAY_PHASE504                          = -1594145,    //Arthas
    SAY_PHASE505                          = -1594146,    //Arthas
    SAY_PHASE506                          = -1594147,    //Human
    SAY_PHASE507                          = -1594148,    //Arthas
    SAY_PHASE508                          = -1594149,    //Arthas
    SAY_PHASE509                          = -1594150,    //Arthas
    SAY_PHASE510                          = -1594151,    //Arthas
    SAY_PHASE511                          = -1594152,    //Arthas
    SAY_PHASE512                          = -1594153,    //Arthas
    SAY_PHASE513                          = -1594154,    //Arthas

    SAY_EPOCH_INTRO                       = -1594155,
    SAY_ARTHAS_INTRO                      = -1594156,
    SAY_EPOCH_AGGRO                       = -1594157,

    SAY_PHASE514                        = -1594158,   //Arthas Shkaf 01
    SAY_PHASE515                        = -1594159,   //Arthas Shkaf 02
    SAY_PHASE601                        = -1594160,  //Arthas Fire
    SAY_PHASE602                        = -1594161,  //Arthas Picnic
    SAY_PHASE603                        = -1594162,  //Arthas Picnic End
    SAY_PHASE605                        = -1594164,  //Arthas mall start
    SAY_PHASE606                        = -1594188,

    SAY_MALGANIS_ESCAPE02    = -1594180,
    SAY_MALGANIS_ESCAPE01    = -1594187,
    SAY_MALGANIS_OUTRO       = -1594182,
    SAY_ARTHAS_OUTRO01       = -1594181,
    SAY_ARTHAS_OUTRO02       = -1594183,
    SAY_ARTHAS_OUTRO03       = -1594184,

    /*SPELL*/
    SPELL_EXORCISM_N          = 52445,
    SPELL_EXORCISM_H          = 58822,
    SPELL_HOLY_LIGHT          = 52444,
    SPELL_ARTHAS_AURA         = 52442,

///-> marines
    SPELL_HEROIC_STRIKE  = 25710,
    SPELL_DEFEND         = 52317,
///->priest
    SPELL_SMITE         = 61923,
    SPELL_HEAL          = 62442,

    /*OTHER*/
    POINT_LAST_POINT    = 0xFFFFFF,
    FACTION             = 2076
};

#define Last1X 2083.483f
#define Last1Y 1282.313f
#define Last1Z 141.198f

#define Last2X 2083.681f
#define Last2Y 1292.809f
#define Last2Z 141.141f

#define Last3X 2082.158f
#define Last3Y 1290.406f
#define Last3Z 141.261f

#define Last4X 2081.899f
#define Last4Y 1285.122f
#define Last4Z 141.302f

    float Last5X;
    float Last5Y;
    float Last5Z;

const float SummonScourge[2][4] =
{
    {2351.68f, 1215.54f, 130.20f, 4.73f}, //right wing
    {2221.47f, 1331.13f, 127.68f, 3.11f}, //left wing
};

struct MANGOS_DLL_DECL npc_arthasAI : public npc_escortAI
{
    npc_arthasAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsHeroic = pCreature->GetMap()->IsRaidOrHeroicDungeon();
        m_creature->SetActiveObjectState(true);
        m_creature->SetSpeedRate(MOVE_RUN, 1);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsHeroic;

    ObjectGuid m_uiMalganisGUID;
    ObjectGuid m_uiHuman01GUID;
    ObjectGuid m_uiHuman02GUID;
    ObjectGuid m_uiHuman03GUID;
    ObjectGuid m_uiPeople01GUID;
    ObjectGuid m_uiPeople02GUID;

    uint32 culling_faction;
    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    uint32 m_uiHealTimer;
    uint32 m_uiExorcismTimer;
    uint32 m_uiSummonTimer;
    uint32 m_uiWaveCount;

    Creature* Malganis;
    Creature* pEpoch;
    bool StartEvent;

    void DamageTaken(Unit* pDealer, uint32& uiDamage)
    {
        if (uiDamage >= m_creature->GetHealth())
            uiDamage = 0;
    }

    void Reset()
    {
        if (!m_pInstance) return;

        m_creature->SetSpeedRate(MOVE_RUN, 1);

        if (m_pInstance->GetData(TYPE_INTRO) == NOT_STARTED)
        {
            m_creature->setFaction(35);
            RemoveGossip();
        }

        if (m_pInstance->GetData(TYPE_PHASE) == 11)
        {
            m_creature->SetVisibility(VISIBILITY_OFF);
        }
    }

    void RemoveGossip()
    {
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void MoveSoldiers()
    {
         if (Creature* pMarine01 = m_pInstance->GetSingleCreatureFromStorage(NPC_MARINE_1))
         {
            pMarine01->GetMotionMaster()->MovePoint(0, 2083.483f,1282.313f,141.198f);
            pMarine01->setFaction(culling_faction);
         }
         if (Creature* pMarine02 = m_pInstance->GetSingleCreatureFromStorage(NPC_MARINE_2))
         {
            pMarine02->GetMotionMaster()->MovePoint(0, 2083.681f,1292.809f,141.141f);
            pMarine02->setFaction(culling_faction);
         }
         if (Creature* pMarine03 = m_pInstance->GetSingleCreatureFromStorage(NPC_MARINE_3))
         {
            pMarine03->GetMotionMaster()->MovePoint(0, 2082.158f,1290.406f,141.261f);
            pMarine03->setFaction(culling_faction);
         }
         if (Creature* pMarine04 = m_pInstance->GetSingleCreatureFromStorage(NPC_MARINE_4))
         {
            pMarine04->GetMotionMaster()->MovePoint(0, 2081.899f,1285.122f,141.302f);
            pMarine04->setFaction(culling_faction);
         }
         if (Creature* pPriest01 = m_pInstance->GetSingleCreatureFromStorage(NPC_PRIEST_1))
         {
            pPriest01->GetMotionMaster()->MovePoint(0, 2081.072f,1292.233f,141.329f);
            pPriest01->setFaction(culling_faction);
         }
         if (Creature* pPriest02 = m_pInstance->GetSingleCreatureFromStorage(NPC_PRIEST_2))
         {
            pPriest02->GetMotionMaster()->MovePoint(0, 2080.632f,1283.004f,141.358f);
            pPriest02->setFaction(culling_faction);
         }
    }

    void EnableEscort()
    {
        SetEscortPaused(false);
    }

    void SummonPeople()
    {
        if (Creature* pCityman = m_creature->SummonCreature(NPC_CITY,2091.977f,1275.021f,140.757f,0.558f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000))
            m_uiPeople01GUID = pCityman->GetObjectGuid();
        if (Creature* pCrazyman = m_creature->SummonCreature(NPC_CRAZYMAN,2093.514f,1275.842f,140.408f,3.801f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000))
            m_uiPeople02GUID = pCrazyman->GetObjectGuid();
    }

    void StartAI()
    {
         SummonPeople();
         m_uiStep = 0;
         m_uiStepTimer = 100;
         StartEvent = true;
    }

    void Aggro(Unit* who)
    {
        DoCast(m_creature, SPELL_ARTHAS_AURA);
    }

    void EnterEvadeMode()
    {
        if (!m_pInstance)
            return;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(false);
        m_uiExorcismTimer = 7400;
        m_uiHealTimer = 100;

        m_creature->SetLootRecipient(NULL);

        if (m_pInstance->GetData(TYPE_PHASE) > 4)
        {
            npc_escortAI::EnterEvadeMode();
        }

        if (m_pInstance->GetData(TYPE_PHASE) > 2 && m_pInstance->GetData(TYPE_PHASE) < 5)
        {
            m_creature->GetMotionMaster()->MovePoint(POINT_LAST_POINT, Last5X, Last5Y, Last5Z);
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || pWho == m_creature)
            return;

        if (m_pInstance && m_pInstance->GetData(TYPE_PHASE) == 4) return;

        npc_escortAI::AttackStart(pWho);
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

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 2:
                DoScriptText(SAY_INTRO18, m_creature);
                SetRun(true);
                break;
            case 8:
                SetEscortPaused(true);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                m_pInstance->SetData(TYPE_INTRO, DONE);
                SetRun(false);
                break;
            case 9:
                DoScriptText(SAY_ENTER01, m_creature);
                break;
            case 10:
                SetEscortPaused(true);
                MoveSoldiers();
                m_pInstance->SetData(TYPE_PHASE, 2);
                ResetStep(2000);
                if (Unit* pCityman = m_pInstance->GetSingleCreatureFromStorage(NPC_CITY))
                {
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pCityman->GetObjectGuid());
                    pCityman->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                    pCityman->GetMotionMaster()->MovePoint(0, 2088.625f,1279.191f,140.743f);
                }
                break;
            case 14:
                if (Creature* pHuman01 = m_creature->SummonCreature(NPC_CITY,2397.308f,1207.565f,134.038f,5.593f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000))
                    m_uiHuman01GUID = pHuman01->GetObjectGuid();
                if (Creature* pHuman02 = m_creature->SummonCreature(NPC_CITY,2400.770f,1207.362f,134.038f,3.454f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000))
                    m_uiHuman02GUID = pHuman02->GetObjectGuid();
                if (Creature* pHuman03 = m_creature->SummonCreature(NPC_CITY,2400.547f,1204.892f,134.038f,2.479f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,30000))
                    m_uiHuman03GUID = pHuman03->GetObjectGuid();
                break;
            case 20:
                SetEscortPaused(true);
                m_creature->setFaction(35);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetRun(false);
                break;
            case 21:
                DoScriptText(SAY_PHASE502, m_creature);
                break;
            case 22:
                SetEscortPaused(true);
                m_pInstance->SetData(TYPE_PHASE, 6);
                ResetStep(1000);
                break;
            case 25:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_READY1H);
                m_creature->SummonCreature(NPC_TIME_RIFT,2428.901f, 1192.164f, 148.076f, 5.09f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
                DoScriptText(SAY_PHASE508, m_creature);
                break;
            case 26:
                m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STAND);
                DoScriptText(SAY_PHASE509, m_creature);
                break;
            case 29:
                m_creature->SummonCreature(NPC_TIME_RIFT,2413.773f, 1137.820f, 148.076f, 5.09f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
                m_creature->SummonCreature(NPC_TIME_RIFT,2404.990f, 1175.121f, 148.076f, 5.09f,TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
                DoScriptText(SAY_PHASE510, m_creature);
                break;
            case 30:
                DoScriptText(SAY_PHASE513, m_creature);
                break;
            case 31:
                ResetStep(1000);
                m_pInstance->SetData(TYPE_PHASE, 7);
                break;
            case 32:
                SetEscortPaused(true);
                m_pInstance->SetData(TYPE_PHASE, 8);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                SetRun(false);
                break;
            case 36:
                DoScriptText(SAY_PHASE514, m_creature);
                break;
            case 37:
                if (GameObject* pGate = m_pInstance->GetSingleGameObjectFromStorage(GO_SHKAF_GATE))
                    pGate->SetGoState(GO_STATE_ACTIVE);
                SetRun(true);
                DoScriptText(SAY_PHASE515, m_creature);
                break;
            case 45:
                DoScriptText(SAY_PHASE601, m_creature);
                break;
            case 48:
                DoScriptText(SAY_PHASE602, m_creature);
                break;
            case 51:
                SetEscortPaused(true);
                m_pInstance->SetData(TYPE_PHASE, 9);
                DoScriptText(SAY_PHASE606, m_creature);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                break;
            case 53:
                SetEscortPaused(true);
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->MovementExpired(false);
                m_creature->setFaction(FACTION);
                DoScriptText(SAY_PHASE605, m_creature);
                if (Creature* pMalganis = m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS))
                {
                    m_pInstance->SetData(TYPE_MALGANIS, IN_PROGRESS);
                    pMalganis->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->AI()->AttackStart(pMalganis);
                    pMalganis->AI()->AttackStart(m_creature);
                }
                break;
        }
    }

    void JumpNextStep(uint32 Timer)
    {
        m_uiStepTimer = Timer;
        m_uiStep++;
    }

    void ResetStep(uint32 Timer)
    {
       m_uiStep = 0;
       m_uiStepTimer = Timer;
    }

    void IntroEvent()
    {
        switch(m_uiStep)
        {
            case 0:
                DoScriptText(SAY_INTRO01, m_creature);
                JumpNextStep(2000);
                break;
            case 1:
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                DoScriptText(SAY_INTRO02, pUther);
                JumpNextStep(8000);
                break;
            case 2:
                m_creature->SetWalk(true);
                DoScriptText(SAY_INTRO03, m_creature);
                m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                m_creature->GetMotionMaster()->MovePoint(0, 1908.334f, 1315.354f, 149.551f);
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                    pUther->GetMotionMaster()->MovePoint(0, 1903.600f, 1296.678f, 143.383f);
                JumpNextStep(2000);
                break;
            case 3:
                if (Creature* pJaina = m_pInstance->GetSingleCreatureFromStorage(NPC_JAINA))
                    pJaina->GetMotionMaster()->MovePoint(0, 1899.641f, 1298.684f, 143.831f);
                JumpNextStep(7000);
                break;
            case 4:
                m_creature->GetMotionMaster()->MovementExpired(false);
                m_creature->GetMotionMaster()->MovePoint(0, 1911.087f, 1314.263f, 150.026f);
                JumpNextStep(1000);
                break;
            case 5:
                DoScriptText(SAY_INTRO04, m_creature);
                JumpNextStep(10000);
                break;
            case 6:
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                    DoScriptText(SAY_INTRO05, pUther);
                JumpNextStep(1000);
                break;
            case 7:
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pUther->GetObjectGuid());
                DoScriptText(SAY_INTRO06, m_creature);
                JumpNextStep(4000);
                break;
            case 8:
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                    DoScriptText(SAY_INTRO07, pUther);
                JumpNextStep(6000);
                break;
            case 9:
                DoScriptText(SAY_INTRO08, m_creature);
                JumpNextStep(4000);
                break;
            case 10:
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                    DoScriptText(SAY_INTRO09, pUther);
                JumpNextStep(8000);
                break;
            case 11:
                DoScriptText(SAY_INTRO10, m_creature);
                JumpNextStep(4000);
                break;
            case 12:
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                    DoScriptText(SAY_INTRO11, pUther);
                JumpNextStep(4000);
                break;
            case 13:
               DoScriptText(SAY_INTRO12, m_creature);
               JumpNextStep(11000);
               break;
            case 14:
                if (Creature* pJaina = m_pInstance->GetSingleCreatureFromStorage(NPC_JAINA))
                    DoScriptText(SAY_INTRO13, pJaina);
                JumpNextStep(3000);
                break;
            case 15:
                DoScriptText(SAY_INTRO14, m_creature);
                JumpNextStep(9000);
                break;
            case 16:
                if (Creature* pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                    DoScriptText(SAY_INTRO15, pUther);
                JumpNextStep(5000);
                break;
            case 17:
                if (Creature* pJaina = m_pInstance->GetSingleCreatureFromStorage(NPC_JAINA))
                {
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pJaina->GetObjectGuid());
                    pJaina->GetMotionMaster()->MovePoint(0, 1794.357f,1272.183f,140.558f);
                 }
                 JumpNextStep(1000);
                 break;
            case 18:
                DoScriptText(SAY_INTRO16, m_creature);
                JumpNextStep(1000);
                break;
            case 19:
                if (Creature* pJaina = m_pInstance->GetSingleCreatureFromStorage(NPC_JAINA))
                    DoScriptText(SAY_INTRO17, pJaina);
                JumpNextStep(3000);
                break;
            case 20:
                m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                ((npc_arthasAI*)m_creature->AI())->Start(false);
                JumpNextStep(3000);
                break;
        }
    }

    void EnterEvent()
    {
        switch(m_uiStep)
        {
            case 0:
                if (Unit* pCityman = m_pInstance->GetSingleCreatureFromStorage(NPC_CITY))
                DoScriptText(SAY_ENTER02, pCityman);
                JumpNextStep(4000);
                break;
            case 1:
                m_creature->GetMotionMaster()->MovePoint(0, 2087.689f,1280.344f,140.73f);
                DoScriptText(SAY_ENTER03, m_creature);
                JumpNextStep(3000);
                break;
            case 2:
                if (Unit* pCityman = m_pInstance->GetSingleCreatureFromStorage(NPC_CITY))
                   DoScriptText(SAY_ENTER04, pCityman);
                m_creature->HandleEmoteCommand(37);
                JumpNextStep(1000);
                break;
            case 3:
                if (Unit* pCityman = m_pInstance->GetSingleCreatureFromStorage(NPC_CITY))
                    m_creature->DealDamage(pCityman, pCityman->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                if (Unit* pCrazyman = m_pInstance->GetSingleCreatureFromStorage(NPC_CRAZYMAN))
                {
                    DoScriptText(SAY_ENTER05, pCrazyman);
                    pCrazyman->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pCrazyman->GetObjectGuid());
                    m_creature->GetMotionMaster()->MovePoint(0, 2092.154f,1276.645f,140.52f);
                }
                JumpNextStep(3000);
                break;
            case 4:
                m_creature->HandleEmoteCommand(37);
                JumpNextStep(1000);
                break;
            case 5:
                if (Unit* pCrazyman = m_pInstance->GetSingleCreatureFromStorage(NPC_CRAZYMAN))
                   pCrazyman->DealDamage(pCrazyman, pCrazyman->GetMaxHealth(),NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                JumpNextStep(1000);
                break;
            case 6:
                m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                m_creature->GetMotionMaster()->MovePoint(0, 2091.179f,1278.065f,140.476f);
                DoScriptText(SAY_ENTER06, m_creature);
                JumpNextStep(3000);
                break;
            case 7:
                if (Creature* pStalkerM = m_creature->SummonCreature(NPC_INVIS_STALKER,2117.349f,1288.624f,136.271f,1.37f,TEMPSUMMON_TIMED_DESPAWN,60000))
                  pStalkerM->CastSpell(pStalkerM,63793,false);
                JumpNextStep(1000);
               break;
            case 8:
                m_pInstance->SetData(TYPE_ENCOUNTER, IN_PROGRESS);
                if (Creature* pTempMalganis = m_creature->SummonCreature(NPC_MALGANIS,2117.349f,1288.624f,136.271f,1.37f,TEMPSUMMON_TIMED_DESPAWN,29000))
                {
                    m_uiMalganisGUID = pTempMalganis->GetObjectGuid();
                    DoScriptText(SAY_ENTER07, pTempMalganis);
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTempMalganis->GetObjectGuid());
                    pTempMalganis->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                    pTempMalganis->setFaction(35);
                }
                JumpNextStep(11000);
                break;
            case 9:
                if (Unit* pTempMalganis = m_creature->GetMap()->GetUnit(m_uiMalganisGUID))
                    DoScriptText(SAY_ENTER08, pTempMalganis);
                JumpNextStep(17000);
                break;
            case 10:
                DoScriptText(SAY_ENTER09, m_creature);
                JumpNextStep(7000);
                break;
            case 11:
                m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                DoScriptText(SAY_ENTER10, m_creature);
                JumpNextStep(12000);
                break;
            case 12:
                m_creature->GetMotionMaster()->MovePoint(0, 2084.584f,1278.331f,141.479f);
                JumpNextStep(4000);
                break;
            case 13:
                m_creature->GetMotionMaster()->MovePoint(0, 2087.414f,1279.293f,140.933f);
                JumpNextStep(2000);
                break;
            case 14:
                Last5X = m_creature->GetPositionX();
                Last5Y = m_creature->GetPositionY();
                Last5Z = m_creature->GetPositionZ();
                if (m_bIsHeroic)
                   m_pInstance->SetData(TYPE_BONUS, IN_PROGRESS);
                m_uiWaveCount = 0;
                SetRun(true);
                m_pInstance->SetData(TYPE_WING, RIGHT);
                m_creature->setFaction(FACTION);
                m_uiSummonTimer = 100;
                m_pInstance->SetData(TYPE_PHASE, 3);
                break;
        }
    }

    void SummonWing()
    {
        m_uiWaveCount++;
        m_pInstance->DoUpdateWorldState(WORLD_STATE_COS_WAVE_COUNT, m_uiWaveCount);

        switch(m_uiWaveCount)
        {
            case 1:
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 2:
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_FIEND,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 3:
               m_pInstance->SetData(TYPE_WING, LEFT);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_FIEND,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 4:
               m_pInstance->SetData(TYPE_WING, RIGHT);
               m_creature->SummonCreature(NPC_ACOLYTE,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_ACOLYTE,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_FIEND,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 5:
               m_pInstance->SetData(TYPE_PHASE, 4);
               if (Creature* pMeathook = m_creature->SummonCreature(NPC_MEATHOOK, SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3], TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000))
               {
                   DoScriptText(SAY_MEATHOOK_SPAWN, pMeathook);
               }
               break;
            case 6:
               m_pInstance->SetData(TYPE_WING, LEFT);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_FIEND,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_FIEND,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 7:
               m_pInstance->SetData(TYPE_WING, RIGHT);
               m_creature->SummonCreature(NPC_CONSTRUCT,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 8:
               m_pInstance->SetData(TYPE_WING, LEFT);
               m_creature->SummonCreature(NPC_CONSTRUCT,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 9:
               m_pInstance->SetData(TYPE_WING, RIGHT);
               m_creature->SummonCreature(NPC_CONSTRUCT,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_FIEND,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_NECROMANCER,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               m_creature->SummonCreature(NPC_GHOUL,SummonScourge[0][0], SummonScourge[0][1], SummonScourge[0][2], SummonScourge[0][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000);
               break;
            case 10:
               m_pInstance->SetData(TYPE_PHASE, 4);
               if (Creature* pSalramm = m_creature->SummonCreature(NPC_SALRAMM,SummonScourge[1][0], SummonScourge[1][1], SummonScourge[1][2], SummonScourge[1][3],TEMPSUMMON_CORPSE_TIMED_DESPAWN,29000))
               {
                   DoScriptText(SAY_SALRAMM_SPAWN, pSalramm);
               }
               break;
        }
    }

    void HouseEvent()
    {
        switch(m_uiStep)
        {
            case 0:
                if (Creature* pHuman = m_pInstance->instance->GetCreature(m_uiHuman01GUID))
                {
                    m_creature->SetGuidValue(UNIT_FIELD_TARGET, pHuman->GetObjectGuid());
                    pHuman->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                    DoScriptText(SAY_PHASE503, pHuman);
                }
                JumpNextStep(4000);
                break;
            case 1:
                DoScriptText(SAY_PHASE504, m_creature);
                m_creature->GetMotionMaster()->MovePoint(0, 2396.035f, 1206.942f, 134.038f);
                JumpNextStep(3000);
                break;
            case 2:
                m_creature->HandleEmoteCommand(37);
                JumpNextStep(2000);
                break;
            case 3:
                DoScriptText(SAY_PHASE505, m_creature);
                JumpNextStep(2000);
                break;
            case 4:
                if (Creature* pHuman = m_pInstance->instance->GetCreature(m_uiHuman01GUID))
                    DoScriptText(SAY_PHASE506, pHuman);
                JumpNextStep(6000);
                break;
            case 5:
                m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                if (Creature* pHuman = m_pInstance->instance->GetCreature(m_uiHuman01GUID))
                {
                    pHuman->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                    pHuman->UpdateEntry(NPC_INFINITE_ADVERSARY);
                }
                if (Creature* pHuman2 = m_pInstance->instance->GetCreature(m_uiHuman02GUID))
                    pHuman2->UpdateEntry(NPC_INFINITE_HUNTER);
                if (Creature* pHuman3 = m_pInstance->instance->GetCreature(m_uiHuman03GUID))
                    pHuman3->UpdateEntry(NPC_INFINITE_AGENT);
                JumpNextStep(1000);
                break;
            case 6:
              SetRun(true);
              m_creature->GetMotionMaster()->MovePoint(0, 2384.320f, 1202.779f, 134.040f);
              DoScriptText(SAY_PHASE507, m_creature);
              JumpNextStep(5000);
              break;
            case 7:
              SetEscortPaused(false);
              m_creature->setFaction(FACTION);
              m_pInstance->SetData(TYPE_PHASE, 5);
              JumpNextStep(1000);
              break;
        }
    }

    void EpochEvent()
    {
        switch(m_uiStep)
        {
            case 0:
                m_creature->SummonCreature(NPC_TIME_RIFT_2,2445.629f,1111.500f,148.076f,3.229f,TEMPSUMMON_TIMED_DESPAWN,9000);
                JumpNextStep(2000);
                break;
            case 1:
                pEpoch = m_creature->SummonCreature(NPC_EPOCH,2445.629f,1111.500f,148.076f,3.229f,TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN,900000);
               if (pEpoch)
               {
                   pEpoch->setFaction(35);
                   DoScriptText(SAY_EPOCH_INTRO, pEpoch);
                   m_creature->SetGuidValue(UNIT_FIELD_TARGET, pEpoch->GetObjectGuid());
               }
               JumpNextStep(20000);
               break;
            case 2:
                DoScriptText(SAY_ARTHAS_INTRO, m_creature);
                JumpNextStep(6000);
                break;
            case 3:
               if (pEpoch)
               {
                   DoScriptText(SAY_EPOCH_AGGRO, pEpoch);
                   m_creature->AI()->AttackStart(pEpoch);
                   pEpoch->AI()->AttackStart(m_creature);
                   pEpoch->setFaction(14);
               }
               m_pInstance->SetData(TYPE_PHASE, 5);
               SetRun(false);
               JumpNextStep(6000);
               break;
        }
    }

    void MalganisEvent()
    {
         Map::PlayerList const &PlayerList = m_pInstance->instance->GetPlayers();
         bool bNeedSpawn = false;

         switch(m_uiStep)
         {
             case 0:
                m_creature->setFaction(35);
                m_creature->GetMotionMaster()->MovePoint(0, 2302.326f, 1491.386f, 128.362f);
                if (Creature* pMalganis = m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS))
                {
                  DoScriptText(SAY_MALGANIS_ESCAPE01, Malganis);
                  pMalganis->InterruptNonMeleeSpells(false);
                  pMalganis->GetMotionMaster()->MovePoint(0, 2296.665f,1502.362f,128.362f);
                  m_creature->SetGuidValue(UNIT_FIELD_TARGET, Malganis->GetObjectGuid());
                  pMalganis->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                }
                JumpNextStep(10000);
                break;
             case 1:
                if (Creature* pMalganis = m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS))
                   DoScriptText(SAY_MALGANIS_ESCAPE02, pMalganis);
                JumpNextStep(10000);
                break;
             case 2:
                DoScriptText(SAY_ARTHAS_OUTRO01, m_creature);
                JumpNextStep(5000);
                break;
             case 3:
                if (Creature* pMalganis = m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS))
                   DoScriptText(SAY_MALGANIS_OUTRO, pMalganis);
                JumpNextStep(20000);
                break;
             case 4:
                if (Creature* pMalganis = m_pInstance->GetSingleCreatureFromStorage(NPC_MALGANIS))
                {
                   Malganis->SetVisibility(VISIBILITY_OFF);
                   m_creature->GetMotionMaster()->MovePoint(0, pMalganis->GetPositionX(), pMalganis->GetPositionY(), pMalganis->GetPositionZ());
                }
                m_creature->SetGuidValue(UNIT_FIELD_TARGET, ObjectGuid());
                m_creature->SetWalk(false);
                JumpNextStep(3000);
                break;
            case 5:
                DoScriptText(SAY_ARTHAS_OUTRO02, m_creature);
                JumpNextStep(6000);
                break;
            case 6:
                m_creature->GetMotionMaster()->MovePoint(0, 2298.298f,1500.362f,128.362f);
                DoScriptText(SAY_ARTHAS_OUTRO03, m_creature);
                JumpNextStep(11000);
                break;
            case 7:
                m_creature->GetMotionMaster()->MovePoint(0, 2243.311f, 1476.025f, 132.352f);

                if (!PlayerList.isEmpty())
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (i->getSource()->GetQuestStatus(QUEST_A_ROYAL_ESCORT) == QUEST_STATUS_INCOMPLETE ||
                            i->getSource()->GetQuestStatus(QUEST_A_ROYAL_ESCORT) == QUEST_STATUS_COMPLETE)
                        {
                            bNeedSpawn = true;
                            break;
                        }

                if (bNeedSpawn)
                    m_creature->SummonCreature(NPC_CHROMI03, 2311.61f, 1497.85f, 128.01f, 4.14f, TEMPSUMMON_TIMED_DESPAWN, 1800000);
                JumpNextStep(11000);
                break;
            case 8:
                m_pInstance->SetData(TYPE_PHASE, 12);
                m_creature->SetVisibility(VISIBILITY_OFF);
                break;
         }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       npc_escortAI::UpdateAI(uiDiff);

       if (!m_pInstance) return;

       if (StartEvent == true)
       {
         if (m_pInstance->GetData(TYPE_INTRO) != DONE)
         {
            if (m_uiStepTimer < uiDiff)
            {
               IntroEvent();
            }
            else m_uiStepTimer -= uiDiff;
         }

         if (m_pInstance->GetData(TYPE_PHASE) == 2)
         {
            if (m_uiStepTimer < uiDiff)
            {
               EnterEvent();
            }
            else m_uiStepTimer -= uiDiff;
         }

         if (m_pInstance->GetData(TYPE_PHASE) == 3)
         {
            if (m_uiSummonTimer < uiDiff)
            {
               SummonWing();
               m_uiSummonTimer = 70000;
            }
            else m_uiSummonTimer -= uiDiff;
         }

         if (m_pInstance->GetData(TYPE_PHASE) == 4 && m_pInstance->GetData(TYPE_ENCOUNTER) == DONE)
         {
            m_pInstance->SetData(TYPE_PHASE, 5);
            SetRun(true);
            EnableEscort();
            DoScriptText(SAY_PHASE501, m_creature);
         }

         if (m_pInstance->GetData(TYPE_PHASE) == 6)
         {
            if(m_uiStepTimer < uiDiff)
            {
               HouseEvent();
            }
            else m_uiStepTimer -= uiDiff;
         }

         if (m_pInstance->GetData(TYPE_PHASE) == 7)
         {
            if(m_uiStepTimer < uiDiff)
            {
               EpochEvent();
            }
            else m_uiStepTimer -= uiDiff;
         }

         } //close event!

         if (m_pInstance->GetData(TYPE_PHASE) == 10)
         {
            SetEscortPaused(true);
            ResetStep(1000);
            m_creature->AttackStop();
            m_pInstance->SetData(TYPE_PHASE, 11);
         }

         if (m_pInstance->GetData(TYPE_PHASE) == 11)
         {
            if (m_uiStepTimer < uiDiff)
            {
               MalganisEvent();
            }
            else m_uiStepTimer -= uiDiff;
         }

       //} //close event!

       if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
          return;

       if (m_pInstance->GetData(TYPE_PHASE) > 9) return;

       if (m_uiExorcismTimer < uiDiff)
       {
          if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
              DoCast(target, m_bIsHeroic ? SPELL_EXORCISM_H : SPELL_EXORCISM_N);

          m_uiExorcismTimer = 7300;
       }else m_uiExorcismTimer -= uiDiff;

       if (m_uiHealTimer < uiDiff)
       {
          if (m_creature->GetHealthPercent() < 40.0f)
          {
            DoCast(m_creature, SPELL_HOLY_LIGHT);
            m_uiHealTimer = 20000;
          }
       } else m_uiHealTimer -= uiDiff;

       return;
    }
};

/*###
## npc_uther
###*/

struct MANGOS_DLL_DECL npc_utherAI : public npc_escortAI
{
    npc_utherAI(Creature* pCreature) : npc_escortAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiStep;
    uint32 m_uiStepTimer;
    bool StartEvent;

    ObjectGuid m_uiKnightGUID01;
    ObjectGuid m_uiKnightGUID02;
    ObjectGuid m_uiKnightGUID03;

    void Reset()
    {
          m_creature->SetVisibility(VISIBILITY_OFF);
          m_uiStep = 0;
          m_uiStepTimer = 100;
    }

    void StartAI()
    {
           //m_pInstance->SetWeather(WEATHER_STATE_MEDIUM_RAIN, 0.9999f);
           StartEvent = true;
           m_creature->SetVisibility(VISIBILITY_ON);
           ((npc_utherAI*)m_creature->AI())->Start(true);

           if (Creature* pKnight01 = m_creature->SummonCreature(NPC_KNIGHT,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),m_creature->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,110000))
           {
               m_uiKnightGUID01 = pKnight01->GetObjectGuid();
               pKnight01->SetWalk(false);
               pKnight01->GetMotionMaster()->MoveFollow(m_creature,PET_FOLLOW_DIST,M_PI_F/2);
           }

           if (Creature* pKnight02 = m_creature->SummonCreature(NPC_KNIGHT,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),m_creature->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,110000))
           {
               m_uiKnightGUID02 = pKnight02->GetObjectGuid();
               pKnight02->SetWalk(false);
               pKnight02->GetMotionMaster()->MoveFollow(m_creature,PET_FOLLOW_DIST,M_PI_F/4);
           }

           if (Creature* pKnight03 = m_creature->SummonCreature(NPC_KNIGHT,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ(),m_creature->GetOrientation(),TEMPSUMMON_TIMED_DESPAWN,110000))
           {
               m_uiKnightGUID03 = pKnight03->GetObjectGuid();
               pKnight03->SetWalk(false);
               pKnight03->GetMotionMaster()->MoveFollow(m_creature,PET_FOLLOW_DIST,M_PI_F/3);
           }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 3:
                if (Creature* pArthas = m_pInstance->GetSingleCreatureFromStorage(NPC_ARTHAS))
                {
                    pArthas->SetWalk(false);
                    pArthas->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                    pArthas->GetMotionMaster()->MovePoint(0, 1902.974f, 1291.635f, 143.337f);
                }
                break;
            case 4:
                SetRun(false);
                if (Creature *pArthas = m_pInstance->GetSingleCreatureFromStorage(NPC_ARTHAS))
                   ((npc_arthasAI*)pArthas->AI())->StartAI();
                break;
            case 6:
                m_creature->SetVisibility(VISIBILITY_OFF);
                if (Creature* pJaina = m_pInstance->GetSingleCreatureFromStorage(NPC_JAINA))
                   pJaina->SetVisibility(VISIBILITY_OFF);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
       npc_escortAI::UpdateAI(uiDiff);

       if (!m_pInstance) return;

       return;
    }
};

/*###
## npc_chromi_middle
###*/

#define GOSSIP_ITEM_CHROMI1 "What do you think they're up to?"
#define GOSSIP_ITEM_CHROMI2 "What want me to do what?"
#define GOSSIP_ITEM_CHROMI3 "Very well, Chromie."

enum
{
  QUEST_ROYAL_ESCORT               = 13151,
  GOSSIP_TEXTID_CHROMI1            = 12953,
  GOSSIP_TEXTID_CHROMI2            = 12949,
  GOSSIP_TEXTID_CHROMI3            = 12950,
  GOSSIP_TEXTID_CHROMI4            = 12952
};

bool GossipHello_npc_chromi_middle(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    //if (pPlayer->GetQuestStatus(QUEST_DISPELLING_ILLUSIONS) == QUEST_STATUS_INCOMPLETE)
    //return false;

    ScriptedInstance* pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
    if (pInstance && pInstance->GetData(TYPE_INTRO) == NOT_STARTED)
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMI1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_CHROMI1, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_chromi_middle(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (ScriptedInstance* m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData()))
      if (m_pInstance->GetData(TYPE_INTRO) != NOT_STARTED) return true;

    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMI2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

       pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_CHROMI2, pCreature->GetObjectGuid());
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_CHROMI3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

       pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_CHROMI3, pCreature->GetObjectGuid());
    }

    if (uiAction == GOSSIP_ACTION_INFO_DEF+3)
    {
       if (ScriptedInstance* m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData()))
        {
            m_pInstance->DoUpdateWorldState(WORLD_STATE_COS_CRATE_ON, 0);
            m_pInstance->SetData(TYPE_INTRO, IN_PROGRESS);
            if (Creature *pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                ((npc_utherAI*)pUther->AI())->StartAI();
        }

       pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_CHROMI4, pCreature->GetObjectGuid());
    }

    return true;
}

struct MANGOS_DLL_DECL npc_chromi_middleAI : public ScriptedAI
{
    npc_chromi_middleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_creature->SetActiveObjectState(true);
        m_bUtherHere = false;
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool m_bUtherHere;

    void Reset()
    {
        m_bUtherHere = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bUtherHere && m_pInstance && pWho && pWho->GetTypeId() == TYPEID_PLAYER && m_creature->GetDistance2d(pWho) <= 15 && ((Player*)pWho)->GetQuestStatus(QUEST_A_ROYAL_ESCORT) == QUEST_STATUS_INCOMPLETE)
        {
            m_pInstance->DoUpdateWorldState(WORLD_STATE_COS_CRATE_ON, 0);
            m_pInstance->SetData(TYPE_INTRO, IN_PROGRESS);
            if (Creature *pUther = m_pInstance->GetSingleCreatureFromStorage(NPC_UTHER))
                ((npc_utherAI*)pUther->AI())->StartAI();
            m_bUtherHere = true;
        }
    }
};

/*###
## npc_arthas_dialog
###*/

enum
{
   GOSSIP_MENU_ARTHAS_1                        = 100001,
   GOSSIP_MENU_ARTHAS_2                        = 100002,
   GOSSIP_MENU_ARTHAS_3                        = 100003,
   GOSSIP_MENU_ARTHAS_4                        = 100004,
   GOSSIP_MENU_ARTHAS_5                        = 100005
};

#define GOSSIP_ITEM_ARTHAS_0 "I'm ready to start Culling of Stratholme."
#define GOSSIP_ITEM_ARTHAS_1 "Yes, my Prince. We're ready."
#define GOSSIP_ITEM_ARTHAS_2 "We're only doing what is best for Loarderon your Highness."
#define GOSSIP_ITEM_ARTHAS_3 "I'm ready."
#define GOSSIP_ITEM_ARTHAS_4 "For Lordaeron!"
#define GOSSIP_ITEM_ARTHAS_5 "I'm ready to battle the dreadlord, sire."

bool GossipHello_npc_arthas(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());

    if (pInstance && pInstance->GetData(TYPE_PHASE) == 0)
    {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ARTHAS_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ARTHAS_1, pCreature->GetObjectGuid());
    }

    if (pInstance && pInstance->GetData(TYPE_PHASE) == 5)
    {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ARTHAS_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ARTHAS_2, pCreature->GetObjectGuid());
    }

    if (pInstance && pInstance->GetData(TYPE_PHASE) == 8)
    {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ARTHAS_3, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ARTHAS_3, pCreature->GetObjectGuid());
    }

    if (pInstance && pInstance->GetData(TYPE_PHASE) == 9)
    {
       pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_ARTHAS_4, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
       pPlayer->SEND_GOSSIP_MENU(GOSSIP_MENU_ARTHAS_4, pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_arthas(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
      ScriptedInstance* m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());

      if (m_pInstance && m_pInstance->GetData(TYPE_PHASE) == 0)
      {
          m_pInstance->SetData(TYPE_PHASE, 1);
         ((npc_arthasAI*)pCreature->AI())->EnableEscort();
         ((npc_arthasAI*)pCreature->AI())->RemoveGossip();
         ((npc_arthasAI*)pCreature->AI())->culling_faction = pPlayer->getFaction();
      }

      if (m_pInstance && m_pInstance->GetData(TYPE_PHASE) == 5)
      {
         ((npc_arthasAI*)pCreature->AI())->EnableEscort();
         ((npc_arthasAI*)pCreature->AI())->RemoveGossip();
      }

      if (m_pInstance && m_pInstance->GetData(TYPE_PHASE) == 8)
      {
         ((npc_arthasAI*)pCreature->AI())->EnableEscort();
         ((npc_arthasAI*)pCreature->AI())->RemoveGossip();
      }

      if (m_pInstance && m_pInstance->GetData(TYPE_PHASE) == 9)
      {
         ((npc_arthasAI*)pCreature->AI())->EnableEscort();
         ((npc_arthasAI*)pCreature->AI())->RemoveGossip();
         if (Creature* pMalganis = pCreature->SummonCreature(NPC_MALGANIS,2296.665f,1502.362f,128.362f,4.961f,TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN,900000))
         {
           pMalganis->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
         }
      }

    return true;
}

/*###
## npc_arthas_priest
###*/

struct MANGOS_DLL_DECL npc_arthas_priestAI : public ScriptedAI
{
   npc_arthas_priestAI(Creature *c) : ScriptedAI(c)
   {
        SetCombatMovement(false);
        Reset();
   }

   uint32 m_uiSmiteTimer;
   uint32 m_uiHealTimer;

   void Reset()
   {
     m_uiSmiteTimer = 100;
     m_uiHealTimer = 1000;
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
   }

   void EnterEvadeMode()
   {
      m_creature->RemoveAllAuras();
      m_creature->DeleteThreatList();
      m_creature->CombatStop(true);
      m_creature->LoadCreatureAddon(false);

      m_creature->SetLootRecipient(NULL);

      Reset();
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

   void UpdateAI(const uint32 uiDiff)
   {

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSmiteTimer < uiDiff)
        {
           DoCast(m_creature->getVictim(), SPELL_SMITE);
           m_uiSmiteTimer = 3000;
        }
        else m_uiSmiteTimer -= uiDiff;

        if (m_uiHealTimer < uiDiff)
        {
          if (m_creature->GetHealthPercent() <= 40.0f)
          {
             m_creature->InterruptNonMeleeSpells(false);
             DoCast(m_creature, SPELL_HEAL);
             m_uiHealTimer = 3000;
          }
        }
        else m_uiHealTimer -= uiDiff;

        DoMeleeAttackIfReady();

        return;
   }
};

/*###
## npc_arthas_marine
###*/

struct MANGOS_DLL_DECL npc_arthas_marineAI : public ScriptedAI
{
   npc_arthas_marineAI(Creature *c) : ScriptedAI(c)
   {
       Reset();
   }

   uint32 m_uiHealTimer;
   uint32 m_uiHeroicTimer;
   uint32 m_uiDefendTimer;

   void Reset()
   {
       m_uiHealTimer = 3000;
       m_uiHeroicTimer = 7000;
       m_uiDefendTimer = 9000;
   }

   void Aggro(Unit* who)
   {
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

      if (m_creature->GetEntry() == NPC_MARINE_1)
          m_creature->GetMotionMaster()->MovePoint(POINT_LAST_POINT, Last1X, Last1Y, Last1Z);
      if (m_creature->GetEntry() == NPC_MARINE_2)
          m_creature->GetMotionMaster()->MovePoint(POINT_LAST_POINT, Last2X, Last2Y, Last2Z);
      if (m_creature->GetEntry() == NPC_MARINE_3)
          m_creature->GetMotionMaster()->MovePoint(POINT_LAST_POINT, Last3X, Last3Y, Last3Z);
      if (m_creature->GetEntry() == NPC_MARINE_4)
          m_creature->GetMotionMaster()->MovePoint(POINT_LAST_POINT, Last4X, Last4Y, Last4Z);

      Reset();
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

   void UpdateAI(const uint32 uiDiff)
   {

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiHealTimer < uiDiff)
        {
           if (m_creature->GetHealthPercent() <= 40.0f)
           {
              if (Creature* pHeal = GetClosestCreatureWithEntry(m_creature, NPC_PRIEST_1, 70.0f))
              {
                 if (pHeal->GetHealthPercent() > 40.0f)
                 {
                    pHeal->InterruptNonMeleeSpells(false);
                    pHeal->CastSpell(m_creature, SPELL_HEAL, false);
                    m_uiHealTimer = 3000;
                 }
              }
              else if (Creature* pHeal2 = GetClosestCreatureWithEntry(m_creature, NPC_PRIEST_2, 70.0f))
              {
                 if (pHeal2->GetHealthPercent() > 40.0f)
                 {
                    pHeal2->InterruptNonMeleeSpells(false);
                    pHeal2->CastSpell(m_creature, SPELL_HEAL, false);
                    m_uiHealTimer = 3000;
                 }
              }
           }
        }
        else m_uiHealTimer -= uiDiff;

        if (m_uiHeroicTimer < uiDiff)
        {
           DoCast(m_creature->getVictim(), SPELL_HEROIC_STRIKE);
           m_uiHeroicTimer = 7000;
        }
        else m_uiHeroicTimer -= uiDiff;

        if (m_uiDefendTimer < uiDiff)
        {
           DoCast(m_creature, SPELL_DEFEND);
           m_uiDefendTimer = 9000;
        }
        else m_uiDefendTimer -= uiDiff;

        DoMeleeAttackIfReady();

        return;
   }
};

/*###
## npc_dark_conversion
###*/

/*enum
{
   SAY_PEOPLE01         = -1594099,
   SAY_PEOPLE02         = -1594100,
   SAY_PEOPLE03         = -1594101,
   SAY_PEOPLE04         = -1594102,
   SAY_PEOPLE05         = -1594103,
};*/

struct MANGOS_DLL_DECL npc_dark_conversionAI : public ScriptedAI
{
   npc_dark_conversionAI(Creature *pCreature) : ScriptedAI(pCreature)
   {
    m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    m_creature->SetActiveObjectState(true);

    if (m_pInstance && m_pInstance->GetData(TYPE_ENCOUNTER) == IN_PROGRESS)
        m_creature->UpdateEntry(NPC_ZOMBIE);

    Reset();
   }

ScriptedInstance* m_pInstance;

bool Special;
bool Conversion;
uint32 m_uiStep;
uint32 m_uiStepTimer;

   void Reset()
   {
       m_creature->setFaction(35);
       Conversion = false;
       Special = false;
       m_uiStep = 1;
       m_uiStepTimer = 5000;

       if (m_pInstance && m_pInstance->GetData(TYPE_ENCOUNTER) == IN_PROGRESS)
            m_creature->UpdateEntry(NPC_ZOMBIE);
   }

   void MalganisScared(Creature* target, float horizontalSpeed, float verticalSpeed)
   {
        float angle = target->GetAngle(m_creature);
        float vsin = sin(angle);
        float vcos = cos(angle);

        float ox, oy, oz;
        m_creature->GetPosition(ox, oy, oz);

        float g = 19.23f;// seems that physic constant g(earth's gravity) in world of warcraft is about 2 times larger than real
        float dh = verticalSpeed*verticalSpeed / (2*g); // maximum parabola height
        float time = sqrtf(dh/(0.124976 * verticalSpeed));  //full move time in seconds     // should be  time = 2*Vert_speed/g, but..

        float dis = time * horizontalSpeed;
        float fx = ox + dis * vcos;
        float fy = oy + dis * vsin;
        float fz = oz;

        m_creature->UpdateGroundPositionZ(fx, fy, fz);

        m_creature->SetWalk(false);
        m_creature->GetMotionMaster()->MovePoint(0, fx, fy, fz);
   }

   void DarkConversion(bool Move)
   {
        m_creature->UpdateEntry(NPC_ZOMBIE);
        if(Move == true)
        {
           if(Creature* pArthas = m_pInstance->GetSingleCreatureFromStorage(NPC_ARTHAS))
              m_creature->GetMotionMaster()->MovePoint(0, pArthas->GetPositionX(), pArthas->GetPositionY(), pArthas->GetPositionZ());
        }
   }

   void UpdateAI(const uint32 uiDiff)
    {
       if (!m_pInstance) return;

       if (m_pInstance->GetData(TYPE_ENCOUNTER) == IN_PROGRESS)
       {
          if (Creature* pMalganis = GetClosestCreatureWithEntry(m_creature, NPC_MALGANIS, 20.0f))
          {
               if (Special == false)
               {
                  float Dist = m_creature->GetDistance2d(pMalganis->GetPositionX(), pMalganis->GetPositionY());
                  Dist = Dist + 2.0f;
                  MalganisScared(pMalganis, Dist, 1.0f);
                  m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_COWER);
                  m_uiStepTimer = 5000;
                  Special = true;
               }
          }

          if (m_uiStepTimer < uiDiff && Conversion != true)
          {
             Conversion = true;
             if (Special != false)
                DarkConversion(true);
             else
                DarkConversion(false);
          }
          else m_uiStepTimer -= uiDiff;

       }

       DoMeleeAttackIfReady();

       return;
    }
};

CreatureAI* GetAI_npc_chromi_middle(Creature* pCreature)
{
    return new npc_chromi_middleAI(pCreature);
}

CreatureAI* GetAI_npc_uther(Creature* pCreature)
{
    return new npc_utherAI(pCreature);
}

CreatureAI* GetAI_npc_arthas(Creature* pCreature)
{
    return new npc_arthasAI(pCreature);
}

CreatureAI* GetAI_npc_arthas_priest(Creature* pCreature)
{
    return new npc_arthas_priestAI(pCreature);
}

CreatureAI* GetAI_npc_arthas_marine(Creature* pCreature)
{
    return new npc_arthas_marineAI(pCreature);
}

CreatureAI* GetAI_npc_dark_conversion(Creature* pCreature)
{
    return new npc_dark_conversionAI(pCreature);
}

void AddSC_culling_of_stratholme()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_chromi_middle";
    pNewScript->pGossipHello =  &GossipHello_npc_chromi_middle;
    pNewScript->pGossipSelect = &GossipSelect_npc_chromi_middle;
    pNewScript->GetAI = &GetAI_npc_chromi_middle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_uther";
    pNewScript->GetAI = &GetAI_npc_uther;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arthas";
    pNewScript->GetAI = &GetAI_npc_arthas;
    pNewScript->pGossipHello =  &GossipHello_npc_arthas;
    pNewScript->pGossipSelect = &GossipSelect_npc_arthas;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arthas_priest";
    pNewScript->GetAI = &GetAI_npc_arthas_priest;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_arthas_marine";
    pNewScript->GetAI = &GetAI_npc_arthas_marine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dark_conversion";
    pNewScript->GetAI = &GetAI_npc_dark_conversion;
    pNewScript->RegisterSelf();
}
