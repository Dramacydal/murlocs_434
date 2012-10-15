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
SDName: boss_auriaya
SD%Complete:
SDComment: need correct setstack for feral defender buff
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //yells
    SAY_AGGRO       = -1603110,
    SAY_SLAY_1      = -1603112,
    SAY_SLAY_2      = -1603111,
    SAY_BERSERK     = -1603113,
    SAY_DEATH       = -1603114,
    EMOTE_SCREECH   = -1603115,
    EMOTE_DEFENDER  = -1603116,

    //sanctum sentry
    SPELL_RIP_FLESH                     = 64375,
    SPELL_RIP_FLESH_H                   = 64667,
    SPELL_SAVAGE_POUNCE                 = 64666,
    SPELL_SAVAGE_POUNCE_H               = 64374,
    SPELL_STRENGHT_OF_PACK              = 64369,
    SPELL_STRENGHT_OF_PACK_BUFF         = 64381,

    // Auriaya
    SPELL_BERSERK                       = 47008,
    SPELL_GUARDIAN_SWARM                = 64396,            // triggers 64397
    SPELL_SENTINEL_BLAST                = 64389,            // triggers 64392
    SPELL_SENTINEL_BLAST_H              = 64678,            // triggers 64679
    SPELL_SONIC_SCREECH                 = 64422,
    SPELL_SONIC_SCREECH_H               = 64688,
    SPELL_TERRIFYING_SCREECH            = 64386,
    SPELL_ACTIVATE_FERAL_DEFENDER       = 64449,            // triggers 64447
    SPELL_ACTIVATE_FERAL_DEFENDER_TRIGG = 64448,

    // Feral Defender spells
    SPELL_FERAL_ESSENCE                 = 64455,
    SPELL_FERAL_ESSENCE_REMOVAL         = 64456,            // remove 1 stack of 64455
    SPELL_FERAL_POUNCE                  = 64478,
    SPELL_FERAL_POUNCE_H                = 64669,
    //SPELL_FERAL_RUSH                    = 64489,            // should trigger repeatedly on random targets 64496 - 64674
    SPELL_SEEPING_FERAL_ESSENCE_SUMMON  = 64457,
    SPELL_FEIGN_DEATH                   = 64461,            // dummy - possible related to the feral defender feign death - not used
    SPELL_FULL_HEAL                     = 64460,            // on feign death remove
    SPELL_FEIGN_DEATH_2                 = 57685,
    SPELL_FERAL_RUSH                    = 64496,
    SPELL_FERAL_RUSH_H                  = 64674,

    // Seeping Feral Essence
    SPELL_SEEPING_FERAL_ESSENCE         = 64458,
    SPELL_SEEPING_FERAL_ESSENCE_H       = 64676,

    //NPC_SEEPING_FERAL_ESSENCE           = 34098,          // summoned by the feral defender on feign death
    //NPC_GUARDIAN_SWARN                  = 34034,          // summoned by spell
    NPC_FERAL_DEFENDER_STALKER          = 34096,
    //NPC_FERAL_DEFENDER          = 34035,
};

struct MANGOS_DLL_DECL boss_auriayaAI : public ScriptedAI
{
    boss_auriayaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiEnrageTimer;
    uint32 m_uiSwarmTimer;
    uint32 m_uiSonicScreechTimer;
    uint32 m_uiSentinelBlastTimer;
    uint32 m_uiTerrifyingScreechTimer;
    uint32 m_uiDefenderTimer;

    void Reset()
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_uiEnrageTimer             = 10*MINUTE*IN_MILLISECONDS;
        m_uiSwarmTimer              = 50000;
        m_uiSonicScreechTimer       = 58000;
        m_uiSentinelBlastTimer      = 40000;
        m_uiTerrifyingScreechTimer  = 38000;
        m_uiDefenderTimer           = 1*MINUTE*IN_MILLISECONDS;

