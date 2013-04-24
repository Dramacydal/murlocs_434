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
SDName: Howling_Fjord
SD%Complete: ?
SDComment: Quest support: 11221, 11300, 11464, 11343
SDCategory: Howling Fjord
EndScriptData */

/* ContentData
npc_ancient_male_vrykul
at_ancient_male_vrykul
npc_daegarn
npc_deathstalker_razael - TODO, can be moved to database
npc_dark_ranger_lyana - TODO, can be moved to database
npc_silvermoon_harry
EndContentData */

#include "precompiled.h"

enum
{
    SPELL_ECHO_OF_YMIRON                    = 42786,
    SPELL_SECRET_OF_NIFFELVAR               = 43458,
    QUEST_ECHO_OF_YMIRON                    = 11343,
    NPC_MALE_VRYKUL                         = 24314,
    NPC_FEMALE_VRYKUL                       = 24315,

    SAY_VRYKUL_CURSED                       = -1000635,
    EMOTE_VRYKUL_POINT                      = -1000636,
    EMOTE_VRYKUL_SOB                        = -1000637,
    SAY_VRYKUL_DISPOSE                      = -1000638,
    SAY_VRYKUL_BEG                          = -1000639,
    SAY_VRYKUL_WHAT                         = -1000640,
    SAY_VRYKUL_HIDE                         = -1000641,
};

struct MANGOS_DLL_DECL npc_ancient_male_vrykulAI : public ScriptedAI
{
    npc_ancient_male_vrykulAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bEventInProgress;
    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;

    void Reset()
    {
        m_bEventInProgress = false;
        m_uiPhase = 0;
        m_uiPhaseTimer = 0;
    }

    void StartEvent()
    {
        if (m_bEventInProgress)
            return;

        m_bEventInProgress = true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bEventInProgress)
            return;

        if (m_uiPhaseTimer < uiDiff)
            m_uiPhaseTimer = 5000;
        else
        {
            m_uiPhaseTimer -= uiDiff;
            return;
        }

        Creature* pFemale = GetClosestCreatureWithEntry(m_creature, NPC_FEMALE_VRYKUL, 10.0f);

        switch(m_uiPhase)
        {
            case 0:
                DoScriptText(SAY_VRYKUL_CURSED, m_creature);
                DoScriptText(EMOTE_VRYKUL_POINT, m_creature);
                break;
            case 1:
                if (pFemale)
                    DoScriptText(EMOTE_VRYKUL_SOB, pFemale);
                DoScriptText(SAY_VRYKUL_DISPOSE, m_creature);
                break;
            case 2:
                if (pFemale)
                    DoScriptText(SAY_VRYKUL_BEG, pFemale);
                break;
            case 3:
                DoScriptText(SAY_VRYKUL_WHAT, m_creature);
                break;
            case 4:
                if (pFemale)
                    DoScriptText(SAY_VRYKUL_HIDE, pFemale);
                break;
            case 5:
                DoCastSpellIfCan(m_creature, SPELL_SECRET_OF_NIFFELVAR);
                break;
            case 6:
                Reset();
                return;
        }

        ++m_uiPhase;
    }
};

CreatureAI* GetAI_npc_ancient_male_vrykul(Creature* pCreature)
{
    return new npc_ancient_male_vrykulAI(pCreature);
}

bool AreaTrigger_at_ancient_male_vrykul(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (pPlayer->isAlive() && pPlayer->GetQuestStatus(QUEST_ECHO_OF_YMIRON) == QUEST_STATUS_INCOMPLETE &&
        pPlayer->HasAura(SPELL_ECHO_OF_YMIRON))
    {
        if (Creature* pCreature = GetClosestCreatureWithEntry(pPlayer, NPC_MALE_VRYKUL, 20.0f))
        {
            if (npc_ancient_male_vrykulAI* pVrykulAI = dynamic_cast<npc_ancient_male_vrykulAI*>(pCreature->AI()))
                pVrykulAI->StartEvent();
        }
    }

    return true;
}

/*######
## npc_daegarn
######*/

enum
{
    QUEST_DEFEAT_AT_RING            = 11300,

    NPC_FIRJUS                      = 24213,
    NPC_JLARBORN                    = 24215,
    NPC_YOROS                       = 24214,
    NPC_OLUF                        = 23931,

