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
SDName: Boss_Malygos
SD%Complete: 98%
SDComment: Hacks, hacks, hacks...
SDAuthor: Tasssadar + PSZ + some corrections from MaxXx2021 and /dev/rsa
SDCategory: Nexus, Eye of Eternity
EndScriptData */

#include "precompiled.h"
#include "eye_of_eternity.h"
#include "Vehicle.h"

enum
{
    // ******************************** SPELLS ******************************** //
    SPELL_BERSERK                   = 64238,

    //////////////// PHASE 0 ////////////////
    SPELL_PORTAL_BEAM               = 56046,

    //////////////// PHASE 1 ////////////////
    SPELL_ARCANE_BREATH             = 56272,
    SPELL_ARCANE_BREATH_H           = 60072,
    SPELL_ARCANE_STORM              = 61693,
    SPELL_ARCANE_STORM_H            = 61694,
    SPELL_VORTEX                    = 56105,
    SPELL_VORTEX_DMG_AURA           = 56266, // on 10 sec, deal 2000 damage all player around caster
    SPELL_VORTEX_VISUAL             = 55873, // visual effect around platform. summon trigger
    SPELL_VORTEX_CHANNEL            = 56237, // Malygos Channel Effect
    SPELL_POWER_SPARK               = 56152, // if spark reach malygos then buff him
    SPELL_POWER_SPARK_PLAYERS       = 55852, // if spark is killed - buff players in area
    SPELL_POWER_SPARK_VISUAL        = 55845,

    //////////////// PHASE 2 ////////////////
    SPELL_ARCANE_BOMB_MISSILE       = 56430,
    SPELL_ARCANE_BOMB_DAMAGE        = 56431, // cast by arcane overload
    SPELL_ARCANE_OVERLOAD           = 56432, // cast this on arcane overload NPCs
    SPELL_SURGE_OF_POWER_BREATH     = 56505, // omfg, they say deep breath, but its this!
    SPELL_DESTROY_PLATFORM_PRE      = 58842, // lights all over the platform
    SPELL_DESTROY_PLATFORM_BOOM     = 59084, // Big Blue boom
    //NPCs spells
    SPELL_ARCANE_SHOCK              = 57058,
    SPELL_ARCANE_SHOCK_H            = 60073,
    SPELL_HASTE                     = 57060,
    SPELL_ARCANE_BARRAGE            = 50804, // hack, right spell is 56397

    //////////////// PHASE 3 ////////////////
    SPELL_STATIC_FIELD_MISSILE      = 57430,
    SPELL_STATIC_FIELD              = 57428,
    SPELL_SURGE_OF_POWER            = 57407, // this is on one target
    SPELL_SURGE_OF_POWER_H          = 60936, // this is on unlimited tagets, must limit it in mangos
    SPELL_ARCANE_PULSE              = 57432,

    SPELL_ALEXSTRASZAS_GIFT_BEAM    = 61028,
    SPELL_ALEXSTRASZAS_GIFT_VISUAL  = 61023,

    // ******************************** Items, NPCs & GameObjects ******************************** //

    ITEM_KEY_TO_FOCUSING_IRIS       = 44582,
    ITEM_KEY_TO_FOCUSING_IRIS_H     = 44581,

    //////////////// PHASE 1 ////////////////
    NPC_VORTEX                      = 30090,
    NPC_POWER_SPARK                 = 30084,
    NPC_SPARK_PORTAL                = 30118, // for power sparks
    VEHICLE_VORTEX                  = 168,

    //////////////// PHASE 2 ////////////////
    NPC_HOVER_DISK                  = 30248,
    NPC_NEXUS_LORD                  = 30245, // 2 (normal) and 4 (heroic) of them are spawned on beginning of phase 2
    NPC_SCION_OF_ETERNITY           = 30249, // 4 (normal) and 8 (heroic) of them are spawned on beginning of phase 2
    NPC_ARCANE_OVERLOAD             = 30282,
    NPC_SURGE_OF_POWER              = 30334,

    //////////////// PHASE 3 ////////////////
    NPC_STATIC_FIELD                = 30592, // trigger for that spell
    NPC_WYRMREST_SKYTALON           = 30161, // dragon vehicle in 3-d Phase
    NPC_ALEXSTRASZA                 = 32295,
    NPC_ALEXSTRASZAS_GIFT           = 32448,

    SPELL_CALL_SKYTALON             = 58846,
    SPELL_CALL_SKYTALON_0           = 56070,
    SPELL_VEHICLE_HARDCODED         = 46598,

    SAY_INTRO1                      = -1616000,
    SAY_INTRO2                      = -1616001,
    SAY_INTRO3                      = -1616002,
    SAY_INTRO4                      = -1616003,
    SAY_INTRO5                      = -1616004,
    SAY_INTRO_PHASE3                = -1616018,
    SAY_AGGRO1                      = -1616005,
    SAY_AGGRO2                      = -1616013,
    SAY_AGGRO3                      = -1616019,
    SAY_VORTEX                      = -1616006,
    SAY_POWER_SPARK                 = -1616035,
    SAY_POWER_SPARK_BUFF            = -1616007,
    SAY_KILL1_1                     = -1616008,
    SAY_KILL1_2                     = -1616009,
    SAY_KILL1_3                     = -1616010,
    SAY_KILL2_1                     = -1616020,
    SAY_KILL2_2                     = -1616021,
    SAY_KILL2_3                     = -1616022,
    SAY_KILL3_1                     = -1616023,
    SAY_KILL3_2                     = -1616024,
    SAY_KILL3_3                     = -1616025,
    SAY_END_PHASE1                  = -1616012,
    SAY_END_PHASE2                  = -1616017,
    SAY_ARCANE_PULSE                = -1616014,
    SAY_ARCANE_PULSE_WARN           = -1616015,
    SAY_ARCANE_OVERLOAD             = -1616016,
    SAY_SURGE_OF_POWER              = -1616026,
    SAY_CAST_SPELL1                 = -1616027,
    SAY_CAST_SPELL2                 = -1616028,
    SAY_CAST_SPELL3                 = -1616029,
    SAY_DEATH                       = -1616030,
    // Alexstrasza
    SAY_OUTRO1                      = -1616031,
    SAY_OUTRO2                      = -1616032,
    SAY_OUTRO3                      = -1616033,
    SAY_OUTRO4                      = -1616034,

    PLATFORM_MIN_X                  = 722,
    PLATFORM_MAX_X                  = 768,
    PLATFORM_MIN_Y                  = 1290,
    PLATFORM_MAX_Y                  = 1339,

