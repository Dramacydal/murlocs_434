/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_leviathan
SD%Complete: 
SDComment: needs vehicles , add Flying Mobs, Throw Passenger, Grab Crate
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"
#include "Vehicle.h"

enum say
{
    SAY_AGGRO           = -1603203,
    SAY_DEATH           = -1603202,
    SAY_SLAY            = -1603201,
    SAY_CHANGE1         = -1603204,
    SAY_CHANGE2         = -1603205,
    SAY_CHANGE3         = -1603206,
    SAY_PLAYER_ON_TOP   = -1603207,
    SAY_OVERLOAD1       = -1603208,
    SAY_OVERLOAD2       = -1603209,
    SAY_OVERLOAD3       = -1603210,
    SAY_HARD_MODE       = -1603211,
    SAY_TOWERS_DOWN     = -1603212,
    SAY_FROST_TOWER     = -1603213,
    SAY_FIRE_TOWER      = -1603214,
    SAY_ENERGY_TOWER    = -1603215,
    SAY_NATURE_TOWER    = -1603216,

    EMOTE_PURSUE        = -1603352,
};

enum spells
{
    SPELL_PURSUED           = 62374,

    SPELL_MISSILE_BARRAGE   = 62400,
    SPELL_FLAME_VENTS       = 62396,
    SPELL_FLAME_VENTS_TIGGER= 63847,
    SPELL_BATTERING_RAM     = 62376,

    SPELL_GATHERING_SPEED   = 62375,
    // interupted by
    SPELL_OVERLOAD_CIRCUIT  = 62399,

    SPELL_SEARING_FLAME     = 62402, // used by defense turret
    // interupted by
    SPELL_SYSTEMS_SHUTDOWN  = 62475,

    SPELL_FLAME_CANNON      = 62395,
    //SPELL_FLAME_CANNON    = 64692, trigger the same spell
    
    // used by players -> to be added later
    SPELL_ELECTROSHOCK      = 62522,
    SPELL_SMOKE_TRAIL       = 63575,

    // tower of nature
    SPELL_LASH              = 65062,
    SPELL_FREYA_WARD        = 62906,
    SPELL_TOWER_OF_LIFE     = 64482,
    // tower of flames
    SPELL_MIMIRON_INFERNO   = 62910,
    SPELL_TOWER_OF_FLAMES   = 65075,
    // tower of frost
    SPELL_TOWER_OF_FROST    = 65079,
    SPELL_HODIR_FURY        = 62533,
    // tower of storms
    SPELL_THORIMS_HAMMER    = 62911,
    SPELL_TOWER_OF_STORMS   = 65076,

    // pool of tar
    SPELL_TAR_PASSIV        = 62288,
    SPELL_BLAZE             = 62292,

    // Pyrit Vehicles
    SPELL_LIQUID_PYRITE     = 62494,
    

    AURA_DUMMY_BLUE         = 63294,
    AURA_DUMMY_GREEN        = 63295,
    AURA_DUMMY_YELLOW       = 63292,

};

enum Mobs
{
    MOB_MECHANOLIFT     = 33214,
    MOB_LIQUID          = 33189,
    MOB_CONTAINER       = 33218,
    
    MOB_THORIM_BEACON           = 33365,
    MOB_MIMIRON_BEACON          = 33370,
    MOB_HODIR_BEACON            = 33212,
    MOB_FREYA_BEACON            = 33367,
    NPC_THORIM_TARGET_BEACON    = 33364,
    NPC_MIMIRON_TARGET_BEACON   = 33369,
    NPC_HODIR_TARGET_BEACON     = 33108,
    NPC_FREYA_TARGET_BEACON     = 33366,

    DEFENSE_TURRET      = 33142,
    KEEPER_OF_NORGANNON = 33686
};

enum Seats
{
    SEAT_PLAYER = 0,
    SEAT_TURRET = 1,
    SEAT_DEVICE = 2,
    SEAT_CANNON = 7,
};

enum Vehicles
{
    VEHICLE_SIEGE = 33060,
    VEHICLE_CHOPPER = 33062,
    VEHICLE_DEMOLISHER = 33109,
};

