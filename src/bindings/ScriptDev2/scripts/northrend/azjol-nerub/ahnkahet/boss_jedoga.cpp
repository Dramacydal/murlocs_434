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
SDName: Boss_Jedoga
SD%Complete: 90%
SDAuthor: Tassadar
SDComment: Correct Timers
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"

struct Locations
{
    float x, y, z, o;
    uint32 id;
};
enum
{
    SPELL_SPHERE_VISUAL                 = 56075,
    SPELL_GIFT_OF_THE_HERALD            = 56219,
    SPELL_CYCLONE_STRIKE                = 60030,
    SPELL_CYCLONE_STRIKE_H              = 56855,
    SPELL_LIGHTNING_BOLT                = 56891,
    SPELL_LIGHTNING_BOLT_H              = 60032,
    SPELL_THUNDERSHOCK                  = 56926,
    SPELL_THUNDERSHOCK_H                = 60029,

    SAY_AGGRO                           = -1619017,
    SAY_CALL_SACRIFICE_1                = -1619018,
    SAY_CALL_SACRIFICE_2                = -1619019,
    SAY_SACRIFICE_1                     = -1619020,
    SAY_SACRIFICE_2                     = -1619021,
    SAY_SLAY_1                          = -1619022,
    SAY_SLAY_2                          = -1619023,
    SAY_SLAY_3                          = -1619024,
    SAY_DEATH                           = -1619025,
    SAY_PREACHING_1                     = -1619026,
    SAY_PREACHING_2                     = -1619027,
    SAY_PREACHING_3                     = -1619028,
    SAY_PREACHING_4                     = -1619029,
    SAY_PREACHING_5                     = -1619030,

    SAY_VOLUNTEER_1                     = -1619031,         //said by the volunteer image
    SAY_VOLUNTEER_2                     = -1619032,

    //NPC_TWILIGHT_INITIATE               = 30114,
    NPC_TWILIGHT_VOLUNTEER              = 30385,
    NPC_JEDOGA                          = 29310,

    GO_CIRCLE                           = 194394,           // Propably wrong id

    //Jedoga Shadowseeker's phases
    PHASE_NOSTART                       = 0,
    PHASE_PREACHING                     = 1,
    PHASE_DESCEND                       = 2,
        SUBPHASE_FLY_DESCEND            = 21,
    PHASE_FIGHT                         = 3,
    PHASE_SACRIFACE                     = 4,
        SUBPHASE_FLY_UP                 = 41,
        SUBPHASE_CALL_VOLUNTEER         = 42,
        SUBPHASE_WAIT_FOR_VOLUNTEER     = 43,
        SUBPHASE_SACRIFACE              = 44,

    //Twilight Volunteer's sacriface phases
    SACRIFACE_CHOOSEN                   = 1,
    SACRIFACE_DIE                       = 2,

    VOLUNTEER_COUNT                     = 29,
};
#define CENTER_X                        367.800f
#define CENTER_Y                        -704.403f
#define GROUND_Z                        -16.17f

#define JEDOGA_X                        371.47f
#define JEDOGA_Y                        -701.71f
#define JEDOGA_Z                        -11.720f
#define JEDOGA_O                        5.565f