        for (GUIDList::iterator itr = m_pInstance->m_lSanctumSentryGuids.begin(); itr != m_pInstance->m_lSanctumSentryGuids.end(); ++itr)
        {
            if (Creature* pSanity = m_creature->GetMap()->GetCreature(*itr))
            {
                if (!pSanity->isAlive())
                {
                    pSanity->Respawn();
                    pSanity->NearTeleportTo(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0.0f);
                }
            }
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, DONE);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, IN_PROGRESS);

        for (GUIDList::iterator itr = m_pInstance->m_lSanctumSentryGuids.begin(); itr != m_pInstance->m_lSanctumSentryGuids.end(); ++itr)
        {
            if (Creature* pSanity = m_creature->GetMap()->GetCreature(*itr))
            {
                if (pSanity->isAlive())
                    pSanity->SetInCombatWithZone();
            }
        }

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AURIAYA, FAIL);
    }

    void JustSummoned(Creature *pSummoned)
    {
        // Summon the feral defender
        if (pSummoned->GetEntry() == NPC_FERAL_DEFENDER_STALKER)
            DoCastSpellIfCan(pSummoned, SPELL_ACTIVATE_FERAL_DEFENDER, CAST_INTERRUPT_PREVIOUS);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiTerrifyingScreechTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_TERRIFYING_SCREECH) == CAST_OK)
            {
                DoScriptText(EMOTE_SCREECH, m_creature);
                m_uiTerrifyingScreechTimer = 35000;
                m_uiSentinelBlastTimer = 2000;
            }
        }
        else
            m_uiTerrifyingScreechTimer -= uiDiff;

        if (m_uiSentinelBlastTimer < uiDiff)
        {
            // Cast Sentinel blast right after terrifying screech
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SENTINEL_BLAST : SPELL_SENTINEL_BLAST_H) == CAST_OK)
                m_uiSentinelBlastTimer = 35000;
        }
        else
            m_uiSentinelBlastTimer -= uiDiff;

        if (m_uiDefenderTimer)
        {
            if (m_uiDefenderTimer <= uiDiff)
            {
                // Summon the feral defender trigger
                if (DoCastSpellIfCan(m_creature, SPELL_ACTIVATE_FERAL_DEFENDER_TRIGG) == CAST_OK)
                {
                    DoScriptText(EMOTE_DEFENDER, m_creature);
                    m_uiDefenderTimer = 0;
                }
            }
            else
                m_uiDefenderTimer -= uiDiff;
        }

        if (m_uiSonicScreechTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SONIC_SCREECH : SPELL_SONIC_SCREECH_H) == CAST_OK)
                m_uiSonicScreechTimer = 27000;
        }
        else
            m_uiSonicScreechTimer -= uiDiff;

        if (m_uiSwarmTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER | SELECT_FLAG_NOT_IN_MELEE_RANGE))
            {
                if (DoCastSpellIfCan(m_creature, SPELL_GUARDIAN_SWARM) == CAST_OK)
                    m_uiSwarmTimer = 37000;
            }
        }
        else
            m_uiSwarmTimer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BERSERK) == CAST_OK)
            {
                DoScriptText(SAY_BERSERK, m_creature);
                m_uiEnrageTimer = 10*MINUTE*IN_MILLISECONDS;
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_auriaya(Creature* pCreature)
{
    return new boss_auriayaAI(pCreature);
}

struct MANGOS_DLL_DECL boss_feral_defenderAI : public ScriptedAI
{
    boss_feral_defenderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiPounceTimer;
    uint32 m_uiFeralRushTimer;
    uint32 m_uiReviveDelayTimer;
    uint32 m_uiKilledCount;

    void Reset()
    {
        m_uiReviveDelayTimer    = 0;
        m_uiPounceTimer         = 5000;
        m_uiFeralRushTimer      = 10000;
        m_uiKilledCount         = 0;

        DoCastSpellIfCan(m_creature, SPELL_FERAL_ESSENCE);
    }

    void JustDied(Unit* pKiller)
    {
        // Set achiev criteria to true
        if (m_pInstance)
            m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_NINE_LIVES, true);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        // If we don't have the feral essence anymore then ignore this
        if (m_uiKilledCount >= 8)                           // 9-1 == 8
            return;

        if (m_uiReviveDelayTimer)                           // Already faking
        {
            uiDamage = 0;
            return;
        }

        if (uiDamage > m_creature->GetHealth())
        {
            if (m_creature->HasAura(SPELL_FERAL_ESSENCE))
            {
                uiDamage = 0;

                m_creature->InterruptNonMeleeSpells(true);
                m_creature->SetHealth(0);
                m_creature->StopMoving();
                m_creature->ClearComboPointHolders();
                //m_creature->RemoveAllAurasOnDeath(); // TODO Should only remove negative auras
                m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
                m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->ClearAllReactives();
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->SetStandState(UNIT_STAND_STATE_DEAD);

                // Remove one aura stack and summon a feral essence
                DoCastSpellIfCan(m_creature, SPELL_FERAL_ESSENCE_REMOVAL, CAST_TRIGGERED);
                DoCastSpellIfCan(m_creature, SPELL_SEEPING_FERAL_ESSENCE_SUMMON, CAST_TRIGGERED);

                m_uiReviveDelayTimer = 30000;
                ++m_uiKilledCount;
            }
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        // Cast seeping feral essence on the summoned
        pSummoned->CastSpell(pSummoned, m_bIsRegularMode ? SPELL_SEEPING_FERAL_ESSENCE : SPELL_SEEPING_FERAL_ESSENCE_H, true, NULL, NULL, m_creature->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiReviveDelayTimer)
        {
            if (m_uiReviveDelayTimer <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FULL_HEAL) == CAST_OK)
                {
                    m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    m_creature->SetStandState(UNIT_STAND_STATE_STAND);

                    DoResetThreat();
                    m_uiReviveDelayTimer = 0;

                    // Assume Attack - it should jump to victim
                    if (m_creature->getVictim())
                        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                }
            }
            else
                m_uiReviveDelayTimer -= uiDiff;

            // No Further action while faking
            return;
        }

        if (m_uiPounceTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FERAL_POUNCE : SPELL_FERAL_POUNCE_H) == CAST_OK)
                    m_uiPounceTimer = 5000;
            }
        }
        else
            m_uiPounceTimer -= uiDiff;

        if (m_uiFeralRushTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
            {
                if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_FERAL_RUSH : SPELL_FERAL_RUSH_H) == CAST_OK)
                    m_uiFeralRushTimer = 12000;
            }
        }
        else
            m_uiFeralRushTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_feral_defender(Creature* pCreature)
{
    return new boss_feral_defenderAI(pCreature);
}

