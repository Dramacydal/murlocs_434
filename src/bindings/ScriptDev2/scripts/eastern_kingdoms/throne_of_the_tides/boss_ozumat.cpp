/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_ozumat
SD%Complete: 0
SDComment: Placeholder
SDCategory: Throne of the Tides
EndScriptData */

#include "precompiled.h"
#include "throne_of_the_tides.h"
#include "MapPersistentStateMgr.h"

#define GOSSIP_READY "We are ready!"

enum ScriptTexts
{
    SAY_INTRO_1     = 0,
    SAY_INTRO_2     = 1,
    SAY_INTRO_3_1   = 2,
    SAY_PHASE_2_1   = 3,
    SAY_50          = 4,
    SAY_25          = 5,
    SAY_INTRO_3_2   = 6,
    SAY_PHASE_2_2   = 7,
    SAY_PHASE_3_1   = 8,
    SAY_PHASE_3_2   = 9,
    SAY_DEATH       = 10,
    SAY_KILL        = 11,
};

enum Spells
{
    SPELL_BLIGHT_OF_OZUMAT_SELF         = 83585,
    SPELL_BLIGHT_OF_OZUMAT_TRIGGER      = 83518,
    SPELL_BLIGHT_OF_OZUMAT_MISSILE      = 83506,
    SPELL_BLIGHT_OF_OZUMAT_SUMMON_1     = 83524,
    SPELL_BLIGHT_OF_OZUMAT_SUMMON_2     = 83606,
    SPELL_BLIGHT_OF_OZUMAT_DUMMY        = 83672,
    SPELL_BLIGHT_OF_OZUMAT_AURA         = 83525,
    SPELL_BLIGHT_OF_OZUMAT_DMG          = 83561,
    SPELL_BLIGHT_OF_OZUMAT_DMG_H        = 91495,
    SPELL_BLIGHT_OF_OZUMAT_AOE          = 83607,
    SPELL_BLIGHT_OF_OZUMAT_AOE_DMG      = 83608,
    SPELL_BLIGHT_OF_OZUMAT_AOE_DMG_H    = 91494,
    SPELL_TIDAL_SURGE                   = 76133,
        
    // Vicious Mindslasher
    SPELL_BRAIN_SPIKE                   = 83915,
    SPELL_BRAIN_SPIKE_H                 = 91497,
    SPELL_VEIL_OF_SHADOW                = 83926,
    SPELL_SHADOW_BOLT                   = 83914,

    // Unyielding Behemoth
    SPELL_BLIGHT_SPRAY                  = 83985,

    // Faceless Sapper
    SPELL_ENTANGLING_GRASP              = 83463,

    SPELL_ENCOUNTER_COMPLETE            = 95673,
}; 

enum Events
{
    EVENT_INTRO_2               = 1,
    EVENT_INTRO_3_2             = 2,
    EVENT_SUMMON_MURLOC         = 3,
    EVENT_SUMMON_BEHEMOTH       = 4,
    EVENT_SUMMON_MINDLASHER     = 5,
    EVENT_SUMMON_SAPPER         = 6,
    EVENT_SUMMON_BEAST          = 7,
    EVENT_BLIGHT_OF_OZUMAT      = 8,
    EVENT_PLAYER_CHECK          = 9,
    EVENT_SHADOW_BOLT           = 10,
    EVENT_BRAIN_SPIKE           = 11,
    EVENT_VEIL_OF_SHADOW        = 12,
    EVENT_BLIGHT_SPRAY          = 13,
    EVENT_PHASE_2_2             = 14,
};

enum Adds
{
    NPC_DEEP_MURLOC_INVADER     = 44658,
    NPC_VICIOUS_MINDLASHER      = 44715,
    NPC_UNYIELDING_BEHEMOTH     = 44648,
    NPC_FACELESS_SAPPER         = 44752,
    NPC_BLIGHT_BEAST            = 44841,
    NPC_BLIGHT_OF_OZUMAT_1      = 44801,
    NPC_BLIGHT_OF_OZUMAT_2      = 44834,
};

