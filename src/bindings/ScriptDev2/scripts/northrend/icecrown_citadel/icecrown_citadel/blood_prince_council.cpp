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
SDName: blood_prince_council
SD%Complete: 90%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */
// Need implement true movement for kinetic bomb, correct yells.
#include "precompiled.h"
#include "icecrown_citadel.h"

enum BossSpells
{
    SPELL_BERSERK                           = 47008,
    SPELL_FAKE_DEATH                        = 71598,

    //Darkfallen Orb
    SPELL_INVOCATION_OF_BLOOD_V             = 70952,   //
    SPELL_INVOCATION_OF_BLOOD_K             = 70981,   //
    SPELL_INVOCATION_OF_BLOOD_T             = 70982,   //
    SPELL_INVOCATION_OF_BLOOD_AURA          = 70983,   // Triggered, override

    //Valanar
    SPELL_KINETIC_BOMB                      = 72053,
    NPC_KINETIC_BOMB_TARGET                 = 38458,
    NPC_KINETIC_BOMB                        = 38454,
    SPELL_KINETIC_BOMB_EXPLODE              = 72052,
    SPELL_SHOCK_VORTEX                      = 72037,
    NPC_SHOCK_VORTEX                        = 38422,
    SPELL_SHOCK_VORTEX_AURA                 = 71945,
    SPELL_SHOCK_VORTEX_2                    = 72039,

    //Taldaram
    SPELL_GLITTERING_SPARKS                 = 71807,
    SPELL_CONJURE_FLAME_1                   = 71718,
    SPELL_SUMMON_FLAME_1                    = 71719, //Dummy effect
    NPC_BALL_OF_FLAMES_1                    = 38332,
    SPELL_CONJURE_FLAME_2                   = 72040,
    SPELL_SUMMON_FLAME_2                    = 72041, //Dummy effect
    NPC_BALL_OF_FLAMES_2                    = 38451,
    SPELL_FLAMES_AURA                       = 71709,
    SPELL_FLAMES                            = 71393,

    //Keleseth
    SPELL_SHADOW_LANCE                      = 71405,
    SPELL_SHADOW_LANCE_2                    = 71815,
    SPELL_SHADOW_RESONANCE                  = 71943,
    SPELL_SHADOW_RESONANCE_AURA             = 71822,
    NPC_DARK_NUCLEUS                        = 38369,

    // Blood orb
    SPELL_BLOOD_ORB_STATE_VISUAL            = 72100,
};

struct MANGOS_DLL_DECL boss_valanar_iccAI : public BSWScriptedAI
{
    boss_valanar_iccAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Creature* pBrother1;
    Creature* pBrother2;
    bool invocated;
    uint32 m_health;

