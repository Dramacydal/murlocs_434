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
SDName: Boss_Skadi
SD%Complete: 20%
SDComment: starts at trigger 4991
SDCategory: Utgarde Pinnacle
EndScriptData */


/* TODO
better fly script
remove hacks like the shoot at grauf
trigger mob should use summon spells
*/
#include "precompiled.h"
#include "utgarde_pinnacle.h"
#include "Vehicle.h"
#include "MotionMaster.h"

enum
{
    SAY_AGGRO                       = -1575019,
    SAY_DRAKEBREATH_1               = -1575020,
    SAY_DRAKEBREATH_2               = -1575021,
    SAY_DRAKEBREATH_3               = -1575022,
    SAY_DRAKE_HARPOON_1             = -1575023,
    SAY_DRAKE_HARPOON_2             = -1575024,
    SAY_KILL_1                      = -1575025,
    SAY_KILL_2                      = -1575026,
    SAY_KILL_3                      = -1575027,
    SAY_DEATH                       = -1575028,
    SAY_DRAKE_DEATH                 = -1575029,
    EMOTE_HARPOON_RANGE             = -1575030,

    SPELL_CRUSH                     = 50234,
    SPELL_CRUSH_H                   = 59330,

    SPELL_WHIRLWIND                 = 50228,
    SPELL_WHIRLWIND_H               = 59322,

    SPELL_POISONED_SPEAR            = 50255,
    SPELL_POISONED_SPEAR_H          = 59331,

    SPELL_SKADI_TELEPORT            = 61790,

    // casted with base of creature 22515 (World Trigger), so we must make sure
    // to use the close one by the door leading further in to instance.
    SPELL_SUMMON_GAUNTLET_MOBS      = 48630,                // tick every 30 sec
    SPELL_SUMMON_GAUNTLET_MOBS_H    = 59275,                // tick every 25 sec

    SPELL_GAUNTLET_PERIODIC         = 47546,                // what is this? Unknown use/effect, but probably related

    SPELL_LAUNCH_HARPOON            = 48641,                // this spell hit drake to reduce HP (force triggered from 48641)

    SPELL_SUMMON_1                  = 48631,
    SPELL_SUMMON_2                  = 48632,
    SPELL_SUMMON_3                  = 48633,
    SPELL_SUMMON_4                  = 48634,
    SPELL_SUMMON_5                  = 48635,
    SPELL_SUMMON_6                  = 48636,
};

#define HIT_GRAUF_MESSAGE "I've hit Grauf!"
/*######
## boss_skadi
######*/

enum CombatPhase
{
    FLYING,
    SKADI
};

struct FlightPositionSkadi
{
    float x, y,z;
};

static FlightPositionSkadi FlightPosition[]=
{
       {341.740997f, -516.955017f, 104.668999f},   // Start
       {293.299f, -505.95f, 142.03f},              // short time after start
       {301.664f, -535.164f, 146.097f},            // rotating position
       {526.896f, -546.387f, 119.209f},            // shoot position
       // PONTOS DO BREACH
       {485.4577f, -511.2515f, 115.3011f}, // Breath 1
       {435.1892f, -514.5232f, 118.6719f}, // Breath 1

       {413.9327f, -540.9407f, 138.2614f}, // fly over the wall

       {477.311981f, -509.296814f, 104.723083f},   // Postion where skadi "jump" after graufs death
};

struct MANGOS_DLL_DECL boss_skadiAI : public ScriptedAI
{
    boss_skadiAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pinnacle* m_pInstance;
    bool m_bIsRegularMode;

    CombatPhase m_CombatPhase;

    uint32 m_uiCrush;
    uint32 m_uiWhirlwind;
    uint32 m_uiPoisonedSpear;