enum eAchievementData
{
    ACHIEV_10_ORBITAL_BOMBARDMENT = 2913, // one Tower
    ACHIEV_25_ORBITAL_BOMBARDMENT = 2918,
    ACHIEV_10_ORBITAL_DEVASTATION = 2914, // two Towers
    ACHIEV_25_ORBITAL_DEVASTATION = 2916,
    ACHIEV_10_NUKED_FROM_ORBIT = 2915,    // three Towers
    ACHIEV_25_NUKED_FROM_ORBIT = 2917,
    ACHIEV_10_ORBIT_UARY = 3056,
    ACHIEV_25_ORBIT_UARY = 3057,
/* Nicht implementiert
    ACHIEV_10_SHUTOUT = 2911,
    ACHIEV_25_SHUTOUT = 2912,
    ACHIEV_10_THREE_CAR_GARAGE = 2907,
    ACHIEV_25_THREE_CAR_GARAGE = 2908,
    ACHIEV_10_UNBROKEN = 2905,
    ACHIEV_25_UNBROKEN = 2906,*/ 
};

struct Positions
{
    float x,y,z,o;
};
static Positions Center[]=
{
    {354.8771f, -12.90240f, 409.803650f, 0.0f},
};

const Positions PosSiege[5] =
{
    {-814.59f,-64.54f,429.92f,5.969f},
    {-784.37f,-33.31f,429.92f,5.096f},
    {-808.99f,-52.10f,429.92f,5.668f},
    {-798.59f,-44.00f,429.92f,5.663f},
    {-812.83f,-77.71f,429.92f,0.046f},
};

const Positions PosChopper[5] =
{
    {-717.83f,-106.56f,430.02f,0.122f},
    {-717.83f,-114.23f,430.44f,0.122f},
    {-717.83f,-109.70f,430.22f,0.122f},
    {-718.45f,-118.24f,430.26f,0.052f},
    {-718.45f,-123.58f,430.41f,0.085f},
};

const Positions PosDemolisher[5] =
{
    {-724.12f,-176.64f,430.03f,2.543f},
    {-766.70f,-225.03f,430.50f,1.710f},
    {-729.54f,-186.26f,430.12f,1.902f},
    {-756.01f,-219.23f,430.50f,2.369f},
    {-798.01f,-227.24f,429.84f,1.446f},
};

const float WayMimironBeacon[4][3] = 
{
    {161.45f, -37.50f, 409.80f},
    {275.14f,  64.12f, 409.80f},
    {349.67f, -31.71f, 409.80f},
    {246.72f,-129.38f, 409.80f},
};


struct MANGOS_DLL_DECL boss_flame_leviathan : public ScriptedAI
{
    boss_flame_leviathan(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiBatteringRamTimer;
    uint32 m_uiFlameVentsTimer;
    uint32 m_uiMissileBarrageTimer;
    uint32 m_uiPursueTimer;
    uint32 m_uiGatheringSpeedTimer;
    uint32 m_uiSummonFlyerTimer;
    uint8 maxFlyers;

    uint8 uiActiveTowers;
    uint8 uiShutdown;

    bool isHardMode;
    bool isHodirsTower;
    bool isFreyasTower;
    bool isMimironsTower;
    bool isThorimsTower;

    uint32 m_uiFreyaWardTimer;
    uint32 m_uiMimironInfernoTimer;
    uint32 m_uiHodirFuryTimer;
    uint32 m_uiThorimHammerTimer;

    void Reset()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEVIATHAN, NOT_STARTED); 

        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        m_creature->ApplySpellImmune(49560, 0, 0, true);
        
        m_creature->RemoveAurasDueToSpell(SPELL_TOWER_OF_FROST);
        m_creature->RemoveAurasDueToSpell(SPELL_TOWER_OF_STORMS);
        m_creature->RemoveAurasDueToSpell(SPELL_TOWER_OF_FLAMES);
        m_creature->RemoveAurasDueToSpell(SPELL_TOWER_OF_LIFE);

        m_uiBatteringRamTimer   = 15000 + rand()%20000;
        m_uiFlameVentsTimer     = 15000 + rand()%10000;
        m_uiMissileBarrageTimer = 1000;
        m_uiPursueTimer         = 0;
        m_uiGatheringSpeedTimer = 50000;
        m_uiSummonFlyerTimer    = 2000;
        maxFlyers = 10;

        uiActiveTowers = 0;

        isHardMode      = false;
        isHodirsTower   = false;
        isFreyasTower   = false;
        isMimironsTower = false;
        isThorimsTower  = false;