    void Reset()
    {
        if(!m_pInstance)
            return;
        resetTimers();
        invocated = false;
        m_health = m_creature->GetMaxHealth();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0,1))
        {
            case 0:
                DoScriptText(-1631302,m_creature,pVictim);
                break;
            case 1:
                DoScriptText(-1631303,m_creature,pVictim);
                break;
        }
    }

    void EnterEvadeMode()
    {
        m_creature->CombatStop(true);
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->GetMotionMaster()->MoveTargetedHome();
        m_creature->SetLootRecipient(NULL);
        Reset();
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;
        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, FAIL);
        m_creature->LoadCreatureAddon(false);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;
        DoScriptText(-1631304,m_creature,pKiller);
        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, DONE);

        if (pBrother1 && pBrother1->isAlive())
            pKiller->DealDamage(pBrother1, pBrother1->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        if (pBrother2 && pBrother2->isAlive())
            pKiller->DealDamage(pBrother2, pBrother2->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (!m_pInstance->IsEncounterAvailable(TYPE_BLOOD_COUNCIL))
        {
            m_pInstance->SendGMEncounterNotification(pWho, TYPE_BLOOD_COUNCIL);
            EnterEvadeMode();
            return;
        }

        pBrother1 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_TALDARAM));
        pBrother2 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KELESETH));

        if (pBrother1 && !pBrother1->isAlive())
            pBrother1->Respawn();
        if (pBrother2 && !pBrother2->isAlive())
            pBrother2->Respawn();

        if (pBrother1) pBrother1->SetInCombatWithZone();
        if (pBrother2) pBrother2->SetInCombatWithZone();

        invocated = false;
        m_health = m_creature->GetMaxHealth();
        m_creature->SetHealth(1);

        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, IN_PROGRESS);
        m_creature->CastSpell(m_creature,(urand(0,1) ? SPELL_INVOCATION_OF_BLOOD_T : SPELL_INVOCATION_OF_BLOOD_K),true);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_pInstance)
            return;

        if (!m_pInstance->IsEncounterAvailable(TYPE_BLOOD_COUNCIL))
        {
            m_pInstance->SendGMEncounterNotification(pDoneBy, TYPE_BLOOD_COUNCIL);
            EnterEvadeMode();
            uiDamage = 0;
            return;
        }

        if (!m_creature || !m_creature->isAlive())
            return;

        if (!invocated)
            uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (hasAura(SPELL_INVOCATION_OF_BLOOD_V))
        {
            if (!invocated)
            {
                DoScriptText(-1631307,m_creature);
                m_creature->SetHealth(m_health);
                invocated = true;
            }
            if (timedQuery(SPELL_INVOCATION_OF_BLOOD_V, uiDiff))
            {
                if (doCast(urand(0,1) ? SPELL_INVOCATION_OF_BLOOD_K : SPELL_INVOCATION_OF_BLOOD_T) == CAST_OK)
                {
                    doRemove(SPELL_INVOCATION_OF_BLOOD_V);
                    invocated = false;
                    m_health = m_creature->GetHealth();
                    m_creature->SetHealth(1);
                }
            }
            timedCast(SPELL_KINETIC_BOMB, uiDiff);
            timedCast(SPELL_SHOCK_VORTEX_2, uiDiff);
        }
        else
        {
            timedCast(SPELL_KINETIC_BOMB, uiDiff);
            timedCast(SPELL_SHOCK_VORTEX, uiDiff);
        }

        if (timedQuery(SPELL_BERSERK, uiDiff))
        {
             doCast(SPELL_BERSERK);
             DoScriptText(-1631305,m_creature);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_valanar_icc(Creature* pCreature)
{
    return new boss_valanar_iccAI(pCreature);
}

struct MANGOS_DLL_DECL boss_taldaram_iccAI : public BSWScriptedAI
{
    boss_taldaram_iccAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Creature* pBrother1;
    Creature* pBrother2;
    bool invocated;
    uint8 ballscount;
    uint32 m_health;

    void Reset()
    {
        if(!m_pInstance)
            return;
        resetTimers();
        invocated = false;
        ballscount = 0;
        m_health = m_creature->GetMaxHealth();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void EnterEvadeMode()
    {
        m_creature->CombatStop(true);
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->GetMotionMaster()->MoveTargetedHome();
        m_creature->SetLootRecipient(NULL);
        Reset();
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;
        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, FAIL);
        m_creature->LoadCreatureAddon(false);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;
        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, DONE);

        if (pBrother1 && pBrother1->isAlive())
            pKiller->DealDamage(pBrother1, pBrother1->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        if (pBrother2 && pBrother2->isAlive())
            pKiller->DealDamage(pBrother2, pBrother2->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

    }

    void KilledUnit(Unit* pVictim)
    {
        if (!m_pInstance)
            return;
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (!m_pInstance->IsEncounterAvailable(TYPE_BLOOD_COUNCIL))
        {
            m_pInstance->SendGMEncounterNotification(pWho, TYPE_BLOOD_COUNCIL);
            EnterEvadeMode();
            return;
        }

        pBrother1 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_VALANAR));
        pBrother2 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_KELESETH));
        if (pBrother1 && !pBrother1->isAlive())
            pBrother1->Respawn();
        if (pBrother2 && !pBrother2->isAlive())
            pBrother2->Respawn();
        if (pBrother1)
            pBrother1->SetInCombatWithZone();
        if (pBrother2)
            pBrother2->SetInCombatWithZone();

        if (!hasAura(SPELL_INVOCATION_OF_BLOOD_T))
        {
            invocated = false;
            m_health = m_creature->GetMaxHealth();
            m_creature->SetHealth(1);
        }
        else
            m_creature->SetHealth(m_creature->GetMaxHealth());

        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, IN_PROGRESS);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_pInstance)
            return;

        if (!m_pInstance->IsEncounterAvailable(TYPE_BLOOD_COUNCIL))
        {
            m_pInstance->SendGMEncounterNotification(pDoneBy, TYPE_BLOOD_COUNCIL);
            EnterEvadeMode();
            uiDamage = 0;
            return;
        }

        if (!m_creature || !m_creature->isAlive())
            return;

        if (!invocated)
            uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (hasAura(SPELL_INVOCATION_OF_BLOOD_T))
        {
            if (!invocated)
            {
                DoScriptText(-1631307,m_creature);
                m_creature->SetHealth(m_health);
                invocated = true;
            }
            if (timedQuery(SPELL_INVOCATION_OF_BLOOD_T, uiDiff))
            {
                if (doCast(urand(0,1) ? SPELL_INVOCATION_OF_BLOOD_V : SPELL_INVOCATION_OF_BLOOD_K) == CAST_OK)
                {
                    doRemove(SPELL_INVOCATION_OF_BLOOD_T);
                    invocated = false;
                    m_health = m_creature->GetHealth();
                    m_creature->SetHealth(1);
                }
            }
            if (ballscount > 0 && !m_creature->IsNonMeleeSpellCasted(false))
            {
                doCast(SPELL_SUMMON_FLAME_2);
                --ballscount;
            }
            timedCast(SPELL_GLITTERING_SPARKS, uiDiff);
            if (timedCast(SPELL_CONJURE_FLAME_2, uiDiff) == CAST_OK) ballscount = 1;
         }
         else
         {
            if (ballscount > 0 && !m_creature->IsNonMeleeSpellCasted(false))
            {
                doCast(SPELL_SUMMON_FLAME_1);
                --ballscount;
            };
            timedCast(SPELL_GLITTERING_SPARKS, uiDiff);
            if (timedCast(SPELL_CONJURE_FLAME_1, uiDiff) == CAST_OK) 
                ballscount = 1;
         }

        if (timedQuery(SPELL_BERSERK, uiDiff))
        {
            doCast(SPELL_BERSERK);
            DoScriptText(-1631305,m_creature);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_taldaram_icc(Creature* pCreature)
{
    return new boss_taldaram_iccAI(pCreature);
}

struct MANGOS_DLL_DECL boss_keleseth_iccAI : public BSWScriptedAI
{
    boss_keleseth_iccAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Creature* pBrother1;
    Creature* pBrother2;
    bool invocated;
    uint32 m_health;

    void Reset()
    {
        if(!m_pInstance)
            return;
        resetTimers();
        invocated = false;
        m_health = m_creature->GetMaxHealth();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
    }

    void EnterEvadeMode()
    {
        m_creature->CombatStop(true);
        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->GetMotionMaster()->MoveTargetedHome();
        m_creature->SetLootRecipient(NULL);
        Reset();
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;
        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, FAIL);
        m_creature->LoadCreatureAddon(false);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;
        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, DONE);

        if (pBrother1 && pBrother1->isAlive())
            pKiller->DealDamage(pBrother1, pBrother1->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

        if (pBrother2 && pBrother2->isAlive())
            pKiller->DealDamage(pBrother2, pBrother2->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);

    }

    void KilledUnit(Unit* pVictim)
    {
        if (!m_pInstance)
            return;
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (!m_pInstance->IsEncounterAvailable(TYPE_BLOOD_COUNCIL))
        {
            m_pInstance->SendGMEncounterNotification(pWho, TYPE_BLOOD_COUNCIL);
            EnterEvadeMode();
            return;
        }

        pBrother1 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_TALDARAM));
        pBrother2 = m_creature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_VALANAR));

        if (pBrother1 && !pBrother1->isAlive())
            pBrother1->Respawn();
        if (pBrother2 && !pBrother2->isAlive())
            pBrother2->Respawn();
        if (pBrother1)
            pBrother1->SetInCombatWithZone();
        if (pBrother2)
            pBrother2->SetInCombatWithZone();

        if (!hasAura(SPELL_INVOCATION_OF_BLOOD_K))
        {
            invocated = false;
            m_health = m_creature->GetMaxHealth();
            m_creature->SetHealth(1);
        }
        else
            m_creature->SetHealth(m_creature->GetMaxHealth());

        m_pInstance->SetData(TYPE_BLOOD_COUNCIL, IN_PROGRESS);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        DoStartMovement(pWho, 30.0f);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_pInstance)
            return;

        if (!m_pInstance->IsEncounterAvailable(TYPE_BLOOD_COUNCIL))
        {
            m_pInstance->SendGMEncounterNotification(pDoneBy, TYPE_BLOOD_COUNCIL);
            EnterEvadeMode();
            return;
        }

        if (!m_creature || !m_creature->isAlive())
            return;

        if (!invocated)
            uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (hasAura(SPELL_INVOCATION_OF_BLOOD_K))
        {
            if (!invocated)
            {
                DoScriptText(-1631307,m_creature);
                m_creature->SetHealth(m_health);
                invocated = true;
            }

            if (timedQuery(SPELL_INVOCATION_OF_BLOOD_K, uiDiff))
            {
                if (doCast(urand(0,1) ? SPELL_INVOCATION_OF_BLOOD_V : SPELL_INVOCATION_OF_BLOOD_T) == CAST_OK)
                {
                    doRemove(SPELL_INVOCATION_OF_BLOOD_K);
                    invocated = false;
                    m_health = m_creature->GetHealth();
                    m_creature->SetHealth(1);
                }
            }

            timedCast(SPELL_SHADOW_LANCE_2, uiDiff);
            timedCast(SPELL_SHADOW_RESONANCE, uiDiff);
        }
        else
        {
            timedCast(SPELL_SHADOW_LANCE, uiDiff);
            timedCast(SPELL_SHADOW_RESONANCE, uiDiff);
        }

        if (timedQuery(SPELL_BERSERK, uiDiff))
        {
            doCast(SPELL_BERSERK);
            DoScriptText(-1631305,m_creature);
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_keleseth_icc(Creature* pCreature)
{
    return new boss_keleseth_iccAI(pCreature);
}

struct MANGOS_DLL_DECL mob_dark_nucleusAI : public ScriptedAI
{
    mob_dark_nucleusAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_lifetimer;
    uint32 m_casttimer;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetInCombatWithZone();
        m_lifetimer = 180000;
        m_casttimer = 2000;
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance) return;
        m_creature->SetSpeedRate(MOVE_RUN, 0.5);
        SetCombatMovement(true);
        DoStartMovement(pWho);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_BLOOD_COUNCIL) != IN_PROGRESS) 
              m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_casttimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SHADOW_RESONANCE_AURA) == CAST_OK)
                m_lifetimer -= 4000;

            m_casttimer = 2000;
        }
        else
            m_casttimer -= uiDiff;

        if (m_lifetimer <= uiDiff)
            m_creature->ForcedDespawn();
        else
            m_lifetimer -= uiDiff;

    }
};

