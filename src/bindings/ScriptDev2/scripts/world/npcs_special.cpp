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
SDName: Npcs_Special
SD%Complete: 100
SDComment: To be used for special NPCs that are located globally.
SDCategory: NPCs
EndScriptData
*/

#include "precompiled.h"
#include "escort_ai.h"
#include "ObjectMgr.h"
#include "GameEventMgr.h"
#include "GridMap.h"
#include "Language.h"
#include "SpellMgr.h"
#include "World.h"

/* ContentData
npc_air_force_bots       80%    support for misc (invisible) guard bots in areas where player allowed to fly. Summon guards after a preset time if tagged by spell
npc_chicken_cluck       100%    support for quest 3861 (Cluck!)
npc_dancing_flames      100%    midsummer event NPC
npc_guardian            100%    guardianAI used to prevent players from accessing off-limits areas. Not in use by SD2
npc_garments_of_quests   80%    NPC's related to all Garments of-quests 5621, 5624, 5625, 5648, 5650
npc_injured_patient     100%    patients for triage-quests (6622 and 6624)
npc_doctor              100%    Gustaf Vanhowzen and Gregory Victor, quest 6622 and 6624 (Triage)
npc_innkeeper            25%    ScriptName not assigned. Innkeepers in general.
npc_mount_vendor        100%    Regular mount vendors all over the world. Display gossip if player doesn't meet the requirements to buy
npc_sayge               100%    Darkmoon event fortune teller, buff player based on answers given
npc_tabard_vendor        50%    allow recovering quest related tabards, achievement related ones need core support
npc_locksmith            75%    list of keys needs to be confirmed
npc_death_knight_gargoyle       AI for summoned gargoyle of deathknights
npc_horseman_fire_bunny
npc_shade_of_horseman
EndContentData */

/*########
# npc_air_force_bots
#########*/

enum SpawnType
{
    SPAWNTYPE_TRIPWIRE_ROOFTOP,                             // no warning, summon creature at smaller range
    SPAWNTYPE_ALARMBOT                                      // cast guards mark and summon npc - if player shows up with that buff duration < 5 seconds attack
};

struct SpawnAssociation
{
    uint32 m_uiThisCreatureEntry;
    uint32 m_uiSpawnedCreatureEntry;
    SpawnType m_SpawnType;
};

enum
{
    SPELL_GUARDS_MARK               = 38067,
    AURA_DURATION_TIME_LEFT         = 5000
};

const float RANGE_TRIPWIRE          = 15.0f;
const float RANGE_GUARDS_MARK       = 50.0f;

SpawnAssociation m_aSpawnAssociations[] =
{
    {2614,  15241, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Alliance)
    {2615,  15242, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Horde)
    {21974, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Area 52)
    {21993, 15242, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Horde - Bat Rider)
    {21996, 15241, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Alliance - Gryphon)
    {21997, 21976, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Goblin - Area 52 - Zeppelin)
    {21999, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Alliance)
    {22001, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Horde)
    {22002, 15242, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Horde)
    {22003, 15241, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Ground (Alliance)
    {22063, 21976, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Goblin - Area 52)
    {22065, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Ethereal - Stormspire)
    {22066, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Scryer - Dragonhawk)
    {22068, 22064, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Ethereal - Stormspire)
    {22069, 22064, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Stormspire)
    {22070, 22067, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Scryer)
    {22071, 22067, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Scryer)
    {22078, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Aldor)
    {22079, 22077, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Aldor - Gryphon)
    {22080, 22077, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Aldor)
    {22086, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Sporeggar)
    {22087, 22085, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Sporeggar - Spore Bat)
    {22088, 22085, SPAWNTYPE_TRIPWIRE_ROOFTOP},             //Air Force Trip Wire - Rooftop (Sporeggar)
    {22090, 22089, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Toshley's Station - Flying Machine)
    {22124, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Alarm Bot (Cenarion)
    {22125, 22122, SPAWNTYPE_ALARMBOT},                     //Air Force Guard Post (Cenarion - Stormcrow)
    {22126, 22122, SPAWNTYPE_ALARMBOT}                      //Air Force Trip Wire - Rooftop (Cenarion Expedition)
};

struct MANGOS_DLL_DECL npc_air_force_botsAI : public ScriptedAI
{
    npc_air_force_botsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pSpawnAssoc = NULL;

        // find the correct spawnhandling
        static uint32 uiEntryCount = sizeof(m_aSpawnAssociations)/sizeof(SpawnAssociation);

        for (uint8 i=0; i<uiEntryCount; ++i)
        {
            if (m_aSpawnAssociations[i].m_uiThisCreatureEntry == pCreature->GetEntry())
            {
                m_pSpawnAssoc = &m_aSpawnAssociations[i];
                break;
            }
        }

        if (!m_pSpawnAssoc)
            error_db_log("SD2: Creature template entry %u has ScriptName npc_air_force_bots, but it's not handled by that script", pCreature->GetEntry());
        else
        {
            CreatureInfo const* spawnedTemplate = GetCreatureTemplateStore(m_pSpawnAssoc->m_uiSpawnedCreatureEntry);

            if (!spawnedTemplate)
            {
                error_db_log("SD2: Creature template entry %u does not exist in DB, which is required by npc_air_force_bots", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
                m_pSpawnAssoc = NULL;
                return;
            }
        }
    }

    SpawnAssociation* m_pSpawnAssoc;
    ObjectGuid m_spawnedGuid;

    void Reset() { }

    Creature* SummonGuard()
    {
        Creature* pSummoned = m_creature->SummonCreature(m_pSpawnAssoc->m_uiSpawnedCreatureEntry, 0.0f, 0.0f, 0.0f, 0.0f, TEMPSUMMON_TIMED_OOC_DESPAWN, 300000);

        if (pSummoned)
            m_spawnedGuid = pSummoned->GetObjectGuid();
        else
        {
            error_db_log("SD2: npc_air_force_bots: wasn't able to spawn creature %u", m_pSpawnAssoc->m_uiSpawnedCreatureEntry);
            m_pSpawnAssoc = NULL;
        }

        return pSummoned;
    }

    Creature* GetSummonedGuard()
    {
        Creature* pCreature = m_creature->GetMap()->GetCreature(m_spawnedGuid);

        if (pCreature && pCreature->isAlive())
            return pCreature;

        return NULL;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_pSpawnAssoc)
            return;

        if (pWho->isTargetableForAttack() && m_creature->IsHostileTo(pWho))
        {
            Player* pPlayerTarget = pWho->GetTypeId() == TYPEID_PLAYER ? (Player*)pWho : NULL;

            // airforce guards only spawn for players
            if (!pPlayerTarget)
                return;

            Creature* pLastSpawnedGuard = m_spawnedGuid ? GetSummonedGuard() : NULL;

            // prevent calling GetCreature at next MoveInLineOfSight call - speedup
            if (!pLastSpawnedGuard)
                m_spawnedGuid.Clear();

            switch(m_pSpawnAssoc->m_SpawnType)
            {
                case SPAWNTYPE_ALARMBOT:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_GUARDS_MARK))
                        return;

                    Aura* pMarkAura = pWho->GetAura(SPELL_GUARDS_MARK, EFFECT_INDEX_0);
                    if (pMarkAura)
                    {
                        // the target wasn't able to move out of our range within 25 seconds
                        if (!pLastSpawnedGuard)
                        {
                            pLastSpawnedGuard = SummonGuard();

                            if (!pLastSpawnedGuard)
                                return;
                        }

                        if (pMarkAura->GetAuraDuration() < AURA_DURATION_TIME_LEFT)
                        {
                            if (!pLastSpawnedGuard->getVictim())
                                pLastSpawnedGuard->AI()->AttackStart(pWho);
                        }
                    }
                    else
                    {
                        if (!pLastSpawnedGuard)
                            pLastSpawnedGuard = SummonGuard();

                        if (!pLastSpawnedGuard)
                            return;

                        pLastSpawnedGuard->CastSpell(pWho, SPELL_GUARDS_MARK, true);
                    }
                    break;
                }
                case SPAWNTYPE_TRIPWIRE_ROOFTOP:
                {
                    if (!pWho->IsWithinDistInMap(m_creature, RANGE_TRIPWIRE))
                        return;

                    if (!pLastSpawnedGuard)
                        pLastSpawnedGuard = SummonGuard();

                    if (!pLastSpawnedGuard)
                        return;

                    // ROOFTOP only triggers if the player is on the ground
                    if (!pPlayerTarget->IsFlying())
                    {
                        if (!pLastSpawnedGuard->getVictim())
                            pLastSpawnedGuard->AI()->AttackStart(pWho);
                    }
                    break;
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_air_force_bots(Creature* pCreature)
{
    return new npc_air_force_botsAI(pCreature);
}

/*########
# npc_chicken_cluck
#########*/

enum
{
    EMOTE_A_HELLO           = -1000204,
    EMOTE_H_HELLO           = -1000205,
    EMOTE_CLUCK_TEXT2       = -1000206,

    QUEST_CLUCK             = 3861,
    FACTION_FRIENDLY        = 35,
    FACTION_CHICKEN         = 31
};

struct MANGOS_DLL_DECL npc_chicken_cluckAI : public ScriptedAI
{
    npc_chicken_cluckAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiResetFlagTimer;

    void Reset()
    {
        m_uiResetFlagTimer = 120000;

        m_creature->setFaction(FACTION_CHICKEN);
        m_creature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        if (uiEmote == TEXTEMOTE_CHICKEN)
        {
            if (!urand(0, 29))
            {
                if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_NONE)
                {
                    m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                    m_creature->setFaction(FACTION_FRIENDLY);

                    DoScriptText(EMOTE_A_HELLO, m_creature);

                    /* are there any difference in texts, after 3.x ?
                    if (pPlayer->GetTeam() == HORDE)
                        DoScriptText(EMOTE_H_HELLO, m_creature);
                    else
                        DoScriptText(EMOTE_A_HELLO, m_creature);
                    */
                }
            }
        }

        if (uiEmote == TEXTEMOTE_CHEER)
        {
            if (pPlayer->GetQuestStatus(QUEST_CLUCK) == QUEST_STATUS_COMPLETE)
            {
                m_creature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER);
                m_creature->setFaction(FACTION_FRIENDLY);
                DoScriptText(EMOTE_CLUCK_TEXT2, m_creature);
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Reset flags after a certain time has passed so that the next player has to start the 'event' again
        if (m_creature->HasFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_QUESTGIVER))
        {
            if (m_uiResetFlagTimer < uiDiff)
                EnterEvadeMode();
            else
                m_uiResetFlagTimer -= uiDiff;
        }

        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_chicken_cluck(Creature* pCreature)
{
    return new npc_chicken_cluckAI(pCreature);
}

bool QuestAccept_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

bool QuestRewarded_npc_chicken_cluck(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_CLUCK)
    {
        if (npc_chicken_cluckAI* pChickenAI = dynamic_cast<npc_chicken_cluckAI*>(pCreature->AI()))
            pChickenAI->Reset();
    }

    return true;
}

/*######
## npc_dancing_flames
######*/

enum
{
    SPELL_FIERY_SEDUCTION = 47057
};

struct MANGOS_DLL_DECL npc_dancing_flamesAI : public ScriptedAI
{
    npc_dancing_flamesAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        m_creature->SetFacingToObject(pPlayer);

        if (pPlayer->HasAura(SPELL_FIERY_SEDUCTION))
            pPlayer->RemoveAurasDueToSpell(SPELL_FIERY_SEDUCTION);

        if (pPlayer->IsMounted())
        {
            pPlayer->Unmount();                             // doesnt remove mount aura
            pPlayer->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
        }

        switch(uiEmote)
        {
            case TEXTEMOTE_DANCE: DoCastSpellIfCan(pPlayer, SPELL_FIERY_SEDUCTION); break;// dance -> cast SPELL_FIERY_SEDUCTION
            case TEXTEMOTE_WAVE:  m_creature->HandleEmote(EMOTE_ONESHOT_WAVE);      break;// wave -> wave
            case TEXTEMOTE_JOKE:  m_creature->HandleEmote(EMOTE_STATE_LAUGH);       break;// silly -> laugh(with sound)
            case TEXTEMOTE_BOW:   m_creature->HandleEmote(EMOTE_ONESHOT_BOW);       break;// bow -> bow
            case TEXTEMOTE_KISS:  m_creature->HandleEmote(TEXTEMOTE_CURTSEY);       break;// kiss -> curtsey
        }
    }
};

CreatureAI* GetAI_npc_dancing_flames(Creature* pCreature)
{
    return new npc_dancing_flamesAI(pCreature);
}

/*######
## Triage quest
######*/

enum
{
    SAY_DOC1                    = -1000201,
    SAY_DOC2                    = -1000202,
    SAY_DOC3                    = -1000203,

    QUEST_TRIAGE_H              = 6622,
    QUEST_TRIAGE_A              = 6624,

    DOCTOR_ALLIANCE             = 12939,
    DOCTOR_HORDE                = 12920,
    ALLIANCE_COORDS             = 7,
    HORDE_COORDS                = 6
};

struct Location
{
    float x, y, z, o;
};

static Location AllianceCoords[]=
{
    {-3757.38f, -4533.05f, 14.16f, 3.62f},                  // Top-far-right bunk as seen from entrance
    {-3754.36f, -4539.13f, 14.16f, 5.13f},                  // Top-far-left bunk
    {-3749.54f, -4540.25f, 14.28f, 3.34f},                  // Far-right bunk
    {-3742.10f, -4536.85f, 14.28f, 3.64f},                  // Right bunk near entrance
    {-3755.89f, -4529.07f, 14.05f, 0.57f},                  // Far-left bunk
    {-3749.51f, -4527.08f, 14.07f, 5.26f},                  // Mid-left bunk
    {-3746.37f, -4525.35f, 14.16f, 5.22f},                  // Left bunk near entrance
};

//alliance run to where
#define A_RUNTOX -3742.96f
#define A_RUNTOY -4531.52f
#define A_RUNTOZ 11.91f

static Location HordeCoords[]=
{
    {-1013.75f, -3492.59f, 62.62f, 4.34f},                  // Left, Behind
    {-1017.72f, -3490.92f, 62.62f, 4.34f},                  // Right, Behind
    {-1015.77f, -3497.15f, 62.82f, 4.34f},                  // Left, Mid
    {-1019.51f, -3495.49f, 62.82f, 4.34f},                  // Right, Mid
    {-1017.25f, -3500.85f, 62.98f, 4.34f},                  // Left, front
    {-1020.95f, -3499.21f, 62.98f, 4.34f}                   // Right, Front
};

//horde run to where
#define H_RUNTOX -1016.44f
#define H_RUNTOY -3508.48f
#define H_RUNTOZ 62.96f

const uint32 AllianceSoldierId[3] =
{
    12938,                                                  // 12938 Injured Alliance Soldier
    12936,                                                  // 12936 Badly injured Alliance Soldier
    12937                                                   // 12937 Critically injured Alliance Soldier
};

const uint32 HordeSoldierId[3] =
{
    12923,                                                  //12923 Injured Soldier
    12924,                                                  //12924 Badly injured Soldier
    12925                                                   //12925 Critically injured Soldier
};

/*######
## npc_doctor (handles both Gustaf Vanhowzen and Gregory Victor)
######*/

struct MANGOS_DLL_DECL npc_doctorAI : public ScriptedAI
{
    npc_doctorAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid m_playerGuid;

    uint32 SummonPatient_Timer;
    uint32 SummonPatientCount;
    uint32 PatientDiedCount;
    uint32 PatientSavedCount;

    bool Event;

    GUIDList Patients;
    std::vector<Location*> Coordinates;

    void Reset()
    {
        m_playerGuid.Clear();

        SummonPatient_Timer = 10000;
        SummonPatientCount = 0;
        PatientDiedCount = 0;
        PatientSavedCount = 0;

        Patients.clear();
        Coordinates.clear();

        Event = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void BeginEvent(Player* pPlayer);
    void PatientDied(Location* Point);
    void PatientSaved(Creature* soldier, Player* pPlayer, Location* Point);
    void UpdateAI(const uint32 diff);
};

/*#####
## npc_injured_patient (handles all the patients, no matter Horde or Alliance)
#####*/

struct MANGOS_DLL_DECL npc_injured_patientAI : public ScriptedAI
{
    npc_injured_patientAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    ObjectGuid m_doctorGuid;
    Location* Coord;

    void Reset()
    {
        m_doctorGuid.Clear();
        Coord = NULL;

        //no select
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //no regen health
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
        //to make them lay with face down
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

        uint32 mobId = m_creature->GetEntry();

        switch (mobId)
        {                                                   //lower max health
            case 12923:
            case 12938:                                     //Injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.75));
                break;
            case 12924:
            case 12936:                                     //Badly injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.50));
                break;
            case 12925:
            case 12937:                                     //Critically injured Soldier
                m_creature->SetHealth(uint32(m_creature->GetMaxHealth()*.25));
                break;
        }
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && spell->Id == 20804)
        {
            if ((((Player*)caster)->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (((Player*)caster)->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE))
            {
                if (m_doctorGuid)
                {
                    if (Creature* pDoctor = m_creature->GetMap()->GetCreature(m_doctorGuid))
                    {
                        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                            pDocAI->PatientSaved(m_creature, (Player*)caster, Coord);
                    }
                }
            }
            //make not selectable
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            //regen health
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            //stand up
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            switch(urand(0, 2))
            {
                case 0: DoScriptText(SAY_DOC1,m_creature); break;
                case 1: DoScriptText(SAY_DOC2,m_creature); break;
                case 2: DoScriptText(SAY_DOC3,m_creature); break;
            }

            m_creature->SetWalk(false);

            uint32 mobId = m_creature->GetEntry();

            switch (mobId)
            {
                case 12923:
                case 12924:
                case 12925:
                    m_creature->GetMotionMaster()->MovePoint(0, H_RUNTOX, H_RUNTOY, H_RUNTOZ);
                    break;
                case 12936:
                case 12937:
                case 12938:
                    m_creature->GetMotionMaster()->MovePoint(0, A_RUNTOX, A_RUNTOY, A_RUNTOZ);
                    break;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //lower HP on every world tick makes it a useful counter, not officlone though
        uint32 uiHPLose = uint32(0.05f * diff);
        if (m_creature->isAlive() && m_creature->GetHealth() > 1 + uiHPLose)
        {
            m_creature->SetHealth(m_creature->GetHealth() - uiHPLose);
        }

        if (m_creature->isAlive() && m_creature->GetHealth() <= 1 + uiHPLose)
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_IN_COMBAT);
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetDeathState(JUST_DIED);
            m_creature->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_DEAD);

            if (m_doctorGuid)
            {
                if (Creature* pDoctor = m_creature->GetMap()->GetCreature(m_doctorGuid))
                {
                    if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pDoctor->AI()))
                        pDocAI->PatientDied(Coord);
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_injured_patient(Creature* pCreature)
{
    return new npc_injured_patientAI(pCreature);
}

/*
npc_doctor (continue)
*/

void npc_doctorAI::BeginEvent(Player* pPlayer)
{
    m_playerGuid = pPlayer->GetObjectGuid();

    SummonPatient_Timer = 10000;
    SummonPatientCount = 0;
    PatientDiedCount = 0;
    PatientSavedCount = 0;

    switch(m_creature->GetEntry())
    {
        case DOCTOR_ALLIANCE:
            for(uint8 i = 0; i < ALLIANCE_COORDS; ++i)
                Coordinates.push_back(&AllianceCoords[i]);
            break;
        case DOCTOR_HORDE:
            for(uint8 i = 0; i < HORDE_COORDS; ++i)
                Coordinates.push_back(&HordeCoords[i]);
            break;
    }

    Event = true;
    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
}

void npc_doctorAI::PatientDied(Location* Point)
{
    Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid);

    if (pPlayer && ((pPlayer->GetQuestStatus(6624) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(6622) == QUEST_STATUS_INCOMPLETE)))
    {
        ++PatientDiedCount;

        if (PatientDiedCount > 5 && Event)
        {
            if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_A);
            else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                pPlayer->FailQuest(QUEST_TRIAGE_H);

            Reset();
            return;
        }

        Coordinates.push_back(Point);
    }
    else
        // If no player or player abandon quest in progress
        Reset();
}

void npc_doctorAI::PatientSaved(Creature* soldier, Player* pPlayer, Location* Point)
{
    if (pPlayer && m_playerGuid == pPlayer->GetObjectGuid())
    {
        if ((pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE) || (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE))
        {
            ++PatientSavedCount;

            if (PatientSavedCount == 15)
            {
                for(GUIDList::const_iterator itr = Patients.begin(); itr != Patients.end(); ++itr)
                {
                    if (Creature* Patient = m_creature->GetMap()->GetCreature(*itr))
                        Patient->SetDeathState(JUST_DIED);
                }

                if (pPlayer->GetQuestStatus(QUEST_TRIAGE_A) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_A, m_creature);
                else if (pPlayer->GetQuestStatus(QUEST_TRIAGE_H) == QUEST_STATUS_INCOMPLETE)
                    pPlayer->GroupEventHappens(QUEST_TRIAGE_H, m_creature);

                Reset();
                return;
            }

            Coordinates.push_back(Point);
        }
    }
}

void npc_doctorAI::UpdateAI(const uint32 diff)
{
    if (Event && SummonPatientCount >= 20)
    {
        Reset();
        return;
    }

    if (Event)
    {
        if (SummonPatient_Timer < diff)
        {
            Creature* Patient = NULL;
            Location* Point = NULL;

            if (Coordinates.empty())
                return;

            std::vector<Location*>::iterator itr = Coordinates.begin()+rand()%Coordinates.size();
            uint32 patientEntry = 0;

            switch(m_creature->GetEntry())
            {
                case DOCTOR_ALLIANCE: patientEntry = AllianceSoldierId[urand(0, 2)]; break;
                case DOCTOR_HORDE:    patientEntry = HordeSoldierId[urand(0, 2)]; break;
                default:
                    error_log("SD2: Invalid entry for Triage doctor. Please check your database");
                    return;
            }

            Point = *itr;

            Patient = m_creature->SummonCreature(patientEntry, Point->x, Point->y, Point->z, Point->o, TEMPSUMMON_TIMED_OOC_DESPAWN, 5000);

            if (Patient)
            {
                //303, this flag appear to be required for client side item->spell to work (TARGET_SINGLE_FRIEND)
                Patient->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);

                Patients.push_back(Patient->GetObjectGuid());

                npc_injured_patientAI* pPatientAI = dynamic_cast<npc_injured_patientAI*>(Patient->AI());

                if (pPatientAI)
                {
                    pPatientAI->m_doctorGuid = m_creature->GetObjectGuid();

                    if (Point)
                        pPatientAI->Coord = Point;
                }

                Coordinates.erase(itr);
            }
            SummonPatient_Timer = 10000;
            ++SummonPatientCount;
        }else SummonPatient_Timer -= diff;
    }
}

bool QuestAccept_npc_doctor(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if ((pQuest->GetQuestId() == QUEST_TRIAGE_A) || (pQuest->GetQuestId() == QUEST_TRIAGE_H))
    {
        if (npc_doctorAI* pDocAI = dynamic_cast<npc_doctorAI*>(pCreature->AI()))
            pDocAI->BeginEvent(pPlayer);
    }

    return true;
}

CreatureAI* GetAI_npc_doctor(Creature* pCreature)
{
    return new npc_doctorAI(pCreature);
}

/*######
## npc_garments_of_quests
######*/

enum
{
    SPELL_LESSER_HEAL_R2    = 2052,
    SPELL_FORTITUDE_R1      = 1243,

    QUEST_MOON              = 5621,
    QUEST_LIGHT_1           = 5624,
    QUEST_LIGHT_2           = 5625,
    QUEST_SPIRIT            = 5648,
    QUEST_DARKNESS          = 5650,

    ENTRY_SHAYA             = 12429,
    ENTRY_ROBERTS           = 12423,
    ENTRY_DOLF              = 12427,
    ENTRY_KORJA             = 12430,
    ENTRY_DG_KEL            = 12428,

    SAY_COMMON_HEALED       = -1000231,
    SAY_DG_KEL_THANKS       = -1000232,
    SAY_DG_KEL_GOODBYE      = -1000233,
    SAY_ROBERTS_THANKS      = -1000256,
    SAY_ROBERTS_GOODBYE     = -1000257,
    SAY_KORJA_THANKS        = -1000258,
    SAY_KORJA_GOODBYE       = -1000259,
    SAY_DOLF_THANKS         = -1000260,
    SAY_DOLF_GOODBYE        = -1000261,
    SAY_SHAYA_THANKS        = -1000262,
    SAY_SHAYA_GOODBYE       = -1000263,
};

struct MANGOS_DLL_DECL npc_garments_of_questsAI : public npc_escortAI
{
    npc_garments_of_questsAI(Creature* pCreature) : npc_escortAI(pCreature) { Reset(); }

    ObjectGuid m_playerGuid;

    bool m_bIsHealed;
    bool m_bCanRun;

    uint32 m_uiRunAwayTimer;

    void Reset()
    {
        m_playerGuid.Clear();

        m_bIsHealed = false;
        m_bCanRun = false;

        m_uiRunAwayTimer = 5000;

        m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
        // expect database to have RegenHealth=0
        m_creature->SetHealth(int(m_creature->GetMaxHealth()*0.7));
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_LESSER_HEAL_R2 || pSpell->Id == SPELL_FORTITUDE_R1)
        {
            // not while in combat
            if (m_creature->isInCombat())
                return;

            // nothing to be done now
            if (m_bIsHealed && m_bCanRun)
                return;

            if (pCaster->GetTypeId() == TYPEID_PLAYER)
            {
                switch(m_creature->GetEntry())
                {
                    case ENTRY_SHAYA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_MOON) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_SHAYA_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_ROBERTS:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_1) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_ROBERTS_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DOLF:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_LIGHT_2) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DOLF_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_KORJA:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_SPIRIT) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_KORJA_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                    case ENTRY_DG_KEL:
                        if (((Player*)pCaster)->GetQuestStatus(QUEST_DARKNESS) == QUEST_STATUS_INCOMPLETE)
                        {
                            if (m_bIsHealed && !m_bCanRun && pSpell->Id == SPELL_FORTITUDE_R1)
                            {
                                DoScriptText(SAY_DG_KEL_THANKS, m_creature, pCaster);
                                m_bCanRun = true;
                            }
                            else if (!m_bIsHealed && pSpell->Id == SPELL_LESSER_HEAL_R2)
                            {
                                m_playerGuid = pCaster->GetObjectGuid();
                                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                                DoScriptText(SAY_COMMON_HEALED, m_creature, pCaster);
                                m_bIsHealed = true;
                            }
                        }
                        break;
                }

                // give quest credit, not expect any special quest objectives
                if (m_bCanRun)
                    ((Player*)pCaster)->TalkedToCreature(m_creature->GetEntry(), m_creature->GetObjectGuid());
            }
        }
    }

    void WaypointReached(uint32 uiPointId) {}

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (m_bCanRun && !m_creature->isInCombat())
        {
            if (m_uiRunAwayTimer <= uiDiff)
            {
                if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_playerGuid))
                {
                    switch(m_creature->GetEntry())
                    {
                        case ENTRY_SHAYA:   DoScriptText(SAY_SHAYA_GOODBYE, m_creature, pPlayer);   break;
                        case ENTRY_ROBERTS: DoScriptText(SAY_ROBERTS_GOODBYE, m_creature, pPlayer); break;
                        case ENTRY_DOLF:    DoScriptText(SAY_DOLF_GOODBYE, m_creature, pPlayer);    break;
                        case ENTRY_KORJA:   DoScriptText(SAY_KORJA_GOODBYE, m_creature, pPlayer);   break;
                        case ENTRY_DG_KEL:  DoScriptText(SAY_DG_KEL_GOODBYE, m_creature, pPlayer);  break;
                    }

                    Start(true);
                }
                else
                    EnterEvadeMode();                       // something went wrong

                m_uiRunAwayTimer = 30000;
            }
            else
                m_uiRunAwayTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_garments_of_quests(Creature* pCreature)
{
    return new npc_garments_of_questsAI(pCreature);
}