    NPC_PRISONER_1                  = 24253,                // looks the same but has different abilities
    NPC_PRISONER_2                  = 24254,
    NPC_PRISONER_3                  = 24255,
};

static float afSummon[] = {838.81f, -4678.06f, -94.182f};
static float afCenter[] = {801.88f, -4721.87f, -96.143f};

// TODO: make prisoners help (unclear if summoned or using npc's from surrounding cages (summon inside small cages?))
struct MANGOS_DLL_DECL npc_daegarnAI : public ScriptedAI
{
    npc_daegarnAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bEventInProgress;
    ObjectGuid m_playerGuid;

    void Reset()
    {
        m_bEventInProgress = false;
        m_playerGuid.Clear();
    }

    void StartEvent(Player* pPlayer)
    {
        if (m_bEventInProgress)
            return;

        m_playerGuid = pPlayer->GetObjectGuid();

        SummonGladiator(NPC_FIRJUS);
    }

    void JustSummoned(Creature* pSummon)
    {
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
        {
            if (pPlayer->isAlive())
            {
                pSummon->SetWalk(false);
                pSummon->GetMotionMaster()->MovePoint(0, afCenter[0], afCenter[1], afCenter[2]);
                return;
            }
        }

        Reset();
    }

    void SummonGladiator(uint32 uiEntry)
    {
        m_creature->SummonCreature(uiEntry, afSummon[0], afSummon[1], afSummon[2], 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 20*IN_MILLISECONDS);
    }

    void SummonedMovementInform(Creature* pSummoned, uint32 uiMotionType, uint32 uiPointId)
    {
        Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

        // could be group, so need additional here.
        if (!pPlayer || !pPlayer->isAlive())
        {
            Reset();
            return;
        }

        if (pSummoned->IsWithinDistInMap(pPlayer, 75.0f))   // ~the radius of the ring
            pSummoned->AI()->AttackStart(pPlayer);
    }

    void SummonedCreatureDespawn(Creature* pSummoned)
    {
        uint32 uiEntry = 0;

        // will eventually reset the event if something goes wrong
        switch(pSummoned->GetEntry())
        {
            case NPC_FIRJUS:    uiEntry = NPC_JLARBORN; break;
            case NPC_JLARBORN:  uiEntry = NPC_YOROS;    break;
            case NPC_YOROS:     uiEntry = NPC_OLUF;     break;
            case NPC_OLUF:      Reset();                return;
        }

        SummonGladiator(uiEntry);
    }
};

bool QuestAccept_npc_daegarn(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_DEFEAT_AT_RING)
    {
        if (npc_daegarnAI* pDaegarnAI = dynamic_cast<npc_daegarnAI*>(pCreature->AI()))
            pDaegarnAI->StartEvent(pPlayer);
    }

    return true;
}

CreatureAI* GetAI_npc_daegarn(Creature* pCreature)
{
    return new npc_daegarnAI(pCreature);
}

/*######
## npc_deathstalker_razael - TODO, can be moved to database
######*/

#define GOSSIP_ITEM_DEATHSTALKER_RAZAEL "High Executor Anselm requests your report."

enum
{
    QUEST_REPORTS_FROM_THE_FIELD       = 11221,
    SPELL_RAZAEL_KILL_CREDIT           = 42756,
    GOSSIP_TEXTID_DEATHSTALKER_RAZAEL1 = 11562,
    GOSSIP_TEXTID_DEATHSTALKER_RAZAEL2 = 11564
};

bool GossipHello_npc_deathstalker_razael(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DEATHSTALKER_RAZAEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEATHSTALKER_RAZAEL1, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_deathstalker_razael(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DEATHSTALKER_RAZAEL2, pCreature->GetObjectGuid());
            pCreature->CastSpell(pPlayer, SPELL_RAZAEL_KILL_CREDIT, true);
            break;
    }

    return true;
}

/*######
## npc_dark_ranger_lyana - TODO, can be moved to database
######*/

#define GOSSIP_ITEM_DARK_RANGER_LYANA "High Executor Anselm requests your report."

enum
{
    GOSSIP_TEXTID_DARK_RANGER_LYANA1    = 11586,
    GOSSIP_TEXTID_DARK_RANGER_LYANA2    = 11588,
    SPELL_DARK_RANGER_LYANA_KILL_CREDIT = 42799
};