    NEXUS_LORD_COUNT                = 2,
    NEXUS_LORD_COUNT_H              = 4,
    SCION_OF_ETERNITY_COUNT         = 4,
    SCION_OF_ETERNITY_COUNT_H       = 8,

    PHASE_INTRO                     = 0,
    PHASE_FLOOR                     = 1,
        SUBPHASE_VORTEX             = 11,
    PHASE_ADDS                      = 2,
        SUBPHASE_TALK               = 21,
        SUBPHASE_DEEP_BREATH        = 22,
    PHASE_DRAGONS                   = 3,
        SUBPHASE_DESTROY_PLATFORM_1 = 31,
        SUBPHASE_DESTROY_PLATFORM_2 = 32,
        SUBPHASE_DESTROY_PLATFORM_3 = 33,
        SUBPHASE_DESTROY_PLATFORM_4 = 34,
        SUBPHASE_SURGE_OF_POWER     = 35,

    POINT_ID_WAYPOINT               = 1,
    POINT_ID_LAND                   = 2,
    POINT_ID_VORTEX_AIR             = 3,
    POINT_ID_PHASE_2_WP             = 4,
    POINT_ID_DEEP_BREATH            = 5,

    //hacks
    SPELL_FLIGHT                    = 59553,
    MODEL_ID_INVISIBLE              = 11686
};

struct LocationsXY
{
    float x, y;
};

static LocationsXY WPs[]=
{
    {810.0f, 1248.0f},
    {810.0f, 1358.0f},
    {700.0f, 1358.0f},
    {700.0f, 1248.0f},
};

//Also spawn locations for scions of eternity
static LocationsXY VortexLoc[]=
{
    {754, 1311},
    {739, 1325},
    {755, 1330},
    {774, 1321},
    {781, 1308},
    {779, 1288},
    {764, 1274},
    {744, 1274},
    {730, 1286},
    {726, 1299},
    {728, 1312},
    {739, 1325},
    {755, 1330},
    {774, 1321},
    {781, 1308},
    {779, 1288},
    {764, 1274},
    {744, 1274},
    {730, 1286},
    {726, 1299},
    {728, 1312},
};

#define MAX_VORTEX              21

#define CENTER_X                754.50f
#define CENTER_Y                1301.76f
#define FLOOR_Z                 266.17f
#define AIR_Z                   296.17f
#define VORTEX_FARSIGHT_X       749.00f
#define VORTEX_FARSIGHT_Y       1244.00f
#define VORTEX_FARSIGHT_Z       332.00f
#define VORTEX_FARSIGHT_O       1.54f

/*######
## boss_malygos
######*/

struct MANGOS_DLL_DECL boss_malygosAI : public ScriptedAI
{
    boss_malygosAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint8 m_uiPhase;
    uint8 m_uiSubPhase;
    uint8 m_uiSpeechCount;
    uint32 m_uiSpeechTimer;
    uint32 m_uiPortalNeedResTimer;
    uint8 m_uiVortexPhase;
    std::list<ObjectGuid> m_lSparkPortalGUIDList;

    uint32 m_uiTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiVortexTimer;
    uint32 m_uiArcaneBreathTimer;
    uint32 m_uiPowerSparkTimer;
    uint32 m_uiDeepBreathTimer;
    uint32 m_uiShellTimer;
    uint32 m_uiArcaneStormTimer;
    uint32 m_uiStaticFieldTimer;
    uint32 m_uiArcanePulseTimer;
    uint32 m_uiSurgeOfPowerTimer;
    uint32 m_uiCheckTimer;
    uint32 m_uiMovingSteps;

    ObjectGuid m_uiTargetSparkPortalGUID;
    uint8 m_uiWP;

    bool m_bReadyForWPMove;
    bool m_bPortalNeedRes;

    float m_fAngle;

    void Reset()
    {
        m_fAngle = 0.0f;
        m_uiMovingSteps = 0;
        m_bReadyForWPMove = true;
        m_bPortalNeedRes = false;
        m_uiPortalNeedResTimer = 0;
        m_uiPhase = PHASE_INTRO;
        m_uiSubPhase = 0;
        m_uiVortexPhase = 0;
        m_uiTimer = 0;
        m_uiEnrageTimer = 600000;
        m_uiArcaneBreathTimer = urand(13000, 16000);
        m_uiArcaneStormTimer = urand(10000, 15000);
        m_uiVortexTimer = 60000;
        m_uiPowerSparkTimer = urand(20000, 30000);
        m_uiDeepBreathTimer = 65000;
        m_uiShellTimer = 0;
        m_uiStaticFieldTimer = 10000;
        m_uiArcanePulseTimer = 1000;
        m_uiSurgeOfPowerTimer = 8000;
        m_uiCheckTimer = 1000;

        m_uiWP = 0;
        m_uiSpeechCount = 0;
        m_uiSpeechTimer = 15000;
        m_creature->SetSpeedRate(MOVE_RUN, 2.85714f);
        m_creature->SetSpeedRate(MOVE_WALK, 6.0f);
        m_creature->SetSpeedRate(MOVE_FLIGHT, 2.0f);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
        m_creature->SetLevitate(true);
        m_creature->GetMotionMaster()->Clear();

        DespawnCreatures(NPC_POWER_SPARK);
        DespawnCreatures(NPC_ARCANE_OVERLOAD);
        DespawnCreatures(NPC_NEXUS_LORD);
        DespawnCreatures(NPC_SCION_OF_ETERNITY);
        DespawnCreatures(NPC_HOVER_DISK);
        DespawnCreatures(NPC_STATIC_FIELD);

        if (!m_lSparkPortalGUIDList.empty())
            for (std::list<ObjectGuid>::iterator itr = m_lSparkPortalGUIDList.begin(); itr != m_lSparkPortalGUIDList.end(); ++itr)
                if (Creature* pSparkPortal = m_creature->GetMap()->GetCreature(*itr))
                    pSparkPortal->Respawn();

        m_lSparkPortalGUIDList.clear();

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO1, m_creature);
        std::list<Creature*> lSparkPortalList;
        GetCreatureListWithEntryInGrid(lSparkPortalList, m_creature, NPC_SPARK_PORTAL, 200.0f);
        if (!lSparkPortalList.empty())
            for (std::list<Creature*>::iterator itr = lSparkPortalList.begin(); itr != lSparkPortalList.end(); ++itr)
                if (*itr)
                    m_lSparkPortalGUIDList.push_back((*itr)->GetObjectGuid());

