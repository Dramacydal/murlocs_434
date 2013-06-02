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
SDName: boss_alizabal
SD%Complete: 10
SDComment: Placeholder
SDCategory: Baradin Hold
EndScriptData */

#include "precompiled.h"
#include "baradin_hold.h"

enum ScriptTexts
{
    SAY_INTRO               = -1002011,

    SAY_SEETHING_HATE_1     = -1002012,
    SAY_SEETHING_HATE_2     = -1002013,
    SAY_SEETHING_HATE_3     = -1002014,
    SAY_SEETHING_HATE_4     = -1002015,
    SAY_SEETHING_HATE_5     = -1002016,

    SAY_SKEWER_1            = -1002017,
    SAY_SKEWER_2            = -1002018,

    SAY_KILL_1              = -1002019,
    SAY_KILL_2              = -1002020,
    SAY_KILL_3              = -1002021,
    SAY_KILL_4              = -1002022,

    SAY_WIPE                = -1002023,

    SAY_DEATH               = -1002024,

    SAY_AGGRO               = -1002025,
};

enum Spells
{
    SPELL_SKEWER                = 104936,
    SPELL_SEETHING_HATE_DUMMY   = 105065,
    SPELL_SEETHING_HATE         = 105067,
    SPELL_SEETHING_HATE_DMG_10  = 105069,
    SPELL_SEETHING_HATE_DMG_25  = 108094,
    SPELL_BLADE_DANCE_CHARGE    = 105726,
    SPELL_BLADE_DANCE_DUMMY     = 106248,
    SPELL_BLADE_DANCE_SELF      = 105828,
    SPELL_BLADE_DANCE           = 105784,
    SPELL_BERSERK               = 47008,
};

enum Events
{
    EVENT_SKEWER                = 1,
    EVENT_SEETHING_HATE         = 2,
    EVENT_BLADE_DANCE           = 3,
    EVENT_BERSERK               = 4,
    EVENT_BLADE_DANCE_CHARGE    = 5,
};

struct MANGOS_DLL_DECL boss_alizabalAI : public ScriptedAI
{
    boss_alizabalAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
        introDone = false;
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool introDone;
    uint8 uiCharges;
    EventMap events;

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who->GetTypeId() == TYPEID_PLAYER && !((Player*)who)->isGameMaster() && !introDone && me->IsWithinDistInMap(who, 70.0f))
        {
            DoScriptText(SAY_INTRO, me);
            introDone = true;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALIZABAL, IN_PROGRESS);

        uiCharges = 0;
        events.ScheduleEvent(EVENT_BERSERK, 300000);
        events.ScheduleEvent(EVENT_BLADE_DANCE, 35000);
        if (urand(0, 1))
        {
            events.ScheduleEvent(EVENT_SKEWER, 8000);
            events.ScheduleEvent(EVENT_SEETHING_HATE, 16000);
        }
        else
        {
            events.ScheduleEvent(EVENT_SKEWER, 16000);
            events.ScheduleEvent(EVENT_SEETHING_HATE, 8000);
        }

        DoScriptText(SAY_AGGRO, me);
    }

    void JustDied(Unit* pKiller) override
    {
        DoScriptText(SAY_DEATH, me);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALIZABAL, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        DoScriptText(RAND(SAY_KILL_1, SAY_KILL_2, SAY_KILL_3, SAY_KILL_4), me);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ALIZABAL, FAIL);

        DoScriptText(SAY_WIPE, me);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        events.Update(uiDiff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_BERSERK:
                    DoCast(me, SPELL_BERSERK, true);
                    break;
                case EVENT_SKEWER:
                    DoScriptText(RAND(SAY_SKEWER_1, SAY_SKEWER_2), me);
                    DoCast(me->getVictim(), SPELL_SKEWER);
                    events.ScheduleEvent(EVENT_SKEWER, 20500);
                    break;
                case EVENT_SEETHING_HATE:
                    DoScriptText(RAND(SAY_SEETHING_HATE_1, SAY_SEETHING_HATE_2, SAY_SEETHING_HATE_3, SAY_SEETHING_HATE_4, SAY_SEETHING_HATE_5), me);
                    DoCastAOE(SPELL_SEETHING_HATE_DUMMY);
                    events.ScheduleEvent(EVENT_SEETHING_HATE, 20500);
                    break;
                case EVENT_BLADE_DANCE:
                    events.CancelEvent(EVENT_SKEWER);
                    events.CancelEvent(EVENT_SEETHING_HATE);
                    DoCast(me, SPELL_BLADE_DANCE_SELF);
                    DoCast(me, SPELL_BLADE_DANCE, true);
                    DoCastAOE(SPELL_BLADE_DANCE_DUMMY, true);
                    events.ScheduleEvent(EVENT_BLADE_DANCE, 60000);
                    events.ScheduleEvent(EVENT_BLADE_DANCE_CHARGE, 4000);
                    if (urand(0, 1))
                    {
                        events.RescheduleEvent(EVENT_SKEWER, 23000);
                        events.RescheduleEvent(EVENT_SEETHING_HATE, 31000);
                    }
                    else
                    {
                        events.RescheduleEvent(EVENT_SKEWER, 31000);
                        events.RescheduleEvent(EVENT_SEETHING_HATE, 23000);
                    }
                    break;
                case EVENT_BLADE_DANCE_CHARGE:
                    events.CancelEvent(EVENT_BLADE_DANCE);
                    DoCastAOE(SPELL_BLADE_DANCE_DUMMY);
                    uiCharges++;
                    if (uiCharges > 3)
                        uiCharges = 0;
                    else
                        events.RescheduleEvent(EVENT_BLADE_DANCE, 4000);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_alizabal(Creature* pCreature)
{
    return new boss_alizabalAI(pCreature);
}

void AddSC_boss_alizabal()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_alizabal";
    pNewScript->GetAI = &GetAI_boss_alizabal;
    pNewScript->RegisterSelf();
}
