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
SDName: trial_of_the_crusader
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Crusader Coliseum
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_crusader.h"

enum BossSpells
{
    // Fjola
    SPELL_TWIN_SPIKE_L            = 66075,
    SPELL_LIGHT_SURGE             = 65766,
    SPELL_LIGHT_SHIELD            = 65858,
    SPELL_TWIN_PACT_L             = 65876,
    SPELL_LIGHT_VORTEX            = 66046,
    SPELL_LIGHT_TOUCH             = 67296,

    // Eydis
    SPELL_TWIN_SPIKE_D            = 66069,
    SPELL_DARK_SURGE              = 65768,
    SPELL_DARK_SHIELD             = 65874,
    SPELL_TWIN_PACT_D             = 65875,
    SPELL_DARK_VORTEX             = 66058,
    SPELL_DARK_TOUCH              = 67281,

    SPELL_TWIN_POWER              = 65916,

    SPELL_LIGHT_ESSENCE           = 65686,
    SPELL_DARK_ESSENCE            = 65684,

    SPELL_EMPOWERED_DARK          = 67215,
    SPELL_EMPOWERED_LIGHT         = 65748,

    SPELL_BERSERK                 = 64238,
    SPELL_REMOVE_TOUCH            = 68084,
    SPELL_NONE                    = 0,

    SPELL_UNLEASHED_DARK          = 65808,
    SPELL_UNLEASHED_LIGHT         = 65795,
};

enum Equipment
{
    EQUIP_MAIN_1                  = 49303,
    EQUIP_OFFHAND_1               = 47146,
    EQUIP_RANGED_1                = 47267,
    EQUIP_MAIN_2                  = 45990,
    EQUIP_OFFHAND_2               = 47470,
    EQUIP_RANGED_2                = 47267,
    EQUIP_DONE                    = EQUIP_NO_CHANGE,
};

enum Summons
{
    NPC_DARK_ESSENCE              = 34567,
    NPC_LIGHT_ESSENCE             = 34568,

    NPC_UNLEASHED_DARK            = 34628,
    NPC_UNLEASHED_LIGHT           = 34630,
};

enum Yells
{
    SAY_AGGRO                     = -1713541,
    SAY_LIGHT_VORTEX              = -1713542,
    SAY_SLAY_1                    = -1713543,
    SAY_SLAY_2                    = -1713544,
    SAY_PACT                      = -1713545,
    SAY_DARK_VORTEX               = -1713546,
    SAY_DEATH                     = -1713547,

    EMOTE_LIGHT_VORTEX            = -1713538,
    EMOTE_PACT                    = -1713539,
    EMOTE_DARK_VORTEX             = -1713540,
};

enum Phases
{
    PHASE_NORMAL                  = 0,
    PHASE_SPECIAL                 = 1,
};

enum Specials
{
    LIGHT_VORTEX                  = 0,
    LIGHT_PACT                    = 1,
    DARK_VORTEX                   = 2,
    DARK_PACT                     = 3,
};

#define    MAX_X                         = 598.80f;
#define    MIN_X                         = 527.54f;
#define    MAX_Y                         = 176.33f;
#define    MIN_Y                         = 104.54f;

struct MANGOS_DLL_DECL boss_fjolaAI : public ScriptedAI
{
    boss_fjolaAI(Creature* pCreature) : ScriptedAI(pCreature)
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
    bool m_bAchievFailed;
    float x, y, z;

    uint32 m_uiAchievTimer;
    uint32 m_uiPhase;
    uint32 m_uiTwinSpikeLTimer;
    uint32 m_uiSpecialAbilityTimer;
    uint32 m_uiLightTouchTimer;
    uint32 m_uiCheckTouchBuff;
    uint32 m_uiOrbsTimer;
    uint32 m_uiNextSpell;

    Unit *pSister;

