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
SDName: pet_scripts
SD%Complete:
SDComment: To be used for pets.
SDCategory: Pet's
EndScriptData
*/

#include "precompiled.h"
#include "pet_ai.h"

/* ContentData
pet_greater_fire_elemental       ?%    support for shaman pet Greater fire elemental
pet_greater_earth_elemental      ?%    support for shaman pet Greater earth elemental
EndContentData */

enum greater_fire_elemental_spells
{
    SPELL_FIRE_RING       = 12470,
    SPELL_FIRE_EXPLODE    = 57984,
};

struct MANGOS_DLL_DECL pet_greater_fire_elemental : public ScriptedPetAI
{
    pet_greater_fire_elemental(Creature* pCreature) : ScriptedPetAI(pCreature) 
    {
        Reset();
    }

    uint32 m_uiFireRingTimer;
    uint32 m_uiFireExplodeTimer;
    Pet* m_pet;

    void Reset()
    {
        m_pet = (Pet*)m_creature;
        m_pet->GetCharmInfo()->SetReactState(REACT_AGGRESSIVE);
        m_pet->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        m_uiFireRingTimer    = urand(3000,9000);
        m_uiFireExplodeTimer = urand(4000,8000);

        if(m_pet->GetOwner()->IsPvP())
            m_pet->SetPvP(true);
        if(m_pet->GetOwner()->IsFFAPvP())
            m_pet->SetFFAPvP(true);
    }

    void UpdatePetAI(const uint32 diff)
    {
        if (!m_pet->GetOwner() || !m_pet->GetOwner()->isAlive())
            m_pet->ForcedDespawn();

        if (m_pet->GetOwner()->getVictim() && m_pet->getVictim() != m_pet->GetOwner()->getVictim())
            AttackStart(m_pet->GetOwner()->getVictim());

        if (m_uiFireRingTimer <= diff)
        {
            if (m_pet->IsWithinDistInMap(m_pet->getVictim(),ATTACK_DISTANCE))
                DoCastSpellIfCan(m_pet,SPELL_FIRE_RING);
            m_uiFireRingTimer = urand(4000,12000);
        }
        else m_uiFireRingTimer -= diff;

        if (m_uiFireExplodeTimer <= diff)
        {
            DoCastSpellIfCan(m_pet->getVictim(),SPELL_FIRE_EXPLODE);
            m_uiFireExplodeTimer = urand(4000,8000);
        } else m_uiFireExplodeTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void UpdatePetOOCAI(const uint32 diff)
    {
        if (!m_pet->GetOwner() || !m_pet->GetOwner()->isAlive())
            m_pet->ForcedDespawn();

        if (m_pet->GetOwner() && m_pet->GetOwner()->getVictim())
        {
            AttackStart(m_pet->GetOwner()->getVictim());
        }
        else if (m_pet->GetOwner()->isInCombat())
        {
            AttackStart(m_pet->GetOwner()->getAttackerForHelper());
        }
    }
};

CreatureAI* GetAI_pet_greater_fire_elemental(Creature* pCreature)
{
    return new pet_greater_fire_elemental(pCreature);
}

enum greater_earth_elemental_spells
{
    SPELL_EARTH_THREAT    = 36213,
    SPELL_EARTH_EXPLODE   = 12374,
    SPELL_EARTH_AOE       = 33840,
};

struct MANGOS_DLL_DECL pet_greater_earth_elemental : public ScriptedPetAI
{
    pet_greater_earth_elemental(Creature* pCreature) : ScriptedPetAI(pCreature) 
    {
        Reset();
    }

    uint32 m_uiEarthThreatTimer;
    uint32 m_uiEarthAOETimer;
    uint32 m_uiEarthAttackTimer;
    Pet* m_pet;

    void Reset()
    {
        m_pet = (Pet*)m_creature;
        m_pet->GetCharmInfo()->SetReactState(REACT_AGGRESSIVE);
        m_pet->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        m_uiEarthThreatTimer    = urand(5000,12000);
        m_uiEarthAOETimer       = urand(10000,30000);
        m_uiEarthAttackTimer    = urand(2000,8000);

        if(m_pet->GetOwner()->IsPvP())
            m_pet->SetPvP(true);
        if(m_pet->GetOwner()->IsFFAPvP())
            m_pet->SetFFAPvP(true);
    }

