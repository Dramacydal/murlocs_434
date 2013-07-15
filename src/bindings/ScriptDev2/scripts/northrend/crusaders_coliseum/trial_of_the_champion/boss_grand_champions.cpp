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
SDName: boss_grand_champions
SD%Complete: 70%
SDComment: missing yells. hunter AI sucks. no pvp diminuishing returns(is it DB related?). modified by /dev/rsa
SDCategory: Trial Of the Champion
EndScriptData */

#include "precompiled.h"
#include "trial_of_the_champion.h"
#include "MapPersistentStateMgr.h"

enum
{
    // Common
    SPELL_BERSERK                           = 47008,
    // Yells
    // Warrior
    SPELL_MORTAL_STRIKE                     = 68783,
    SPELL_MORTAL_STRIKE_H                   = 68784,
    SPELL_BLADESTORM                        = 63784,
    SPELL_INTERCEPT                         = 67540,
    SPELL_ROLLING_THROW                     = 47115, //need core support for spell 67546, using 47115 instead
    // Mage
    SPELL_FIREBALL                          = 66042,
    SPELL_FIREBALL_H                        = 68310,
    SPELL_BLAST_WAVE                        = 66044,
    SPELL_BLAST_WAVE_H                      = 68312,
    SPELL_HASTE                             = 66045,
    SPELL_POLYMORPH                         = 66043,
    SPELL_POLYMORPH_H                       = 68311,
    // Shaman
    SPELL_CHAIN_LIGHTNING                   = 67529,
    SPELL_CHAIN_LIGHTNING_H                 = 68319,
    SPELL_EARTH_SHIELD                      = 67530,
    SPELL_HEALING_WAVE                      = 67528,
    SPELL_HEALING_WAVE_H                    = 68318,
    SPELL_HEX_OF_MENDING                    = 67534,
    // Hunter
    SPELL_DISENGAGE                         = 68340,
    SPELL_LIGHTNING_ARROWS                  = 66083,
    SPELL_MULTI_SHOT                        = 66081,
    SPELL_SHOOT                             = 66079,
    // Rogue
    SPELL_EVISCERATE                        = 67709,
    SPELL_EVISCERATE_H                      = 68317,
    SPELL_FAN_OF_KNIVES                     = 67706,
    SPELL_POISON_BOTTLE                     = 67701
};

// Warrior
struct MANGOS_DLL_DECL mob_toc5_warriorAI : public ScriptedAI
{
    mob_toc5_warriorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mortal_Strike_Timer;
    uint32 Bladestorm_Timer;
    uint32 Rolling_Throw_Timer;
    uint32 Intercept_Cooldown;
    uint32 intercept_check;
    uint32 m_uiBerserk_Timer;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        Mortal_Strike_Timer = m_bIsRegularMode ? 9000 : 6000;
        Bladestorm_Timer = m_bIsRegularMode ? 30000 : 20000;
        Rolling_Throw_Timer = m_bIsRegularMode ? 45000 : 30000;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        Intercept_Cooldown = 0;
        intercept_check = 1000;
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

//      m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(DATA_CHAMPIONS_COUNT, m_pInstance->GetData(DATA_CHAMPIONS_COUNT) - 1);
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

        if (m_pInstance->GetData(DATA_CHAMPIONS_COUNT) < 1)
        {
            m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);

            #ifndef WIN32
            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_GRAND_CHAMP, m_creature);
            #endif

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_GRAND_CHAMP, 0, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Mortal_Strike_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MORTAL_STRIKE : SPELL_MORTAL_STRIKE_H);
            Mortal_Strike_Timer = m_bIsRegularMode ? 6000 : 4000;
        }
        else
            Mortal_Strike_Timer -= diff;

        if (Rolling_Throw_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_ROLLING_THROW);
            Rolling_Throw_Timer = m_bIsRegularMode ? 30000 : 15000;
        }
        else
            Rolling_Throw_Timer -= diff;

        if (Bladestorm_Timer < diff)
        {
            DoCast(m_creature, SPELL_BLADESTORM);
            Bladestorm_Timer = m_bIsRegularMode ? 60000 : 20000;
        }
        else
            Bladestorm_Timer -= diff;

        if (intercept_check < diff)
        {
            if (!m_creature->IsWithinDistInMap(m_creature->getVictim(), 8) && m_creature->IsWithinDistInMap(m_creature->getVictim(), 25) && Intercept_Cooldown < diff)
            {
                DoCast(m_creature->getVictim(), SPELL_INTERCEPT);
                Intercept_Cooldown = m_bIsRegularMode ? 15000 : 10000;
            }
            intercept_check = 1000;
        }
        else
        {
            intercept_check -= diff;
            Intercept_Cooldown -= diff;
        }
        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        }
        else
            m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_toc5_warrior(Creature* pCreature)
{
    return new mob_toc5_warriorAI(pCreature);
}