    void Reset()
    {
        m_CombatPhase = SKADI;
        m_uiCrush = 10000;
        m_uiWhirlwind = urand(2000, 5000);
        m_uiPoisonedSpear = 6000;
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_SKADI, FAIL);
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_MY_GIRL_LOVES_SKADI_ALL_THE_TIME, false);
            if (Creature* pGrauf = m_pInstance->GetSingleCreatureFromStorage(NPC_GRAUF))
            {
                pGrauf->Respawn();
            }
        }
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
        {
            if (Creature* pGrauf = m_pInstance->GetSingleCreatureFromStorage(NPC_GRAUF))
            {
                m_creature->EnterVehicle(pGrauf->GetVehicleKit(),0);
                m_CombatPhase = FLYING;
            }
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_KILL_1, m_creature); break;
            case 1: DoScriptText(SAY_KILL_2, m_creature); break;
            case 2: DoScriptText(SAY_KILL_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SKADI, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_CombatPhase)
        {
            case FLYING:
                // Nothing
                break;
            case SKADI:
            {

                if (m_uiCrush < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CRUSH : SPELL_CRUSH_H) == CAST_OK)
                        m_uiCrush = 10000;
                }
                else
                    m_uiCrush -= uiDiff;

                if (m_uiWhirlwind < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_WHIRLWIND : SPELL_WHIRLWIND_H) == CAST_OK)
                        m_uiWhirlwind = urand(15000, 20000);
                }
                else
                    m_uiWhirlwind -= uiDiff;

                if (m_uiPoisonedSpear < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, 0.0f, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_POISONED_SPEAR : SPELL_POISONED_SPEAR_H) == CAST_OK)
                            m_uiPoisonedSpear = 6000;
                    }
                }
                else
                    m_uiPoisonedSpear -= uiDiff;

                DoMeleeAttackIfReady();
            }
        }
    }
};

CreatureAI* GetAI_boss_skadi(Creature* pCreature)
{
    return new boss_skadiAI(pCreature);
}