/*######
## npc_guardian
######*/

#define SPELL_DEATHTOUCH                5

struct MANGOS_DLL_DECL npc_guardianAI : public ScriptedAI
{
    npc_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->isAttackReady())
        {
            m_creature->CastSpell(m_creature->getVictim(),SPELL_DEATHTOUCH, true);
            m_creature->resetAttackTimer();
        }
    }
};

CreatureAI* GetAI_npc_guardian(Creature* pCreature)
{
    return new npc_guardianAI(pCreature);
}

/*########
# npc_innkeeper
#########*/

// Script applied to all innkeepers by npcflag.
// Are there any known innkeepers that does not hape the options in the below?
// (remember gossipHello is not called unless npcflag|1 is present)

enum
{
    TEXT_ID_WHAT_TO_DO              = 1853,

    SPELL_TRICK_OR_TREAT            = 24751,                 // create item or random buff
    SPELL_TRICK_OR_TREATED          = 24755,                 // buff player get when tricked or treated
};

#define GOSSIP_ITEM_TRICK_OR_TREAT  "Trick or Treat!"
#define GOSSIP_ITEM_WHAT_TO_DO      "What can I do at an Inn?"

bool GossipHello_npc_innkeeper(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PrepareGossipMenu(pCreature, pPlayer->GetDefaultGossipMenuForSource(pCreature));

    if (IsHolidayActive(HOLIDAY_HALLOWS_END) && !pPlayer->HasAura(SPELL_TRICK_OR_TREATED, EFFECT_INDEX_0))
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_TRICK_OR_TREAT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);

    // Should only apply to innkeeper close to start areas.
    if (AreaTableEntry const* pAreaEntry = GetAreaEntryByAreaID(pCreature->GetAreaId()))
    {
        if (pAreaEntry->flags & AREA_FLAG_LOWLEVEL)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ITEM_WHAT_TO_DO, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
    }

    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    pPlayer->SendPreparedGossip(pCreature);
    return true;
}

bool GossipSelect_npc_innkeeper(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->SEND_GOSSIP_MENU(TEXT_ID_WHAT_TO_DO, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pCreature->CastSpell(pPlayer, SPELL_TRICK_OR_TREAT, true);
            break;
        case GOSSIP_OPTION_VENDOR:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_OPTION_INNKEEPER:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->SetBindPoint(pCreature->GetObjectGuid());
            break;
    }

    return true;
}

/*######
## npc_mount_vendor
######*/

bool GossipHello_npc_mount_vendor(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    bool canBuy;
    canBuy = false;
    uint32 vendor = pCreature->GetEntry();
    uint8 race = pPlayer->getRace();

    switch (vendor)
    {
        case 384:                                           //Katie Hunter
        case 1460:                                          //Unger Statforth
        case 2357:                                          //Merideth Carlson
        case 4885:                                          //Gregor MacVince
            if (pPlayer->GetReputationRank(72) != REP_EXALTED && race != RACE_HUMAN)
                pPlayer->SEND_GOSSIP_MENU(5855, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 1261:                                          //Veron Amberstill
            if (pPlayer->GetReputationRank(47) != REP_EXALTED && race != RACE_DWARF)
                pPlayer->SEND_GOSSIP_MENU(5856, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 3362:                                          //Ogunaro Wolfrunner
            if (pPlayer->GetReputationRank(76) != REP_EXALTED && race != RACE_ORC)
                pPlayer->SEND_GOSSIP_MENU(5841, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 3685:                                          //Harb Clawhoof
            if (pPlayer->GetReputationRank(81) != REP_EXALTED && race != RACE_TAUREN)
                pPlayer->SEND_GOSSIP_MENU(5843, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 4730:                                          //Lelanai
            if (pPlayer->GetReputationRank(69) != REP_EXALTED && race != RACE_NIGHTELF)
                pPlayer->SEND_GOSSIP_MENU(5844, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 4731:                                          //Zachariah Post
            if (pPlayer->GetReputationRank(68) != REP_EXALTED && race != RACE_UNDEAD)
                pPlayer->SEND_GOSSIP_MENU(5840, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 7952:                                          //Zjolnir
            if (pPlayer->GetReputationRank(530) != REP_EXALTED && race != RACE_TROLL)
                pPlayer->SEND_GOSSIP_MENU(5842, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 7955:                                          //Milli Featherwhistle
            if (pPlayer->GetReputationRank(54) != REP_EXALTED && race != RACE_GNOME)
                pPlayer->SEND_GOSSIP_MENU(5857, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 16264:                                         //Winaestra
            if (pPlayer->GetReputationRank(911) != REP_EXALTED && race != RACE_BLOODELF)
                pPlayer->SEND_GOSSIP_MENU(10305, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
        case 17584:                                         //Torallius the Pack Handler
            if (pPlayer->GetReputationRank(930) != REP_EXALTED && race != RACE_DRAENEI)
                pPlayer->SEND_GOSSIP_MENU(10239, pCreature->GetObjectGuid());
            else canBuy = true;
            break;
    }

    if (canBuy)
    {
        if (pCreature->isVendor())
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }
    return true;
}

bool GossipSelect_npc_mount_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

/*######
## npc_sayge
######*/

#define SPELL_DMG       23768                               //dmg
#define SPELL_RES       23769                               //res
#define SPELL_ARM       23767                               //arm
#define SPELL_SPI       23738                               //spi
#define SPELL_INT       23766                               //int
#define SPELL_STM       23737                               //stm
#define SPELL_STR       23735                               //str
#define SPELL_AGI       23736                               //agi
#define SPELL_FORTUNE   23765                               //faire fortune

bool GossipHello_npc_sayge(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu(pCreature->GetObjectGuid());

    if (pPlayer->HasSpellCooldown(SPELL_INT) ||
        pPlayer->HasSpellCooldown(SPELL_ARM) ||
        pPlayer->HasSpellCooldown(SPELL_DMG) ||
        pPlayer->HasSpellCooldown(SPELL_RES) ||
        pPlayer->HasSpellCooldown(SPELL_STR) ||
        pPlayer->HasSpellCooldown(SPELL_AGI) ||
        pPlayer->HasSpellCooldown(SPELL_STM) ||
        pPlayer->HasSpellCooldown(SPELL_SPI))
        pPlayer->SEND_GOSSIP_MENU(7393, pCreature->GetObjectGuid());
    else
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Yes", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);
        pPlayer->SEND_GOSSIP_MENU(7339, pCreature->GetObjectGuid());
    }

    return true;
}

void SendAction_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Slay the Man",                      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Turn him over to liege",            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confiscate the corn",               GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let him go and have the corn",      GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
            pPlayer->SEND_GOSSIP_MENU(7340, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painfully",     GOSSIP_SENDER_MAIN+1, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Execute your friend painlessly",    GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let your friend go",                GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7341, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Confront the diplomat",             GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Show not so quiet defiance",        GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Remain quiet",                      GOSSIP_SENDER_MAIN+2, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7361, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Speak against your brother openly", GOSSIP_SENDER_MAIN+6, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Help your brother in",              GOSSIP_SENDER_MAIN+7, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Keep your brother out without letting him know", GOSSIP_SENDER_MAIN+8, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7362, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, keep gold",            GOSSIP_SENDER_MAIN+5, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Take credit, share the gold",       GOSSIP_SENDER_MAIN+4, GOSSIP_ACTION_INFO_DEF);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Let the knight take credit",        GOSSIP_SENDER_MAIN+3, GOSSIP_ACTION_INFO_DEF);
            pPlayer->SEND_GOSSIP_MENU(7363, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF:
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Thanks",                            GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->SEND_GOSSIP_MENU(7364, pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pCreature->CastSpell(pPlayer, SPELL_FORTUNE, false);
            pPlayer->SEND_GOSSIP_MENU(7365, pCreature->GetObjectGuid());
            break;
    }
}

bool GossipSelect_npc_sayge(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiSender)
    {
        case GOSSIP_SENDER_MAIN:
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+1:
            pCreature->CastSpell(pPlayer, SPELL_DMG, false);
            pPlayer->AddSpellCooldown(SPELL_DMG,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+2:
            pCreature->CastSpell(pPlayer, SPELL_RES, false);
            pPlayer->AddSpellCooldown(SPELL_RES,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+3:
            pCreature->CastSpell(pPlayer, SPELL_ARM, false);
            pPlayer->AddSpellCooldown(SPELL_ARM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+4:
            pCreature->CastSpell(pPlayer, SPELL_SPI, false);
            pPlayer->AddSpellCooldown(SPELL_SPI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+5:
            pCreature->CastSpell(pPlayer, SPELL_INT, false);
            pPlayer->AddSpellCooldown(SPELL_INT,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+6:
            pCreature->CastSpell(pPlayer, SPELL_STM, false);
            pPlayer->AddSpellCooldown(SPELL_STM,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+7:
            pCreature->CastSpell(pPlayer, SPELL_STR, false);
            pPlayer->AddSpellCooldown(SPELL_STR,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
        case GOSSIP_SENDER_MAIN+8:
            pCreature->CastSpell(pPlayer, SPELL_AGI, false);
            pPlayer->AddSpellCooldown(SPELL_AGI,0,time(NULL) + 7200);
            SendAction_npc_sayge(pPlayer, pCreature, uiAction);
            break;
    }
    return true;
}

/*######
## npc_tabard_vendor
######*/

enum
{
    QUEST_TRUE_MASTERS_OF_LIGHT                = 9737,
    QUEST_THE_UNWRITTEN_PROPHECY               = 9762,
    QUEST_INTO_THE_BREACH                      = 10259,
    QUEST_BATTLE_OF_THE_CRIMSON_WATCH          = 10781,
    QUEST_SHARDS_OF_AHUNE                      = 11972,

    ACHIEVEMENT_EXPLORE_NORTHREND              = 45,
    ACHIEVEMENT_TWENTYFIVE_TABARDS             = 1021,
    ACHIEVEMENT_THE_LOREMASTER_A               = 1681,
    ACHIEVEMENT_THE_LOREMASTER_H               = 1682,

    ITEM_TABARD_OF_THE_HAND                    = 24344,
    ITEM_TABARD_OF_THE_BLOOD_KNIGHT            = 25549,
    ITEM_TABARD_OF_THE_PROTECTOR               = 28788,
    ITEM_OFFERING_OF_THE_SHATAR                = 31408,
    ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI   = 31404,
    ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI  = 31405,
    ITEM_TABARD_OF_THE_SUMMER_SKIES            = 35279,
    ITEM_TABARD_OF_THE_SUMMER_FLAMES           = 35280,
    ITEM_TABARD_OF_THE_ACHIEVER                = 40643,
    ITEM_LOREMASTERS_COLORS                    = 43300,
    ITEM_TABARD_OF_THE_EXPLORER                = 43348,

    SPELL_TABARD_OF_THE_BLOOD_KNIGHT           = 54974,
    SPELL_TABARD_OF_THE_HAND                   = 54976,
    SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI  = 54977,
    SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI = 54982,
    SPELL_TABARD_OF_THE_ACHIEVER               = 55006,
    SPELL_TABARD_OF_THE_PROTECTOR              = 55008,
    SPELL_LOREMASTERS_COLORS                   = 58194,
    SPELL_TABARD_OF_THE_EXPLORER               = 58224,
    SPELL_TABARD_OF_SUMMER_SKIES               = 62768,
    SPELL_TABARD_OF_SUMMER_FLAMES              = 62769
};

#define GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT               "I've lost my Tabard of Blood Knight."
#define GOSSIP_LOST_TABARD_OF_THE_HAND                   "I've lost my Tabard of the Hand."
#define GOSSIP_LOST_TABARD_OF_THE_PROTECTOR              "I've lost my Tabard of the Protector."
#define GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI  "I've lost my Green Trophy Tabard of the Illidari."
#define GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI "I've lost my Purple Trophy Tabard of the Illidari."
#define GOSSIP_LOST_TABARD_OF_SUMMER_SKIES               "I've lost my Tabard of Summer Skies."
#define GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES              "I've lost my Tabard of Summer Flames."
#define GOSSIP_LOST_LOREMASTERS_COLORS                   "I've lost my Loremaster's Colors."
#define GOSSIP_LOST_TABARD_OF_THE_EXPLORER               "I've lost my Tabard of the Explorer."
#define GOSSIP_LOST_TABARD_OF_THE_ACHIEVER               "I've lost my Tabard of the Achiever."

bool GossipHello_npc_tabard_vendor(Player* pPlayer, Creature* pCreature)
{
    bool m_bLostBloodKnight = false;
    bool m_bLostHand        = false;
    bool m_bLostProtector   = false;
    bool m_bLostIllidari    = false;
    bool m_bLostSummer      = false;

    //Tabard of the Blood Knight
    if (pPlayer->GetQuestRewardStatus(QUEST_TRUE_MASTERS_OF_LIGHT) && !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_BLOOD_KNIGHT, 1, true))
        m_bLostBloodKnight = true;

    //Tabard of the Hand
    if (pPlayer->GetQuestRewardStatus(QUEST_THE_UNWRITTEN_PROPHECY) && !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_HAND, 1, true))
        m_bLostHand = true;

    //Tabard of the Protector
    if (pPlayer->GetQuestRewardStatus(QUEST_INTO_THE_BREACH) && !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_PROTECTOR, 1, true))
        m_bLostProtector = true;

    //Green Trophy Tabard of the Illidari
    //Purple Trophy Tabard of the Illidari
    if (pPlayer->GetQuestRewardStatus(QUEST_BATTLE_OF_THE_CRIMSON_WATCH) &&
        (!pPlayer->HasItemCount(ITEM_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
        !pPlayer->HasItemCount(ITEM_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, 1, true) &&
        !pPlayer->HasItemCount(ITEM_OFFERING_OF_THE_SHATAR, 1, true)))
        m_bLostIllidari = true;

    //Tabard of Summer Skies
    //Tabard of Summer Flames
    if (pPlayer->GetQuestRewardStatus(QUEST_SHARDS_OF_AHUNE) &&
        !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_SKIES, 1, true) &&
        !pPlayer->HasItemCount(ITEM_TABARD_OF_THE_SUMMER_FLAMES, 1, true))
        m_bLostSummer = true;

    if (m_bLostBloodKnight || m_bLostHand || m_bLostProtector || m_bLostIllidari || m_bLostSummer)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);

        if (m_bLostBloodKnight)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_BLOOD_KNIGHT, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +1);

        if (m_bLostHand)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_HAND, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF +2);

        if (m_bLostProtector)
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_THE_PROTECTOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+3);

        if (m_bLostIllidari)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+4);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+5);
        }

        if (m_bLostSummer)
        {
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_SKIES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+6);
            pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_LOST_TABARD_OF_SUMMER_FLAMES, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+7);
        }

        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }
    else
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_tabard_vendor(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    switch(uiAction)
    {
        case GOSSIP_ACTION_TRADE:
            pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
            break;
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_THE_BLOOD_KNIGHT, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+2:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_THE_HAND, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+3:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_THE_PROTECTOR, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+4:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_GREEN_TROPHY_TABARD_OF_THE_ILLIDARI, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+5:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_PURPLE_TROPHY_TABARD_OF_THE_ILLIDARI, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+6:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_SUMMER_SKIES, false);
            break;
        case GOSSIP_ACTION_INFO_DEF+7:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_TABARD_OF_SUMMER_FLAMES, false);
            break;
    }
    return true;
}

