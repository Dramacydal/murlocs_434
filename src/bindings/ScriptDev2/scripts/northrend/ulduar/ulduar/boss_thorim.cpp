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
SDName: boss_thorim
SD%Complete:
SDComment:  better solution for teleport into arena
            Implement lightning orbs, summon Sit on the platform in the first 3 min.
            Runic Smash from Runic Colossus need core support, unknow trigger spell 62406 and 62403
            Achievments: Don't Stand in the Lightning, Who Needs Bloodlust?
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //yells
    SAY_AGGRO1          = -1603190,
    SAY_AGGRO2          = -1603191,
    SAY_SPECIAL1        = -1603192,
    SAY_SPECIAL2        = -1603193,
    SAY_SPECIAL3        = -1603194,
    SAY_JUMP            = -1603195,
    SAY_SLAY1           = -1603196,
    SAY_SLAY2           = -1603197,
    SAY_BERSERK         = -1603198,
    SAY_ARENA_WIPE      = -1603199,
    SAY_DEATH           = -1603200,
    SAY_OUTRO1          = -1603201,
    SAY_OUTRO2          = -1603202,
    SAY_OUTRO3          = -1603203,
    SAY_OUTRO_HARD1     = -1603204,
    SAY_OUTRO_HARD2     = -1603205,
    SAY_OUTRO_HARD3     = -1603206,

    // Sif
    SAY_SIF_INTRO           = -1603210,
    SAY_SIF_EVENT           = -1603211,
    SAY_SIF_DESPAWN         = -1603212,

    SPELL_THORIM_CREDIT     = 64985,        // custom spell in spell_dbc.sql

    // spells
    // phase1
    SPELL_SHEAT_OF_LIGHTNING        = 62276,
    SPELL_STORMHAMMER               = 62042,
    SPELL_DEAFENING_THUNDER         = 62470,
    SPELL_LIGHTNING_SHOCK           = 62017,
    SPELL_CHARGE_ORB                = 62016,
    NPC_THUNDER_ORB                 = 33378,    // npc used to cast charged orb
    SPELL_BERSERK_ADDS              = 62560,    // 5 min phase 1 -> for adds
    SPELL_SUMMON_LIGHTNING_ORB      = 62391,
    // phase2
    SPELL_TOUTCH_OF_DOMINION        = 62565,    // not available in hard mode
    SPELL_CHAIN_LIGHTNING           = 62131,
    SPELL_CHAIN_LIGHTNING_H         = 64390,
    SPELL_LIGHTNING_CHARGE          = 62279,
    SPELL_LIGHTNING_CHARGE_ORB      = 62466,
    SPELL_UNBALANCING_STRIKE        = 62130,
    SPELL_BERSERK                   = 26662,    // 5 min phase 2

    // hard mode
    SPELL_FROSTBOLT_VOLLEY          = 62580,
    SPELL_FROSTBOLT_VOLLEY_H        = 62604,
    SPELL_FROST_NOVA                = 62597,
    SPELL_FROST_NOVA_H              = 62605,
    SPELL_BLIZZARD                  = 62576,
    SPELL_BLIZZARD_H                = 62602,
    NPC_SIF                         = 33196,
    SPELL_SOUL_CHANNEL              = 40401,

    // arena
    MOB_DARK_RUNE_CHAMPION          = 32876,
    MOB_DARK_RUNE_COMMONER          = 32904,
    MOB_DARK_RUNE_EVOKER            = 32878,
    MOB_DARK_RUNE_WARBRINGER        = 32877,

    // traps
    NPC_TRAP_BUNNY                  = 33725,
    NPC_TRAP_BUNNY2                 = 33054,
    SPELL_PARALYTIC_FIELD           = 63540,
    SPELL_PARALYTIC_FIELD2          = 62241,

    // mobs spells
    // acolyte
    SPELL_GREATER_HEAL              = 62334,
    SPELL_GREATER_HEAL_H            = 62442,
    SPELL_RENEW                     = 62333,
    SPELL_RENEW_H                   = 62441,
    SPELL_HOLY_SMITE                = 62335,
    SPELL_HOLY_SMITE_H              = 62443,
    // champion
    SPELL_MORTAL_STRIKE             = 35054,
    SPELL_CHARGE_CHAMPION           = 32323,
    SPELL_WHIRLWIND                 = 15578,
    // commoner         
    SPELL_LOW_BLOW                  = 62326,
    SPELL_PUMMEL                    = 38313,
    // evoker
    SPELL_RUNIC_LIGHTNING           = 62327,
    SPELL_RUNIC_LIGHTNING_H         = 62445,
    SPELL_RUNIC_MENDING             = 62328,
    SPELL_RUNIC_MENDING_H           = 62446,
    SPELL_RUNIC_SHIELD              = 62321,
    SPELL_RUNIC_SHIELD_H            = 62529,
    // warbringer
    SPELL_RUNIC_STRIKE              = 62322,
    SPELL_AURA_CELERITY             = 62320,

    // ring guard
    SPELL_WHIRLING_TRIP             = 64151,
    SPELL_IMPALE                    = 62331,
    SPELL_IMPALE_H                  = 62418,
    // honor guard
    SPELL_CLEAVE                    = 42724, 
    SPELL_HAMSTRING                 = 48639,
    SPELL_SHIELD_SMASH              = 62332,
    SPELL_SHIELD_SMASH_H            = 62420,

    // hallway
    MOB_DARK_RUNE_ACOLYTE           = 33110,
    MOB_IRON_RING_GUARD             = 32874,

    MOB_IRON_HONOR_GUARD            = 33125,

    // Colossus
    SPELL_SMASH                     = 62339,
    //SPELL_SMASH_RIGHT             = 62414,
    SPELL_RUNIC_SMASH_L             = 62058,
    SPELL_RUNIC_SMASH_R             = 62057,
    SPELL_RUNIC_SMASH_DMG           = 62465,
    SPELL_RUNIC_BARRIER             = 62338,
    SPELL_CHARGE                    = 62613,
    SPELL_CHARGE_H                  = 62614,

    MOB_IRON_HOHOR_GUARD            = 32875,
    MINIBOSS_ANCIENT_RUNE_GIANT     = 32873,
    SPELL_RUNIC_FORTIFICATION       = 62942,
    SPELL_STOMP                     = 62411,
    SPELL_STOMP_H                   = 62413,
    SPELL_RUNE_DETONATION           = 62526,

    // pre adds:
    SPELL_ACID_BREATH               = 62315,
    SPELL_ACID_BREATH_H             = 62415,
    SPELL_SWEEP                     = 62316,
    SPELL_SWEEP_H                   = 62417,
    // captains
    SPELL_DEVASTATE                 = 62317,
    SPELL_HEROIC_STRIKE             = 62444,
    // mercenary
    SPELL_SHOOT                     = 16496,
    SPELL_BARBED_SHOT               = 62318,
    SPELL_WING_CLIP                 = 40652,

    ACHIEV_LOSE_ILLUSION            = 3176,
    ACHIEV_LOSE_ILLUSION_H          = 3183,
    ACHIEV_SIFFED                   = 2977,
    ACHIEV_SIFFED_H                 = 2978,
};

