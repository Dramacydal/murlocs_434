/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_anraphet
SD%Complete: 10
SDComment: Placeholder
SDCategory: Halls of Origination
EndScriptData */

#include "precompiled.h"
#include "halls_of_origination.h"

enum
{
    // ToDo: add spells and yells here
};

struct MANGOS_DLL_DECL boss_anraphetAI : public ScriptedAI
{
    boss_anraphetAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset() override
    {
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANRAPHET, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANRAPHET, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ANRAPHET, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_anraphet(Creature* pCreature)
{
    return new boss_anraphetAI(pCreature);
}

struct MANGOS_DLL_DECL boss_wardenAI : public ScriptedAI
{
    boss_wardenAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();

        type = MAX_ENCOUNTER;

        switch (pCreature->GetEntry())
        {
            case NPC_EARTH_WARDEN: type = TYPE_EARTH_WARDEN; break;
            case NPC_WATER_WARDEN: type = TYPE_WATER_WARDEN; break;
            case NPC_AIR_WARDEN: type = TYPE_AIR_WARDEN; break;
            case NPC_FLAME_WARDEN: type = TYPE_FLAME_WARDEN; break;
        }
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 type;

    void Reset() override
    {
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(type, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(type, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(type, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_warden(Creature* pCreature)
{
    return new boss_wardenAI(pCreature);
}

void AddSC_boss_anraphet()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_anraphet";
    pNewScript->GetAI = &GetAI_boss_anraphet;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_warden";
    pNewScript->GetAI = &GetAI_boss_warden;
    pNewScript->RegisterSelf();
}
