/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_freya
SD%Complete:
SDComment: Achiements: Con-speed-atory, Deforestation
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    /* YELLS */
    // freya
    SAY_AGGRO               = -1603140,
    SAY_AGGRO_HARD          = -1603141,
    SAY_ADDS_CONSERVATOR    = -1603142,
    SAY_ADDS_TRIO           = -1603143,
    SAY_ADDS_LASHER         = -1603144,
    SAY_SLAY1               = -1603145,
    SAY_SLAY2               = -1603146,
    SAY_DEATH               = -1603147,
    SAY_BERSERK             = -1603148,
    EMOTE_ALLIES_NATURE     = -1603150,
    EMOTE_LIFEBINDERS_GIFT  = -1603151,
    EMOTE_GROUND_TREMMOR    = -1603152,
    EMOTE_IRON_ROOTS        = -1603153,
    //brightleaf
    SAY_BRIGHTLEAF_AGGRO    = -1603160,
    SAY_BRIGHTLEAF_SLAY1    = -1603161,
    SAY_BRIGHTLEAF_SLAY2    = -1603162,
    SAY_BRIGHTLEAF_DEATH    = -1603163,
    //Ironbranch
    SAY_IRONBRANCH_AGGRO    = -1603170,
    SAY_IRONBRANCH_SLAY1    = -1603171,
    SAY_IRONBRANCH_SLAY2    = -1603172,
    SAY_IRONBRANCH_DEATH    = -1603173,
    //Stonebark
    SAY_STONEBARK_AGGRO     = -1603180,
    SAY_STONEBARK_SLAY1     = -1603181,
    SAY_STONEBARK_SLAY2     = -1603182,
    SAY_STONEBARK_DEATH     = -1603183,

    SPELL_FREYA_CREDIT      = 65074,

    /* BOSS SPELLS */
    SPELL_ATTUNED_TO_NATURE             = 62519, //increases healing, start at 150 stacks
    SPELL_ATTUNED_10_STACKS             = 62525,
    SPELL_ATTUNED_2_STACKS              = 62524,
    SPELL_ATTUNED_25_STACKS             = 62521,
    SPELL_TOUCH_OF_EONAR                = 62528, //heals Freya, 6k per second
    SPELL_TOUCH_OF_EONAR_H              = 62892, //heals Freya, 24k per second
    SPELL_SUNBEAM                       = 62623,
    SPELL_SUNBEAM_H                     = 62872,
    SPELL_BERSERK                       = 47008, // 10 min
    SPELL_NATURE_BOMB                   = 64604, // 5 Targets (Core)

    /* HARD MODE SPELLS */
    SPELL_DRAINED_OF_POWER              = 62467,
    // brightleaf
    SPELL_BRIGHTLEAF_ESSENCE_CHANNEL    = 62485,
    SPELL_BRIGHTLEAF_ESSENCE_CHANNEL_H  = 65587,

    SPELL_UNSTABLE_ENERGY_FREYA         = 62451,
    SPELL_UNSTABLE_ENERGY_FREYA_H       = 62865,
    
    // ironbrach
    SPELL_IRONBRANCH_ESSENCE_CHANNEL    = 62484,
    SPELL_IRONBRANCH_ESSENCE_CHANNEL_H  = 65588,

    SPELL_STRENGHTEN_IRON_ROOTS         = 63601,
    NPC_STRENGHENED_IRON_ROOTS          = 33168,
    SPELL_IRON_ROOTS_FREYA              = 62438,
    SPELL_IRON_ROOTS_FREYA_H            = 62861,

    // stonebark
    SPELL_STONEBARK_ESSEMCE_CHANNEL     = 62483,
    SPELL_STONEBARK_ESSEMCE_CHANNEL_H   = 65589,
    

    SPELL_GROUND_TREMOR_FREYA           = 62437,
    SPELL_GROUND_TREMOR_FREYA_H         = 62859,
    SPELL_STONEBARKS_ESSENCE            = 65590,
    SPELL_STONEBARKS_ESSENCE_H          = 62386,
    SPELL_EFFECT_STONEBARK              = 63295,

    NPC_SUN_BEAM                        = 33170,
    NPC_UNSTABLE_SUN_BEAM               = 33050,

    // sanctuary adds 
    NPC_EONARS_GIFT                     = 33228,
    SPELL_LIFEBINDERS_GIFT              = 62584,    // after 12 secs, heals Freya & her allies for 30%
    SPELL_LIFEBINDERS_GIFT_H            = 64185,    // the same but for 60%
    SPELL_PHEROMONES                    = 62619,    // protects from conservators grip
    NPC_HEALTHY_SPORE                   = 33215,

    /* ADDS */
    // 6 waves of adds. 1 of the 3 each min 
    NPC_DETONATING_LASHER       = 32918,    // recude 2 stacks
    // spells
    SPELL_FLAME_LASH            = 62608,
    SPELL_DETONATE              = 62598,
    SPELL_DETONATE_H            = 62937, 

    NPC_ANCIENT_CONSERVATOR     = 33203,    // reduce 30 stacks
    //spells
    SPELL_CONSERVATORS_GRIP     = 62532,
    SPELL_NATURES_FURY          = 62589,
    SPELL_NATURES_FURY_H        = 63571,

    /* elemental adds */                    // each one reduces 10 stacks
    NPC_WATER_SPIRIT            = 33202,
    // spells
    SPELL_TIDAL_WAVE            = 62653,
    SPELL_TIDAL_WAVE_H          = 62935,

    NPC_STORM_LASHER            = 32919,
    // spells
    SPELL_STORMBOLT             = 62649,
    SPELL_STORMBOLT_H           = 62938,
    SPELL_LIGHTNING_LASH        = 62648,    // 3 targets
    SPELL_LIGHTNING_LASH_H      = 62939,    // 5 targets

    NPC_SNAPLASHER              = 32916,
    // spells
    SPELL_HARDENED_BARK         = 62664,
    SPELL_HARDENED_BARK_H       = 64191,

    // nature bomb
    SPELL_SUMMON_GO_NATURE_BOMB = 64600,
    NPC_NATURE_BOMB             = 34129,
    GO_NATURE_BOMB              = 194902,
    SPELL_NATURE_BOMB_EX        = 64587,
    SPELL_NATURE_BOMB_EX_H      = 64650,

    /* ELDERS */                            // used in phase 1
    ELDER_BRIGHTLEAF            = 32915,
    ELDER_IRONBRANCH            = 32913,
    ELDER_STONEBARK             = 32914,

    // brightleaf spells
    SPELL_BRIGHTLEAF_FLUX       = 62262,
    SPELL_SOLAR_FLARE           = 62240,
    SPELL_SOLAR_FLARE_H         = 62920,
    SPELL_UNSTABLE_SUN_BEAM     = 62221,
    SPELL_UNSTABLE_SUN_BEAM_A   = 62211,
    SPELL_UNSTABLE_SUN_BEAM_BUFF= 62243,
    SPELL_UNSTABLE_ENERGY       = 62217,    // cancels sun bean 
    SPELL_UNSTABLE_ENERGY_H     = 62922,
    SPELL_PHOTOSYNTHESIS        = 62209,

    // ironbrach spells
    SPELL_IMPALE                = 62310,
    SPELL_IMPALE_H              = 62928,
    SPELL_IRON_ROOTS            = 62283, 
    SPELL_IRON_ROOTS_H          = 62930,
    NPC_IRON_ROOTS              = 33088,
    SPELL_THORN_SWARM           = 62285,
    SPELL_THORN_SWARM_H         = 62931,

    // stonebark spells
    SPELL_FIST_OF_STONE         = 62344,
    SPELL_BROKEN_BONES          = 62356,
    SPELL_GROUND_TREMOR         = 62325,
    SPELL_GROUND_TREMOR_H       = 62932,
    SPELL_PETRIFIED_BARK        = 62337,
    SPELL_PETRIFIED_BARK_H      = 62933,

    // not used because summoned chest doesn't despawn after looted
    SPELL_SUMMON_CHEST_1                = 62950,
    SPELL_SUMMON_CHEST_2                = 62952,
    SPELL_SUMMON_CHEST_3                = 62953,
    SPELL_SUMMON_CHEST_4                = 62954,
    SPELL_SUMMON_CHEST_5                = 62955,
    SPELL_SUMMON_CHEST_6                = 62956,
    SPELL_SUMMON_CHEST_7                = 62957,
    SPELL_SUMMON_CHEST_8                = 62958,

    SPELL_SUMMON_ALLIES_OF_NATURE       = 62678, // better do that in sd2
    SPELL_SUMMON_LASHERS                = 62688, // lashers - broken
    SPELL_SUMMON_ELEMENTALS             = 62686, // elementals -> better in spell
    SPELL_SUMMON_CONSERVATOR            = 62685, // conservator
    SPELL_LIFEBINDERS_GIFT_SUMMON       = 62572, // with missle

    SPELL_SPORE_SUMMON_NE               = 62591,
    SPELL_SPORE_SUMMON_SE               = 62592,
    SPELL_SPORE_SUMMON_SW               = 62593,
    SPELL_SPORE_SUMMON_NW               = 62582,

    SPELL_HEALTHY_SPORE_VISUAL          = 62538,
    SPELL_LIFEBINDERS_VISUAL            = 62579,
    SPELL_GROW                          = 44833,

    SPELL_PHEROMONES_LG                 = 62619,
    SPELL_POTENT_PHEROMONES             = 62541,
};

