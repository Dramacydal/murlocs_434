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
SDName: GO_Scripts
SD%Complete: 100
SDComment: Quest support: 5088, 5097, 5098, 5381, 6481, 10990, 10991, 10992, 12557, 14092/14076. Barov_journal->Teaches spell 26089
SDCategory: Game Objects
EndScriptData */

/* ContentData
go_cat_figurine (the "trap" version of GO, two different exist)
go_barov_journal
go_ethereum_prison
go_ethereum_stasis
go_mysterious_snow_mound
go_resonite_cask
go_sacred_fire_of_life
go_shrine_of_the_birds
go_tablet_of_madness
go_tele_to_dalaran_crystal
go_tele_to_violet_stand
go_andorhal_tower
go_scourge_enclosure
go_lab_work_reagents
go_hand_of_iruxos_crystal
go_org_portal
go_sw_portal
go_rusty_cage
go_arcane_prison
EndContentData */

#include "precompiled.h"

/*######
## go_cat_figurine
######*/

enum
{
    SPELL_SUMMON_GHOST_SABER    = 5968,
};

bool GOUse_go_cat_figurine(Player* pPlayer, GameObject* pGo)
{
    pPlayer->CastSpell(pPlayer, SPELL_SUMMON_GHOST_SABER, true);
    return false;
}

/*######
## go_barov_journal
######*/

enum
{
    SPELL_TAILOR_FELCLOTH_BAG = 26086,
    SPELL_LEARN_FELCLOTH_BAG  = 26095
};

bool GOUse_go_barov_journal(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->HasSkill(SKILL_TAILORING) && pPlayer->GetBaseSkillValue(SKILL_TAILORING) >= 280 && !pPlayer->HasSpell(SPELL_TAILOR_FELCLOTH_BAG))
    {
        pPlayer->CastSpell(pPlayer, SPELL_LEARN_FELCLOTH_BAG, false);
    }
    return true;
}

/*######
## go_ethereum_prison
######*/

enum
{
    FACTION_LC     = 1011,
    FACTION_SHAT   = 935,
    FACTION_CE     = 942,
    FACTION_CON    = 933,
    FACTION_KT     = 989,
    FACTION_SPOR   = 970,

    SPELL_REP_LC   = 39456,
    SPELL_REP_SHAT = 39457,
    SPELL_REP_CE   = 39460,
    SPELL_REP_CON  = 39474,
    SPELL_REP_KT   = 39475,
    SPELL_REP_SPOR = 39476
};

const uint32 uiNpcPrisonEntry[] =
{
    22810, 22811, 22812, 22813, 22814, 22815,               //good guys
    20783, 20784, 20785, 20786, 20788, 20789, 20790         //bad guys
};

bool GOUse_go_ethereum_prison(Player* pPlayer, GameObject* pGo)
{
    uint8 uiRandom = urand(0, (sizeof(uiNpcPrisonEntry) / sizeof(uint32)) -1);

    if (Creature* pCreature = pPlayer->SummonCreature(uiNpcPrisonEntry[uiRandom],
        pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetAngle(pPlayer),
        TEMPSUMMON_TIMED_OOC_DESPAWN, 30000))
    {
        if (!pCreature->IsHostileTo(pPlayer))
        {
            uint32 uiSpell = 0;

            if (FactionTemplateEntry const* pFaction = pCreature->getFactionTemplateEntry())
            {
                switch(pFaction->faction)
                {
                    case FACTION_LC:   uiSpell = SPELL_REP_LC;   break;
                    case FACTION_SHAT: uiSpell = SPELL_REP_SHAT; break;
                    case FACTION_CE:   uiSpell = SPELL_REP_CE;   break;
                    case FACTION_CON:  uiSpell = SPELL_REP_CON;  break;
                    case FACTION_KT:   uiSpell = SPELL_REP_KT;   break;
                    case FACTION_SPOR: uiSpell = SPELL_REP_SPOR; break;
                }

                if (uiSpell)
                    pCreature->CastSpell(pPlayer,uiSpell,false);
                else
                    error_log("SD2: go_ethereum_prison summoned creature (entry %u) but faction (%u) are not expected by script.",pCreature->GetEntry(),pCreature->getFaction());
            }
        }
    }

    return false;
}

/*######
## go_ethereum_stasis
######*/

const uint32 uiNpcStasisEntry[] =
{
    22825, 20888, 22827, 22826, 22828
};

