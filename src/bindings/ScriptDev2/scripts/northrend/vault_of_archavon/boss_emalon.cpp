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
SDName: Boss_Emalon_The_Storm_Watcher
SD%Complete: 0%
SDComment:
SDCategory: Vault of Archavon
EndScriptData */

#include "precompiled.h"
#include "vault_of_archavon.h"

enum
{
    // Emalon spells
    SPELL_CHAIN_LIGHTNING_N                 = 64213,
    SPELL_CHAIN_LIGHTNING_H                 = 64215,
    SPELL_LIGHTNING_NOVA_N                  = 64216,
    SPELL_LIGHTNING_NOVA_H                  = 65279,
    SPELL_OVERCHARGE                        = 64379,        //This spell is used by Time Warder, and temporary by Emalon, because 64218 is bugged
    SPELL_BERSERK                           = 26662,

    // Tempest Minion spells
    SPELL_SHOCK                             = 64363,
    SPELL_OVERCHARGED_BLAST                 = 64219,
    SPELL_OVERCHARGED                       = 64217
};

/*######
## npc_tempest_minion
######*/

struct MANGOS_DLL_DECL npc_tempest_minionAI : public ScriptedAI
{
    npc_tempest_minionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_fDefaultX = m_creature->GetPositionX();
        m_fDefaultY = m_creature->GetPositionY();
        m_fDefaultZ = m_creature->GetPositionZ();
        m_fDefaultO = m_creature->GetOrientation();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_uiEvadeCheckCooldown;

    uint32 m_uiShockTimer;
    uint32 m_uiRespawnTimer;
    uint32 m_uiOverchargedStacksCheckTimer;
    bool m_bDead;
    bool m_bTimeToDie;
    float m_fDefaultX;
    float m_fDefaultY;
    float m_fDefaultZ;
    float m_fDefaultO;

    void Init()
    {
        m_uiEvadeCheckCooldown = 2000;
        m_uiShockTimer = 8000+rand()%4000;
        m_bDead = false;
        m_bTimeToDie = false;
        m_uiRespawnTimer = 4000;
        m_uiOverchargedStacksCheckTimer = 2000;

        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetStandState(UNIT_STAND_STATE_STAND);
        m_creature->SetVisibility(VISIBILITY_ON);
    }

    void Reset()
    {
        Init();
    }

    void Aggro(Unit* pWho)
    {
        m_creature->CallForHelp(80.0f);
    }

    void FakeDeath()
    {
        m_bDead = true;
        m_bTimeToDie = false;
        m_uiRespawnTimer = 4000;
        m_creature->InterruptNonMeleeSpells(false);
        m_creature->SetHealth(0);
        m_creature->StopMoving();
        m_creature->ClearComboPointHolders();
        m_creature->RemoveAllAurasOnDeath();
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
        m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->ClearAllReactives();
        m_creature->SetUInt64Value(UNIT_FIELD_TARGET,0);
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MoveIdle();
        m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
        m_creature->GetMap()->CreatureRelocation(m_creature, m_fDefaultX, m_fDefaultY, m_fDefaultZ, m_fDefaultO);
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (uiDamage < m_creature->GetHealth())
            return;

        if (m_pInstance && (m_pInstance->GetData(TYPE_EMALON) != DONE))
        {
            uiDamage = 0;
            FakeDeath();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            Creature* pEmalon = m_pInstance->GetSingleCreatureFromStorage(NPC_EMALON);
            if ((pEmalon && pEmalon->IsInEvadeMode()) || (m_creature->GetDistance2d(-219.119f, -289.037f) > 80.0f))
            {
                EnterEvadeMode();
                return;
            }
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if (m_bTimeToDie)
        {
            FakeDeath();
            return;
        }

        if (m_bDead)
        {
            if (m_uiRespawnTimer < uiDiff)
            {
                m_creature->SetHealth(m_creature->GetMaxHealth());
                m_creature->SetVisibility(VISIBILITY_OFF);
                Init();
                m_creature->MonsterTextEmote("%s appears to defend Emalon!", NULL, true);
                m_creature->SetInCombatWithZone();
                DoResetThreat();
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->GetMotionMaster()->MoveChase(pTarget);
            }
            else
                m_uiRespawnTimer -= uiDiff;

            return;
        }

        if (m_uiOverchargedStacksCheckTimer < uiDiff)
        {
            m_uiOverchargedStacksCheckTimer = 2000;
            Aura* pAuraOvercharged = m_creature->GetAura(SPELL_OVERCHARGED, EFFECT_INDEX_0);
            if(pAuraOvercharged && pAuraOvercharged->GetStackAmount() >= 10)
            {
                DoCastSpellIfCan(m_creature, SPELL_OVERCHARGED_BLAST);
                m_bTimeToDie = true;
                return;
            }
        }
        else
            m_uiOverchargedStacksCheckTimer -= uiDiff;

        if (m_uiShockTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOCK);
            m_uiShockTimer = 8000+rand()%4000;
        }
        else
            m_uiShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## boss_emalon
######*/

struct MANGOS_DLL_DECL boss_emalonAI : public ScriptedAI
{
    boss_emalonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 m_uiEvadeCheckCooldown;

    Creature* m_auiTempestMinion[4];
    uint32 m_uiChainLightningTimer;
    uint32 m_uiChainLightningCount;
    uint32 m_uiLightningNovaTimer;
    uint32 m_uiOverchargeTimer;
    uint32 m_uiEnrageTimer;

    void Reset()
    {
        m_uiEvadeCheckCooldown = 2000;
        m_uiChainLightningTimer = 15000;
        m_uiChainLightningCount = 0;
        m_uiLightningNovaTimer = 20000;
        m_uiOverchargeTimer = 45000;
        m_uiEnrageTimer = 360000;

        if (m_pInstance)
        {
            m_auiTempestMinion[0] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_1);
            m_auiTempestMinion[1] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_2);
            m_auiTempestMinion[2] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_3);
            m_auiTempestMinion[3] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_4);
        }

        for (uint8 i=0; i<4; ++i)
        {
            Creature* pMinion = m_auiTempestMinion[i];
            if (pMinion && pMinion->isDead())
                pMinion->Respawn();
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
        {
            m_auiTempestMinion[0] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_1);
            m_auiTempestMinion[1] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_2);
            m_auiTempestMinion[2] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_3);
            m_auiTempestMinion[3] = m_pInstance->GetSingleCreatureFromStorage(DATA_TEMPEST_MINION_4);
        }

        m_creature->CallForHelp(80.0f);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_EMALON, DONE);
        for (uint8 i=0; i<4; ++i)
        {
            Creature* pMinion = m_auiTempestMinion[i];
            if (pMinion)
                pMinion->DealDamage(pMinion, pMinion->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEvadeCheckCooldown < uiDiff)
        {
            if (m_creature->GetDistance2d(-219.119f, -289.037f) > 80.0f)
                EnterEvadeMode();
            m_creature->CallForHelp(80.0f);
            m_uiEvadeCheckCooldown = 2000;
        }
        else
            m_uiEvadeCheckCooldown -= uiDiff;

        if (m_uiOverchargeTimer < uiDiff)
        {
            Creature* pMinion = m_auiTempestMinion[rand()%4];
            if(pMinion && pMinion->isAlive())
            {
                m_creature->MonsterTextEmote("%s overcharges Tempest Minion!",NULL, true);
                pMinion->SetHealth(pMinion->GetMaxHealth());
                pMinion->CastSpell(pMinion, SPELL_OVERCHARGE, false);
            }
            m_uiOverchargeTimer = 45000;
        }
        else
            m_uiOverchargeTimer -= uiDiff;

        if (m_uiChainLightningTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING_N : SPELL_CHAIN_LIGHTNING_H);
            m_uiChainLightningTimer = 10000 + rand()%15000;
        }
        else
            m_uiChainLightningTimer -= uiDiff;

        if (m_uiLightningNovaTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_LIGHTNING_NOVA_N : SPELL_LIGHTNING_NOVA_H);
            m_uiLightningNovaTimer = 45000;
        }
        else
            m_uiLightningNovaTimer -= uiDiff;

        if (m_uiEnrageTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BERSERK);
            m_uiEnrageTimer = 30000;
        }
        else
            m_uiEnrageTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