enum Actions
{
    ACTION_NEPTULON_START_EVENT = 1,
    ACTION_NEPTULON_START       = 2,
};

enum Achievement
{
    SPELL_KILL_OZUMAT   = 95673,
};

const Position2 spawnPos[5] = 
{
    {-142.48f, 950.78f, 231.05f, 1.88f},
    {-126.62f, 1015.55f, 230.37f, 4.48f},
    {-171.65f, 1006.13f, 230.67f, 5.90f},
    {-162.53f, 966.55f, 229.43f, 0.65f},
    {-110.35f, 981.47f, 229.90f, 2.83f},
};

struct MANGOS_DLL_DECL npc_neptulonAI : public ScriptedAI
{
    npc_neptulonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    std::list<ObjectGuid> summons;
    uint32 uiMindLasherCount;
    uint32 uiSapperCount;
    bool bActive;
    bool b50;
    bool b25;

    void DespawnAllSummons()
    {
        for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* unit = me->GetMap()->GetAnyTypeCreature(*itr))
                unit->ForcedDespawn(2000);

        summons.clear();
    }

    void Reset() override
    {
        bActive = false;
        b50 = false;
        b25 = false;
        uiMindLasherCount = 0;
        uiSapperCount = 0;
        me->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        events.Reset();
        DespawnAllSummons();
        me->SetHealth(me->GetMaxHealth());
        if (m_pInstance && m_pInstance->GetData(TYPE_OZUMAT) != DONE)
            m_pInstance->SetData(TYPE_OZUMAT, NOT_STARTED);
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_NEPTULON_START_EVENT)
        {
            bActive = true;
            //Talk(SAY_INTRO_1);
            events.ScheduleEvent(EVENT_INTRO_2, 7000);
        }
        else if (action == ACTION_NEPTULON_START)
        {
            bActive = true;
            //Talk(SAY_INTRO_3_1);
            events.ScheduleEvent(EVENT_INTRO_3_2, 30000);
            events.ScheduleEvent(EVENT_SUMMON_MURLOC, urand(5000, 8000));
            events.ScheduleEvent(EVENT_SUMMON_MINDLASHER, 10000);
            events.ScheduleEvent(EVENT_SUMMON_BEHEMOTH, 20000);
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.push_back(summon->GetObjectGuid());
    }

    void SummonedCreatureJustDied(Creature* pCreature) override
    {
        //summons.Despawn(pCreature);
        if (pCreature->GetEntry() == NPC_VICIOUS_MINDLASHER)
        {
            uiMindLasherCount++;
            if (uiMindLasherCount > 2)
            {
                //Talk(SAY_PHASE_2_2);
                events.CancelEvent(EVENT_SUMMON_MURLOC);
                events.ScheduleEvent(EVENT_PHASE_2_2, 10000);
                events.ScheduleEvent(EVENT_SUMMON_SAPPER, 8000);
                events.ScheduleEvent(EVENT_SUMMON_BEAST, 14000);
                events.ScheduleEvent(EVENT_BLIGHT_OF_OZUMAT, urand(9000, 11000));
            }
            else
                events.ScheduleEvent(EVENT_SUMMON_MINDLASHER, urand(10000, 15000));
        }
        else if (pCreature->GetEntry() == NPC_FACELESS_SAPPER)
        {
            uiSapperCount++;
            if (uiSapperCount > 2)
            {
                bActive = false;
                //Talk(SAY_PHASE_3_1);
                CompleteEncounter();
            }
        }
    }

    void CompleteEncounter()
    {
        if (m_pInstance)
        {
            // Achievement
            m_pInstance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_KILL_OZUMAT, 0);

            // Guild Achievement
            //Map::PlayerList const &PlayerList = m_pInstance->instance->GetPlayers();
            //if (!PlayerList.isEmpty())
            //{
            //    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            //    {
            //        if (Player* pPlayer = i->getSource())
            //            if (Group* pGroup = pPlayer->GetGroup())
            //            {
            //                if (ObjectGuid guildGuid = pPlayer->GetGuildGuid())
            //                {
            //                    if (m_pInstance->instance->HasGuildGroup(guildGuid))
            //                    {
            //                        pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_KILL_OZUMAT, 0, NULL, 0);
            //                        break;
            //                    }
            //                }
            //            }
            //    }
            //}

            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_ENCOUNTER_COMPLETE, me);

            m_pInstance->SetData(TYPE_OZUMAT, DONE);

            me->SetPhaseMask(2, true);
        }
        EnterEvadeMode();
    }

    void DamageTaken(Unit* pAttacker, uint32 &damage) override
    {
        damage /= 10;

        if (damage >= me->GetHealth())
        {
            damage = 0;
            //Talk(SAY_DEATH);
            EnterEvadeMode();
        }
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OZUMAT, FAIL);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!bActive)
            return;

        if (me->GetHealthPercent() < 50.0f && !b50)
        {
            b50 = true;
            //Talk(SAY_50);
        }
        if (me->GetHealthPercent() < 25.0f && !b25)
        {
            b25 = true;
            //Talk(SAY_25);
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_INTRO_2:
                    //Talk(SAY_INTRO_2);
                    bActive = false;
                    break;
                case EVENT_INTRO_3_2:
                    //Talk(SAY_INTRO_3_2);
                    break;
                case EVENT_PHASE_2_2:
                    //Talk(SAY_PHASE_2_1);
                    break;
                case EVENT_SUMMON_MINDLASHER:
                {
                    int pos = urand(0, 1);
                    if (Creature* pMindlasher = me->SummonCreature(NPC_VICIOUS_MINDLASHER, spawnPos[pos].x, spawnPos[pos].y, spawnPos[pos].z, spawnPos[pos].o, TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        pMindlasher->AddThreat(me, 1.0f);
                        pMindlasher->AI()->AttackStart(me);
                    }
                    break;
                }
                case EVENT_SUMMON_BEHEMOTH:
                {
                    int pos = urand(0, 1);
                    if (Creature* pBehemoth = me->SummonCreature(NPC_UNYIELDING_BEHEMOTH, spawnPos[pos].x, spawnPos[pos].y, spawnPos[pos].z, spawnPos[pos].o, TEMPSUMMON_MANUAL_DESPAWN, 0))
                    {
                        pBehemoth->AddThreat(me, 1.0f);
                        pBehemoth->AI()->AttackStart(me);
                    }
                    break;
                }
                case EVENT_SUMMON_SAPPER:
                    for (uint8 i = 2; i < 5; i++)
                        if (Creature* pSapper = me->SummonCreature(NPC_FACELESS_SAPPER, spawnPos[i].x, spawnPos[i].y, spawnPos[i].z, spawnPos[i].o, TEMPSUMMON_MANUAL_DESPAWN, 0))
                            pSapper->CastSpell(me, SPELL_ENTANGLING_GRASP, false);
                    break;
                case EVENT_SUMMON_BEAST:
                    if (Creature* pBeast = me->SummonCreature(NPC_BLIGHT_BEAST,
                        me->GetPositionX(),
                        me->GetPositionY(),
                        me->GetPositionZ(),
                        me->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0))

                        if (Player* pTarget = GetRandomPlayer())
                            pBeast->AI()->AttackStart(pTarget);
                    events.ScheduleEvent(EVENT_SUMMON_BEAST, urand(15000, 24000));
                    break;
                case EVENT_SUMMON_MURLOC:
                    for (uint8 i = 0; i < 5; i++)
                    {
                        int pos = urand(0, 1);
                        if (Creature* pMurloc = me->SummonCreature(NPC_DEEP_MURLOC_INVADER, spawnPos[pos].x, spawnPos[pos].y, spawnPos[pos].z, spawnPos[pos].o, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        {
                            pMurloc->AddThreat(me, 1.0f);
                            pMurloc->AI()->AttackStart(me);
                        }
                    }
                    events.ScheduleEvent(EVENT_SUMMON_MURLOC, urand(10000, 17000));
                    break;
                case EVENT_BLIGHT_OF_OZUMAT:
                    if (Player* pTarget = GetRandomPlayer())
                        DoCast(pTarget, SPELL_BLIGHT_OF_OZUMAT_MISSILE);
                    events.ScheduleEvent(EVENT_BLIGHT_OF_OZUMAT, urand(10000, 18000));
                    break;
            }
        }
    }

    Player* GetRandomPlayer()
    {
        std::list<Player*> pAliveList;
        Map::PlayerList const &pPlayerList = me->GetMap()->GetPlayers();
        if (!pPlayerList.isEmpty())
            for (Map::PlayerList::const_iterator itr = pPlayerList.begin(); itr != pPlayerList.end(); ++itr)
                if (Player const* player = itr->getSource())
                    if (player->isAlive())
                        pAliveList.push_back(itr->getSource());

        if (!pAliveList.empty())
        {
            std::list<Player*>::const_iterator itr = pAliveList.begin();
            std::advance(itr, rand() % pAliveList.size());
            return (*itr);
        }
        return NULL;
    }

    bool isPlayerAlive()
    {
        Map::PlayerList const &pPlayerList = me->GetMap()->GetPlayers();

        if (!pPlayerList.isEmpty())
            for (Map::PlayerList::const_iterator itr = pPlayerList.begin(); itr != pPlayerList.end(); ++itr)
                if (Player const* player = itr->getSource())
                    if (player->isAlive())
                        return true;
        return false;
    };
};