// Mage
struct MANGOS_DLL_DECL mob_toc5_mageAI : public ScriptedAI
{
    mob_toc5_mageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Fireball_Timer;
    uint32 Blast_Wave_Timer;
    uint32 Haste_Timer;
    uint32 Polymorph_Timer;
    uint32 m_uiBerserk_Timer;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        Fireball_Timer = 0;
        Blast_Wave_Timer = m_bIsRegularMode ? 20000 : 12000;
        Haste_Timer = m_bIsRegularMode ? 12000 : 9000;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        Polymorph_Timer = m_bIsRegularMode ? 12000 : 10000;
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

//      m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(DATA_CHAMPIONS_COUNT, m_pInstance->GetData(DATA_CHAMPIONS_COUNT) - 1);
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

        if (m_pInstance->GetData(DATA_CHAMPIONS_COUNT) < 1)
        {
            m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);

            #ifndef WIN32
            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_GRAND_CHAMP, m_creature);
            #endif

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_GRAND_CHAMP, 0, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Fireball_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H);
            Fireball_Timer = m_bIsRegularMode ? 5000 : 3000;
        }
        else
            Fireball_Timer -= diff;

        if (Blast_Wave_Timer < diff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_BLAST_WAVE : SPELL_BLAST_WAVE_H);
            Blast_Wave_Timer = m_bIsRegularMode ? 20000 : 12000;
        }
        else
            Blast_Wave_Timer -= diff;

        if (Haste_Timer < diff)
        {
            DoCast(m_creature, SPELL_HASTE);
            Haste_Timer = m_bIsRegularMode ? 10000 : 8000;
        }
        else
            Haste_Timer -= diff;

        if (Polymorph_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(target, m_bIsRegularMode ? SPELL_POLYMORPH : SPELL_POLYMORPH_H);

            Polymorph_Timer = m_bIsRegularMode ? 20000 : 15000;
        }
        else
            Polymorph_Timer -= diff;

        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        }
        else
            m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_toc5_mage(Creature* pCreature)
{
    return new mob_toc5_mageAI(pCreature);
}

// Shaman
struct MANGOS_DLL_DECL mob_toc5_shamanAI : public ScriptedAI
{
    mob_toc5_shamanAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Chain_Lightning_Timer;
    uint32 Earth_Shield_Timer;
    uint32 Healing_Wave_Timer;
    uint32 Hex_Timer;

    float mob1_health;
    float mob2_health;
    float mob3_health;
    uint32 m_uiBerserk_Timer;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        Chain_Lightning_Timer = m_bIsRegularMode ? 2000 : 1000;
        Earth_Shield_Timer = m_bIsRegularMode ? 10000 : 5000;
        Healing_Wave_Timer = m_bIsRegularMode ? 20000 : 12000;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        Hex_Timer = m_bIsRegularMode ? 15000 : 10000;
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

//      m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(DATA_CHAMPIONS_COUNT, m_pInstance->GetData(DATA_CHAMPIONS_COUNT) - 1);
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

        if (m_pInstance->GetData(DATA_CHAMPIONS_COUNT) < 1)
        {
            m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);

