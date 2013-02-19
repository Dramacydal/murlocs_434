/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_valithria
SD%Complete: 70%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */
// Need move emerald dream to phase 32, correct timers and other
#include "precompiled.h"
#include "icecrown_citadel.h"
#include "Spell.h"

static Locations SpawnLoc[]=
{
    {4203.470215f, 2484.500000f, 364.872009f},  // 0 Valithria
    {4166.216797f, 2564.197266f, 364.873047f},  // 1 Valithria Room 1
    {4240.688477f, 2405.794678f, 364.868591f},  // 2 Valithria Room 2
    {4165.112305f, 2405.872559f, 364.872925f},  // 3 Valithria Room 3
    {4239.579102f, 2566.753418f, 364.868439f},  // 4 Valithria Room 4
    {4228.589844f, 2469.110107f, 364.868988f},  // 5 Mob 1
    {4236.000000f, 2479.500000f, 364.869995f},  // 6 Mob 2
    {4235.410156f, 2489.300049f, 364.872009f},  // 7 Mob 3
    {4228.509766f, 2500.310059f, 364.876007f},  // 8 Mob 4
};

enum BossSpells
{
//    SPELL_NIGHTMARE_PORTAL       = 72481, // Not worked yet. Use 71977 (visual effect) instead?
    SPELL_NIGHTMARE_PORTAL       = 71977,
    SPELL_EMERALD_VIGOR          = 70873,
    SPELL_DREAMWALKER_RAGE       = 71189,
    SPELL_IMMUNITY               = 72724,
    SPELL_CORRUPTION             = 70904,
    SPELL_DREAM_SLIP             = 71196,
    SPELL_ICE_SPIKE              = 70702,

    SPELL_ACHIEVEMENT_CHECK      = 72706,

// Summons
    NPC_RISEN_ARCHMAGE           = 37868,
    NPC_SUPPRESSOR               = 37863,
    NPC_BLASING_SKELETON         = 36791,
    NPC_BLISTERING_ZOMBIE        = 37934,
    NPC_GLUTTONOUS_ABOMINATION   = 37886,
    NPC_NIGHTMARE_PORTAL         = 38429, // Not realized yet
    // Mana void
    NPC_MANA_VOID                = 38068, // Bugged, need override
    SPELL_VOID_BUFF              = 71085,
};

struct MobsStruct
{
    uint32 event;
    uint32 mob;
    uint32 starttime;
    uint32 startcooldown;
    uint32 cooldown;
    uint32 cooldowndiff;
};

MobsStruct MobsData[]=
{
    {1, NPC_GLUTTONOUS_ABOMINATION , 30000,  65000,   65000,  5000  },
    {2, NPC_SUPPRESSOR             , 35000,  60000,   60000,  5000  },
    {3, NPC_BLISTERING_ZOMBIE      , 40000,  30000,   30000,  2500  },
    {4, NPC_RISEN_ARCHMAGE         , 50000,  30000,   30000,  2500  },
    {5, NPC_BLASING_SKELETON       , 60000,  60000,   60000,  5000  },
    {6, NULL                       , 10000,  600000,  600000, 1000  }, // door event
};

const uint32 EVENT_MAX_NPC = 6;