bool GOUse_go_ethereum_stasis(Player* pPlayer, GameObject* pGo)
{
    uint8 uiRandom = urand(0, (sizeof(uiNpcStasisEntry) / sizeof(uint32)) -1);

    pPlayer->SummonCreature(uiNpcStasisEntry[uiRandom],
        pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pGo->GetAngle(pPlayer),
        TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);

    return false;
}

/*######
## go_gilded_brazier
######*/

enum
{
    NPC_STILLBLADE = 17716,
};

bool GOUse_go_gilded_brazier(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
    {
        if (Creature* pCreature = pPlayer->SummonCreature(NPC_STILLBLADE, 8087.632f, -7542.740f, 151.568f, 0.122f, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000))
            pCreature->AI()->AttackStart(pPlayer);
    }

    return true;
}

/*######
## go_jump_a_tron
######*/

enum
{
    SPELL_JUMP_A_TRON = 33382,
    NPC_JUMP_A_TRON   = 19041
};

bool GOUse_go_jump_a_tron(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pCreature = GetClosestCreatureWithEntry(pGo, NPC_JUMP_A_TRON, INTERACTION_DISTANCE))
        pCreature->CastSpell(pPlayer, SPELL_JUMP_A_TRON, false);

    return false;
}

/*######
## go_mysterious_snow_mound
######*/

enum
{
    SPELL_SUMMON_DEEP_JORMUNGAR     = 66510,
    SPELL_SUMMON_MOLE_MACHINE       = 66492,
    SPELL_SUMMON_MARAUDER           = 66491,
};

bool GOUse_go_mysterious_snow_mound(Player* pPlayer, GameObject* pGo)
{
    if (urand(0,1))
    {
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_DEEP_JORMUNGAR, true);
    }
    else
    {
        // This is basically wrong, but added for support.
        // Mole machine would summon, along with unkonwn GO (a GO trap?) and then
        // the npc would summon with base of that location.
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_MOLE_MACHINE, true);
        pPlayer->CastSpell(pPlayer, SPELL_SUMMON_MARAUDER, true);
    }

    pGo->SetLootState(GO_JUST_DEACTIVATED);
    return true;
}

/*######
## go_resonite_cask
######*/

enum
{
    NPC_GOGGEROC = 11920
};

bool GOUse_go_resonite_cask(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pGO->SummonCreature(NPC_GOGGEROC, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 300000);

    return false;
}

/*######
## go_sacred_fire_of_life
######*/

enum
{
    NPC_ARIKARA = 10882,
};

bool GOUse_go_sacred_fire_of_life(Player* pPlayer, GameObject* pGO)
{
    if (pGO->GetGoType() == GAMEOBJECT_TYPE_GOOBER)
        pPlayer->SummonCreature(NPC_ARIKARA, -5008.338f, -2118.894f, 83.657f, 0.874f, TEMPSUMMON_TIMED_OOC_DESPAWN, 30000);

    return true;
}

/*######
## go_shrine_of_the_birds
######*/

enum
{
    NPC_HAWK_GUARD   = 22992,
    NPC_EAGLE_GUARD  = 22993,
    NPC_FALCON_GUARD = 22994,
    GO_SHRINE_HAWK   = 185551,
    GO_SHRINE_EAGLE  = 185547,
    GO_SHRINE_FALCON = 185553
};

bool GOUse_go_shrine_of_the_birds(Player* pPlayer, GameObject* pGo)
{
    uint32 uiBirdEntry = 0;

    float fX,fY,fZ;
    pGo->GetClosePoint(fX, fY, fZ, pGo->GetObjectBoundingRadius(), INTERACTION_DISTANCE);

    switch(pGo->GetEntry())
    {
        case GO_SHRINE_HAWK:
            uiBirdEntry = NPC_HAWK_GUARD;
            break;
        case GO_SHRINE_EAGLE:
            uiBirdEntry = NPC_EAGLE_GUARD;
            break;
        case GO_SHRINE_FALCON:
            uiBirdEntry = NPC_FALCON_GUARD;
            break;
    }

    if (uiBirdEntry)
        pPlayer->SummonCreature(uiBirdEntry, fX, fY, fZ, pGo->GetOrientation(), TEMPSUMMON_TIMED_OOC_DESPAWN, 60000);

    return false;
}

/*######
## go_tablet_of_madness
######*/

enum
{
    SPELL_ALCHEMY_GURUBASHI_MOJO_MADNESS = 24266,
    SPELL_LEARN_GURUBASHI_MOJO_MADNESS   = 24267
};

