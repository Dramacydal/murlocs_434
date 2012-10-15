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
SDName: boss_ignis
SD%Complete:
SDComment: slag pot damage missing
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"
#include "Vehicle.h"

enum
{
    //yells
    SAY_AGGRO       = -1603020,
    SAY_SCORCH1     = -1603021,
    SAY_SCORCH2     = -1603022,
    SAY_SLAGPOT     = -1603023,
    EMOTE_FLAMEJETS = -1603024,
    SAY_SUMMON      = -1603025,
    SAY_SLAY_1      = -1603026,
    SAY_SLAY_2      = -1603027,
    SAY_BERSERK     = -1603028,
    SAY_DEATH       = -1603029,

    //ignis the furnace master
    SPELL_FLAME_JETS            = 62680,
    SPELL_FLAME_JETS_H          = 63472,
    SPELL_SCORCH_CHANNELED      = 62546,
    SPELL_SCORCH_CHANNELED_H    = 63474,
    SPELL_CHARGE_SLAG_POT       = 62707,
    SPELL_CHARGE_SLAG_POT_H     = 63535,
    SPELL_SLAG_POT_AURA         = 62717,
    SPELL_SLAG_POT_AURA_H       = 63477,

    SPELL_SLAG_POT_DMG          = 65722,
    SPELL_SLAG_POT_DMG_H        = 65723,

    SPELL_HASTE                 = 62836,

    // Scorch trigger
    SPELL_SCORCH_AURA           = 62548,
    SPELL_SCORCH_AURA_H         = 63476,


    //iron construct
    //NPC_IRON_CONSTRUCT        = 33121,
    SPELL_STONED                = 62468,
    SPELL_HEAT_AURA             = 65667,
    SPELL_MOLTEN                = 62373,
    SPELL_BRITTLE               = 67114,
    SPELL_BRITTLE_H             = 62382,
    SPELL_SHATTER               = 62383,
    SPELL_ACTIVATE_CONSTRUCT    = 62488,
    SPELL_STRENGTH_OF_THE_CREATOR = 64474,
    SPELL_STRENGHT_OF_CREATOR_DIE= 64475,


    //NPC ids
    SPELL_SUMMON_SCORCH_TRIGGER = 62551,
    SPELL_SLAG_IMBUED           = 62836,
    SPELL_SLAG_IMBUED2          = 63536,
    SPELL_MOLTEN_STUN           = 65208, // couldn't find proper spell, so using this one
    SCORCH_DESPAWN_TIME         = 40000,
};

// iron construct
struct MANGOS_DLL_DECL mob_iron_constructAI : public ScriptedAI
{
    mob_iron_constructAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();

        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsStoned;
    bool m_bIsActive;
    uint32 m_uiBrittleCheckTimer;

    void Reset()
    {
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_bIsStoned = false;
        m_bIsActive = false;
        m_uiBrittleCheckTimer = 500;
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_bIsActive)
        {
            uiDamage = 0;
            return;
        }