        if (m_pInstance)
            m_pInstance->SetData(TYPE_MALYGOS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);
        DespawnCreatures(NPC_STATIC_FIELD);
        m_creature->SummonCreature(NPC_ALEXSTRASZA, CENTER_X+20.0f, CENTER_Y+20.0f, AIR_Z, 0, TEMPSUMMON_DEAD_DESPAWN, 0);
        m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z-500.0f, 0);
        m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z-400.0f, 26);
    }

    void KilledUnit(Unit* pVictim)
    {
        uint8 uiText = 0;
        switch (m_uiPhase)
        {
            case PHASE_FLOOR: uiText = urand(0, 2); break;
            case PHASE_ADDS: uiText = urand(3, 5); break;
            case PHASE_DRAGONS: uiText = urand(6, 8); break;
        }
        switch (uiText)
        {
            case 0: DoScriptText(SAY_KILL1_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL1_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL1_3, m_creature); break;
            case 3: DoScriptText(SAY_KILL2_1, m_creature); break;
            case 4: DoScriptText(SAY_KILL2_2, m_creature); break;
            case 5: DoScriptText(SAY_KILL2_3, m_creature); break;
            case 6: DoScriptText(SAY_KILL3_1, m_creature); break;
            case 7: DoScriptText(SAY_KILL3_2, m_creature); break;
            case 8: DoScriptText(SAY_KILL3_3, m_creature); break;
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_POWER_SPARK)
            DoScriptText(SAY_POWER_SPARK_BUFF, m_creature);
    }

    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpell)
    {
        if (pUnit->GetEntry() == NPC_ARCANE_OVERLOAD && pSpell->Id == SPELL_ARCANE_BOMB_MISSILE)
        {
            pUnit->CastSpell(pUnit, SPELL_ARCANE_BOMB_DAMAGE, true);
            pUnit->CastSpell(pUnit, SPELL_ARCANE_OVERLOAD, false, 0, 0, m_creature->GetObjectGuid());
            pUnit->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (!pSummoned)
            return;

        uint32 uiEntry = pSummoned->GetEntry();
        if (uiEntry == NPC_VORTEX || uiEntry == NPC_STATIC_FIELD)
            pSummoned->SetDisplayId(MODEL_ID_INVISIBLE);

        if (uiEntry == NPC_STATIC_FIELD)
        {
            pSummoned->CastSpell(m_creature, SPELL_STATIC_FIELD, false, 0, 0, m_creature->GetObjectGuid());
            pSummoned->ForcedDespawn(30000);
        }

        if (uiEntry == NPC_NEXUS_LORD || uiEntry == NPC_SCION_OF_ETERNITY)
        {
            if (Creature* pDisk = pSummoned->SummonCreature(NPC_HOVER_DISK, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                pDisk->SetLevitate(true);
                pDisk->CastSpell(pDisk, SPELL_FLIGHT, true);
                if (uiEntry == NPC_NEXUS_LORD)
                    pDisk->SetSpeedRate(MOVE_WALK, 1.5f);
                if (VehicleKit* pDiskVehicle = pDisk->GetVehicleKit())
                    pSummoned->EnterVehicle(pDiskVehicle, 0);
            }
            pSummoned->SetInCombatWithZone();
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (!m_creature || !m_creature->getVictim())
            return;

        if (Creature* pDisk = m_creature->getVictim()->SummonCreature(NPC_HOVER_DISK, pSummoned->GetPositionX(), pSummoned->GetPositionY(), pSummoned->GetPositionZ(), 0, TEMPSUMMON_CORPSE_DESPAWN, 0))
        {
            pDisk->setFaction(35);
            pDisk->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
            pDisk->CastSpell(pDisk, SPELL_FLIGHT, true);
            pDisk->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    bool IsThereAnyAdd()
    {
        if (GetClosestCreatureWithEntry(m_creature, NPC_NEXUS_LORD, 200.0f))
            return true;

        if (GetClosestCreatureWithEntry(m_creature, NPC_SCION_OF_ETERNITY, 200.0f))
            return true;

        return false;
    }

    void DespawnCreatures(uint32 uiEntry)
    {
        std::list<Creature*> pCreatures;
        GetCreatureListWithEntryInGrid(pCreatures, m_creature, uiEntry, 200.0f);

        if (pCreatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = pCreatures.begin(); iter != pCreatures.end(); ++iter)
            (*iter)->ForcedDespawn();
    }

    void AntiMagicShell()
    {
        if (Creature* pShell = m_creature->SummonCreature(NPC_ARCANE_OVERLOAD, urand(PLATFORM_MIN_X, PLATFORM_MAX_X), urand(PLATFORM_MIN_Y, PLATFORM_MAX_Y), FLOOR_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 45000))
        {
            pShell->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_creature->CastSpell(pShell, SPELL_ARCANE_BOMB_MISSILE, true);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_ID_WAYPOINT)
        {
            if (Creature* pSparkPortal = GetClosestCreatureWithEntry(m_creature, NPC_SPARK_PORTAL, 100.0f))
            {
                m_creature->SetFacingToObject(pSparkPortal);
                m_creature->CastSpell(pSparkPortal, SPELL_PORTAL_BEAM, true);
                m_uiTargetSparkPortalGUID = pSparkPortal->GetObjectGuid();
            }

            m_bReadyForWPMove = true;
            m_uiTimer = 15000;
        }
        else if (uiPointId == POINT_ID_LAND)
        {
            if (Creature* pTargetSparkPortal = m_creature->GetMap()->GetCreature(m_uiTargetSparkPortalGUID))
                pTargetSparkPortal->Respawn();

            m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
            m_creature->SetLevitate(false);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            m_uiPhase = PHASE_FLOOR;
            m_creature->SetInCombatWithZone();
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(pTarget);
            }
        }
        else if (uiPointId == POINT_ID_VORTEX_AIR)
        {
            if (Creature* pTempVortexVisual = m_creature->SummonCreature(NPC_VORTEX, CENTER_X, CENTER_Y, FLOOR_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000))
            {
                //hacks to make Malygos not to turn during Vortex
                pTempVortexVisual->SetMaxHealth(1000000);
                pTempVortexVisual->SetHealth(1000000);
                pTempVortexVisual->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
                pTempVortexVisual->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                pTempVortexVisual->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                pTempVortexVisual->setFaction(85);
                m_creature->AddThreat(pTempVortexVisual, 1000000000.0f);

                pTempVortexVisual->CastSpell(pTempVortexVisual, SPELL_VORTEX_VISUAL, true);
            }

            ++m_uiVortexPhase;
            m_uiTimer = 0;
        }
        else if (uiPointId == POINT_ID_PHASE_2_WP)
        {
            m_bReadyForWPMove = true;
            if (m_uiSubPhase == SUBPHASE_TALK)
            {
                DoScriptText(SAY_AGGRO2, m_creature);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                AntiMagicShell();
                m_uiShellTimer = urand(15000, 17000);
                m_uiSubPhase = 0;
                for (uint8 i = 0; i < (m_bIsRegularMode ? NEXUS_LORD_COUNT : NEXUS_LORD_COUNT_H); ++i)
                    m_creature->SummonCreature(NPC_NEXUS_LORD, urand(PLATFORM_MIN_X, PLATFORM_MAX_X), urand(PLATFORM_MIN_Y, PLATFORM_MAX_Y), FLOOR_Z+10.0f+urand(0, 15), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                for (uint8 i = 0; i < (m_bIsRegularMode ? SCION_OF_ETERNITY_COUNT : SCION_OF_ETERNITY_COUNT_H); ++i)
                    m_creature->SummonCreature(NPC_SCION_OF_ETERNITY, urand(PLATFORM_MIN_X, PLATFORM_MAX_X), urand(PLATFORM_MIN_Y, PLATFORM_MAX_Y), FLOOR_Z+10.0f+urand(0, 15), 0, TEMPSUMMON_CORPSE_DESPAWN, 0);
                m_uiTimer = 5000;
                return;
            }
            m_uiTimer = 0;
        }
        else if (uiPointId == POINT_ID_DEEP_BREATH)
        {
            DoScriptText(SAY_ARCANE_PULSE, m_creature);
            DoScriptText(SAY_ARCANE_PULSE_WARN, m_creature);

            if (Creature* pTemp = m_creature->SummonCreature(NPC_VORTEX, CENTER_X, CENTER_Y, FLOOR_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 9000))
                m_creature->CastSpell(m_creature, SPELL_SURGE_OF_POWER_BREATH, false, 0, 0, pTemp->GetObjectGuid());

            m_uiShellTimer = urand(2000, 4000);
            m_bReadyForWPMove = true;
            m_uiTimer = 10000;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bPortalNeedRes)
        {
            if (m_uiPortalNeedResTimer <= uiDiff)
            {
                if (Creature* pTargetSparkPortal = m_creature->GetMap()->GetCreature(m_uiTargetSparkPortalGUID))
                    pTargetSparkPortal->Respawn();

                m_bPortalNeedRes = false;
            }
            else
                m_uiPortalNeedResTimer -= uiDiff;
        }

        if (m_uiPhase == PHASE_INTRO && m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != POINT_MOTION_TYPE)
        {
            if (m_bReadyForWPMove)
            {
                if (m_uiTimer <= uiDiff)
                {
                    if (Creature* pTargetSparkPortal = m_creature->GetMap()->GetCreature(m_uiTargetSparkPortalGUID))
                    {
                        pTargetSparkPortal->ForcedDespawn();
                        m_bPortalNeedRes = true;
                        m_uiPortalNeedResTimer = 0;
                    }
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_WAYPOINT, WPs[m_uiWP].x, WPs[m_uiWP].y, AIR_Z);
                    m_bReadyForWPMove = false;
                    ++m_uiWP;
                    if (m_uiWP == 4)
                        m_uiWP = 0;
                }
                else
                    m_uiTimer -= uiDiff;
            }

            if (m_uiSpeechTimer <= uiDiff)
            {
                switch(m_uiSpeechCount)
                {
                    case 0:
                    {
                        DoScriptText(SAY_INTRO1, m_creature);
                        m_uiSpeechTimer = 22000;
                        break;
                    }
                    case 1:
                    {
                        DoScriptText(SAY_INTRO2, m_creature);
                        m_uiSpeechTimer = 24000;
                        break;
                    }
                    case 2:
                    {
                        DoScriptText(SAY_INTRO3, m_creature);
                        m_uiSpeechTimer = 26000;
                        break;
                    }
                    case 3:
                    {
                        DoScriptText(SAY_INTRO4, m_creature);
                        m_uiSpeechTimer = 23000;
                        break;
                    }
                    case 4:
                    {
                        DoScriptText(SAY_INTRO5, m_creature);
                        m_uiSpeechTimer = 120000;
                        break;
                    }
                }
                ++m_uiSpeechCount;
                if (m_uiSpeechCount == 5)
                    m_uiSpeechCount = 0;
            }
            else
                m_uiSpeechTimer -= uiDiff;

            return;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEnrageTimer <= uiDiff)
        {
            SetCombatMovement(true);
            DoCast(m_creature, SPELL_BERSERK, true);
            m_uiEnrageTimer = 600000;
            m_creature->GetMotionMaster()->Clear(false, true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
        else
            m_uiEnrageTimer -= uiDiff;

        if (m_uiPhase == PHASE_FLOOR)
        {
            if (m_uiSubPhase == SUBPHASE_VORTEX)
            {
                if (m_uiVortexPhase > 0 && m_uiTimer <= uiDiff)
                {
                    if (m_uiVortexPhase == 3)
                    {
                        Creature* pVortex = m_creature->SummonCreature(NPC_VORTEX, VORTEX_FARSIGHT_X, VORTEX_FARSIGHT_Y, VORTEX_FARSIGHT_Z, VORTEX_FARSIGHT_O, TEMPSUMMON_TIMED_DESPAWN, 15000);
                        Map* pMap = m_creature->GetMap();
                        if (pMap && pVortex)
                        {
                            Map::PlayerList const &lPlayers = pMap->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                            {
                                if (itr->getSource()->isDead())
                                    continue;

                                //Far sight, should be vehicle but this is enough
                                //Crash the server in group update far members, dunno why
                                //I will try to use this again, maybe I have fix...
                                itr->getSource()->GetCamera().SetView(pVortex);
                                itr->getSource()->CastSpell(itr->getSource(), SPELL_VORTEX_DMG_AURA, true);
                            }
                        }
                        //DoCast(m_creature, SPELL_VORTEX);
                        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
                        DoCast(m_creature, SPELL_VORTEX_CHANNEL);
                    }
                    else if (m_uiVortexPhase > 3 && m_uiVortexPhase < MAX_VORTEX+3)
                    {
                        Map* pMap = m_creature->GetMap();
                        if (pMap)
                        {
                            if (Creature *pVortex = m_creature->SummonCreature(NPC_VORTEX, VortexLoc[m_uiVortexPhase-4].x, VortexLoc[m_uiVortexPhase-4].y, FLOOR_Z+urand(10, 25), 0, TEMPSUMMON_TIMED_DESPAWN, 3000))
                            {
                                Map::PlayerList const &lPlayers = pMap->GetPlayers();
                                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                                {
                                    if (itr->getSource()->isDead())
                                        continue;

                                    itr->getSource()->KnockBackFrom(pVortex, -float(pVortex->GetDistance2d(itr->getSource())), 7);
                                }
                            }
                        }
                    }
                    else if (m_uiVortexPhase == MAX_VORTEX+3)
                    {
                        m_creature->RemoveAurasDueToSpell(SPELL_VORTEX_CHANNEL);
                        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
                        Map* pMap = m_creature->GetMap();
                        if (pMap)
                        {
                            Map::PlayerList const &lPlayers = pMap->GetPlayers();
                            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                            {
                                if (itr->getSource()->isDead())
                                    continue;
                                itr->getSource()->GetCamera().SetView(itr->getSource());
                                itr->getSource()->NearTeleportTo(CENTER_X, CENTER_Y, FLOOR_Z+20.0f, 0);
                            }
                        }
                    }
                    else if (m_uiVortexPhase == MAX_VORTEX+9)
                    {
                        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
                        m_creature->SetLevitate(false);
                        m_uiSubPhase = 0;
                        m_creature->GetMotionMaster()->Clear();
                        if (Unit* pTarget = m_creature->getVictim())
                            m_creature->GetMotionMaster()->MoveChase(pTarget);
                        SetCombatMovement(true);
                        if (m_pInstance)
                            m_pInstance->SetData(TYPE_MALYGOS, IN_PROGRESS);
                    }
                    ++m_uiVortexPhase;
                    m_uiTimer = 500;
                }
                else
                    m_uiTimer -= uiDiff;
                return;
            }

            if (m_uiArcaneBreathTimer <= uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_BREATH : SPELL_ARCANE_BREATH_H);
                m_uiArcaneBreathTimer = urand(13000, 16000);
            }
            else
                m_uiArcaneBreathTimer -= uiDiff;

            if (m_uiArcaneStormTimer <= uiDiff)
            {
                DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
                m_uiArcaneStormTimer = urand(10000, 15000);
            }
            else
                m_uiArcaneStormTimer -= uiDiff;

            if (m_uiPowerSparkTimer <= uiDiff)
            {
                DoScriptText(SAY_POWER_SPARK, m_creature);
                std::list<ObjectGuid>::iterator pTargetSparkPortalGUID = m_lSparkPortalGUIDList.begin();
                advance(pTargetSparkPortalGUID, urand(0, m_lSparkPortalGUIDList.size()-1));
                if (Creature* pTargetSparkPortal = m_creature->GetMap()->GetCreature(*pTargetSparkPortalGUID))
                    if (Creature *pSpark = pTargetSparkPortal->SummonCreature(NPC_POWER_SPARK, pTargetSparkPortal->GetPositionX(), pTargetSparkPortal->GetPositionY(), pTargetSparkPortal->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 120000))
                    {
                        pTargetSparkPortal->CastSpell(pTargetSparkPortal, SPELL_PORTAL_BEAM, true);
                        m_uiTargetSparkPortalGUID = pTargetSparkPortal->GetObjectGuid();
                        pTargetSparkPortal->ForcedDespawn(19500);
                        m_uiPortalNeedResTimer = 19600;
                        m_bPortalNeedRes = true;
                    }
                m_uiPowerSparkTimer = urand(20000, 30000);
            }
            else
                m_uiPowerSparkTimer -= uiDiff;

            if (m_uiVortexTimer <= uiDiff)
            {
                m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
                m_creature->SetLevitate(true);
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_VORTEX_AIR, CENTER_X, CENTER_Y, AIR_Z);
                m_uiSubPhase = SUBPHASE_VORTEX;
                m_uiVortexPhase = 0;
                m_uiVortexTimer = 60000;
                DoScriptText(SAY_VORTEX, m_creature);
                if (m_pInstance)
                    m_pInstance->SetData(TYPE_MALYGOS, SPECIAL);
                return;
            }
            else
                m_uiVortexTimer -= uiDiff;

            if (m_uiTimer <= uiDiff)
            {
                if (m_creature->GetHealthPercent() <= 50.0f)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    DoScriptText(SAY_END_PHASE1, m_creature);
                    DespawnCreatures(NPC_POWER_SPARK);
                    m_uiPhase = PHASE_ADDS;
                    m_uiSubPhase = SUBPHASE_TALK;
                    m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
                    m_creature->SetLevitate(true);
                    SetCombatMovement(false);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, AIR_Z);
                    m_bReadyForWPMove = false;
                    m_uiTimer = 23000;
                    return;
                }
                m_uiTimer = 2000;
            }
            else
                m_uiTimer -= uiDiff;  
        }
        else if (m_uiPhase == PHASE_ADDS)
        {
            if (m_uiSubPhase == SUBPHASE_TALK)
            {
                if (m_uiTimer <= uiDiff)
                {
                    m_creature->SetSpeedRate(MOVE_RUN, 6.0f);
                    m_creature->GetMotionMaster()->Clear();
                    m_uiWP = urand(0, 3);
                    m_creature->GetMotionMaster()->MovePoint(POINT_ID_PHASE_2_WP, WPs[m_uiWP].x, WPs[m_uiWP].y, AIR_Z);
                    m_uiTimer = 15000;
                }
                else
                    m_uiTimer -= uiDiff;

                return;
            }

            if (m_uiCheckTimer <= uiDiff)
            {
                if (!IsThereAnyAdd())
                {
                    m_creature->StopMoving();
                    m_uiPhase = PHASE_DRAGONS;
                    m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM_1;
                    DoScriptText(SAY_END_PHASE2, m_creature);
                    if (Creature* pTempTarget = m_creature->SummonCreature(NPC_VORTEX, CENTER_X, CENTER_Y, FLOOR_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 7000))
                    {
                        pTempTarget->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.5f);
                        pTempTarget->CastSpell(pTempTarget, SPELL_DESTROY_PLATFORM_PRE, false);
                    }
                    m_uiTimer = 6000;
                }

                Map* pMap = m_creature->GetMap();
                if (pMap)
                {
                    Map::PlayerList const &lPlayers = pMap->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                    {
                        Player* pPlayer = itr->getSource();
                        if (pPlayer->GetVehicle() && m_uiPhase == PHASE_ADDS)
                            pPlayer->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, true);
                        else
                            pPlayer->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_ARCANE, false);
                    }
                }

                if (m_uiPhase == PHASE_DRAGONS)
                    return;

                m_uiCheckTimer = 1000;
            }
            else
                m_uiCheckTimer -= uiDiff;

            if (m_bReadyForWPMove)
            {
                if (m_uiTimer <= uiDiff)
                {
                    m_creature->GetMotionMaster()->Clear();
                    if (m_uiSubPhase == SUBPHASE_DEEP_BREATH)
                    {
                        m_uiWP = urand(0, 3);
                        m_creature->GetMotionMaster()->MovePoint(POINT_ID_PHASE_2_WP, WPs[m_uiWP].x, WPs[m_uiWP].y, AIR_Z);
                    }
                    else
                    {
                        float fAng = M_PI/4 + m_creature->GetAngle(CENTER_X, CENTER_Y);
                        m_creature->GetMotionMaster()->MovePoint(POINT_ID_PHASE_2_WP, CENTER_X-80.0f*cos(fAng), CENTER_Y-80.0f*sin(fAng), AIR_Z);
                    }
                    m_uiSubPhase = 0;
                    m_bReadyForWPMove = false;
                }
                else
                    m_uiTimer -= uiDiff;
            }

            if (m_uiSubPhase != SUBPHASE_DEEP_BREATH && m_uiShellTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->StopMoving();

                if (!urand(0, 3))
                    DoScriptText(SAY_ARCANE_OVERLOAD, m_creature);

                AntiMagicShell();
                m_uiShellTimer = urand(15000, 17000);
                m_bReadyForWPMove = true;
                m_uiTimer = 2000;
            }
            else
                m_uiShellTimer -= uiDiff;

            if (m_uiDeepBreathTimer <= uiDiff)
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(POINT_ID_DEEP_BREATH, CENTER_X, CENTER_Y, AIR_Z+20.0f);
                m_bReadyForWPMove = false;
                m_uiTimer = 20000;
                m_uiDeepBreathTimer = 63000;
                m_uiSubPhase = SUBPHASE_DEEP_BREATH;
            }
            else
                m_uiDeepBreathTimer -= uiDiff;
        }
        else if (m_uiPhase == PHASE_DRAGONS)
        {
            if (m_uiSubPhase == SUBPHASE_DESTROY_PLATFORM_1)
            {
                if (m_uiTimer <= uiDiff)
                {
                    DespawnCreatures(NPC_ARCANE_OVERLOAD);
                    DespawnCreatures(NPC_HOVER_DISK);

                    if (Creature* pTempCaster = m_creature->SummonCreature(NPC_VORTEX, CENTER_X, CENTER_Y, FLOOR_Z, 0, TEMPSUMMON_TIMED_DESPAWN, 5000))
                    {
                        pTempCaster->CastSpell(pTempCaster, SPELL_CALL_SKYTALON, true);
                        pTempCaster->CastSpell(pTempCaster, SPELL_DESTROY_PLATFORM_BOOM, false);
                    }

                    m_creature->SetInCombatWithZone();

                    m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM_2;
                    m_uiTimer = 3000;
                }
                else
                    m_uiTimer -= uiDiff;

                return;
            }
            else if (m_uiSubPhase == SUBPHASE_DESTROY_PLATFORM_2)
            {
                if (m_uiTimer <= uiDiff)
                {
                    if (m_pInstance)
                        if (GameObject* pPlatform = m_pInstance->GetSingleGameObjectFromStorage(GO_PLATFORM))
                            pPlatform->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);


                    m_creature->GetMotionMaster()->MovePoint(0, CENTER_X, CENTER_Y, FLOOR_Z);
                    DoScriptText(SAY_INTRO_PHASE3, m_creature);

                    m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM_3;
                    m_uiTimer = 3000;
                }
                else
                    m_uiTimer -= uiDiff;

                return;
            }
            else if (m_uiSubPhase == SUBPHASE_DESTROY_PLATFORM_3)
            {
                if (m_uiTimer<= uiDiff)
                {
                    m_uiSubPhase = SUBPHASE_DESTROY_PLATFORM_4;
                    m_uiTimer = 12000;
                }
                else
                    m_uiTimer -= uiDiff;

                return;
            }
            else if (m_uiSubPhase == SUBPHASE_DESTROY_PLATFORM_4)
            {
                if (m_uiTimer <= uiDiff)
                {
                    m_uiSubPhase = 0;
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    DoScriptText(SAY_AGGRO3, m_creature);
                    m_uiArcaneStormTimer = 6000;
                }
                else
                    m_uiTimer -= uiDiff;

                return;
            }

            if (m_uiSubPhase == SUBPHASE_SURGE_OF_POWER)
            {
                if (m_uiTimer <= uiDiff)
                    m_uiSubPhase = 0;
                else
                    m_uiTimer -= uiDiff;

                return;
            }

            if (m_uiArcanePulseTimer <= uiDiff)
            {
                DoCast(m_creature, SPELL_ARCANE_PULSE);
                m_uiArcanePulseTimer = 1000;
            }
            else
                m_uiArcanePulseTimer -= uiDiff;

            if (m_uiArcaneStormTimer <= uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_ARCANE_STORM : SPELL_ARCANE_STORM_H);
                m_uiArcaneStormTimer = urand(6000, 10000);
            }
            else
                m_uiArcaneStormTimer -= uiDiff;

            if (m_uiStaticFieldTimer <= uiDiff)
            {
                for (uint8 i = 0; i<=50; ++i)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (pTarget->GetEntry() == NPC_WYRMREST_SKYTALON)
                        {
                            switch (urand(0, 5))
                            {
                                case 0: DoScriptText(SAY_CAST_SPELL1, m_creature); break;
                                case 1: DoScriptText(SAY_CAST_SPELL2, m_creature); break;
                                case 2: DoScriptText(SAY_CAST_SPELL3, m_creature); break;
                            }
                            DoCast(pTarget, SPELL_STATIC_FIELD_MISSILE);
                            break;
                        }
                }

                m_uiStaticFieldTimer = urand(10000, 16000);
            }
            else
                m_uiStaticFieldTimer -= uiDiff;

            if (m_uiSurgeOfPowerTimer <= uiDiff)
            {
                for (uint8 i = 0; i<=50; ++i)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        if (pTarget->GetEntry() == NPC_WYRMREST_SKYTALON)
                        {
                            m_uiSubPhase = SUBPHASE_SURGE_OF_POWER;
                            m_uiTimer = 6500;
                            if (urand(0, 1))
                                DoScriptText(SAY_SURGE_OF_POWER, m_creature);
                            DoCast(pTarget, m_bIsRegularMode ? SPELL_SURGE_OF_POWER : SPELL_SURGE_OF_POWER_H);
                            break;
                        }
                }

                m_uiSurgeOfPowerTimer = urand(5000, 15000);
            }
            else
                m_uiSurgeOfPowerTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_power_spark