CreatureAI* GetAI_npc_neptulon(Creature* pCreature)
{
    return new npc_neptulonAI(pCreature);
}

bool OnGossipHello_npc_neptulon(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!pInstance || !pPlayer)
        return true;

    if (pInstance->GetData(TYPE_COMMANDER_ULTHOK) != DONE
        || pInstance->GetData(TYPE_OZUMAT) == IN_PROGRESS
        || pInstance->GetData(TYPE_OZUMAT) == DONE)
        return true;

    pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_READY, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    pPlayer->SEND_GOSSIP_MENU(1, pCreature->GetGUID());

    return true;
}

bool OnGossipSelect_npc_neptulon(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!pInstance || !pPlayer)
        return true;

    if (pInstance->GetData(TYPE_COMMANDER_ULTHOK) != DONE
        || pInstance->GetData(TYPE_OZUMAT) == IN_PROGRESS
        || pInstance->GetData(TYPE_OZUMAT) == DONE)
        return true;

    pPlayer->PlayerTalkClass->ClearMenus();
    pPlayer->CLOSE_GOSSIP_MENU();
    pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    pInstance->SetData(TYPE_OZUMAT, IN_PROGRESS);
    pCreature->AI()->DoAction(ACTION_NEPTULON_START);

    return true;
}