    void Reset() 
    {
        if (!m_pInstance) 
            return;

        pSister = NULL;

        m_uiAchievTimer            = 180000;
        m_uiPhase                  = PHASE_NORMAL;
        m_uiNextSpell              = urand(0,1);
        m_uiTwinSpikeLTimer        = 10000;
        m_uiSpecialAbilityTimer    = 45000;
        m_uiLightTouchTimer        = 15000;
        m_uiCheckTouchBuff         = 1000;
        m_uiOrbsTimer              = 2500;

        m_bAchievFailed            = false;

        SetEquipmentSlots(false, EQUIP_MAIN_1, EQUIP_OFFHAND_1, EQUIP_RANGED_1);

        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetHealth(m_creature->GetMaxHealth());
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_VALKIRIES, FAIL);
        m_pInstance->SetData(DATA_HEALTH_FJOLA, m_creature->GetMaxHealth());
        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) 
            return;

        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_VALKIRIES, DONE);
      
        if (pSister)
        {
            if (pSister->isAlive())
                pKiller->DealDamage(pSister, pSister->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        m_pInstance->SetData(DATA_HEALTH_FJOLA, 0);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_SLAY_1 - urand(0, 1),m_creature,pVictim);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) 
            return;

        pSister = m_pInstance->GetSingleCreatureFromStorage(NPC_DARKBANE);

        m_creature->SetInCombatWithZone();
        DoCastSpellIfCan(m_creature, SPELL_LIGHT_SURGE, CAST_TRIGGERED);

        m_pInstance->SetData(DATA_HEALTH_FJOLA, m_creature->GetMaxHealth());
        m_pInstance->SetData(TYPE_VALKIRIES, IN_PROGRESS);

        m_creature->SummonCreature(NPC_LIGHT_ESSENCE, SpawnLoc[24].x, SpawnLoc[24].y, SpawnLoc[24].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 5000);
        m_creature->SummonCreature(NPC_LIGHT_ESSENCE, SpawnLoc[25].x, SpawnLoc[25].y, SpawnLoc[25].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 5000);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_pInstance) 
            return;

        if (!m_creature || !m_creature->isAlive())
            return;

        if (pDoneBy->GetObjectGuid() == m_creature->GetObjectGuid()) 
            return;

        if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
        {
            if (pDoneBy->HasAura(SPELL_LIGHT_ESSENCE))
                uiDamage /= 2;
            else if (pDoneBy->HasAura(SPELL_DARK_ESSENCE))
                uiDamage += uiDamage/2;
        }

        m_pInstance->SetData(DATA_HEALTH_FJOLA, m_creature->GetHealth() >= uiDamage ? m_creature->GetHealth() - uiDamage : 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) 
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!pSister)
            return;

        if (m_creature->GetHealth() > m_pInstance->GetData(DATA_HEALTH_EYDIS) &&
                                      m_pInstance->GetData(DATA_HEALTH_EYDIS) != 0)
                m_creature->SetHealth(m_pInstance->GetData(DATA_HEALTH_EYDIS));

        if (!m_bAchievFailed)
        {
            if (m_uiAchievTimer < uiDiff)
            {
                m_pInstance->SetSpecialAchievementCriteria(TYPE_SALT_AND_PEPPER, false);
                m_bAchievFailed = true;
            }
            else
                m_uiAchievTimer -= uiDiff;
        }

        if (m_bIsHeroic)
        {
            if (m_uiCheckTouchBuff <= uiDiff)
            {
                Map* pMap = m_creature->GetMap();
                Map::PlayerList const &lPlayers = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                     Unit* pPlayer = itr->getSource();

                     if (!pPlayer)
                         continue;

                     if (pPlayer->HasAura(SPELL_LIGHT_ESSENCE))
                         pPlayer->RemoveAurasDueToSpell(SPELL_LIGHT_TOUCH);
                }
                m_uiCheckTouchBuff = 1000;
            }
            else
                m_uiCheckTouchBuff -= uiDiff;
        }

        if (m_uiOrbsTimer <= uiDiff)
        {
            for (uint32 i = 33; i < 37; i++)
            {
                m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, SpawnLoc[i].x + urand(2.0f, 10.0f), SpawnLoc[i].y + urand(2.0f, 10.0f), SpawnLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
                if (m_bIs25Man)
                    m_creature->SummonCreature(NPC_UNLEASHED_LIGHT, SpawnLoc[i + 8].x + urand(2.0f, 10.0f), SpawnLoc[i + 8].y + urand(2.0f, 10.0f), SpawnLoc[i + 8].z, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
            }
            m_uiOrbsTimer = 30000;
        }
        else
            m_uiOrbsTimer -= uiDiff;

        switch (m_uiPhase)
        {
            case PHASE_NORMAL:
                if (m_uiTwinSpikeLTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_TWIN_SPIKE_L) == CAST_OK)
                        m_uiTwinSpikeLTimer = 10000;
                }
                else
                    m_uiTwinSpikeLTimer -= uiDiff;

                if (m_bIsHeroic)
                {
                    if (m_uiLightTouchTimer <= uiDiff)
                    {
                        if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            if (DoCastSpellIfCan(pTarget, SPELL_LIGHT_TOUCH) == CAST_OK)
                                m_uiLightTouchTimer = 20000;
                    }
                    else
                        m_uiLightTouchTimer -= uiDiff;
                }

                if (m_uiSpecialAbilityTimer <= uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    switch(m_uiNextSpell)
                    {
                    case LIGHT_VORTEX: // Vortex
                        if (DoCastSpellIfCan(m_creature, SPELL_LIGHT_VORTEX) == CAST_OK)
                        {
                            m_creature->SetLevitate(true);
                            m_creature->GetPosition(x, y, z);
                            m_creature->GetMotionMaster()->MovePoint(0, x, y, z + 5.0f);
                            DoScriptText(EMOTE_LIGHT_VORTEX, m_creature);
                            DoScriptText(SAY_LIGHT_VORTEX, m_creature);
                            m_uiPhase = PHASE_SPECIAL;
                            m_uiNextSpell = LIGHT_PACT;
                        }
                        break;
                    case LIGHT_PACT: // Pact
                        pSister->CastSpell(pSister, SPELL_TWIN_POWER, true);
                        m_creature->_AddAura(SPELL_LIGHT_SHIELD, 14000);
                        if (DoCastSpellIfCan(m_creature, SPELL_TWIN_PACT_L) == CAST_OK)
                        {
                            m_creature->SetLevitate(true);
                            m_creature->GetPosition(x, y, z);
                            m_creature->GetMotionMaster()->MovePoint(0, x, y, z + 5.0f);
                            DoScriptText(EMOTE_PACT, m_creature);
                            DoScriptText(SAY_PACT, m_creature);
                            m_uiPhase = PHASE_SPECIAL;
                            m_uiNextSpell = LIGHT_VORTEX;
                        }
                        break;
                    default:
                        break;
                    }
                    m_uiSpecialAbilityTimer = 90000;
                }
                else
                    m_uiSpecialAbilityTimer -= uiDiff;

                break;
            case PHASE_SPECIAL:
                if (m_creature->IsNonMeleeSpellCasted(true))
                    return;

                m_creature->SetLevitate(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_uiPhase = PHASE_NORMAL;
                m_uiSpecialAbilityTimer -= uiDiff;
                break;
            default:
                break;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_fjola(Creature* pCreature)
{
    return new boss_fjolaAI(pCreature);
}

struct MANGOS_DLL_DECL boss_eydisAI : public ScriptedAI
{
    boss_eydisAI(Creature* pCreature) : ScriptedAI(pCreature)
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

    bool m_bAchievFailed;

    float x, y, z;

    uint32 m_uiAchievTimer;
    uint32 m_uiPhase;
    uint32 m_uiTwinSpikeDTimer;
    uint32 m_uiSpecialAbilityTimer;
    uint32 m_uiDarkTouchTimer;
    uint32 m_uiCheckTouchBuff;
    uint32 m_uiOrbsTimer;
    uint32 m_uiNextSpell;

    Unit *pSister;

    void Reset() 
    {
        if (!m_pInstance) 
            return;

        pSister = NULL;

        m_bAchievFailed            = false;

        m_uiAchievTimer            = 180000;
        m_uiPhase                  = PHASE_NORMAL;
        m_uiNextSpell              = urand(2,3);
        m_uiTwinSpikeDTimer        = 10000;
        m_uiSpecialAbilityTimer    = 90000;
        m_uiDarkTouchTimer         = 5000;
        m_uiCheckTouchBuff         = 1000;
        m_uiOrbsTimer              = 2500;

        SetEquipmentSlots(false, EQUIP_MAIN_2, EQUIP_OFFHAND_2, EQUIP_RANGED_2);

        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetHealth(m_creature->GetMaxHealth());
    }

    void JustReachedHome()
    {
        if (!m_pInstance) 
            return;

        m_pInstance->SetData(TYPE_VALKIRIES, FAIL);
        m_pInstance->SetData(DATA_HEALTH_EYDIS, m_creature->GetMaxHealth());
        m_creature->ForcedDespawn();
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance) 
            return;

        DoScriptText(SAY_DEATH, m_creature);
        m_pInstance->SetData(TYPE_VALKIRIES, DONE);

        if (pSister)
        {
            if (pSister->isAlive())
                pKiller->DealDamage(pSister, pSister->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        }

        m_pInstance->SetData(DATA_HEALTH_EYDIS, 0);
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() != TYPEID_PLAYER)
            return;

        DoScriptText(SAY_SLAY_1 - urand(0, 1),m_creature,pVictim);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) 
            return;

        pSister = m_pInstance->GetSingleCreatureFromStorage(NPC_LIGHTBANE);

        m_creature->SetInCombatWithZone();
        DoCastSpellIfCan(m_creature, SPELL_DARK_SURGE, CAST_TRIGGERED);

        m_pInstance->SetData(DATA_HEALTH_EYDIS, m_creature->GetMaxHealth());
        m_pInstance->SetData(TYPE_VALKIRIES, IN_PROGRESS);

        m_creature->SummonCreature(NPC_DARK_ESSENCE, SpawnLoc[22].x, SpawnLoc[22].y, SpawnLoc[22].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 5000);
        m_creature->SummonCreature(NPC_DARK_ESSENCE, SpawnLoc[23].x, SpawnLoc[23].y, SpawnLoc[23].z, 0, TEMPSUMMON_MANUAL_DESPAWN, 5000);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_pInstance) 
            return;

        if (!m_creature || !m_creature->isAlive())
            return;

        if (pDoneBy->GetObjectGuid() == m_creature->GetObjectGuid()) 
            return;

        if (pDoneBy->GetTypeId() == TYPEID_PLAYER)
        {
            if (pDoneBy->HasAura(SPELL_DARK_ESSENCE))
                uiDamage /= 2;
            else if (pDoneBy->HasAura(SPELL_LIGHT_ESSENCE))
                uiDamage += uiDamage/2;
        }

        m_pInstance->SetData(DATA_HEALTH_EYDIS, m_creature->GetHealth() >= uiDamage ? m_creature->GetHealth() - uiDamage : 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) 
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!pSister)
            return;


        if (m_creature->GetHealth() > m_pInstance->GetData(DATA_HEALTH_FJOLA) &&
                                      m_pInstance->GetData(DATA_HEALTH_FJOLA) != 0)
                m_creature->SetHealth(m_pInstance->GetData(DATA_HEALTH_FJOLA));

        if (!m_bAchievFailed)
        {
            if (m_uiAchievTimer < uiDiff)
            {
                m_pInstance->SetSpecialAchievementCriteria(TYPE_SALT_AND_PEPPER, false);
                m_bAchievFailed = true;
            }
            else
                m_uiAchievTimer -= uiDiff;
        }

        if (m_bIsHeroic)
        {
            if (m_uiCheckTouchBuff <= uiDiff)
            {
                Map* pMap = m_creature->GetMap();
                Map::PlayerList const &lPlayers = pMap->GetPlayers();
                for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                     Unit* pPlayer = itr->getSource();

                     if (!pPlayer)
                         continue;

                     if (pPlayer->HasAura(SPELL_DARK_ESSENCE))
                         pPlayer->RemoveAurasDueToSpell(SPELL_DARK_TOUCH);
                }
                m_uiCheckTouchBuff = 1000;
            }
            else
                m_uiCheckTouchBuff -= uiDiff;
        }

        if (m_uiOrbsTimer <= uiDiff)
        {
            for (uint32 i = 37; i < 41; i++)
            {
                m_creature->SummonCreature(NPC_UNLEASHED_DARK, SpawnLoc[i].x + urand(2.0f, 10.0f), SpawnLoc[i].y + urand(2.0f, 5.0f), SpawnLoc[i].z, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
                if (m_bIs25Man)
                    m_creature->SummonCreature(NPC_UNLEASHED_DARK, SpawnLoc[i + 8].x + urand(2.0f, 10.0f), SpawnLoc[i + 8].y + urand(2.0f, 5.0f), SpawnLoc[i + 8].z, 0, TEMPSUMMON_TIMED_DESPAWN, 20000);
            }
            m_uiOrbsTimer = 30000;
        }
        else
            m_uiOrbsTimer -= uiDiff;

        switch (m_uiPhase)
        {
            case PHASE_NORMAL:
                if (m_uiTwinSpikeDTimer <= uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_TWIN_SPIKE_D) == CAST_OK)
                        m_uiTwinSpikeDTimer = 10000;
                }
                else
                    m_uiTwinSpikeDTimer -= uiDiff;

                if (m_bIsHeroic)
                {
                    if (m_uiDarkTouchTimer <= uiDiff)
                    {
                        if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            if (DoCastSpellIfCan(pTarget, SPELL_DARK_TOUCH) == CAST_OK)
                                m_uiDarkTouchTimer = 20000;
                    }
                    else
                        m_uiDarkTouchTimer -= uiDiff;
                }

                if (m_uiSpecialAbilityTimer <= uiDiff)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    switch(m_uiNextSpell)
                    {
                    case DARK_VORTEX:
                        if (DoCastSpellIfCan(m_creature, SPELL_DARK_VORTEX) == CAST_OK)
                        {
                            m_creature->SetLevitate(true);
                            m_creature->GetPosition(x, y, z);
                            m_creature->GetMotionMaster()->MovePoint(0, x, y, z + 5.0f);
                            DoScriptText(EMOTE_DARK_VORTEX, m_creature);
                            DoScriptText(SAY_DARK_VORTEX, m_creature);
                            m_uiPhase = PHASE_SPECIAL;
                            m_uiNextSpell = DARK_PACT;
                        }
                        break;
                    case DARK_PACT:
                        pSister->CastSpell(pSister, SPELL_TWIN_POWER, true);
                        m_creature->_AddAura(SPELL_DARK_SHIELD, 14000);
                        if (DoCastSpellIfCan(m_creature, SPELL_TWIN_PACT_D) == CAST_OK)
                        {
                            m_creature->SetLevitate(true);
                            m_creature->GetPosition(x, y, z);
                            m_creature->GetMotionMaster()->MovePoint(0, x, y, z + 5.0f);
                            DoScriptText(EMOTE_PACT, m_creature);
                            DoScriptText(SAY_PACT, m_creature);
                            m_uiPhase = PHASE_SPECIAL;
                            m_uiNextSpell = DARK_VORTEX;
                        }
                        break;
                    default:
                        break;
                    }
                    m_uiSpecialAbilityTimer = 90000;
                }
                else
                    m_uiSpecialAbilityTimer -= uiDiff;

                break;
            case PHASE_SPECIAL:
                if (m_creature->IsNonMeleeSpellCasted(true))
                    return;

                //m_creature->SetLevitate(false);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_uiPhase = PHASE_NORMAL;
                m_uiSpecialAbilityTimer -= uiDiff;
                break;
            default:
                break;
        }
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_eydis(Creature* pCreature)
{
    return new boss_eydisAI(pCreature);
}