        m_uiFreyaWardTimer      = urand(1000, 10000);
        m_uiMimironInfernoTimer = urand(1000, 10000);
        m_uiHodirFuryTimer      = urand(1000, 10000);
        m_uiThorimHammerTimer   = urand(1000, 10000);

        m_creature->SetSpeedRate(MOVE_RUN, 0.3f);
    }

    void StartFreyaEvent()//summon these 4 on each corner wich wil spawn additional hostile mobs
    {
        if (Creature* pFreayaBeacon = m_creature->SummonCreature(MOB_FREYA_BEACON, 377.02f, -119.10f, 409.81f, 0.0f ,TEMPSUMMON_MANUAL_DESPAWN, 0))
            DoCast(pFreayaBeacon,AURA_DUMMY_GREEN, true);
        if (Creature* pFreayaBeacon = m_creature->SummonCreature(MOB_FREYA_BEACON, 377.02f, 54.78f, 409.81f, 0.0f ,TEMPSUMMON_MANUAL_DESPAWN, 0))
            DoCast(pFreayaBeacon,AURA_DUMMY_GREEN, true);
        if (Creature* pFreayaBeacon = m_creature->SummonCreature(MOB_FREYA_BEACON, 185.62f, 54.78f, 409.81f, 0.0f ,TEMPSUMMON_MANUAL_DESPAWN, 0))
            DoCast(pFreayaBeacon,AURA_DUMMY_GREEN, true);
        if (Creature* pFreayaBeacon = m_creature->SummonCreature(MOB_FREYA_BEACON, 185.62f, -119.10f, 409.81f, 0.0f ,TEMPSUMMON_MANUAL_DESPAWN, 0))
            DoCast(pFreayaBeacon,AURA_DUMMY_GREEN, true);
    }

    void Aggro(Unit *who) 
    {
        CheckForTowers();
        if(m_pInstance) 
        {
            m_pInstance->SetData(TYPE_LEVIATHAN, IN_PROGRESS);
            if(m_pInstance->GetData(TYPE_LEVIATHAN_TP) != DONE)
                m_pInstance->SetData(TYPE_LEVIATHAN_TP, DONE);
        }

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit *killer)
    {
        if(m_pInstance) 
        {
            m_pInstance->SetData(TYPE_LEVIATHAN, DONE);
            if(isHardMode)
                m_pInstance->SetData(TYPE_LEVIATHAN_HARD, DONE);
            
            if (uiActiveTowers)
            {
                switch (uiActiveTowers)
                {
                    case 4:
                        m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_10_ORBIT_UARY : ACHIEV_25_ORBIT_UARY);
                    case 3:
                        m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_10_NUKED_FROM_ORBIT : ACHIEV_25_NUKED_FROM_ORBIT);
                    case 2:
                        m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_10_ORBITAL_DEVASTATION : ACHIEV_25_ORBITAL_DEVASTATION);
                    case 1:
                        m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_10_ORBITAL_BOMBARDMENT : ACHIEV_25_ORBITAL_BOMBARDMENT);
                }
            }
        }

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LEVIATHAN, FAIL);
    }

    void KilledUnit(Unit *who)
    {
        DoScriptText(SAY_SLAY, m_creature);
    }

    // TODO: effect 0 and effect 1 may be on different target
    void SpellHitTarget(Unit *pTarget, const SpellEntry *spell)
    {
        if (spell->Id == SPELL_PURSUED)
            AttackStart(pTarget);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        /*if(spell->Id == 62472)
        vehicle->InstallAllAccessories();
        else if(spell->Id == SPELL_ELECTROSHOCK)
        m_creature->InterruptSpell(CURRENT_CHANNELED_SPELL);*/
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        uiDamage *= 4;
        if(m_creature->HasAura(SPELL_SYSTEMS_SHUTDOWN, EFFECT_INDEX_0))
            uiDamage += uiDamage/2;
    }

    bool CollossusDead()
    {
        std::list<Creature* > lCreatureList;
        GetCreatureListWithEntryInGrid(lCreatureList, m_creature, 33237, 100.0f);
        if (!lCreatureList.empty())
            for(std::list<Creature*>::iterator itr = lCreatureList.begin(); itr != lCreatureList.end(); ++itr)
                if ((*itr)->isAlive())
                    return false;

        return true;
    }

    void CheckForTowers()
    {
        if (!isHodirsTower)
        {
            if (GameObject* pTower = m_pInstance->GetSingleGameObjectFromStorage(GO_TOWER_OF_FROST))
                if (pTower->GetHealth())
                {
                    isHodirsTower = true;
                    m_creature->CastSpell(m_creature, SPELL_TOWER_OF_FROST,true);
                    isHardMode = true;
                    uiActiveTowers++;
                }
        }
        if (!isFreyasTower)
        {
            if (GameObject* pTower = m_pInstance->GetSingleGameObjectFromStorage(GO_TOWER_OF_LIFE))
                if (pTower->GetHealth())
                {
                    isFreyasTower = true;
                    m_creature->CastSpell(m_creature, SPELL_TOWER_OF_LIFE,true);
                    isHardMode = true;
                    uiActiveTowers++;
                }
        }
        if (!isMimironsTower)
        {
            if (GameObject* pTower = m_pInstance->GetSingleGameObjectFromStorage(GO_TOWER_OF_FLAME))
                if (pTower->GetHealth())
                {
                    isMimironsTower = true;
                    m_creature->CastSpell(m_creature, SPELL_TOWER_OF_FLAMES,true);
                    isHardMode = true;
                    uiActiveTowers++;
                }
        }
        if (!isThorimsTower)
        {
            if (GameObject* pTower = m_pInstance->GetSingleGameObjectFromStorage(GO_TOWER_OF_STORMS))
                if (pTower->GetHealth())
                {
                    isThorimsTower = true;
                    m_creature->CastSpell(m_creature, SPELL_TOWER_OF_STORMS,true);
                    isHardMode = true;
                    uiActiveTowers++;
                }
        }
        m_creature->SetHealth(m_creature->GetMaxHealth());
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if(m_pInstance && (m_pInstance->GetData(TYPE_LEVIATHAN) == NOT_STARTED || m_pInstance->GetData(TYPE_LEVIATHAN) == FAIL))
            if (CollossusDead())
            {
                m_creature->GetMotionMaster()->MovePoint(1,342.896f , -14.113f, 409.804f);
                m_pInstance->SetData(TYPE_LEVIATHAN, SPECIAL);
            }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // pursue
        if(m_uiPursueTimer < uiDiff)
        {
            switch(urand(0, 3))
            {
                case 0: DoScriptText(SAY_CHANGE1, m_creature); break;
                case 1: DoScriptText(SAY_CHANGE2, m_creature); break;
                case 2: DoScriptText(SAY_CHANGE3, m_creature); break;
            }
            DoScriptText(EMOTE_PURSUE, m_creature);
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->AddThreat(pTarget, 100.0f);
                DoCast(pTarget, SPELL_PURSUED);
            }

            m_uiPursueTimer = 30000;
        }
        else m_uiPursueTimer -= uiDiff;

        // flame vents
        if(m_uiFlameVentsTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_FLAME_VENTS);
            m_uiFlameVentsTimer = 30000 + rand()%20000;
        }
        else m_uiFlameVentsTimer -= uiDiff;

        // battering ram
        if(m_uiBatteringRamTimer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_BATTERING_RAM);
            m_uiBatteringRamTimer = 25000 + rand()%15000;
        }
        else m_uiBatteringRamTimer -= uiDiff;

        /* flyers
        it should summon some flyers. needs more research!
        if(m_uiSummonFlyerTimer < uiDiff)
        {
            m_creature->SummonCreature(MOB_MECHANOLIFT, m_creature->GetPositionX() + rand()%20, m_creature->GetPositionY() + rand()%20, m_creature->GetPositionZ() + 50, 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 40000);
            m_uiSummonFlyerTimer = 2000;
        }
        else m_uiSummonFlyerTimer -= uiDiff;*/

        // missile barrage
        if(m_uiMissileBarrageTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_MISSILE_BARRAGE);
            m_uiMissileBarrageTimer = 3000 + rand()%2000;
        }
        else m_uiMissileBarrageTimer -= uiDiff;

        if(m_uiGatheringSpeedTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_GATHERING_SPEED);
            m_uiGatheringSpeedTimer = urand(50000, 60000);
        }
        else m_uiGatheringSpeedTimer -= uiDiff;

        // Hard mode event. need more research and scripting
        // this part should be done in other way

        // tower of freya
        if(isFreyasTower && m_uiFreyaWardTimer)
        {
            if(m_uiFreyaWardTimer < uiDiff)
            {
                StartFreyaEvent();   
                m_uiFreyaWardTimer = 0;                
            }
            else m_uiFreyaWardTimer -= uiDiff;
        }

        // tower of mimiron
        if(isMimironsTower && m_uiMimironInfernoTimer)
        {
            if(m_uiMimironInfernoTimer < uiDiff)
            {
                if (Creature* pMimironBeacon = m_creature->SummonCreature(MOB_MIMIRON_BEACON, WayMimironBeacon[0][0], WayMimironBeacon[0][1], WayMimironBeacon[0][2],0, TEMPSUMMON_MANUAL_DESPAWN, 0))
                {
                    DoCast(pMimironBeacon, AURA_DUMMY_YELLOW, true);
                    m_uiMimironInfernoTimer = 0;
                }
            }        
            else m_uiMimironInfernoTimer -= uiDiff;
        }

        // tower of hodir
        if(isHodirsTower && m_uiHodirFuryTimer)
        {
            if(m_uiHodirFuryTimer < uiDiff)
            {
                for (uint8 i = 0; i < 3; ++i)
                {
                    if (Creature* pHodir = DoSpawnCreature(MOB_HODIR_BEACON, 0, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        DoCast(pHodir, AURA_DUMMY_BLUE, true);
                }
                m_uiHodirFuryTimer = 0;
            }
            else m_uiHodirFuryTimer -= uiDiff;
        }

        // tower of thorim
        if(isThorimsTower && m_uiThorimHammerTimer)
        {
            if(m_uiThorimHammerTimer < uiDiff)
            {
                for (uint8 i = 0; i < 3; ++i)
                {
                    if (Creature* pThorim = DoSpawnCreature(MOB_THORIM_BEACON, 0, 0, 0, 0, TEMPSUMMON_MANUAL_DESPAWN, 0))
                        DoCast(pThorim, AURA_DUMMY_BLUE, true);
                }
                m_uiThorimHammerTimer = 0;
            }
            else m_uiThorimHammerTimer -= uiDiff;
        }
        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_defense_turretAI : public ScriptedAI
{
    mob_defense_turretAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(10000, 15000);
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if(spell->Id == SPELL_SYSTEMS_SHUTDOWN)
            m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            DoCast(m_creature, SPELL_SEARING_FLAME);
            m_uiSpell_Timer = urand(10000, 15000);
        }else m_uiSpell_Timer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_pool_of_tarAI : public ScriptedAI
{
    mob_pool_of_tarAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
        DoCast(m_creature, SPELL_TAR_PASSIV, true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        SetCombatMovement(false);        
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if(spell->SchoolMask & SPELL_SCHOOL_MASK_FIRE && !m_creature->HasAura(SPELL_BLAZE))
            DoCast(m_creature,SPELL_BLAZE,true);
    }
    void DamageTaken(Unit * killer, uint32 &uidamage)
    {
        uidamage = 0;
    }
};

struct MANGOS_DLL_DECL mob_mechanoliftAI : public ScriptedAI
{
    mob_mechanoliftAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
    }

    void JustDied(Unit* pKiller)
    {
        if (Creature* pLiquid = DoSpawnCreature(MOB_LIQUID,0,0,0,0,TEMPSUMMON_TIMED_DESPAWN, 30000))
        {
            pLiquid->CastSpell(pLiquid, SPELL_LIQUID_PYRITE, true);
        }

    }
};

struct MANGOS_DLL_DECL mob_freyas_wardAI : public ScriptedAI
{
    mob_freyas_wardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint32 summonTimer ;

    void Reset()
    {
        summonTimer = 5000 ;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(summonTimer <= uiDiff)
        {
            DoCast(m_creature,SPELL_FREYA_WARD, true);
            summonTimer = 20000;
        }
        else
            summonTimer -= uiDiff ;

        if (!m_creature->HasAura(AURA_DUMMY_GREEN, EFFECT_INDEX_1))
            DoCast(m_creature, AURA_DUMMY_GREEN,true);

        if (m_pInstance->GetData(TYPE_LEVIATHAN) != IN_PROGRESS)
        {
            m_creature->ForcedDespawn();
        }
    }
};

struct MANGOS_DLL_DECL mob_hodirs_furyAI : public ScriptedAI
{
    mob_hodirs_furyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint32 m_uiHodirFuryTimer;
    uint32 m_uiSwitchTargetTimer;                       //hack for RandomMovement

    bool m_bHodirFuryReady;

    void Reset()
    {
        m_uiHodirFuryTimer = 0;
        m_bHodirFuryReady = true;
        
        m_uiSwitchTargetTimer = 30000;
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER && m_creature->IsInRange(who,0,5,false) && m_bHodirFuryReady)
        {
            if (Creature* pTrigger = DoSpawnCreature(NPC_HODIR_TARGET_BEACON, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 1000))
                pTrigger->CastSpell(who, SPELL_HODIR_FURY, true);
            m_uiHodirFuryTimer = 4000;
            m_bHodirFuryReady = false;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->HasAura(AURA_DUMMY_BLUE, EFFECT_INDEX_1))
            DoCast(m_creature, AURA_DUMMY_BLUE,true);
        if (m_pInstance->GetData(TYPE_LEVIATHAN) != IN_PROGRESS)
        {
            m_creature->ForcedDespawn();
        }

        if (!m_bHodirFuryReady)
        {
            if(m_uiHodirFuryTimer <= uiDiff)
            {
                m_bHodirFuryReady = true;
            }
            else
                m_uiHodirFuryTimer -= uiDiff;
        }

        
        if(m_uiSwitchTargetTimer <= uiDiff)
        {
            SwitchTarget();
        }
        else
            m_uiSwitchTargetTimer -= uiDiff;
    }

    void SwitchTarget()
    {
        if (m_pInstance)
        {
            if (Player* target = m_pInstance->GetPlayerInMap(true, true))
                m_creature->GetMotionMaster()->MoveChase(target);
        }
    }
};

