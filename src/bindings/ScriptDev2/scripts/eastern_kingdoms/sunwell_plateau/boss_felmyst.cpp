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
SDName: boss_felmyst
SD%Complete: 50%
SDComment: Only ground phase spells
SDCategory: Sunwell Plateau
EndScriptData */

#include "precompiled.h"
#include "sunwell_plateau.h"

enum
{
    SAY_INTRO           = -1580036,
    SAY_KILL_1          = -1580037,
    SAY_KILL_2          = -1580038,
    SAY_DEATH           = -1580042,
    SAY_TAKEOFF         = -1580040,
    SAY_BREATH          = -1580039,
    SAY_BERSERK         = -1580041,

    SPELL_FELBLAZE_VISUAL       = 45068,        // Visual transform aura
    SPELL_NOXIOUS_FUMES         = 47002,
    SPELL_BERSERK               = 26662,

    // ground phase
    SPELL_CLEAVE                = 19983,
    SPELL_CORROSION             = 45866,
    SPELL_GAS_NOVA              = 45855,
    SPELL_ENCAPSULATE           = 45665,
    SPELL_ENCAPSULATE_CHANNEL   = 45661,

    // flight phase
    SPELL_DEMONIC_VAPOR         = 45399,
    SPELL_VAPOR_BEAM_VISUAL     = 45389,
    SPELL_FOG_CORRUPTION        = 45582,
    SPELL_SOUL_SEVER            = 45918,        // kills all charmed targets at wipe - script effect for 45917
    SPELL_SUMMON_VAPOR          = 45391,
    SPELL_SUMMON_VAPOR_TRIAL    = 45410,
    SPELL_SUMMON_BLAZING_DEAD   = 45400,

    SPELL_SUMMON_DEATH_CLOUD    = 45884,

    // npcs
    NPC_UNYELDING_DEAD          = 25268,        // spawned during flight phase
    NPC_DEMONIC_VAPOR           = 25265,
    NPC_DEMONIC_VAPOR_TRAIL     = 25267,

    // phases
    PHASE_GROUND                = 1,
    PHASE_AIR                   = 2,
};

//Positional defines
struct LocationsXY
{
    float x, y, z;
    uint32 id;
};

// Movement coordinates
static LocationsXY MoveLoc[]=
{
    {1489.307f, 701.451f, 50.5f},   // right   0
    {1447.672f, 649.917f, 50.5f},   // right   1
    {1430.092f, 604.318f, 50.5f},   // center  2
    {1462.899f, 536.334f, 50.5f},   // left    3
    {1537.197f, 522.199f, 50.5f},   // left    4
};

