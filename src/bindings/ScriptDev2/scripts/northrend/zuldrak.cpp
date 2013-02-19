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
SDName: Zuldrak
SD%Complete: 100
SDComment: Quest support: 12934.
SDCategory: Zuldrak
EndScriptData */

/* ContentData
npc_gurgthock
EndContentData */

#include "precompiled.h"
#include "BSW_ai.h"
/*######
## npc_gurgthock
######*/

enum
{
    QUEST_FROM_BEYOND = 12934,

    NPC_AZBARIN       = 30026,
    NPC_DUKE_SINGEN   = 30019,
    NPC_ERATHIUS      = 30025,
    NPC_GARGORAL      = 30024
};

static float m_afSpawnLocation[] = {5768.71f, -2969.29f, 273.816f};
static uint32 m_auiBosses[] = {NPC_AZBARIN, NPC_DUKE_SINGEN, NPC_ERATHIUS, NPC_GARGORAL};

struct MANGOS_DLL_DECL npc_gurgthockAI : public ScriptedAI
{
    npc_gurgthockAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    void SetPlayer(Player* pPlayer)
    {
        m_playerGuid = pPlayer->GetObjectGuid();
    }

    void Reset()
    {
        m_playerGuid.Clear();
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        uint32 uiEntry = pSummoned->GetEntry();
        for(uint8 i = 0; i < 4; ++i)
        {
            if (uiEntry == m_auiBosses[i])
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                    pPlayer->GroupEventHappens(QUEST_FROM_BEYOND, m_creature);

                m_playerGuid.Clear();
                return;
            }
        }
    }
};

bool QuestAccept_npc_gurgthock(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_FROM_BEYOND)
    {
        pCreature->SummonCreature(m_auiBosses[urand(0, 3)], m_afSpawnLocation[0], m_afSpawnLocation[1], m_afSpawnLocation[2], 0.0f, TEMPSUMMON_TIMED_OOC_OR_CORPSE_DESPAWN, 600000);

        if (npc_gurgthockAI* pGurthockAI = dynamic_cast<npc_gurgthockAI*>(pCreature->AI()))
            pGurthockAI->SetPlayer(pPlayer);
    }
    return true;
}

CreatureAI* GetAI_npc_gurgthock(Creature* pCreature)
{
    return new npc_gurgthockAI(pCreature);
}

// quest 12665
static WayPoints kittenWaypoints[] = 
{
    WayPoints(0,  5350.657227f, -3882.375977f, 372.701538f),
    WayPoints(1,  5353.090820f, -3891.043701f, 365.277496f),
    WayPoints(2,  5357.884277f, -3970.127441f, 362.510712f),
    WayPoints(3,  5358.240723f, -3982.446289f, 373.571594f),
    WayPoints(4,  5366.723633f, -4036.742432f, 373.617584f),
    WayPoints(5,  5368.980469f, -4042.177002f, 369.792877f),
    WayPoints(6,  5395.889648f, -4089.317627f, 367.040100f),    // say careful
    WayPoints(7,  5454.609863f, -4193.041016f, 363.633362f),
    WayPoints(8,  5507.734375f, -4300.658691f, 363.564972f),
    WayPoints(9,  5536.788574f, -4439.657715f, 368.412231f),
    WayPoints(10, 5556.047363f, -4437.060547f, 368.461334f),
    WayPoints(11, 5564.375488f, -4436.670898f, 376.743622f),
    WayPoints(12, 5606.885742f, -4437.119629f, 375.017975f),
    WayPoints(13, 5619.278809f, -4437.432129f, 387.730469f),
    WayPoints(14, 5657.041504f, -4378.394531f, 386.030273f),    // say dead & spotted
    WayPoints(15, 5622.115723f, -4376.135254f, 388.641663f),
    WayPoints(16, 5548.459961f, -4352.711426f, 368.341278f),    // jump here
    WayPoints(17, 5456.998535f, -4293.076660f, 362.109192f),    // say safe
    WayPoints(18, 5454.424316f, -4220.421875f, 366.033875f),
    WayPoints(19, 5436.777344f, -4136.605957f, 363.592285f),
    WayPoints(20, 5435.850098f, -4082.221191f, 364.994812f),
    WayPoints(21, 5439.071289f, -4045.930664f, 367.724579f),
    WayPoints(22, 5438.240234f, -4038.591064f, 373.358154f),
    WayPoints(23, 5409.894043f, -3983.159912f, 373.748596f),
    WayPoints(24, 5404.242676f, -3972.014404f, 362.793823f),
    WayPoints(25, 5359.875488f, -3886.623779f, 364.212982f),
    WayPoints(26, 5354.668945f, -3879.203369f, 372.503876f),
    WayPoints(27, 5342.416504f, -3861.741211f, 371.346100f),
    WayPoints(28, 5324.147949f, -3798.950928f, 371.937531f)
};