struct MANGOS_DLL_DECL boss_valithria_dreamwalkerAI : public BSWScriptedAI
{
    boss_valithria_dreamwalkerAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (instance_icecrown_spire*)pCreature->GetInstanceData();
        Reset();
    }

    instance_icecrown_spire* pInstance;
    bool battlestarted;
    bool intro;
    uint8 currentDoor;
    uint8 currentDoor2;
    int8 portalscount;
    std::list<uint64> mobsGUIDList;
    Creature* dummyTarget;

    uint32 m_auiEventSpec[EVENT_MAX_NPC];
    uint32 m_auiCreatureIDSpec[EVENT_MAX_NPC];
    uint32 m_auiEventTimerSpec[EVENT_MAX_NPC];
    bool   m_auiEventLockSpec[EVENT_MAX_NPC];

    void ClearEventSpec()
    {
        for (uint32 i = 0; i < EVENT_MAX_NPC; ++i)
        {
            m_auiEventSpec[i] = 0;
            m_auiCreatureIDSpec[i] = 0;
            m_auiEventTimerSpec[i] = 0;
            m_auiEventLockSpec[i] = false;
        }

        for (uint32 i = 0; i < EVENT_MAX_NPC; ++i)
            MobsData[i].cooldown = MobsData[i].startcooldown;
    }

    uint32 GetEventSpec(uint32 EventNum, uint32 creatureID)
    {
        if (m_auiEventLockSpec[EventNum] || m_auiCreatureIDSpec[EventNum] != creatureID)
            return 0;
        else
        {
            debug_log("Valithria: GetEventSpec: send event %u to creature %u",m_auiEventSpec[EventNum], creatureID);
            m_auiEventLockSpec[EventNum] = true;
            return  m_auiEventSpec[EventNum];
        }
    }
    
    void SetNextEventSpec(uint32 EventNum, uint32 creatureID, uint32 timer)
    {
        if (timer < 1000)
        {
            error_log("Valithria: SetNextEventSpec: minimum timer are 1000 ms.! But tried set to %u. Timer set to 1000ms.", timer);
            timer = 1000;
        }

        m_auiEventSpec[EventNum]      = EventNum;
        m_auiCreatureIDSpec[EventNum] = creatureID;
        m_auiEventTimerSpec[EventNum] = timer;
        m_auiEventLockSpec[EventNum]  = false;
        debug_log("Valithria: SetNextEventSpec: setted event %u to creature %u, timer %u",m_auiEventSpec[EventNum], creatureID, timer);
    }
    
    bool GetEventTimerSpec(uint32 EventNum, uint32 creatureID, const uint32 diff)
    {
        if (m_auiEventSpec[EventNum] == 0 || m_auiCreatureIDSpec[EventNum] != creatureID)
            return false;
    
        if (m_auiEventTimerSpec[EventNum] <= diff)
        {
            m_auiEventTimerSpec[EventNum] = 0;
            return true;
        }
        else
        {
            m_auiEventTimerSpec[EventNum] -= diff;
            return false;
        }
    }

    void Reset()
    {
        if (!pInstance)
            return;

        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetHealth(m_creature->GetMaxHealth() / 2.0f);

        if (pInstance->GetData(TYPE_VALITHRIA) != DONE)
            pInstance->SetData(TYPE_VALITHRIA, NOT_STARTED);
        else
            m_creature->ForcedDespawn();

        resetTimers();
        SetCombatMovement(false);
        setStage(0);
        portalscount = 0;
        battlestarted = false;
        intro = false;
        currentDoor;
        currentDoor2;
        mobsGUIDList.clear();
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_VALITHRIA_QUEST)))
        {
            if (pTemp->GetVisibility() == VISIBILITY_ON)
                pTemp->SetVisibility(VISIBILITY_OFF);
        }

        doCast(SPELL_CORRUPTION);

        ClearEventSpec();

        // init first pool
        for (uint32 i = 0; i < EVENT_MAX_NPC; ++i)
            SetNextEventSpec(MobsData[i].event, MobsData[i].mob, MobsData[i].starttime);

        for (uint8 i = 1; i < 5; ++i)
            pInstance->DoCloseDoorGuid(GetDoor(i));
    }

    uint64 GetDoor(uint8 doornum)
    {
        switch (doornum)
        {
            case 1:
                return pInstance->GetData64(GO_VALITHRIA_DOOR_1);
                break;
            case 2:
                return pInstance->GetData64(GO_VALITHRIA_DOOR_2);
                break;
            case 3:
                return pInstance->GetData64(GO_VALITHRIA_DOOR_3);
                break;
            case 4:
                return pInstance->GetData64(GO_VALITHRIA_DOOR_4);
                break;
            default:
                return 0;
                break;
        }
    }

    uint8 CallMobs(uint8 door, uint8 mobscount)
    {
        if (!door)
            return 0;

        uint8 mobs;
        uint32 randommob;

        uint8 maxmobs25 = ( mobscount > 0) ? 7 - mobscount : 5;

        switch (currentDifficulty)
        {
            case RAID_DIFFICULTY_10MAN_NORMAL:
                mobs = urand(1,3);
                break;
            case RAID_DIFFICULTY_10MAN_HEROIC:
                mobs = urand(2,4);
                break;
            case RAID_DIFFICULTY_25MAN_NORMAL:
                mobs = urand(1,maxmobs25);
                break;
            case RAID_DIFFICULTY_25MAN_HEROIC:
                mobs = urand(2,maxmobs25);
                break;
            default:
                mobs = urand(1,5);
                break;
        }

        for (uint8 i = 0; i < mobs; ++i)
        {
            switch (urand(0,4))
            {
                case 0: randommob = NPC_RISEN_ARCHMAGE;        break;
                case 1: randommob = NPC_SUPPRESSOR;            break;
                case 2: randommob = NPC_BLASING_SKELETON;      break;
                case 3: randommob = NPC_BLISTERING_ZOMBIE;     break;
                case 4: randommob = NPC_GLUTTONOUS_ABOMINATION;break;
                default: randommob = NPC_RISEN_ARCHMAGE;       break;
            }

            if (Unit* pTemp = doSummon(randommob, SpawnLoc[door].x, SpawnLoc[door].y, SpawnLoc[door].z, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 300000))
                mobsGUIDList.push_back(pTemp->GetGUID());
        }

        return mobs;
    }

    void QueryEvadeMode()
    {
        if (m_creature->GetHealthPercent() > 1.0f)
        {
            Map* pMap = m_creature->GetMap();
            Map::PlayerList const &players = pMap->GetPlayers();
            for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                if (Player* pPlayer = i->getSource())
                    if(pPlayer->isAlive() && !pPlayer->isGameMaster() && pPlayer->IsWithinDistInMap(m_creature, 60.0f))
                        return;
            }
        }

        pInstance->SetData(TYPE_VALITHRIA, FAIL);
        DoScriptText(-1631409,m_creature);
        DespawnMobs();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(false);
        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MoveTargetedHome();
        m_creature->SetLootRecipient(NULL);
        //pInstance->DoCloseDoorGuid(GetDoor(currentDoor));
        //pInstance->DoCloseDoorGuid(GetDoor(currentDoor2));
        Reset();
    }

    void MoveInLineOfSight(Unit* pWho) 
    {
        if (!pInstance || intro && battlestarted)
            return;

        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!intro)
        {
            DoScriptText(-1631401,m_creature,pWho);
            intro = true;
            doCast(SPELL_IMMUNITY);
        }

        if (!battlestarted && pWho->isAlive() && pWho->IsWithinDistInMap(m_creature, 40.0f))
        {
            if (!pInstance->IsEncounterAvailable(TYPE_VALITHRIA))
            {
                pInstance->SendGMEncounterNotification(pWho, TYPE_VALITHRIA);
                EnterEvadeMode();
                return;
            }

            DoScriptText(-1631401,m_creature,pWho);
            battlestarted = true;
            pInstance->SetData(TYPE_VALITHRIA, IN_PROGRESS);
            m_creature->SetHealth(m_creature->GetMaxHealth()/2.0f);
            dummyTarget = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_COMBAT_TRIGGER));
            if (!dummyTarget)
                dummyTarget = m_creature->SummonCreature(NPC_COMBAT_TRIGGER, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 1000);
            if (!dummyTarget->isAlive())
                dummyTarget->Respawn();
            if (dummyTarget)
            {
                dummyTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                dummyTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                dummyTarget->GetMotionMaster()->MoveIdle();
                dummyTarget->StopMoving();
            }
            m_creature->SetInCombatWith(dummyTarget);
            m_creature->SetHealth(m_creature->GetMaxHealth()/2.0f);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if(!pInstance)
            return;

        switch (urand(0,1))
        {
            case 0:
                DoScriptText(-1631403,m_creature,pVictim);
                break;
            case 1:
                DoScriptText(-1631404,m_creature,pVictim);
                break;
        }
    }

    void JustSummoned(Creature* summoned)
    {
        if (!pInstance || !summoned || !battlestarted)
            return;

        if (summoned->GetEntry() != NPC_NIGHTMARE_PORTAL)
        {
            m_creature->SetInCombatWithZone();
            m_creature->SetInCombatWith(summoned);
            summoned->SetInCombatWith(m_creature);
            summoned->AddThreat(m_creature, 100.0f);
            summoned->GetMotionMaster()->MoveChase(m_creature);
        }
    }

    void DespawnMobs()
    {
        if (mobsGUIDList.empty())
            return;

        for(std::list<uint64>::iterator itr = mobsGUIDList.begin(); itr != mobsGUIDList.end(); ++itr)
        {
            if (Creature* pTemp = m_creature->GetMap()->GetCreature(*itr))
                if (pTemp->isAlive()) 
                {
                    pTemp->DeleteThreatList();
                    pTemp->CombatStop(true);
                    pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                }
        }

        mobsGUIDList.clear();
    }

    void JustDied(Unit *killer)
    {
        if (!pInstance)
            return;

        pInstance->SetData(TYPE_VALITHRIA, FAIL);
        DoScriptText(-1631409,m_creature);
        DespawnMobs();
        m_creature->Respawn();
        Reset();
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_creature || !m_creature->isAlive() || !pInstance)
            return;

        if (uiDamage >= m_creature->GetHealth())
            uiDamage = 0;
    }

    void HealBy(Unit* pHealer, uint32 uiAmountHealed)
    {
        if (!pInstance)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_VALITHRIA))
        {
            pInstance->SendGMEncounterNotification(pHealer, TYPE_VALITHRIA);
            EnterEvadeMode();
            uiAmountHealed = 0;
            return;
        }
    }

    void AttackStart(Unit *who) { }

    void UpdateAI(const uint32 diff)
    {
        if (!hasAura(SPELL_CORRUPTION,m_creature) && getStage() == 0)
            doCast(SPELL_CORRUPTION);

        if (!battlestarted)
            return;

        QueryEvadeMode();

        switch(getStage())
        {
            case 0: 
                if (m_creature->GetHealthPercent() > 90.0f)
                    setStage(2);
                else if (m_creature->GetHealthPercent() < 10.0f)
                    setStage(3);
                break;
            case 1: 
                if (m_creature->GetHealthPercent() < 90.0f && m_creature->GetHealthPercent() > 10.0f)
                    setStage(0);
                else if (m_creature->GetHealthPercent() > 99.9f)
                    setStage(5);
                break;
            case 2: 
                DoScriptText(-1631407,m_creature);
                setStage(1);
                break;
            case 3: 
                DoScriptText(-1631406,m_creature);
                setStage(1);
                break;
            case 4: 
                break;
            case 5: 
                DoScriptText(-1631408, m_creature);
                if (hasAura(SPELL_CORRUPTION,m_creature))
                    doRemove(SPELL_CORRUPTION);
                setStage(6);
                return;
                break;
            case 6: 
                if (timedQuery(SPELL_CORRUPTION, diff))
                    setStage(7);
                return;
                break;
            case 7: 
                doCast(SPELL_DREAMWALKER_RAGE);
                setStage(8);
                return;
                break;
            case 8:
                if (timedQuery(SPELL_CORRUPTION, diff))
                {
                    setStage(9);
                    DespawnMobs();
                }
                return;
                break;
            case 9:
                if (Creature* pTemp = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_VALITHRIA_QUEST)))
                {
                    pTemp->SetPhaseMask(65535,true);
                    if (pTemp->HasAura(SPELL_CORRUPTION))
                        pTemp->RemoveAurasDueToSpell(SPELL_CORRUPTION);
                    if (pTemp->GetVisibility() == VISIBILITY_OFF)
                        pTemp->SetVisibility(VISIBILITY_ON);
                }
                pInstance->SetData(TYPE_VALITHRIA, DONE);

                m_creature->CastSpell(m_creature, SPELL_ACHIEVEMENT_CHECK, true);

                setStage(10);
                m_creature->ForcedDespawn();
                break;
            default:
                break;
        } 


        /*if (timedQuery(NPC_RISEN_ARCHMAGE, diff))
        {
            if (urand(0,1))
                DoScriptText(-1631402, m_creature);

            if (currentDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || currentDifficulty == RAID_DIFFICULTY_25MAN_HEROIC)
            {
                pInstance->DoCloseDoorGuid(GetDoor(currentDoor2));
                currentDoor2 = urand(1,2);
                pInstance->DoOpenDoorGuid(GetDoor(currentDoor2));
                uint8 mobscount = CallMobs(currentDoor2, 0);

                pInstance->DoCloseDoorGuid(GetDoor(currentDoor));
                currentDoor = urand(3,4);
                pInstance->DoOpenDoorGuid(GetDoor(currentDoor));
                CallMobs(currentDoor, mobscount);
            }
            else
            {
                pInstance->DoCloseDoorGuid(GetDoor(currentDoor));
                currentDoor = urand(1,4);
                pInstance->DoOpenDoorGuid(GetDoor(currentDoor));
                CallMobs(currentDoor, 0);
            }
        }*/

        for (uint32 i = 0; i < EVENT_MAX_NPC; ++i)
        {
            if (GetEventTimerSpec(MobsData[i].event, MobsData[i].mob, diff))
            {
                // door event (CD 10 minutes trick)
                if (MobsData[i].mob == NULL)
                {
                    for (uint8 i = 1; i < 5; ++i)
                    {
                        pInstance->DoCloseDoorGuid(GetDoor(i));
                        pInstance->DoOpenDoorGuid(GetDoor(i));
                    }
                }

                if (MobsData[i].mob == NPC_SUPPRESSOR)
                {
                    for (uint8 k = 0; k < 4; ++k)
                    {
                        if (Unit* pTemp = doSummon(MobsData[i].mob, SpawnLoc[urand(1, 4)].x + frand(1.0f, 10.0f), SpawnLoc[urand(1, 4)].y + frand(1.0f, 10.0f), SpawnLoc[urand(1, 4)].z, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 300000))
                            mobsGUIDList.push_back(pTemp->GetGUID());
                    }
                }
                else if (MobsData[i].mob != NULL)
                {
                    if (Unit* pTemp = doSummon(MobsData[i].mob, SpawnLoc[urand(1, 4)].x, SpawnLoc[urand(1, 4)].y, SpawnLoc[urand(1, 4)].z, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 300000))
                        mobsGUIDList.push_back(pTemp->GetGUID());
                } 

                // All mobs CD decrease after Gluttonous Abomination
                if (MobsData[i].mob == NPC_GLUTTONOUS_ABOMINATION)
                {
                    for (uint32 j = 0; j < EVENT_MAX_NPC; ++j)
                        MobsData[j].cooldown = (MobsData[j].cooldown < 10000) ? 5000 : MobsData[j].cooldown - MobsData[j].cooldowndiff;

                    if (urand(0,1))
                        DoScriptText(-1631402, m_creature);
                }

                SetNextEventSpec(MobsData[i].event, MobsData[i].mob, MobsData[i].cooldown);
            }
        }

        if (timedQuery(SPELL_NIGHTMARE_PORTAL, diff) || portalscount > 0)
        {
            if (!portalscount)
            {
                portalscount = 3;
                DoScriptText(-1631405,m_creature);
            }
            doCast(SPELL_NIGHTMARE_PORTAL);

            --portalscount;
        }

        timedCast(SPELL_ICE_SPIKE, diff);

        return;
    }
};

