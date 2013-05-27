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
SDName: boss_mannoroth_and_varothen
SD%Complete: 0
SDComment: Placeholder
SDCategory: Well of Eternity
EndScriptData */

#include "precompiled.h"
#include "well_of_eternity.h";

enum
{
    // ToDo: add spells and yells here
};

struct MANGOS_DLL_DECL boss_mannorothAI : public ScriptedAI
{
    boss_mannorothAI(Creature* pCreature) : ScriptedAI(pCreature)
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
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MANNOROTH, IN_PROGRESS);
    }

    void DamageTaken(Unit* who, uint32& damage) override
    {
        damage *= 5;
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MANNOROTH, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_MANNOROTH, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_mannoroth(Creature* pCreature)
{
    return new boss_mannorothAI(pCreature);
}

void AddSC_boss_mannoroth_and_varothen()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_mannoroth";
    pNewScript->GetAI = &GetAI_boss_mannoroth;
    pNewScript->RegisterSelf();
}