######*/
struct MANGOS_DLL_DECL npc_power_sparkAI : public ScriptedAI
{
    npc_power_sparkAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    bool isDead;
    uint32 m_uiCheckTimer;

    void Reset()
    {
        isDead = false;
        m_uiCheckTimer = 0;

        SetCombatMovement(false);
        m_creature->SetLevitate(true);
        m_creature->CastSpell(m_creature, SPELL_POWER_SPARK_VISUAL, false);
    }

    void AttackStart(Unit *pWho)
    {
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {

        if (isDead)
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;
            isDead = true;
            m_creature->RemoveAllAuras();
            m_creature->SetHealth(1);
            m_creature->CastSpell(m_creature, SPELL_POWER_SPARK_PLAYERS, true);
            m_creature->GetMotionMaster()->Clear();
            m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z+1.5f, 0);
            m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z+1.5f,26);
            m_creature->ForcedDespawn(60000);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (isDead)
            return;

        if (m_uiCheckTimer <= uiDiff)
        {
            if (m_pInstance)
                if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                {
                    m_creature->GetMotionMaster()->Clear();
                    if (m_pInstance->GetData(TYPE_MALYGOS) != SPECIAL)
                    {
                        if (m_creature->IsWithinDist(pMalygos, 5.0f, false))
                        {
                            m_creature->CastSpell(pMalygos, SPELL_POWER_SPARK, true);
                            m_creature->ForcedDespawn(100);
                        }
                        else
                            m_creature->GetMotionMaster()->MovePoint(0, pMalygos->GetPositionX(), pMalygos->GetPositionY(), pMalygos->GetPositionZ());
                    }
                    else
                        m_creature->StopMoving();
                }

            m_uiCheckTimer = 2000;
        }
        else
            m_uiCheckTimer -= uiDiff;
    }
};

