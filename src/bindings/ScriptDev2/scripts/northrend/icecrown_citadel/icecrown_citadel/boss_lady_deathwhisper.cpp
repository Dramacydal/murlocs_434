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
SDName: boss_lady_deathwhisper
SD%Complete: 99%
SDComment:  by michalpolko with special thanks to:
            mangosR2 team and all who are supporting us with feedback, testing and fixes
            TrinityCore for some info about spells IDs
            everybody whom I forgot to mention here ;)

SDCategory: Icecrown Citadel
EndScriptData */

#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
        SPELL_BERSERK                           = 26662,
        SPELL_INSIGNIFICANCE                    = 71204,
        SPELL_DOMINATE_MIND                     = 71289,
        SPELL_MANA_BARRIER                      = 70842,
        SPELL_DEATH_AND_DECAY                   = 71001,
        SPELL_SHADOW_BOLT                       = 71254,
        SPELL_FROSTBOLT                         = 71420,
        SPELL_FROSTBOLT_VOLLEY                  = 72905,

        // Cult Adherents
        NPC_CULT_ADHERENT                       = 37949,
        NPC_REANIMATED_ADHERENT                 = 38010,
        NPC_DEFORMED_FANATIC                    = 38135,

        SPELL_FROST_FEVER                       = 71129,
        SPELL_SHROUD_OF_THE_OCCULT              = 70768,
        SPELL_DEATHCHILL_BOLT                   = 70594,
        SPELL_DEATHCHILL_BLAST                  = 70906,
        SPELL_CURSE_OF_TORPOR                   = 71237,
        SPELL_DARK_EMPOWERMENT                  = 70901, // dummy spell: 70896
        SPELL_DARK_MARTYRDOM                    = 70903,
        SPELL_ADHERENTS_DETERMINATION           = 71234,

        // Cult Fanatic
        NPC_CULT_FANATIC                        = 37890,
        NPC_REANIMATED_FANATIC                  = 38009,

        SPELL_SHADOW_CLEAVE                     = 70670,
        SPELL_NECROTIC_STRIKE                   = 70659,
        SPELL_VAMPIRIC_MIGHT                    = 70674,
        SPELL_DARK_TRANSFORMATION               = 70900, // dummy spell: 70895
        //SPELL_DARK_MARTYRDOM                    = 70903,
        SPELL_FANATICS_DETERMINATION            = 71235,

        // Vengeful Shade
        NPC_VENGEFUL_SHADE                      = 38222,
        SPELL_SUMMON_SPIRIT                     = 71363, // not used since trigger npc is missing (something must be the target of this spell, it can't be player)
        SPELL_SUMMON_SPIRIT_TRIGGERED           = 71426,
        SPELL_VENGEFUL_BLAST_AURA               = 71494, // must proc on melee hit

        // Achievement
        SPELL_ACHIEVEMENT_CREDIT                = 72827,
};

// talks
enum
{
    SAY_SPEECH_1                = -1631011,
    SAY_SPEECH_2                = -1631012,
    SAY_SPEECH_3                = -1631013,
    SAY_SPEECH_4                = -1631014,
    SAY_SPEECH_5                = -1631015,
    SAY_SPEECH_6                = -1631016,
    SAY_SPEECH_7                = -1631017,
    SAY_AGGRO                   = -1631018,
    SAY_PHASE_TWO               = -1631019,
    SAY_DARK_EMPOWERMENT        = -1631020,
    SAY_DARK_TRANSFORMATION     = -1631021,
    SAY_ANIMATE_DEAD            = -1631022,
    SAY_DOMINATE_MIND           = -1631023,
    SAY_BERSERK                 = -1631024,
    SAY_DEATH                   = -1631025,
    SAY_SLAY_1                  = -1631026,
    SAY_SLAY_2                  = -1631027,
};