bool GOUse_go_tablet_of_madness(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->HasSkill(SKILL_ALCHEMY) && pPlayer->GetSkillValue(SKILL_ALCHEMY) >= 300 && !pPlayer->HasSpell(SPELL_ALCHEMY_GURUBASHI_MOJO_MADNESS))
        pPlayer->CastSpell(pPlayer, SPELL_LEARN_GURUBASHI_MOJO_MADNESS, false);

    return true;
}

/*######
## go_tele_to_dalaran_crystal
######*/

enum
{
    QUEST_LEARN_LEAVE_RETURN = 12790,
    QUEST_TELE_CRYSTAL_FLAG  = 12845
};

bool GOUse_go_tele_to_dalaran_crystal(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestRewardStatus(QUEST_TELE_CRYSTAL_FLAG))
        return false;

    //TODO: must send error message (what kind of message? On-screen?)
    return true;
}

/*######
## go_tele_to_violet_stand
######*/

bool GOUse_go_tele_to_violet_stand(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestRewardStatus(QUEST_LEARN_LEAVE_RETURN) || pPlayer->GetQuestStatus(QUEST_LEARN_LEAVE_RETURN) == QUEST_STATUS_INCOMPLETE)
        return false;

    return true;
}

enum
{
    NPC_ZELEMAR_THE_WRATHFULL = 17830,
    SAY_AGGRO                 = -1000579
};

float Position[4] = {-327.99f, 221.74f, -20.31f, 3.87f};

bool GOUse_go_blood_filled_orb(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pZelemar = pGo->SummonCreature(NPC_ZELEMAR_THE_WRATHFULL, Position[0], Position[1], Position[2], Position[3], TEMPSUMMON_TIMED_OOC_DESPAWN, 30000))
    {
        DoScriptText(SAY_AGGRO, pZelemar);
        pZelemar->AI()->AttackStart(pPlayer);
    }
    return false;
}

/*######
## go_andorhal_tower
######*/

enum
{
    QUEST_ALL_ALONG_THE_WATCHTOWERS_ALLIANCE = 5097,
    QUEST_ALL_ALONG_THE_WATCHTOWERS_HORDE    = 5098,
    NPC_ANDORHAL_TOWER_1                     = 10902,
    NPC_ANDORHAL_TOWER_2                     = 10903,
    NPC_ANDORHAL_TOWER_3                     = 10904,
    NPC_ANDORHAL_TOWER_4                     = 10905,
    GO_ANDORHAL_TOWER_1                      = 176094,
    GO_ANDORHAL_TOWER_2                      = 176095,
    GO_ANDORHAL_TOWER_3                      = 176096,
    GO_ANDORHAL_TOWER_4                      = 176097
};

bool GOUse_go_andorhal_tower(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_ALL_ALONG_THE_WATCHTOWERS_ALLIANCE) == QUEST_STATUS_INCOMPLETE || pPlayer->GetQuestStatus(QUEST_ALL_ALONG_THE_WATCHTOWERS_HORDE) == QUEST_STATUS_INCOMPLETE)
    {
        uint32 uiKillCredit = 0;
        switch(pGo->GetEntry())
        {
            case GO_ANDORHAL_TOWER_1:   uiKillCredit = NPC_ANDORHAL_TOWER_1;   break;
            case GO_ANDORHAL_TOWER_2:   uiKillCredit = NPC_ANDORHAL_TOWER_2;   break;
            case GO_ANDORHAL_TOWER_3:   uiKillCredit = NPC_ANDORHAL_TOWER_3;   break;
            case GO_ANDORHAL_TOWER_4:   uiKillCredit = NPC_ANDORHAL_TOWER_4;   break;
        }
        if (uiKillCredit)
            pPlayer->KilledMonsterCredit(uiKillCredit);
    }
    return true;
}

/*######
## go_scourge_enclosure
######*/

enum
{
    SPELL_GYMER_LOCK_EXPLOSION      = 55529,
    NPC_GYMER_LOCK_DUMMY            = 29928

};

bool GOUse_go_scourge_enclosure(Player* pPlayer, GameObject* pGo)
{
    std::list<Creature*> m_lResearchersList;
    GetCreatureListWithEntryInGrid(m_lResearchersList, pGo, NPC_GYMER_LOCK_DUMMY, 15.0f);
    if (!m_lResearchersList.empty())
    {
        for(std::list<Creature*>::iterator itr = m_lResearchersList.begin(); itr != m_lResearchersList.end(); ++itr)
        {
            (*itr)->CastSpell((*itr),SPELL_GYMER_LOCK_EXPLOSION,true);
        }
    }
    pPlayer->KilledMonsterCredit(NPC_GYMER_LOCK_DUMMY);
    return true;
}