struct MANGOS_DLL_DECL mob_mimirons_infernoAI : public ScriptedAI
{
    mob_mimirons_infernoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;
    uint32 infernoTimer;
    uint32 m_uiWalkTimer;
    uint8 waypointId;

    void Reset()
    {
        waypointId = 0;
        m_uiWalkTimer = 200;
        infernoTimer = 4000;
        m_creature->GetMotionMaster()->MovePoint(waypointId,WayMimironBeacon[waypointId][0],WayMimironBeacon[waypointId][1],WayMimironBeacon[waypointId][2]);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        ++waypointId;
        m_uiWalkTimer = 200;
        if (waypointId > 3)
            waypointId = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiWalkTimer)
        {
            if (m_uiWalkTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->MovePoint(waypointId,WayMimironBeacon[waypointId][0],WayMimironBeacon[waypointId][1],WayMimironBeacon[waypointId][2]);
                m_uiWalkTimer = 0;
            }else m_uiWalkTimer -= uiDiff;
        }
        
        if(infernoTimer <= uiDiff)
        {
            if (Creature* pTrigger = DoSpawnCreature(NPC_MIMIRON_TARGET_BEACON, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN, 30000))
            {
                pTrigger->CastSpell(pTrigger, SPELL_MIMIRON_INFERNO, true);
                infernoTimer = 4000;
            }
        }
        else
            infernoTimer -= uiDiff;