// Iron roots & stranghned iron roots
struct MANGOS_DLL_DECL mob_iron_rootsAI : public ScriptedAI
{
    mob_iron_rootsAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_uiCreatureEntry = m_creature->GetEntry();
        m_creature->setFaction(16);
        SetCombatMovement(false);
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    uint32 m_uiCreatureEntry;

    void Reset(){}

    void KilledUnit(Unit* pVictim)
    {
        m_creature->ForcedDespawn(500);
    }

    void JustDied(Unit *pKiller)
    {
        if (!m_pInstance)
            return;

        if (Unit* pVictim = m_pInstance->instance->GetUnit(m_creature->GetCreatorGuid()))
        {
            switch(m_uiCreatureEntry)
            {
                case NPC_IRON_ROOTS:
                    pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS : SPELL_IRON_ROOTS_H);
                    break;
                case NPC_STRENGHENED_IRON_ROOTS:
                    pVictim->RemoveAurasDueToSpell(m_bIsRegularMode ? SPELL_IRON_ROOTS_FREYA : SPELL_IRON_ROOTS_FREYA_H);
                    break;
            }
        }
        m_creature->ForcedDespawn(500);
    }

    void UpdateAI(const uint32 uiuiDiff){}
};

// Elder Brightleaf
struct MANGOS_DLL_DECL boss_elder_brightleafAI : public ScriptedAI
{
    boss_elder_brightleafAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    uint32 m_uiBrightleafFluxTimer;
    uint32 m_uiSolarFlareTimer;
    uint32 m_uiUnstableSunBeanTimer;
    uint32 m_uiUnstabelEnergyTimer;
    uint32 m_uiHealTimer;
    bool m_bHasSunbeam;

    void Reset()
    {
        m_uiBrightleafFluxTimer     = 5000;
        m_uiSolarFlareTimer         = 10000 + urand(1000, 5000);
        m_uiUnstableSunBeanTimer    = 15000;
        m_uiUnstabelEnergyTimer     = 30000;
        m_bHasSunbeam               = false;
    }

