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
SDName: boss_lord_marrowgar
SD%Complete: 99%
SDComment:  by michalpolko with special thanks to:
            mangosR2 team and all who are supporting us with feedback, testing and fixes
            TrinityCore for some info about spells IDs
            everybody whom I forgot to mention here ;)

SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

// talks
enum
{
    SAY_INTRO                   = -1631001,
    SAY_AGGRO                   = -1631002,
    SAY_BONE_STORM              = -1631003,
    SAY_BONE_SPIKE_1            = -1631004,
    SAY_BONE_SPIKE_2            = -1631005,
    SAY_BONE_SPIKE_3            = -1631006,
    SAY_SLAY_1                  = -1631007,
    SAY_SLAY_2                  = -1631008,
    SAY_DEATH                   = -1631009,
    SAY_BERSERK                 = -1631010,
};

// spells
enum
{
    //common
    SPELL_BERSERK                           = 47008,

    //summons
    NPC_BONE_SPIKE                          = 38711,
    NPC_COLDFLAME                           = 36672,

    // Bone Slice
    SPELL_BONE_SLICE_10                     = 69055,
    SPELL_BONE_SLICE_25                     = 70814,

    // Bone Storm
    SPELL_BONE_STORM                        = 69076,

    // Coldflame
    SPELL_COLDFLAME                         = 69140, // not used, hacked with custom code
    SPELL_COLDFLAME_AURA                    = 69145,
    SPELL_COLDFLAME_X                       = 72705,

    // Bone Spike
    SPELL_BONE_SPIKE_10                     = 69057,
    SPELL_BONE_SPIKE_10_H                   = 72088,
    SPELL_BONE_SPIKE_STORM_10               = 73144,
    SPELL_BONE_SPIKE_25                     = 70826,
    SPELL_BONE_SPIKE_25_H                   = 72089,
    SPELL_BONE_SPIKE_STORM_25               = 73145,

    SPELL_IMPALED                           = 69065,
    SPELL_VEHICLE_HARDCODED                 = 46598,

    // phases
    PHASE_NORMAL                            = 1,
    PHASE_BONE_STORM_CHARGE                 = 2,
    PHASE_BONE_STORM_CHARGING               = 3,
    PHASE_BONE_STORM_COLDFLAME              = 4,

    // others
    POINT_CHARGE                            = 1,

    MAX_CHARGES_NORMAL                      = 4,
    MAX_CHARGES_HEROIC                      = 5,
};

/*####
# Lord Marrowgar
####*/
struct MANGOS_DLL_DECL boss_lord_marrowgarAI : public base_icc_bossAI
{
    boss_lord_marrowgarAI(Creature* pCreature) : base_icc_bossAI(pCreature)
    {
        m_pInstance = (instance_icecrown_spire*)pCreature->GetInstanceData();
        m_bSaidIntro = false;
        m_uiMaxCharges = m_bIsHeroic ? MAX_CHARGES_HEROIC : MAX_CHARGES_NORMAL;

        Reset();
    }

    instance_icecrown_spire* m_pInstance;
    bool m_bSaidIntro;

    uint8 m_uiPhase;
    uint8 m_uiChargesCount;
    uint8 m_uiMaxCharges;

    uint32 m_uiBerserkTimer;
    uint32 m_uiBoneSliceTimer;
    uint32 m_uiColdflameTimer;
    uint32 m_uiBoneSpikeTimer;
    uint32 m_uiBoneStormTimer;
    uint32 m_uiBoneStormChargeTimer;
    uint32 m_uiBoneStormColdflameTimer;