/*######
## npc_nexus_lord
######*/

struct MANGOS_DLL_DECL npc_nexus_lordAI : public ScriptedAI
{
    npc_nexus_lordAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;

    float m_fTargetOldX;
    float m_fTargetOldY;
    float m_fVehicleOldX;
    float m_fVehicleOldY;
    uint32 m_uiCheckTimer;
    uint32 m_uiArcaneShockTimer;
    uint32 m_uiHasteTimer;

    void Reset()
    {
        m_uiCheckTimer = 0;
        m_fTargetOldX = 0.0f;
        m_fTargetOldY = 0.0f;
        m_fVehicleOldX = 0.0f;
        m_fVehicleOldY = 0.0f;
        m_uiArcaneShockTimer = urand(8000, 9000);
        m_uiHasteTimer = urand(10000, 12000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (VehicleKit* pDiskVehicle = m_creature->GetVehicle())
        {
            if (Unit* pDiskUnit = pDiskVehicle->GetBase())
            {
                Creature* pDisk = (Creature*)pDiskUnit;

                float fX = pDisk->GetPositionX();
                float fY = pDisk->GetPositionY();

                if (fX != m_fVehicleOldX || fY != m_fVehicleOldY)
                    m_creature->Relocate(fX, fY, pDisk->GetPositionZ(), 0);

                m_fVehicleOldX = fX;
                m_fVehicleOldY = fY;

                Unit* pTarget = m_creature->getVictim();
                if (m_creature->IsWithinDistInMap(pTarget, 4.0f))
                {
                    pDisk->GetMotionMaster()->Clear();
                    pDisk->StopMoving();
                }
                else
                {
                    if (m_uiCheckTimer <= uiDiff)
                    {
                        float fX = pTarget->GetPositionX();
                        float fY = pTarget->GetPositionY();
                        if (abs(fX - m_fTargetOldX) > 1.0f || abs(fY-m_fTargetOldY) > 1.0f)
                        {
                            pDisk->GetMotionMaster()->Clear();
                            pDisk->GetMotionMaster()->MovePoint(0, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ());
                        }
                        m_fTargetOldX = fX;
                        m_fTargetOldY = fY;
                        m_uiCheckTimer = 100;
                    }
                    else
                        m_uiCheckTimer -= uiDiff;
                }
            }
        }

        if (m_uiArcaneShockTimer <= uiDiff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ARCANE_SHOCK : SPELL_ARCANE_SHOCK_H);
            m_uiArcaneShockTimer = urand(8000, 10000);
        }
        else
            m_uiArcaneShockTimer -= uiDiff;

        if (m_uiHasteTimer <= uiDiff)
        {
            DoCast(m_creature, SPELL_HASTE);
            m_uiHasteTimer = urand(25000, 30000);
        }
        else
            m_uiHasteTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
}; 

/*######
## npc_scion_of_eternity
######*/

struct MANGOS_DLL_DECL npc_scion_of_eternityAI : public ScriptedAI
{
    npc_scion_of_eternityAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiArcaneBarrageTimer;

