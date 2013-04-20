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
SDName: boss_lord_rhyolith
SD%Complete: 10
SDComment: Placeholder
SDCategory: Firelands
EndScriptData */

#include "precompiled.h"
#include "firelands.h"

enum
{
    // ToDo: add spells and yells here
};

enum KarEvents
{
    EVENT_MOVE_NEXT_POS = 1,
};

enum KarSpells
{
    SUMMON_ELEMENTALS = 99601,
};

struct kar_the_everburningAI : public ScriptedAI
{
    kar_the_everburningAI(Creature* creature) : ScriptedAI(creature)
    {
        m_pInstance = (ScriptedInstance*)creature->GetInstanceData();
        m_bIsRegularMode = creature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    uint8 phase;
    uint32 timerMove,timerElem;

    void Reset() override
    {
        phase = 0;
        timerElem = 1700;
        //me->SetReactState(REACT_PASSIVE);
    }

    void EnterCombat(Unit* /*who*/) override
    {
    }

    void UpdateAI(const uint32 diff) override
    {
        if (phase == 2 && UpdateVictim())
        {
            events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                    case EVENT_MOVE_NEXT_POS:
                        break;
                }
            }
        }

        if (phase == 0)
        {
            if (Unit* NearPlayer = GetClosestAttackableUnit(me, 5.0f))
            {
                phase = 1;
                timerMove = 7000;
            }

            if (timerElem <= diff)
            {
                me->CastSpell(me->GetPositionX() + urand(0,10) + urand(-10,0),me->GetPositionY() + urand(0,10) + urand(-10,0), me->GetPositionZ(),SUMMON_ELEMENTALS,true);
                timerElem = 1200;
            }
            else timerElem -= diff;
        }

        if (phase == 1)
        {
            if (timerMove <= diff)
            {
                phase = 2;
                me->SetFacingTo(0.931584f);
                //me->SetReactState(REACT_AGGRESSIVE);
                me->SetRespawnCoord(me->GetPositionX(),me->GetPositionY(),me->GetPositionZ(),me->GetOrientation());
                Reset();
            }
            else timerMove -= diff;
        }

        if (phase == 1)
            me->GetMotionMaster()->MovePoint(0, -316.224f, -435.897f, 102.951f);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_kar_the_everburning(Creature* pCreature)
{
    return new kar_the_everburningAI(pCreature);
}

////

struct MANGOS_DLL_DECL Unstable_PyrelordAI : public ScriptedAI
{
    Unstable_PyrelordAI(Creature* creature) : ScriptedAI(creature)
    {
        m_pInstance = (ScriptedInstance*)creature->GetInstanceData();
        m_bIsRegularMode = creature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    bool start;
    uint32 falltimer;
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset() override
    {
        start = false;
        falltimer = 5000;
    }

    void EnterCombat(Unit* /*who*/) override
    {
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!start)
        {
            //me->SetReactState(REACT_AGGRESSIVE);
            start = true;
            return;
        }

        if (falltimer <= diff)
        {
            me->GetMotionMaster()->MoveFall();
            falltimer = 5000;
        }
        else falltimer -= diff;

        if (!UpdateVictim())
            me->GetMotionMaster()->MovePoint(1,-167.431f,-307.385f,57.370f);

        if (!GetKar())
            me->ForcedDespawn();

        if (UpdateVictim())
            DoMeleeAttackIfReady();
    }

    Creature* GetKar()
    {
        return GetClosestCreatureWithEntry(me, 53616, 125.0f, true);
    }
};

struct MANGOS_DLL_DECL boss_lord_rhyolithAI : public ScriptedAI
{
    boss_lord_rhyolithAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset() override
    {
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RHYOLITH, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RHYOLITH, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RHYOLITH, FAIL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lord_rhyolith(Creature* pCreature)
{
    return new boss_lord_rhyolithAI(pCreature);
}

void AddSC_boss_lord_rhyolith()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lord_rhyolith";
    pNewScript->GetAI = &GetAI_boss_lord_rhyolith;
    pNewScript->RegisterSelf();
}