        if (m_creature->HasAura(SPELL_BRITTLE) || m_creature->HasAura(SPELL_BRITTLE_H))
        {
            if (uiDamage > (m_bIsRegularMode ? 3000 : 5000))
            {
                uiDamage = 0;
                if (DoCastSpellIfCan(m_creature, SPELL_SHATTER, CAST_TRIGGERED) == CAST_OK)
                {
                    m_creature->ForcedDespawn(1000);
                    m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                }
            }
        }
    }

    void JustDied(Unit* killer)
    {
        DoCast(m_creature, SPELL_STRENGHT_OF_CREATOR_DIE, true);
    }

    void SpellHit(Unit* pCaster, const SpellEntry *spellInfo)
    {
        if (spellInfo->Id == SPELL_ACTIVATE_CONSTRUCT)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_STONED);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            m_bIsActive = true;
            m_creature->SetInCombatWithZone();
            
            DoCast(pCaster, SPELL_STRENGTH_OF_THE_CREATOR, true);
        }
        else if (spellInfo->Id == SPELL_HEAT_AURA)
        {
            if (Aura* pHeat = m_creature->GetAura(SPELL_HEAT_AURA, EFFECT_INDEX_1))
            {
                if (pHeat->GetStackAmount() > 9)
                {
                    m_creature->RemoveAurasDueToSpell(SPELL_HEAT_AURA);
                    DoCastSpellIfCan(m_creature, SPELL_MOLTEN, CAST_TRIGGERED);
                    DoCastSpellIfCan(m_creature, SPELL_MOLTEN_STUN, CAST_TRIGGERED);
                }
            }
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_bIsStoned)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_STONED) == CAST_OK)
                m_bIsStoned = true;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim() )
            return;

        if (!m_bIsActive)
            m_creature->AI()->EnterEvadeMode();

        if (m_uiBrittleCheckTimer <= uiDiff)
        {
            if (m_creature->HasAura(SPELL_MOLTEN) && !m_creature->HasAura(SPELL_BRITTLE) )
            {
                if (m_creature->IsInWater())
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_BRITTLE : SPELL_BRITTLE_H, CAST_TRIGGERED) == CAST_OK)
                        m_creature->RemoveAurasDueToSpell(SPELL_MOLTEN);
                }
            }
            m_uiBrittleCheckTimer = 500;
        }
        else
            m_uiBrittleCheckTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_iron_construct(Creature* pCreature)
{
    return new mob_iron_constructAI(pCreature);
}

//ignis the furnace master
struct MANGOS_DLL_DECL boss_ignisAI : public ScriptedAI
{
    boss_ignisAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;
    bool m_bIsSlagPot;

    uint32 m_uiScorchTimer;
    uint32 m_uiSlagPotTimer;
    uint32 m_uiSlagPotSwitchTimer;
    uint32 m_uiSlagPotExitTimer;
    uint32 m_uiSlagPotDmgTimer;
    uint32 m_uiFlameJetsTimer;
    uint32 m_uiActivateConstructTimer;

    uint32 m_uiBerserkerTimer;