/*######
## go_lab_work_reagents
######*/

enum
{
    QUEST_LAB_WORK                          = 12557,

    SPELL_WIRHERED_BATWING_KILL_CREDIT      = 51226,
    SPELL_MUDDY_MIRE_MAGGOT_KILL_CREDIT     = 51227,
    SPELL_AMBERSEED_KILL_CREDIT             = 51228,
    SPELL_CHILLED_SERPENT_MUCUS_KILL_CREDIT = 51229,

    GO_AMBERSEED                            = 190459,
    GO_CHILLED_SERPENT_MUCUS                = 190462,
    GO_WITHERED_BATWING                     = 190473,
    GO_MUDDY_MIRE_MAGGOTS                   = 190478,
};

bool GOUse_go_lab_work_reagents(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_LAB_WORK) == QUEST_STATUS_INCOMPLETE)
    {
        uint32 uiCreditSpellId = 0;
        switch (pGo->GetEntry())
        {
            case GO_AMBERSEED:              uiCreditSpellId = SPELL_AMBERSEED_KILL_CREDIT; break;
            case GO_CHILLED_SERPENT_MUCUS:  uiCreditSpellId = SPELL_CHILLED_SERPENT_MUCUS_KILL_CREDIT; break;
            case GO_WITHERED_BATWING:       uiCreditSpellId = SPELL_WIRHERED_BATWING_KILL_CREDIT; break;
            case GO_MUDDY_MIRE_MAGGOTS:     uiCreditSpellId = SPELL_MUDDY_MIRE_MAGGOT_KILL_CREDIT; break;
        }

        if (uiCreditSpellId)
            pPlayer->CastSpell(pPlayer, uiCreditSpellId, true);
    }

    return false;
}

/*######
## go_hand_of_iruxos_crystal
######*/

/* TODO
 * Actually this script is extremely vague, but as long as there is no valid information
 * hidden in some dark places, this will be the best we can do here :(
 * Do not consider this a well proven script.
 */

enum
{
    // QUEST_HAND_OF_IRUXOS     = 5381,
    NPC_IRUXOS                  = 11876,
};

bool GOUse_go_hand_of_iruxos_crystal(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pIruxos = pGo->SummonCreature(NPC_IRUXOS, 0.0f, 0.0f, 0.0f, pPlayer->GetOrientation() + M_PI_F, TEMPSUMMON_TIMED_OOC_DESPAWN, 20000))
        pIruxos->AI()->AttackStart(pPlayer);

    return false;
}

/*######
## go_org_portal
######*/

bool GOUse_go_org_portal(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(13189) == QUEST_STATUS_COMPLETE || pPlayer->GetQuestStatus(13189) == QUEST_STATUS_INCOMPLETE)
         pPlayer->CastSpell(pPlayer, 17609, true);

    return true;
}

/*######
## go_sw_portal
######*/

bool GOUse_go_sw_portal(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(13188) == QUEST_STATUS_COMPLETE || pPlayer->GetQuestStatus(13188) == QUEST_STATUS_INCOMPLETE)
         pPlayer->CastSpell(pPlayer, 17334, true);

    return true;
}

/*######
## go_power_converter
######*/

/* TODO
 * Ranger: THIS IS NOT TRUE WAY! But working!
 */

enum
{
    QUEST_PICKING_UP_SOME_POWER_CONVERTERS   = 10584,
    NPC_ELECTROMENTAL                        = 21729,
};

bool GOUse_go_power_converter(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetQuestStatus(QUEST_PICKING_UP_SOME_POWER_CONVERTERS) == QUEST_STATUS_INCOMPLETE)
        if (Creature* pElectromental = pGo->SummonCreature(NPC_ELECTROMENTAL, 0.0f, 0.0f, 0.0f, pPlayer->GetOrientation() + M_PI_F, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 20000))
            pElectromental->AI()->AttackStart(pPlayer);

    return false;
}

/*###
## go_wind_stone
###*/

enum WSSpells
{
    SPELL_PUNISHMENT = 24803,
    SPELL_SPAWN_IN = 25035,

    AURA_TWILIGHT_SET = 24746,
    AURA_MEDALLION = 24748,
    AURA_RING = 24782,

    SPELL_TEMPLAR_RANDOM = 24745,
    SPELL_TEMPLAR_FIRE = 24747,
    SPELL_TEMPLAR_AIR = 24757,
    SPELL_TEMPLAR_EARTH = 24759,
    SPELL_TEMPLAR_WATER = 24761,