static Locations VolunteerLoc[]=
{
    //29 Volunteers
    {365.68f, -735.95f, -16.17f, 1.607f}, // Right, first line
    {367.12f, -736.13f, -16.17f, 1.607f},
    {369.03f, -736.06f, -16.17f, 1.607f},
    {371.66f, -735.97f, -16.17f, 1.607f},
    {373.47f, -735.63f, -16.17f, 1.607f},

    {365.45f, -739.03f, -16.00f, 1.607f}, // Right, second line
    {367.56f, -738.62f, -16.00f, 1.607f},
    {369.62f, -738.22f, -16.17f, 1.607f},
    {371.66f, -737.82f, -16.06f, 1.607f},
    {373.75f, -737.41f, -16.00f, 1.607f},

    {400.99f, -705.41f, -16.00f, 2.491f}, // Center, from right
    {398.07f, -710.02f, -16.00f, 2.491f},
    {395.34f, -713.76f, -16.00f, 2.491f},
    {393.42f, -716.39f, -16.00f, 2.491f},
    {391.48f, -718.94f, -16.00f, 2.491f},
    {388.80f, -722.46f, -16.00f, 2.491f},
    {386.19f, -725.89f, -16.00f, 2.491f},
    {383.61f, -729.29f, -16.00f, 2.491f},
    {380.37f, -733.55f, -16.00f, 2.491f},
    
    {402.72f, -700.79f, -16.00f, 3.046f}, // Left, first line
    {402.63f, -698.86f, -16.18f, 3.149f},
    {402.62f, -697.10f, -16.17f, 3.149f},
    {402.61f, -695.50f, -16.17f, 3.059f},
    {402.20f, -693.39f, -16.17f, 3.159f},

    {405.31f, -701.29f, -16.00f, 2.924f}, // Left, second line
    {405.46f, -699.25f, -16.00f, 3.198f},
    {405.40f, -697.19f, -16.00f, 3.150f},
    {405.35f, -695.30f, -16.00f, 3.150f},
    {405.29f, -693.26f, -16.00f, 3.150f}
};

/*######
## npc_twilight_volunteer
######*/
struct MANGOS_DLL_DECL npc_twilight_volunteerAI : public ScriptedAI
{
    npc_twilight_volunteerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    bool m_bIsVulunteerNear;
    uint32 m_uiCheckTimer;
    bool m_bIsDead;
    void Reset()
    {
        m_uiPhase = 0;
        m_bIsVulunteerNear = false;
        m_uiCheckTimer = 1000;
        m_bIsDead = false;
        m_creature->SetRespawnDelay(DAY);
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bIsDead)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage > m_creature->GetHealth())
        {
            m_creature->ForcedDespawn(10000);
            m_bIsDead = true;
            uiDamage = 0;

            m_creature->SetHealth(1);
            m_creature->SetVisibility(VISIBILITY_OFF);
        }
    }
    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
                return;

        switch(uiPointId)
        {
            case 0:
                m_bIsVulunteerNear = true;
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->ForcedDespawn(10000);
                m_creature->SetVisibility(VISIBILITY_OFF);
                break;
        }
    }
    void Sacriface(uint8 phase)
    {
        if(m_bIsDead)
            return;

        m_uiPhase = phase;
        switch(m_uiPhase)
        {
            case SACRIFACE_CHOOSEN:
                switch(urand(0, 1))
                {
                    case 0: DoScriptText(SAY_VOLUNTEER_1, m_creature); break;
                    case 1: DoScriptText(SAY_VOLUNTEER_2, m_creature); break;
                }
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, GROUND_Z);
                break;
            case SACRIFACE_DIE:
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                break;
        }
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_bIsDead) return;

        if (m_pInstance->GetData(TYPE_JEDOGA) != IN_PROGRESS) m_creature->ForcedDespawn();

    }
};

/*######
## boss_jedoga
######*/

struct MANGOS_DLL_DECL boss_jedogaAI : public ScriptedAI
{
    boss_jedogaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsVulunteerNear;
    bool m_bVolunteerDied;
    bool m_isRealEvade;
    uint8 m_uiPhase;
    uint8 m_uiSubPhase;
    uint8 m_uiPreachingText;
    Creature *pVolunteer;
    uint8 m_uiLastSacrifaceHP;

    uint32 m_uiPreachingTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiSacrifaceTimer;
    uint32 m_uiCycloneStrikeTimer;
    uint32 m_uiLightningBoltTimer;
    uint32 m_uiThundershockTimer;