    void Aggro(Unit* pWho)
    {    
        DoScriptText(SAY_BRIGHTLEAF_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_BRIGHTLEAF_SLAY1 : SAY_BRIGHTLEAF_SLAY2, m_creature);
    }

    void JustDied(Unit *killer)
    {
        DoScriptText(SAY_BRIGHTLEAF_DEATH, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiBrightleafFluxTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BRIGHTLEAF_FLUX) == CAST_OK)
                m_uiBrightleafFluxTimer = 5000;
        }
        else
            m_uiBrightleafFluxTimer -= uiDiff;

        if(m_uiSolarFlareTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SOLAR_FLARE : SPELL_SOLAR_FLARE_H) == CAST_OK)
                    m_uiSolarFlareTimer = 10000 + urand(1000, 5000);
            }
        }
        else
            m_uiSolarFlareTimer -= uiDiff;

        if(m_uiUnstableSunBeanTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_UNSTABLE_SUN_BEAM) == CAST_OK)
                m_uiUnstableSunBeanTimer = urand(7000, 12000);
        }
        else
            m_uiUnstableSunBeanTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_elder_brightleaf(Creature* pCreature)
{
    return new boss_elder_brightleafAI(pCreature);
}

// Elder Ironbranch
struct MANGOS_DLL_DECL boss_elder_ironbranchAI : public ScriptedAI
{
    boss_elder_ironbranchAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    uint32 m_uiImpaleTimer;
    uint32 m_uiIronrootsTimer;
    uint32 m_uiThornSwarmTimer;

    void Reset()
    {
        m_uiImpaleTimer         = 10000 + urand (1000, 5000);
        m_uiIronrootsTimer      = 20000 + urand (1000, 7000);
        m_uiThornSwarmTimer     = 30000;
    }

    void Aggro(Unit* pWho)
    {    
        DoScriptText(SAY_IRONBRANCH_AGGRO, m_creature);
    }

    void JustDied(Unit *killer)
    {
        DoScriptText(SAY_IRONBRANCH_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_IRONBRANCH_SLAY1 : SAY_IRONBRANCH_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiImpaleTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H) == CAST_OK)
                    m_uiImpaleTimer = 10000 + urand (1000, 5000);
            }
        }
        else
            m_uiImpaleTimer -= uiDiff;

        if(m_uiIronrootsTimer < uiDiff)
        {
            if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                target->CastSpell(target, m_bIsRegularMode ? SPELL_IRON_ROOTS : SPELL_IRON_ROOTS_H, true);
            m_uiIronrootsTimer = 20000 + urand (1000, 7000);
        }
        else
            m_uiIronrootsTimer -= uiDiff;

        if(m_uiThornSwarmTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_THORN_SWARM : SPELL_THORN_SWARM_H) == CAST_OK)
                m_uiThornSwarmTimer = 30000;
        }
        else
            m_uiThornSwarmTimer -= uiDiff;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_boss_elder_ironbranch(Creature* pCreature)
{
    return new boss_elder_ironbranchAI(pCreature);
}

// Stonebark
struct MANGOS_DLL_DECL boss_elder_stonebarkAI : public ScriptedAI
{
    boss_elder_stonebarkAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    uint32 m_uiFistsOfStoneTimer;
    uint32 m_uiGroundTremorTimer;
    uint32 m_uiPetrifiedBarkTimer;

    void Reset()
    {
        m_uiFistsOfStoneTimer   = 20000;
        m_uiGroundTremorTimer   = 15000;
        m_uiPetrifiedBarkTimer  = 25000;
    }

    void Aggro(Unit* pWho)
    {    
        DoScriptText(SAY_STONEBARK_AGGRO, m_creature);
    }

    void JustDied(Unit *killer)
    {
        DoScriptText(SAY_STONEBARK_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_STONEBARK_SLAY1 : SAY_STONEBARK_SLAY2, m_creature);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if(m_uiFistsOfStoneTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_FIST_OF_STONE) == CAST_OK)
                m_uiFistsOfStoneTimer = 30000;
        }
        else
            m_uiFistsOfStoneTimer -= uiDiff;

        if(m_uiGroundTremorTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_GROUND_TREMOR : SPELL_GROUND_TREMOR_H) == CAST_OK)
                m_uiGroundTremorTimer = 15000 + urand (1000, 5000);
        }
        else
            m_uiGroundTremorTimer -= uiDiff;

        if(m_uiPetrifiedBarkTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_PETRIFIED_BARK : SPELL_PETRIFIED_BARK_H) == CAST_OK)
                m_uiPetrifiedBarkTimer = 20000 + urand (1000, 5000);
        }
        else
            m_uiPetrifiedBarkTimer -= uiDiff;

        DoMeleeAttackIfReady(); 
    }
};

CreatureAI* GetAI_boss_elder_stonebark(Creature* pCreature)
{
    return new boss_elder_stonebarkAI(pCreature);
}

// Freya
struct MANGOS_DLL_DECL boss_freyaAI : public ScriptedAI
{
    boss_freyaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSummonTimer;
    uint32 m_uiWaveNumber;
    uint32 m_uiWaveType;
    uint32 m_uiWaveTypeInc;
    uint32 m_uiSunbeamTimer;
    uint32 m_uiEnrageTimer;

    bool m_bIsHardMode;
    
    uint32 m_uiNatureBombTimer;
    uint32 m_uiLifebindersGiftTimer;

    bool m_bIsOutro;
    uint32 m_uiOutroTimer;
    uint32 m_uiStep;

    // hard mode timers
    uint32 m_uiUnstableEnergyTimer;
    uint32 m_uiStrenghtenIronRootsTimer;
    uint32 m_uiGroundTremorTimer;

