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
SDName: boss_archbishop_benedictus
SD%Complete: 0
SDComment: Placeholder
SDCategory: Hour of Twilight
EndScriptData */

#include "precompiled.h"
#include "hour_of_twilight.h";

enum
{
    // ToDo: add spells and yells here
    SAY_DEATH                   = -1002000,
    SAY_TRANSFORMATION          = -1002009,
    SAY_KILL                    = -1002010,
    SAY_AGGRO                   = -1002006,

    SPELL_TWILIGHT_EPITAPHY     = 103754,
    SPELL_TWILIGHT_EPITAPHY_2   = 103755,
};

struct MANGOS_DLL_DECL boss_archbishop_benedictusAI : public ScriptedAI
{
    boss_archbishop_benedictusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool transformed;
    EventMap events;

    void Reset() override
    {
        transformed = false;
        m_creature->SetDisplayId(m_creature->GetNativeDisplayId());
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BENEDICTUS, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BENEDICTUS, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void KilledUnit(Unit* pVictim) override
    {
        DoScriptText(SAY_KILL, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BENEDICTUS, FAIL);
    }

    void SpellHitTarget(Unit* target, SpellEntry const* spell) override
    {
        if (!transformed && spell->Id == SPELL_TWILIGHT_EPITAPHY_2)
        {
            transformed = true;
            m_creature->SetDisplayId(38992);
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        if (!transformed && m_creature->GetHealthPercent() < 60.0f)
        {
            DoScriptText(SAY_TRANSFORMATION, m_creature);
            DoCast(m_creature, SPELL_TWILIGHT_EPITAPHY);
            return;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_archbishop_benedictus(Creature* pCreature)
{
    return new boss_archbishop_benedictusAI(pCreature);
}

void AddSC_boss_archbishop_benedictus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_archbishop_benedictus";
    pNewScript->GetAI = &GetAI_boss_archbishop_benedictus;
    pNewScript->RegisterSelf();
}