        if (!m_creature->HasAura(AURA_DUMMY_YELLOW, EFFECT_INDEX_1))
            DoCast(m_creature, AURA_DUMMY_YELLOW,true);
        if (m_pInstance->GetData(TYPE_LEVIATHAN) != IN_PROGRESS)
        {
            m_creature->ForcedDespawn();
        }
    }
};

struct MANGOS_DLL_DECL mob_thorims_hammerAI : public ScriptedAI
{
    mob_thorims_hammerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;
    uint32 m_uiHammerTimer;
    uint32 m_uiSwitchTargetTimer;                       //hack for RandomMovement

    bool m_bHammerReady;

    void Reset()
    {
        m_uiHammerTimer = 0;
        m_bHammerReady = true;
        
        m_uiSwitchTargetTimer = 30000;
    }

    void MoveInLineOfSight(Unit* who)
    {
        if (who->GetTypeId() == TYPEID_PLAYER && m_creature->IsInRange(who,0,10,false) && m_bHammerReady)
        {
            if (Creature* pTrigger = DoSpawnCreature(NPC_THORIM_TARGET_BEACON,0 ,0 ,0 ,0 , TEMPSUMMON_TIMED_DESPAWN, 1000))
                    pTrigger->CastSpell(who, SPELL_THORIMS_HAMMER, true);
            m_bHammerReady = false;
            m_uiHammerTimer = 4000;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bHammerReady)
        {
            if(m_uiHammerTimer <= uiDiff)
            {
                m_bHammerReady = true;
            }
            else
                m_uiHammerTimer -= uiDiff;
        }

        if(m_uiSwitchTargetTimer <= uiDiff)
        {
            SwitchTarget();
        }
        else
            m_uiSwitchTargetTimer -= uiDiff;


        if (!m_creature->HasAura(AURA_DUMMY_BLUE, EFFECT_INDEX_1))
            DoCast(m_creature, AURA_DUMMY_BLUE,true);
        if (m_pInstance->GetData(TYPE_LEVIATHAN) != IN_PROGRESS)
        {
            m_creature->ForcedDespawn();
        }
    }