/*######
## npc_mirror_image
######*/

enum MirrorImageSpells
{
    SPELL_CLONE_CASTER      = 45204,
    SPELL_CLONE_CASTER_1    = 69837,
//    SPELL_CLONE_CASTER_1  = 58836,
    SPELL_CLONE_THREAT      = 58838,

    SPELL_FROSTBOLT         = 59638,
    SPELL_FIREBOLT          = 88082,
    SPELL_ARCANE_BLAST      = 88084,

    SPELL_MAGEARMOR         = 6117,
    SPELL_FROSTARMOR        = 7302,
    SPELL_MOLTENARMOR       = 30482,

    SPELL_INVISIBILITY      = 32612,

    SPELL_CHECK_TIMER       = 1000,
};

struct MANGOS_DLL_DECL npc_mirror_imageAI : public ScriptedAI
{
    npc_mirror_imageAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool inCombat;
    Unit *owner;
    float angle;
    bool movement;
    uint32 m_auraCheckTimer;
    uint32 m_spell;

    void Reset() 
    {
        m_auraCheckTimer = SPELL_CHECK_TIMER;

        owner = m_creature->GetOwner();
        if (!owner) 
            return;

        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_2, owner->GetUInt32Value(UNIT_FIELD_BYTES_2));
        m_creature->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, DEFAULT_WORLD_OBJECT_SIZE);
        m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID,     owner->GetUInt32Value(PLAYER_VISIBLE_ITEM_16_ENTRYID));
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, owner->GetUInt32Value(PLAYER_VISIBLE_ITEM_17_ENTRYID));
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, owner->GetUInt32Value(PLAYER_VISIBLE_ITEM_18_ENTRYID));
        m_creature->SetSpeedRate(MOVE_RUN, owner->GetSpeedRate(MOVE_RUN), true);

        m_creature->SetMaxHealth(owner->GetMaxHealth() / 2);
        m_creature->SetHealth(owner->GetMaxHealth() / 2);

        m_creature->SetMaxPower(POWER_MANA, owner->GetMaxPower(POWER_MANA) / 2);
        m_creature->SetPower(POWER_MANA, owner->GetMaxPower(POWER_MANA) / 2);

        inCombat = false;
        movement = false;

        if (!m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            angle = m_creature->GetAngle(owner);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST + 3.0f, angle);
        }

        if (owner->IsPvP())
            m_creature->SetPvP(true);
        if (owner->IsFFAPvP())
            m_creature->SetFFAPvP(true);

        m_spell = SPELL_FROSTBOLT;
        // Glyph of Mirror Image
        if (owner->GetTypeId() == TYPEID_PLAYER && owner->HasAura(63093))
        {
            switch (((Player*)owner)->GetPrimaryTalentTree(((Player*)owner)->GetActiveSpec()))
            {
                case 799:
                    m_spell = SPELL_ARCANE_BLAST;
                    break;
                case 851:
                    m_spell = SPELL_FIREBOLT;
                    break;
            }
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            //if (owner)
            //     m_creature->CastSpell(m_creature, SPELL_CLONE_THREAT, true, NULL, NULL, owner->GetGUID());
            m_creature->clearUnitState(UNIT_STAT_FOLLOW);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            m_creature->AddThreat(pWho, 100.0f);
            DoStartMovement(pWho, 30.0f);
            SetCombatMovement(true);
            inCombat = true;
        }
    }

    void EnterEvadeMode()
    {
        if (m_creature->IsInEvadeMode() || !m_creature->isAlive())
            return;

        inCombat = false;

        m_creature->AttackStop();
        m_creature->CombatStop(true);
        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            angle = m_creature->GetAngle(owner);
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST + 3.0f, angle);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!owner || !owner->isAlive()) 
            m_creature->ForcedDespawn();

        if (m_auraCheckTimer <= diff)
        {
            m_auraCheckTimer = SPELL_CHECK_TIMER;
            if (owner)
            {
                if (!m_creature->HasAura(SPELL_CLONE_CASTER))
                    m_creature->CastSpell(m_creature, SPELL_CLONE_CASTER, true, NULL, NULL, owner->GetGUID());

                if (!m_creature->HasAura(SPELL_CLONE_CASTER_1))
                    m_creature->CastSpell(m_creature, SPELL_CLONE_CASTER_1, true, NULL, NULL, owner->GetGUID());

                if (owner->HasAura(SPELL_FROSTARMOR) && !m_creature->HasAura(SPELL_FROSTARMOR))
                    m_creature->CastSpell(m_creature, SPELL_FROSTARMOR, true);

                if (owner->HasAura(SPELL_MAGEARMOR) && !m_creature->HasAura(SPELL_MAGEARMOR))
                    m_creature->CastSpell(m_creature, SPELL_MAGEARMOR, true);

                if (owner->HasAura(SPELL_MOLTENARMOR) && !m_creature->HasAura(SPELL_MOLTENARMOR))
                    m_creature->CastSpell(m_creature, SPELL_MOLTENARMOR, true);

                if (owner->HasAura(SPELL_INVISIBILITY))
                {
                    if (!m_creature->HasAura(SPELL_INVISIBILITY))
                        m_creature->CastSpell(m_creature, SPELL_INVISIBILITY, true);
                }
                else
                    m_creature->RemoveAurasDueToSpell(SPELL_INVISIBILITY);
            }
        }
        else
            m_auraCheckTimer -= diff;

        Unit* ownerVictim = owner ? owner->getVictim() : NULL;
        if (m_creature->getVictim())
        {
            if (m_creature->getVictim()->hasUnitState(UNIT_STAT_NO_FREE_MOVE))
            {
                m_creature->AttackStop();
                return;
            }
            else if (ownerVictim && m_creature->getVictim() != ownerVictim && !ownerVictim->hasUnitState(UNIT_STAT_NO_FREE_MOVE))
                AttackStart(ownerVictim);
        }
        else if (ownerVictim && !ownerVictim->hasUnitState(UNIT_STAT_NO_FREE_MOVE))
            AttackStart(ownerVictim);

        if (inCombat && !m_creature->getVictim())
        {
            EnterEvadeMode();
            return;
        }

        if (!inCombat)
            return;

        if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 30.0f))
        {
            movement = false;
            if (!m_creature->IsNonMeleeSpellCasted(false))
                DoCastSpellIfCan(m_creature->getVictim(), m_spell);
        }
        else
        {
            if (!movement) 
            {
                DoStartMovement(m_creature->getVictim(), 20.0f);
                movement = true;
            }
        }

        //DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_mirror_image(Creature* pCreature)
{
    return new npc_mirror_imageAI(pCreature);
};

/*####
 ## npc_snake_trap_serpents - Summonned snake id are 19921 and 19833
 ####*/

// Viper
#define SPELL_MIND_NUMBING_POISON    25810
// Viper
#define SPELL_CRIPPLING_POISON       30981
// Venomous Snake
#define SPELL_DEADLY_POISON          34655

#define MOB_VIPER 19921
#define MOB_VENOM_SNIKE 19833

struct MANGOS_DLL_DECL npc_snake_trap_serpentsAI : public ScriptedAI
{
    npc_snake_trap_serpentsAI(Creature *c) : ScriptedAI(c) { Reset(); }

    uint32 m_spellTimer;
    Unit* m_owner;

    uint32 spellId;

    void Reset()
    {
        m_spellTimer = 500;

        if (m_creature->GetEntry() == MOB_VIPER)
            spellId = urand(0, 1) ? SPELL_MIND_NUMBING_POISON : SPELL_CRIPPLING_POISON;
        else
            spellId = SPELL_DEADLY_POISON;

        m_owner = m_creature->GetCharmerOrOwner();
        if (!m_owner)
            return;

        m_creature->SetLevel(m_owner->getLevel());
        m_creature->setFaction(m_owner->getFaction());
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->SetInCombatWith(pWho);
            m_creature->AddThreat(pWho, 100.0f);
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(pWho);
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->getVictim())
        {
            if (m_owner && m_owner->getVictim())
                AttackStart(m_owner->getVictim());
            return;
        }

        if (m_spellTimer <= diff)
        {
            if (m_creature->GetEntry() == MOB_VIPER)
                m_spellTimer = urand(3000, 5000);
            else if (m_creature->GetEntry() == MOB_VENOM_SNIKE)
                m_spellTimer = urand(2500, 4500);

            Unit* target = m_creature->getVictim();

            Unit::SpellAuraHolderBounds spair = target->GetSpellAuraHolderBounds(spellId);
            for (Unit::SpellAuraHolderMap::const_iterator iter = spair.first; iter != spair.second; ++iter)
                if (iter->second->GetCasterGuid() != m_creature->GetObjectGuid())
                    return;

            if (m_creature->GetEntry() == MOB_VIPER)                // Viper - 19921
            {
                if (!urand(0, 2))                                   // 33% chance to cast
                    DoCast(target, spellId);
            }
            else if (m_creature->GetEntry() == MOB_VENOM_SNIKE)     // Venomous Snake - 19833
            {
                if (urand(0, 1) == 0)                               // 80% chance to cast
                    DoCast(target, spellId);
            }
        }
        else
            m_spellTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_snake_trap_serpents(Creature* pCreature)
{
    return new npc_snake_trap_serpentsAI(pCreature);
}

struct MANGOS_DLL_DECL npc_rune_blade : public ScriptedAI
{
    npc_rune_blade(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    Unit* owner;

    void Reset()
    {
        owner = m_creature->GetOwner();
        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        // Cannot be Selected or Attacked
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        m_creature->SetLevel(owner->getLevel());
        m_creature->setFaction(owner->getFaction());

        // Add visible weapon
        if (Item const * item = ((Player *)owner)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
            m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, item->GetProto()->ItemId);

        // Add stats scaling
        int32 damageDone=owner->CalculateDamage(BASE_ATTACK, true); // might be average damage instead ?
        int32 meleeSpeed=owner->m_modAttackSpeedPct[BASE_ATTACK];
        m_creature->CastCustomSpell(m_creature, 51906, &damageDone, &meleeSpeed, NULL, true);

        // Visual Glow
        m_creature->CastSpell(m_creature, 53160, true);

        SetCombatMovement(true);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!owner) return;

        if (!m_creature->getVictim())
        {
            if (owner->getVictim())
                AttackStart(owner->getVictim());
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_rune_blade(Creature* pCreature)
{
    return new npc_rune_blade(pCreature);
}

/*########
# mob_death_knight_gargoyle AI
#########*/

// UPDATE `creature_template` SET `ScriptName` = 'mob_death_knight_gargoyle' WHERE `entry` = '27829';

enum GargoyleSpells
{
    SPELL_GARGOYLE_STRIKE = 51963      // Don't know if this is the correct spell, it does about 700-800 damage points
};

struct MANGOS_DLL_DECL npc_death_knight_gargoyle : public ScriptedAI
{
    npc_death_knight_gargoyle(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool inCombat;
    Unit *owner;


    void Reset() 
    {
        owner = m_creature->GetOwner();
        if (!owner)
            return;

        m_creature->SetLevel(owner->getLevel());
        m_creature->setFaction(owner->getFaction());

        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        m_creature->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        m_creature->SetLevitate(true);

        inCombat = false;

        float fPosX, fPosY, fPosZ;
        owner->GetPosition(fPosX, fPosY, fPosZ);

        m_creature->NearTeleportTo(fPosX, fPosY, fPosZ+10.0f, m_creature->GetAngle(owner));

        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST + 3.0f, m_creature->GetAngle(owner));
        }

        if(owner->IsPvP())
            m_creature->SetPvP(true);
        if(owner->IsFFAPvP())
            m_creature->SetFFAPvP(true);
    }

    void EnterEvadeMode()
    {
        if (m_creature->IsInEvadeMode() || !m_creature->isAlive())
            return;

        inCombat = false;

        m_creature->AttackStop();
        m_creature->CombatStop(true);
        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST + 3.0f, m_creature->GetAngle(owner));
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->clearUnitState(UNIT_STAT_FOLLOW);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            m_creature->AddThreat(pWho, 100.0f);
            DoStartMovement(pWho, 10.0f);
            SetCombatMovement(true);
            inCombat = true;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!owner || !owner->IsInWorld() || owner->isDead())
        {
            m_creature->ForcedDespawn();
            return;
        }

        if (!m_creature->getVictim())
            if (owner && owner->getVictim())
                AttackStart(owner->getVictim());

        if (m_creature->getVictim() && m_creature->getVictim() != owner->getVictim())
            AttackStart(owner->getVictim());

        if (inCombat && !m_creature->getVictim())
        {
            EnterEvadeMode();
            return;
        }

        if (!inCombat)
            return;

        DoCastSpellIfCan(m_creature->getVictim(), SPELL_GARGOYLE_STRIKE);
    }
};

CreatureAI* GetAI_npc_death_knight_gargoyle(Creature* pCreature)
{
    return new npc_death_knight_gargoyle(pCreature);
}

/*########
# npc_risen_ally AI
#########*/

struct MANGOS_DLL_DECL npc_risen_allyAI : public ScriptedAI
{
    npc_risen_allyAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
    }

    uint32 StartTimer;

    void Reset()
    {
        StartTimer = 2000;
        m_creature->SetSheath(SHEATH_STATE_MELEE);
        m_creature->SetByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_ABANDONED);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
        m_creature->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        m_creature->SetUInt32Value(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_NONE);
        m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);
        if (Player* creator = m_creature->GetMap()->GetPlayer(m_creature->GetCreatorGuid()))
        {
           m_creature->SetLevel(creator->getLevel());
           m_creature->setFaction(creator->getFaction());
        }
    }

    void JustDied(Unit* killer)
    {
        if (!m_creature)
            return;

        if (Player* creator = m_creature->GetMap()->GetPlayer(m_creature->GetCreatorGuid()))
        {
            creator->RemoveAurasDueToSpell(46619);
            creator->RemoveAurasDueToSpell(62218);
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho) return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 10.0f);
            SetCombatMovement(true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(StartTimer > uiDiff)
        {
            StartTimer -= uiDiff;
            return;
        }

        if(!m_creature->isCharmed())
            m_creature->ForcedDespawn();

        if (m_creature->isInCombat())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_risen_ally(Creature* pCreature)
{
    return new npc_risen_allyAI(pCreature);
}

/*########
# npc_explosive_decoyAI
#########*/

struct MANGOS_DLL_DECL npc_explosive_decoyAI : public ScriptedAI
{
    npc_explosive_decoyAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    Player* p_owner;

    void Reset()
    {
        p_owner = NULL;
        SetCombatMovement(false);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_creature || !m_creature->isAlive())
            return;

        if (uiDamage > 0)
            m_creature->CastSpell(m_creature, 53273, true);
    }

    void JustDied(Unit* killer)
    {
        if (!m_creature || !p_owner)
            return;

        SpellEntry const* createSpell = GetSpellStore()->LookupEntry(m_creature->GetUInt32Value(UNIT_CREATED_BY_SPELL));

        if (createSpell)
            p_owner->SendCooldownEvent(createSpell);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (p_owner)
            return;

        p_owner = m_creature->GetMap()->GetPlayer(m_creature->GetCreatorGuid());

        if (!p_owner) 
            return;

        m_creature->setFaction(p_owner->getFaction());
        m_creature->SetCreatorGuid(ObjectGuid());
    }
};

CreatureAI* GetAI_npc_explosive_decoy(Creature* pCreature)
{
    return new npc_explosive_decoyAI(pCreature);
}

/*########
# npc_eye_of_kilroggAI
#########*/

struct MANGOS_DLL_DECL npc_eye_of_kilrogg : public ScriptedAI
{
    npc_eye_of_kilrogg(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    Player* p_owner;

    void Reset()
    {
        p_owner = NULL;
    }

    void UpdateAI(const uint32 diff)
    {
        if (p_owner)
            return;

        p_owner = (Player*)m_creature->GetCharmerOrOwner();

        if (!p_owner)
            return;

        if (!m_creature->HasAura(2585))
            m_creature->CastSpell(m_creature, 2585, true);

        if (p_owner->HasAura(58081))
            m_creature->CastSpell(m_creature, 58083, true);

    }
};

CreatureAI* GetAI_npc_eye_of_kilrogg(Creature* pCreature)
{
    return new npc_eye_of_kilrogg(pCreature);
}

/*########
# npc_fire_bunny
#########*/

enum
{
    NPC_FIRE_BUNNY          = 23686,
    SPELL_THROW_BUCKET      = 42339,
    SPELL_EXTINGUISH_VISUAL = 42348,
    SPELL_FLAMES_LARGE      = 42075,
    SPELL_SMOKE             = 42355,
    SPELL_CONFLAGRATE       = 42132,
    SPELL_HORSEMAN_MOUNT    = 48024,
    SPELL_HORSMAN_SHADE_VIS = 43904,
    SPELL_Q_STOP_THE_FIRE   = 42242,
    SPELL_Q_LET_THE_FIRES_C = 47775,
    SPELL_LAUGH_DELAYED_8   = 43893,