enum
{
    SPELL_STEALTH       = 52188,
    QUEST_KITTEN        = 12665,
    MAX_KITTEN_WAYPOINT = 29,

    WHISPER_KITTEN_1    = -2000000,
    WHISPER_KITTEN_2    = -2000001,
    WHISPER_KITTEN_3    = -2000002,
    WHISPER_KITTEN_4    = -2000003,
    WHISPER_KITTEN_5    = -2000004,
    WHISPER_KITTEN_6    = -2000005,
};

struct MANGOS_DLL_DECL npc_harkoa_kittenAI : public ScriptedAI
{
    npc_harkoa_kittenAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    Player* m_player;
    uint8 uiStage;
    uint32 uiStageDelay;
    bool bIsMoving;

    void Reset()
    {
        m_player = NULL;
        uiStage = 0;
        uiStageDelay = 1000;
        bIsMoving = false;
        m_creature->SetWalk(false);
        m_creature->CastSpell(m_creature, SPELL_STEALTH, true);
    }

    void PassengerBoarded(Unit* who, int8 seatId, bool apply)
    {
        if (who->GetTypeId() != TYPEID_PLAYER)
            return;

        Player* pWho = (Player*)who;

        if (apply)
        {
            m_player = pWho;
            DoScriptText(WHISPER_KITTEN_1, m_creature, pWho);
            pWho->CastSpell(pWho, SPELL_STEALTH, true);
            pWho->SetClientControl(pWho, 0);
            m_creature->setFaction(190);
            pWho->setFaction(35);
        }
        else
        {
            pWho->RemoveAurasDueToSpell(SPELL_STEALTH);
            pWho->setFactionForRace(pWho->getRace());
            m_player = NULL;
        }
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData)
    {
        bIsMoving = false;

        if (uiData == 14)
            DoScriptText(WHISPER_KITTEN_3, m_creature, m_player);
        else if (uiData == 17)
            DoScriptText(WHISPER_KITTEN_5, m_creature, m_player);
        else if (uiData == 28)
        {
            DoScriptText(WHISPER_KITTEN_6, m_creature, m_player);
            m_player->AreaExploredOrEventHappens(QUEST_KITTEN);
            m_creature->ForcedDespawn(2000);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        switch (uiStage)
        {
            case 0:
            {
                // Altar of Har'koa
                if (m_creature->GetAreaId() != 4322)
                    break;

                if (uiStageDelay)
                {
                    if (uiStageDelay < uiDiff)
                    {
                        uiStageDelay = 1000;
                        if (m_player)
                        {
                            uiStageDelay = 3000;
                            ++uiStage;
                        }
                    }
                    else
                        uiStageDelay -= uiDiff;
                }
                break;
            }
            default:
            {
                if (!m_player || bIsMoving)
                    break;

                if (uiStage - 1 < MAX_KITTEN_WAYPOINT)
                {
                    if (uiStageDelay > uiDiff)
                    {
                        uiStageDelay -= uiDiff;
                        break;
                    }
                    else
                        uiStageDelay = 0;

                    if (m_creature->GetVehicleKit() && !m_creature->GetVehicleKit()->GetPassenger(0))
                    {
                        m_player = NULL;
                        m_creature->ForcedDespawn();
                        break;
                    }

                    if (uiStage - 1 == 16)
                        m_creature->GetMotionMaster()->MoveJump(kittenWaypoints[uiStage - 1].x, kittenWaypoints[uiStage - 1].y, kittenWaypoints[uiStage - 1].z, 75.0f, 1.0f, uiStage - 1);
                    else
                        m_creature->GetMotionMaster()->MovePoint(uiStage - 1, kittenWaypoints[uiStage - 1].x, kittenWaypoints[uiStage - 1].y, kittenWaypoints[uiStage - 1].z);

                    bIsMoving = true;

                    if (uiStage - 1 == 9)
                        DoScriptText(WHISPER_KITTEN_2, m_creature, m_player);
                    else if (uiStage - 1 == 14)
                        uiStageDelay = 7000;
                    else if (uiStage - 1 == 15)
                        DoScriptText(WHISPER_KITTEN_4, m_creature, m_player);
                    else if (uiStage - 1 == 17)
                        uiStageDelay = 3000;
                    ++uiStage;
                }
                else
                {
                    m_player = NULL;
                    m_creature->ForcedDespawn();
                }
                break;
            }
        }
    }
};

CreatureAI* GetAI_npc_harkoa_kitten(Creature* pCreature)
{
    return new npc_harkoa_kittenAI(pCreature);
}

// quest 12668
enum
{
    SPELL_SOUL_FONT_VOID                        = 52222,
    QUEST_FOUNDATION_FOR_REVENGE_QUEST_CREDIT   = 28713

};
struct MANGOS_DLL_DECL npc_quetzlun_trollAI : public ScriptedAI
{
    npc_quetzlun_trollAI(Creature* pCreature) : ScriptedAI(pCreature) { }