struct MANGOS_DLL_DECL boss_felmystAI : public ScriptedAI
{
    boss_felmystAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_sunwell_plateau*)pCreature->GetInstanceData();
        m_bHasTransformed = false;
        Reset();
    }

    instance_sunwell_plateau* m_pInstance;

    bool m_bHasTransformed;

    uint8 m_uiPhase;
    uint32 m_uiBerserkTimer;
    uint32 m_uiMovemetnTimer;

    // Ground Phase timers
    uint32 m_uiFlyPhaseTimer;
    uint32 m_uiCorrosionTimer;
    uint32 m_uiCleaveTimer;
    uint32 m_uiEncapsulateTimer;
    uint32 m_uiGasNovaTimer;

    // Air Phase
    uint32 m_uiLastPointId;
    uint32 m_uiDemonicVaporTimer;
    uint32 m_uiFogOfCorruptionTimer;
    uint8  m_uiBreathCount;    // 3 breath attacks a air phase(deathCloud Attacks)
    uint8  m_uiVaporCount;     // 2 of these attacks during air phase
    uint32 m_uiFogSummonTimer;
    bool   m_bIsBreath;

    void Reset()
    {
        // Transform into Felmyst dragon
        DoCastSpellIfCan(m_creature, SPELL_FELBLAZE_VISUAL);

        m_uiPhase               = PHASE_GROUND;
        m_uiBerserkTimer        = 10*MINUTE*IN_MILLISECONDS;

        // Ground Phase
        m_uiCorrosionTimer      = 30000;
        m_uiCleaveTimer         = urand(2000, 5000);
        m_uiGasNovaTimer        = 17000;
        m_uiEncapsulateTimer    = urand(30000, 40000);
        m_uiFlyPhaseTimer       = 60000;        // flight phase after 1 min
        m_uiMovemetnTimer       = 11000;

        m_creature->SetLevitate(true);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bHasTransformed)
        {
            if (pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinLOSInMap(m_creature) && pWho->IsWithinDistInMap(m_creature, 100.0f))
            {
                DoScriptText(SAY_INTRO, m_creature);
                m_bHasTransformed = true;
            }
            else
                return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void EnterEvadeMode()
    {
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);

        // Add the visual aura back when evading - workaround because there is no way to remove only the negative auras
        DoCastSpellIfCan(m_creature, SPELL_FELBLAZE_VISUAL, CAST_TRIGGERED);

        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(NULL);

        Reset();
    }

    void Aggro(Unit* pWho)
    {
        DoCastSpellIfCan(m_creature, SPELL_NOXIOUS_FUMES);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, IN_PROGRESS);

        m_creature->SetInCombatWithZone();
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        SetCombatMovement(false);
        m_creature->SetWalk(false);
        m_creature->SetSpeedRate(MOVE_RUN, 2.0f);
        m_creature->GetMotionMaster()->MovePoint(2, pWho->GetPositionX(), pWho->GetPositionY(), pWho->GetPositionZ()+5.0f);
    }

    void KilledUnit(Unit* pVictim)
    {
        //won't say killing pet/other unit
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;
        DoScriptText(urand(0, 1) ? SAY_KILL_1 : SAY_KILL_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, DONE);
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        switch (uiPointId)
        {
            case 1:
                m_bIsBreath = false;
                m_uiFogOfCorruptionTimer = 5000;
                break;
            case 2:
                m_creature->SetLevitate(false);
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                break;
            case 3:
                m_uiMovemetnTimer = 300;
                break;
            default:
                break;
        }

    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_FELMYST, FAIL);
        m_creature->SetLevitate(true);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pTarget->GetTypeId() == TYPEID_PLAYER && pSpell->Id == SPELL_ENCAPSULATE_CHANNEL)
            pTarget->CastSpell(pTarget, SPELL_ENCAPSULATE, true, NULL, NULL, m_creature->GetObjectGuid());
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (!pSummoned)
            return;

        if (pSummoned->GetEntry() == NPC_DEMONIC_VAPOR)
            m_creature->CastSpell(pSummoned, SPELL_VAPOR_BEAM_VISUAL, true);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_uiMovemetnTimer < uiDiff)
            {
                // movement
                if (m_uiLastPointId < 2)
                    m_uiLastPointId = urand(2, 4);
                else if (m_uiLastPointId > 2)
                    m_uiLastPointId = urand(0, 2);
                else
                    m_uiLastPointId = urand(0, 1) ? urand(0, 1) : urand(3, 4);

                m_creature->GetMotionMaster()->MovePoint(3, MoveLoc[m_uiLastPointId].x, MoveLoc[m_uiLastPointId].y, MoveLoc[m_uiLastPointId].z);
                m_uiMovemetnTimer = 60000;
            }
            else
                m_uiMovemetnTimer -= uiDiff;
            return;
        }

        if (m_uiBerserkTimer)
        {
            if (m_uiBerserkTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiBerserkTimer = 0;
                }
            }
            else
                m_uiBerserkTimer -= uiDiff;
        }

        if (m_uiPhase == PHASE_GROUND)
        {
            if (m_uiCleaveTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CLEAVE) == CAST_OK)
                    m_uiCleaveTimer = urand(2000, 5000);
            }
            else
                m_uiCleaveTimer -= uiDiff;

            if (m_uiCorrosionTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CORROSION) == CAST_OK)
                    m_uiCorrosionTimer = urand(15000, 30000);;
            }
            else
                m_uiCorrosionTimer -= uiDiff;

            if (m_uiGasNovaTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_GAS_NOVA) == CAST_OK)
                    m_uiGasNovaTimer = urand(3000, 35000);
            }
            else
                m_uiGasNovaTimer -= uiDiff;

            if (m_uiEncapsulateTimer < uiDiff)
            {
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, uint32(0), SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_ENCAPSULATE_CHANNEL) == CAST_OK)
                        m_uiEncapsulateTimer = urand(30000, 40000);
                }
            }
            else
                m_uiEncapsulateTimer -= uiDiff;

            if (m_uiFlyPhaseTimer < uiDiff)
            {
                m_creature->GetMotionMaster()->Clear();
                SetCombatMovement(false);
                m_creature->SetLevitate(true);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                m_uiLastPointId = urand(0, 4);
                m_creature->GetMotionMaster()->MovePoint(0, MoveLoc[m_uiLastPointId].x, MoveLoc[m_uiLastPointId].y, MoveLoc[m_uiLastPointId].z);
                DoScriptText(SAY_TAKEOFF, m_creature);
                m_uiPhase                   = PHASE_AIR;
                m_uiDemonicVaporTimer       = 10000;
                m_uiFogOfCorruptionTimer    = 35000;
                m_uiBreathCount             = 0;
                m_uiVaporCount              = 0;
                m_bIsBreath                 = false;
             }
             else
                m_uiFlyPhaseTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else if (m_uiPhase == PHASE_AIR)
        {
            if (m_uiFogOfCorruptionTimer < uiDiff)
            {
                ++m_uiBreathCount;
                // TODO: Skip Fog Of Corruption. because spell not implemented yet
                //if (m_uiBreathCount == 3)
                {
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(2, m_creature->getVictim()->GetPositionX(), m_creature->getVictim()->GetPositionY(), m_creature->getVictim()->GetPositionZ()+5.0f);
                    m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
                    m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                    m_uiPhase = PHASE_GROUND;
                    m_uiFlyPhaseTimer = 60000;
                    return;
                }
                DoScriptText(SAY_BREATH, m_creature);
                m_uiFogSummonTimer = 1000;
                m_uiFogOfCorruptionTimer = 60000;
                m_bIsBreath = true;
                // movement
                if (m_uiLastPointId < 2)
                    m_uiLastPointId = urand(2, 4);
                else if (m_uiLastPointId > 2)
                    m_uiLastPointId = urand(0, 2);
                else
                    m_uiLastPointId = urand(0, 1) ? urand(0, 1) : urand(3, 4);

                m_creature->GetMotionMaster()->MovePoint(1, MoveLoc[m_uiLastPointId].x, MoveLoc[m_uiLastPointId].y, MoveLoc[m_uiLastPointId].z);
            }
            else
                m_uiFogOfCorruptionTimer -= uiDiff;

            if (m_bIsBreath)
            {
                if (m_uiFogSummonTimer < uiDiff)
                {
                    //DoCast(m_creature, SPELL_SUMMON_DEATH_CLOUD); //TODO: this is wrong ! must find exact summoner.
                    m_uiFogSummonTimer = 2000;
                }
                else
                    m_uiFogSummonTimer -= uiDiff;
            }

            // need to implent only 2 of these per air phase
            if (m_uiVaporCount < 2)
            {
                if (m_uiDemonicVaporTimer < uiDiff)
                {
                    DoCast(m_creature, SPELL_SUMMON_VAPOR);
                    ++m_uiVaporCount;
                    m_uiDemonicVaporTimer = 10000;
                }
                else
                    m_uiDemonicVaporTimer -= uiDiff;
            }
        }
    }
};