    void Reset()
    {
        m_uiArcaneBarrageTimer = urand(4000, 12000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcaneBarrageTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (!pTarget->GetVehicle())
                {
                    int32 uiDmg = m_bIsRegularMode ? urand(14138, 15862) : urand(16965, 19035);
                    m_creature->CastCustomSpell(pTarget, SPELL_ARCANE_BARRAGE, &uiDmg, 0, 0, true);
                    m_uiArcaneBarrageTimer = urand(4000, 12000);
                }
            }
        }
        else
            m_uiArcaneBarrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
}; 

/*######
## npc_hover_disk
######*/
struct MANGOS_DLL_DECL npc_hover_diskAI : public ScriptedAI
{
    npc_hover_diskAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiCheckTimer;
    bool m_bPassengerHere;
    bool m_bMoved;

    void Reset()
    {
        m_uiCheckTimer = 500;
        m_bPassengerHere = false;
        m_bMoved = false;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->setFaction(35);
        m_creature->CastSpell(m_creature, SPELL_FLIGHT, true);
        m_creature->SetLevitate(true);
    }

    void AttackStart(Unit *pWho)
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCheckTimer <= uiDiff)
        {
            if (m_creature->GetVehicleKit()->GetPassenger(0) && !m_bPassengerHere)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_bMoved = false;
                m_bPassengerHere = true;
            }
            else
                if (!m_bMoved && m_bPassengerHere)
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->GetMap()->CreatureRelocation(m_creature, m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z+1.5f, 0);
                    m_creature->MonsterMoveWithSpeed(m_creature->GetPositionX(), m_creature->GetPositionY(), FLOOR_Z+1.5f, 26);
                    m_bMoved = true;
                    m_bPassengerHere = false;
                }
            m_uiCheckTimer = 500;
        }
        else
            m_uiCheckTimer -= uiDiff;
    }
};