    void Reset() { }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
        {
            if (pDoneBy)
            {
                if (Player* plr = pDoneBy->GetCharmerOrOwnerPlayerOrPlayerItself())
                    if (m_creature->HasAura(SPELL_SOUL_FONT_VOID))
                        plr->KilledMonsterCredit(QUEST_FOUNDATION_FOR_REVENGE_QUEST_CREDIT);
            }
        }
    }
};

CreatureAI* GetAI_npc_quetzlun_troll(Creature* pCreature)
{
    return new npc_quetzlun_trollAI(pCreature);
}

// quest 12674
enum
{
    SPELL_QUETZLUNS_HEX = 52250,
    SPELL_HEX_OF_AIR    = 52249,
    SPELL_HEX_OF_FIRE   = 52278,
    SPELL_HEX_OF_FROST  = 52287,

    NPC_PRIEST_MUFUNU   = 28752,
    NPC_PRIEST_TUATUA   = 28754,
    NPC_PRIEST_HAWINII  = 28756,

    KILL_CREDIT_MUFUNU  = 28753,
    KILL_CREDIT_TUATUA  = 28755,
    KILL_CREDIT_HAWINII = 28757,

    SAY_MUFUNU          = -2000006,
    SAY_TUATUA          = -2000007,
    SAY_HAWINII         = -2000008
};
struct MANGOS_DLL_DECL npc_quetzlun_priestAI : public ScriptedAI
{
    npc_quetzlun_priestAI(Creature* pCreature) : ScriptedAI(pCreature) { }

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage > m_creature->GetHealth())
        {
            if (pDoneBy)
            {
                if (Player* plr = pDoneBy->GetCharmerOrOwnerPlayerOrPlayerItself())
                {
                    if (m_creature->HasAura(SPELL_QUETZLUNS_HEX))
                    {
                        uint32 credit = 0;
                        uint32 spellId = 0;
                        uint32 textId = 0;
                        if (m_creature->GetEntry() == NPC_PRIEST_MUFUNU)
                        {
                            credit = KILL_CREDIT_MUFUNU;
                            spellId = SPELL_HEX_OF_AIR;
                            textId = SAY_MUFUNU;
                        }
                        else if (m_creature->GetEntry() == NPC_PRIEST_TUATUA)
                        {
                            credit = KILL_CREDIT_TUATUA;
                            spellId = SPELL_HEX_OF_FIRE;
                            textId = SAY_TUATUA;
                        }
                        else if (m_creature->GetEntry() == NPC_PRIEST_HAWINII)
                        {
                            credit = KILL_CREDIT_HAWINII;
                            spellId = SPELL_HEX_OF_FROST;
                            textId = SAY_HAWINII;
                        }
                        else
                            return;

                        uiDamage = m_creature->GetHealth() - 1;
                        DoResetThreat();
                        m_creature->RemoveAurasDueToSpell(SPELL_QUETZLUNS_HEX);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                        m_creature->ForcedDespawn(10000);
                        m_creature->CastSpell(m_creature, spellId, true);

                        plr->KilledMonsterCredit(credit);
                        DoScriptText(textId, m_creature, plr);
                    }
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_quetzlun_priest(Creature* pCreature)
{
    return new npc_quetzlun_priestAI(pCreature);
}

// Mam'toth Disciple (28861)
enum
{
    QUEST_WOOLY_JUSTICE_KILL_CREDIT = 28876
};

struct MANGOS_DLL_DECL npc_mamtoth_discipleAI : public ScriptedAI
{
    npc_mamtoth_discipleAI(Creature* pCreature) : ScriptedAI(pCreature) { }

    void Reset() { }

    void JustDied(Unit* pKiller)
    {
        if (!pKiller->GetObjectGuid().IsVehicle())
            return;

        Player* plr = pKiller->GetCharmerOrOwnerPlayerOrPlayerItself();
        if (!plr)
            return;

        plr->KilledMonsterCredit(QUEST_WOOLY_JUSTICE_KILL_CREDIT);
    }
};

CreatureAI* GetAI_npc_mamtoth_disciple(Creature* pCreature)
{
    return new npc_mamtoth_discipleAI(pCreature);
}

void AddSC_zuldrak()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_gurgthock";
    pNewScript->GetAI = &GetAI_npc_gurgthock;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_gurgthock;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_harkoa_kitten";
    pNewScript->GetAI = &GetAI_npc_harkoa_kitten;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_quetzlun_troll";
    pNewScript->GetAI = &GetAI_npc_quetzlun_troll;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_quetzlun_priest";
    pNewScript->GetAI = &GetAI_npc_quetzlun_priest;
    pNewScript->RegisterSelf();
    pNewScript = new Script;

    pNewScript->Name = "npc_mamtoth_disciple";
    pNewScript->GetAI = &GetAI_npc_mamtoth_disciple;
    pNewScript->RegisterSelf();
}