CreatureAI* GetAI_boss_felmyst(Creature* pCreature)
{
    return new boss_felmystAI(pCreature);
}

/*######
## felmyst_vapor
######*/

struct MANGOS_DLL_DECL npc_felmyst_vaporAI : public ScriptedAI
{
    npc_felmyst_vaporAI(Creature* pCreature) : ScriptedAI(pCreature)
   {
       m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, 0.0f);
       m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 0.0f );
       Reset();
   }

    void Reset()
    {
        // workaround, to select a close victim (== summoner in this case)
        if (Player* pPlayer = m_creature->GetMap()->GetPlayer(m_creature->GetCreatorGuid()))
            AttackStart(pPlayer);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // ignore threat list
        if (!m_creature->getVictim())
        {
            Unit* pTarget =  GetClosestAttackableUnit(m_creature, 100.0f);  // maybe we need to exclude pets?
            // unsummon if no unit is present
            if (!pTarget)
                m_creature->ForcedDespawn();
            else
                AttackStart(pTarget);
        }

        // no melee attack
    }
};

CreatureAI* GetAI_npc_felmyst_vapor(Creature* pCreature)
{
    return new npc_felmyst_vaporAI(pCreature);
}

/*######
## felmyst_vapor_Cloud  ==  this is the cloud(npc) left behind by the demonic vapor beam attack
######*/