    void Reset()
    {
        m_uiPhase = PHASE_PREACHING;
        m_uiSubPhase = 0;
        m_uiPreachingText = 0;
        m_bIsVulunteerNear = false;
        m_bVolunteerDied = false;
        m_uiLastSacrifaceHP = 0;
        m_isRealEvade = true;

        m_uiCheckTimer = 1000;
        m_uiSacrifaceTimer = 2000;
        m_uiPreachingTimer = 0;
        m_uiCycloneStrikeTimer = 17000;
        m_uiLightningBoltTimer = 3000;
        m_uiThundershockTimer = 30000;

        DoCast(m_creature, SPELL_SPHERE_VISUAL);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, NOT_STARTED);
    }
    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        AttackStart(pWho);
         if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, IN_PROGRESS);
    }
    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }
    void EnterEvadeMode()
    {
        if (m_uiPhase == PHASE_PREACHING && !m_isRealEvade) return;

        m_uiPhase = PHASE_PREACHING;
        SetCombatMovement(false);
        m_creature->GetMotionMaster()->MovementExpired();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->InterruptNonMeleeSpells(true);
        DoCast(m_creature, SPELL_SPHERE_VISUAL);
        m_creature->GetMap()->CreatureRelocation(m_creature, JEDOGA_X, JEDOGA_Y, JEDOGA_Z, JEDOGA_O);
        m_creature->MonsterMoveWithSpeed(JEDOGA_X, JEDOGA_Y, JEDOGA_Z, 26);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, NOT_STARTED);

        if (m_isRealEvade)
        {
            m_creature->SetHealth(m_creature->GetMaxHealth());
            RespawnInitiates();
            m_isRealEvade = false;
        }
    }

    void RespawnInitiates()
    {
        std::list<Creature*> lInitiates;                      //respawn Twilight initiates
        GetCreatureListWithEntryInGrid(lInitiates, m_creature, NPC_TWILIGHT_INITIATE, DEFAULT_VISIBILITY_INSTANCE);

        if (!lInitiates.empty())
        {
            for(std::list<Creature*>::iterator iter = lInitiates.begin(); iter != lInitiates.end(); ++iter)
            {
                if ((*iter) && !(*iter)->isAlive())
                    (*iter)->Respawn();
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
         if (m_pInstance)
            m_pInstance->SetData(TYPE_JEDOGA, DONE);
    }

    Creature* SelectRandomVolunteer(float fRange)
    {
        std::list<Creature* > lVolunteerList;
        GetCreatureListWithEntryInGrid(lVolunteerList, m_creature, NPC_TWILIGHT_VOLUNTEER, fRange);

        //This should not appear!
        if (lVolunteerList.empty()){
            EnterEvadeMode();
            debug_log("SD2: AhnKahet: No volunteer to sacriface!");
            return NULL;
        }


        std::list<Creature* >::iterator iter = lVolunteerList.begin();
        advance(iter, urand(0, lVolunteerList.size()-1));

        return *iter;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiPhase == PHASE_NOSTART)
            return;
        else if(m_uiPhase == PHASE_PREACHING)
        {
            if(m_uiCheckTimer <= uiDiff)
            {
                if(Creature *pTemp = GetClosestCreatureWithEntry(m_creature, NPC_TWILIGHT_INITIATE, 150.0f))
                    m_uiCheckTimer = 2000;
                else
                {
                    m_uiPhase = PHASE_DESCEND;
                    m_uiSubPhase = SUBPHASE_FLY_DESCEND;
                    return;
                }
            }else m_uiCheckTimer -= uiDiff;

            if(m_uiPreachingTimer > uiDiff)
            {
                m_uiPreachingTimer -= uiDiff;
                return;
            }

            if (m_pInstance)
                if(m_pInstance->GetData(TYPE_TALDARAM) != DONE)
                    return;

            switch(m_uiPreachingText)
            {
               case 0:
                   DoScriptText(SAY_PREACHING_1, m_creature);
                   m_uiPreachingText++;
                   m_uiPreachingTimer = 9500;
                   break;
               case 1:
                   DoScriptText(SAY_PREACHING_2, m_creature);
                   m_uiPreachingText++;
                   m_uiPreachingTimer = 6500;
                   break;
               case 2:
                   DoScriptText(SAY_PREACHING_3, m_creature);
                   m_uiPreachingText++;
                   m_uiPreachingTimer = 8500;
                   break;
               case 3:
                   DoScriptText(SAY_PREACHING_4, m_creature);
                   m_uiPreachingText++;
                   m_uiPreachingTimer = 7500;
                   break;
               case 4:
                   DoScriptText(SAY_PREACHING_5, m_creature);
                   m_uiPreachingText = 0;
                   m_uiPreachingTimer = 12000;
                   break;
            }
            return;
        }
        else if(m_uiPhase == PHASE_DESCEND)
        {
            if(m_uiSubPhase == SUBPHASE_FLY_DESCEND)
            {
                if(GetClosestCreatureWithEntry(m_creature, NPC_TWILIGHT_VOLUNTEER, 150.0f))
                    return;

                SetCombatMovement(true);
                m_creature->GetMap()->CreatureRelocation(m_creature, CENTER_X, CENTER_Y, GROUND_Z, JEDOGA_O);
                m_creature->MonsterMoveWithSpeed(CENTER_X, CENTER_Y, GROUND_Z, 26);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                  m_creature->SetInCombatWithZone();
                //Spawn Volunteers
                for(int i = 0; i <= 28; ++i)
                {
                    if(Creature *pTemp = m_creature->SummonCreature(NPC_TWILIGHT_VOLUNTEER, VolunteerLoc[i].x, VolunteerLoc[i].y, VolunteerLoc[i].z, VolunteerLoc[i].o, TEMPSUMMON_CORPSE_DESPAWN, 0))
                        pTemp->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                }
                m_uiSubPhase = 0;
                m_uiPhase = PHASE_FIGHT;
                return;
            }
        }
        else if(m_uiPhase == PHASE_FIGHT)
        {
            //Evade if no target in this phase
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            {
                m_isRealEvade = true;
                EnterEvadeMode();
                return;
            }

            //Spells
            //Cyclone Strike
            if(m_uiCycloneStrikeTimer <= uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_CYCLONE_STRIKE : SPELL_CYCLONE_STRIKE_H);
                m_uiCycloneStrikeTimer = 10000 + rand()%10000;
            }else m_uiCycloneStrikeTimer -= uiDiff;

            //Lightning Bolt
            if(m_uiLightningBoltTimer <= uiDiff)
            {
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_LIGHTNING_BOLT : SPELL_LIGHTNING_BOLT_H);
                m_uiLightningBoltTimer = 3000 + rand()%2000;
            }else m_uiLightningBoltTimer -= uiDiff;

            //Thundershock
            if(m_uiThundershockTimer <= uiDiff)
            {
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, m_bIsRegularMode ? SPELL_THUNDERSHOCK : SPELL_THUNDERSHOCK_H);
                m_uiThundershockTimer = 20000 + rand()%10000;
            }else m_uiThundershockTimer -= uiDiff;

            //Health check
            if(m_uiCheckTimer <= uiDiff)
            {
                uint8 health = m_creature->GetHealth()*100 / m_creature->GetMaxHealth();                    
                if(m_uiLastSacrifaceHP == 0 && health <= 75)
                {
                    m_uiLastSacrifaceHP = 75;
                    m_uiPhase = PHASE_SACRIFACE;
                    m_uiSubPhase = SUBPHASE_FLY_UP;
                    return;
                }
                else if(m_uiLastSacrifaceHP == 75 && health <= 50)
                {
                    m_uiLastSacrifaceHP = 50;
                    m_uiPhase = PHASE_SACRIFACE;
                    m_uiSubPhase = SUBPHASE_FLY_UP;
                    return;
                }
                else if(m_uiLastSacrifaceHP == 50 && health <= 25)
                {
                    m_uiLastSacrifaceHP = 25;
                    m_uiPhase = PHASE_SACRIFACE;
                    m_uiSubPhase = SUBPHASE_FLY_UP;
                    return;
                }
                m_uiCheckTimer = 1000;
            }else m_uiCheckTimer -= uiDiff; 

            DoMeleeAttackIfReady();
        }
        else if(m_uiPhase == PHASE_SACRIFACE)
        {
            if(m_uiSubPhase == SUBPHASE_FLY_UP)
            {
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MovementExpired();
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->InterruptNonMeleeSpells(true);
                DoCast(m_creature, SPELL_SPHERE_VISUAL);
                m_creature->GetMap()->CreatureRelocation(m_creature, JEDOGA_X, JEDOGA_Y, JEDOGA_Z, JEDOGA_O);
                m_creature->MonsterMoveWithSpeed(JEDOGA_X, JEDOGA_Y, JEDOGA_Z, 26);
                m_uiSubPhase = SUBPHASE_CALL_VOLUNTEER;
                GameObject* pCircle = GetClosestGameObjectWithEntry(m_creature,GO_CIRCLE,50.0f);
                if (pCircle && !pCircle->isSpawned())
                    pCircle->SetRespawnTime(10000);
            }
            else if(m_uiSubPhase == SUBPHASE_CALL_VOLUNTEER)
            {
                pVolunteer = SelectRandomVolunteer(150.0f);
                if(pVolunteer)
                {
                    switch(urand(0, 1))
                    {
                        case 0: DoScriptText(SAY_CALL_SACRIFICE_1, m_creature); break;
                        case 1: DoScriptText(SAY_CALL_SACRIFICE_2, m_creature); break;
                    }
                    ((npc_twilight_volunteerAI*)pVolunteer->AI())->Sacriface(SACRIFACE_CHOOSEN);
                    m_uiSubPhase = SUBPHASE_WAIT_FOR_VOLUNTEER;
                }
            }
            else if(m_uiSubPhase == SUBPHASE_WAIT_FOR_VOLUNTEER)
            {
                if(m_uiCheckTimer <= uiDiff)
                {
                    if(pVolunteer && pVolunteer->isAlive()){
                        if(pVolunteer->GetVisibility() == VISIBILITY_OFF)
                        {
                            if(((npc_twilight_volunteerAI*)pVolunteer->AI())->m_bIsVulunteerNear)
                                m_bVolunteerDied = false;
                            else
                                m_bVolunteerDied = true;
                            m_uiSubPhase = SUBPHASE_SACRIFACE;
                        }
                    }else{
                        m_bIsVulunteerNear = true;
                        m_bVolunteerDied = true;
                        m_uiSubPhase = SUBPHASE_SACRIFACE;
                    }
                    m_uiCheckTimer = 1000;
                }else m_uiCheckTimer -= uiDiff;
            }
            else if(m_uiSubPhase == SUBPHASE_SACRIFACE)
            {
                switch(urand(0, 1))
                {
                    case 0: DoScriptText(SAY_SACRIFICE_1, m_creature); break;
                    case 1: DoScriptText(SAY_SACRIFICE_2, m_creature); break;
                }
                if(!m_bVolunteerDied)
                    DoCast(m_creature, SPELL_GIFT_OF_THE_HERALD);

                m_creature->GetMap()->CreatureRelocation(m_creature, CENTER_X, CENTER_Y, GROUND_Z, JEDOGA_O);
                m_creature->MonsterMoveWithSpeed(CENTER_X, CENTER_Y, GROUND_Z, 26);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveAurasDueToSpell(SPELL_SPHERE_VISUAL);
                SetCombatMovement(true);
                m_uiPhase = PHASE_FIGHT;
                if(m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }
        }
    }
};
CreatureAI* GetAI_boss_jedoga(Creature* pCreature)
{
    return new boss_jedogaAI(pCreature);
}

CreatureAI* GetAI_npc_twilight_volunteer(Creature* pCreature)
{
    return new npc_twilight_volunteerAI(pCreature);
}

void AddSC_boss_jedoga()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_jedoga";
    pNewScript->GetAI = &GetAI_boss_jedoga;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_twilight_volunteer";
    pNewScript->GetAI = &GetAI_npc_twilight_volunteer;
    pNewScript->RegisterSelf();
}