CreatureAI* GetAI_boss_valithria_dreamwalker(Creature* pCreature)
{
    return new boss_valithria_dreamwalkerAI(pCreature);
}

struct MANGOS_DLL_DECL mob_nightmare_portalAI : public BSWScriptedAI
{
    mob_nightmare_portalAI(Creature *pCreature) : BSWScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool portalcasted;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        SetCombatMovement(false);
        m_creature->GetMotionMaster()->MoveRandom();
        m_creature->SetDisplayId(29352);
        portalcasted = false;
    }

    void MoveInLineOfSight(Unit* pWho) 
    {
        if (!m_pInstance || portalcasted)
            return;

        if (pWho->isAlive() && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDistInMap(m_creature, 2.0f))
        {
            Map::PlayerList const &players = m_pInstance->instance->GetPlayers();
            for (Map::PlayerList::const_iterator i = players.begin(); i != players.end(); ++i)
            {
                if (Player* pPlayer = i->getSource())
                    if (pPlayer->IsInWorld() && pPlayer->isAlive() && !pPlayer->isGameMaster() && pPlayer->IsWithinDistInMap(m_creature, 10.0f))
                        pPlayer->CastSpell(pPlayer, SPELL_EMERALD_VIGOR, true);
            }
            portalcasted = true;
        }
    }

    void AttackStart(Unit *pWho) { }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_VALITHRIA) != IN_PROGRESS || portalcasted)
            if (timedQuery(SPELL_EMERALD_VIGOR, uiDiff))
                m_creature->ForcedDespawn();
    }
};

