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
// Jaraxxus - Magic aura (from start?) not fully offlike implemented.
// Legion flame visual effect not imlemented

/* ScriptData
SDName: trial_of_the_crusader
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum Yells
{
    SAY_AGGRO                          = -1713517,
    SAY_DEATH                          = -1713525,
    SAY_SLAY_1                         = -1713567,
    SAY_SLAY_2                         = -1713568,
    SAY_INFERNAL_VOLCANO               = -1713520,
    SAY_NETHER_PORTAL                  = -1713519,
    SAY_SUMMON_MISTRESS                = -1713521,
    SAY_SUMMON_INFERNAL                = -1713524,
    SAY_INCINERATE_FLESH               = -1713523,
    SAY_ENRAGE                         = -1713750,

    EMOTE_INCINERATE_FLESH             = -1713522,
};

enum Equipment
{
    EQUIP_MAIN                         = 47266,
    EQUIP_DONE                         = EQUIP_NO_CHANGE,
};

enum Summons
{
    NPC_LEGION_FLAME                   = 34784,
    NPC_INFERNAL_VOLCANO               = 34813,
    NPC_FEL_INFERNAL                   = 34815,
    NPC_NETHER_PORTAL                  = 34825,
    NPC_MISTRESS                       = 34826,
};

enum BossSpells
{
    SPELL_NETHER_POWER                 = 67108,
    SPELL_SUMMON_VOLCAN_N              = 66258,
    SPELL_SUMMON_VOLCAN_H              = 67902,
    SPELL_SUMMON_INFERNAL_PERIODIC     = 66252,
    SPELL_FEL_FIREBALL                 = 66532,
    SPELL_FEL_LIGHTING                 = 66528,
    SPELL_INCINERATE_FLESH             = 66237,
    SPELL_BURNING_INFERNO              = 66242,
    SPELL_SUMMON_NETHER_PORTAL_N       = 66269,
    SPELL_SUMMON_NETHER_PORTAL_H       = 67899,
    SPELL_SUMMON_MISTRESS_PERIODIC     = 67103,
    SPELL_LEGION_FLAME                 = 66197,
    SPELL_LEGION_FLAME_AURA            = 66201,
    SPELL_SHIVAN_SLASH                 = 67098,
    SPELL_SPINNING_STRIKE              = 66283, // Need core support
    SPELL_MISTRESS_KISS                = 67906,
    SPELL_FEL_INFERNO                  = 67047,
    SPELL_FEL_STREAK                   = 66494,
    SPELL_WILFRED_PORTAL               = 68424,
    SPELL_ENRAGE                       = 26662,
};

/*######
## boss_jaraxxus
######*/

struct MANGOS_DLL_DECL boss_jaraxxusAI : public ScriptedAI
{
    boss_jaraxxusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_trial_of_the_crusader*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    instance_trial_of_the_crusader* m_pInstance;
    Difficulty m_uiMapDifficulty;
    bool m_bIsHeroic;
    bool m_bIs25Man;

    uint32 m_uiNetherPowerTimer;
    uint32 m_uiFelFireballTimer;
    uint32 m_uiFelLightingTimer;
    uint32 m_uiIncinerateFleshTimer;
    uint32 m_uiLegionFlameTimer;
    uint32 m_uiInfernalEruptionTimer;
    uint32 m_uiNetherPortalTimer;
    uint32 m_uiEnrageTimer;

    std::list<Creature*> mistressEntryList;