            #ifndef WIN32
            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_GRAND_CHAMP, m_creature);
            #endif

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_GRAND_CHAMP, 0, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Chain_Lightning_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);
            Chain_Lightning_Timer = m_bIsRegularMode ? 12000 : 8000;
        }
        else
            Chain_Lightning_Timer -= diff;

        if (Hex_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_HEX_OF_MENDING);
            Hex_Timer = m_bIsRegularMode ? 30000 : 20000;
        }
        else
            Hex_Timer -= diff;

        if (Healing_Wave_Timer < diff)
        {
            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
            {
                if (pTemp->isAlive())
                    mob1_health = pTemp->GetHealth()*100 / pTemp->GetMaxHealth();
                else
                    mob1_health = 100;
            }
            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
            {
                if (pTemp->isAlive())
                    mob2_health = pTemp->GetHealth()*100 / pTemp->GetMaxHealth();
                else
                    mob2_health = 100;
            }
            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
            {
                if (pTemp->isAlive())
                    mob3_health = pTemp->GetHealth()*100 / pTemp->GetMaxHealth();
                else
                    mob3_health = 100;
            }
            if (mob1_health < mob2_health && mob1_health < mob3_health && mob1_health < 70)
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                    DoCast(pTemp, m_bIsRegularMode ? SPELL_HEALING_WAVE : SPELL_HEALING_WAVE_H);

            if (mob1_health > mob2_health && mob2_health < mob3_health && mob2_health < 70)
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                    DoCast(pTemp, m_bIsRegularMode ? SPELL_HEALING_WAVE : SPELL_HEALING_WAVE_H);

            if (mob3_health < mob2_health && mob1_health > mob3_health && mob3_health < 70)
                if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                    DoCast(pTemp, m_bIsRegularMode ? SPELL_HEALING_WAVE : SPELL_HEALING_WAVE_H);

            Healing_Wave_Timer = m_bIsRegularMode ? 8000 : 6000;
        }
        else
            Healing_Wave_Timer -= diff;

        if (Earth_Shield_Timer < diff)
        {
            switch(urand(0, 2))
            {
                case 0:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                    {
                        if (pTemp->isAlive())
                            DoCast(pTemp, SPELL_EARTH_SHIELD);
                        else
                            DoCast(m_creature, SPELL_EARTH_SHIELD);
                    }
                    break;
                case 1:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                    {
                        if (pTemp->isAlive())
                            DoCast(pTemp, SPELL_EARTH_SHIELD);
                        else
                            DoCast(m_creature, SPELL_EARTH_SHIELD);
                    }
                    break;
                case 2:
                    if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                    {
                        if (pTemp->isAlive())
                            DoCast(pTemp, SPELL_EARTH_SHIELD);
                        else
                            DoCast(m_creature, SPELL_EARTH_SHIELD);
                    }
                    break;
            }
            Earth_Shield_Timer = m_bIsRegularMode ? 35000 : 25000;
        }
        else
            Earth_Shield_Timer -= diff;

        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        }
        else
            m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_toc5_shaman(Creature* pCreature)
{
    return new mob_toc5_shamanAI(pCreature);
}

// Hunter
struct MANGOS_DLL_DECL mob_toc5_hunterAI : public ScriptedAI
{
    mob_toc5_hunterAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Shoot_Timer;
    uint32 Lightning_Arrows_Timer;
    uint32 Multi_Shot_Timer;
    uint32 Disengage_Cooldown;
    uint32 enemy_check;
    uint32 disengage_check;
    uint32 m_uiBerserk_Timer;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        Shoot_Timer = 0;
        Lightning_Arrows_Timer = m_bIsRegularMode ? 18000 : 10000;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        Multi_Shot_Timer = m_bIsRegularMode ? 15000 : 8000;
        Disengage_Cooldown = 0;
        enemy_check = 1000;
        disengage_check;
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

//      m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(DATA_CHAMPIONS_COUNT, m_pInstance->GetData(DATA_CHAMPIONS_COUNT) - 1);
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

        if (m_pInstance->GetData(DATA_CHAMPIONS_COUNT) < 1)
        {
            m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);

            #ifndef WIN32
            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_GRAND_CHAMP, m_creature);
            #endif

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_GRAND_CHAMP, 0, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (enemy_check < diff)
        {
            if (!m_creature->IsWithinDistInMap(m_creature->getVictim(), 8) && m_creature->IsWithinDistInMap(m_creature->getVictim(), 30))
            {
                m_creature->SetSpeedRate(MOVE_RUN, 0.0001f);
            }
            else
            {
                m_creature->SetSpeedRate(MOVE_RUN, 1.2f);
            }
            enemy_check = 100;
        }
        else
            enemy_check -= diff;

        if (Disengage_Cooldown>0)
            Disengage_Cooldown -= diff;

        if (Shoot_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_SHOOT);
            Shoot_Timer = m_bIsRegularMode ? 5000 : 3000;
        }
        else
            Shoot_Timer -= diff;

        if (Multi_Shot_Timer < diff)
        {
            m_creature->CastStop(SPELL_SHOOT);
            DoCast(m_creature->getVictim(), SPELL_MULTI_SHOT);
            Multi_Shot_Timer = m_bIsRegularMode ? 10000 : 5000;
        }
        else
            Multi_Shot_Timer -= diff;

        if (Lightning_Arrows_Timer < diff)
        {
            m_creature->CastStop(SPELL_SHOOT);
            DoCast(m_creature, SPELL_LIGHTNING_ARROWS);
            Lightning_Arrows_Timer = m_bIsRegularMode ? 15000 : 8000;
        }
        else
            Lightning_Arrows_Timer -= diff;

        if (disengage_check < diff)
        {
            if (m_creature->IsWithinDistInMap(m_creature->getVictim(), 5) && Disengage_Cooldown == 0)
            {
                DoCast(m_creature, SPELL_DISENGAGE);
                Disengage_Cooldown = m_bIsRegularMode ? 15000 : 10000;
            }
            disengage_check = 1000;
        }
        else
            disengage_check -= diff;

        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        }
        else
            m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_toc5_hunter(Creature* pCreature)
{
    return new mob_toc5_hunterAI(pCreature);
}