// Sanctum Sentry
struct MANGOS_DLL_DECL boss_sanctum_sentryAI : public ScriptedAI
{
    boss_sanctum_sentryAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    bool m_bIsFollowing;

    uint32 m_uiRip_Flesh_Timer;
    uint32 m_uiJump_Timer;

    void Reset()
    {
        m_uiRip_Flesh_Timer = 13000;
        m_uiJump_Timer = 0;
        m_bIsFollowing = false;
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            if (Creature* pAuriaya = m_pInstance->GetSingleCreatureFromStorage(NPC_AURIAYA))
            {
                if (pAuriaya->isAlive())
                    pAuriaya->SetInCombatWithZone();
            }

            for (GUIDList::iterator itr = m_pInstance->m_lSanctumSentryGuids.begin(); itr != m_pInstance->m_lSanctumSentryGuids.end(); ++itr)
            {
                if (Creature* pSanity = m_creature->GetMap()->GetCreature(*itr))
                {
                    if (pSanity->isAlive())
                        pSanity->SetInCombatWithZone();
                }
            }
        }
        DoCast(m_creature, SPELL_STRENGHT_OF_PACK);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() != FOLLOW_MOTION_TYPE)
                if (m_pInstance)
                    if (Creature *pAuriaya = m_pInstance->GetSingleCreatureFromStorage(NPC_AURIAYA))
                        m_creature->GetMotionMaster()->MoveFollow(pAuriaya, 5.0f, urand(60, 250)/100.0f);
            return;
        }

        if (m_uiRip_Flesh_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_RIP_FLESH : SPELL_RIP_FLESH_H) == CAST_OK)
                m_uiRip_Flesh_Timer = 13000;
        }
        else
            m_uiRip_Flesh_Timer -= diff;

        if (m_uiJump_Timer < diff)
        {
            if (!m_creature->IsWithinDistInMap(m_creature->getVictim(), 8) && m_creature->IsWithinDistInMap(m_creature->getVictim(), 25))
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SAVAGE_POUNCE : SPELL_SAVAGE_POUNCE_H);
            m_uiJump_Timer = 1000;
        }
        else
            m_uiJump_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sanctum_sentry(Creature* pCreature)
{
    return new boss_sanctum_sentryAI(pCreature);
}

void AddSC_boss_auriaya()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_auriaya";
    pNewScript->GetAI = GetAI_boss_auriaya;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_sanctum_sentry";
    pNewScript->GetAI = &GetAI_boss_sanctum_sentry;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_feral_defender";
    pNewScript->GetAI = &GetAI_boss_feral_defender;
    pNewScript->RegisterSelf();
}