    void SwitchTarget()
    {
        if (m_pInstance)
        {
            if (Player* target = m_pInstance->GetPlayerInMap(true, true))
                m_creature->GetMotionMaster()->MoveChase(target);
        }
    }
};

CreatureAI* GetAI_mob_defense_turret(Creature* pCreature)
{
    return new mob_defense_turretAI(pCreature);
}

CreatureAI* GetAI_boss_flame_leviathan(Creature* pCreature)
{
    return new boss_flame_leviathan(pCreature);
}

CreatureAI* GetAI_mob_pool_of_tar(Creature* pCreature)
{
    return new mob_pool_of_tarAI(pCreature);
}

CreatureAI* GetAI_mob_mechanolift(Creature* pCreature)
{
    return new mob_mechanoliftAI(pCreature);
}

CreatureAI* GetAI_mob_freyas_ward(Creature* pCreature)
{
    return new mob_freyas_wardAI(pCreature);
}

CreatureAI* GetAI_mob_hodirs_fury(Creature* pCreature)
{
    return new mob_hodirs_furyAI(pCreature);
}

CreatureAI* GetAI_mob_mimirons_inferno(Creature* pCreature)
{
    return new mob_mimirons_infernoAI(pCreature);
}

CreatureAI* GetAI_mob_thorims_hammer(Creature* pCreature)
{
    return new mob_thorims_hammerAI(pCreature);
}