    PHASE_INITIAL           = 0,
    PHASE_1ST_SPEACH        = 1,
    PHASE_2ND_SPEACH        = 2,
    PHASE_FAIL              = 3,
    PHASE_END               = 4,

    YELL_IGNITE             = -1110001,
    YELL_1ST                = -1110002,
    YELL_2ND                = -1110003,
    YELL_FAIL               = -1110004,
    YELL_VICTORY            = -1110005,
    YELL_CONFLAGRATION      = -1110006
};

struct MANGOS_DLL_DECL npc_horseman_fire_bunnyAI : public Scripted_NoMovementAI
{
    npc_horseman_fire_bunnyAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        Reset();
        m_creature->RemoveAllAuras();
    }

    void Reset()
    {
        if (!m_creature->isAlive())
            m_creature->Respawn();
    }

    void SpellHit(Unit* pWho, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_THROW_BUCKET)
        {
            pWho->CastSpell(m_creature, SPELL_EXTINGUISH_VISUAL, false);
            m_creature->RemoveAurasDueToSpell(SPELL_FLAMES_LARGE);
        }
        if (pSpell->Id == SPELL_CONFLAGRATE)
        {
            DoCastSpellIfCan(m_creature, SPELL_FLAMES_LARGE);
            m_creature->RemoveAurasDueToSpell(SPELL_CONFLAGRATE);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_creature->SelectHostileTarget() || m_creature->getVictim())
            EnterEvadeMode(); // Dunno how to prevent them from entering combat while hit by SPELL_EXTINGUISH_VISUAL (spelleffect 2)
    }
};

CreatureAI* GetAI_npc_horseman_fire_bunny(Creature* pCreature)
{
    return new npc_horseman_fire_bunnyAI (pCreature);
};

/*########
# npc_shade of horseman
#########*/

struct MANGOS_DLL_DECL npc_shade_of_horsemanAI : public ScriptedAI
{
    npc_shade_of_horsemanAI(Creature* pCreature) : ScriptedAI(pCreature){Reset();}

    uint8 uiPhase;
    uint32 m_uiEventTimer;
    uint32 m_uiConflagrationTimer;
    uint32 m_uiConflagrationProcTimer;
    bool bIsConflagrating;

    GUIDList lFireBunnies;

    void Reset()
    {
        if (!m_creature->isAlive())
            m_creature->Respawn();

        uiPhase = PHASE_INITIAL;
        lFireBunnies.clear();
        bIsConflagrating = true;

        m_uiEventTimer = 2.5*MINUTE*IN_MILLISECONDS;

        m_uiConflagrationTimer = 30000;
        m_uiConflagrationProcTimer = 2000;

        DoCastSpellIfCan(m_creature, SPELL_HORSEMAN_MOUNT);
        DoCastSpellIfCan(m_creature, SPELL_HORSMAN_SHADE_VIS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (uiPhase == PHASE_INITIAL)
        {
            DoScriptText(YELL_IGNITE, m_creature);
            ++uiPhase;
            return;
        }
        else if (uiPhase == PHASE_END)
            return;

        if (!bIsConflagrating)
        {
            bool IsVictory = true;
            for (GUIDList::iterator itr = lFireBunnies.begin(); itr != lFireBunnies.end(); ++itr)
                if (Creature* pFireBunny = m_creature->GetMap()->GetCreature(*itr))
                    if (pFireBunny->HasAura(SPELL_FLAMES_LARGE))
                        IsVictory = false;
            if (IsVictory)
            {
                DoScriptText(YELL_VICTORY, m_creature);
                DoCastSpellIfCan(m_creature, SPELL_Q_STOP_THE_FIRE, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_Q_LET_THE_FIRES_C, CAST_TRIGGERED);
                m_creature->ForcedDespawn(5000);
                uiPhase = PHASE_END;
                return;
            }
        }

        if (m_uiEventTimer < uiDiff)
        {
            switch(uiPhase)
            {
                case PHASE_1ST_SPEACH:
                    DoScriptText(YELL_1ST, m_creature);
                    m_uiEventTimer = 2 *MINUTE*IN_MILLISECONDS;
                    break;

                case PHASE_2ND_SPEACH:
                    DoScriptText(YELL_2ND, m_creature);
                    m_uiEventTimer = 0.5 *MINUTE*IN_MILLISECONDS;
                    break;
                case PHASE_FAIL:
                    DoScriptText(YELL_FAIL, m_creature);
                    m_creature->ForcedDespawn(10000);
                    for (GUIDList::iterator itr = lFireBunnies.begin(); itr != lFireBunnies.end(); ++itr)
                        if (Creature* pFireBunny = m_creature->GetMap()->GetCreature(*itr))
                        {
                            if (pFireBunny->HasAura(SPELL_FLAMES_LARGE))
                                pFireBunny->RemoveAurasDueToSpell(SPELL_FLAMES_LARGE);
                            pFireBunny->CastSpell(m_creature, SPELL_SMOKE, true);
                            pFireBunny->ForcedDespawn(60000);
                        }
                    break;
            }
            ++uiPhase;
            DoCastSpellIfCan(m_creature, SPELL_LAUGH_DELAYED_8);
        }
        else
            m_uiEventTimer -= uiDiff;

        if (m_uiConflagrationTimer < uiDiff)
        {
            bIsConflagrating = !bIsConflagrating;
            m_creature->GetMotionMaster()->MovementExpired();
            m_creature->GetMotionMaster()->MoveTargetedHome();
            m_uiConflagrationProcTimer = 2000;
            m_uiConflagrationTimer = bIsConflagrating ? 10000 : 30000;
            if (bIsConflagrating)
                DoScriptText(YELL_CONFLAGRATION, m_creature);
        }
        else
            m_uiConflagrationTimer -= uiDiff;

        if (bIsConflagrating)
            if (m_uiConflagrationProcTimer < uiDiff)
            {
                m_uiConflagrationProcTimer = 2000;
                if (lFireBunnies.empty())
                {
                    std::list<Creature*> tempFireBunnies;
                    GetCreatureListWithEntryInGrid(tempFireBunnies, m_creature, NPC_FIRE_BUNNY, 50.0f);
                    for (std::list<Creature*>::iterator itr = tempFireBunnies.begin(); itr != tempFireBunnies.end(); ++itr)
                        lFireBunnies.push_back((*itr)->GetObjectGuid());
                }

                if (lFireBunnies.empty())
                {
                    m_creature->ForcedDespawn(5000);
                    error_log("Missing DB spawns of Fire Bunnies (Horseman Village Event)");
                    uiPhase = PHASE_END;
                    return;
                }

                if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == POINT_MOTION_TYPE)
                    return;
                
                for (GUIDList::iterator itr = lFireBunnies.begin(); itr != lFireBunnies.end(); ++itr)
                    if (Creature* pFireBunny = m_creature->GetMap()->GetCreature(*itr))
                        if (!pFireBunny->HasAura(SPELL_FLAMES_LARGE))
                        {
                            if (DoCastSpellIfCan(pFireBunny, SPELL_CONFLAGRATE) != CAST_OK)
                            {
                                float x,y,z;
                                pFireBunny->GetPosition(x,y,z);
                                pFireBunny->GetClosePoint(x,y,z,0,5,0);
                                m_creature->GetMotionMaster()->MovePoint(0, x,y,z+15);
                                break;
                            }
                        }
            }
            else
                m_uiConflagrationProcTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_shade_of_horseman(Creature* pCreature)
{
    return new npc_shade_of_horsemanAI (pCreature);
};

enum
{
    NPC_PHASE_CHANGER_1   = 2000006410,
    NPC_PHASE_CHANGER_2   = 2000006411,
    NPC_PHASE_CHANGER_3   = 2000006412,
    NPC_PHASE_CHANGER_5   = 2000006414,
    NPC_PHASE_CHANGER_6   = 2000006415,
    NPC_PHASE_CHANGER_7   = 2000006416,
    NPC_PHASE_CHANGER_8   = 2000006417,
    NPC_PHASE_CHANGER_DECLINE  = 2000006413,
};

bool GossipHello_npc_phase_changer(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_1, GOSSIP_SENDER_MAIN, 1);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_2, GOSSIP_SENDER_MAIN, 2);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_3, GOSSIP_SENDER_MAIN, 3);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_5, GOSSIP_SENDER_MAIN, 4);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_6, GOSSIP_SENDER_MAIN, 5);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_7, GOSSIP_SENDER_MAIN, 6);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_8, GOSSIP_SENDER_MAIN, 7);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_PHASE_CHANGER_DECLINE, GOSSIP_SENDER_MAIN, 8);

    player->PlayerTalkClass->SendGossipMenu(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    return true;
}

bool GossipSelect_npc_phase_changer(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if(sender != GOSSIP_SENDER_MAIN) return true;
    if(!player->getAttackers().empty()) return true;

    switch (action)
    {
        case 1: player->SetPhaseMask(64, true);	break;
        case 2: player->SetPhaseMask(128, true);	break;
        case 3: player->SetPhaseMask(256, true);	break;
        case 4: player->SetPhaseMask(  2, true);	break;
        case 5: player->SetPhaseMask(  4, true);	break;
        case 6: player->SetPhaseMask(  6, true);	break;
        case 7: player->SetPhaseMask(  8, true);	break;
        case 8: player->SetPhaseMask(  1, true);	break;
    }

    player->CLOSE_GOSSIP_MENU();
    return true;
}

enum
{
    NPC_EVENT1_ACCEPT   = 2000006400,
    NPC_EVENT1_DECLINE  = 2000006401,
};

bool GossipHello_npc_event_1(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_EVENT1_ACCEPT, GOSSIP_SENDER_MAIN, 1);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_EVENT1_DECLINE, GOSSIP_SENDER_MAIN, 2);

    player->PlayerTalkClass->SendGossipMenu(889000, creature->GetGUID());
    return true;
}

bool GossipSelect_npc_event_1(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if(sender != GOSSIP_SENDER_MAIN) return true;
    if(!player->getAttackers().empty()) return true;

    if(action == 1)
    {
//megai2: yeees this have an chathandler class, but i don't know how scripts are linked with main process
       /* // id, or string, or [name] Shift-click form |color|Htele:id|h[name]|h|r
        GameTele const* tele = ExtractGameTeleFromLink("stalgornair");

        if (!tele)        
            return true;       

        //Ranger: crashfix?
        if (player && player->IsInWorld())
            if (player->GetMapId() == tele->mapId && player->GetMap() && player->GetMap()->IsBattleGroundOrArena())
                return true;            
       
        player->SaveRecallPosition();

        player->TeleportTo(tele->mapId, tele->position_x, tele->position_y, tele->position_z, tele->orientation);	*/
        player->TeleportTo(0, -4495.996f, -1646.5987f, 503.5f, 2.18f); 
    }
    else 
    {
	player->CastSpell(player, 49398, false);
    }

    player->CLOSE_GOSSIP_MENU();
    return true;
}

enum
{
    NPC_EVENT_ACCEPT    = 2000006300,
    NPC_EVENT_DECLINE   = 2000006301,
};

bool GossipHello_npc_event(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_EVENT_ACCEPT, GOSSIP_SENDER_MAIN, 1);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, NPC_EVENT_DECLINE, GOSSIP_SENDER_MAIN, 2);
    player->PlayerTalkClass->SendGossipMenu(888000, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_event(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if(sender != GOSSIP_SENDER_MAIN) return true;
    if(!player->getAttackers().empty()) return true;	

    if(action == 1)
        player->SetPhaseMask(128, true);	
    else 
        player->SetPhaseMask(1, true);

    player->CLOSE_GOSSIP_MENU();
    return true;
}

#define NPC_EVENT_2A 88001
#define NPC_EVENT_2B 88002
#define NPC_EVENT_2_HELLO 888001
#define NPC_EVENT_2_CONFIRM 888002
#define NPC_EVENT_2_COST 50000

enum
{
    NPC_EVENT2_ASK_FEST     = 2000006200,
    NPC_EVENT2_WANT_FEST    = 2000006201,
    NPC_EVENT2_READY_M      = 2000006202,
    NPC_EVENT2_READY_F      = 2000006203,
    NPC_EVENT2_CONFIRM      = 2000006204,
    NPC_EVENT2_WELCOME      = 2000006205,
};

bool GossipHello_npc_event_2(Player *player, Creature *creature)
{
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, NPC_EVENT2_ASK_FEST, GOSSIP_SENDER_MAIN, 1);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, NPC_EVENT2_WANT_FEST, GOSSIP_SENDER_MAIN, 2);

    player->PlayerTalkClass->SendGossipMenu(NPC_EVENT_2_HELLO, creature->GetGUID());
    return true;
}

bool GossipSelect_npc_event_2(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if(!player->getAttackers().empty()) return true;

    switch (sender)
    {
        case GOSSIP_SENDER_MAIN:
        {    
            if(action == 1)
            {
                uint32 eCreature = creature->GetEntry();
                if (eCreature == NPC_EVENT_2A)
                {
                    uint32 spellIds[6] = {45729, 11543, 11544, 6668, 11541, 11540};
                    player->CastSpell(player, spellIds[urand(0,5)], false);
                }
                else
                if (eCreature == NPC_EVENT_2B)
                    player->CastSpell(player, 45153, false);
                
                player->CLOSE_GOSSIP_MENU();
            }
            else
            {
                player->PlayerTalkClass->ClearMenus();

                player->ADD_GOSSIP_ITEM_EXTENDED_ID(GOSSIP_ICON_CHAT, player->getGender() == GENDER_MALE ? NPC_EVENT2_READY_M : NPC_EVENT2_READY_F, GOSSIP_SENDER_MAIN+1, 1, NPC_EVENT2_CONFIRM, NPC_EVENT_2_COST, false);
                player->PlayerTalkClass->SendGossipMenu(NPC_EVENT_2_CONFIRM, creature->GetGUID());
            }
            break;
        }
        case GOSSIP_SENDER_MAIN+1:
        {
            if (player->GetMoney() >= uint32(NPC_EVENT_2_COST))
            {
                player->SetDisplayId(22984);
                player->ModifyMoney(-NPC_EVENT_2_COST);

                creature->MonsterSay(NPC_EVENT2_WELCOME, LANG_UNIVERSAL, player);
            }
            else
                player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, creature, 0, 0);

            player->CLOSE_GOSSIP_MENU();
            break;
        }
    }
    return true;
}

struct MANGOS_DLL_DECL npc_event_2 : public ScriptedAI
{
    npc_event_2(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiResetFlagTimer;

    void Reset()
    {
        m_creature->HandleEmote(EMOTE_ONESHOT_DANCE);
        m_uiResetFlagTimer = 5000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // Reset flags after a certain time has passed so that the next player has to start the 'event' again
        if (m_uiResetFlagTimer <= 0)
        {
            uint32 value = m_creature->GetUInt32Value(UNIT_NPC_EMOTESTATE);
            if (value != EMOTE_ONESHOT_DANCE)
                Reset();
        }                
        else
            m_uiResetFlagTimer -= uiDiff;

        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_event_2(Creature* pCreature)
{
    return new npc_event_2(pCreature);
}

/************************/
typedef struct
{
    int32 iText;
    uint32 mapId;
    float x;
    float y;
    float z;
    uint32 team;
    uint32 levelMin;
    bool group;
} teleStruct;

#define NPC_TELE_HELLO 888003
#define TELE_COST_GOLD sWorld.getConfig(CONFIG_UINT32_CUSTOM_NPC_TELEPORT_GOLD_COST)
#define TELE_COST_COIN (TELE_COST_GOLD*100*100)
#define NUM_TELE_LOCS 17

teleStruct teleList[NUM_TELE_LOCS] = 
{
    {2000006000, 0, -8833.38f, 628.628f, 94.0066f, ALLIANCE, 1, false},       // Stormwind
    {2000006001, 0, -4918.88f, -940.406f, 501.564f, ALLIANCE, 1, false},      // Ironforge
    {2000006002, 1, 9949.56f, 2284.21f, 1341.4f, ALLIANCE, 1, false},         // Darnassus
    {2000006003, 530, -3965.7f, -11653.6f, -138.844f, ALLIANCE, 1, false},   // The Exodar
    
    {2000006004, 1, 1606.2608f, -4389.229f, 10.24f, HORDE, 1, false},            // Orgrimmar
    {2000006005, 1, -1277.37f, 124.804f, 131.287f, HORDE, 1, false},          // Thunder Bluff
    {2000006006, 0, 1584.07f, 241.987f, -52.1534f, HORDE, 1, false},         // Undercity
    {2000006007, 530, 9487.69f, -7279.2f, 14.2866f, HORDE, 1, false},         // Silvermoon City

    {2000006008, 571, 5804.15f, 624.771f, 647.767f, 0, 68, false},               // Dalaran
    {2000006009, 530, -1872.52f, 5399.83f, -12.43f, 0, 58, false},               // Shattrath City
    {2000006010, 0, -14297.2f, 530.993f, 8.77916f, 0, 30, false},             // Booty Bay
    {2000006011, 1, -7177.15f, -3785.34f, 8.36981f, 0, 40, false},            // Gadgetzan
    {2000006012, 1, -956.664f, -3754.71f, 5.33239f, 0, 30, false},           // Ratchet
    {2000006013, 571, 2213.95f, 5273.15f, 11.2565f, ALLIANCE, 68, false},     // Valiance Keep
    {2000006014, 571, 2840.47f, 6198.06f, 85.7995f, HORDE, 68, false},           // Warsong Hold
    {2000006015, 571, 1942.86f, -6167.11f, 23.724f, HORDE, 68, false},        // Vengeance Landing
    {2000006016, 571, 617.57f, -4997.03f, 3.6806f, ALLIANCE, 68, false},         // Valgarde
};

#define TELE_AGREE_ID 2000006017

bool GossipHello_npc_teleport(Player *player, Creature *_Creature)
{
    for (uint8 i = 0; i < NUM_TELE_LOCS; i++)
        if (teleList[i].team == 0 || teleList[i].team == player->GetTeam() || player->isGameMaster())
            player->ADD_GOSSIP_ITEM_EXTENDED_ID(GOSSIP_ICON_TAXI, teleList[i].iText, GOSSIP_SENDER_MAIN, i+1, TELE_AGREE_ID, TELE_COST_COIN, false);

    player->SEND_GOSSIP_MENU(NPC_TELE_HELLO, _Creature->GetGUID());
    return true;
}

void SendDefaultMenu_npc_teleport(Player *player, Creature *_Creature, uint32 action )
{
    if(!player->getAttackers().empty())
    {
        player->CLOSE_GOSSIP_MENU();
        //_Creature->MonsterSay("You are in combat!", LANG_UNIVERSAL, NULL);
        return;
    }

    uint32 money = player->GetMoney();

    if (money < TELE_COST_COIN && !player->isGameMaster()) 
    {
        player->CLOSE_GOSSIP_MENU();
        player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, _Creature, 0, 0);
        return;
    }

    if (action < 1 || action > NUM_TELE_LOCS)
        return;

    uint8 i = action - 1;
    player->CLOSE_GOSSIP_MENU();
    if (player->getLevel() >= teleList[i].levelMin)
    {
        if (!player->isGameMaster())
            player->ModifyMoney(-1*TELE_COST_COIN);
        player->TeleportTo(teleList[i].mapId, teleList[i].x, teleList[i].y, teleList[i].z, player->GetOrientation());
    }
    else
    {
        WorldPacket data(SMSG_INVENTORY_CHANGE_FAILURE, 1+8+8+1);
        data << uint8(EQUIP_ERR_PURCHASE_LEVEL_TOO_LOW);
        data << (ObjectGuid());
        data << (ObjectGuid());
        data << uint8(0);
        data << uint32(teleList[i].levelMin);
        player->GetSession()->SendPacket(&data);
    }
}

bool GossipSelect_npc_teleport(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
    {
        player->PlayerTalkClass->ClearMenus();
        SendDefaultMenu_npc_teleport(player, _Creature, action);
    }
    return true;
}

/************************/
#define OUT_OF_COMBAT_TIME 5000

struct MANGOS_DLL_DECL npc_training_dummy : public Scripted_NoMovementAI
{
    npc_training_dummy(Creature* pCreature) : Scripted_NoMovementAI(pCreature) {Reset();}

    uint32 combat_timer;

    void Reset()
    {
        combat_timer = 0;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        combat_timer = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_creature->SetHealth(m_creature->GetMaxHealth());

        combat_timer += uiDiff;
        if (combat_timer > OUT_OF_COMBAT_TIME)
            EnterEvadeMode();
    }
};

CreatureAI* GetAI_npc_training_dummy(Creature* pCreature)
{
    return new npc_training_dummy(pCreature);
}
/************************/
enum PilgrimZones
{
    ZONE_DUN_MOROUGH   = 1,
    ZONE_ELWYNN_FOREST = 12,
    ZONE_DARNASSUS     = 1657,
    ZONE_EXODAR        = 3557,

    ZONE_DUROTAR       = 14,
    ZONE_UNDERCITY     = 1497,
    ZONE_THUNDER_BLUFF = 1638,
    ZONE_EVERSONG      = 3430
};

Player* SelectRandomFriendlyPlayer(Unit* source, Player* except /*= NULL*/, float radius /*= ATTACK_DISTANCE*/)
{
    std::list<Player*> targets;

    MaNGOS::AnyPlayerInObjectRangeCheck u_check(source, radius);
    MaNGOS::PlayerListSearcher<MaNGOS::AnyPlayerInObjectRangeCheck> searcher(targets, u_check);

    Cell::VisitAllObjects(source, searcher, radius);
    // remove current target
    if(except)
        targets.remove(except);

    // remove not LoS targets
    for(std::list<Player*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if(!source->IsWithinLOSInMap(*tIter) || !(*tIter)->IsFriendlyTo(source))
        {
            std::list<Player*>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }

    // no appropriate targets
    if(targets.empty())
        return NULL;

    // select random
    uint32 rIdx = urand(0,targets.size()-1);
    std::list<Player*>::iterator tcIter = targets.begin();
    for(uint32 i = 0; i < rIdx; ++i)
        ++tcIter;

    return *tcIter;
}

bool IsPlayerInTeamEventZone(Player* pl)
{
    switch (pl->GetZoneId())
    {
        case ZONE_DUN_MOROUGH:
        case ZONE_ELWYNN_FOREST:
        case ZONE_DARNASSUS:
        case ZONE_EXODAR:
            return pl->GetTeam() == ALLIANCE;
        case ZONE_DUROTAR:
        case ZONE_UNDERCITY:
        case ZONE_THUNDER_BLUFF:
        case ZONE_EVERSONG:
            return pl->GetTeam() == HORDE;
        default:
            return false;
    }
}

enum 
{
    PILGRIM_JOIN_PAUNCH = 2000006100,
    PILGRIM_THROW_FOOD  = 2000006101,
    PILGRIM_SHARE_FOOD  = 2000006102,
    PILGRIM_SIT_TABLE   = 2000006103,
};

bool GossipHello_npc_pilgrims_table(Player *player, Creature *_Creature)
{
    if (IsPlayerInTeamEventZone(player))
    {
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, PILGRIM_JOIN_PAUNCH, GOSSIP_SENDER_MAIN, 1);
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, PILGRIM_THROW_FOOD, GOSSIP_SENDER_MAIN, 2);
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, PILGRIM_SHARE_FOOD, GOSSIP_SENDER_MAIN, 3);
    }
    else
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, PILGRIM_SIT_TABLE, GOSSIP_SENDER_MAIN, 4);
    

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _Creature->GetGUID());
    return true;
}