struct MANGOS_DLL_DECL mob_light_essenceAI : public ScriptedAI
{
    mob_light_essenceAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() 
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetWalk(true);
        SetCombatMovement(false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) 
            m_creature->ForcedDespawn();

        if (m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) 
        {
            Map* pMap = m_creature->GetMap();
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                 Unit* pPlayer = itr->getSource();
                 if (!pPlayer) continue;
                 if (pPlayer)
                     pPlayer->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
            }

            m_creature->ForcedDespawn();
        }
        return;
    }
};

CreatureAI* GetAI_mob_light_essence(Creature* pCreature)
{
    return new mob_light_essenceAI(pCreature);
};

bool GossipHello_mob_light_essence(Player *player, Creature* pCreature)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pCreature->GetInstanceData();

    if (!pInstance) 
        return true;

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetObjectGuid());
    player->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
    player->CastSpell(player,SPELL_LIGHT_ESSENCE, true);
    player->RemoveAurasDueToSpell(SPELL_LIGHT_TOUCH);
    player->CLOSE_GOSSIP_MENU();
    return true;
};

struct MANGOS_DLL_DECL mob_dark_essenceAI : public ScriptedAI
{
    mob_dark_essenceAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    void Reset() 
    {
        m_creature->SetRespawnDelay(DAY);
        m_creature->SetWalk(true);
        SetCombatMovement(false);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) 
             m_creature->ForcedDespawn();