    uint32 m_uiThreeWaveCheckTimer;
    uint32 m_uiThreeWaveRespawnTimer;
    bool m_bThreeWaveCheckTimerStarted;
    bool m_bWaveCheck;
    ObjectGuid m_uiWaterSpiritGUID;
    ObjectGuid m_uiStormLasherGUID;
    ObjectGuid m_uiSnapLasherGUID;

    bool m_bIsBrightleafAlive;
    bool m_bIsIronbranchAlive;
    bool m_bIsStonebarkAlive;

    uint32 m_uiAchievProgress;

    void Reset()
    {
        m_uiSummonTimer                 = 15000;
        m_uiWaveNumber                  = 0;
        m_uiWaveType                    = urand(0,2);
        m_uiWaveTypeInc                 = urand(1,2);
        m_uiSunbeamTimer                = rand()%10000;
        m_uiEnrageTimer                 = 600000; //10 minutes
        m_bIsHardMode                   = false;
        m_uiLifebindersGiftTimer        = 30000;
        m_uiUnstableEnergyTimer         = 25000;
        m_uiStrenghtenIronRootsTimer    = 25000 + urand(1000, 5000);
        m_uiGroundTremorTimer           = 20000;
        m_uiNatureBombTimer             = 7000;
        m_uiThreeWaveCheckTimer         = 1000;
        m_bWaveCheck                    = false;
        m_bThreeWaveCheckTimerStarted   = false;
        m_uiThreeWaveRespawnTimer       = 12000;

        m_uiOutroTimer                  = 10000;
        m_uiStep                        = 1;
        m_bIsOutro                      = false;

        m_uiAchievProgress              = 0;

        if(m_pInstance) 
        {
            // remove elder auras
            if (Creature* pBrightleaf = m_pInstance->GetSingleCreatureFromStorage(NPC_BRIGHTLEAF))
            {
                if (pBrightleaf->isAlive())
                    pBrightleaf->RemoveAllAuras();
            }
            if (Creature* pIronbranch = m_pInstance->GetSingleCreatureFromStorage(NPC_IRONBRACH))
            {
                if (pIronbranch->isAlive())
                    pIronbranch->RemoveAllAuras();
            }
            if (Creature* pStonebark = m_pInstance->GetSingleCreatureFromStorage(NPC_STONEBARK))
            {
                if (pStonebark->isAlive())
                    pStonebark->RemoveAllAuras();
            }
        }
    }