struct MANGOS_DLL_DECL npc_felmyst_vapor_cloudAI : public ScriptedAI
{
    npc_felmyst_vapor_cloudAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        SetCombatMovement(false);
    }

    uint32 m_summonTimer;
    uint32 m_createSkeletonTimer;
    uint32 m_cloudlifeTimer;

    void Reset()
    {
        // some delay to give a chance to flee from the skeletons
        m_createSkeletonTimer = 5000;
        m_summonTimer = 1500;
        m_cloudlifeTimer = 25000;
    }

    // CreatureNullAI
    void AttackStart(Unit *) {}
    void AttackedBy( Unit *) {}
    void EnterEvadeMode() {}

    void JustSummoned(Creature* pSummoned)
    {
        if (!pSummoned)
            return;

        if (pSummoned->GetEntry() == NPC_UNYELDING_DEAD )
            pSummoned->SetInCombatWithZone();
    }

    void MoveInLineOfSight(Unit* pWho)    // change this and use void update INTERACTION_DISTANCE to player from Vapor cloud to spawn skellys
    {
        // summon skeleton if unit is close
        if (!m_summonTimer && m_creature->IsHostileTo(pWho) && m_creature->IsWithinDistInMap(pWho, 3))
        {
            // 50% chance - to not make spawns to much as there are many of this mobs in a VaporCloud
            if (!urand(0,1))
                DoCast(pWho, SPELL_SUMMON_BLAZING_DEAD, true);
            m_summonTimer = 1000;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // on-create summon
        if (m_createSkeletonTimer)
        {
            m_createSkeletonTimer -= uiDiff;
            if (m_createSkeletonTimer <= 0)
            {
                DoCast(m_creature, SPELL_SUMMON_BLAZING_DEAD, true);
                m_createSkeletonTimer = 0;
            }
        }

        // movement cooldown
        if (m_summonTimer)
        {
            m_summonTimer -= uiDiff;
            if (m_summonTimer <= 0)
                m_summonTimer = 0;
        }

        //despawn after 25 seconds
        if (m_cloudlifeTimer < uiDiff)
        {
            m_creature->ForcedDespawn();
        }
        else
            m_cloudlifeTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_felmyst_vapor_cloud(Creature* pCreature)
{
    return new npc_felmyst_vapor_cloudAI(pCreature);
}

/*######
## mob_fog_of_corruption
######*/
//////////////// TODO: implementing is useless. because mind controls not implementd. ////////////////

void AddSC_boss_felmyst()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_felmyst";
    pNewScript->GetAI = &GetAI_boss_felmyst;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="npc_felmyst_vapor";
    pNewScript->GetAI = &GetAI_npc_felmyst_vapor;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="npc_felmyst_vapor_cloud";
    pNewScript->GetAI = &GetAI_npc_felmyst_vapor_cloud;
    pNewScript->RegisterSelf();
}