void SendDefaultMenu_npc_pilgrims_table(Player *player, Creature *_Creature, uint32 action )
{
    if(!player->getAttackers().empty())
    {
        player->CLOSE_GOSSIP_MENU();
        return;
    }

    player->CLOSE_GOSSIP_MENU();

    switch (action)
    {
        // Spirit of Sharing
        case 1:
        {
            if (!IsPlayerInTeamEventZone(player))
                break;
            player->CastSpell(player, 61849, true);
            break;
        }
        // Food Fight
        case 2:
        {
            if (!IsPlayerInTeamEventZone(player))
                break;
           
            if (Player* target = SelectRandomFriendlyPlayer(player, player, 10))
            {
                uint32 spellIds[5] = {61925, 61926, 61927, 61928, 61929};
                player->CastSpell(target, spellIds[urand(0,4)], true);
            }
            break;
        }
        // Pass Food
        case 3:
        {
            if (!IsPlayerInTeamEventZone(player))
                break;
            
            if (Player* target = SelectRandomFriendlyPlayer(player, player, 10))
            {
                uint32 spellIds[5] = {66372, 66373, 66374, 66375, 66376};
                player->CastSpell(target, spellIds[urand(0,4)], true);
            }
            break;
        }
        // Pilgrim's Peril
        case 4:
        {
            if (IsPlayerInTeamEventZone(player))
                break;

            if (player->HasItemOrGemWithIdEquipped(46800, 1) || // Pilgrim's Attire
                player->HasItemOrGemWithIdEquipped(44785, 1) || // Pilgrim's Dress 
                player->HasItemOrGemWithIdEquipped(46824, 1))   // Pilgrim's Robe
                player->CastSpell(player, 65403, true);
            break;
        }
    }
}

bool GossipSelect_npc_pilgrims_table(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
    {
        player->PlayerTalkClass->ClearMenus();
        SendDefaultMenu_npc_pilgrims_table(player, _Creature, action);
    }
    return true;
}

/************************/
enum
{
    SPELL_CREATE_MISTLETOE      = 26206,
    SPELL_CREATE_FRESH_HOLLY    = 26207,
    SPELL_CREATE_SNOWFlAKES     = 45036,
    SPELL_MISTLETOE_DEBUFF      = 26218,
};

struct MANGOS_DLL_DECL npc_winter_revelerAI : public ScriptedAI
{
    npc_winter_revelerAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset() {}

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        if (uiEmote == TEXTEMOTE_KISS)
        {
            m_creature->SetFacingToObject(pPlayer);
            if (!pPlayer->HasAura(SPELL_MISTLETOE_DEBUFF))
            {
                uint32 spellId = 0;
                switch (urand(0,2))
                {
                    case 0: spellId = SPELL_CREATE_MISTLETOE; break;
                    case 1: spellId = SPELL_CREATE_FRESH_HOLLY; break;
                    case 2: spellId = SPELL_CREATE_SNOWFlAKES; break;
                }

                pPlayer->CastSpell(pPlayer, spellId, true);
                m_creature->CastSpell(pPlayer, SPELL_MISTLETOE_DEBUFF, false);
                pPlayer->CastSpell(pPlayer, SPELL_MISTLETOE_DEBUFF, true);
            }
            else
                m_creature->HandleEmote(TEXTEMOTE_LAUGH);
        }
    }
};

CreatureAI* GetAI_npc_winter_reveler(Creature* pCreature)
{
    return new npc_winter_revelerAI(pCreature);
}
/************************/

enum
{
    EXP_COST_GOLD       = 10,
    EXP_COST            = EXP_COST_GOLD * 10000,
    
    EXP_GOSSIP_TEXT     = 890000,

    EXP_XP_OFF          = 2000006600,
    EXP_XP_ON           = 2000006601,
    EXP_XP_CONFIRM      = 2000006602,

    EXP_EMOTE_S1        = 2000006603,

    EXP_EMOTE_B1        = 2000006607,
};

bool GossipHello_npc_experience(Player *player, Creature *_Creature)
{
    if (player->HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_XP_USER_DISABLED))
        player->ADD_GOSSIP_ITEM_EXTENDED_ID(GOSSIP_ICON_CHAT, EXP_XP_ON, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1, EXP_XP_CONFIRM, EXP_COST, false);
    else
        player->ADD_GOSSIP_ITEM_EXTENDED_ID(GOSSIP_ICON_CHAT, EXP_XP_OFF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+2, EXP_XP_CONFIRM, EXP_COST, false);

    player->SEND_GOSSIP_MENU(EXP_GOSSIP_TEXT, _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_experience(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
    {
        player->CLOSE_GOSSIP_MENU();
        uint32 money = player->GetMoney();
        if (money < EXP_COST) 
        {
            player->SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, _Creature, 0, 0);
            return false;
        }

        player->ModifyMoney(-1 * EXP_COST);

        if (action == GOSSIP_ACTION_INFO_DEF+1)
            player->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_XP_USER_DISABLED);
        else if (action == GOSSIP_ACTION_INFO_DEF+2)
            player->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_XP_USER_DISABLED);
    }
    return true;
}

struct MANGOS_DLL_DECL npc_experienceAI : public ScriptedAI
{
    npc_experienceAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 emote_timer;

    void Reset()
    {
        emote_timer = urand(30 * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (emote_timer < uiDiff)
        {
            m_creature->MonsterTextEmote((m_creature->GetEntry() == 35365 ? EXP_EMOTE_B1: EXP_EMOTE_S1) + urand(0,3), NULL);
            emote_timer = urand(30 * IN_MILLISECONDS, 10 * MINUTE * IN_MILLISECONDS);
        }
        else
            emote_timer -= uiDiff;

        if (m_creature->SelectHostileTarget() && m_creature->getVictim())
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_experience(Creature* pCreature)
{
    return new npc_experienceAI(pCreature);
}

/*****************************************************/

enum
{
    NPC_PORTAL_HALL_TO_HEART    = 29588,
    SPELL_HALL_TO_HEART         = 54744,

    NPC_PORTAL_HEART_TO_HALL    = 29589,
    SPELL_HEART_TO_HALL         = 54746,

    PORTAL_TIMER                = 1000,
};

struct MANGOS_DLL_DECL npc_acherus_portalAI : public ScriptedAI
{
    npc_acherus_portalAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 spellId;
    uint32 timer;

    void Reset()
    {
        spellId = 0;
        if (m_creature->GetEntry() == NPC_PORTAL_HALL_TO_HEART)
            spellId = SPELL_HALL_TO_HEART;
        else if (m_creature->GetEntry() == NPC_PORTAL_HEART_TO_HALL)
            spellId = SPELL_HEART_TO_HALL;

        timer = PORTAL_TIMER;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (timer < uiDiff)
        {
            if (Player* target = SelectRandomFriendlyPlayer(m_creature, NULL, 3))
                m_creature->CastSpell(target, spellId, true);
            timer = PORTAL_TIMER;
        }
        else
            timer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_acherus_portal(Creature* pCreature)
{
    return new npc_acherus_portalAI(pCreature);
}

/*****************************************************/

enum
{
    NPC_SPRING_RABBIT           = 32791,

    //SPELL_RABBIT_COSTUME        = 61716,
    //SPELL_NOBLEGARDEN_LAY_AURA  = 61719,
    //SPELL_SPRING_RABBIT_JUMP    = 61724,
    SPELL_SUMMON_BABY_BUNNY     = 61727,
    SPELL_SPRING_RABBIT_IN_LOVE = 61728,
    SPELL_NOBLEGARDEN_BUNNY     = 61734,
    SPELL_SPRING_FLING          = 61875,

    RABBIT_JUMP_DISTANCE        = 15,

    //RABBIT_JUMP_DELAY           = 2 * IN_MILLISECONDS,
    //RABBIT_LOVE_TIME            = 5 * IN_MILLISECONDS,
    RABBIT_RESET_TIMER          = 60 * IN_MILLISECONDS,
    RABBIT_SEARCH_TIMER         = 3 * IN_MILLISECONDS,
};

Unit* FindRabbit(Unit* source, Unit* except /*= NULL*/, float radius /*= ATTACK_DISTANCE*/)
{
    std::list<Unit*> targets;

    MaNGOS::AnyUnitInObjectRangeCheck u_check(source, radius);
    MaNGOS::UnitListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> searcher(targets, u_check);

    Cell::VisitAllObjects(source, searcher, radius);
    // remove current target
    if(except)
        targets.remove(except);

    // remove bad targets
    for(std::list<Unit*>::iterator tIter = targets.begin(); tIter != targets.end();)
    {
        if (!source->IsWithinLOSInMap(*tIter) || (*tIter)->GetTypeId() != TYPEID_UNIT || (*tIter)->GetEntry() != NPC_SPRING_RABBIT)
        {
            std::list<Unit *>::iterator tIter2 = tIter;
            ++tIter;
            targets.erase(tIter2);
        }
        else
            ++tIter;
    }

    // no appropriate targets
    if (targets.empty())
        return NULL;

    // select random
    Unit* pUnit = NULL;
    float mindist = 9999999.0f;
    for(std::list<Unit*>::iterator tIter = targets.begin(); tIter != targets.end(); ++tIter)
    {
        float dist = source->GetDistance(*tIter);
        if (dist < mindist)
        {
            mindist = dist;
            pUnit = *tIter;
        }
    }

    return pUnit;
}

struct MANGOS_DLL_DECL npc_spring_rabbitAI : public ScriptedAI
{
    npc_spring_rabbitAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    Unit* owner;
    uint32 m_SearchTimer;
    uint32 m_ResetTimer;

    void Reset()
    {
        m_SearchTimer = RABBIT_SEARCH_TIMER;
        m_ResetTimer = 0;

        owner = m_creature->GetOwner();
        if (!owner)
            return;

        m_creature->GetMotionMaster()->Clear(false);
        m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, m_creature->GetAngle(owner));
    }

    bool IsAchievementZone()
    {
        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return false;

        switch(m_creature->GetAreaId())
        {
            case 87:        // Goldshire
            case 131:       // Kharanos
            case 3576:      // Azure Watch
            case 4395:      // Dolanaar
                return ((Player*)owner)->GetTeam() == ALLIANCE;
            case 159:       // Brill
            case 222:       // Bloodhoof Village
            case 362:       // Razor Hill
            case 3665:      // Falconwing Square
                return ((Player*)owner)->GetTeam() == HORDE;
        }

        return false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->IsInWorld())
        {
            m_SearchTimer = RABBIT_SEARCH_TIMER;
            m_ResetTimer = 0;
            return;
        }

        if (m_SearchTimer)
        {
            if (m_SearchTimer <= uiDiff)
            {
                m_SearchTimer = 0;
                if (!IsHolidayActive(HOLIDAY_NOBLEGARDEN) || !owner || !owner->IsInWorld())
                {
                    m_SearchTimer = RABBIT_SEARCH_TIMER;
                    return;
                }

                if (Unit* pUnit = FindRabbit((Unit*)m_creature, (Unit*)m_creature, RABBIT_JUMP_DISTANCE))
                {
                    m_creature->SetOrientation(m_creature->GetAngle(pUnit));
                    pUnit->SetOrientation(pUnit->GetAngle(m_creature));
                    m_creature->CastSpell(pUnit, SPELL_SPRING_RABBIT_IN_LOVE, true);
                    m_creature->CastSpell(pUnit, SPELL_SUMMON_BABY_BUNNY, true);
                    owner->CastSpell(owner, SPELL_SPRING_FLING, true);

                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, m_creature->GetAngle(owner));

                    m_ResetTimer = RABBIT_RESET_TIMER;
                }
                else
                    m_SearchTimer = RABBIT_SEARCH_TIMER;
            }
            else
                m_SearchTimer -= uiDiff;
        }

        if (m_ResetTimer)
        {
            if (m_ResetTimer <= uiDiff)
            {
                m_ResetTimer = 0;
                m_SearchTimer = RABBIT_SEARCH_TIMER;
                return;
            }
            else
                m_ResetTimer -= uiDiff;
        }
    }
};

bool GOUse_go_spring_egg(Player* pPlayer, GameObject* pGo)
{
    if (pGo->getLootState() == GO_READY && urand(0, 100) < 25)
        pPlayer->CastSpell(pPlayer, SPELL_NOBLEGARDEN_BUNNY, true);

    return false;
}

CreatureAI* GetAI_npc_spring_rabbit(Creature* pCreature)
{
    return new npc_spring_rabbitAI(pCreature);
}

/*************************************************/

enum
{
    CHILD_UPDATE_TIMER      = 1000,

    CHILD_ENTRY_HUMAN       = 14305,
    CHILD_ENTRY_ORC         = 14444,

    CHILD_ENTRY_BLOOD_ELF   = 22817,
    CHILD_ENTRY_DRAENEI     = 22818,

    CHILD_ENTRY_WOLVAR      = 33532,
    CHILD_ENTRY_ORACLE      = 33533,

    NPC_SAMURO              = 25151,
    NPC_ALEXSTRASZA         = 26917,
    NPC_HEMET               = 27986,
    NPC_ETYMIDIAN           = 28092,

    GAMEOBJECT_AUCHINDOUN_STONE = 184458,

    SPELL_ZEPPELIN          = 65357,
};

#define NPC_SEARCH_RANGE 20.0f

struct MANGOS_DLL_DECL npc_child_eventAI : public ScriptedAI
{
    npc_child_eventAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    Unit* owner;
    uint32 m_UpdateTimer;

    void Reset()
    {
        m_UpdateTimer = CHILD_UPDATE_TIMER;
        owner = m_creature->GetOwner();

        if (!owner)
            return;

        m_creature->GetMotionMaster()->Clear(false);
        m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST, m_creature->GetAngle(owner));
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_UpdateTimer < uiDiff)
        {
            m_UpdateTimer = CHILD_UPDATE_TIMER;

            if (!m_creature->IsInWorld() || !IsHolidayActive(HOLIDAY_CHILDRENS_WEEK) || !owner || !owner->IsInWorld())
                return;

            uint32 zoneid, areaid;
            owner->GetZoneAndAreaId(zoneid, areaid);

            uint32 questId = 0;
            uint32 entry = m_creature->GetEntry();
            switch(areaid)
            {
                case 395:       // Grizzlemaw
                    if (entry == CHILD_ENTRY_ORACLE)
                        questId = 13929;
                    else if (entry == CHILD_ENTRY_WOLVAR)
                        questId = 13930;
                    break;
                case 4099:      // Winterfin Retreat
                    if (entry == CHILD_ENTRY_ORACLE)
                        questId = 13950;
                    break;
                case 4154:      // Snowfall Glade
                    if (entry == CHILD_ENTRY_WOLVAR)
                        questId = 13951;
                    break;
                case 4175:      // Bronze Dragonshrine
                    if (entry == CHILD_ENTRY_ORACLE)
                        questId = 13933;
                    else if (entry == CHILD_ENTRY_WOLVAR)
                        questId = 13934;
                    break;
            }

            if (entry == CHILD_ENTRY_ORACLE || entry == CHILD_ENTRY_WOLVAR)
            {
                if (GetClosestCreatureWithEntry(m_creature, NPC_ALEXSTRASZA, NPC_SEARCH_RANGE))
                    questId = entry == CHILD_ENTRY_ORACLE ? 13954 : 13955;

                if (GetClosestCreatureWithEntry(m_creature, NPC_ETYMIDIAN, NPC_SEARCH_RANGE))
                    questId = entry == CHILD_ENTRY_ORACLE ? 13956 : 0;

                if (GetClosestCreatureWithEntry(m_creature, NPC_HEMET, NPC_SEARCH_RANGE))
                    questId = entry == CHILD_ENTRY_WOLVAR ? 13957 : 0;
            }
            else if (entry == CHILD_ENTRY_BLOOD_ELF)
            {
                if (GetClosestCreatureWithEntry(m_creature, NPC_SAMURO, NPC_SEARCH_RANGE))
                    questId = 11975;
            }
            else if (entry == CHILD_ENTRY_DRAENEI)
            {
                if (GetClosestGameObjectWithEntry(m_creature, GAMEOBJECT_AUCHINDOUN_STONE, NPC_SEARCH_RANGE))
                    questId = 10950;
            }

            if (questId)
                ((Player*)owner)->AreaExploredOrEventHappens(questId);
        }
        else
            m_UpdateTimer -= uiDiff;
    }