    SPELL_DUKE_RANDOM = 24762,
    SPELL_DUKE_FIRE = 24766,
    SPELL_DUKE_AIR = 24769,
    SPELL_DUKE_EARTH = 24771,
    SPELL_DUKE_WATER = 24773,

    SPELL_ROYAL_RANDOM = 24785,
    SPELL_ROYAL_FIRE = 24787,
    SPELL_ROYAL_AIR = 24791,
    SPELL_ROYAL_EARTH = 24792,
    SPELL_ROYAL_WATER = 24793
};

enum WSGossip
{
    GOSSIPID_LESSER_WS = 6540,
    GOSSIPID_WS = 6542,
    GOSSIPID_GREATER_WS = 6543
};

enum WSCreatures
{
    NPC_TEMPLAR_FIRE = 15209,
    NPC_TEMPLAR_WATER = 15211,
    NPC_TEMPLAR_AIR = 15212,
    NPC_TEMPLAR_EARTH = 15307,

    NPC_DUKE_FIRE = 15206,
    NPC_DUKE_WATER = 15207,
    NPC_DUKE_EARTH = 15208,
    NPC_DUKE_AIR = 15220,

    NPC_ROYAL_FIRE = 15203,
    NPC_ROYAL_AIR = 15204,
    NPC_ROYAL_EARTH = 15205,
    NPC_ROYAL_WATER = 15305
};

enum WSItems
{
    ITEM_TEMPLAR_FIRE = 20416,
    ITEM_TEMPLAR_EARTH =  20419,
    ITEM_TEMPLAR_WATER = 20420,
    ITEM_TEMPLAR_AIR = 20418,

    ITEM_DUKE_FIRE = 20432,
    ITEM_DUKE_EARTH =  20435,
    ITEM_DUKE_WATER = 20436,
    ITEM_DUKE_AIR = 20433,

    ITEM_ROYAL_FIRE = 20447,
    ITEM_ROYAL_EARTH =  20449,
    ITEM_ROYAL_WATER = 20450,
    ITEM_ROYAL_AIR = 20448,
};

enum WS
{
    TEMPLAR = 0,
    DUKE = 1,
    ROYAL = 2,

    FIRE = 0x1,
    WATER = 0x2,
    EARTH = 0x4,
    AIR = 0x8
};

enum WSTexts
{
    SAY_TEMPLAR_AGGRO = 0,
    SAY_DUKE_AGGRO = 0,
    YELL_ROYAL_AGGRO = 0
};

#define GOSSIP_TEMPLAR_RANDOM "I am no cultist, you monster! Come to me and face your destruction!"
#define GOSSIP_TEMPLAR_FIRE "Crimson Templar! I hold your signet! Heed my call!"
#define GOSSIP_TEMPLAR_EARTH "Earthen Templar! I hold your signet! Heed my call!"
#define GOSSIP_TEMPLAR_AIR "Hoary Templar! I hold your signet! Heed my call!"
#define GOSSIP_TEMPLAR_WATER "Azure Templar! I hold your signet! Heed my call!"

#define GOSSIP_DUKE_RANDOM "You will listen to this, vile duke! I am not your Twilight's Hammer lapdog! I am here to challenge you! Come! Come, and meet your death..."
#define GOSSIP_DUKE_FIRE "Duke of Cynders! I hold your signet! Heed my call!"
#define GOSSIP_DUKE_EARTH "The Duke of Shards! I hold your signet! Heed my call!"
#define GOSSIP_DUKE_AIR "The Duke of Zephyrs! I hold your signet! Heed my call!"
#define GOSSIP_DUKE_WATER "The Duke of Fathoms! I hold your signet! Heed my call!"

#define GOSSIP_ROYAL_RANDOM "The day of the judgement has come, fiend! I challenge you to battle!"
#define GOSSIP_ROYAL_FIRE "Prince Skaldrenox! I hold your signet! Heed my call!"
#define GOSSIP_ROYAL_EARTH "Baron Kazum! I hold your signet! Heed my call!"
#define GOSSIP_ROYAL_AIR "High Marshal Whirlaxis! I hold your signet! Heed my call!"
#define GOSSIP_ROYAL_WATER "Lord Skwol! I hold your signet! Heed my call!"

uint8 GetPlayerRank(Player* pPlayer) // For random summoning
{
    bool setAura = pPlayer->HasAura(AURA_TWILIGHT_SET);
    bool medallionAura = pPlayer->HasAura(AURA_MEDALLION);
    bool ringAura = pPlayer->HasAura(AURA_RING);

    if (setAura && medallionAura && ringAura)
        return 3;
    else if (setAura && medallionAura)
        return 2;
    else if (setAura)
        return 1;
    else
        return 0;
}