struct boss_skadi_graufAI : public ScriptedAI
{
    boss_skadi_graufAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        vehicle = m_creature->GetVehicleKit();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
    }

    instance_pinnacle* m_pInstance;
    VehicleKit* vehicle;
    bool m_bIsRegularMode;

    bool isInFlight;
    uint32 uiWaypointId;
    uint32 uiMovementTimer;

    uint8 m_uiHarpoonHitCounter;
    uint8 m_uiHarpoonHitCounterAchiev;
    uint32 m_uiSummon;

    void Reset()
    {
        isInFlight = false;
        uiWaypointId = 0;
        uiMovementTimer = 1000;
        m_uiHarpoonHitCounter = 0;
        m_uiHarpoonHitCounterAchiev = 0;
        m_uiSummon = 5000;
    }

    void JustReachedHome()
    {
        if (Unit* passenger = vehicle->GetPassenger(0))
        {
            passenger->ExitVehicle();
            if (passenger->GetTypeId() != TYPEID_PLAYER)
                ((Creature*)passenger)->AI()->EnterEvadeMode();
        }
        // make boss land
        m_creature->SetLevitate(false);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
        m_creature->SetWalk(true);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData)
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        switch(uiData)
        {
            case 1: // short after start
            case 2: // rotating position
                ++uiWaypointId;
                uiMovementTimer = 1000;
                break;
            case 3: // shoot position
            {
                ++uiWaypointId;
                uiMovementTimer = 15000;
                break;
            }
            case 4: // Breath 1
                m_uiHarpoonHitCounterAchiev = 0;
                ++uiWaypointId;
                if (m_pInstance)
                {
                    if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
                        DoScriptText(SAY_DRAKEBREATH_1, pSkadi);
                    m_pInstance->DoMakeFreezingCloud();
                }
                uiMovementTimer = 1000;
                break;
            case 5: // Breath 2
                ++uiWaypointId;
                if (m_pInstance)
                {
                    if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
                        DoScriptText(SAY_DRAKEBREATH_2, pSkadi);
                }
                uiMovementTimer = 1000;
                break;
            case 6: // fly over wall
                uiWaypointId= 3;
                if (m_pInstance)
                {
                    if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
                        DoScriptText(SAY_DRAKEBREATH_3, pSkadi);
                }
                uiMovementTimer = 1000;
                break;
            default:
                uiWaypointId = 0;
                isInFlight = false;
                uiMovementTimer = 1000;
                break;
        }

        if ( uiWaypointId > 6 )
            error_log("SD2: Instance Pinnacle: Skadi Grauf try move to point %u!", uiWaypointId);
    }

    void JustDied(Unit* pTarget)
    {
        if (m_pInstance)
        {
            if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
            {
                DoScriptText(SAY_DRAKE_DEATH, pSkadi);
            }
            if (m_uiHarpoonHitCounterAchiev == m_uiHarpoonHitCounter)
            {
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_MY_GIRL_LOVES_SKADI_ALL_THE_TIME, true);
            }
        }
    }

    void HarpoonHit()
    {
        ++m_uiHarpoonHitCounter;
        ++m_uiHarpoonHitCounterAchiev;
        if (m_uiHarpoonHitCounter > 6)
        {
            if (m_pInstance)
            {
                if (Creature* pSkadi = m_pInstance->GetSingleCreatureFromStorage(NPC_SKADI))
                {
                    pSkadi->ExitVehicle();
                    pSkadi->CastSpell(pSkadi, SPELL_SKADI_TELEPORT, true);
                    if (boss_skadiAI* skadiAI = dynamic_cast<boss_skadiAI*> (pSkadi->AI()))
                    {
                        pSkadi->SetInCombatWithZone();
                        skadiAI->m_CombatPhase = SKADI;
                    }
                }
            }
            m_creature->DealDamage(m_creature, m_creature->GetHealth(),  NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NONE, NULL, false);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->getVictim() || (!uiWaypointId && !m_creature->SelectHostileTarget()))
            return;

        if (vehicle->GetPassenger(0) && !isInFlight)
        {
            isInFlight = true;
            m_creature->SetLevitate(true);
            m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
            m_creature->SetWalk(false);
            uiWaypointId = 1;
            uiMovementTimer = 1000;
        }

        if (isInFlight && m_creature->isAlive())
        {
            if (uiMovementTimer < uiDiff)
            {
                m_creature->StopMoving();
                m_creature->GetMotionMaster()->Clear();
                if ( uiWaypointId < 7)
                    m_creature->GetMotionMaster()->MovePoint(uiWaypointId, FlightPosition[uiWaypointId].x, FlightPosition[uiWaypointId].y, FlightPosition[uiWaypointId].z);
                uiMovementTimer = 20000;
            }
            else
                uiMovementTimer -= uiDiff;

            if (m_uiSummon < uiDiff)
            {
                if (Creature* pTemp = m_creature->SummonCreature(NPC_YMIRJAR_WARRIOR, 471.0f +irand(-5,5), -506.0f+irand(-5,5), 105.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                    pTemp->SetInCombatWithZone();
                if (Creature* pTemp = m_creature->SummonCreature(NPC_YMIRJAR_HARPOONER, 471.0f+irand(-5,5), -506.0f+irand(-5,5), 105.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                    pTemp->SetInCombatWithZone();
                if (Creature* pTemp = m_creature->SummonCreature(NPC_YMIRJAR_WITCH_DOCTOR, 471.0f+irand(-5,5), -506.0f+irand(-5,5), 105.0f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                    pTemp->SetInCombatWithZone();
                m_uiSummon = m_bIsRegularMode ? 30000 : 25000;
            }
            else
                m_uiSummon -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_skadi_grauf(Creature* pCreature)
{
    return new boss_skadi_graufAI(pCreature);
}

bool AreaTrigger_at_skadi(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (instance_pinnacle* pInstance = (instance_pinnacle*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SKADI) == NOT_STARTED)
            pInstance->SetData(TYPE_SKADI, SPECIAL);
    }

    return false;
}

bool GOHello_go_harpoon_launcher(Player *pPlayer, GameObject *pGO)
{
    if (instance_pinnacle* pInstance = (instance_pinnacle*)pGO->GetInstanceData())
    {
        if (Creature* pGrauf = pInstance->GetSingleCreatureFromStorage(NPC_GRAUF))
        {
            if (pGO->HasInArc(M_PI_F, pGrauf))
            {
                if (boss_skadi_graufAI* graufAI = (boss_skadi_graufAI*)pGrauf->AI())
                {
                    graufAI->HarpoonHit();
                    pPlayer->MonsterSay(HIT_GRAUF_MESSAGE,LANG_UNIVERSAL);
                }
            }
        }
    }
    return true;
}

void AddSC_boss_skadi()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_skadi";
    pNewScript->GetAI = &GetAI_boss_skadi;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_skadi_grauf";
    pNewScript->GetAI = &GetAI_boss_skadi_grauf;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_skadi";
    pNewScript->pAreaTrigger = &AreaTrigger_at_skadi;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "harpoon_Skadi";
    pNewScript->pGOUse = &GOHello_go_harpoon_launcher;
    pNewScript->RegisterSelf();
}