/*######
## npc_alexstrasza
######*/
struct MANGOS_DLL_DECL npc_alexstraszaAI : public ScriptedAI
{
    npc_alexstraszaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiTimer;
    uint8 m_uiCount;

    void Reset()
    {
        m_uiTimer = 9500;
        m_uiCount = 0;
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
        m_creature->SetLevitate(true);
        if (m_pInstance)
            if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                m_creature->SetFacingToObject(pMalygos);
    }

    void SpellHitTarget(Unit* pUnit, const SpellEntry* pSpell)
    {
        if (pUnit->GetEntry() == NPC_ALEXSTRASZAS_GIFT && pSpell->Id == SPELL_ALEXSTRASZAS_GIFT_BEAM)
            pUnit->CastSpell(pUnit, SPELL_ALEXSTRASZAS_GIFT_VISUAL, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiCount == 6)
            return;

        if (m_uiTimer <= uiDiff)
        {
            switch (m_uiCount)
            {
                case 0:
                    if (m_pInstance)
                        if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                            pMalygos->RemoveCorpse();
                    DoScriptText(SAY_OUTRO1, m_creature);
                    m_uiTimer = 5000;
                    break;
                case 1:
                    DoScriptText(SAY_OUTRO2, m_creature);
                    m_uiTimer = 3500;
                    break;
                case 2:
                    DoScriptText(SAY_OUTRO3, m_creature);
                    m_uiTimer = 22000;
                    break;
                case 3:
                    DoScriptText(SAY_OUTRO4, m_creature);
                    m_uiTimer = 19500;
                    break;
                case 4:
                    DoCast(m_creature, SPELL_ALEXSTRASZAS_GIFT_BEAM);
                       m_uiTimer = 3000;
                    break;
                case 5:
                    if (m_pInstance)
                        m_pInstance->SetData(TYPE_MALYGOS, DONE);
                    break;
            }
            ++m_uiCount;
        }
        else
            m_uiTimer -= uiDiff;
    }
};