enum phases
{
    PHASE_PREADDS       = 0,
    PHASE_INTRO         = 1,
    PHASE_BALCONY       = 2,
    PHASE_ARENA         = 3,
    PHASE_OUTRO         = 4,
};

#define LOC_Z                       419.5f  
struct LocationsXY
{
    float x, y, z;
    uint32 id;
};
static LocationsXY ArenaLoc[]=
{
    {2158.082f, -240.572f},
    {2111.883f, -240.561f},
    {2105.243f, -274.499f},
    {2163.927f, -277.834f},
    {2104.865f, -251.027f},
    {2167.612f, -262.128f},
};

static LocationsXY OrbLoc[]=
{
    {2134.57f, -440.31f, 438.33f},
    {2225.91f, -431.68f, 412.17f},
    {2228.26f, -266.46f, 412.17f},
};

// trap bunny
struct MANGOS_DLL_DECL mob_thorim_trap_bunnyAI : public ScriptedAI
{
    mob_thorim_trap_bunnyAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        pCreature->setFaction(14);
        Reset();
    }

    bool m_bHasStunAura;
    uint32 m_uiAuraExpireTimer;

    void Reset()
    {
       m_bHasStunAura = false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho->isTargetableForAttack() && pWho->isInAccessablePlaceFor(m_creature) && !m_bHasStunAura &&
            pWho->GetTypeId() == TYPEID_PLAYER && m_creature->IsWithinDistInMap(pWho, 12) && m_creature->IsWithinLOSInMap(pWho))
        {
            m_bHasStunAura = true;
            m_uiAuraExpireTimer = 15000;
            DoCast(m_creature, SPELL_PARALYTIC_FIELD);
        }
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiAuraExpireTimer < uiDiff && m_bHasStunAura)
            m_bHasStunAura = false;
        else m_uiAuraExpireTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_thorim_trap_bunny(Creature* pCreature)
{
    return new mob_thorim_trap_bunnyAI(pCreature);
}

// dark rune acolyte
struct MANGOS_DLL_DECL mob_dark_rune_acolyteAI : public ScriptedAI
{
    mob_dark_rune_acolyteAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(3000, 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            switch(urand(0, 4))
            {
                case 0:
                case 1:
                    if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_GREATER_HEAL : SPELL_GREATER_HEAL_H);
                break;
                case 2:
                case 3:
                    if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_RENEW : SPELL_RENEW_H);
                break;
                case 4:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_HOLY_SMITE : SPELL_HOLY_SMITE_H);
                break;
            }
            m_uiSpell_Timer = urand(3000, 6000);
        }else m_uiSpell_Timer -= uiDiff;        
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_acolyte(Creature* pCreature)
{
    return new mob_dark_rune_acolyteAI(pCreature);
}

// dark rune champion
struct MANGOS_DLL_DECL mob_dark_rune_championAI : public ScriptedAI
{
    mob_dark_rune_championAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(3000, 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            switch(urand(0, 2))
            {
                case 0:
                    DoCast(m_creature->getVictim(), SPELL_MORTAL_STRIKE);
                break;
                case 1:
                    DoCast(m_creature->getVictim(), SPELL_CHARGE_CHAMPION);
                break;
                case 2:
                    DoCast(m_creature->getVictim(), SPELL_WHIRLWIND);
                break;
            }
            m_uiSpell_Timer = urand(3000, 6000);
        }else m_uiSpell_Timer -= uiDiff;        
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_champion(Creature* pCreature)
{
    return new mob_dark_rune_championAI(pCreature);
}

// dark rune commoner
struct MANGOS_DLL_DECL mob_dark_rune_commonerAI : public ScriptedAI
{
    mob_dark_rune_commonerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(3000, 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            switch(urand(0, 1))
            {
                case 0:
                    DoCast(m_creature->getVictim(), SPELL_LOW_BLOW);
                    break;
                case 1:
                    DoCast(m_creature->getVictim(), SPELL_PUMMEL);
                    break;
            }
            m_uiSpell_Timer = urand(3000, 6000);
        }else m_uiSpell_Timer -= uiDiff;        
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_commoner(Creature* pCreature)
{
    return new mob_dark_rune_commonerAI(pCreature);
}

// dark rune evoker
struct MANGOS_DLL_DECL mob_dark_rune_evokerAI : public ScriptedAI
{
    mob_dark_rune_evokerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(3000, 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            switch(urand(0, 4))
            {
                case 0:
                case 1:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_RUNIC_LIGHTNING : SPELL_RUNIC_LIGHTNING_H);
                    break;
                case 2:
                case 3:
                    if (Unit* pTarget = DoSelectLowestHpFriendly(50.0f))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_RUNIC_MENDING : SPELL_RUNIC_MENDING_H);
                    break;
                case 4:
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_RUNIC_SHIELD : SPELL_RUNIC_SHIELD_H);
                    break;
            }
            m_uiSpell_Timer = urand(3000, 6000);
        }else m_uiSpell_Timer -= uiDiff;        
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_evoker(Creature* pCreature)
{
    return new mob_dark_rune_evokerAI(pCreature);
}

