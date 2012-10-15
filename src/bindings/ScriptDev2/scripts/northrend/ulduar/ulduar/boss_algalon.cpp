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
SDName: boss_algalon
SD%Complete: 
SDComment: cosmic smash, phase punch, black hole phasing need some core support
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    SAY_INTRO1                      = -1603270,
    SAY_INTRO2                      = -1603271,
    SAY_INTRO3                      = -1603272,
    SAY_ENGAGE                      = -1603141,
    SAY_AGGRO                       = -1603140,
    SAY_SLAY1                       = -1603145,
    SAY_SLAY2                       = -1603146,
    SAY_SUMMON_STAR                 = -1603148,
    SAY_BIGBANG1                    = -1603142,
    SAY_BIGBANG2                    = -1603143,
    SAY_PHASE2                      = -1603144, 
    SAY_BERSERK                     = -1603147,
    SAY_DESPAWN1                    = -1603273,
    SAY_DESPAWN2                    = -1603274,
    SAY_DESPAWN3                    = -1603275,
    SAY_OUTRO1                      = -1603276,
    SAY_OUTRO2                      = -1603277,
    SAY_OUTRO3                      = -1603278,
    SAY_OUTRO4                      = -1603149,
    SAY_OUTRO5                      = -1603279,

    ITEM_PLANETARIUM_KEY            = 45796,
    ITEM_PLANETARIUM_KEY_H          = 45798,

    SPELL_ALGALON_EVENT_BEAM        = 64367,
    SPELL_ALGALON_EVENT_CLIMAX      = 64580, // This spells are used for environment

    //spells to be casted
    SPELL_QUANTUM_STRIKE            = 64395, //Normal Quantum Strike
    SPELL_QUANTUM_STRIKE_H          = 64592, //Heroic Quantum Strike
    SPELL_PHASE_PUNCH               = 64412, //Phase punch 
    SPELL_PHASE_PUNCH_SHIFT         = 64417,
    SPELL_CONSTELLATION_TRIGGER     = 65508, // this should make the space effect
    SPELL_COSMIC_SMASH              = 62301, //Normal Cosmic Smash
    SPELL_COSMIC_SMASH_H            = 64598, //Heroic Cosmic Smash
    SPELL_COSMIC_SMASH_MISSILE      = 62304,
    SPELL_BIG_BANG                  = 64443, //Normal Big Bang
    SPELL_BIG_BANG_H                = 64584, //Heroic Big Bang
    SPELL_ASCEND                    = 64487, //Ascend to the Heavens
    SPELL_BERSERK                   = 47008, //Berserk
    NPC_SMASH_TARGET_DUMMY          = 33105,

    // mobs
    NPC_COLLAPSING_STAR             = 32955,    // they lose 1%hp per sec & cast black hole explosion when they die -> leave a black hole
    SPELL_BLACK_HOLE_EXPLOSION      = 64122,
    SPELL_BLACK_HOLE_EXPLOSION_H    = 65108,
    SPELL_BLACK_HOLE_DESPAWN        = 64391,    // dummy spell
    SPELL_SUMMON_BLACK_HOLE         = 62189,
    SPELL_BLACK_HOLE_VISUAL         = 64135,
    SPELL_BLACK_HOLE_SPAWN          = 62003,
    SPELL_BLACK_HOLE_TRIGG          = 62185,    // shifts the phase
    SPELL_BLACK_HOLE_SHIFT          = 62168,
    SPELL_BLACK_HOLE_DMG            = 62169,

    NPC_BLACK_HOLE                  = 32953,    // players must stay inside to avoid big bang

    NPC_LIVING_CONSTELLATION        = 33052,    // if one enters a black hole they are despawned
    SPELL_ARCANE_BARRAGE            = 64599,    //Arcane Barage
    SPELL_ARCANE_BARRAGE_H          = 64607,    //Heroic Arcane Barage?

    NPC_DARK_MATTER                 = 33089,    // populates the black holes
    NPC_UNLEASHED_DARK_MATTER       = 34097,    // summoned by black holes in phase 2
    NPC_COSMIC_SMASH                = 33104,

    ACHIEV_FEED_TEARS               = 3004,
    ACHIEV_FEED_TEARS_H             = 3005,     // nobody dies in the raid lockout
    ACHIEV_HERALD_OF_TITANS         = 3316,
    ACHIEV_OBSERVED                 = 3036,
    ACHIEV_OBSERVED_H               = 3037,
    ACHIEV_SUPERMASSIVE             = 3003,
    ACHIEV_SUPERMASSIVE_H           = 3002,

    UI_STATE_ALGALON_TIMER_ON       = 4132,
    UI_STATE_ALGALON_TIMER_COUNT    = 4131,
};