    void SpellHit(Unit* pWho, const SpellEntry* spellInfo)
    {
        if (!owner || owner != pWho || !spellInfo)
            return;

        if (m_creature->GetEntry() == CHILD_ENTRY_ORACLE || m_creature->GetEntry() == CHILD_ENTRY_WOLVAR)
        {
            if (spellInfo->Id == SPELL_ZEPPELIN)
                ((Player*)owner)->KilledMonsterCredit(36209);
        }
    }
};

CreatureAI* GetAI_npc_child_event(Creature* pCreature)
{
    return new npc_child_eventAI(pCreature);
}
/*********************************************************/

enum
{
    NPC_PREMIUM_HELLO           = 888004,
    NPC_PREMIUM_HELLO_INACTIVE  = 888005,
    NPC_PREMIUM_HELLO_BYE       = 888006,
    NPC_PREMIUM_TRAVEL          = 888007,
    NPC_PREMIUM_TRAVEL_TIRED    = 888008,

    NPC_PREMIUM_MENU_BANK_M     = 2000006700,
    NPC_PREMIUM_MENU_AUCTION_M  = 2000006701,
    NPC_PREMIUM_MENU_STABLE_M   = 2000006702,
    NPC_PREMIUM_MENU_TRAVEL_M   = 2000006703,
    NPC_PREMIUM_MENU_MAIL_M     = 2000006704,

    NPC_PREMIUM_MENU_BANK_F     = 2000006705,
    NPC_PREMIUM_MENU_AUCTION_F  = 2000006706,
    NPC_PREMIUM_MENU_STABLE_F   = 2000006707,
    NPC_PREMIUM_MENU_TRAVEL_F   = 2000006708,
    NPC_PREMIUM_MENU_MAIL_F     = 2000006709,

    NPC_PREMIUM_ACTION_BANK     = 1,
    NPC_PREMIUM_ACTION_AUCTION  = 2,
    NPC_PREMIUM_ACTION_MAIL     = 3,
    NPC_PREMIUM_ACTION_STABLE   = 4,
    NPC_PREMIUM_ACTION_TRAVEL   = 5,

    NPC_PREMIUM_SPELL_COOLDOWN_CHECKER = 5581,
};

bool GossipHello_npc_premium_pet(Player *player, Creature *_Creature)
{
    if (player->GetObjectGuid() != _Creature->GetOwnerGuid())
    {
        player->SEND_GOSSIP_MENU(NPC_PREMIUM_HELLO_BYE, _Creature->GetGUID());
        return true;
    }

    if (!player->IsPremiumActive())
    {
        player->SEND_GOSSIP_MENU(NPC_PREMIUM_HELLO_INACTIVE, _Creature->GetGUID());
        return true;
    }

    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_MONEY_BAG, player->getGender() == GENDER_MALE ? NPC_PREMIUM_MENU_BANK_M : NPC_PREMIUM_MENU_BANK_F, GOSSIP_SENDER_MAIN, NPC_PREMIUM_ACTION_BANK);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_VENDOR, player->getGender() == GENDER_MALE ? NPC_PREMIUM_MENU_AUCTION_M : NPC_PREMIUM_MENU_AUCTION_F, GOSSIP_SENDER_MAIN, NPC_PREMIUM_ACTION_AUCTION);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, player->getGender() == GENDER_MALE ? NPC_PREMIUM_MENU_MAIL_M : NPC_PREMIUM_MENU_MAIL_F, GOSSIP_SENDER_MAIN, NPC_PREMIUM_ACTION_MAIL);
    if (player->getClass() == CLASS_HUNTER)
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, player->getGender() == GENDER_MALE ? NPC_PREMIUM_MENU_STABLE_M : NPC_PREMIUM_MENU_STABLE_F, GOSSIP_SENDER_MAIN, NPC_PREMIUM_ACTION_STABLE);
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, player->getGender() == GENDER_MALE ? NPC_PREMIUM_MENU_TRAVEL_M : NPC_PREMIUM_MENU_TRAVEL_F, GOSSIP_SENDER_MAIN, NPC_PREMIUM_ACTION_TRAVEL);
    player->SEND_GOSSIP_MENU(NPC_PREMIUM_HELLO, _Creature->GetGUID());
    return true;
}

bool GossipSelect_npc_premium_pet(Player* player, Creature* _Creature, uint32 sender, uint32 action)
{
    player->PlayerTalkClass->ClearMenus();

    if(!player->getAttackers().empty())
    {
        player->CLOSE_GOSSIP_MENU();
        return true;
    }

    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
    {
        player->CLOSE_GOSSIP_MENU();
        switch (action)
        {
            case NPC_PREMIUM_ACTION_BANK:
            {
                player->GetSession()->SendShowBank(_Creature->GetObjectGuid());
                break;
            }
            case NPC_PREMIUM_ACTION_AUCTION:
            {
                player->SetAuctionAccessMode(0);
                player->GetSession()->SendAuctionHello(_Creature);
                break;
            }
            case NPC_PREMIUM_ACTION_MAIL:
            {
                player->GetSession()->SendShowMailBox(_Creature->GetObjectGuid());
                break;
            }
            case NPC_PREMIUM_ACTION_STABLE:
            {
                if (player->getClass() != CLASS_HUNTER)
                    return true;
                player->GetSession()->SendStablePet(_Creature->GetObjectGuid());
                break;
            }
            case NPC_PREMIUM_ACTION_TRAVEL:
            {
                if (!player->HasSpellCooldown(NPC_PREMIUM_SPELL_COOLDOWN_CHECKER))
                {
                    for (int i = 0; i < NUM_TELE_LOCS; ++i)
                        if (teleList[i].team == 0 || teleList[i].team == player->GetTeam() || player->isGameMaster())
                            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, teleList[i].iText, GOSSIP_SENDER_MAIN+1, i+1);

                    player->SEND_GOSSIP_MENU(NPC_PREMIUM_TRAVEL, _Creature->GetGUID());
                }
                else
                    player->SEND_GOSSIP_MENU(NPC_PREMIUM_TRAVEL_TIRED, _Creature->GetGUID());                    
                break;
            }
        }
    }
    // Teleportation
    else if (sender == GOSSIP_SENDER_MAIN + 1)
    {
        if (action < 1 || action > NUM_TELE_LOCS)
            return true;

        uint8 i = action - 1;
        player->CLOSE_GOSSIP_MENU();
        if (player->getLevel() >= teleList[i].levelMin)
        {
            player->AddSpellCooldown(NPC_PREMIUM_SPELL_COOLDOWN_CHECKER, 0, time(NULL) + sWorld.getConfig(CONFIG_UINT32_PREMIUM_TELEPORT_COOLDOWN_MINUTES) * MINUTE);
            player->TeleportTo(teleList[i].mapId, teleList[i].x, teleList[i].y, teleList[i].z, player->GetOrientation());
        }
        else
        {
            WorldPacket data(SMSG_INVENTORY_CHANGE_FAILURE, 1+8+8+1);
            data << uint8(EQUIP_ERR_PURCHASE_LEVEL_TOO_LOW);
            data << (ObjectGuid());
            data << (ObjectGuid());
            data << uint8(0);
            data << uint32(teleList[i].levelMin);
            player->GetSession()->SendPacket(&data);
        }
    }

    return true;
}

/******************/
enum
{
    NPC_ICE_CALLER_UPDATE_TIMER             = 10000,

    SPELL_CRAB_DISGUISE                     = 46337,
    SPELL_INNOCENT_DISGUISE_QUEST_COMPLETE  = 46375,
};

struct MANGOS_DLL_DECL npc_ice_callerAI : public ScriptedAI
{
    npc_ice_callerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_UpdateTimer;

    void Reset()
    {
        m_UpdateTimer = NPC_ICE_CALLER_UPDATE_TIMER;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_UpdateTimer < uiDiff)
        {
            m_UpdateTimer = NPC_ICE_CALLER_UPDATE_TIMER;
            if (Player* pl = SelectRandomFriendlyPlayer(m_creature, NULL, 10.0f))
                if (pl->HasAura(SPELL_CRAB_DISGUISE))
                    m_creature->CastSpell(pl, SPELL_INNOCENT_DISGUISE_QUEST_COMPLETE, true);
        }
        else
            m_UpdateTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_ice_caller(Creature* pCreature)
{
    return new npc_ice_callerAI(pCreature);
}

/***************************/

bool GossipHello_npc_de_meza(Player *player, Creature *_Creature)
{
    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, 2000006611, GOSSIP_SENDER_MAIN, 1);
    player->SEND_GOSSIP_MENU(890001, _Creature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_de_meza(Player *player, Creature *_Creature, uint32 sender, uint32 action )
{
    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
    {
        player->CLOSE_GOSSIP_MENU();

        _Creature->CastSpell(player, 50517, false);
    }
    return true;
}

/***************************/

enum
{
    RELAYRACE_UPDATE_TIMER      = 1000,

    SPELL_RELAYRACE_KILL_CREDIT = 44601,
    SPELL_RELAYRACE_THROW_KEG   = 43662,
    SPELL_RELAYRACE_CREATE_KEG  = 43660,
    ITEM_RELAYRACE_KEG          = 33797,
    RANGE_RELAYRACE             = 25,

    QUEST_RELAYRACE_ALLIANCE    = 11122,
    QUEST_RELAYRACE_HORDE       = 11412,
};

struct MANGOS_DLL_DECL npc_relayrace_dealerAI : public ScriptedAI
{
    npc_relayrace_dealerAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_UpdateTimer;

    void Reset()
    {
        m_UpdateTimer = RELAYRACE_UPDATE_TIMER;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_UpdateTimer < uiDiff)
        {
            m_UpdateTimer = RELAYRACE_UPDATE_TIMER;

            std::list<Player*> targets;
            MaNGOS::AnyPlayerInObjectRangeCheck u_check(m_creature, RANGE_RELAYRACE);
            MaNGOS::PlayerListSearcher<MaNGOS::AnyPlayerInObjectRangeCheck> searcher(targets, u_check);
            Cell::VisitAllObjects(m_creature, searcher, RANGE_RELAYRACE);

            for (std::list<Player*>::iterator iter = targets.begin(); iter != targets.end(); ++iter)
            {
                Player* pl = *iter;
                if (!pl->IsInWorld() || !pl->HasAura(43880) && !pl->HasAura(43883))
                    continue;

                uint32 questId = pl->GetTeam() == ALLIANCE ? QUEST_RELAYRACE_ALLIANCE : QUEST_RELAYRACE_HORDE;
                if (pl->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
                {
                    if (!pl->HasItemCount(ITEM_RELAYRACE_KEG, 1, true))
                        m_creature->CastSpell(pl, SPELL_RELAYRACE_CREATE_KEG, false);
                }
            }
        }
        else
            m_UpdateTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_relayrace_dealer(Creature* pCreature)
{
    return new npc_relayrace_dealerAI(pCreature);
}

struct MANGOS_DLL_DECL npc_relayrace_masterAI : public ScriptedAI
{
    npc_relayrace_masterAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_UpdateTimer;

    void Reset()
    {
        m_UpdateTimer = RELAYRACE_UPDATE_TIMER;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_UpdateTimer < uiDiff)
        {
            m_UpdateTimer = RELAYRACE_UPDATE_TIMER;

            std::list<Player*> targets;
            MaNGOS::AnyPlayerInObjectRangeCheck u_check(m_creature, RANGE_RELAYRACE);
            MaNGOS::PlayerListSearcher<MaNGOS::AnyPlayerInObjectRangeCheck> searcher(targets, u_check);
            Cell::VisitAllObjects(m_creature, searcher, RANGE_RELAYRACE);
            for (std::list<Player*>::iterator iter = targets.begin(); iter != targets.end(); ++iter)
            {
                Player* pl = *iter;
                if (!pl->IsInWorld() || !pl->HasAura(43880) && !pl->HasAura(43883))
                    continue;

                if (pl->HasItemCount(ITEM_RELAYRACE_KEG, 1, false))
                {
                    pl->CastSpell(m_creature, SPELL_RELAYRACE_THROW_KEG, true);
                    pl->DestroyItemCount(ITEM_RELAYRACE_KEG, 1, true);
                    pl->CastSpell(pl, SPELL_RELAYRACE_KILL_CREDIT, true);
                }
            }
        }
        else
            m_UpdateTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_relayrace_master(Creature* pCreature)
{
    return new npc_relayrace_masterAI(pCreature);
}

/**********************************/

enum
{
    SPELL_BREWFEST_BREW_TOSS        = 48249,
    SPELL_THE_BREWMAIDENS_BLESSING  = 68269,
    SPELL_DARK_BREWMAIDENS_STUN     = 47340,
    ITEM_BREWFEST_BREW              = 33929,
};

struct MANGOS_DLL_DECL npc_brewfest_maidenAI : public ScriptedAI
{
    npc_brewfest_maidenAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    Unit *owner;
    bool inCombat;
    bool passive;

    void Reset()
    {
        owner = m_creature->GetOwner();
        if (!owner) 
            return;

        inCombat = false;
        passive = m_creature->GetEntry() == 36021;
        if (passive)
            m_creature->CastSpell(m_creature, SPELL_THE_BREWMAIDENS_BLESSING, true);

        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST + 3.0f, m_creature->GetAngle(owner));
        }
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho || passive)
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->clearUnitState(UNIT_STAT_FOLLOW);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            m_creature->AddThreat(pWho, 100.0f);
            DoStartMovement(pWho, 2.0f);
            SetCombatMovement(true);
            inCombat = true;
        }
    }

    void EnterEvadeMode()
    {
        if (m_creature->IsInEvadeMode() || !m_creature->isAlive())
            return;

        inCombat = false;

        m_creature->AttackStop();
        m_creature->CombatStop(true);
        if (owner && !m_creature->hasUnitState(UNIT_STAT_FOLLOW))
        {
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveFollow(owner, PET_FOLLOW_DIST + 3.0f, m_creature->GetAngle(owner));
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!owner || !owner->IsInWorld())
        {
            m_creature->ForcedDespawn();
            return;
        }

        if (!m_creature->getVictim())
            if (owner && owner->getVictim())
                AttackStart(owner->getVictim());

        if (m_creature->getVictim() && m_creature->getVictim() != owner->getVictim())
            AttackStart(owner->getVictim());

        if (inCombat && !m_creature->getVictim())
        {
            EnterEvadeMode();
            return;
        }
    }

    void ReceiveEmote(Player* pPlayer, uint32 uiEmote)
    {
        if (m_creature->isInCombat() || pPlayer->isInCombat())
            return;

        m_creature->SetFacingToObject(pPlayer);

        if (uiEmote == TEXTEMOTE_WAVE)
        {
            if (pPlayer->HasItemCount(ITEM_BREWFEST_BREW, 1, true))
                DoCastSpellIfCan(pPlayer, SPELL_DARK_BREWMAIDENS_STUN);
            else
                DoCastSpellIfCan(pPlayer, SPELL_BREWFEST_BREW_TOSS);
        }
    }
};

CreatureAI* GetAI_npc_brewfest_maiden(Creature* pCreature)
{
    return new npc_brewfest_maidenAI(pCreature);
}

/***********************************/

enum
{
    WORMHOLE_HELLO          = 14785,
    WORMHOLE_TUNDRA         = 2000006612,
    WORMHOLE_FJORD          = 2000006613,
    WORMHOLE_BASIN          = 2000006614,
    WORMHOLE_ICECROWN       = 2000006615,
    WORMHOLE_PEAKS          = 2000006616,
    WORMHOLE_UNDER          = 2000006617,

    WORMHOLE_SPELL_TUNDRA   = 67834,
    WORMHOLE_SPELL_BASIN    = 67835,
    WORMHOLE_SPELL_ICECROWN = 67836,
    WORMHOLE_SPELL_PEAKS    = 67837,
    WORMHOLE_SPELL_FJORD    = 67838,
};

struct MANGOS_DLL_DECL npc_wormholeAI : public ScriptedAI
{
    npc_wormholeAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    bool hasUndergroundOption;

    void Reset()
    {
        hasUndergroundOption = roll_chance_i(15);
    }
};

CreatureAI* GetAI_npc_wormhole(Creature* pCreature)
{
    return new npc_wormholeAI(pCreature);
}

bool GossipHello_npc_wormhole(Player *player, Creature *_Creature)
{
    if (_Creature->GetCreatorGuid() == player->GetObjectGuid())
    {
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, WORMHOLE_TUNDRA, GOSSIP_SENDER_MAIN, 1);
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, WORMHOLE_FJORD, GOSSIP_SENDER_MAIN, 2);
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, WORMHOLE_BASIN, GOSSIP_SENDER_MAIN, 3);
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, WORMHOLE_ICECROWN, GOSSIP_SENDER_MAIN, 4);
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, WORMHOLE_PEAKS, GOSSIP_SENDER_MAIN, 5);
        if (_Creature->AI())
        {
            if (npc_wormholeAI* ai = (npc_wormholeAI*)_Creature->AI())
                if (ai->hasUndergroundOption)
                    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, WORMHOLE_UNDER, GOSSIP_SENDER_MAIN, 6);
        }
        player->SEND_GOSSIP_MENU(WORMHOLE_HELLO, _Creature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_npc_wormhole(Player *player, Creature *_Creature, uint32 sender, uint32 action)
{
    // Main menu
    if (sender == GOSSIP_SENDER_MAIN)
    {
        player->CLOSE_GOSSIP_MENU();

        switch(action)
        {
            case 1:
            {
                if (urand(0, 1))
                    player->TeleportTo(571, 4305.505859f, 5450.839844f, 63.005806f, 0.627286f);
                else
                    player->TeleportTo(571, 3201.936279f, 5630.123535f, 133.658798f, 3.855272f);
                break;
            }
            case 2: player->CastSpell(player, WORMHOLE_SPELL_FJORD, true); break;
            case 3: player->CastSpell(player, WORMHOLE_SPELL_BASIN, true); break;
            case 4: player->CastSpell(player, WORMHOLE_SPELL_ICECROWN, true); break;
            case 5: player->CastSpell(player, WORMHOLE_SPELL_PEAKS, true); break;
            case 6: player->TeleportTo(571, 5860.15f, 516.92f, 599.82f, 3.32f); break;
        }
    }

    return true;
}

/***********************************/

enum
{
    GOSSIP_ITEM_LOST_ICC_RING_M = 2000006618,
    GOSSIP_ITEM_LOST_ICC_RING_F = 2000006619,

    MAX_RING_QUESTS             = 16,
    MAX_RINGS                   = 25
};

const uint32 iccRings[MAX_RINGS] = 
{
    50375,
    50376,
    50377,
    50378,
    52569,
    50384,
    50386,
    50387,
    50388,
    50397,
    50398,
    50399,
    50400,
    50401,
    50402,
    50403,
    50404,
    52570,
    52571,
    52572,
};

const uint32 questToRing[MAX_RING_QUESTS][2] =
{
    // Choose Your Path
    { 24815,     0 },
    // Path of Courage
    { 24827, 50388 },
    { 24834, 50403 },
    { 24835, 50404 },
    // Path of Destruction
    { 24823, 50397 },
    { 24828, 50384 },
    { 24829, 50398 },
    // Path of Might
    { 25239, 52570 },
    { 25240, 52571 },
    { 25242, 52572 },
    // Path of Vengeance
    { 24826, 50387 },
    { 24832, 50401 },
    { 24833, 50402 },
    // Path of Wisdom
    { 24825, 50386 },
    { 24830, 50399 },
    { 24831, 50400 }
};

bool GossipHello_npc_ormus(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PrepareGossipMenu(pCreature, pPlayer->GetDefaultGossipMenuForSource(pCreature));

    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TALK, pPlayer->getGender() == GENDER_MALE ? GOSSIP_ITEM_LOST_ICC_RING_M : GOSSIP_ITEM_LOST_ICC_RING_F, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);

    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    pPlayer->SendPreparedGossip(pCreature);
    return true;
}