uint8 GetItems(Player* pPlayer, WS type)
{
    if (!pPlayer)
        return 0x0;

    uint8 result = 0x0;

    switch (type)
    {
        case TEMPLAR:
        {
            if (pPlayer->HasItemCount(ITEM_TEMPLAR_FIRE, 1))
                result |= FIRE;
            if (pPlayer->HasItemCount(ITEM_TEMPLAR_WATER, 1))
                result |= WATER;
            if (pPlayer->HasItemCount(ITEM_TEMPLAR_EARTH, 1))
                result |= EARTH;
            if (pPlayer->HasItemCount(ITEM_TEMPLAR_AIR, 1))
                result |= AIR;
            break;
        }
        case DUKE:
        {
            if (pPlayer->HasItemCount(ITEM_DUKE_FIRE, 1))
                result |= FIRE;
            if (pPlayer->HasItemCount(ITEM_DUKE_WATER, 1))
                result |= WATER;
            if (pPlayer->HasItemCount(ITEM_DUKE_EARTH, 1))
                result |= EARTH;
            if (pPlayer->HasItemCount(ITEM_DUKE_AIR, 1))
                result |= AIR;
            break;
        }
        case ROYAL:
        {
            if (pPlayer->HasItemCount(ITEM_ROYAL_FIRE, 1))
                result |= FIRE;
            if (pPlayer->HasItemCount(ITEM_ROYAL_WATER, 1))
                result |= WATER;
            if (pPlayer->HasItemCount(ITEM_ROYAL_EARTH, 1))
                result |= EARTH;
            if (pPlayer->HasItemCount(ITEM_ROYAL_AIR, 1))
                result |= AIR;
            break;
        }
        default:
            break;
    }
    return result;
}

void SummonNPC(GameObject* pGo, Player* pPlayer, uint32 npc, uint32 spell)
{
    if (!pPlayer || !pGo || !npc || !spell)
        return;

    pPlayer->CastSpell(pPlayer, spell, true);
    if (Creature* summons = pGo->SummonCreature(npc, pGo->GetPositionX(), pGo->GetPositionY(), pGo->GetPositionZ(), pPlayer->GetOrientation() - M_PI, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 10 * 60 * 1000))
    {
        summons->CastSpell(summons, SPELL_SPAWN_IN, false);
        switch (summons->GetEntry())
        {
            case NPC_TEMPLAR_FIRE:
            case NPC_TEMPLAR_WATER:
            case NPC_TEMPLAR_AIR:
            case NPC_TEMPLAR_EARTH:
                summons->MonsterSay("The Abyssal Council does not tolerate deceit! Feel our wrath, little human!", LANG_UNIVERSAL);
                break;
    
            case NPC_DUKE_FIRE:
            case NPC_DUKE_WATER:
            case NPC_DUKE_EARTH:
            case NPC_DUKE_AIR:
                summons->MonsterSay("What? Such a small, frail thing beckons me? This will not do unpunished!", LANG_UNIVERSAL);
                break;
            case NPC_ROYAL_FIRE:
            case NPC_ROYAL_AIR:
            case NPC_ROYAL_EARTH:
            case NPC_ROYAL_WATER:
                summons->MonsterSay("Your treachery only speeds your doom. This world will fall to darkness!", LANG_UNIVERSAL);
                break;
        }
        summons->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        summons->SendMeleeAttackStart(pPlayer);
        summons->AI()->AttackStart(pPlayer);
    }
}