    void UpdatePetAI(const uint32 diff)
    {
        if (!m_pet->GetOwner() || !m_pet->GetOwner()->isAlive())
            m_pet->ForcedDespawn();

        if (m_pet->GetOwner()->getVictim() && m_pet->getVictim() != m_pet->GetOwner()->getVictim())
            AttackStart(m_pet->GetOwner()->getVictim());

        if (m_uiEarthThreatTimer <= diff)
        {
            DoCastSpellIfCan(m_pet,SPELL_EARTH_THREAT);
            m_uiEarthThreatTimer = urand(4000,12000);
        }
        else m_uiEarthThreatTimer -= diff;

        if (m_uiEarthAOETimer <= diff)
        {
            if (m_pet->IsWithinDistInMap(m_pet->getVictim(),ATTACK_DISTANCE))
                DoCastSpellIfCan(m_pet,SPELL_EARTH_AOE);
            m_uiEarthAOETimer = urand(10000,30000);
        }
        else m_uiEarthAOETimer -= diff;

        if (m_uiEarthAttackTimer <= diff)
        {
            DoCastSpellIfCan(m_pet->getVictim(),SPELL_EARTH_EXPLODE);
            m_uiEarthAttackTimer = urand(2000,8000);
        }
        else m_uiEarthAttackTimer -= diff;

        DoMeleeAttackIfReady();
    }

    void UpdatePetOOCAI(const uint32 diff)
    {
        if (!m_pet->GetOwner() || !m_pet->GetOwner()->isAlive())
            m_pet->ForcedDespawn();

        if (m_pet->GetOwner() && m_pet->GetOwner()->getVictim())
        {
            AttackStart(m_pet->GetOwner()->getVictim());
        }
        else if (m_pet->GetOwner()->isInCombat())
        {
            AttackStart(m_pet->GetOwner()->getAttackerForHelper());
        }
    }
};

CreatureAI* GetAI_pet_greater_earth_elemental(Creature* pCreature)
{
    return new pet_greater_earth_elemental(pCreature);
}

enum pet_soul_trader_beacon_auras_and_spells
{
    //ETHEREAL_PET_AURA                          = 50051,
    SPELL_ETHEREAL_PET_ONKILL_STEAL_ESSENCE    = 50101,
    SPELL_ETHEREAL_PET_ONKILL_GIVE_TOKEN       = 50067,
};

struct MANGOS_DLL_DECL pet_soul_trader_beacon : public ScriptedPetAI
{
    pet_soul_trader_beacon(Creature* pCreature) : ScriptedPetAI(pCreature) 
    {
        Reset();
    }

    Pet* m_pet;
    Unit* owner;

    void Reset()
    {
        m_pet = (Pet*)m_creature;
        owner = (Player*)m_pet->GetOwner();
    }

    void UpdatePetOOCAI(const uint32 diff)
    {
        if (!owner || !owner->IsInWorld() || !owner->isAlive() || !owner->GetMap() || !m_pet->IsWithinDistInMap(owner, 100.0f))
            m_pet->ForcedDespawn();
    }

    void OwnerKilledUnit(Unit* pVictim)
    {
        if (!owner || !owner->IsInWorld() || !owner->isAlive() || !owner->GetMap())
        {
            m_pet->ForcedDespawn();
            return;
        }

        if (pVictim->GetTypeId() == TYPEID_UNIT)
        {
            // lvl diff
            if (pVictim->getLevel() + 8 < owner->getLevel())
                return;

            // Is distance expected?
            if (m_pet->IsWithinDistInMap(pVictim, 30.0f))
            {
                m_pet->CastSpell(pVictim, SPELL_ETHEREAL_PET_ONKILL_STEAL_ESSENCE, true);
                m_pet->CastSpell(m_pet, SPELL_ETHEREAL_PET_ONKILL_GIVE_TOKEN, true);
            }
        }
    }

};

CreatureAI* GetAI_pet_soul_trader_beacon(Creature* pCreature)
{
    return new pet_soul_trader_beacon(pCreature);
}

bool GossipHello_pet_soul_trader_beacon(Player* pPlayer, Creature* pCreature)
{
    pPlayer->PlayerTalkClass->ClearMenus();

    if (pPlayer->GetObjectGuid() != pCreature->GetOwnerGuid())
        return false;

    if (!pPlayer->getAttackers().empty())
        return false;

    if (pCreature->isVendor())
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, GOSSIP_TEXT_BROWSE_GOODS, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_TRADE);
        pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    }

    return true;
}

bool GossipSelect_pet_soul_trader_beacon(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_TRADE)
        pPlayer->SEND_VENDORLIST(pCreature->GetObjectGuid());

    return true;
}

void AddSC_pet_scripts()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "pet_greater_fire_elemental";
    pNewScript->GetAI = &GetAI_pet_greater_fire_elemental;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "pet_greater_earth_elemental";
    pNewScript->GetAI = &GetAI_pet_greater_earth_elemental;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "pet_soul_trader_beacon";
    pNewScript->GetAI = &GetAI_pet_soul_trader_beacon;
    pNewScript->pGossipHello = &GossipHello_pet_soul_trader_beacon;
    pNewScript->pGossipSelect = &GossipSelect_pet_soul_trader_beacon;
    pNewScript->RegisterSelf();
}