// Rogue
struct MANGOS_DLL_DECL mob_toc5_rogueAI : public ScriptedAI
{
    mob_toc5_rogueAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Eviscerate_Timer;
    uint32 FoK_Timer;
    uint32 Poison_Timer;
    uint32 m_uiBerserk_Timer;

    void Reset()
    {
        m_creature->SetRespawnDelay(DAY);
        Eviscerate_Timer = m_bIsRegularMode ? 20000 : 10000;
        FoK_Timer = m_bIsRegularMode ? 15000 : 10000;
        m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        Poison_Timer = m_bIsRegularMode ? 12000 : 5000;
        m_creature->GetMotionMaster()->MovePoint(0, 746, 614, m_creature->GetPositionZ());
        m_creature->SetWalk(true);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
            if (pTemp->isAlive())
                pTemp->SetInCombatWithZone();

//      m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(DATA_CHAMPIONS_COUNT, m_pInstance->GetData(DATA_CHAMPIONS_COUNT) - 1);
        m_creature->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

        if (m_pInstance->GetData(DATA_CHAMPIONS_COUNT) < 1)
        {
            m_pInstance->SetData(TYPE_GRAND_CHAMPIONS, DONE);

            #ifndef WIN32
            if (DungeonPersistentState* state = ((DungeonMap*)m_pInstance->instance)->GetPersistanceState())
                state->UpdateEncounterState(ENCOUNTER_CREDIT_CAST_SPELL, SPELL_CREDIT_GRAND_CHAMP, m_creature);
            #endif

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_1))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_2))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);

            if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(DATA_CHAMPION_3))
                pTemp->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_LOOTABLE);
        }

        m_pInstance->UpdateAchievementCriteria(28, SPELL_CREDIT_GRAND_CHAMP, 0, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Eviscerate_Timer < diff)
        {
            DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_EVISCERATE : SPELL_EVISCERATE_H);
            Eviscerate_Timer = m_bIsRegularMode ? 15000 : 10000;
        }
        else
            Eviscerate_Timer -= diff;

        if (FoK_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_FAN_OF_KNIVES);
            FoK_Timer = m_bIsRegularMode ? 12000 : 7000;
        }
        else
            FoK_Timer -= diff;

        if (Poison_Timer < diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM,0))
                DoCast(m_creature, SPELL_POISON_BOTTLE);

            Poison_Timer = m_bIsRegularMode ? 10000 : 5000;
        }
        else
            Poison_Timer -= diff;

        if (m_uiBerserk_Timer < diff)
        {
            DoCast(m_creature, SPELL_BERSERK);
            m_uiBerserk_Timer = m_bIsRegularMode ? 300000 : 180000;
        }
        else
            m_uiBerserk_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_toc5_rogue(Creature* pCreature)
{
    return new mob_toc5_rogueAI(pCreature);
}

void AddSC_boss_grand_champions()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "mob_toc5_warrior";
    NewScript->GetAI = &GetAI_mob_toc5_warrior;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_toc5_mage";
    NewScript->GetAI = &GetAI_mob_toc5_mage;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_toc5_shaman";
    NewScript->GetAI = &GetAI_mob_toc5_shaman;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_toc5_hunter";
    NewScript->GetAI = &GetAI_mob_toc5_hunter;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_toc5_rogue";
    NewScript->GetAI = &GetAI_mob_toc5_rogue;
    NewScript->RegisterSelf();
}
