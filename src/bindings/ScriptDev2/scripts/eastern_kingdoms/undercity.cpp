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
SDName: Undercity
SD%Complete: 95
SDComment: Quest support: 6628(Parqual Fintallas questions/'answers' might have more to it, need more info), 9180(post-event).
SDCategory: Undercity
EndScriptData */

/* ContentData
npc_lady_sylvanas_windrunner
npc_highborne_lamenter
npc_parqual_fintallas
EndContentData */

#include "precompiled.h"

/*######
## npc_lady_sylvanas_windrunner
######*/

enum
{
    SAY_LAMENT_END                  = -1000196,
    EMOTE_LAMENT_END                = -1000197,

    SOUND_CREDIT                    = 10896,
    ENTRY_HIGHBORNE_LAMENTER        = 21628,
    ENTRY_HIGHBORNE_BUNNY           = 21641,

    SPELL_HIGHBORNE_AURA            = 37090,
    SPELL_SYLVANAS_CAST             = 36568,
    SPELL_RIBBON_OF_SOULS           = 34432,                  //the real one to use might be 37099
    SPELL_BLACK_ARROW               = 59712,
    SPELL_FADE                      = 20672,
    SPELL_MULTI_SHOT                = 59713,
    SPELL_SHOOT_SYLVANAS            = 59710,
    SPELL_SUMMON_SKELETON           = 59711,
};

float HighborneLoc[4][3]=
{
    {1285.41f, 312.47f, 0.51f},
    {1286.96f, 310.40f, 1.00f},
    {1289.66f, 309.66f, 1.52f},
    {1292.51f, 310.50f, 1.99f},
};
#define HIGHBORNE_LOC_Y             -61.00f
#define HIGHBORNE_LOC_Y_NEW         -55.50f

struct MANGOS_DLL_DECL npc_lady_sylvanas_windrunnerAI : public ScriptedAI
{
    npc_lady_sylvanas_windrunnerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 LamentEvent_Timer;
    bool LamentEvent;
    ObjectGuid m_targetGuid;

    float myX;
    float myY;
    float myZ;

    uint32 m_uiBlackArrowTimer;
    uint32 m_uiMultiShotTimer;
    uint32 m_uiFadeTimer;
    uint32 m_uiShootTimer;
    uint32 m_uiSummonSkeletonTimer;

    void Reset()
    {
        myX = m_creature->GetPositionX();
        myY = m_creature->GetPositionY();
        myZ = m_creature->GetPositionZ();

        LamentEvent_Timer = 5000;
        LamentEvent = false;
        m_targetGuid.Clear();

        m_uiBlackArrowTimer     = 15000;
        m_uiMultiShotTimer      = 11000;
        m_uiFadeTimer           = 15000;
        m_uiShootTimer          = 5000;
        m_uiSummonSkeletonTimer = 17000;
    }

    void JustSummoned(Creature *summoned)
    {
        if (summoned->GetEntry() == ENTRY_HIGHBORNE_BUNNY)
        {
            if (Creature* pBunny = m_creature->GetMap()->GetCreature(m_targetGuid))
            {
                pBunny->NearTeleportTo(pBunny->GetPositionX(), pBunny->GetPositionY(), myZ+15.0f, 0.0f);
                summoned->CastSpell(pBunny,SPELL_RIBBON_OF_SOULS,false);
            }

            m_targetGuid = summoned->GetObjectGuid();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (LamentEvent)
        {
            if (LamentEvent_Timer < uiDiff)
            {
                float raX = myX;
                float raY = myY;
                float raZ = myZ;

                m_creature->GetRandomPoint(myX,myY,myZ,20.0,raX,raY,raZ);
                m_creature->SummonCreature(ENTRY_HIGHBORNE_BUNNY,raX,raY,myZ,0,TEMPSUMMON_TIMED_DESPAWN,3000);

                LamentEvent_Timer = 2000;
                if (!m_creature->HasAura(SPELL_SYLVANAS_CAST, EFFECT_INDEX_0))
                {
                    DoScriptText(SAY_LAMENT_END, m_creature);
                    DoScriptText(EMOTE_LAMENT_END, m_creature);
                    LamentEvent = false;
                }
            }else LamentEvent_Timer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBlackArrowTimer < uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_BLACK_ARROW);
            m_uiBlackArrowTimer = urand(15000, 25000);
        }
        else
            m_uiBlackArrowTimer -= uiDiff;

        if (m_uiMultiShotTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_MULTI_SHOT);
            m_uiMultiShotTimer = urand(11000, 14000);
        }
        else
            m_uiMultiShotTimer -= uiDiff;