bool GossipHello_npc_dark_ranger_lyana(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_REPORTS_FROM_THE_FIELD) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_DARK_RANGER_LYANA, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DARK_RANGER_LYANA1, pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_dark_ranger_lyana(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXTID_DARK_RANGER_LYANA2, pCreature->GetObjectGuid());
            pCreature->CastSpell(pPlayer, SPELL_DARK_RANGER_LYANA_KILL_CREDIT, true);
            break;
    }

    return true;
}

/*######
## npc_greer_orehammer
######*/

enum
{
    GOSSIP_ITEM_TAXI                        = -3000106,
    GOSSIP_ITEM_GET_BOMBS                   = -3000107,
    GOSSIP_ITEM_FLIGHT                      = -3000108,

    QUEST_MISSION_PLAGUE_THIS               = 11332,
    ITEM_PRECISION_BOMBS                    = 33634,
    TAXI_PATH_PLAGUE_THIS                   = 745,
};

bool GossipHello_npc_greer_orehammer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->GetQuestStatus(QUEST_MISSION_PLAGUE_THIS) == QUEST_STATUS_INCOMPLETE)
    {
        if (!pPlayer->HasItemCount(ITEM_PRECISION_BOMBS, 1, true))
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GET_BOMBS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_FLIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    }

    if (pCreature->isTaxi())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, GOSSIP_ITEM_TAXI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_greer_orehammer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_PRECISION_BOMBS, 10))
                pPlayer->SendNewItem(pItem, 10, true, false);

            pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->ActivateTaxiPathTo(TAXI_PATH_PLAGUE_THIS);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            pPlayer->GetSession()->SendTaxiMenu(pCreature);
            break;
    }

    return true;
}

/*######
## npc_silvermoon_harry
######*/

enum
{
    QUEST_GAMBLING_DEBT         = 11464,

    SAY_AGGRO                   = -1000603,
    SAY_BEATEN                  = -1000604,

    GOSSIP_ITEM_GAMBLING_DEBT   = -3000101,
    GOSSIP_ITEM_PAYING          = -3000102,

    SPELL_BLAST_WAVE            = 15091,
    SPELL_SCORCH                = 50183,

    ITEM_HARRY_DEBT             = 34115,
    FACTION_HOSTILE_SH          = 90,                       // guessed, possibly not correct
};

struct MANGOS_DLL_DECL npc_silvermoon_harryAI : public ScriptedAI
{
    npc_silvermoon_harryAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool m_bHarryBeaten;
    uint32 m_uiBlastWaveTimer;
    uint32 m_uiScorchTimer;
    uint32 m_uiResetBeatenTimer;

    void Reset()
    {
        m_bHarryBeaten = false;

        // timers guessed
        m_uiScorchTimer = 5*IN_MILLISECONDS;
        m_uiBlastWaveTimer = 7*IN_MILLISECONDS;

        m_uiResetBeatenTimer = MINUTE*IN_MILLISECONDS;

        if (m_creature->getFaction() != m_creature->GetCreatureInfo()->faction_A)
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsHostileTo(pAttacker))
            AttackStart(pAttacker);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (uiDamage > m_creature->GetHealth() || (m_creature->GetHealth() - uiDamage)*100 / m_creature->GetMaxHealth() < 20)
        {
            if (Player* pPlayer = pDoneBy->GetCharmerOrOwnerPlayerOrPlayerItself())
            {
                if (!m_bHarryBeaten && pPlayer->GetQuestStatus(QUEST_GAMBLING_DEBT) == QUEST_STATUS_INCOMPLETE)
                {
                    uiDamage = 0;                           // Take 0 damage

                    m_creature->RemoveAllAuras();
                    m_creature->DeleteThreatList();
                    m_creature->CombatStop(true);

                    if (m_creature->getFaction() != m_creature->GetCreatureInfo()->faction_A)
                        m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);

                    DoScriptText(SAY_BEATEN, m_creature);
                    m_bHarryBeaten = true;
                }
            }
        }
    }

    bool IsBeaten()
    {
        return m_bHarryBeaten;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bHarryBeaten)
        {
            if (m_uiResetBeatenTimer < uiDiff)
                EnterEvadeMode();
            else
                m_uiResetBeatenTimer-= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiScorchTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SCORCH);
            m_uiScorchTimer = 10*IN_MILLISECONDS;
        }
        else
            m_uiScorchTimer -= uiDiff;

        if (m_uiBlastWaveTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BLAST_WAVE);
            m_uiBlastWaveTimer = 40*IN_MILLISECONDS;
        }
        else
            m_uiBlastWaveTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_silvermoon_harry(Creature* pCreature)
{
    return new npc_silvermoon_harryAI(pCreature);
}