static Locations SpawnLoc[]=
{
    {-623.055481f, 2211.326660f, 51.764259f},  // 0 Lady's stay point
    {-620.197449f, 2272.062256f, 50.848679f},  // 1 Right Door 1
    {-598.636353f, 2272.062256f, 50.848679f},  // 2 Right Door 2
    {-578.495728f, 2272.062256f, 50.848679f},  // 3 Right Door 3
    {-578.495728f, 2149.211182f, 50.848679f},  // 4 Left Door 1
    {-598.636353f, 2149.211182f, 50.848679f},  // 5 Left Door 2
    {-620.197449f, 2149.211182f, 50.848679f},  // 6 Left Door 3
    {-525.652466f, 2216.611328f, 62.823681f},  // 7 Upper marsh 1
    {-525.652466f, 2211.611328f, 62.823681f},  // 8 Upper marsh 2
    {-525.652466f, 2206.611328f, 62.823681f},  // 9 Upper marsh 3
};

struct MANGOS_DLL_DECL boss_lady_deathwhisper_eventAI : public base_icc_bossAI
{
    boss_lady_deathwhisper_eventAI(Creature* pCreature) : base_icc_bossAI(pCreature)
    {
        m_uiStep = 0;
        m_uiEventTimer = 0;
        m_bIsEventStarted = false;
        m_bIsEventFinished = false;
    }

    uint32 m_uiStep;
    uint32 m_uiEventTimer;
    bool m_bIsEventStarted;
    bool m_bIsEventFinished;

    void Reset(){}

    void NextStep(uint32 uiTimer)
    {
        m_uiEventTimer = uiTimer;
        ++m_uiStep;
    }