CreatureAI* GetAI_mob_dark_nucleus(Creature* pCreature)
{
     return new mob_dark_nucleusAI (pCreature);
};

struct MANGOS_DLL_DECL mob_ball_of_flamesAI : public ScriptedAI
{
    mob_ball_of_flamesAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 grow_timer;
    float fPosX, fPosY, fPosZ;
    float m_Size;
    float m_Size0;
    bool finita;
    bool movementstarted;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetInCombatWithZone();
        finita = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        m_creature->SetLevitate(true);
        SetCombatMovement(false);
        m_creature->GetPosition(fPosX, fPosY, fPosZ);
        m_creature->GetRandomPoint(fPosX, fPosY, fPosZ, urand(40, 60), fPosX, fPosY, fPosZ);
        m_creature->GetMotionMaster()->MovePoint(1, fPosX, fPosY, fPosZ);
        movementstarted = true;
        m_Size0 = m_creature->GetObjectScale();
        m_Size = m_Size0;
        grow_timer = 500;

        m_creature->SetDisplayId(26767);
//        if (m_creature->GetEntry() == NPC_BALL_OF_FLAMES_2)
            DoCast(m_creature, SPELL_FLAMES_AURA);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (!m_pInstance || type != POINT_MOTION_TYPE) return;
        if (id != 1)
            m_creature->GetMotionMaster()->MovePoint(1, fPosX, fPosY, fPosZ);
        else movementstarted = false;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_BLOOD_COUNCIL) != IN_PROGRESS || finita)
              m_creature->ForcedDespawn();

        if (!movementstarted)
        {
            DoCast(m_creature, SPELL_FLAMES);
            finita = true;
            return;
        }

        if (m_creature->GetEntry() == NPC_BALL_OF_FLAMES_2)
        {
            if (!m_creature->HasAura(SPELL_FLAMES_AURA))
                DoCast(m_creature, SPELL_FLAMES_AURA);


            if (grow_timer <= uiDiff)
            {
                m_Size = m_Size*1.03;
                m_creature->SetObjectScale(m_Size);
                grow_timer = 500;
            }
            else
                grow_timer -= uiDiff;
        }
        else return;

    }
};