bool GossipHello_npc_silvermoon_harry(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

    if (pPlayer->GetQuestStatus(QUEST_GAMBLING_DEBT) == QUEST_STATUS_INCOMPLETE)
    {
        if (npc_silvermoon_harryAI* pHarryAI = dynamic_cast<npc_silvermoon_harryAI*>(pCreature->AI()))
        {
            if (!pHarryAI->IsBeaten())
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_GAMBLING_DEBT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            else
                pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_ITEM_PAYING, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_silvermoon_harry(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();

            DoScriptText(SAY_AGGRO, pCreature, pPlayer);
            pCreature->setFaction(FACTION_HOSTILE_SH);
            pCreature->AI()->AttackStart(pPlayer);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            if (!pPlayer->HasItemCount(ITEM_HARRY_DEBT, 1))
            {
                if (Item* pItem = pPlayer->StoreNewItemInInventorySlot(ITEM_HARRY_DEBT, 1))
                {
                    pPlayer->SendNewItem(pItem, 1, true, false);
                    pPlayer->CLOSE_GOSSIP_MENU();
                    pCreature->AI()->EnterEvadeMode();
                }
            }
            break;
    }

    return true;
}

/*######
## quest_jack_likes_his_drink
######*/

enum
{
    SAY_OLGA_0                  = -1999818, // whisper
    SAY_OLGA_1                  = -1999817,
    SAY_JACK_ADAMS_2            = -1999816,
    SAY_OLGA_3                  = -1999815,
    SAY_JACK_ADAMS_4            = -1999814,
    SAY_JACK_ADAMS_5            = -1999813, // faint
    SAY_JACK_ADAMS_6            = -1999812, // wakeup

    NPC_TEXT_OLGA_1             = 12180,
    NPC_JACK_ADAMS              = 24788,

    SPELL_VOMIT                 = 41995,
    ITEM_JACK_ADAMS_DEBT        = 34116,
    QUEST_JACK_LIKES_HIS_DRINK  = 11466,
};

float OlgaWaypoints[2][4] =
{
    {0.0f, -91.81f, -3532.70f, 7.71f},
    {1.0f, -87.23f, -3544.23f, 7.71f}
};

#define GOSSIP_ITEM_OLGA_1 "I'd like to buy Jack a drink. Perhaps something... extra strong."
#define GOSSIP_ITEM_OLGA_2 "Here's a gold, buy yourself something nice."
#define GOSSIP_ITEM_OLGA_3 "Do you really want to bribe Olga?"
#define GOSSIP_ITEM_JACK_ADAMS_1 "<Discreetly search the pirate's pockets for Taruk's payment.>"

struct MANGOS_DLL_DECL npc_olgaAI : public ScriptedAI
{
    npc_olgaAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    bool bEventInProgress;
    bool bInRightPoint;

    uint8 subevent;
    uint32 m_uiEventTimer;
    uint32 m_uiResetJackTimer;
    ObjectGuid JackAdamsGUID;

    void Reset()
    {
        bEventInProgress = false;
        bInRightPoint = true;
        subevent = 0;
        m_uiEventTimer = 5000;
        m_uiResetJackTimer = 30000;
        // restore DB flags (GOSSIP)
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, m_creature->GetCreatureInfo()->npcflag);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == OlgaWaypoints[1][0] || uiPointId == OlgaWaypoints[0][0])
            bInRightPoint = true;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // if not event in progress and Jack is unconscous lets awake him after certain amount of time
        if (!bEventInProgress)
        {
            if (m_uiResetJackTimer <= uiDiff)
            {
                Creature* pJack = m_creature->GetMap()->GetCreature(JackAdamsGUID);
                if (pJack && pJack->isAlive() && pJack->GetByteValue(UNIT_FIELD_BYTES_1,0) == UNIT_STAND_STATE_DEAD)
                    pJack->SetByteValue(UNIT_FIELD_BYTES_1,0,UNIT_STAND_STATE_STAND);
                m_uiResetJackTimer = 30000;
            }else m_uiResetJackTimer -= uiDiff;
        }

        if(bEventInProgress && bInRightPoint)
        {
            if (m_uiEventTimer <= uiDiff)
            {
                // if there is no Jack around or he is dead there is no point to continue
                Creature* pJack = m_creature->GetMap()->GetCreature(JackAdamsGUID);
                if (!pJack || !pJack->isAlive())
                {
                    m_creature->AI()->EnterEvadeMode();
                    return;
                }

                switch(subevent)
                {
                    case 0:
                        DoScriptText(SAY_OLGA_1,m_creature);
                        m_creature->GetMotionMaster()->MovePoint(OlgaWaypoints[1][0],OlgaWaypoints[1][1],OlgaWaypoints[1][2],OlgaWaypoints[1][3]);
                        m_uiEventTimer = 3000;
                        break;
                    case 1:
                        DoScriptText(SAY_JACK_ADAMS_2,pJack,m_creature);
                        m_uiEventTimer = 1000;
                        bInRightPoint = false;
                        break;
                    case 2:
                        m_creature->SetFacingToObject(pJack);
                        m_uiEventTimer = 2000;
                        break;
                    case 3:
                        DoScriptText(SAY_OLGA_3,m_creature,pJack);
                        m_uiEventTimer = 7000;
                        break;
                    case 4:
                        pJack->SetByteValue(UNIT_FIELD_BYTES_1,0,UNIT_STAND_STATE_KNEEL);
                        DoScriptText(SAY_JACK_ADAMS_4,pJack,m_creature);
                        m_uiEventTimer = 5000;
                        break;
                    case 5:
                        pJack->SetByteValue(UNIT_FIELD_BYTES_1,0,UNIT_STAND_STATE_STAND);
                        m_uiEventTimer = 3000;
                        break;
                    case 6:
                        pJack->CastSpell(pJack,SPELL_VOMIT,false);
                        m_uiEventTimer = 3000;
                        break;
                    case 7:
                        DoScriptText(SAY_JACK_ADAMS_5,pJack,m_creature);
                        pJack->SetByteValue(UNIT_FIELD_BYTES_1,0,UNIT_STAND_STATE_DEAD);
                        pJack->SetUInt32Value(UNIT_NPC_FLAGS, m_creature->GetCreatureInfo()->npcflag);
                        m_uiEventTimer = 3000;
                    case 8:
                        m_creature->GetMotionMaster()->MovePoint(OlgaWaypoints[0][0],OlgaWaypoints[0][1],OlgaWaypoints[0][2],OlgaWaypoints[0][3]);
                        bInRightPoint = false;
                        break;
                    case 9:
                        m_creature->SetFacingToObject(pJack);
                        Reset();
                        return;
                    default: break;
                }
                ++ subevent;
            }else m_uiEventTimer -= uiDiff;
        }
    }
};