    void Aggro(Unit *who) 
    {
        DoCast(m_creature, SPELL_ATTUNED_TO_NATURE, true);
        DoCast(m_creature, m_bIsRegularMode ? SPELL_TOUCH_OF_EONAR : SPELL_TOUCH_OF_EONAR_H, true);
        if(m_pInstance) 
        {
            m_pInstance->SetData(TYPE_FREYA, IN_PROGRESS);

            // check brightleaf
            if (Creature* pBrightleaf = m_pInstance->GetSingleCreatureFromStorage(NPC_BRIGHTLEAF))
            {
                if (pBrightleaf->isAlive())
                {
                    pBrightleaf->CastSpell(pBrightleaf, SPELL_DRAINED_OF_POWER, true);
                    pBrightleaf->CastSpell(m_creature, m_bIsRegularMode ? SPELL_BRIGHTLEAF_ESSENCE_CHANNEL : SPELL_BRIGHTLEAF_ESSENCE_CHANNEL_H, true);
                    m_bIsBrightleafAlive = true;
                    m_uiAchievProgress++;
                }
                else
                    m_bIsBrightleafAlive = false;
            }

            // check ironbranch
            if (Creature* pIronbranch = m_pInstance->GetSingleCreatureFromStorage(NPC_IRONBRACH))
            {
                if (pIronbranch->isAlive())
                {
                    pIronbranch->CastSpell(pIronbranch, SPELL_DRAINED_OF_POWER, true);
                    pIronbranch->CastSpell(m_creature, m_bIsRegularMode ? SPELL_IRONBRANCH_ESSENCE_CHANNEL : SPELL_IRONBRANCH_ESSENCE_CHANNEL_H, true);
                    m_bIsIronbranchAlive = true;
                    m_uiAchievProgress++;
                }
                else
                    m_bIsIronbranchAlive = false;
            }

            // check stonebark
            if (Creature* pStonebark = m_pInstance->GetSingleCreatureFromStorage(NPC_STONEBARK))
            {
                if (pStonebark->isAlive())
                {
                    pStonebark->CastSpell(pStonebark, SPELL_DRAINED_OF_POWER, true);
                    pStonebark->CastSpell(m_creature, m_bIsRegularMode ? SPELL_STONEBARK_ESSEMCE_CHANNEL : SPELL_STONEBARK_ESSEMCE_CHANNEL_H, true);
                    m_bIsStonebarkAlive = true;
                    m_uiAchievProgress++;
                }
                else
                    m_bIsStonebarkAlive = false;
            }
        }

        m_bIsHardMode = CheckHardMode();

        if(!m_bIsHardMode)
            DoScriptText(SAY_AGGRO, m_creature);
        else
            DoScriptText(SAY_AGGRO_HARD, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_FREYA, FAIL);
            m_pInstance->SetData(TYPE_FREYA_HARD, FAIL);
        }
    }

    void DoOutro()
    {
        if(m_pInstance) 
        {
            switch(m_uiAchievProgress)
            {
                case 3:
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_KNOCK_KNOCK_WOOD, true);
                    m_pInstance->SetData(TYPE_FREYA_HARD, DONE);
                case 2:
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_KNOCK_WOOD, true);
                case 1:
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_WOOD, true);
                    break;
            }
            m_pInstance->SetData(TYPE_FREYA, DONE);
        }
        DoCast(m_creature, SPELL_FREYA_CREDIT, true);
        m_creature->ForcedDespawn();
    }

    // for debug only!
    void JustDied(Unit* pKiller)
    {
        if(m_pInstance) 
        {
            switch(m_uiAchievProgress)
            {
                case 3:
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_KNOCK_KNOCK_WOOD, true);
                    m_pInstance->SetData(TYPE_FREYA_HARD, DONE);
                case 2:
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_KNOCK_WOOD, true);
                case 1:
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KNOCK_WOOD, true);
                    break;
            }
            m_pInstance->SetData(TYPE_FREYA, DONE);
        }
        DoCast(m_creature, SPELL_FREYA_CREDIT, true);
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
        if(m_creature->GetHealth() < uiDamage || m_creature->GetHealthPercent() < 1.0f)
        {
            uiDamage = 0;
            m_bIsOutro = true;
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    bool CheckHardMode()
    {
        if(m_bIsBrightleafAlive && m_bIsIronbranchAlive && m_bIsStonebarkAlive)
            return true;
        return false;
    }

    // summon 10 Lashers. Should be done by a spell which needs core fix
    void SummonLashers()
    {
        int i;
        float x,y;
        for(i = 0; i < 10; ++i)
        {
            x = (rand_norm() * 30.0f) - 15.0f;
            y = (rand_norm() * 30.0f) - 15.0f;
            if(Creature* pLasher = DoSpawnCreature(NPC_DETONATING_LASHER, x, y, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
            {
                pLasher->setFaction(m_creature->getFaction());
                if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pLasher->AddThreat(pTarget, 1.0f);
            }
        }
    }

    // summon conservator. Should be done by a spell which needs core fix
    void SummonConservator()
    {
        float x = (rand_norm() * 30.0f) - 15.0f;
        float y = (rand_norm() * 30.0f) - 15.0f;
        if(Creature* pAdd = DoSpawnCreature(NPC_ANCIENT_CONSERVATOR, x, y, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
        {
            pAdd->setFaction(m_creature->getFaction());
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pAdd->AddThreat(pTarget, 1.0f);
        }
    }

    // summmon the 3 elementals. Should be done by a spell which needs core fix.
    void SummonElementals()
    {
        if(Creature* pSpirit = DoSpawnCreature(NPC_WATER_SPIRIT, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7*DAY*IN_MILLISECONDS))
        {
            pSpirit->setFaction(m_creature->getFaction());
            m_uiWaterSpiritGUID = pSpirit->GetObjectGuid();
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSpirit->AddThreat(pTarget, 1.0f);
        }

        if(Creature* pStormLasher = DoSpawnCreature(NPC_STORM_LASHER, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7*DAY*IN_MILLISECONDS))
        {
            pStormLasher->setFaction(m_creature->getFaction());
            m_uiStormLasherGUID = pStormLasher->GetObjectGuid();
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pStormLasher->AddThreat(pTarget, 1.0f);
        }

        if(Creature* pSnapLasher = DoSpawnCreature(NPC_SNAPLASHER, 0, 0, 0, 0, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 7*DAY*IN_MILLISECONDS))
        {
            pSnapLasher->setFaction(m_creature->getFaction());
            m_uiSnapLasherGUID = pSnapLasher->GetObjectGuid();
            if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                pSnapLasher->AddThreat(pTarget, 1.0f);
        }
    }

    void initNextWave()
    {
        m_uiWaveType = (m_uiWaveType + m_uiWaveTypeInc) % 3;
        ++m_uiWaveNumber;
        m_uiSummonTimer = 60000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(!m_bIsOutro)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            // check if the 3 elementals die at the same time
            if (m_bWaveCheck)
            {
                if(m_uiThreeWaveCheckTimer < uiDiff)
                {
                    Creature* pWaterSpirit = m_creature->GetMap()->GetCreature(m_uiWaterSpiritGUID);
                    Creature* pStormLasher = m_creature->GetMap()->GetCreature(m_uiStormLasherGUID);
                    Creature* pSnapLasher = m_creature->GetMap()->GetCreature(m_uiSnapLasherGUID);

                    if(pWaterSpirit && pStormLasher && pSnapLasher)
                    {
                        if(!pWaterSpirit->isAlive() && !pStormLasher->isAlive() && !pSnapLasher->isAlive())
                        {
                            m_bWaveCheck = false;
                            m_bThreeWaveCheckTimerStarted = false;

                            m_creature->CastSpell(m_creature, SPELL_ATTUNED_10_STACKS, true);
                            m_creature->CastSpell(m_creature, SPELL_ATTUNED_10_STACKS, true);
                            m_creature->CastSpell(m_creature, SPELL_ATTUNED_10_STACKS, true);
                        }
                        else if (!m_bThreeWaveCheckTimerStarted)
                        {
                            // if at least 1 is alive
                            if (!pWaterSpirit->isAlive() || !pSnapLasher->isAlive() || !pStormLasher->isAlive())
                            {
                                m_bThreeWaveCheckTimerStarted = true;
                                m_uiThreeWaveRespawnTimer = 12000;
                            }
                        }
                    }
                    m_uiThreeWaveCheckTimer = 1000;
                }
                else
                    m_uiThreeWaveCheckTimer -= uiDiff;

                // respawn 3 adds after 12 sec from one's death
                if (m_bThreeWaveCheckTimerStarted)
                {
                    if (m_uiThreeWaveRespawnTimer <= uiDiff)
                    {
                        Creature* pWaterSpirit = m_creature->GetMap()->GetCreature(m_uiWaterSpiritGUID);
                        Creature* pStormLasher = m_creature->GetMap()->GetCreature(m_uiStormLasherGUID);
                        Creature* pSnapLasher = m_creature->GetMap()->GetCreature(m_uiSnapLasherGUID);

                        if (pWaterSpirit && !pWaterSpirit->isAlive())
                        {
                            pWaterSpirit->Respawn();
                        }
                        if (pStormLasher && !pStormLasher->isAlive())
                        {
                            pStormLasher->Respawn();
                        }
                        if (pSnapLasher && !pSnapLasher->isAlive())
                        {
                            pSnapLasher->Respawn();
                        }
                        m_bThreeWaveCheckTimerStarted = false;
                        m_uiThreeWaveRespawnTimer = 12000;
                    }
                    else
                        m_uiThreeWaveRespawnTimer -= uiDiff;
                }
            }

            // Hardmode
            if(m_bIsBrightleafAlive)
            {
                // hacky way, should be done by spell
                if(m_uiUnstableEnergyTimer < uiDiff)
                {
                    //DoCast(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_ENERGY_FREYA : SPELL_UNSTABLE_ENERGY_FREYA_H);
                    for(int8 i = 0; i < 3; ++i)
                    {
                        if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                            m_creature->SummonCreature(NPC_SUN_BEAM, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 20000);
                    }
                    m_uiUnstableEnergyTimer = urand(25000, 30000);
                }
                else
                    m_uiUnstableEnergyTimer -= uiDiff;
            }

            if(m_bIsIronbranchAlive)
            {
                if(m_uiStrenghtenIronRootsTimer < uiDiff)
                {
                    if(Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                    {
                        DoScriptText(EMOTE_IRON_ROOTS, m_creature, target);
                        target->CastSpell(target, m_bIsRegularMode ? SPELL_IRON_ROOTS_FREYA : SPELL_IRON_ROOTS_FREYA_H, true);
                    }
                    m_uiStrenghtenIronRootsTimer = 50000 + urand(10000, 20000);
                }
                else
                    m_uiStrenghtenIronRootsTimer -= uiDiff;
            }

            if(m_bIsStonebarkAlive)
            {
                if(m_uiGroundTremorTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_GROUND_TREMOR_FREYA : SPELL_GROUND_TREMOR_FREYA_H) == CAST_OK)
                    {
                        DoScriptText(EMOTE_GROUND_TREMMOR, m_creature);
                        m_uiGroundTremorTimer = 20000;
                    }
                }
                else
                    m_uiGroundTremorTimer -= uiDiff;
            }

            //Phase 1, waves of adds
            if(m_uiWaveNumber < 6)
            {
                if(m_uiSummonTimer < uiDiff)
                {
                    DoScriptText(EMOTE_ALLIES_NATURE, m_creature);
                    switch(m_uiWaveType)
                    {
                        case 0:
                            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_LASHERS) == CAST_OK)
                            {
                                /*SummonLashers();*/ 
                                DoScriptText(SAY_ADDS_LASHER, m_creature);
                                initNextWave();
                            }
                            break;
                        case 1:
                            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_CONSERVATOR) == CAST_OK)
                            {
                                /*SummonConservator();*/
                                DoScriptText(SAY_ADDS_CONSERVATOR, m_creature);
                                initNextWave();
                            }
                            break;
                        case 2:
                            //DoCast(m_creature, SPELL_SUMMON_ELEMENTALS); 
                            SummonElementals();
                            DoScriptText(SAY_ADDS_TRIO, m_creature);
                            m_bWaveCheck = true;
                            m_uiThreeWaveCheckTimer = 1000;
                            initNextWave();
                            break;
                        default:
                            break;
                    }

                }
                else
                    m_uiSummonTimer -= uiDiff;
            }
            // Phase 2
            else
            {
                if(m_uiNatureBombTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_NATURE_BOMB) == CAST_OK)
                    {
                        m_uiNatureBombTimer = urand(7000, 12000);
                    }
                }
                else
                    m_uiNatureBombTimer -= uiDiff;
            }

            //All phases
            if(m_uiSunbeamTimer < uiDiff)
            {
                if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, (uint32)0, SELECT_FLAG_PLAYER))
                {
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_SUNBEAM : SPELL_SUNBEAM_H) == CAST_OK)
                    {
                        m_uiSunbeamTimer = 6000 + rand()%10000;
                    }
                }
            }
            else
                m_uiSunbeamTimer -= uiDiff;

            if(m_uiLifebindersGiftTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_LIFEBINDERS_GIFT_SUMMON) == CAST_OK)
                {
                    DoScriptText(EMOTE_LIFEBINDERS_GIFT, m_creature);
                    m_uiLifebindersGiftTimer = 30000;
                }
            }
            else
                m_uiLifebindersGiftTimer -= uiDiff;

            if(m_uiEnrageTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT) == CAST_OK)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    m_uiEnrageTimer = 30000;
                }
            }
            else
                m_uiEnrageTimer -= uiDiff;

            DoMeleeAttackIfReady();
        }

        // outro
        if(m_bIsOutro)
        {
            switch(m_uiStep)
            {
                case 1:
                    if(SpellAuraHolder* pTempHolder = m_creature->GetSpellAuraHolder(SPELL_ATTUNED_TO_NATURE))
                    {
                        if(pTempHolder->GetStackAmount() >= 25)
                        {
                            if (m_pInstance)
                                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_BACK_TO_NATURE, true);
                        }
                    }
                    m_creature->setFaction(35);
                    m_creature->RemoveAllAuras();
                    m_creature->DeleteThreatList();
                    m_creature->CombatStop(true);
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                    m_creature->GetMotionMaster()->MovePoint(0, 2359.40f, -52.39f, 425.64f);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    ++m_uiStep;
                    m_uiOutroTimer = 7000;
                    break;
                case 3:
                    DoScriptText(SAY_DEATH, m_creature);
                    ++m_uiStep;
                    m_uiOutroTimer = 10000;
                    break;
                case 5:
                    DoOutro();
                    ++m_uiStep;
                    m_uiOutroTimer = 10000;
                    break;
            }
        }
        else
            return;

        if (m_uiOutroTimer <= uiDiff)
        {
            ++m_uiStep;
            m_uiOutroTimer = 330000;
        } m_uiOutroTimer -= uiDiff;
    }
};