bool pGossipHelloGO_go_wind_stone(Player *pPlayer, GameObject* pGo)
{
    if (!pPlayer || !pGo)
        return false;

    uint8 rank = GetPlayerRank(pPlayer);

    uint32 gossipId = pGo->GetGOInfo()->GetGossipMenuId();
    switch (gossipId)
    {
        case GOSSIPID_LESSER_WS:
        {
            if (rank >= 1) // 1 or 2 or 3
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEMPLAR_RANDOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
            else
            {
                pPlayer->CastSpell(pPlayer, SPELL_PUNISHMENT, true);
                break;
            }

            uint8 item = GetItems(pPlayer, TEMPLAR);
            if (item & FIRE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEMPLAR_FIRE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
            if (item & WATER)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEMPLAR_WATER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
            if (item & EARTH)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEMPLAR_EARTH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
            if (item & AIR)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_TEMPLAR_AIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
            break;
        }
        case GOSSIPID_WS:
        {
            if (rank >= 2) // 2 or 3
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DUKE_RANDOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
            else
            {
                pPlayer->CastSpell(pPlayer, SPELL_PUNISHMENT, true);
                break;
            }

            uint8 item = GetItems(pPlayer, DUKE);
            if (item & FIRE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DUKE_FIRE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
            if (item & WATER)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DUKE_WATER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
            if (item & EARTH)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DUKE_EARTH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
            if (item & AIR)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_DUKE_AIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
            break;
        }
        case GOSSIPID_GREATER_WS:
        {
            if (rank == 3) // 3
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROYAL_RANDOM, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
            else
            {
                pPlayer->CastSpell(pPlayer, SPELL_PUNISHMENT, true);
                break;
            }

            uint8 item = GetItems(pPlayer, ROYAL);
            if (item & FIRE)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROYAL_FIRE, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
            if (item & WATER)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROYAL_WATER, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
            if (item & EARTH)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROYAL_EARTH, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
            if (item & AIR)
                pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ROYAL_AIR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 15);
            break;
        }
        default:
            break;
    }

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(gossipId, pGo), pGo->GetGUID());
    return true;
}

bool pGossipSelectGO_go_wind_stone(Player* pPlayer, GameObject* pGo, uint32 /*sender*/, uint32 action)
{
    if (!pPlayer || !pGo)
        return false;

    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->PlayerTalkClass->CloseGossip();

    switch (action)
    {
        case GOSSIP_ACTION_INFO_DEF + 1:
            SummonNPC(pGo, pPlayer, RAND(NPC_TEMPLAR_WATER, NPC_TEMPLAR_FIRE, NPC_TEMPLAR_EARTH, NPC_TEMPLAR_AIR), SPELL_TEMPLAR_RANDOM);
            break;
        case GOSSIP_ACTION_INFO_DEF + 2:
            SummonNPC(pGo, pPlayer, NPC_TEMPLAR_FIRE, SPELL_TEMPLAR_FIRE);
            break;
        case GOSSIP_ACTION_INFO_DEF + 3:
            SummonNPC(pGo, pPlayer, NPC_TEMPLAR_WATER, SPELL_TEMPLAR_WATER);
            break;
        case GOSSIP_ACTION_INFO_DEF + 4:
            SummonNPC(pGo, pPlayer, NPC_TEMPLAR_EARTH, SPELL_TEMPLAR_EARTH);
            break;
        case GOSSIP_ACTION_INFO_DEF + 5:
            SummonNPC(pGo, pPlayer, NPC_TEMPLAR_AIR, SPELL_TEMPLAR_AIR);
            break;

        case GOSSIP_ACTION_INFO_DEF + 6:
            SummonNPC(pGo, pPlayer, RAND(NPC_DUKE_FIRE, NPC_DUKE_WATER, NPC_DUKE_EARTH, NPC_DUKE_AIR), SPELL_DUKE_RANDOM);
            break;
        case GOSSIP_ACTION_INFO_DEF + 7:
            SummonNPC(pGo, pPlayer, NPC_DUKE_FIRE, SPELL_DUKE_FIRE);
            break;
        case GOSSIP_ACTION_INFO_DEF + 8:
            SummonNPC(pGo, pPlayer, NPC_DUKE_WATER, SPELL_DUKE_WATER);
            break;
        case GOSSIP_ACTION_INFO_DEF + 9:
            SummonNPC(pGo, pPlayer, NPC_DUKE_EARTH, SPELL_DUKE_EARTH);
            break;
        case GOSSIP_ACTION_INFO_DEF + 10:
            SummonNPC(pGo, pPlayer, NPC_DUKE_AIR, SPELL_DUKE_AIR);
            break;

        case GOSSIP_ACTION_INFO_DEF + 11:
            SummonNPC(pGo, pPlayer, RAND(NPC_ROYAL_FIRE, NPC_ROYAL_AIR, NPC_ROYAL_EARTH, NPC_ROYAL_WATER), SPELL_ROYAL_RANDOM);
            break;
        case GOSSIP_ACTION_INFO_DEF + 12:
            SummonNPC(pGo, pPlayer, NPC_ROYAL_FIRE, SPELL_ROYAL_FIRE);
            break;
        case GOSSIP_ACTION_INFO_DEF + 13:
            SummonNPC(pGo, pPlayer, NPC_ROYAL_WATER, SPELL_ROYAL_WATER);
            break;
        case GOSSIP_ACTION_INFO_DEF + 14:
            SummonNPC(pGo, pPlayer, NPC_ROYAL_EARTH, SPELL_ROYAL_EARTH);
            break;
        case GOSSIP_ACTION_INFO_DEF + 15:
            SummonNPC(pGo, pPlayer, NPC_ROYAL_AIR, SPELL_ROYAL_AIR);
            break;

        default:
            break;
    }
    return true;
}