bool GossipHello_npc_olga(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_JACK_LIKES_HIS_DRINK) == QUEST_STATUS_INCOMPLETE && pPlayer->GetMoney() >= 10000)
    {
        Creature* pJack = GetClosestCreatureWithEntry(pCreature,NPC_JACK_ADAMS,20.0f);
        if (pJack && pJack->isAlive() && pJack->GetByteValue(UNIT_FIELD_BYTES_1,0) == UNIT_STAND_STATE_STAND)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_OLGA_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
            ((npc_olgaAI*)pCreature->AI())->JackAdamsGUID = pJack->GetObjectGuid();
        }
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_olga(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, GOSSIP_ITEM_OLGA_2, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2,GOSSIP_ITEM_OLGA_3, 10000,false);
            pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_OLGA_1, pCreature->GetObjectGuid());
            break;

        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            DoScriptText(SAY_OLGA_0,pCreature,pPlayer);
            pCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
            pPlayer->ModifyMoney(-10000);
            ((npc_olgaAI*)pCreature->AI())->bEventInProgress = true;
            break;
    }
    return true;
}

bool GossipHello_npc_jack_adams(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_JACK_LIKES_HIS_DRINK) == QUEST_STATUS_INCOMPLETE && pPlayer->GetItemCount(ITEM_JACK_ADAMS_DEBT,false) < 1)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_JACK_ADAMS_1, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_jack_adams(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        ItemPosCountVec dest;
        uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, ITEM_JACK_ADAMS_DEBT, 1, false);
        if (msg == EQUIP_ERR_OK)
            pPlayer->StoreNewItem(dest, ITEM_JACK_ADAMS_DEBT, 1, true);
        pCreature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        // 50% chance that jack standup immediately
        if (urand(0, 1) == 0)
        {
            pCreature->SetByteValue(UNIT_FIELD_BYTES_1,0,UNIT_STAND_STATE_STAND);
            DoScriptText(SAY_JACK_ADAMS_6,pCreature,pPlayer);
        }
    }
    return true;
}