CreatureAI* GetAI_boss_freya(Creature* pCreature)
{
    return new boss_freyaAI(pCreature);
}

// Script for all the npcs found on the ground during Freya encounter
struct MANGOS_DLL_DECL mob_freya_groundAI : public ScriptedAI
{
     mob_freya_groundAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiNatureBomb;
    uint32 m_uiOpenNatureBomb;
    uint32 m_uiDieTimer;
    uint32 m_uiEonarsGift;
    uint32 m_uiNonSelectable;
    uint32 m_uiGrow_Timer;
    uint32 m_uiUnstableEnergy;
    uint32 m_uiHealthyGrow;
    float m_fSize;

    bool m_bHasGrow;

    void Reset()
    {
        m_uiNatureBomb          = urand(9000,11000);
        m_uiOpenNatureBomb      = m_uiNatureBomb - 2000;
        m_uiDieTimer            = 60000;
        m_uiEonarsGift          = urand(11000,13000);
        m_uiNonSelectable       = 5000;
        m_uiUnstableEnergy      = 10500;
        m_uiGrow_Timer          = 0;
        m_bHasGrow              = true;
        m_uiHealthyGrow         = urand(3000,12000);

        // the invisible displayIds should be set in DB.
        switch(m_creature->GetEntry())
        {
            case NPC_NATURE_BOMB:
                m_creature->setFaction(14);
                m_fSize = 1;
                m_creature->SetDisplayId(25865);     // invisible
                DoCast(m_creature, SPELL_SUMMON_GO_NATURE_BOMB ,true);
                break;
            case NPC_EONARS_GIFT:
                m_fSize = float(0.1);
                m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, m_fSize);
                DoCast(m_creature, SPELL_LIFEBINDERS_VISUAL);
                break;
            case NPC_HEALTHY_SPORE:
                DoCast(m_creature, SPELL_HEALTHY_SPORE_VISUAL, true);
                DoCast(m_creature, SPELL_POTENT_PHEROMONES, true);
                break;
            case NPC_SUN_BEAM:
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetDisplayId(25865);     // invisible
                DoCast(m_creature, SPELL_LIFEBINDERS_VISUAL);
                DoCast(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_ENERGY_FREYA : SPELL_UNSTABLE_ENERGY_FREYA_H);
                DoCast(m_creature, SPELL_PHOTOSYNTHESIS, true);
                m_creature->ForcedDespawn(11000);
                break;
            case NPC_UNSTABLE_SUN_BEAM:
                m_creature->SetDisplayId(25865);     // invisible
                DoCast(m_creature, SPELL_LIFEBINDERS_VISUAL, true);
                DoCast(m_creature, SPELL_PHOTOSYNTHESIS, true);
                DoCast(m_creature, SPELL_UNSTABLE_SUN_BEAM_A, true);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->ForcedDespawn(11000);
                break;
        }

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetRespawnDelay(DAY);           
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }
    
    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        if (m_creature->GetEntry() == NPC_HEALTHY_SPORE)
            uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_FREYA) != IN_PROGRESS) 
        {
            m_creature->ForcedDespawn();
        }

        if(!m_creature->isAlive())
            return;

        switch(m_creature->GetEntry())
        {
            case NPC_NATURE_BOMB:
            {
                if(m_uiOpenNatureBomb < uiDiff)
                {
                    if (GameObject* pNaturBomb = GetClosestGameObjectWithEntry(m_creature, GO_NATURE_BOMB, 10.0f))
                    {
                        pNaturBomb->Use(m_creature);
                    }
                    m_uiOpenNatureBomb = 5000;
                }
                else
                    m_uiOpenNatureBomb -= uiDiff;

                if(m_uiNatureBomb < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_NATURE_BOMB_EX : SPELL_NATURE_BOMB_EX_H) == CAST_OK)
                    {
                        if (GameObject* pNaturBomb = GetClosestGameObjectWithEntry(m_creature, GO_NATURE_BOMB, 10.0f))
                        {
                            pNaturBomb->Delete();
                        }
                        m_creature->ForcedDespawn(1000);
                        m_uiNatureBomb = 3000;
                    }
                }
                else
                    m_uiNatureBomb -= uiDiff;
                break;
            }
            case NPC_EONARS_GIFT:
            {
                if (m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X) < 1.5)
                {
                    DoCast(m_creature, SPELL_GROW, true);
                }

                if(m_uiEonarsGift < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIFEBINDERS_GIFT : SPELL_LIFEBINDERS_GIFT_H) == CAST_OK)
                        m_uiEonarsGift = 5000;
                }
                else
                    m_uiEonarsGift -= uiDiff;

                if(m_uiNonSelectable < uiDiff && m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE))
                {
                    if (DoCastSpellIfCan(m_creature, SPELL_PHEROMONES_LG) == CAST_OK)
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                }
                else
                    m_uiNonSelectable -= uiDiff;
                break;
            }
            case NPC_HEALTHY_SPORE:
            {
                if(!m_bHasGrow && m_fSize < 0.25)
                    m_creature->ForcedDespawn();

                if(m_uiHealthyGrow < uiDiff)
                {
                    if(m_bHasGrow)
                    {
                        m_fSize = float(urand(150,225))/100;
                        m_bHasGrow = false;
                    }
                    else
                        m_fSize = float(urand(1,300))/100;
                    if(m_fSize < 1)
                        m_fSize = 0.1f;
                    m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, m_fSize);
                    m_uiHealthyGrow = urand(3000,5000);
                }
                else
                    m_uiHealthyGrow -= uiDiff;

                break;
            }
            case NPC_UNSTABLE_SUN_BEAM:
            {
                if(m_uiUnstableEnergy < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_UNSTABLE_ENERGY : SPELL_UNSTABLE_ENERGY_H) == CAST_OK)
                        m_uiUnstableEnergy = 10000;
                }
                else
                    m_uiUnstableEnergy -= uiDiff;

                break;
            }
            default:
                break;

        }
    }
};