    void Reset() 
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_JARAXXUS, NOT_STARTED);

        m_uiNetherPowerTimer        = 0;
        m_uiFelFireballTimer        = 15000;
        m_uiFelLightingTimer        = 12000;
        m_uiIncinerateFleshTimer    = 20000;
        m_uiLegionFlameTimer        = 30000;
        m_uiInfernalEruptionTimer   = 80000;
        m_uiNetherPortalTimer       = 20000;
        m_uiEnrageTimer             = 600000;

        m_creature->SetRespawnDelay(DAY);

        mistressEntryList.clear();
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_JARAXXUS, FAIL);
        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) 
            return;

        // Find required NPC as achievement criteria
        mistressEntryList.clear();
        GetCreatureListWithEntryInGrid(mistressEntryList, m_creature, NPC_MISTRESS, 250.0f);

        if (mistressEntryList.empty())
            m_pInstance->SetSpecialAchievementCriteria(TYPE_THREE_SIXTY_PAIN_SPIKE, false);
        else
            m_pInstance->SetSpecialAchievementCriteria(TYPE_THREE_SIXTY_PAIN_SPIKE, mistressEntryList.size() >= 2);

        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_JARAXXUS, DONE);
        m_pInstance->SetData(TYPE_EVENT,1999);
        m_pInstance->SetData(TYPE_STAGE,0);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) 
            return;

        DoScriptText(SAY_AGGRO, m_creature);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetInCombatWithZone();
        m_pInstance->SetData(TYPE_JARAXXUS, IN_PROGRESS);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_SLAY_1 - urand(0, 1),m_creature,pVictim);
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiEnrageTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
            {
                m_uiEnrageTimer = 600000;
                DoScriptText(SAY_ENRAGE, m_creature);
            }
        }
        else
            m_uiEnrageTimer -= uiDiff;

        if (m_uiNetherPowerTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_NETHER_POWER) == CAST_OK)
                m_uiNetherPowerTimer = 30000;
        }
        else
            m_uiNetherPowerTimer -= uiDiff;

        if (m_uiFelFireballTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                if (DoCastSpellIfCan(pTarget, SPELL_FEL_FIREBALL) == CAST_OK)
                    m_uiFelFireballTimer = 12000;
        }
        else
            m_uiFelFireballTimer -= uiDiff;

        if (m_uiFelLightingTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                if (DoCastSpellIfCan(pTarget, SPELL_FEL_LIGHTING) == CAST_OK)
                    m_uiFelLightingTimer = 25000;
        }
        else
            m_uiFelLightingTimer -= uiDiff;

        if (m_uiIncinerateFleshTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_INCINERATE_FLESH) == CAST_OK)
                {
                    DoScriptText(SAY_INCINERATE_FLESH, m_creature);
                    DoScriptText(EMOTE_INCINERATE_FLESH, m_creature, pTarget);
                    m_uiIncinerateFleshTimer = 22000;
                }
            }
        }
        else
            m_uiIncinerateFleshTimer -= uiDiff;

        if (m_uiLegionFlameTimer <= uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                m_creature->CastSpell(pTarget, SPELL_LEGION_FLAME, false);
            
            m_uiLegionFlameTimer = 30000;
        }
        else
            m_uiLegionFlameTimer -= uiDiff;

        if (m_uiInfernalEruptionTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsHeroic ? SPELL_SUMMON_VOLCAN_H : SPELL_SUMMON_VOLCAN_N) == CAST_OK)
            {
                m_uiInfernalEruptionTimer = 120000;
                DoScriptText(SAY_INFERNAL_VOLCANO, m_creature);
                DoScriptText(SAY_SUMMON_INFERNAL, m_creature);
            }
        }
        else
            m_uiInfernalEruptionTimer -= uiDiff;

        if (m_uiNetherPortalTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsHeroic ? SPELL_SUMMON_NETHER_PORTAL_H : SPELL_SUMMON_NETHER_PORTAL_N) == CAST_OK)
            {
                DoScriptText(SAY_NETHER_PORTAL, m_creature);
                m_uiNetherPortalTimer = 120000;
            }
        }
        else
            m_uiNetherPortalTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_jaraxxus(Creature* pCreature)
{
    return new boss_jaraxxusAI(pCreature);
}

struct MANGOS_DLL_DECL mob_legion_flameAI : public ScriptedAI
{
    mob_legion_flameAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        m_creature->_AddAura(SPELL_LEGION_FLAME_AURA);
        m_creature->ForcedDespawn(60000);
        m_creature->SetRespawnDelay(DAY);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
        {
            m_creature->RemoveAllAuras();
            m_creature->ForcedDespawn();
        }
    }
};

CreatureAI* GetAI_mob_legion_flame(Creature* pCreature)
{
    return new mob_legion_flameAI(pCreature);
}

struct MANGOS_DLL_DECL mob_infernal_volcanoAI : public ScriptedAI
{
    mob_infernal_volcanoAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_uiMapDifficulty;
    bool m_bIsHeroic;
    bool m_bIs25Man;

    uint32 m_uiSummonInfernalTimer;

    void Reset()
    {
        m_creature->SetInCombatWithZone();
        SetCombatMovement(false);

        m_uiSummonInfernalTimer = 12000;

        if (!m_bIsHeroic)
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        DoCastSpellIfCan(m_creature, SPELL_SUMMON_INFERNAL_PERIODIC);
    }

    void JustDied(Unit* Killer)
    {
        m_creature->ForcedDespawn();
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_uiSummonInfernalTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_INFERNAL_PERIODIC);
            m_uiSummonInfernalTimer = 9000;
        }
        else
            m_uiSummonInfernalTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_infernal_volcano(Creature* pCreature)
{
    return new mob_infernal_volcanoAI(pCreature);
}