CreatureAI* GetAI_npc_olga(Creature* pCreature)
{
    return new npc_olgaAI(pCreature);
}

// Fjord Turkey (24746)

enum
{
    SPELL_TURKEY_MARKER     = 25281,
    SPELL_TURKEY_VENGEANCE  = 25285
};

struct MANGOS_DLL_DECL npc_fjord_turkeyAI : public ScriptedAI
{
    npc_fjord_turkeyAI(Creature* pCreature) : ScriptedAI(pCreature) { }

    void Reset() { }

    void JustDied(Unit* pKiller)
    {
        Player* plr = pKiller->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!plr)
            return;

        if (SpellAuraHolder* holder = plr->GetSpellAuraHolder(SPELL_TURKEY_MARKER, plr->GetObjectGuid()))
        {
            if (holder->GetStackAmount() + 1 >= 15)
            {
                plr->RemoveSpellAuraHolder(holder);
                plr->CastSpell(plr, SPELL_TURKEY_VENGEANCE, true);
                return;
            }

            int32 duration = holder->GetAuraDuration();
            holder->ModStackAmount(1);
            holder->SetAuraDuration(duration);
        }
        else
            plr->CastSpell(plr, SPELL_TURKEY_MARKER, true);
    }
};

CreatureAI* GetAI_npc_fjord_turkey(Creature* pCreature)
{
    return new npc_fjord_turkeyAI(pCreature);
}

void AddSC_howling_fjord()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_ancient_male_vrykul";
    pNewScript->GetAI = &GetAI_npc_ancient_male_vrykul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_ancient_male_vrykul";
    pNewScript->pAreaTrigger = &AreaTrigger_at_ancient_male_vrykul;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_daegarn";
    pNewScript->GetAI = &GetAI_npc_daegarn;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_daegarn;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_deathstalker_razael";
    pNewScript->pGossipHello = &GossipHello_npc_deathstalker_razael;
    pNewScript->pGossipSelect = &GossipSelect_npc_deathstalker_razael;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dark_ranger_lyana";
    pNewScript->pGossipHello = &GossipHello_npc_dark_ranger_lyana;
    pNewScript->pGossipSelect = &GossipSelect_npc_dark_ranger_lyana;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_greer_orehammer";
    pNewScript->pGossipHello = &GossipHello_npc_greer_orehammer;
    pNewScript->pGossipSelect = &GossipSelect_npc_greer_orehammer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_silvermoon_harry";
    pNewScript->GetAI = &GetAI_npc_silvermoon_harry;
    pNewScript->pGossipHello = &GossipHello_npc_silvermoon_harry;
    pNewScript->pGossipSelect = &GossipSelect_npc_silvermoon_harry;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_olga";
    pNewScript->GetAI = &GetAI_npc_olga;
    pNewScript->pGossipHello = &GossipHello_npc_olga;
    pNewScript->pGossipSelect = &GossipSelect_npc_olga;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_jack_adams";
    pNewScript->pGossipHello = &GossipHello_npc_jack_adams;
    pNewScript->pGossipSelect = &GossipSelect_npc_jack_adams;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fjord_turkey";
    pNewScript->GetAI = &GetAI_npc_fjord_turkey;
    pNewScript->RegisterSelf();
}