// Script for Freya's adds
struct MANGOS_DLL_DECL mob_freya_spawnedAI : public ScriptedAI
{
    mob_freya_spawnedAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiTidalWave;
    uint32 m_uiStormbolt;
    uint32 m_uiLightningLash;
    uint32 m_uiFlameLash;
    uint32 m_uiResetAggro;
    uint32 m_uiNaturesFury;
    uint32 m_uiRespawnSpores;

    void Reset()
    {
        m_uiTidalWave         = urand(2000,4000);
        m_uiStormbolt         = 1000;
        m_uiLightningLash     = urand(11000,14000);
        m_uiFlameLash         = urand(5000,10000);
        m_uiResetAggro        = 5000;
        m_uiNaturesFury       = urand(8000,10000);
        m_uiRespawnSpores     = 5000;

        switch(m_creature->GetEntry())
        {
            case NPC_ANCIENT_CONSERVATOR:
            {
                DoSpores(10);
                break;
            }
            case NPC_WATER_SPIRIT:
            case NPC_SNAPLASHER:
            case NPC_STORM_LASHER:
            {
                m_creature->SetRespawnTime(30);
                m_creature->SetCorpseDelay(30);
                m_creature->SetRespawnDelay(30);
                break;
            }
            default:
                break;
        }
    }