//Positional defines 
struct LocationsXY
{
    float x, y;
    uint32 id;
};

static LocationsXY PositionLoc[]=
{
    {1620, -320.75f},
    {1620, -290.75f },
    {1645.5f, -290.75f },
    {1645.5f, -320.75f },
};

//Black hole
struct MANGOS_DLL_DECL mob_black_holeAI : public ScriptedAI
{
    mob_black_holeAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();

        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;

    bool m_bHasAura;
    uint32 m_uiRaidCheckTimer;
    bool m_bIsPhase2;
    uint32 m_uiSummonTimer;

    void Reset()
    {
        m_bHasAura = false;
        m_uiRaidCheckTimer = 1000; 
        m_bIsPhase2 = false;
        m_uiSummonTimer = 5000;
        DoCast(m_creature, SPELL_BLACK_HOLE_SPAWN);
        m_creature->SetRespawnDelay(DAY);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_ALGALON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // summon unleashed dark matter in phase 2
        if (m_uiSummonTimer < uiDiff && m_bIsPhase2)
        {
        if(Creature* pTemp = m_creature->SummonCreature(NPC_UNLEASHED_DARK_MATTER, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
            pTemp->SetInCombatWithZone();
            m_uiSummonTimer = urand(10000, 15000);
        }
        else m_uiSummonTimer -= uiDiff;

        // phase players into the void
        if (m_uiRaidCheckTimer < uiDiff && !m_bIsPhase2)
        {
            if(!m_bHasAura)
            {
                DoCast(m_creature, SPELL_BLACK_HOLE_TRIGG);
                m_bHasAura = true;
            }

            if(Creature *pConstellation = GetClosestCreatureWithEntry(m_creature, NPC_LIVING_CONSTELLATION, 2))
            {
                pConstellation->DealDamage(pConstellation, pConstellation->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
            m_uiRaidCheckTimer = 500;
        }
        else m_uiRaidCheckTimer -= uiDiff;
    }
};

//Algalon
struct MANGOS_DLL_DECL boss_algalonAI : public ScriptedAI
{
    boss_algalonAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_bHasStarted       = false;    // flag used to check if the encounter has been started from the console
        m_bIsInProgress     = false;    // flag used for time counter
        m_bIsFirstTime      = true;     // flag used to show if this is the first aggro
        m_uiDespawnTimer    = 3600000;  // 1h;
        m_uiLastTimer       = 3600000;  // 1h;
        m_bFeedOnTears      = true;     // flag used to check the Fead on Tears achiev
        pCreature->SetVisibility(VISIBILITY_OFF);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;   

    uint32 m_uiBerserk_Timer;
    uint32 m_uiBigBang_Timer;
    uint32 m_uiCosmicSmash_Timer;
    uint32 m_uiPhasePunch_Timer;
    uint32 m_uiQuantumStrike_Timer;
    uint32 m_uiCollapsingStar_Timer;
    uint32 m_uiLivingConstellationTimer;
    uint32 m_uiRaidCheckTimer;
    uint32 m_uiDarkMaterTimer;
    uint32 m_uiDespawnTimer;
    uint32 m_uiLastTimer;

    // intro & outro
    bool m_bIsOutro;
    uint32 m_uiOutroTimer;
    uint32 m_uiOutroStep;
    bool m_bIsIntro;
    uint32 m_uiIntroTimer;
    uint32 m_uiIntroStep;

    bool m_bHasStarted;
    bool m_bIsFirstTime;
    bool m_bIsInProgress;

    uint8 m_uiCombatPhase;
    bool m_bIsPhase2;

    bool m_bIsDespawned;
    bool m_bFeedOnTears;

    void Reset()
    {
        m_uiQuantumStrike_Timer     = 4000 + rand()%10000;
        m_uiBerserk_Timer           = 360000; //6 minutes
        m_uiCollapsingStar_Timer    = urand(15000, 20000); //Spawns between 15 to 20 seconds
        m_uiLivingConstellationTimer = 60000;
        m_uiBigBang_Timer           = 90000;
        m_uiDarkMaterTimer          = 90000;
        m_uiPhasePunch_Timer        = 8000;
        m_uiCosmicSmash_Timer       = urand(30000, 60000);
        m_uiRaidCheckTimer          = 1000;

        m_uiCombatPhase         = 0;     // it's 0 for idle, 1 for intro and 2 for combat
        m_bIsPhase2             = false; // flag used below 30% hp

        m_uiOutroTimer          = 10000;
        m_uiOutroStep           = 1;
        m_bIsIntro              = true;
        m_uiIntroTimer          = 10000;
        m_uiIntroStep           = 1;
        m_bIsOutro              = false;  

        m_bIsDespawned          = false; // flaged used to show that time has run out
    }

    void KilledUnit(Unit *victim)
    {
        switch(urand(0, 1))
        {
        case 0: DoScriptText(SAY_SLAY1, m_creature); break;
        case 1: DoScriptText(SAY_SLAY2, m_creature); break;
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALGALON, NOT_STARTED);
        m_bIsFirstTime = false;
    }

    void DoOutro()
    {
        if (m_pInstance)
        {
            m_pInstance->SetData(TYPE_ALGALON, DONE);
            m_pInstance->DoUpdateWorldState(UI_STATE_ALGALON_TIMER_ON, 0);
            // hacky way to complete achievements; use only if you have this function
            m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_OBSERVED : ACHIEV_OBSERVED_H);

            if(m_bFeedOnTears)
                m_pInstance->DoCompleteAchievement(m_bIsRegularMode ? ACHIEV_FEED_TEARS : ACHIEV_FEED_TEARS_H);
        }

        m_creature->ForcedDespawn();
    }

    void AttackStart(Unit* pWho)
    {
        if(!m_bIsInProgress)
            return;

        if (m_creature->Attack(pWho, true)) 
        {
            m_creature->SetWalk(false);
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho);
        }
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pWho)
            return;

        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(m_creature) && m_creature->IsHostileTo(pWho) &&
            pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 40) && m_uiCombatPhase == 0 && m_bHasStarted && m_bIsFirstTime)
            m_uiCombatPhase = 1;
    }

    void StartEncounter()
    {
        m_creature->SetVisibility(VISIBILITY_ON);
        m_bIsFirstTime  = true;
        m_bHasStarted   = true;
        m_bIsInProgress = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
        if(m_creature->GetHealthPercent() < 1.0f)
        {
            uiDamage = 0;
            m_bIsOutro = true;
        }
    }

    void SummonCreature(uint32 m_uiCreatureEntry)
    {
        float angle = (float) rand()*360/RAND_MAX + 1;
        float homeX = 1630.475f + urand(15, 30)*cos(angle*(M_PI/180));
        float homeY = -286.989f + urand(15, 30)*sin(angle*(M_PI/180));
        if(Creature* pTemp = m_creature->SummonCreature(m_uiCreatureEntry, homeX, homeY, 417.32f, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
        {
            pTemp->SetInCombatWithZone();
            if(pTemp->GetEntry() == NPC_DARK_MATTER)
                pTemp->CastSpell(pTemp, SPELL_BLACK_HOLE_SHIFT, false);
        }
    }

    void Aggro(Unit* pWho)
    {
        // set combat phase
        m_uiCombatPhase = 2;

        if(m_bIsFirstTime)
        {
            m_bIsInProgress = true;
            DoScriptText(SAY_ENGAGE, m_creature);
            if(m_pInstance)
            {
                m_pInstance->DoUpdateWorldState(UI_STATE_ALGALON_TIMER_ON, 1);
                m_pInstance->DoUpdateWorldState(UI_STATE_ALGALON_TIMER_COUNT, m_uiDespawnTimer / 60000);
            }
        }
        else
            DoScriptText(SAY_AGGRO, m_creature);

        m_creature->SetInCombatWithZone();
        DoCast(m_creature, SPELL_CONSTELLATION_TRIGGER);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALGALON, IN_PROGRESS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // despawn timer
        if(m_uiDespawnTimer < uiDiff && !m_bIsDespawned && m_bIsInProgress)
        {
            m_bIsDespawned = true;
            m_bIsOutro = true;
            m_bIsInProgress = false;
        }
        else m_uiDespawnTimer -= uiDiff;

        // update world state
        if(m_uiDespawnTimer < m_uiLastTimer - 60000 && !m_bIsDespawned)
        {
            m_uiLastTimer = m_uiDespawnTimer;
            uint32 tMinutes = m_uiDespawnTimer / 60000;
            if(m_pInstance)
                m_pInstance->DoUpdateWorldState(UI_STATE_ALGALON_TIMER_COUNT, tMinutes);
        }

        if(!m_bIsOutro)
        {
            // intro
            if(m_uiCombatPhase == 1)
            {
                if(m_bIsIntro  && m_bIsFirstTime)
                {
                    switch(m_uiIntroStep)
                    {
                    case 1:
                        DoScriptText(SAY_INTRO1, m_creature);
                        ++m_uiIntroStep;
                        m_uiIntroTimer = 8000;
                        break;
                    case 3:
                        DoScriptText(SAY_INTRO2, m_creature);
                        ++m_uiIntroStep;
                        m_uiIntroTimer = 6000;
                        break;
                    case 5:
                        DoScriptText(SAY_INTRO3, m_creature);
                        ++m_uiIntroStep;
                        m_uiIntroTimer = 10000;
                        break;
                    case 7:
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_bIsIntro = false;
                        ++m_uiIntroStep;
                        m_uiIntroTimer = 10000;
                        break;
                    }
                }
                else return;

                if (m_uiIntroTimer <= uiDiff)
                {
                    ++m_uiIntroStep;
                    m_uiIntroTimer = 330000;
                } m_uiIntroTimer -= uiDiff;
            }

            // combat
            if(m_uiCombatPhase == 2)
            {
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                // spells
                if(m_uiQuantumStrike_Timer < uiDiff)
                {
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_QUANTUM_STRIKE : SPELL_QUANTUM_STRIKE_H);
                    m_uiQuantumStrike_Timer = 4000 + rand()%10000;
                }else m_uiQuantumStrike_Timer -= uiDiff;

                if(m_uiBigBang_Timer < uiDiff)
                {
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_BIG_BANG : SPELL_BIG_BANG_H);
                    m_uiBigBang_Timer = 90000;
                    m_uiDarkMaterTimer = 1000;
                }else m_uiBigBang_Timer -= uiDiff;

                if(m_uiCosmicSmash_Timer < uiDiff)
                {
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_COSMIC_SMASH : SPELL_COSMIC_SMASH_H);
                    m_uiCosmicSmash_Timer = urand(30000, 60000);
                }else m_uiCosmicSmash_Timer -= uiDiff;

                if(m_uiPhasePunch_Timer < uiDiff)
                {
                    if(Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                        DoCast(pTarget,SPELL_PHASE_PUNCH);
                    m_uiPhasePunch_Timer = 15000;
                }else m_uiPhasePunch_Timer -= uiDiff;

                // hack, phase punch needs core support
                // PLEASE REMOVE FOR REVISION!
                if(m_uiRaidCheckTimer < uiDiff)
                {
                    Map *map = m_creature->GetMap();
                    if (map->IsDungeon())
                    {
                        Map::PlayerList const &PlayerList = map->GetPlayers();

                        if (PlayerList.isEmpty())
                            return;

                        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        {
                            if (i->getSource()->isAlive() && i->getSource()->HasAura(SPELL_PHASE_PUNCH, EFFECT_INDEX_0))
                            {
                                Aura *phasePunch = i->getSource()->GetAura(SPELL_PHASE_PUNCH, EFFECT_INDEX_0);
                                if(phasePunch->GetStackAmount() > 4)
                                {
                                    i->getSource()->RemoveAurasDueToSpell(SPELL_PHASE_PUNCH);
                                    i->getSource()->CastSpell(i->getSource(), SPELL_PHASE_PUNCH_SHIFT, false);
                                }
                            }
                            if (!i->getSource()->isAlive())
                                m_bFeedOnTears = false;
                        }
                    }
                    m_uiRaidCheckTimer = 1000;
                }else m_uiRaidCheckTimer -= uiDiff;

                // berserk
                if(m_uiBerserk_Timer < uiDiff)
                {
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    m_uiBerserk_Timer = 360000;
                }else m_uiBerserk_Timer -= uiDiff;

                // summons
                if(m_uiCollapsingStar_Timer < uiDiff && !m_bIsPhase2)
                {
                    DoScriptText(SAY_SUMMON_STAR, m_creature);
                    SummonCreature(NPC_COLLAPSING_STAR);
                    m_uiCollapsingStar_Timer = urand(15000, 20000);
                }else m_uiCollapsingStar_Timer -= uiDiff;

                if(m_uiLivingConstellationTimer < uiDiff && !m_bIsPhase2)
                {
                    for(uint8 i = 0; i < urand (1, 3); i++)
                        SummonCreature(NPC_LIVING_CONSTELLATION);
                    m_uiLivingConstellationTimer = 30000;
                }else m_uiLivingConstellationTimer -= uiDiff;

                if(m_uiDarkMaterTimer < uiDiff && !m_bIsPhase2)
                {
                    for(uint8 i = 0; i < 7; i++)
                        SummonCreature(NPC_DARK_MATTER);
                    m_uiDarkMaterTimer = 90000;
                }else m_uiDarkMaterTimer -= uiDiff;

                // hp check -> start phase 2
                if(!m_bIsPhase2 && m_creature->GetHealthPercent() < 20)
                {
                    DoScriptText(SAY_PHASE2, m_creature);
                    m_bIsPhase2 = true;

                    std::list<Creature*> lAdds;
                    GetCreatureListWithEntryInGrid(lAdds, m_creature, NPC_COLLAPSING_STAR, DEFAULT_VISIBILITY_INSTANCE);
                    GetCreatureListWithEntryInGrid(lAdds, m_creature, NPC_BLACK_HOLE, DEFAULT_VISIBILITY_INSTANCE);

                    if (!lAdds.empty())
                    {
                        for(std::list<Creature*>::iterator iter = lAdds.begin(); iter != lAdds.end(); ++iter)
                        {
                            if ((*iter) && (*iter)->isAlive())
                                (*iter)->ForcedDespawn();
                        }
                    }

                    for (uint8 i = 0; i < 4; ++i)
                    {
                        if(Creature* pTemp = m_creature->SummonCreature(NPC_BLACK_HOLE, PositionLoc[i].x, PositionLoc[i].y, m_creature->GetPositionZ(), 0, TEMPSUMMON_MANUAL_DESPAWN, 0))
                            ((mob_black_holeAI*)pTemp->AI())->m_bIsPhase2 = true;
                    }
                }

                DoMeleeAttackIfReady();

                EnterEvadeIfOutOfCombatArea(uiDiff);
            }
        }
        // outro: both for defeat and despawn
        if(m_bIsOutro)
        {
            switch(m_uiOutroStep)
            {
            case 1:
                m_creature->RemoveAllAuras();
                m_creature->DeleteThreatList();
                m_creature->CombatStop(true);
                m_creature->InterruptNonMeleeSpells(false);
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->GetMotionMaster()->MovePoint(0, 1631.970f, -302.635f, 417.321f);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                ++m_uiOutroStep;
                m_uiOutroTimer = 5000;
                break;
            case 3:
                if(m_bIsDespawned)
                {
                    DoScriptText(SAY_DESPAWN1, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 15000;
                }
                else
                {
                    DoScriptText(SAY_OUTRO1, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 37000;
                }
                break;
            case 5:
                if(m_bIsDespawned)
                {
                    DoScriptText(SAY_DESPAWN2, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 8000;
                }
                else
                {
                    DoScriptText(SAY_OUTRO2, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 17000;
                }
                break;
            case 7:
                if(m_bIsDespawned)
                {
                    DoScriptText(SAY_DESPAWN3, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 7000;
                }
                else
                {
                    DoScriptText(SAY_OUTRO3, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 12000;
                }
                break;
            case 9:
                if(m_bIsDespawned)
                {
                    DoCast(m_creature, SPELL_ASCEND);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 5000;
                }
                else
                {
                    DoScriptText(SAY_OUTRO4, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 11000;
                }
                break;
            case 11:
                if(m_bIsDespawned)
                {
                    if(m_pInstance)
                    {
                        m_pInstance->DoUpdateWorldState(UI_STATE_ALGALON_TIMER_ON, 0);
                        m_pInstance->SetData(TYPE_ALGALON, FAIL);
                    }
                    m_creature->ForcedDespawn();
                }
                else
                {
                    DoScriptText(SAY_OUTRO5, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 13000;
                }
                break;
            case 13:
                DoOutro();
                ++m_uiOutroStep;
                m_uiOutroTimer = 10000;
                break;
            }
        }
        else 
            return;

        if (m_uiOutroTimer <= uiDiff)
        {
            ++m_uiOutroStep;
            m_uiOutroTimer = 330000;
        } m_uiOutroTimer -= uiDiff;
    }
};

//Collapsing Star
struct MANGOS_DLL_DECL mob_collapsing_starAI : public ScriptedAI
{
    mob_collapsing_starAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiHealthTimer;
    uint32 m_uiDieTimer;
    uint32 m_uiSummonTimer;

    void Reset()
    {
        m_uiHealthTimer = 1000;
        m_uiDieTimer    = 600000;
        m_uiSummonTimer = 600000;
        m_creature->SetRespawnDelay(DAY);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if(uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;
            m_creature->SetHealth(m_creature->GetMaxHealth());
            m_uiDieTimer = 1500;
            m_uiSummonTimer = 1000;
            DoCast(m_creature, m_bIsRegularMode ? SPELL_BLACK_HOLE_EXPLOSION : SPELL_BLACK_HOLE_EXPLOSION_H);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_ALGALON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSummonTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_SUMMON_BLACK_HOLE);
            m_uiSummonTimer = 60000;
        }
        else m_uiSummonTimer -= uiDiff;

        if (m_uiDieTimer < uiDiff)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else
            m_uiDieTimer -= uiDiff;

        // movement should be improved
        // npc should ignore threat tables
        if(m_uiHealthTimer < uiDiff && m_creature->GetHealthPercent() > 1.0f)
        {
            m_creature->GetMotionMaster()->MoveConfused();
            m_creature->DealDamage(m_creature, (m_creature->GetMaxHealth() * 0.01), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            m_uiHealthTimer = 1000;
        }else m_uiHealthTimer -= uiDiff;
    }
};

//Living constellation
struct MANGOS_DLL_DECL mob_living_constellationAI : public ScriptedAI
{
    mob_living_constellationAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiArcaneBarrageTimer;

    void Reset()
    {
        m_uiArcaneBarrageTimer = 15000;
        m_creature->SetRespawnDelay(DAY);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_ALGALON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiArcaneBarrageTimer < uiDiff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0)){
                DoCast(target, m_bIsRegularMode ? SPELL_ARCANE_BARRAGE : SPELL_ARCANE_BARRAGE_H);
            }
            m_uiArcaneBarrageTimer = 15000;
        }else m_uiArcaneBarrageTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL mob_cosmic_smash_targetAI : public ScriptedAI
{
    mob_cosmic_smash_targetAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->SetDisplayId(11686);     // make invisible
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSpellTimer;

    void Reset()
    {
        m_uiSpellTimer = 5000;
        m_creature->SetRespawnDelay(DAY);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_ALGALON) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpellTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_COSMIC_SMASH_MISSILE);
            m_uiSpellTimer = 60000;
        }else m_uiSpellTimer -= uiDiff;
    }
};


bool GOHello_go_celestial_acces(Player* pPlayer, GameObject* pGo)
{
    instance_ulduar* m_pInstance = (instance_ulduar*)pGo->GetInstanceData();
    bool m_bIsRegularMode = pGo->GetMap()->IsRegularDifficulty();
    bool m_bHasItem = false;

    // check if the player has the key
    if (m_bIsRegularMode)
    {
        if(pPlayer->HasItemCount(ITEM_PLANETARIUM_KEY, 1) || pPlayer->HasItemCount(ITEM_PLANETARIUM_KEY_H, 1)) 
            m_bHasItem = true;
    }
    else
    {
        if(pPlayer->HasItemCount(ITEM_PLANETARIUM_KEY_H, 1))
            m_bHasItem = true;
    }

    if(!m_bHasItem)
        return false;

    if (!m_pInstance)
        return false;

    // disable if encounter is already done
    if (m_pInstance->GetData(TYPE_ALGALON) == DONE)
    {
        pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
        return false;
    }

    // start encounter
    if (Creature* pAlgalon = m_pInstance->GetSingleCreatureFromStorage(NPC_ALGALON))
    {
        if(pAlgalon->isAlive())
        {
            ((boss_algalonAI*)pAlgalon->AI())->StartEncounter();
            pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);

            // open celestial door
            if(GameObject* pDoor = m_pInstance->GetSingleGameObjectFromStorage(GO_CELESTIAL_DOOR))
                m_pInstance->DoUseDoorOrButton(pDoor->GetObjectGuid());
        }
    }

    return false;
}

CreatureAI* GetAI_boss_algalon(Creature* pCreature)
{
    return new boss_algalonAI(pCreature);
}

CreatureAI* GetAI_mob_collapsing_star(Creature* pCreature)
{
    return new mob_collapsing_starAI(pCreature);
}

CreatureAI* GetAI_mob_living_constellation(Creature* pCreature)
{
    return new mob_living_constellationAI(pCreature);
}

CreatureAI* GetAI_mob_cosmic_smash_target(Creature* pCreature)
{
    return new mob_cosmic_smash_targetAI(pCreature);
}

CreatureAI* GetAI_mob_black_hole(Creature* pCreature)
{
    return new mob_black_holeAI(pCreature);
}

void AddSC_boss_algalon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_algalon";
    newscript->GetAI = &GetAI_boss_algalon;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_collapsing_star";
    newscript->GetAI = &GetAI_mob_collapsing_star;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_living_constellation";
    newscript->GetAI = &GetAI_mob_living_constellation;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_cosmic_smash_target";
    newscript->GetAI = &GetAI_mob_cosmic_smash_target;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_black_hole";
    newscript->GetAI = &GetAI_mob_black_hole;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_celestial_acces";
    newscript->pGOUse = &GOHello_go_celestial_acces;
    newscript->RegisterSelf();
}
