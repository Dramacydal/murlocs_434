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
SDName: boss_romogg
SD%Complete: 10
SDComment: Placeholder
SDCategory: Blackrock Caverns
EndScriptData */

#include "precompiled.h"
#include "blackrock_caverns.h"

enum Spells
{
    SPELL_CALL_FOR_HELP = 82137,
    SPELL_QUAKE = 75272,
    SPELL_CHAINS_OF_WOE = 75539,
    SPELL_CHAINS_OF_WOE_VISUAL = 75441,
    SPELL_CHAINS_OF_WOE_AURA = 82192,
    SPELL_THE_SKULLCRACKER = 75543,
    SPELL_WOUNDING_STRIKE = 69651,
};

enum Events
{
    EVENT_QUAKE = 1,
    EVENT_CHAINS_OF_WOE = 2,
    EVENT_WOUNDING_STRIKE = 3,
};

struct MANGOS_DLL_DECL boss_romoggAI : public ScriptedAI
{
    boss_romoggAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    bool castSkullCracker;
    ObjectGuid chainsOfWoe;

    void Reset() override
    {
        castSkullCracker = false;

        if (Creature* creature = m_creature->GetMap()->GetAnyTypeCreature(chainsOfWoe))
            creature->ForcedDespawn();

        DespawnCreatures(NPC_ANGERED_EARTH);
    }

    void EnterCombat(Unit* /*who*/) override
    {
        DoCast(m_creature, SPELL_CALL_FOR_HELP);
        events.ScheduleEvent(EVENT_QUAKE, urand(40000, 60000), 0, 0);
        events.ScheduleEvent(EVENT_WOUNDING_STRIKE, urand(2000, 4000), 0, 0);
        DoCastAOE(SPELL_CALL_FOR_HELP);
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ROMOGG, IN_PROGRESS);

        m_creature->MonsterYell("Boss Cho'gall not gonna be happy 'bout dis!", LANG_UNIVERSAL, NULL);
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ROMOGG, DONE);

        DespawnCreatures(NPC_ANGERED_EARTH);
        m_creature->MonsterYell("Rom'ogg sorry...", LANG_UNIVERSAL, NULL);

        if (Creature* creature = m_creature->GetMap()->GetAnyTypeCreature(chainsOfWoe))
            creature->ForcedDespawn();
    }

    void KilledUnit(Unit* pVictim) override
    {
        m_creature->MonsterYell("Dat's what you get! Noting!", LANG_UNIVERSAL, NULL);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_ROMOGG, FAIL);
    }

    void SummonedCreatureJustDied(Creature* creature)
    {
        if (creature->GetEntry() == NPC_CHAINS_OF_WOE)
            creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        //Creature* chains = m_creature->GetMap()->GetAnyTypeCreature(chainsOfWoe);
        //if(chains && chains->isAlive())
        //{
        //    /* Buggy!

        //    Map::PlayerList const &PlayerList = me->GetMap()->GetPlayers();

        //    if (!PlayerList.isEmpty())
        //    {
        //        for (Map::PlayerList::const_iterator i = PlayerList.begin(); i != PlayerList.end(); ++i)
        //            if (!i->getSource()->HasAura(SPELL_CHAINS_OF_WOE_AURA))
        //                me->CastSpell(i->getSource(),SPELL_CHAINS_OF_WOE_AURA,true);
        //    }*/
        //}

        if (castSkullCracker)
        {
            m_creature->MonsterYell("Stand still! Rom'ogg crack your skulls!", LANG_UNIVERSAL, NULL);

            castSkullCracker = false;
            DoCastAOE(SPELL_THE_SKULLCRACKER);

            return;
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_QUAKE:
                    DoCastAOE(SPELL_QUAKE);

                    if (!m_bIsRegularMode)
                    {
                        // Summon Angered Earth
                        WorldLocation myPos;
                        m_creature->GetPosition(myPos);

                        for(uint8 i=1; i<=5; i++)
                            m_creature->SummonCreature(NPC_ANGERED_EARTH,myPos.coord_x,myPos.coord_y,myPos.coord_z,0.0f,TEMPSUMMON_CORPSE_DESPAWN, 0);
                    }

                    events.ScheduleEvent(EVENT_QUAKE, 25000);
                    break;
                case EVENT_CHAINS_OF_WOE:
                    DoCastAOE(SPELL_CHAINS_OF_WOE);

                    castSkullCracker = true;
                    events.ScheduleEvent(EVENT_CHAINS_OF_WOE, 20000);
                    break;
                case EVENT_WOUNDING_STRIKE:
                    DoCastVictim(SPELL_WOUNDING_STRIKE);
                    events.ScheduleEvent(EVENT_WOUNDING_STRIKE, 15000);
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }

    void JustSummoned(Creature* summon) override
    {
        summon->SetActiveObjectState(true);

        if (summon->GetEntry() == NPC_CHAINS_OF_WOE)
        {
            summon->CastSpell(summon, SPELL_CHAINS_OF_WOE_VISUAL, true);
            //summon->SetReactState(REACT_PASSIVE);
            chainsOfWoe = summon->GetObjectGuid();
        }
        else if(summon->GetEntry() == NPC_ANGERED_EARTH)
        {
            summon->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
    }

    void DespawnCreatures(uint32 entry)
    {
        std::list<Creature*> creatures;
        GetCreatureListWithEntryInGrid(creatures, m_creature, entry, 500.0f);

        if (creatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
            (*iter)->ForcedDespawn();
    }
};

CreatureAI* GetAI_boss_romogg(Creature* pCreature)
{
    return new boss_romoggAI(pCreature);
}

void AddSC_boss_romogg()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_romogg";
    pNewScript->GetAI = &GetAI_boss_romogg;
    pNewScript->RegisterSelf();
}