CreatureAI* GetAI_mob_ball_of_flames(Creature* pCreature)
{
     return new mob_ball_of_flamesAI (pCreature);
};

struct MANGOS_DLL_DECL mob_kinetic_bombAI : public ScriptedAI
{
    mob_kinetic_bombAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_lifetimer;
    float fPosX0, fPosY0, fPosZ0;
    float fPosX1, fPosY1, fPosZ1;
    bool finita;
    Creature *owner;

    void Reset()
    {
        owner = m_creature->GetMap()->GetCreature(m_creature->GetCreatorGuid());

        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetInCombatWithZone();

        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
        m_creature->SetLevitate(true);
        m_creature->SetSpeedRate(MOVE_RUN, 0.2f);
        m_creature->SetSpeedRate(MOVE_WALK, 0.2f);
        SetCombatMovement(false);

        m_lifetimer = 60000;
//        m_creature->SetDisplayId(31095);

        m_creature->GetPosition(fPosX0, fPosY0, fPosZ0);

        if (!owner) return;
        owner->GetPosition(fPosX1, fPosY1, fPosZ1);

        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(1, fPosX1, fPosY1, fPosZ1);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE)
            return;
        if ( id ==1 )
        {
            finita = true;
            DoCast(m_creature, SPELL_KINETIC_BOMB_EXPLODE);
        }
        else
            m_creature->GetMotionMaster()->MovePoint(1, fPosX1, fPosY1, fPosZ1);
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

/*
Place shock bomb movement here
*/

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_BLOOD_COUNCIL) != IN_PROGRESS || finita)
            m_creature->ForcedDespawn();

    }
};