/*######
## go_focusing_iris
######*/
bool GOHello_go_focusing_iris(Player* pPlayer, GameObject* pGo)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData())
    {
        if (Unit* pMalygos = pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
        {
            if (Creature* pSparkPortal = GetClosestCreatureWithEntry(pMalygos, NPC_SPARK_PORTAL, 100.0f))
                if (pSparkPortal->HasAura(SPELL_PORTAL_BEAM))
                    pSparkPortal->ForcedDespawn();

            pMalygos->GetMotionMaster()->Clear();
            float fAng = pMalygos->GetAngle(CENTER_X, CENTER_Y);
            pMalygos->GetMotionMaster()->MovePoint(POINT_ID_LAND, CENTER_X-40.76f*cos(fAng), CENTER_Y-40.76f*sin(fAng), FLOOR_Z+7.0f);
            return true;
        }
        else
            return false;
    }
    return false;
}

CreatureAI* GetAI_boss_malygos(Creature* pCreature)
{
    return new boss_malygosAI(pCreature);
}

CreatureAI* GetAI_npc_power_spark(Creature* pCreature)
{
    return new npc_power_sparkAI(pCreature);
}

CreatureAI* GetAI_npc_nexus_lord(Creature* pCreature)
{
    return new npc_nexus_lordAI(pCreature);
} 

CreatureAI* GetAI_npc_scion_of_eternity(Creature* pCreature)
{
    return new npc_scion_of_eternityAI(pCreature);
} 

CreatureAI* GetAI_npc_hover_disk(Creature* pCreature)
{
    return new npc_hover_diskAI(pCreature);
} 

CreatureAI* GetAI_npc_alexstrasza(Creature* pCreature)
{
    return new npc_alexstraszaAI(pCreature);
}

struct MANGOS_DLL_DECL npc_whyrmrest_skytalonAI : public ScriptedAI
{
    npc_whyrmrest_skytalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool Active;
    bool seated;
    ObjectGuid ownerGUID;
    uint32 StartTimer;
    uint32 SeatTimer;

    void Reset()
    {
        Active = false;
        seated = false;
        ownerGUID = ObjectGuid();
        StartTimer = 500;
        SeatTimer = 2000;
        ownerGUID = m_creature->GetCreatorGuid();
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
        m_creature->SetLevitate(true);
    }

    void AttackStart(Unit *) {}
    void MoveInLineOfSight(Unit*) {}

    void JustDied(Unit* killer)
    {
        if (!m_creature || m_creature->GetTypeId() != TYPEID_UNIT)
            return;

        if (ownerGUID.IsEmpty())
            ownerGUID = m_creature->GetCreatorGuid();

        Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID);

        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        owner->RemoveAurasDueToSpell(SPELL_VEHICLE_HARDCODED);
        m_creature->SetCreatorGuid(ObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff)
    {

        if (ownerGUID.IsEmpty())
        {
            ownerGUID = m_creature->GetCreatorGuid();
            if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
                m_creature->setFaction(owner->getFaction());
        }

        if (!ownerGUID.IsEmpty() && !seated && Active)
        {
            if (SeatTimer < uiDiff)
            {
                if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
                {
                    m_creature->setFaction(owner->getFaction());
                    owner->CastSpell(m_creature, SPELL_VEHICLE_HARDCODED, true);
                    if (Creature* pMalygos = m_pInstance->GetSingleCreatureFromStorage(NPC_MALYGOS))
                    {
                        pMalygos->SetInCombatWith(m_creature);
                        pMalygos->AddThreat(m_creature);
                    }
                    seated = true;
                }
            }
            else
                SeatTimer -= uiDiff;
        }

        if (!ownerGUID.IsEmpty() && !Active)
        {
            if (StartTimer < uiDiff)
            {
                if (Unit* owner = m_creature->GetMap()->GetUnit(ownerGUID))
                {
                    float x, y, z;
                    owner->GetClosePoint(x, y, z, owner->GetObjectBoundingRadius(), 1.0f, owner->GetAngle(m_creature));
                    m_creature->GetMotionMaster()->MovePoint(0, x, y, z);
                    Active = true;
                }
            }
            else
                StartTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_whyrmrest_skytalon(Creature* pCreature)
{
    return new npc_whyrmrest_skytalonAI(pCreature);
}

void AddSC_boss_malygos()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_malygos";
    newscript->GetAI = &GetAI_boss_malygos;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_power_spark";
    newscript->GetAI = &GetAI_npc_power_spark;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_nexus_lord";
    newscript->GetAI = &GetAI_npc_nexus_lord;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_scion_of_eternity";
    newscript->GetAI = &GetAI_npc_scion_of_eternity;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_hover_disk";
    newscript->GetAI = &GetAI_npc_hover_disk;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_whyrmrest_skytalon";
    newscript->GetAI = &GetAI_npc_whyrmrest_skytalon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_alexstrasza";
    newscript->GetAI = &GetAI_npc_alexstrasza;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_focusing_iris";
    newscript->pGOUse = &GOHello_go_focusing_iris;
    newscript->RegisterSelf();
}