    void MoveInLineOfSight(Unit *pWho)
    {
        if (!m_bIsEventStarted)
        {
            if (pWho->GetTypeId() == TYPEID_PLAYER)
            {
                m_bIsEventStarted = true;
                NextStep(0);
                return;
            }
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    // for the fight handler
    virtual void UpdateFightAI(const uint32 uiDiff){}

    void UpdateAI(const uint32 uiDiff)
    {
        UpdateFightAI(uiDiff);

        if (!m_bIsEventStarted || m_bIsEventFinished)
            return;

        if (m_uiEventTimer <= uiDiff)
        {
            switch (m_uiStep)
            {
                case 0:
                {
                    break;
                }
                case 1:
                {
                    DoScriptText(SAY_SPEECH_1, m_creature);
                    NextStep(13000);
                    break;
                }
                case 2:
                {
                    DoScriptText(SAY_SPEECH_2, m_creature);
                    NextStep(11000);
                    break;
                }
                case 3:
                {
                    DoScriptText(SAY_SPEECH_3, m_creature);
                    NextStep(10000);
                    break;
                }
                case 4:
                {
                    DoScriptText(SAY_SPEECH_4, m_creature);
                    NextStep(9000);
                    break;
                }
                case 5:
                {
                    DoScriptText(SAY_SPEECH_5, m_creature);
                    NextStep(10000);
                    break;
                }
                case 6:
                {
                    DoScriptText(SAY_SPEECH_6, m_creature);
                    NextStep(10000);
                    break;
                }
                case 7:
                {
                    DoScriptText(SAY_SPEECH_7, m_creature);
                    NextStep(10000);
                    break;
                }
                case 8:
                {
                    m_bIsEventFinished = true;
                    break;
                }
                default:
                    break;
            }
        }
        else
            m_uiEventTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL boss_lady_deathwhisperAI : public boss_lady_deathwhisper_eventAI
{
    boss_lady_deathwhisperAI(Creature* pCreature) : boss_lady_deathwhisper_eventAI(pCreature)
    {
        m_pInstance = ((instance_icecrown_spire*)pCreature->GetInstanceData());
        Reset();
    }

    instance_icecrown_spire* m_pInstance;

    bool m_bIsPhaseOne;
    uint32 m_uiManaBarrierCheckTimer;

    bool m_bIsLeftSideSummon;

    uint32 m_uiBerserkTimer;
    uint32 m_uiSummonWaveTimer;
    uint32 m_uiTouchOfInsignificanceTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiDeathAndDecayTimer;
    uint32 m_uiFrostboltTimer;
    uint32 m_uiFrostboltVolleyTimer;
    uint32 m_uiDominateMindTimer;
    uint32 m_uiVengefulShadeTimer;

    uint32 m_uiMindControlCount;
    std::list<Creature*> SummonsEntryList;

    void Reset()
    {
        m_bIsPhaseOne                   = true;
        m_bIsLeftSideSummon             = true;
        m_uiManaBarrierCheckTimer       = 1000;
        m_uiBerserkTimer                = 10 * MINUTE * IN_MILLISECONDS;
        m_uiSummonWaveTimer             = 10000;
        m_uiTouchOfInsignificanceTimer  = 0;
        m_uiShadowBoltTimer             = 2000;
        m_uiDeathAndDecayTimer          = 30000;
        m_uiFrostboltTimer              = urand(5000, 10000);
        m_uiFrostboltVolleyTimer        = 5000;
        m_uiDominateMindTimer           = urand(30000, 45000);
        m_uiVengefulShadeTimer          = 10000;

        SetCombatMovement(false);

        m_uiMindControlCount = 0;

        if (m_bIs25Man)
        {
            if (m_bIsHeroic)
                m_uiMindControlCount = 3; // 3 targets on heroic 25man
            else
                m_uiMindControlCount = 1; // 1 target on normal 25man
        }
        else
        {
            if (m_bIsHeroic)
                m_uiMindControlCount = 1; // 1 target on heroic 10man
        }
    }

    void Aggro(Unit *pWho)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_DEATHWHISPER, IN_PROGRESS);
            m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_FULL_HOUSE, true);
        }

        // make sure that the event will not continue in the fight
        m_bIsEventFinished = true;

        DoScriptText(SAY_AGGRO, m_creature);
        DoCastSpellIfCan(m_creature, SPELL_MANA_BARRIER, CAST_TRIGGERED);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_DEATHWHISPER, FAIL);
            m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_FULL_HOUSE, false);
        }
    }

    void JustDied(Unit *pKiller)
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_DEATHWHISPER, DONE);

            // Find required NPC as achievement criteria
            SummonsEntryList.clear();
            GetCreatureListWithEntryInGrid(SummonsEntryList, m_creature, NPC_CULT_ADHERENT, 250.0f);
            if (SummonsEntryList.empty())
                m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_FULL_HOUSE, false);

            SummonsEntryList.clear();
            GetCreatureListWithEntryInGrid(SummonsEntryList, m_creature, NPC_CULT_FANATIC, 250.0f);
            if (SummonsEntryList.empty())
                m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_FULL_HOUSE, false);
            SummonsEntryList.clear();
            GetCreatureListWithEntryInGrid(SummonsEntryList, m_creature, NPC_REANIMATED_FANATIC, 250.0f);
            if (SummonsEntryList.empty())
                m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_FULL_HOUSE, false);

            SummonsEntryList.clear();
            GetCreatureListWithEntryInGrid(SummonsEntryList, m_creature, NPC_REANIMATED_ADHERENT, 250.0f);
            if (SummonsEntryList.empty())
                m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_FULL_HOUSE, false);

            SummonsEntryList.clear();
            GetCreatureListWithEntryInGrid(SummonsEntryList, m_creature, NPC_DEFORMED_FANATIC, 250.0f);
            if (SummonsEntryList.empty())
                m_pInstance->SetSpecialAchievementCriteria(ACHIEVE_FULL_HOUSE, false);

            m_pInstance->DoUpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, SPELL_ACHIEVEMENT_CREDIT);
        }

        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit *pVictim)
    {
        DoScriptText(SAY_SLAY_1 - urand(0, 1), m_creature);
    }

    void JustSummoned(Creature *pSummoned)
    {
        pSummoned->SetInCombatWithZone();
    }

    void DoSummonShade()
    {
        if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1, uint32(0), SELECT_FLAG_PLAYER))
        {
            float x, y, z;
            pTarget->GetNearPoint(pTarget, x, y, z, pTarget->GetObjectBoundingRadius(), 10.0f, frand(-M_PI_F, M_PI_F));

            if (Creature *pShade = m_creature->SummonCreature(NPC_VENGEFUL_SHADE, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 7000))
            {
                pShade->SetSpeedRate(MOVE_RUN, 0.5f);
                pShade->AddThreat(pTarget, 10000000.0f, true);
                pShade->AI()->AttackStart(pTarget);
            }
        }
    }

    void DoSummonWave()
    {
        // summoning on all sides
        if (m_bIs25Man)
        {
            // summon 1 fanatic and 2 adherents on right side
            m_creature->SummonCreature(NPC_CULT_ADHERENT, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(NPC_CULT_FANATIC, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(NPC_CULT_ADHERENT, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);

            // summon 2 fanatics and 1 adherent on left side
            m_creature->SummonCreature(NPC_CULT_FANATIC, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(NPC_CULT_ADHERENT, SpawnLoc[5].x, SpawnLoc[5].y, SpawnLoc[5].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(NPC_CULT_FANATIC, SpawnLoc[6].x, SpawnLoc[6].y, SpawnLoc[6].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);

            // summon random on stairs
            m_creature->SummonCreature(roll_chance_i(50) ? NPC_CULT_FANATIC : NPC_CULT_ADHERENT, SpawnLoc[8].x, SpawnLoc[8].y, SpawnLoc[8].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
        }
        // summoning on 1 side alternatively
        else
        {
            // left side summoning
            if (m_bIsLeftSideSummon)
            {
                // summon 1 fanatic and 2 adherents
                m_creature->SummonCreature(NPC_CULT_ADHERENT, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
                m_creature->SummonCreature(NPC_CULT_FANATIC, SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
                m_creature->SummonCreature(NPC_CULT_ADHERENT, SpawnLoc[3].x, SpawnLoc[3].y, SpawnLoc[3].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            }
            else
            {
                // summon 2 fanatics and 1 adherent
                m_creature->SummonCreature(NPC_CULT_FANATIC, SpawnLoc[4].x, SpawnLoc[4].y, SpawnLoc[4].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
                m_creature->SummonCreature(NPC_CULT_ADHERENT, SpawnLoc[5].x, SpawnLoc[5].y, SpawnLoc[5].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
                m_creature->SummonCreature(NPC_CULT_FANATIC, SpawnLoc[6].x, SpawnLoc[6].y, SpawnLoc[6].z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
            }

            // change sides for next summoning
            m_bIsLeftSideSummon = !m_bIsLeftSideSummon;
        }
    }

    void UpdateFightAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Berserk - both phases
        if (m_uiBerserkTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                m_uiBerserkTimer = 10 * MINUTE * IN_MILLISECONDS;
            }
        }
        else
            m_uiBerserkTimer -= uiDiff;

        // Death and Decay - both phases
        if (m_uiDeathAndDecayTimer <= uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_DEATH_AND_DECAY) == CAST_OK)
                    m_uiDeathAndDecayTimer = 30000;
            }
        }
        else
            m_uiDeathAndDecayTimer -= uiDiff;

        // Dominate Mind - both phases
        if (m_uiMindControlCount)
        {
            if (m_uiDominateMindTimer <= uiDiff)
            {
                /* uncomment when spell implemented in core
                for (int i = 0; i < m_uiMindControlCount; ++i)
                {
                    if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                        DoCastSpellIfCan(pTarget, SPELL_DOMINATE_MIND, CAST_TRIGGERED);
                }*/

                DoScriptText(SAY_DOMINATE_MIND, m_creature);
                m_uiDominateMindTimer = 45000;
            }
            else
                m_uiDominateMindTimer -= uiDiff;
        }


         // PHASE ONE
        if (m_bIsPhaseOne)
        {
            // Mana Barrier check
            if (m_uiManaBarrierCheckTimer <= uiDiff)
            {
                // phase ends when Mana Barrier wears off
                if (!m_creature->HasAura(SPELL_MANA_BARRIER))
                {
                    DoScriptText(SAY_PHASE_TWO, m_creature);
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    m_bIsPhaseOne = false;
                    return;
                }

                m_uiManaBarrierCheckTimer = 1000;
            }
            else
                m_uiManaBarrierCheckTimer -= uiDiff;

            // Summon wave
            if (m_uiSummonWaveTimer <= uiDiff)
            {
                DoSummonWave();
                m_uiSummonWaveTimer = 60000;
                DoScriptText(SAY_ANIMATE_DEAD, m_creature);
            }
            else
                m_uiSummonWaveTimer -= uiDiff;

            // Shadow Bolt
            if (m_uiShadowBoltTimer <= uiDiff)
            {
                if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_SHADOW_BOLT) == CAST_OK)
                        m_uiShadowBoltTimer = 2000;
                }
            }
            else
                m_uiShadowBoltTimer -= uiDiff;
        }
        else // PHASE TWO
        {
            if (m_bIsHeroic)
            {
                // Summon wave
                if (m_uiSummonWaveTimer <= uiDiff)
                {
                    DoSummonWave();
                    m_uiSummonWaveTimer = 60000;
                    DoScriptText(SAY_ANIMATE_DEAD, m_creature);
                }
                else
                    m_uiSummonWaveTimer -= uiDiff;
            }

            // Touch of Insignificance
            if (m_uiTouchOfInsignificanceTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_INSIGNIFICANCE) == CAST_OK)
                    m_uiTouchOfInsignificanceTimer = urand(7000, 10000);
            }
            else
                m_uiTouchOfInsignificanceTimer -= uiDiff;

            // Frostbolt
            if (m_uiFrostboltTimer <= uiDiff)
            {
                if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    if (DoCastSpellIfCan(pTarget, SPELL_FROSTBOLT) == CAST_OK)
                        m_uiFrostboltTimer = urand(5000, 10000);
                }
            }
            else
                m_uiFrostboltTimer -= uiDiff;

            // Frostbolt Volley
            if (m_uiFrostboltVolleyTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FROSTBOLT_VOLLEY) == CAST_OK)
                    m_uiFrostboltVolleyTimer = urand(15000, 20000);
            }
            else
                m_uiFrostboltVolleyTimer -= uiDiff;

            // Vengeful Shade
            if (m_uiVengefulShadeTimer <= uiDiff)
            {
                DoSummonShade();
                m_uiVengefulShadeTimer = 10000;
            }
            else
                m_uiVengefulShadeTimer -= uiDiff;

            // melee attack in 2nd phase only
            DoMeleeAttackIfReady();
        }
    }
};