struct MANGOS_DLL_DECL npc_vicious_mindslasherAI : public ScriptedAI
{
    npc_vicious_mindslasherAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* who) override
    {
        events.ScheduleEvent(EVENT_BRAIN_SPIKE, urand(6000, 10000));
        if (!m_bIsRegularMode)
            events.ScheduleEvent(EVENT_VEIL_OF_SHADOW, urand(10000, 15000));
        events.ScheduleEvent(EVENT_SHADOW_BOLT, 2000);
    }

    void KilledUnit(Unit* victim) override
    {
        //if (m_pInstance)
        //    if (Creature* pNeptulon = m_pInstance->GetSingleCreatureFromStorage(NPC_NEPTULON))
        //        pNeptulon->AI()->Talk(SAY_KILL);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BRAIN_SPIKE:
                    DoCast(me, SPELL_BRAIN_SPIKE);
                    events.ScheduleEvent(EVENT_BRAIN_SPIKE, urand(15000, 20000));
                    break;
                case EVENT_VEIL_OF_SHADOW:
                    DoCast(me->getVictim(), SPELL_VEIL_OF_SHADOW);
                    events.ScheduleEvent(EVENT_VEIL_OF_SHADOW, urand(13000, 20000));
                    break;
                case EVENT_SHADOW_BOLT:
                    DoCast(me->getVictim(), SPELL_SHADOW_BOLT);
                    events.ScheduleEvent(EVENT_SHADOW_BOLT, 2000);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_vicious_mindslasher(Creature* pCreature)
{
    return new npc_vicious_mindslasherAI(pCreature);
}

struct MANGOS_DLL_DECL npc_unyielding_behemothAI : public ScriptedAI
{
    npc_unyielding_behemothAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void KilledUnit(Unit* victim) override
    {
        //if (m_pInstance)
        //    if (Creature* pNeptulon = m_pInstance->GetSingleCreatureFromStorage(NPC_NEPTULON))
        //        pNeptulon->AI()->Talk(SAY_KILL);
    }

    void EnterCombat(Unit* who) override
    {
        events.ScheduleEvent(EVENT_BLIGHT_SPRAY, urand(8000, 12000));
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BLIGHT_SPRAY:
                    DoCast(me, SPELL_BLIGHT_SPRAY);
                    events.ScheduleEvent(EVENT_BLIGHT_SPRAY, urand(15000, 23000));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_unyielding_behemoth(Creature* pCreature)
{
    return new npc_unyielding_behemothAI(pCreature);
}

struct MANGOS_DLL_DECL npc_faceless_sapperAI : public ScriptedAI
{
    npc_faceless_sapperAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void UpdateAI(const uint32 diff) override
    {
    }
};

CreatureAI* GetAI_npc_faceless_sapper(Creature* pCreature)
{
    return new npc_faceless_sapperAI(pCreature);
}

struct MANGOS_DLL_DECL npc_blight_of_ozumatAI : public Scripted_NoMovementAI
{
    npc_blight_of_ozumatAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        Reset();
    }

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        DoCast(me, SPELL_BLIGHT_OF_OZUMAT_AURA);
    }

    void UpdateAI(const uint32 diff) override
    {
    }
};

CreatureAI* GetAI_npc_blight_of_ozumat(Creature* pCreature)
{
    return new npc_blight_of_ozumatAI(pCreature);
}

bool OnTrigger_at_tott_ozumat(Player* pPlayer, const AreaTriggerEntry* /*pAt*/)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData();
    if (!pInstance || !pPlayer)
        return true;

    if (pInstance->GetData(TYPE_NEPTULON_EVENT) != DONE
        && pInstance->GetData(TYPE_OZUMAT) != IN_PROGRESS
        && pInstance->GetData(TYPE_OZUMAT) != DONE)
    {
        pInstance->SetData(TYPE_NEPTULON_EVENT, DONE);
        if (Creature* pNeptulon = pInstance->GetSingleCreatureFromStorage(NPC_NEPTULON))
        {
            pNeptulon->AI()->DoAction(ACTION_NEPTULON_START_EVENT);
        }
    }

    return true;
}

void AddSC_boss_ozumat()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_neptulon";
    pNewScript->GetAI = &GetAI_npc_neptulon;
    pNewScript->pGossipHello = &OnGossipHello_npc_neptulon;
    pNewScript->pGossipSelect = &OnGossipSelect_npc_neptulon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_vicious_mindslasher";
    pNewScript->GetAI = &GetAI_npc_vicious_mindslasher;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_unyielding_behemoth";
    pNewScript->GetAI = &GetAI_npc_unyielding_behemoth;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_faceless_sapper";
    pNewScript->GetAI = &GetAI_npc_faceless_sapper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_blight_of_ozumat";
    pNewScript->GetAI = &GetAI_npc_blight_of_ozumat;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_tott_ozumat";
    pNewScript->pAreaTrigger = OnTrigger_at_tott_ozumat;
    pNewScript->RegisterSelf();
}