        if (m_uiFadeTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_FADE);
            m_uiFadeTimer = urand(15000, 20000);
        }
        else
            m_uiFadeTimer -= uiDiff;

        if (m_uiShootTimer < uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SHOOT_SYLVANAS);
            m_uiShootTimer = urand(6000, 9000);
        }
        else
            m_uiShootTimer -= uiDiff;

        if (m_uiSummonSkeletonTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_SUMMON_SKELETON);
            m_uiSummonSkeletonTimer = urand(17000, 23000);
        }
        else
            m_uiSummonSkeletonTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_npc_lady_sylvanas_windrunner(Creature* pCreature)
{
    return new npc_lady_sylvanas_windrunnerAI(pCreature);
}

bool QuestRewarded_npc_lady_sylvanas_windrunner(Player* pPlayer, Creature* pCreature, Quest const* pQuest)
{
    if (pQuest->GetQuestId() == 9180)
    {
        if (npc_lady_sylvanas_windrunnerAI* pSylvanAI = dynamic_cast<npc_lady_sylvanas_windrunnerAI*>(pCreature->AI()))
        {
            pSylvanAI->LamentEvent = true;
            pSylvanAI->DoPlaySoundToSet(pCreature, SOUND_CREDIT);
        }

        pCreature->CastSpell(pCreature,SPELL_SYLVANAS_CAST,false);

        for(uint8 i = 0; i < 4; ++i)
            pCreature->SummonCreature(ENTRY_HIGHBORNE_LAMENTER, HighborneLoc[i][0], HighborneLoc[i][1], HIGHBORNE_LOC_Y, HighborneLoc[i][2], TEMPSUMMON_TIMED_DESPAWN, 160000);
    }

    return true;
}

/*######
## npc_highborne_lamenter
######*/

struct MANGOS_DLL_DECL npc_highborne_lamenterAI : public ScriptedAI
{
    npc_highborne_lamenterAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 EventMove_Timer;
    uint32 EventCast_Timer;
    bool EventMove;
    bool EventCast;

    void Reset()
    {
        EventMove_Timer = 10000;
        EventCast_Timer = 17500;
        EventMove = true;
        EventCast = true;
    }

    void UpdateAI(const uint32 diff)
    {
        if (EventMove)
        {
            if (EventMove_Timer < diff)
            {
                m_creature->SetLevitate(true);
                m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(),m_creature->GetPositionY(),HIGHBORNE_LOC_Y_NEW,3.f);
                EventMove = false;
            }else EventMove_Timer -= diff;
        }
        if (EventCast)
        {
            if (EventCast_Timer < diff)
            {
                DoCastSpellIfCan(m_creature,SPELL_HIGHBORNE_AURA);
                EventCast = false;
            }else EventCast_Timer -= diff;
        }
    }
};
CreatureAI* GetAI_npc_highborne_lamenter(Creature* pCreature)
{
    return new npc_highborne_lamenterAI(pCreature);
}

/*######
## npc_parqual_fintallas
######*/

#define SPELL_MARK_OF_SHAME 6767

bool GossipHello_npc_parqual_fintallas(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(6628) == QUEST_STATUS_INCOMPLETE && !pPlayer->HasAura(SPELL_MARK_OF_SHAME, EFFECT_INDEX_0))
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Gul'dan", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Kel'Thuzad", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Ner'zhul", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
        pPlayer->SEND_GOSSIP_MENU(5822, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(5821, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_parqual_fintallas(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->CastSpell(pPlayer,SPELL_MARK_OF_SHAME,false);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF+2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pPlayer->AreaExploredOrEventHappens(6628);
    }
    return true;
}

/*######
## AddSC
######*/

void AddSC_undercity()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_sylvanas_windrunner";
    pNewScript->GetAI = &GetAI_npc_lady_sylvanas_windrunner;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_lady_sylvanas_windrunner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_highborne_lamenter";
    pNewScript->GetAI = &GetAI_npc_highborne_lamenter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_parqual_fintallas";
    pNewScript->pGossipHello = &GossipHello_npc_parqual_fintallas;
    pNewScript->pGossipSelect = &GossipSelect_npc_parqual_fintallas;
    pNewScript->RegisterSelf();
}