/*######
## npc_tempest_warder
######*/

struct MANGOS_DLL_DECL npc_tempest_warderAI : public ScriptedAI
{
    npc_tempest_warderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiShockTimer;
    bool m_bOvercharged;
    uint32 m_uiOverchargedStacksCheckTimer;
    bool m_bTimeToDie;

    void Reset()
    {
        m_uiShockTimer = 8000+rand()%4000;
        m_bOvercharged = false;
        uint32 m_uiOverchargedStacksCheckTimer = 2000;
        m_bTimeToDie = false;
    }

    void Aggro(Unit* pWho) {}

    void UpdateAI(const uint32 uiDiff)
    {  
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_bTimeToDie)
        {
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            return;
        }

        if (!m_bOvercharged && ((m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 37))
        {
            DoCastSpellIfCan(m_creature, SPELL_OVERCHARGE);
            m_bOvercharged = true;
        }

        if (m_bOvercharged)
        {
            if (m_uiOverchargedStacksCheckTimer < uiDiff)
            {
                m_uiOverchargedStacksCheckTimer = 2000;
                Aura* pAuraOvercharged = m_creature->GetAura(SPELL_OVERCHARGED, EFFECT_INDEX_0);
                if(pAuraOvercharged && pAuraOvercharged->GetStackAmount() >= 10)
                {
                    DoCastSpellIfCan(m_creature, SPELL_OVERCHARGED_BLAST);
                    m_bTimeToDie = true;
                    return;
                }
            }
            else
                m_uiOverchargedStacksCheckTimer -= uiDiff;
        }

        if (m_uiShockTimer < uiDiff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHOCK);
            m_uiShockTimer = 8000+rand()%4000;
        }
        else
            m_uiShockTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_emalonAI(Creature* pCreature)
{
    return new boss_emalonAI(pCreature);
}

CreatureAI* GetAI_npc_tempest_minionAI(Creature* pCreature)
{
    return new npc_tempest_minionAI(pCreature);
}

CreatureAI* GetAI_npc_tempest_warderAI(Creature* pCreature)
{
    return new npc_tempest_warderAI(pCreature);
}

void AddSC_boss_emalon()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_emalon";
    newscript->GetAI = &GetAI_boss_emalonAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tempest_minion";
    newscript->GetAI = &GetAI_npc_tempest_minionAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_tempest_warder";
    newscript->GetAI = &GetAI_npc_tempest_warderAI;
    newscript->RegisterSelf();
}