CreatureAI* GetAI_mob_kinetic_bomb(Creature* pCreature)
{
     return new mob_kinetic_bombAI (pCreature);
};

struct MANGOS_DLL_DECL mob_shock_vortexAI : public ScriptedAI
{
    mob_shock_vortexAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 m_lifetimer;
    bool finita;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetInCombatWithZone();
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_lifetimer = 8000;
        SetCombatMovement(false);
        DoCast(m_creature, SPELL_SHOCK_VORTEX_AURA);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_BLOOD_COUNCIL) != IN_PROGRESS || finita)
              m_creature->ForcedDespawn();

        if (!m_creature->HasAura(SPELL_SHOCK_VORTEX_AURA))
              DoCast(m_creature, SPELL_SHOCK_VORTEX_AURA);

        if (m_lifetimer <= uiDiff)
            finita = true;
        else
            m_lifetimer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_shock_vortex(Creature* pCreature)
{
     return new mob_shock_vortexAI (pCreature);
};

struct MANGOS_DLL_DECL mob_kinetic_bomb_targetAI : public ScriptedAI
{
    mob_kinetic_bomb_targetAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    Creature* bomb;
    ScriptedInstance* m_pInstance;

    void Reset()
    {
        m_creature->SetDisplayId(21342);
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        SetCombatMovement(false);
        bomb = NULL;
    }

    void SummonedCreatureJustDied(Creature* summoned)
    {
         if (!m_pInstance || !summoned) return;

         if (summoned == bomb)
             m_creature->ForcedDespawn();
    }

    void AttackStart(Unit *who)
    {
        //ignore all attackstart commands
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance || m_pInstance->GetData(TYPE_BLOOD_COUNCIL) != IN_PROGRESS)
              m_creature->ForcedDespawn();

        if (!bomb)
            bomb = m_creature->SummonCreature(NPC_KINETIC_BOMB,m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ()+50.0f,0,TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN,60000);

    }
};

CreatureAI* GetAI_mob_kinetic_bomb_target(Creature* pCreature)
{
     return new mob_kinetic_bomb_targetAI (pCreature);
};

struct MANGOS_DLL_DECL boss_blood_queen_lanathel_introAI : public BSWScriptedAI
{
    boss_blood_queen_lanathel_introAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    uint32 UpdateTimer;