    void JustDied(Unit* Killer)
    {
        switch (m_creature->GetEntry())
        {
            case NPC_ANCIENT_CONSERVATOR:
            {
                DoCast(m_creature, SPELL_ATTUNED_25_STACKS, true);
                break;
            }
            case NPC_DETONATING_LASHER:
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_DETONATE : SPELL_DETONATE_H, true);
                DoCast(m_creature, SPELL_ATTUNED_2_STACKS, true);
                break;
            }
            default:
                break;
        }
    }

    void DoSpores(int8 times)
    {
        for(int8 i = 0; i < times; ++i)
        {
            for(int8 itr = 0; i < 3; ++i)
                DoCast(m_creature, SPELL_SPORE_SUMMON_NE + itr, true);
            DoCast(m_creature, SPELL_SPORE_SUMMON_NW, true);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_FREYA) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_creature->GetEntry())
        {
            case NPC_DETONATING_LASHER:
            {
                if(m_uiFlameLash < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FLAME_LASH) == CAST_OK)
                        m_uiFlameLash = urand(5000,10000);
                }
                else
                    m_uiFlameLash -= uiDiff;
            
                if(m_uiResetAggro < uiDiff)
                {
                    DoResetThreat();
                    m_uiResetAggro = 5000;
                }
                else
                    m_uiResetAggro -= uiDiff;

                break;
            }
            case NPC_ANCIENT_CONSERVATOR:
            {
                DoCastSpellIfCan(m_creature, SPELL_CONSERVATORS_GRIP, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
                if(m_uiNaturesFury < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, (uint32)0, SELECT_FLAG_PLAYER))
                    {
                        if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_NATURES_FURY : SPELL_NATURES_FURY_H) == CAST_OK)
                            m_uiNaturesFury = urand(5000,6000);
                    }
                }
                else
                    m_uiNaturesFury -= uiDiff;

                if(m_uiRespawnSpores < uiDiff)
                {
                    DoSpores(3);
                    m_uiRespawnSpores = 5000;
                }
                else
                    m_uiRespawnSpores -= uiDiff;

                break;
            }
            case NPC_WATER_SPIRIT:
            {
                if (m_uiTidalWave < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_TIDAL_WAVE : SPELL_TIDAL_WAVE_H) == CAST_OK)
                        m_uiTidalWave = urand(7000,9000);
                }
                else
                    m_uiTidalWave -= uiDiff;

                break;
            }
            case NPC_STORM_LASHER:
            {
                if (m_uiLightningLash < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_LIGHTNING_LASH : SPELL_LIGHTNING_LASH_H) == CAST_OK)
                        m_uiLightningLash = urand(11000,14000);
                }
                else
                    m_uiLightningLash -= uiDiff;

                if (m_uiStormbolt < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_STORMBOLT : SPELL_STORMBOLT_H) == CAST_OK)
                        m_uiStormbolt = 2000;
                }
                else
                    m_uiStormbolt -= uiDiff;

                break;
            }
            case NPC_SNAPLASHER:
            {
                DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_HARDENED_BARK : SPELL_HARDENED_BARK_H, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
            }
            default:
                break;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_freya_ground(Creature* pCreature)
{
    return new mob_freya_groundAI(pCreature);
}

CreatureAI* GetAI_mob_freya_spawned(Creature* pCreature)
{
    return new mob_freya_spawnedAI(pCreature);
}

CreatureAI* GetAI_mob_iron_roots(Creature* pCreature)
{
    return new mob_iron_rootsAI(pCreature);
}

void AddSC_boss_freya()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_freya";
    newscript->GetAI = &GetAI_boss_freya;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_elder_brightleaf";
    newscript->GetAI = &GetAI_boss_elder_brightleaf;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_elder_ironbranch";
    newscript->GetAI = &GetAI_boss_elder_ironbranch;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_elder_stonebark";
    newscript->GetAI = &GetAI_boss_elder_stonebark;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_freya_ground";
    newscript->GetAI = &GetAI_mob_freya_ground;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_freya_spawned";
    newscript->GetAI = &GetAI_mob_freya_spawned;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_iron_roots";
    newscript->GetAI = &GetAI_mob_iron_roots;
    newscript->RegisterSelf();
}