    void Reset()
    {
        SetCombatMovement(true);

        m_uiPhase                   = PHASE_NORMAL;

        m_uiBerserkTimer            = 10 * MINUTE * IN_MILLISECONDS;
        m_uiBoneSliceTimer          = 10000;
        m_uiColdflameTimer          = 5000;
        m_uiBoneSpikeTimer          = 12000;
        m_uiBoneStormTimer          = 45000;
        m_uiBoneStormChargeTimer    = 3000;
        m_uiBoneStormColdflameTimer = 1000;

        m_uiChargesCount            = 0;

        m_creature->SetSpeedRate(MOVE_RUN, 1.0f);
        if (m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_BONED, true);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        ScriptedAI::MoveInLineOfSight(pWho);

        if (m_bSaidIntro)
            return;

        DoScriptText(SAY_INTRO, m_creature);
        m_bSaidIntro = true;
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_MARROWGAR, FAIL);
            m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_BONED, false);
        }
    }

    void Aggro(Unit* pWho)
    {
        if(!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_MARROWGAR, IN_PROGRESS);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_SLAY_1 - urand(0, 1),m_creature,pVictim);
    }

    void JustDied(Unit *killer)
    {
        if(m_pInstance)
            m_pInstance->SetData(TYPE_MARROWGAR, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void JustSummoned(Creature *pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_COLDFLAME)
        {
            float x, y, z = pSummoned->GetPositionZ();
            m_creature->GetNearPoint2D(x, y, 120.0f, m_creature->GetAngle(pSummoned));

            pSummoned->SetInCombatWithZone();
            pSummoned->CastSpell(pSummoned, SPELL_COLDFLAME_AURA, true);
            pSummoned->SetSpeedRate(MOVE_WALK, 2.0f); // should be via DB
            pSummoned->GetMotionMaster()->MovePoint(0, x, y, z, false);
        }
    }

    // hacky way
    void DoSummonSingleColdFlame()
    {
        if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
        {
            float summon_x, summon_y, summon_z, ang;
            ang = m_creature->GetAngle(pTarget);
            m_creature->GetNearPoint(m_creature, summon_x, summon_y, summon_z, m_creature->GetObjectBoundingRadius(), 5.0f, ang);

            m_creature->SummonCreature(NPC_COLDFLAME, summon_x, summon_y, summon_z, ang, TEMPSUMMON_TIMED_DESPAWN, 12000);
        }
    }

    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if (uiType != POINT_MOTION_TYPE)
            return;

        if (uiPointId == POINT_CHARGE)
        {
            m_uiPhase = PHASE_BONE_STORM_COLDFLAME;
            ++m_uiChargesCount;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Berserk
        if (m_uiBerserkTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED))
            {
                m_uiBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
                DoScriptText(SAY_BERSERK, m_creature);
            }
        }
        else
            m_uiBerserkTimer -= uiDiff;

        switch (m_uiPhase)
        {
            case PHASE_NORMAL:
            {
                // Coldflame (regular)
                if (m_uiColdflameTimer <= uiDiff)
                {
                    //if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_COLDFLAME) == CAST_OK)
                    DoSummonSingleColdFlame();
                    m_uiColdflameTimer = 5000;
                }
                else
                    m_uiColdflameTimer -= uiDiff;

                // Bone Storm
                if (m_uiBoneStormTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_BONE_STORM) == CAST_OK)
                    {
                        SetCombatMovement(false);
                        m_uiBoneStormTimer = 90000;
                        m_uiPhase = PHASE_BONE_STORM_CHARGE;
                        m_creature->SetSpeedRate(MOVE_RUN, 3.0f);
                        DoScriptText(SAY_BONE_STORM, m_creature);
                    }
                }
                else
                    m_uiBoneStormTimer -= uiDiff;

                // Bone Slice
                if (m_uiBoneSliceTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_BONE_SLICE_10) == CAST_OK)
                        m_uiBoneSliceTimer = 1000;
                }
                else
                    m_uiBoneSliceTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            case PHASE_BONE_STORM_CHARGE:
            {
                // next charge to random enemy
                if (m_uiBoneStormChargeTimer <= uiDiff)
                {
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        float x, y, z;
                        pTarget->GetPosition(x, y, z);
                        m_creature->GetMotionMaster()->MovePoint(POINT_CHARGE, x, y, z);
                        m_uiBoneStormChargeTimer = 3000;
                        m_uiPhase = PHASE_BONE_STORM_CHARGING;
                    }
                }
                else
                    m_uiBoneStormChargeTimer -= uiDiff;

                break;
            }
            case PHASE_BONE_STORM_CHARGING: // waiting to arrive at target position
            {
                break;
            }
            case PHASE_BONE_STORM_COLDFLAME:
            {
                if (m_uiBoneStormColdflameTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_COLDFLAME_X) == CAST_OK)
                    {
                        m_uiBoneStormColdflameTimer = 1000;

                        if (m_uiChargesCount >= m_uiMaxCharges)
                        {
                            m_creature->RemoveAurasDueToSpell(SPELL_BONE_STORM);
                            m_creature->InterruptSpell(CURRENT_MELEE_SPELL); // Bone Slice related
                            m_uiBoneSliceTimer = 10000;
                            SetCombatMovement(true);
                            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                            m_uiChargesCount = 0;
                            m_uiPhase = PHASE_NORMAL;
                            m_creature->SetSpeedRate(MOVE_RUN, 1.0f);
                            return;
                        }
                        else
                            m_uiPhase = PHASE_BONE_STORM_CHARGE;
                    }
                }
                else
                    m_uiBoneStormColdflameTimer -= uiDiff;

                break;
            }
        }

        // Bone Spike - on heroic in every phase
        if (m_bIsHeroic || m_uiPhase == PHASE_NORMAL)
        {
            if (m_uiBoneSpikeTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BONE_SPIKE_STORM_10) == CAST_OK)
                {
                    m_uiBoneSpikeTimer = urand(20000, 30000);
                    DoScriptText(SAY_BONE_SPIKE_1 - urand(0, 2), m_creature);
                }
            }
            else
                m_uiBoneSpikeTimer -= uiDiff;
        }
    }
};