/*######
## go_rusty_cage
######*/

enum eRustyCage
{
    NPC_GOBLIN_PRISIONER    = 29466
};

bool GOUse_go_rusty_cage(Player* pPlayer, GameObject* pGO)
{
    if (Creature* pGoblinPrisoner = GetClosestCreatureWithEntry(pGO, NPC_GOBLIN_PRISIONER, INTERACTION_DISTANCE))
    {
        pGO->SetGoState(GO_STATE_ACTIVE);
        pPlayer->KilledMonsterCredit(NPC_GOBLIN_PRISIONER, pGoblinPrisoner->GetObjectGuid());
        pGoblinPrisoner->MonsterSay("I am free? I am free! Time to hightail it! Thanks, friend!", LANG_UNIVERSAL);
        pGoblinPrisoner->ForcedDespawn(3000);
    }

    return true;
}

/*######
## go_arcane_prison
######*/

enum eArcanePrison
{
    QUEST_PRISON_BREAK                  = 11587,
    SPELL_ARCANE_PRISONER_KILL_CREDIT   = 45456
};

bool GOUse_go_arcane_prison(Player* pPlayer, GameObject* pGO)
{
    if (pPlayer->GetQuestStatus(QUEST_PRISON_BREAK) == QUEST_STATUS_INCOMPLETE)
    {
        pGO->SummonCreature(25318, 3485.089844f, 6115.7422188f, 70.966812f, 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
        pPlayer->CastSpell(pPlayer, SPELL_ARCANE_PRISONER_KILL_CREDIT, true);
        return true;
    }
    else
        return false;
}

void AddSC_go_scripts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_cat_figurine";
    pNewScript->pGOUse =          &GOUse_go_cat_figurine;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_barov_journal";
    pNewScript->pGOUse =          &GOUse_go_barov_journal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_ethereum_prison";
    pNewScript->pGOUse =          &GOUse_go_ethereum_prison;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_ethereum_stasis";
    pNewScript->pGOUse =          &GOUse_go_ethereum_stasis;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_gilded_brazier";
    pNewScript->pGOUse =          &GOUse_go_gilded_brazier;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_jump_a_tron";
    pNewScript->pGOUse =          &GOUse_go_jump_a_tron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_mysterious_snow_mound";
    pNewScript->pGOUse =          &GOUse_go_mysterious_snow_mound;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_resonite_cask";
    pNewScript->pGOUse =          &GOUse_go_resonite_cask;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_sacred_fire_of_life";
    pNewScript->pGOUse =          &GOUse_go_sacred_fire_of_life;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_shrine_of_the_birds";
    pNewScript->pGOUse =          &GOUse_go_shrine_of_the_birds;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_tablet_of_madness";
    pNewScript->pGOUse =          &GOUse_go_tablet_of_madness;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_tele_to_dalaran_crystal";
    pNewScript->pGOUse =          &GOUse_go_tele_to_dalaran_crystal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_tele_to_violet_stand";
    pNewScript->pGOUse =          &GOUse_go_tele_to_violet_stand;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_blood_filled_orb";
    pNewScript->pGOUse =          &GOUse_go_blood_filled_orb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_andorhal_tower";
    pNewScript->pGOUse =          &GOUse_go_andorhal_tower;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_scourge_enclosure";
    pNewScript->pGOUse =          &GOUse_go_scourge_enclosure;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_lab_work_reagents";
    pNewScript->pGOUse =          &GOUse_go_lab_work_reagents;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_hand_of_iruxos_crystal";
    pNewScript->pGOUse =          &GOUse_go_hand_of_iruxos_crystal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_org_portal";
    pNewScript->pGOUse =          &GOUse_go_org_portal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_power_converter";
    pNewScript->pGOUse =          &GOUse_go_power_converter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_sw_portal";
    pNewScript->pGOUse =          &GOUse_go_sw_portal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_wind_stone";
    pNewScript->pGossipHelloGO =  &pGossipHelloGO_go_wind_stone;
    pNewScript->pGossipSelectGO = &pGossipSelectGO_go_wind_stone;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_rusty_cage";
    pNewScript->pGOUse =  &GOUse_go_rusty_cage;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_arcane_prison";
    pNewScript->pGOUse =  &GOUse_go_arcane_prison;
    pNewScript->RegisterSelf();
}