bool GossipSelect_npc_ormus(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        
        for (int i = 0; i < MAX_RINGS; ++i)
            if (pPlayer->HasItemCount(iccRings[i], 1, true))
                return true;

        uint32 itemId = 0;
        for (int i = MAX_RING_QUESTS - 1; i >= 0; --i)
            if (pPlayer->GetQuestRewardStatus(questToRing[i][0]))
            {
                if (i == 0)
                    itemId = iccRings[urand(0, 4)];
                else
                    itemId = questToRing[i][1];
                break;
            }

        if (!itemId)
            return true;

        ItemPosCountVec dest;
        uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, itemId, 1);
        if (msg != EQUIP_ERR_OK)
            return true;

        Item* pItem = pPlayer->StoreNewItem(dest, itemId, true);
        if (!pItem)
            return true;

        if (pItem)
            pPlayer->SendNewItem(pItem, 1, true, true);

        return true;
    }

    return false;
}

/***********************************/

enum FunTrainerDefines
{
    SPELL_PROF_ALCHEMY                  = 51304,
    SPELL_PROF_BLACKSMITHING            = 51300,
    SPELL_PROF_ENCHANTING               = 51313,
    SPELL_PROF_ENGINEERING              = 51306,
    SPELL_PROF_INSCRIPTION              = 45363,
    SPELL_PROF_JEWELCRAFTING            = 51311,
    SPELL_PROF_LEATHERWORKING           = 51302,
    SPELL_PROF_TAILORING                = 51309,
    SPELL_PROF_HERBALISM                = 50300,
    SPELL_PROF_MINING                   = 50310,
    SPELL_PROF_SKINNING                 = 50305,

    PROF_INDEX_MAX                      = 11,

    OPTION_ID_LEARN_PROF                = 2000006620,
    OPTION_ID_LEARN_PROF_ALCHEMY        = 2000006621,
    OPTION_ID_LEARN_PROF_BLACKSMITHING  = 2000006622,
    OPTION_ID_LEARN_PROF_ENCHANTING     = 2000006623,
    OPTION_ID_LEARN_PROF_INSCRIPTION    = 2000006624,
    OPTION_ID_LEARN_PROF_JEWELCRAFTING  = 2000006625,
    OPTION_ID_LEARN_PROF_LEATHERWORKING = 2000006626,
    OPTION_ID_LEARN_PROF_TAILORING      = 2000006627,
    OPTION_ID_LEARN_PROF_HERBALISM      = 2000006628,
    OPTION_ID_LEARN_PROF_MINING         = 2000006629,
    OPTION_ID_LEARN_PROF_SKINNING       = 2000006630,
    OPTION_ID_LEARN_PROF_ENGINEERING    = 2000006631,

    OPTION_ID_VENDOR                    = 2000006662,

    NPC_TEXT_GREETINGS                  = 564,
    NPC_TEXT_CHOOSE_PROF                = 90000,
    NPC_TEXT_ALREADY_HAVE               = 90001,
    NPC_TEXT_TOO_MANY_PROF              = 90002,
};

static uint32 m_profByIndex[] =
{
    SPELL_PROF_ALCHEMY, SPELL_PROF_BLACKSMITHING, SPELL_PROF_ENCHANTING, SPELL_PROF_INSCRIPTION,
    SPELL_PROF_JEWELCRAFTING, SPELL_PROF_LEATHERWORKING, SPELL_PROF_TAILORING,
    SPELL_PROF_HERBALISM, SPELL_PROF_MINING, SPELL_PROF_SKINNING, SPELL_PROF_ENGINEERING
};

bool GossipHello_npc_fun_profession_trainer(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isVendor())
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_VENDOR, OPTION_ID_VENDOR, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, OPTION_ID_LEARN_PROF, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_GREETINGS, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_fun_profession_trainer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    // main menu
    if (uiSender == GOSSIP_SENDER_MAIN)
    {
        switch (uiAction)
        {
            case GOSSIP_ACTION_INFO_DEF + 1:
            {
                pPlayer->CLOSE_GOSSIP_MENU();
                pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());
                break;
            }
            case GOSSIP_ACTION_INFO_DEF + 2:
            {
                pPlayer->PlayerTalkClass->ClearMenus();
                if (pPlayer->GetFreePrimaryProfessionPoints() > 0)
                {
                    for (uint32 i = 0; i < PROF_INDEX_MAX; ++i)
                        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, OPTION_ID_LEARN_PROF_ALCHEMY + i, GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF + 1 + i);

                    pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_CHOOSE_PROF, pCreature->GetObjectGuid());

                }
                else
                    pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_TOO_MANY_PROF, pCreature->GetObjectGuid());

                break;
            }
        }
    }
    // choose profession menu
    else if (uiSender == GOSSIP_SENDER_MAIN + 1)
    {
        if (uiAction >= GOSSIP_ACTION_INFO_DEF + 1 && uiAction < GOSSIP_ACTION_INFO_DEF + 1 + PROF_INDEX_MAX)
        {
            if (pPlayer->GetFreePrimaryProfessionPoints() > 0)
            {
                uint32 profIndex = uiAction - GOSSIP_ACTION_INFO_DEF - 1;
                uint32 spellId = m_profByIndex[profIndex];
                SpellLearnSkillNode const* spellLearnSkill = _GetSpellLearnSkill(spellId);
                uint32 skillId = spellLearnSkill->skill;

                if (pPlayer->HasSkill(skillId))
                {
                    pPlayer->PlayerTalkClass->ClearMenus();
                    pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_ALREADY_HAVE, pCreature->GetObjectGuid());
                }
                else
                {
                    pPlayer->CLOSE_GOSSIP_MENU();

                    pPlayer->learnSpell(spellId, false);
                    uint32 first_spell = _GetFirstSpellInChain(spellId);
                    if (GetTalentSpellCost(first_spell))
                        pPlayer->SendTalentsInfoData(false);

                    pPlayer->SetSkill(skillId, spellLearnSkill->maxvalue, spellLearnSkill->maxvalue);
                }
            }
        }
    }

    return true;
}

enum FunPetTrainerDefines
{
    OPTION_ID_HOW_TO_TAME       = 2000006632,
    OPTION_ID_READY_M           = 2000006633,
    OPTION_ID_READY_F           = 2000006634,

    //OPTION_ID_WOLVES            = 

    NPC_TEXT_HOW_TO_TAME        = 90003,
    NPC_TEXT_ALREADY_HAS_PET    = 90004,
    NPC_TEXT_MUST_ENTER_ID      = 90005,
    NPC_TEXT_NOT_TAMEABLE       = 90006,

    MAX_PETS_IN_LIST            = 10,

    PET_SENDER_MAIN             = GOSSIP_SENDER_MAIN,
    PET_SENDER_PETS_START       = PET_SENDER_MAIN + 1,

    // sender = PET_SENDER_PETS_START + (family-1)*100 + page
};


uint32 GetFamilyByIndex(uint32 idx)
{
    if (idx <= 8)
        return idx + 1;
    if (idx <= 10)
        return idx + 2;
    if (idx <= 12)
        return idx + 9;
    if (idx <- 16)
        return idx + 11;
    if (idx <= 22)
        return idx + 13;
    if (idx <= 32)
        return idx + 14;

    return 0;
}

/*
0  1 Wolf Волк
1  2 Cat Кошка
2  3 Spider Паук
3  4 Bear Медведь
4  5 Boar Вепрь
5  6 Crocolisk Кроколиск
6  7 Carrion Bird Падальщик
7  8 Crab Краб
8  9 Gorilla Горилла
9  11 Raptor Ящер
10 12 Tallstrider Долгоног
11 20 Scorpid Скорпид
12 21 Turtle Черепаха
13 24 Bat Летучая мышь
14 25 Hyena Гиена
15 26 Bird of Prey Сова
16 27 Wind Serpent Крылатый змей
17 30 Dragonhawk Дракондор
18 31 Ravager Опустошитель
19 32 Warp Stalker Прыгуана
20 33 Sporebat Спороскат
21 34 Nether Ray Скат Пустоты
22 35 Serpent Змей
23 37 Moth Мотылек
24 38 Chimaera Химера
25 39 Devilsaur Дьявозавр
26 40 Ghoul Вурдалак
27 41 Silithid Силитид
28 42 Worm Червь
29 43 Rhino Люторог
30 44 Wasp Оса
31 45 Core Hound Гончая Недр
32 46 Spirit Beast Дух зверя
*/

bool GossipHello_npc_fun_pet_trainer(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PrepareGossipMenu(pCreature, pPlayer->GetDefaultGossipMenuForSource(pCreature));
    
    if (pPlayer->getClass() == CLASS_HUNTER)
        pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, OPTION_ID_HOW_TO_TAME, PET_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);

    pPlayer->TalkedToCreature(pCreature->GetEntry(), pCreature->GetObjectGuid());
    pPlayer->SendPreparedGossip(pCreature);
    return true;
}

bool GossipSelect_npc_fun_pet_trainer(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    // main menu
    if (uiSender == PET_SENDER_MAIN)
    {
        if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
        {
            pPlayer->PlayerTalkClass->ClearMenus();
            pPlayer->ADD_GOSSIP_ITEM_EXTENDED_ID(GOSSIP_ICON_CHAT, pPlayer->getGender() == GENDER_MALE ? OPTION_ID_READY_M : OPTION_ID_READY_F, PET_SENDER_MAIN + 1, 1, 0, 0, true);
            pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_HOW_TO_TAME, pCreature->GetObjectGuid());
            return true;
        }
    }

    return false;
}

bool GossipSelect_npc_fun_pet_trainer_with_code(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction, const char* sCode)
{
    if (uiSender == PET_SENDER_MAIN + 1)
    {
        if (uiAction == 1)
        {
            if (pPlayer->getClass() != CLASS_HUNTER)
                return true;

            pPlayer->PlayerTalkClass->ClearMenus();
            if (!*sCode)
            {
                pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_MUST_ENTER_ID, pCreature->GetObjectGuid());
                return true;
            }

            uint32 creatureId = (uint32)atoi(sCode);
            CreatureInfo const* cinfo = GetCreatureTemplateStore(creatureId);
            if (!cinfo)
            {
                pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_MUST_ENTER_ID, pCreature->GetObjectGuid());
                return true;
            }

            if (cinfo->IsExotic() && !pPlayer->CanTameExoticPets() || !cinfo->isTameable(pPlayer->CanTameExoticPets()))
            {
                pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_NOT_TAMEABLE, pCreature->GetObjectGuid());
                return true;
            }

            #ifndef WIN32
            if (sObjectMgr.GetAllowedPets().find(cinfo->Entry) == sObjectMgr.GetAllowedPets().end())
            {
                pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_MUST_ENTER_ID, pCreature->GetObjectGuid());
                return true;
            }
            #endif

            if (pPlayer->GetPetGuid() || pPlayer->GetCharmGuid())
            {
                pPlayer->SEND_GOSSIP_MENU(NPC_TEXT_ALREADY_HAS_PET, pCreature->GetObjectGuid());
                return true;
            }

            Creature* tempPet = pCreature->SummonCreature(cinfo->Entry, pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ() + 0.5f, pCreature->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 5000);
            if (!tempPet)
                return true;

            tempPet->SetLevel(80);

            pPlayer->CastSpell(tempPet, 13481, true);
            return true;
        }
    }

    return false;
}

/*********************************/
enum
{
    FUN_TELE_SILITHUS_MALL      = 2000006635,
    FUN_TELE_QUELTHALAS_MALL    = 2000006636,
    FUN_TELE_NAGRAND_DUELLING   = 2000006637,
    FUN_TELE_HELLFIRE_DUELLING  = 2000006638,
    FUN_TELE_LOCHMODAN_DUELLING = 2000006639,
    FUN_TELE_STRANGLETHORN_DUELLING = 2000006640,
    FUN_TELE_STONETALON_DUELLING = 2000006641,
    FUN_TELE_ARATHI_FARM        = 2000006642,
    FUN_TELE_ARATHI_FARM_G      = 2000006643,
    FUN_TELE_ARATHI_STROM       = 2000006644,
    FUN_TELE_ARATHI_STROM_G     = 2000006645,
    FUN_TELE_FERALAS            = 2000006646,
    FUN_TELE_FERALAS_G          = 2000006647,
    FUN_TELE_HILLSBRAD          = 2000006648,
    FUN_TELE_HILLSBRAD_G        = 2000006649,
    FUN_TELE_HYJAL              = 2000006650,
    FUN_TELE_HYJAL_G            = 2000006651,

    FUN_TELE_HELLO              = 90007,

    NUM_TELE_LOCS_FUN           = 17,

    SPELL_FUN_SUMMON            = 48955,
};

teleStruct teleListFun[NUM_TELE_LOCS_FUN] = 
{
    { FUN_TELE_SILITHUS_MALL,          1, -10732.93f,   2494.98f,    6.78f, HORDE,    0, false },
    { FUN_TELE_QUELTHALAS_MALL,        0,   4290.66f,  -2788.28f,    6.40f, ALLIANCE, 0, false },

    { FUN_TELE_NAGRAND_DUELLING,     530,  -2508.61f,   8610.92f,  191.71f, 0, 0, false },
    { FUN_TELE_HELLFIRE_DUELLING,    530,   -814.55f,   1518.64f,  451.13f, 0, 0, false },
    { FUN_TELE_LOCHMODAN_DUELLING,     0,  -5155.56f,  -3459.94f,  298.42f, 0, 0, false },
    { FUN_TELE_STRANGLETHORN_DUELLING, 0, -12128.36f,    939.71f,    3.06f, 0, 0, false },
    { FUN_TELE_STONETALON_DUELLING,    1,  2840.99f,    2265.07f,  204.80f, 0, 0, false },

    { FUN_TELE_ARATHI_FARM,            0,  -1838.24f,  -4237.79f,    2.23f, 0, 0, false },
    { FUN_TELE_ARATHI_FARM_G,          0,  -1838.24f,  -4237.79f,    2.23f, 0, 0, true  },

    { FUN_TELE_ARATHI_STROM,           0,  -1568.08f,  -1804.41f,   67.28f, 0, 0, false },
    { FUN_TELE_ARATHI_STROM_G,         0,  -1568.08f,  -1804.41f,   67.28f, 0, 0, true  },

    { FUN_TELE_FERALAS,                1,  -5804.15f,   3386.73f,   55.99f, 0, 0, false },
    { FUN_TELE_FERALAS_G,              1,  -5804.15f,   3386.73f,   55.99f, 0, 0, true  },

    { FUN_TELE_HYJAL,                  1,   4931.33f,  -2642.24f, 1427.76f, 0, 0, false },
    { FUN_TELE_HYJAL_G,                1,   4931.33f,  -2642.24f, 1427.76f, 0, 0, true  },

    { FUN_TELE_HILLSBRAD,              0,  -1332.55f,    556.90f,  102.10f, 0, 0, false },
    { FUN_TELE_HILLSBRAD_G,            0,  -1332.55f,    556.90f,  102.10f, 0, 0, true  },
};