// dark rune warbringer
struct MANGOS_DLL_DECL mob_dark_rune_warbringerAI : public ScriptedAI
{
    mob_dark_rune_warbringerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(4000, 7000);
        DoCast(m_creature, SPELL_AURA_CELERITY);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            DoCast(m_creature->getVictim(), SPELL_RUNIC_STRIKE);
            m_uiSpell_Timer = urand(4000, 7000);
        }else m_uiSpell_Timer -= uiDiff;        

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_warbringer(Creature* pCreature)
{
    return new mob_dark_rune_warbringerAI(pCreature);
}

// dark rune ring guard
struct MANGOS_DLL_DECL mob_dark_rune_ring_guardAI : public ScriptedAI
{
    mob_dark_rune_ring_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(3000, 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            switch(urand(0, 1))
            {
                case 0:
                    DoCast(m_creature->getVictim(), SPELL_WHIRLING_TRIP);
                break;
                case 1:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_IMPALE : SPELL_IMPALE_H);
                break;
            }
            m_uiSpell_Timer = urand(3000, 6000);
        }else m_uiSpell_Timer -= uiDiff;        
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_ring_guard(Creature* pCreature)
{
    return new mob_dark_rune_ring_guardAI(pCreature);
}

// dark rune honor guard
struct MANGOS_DLL_DECL mob_dark_rune_honor_guardAI : public ScriptedAI
{
    mob_dark_rune_honor_guardAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(3000, 6000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < uiDiff)
        {
            switch(urand(0, 2))
            {
                case 0:
                    DoCast(m_creature->getVictim(), SPELL_CLEAVE);
                break;
                case 1:
                    DoCast(m_creature->getVictim(), SPELL_HAMSTRING);
                break;
                case 2:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SHIELD_SMASH : SPELL_SHIELD_SMASH_H);
                break;
            }
            m_uiSpell_Timer = urand(3000, 6000);
        }else m_uiSpell_Timer -= uiDiff;        
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_honor_guard(Creature* pCreature)
{
    return new mob_dark_rune_honor_guardAI(pCreature);
}

// thorim
struct MANGOS_DLL_DECL boss_thorimAI : public ScriptedAI
{
    boss_thorimAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    uint32 m_uiPhase;

    uint32 m_uiArenaBerserkTimer;
    uint32 m_uiBerserkTimer;
    uint32 m_uiArenaYellTimer;
    uint32 m_uiStormHammerTimer;
    uint32 m_uiDeafeningThunderTimer;
    uint32 m_uiChargeOrbTimer;
    uint32 m_uiSummonWavesTimer;
    uint32 m_uiJumpTimer;
    ObjectGuid m_uiStormTargetGUID;

    uint32 m_uiChainLightningTimer;
    uint32 m_uiLightningChargeTimer;
    uint32 m_uiOrbChargeTimer;
    uint32 m_uiUnbalancingStrikeTimer;

    uint32 m_uiHardModeTimer;

    bool m_bIsPhaseOneEnd;
    bool m_bIsHardMode;
    uint32 m_uiPreAddsKilled;

    ObjectGuid m_uiSifGUID;

    //Arena Door
    uint32 m_uiDoorArenaTimer;

    // intro & outro
    bool m_bIsOutro;
    uint32 m_uiOutroTimer;
    uint32 m_uiOutroStep;
    bool m_bIsIntro;
    uint32 m_uiIntroTimer;
    uint32 m_uiIntroStep;

    // mob list check
    GUIDList lIronDwarfes;
    GUIDList m_lOrbsGUIDList;
    GUIDList m_lArenaSummonGUID;

    void Reset()
    {
        m_uiPreAddsKilled       = 0;
        m_uiPhase               = PHASE_PREADDS;
        SetCombatMovement(false);

        m_bIsHardMode           = true;
        m_bIsPhaseOneEnd        = false;

        m_uiArenaBerserkTimer   = 280000; // 5 min - 20 secs intro
        m_uiBerserkTimer        = 300000; // 5 min
        m_uiHardModeTimer       = 160000; // 3 min - 20 sec intro
        m_uiArenaYellTimer      = 30000;
        m_uiSummonWavesTimer    = 10000;
        m_uiJumpTimer           = 5000;

        m_uiStormHammerTimer        = 20000;
        m_uiDeafeningThunderTimer   = 22000;
        m_uiChargeOrbTimer          = 15000;

        m_uiChainLightningTimer     = urand(10000, 15000);
        m_uiLightningChargeTimer    = urand(10000, 15000);
        m_uiUnbalancingStrikeTimer  = urand(15000, 20000);
        m_uiOrbChargeTimer          = 20000;

        m_uiOutroTimer          = 10000;
        m_uiOutroStep           = 1;
        m_bIsIntro              = false;
        m_uiIntroTimer          = 10000;
        m_uiIntroStep           = 1;
        m_bIsOutro              = false;
        lIronDwarfes.clear();
        m_lOrbsGUIDList.clear();

        // exploit check
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE); 