CreatureAI* GetAI_boss_lady_deathwhisper(Creature* pCreature)
{
    return new boss_lady_deathwhisperAI(pCreature);
}

// adds AI
struct MANGOS_DLL_DECL mob_cult_fanaticAI : public ScriptedAI
{
    mob_cult_fanaticAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiCleaveTimer;
    uint32 m_uiNecroticStrikeTimer;
    uint32 m_uiTransformationTimer;
    uint32 m_uiVampiricMightTimer;

    void Reset()
    {
        m_uiCleaveTimer = urand(4000, 8000);
        m_uiNecroticStrikeTimer = urand(5000, 10000);
        m_uiTransformationTimer = urand(20000, 60000);
        m_uiVampiricMightTimer = urand(20000, 60000);
    }

    void JustReachedHome()
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Cleave
        if (m_uiCleaveTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_CLEAVE) == CAST_OK)
                m_uiCleaveTimer = urand(4000, 8000);
        }
        else
            m_uiCleaveTimer -= uiDiff;

        // Necrotic Strike
        if (m_uiNecroticStrikeTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_NECROTIC_STRIKE) == CAST_OK)
                m_uiNecroticStrikeTimer = urand(5000, 10000);
        }
        else
            m_uiNecroticStrikeTimer -= uiDiff;

        // random transformation
        if (m_uiTransformationTimer <= uiDiff)
        {
            if (roll_chance_i(50))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DARK_TRANSFORMATION) == CAST_OK)
                {
                    m_creature->SetSpeedRate(MOVE_RUN, 0.5f);
                    m_uiTransformationTimer = HOUR * IN_MILLISECONDS;
                }
            }
            else
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DARK_MARTYRDOM) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_FANATICS_DETERMINATION, CAST_TRIGGERED);
                    m_uiTransformationTimer = HOUR * IN_MILLISECONDS;
                }
            }
        }
        else
            m_uiTransformationTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_cult_fanatic(Creature* pCreature)
{
    return new mob_cult_fanaticAI(pCreature);
}