bool GossipHello_npc_fun_teleporter(Player* player, Creature* pCreature)
{
    for (uint8 i = 0; i < NUM_TELE_LOCS_FUN; ++i)
        if (teleListFun[i].team == 0 || teleListFun[i].team == player->GetTeam() || player->isGameMaster())
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, teleListFun[i].iText, GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF + i);

    player->SEND_GOSSIP_MENU(FUN_TELE_HELLO, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_fun_teleporter(Player* player, Creature* pCreature, uint32 sender, uint32 action )
{
    if (!player->getAttackers().empty())
        return true;

    #ifndef WIN32

    // Main menu
    if (sender == GOSSIP_SENDER_MAIN + 1)
    {
        if (action >= GOSSIP_ACTION_INFO_DEF && action < GOSSIP_ACTION_INFO_DEF + NUM_TELE_LOCS_FUN)
        {
            action -= GOSSIP_ACTION_INFO_DEF;
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();
            player->TeleportTo(teleListFun[action].mapId, teleListFun[action].x, teleListFun[action].y, teleListFun[action].z, player->GetOrientation());
            if (teleListFun[action].group && player->GetGroup())
            {
                uint32 zone = sTerrainMgr.GetZoneId(teleListFun[action].mapId, teleListFun[action].x, teleListFun[action].y, teleListFun[action].z);
                // check every member of the group to be able to join
                for (GroupReference *itr = player->GetGroup()->GetFirstMember(); itr != NULL; itr = itr->next())
                {
                    Player* member = itr->getSource();
                    if (!member || member == player || !member->IsInWorld())
                        continue;

                    member->SetSummonPoint(teleListFun[action].mapId, teleListFun[action].x, teleListFun[action].y, teleListFun[action].z);
                    WorldPacket data(SMSG_SUMMON_REQUEST, 8+4+4);
                    data << player->GetObjectGuid();                            // summoner guid
                    data << uint32(zone);                                       // summoner zone
                    data << uint32(MAX_PLAYER_SUMMON_DELAY * IN_MILLISECONDS);  // auto decline after msecs
                    member->GetSession()->SendPacket(&data);
                }
            }
        }
    }

    #endif

    return true;
}

/*********************************/

typedef struct
{
    uint32 uiText;
    eConfigUInt32Values Rate;
    eConfigUInt32Values Bracket;
    uint8  plClass;
    uint32 items[9];
} morpherEntry;

enum
{
    MAX_MORPHS              = 40,

    GOSSIP_FUN_SERVICES     = 90008,
    GOSSIP_SERVICES_SURE    = 90009,

    GOSSIP_FUN_MORPHER      = 90010,
    GOSSIP_FUN_MORPHER_ERROR = 90011,
    
    GOSSIP_REROLL_ERROR_H   = 90012,
    GOSSIP_REROLL_ERROR_A   = 90013,

    ACTION_MORPH            = 2000006652,
    ACTION_A1               = 2000006653,
    ACTION_A2               = 2000006654,
    ACTION_A3               = 2000006655,
    ACTION_A4               = 2000006656,
    ACTION_REMOVE_MORPH     = 2000006657,
    ACTION_YES              = 2000006658,
    ACTION_NO               = 2000006659,
    ACTION_RACE             = 2000006660,
    ACTION_FACTION          = 2000006661,
    ACTION_CUSTOMIZE        = 2000006663,

    SENDER_FUN_SERVICES_MAIN            = GOSSIP_SENDER_MAIN + 1,
    SENDER_FUN_SERVICES_MORPHS          = SENDER_FUN_SERVICES_MAIN + 1,
    SENDER_FUN_SERVICES_MORPHS_SURE     = SENDER_FUN_SERVICES_MAIN + 2,
    SENDER_FUN_SERVICES_CUSTOMIZE_SURE  = SENDER_FUN_SERVICES_MAIN + 3,
    SENDER_FUN_SERVICES_RACE_SURE       = SENDER_FUN_SERVICES_MAIN + 4,
    SENDER_FUN_SERVICES_FACTION_SURE    = SENDER_FUN_SERVICES_MAIN + 5,

    OPTION_FUN_SERVICES_FIRST = GOSSIP_ACTION_INFO_DEF,
};

morpherEntry morphs[MAX_MORPHS] =
{                                                                                       // head   shoul  chest  waist  legs   feet   wrist  hand   back
    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_WARRIOR, { 24545, 24546, 24544, 28385, 24547, 28383, 1,     24549, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_WARRIOR, { 30488, 30490, 30486, 32805, 30489, 32793, 1,     30487, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_WARRIOR, { 33730, 33732, 33728, 33811, 33731, 33812, 1,     33729, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_WARRIOR, { 35068, 35070, 35066, 35161, 35069, 35146, 1,     35067, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_PALADIN, { 27704, 27706, 27702, 28641, 27705, 28642, 1,     27703, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_PALADIN, { 31997, 31996, 31992, 32801, 31995, 32789, 1,     31993, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_PALADIN, { 33697, 33699, 33695, 33888, 33698, 33890, 1,     33696, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_PALADIN, { 35029, 35031, 35027, 35155, 35030, 35140, 1,     35028, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_ROGUE,   { 25830, 25832, 25831, 28423, 25833, 28422, 1,     25834, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_ROGUE,   { 31999, 32001, 32002, 32802, 32000, 32790, 1,     31998, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_ROGUE,   { 33701, 33703, 33704, 33891, 33702, 33892, 1,     33700, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_ROGUE,   { 35033, 35035, 35036, 35156, 35034, 35141, 1,     35032, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_PRIEST,  { 31410, 31412, 31413, 0,     31411, 28402, 1,     31409, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_PRIEST,  { 32016, 32018, 32019, 32979, 32017, 32981, 1,     32015, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_PRIEST,  { 33718, 33720, 33721, 33900, 33719, 33902, 33901, 33717, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_PRIEST,  { 35054, 35056, 35057, 35159, 35055, 35144, 35174, 35053, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_SHAMAN,  { 31400, 31407, 31396, 28639, 31406, 28640, 1,     31397, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_SHAMAN,  { 32006, 32008, 32004, 32804, 32007, 32792, 1,     32005, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_SHAMAN,  { 33708, 33710, 33706, 33895, 33709, 33896, 1,     33707, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_SHAMAN,  { 35044, 35046, 35042, 35157, 35045, 35142, 1,     35043, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_MAGE,    { 25855, 25854, 25856, 28409, 25858, 28410, 1,     25857, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_MAGE,    { 32048, 32047, 32050, 32807, 32051, 32795, 1,     32049, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_MAGE,    { 33758, 33757, 33760, 33912, 33761, 33914, 1,     33759, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_MAGE,    { 35097, 35096, 35099, 35164, 35100, 35149, 1,     35098, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_WARLOCK, { 24553, 24554, 24552, 28404, 24555, 28402, 1,     24556, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_WARLOCK, { 31980, 31979, 31982, 32799, 31983, 32787, 1,     31981, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_WARLOCK, { 33683, 33682, 33685, 33882, 33686, 33884, 1,     33684, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_WARLOCK, { 35010, 35009, 35012, 35153, 35013, 35138, 1,     35011, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_HUNTER,  { 28331, 28333, 28334, 28450, 28332, 28449, 1,     28335, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_HUNTER,  { 31962, 31964, 31960, 32797, 31963, 32785, 1,     31961, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_HUNTER,  { 33666, 33668, 33664, 33877, 33667, 33878, 1,     33665, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_HUNTER,  { 34992, 34994, 34990, 35151, 34993, 35136, 1,     34991, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_DRUID,   { 28127, 28129, 28130, 28443, 28128, 28444, 1,     28126, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_DRUID,   { 31988, 31990, 31991, 32798, 31989, 32786, 1,     31987, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_DRUID,   { 33691, 33693, 33694, 33879, 33692, 33880, 1,     33690, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_DRUID,   { 35023, 35025, 35026, 35152, 35024, 35137, 1,     35022, 1     } },

    { ACTION_A1, CONFIG_UINT32_FUN_RATE_A1, CONFIG_UINT32_FUN_BRACKET_A1, CLASS_DEATH_KNIGHT, { 24545, 24546, 24544, 28385, 24547, 28383, 1,     24549, 1     } },
    { ACTION_A2, CONFIG_UINT32_FUN_RATE_A2, CONFIG_UINT32_FUN_BRACKET_A2, CLASS_DEATH_KNIGHT, { 30488, 30490, 30486, 32805, 30489, 32793, 1,     30487, 1     } },
    { ACTION_A3, CONFIG_UINT32_FUN_RATE_A3, CONFIG_UINT32_FUN_BRACKET_A3, CLASS_DEATH_KNIGHT, { 33730, 33732, 33728, 33811, 33731, 33812, 1,     33729, 1     } },
    { ACTION_A4, CONFIG_UINT32_FUN_RATE_A4, CONFIG_UINT32_FUN_BRACKET_A4, CLASS_DEATH_KNIGHT, { 35068, 35070, 35066, 35161, 35069, 35146, 1,     35067, 1     } },

};

bool GossipHello_npc_fun_services(Player* player, Creature* pCreature)
{
    #ifndef WIN32

    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_MORPH, SENDER_FUN_SERVICES_MAIN, OPTION_FUN_SERVICES_FIRST);
    if (!player->HasAtLoginFlag(AT_LOGIN_CUSTOMIZE) && sWorld.getConfig(CONFIG_BOOL_FUN_REROLL_ENABLED))
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_CUSTOMIZE, SENDER_FUN_SERVICES_MAIN, OPTION_FUN_SERVICES_FIRST+1);
    if (!player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE) && sWorld.getConfig(CONFIG_BOOL_FUN_REROLL_ENABLED))
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_RACE, SENDER_FUN_SERVICES_MAIN, OPTION_FUN_SERVICES_FIRST+2);
    if (!player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION) && sWorld.getConfig(CONFIG_BOOL_FUN_REROLL_ENABLED))
        player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_FACTION, SENDER_FUN_SERVICES_MAIN, OPTION_FUN_SERVICES_FIRST+3);
    player->SEND_GOSSIP_MENU(GOSSIP_FUN_SERVICES, pCreature->GetObjectGuid());

    #endif

    return true;
}

bool GossipSelect_npc_fun_services(Player* player, Creature* pCreature, uint32 sender, uint32 action)
{
    // Main menu
    if (sender == SENDER_FUN_SERVICES_MAIN)
    {
        player->CLOSE_GOSSIP_MENU();
        if (action == OPTION_FUN_SERVICES_FIRST)
        {
            player->PlayerTalkClass->ClearMenus();
            for (uint8 i = 0; i < MAX_MORPHS; ++i)
                if (morphs[i].plClass == player->getClass() || player->isGameMaster())
                    player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, morphs[i].uiText, SENDER_FUN_SERVICES_MORPHS, OPTION_FUN_SERVICES_FIRST + i);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_INTERACT_1, ACTION_REMOVE_MORPH, SENDER_FUN_SERVICES_MORPHS, OPTION_FUN_SERVICES_FIRST + MAX_MORPHS);

            player->SEND_GOSSIP_MENU(GOSSIP_FUN_MORPHER, pCreature->GetObjectGuid());
        }
        else if (action == OPTION_FUN_SERVICES_FIRST + 1)
        {
            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_YES, SENDER_FUN_SERVICES_CUSTOMIZE_SURE, OPTION_FUN_SERVICES_FIRST);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_NO, SENDER_FUN_SERVICES_CUSTOMIZE_SURE, OPTION_FUN_SERVICES_FIRST + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_SERVICES_SURE, pCreature->GetObjectGuid());
        }
        else if (action == OPTION_FUN_SERVICES_FIRST + 2)
        {
            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_YES, SENDER_FUN_SERVICES_RACE_SURE, OPTION_FUN_SERVICES_FIRST);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_NO, SENDER_FUN_SERVICES_RACE_SURE, OPTION_FUN_SERVICES_FIRST + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_SERVICES_SURE, pCreature->GetObjectGuid());
        }
        else if (action == OPTION_FUN_SERVICES_FIRST + 3)
        {
            player->PlayerTalkClass->ClearMenus();
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_YES, SENDER_FUN_SERVICES_FACTION_SURE, OPTION_FUN_SERVICES_FIRST);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_NO, SENDER_FUN_SERVICES_FACTION_SURE, OPTION_FUN_SERVICES_FIRST + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_SERVICES_SURE, pCreature->GetObjectGuid());
        }

    }
    else if (sender == SENDER_FUN_SERVICES_MORPHS)
    {
        if (action >= OPTION_FUN_SERVICES_FIRST && action < OPTION_FUN_SERVICES_FIRST + MAX_MORPHS)
        {
            action -= OPTION_FUN_SERVICES_FIRST;
            player->CLOSE_GOSSIP_MENU();

            if (!player->isGameMaster() && player->GetMaxPersonalArenaRatingRequirement(sWorld.getConfig(morphs[action].Bracket)) < sWorld.getConfig(morphs[action].Rate))
            {
                player->PlayerTalkClass->ClearMenus();
                player->SEND_GOSSIP_MENU(GOSSIP_FUN_MORPHER_ERROR, pCreature->GetObjectGuid());
                return true;
            }

            if (!player->m_vis)
                player->m_vis = new Visuals;
            
            player->m_vis->m_visHead        = morphs[action].items[0];
            player->m_vis->m_visShoulders   = morphs[action].items[1];
            player->m_vis->m_visChest       = morphs[action].items[2];
            player->m_vis->m_visWaist       = morphs[action].items[3];
            player->m_vis->m_visLegs        = morphs[action].items[4];
            player->m_vis->m_visFeet        = morphs[action].items[5];
            player->m_vis->m_visWrists      = morphs[action].items[6];
            player->m_vis->m_visHands       = morphs[action].items[7];
            player->m_vis->m_visBack        = morphs[action].items[8];
            player->m_vis->m_type           = VISUALS_TYPE_MORPH;

            player->HandleAltVisSwitch();
        }
        else if (action == OPTION_FUN_SERVICES_FIRST + MAX_MORPHS)
        {
            player->PlayerTalkClass->ClearMenus();

            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_YES, SENDER_FUN_SERVICES_MORPHS_SURE, OPTION_FUN_SERVICES_FIRST);
            player->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, ACTION_NO, SENDER_FUN_SERVICES_MORPHS_SURE, OPTION_FUN_SERVICES_FIRST + 1);
            player->SEND_GOSSIP_MENU(GOSSIP_SERVICES_SURE, pCreature->GetObjectGuid());
        }

    }
    else if (sender == SENDER_FUN_SERVICES_MORPHS_SURE)
    {
        player->CLOSE_GOSSIP_MENU();
        if (action == OPTION_FUN_SERVICES_FIRST)
        {
            if (player->m_vis)
            {
                delete player->m_vis;
                player->m_vis = NULL;
            }
            player->HandleAltVisSwitch();
        }
    }
    else if (sender == SENDER_FUN_SERVICES_CUSTOMIZE_SURE || sender == SENDER_FUN_SERVICES_RACE_SURE || sender == SENDER_FUN_SERVICES_FACTION_SURE)
    {
        player->CLOSE_GOSSIP_MENU();
        if (action == OPTION_FUN_SERVICES_FIRST)
        {
            RerollType type;
            if (sender == SENDER_FUN_SERVICES_CUSTOMIZE_SURE)
                type = REROLL_CUSTOMIZE;
            else if (sender == SENDER_FUN_SERVICES_RACE_SURE)
                type = REROLL_RACE;
            else if (sender == SENDER_FUN_SERVICES_FACTION_SURE)
                type = REROLL_FACTION;

            #ifndef WIN32
            uint32 cost = sWorld.funSettings.GetRerollCost(type);
            switch (sWorld.funSettings.GetRerollCostType(type))
            {
                case COST_FREE:
                    break;
                case COST_HONOR:
                    if (player->GetCurrencyCount(CURRENCY_HONOR_POINTS) < cost)
                    {
                        player->PlayerTalkClass->ClearMenus();
                        player->SEND_GOSSIP_MENU(GOSSIP_REROLL_ERROR_H, pCreature->GetObjectGuid());
                        return true;
                    }
                    else
                        player->ModifyCurrencyCount(CURRENCY_HONOR_POINTS, -int32(cost));
                    break;
                case COST_AP:
                    if (player->GetCurrencyCount(CURRENCY_CONQUEST_POINTS) < cost)
                    {
                        player->PlayerTalkClass->ClearMenus();
                        player->SEND_GOSSIP_MENU(GOSSIP_REROLL_ERROR_A, pCreature->GetObjectGuid());
                        return true;
                    }
                    else
                        player->ModifyCurrencyCount(CURRENCY_CONQUEST_POINTS, -int32(cost));
                    break;
                default:
                    return true;
            }

            if (type == REROLL_CUSTOMIZE)
            {
                ChatHandler(player).PSendSysMessage(LANG_CUSTOMIZE_PLAYER, player->GetName());
                player->SetAtLoginFlag(AT_LOGIN_CUSTOMIZE);
                CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '8' WHERE guid = '%u'", player->GetObjectGuid().GetCounter());
            }
            else if (type == REROLL_FACTION)
            {
                ChatHandler(player).PSendSysMessage(LANG_CHANGEFACTION_PLAYER, player->GetName());
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_FACTION);
                player->SetAtLoginFlag(AT_LOGIN_CONVERT_TITLES);
                CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '320' WHERE guid = '%u'", player->GetObjectGuid().GetCounter());
            }
            else if (type == REROLL_RACE)
            {
                ChatHandler(player).PSendSysMessage(LANG_CHANGERACE_PLAYER, player->GetName());
                player->SetAtLoginFlag(AT_LOGIN_CHANGE_RACE);
                CharacterDatabase.PExecute("UPDATE characters SET at_login = at_login | '128' WHERE guid = '%u'", player->GetObjectGuid().GetCounter());
            }
            #endif
        }
    }

    return true;
}

/*********************************/

void AddSC_npcs_special()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_air_force_bots";
    pNewScript->GetAI = &GetAI_npc_air_force_bots;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_chicken_cluck";
    pNewScript->GetAI = &GetAI_npc_chicken_cluck;
    pNewScript->pQuestAcceptNPC =   &QuestAccept_npc_chicken_cluck;
    pNewScript->pQuestRewardedNPC = &QuestRewarded_npc_chicken_cluck;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_dancing_flames";
    pNewScript->GetAI = &GetAI_npc_dancing_flames;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_injured_patient";
    pNewScript->GetAI = &GetAI_npc_injured_patient;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_doctor";
    pNewScript->GetAI = &GetAI_npc_doctor;
    pNewScript->pQuestAcceptNPC = &QuestAccept_npc_doctor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_garments_of_quests";
    pNewScript->GetAI = &GetAI_npc_garments_of_quests;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_guardian";
    pNewScript->GetAI = &GetAI_npc_guardian;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_innkeeper";
    pNewScript->pGossipHello = &GossipHello_npc_innkeeper;
    pNewScript->pGossipSelect = &GossipSelect_npc_innkeeper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mount_vendor";
    pNewScript->pGossipHello =  &GossipHello_npc_mount_vendor;
    pNewScript->pGossipSelect = &GossipSelect_npc_mount_vendor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_sayge";
    pNewScript->pGossipHello = &GossipHello_npc_sayge;
    pNewScript->pGossipSelect = &GossipSelect_npc_sayge;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_tabard_vendor";
    pNewScript->pGossipHello =  &GossipHello_npc_tabard_vendor;
    pNewScript->pGossipSelect = &GossipSelect_npc_tabard_vendor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_mirror_image";
    pNewScript->GetAI = &GetAI_npc_mirror_image;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_snake_trap_serpents";
    pNewScript->GetAI = &GetAI_npc_snake_trap_serpents;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_runeblade";
    pNewScript->GetAI = &GetAI_npc_rune_blade;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_death_knight_gargoyle";
    pNewScript->GetAI = &GetAI_npc_death_knight_gargoyle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_event";
    pNewScript->pGossipHello = &GossipHello_npc_event;
    pNewScript->pGossipSelect = &GossipSelect_npc_event;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_phase_changer";
    pNewScript->pGossipHello = &GossipHello_npc_phase_changer;
    pNewScript->pGossipSelect = &GossipSelect_npc_phase_changer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_event_1";
    pNewScript->pGossipHello = &GossipHello_npc_event_1;
    pNewScript->pGossipSelect = &GossipSelect_npc_event_1;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_event_2";
    pNewScript->GetAI = &GetAI_npc_event_2;
    pNewScript->pGossipHello = &GossipHello_npc_event_2;
    pNewScript->pGossipSelect = &GossipSelect_npc_event_2;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_teleport";
    pNewScript->pGossipHello = &GossipHello_npc_teleport;
    pNewScript->pGossipSelect = &GossipSelect_npc_teleport;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_training_dummy";
    pNewScript->GetAI = &GetAI_npc_training_dummy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_pilgrims_table";
    pNewScript->pGossipHello = &GossipHello_npc_pilgrims_table;
    pNewScript->pGossipSelect = &GossipSelect_npc_pilgrims_table;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_winter_reveler";
    pNewScript->GetAI = &GetAI_npc_winter_reveler;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_risen_ally";
    pNewScript->GetAI = &GetAI_npc_risen_ally;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_explosive_decoy";
    pNewScript->GetAI = &GetAI_npc_explosive_decoy;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_eye_of_kilrogg";
    pNewScript->GetAI = &GetAI_npc_eye_of_kilrogg;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_horseman_fire_bunny";
    pNewScript->GetAI = &GetAI_npc_horseman_fire_bunny;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shade_of_horseman";
    pNewScript->GetAI = &GetAI_npc_shade_of_horseman;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_experience";
    pNewScript->pGossipHello = &GossipHello_npc_experience;
    pNewScript->pGossipSelect = &GossipSelect_npc_experience;
    pNewScript->GetAI = &GetAI_npc_experience;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_acherus_portal";
    pNewScript->GetAI = &GetAI_npc_acherus_portal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spring_rabbit";
    pNewScript->GetAI = &GetAI_npc_spring_rabbit;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_spring_egg";
    pNewScript->pGOUse = &GOUse_go_spring_egg;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_child_event";
    pNewScript->GetAI = &GetAI_npc_child_event;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_premium_pet";
    pNewScript->pGossipHello = &GossipHello_npc_premium_pet;
    pNewScript->pGossipSelect = &GossipSelect_npc_premium_pet;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ice_caller";
    pNewScript->GetAI = &GetAI_npc_ice_caller;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_de_meza";
    pNewScript->pGossipHello = &GossipHello_npc_de_meza;
    pNewScript->pGossipSelect = &GossipSelect_npc_de_meza;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_relayrace_dealer";
    pNewScript->GetAI = &GetAI_npc_relayrace_dealer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_relayrace_master";
    pNewScript->GetAI = &GetAI_npc_relayrace_master;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_brewfest_maiden";
    pNewScript->GetAI = &GetAI_npc_brewfest_maiden;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wormhole";
    pNewScript->GetAI = &GetAI_npc_wormhole;
    pNewScript->pGossipHello = &GossipHello_npc_wormhole;
    pNewScript->pGossipSelect = &GossipSelect_npc_wormhole;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ormus";
    pNewScript->pGossipHello = &GossipHello_npc_ormus;
    pNewScript->pGossipSelect = &GossipSelect_npc_ormus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fun_profession_trainer";
    pNewScript->pGossipHello =  &GossipHello_npc_fun_profession_trainer;
    pNewScript->pGossipSelect = &GossipSelect_npc_fun_profession_trainer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fun_pet_trainer";
    pNewScript->pGossipHello = &GossipHello_npc_fun_pet_trainer;
    pNewScript->pGossipSelect = &GossipSelect_npc_fun_pet_trainer;
    pNewScript->pGossipSelectWithCode = &GossipSelect_npc_fun_pet_trainer_with_code;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fun_teleporter";
    pNewScript->pGossipHello = &GossipHello_npc_fun_teleporter;
    pNewScript->pGossipSelect = &GossipSelect_npc_fun_teleporter;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_fun_services";
    pNewScript->pGossipHello = &GossipHello_npc_fun_services;
    pNewScript->pGossipSelect = &GossipSelect_npc_fun_services;
    pNewScript->RegisterSelf();
}