        if (m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) 
        {
            Map* pMap = m_creature->GetMap();
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                 Unit* pPlayer = itr->getSource();
                 if (!pPlayer) continue;
                 if (pPlayer)
                     pPlayer->RemoveAurasDueToSpell(SPELL_DARK_ESSENCE);
            }
            m_creature->ForcedDespawn();
        }
        return;
    }
};

CreatureAI* GetAI_mob_dark_essence(Creature* pCreature)
{
    return new mob_dark_essenceAI(pCreature);
};

bool GossipHello_mob_dark_essence(Player *player, Creature* pCreature)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pCreature->GetInstanceData();

    if (!pInstance) 
        return true;

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, pCreature->GetObjectGuid());
    player->RemoveAurasDueToSpell(SPELL_LIGHT_ESSENCE);
    player->CastSpell(player,SPELL_DARK_ESSENCE, false);
    player->RemoveAurasDueToSpell(SPELL_DARK_TOUCH);
    player->CLOSE_GOSSIP_MENU();
    return true;
}

struct MANGOS_DLL_DECL mob_unleashed_darkAI : public ScriptedAI
{
    mob_unleashed_darkAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->GetMotionMaster()->MoveRandom();

        SetCombatMovement(false); 

        m_uiCheckTimer = 1000;
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
 