/*####
# Coldflame
####*/
struct MANGOS_DLL_DECL mob_coldflameAI : public ScriptedAI
{
    mob_coldflameAI(Creature *pCreature) : ScriptedAI(pCreature){}
    void Reset(){}
    void AttackStart(Unit *who){}
    void UpdateAI(const uint32 uiDiff){}
};

/*####
# Bone Spike
####*/
struct MANGOS_DLL_DECL mob_bone_spikeAI : public ScriptedAI
{
    mob_bone_spikeAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((instance_icecrown_spire*)pCreature->GetInstanceData());
        m_victimGuid.Clear();
        m_bEmerged = false;
        SetCombatMovement(false);
    }

    instance_icecrown_spire* m_pInstance;
    bool m_bEmerged;
    ObjectGuid m_victimGuid;
    uint32 m_uiEmpaledTime;

    void Reset()
    {
        m_uiEmpaledTime = 0;
    }
    void AttackStart(Unit *pWho){}

    void JustDied(Unit *Killer)
    {
        if (Unit *pCreator = m_creature->GetCreator())
        {
            pCreator->RemoveAurasDueToSpell(SPELL_IMPALED);
            m_creature->ForcedDespawn();
        }

        if (m_pInstance && m_uiEmpaledTime > 8000)
            m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_BONED, false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        m_uiEmpaledTime += uiDiff;

        if (!m_bEmerged)
        {
            m_creature->HandleEmote(EMOTE_ONESHOT_EMERGE);
            m_bEmerged = true;
        }
    }
};

CreatureAI* GetAI_mob_bone_spike(Creature* pCreature)
{
    return new mob_bone_spikeAI(pCreature);
}

CreatureAI* GetAI_mob_coldflame(Creature* pCreature)
{
    return new mob_coldflameAI(pCreature);
}

CreatureAI* GetAI_boss_lord_marrowgar(Creature* pCreature)
{
    return new boss_lord_marrowgarAI(pCreature);
}

void AddSC_boss_lord_marrowgar()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_lord_marrowgar";
    newscript->GetAI = &GetAI_boss_lord_marrowgar;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_coldflame";
    newscript->GetAI = &GetAI_mob_coldflame;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_bone_spike";
    newscript->GetAI = &GetAI_mob_bone_spike;
    newscript->RegisterSelf();

}
