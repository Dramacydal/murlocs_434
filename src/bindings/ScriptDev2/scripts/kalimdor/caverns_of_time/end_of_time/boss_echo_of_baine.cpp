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
SDName: boss_echo_of_baine
SD%Complete: 0
SDComment: Placeholder
SDCategory: End of Time
EndScriptData */

#include "precompiled.h"
#include "end_of_time.h"

enum Yells
{
    SAY_INTRO = 1,
    SAY_AGGRO,
    SAY_DEATH,
    SAY_KILL_1,
    SAY_KILL_2,
    SAY_KILL_3,
    SAY_THROW_TOTEM,
    SAY_PULVERIZE,
};

enum
{
    SPELL_BAINE_VIS   = 101624, // Visuals on boss (the totems on the back etc.)

    SPELL_THROW_TOTEM = 101615, // Triggers missile at location, with summon totem and kb.
    SPELL_PULVERIZE_J = 101626, // Jump b. target, activate platform.
    SPELL_PULVERIZE_D = 101627, // Damage spell.
    SPELL_PULV_DBM    = 101625, // DBM says this, fuck it.
    SPELL_MOLTEN_AXE  = 101836, // Extra damage on melee attack, change targets from caster to target. When he falls into lava after Pulverize.
    SPELL_MOLTEN_FIST = 101866, // Extra dmg on melee for players when they touch the lava, they get this when baine gets Molten Axe.

    SPELL_TB_TOTEM    = 101602, // Throw totem back at Baine on click.
    SPELL_TB_TOTEM_A  = 107837, // Visual aura: player has totem to throw.
};

enum Events
{
    EVENT_PULVERIZE = 1,
    EVENT_PULVERIZE_DAMAGE,
    EVENT_THROW_TOTEM,
    EVENT_LAVA_CHECK,
};

enum Creatures
{
    NPC_ROCK_ISLAND = 54496,
};

struct MANGOS_DLL_DECL boss_echo_of_baineAI : public ScriptedAI
{
    boss_echo_of_baineAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool introDone;
    Unit* pulverizeTarget;
    EventMap events;

    void Reset() override
    {
        introDone = false;
        pulverizeTarget = NULL;

        events.Reset();

        if (!me->HasAura(SPELL_BAINE_VIS))
            DoCast(me, SPELL_BAINE_VIS);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!introDone && me->IsWithinDistInMap(who, 40.0f, false))
        {
            introDone = true;
            //Talk(SAY_INTRO);
            return;
        }

        ScriptedAI::MoveInLineOfSight(who);
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BAINE, IN_PROGRESS);

        //Talk(SAY_AGGRO);

        events.ScheduleEvent(EVENT_PULVERIZE, 40000);
        events.ScheduleEvent(EVENT_THROW_TOTEM, 25000);
        events.ScheduleEvent(EVENT_LAVA_CHECK, 1000);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BAINE, DONE);
    }

    void KilledUnit(Unit* pVictim) override
    {
        //Talk(RAND(SAY_KILL_1, SAY_KILL_2, SAY_KILL_3));
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_BAINE, FAIL);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (me->IsNonMeleeSpellCasted(false))
            return;

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_PULVERIZE:
                    //Talk(SAY_PULVERIZE);
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        DoCast(pTarget, SPELL_PULVERIZE_J);
                        DoCast(pTarget, SPELL_PULV_DBM);
                    }
                    events.ScheduleEvent(EVENT_PULVERIZE, 40000); // every 40 secs.
                    events.ScheduleEvent(EVENT_PULVERIZE_DAMAGE, 3000); // You have 3 secs to run.
                    break;
                case EVENT_PULVERIZE_DAMAGE:
                    if (GameObject* platform = GetClosestGameObjectWithEntry(me, 209255, 20.0f))
                        platform->SetGoState(GO_STATE_ACTIVE);
                    DoCast(me, SPELL_PULVERIZE_D);
                    break;
                case EVENT_THROW_TOTEM:
                    //Talk(SAY_THROW_TOTEM);
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_THROW_TOTEM);
                    events.ScheduleEvent(EVENT_THROW_TOTEM, 25000); // every 25 secs.
                    break;
                case EVENT_LAVA_CHECK:
                {
                    Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
                        if (Player* player = i->getSource())
                            if (player->IsInWater())
                                player->CastSpell(player, SPELL_MOLTEN_FIST, true); // Add the damage aura to players in Magma.
                    events.ScheduleEvent(EVENT_LAVA_CHECK, 1000);
                    break;
                }
            }
        }

        DoMeleeAttackIfReady();

        EnterEvadeIfOutOfCombatArea(diff);
    }
};

CreatureAI* GetAI_boss_echo_of_baine(Creature* pCreature)
{
    return new boss_echo_of_baineAI(pCreature);
}

void AddSC_boss_echo_of_baine()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_echo_of_baine";
    pNewScript->GetAI = &GetAI_boss_echo_of_baine;
    pNewScript->RegisterSelf();
}