    void Reset()
    {
        m_uiScorchTimer = 12000;
        m_uiSlagPotTimer = 19000;
        m_uiFlameJetsTimer = 21000;
        m_uiActivateConstructTimer = 25000;

        m_uiBerserkerTimer = 600000; // 10 Minutes

        m_bIsSlagPot = false;
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS , IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, FAIL);
        if (VehicleKit *pVehKit = m_creature->GetVehicleKit())
            pVehKit->RemoveAllPassengers();
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_IGNIS, DONE);
    }

    void JustSummoned(Creature* pCreature)
    {
        if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SCORCH_CHANNELED : SPELL_SCORCH_CHANNELED_H) == CAST_OK && !pCreature->IsInWater())
        {
            m_creature->AddThreat(pCreature, 100000000000000.0f, true);
            m_creature->SetFacingToObject(pCreature);
            pCreature->SetDisplayId(11686);
            pCreature->SetInCombatWithZone();
            pCreature->ForcedDespawn(SCORCH_DESPAWN_TIME);
        }
        else
            pCreature->ForcedDespawn();
    }

    void DamageTaken(Unit *pDoneBy, uint32 &uiDamage)
    {
        if (pDoneBy->HasAura(SPELL_SLAG_POT_AURA) || pDoneBy->HasAura(SPELL_SLAG_POT_AURA_H))
            uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiScorchTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_SCORCH_TRIGGER, CAST_TRIGGERED) == CAST_OK)
            {
                DoScriptText(urand(0,1) ? SAY_SCORCH1 : SAY_SCORCH2, m_creature);
                m_uiScorchTimer = urand(20000, 25000);
            }
        }
        else
            m_uiScorchTimer -= uiDiff;

        if (m_uiSlagPotTimer <= uiDiff)
        {
            if (Unit* pVictim = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 1))
                if (Player *pTarget = pVictim->GetCharmerOrOwnerPlayerOrPlayerItself())
                    if (DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_CHARGE_SLAG_POT : SPELL_CHARGE_SLAG_POT_H) == CAST_OK)
                    {
                        DoScriptText(SAY_SLAGPOT, m_creature);
                        pTarget->EnterVehicle(m_creature->GetVehicleKit(), 0);
                        pTarget->CastSpell(pTarget, m_bIsRegularMode ? SPELL_SLAG_POT_AURA : SPELL_SLAG_POT_AURA_H, true, 0,0, m_creature->GetObjectGuid());
                        m_bIsSlagPot = true;
                        m_uiSlagPotSwitchTimer = 1500;
                        m_uiSlagPotDmgTimer = 2000;
                        m_uiSlagPotExitTimer = 10000;
                        m_uiSlagPotTimer = urand(15000, 25000);
                    }
        }
        else
            m_uiSlagPotTimer -= uiDiff;

        if (m_bIsSlagPot)
        {
            if (m_uiSlagPotSwitchTimer <= uiDiff)
            {
                if (VehicleKit *pVehKit = m_creature->GetVehicleKit())
                    if (Unit *pPassenger = pVehKit->GetPassenger(0))
                    {
                        pVehKit->RemovePassenger(pPassenger);
                        pVehKit->AddPassenger(pPassenger, 1);
                    }
                m_uiSlagPotSwitchTimer = 10000;
                m_uiSlagPotDmgTimer = 500;
            }
            else
                m_uiSlagPotSwitchTimer -= uiDiff;

            if (m_uiSlagPotDmgTimer <= uiDiff)
            {
                if (VehicleKit *pVehKit = m_creature->GetVehicleKit())
                    if (Unit *pPassenger = pVehKit->GetPassenger(1))
                    {
                        pPassenger->CastSpell(pPassenger, m_bIsRegularMode ? SPELL_SLAG_POT_DMG : SPELL_SLAG_POT_DMG_H, true ,0 ,0, m_creature->GetObjectGuid());
                    }
                m_uiSlagPotDmgTimer = 3000;
            }
            else
                m_uiSlagPotDmgTimer -= uiDiff;

            if (m_uiSlagPotExitTimer <= uiDiff)
            {
                if (VehicleKit *pVehKit = m_creature->GetVehicleKit())
                    pVehKit->RemoveAllPassengers();
                m_uiSlagPotExitTimer = 10000;
                m_bIsSlagPot = false;
            }
            else
                m_uiSlagPotExitTimer -= uiDiff;
        }

        if (m_uiFlameJetsTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_JETS : SPELL_FLAME_JETS_H) == CAST_OK)
            {
                DoScriptText(EMOTE_FLAMEJETS, m_creature);
                m_uiFlameJetsTimer = urand(20000, 25000);
            }
        }
        else
            m_uiFlameJetsTimer -= uiDiff;

        if (m_uiActivateConstructTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_ACTIVATE_CONSTRUCT) == CAST_OK)
            {
                DoScriptText(SAY_SUMMON, m_creature);
                m_uiActivateConstructTimer = m_bIsRegularMode ? 30000 : 40000;
            }
        }
        else
            m_uiActivateConstructTimer -= uiDiff;

        if (m_uiBerserkerTimer <= uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, 26662, CAST_AURA_NOT_PRESENT | CAST_TRIGGERED) == CAST_OK)
            {
                DoScriptText(SAY_BERSERK, m_creature);
            }
        }
        else
            m_uiBerserkerTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_ignis(Creature* pCreature)
{
    return new boss_ignisAI(pCreature);
}

void AddSC_boss_ignis()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_ignis";
    NewScript->GetAI = GetAI_boss_ignis;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_iron_construct";
    NewScript->GetAI = &GetAI_mob_iron_construct;
    NewScript->RegisterSelf();
}