    void Reset()
    {
        m_creature->SetVisibility(VISIBILITY_OFF);

        if(!pInstance)
            return;

        if (pInstance->GetData(TYPE_BLOOD_COUNCIL) == IN_PROGRESS)
            return;

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (pInstance->GetData(TYPE_BLOOD_COUNCIL) == DONE)
        {
            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            m_creature->SetVisibility(VISIBILITY_OFF);
        }
        else
        {
            if (Creature* pPrince = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_TALDARAM)))
                doCast(SPELL_FAKE_DEATH,pPrince);
            if (Creature* pPrince = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_KELESETH)))
                doCast(SPELL_FAKE_DEATH,pPrince);
            if (Creature* pPrince = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_VALANAR)))
                doCast(SPELL_FAKE_DEATH,pPrince);
            pInstance->SetData(TYPE_BLOOD_COUNCIL, NOT_STARTED);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        }
    }

    void AttackStart(Unit *who)
    {
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!pInstance)
            return;

        if (pInstance->GetData(TYPE_BLOOD_COUNCIL) != NOT_STARTED)
            return;

        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER && pWho->IsWithinDistInMap(m_creature, 50.0f))
        {
            pInstance->SetData(TYPE_EVENT, 800);
            pInstance->SetData(TYPE_BLOOD_COUNCIL,IN_PROGRESS);
        }
    }

    void UpdateAI(const uint32 diff)
    {

        if (pInstance->GetData(TYPE_BLOOD_COUNCIL) == FAIL)
        {
            Reset();
            return;
        }

        if (pInstance->GetData(TYPE_EVENT_NPC) == NPC_LANATHEL_INTRO)
        {
            UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
            if (UpdateTimer <= diff)
            {
                debug_log("EventMGR: creature %u received signal %u ",m_creature->GetEntry(),pInstance->GetData(TYPE_EVENT));
                switch (pInstance->GetData(TYPE_EVENT))
                {
                    case 800:
                          m_creature->SetVisibility(VISIBILITY_ON);
                          DoScriptText(-1631301, m_creature);
                          UpdateTimer = 15000;
                          pInstance->SetData(TYPE_EVENT,810);
                          break;
                    case 810:
                          DoScriptText(-1631311, m_creature);
                          if (Creature* pPrince = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_TALDARAM)))
                              doRemove(SPELL_FAKE_DEATH, pPrince);

                          if (Creature* pPrince = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_KELESETH)))
                              doRemove(SPELL_FAKE_DEATH, pPrince);

                          if (Creature* pPrince = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_VALANAR)))
                              doRemove(SPELL_FAKE_DEATH, pPrince);

                          UpdateTimer = 5000;
                          pInstance->SetData(TYPE_EVENT,820);
                          break;
                    case 820:
                          m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                          m_creature->SetVisibility(VISIBILITY_OFF);
                          UpdateTimer = 2000;
                          pInstance->SetData(TYPE_EVENT,830);
                          break;
                    default:
                          break;
                }
            }
            else
                UpdateTimer -= diff;
            pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
        }
    }
};

CreatureAI* GetAI_boss_blood_queen_lanathel_intro(Creature* pCreature)
{
    return new boss_blood_queen_lanathel_introAI(pCreature);
}

void AddSC_blood_prince_council()
{
    Script* newscript;

    newscript = new Script;
    newscript->Name = "boss_taldaram_icc";
    newscript->GetAI = &GetAI_boss_taldaram_icc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_keleseth_icc";
    newscript->GetAI = &GetAI_boss_keleseth_icc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_valanar_icc";
    newscript->GetAI = &GetAI_boss_valanar_icc;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_nucleus";
    newscript->GetAI = &GetAI_mob_dark_nucleus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ball_of_flames";
    newscript->GetAI = &GetAI_mob_ball_of_flames;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_kinetic_bomb";
    newscript->GetAI = &GetAI_mob_kinetic_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shock_vortex";
    newscript->GetAI = &GetAI_mob_shock_vortex;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_kinetic_bomb_target";
    newscript->GetAI = &GetAI_mob_kinetic_bomb_target;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_blood_queen_lanathel_intro";
    newscript->GetAI = &GetAI_boss_blood_queen_lanathel_intro;
    newscript->RegisterSelf();
}