        if (m_uiCheckTimer < uiDiff)
        {
            Map* pMap = m_creature->GetMap();
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                 Unit* pPlayer = itr->getSource();

                 if (!pPlayer) 
                     continue;

                 if (pPlayer->isAlive() && pPlayer->IsWithinDistInMap(m_creature, 4.0f))
                 {
                     if (pPlayer->HasAura(SPELL_DARK_ESSENCE))
                     {
                         if (pPlayer->HasAura(SPELL_EMPOWERED_LIGHT))
                             pPlayer->RemoveAurasDueToSpell(SPELL_EMPOWERED_LIGHT);

                         pPlayer->_AddAura(SPELL_EMPOWERED_DARK, 20000);
                         m_creature->ForcedDespawn(300);
                     }
                     else
                     {
                         DoCastSpellIfCan(m_creature, SPELL_UNLEASHED_DARK, CAST_TRIGGERED);
                         m_creature->ForcedDespawn(700);
                     }
                 }
            }
            m_uiCheckTimer = 500;
        }
        else 
            m_uiCheckTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_unleashed_dark(Creature *pCreature)
{
    return new mob_unleashed_darkAI(pCreature);
}

struct MANGOS_DLL_DECL mob_unleashed_lightAI : public ScriptedAI
{
    mob_unleashed_lightAI(Creature *pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiCheckTimer;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->GetMotionMaster()->MoveRandom();

        SetCombatMovement(false); 

        m_uiCheckTimer = 1000;
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_VALKIRIES) != IN_PROGRESS) 
            m_creature->ForcedDespawn();
 
        if (m_uiCheckTimer < uiDiff)
        {
            Map* pMap = m_creature->GetMap();
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            for (Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
            {
                 Unit* pPlayer = itr->getSource();

                 if (!pPlayer) 
                     continue;

                 if (pPlayer->isAlive() && pPlayer->IsWithinDistInMap(m_creature, 4.0f))
                 {
                     if (pPlayer->HasAura(SPELL_LIGHT_ESSENCE))
                     {
                         if (pPlayer->HasAura(SPELL_EMPOWERED_DARK))
                             pPlayer->RemoveAurasDueToSpell(SPELL_EMPOWERED_DARK);

                         pPlayer->_AddAura(SPELL_EMPOWERED_LIGHT, 20000);
                         m_creature->ForcedDespawn(300);
                     }
                     else
                     {
                         DoCastSpellIfCan(m_creature, SPELL_UNLEASHED_LIGHT, CAST_TRIGGERED);
                         m_creature->ForcedDespawn(700);
                     }
                 }
            }
            m_uiCheckTimer = 500;
        }
        else 
            m_uiCheckTimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_unleashed_light(Creature *pCreature)
{
    return new mob_unleashed_lightAI(pCreature);
}

void AddSC_twin_valkyr()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_fjola";
    pNewScript->GetAI = &GetAI_boss_fjola;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_eydis";
    pNewScript->GetAI = &GetAI_boss_eydis;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_unleashed_light";
    pNewScript->GetAI = &GetAI_mob_unleashed_light;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_unleashed_dark";
    pNewScript->GetAI = &GetAI_mob_unleashed_dark;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_light_essence";
    pNewScript->GetAI = &GetAI_mob_light_essence;
    pNewScript->pGossipHello = &GossipHello_mob_light_essence;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_dark_essence";
    pNewScript->GetAI = &GetAI_mob_dark_essence;
    pNewScript->pGossipHello = &GossipHello_mob_dark_essence;
    pNewScript->RegisterSelf();
}