        for (GUIDList::iterator itr = m_lArenaSummonGUID.begin(); itr != m_lArenaSummonGUID.end(); itr++)
        {
            if (Creature *pTmp = m_creature->GetMap()->GetCreature(*itr))
            {
                pTmp->ForcedDespawn();
            }
        }
        if (m_pInstance)
        {
            for (GUIDList::iterator itr = m_pInstance->m_lThorimMobsGuids.begin(); itr != m_pInstance->m_lThorimMobsGuids.end(); itr++)
                if (Creature *pTmp = m_pInstance->instance->GetCreature(*itr))
                    if (!pTmp->isAlive())
                        pTmp->Respawn();
        }


            
    }
    // support CORE!!!
    void SpellHitTarget(Unit* pSpellTarget, const SpellEntry* pSpell)
    {
        if(pSpell->Id == SPELL_STORMHAMMER)
            pSpellTarget->CastSpell(pSpellTarget, SPELL_DEAFENING_THUNDER, false,0,0,m_creature->GetObjectGuid());
    }

    void JustReachedHome()
    {
        if(m_pInstance)
        {
            m_pInstance->SetData(TYPE_THORIM, FAIL);
            m_pInstance->SetData(TYPE_THORIM_HARD, FAIL);
        }

    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_SLAY1 : SAY_SLAY2, m_creature);
    }

    void DoOutro()
    {
        if(m_pInstance) 
        {
            if(m_bIsHardMode)
            {
                m_pInstance->SetData(TYPE_THORIM_HARD, DONE);
            }
            m_pInstance->SetData(TYPE_THORIM, DONE);
        }
        DoCast(m_creature, SPELL_THORIM_CREDIT, true);
        m_creature->ForcedDespawn();
    }

    // for debug only
    void JustDied(Unit* pKiller)
    {
        if(m_pInstance) 
        {
            if(m_bIsHardMode)
                m_pInstance->SetData(TYPE_THORIM_HARD, DONE);
            m_pInstance->SetData(TYPE_THORIM, DONE);
        }
        DoCast(m_creature, SPELL_THORIM_CREDIT, true);
    }

    // start outro
    void DamageTaken(Unit *done_by, uint32 &uiDamage)
    {
        // outro
        if((m_creature->GetHealthPercent() < 1.0f || m_creature->GetHealth() <= uiDamage) && m_uiPhase == PHASE_ARENA)
        {
            if (m_bIsHardMode)
            {
                if (m_pInstance)
                    m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_LOSE_YOUR_ILLUSION, true);
            }
            uiDamage = 0;
            m_uiPhase = PHASE_OUTRO;
        }
    }

    void StartEncounter()
    {
        m_uiPhase   = PHASE_INTRO;
        m_bIsIntro  = true;
    }

    // hacky way for berserk in phase 1 :)
    void KillPlayers()
    {
        Map *map = m_creature->GetMap();
        if (map->IsDungeon())
        {
            Map::PlayerList const &PlayerList = map->GetPlayers();

            if (PlayerList.isEmpty())
                return;

            for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
            {
                if (i->getSource()->isAlive() && m_creature->GetDistance(i->getSource()->GetPositionX(), i->getSource()->GetPositionY(), i->getSource()->GetPositionZ()) < 200.0f)
                    i->getSource()->DealDamage(i->getSource(), i->getSource()->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        } 
    }

    Creature* SelectRandomOrb()
    {
        std::list<Creature* > lThunderList;
        GetCreatureListWithEntryInGrid(lThunderList, m_creature, NPC_THUNDER_ORB, 100.0f);
 
        //This should not appear!
        if (lThunderList.empty()){
            m_uiChargeOrbTimer = 5000;
            return NULL;
        }
            
        std::list<Creature* >::iterator iter = lThunderList.begin();
        advance(iter, urand(0, lThunderList.size()-1));
 
        return *iter;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        switch(m_uiPhase)
        {
            // start the encounter when all the preadds have died
            case PHASE_PREADDS:
                if(m_uiPreAddsKilled == 4)
                    StartEncounter();
                break;
            // do intro
            case PHASE_INTRO:
            {
                // intro
                if(m_bIsIntro)
                {
                    switch(m_uiIntroStep)
                    {
                        case 1:
                            // wait 10 secs
                            ++m_uiIntroStep;
                            m_uiIntroTimer = 10000;
                            break;
                        case 3:
                            DoScriptText(SAY_AGGRO1, m_creature);
                            DoCast(m_creature, SPELL_SHEAT_OF_LIGHTNING);
                            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            m_creature->SetInCombatWithZone();
                            if (m_pInstance)
                                m_pInstance->SetData(TYPE_THORIM, IN_PROGRESS);
                            ++m_uiIntroStep;
                            m_uiIntroTimer = 10000;
                            break;
                        case 5:
                            DoScriptText(SAY_AGGRO2, m_creature);
                            if(Creature* pSif = m_creature->SummonCreature(NPC_SIF, m_creature->GetPositionX() + 10, m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_TIMED_OOC_DESPAWN, 180000))
                            {
                                pSif->setFaction(35);
                                m_uiSifGUID = pSif->GetObjectGuid();
                            }
                            ++m_uiIntroStep;
                            m_uiIntroTimer = 9000;
                            break;
                        case 7:
                            if(Creature* pSif = m_pInstance->instance->GetCreature(m_uiSifGUID))
                                DoScriptText(SAY_SIF_INTRO, pSif);
                            m_uiPhase = PHASE_BALCONY;
                            m_bIsIntro = false;
                            ++m_uiIntroStep;
                            m_uiIntroTimer = 9000;
                            break;
                        default:
                            break;
                    }
                }
                else return;

                if (m_uiIntroTimer <= uiDiff)
                {
                    ++m_uiIntroStep;
                    m_uiIntroTimer = 330000;
                } m_uiIntroTimer -= uiDiff;

                break;
            }
            // balcony phase
            case PHASE_BALCONY:
            {
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                // phase 2
                if(!m_bIsPhaseOneEnd)
                {
                    if(m_pInstance->GetData(TYPE_RUNIC_COLOSSUS) == DONE && m_pInstance->GetData(TYPE_RUNE_GIANT) == DONE)
                    {
                        if (GetPlayerAtMinimumRange(15.0f))
                        {                           
                            // say
                            DoScriptText(SAY_JUMP, m_creature);
                            m_creature->RemoveAurasDueToSpell(SPELL_SHEAT_OF_LIGHTNING);
                            // move in arena
                            SetCombatMovement(true);
                            m_bIsPhaseOneEnd = true;
                            if(m_bIsHardMode)
                            {
                                if(Creature* Sif = m_pInstance->instance->GetCreature(m_uiSifGUID))
                                {
                                    Sif->setFaction(14);
                                    DoScriptText(SAY_SIF_EVENT, Sif);
                                    Sif->SetInCombatWithZone();
                                    Sif->MonsterMoveJump(2134.719f, -263.148f, 419.846f, 30, 30);
                                    if (m_pInstance)
                                        m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_SIFFED, true);
                                }
                            }
                            else
                            {
                                DoCast(m_creature, SPELL_TOUTCH_OF_DOMINION, true);
                            }
                        }
                    }
                }
                if (m_bIsPhaseOneEnd)
                {
                    if(m_uiJumpTimer < uiDiff && m_bIsPhaseOneEnd)
                    {
                        m_creature->GetMap()->CreatureRelocation(m_creature, 2134.719f, -263.148f, 419.846f, 0);
                        m_creature->Relocate(2134.719f, -263.148f, 419.846f, 0);
                        m_creature->GetMotionMaster()->MovePoint(0, 2134.719f, -263.148f, 419.846f);
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                        m_uiPhase = PHASE_ARENA;
                    }
                    else
                        m_uiJumpTimer -= uiDiff;
                }

                // hard mode check
                if (m_uiHardModeTimer <= uiDiff && m_bIsHardMode)
                {
                    m_bIsHardMode = false;
                    if(Creature* Sif = m_pInstance->instance->GetCreature(m_uiSifGUID))
                    {
                        if(Sif && Sif->isAlive())
                        {
                            DoScriptText(SAY_SIF_DESPAWN, Sif);
                            Sif->ForcedDespawn();
                        }
                    }
                    m_uiHardModeTimer = 330000;
                } m_uiHardModeTimer -= uiDiff;

                // spawn adds in arena
                if(m_uiSummonWavesTimer < uiDiff)
                {
                    // 1-2 warbringer
                    // 1 evoker
                    // 5-6 commoners
                    // 1 champion
                    // 1 acolyte
                    uint8 i;
                    uint8 k;    
                    switch(urand(0, 4))
                    {
                        case 0:
                            i = urand(0, 5);
                            if(Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_CHAMPION, ArenaLoc[i].x, ArenaLoc[i].y, LOC_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                            {
                                pTemp->GetMotionMaster()->MovePoint(0, 2134.72f, -263.148f, 419.846f);
                                if(pTemp->IsWithinLOSInMap(m_creature->getVictim()))
                                {
                                    pTemp->AI()->AttackStart(m_creature->getVictim());
                                    pTemp->AddThreat(m_creature->getVictim(), 100.0f);
                                }
                            }
                            break;
                        case 1:
                            i = urand(0, 5);
                            if(Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_EVOKER, ArenaLoc[i].x, ArenaLoc[i].y, LOC_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                            {
                                pTemp->GetMotionMaster()->MovePoint(0, 2134.72f, -263.148f, 419.846f);
                                if(pTemp->IsWithinLOSInMap(m_creature->getVictim()))
                                {
                                    pTemp->AI()->AttackStart(m_creature->getVictim());
                                    pTemp->AddThreat(m_creature->getVictim(), 100.0f);
                                }
                            }
                            break;
                        case 2:
                            i = urand(5, 6);
                            for(uint8 j = 0; j < i; j++)
                            {
                                if(Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_COMMONER, ArenaLoc[j].x, ArenaLoc[j].y, LOC_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                                {
                                    pTemp->GetMotionMaster()->MovePoint(0, 2134.72f, -263.148f, 419.846f);
                                    if(pTemp->IsWithinLOSInMap(m_creature->getVictim()))
                                    {
                                        pTemp->AI()->AttackStart(m_creature->getVictim());
                                        pTemp->AddThreat(m_creature->getVictim(), 100.0f);
                                    }
                                }
                            }
                            break;
                        case 3:
                            k = urand(0, 3);
                            i = urand(k + 1, k + 2);
                            for(uint8 j = k; j < i; j++)
                            {
                                if(Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_WARBRINGER, ArenaLoc[j].x, ArenaLoc[j].y, LOC_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                                {
                                    pTemp->GetMotionMaster()->MovePoint(0, 2134.72f, -263.148f, 419.846f);
                                    if(pTemp->IsWithinLOSInMap(m_creature->getVictim()))
                                    {
                                        pTemp->AI()->AttackStart(m_creature->getVictim());
                                        pTemp->AddThreat(m_creature->getVictim(), 100.0f);
                                    }
                                }
                            }
                            break;
                        case 4:
                            i = urand(0, 5);
                            if(Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_ACOLYTE, ArenaLoc[i].x, ArenaLoc[i].y, LOC_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 120000))
                            {
                                pTemp->GetMotionMaster()->MovePoint(0, 2134.72f, -263.148f, 419.846f);
                                if(pTemp->IsWithinLOSInMap(m_creature->getVictim()))
                                {
                                    pTemp->AI()->AttackStart(m_creature->getVictim());
                                    pTemp->AddThreat(m_creature->getVictim(), 100.0f);
                                }
                            }
                            break;
                    }
                    m_uiSummonWavesTimer = urand (5000, 7000);
                }
                else m_uiSummonWavesTimer -= uiDiff; 

                // phase 1 spells
                // charge orb
                // doesn't work right, needs fixing
                if(m_uiChargeOrbTimer < uiDiff)
                {
                    if (Creature* pOrb = SelectRandomOrb())
                        DoCast(pOrb, SPELL_CHARGE_ORB);
                    m_uiChargeOrbTimer = 20000;
                }
                else m_uiChargeOrbTimer -= uiDiff; 

                // storm hammer
                if(m_uiStormHammerTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {                        
                        // should target only the players in the arena!
                        if(pTarget->IsInRange2d(2134.0f, -263.0f ,0 , 50))  // middle point of arena - width of arena ~45m
                        {
                            DoCast(pTarget, SPELL_STORMHAMMER);
                            m_uiStormHammerTimer = 15000;
                        }
                    }
                }
                else m_uiStormHammerTimer -= uiDiff; 

                if(m_uiArenaYellTimer < uiDiff)
                {
                    switch(urand(0, 2))
                    {
                        case 0: DoScriptText(SAY_SPECIAL1, m_creature); break;
                        case 1: DoScriptText(SAY_SPECIAL2, m_creature); break;
                        case 2: DoScriptText(SAY_SPECIAL3, m_creature); break;
                    }
                    m_uiArenaYellTimer = 30000;
                }
                else m_uiArenaYellTimer -= uiDiff;

                // phase 1 berserk
                if(m_uiArenaBerserkTimer < uiDiff)
                {
                    DoScriptText(SAY_ARENA_WIPE, m_creature);
                    //DoCast(m_creature, SPELL_BERSERK_ADDS);
                    // workaround because berserk doesn't work. It's casted on players not on adds. Needs core fix
                    KillPlayers();
                    DoCast(m_creature, SPELL_SUMMON_LIGHTNING_ORB);
                    m_uiArenaBerserkTimer = 30000;
                }
                else m_uiArenaBerserkTimer -= uiDiff;

                break;
            }
            // arena phase
            case PHASE_ARENA:
            {
                if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                    return;

                // all spells
                // chain lightning
                if(m_uiChainLightningTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);
                    m_uiChainLightningTimer = 10000 + rand()%5000;
                }
                else m_uiChainLightningTimer -= uiDiff; 

                // lightning charge
                if(m_uiLightningChargeTimer < uiDiff)
                {
                    DoCast(m_creature, SPELL_LIGHTNING_CHARGE);
                    m_uiLightningChargeTimer = 15000;
                    m_uiOrbChargeTimer = 2000;
                }
                else m_uiLightningChargeTimer -= uiDiff; 

                if(m_uiOrbChargeTimer < uiDiff)
                {
                    if (Creature* pOrb = SelectRandomOrb())
                        DoCast(pOrb, SPELL_LIGHTNING_CHARGE_ORB);
                    m_uiOrbChargeTimer = 20000;
                }
                else m_uiOrbChargeTimer -= uiDiff;

                // unbalancing strike
                if(m_uiUnbalancingStrikeTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                        DoCast(pTarget, SPELL_UNBALANCING_STRIKE);
                    m_uiUnbalancingStrikeTimer = 25000;
                }
                else m_uiUnbalancingStrikeTimer -= uiDiff; 

                // phase 2 berserk
                if(m_uiBerserkTimer < uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    DoScriptText(SAY_BERSERK, m_creature);
                    DoCast(m_creature, SPELL_BERSERK);
                    m_uiBerserkTimer = 30000;
                }
                else m_uiBerserkTimer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            // outro
            case PHASE_OUTRO:
            {
                switch(m_uiOutroStep)
                {
                case 1:
                    m_creature->setFaction(35);
                    m_creature->RemoveAllAuras();
                    m_creature->DeleteThreatList();
                    m_creature->CombatStop(true);
                    m_creature->InterruptNonMeleeSpells(false);
                    m_creature->SetHealth(m_creature->GetMaxHealth());
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 1000;
                    break;
                case 3:
                    m_creature->SetOrientation(4.99f);
                    DoScriptText(SAY_DEATH, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 3000;
                    break;
                case 5:
                    if(m_bIsHardMode)
                    {
                        DoScriptText(SAY_OUTRO_HARD1, m_creature);
                        if(Creature* Sif = m_pInstance->instance->GetCreature(m_uiSifGUID))
                            DoCast(Sif, SPELL_STORMHAMMER);
                    }
                    else
                        DoScriptText(SAY_OUTRO1, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 1000;
                    break;
                case 7:
                    if(m_bIsHardMode)
                    {
                        if(Creature* Sif = m_pInstance->instance->GetCreature(m_uiSifGUID))
                        {
                            //summon a tentacule
                            if(Creature* pTentacule = m_creature->SummonCreature(34266, Sif->GetPositionX(), Sif->GetPositionY(), Sif->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 7000))
                            {
                                pTentacule->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                                pTentacule->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            }
                            Sif->ForcedDespawn();
                        }
                    }
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 7000;
                    break;
                case 9:
                    if(m_bIsHardMode)
                        DoScriptText(SAY_OUTRO_HARD2, m_creature);
                    else
                        DoScriptText(SAY_OUTRO2, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 13000;
                    break;
                case 11:
                    if(m_bIsHardMode)
                        DoScriptText(SAY_OUTRO_HARD3, m_creature);
                    else
                        DoScriptText(SAY_OUTRO3, m_creature);
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 15000;
                    break;
                case 13:
                    DoOutro();
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 10000;
                    break;
                }

                if (m_uiOutroTimer <= uiDiff)
                {
                    ++m_uiOutroStep;
                    m_uiOutroTimer = 330000;
                } m_uiOutroTimer -= uiDiff;

                break;
            }
        }            
    }
};

CreatureAI* GetAI_boss_thorim(Creature* pCreature)
{
    return new boss_thorimAI(pCreature);
}

struct MANGOS_DLL_DECL boss_runic_colossusAI : public ScriptedAI
{
    boss_runic_colossusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    uint32 m_uiSpellTimer;
    uint32 m_uiRunicBarrierTimer;
    uint32 m_uiSmashTimer;
    uint32 m_uiTriggerSmashTimer;
    bool m_bIsSmash;

    bool m_bIsLeft;

    void Reset()
    {
        m_uiSpellTimer = urand(5000, 10000);
        m_uiRunicBarrierTimer = 15000;
        m_uiSmashTimer  = 3000;
        m_uiTriggerSmashTimer = 20000;
        m_bIsSmash  = true;
        m_bIsLeft = false;

        if(m_pInstance) 
            m_pInstance->SetData(TYPE_RUNIC_COLOSSUS, NOT_STARTED);
    }

    void JustDied(Unit *killer)
    {
        if(m_pInstance) 
            m_pInstance->SetData(TYPE_RUNIC_COLOSSUS, DONE);
    }

    void AttackedBy(Unit* pWho)
    {
        if (m_bIsSmash && pWho->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->SetInCombatWithZone();
            m_creature->InterruptNonMeleeSpells(false);
            m_bIsSmash = false;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiSmashTimer < uiDiff && m_bIsSmash)
        {
            switch(urand(0,1))
            {
                case 0:
                    DoCast(m_creature, SPELL_RUNIC_SMASH_L);
                    m_bIsLeft = true;
                    break;
                case 1:
                    DoCast(m_creature, SPELL_RUNIC_SMASH_R);
                    m_bIsLeft = false;
                    break;
            }
            m_uiSmashTimer = 8000;
            m_uiTriggerSmashTimer = 5000;

        }
        else
            m_uiSmashTimer -= uiDiff;

        if (m_uiTriggerSmashTimer < uiDiff && m_bIsSmash)
        {
            if (m_pInstance)
            {
                if (Map* pMap = m_creature->GetMap())
                {
                    if (m_bIsLeft)
                    {
                        for (GUIDList::iterator itr =   m_pInstance->m_lLeftHandTriggerGuids.begin(); itr != m_pInstance->m_lLeftHandTriggerGuids.end(); ++itr)
                        {
                            if (Unit* trigger = pMap->GetUnit(*itr))
                            {
                                trigger->CastSpell(trigger, SPELL_RUNIC_SMASH_DMG, false, 0, 0, m_creature->GetObjectGuid());
                            }
                        }
                    }
                    else
                    {
                        for (GUIDList::iterator itr =   m_pInstance->m_lRightHandTriggerGuids.begin(); itr != m_pInstance->m_lRightHandTriggerGuids.end(); ++itr)
                        {
                            if (Unit* trigger = pMap->GetUnit(*itr))
                            {
                                trigger->CastSpell(trigger, SPELL_RUNIC_SMASH_DMG, false, 0, 0, m_creature->GetObjectGuid());
                            }
                        }
                    }
                }
            }
            m_uiTriggerSmashTimer = 20000;
        }
        else
            m_uiTriggerSmashTimer -= uiDiff;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpellTimer < uiDiff)
        {
            switch(urand(0, 1))
            {
                case 0:
                    DoCast(m_creature, SPELL_SMASH);
                    break;
                case 1:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode ? SPELL_CHARGE : SPELL_CHARGE_H);
                    break;
            }
            m_uiSpellTimer = urand(5000, 10000);
        }
        else
            m_uiSpellTimer -= uiDiff;

        if (m_uiRunicBarrierTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RUNIC_BARRIER) == CAST_OK)
                m_uiRunicBarrierTimer = urand(25000, 30000);
        }
        else
            m_uiRunicBarrierTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_runic_colossus(Creature* pCreature)
{
    return new boss_runic_colossusAI(pCreature);
}

struct MANGOS_DLL_DECL boss_ancient_rune_giantAI : public ScriptedAI
{
    boss_ancient_rune_giantAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    uint32 m_uiSpellTimer;
    uint32 m_uiSummonTimer;
    bool m_bIsSummoning;

    void Reset()
    {
        m_uiSpellTimer = urand(5000, 10000);
        m_uiSummonTimer = 5000;
        m_bIsSummoning = true;

        if(m_pInstance) 
            m_pInstance->SetData(TYPE_RUNE_GIANT, NOT_STARTED);
    }

    void JustDied(Unit *killer)
    {
        if(m_pInstance) 
            m_pInstance->SetData(TYPE_RUNE_GIANT, DONE);
    }

    void AttackedBy(Unit* pWho)
    {
        if (m_bIsSummoning && pWho->GetTypeId() == TYPEID_PLAYER)
        {
            m_creature->SetInCombatWith(pWho);
            DoCast(m_creature, SPELL_RUNIC_FORTIFICATION, true);
            m_bIsSummoning = false;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if ((m_pInstance->GetData(TYPE_RUNIC_COLOSSUS) == DONE) && m_bIsSummoning)
        {
            // summon adds before aggro and after the runic colossus has died
            if(m_uiSummonTimer < uiDiff)
            {
                // honor guard
                if(Creature* pTemp = m_creature->SummonCreature(MOB_IRON_HONOR_GUARD, OrbLoc[0].x + 30, OrbLoc[0].y, OrbLoc[0].z, 0, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, 60000))
                {
                    pTemp->GetMotionMaster()->MovePoint(0, OrbLoc[1].x, OrbLoc[1].y, OrbLoc[1].z);
                    pTemp->SetInCombatWithZone();
                }
                m_uiSummonTimer = 7000;
            }
            else m_uiSummonTimer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpellTimer < uiDiff)
        {
            switch(urand(0, 1))
            {
            case 0:
                DoCast(m_creature->getVictim(), m_bIsRegularMode? SPELL_STOMP : SPELL_STOMP_H);
                break;
            case 1:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(pTarget, SPELL_RUNE_DETONATION);
                break;
            }
            m_uiSpellTimer = urand(5000,10000);
        }else m_uiSpellTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ancient_rune_giant(Creature* pCreature)
{
    return new boss_ancient_rune_giantAI(pCreature);
}

struct MANGOS_DLL_DECL mob_thorim_preaddsAI : public ScriptedAI
{
    mob_thorim_preaddsAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool m_bIsRegularMode;
    instance_ulduar* m_pInstance;

    // jormungar
    uint32 m_uiAcidBreathTimer;
    uint32 m_uiSweepTimer;

    // captain
    uint32 m_uiDevastateTimer;
    uint32 m_uiHeroicStrikeTimer;

    // mercenary
    uint32 m_uiShootTimer;
    uint32 m_uiBarbedShotTimer;
    uint32 m_uiWingClipTimer;

    void Reset()
    {
        // jormungar
        m_uiAcidBreathTimer      = urand(7000, 14000);
        m_uiSweepTimer           = urand(15000, 20000);

        // captain
        m_uiDevastateTimer       = urand(3000, 7000);
        m_uiHeroicStrikeTimer    = urand(8000, 15000);

        // mercenary
        m_uiShootTimer           = 1000;
        m_uiBarbedShotTimer      = urand(7000, 10000);
        m_uiWingClipTimer        = urand(10000, 15000);
    }

    void AttackStart(Unit* pWho)
    {
        if (m_creature->Attack(pWho, true)) 
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            if(m_creature->GetEntry() == NPC_MERCENARY_ALY || m_creature->GetEntry() == NPC_MERCENARY_HORDE)
                DoStartMovement(pWho, 20);
            else
                DoStartMovement(pWho);
        }
    }

    void JustDied(Unit *killer)
    {
        // start the encounter
        if (Creature* pThorim = m_pInstance->GetSingleCreatureFromStorage(NPC_THORIM))
        {
            if(pThorim->isAlive())
                ((boss_thorimAI*)pThorim->AI())->m_uiPreAddsKilled++;
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(m_creature->GetEntry())
        {
            case NPC_JORMUNGAR_BEHEMOTH:
            {
                if(m_uiAcidBreathTimer < uiDiff)
                {
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_ACID_BREATH : SPELL_ACID_BREATH_H);
                    m_uiAcidBreathTimer = urand(7000, 14000);
                }
                else m_uiAcidBreathTimer -= uiDiff;

                if(m_uiSweepTimer < uiDiff)
                {
                    DoCast(m_creature, m_bIsRegularMode ? SPELL_SWEEP : SPELL_SWEEP_H);
                    m_uiSweepTimer = urand(15000, 23000);
                }
                else m_uiSweepTimer -= uiDiff;

                break;
            }
            case NPC_CAPTAIN_ALY:
            case NPC_CAPTAIN_HORDE:
            {
                if(m_uiDevastateTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                        DoCast(pTarget, SPELL_DEVASTATE);
                    m_uiDevastateTimer = urand(4000, 7000);
                }
                else m_uiDevastateTimer -= uiDiff;

                if(m_uiHeroicStrikeTimer < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_HEROIC_STRIKE);
                    m_uiHeroicStrikeTimer = urand(10000, 15000);
                }
                else m_uiHeroicStrikeTimer -= uiDiff;

                break;
            }
            case NPC_MERCENARY_ALY:
            case NPC_MERCENARY_HORDE:
            {
                if(m_uiShootTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_SHOOT);
                    m_uiShootTimer = urand(1000, 3000);
                }
                else m_uiShootTimer -= uiDiff;

                if(m_uiBarbedShotTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_BARBED_SHOT);
                    m_uiBarbedShotTimer = urand(7000, 10000);
                }
                else m_uiBarbedShotTimer -= uiDiff;

                if(m_uiWingClipTimer < uiDiff)
                {
                    DoCast(m_creature->getVictim(), SPELL_WING_CLIP);
                    m_uiWingClipTimer = urand(10000, 15000);
                }
                else m_uiWingClipTimer -= uiDiff;

                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_thorim_preadds(Creature* pCreature)
{
    return new mob_thorim_preaddsAI(pCreature);
}

// sif
struct MANGOS_DLL_DECL npc_sifAI : public ScriptedAI
{
    npc_sifAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        //pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        //pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSpellTimer;

    void Reset()
    {
        m_uiSpellTimer = urand(5000, 10000);
        m_creature->SetRespawnDelay(DAY);
    }

    void AttackStart(Unit* pWho)
    {
        if (!pWho) 
            return;

        if (m_creature->Attack(pWho, true))
        {
            m_creature->AddThreat(pWho);
            m_creature->SetInCombatWith(pWho);
            pWho->SetInCombatWith(m_creature);
            DoStartMovement(pWho, 10.0f);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_THORIM) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpellTimer < uiDiff)
        {
            switch(urand(0, 2))
            {
                case 0:
                    DoCast(m_creature, m_bIsRegularMode? SPELL_FROSTBOLT_VOLLEY : SPELL_FROSTBOLT_VOLLEY_H);
                    break;
                case 1:
                    DoCast(m_creature, m_bIsRegularMode? SPELL_FROST_NOVA : SPELL_FROST_NOVA_H);
                    break;
                case 2:
                    // it should be casted around the room!
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, m_bIsRegularMode? SPELL_BLIZZARD : SPELL_BLIZZARD_H);
                    break;
            }

            m_uiSpellTimer = urand(3000, 6000);
        }else m_uiSpellTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_sif(Creature* pCreature)
{
    return new npc_sifAI(pCreature);
}

// script for the orb on the hallway which should wipe the raid. Needs more research!
struct MANGOS_DLL_DECL npc_lightning_orbAI : public ScriptedAI
{
    npc_lightning_orbAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        pCreature->setFaction(14);
        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiMoveTimer;
    uint8 m_uiWaypoint;

    void Reset()
    {
        m_uiMoveTimer = 1000;
        m_uiWaypoint = 0;
        m_creature->SetRespawnDelay(DAY);
        // find the correct aura for raid wipe!!!
    }

    void AttackStart(Unit* pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_THORIM) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_uiMoveTimer < uiDiff && m_uiWaypoint < 4)
        {
            m_creature->GetMotionMaster()->MovePoint(0, OrbLoc[m_uiWaypoint].x, OrbLoc[m_uiWaypoint].y, OrbLoc[m_uiWaypoint].z);
            m_uiWaypoint +=1;
            m_uiMoveTimer = 10000;
        }
        else m_uiMoveTimer -= uiDiff;
    }
};

CreatureAI* GetAI_npc_lightning_orb(Creature* pCreature)
{
    return new npc_lightning_orbAI(pCreature);
}


void AddSC_boss_thorim()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_thorim";
    newscript->GetAI = &GetAI_boss_thorim;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_runic_colossus";
    newscript->GetAI = &GetAI_boss_runic_colossus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_ancient_rune_giant";
    newscript->GetAI = &GetAI_boss_ancient_rune_giant;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_sif";
    newscript->GetAI = &GetAI_npc_sif;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_lightning_orb";
    newscript->GetAI = &GetAI_npc_lightning_orb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rune_acolyte";
    newscript->GetAI = &GetAI_mob_dark_rune_acolyte;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rune_champion";
    newscript->GetAI = &GetAI_mob_dark_rune_champion;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rune_commoner";
    newscript->GetAI = &GetAI_mob_dark_rune_commoner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rune_evoker";
    newscript->GetAI = &GetAI_mob_dark_rune_evoker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rune_warbringer";
    newscript->GetAI = &GetAI_mob_dark_rune_warbringer;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rune_ring_guard";
    newscript->GetAI = &GetAI_mob_dark_rune_ring_guard;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rune_honor_guard";
    newscript->GetAI = &GetAI_mob_dark_rune_honor_guard;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_thorim_preadds";
    newscript->GetAI = &GetAI_mob_thorim_preadds;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_thorim_trap_bunny";
    newscript->GetAI = &GetAI_mob_thorim_trap_bunny;
    newscript->RegisterSelf();


}