CreatureAI* GetAI_mob_nightmare_portal(Creature *pCreature)
{
    return new mob_nightmare_portalAI(pCreature);
}

struct MANGOS_DLL_DECL mob_mana_voidAI : public ScriptedAI
{
    mob_mana_voidAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_ui_Timer;

    void Reset()
    {
//        m_creature->SetDisplayId(29308);
        SetCombatMovement(false); 
//        m_creature->GetMotionMaster()->MoveRandom();
        m_creature->CastSpell(m_creature, SPELL_VOID_BUFF, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_ui_Timer = 30000;
    }

    void AttackStart(Unit *pWho) { }

    void JustDied(Unit *killer)
    {
       m_creature->RemoveCorpse();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_VALITHRIA) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_ui_Timer < uiDiff)
            m_creature->ForcedDespawn();
        else
            m_ui_Timer -= uiDiff;
    }

};

CreatureAI* GetAI_mob_mana_void(Creature *pCreature)
{
    return new mob_mana_voidAI(pCreature);
}

void AddSC_boss_valithria_dreamwalker()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_valithria_dreamwalker";
    newscript->GetAI = &GetAI_boss_valithria_dreamwalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_nightmare_portal";
    newscript->GetAI = &GetAI_mob_nightmare_portal;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mana_void";
    newscript->GetAI = &GetAI_mob_mana_void;
    newscript->RegisterSelf();
}