struct MANGOS_DLL_DECL mob_cult_adherentAI : public ScriptedAI
{
    mob_cult_adherentAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiTransformationTimer;
    uint32 m_uiFrostFeverTimer;
    uint32 m_uiShroudTimer;
    uint32 m_uiDeathchillTimer;
    uint32 m_uiCurseOfTorporTimer;

    void Reset()
    {
        m_uiTransformationTimer = urand(20000, 60000);
        m_uiCurseOfTorporTimer  = urand(5000, 10000);
        m_uiDeathchillTimer     = urand(3000, 5000);
        m_uiFrostFeverTimer     = urand(5000, 10000);
        m_uiShroudTimer         = urand(15000, 30000);
    }

    void JustReachedHome()
    {
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Frost Fever
        if (m_uiFrostFeverTimer <= uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_FROST_FEVER) == CAST_OK)
                    m_uiFrostFeverTimer = urand(5000, 10000);
            }
        }
        else
            m_uiFrostFeverTimer -= uiDiff;

        // Deathchill Bolt/Blast
        if (m_uiDeathchillTimer <= uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, m_creature->HasAura(SPELL_DARK_EMPOWERMENT) ? SPELL_DEATHCHILL_BLAST : SPELL_DEATHCHILL_BOLT) == CAST_OK)
                    m_uiDeathchillTimer = urand(3000, 5000);
            }
        }
        else
            m_uiDeathchillTimer -= uiDiff;

        // Curse of Torpor
        if (m_uiCurseOfTorporTimer <= uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_CURSE_OF_TORPOR) == CAST_OK)
                    m_uiCurseOfTorporTimer = urand(5000, 15000);
            }
        }
        else
            m_uiCurseOfTorporTimer -= uiDiff;

        // random transformation
        if (m_uiTransformationTimer <= uiDiff)
        {
            if (roll_chance_i(50))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DARK_EMPOWERMENT) == CAST_OK)
                    m_uiTransformationTimer = HOUR * IN_MILLISECONDS;
            }
            else
            {
                if (DoCastSpellIfCan(m_creature, SPELL_DARK_MARTYRDOM) == CAST_OK)
                {
                    DoCastSpellIfCan(m_creature, SPELL_ADHERENTS_DETERMINATION, CAST_TRIGGERED);
                    m_uiTransformationTimer = HOUR * IN_MILLISECONDS;
                }
            }
        }
        else
            m_uiTransformationTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_cult_adherent(Creature* pCreature)
{
    return new mob_cult_adherentAI(pCreature);
}

void AddSC_boss_lady_deathwhisper()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lady_deathwhisper";
    pNewScript->GetAI = &GetAI_boss_lady_deathwhisper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_cult_adherent";
    pNewScript->GetAI = &GetAI_mob_cult_adherent;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_cult_fanatic";
    pNewScript->GetAI = &GetAI_mob_cult_fanatic;
    pNewScript->RegisterSelf();

}