bool GossipHello_mob_lorekeeper(Player *player, Creature *pCreature)
{
    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "Gib mir Macht mit einen Zerstörer."            , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "Gib mir Stärke mit einer Belagerungsmaschine." , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
    player->ADD_GOSSIP_ITEM( GOSSIP_ICON_CHAT, "Gib mir Geschwindigkeit mit einen Moped."      , GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_mob_lorekeeper(Player *pPlayer, Creature *pCreature, uint32 sender, uint32 uiAction )
{
    uint32 i = urand(0,4);
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->SummonCreature(VEHICLE_DEMOLISHER, PosSiege[i].x, PosSiege[i].y, PosSiege[i].z, PosSiege[i].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 2)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->SummonCreature(VEHICLE_SIEGE, PosDemolisher[i].x, PosDemolisher[i].y, PosDemolisher[i].z, PosDemolisher[i].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);
    }
    if (uiAction == GOSSIP_ACTION_INFO_DEF + 3)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->SummonCreature(VEHICLE_CHOPPER, PosChopper[i].x, PosChopper[i].y, PosChopper[i].z, PosChopper[i].o, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 3000);

    }
    return true;
}

void AddSC_boss_leviathan()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_flame_leviathan";
    newscript->GetAI = &GetAI_boss_flame_leviathan;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_defense_turret";
    newscript->GetAI = &GetAI_mob_defense_turret;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_pool_of_tar";
    newscript->GetAI = &GetAI_mob_pool_of_tar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mechanolift";
    newscript->GetAI = &GetAI_mob_mechanolift;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_freyas_ward";
    newscript->GetAI = &GetAI_mob_freyas_ward;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_hodirs_fury";
    newscript->GetAI = &GetAI_mob_hodirs_fury;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_mimiron_inferno";
    newscript->GetAI = &GetAI_mob_mimirons_inferno;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_thorims_hammer";
    newscript->GetAI = &GetAI_mob_thorims_hammer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_lorekeeper";
    newscript->pGossipHello = &GossipHello_mob_lorekeeper;
    newscript->pGossipSelect = &GossipSelect_mob_lorekeeper;
    newscript->RegisterSelf();
}