struct MANGOS_DLL_DECL mob_fel_infernalAI : public ScriptedAI
{
    mob_fel_infernalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiFelInfernoTimer;
    uint32 m_uiFelStreakTimer;

    void Reset()
    {
        m_uiFelInfernoTimer      = 10000,
        m_uiFelStreakTimer       = 20000,

        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) 
            return;
    }

    void JustDied(Unit* Killer)
    {
        m_creature->ForcedDespawn(5000);
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance) 
            return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiFelInfernoTimer <= uiDiff)
        {
            if (Unit* pTarget= m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->CastSpell(pTarget, SPELL_FEL_INFERNO, false);

            m_uiFelInfernoTimer = 25000;
        }
        else
            m_uiFelInfernoTimer -= uiDiff;

        if (m_uiFelStreakTimer <= uiDiff)
        {
            if (Unit* pTarget= m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                m_creature->CastSpell(pTarget, SPELL_FEL_STREAK, false);

            m_uiFelStreakTimer = 25000;
        }
        else
            m_uiFelStreakTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_fel_infernal(Creature* pCreature)
{
    return new mob_fel_infernalAI(pCreature);
}

struct MANGOS_DLL_DECL mob_nether_portalAI : public ScriptedAI
{
    mob_nether_portalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_uiMapDifficulty;
    bool m_bIsHeroic;
    bool m_bIs25Man;

    uint32 m_uiSummonMistressTimer;

    void Reset()
    {
        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);
        SetCombatMovement(false);

        m_uiSummonMistressTimer = 18000;

        if (!m_bIsHeroic)
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        DoCastSpellIfCan(m_creature, SPELL_SUMMON_MISTRESS_PERIODIC, CAST_TRIGGERED);
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (m_uiSummonMistressTimer <= uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_SUMMON_MISTRESS_PERIODIC, CAST_TRIGGERED);
            m_uiSummonMistressTimer = 9000;
        }
        else
            m_uiSummonMistressTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_nether_portal(Creature* pCreature)
{
    return new mob_nether_portalAI(pCreature);
}

struct MANGOS_DLL_DECL mob_mistress_of_painAI : public ScriptedAI
{
    mob_mistress_of_painAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Difficulty m_uiMapDifficulty;
    bool m_bIsHeroic;
    bool m_bIs25Man;

    uint32 m_uiShivanSlashTimer;
    uint32 m_uiSpinningStrikeTimer;
    uint32 m_uiMistressKissTimer;

    void Reset()
    {
        m_creature->SetInCombatWithZone();
        m_creature->SetRespawnDelay(DAY);

        m_uiShivanSlashTimer       = 10000;
        m_uiSpinningStrikeTimer    = 5000;
        m_uiMistressKissTimer      = 8000;
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER) 
            return;
    }

    void JustDied(Unit* Killer)
    {
        m_creature->ForcedDespawn();
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance) 
            return;
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) 
            return;

        if (m_pInstance->GetData(TYPE_JARAXXUS) != IN_PROGRESS) 
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiShivanSlashTimer <= uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_SHIVAN_SLASH, false);
            m_uiShivanSlashTimer = 10000;
        }
        else
            m_uiShivanSlashTimer -= uiDiff;

        if (m_uiSpinningStrikeTimer <= uiDiff)
        {
            m_creature->CastSpell(m_creature->getVictim(), SPELL_SPINNING_STRIKE, false);
            m_uiSpinningStrikeTimer = 10000;
        }
        else
            m_uiSpinningStrikeTimer -= uiDiff;

        if (m_bIsHeroic)
        {
            if (m_uiMistressKissTimer <= uiDiff)
            {
                if (Unit* pTarget= m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    m_creature->CastSpell(pTarget, SPELL_MISTRESS_KISS, false);

                m_uiMistressKissTimer = 10000;
            }
            else
                m_uiMistressKissTimer -= uiDiff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_mistress_of_pain(Creature* pCreature)
{
    return new mob_mistress_of_painAI(pCreature);
}

void AddSC_boss_jaraxxus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_jaraxxus";
    pNewScript->GetAI = &GetAI_boss_jaraxxus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_legion_flame";
    pNewScript->GetAI = &GetAI_mob_legion_flame;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_infernal_volcano";
    pNewScript->GetAI = &GetAI_mob_infernal_volcano;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_fel_infernal";
    pNewScript->GetAI = &GetAI_mob_fel_infernal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_nether_portal";
    pNewScript->GetAI = &GetAI_mob_nether_portal;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_mistress_of_pain";
    pNewScript->GetAI = &GetAI_mob_mistress_of_pain;
    pNewScript->RegisterSelf();
}