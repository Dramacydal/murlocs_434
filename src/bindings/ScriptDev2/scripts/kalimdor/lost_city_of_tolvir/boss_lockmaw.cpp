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
SDName: boss_lockmaw
SD%Complete: 10
SDComment: Placeholder
SDCategory: Lost City of Tol'Vir
EndScriptData */

#include "precompiled.h"
#include "lost_city_of_tolvir.h"

#define SAY_AGGRO "Augh smart ! Augh already steal treasure while you no looking !"
#define SAY_DIED "Gwaaaaaaaaaaaahhh!!!"

enum Spells
{
    //Lockmaw
    SPELL_DUST_FLAIL = 81642,
    SPELL_SCENT_OF_BLOOD = 81690,
    H_SPELL_SCENT_OF_BLOOD = 89998,
    SPELL_VENOMOUS_RAGE = 81706,
    SPELL_VISCOUS_POISON = 81630,
    H_SPELL_VISCOUS_POISON = 90004,
    //Augh
    SPELL_H_FURY = 23537,
    SPELL_H_FIRE_DRAGON = 29964,
    SPELL_CLOUD = 7964,
    SPELL_TEMPEST = 1680,
    SPELL_PARALYTIC_BLOW_DART = 89989,
};

enum Events
{
    //Lockmaw
    EVENT_DUST_FLAIL = 1,
    EVENT_SCENT_OF_BLOOD = 2,
    EVENT_VISCOUS_POISON = 3,
    //Augh
    EVENT_PARALYTIC_BLOW_DART = 4,
    EVENT_CLOUD = 5,
    EVENT_FIRE_DRAGON = 6,
    EVENT_TEMPEST = 7,
};

enum SummonIds
{
    NPC_FRENZIED_CROCOLISK = 43658,
};

const Position2 SummonLocations[5] =
{
    {-11033.29f, -1674.57f, -0.56f, 1.09f},
    {-11029.84f, -1673.09f, -0.37f, 2.33f},
    {-11007.25f, -1666.37f, -0.23f, 2.46f},
    {-11006.83f, -1666.85f, -0.25f, 2.23f},
    {-11031.00f, -1653.59f, 0.86f, 2.42f},
};

struct MANGOS_DLL_DECL boss_lockmawAI : public ScriptedAI
{
    boss_lockmawAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    std::list<ObjectGuid> Summons;
    bool Rage;

    void DespawnAllSummon()
    {
        for (std::list<ObjectGuid>::iterator itr = Summons.begin(); itr != Summons.end(); ++itr)
            if (Creature* creature = m_creature->GetMap()->GetAnyTypeCreature(*itr))
                creature->ForcedDespawn();
    }

    void Reset() override
    {
        events.Reset();
        DespawnAllSummon();
        Rage = false;
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOCKMAW, IN_PROGRESS);
    }

    void JustDied(Unit* pKiller) override
    {
        DespawnAllSummon();
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOCKMAW, DONE);

        m_creature->SummonCreature(49045, SummonLocations[4].x, SummonLocations[4].y, SummonLocations[4].z, SummonLocations[4].o, TEMPSUMMON_MANUAL_DESPAWN, 0);
    }

     void EnterCombat(Unit* /*Who*/) override
    {
        m_creature->SetInCombatWithZone();
        EnterPhaseGround();
    }

    void EnterPhaseGround()
    {
        events.ScheduleEvent(EVENT_DUST_FLAIL, urand(15000, 22000));
        events.ScheduleEvent(EVENT_VISCOUS_POISON, 12000);
        events.ScheduleEvent(EVENT_SCENT_OF_BLOOD, 30000);
    }

    void KilledUnit(Unit* pVictim) override
    {
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LOCKMAW, FAIL);
    }

    void JustSummoned(Creature* unit) override
    {
        if (unit->GetEntry() == NPC_FRENZIED_CROCOLISK)
            Summons.push_back(unit->GetObjectGuid());
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_creature->GetHealthPercent() < 30 && Rage == false)
        {
            DoCast(m_creature, SPELL_VENOMOUS_RAGE);
            Rage = true;
        }

        events.Update(uiDiff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while(uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_DUST_FLAIL:
                    DoCast(m_creature->getVictim(), SPELL_DUST_FLAIL);
                    events.ScheduleEvent(EVENT_DUST_FLAIL, urand(15000, 22000));
                return;

                case EVENT_VISCOUS_POISON:
                    DoCast(m_creature->getVictim(), SPELL_VISCOUS_POISON);
                    events.ScheduleEvent(EVENT_VISCOUS_POISON, 12000);
                    return;

                case EVENT_SCENT_OF_BLOOD:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(m_creature->getVictim(), SPELL_SCENT_OF_BLOOD);

                    for(uint8 i=0; i<3; i++)
                    {
                        if (Creature* Crocolisk = m_creature->SummonCreature(NPC_FRENZIED_CROCOLISK, SummonLocations[i].x, SummonLocations[i].y, SummonLocations[i].z, SummonLocations[i].o, TEMPSUMMON_CORPSE_DESPAWN, 0))
                        {
                            Crocolisk->AddThreat(m_creature->getVictim(), 0.0f);
                            Crocolisk->SetInCombatWithZone();
                        }
                    }

                    events.ScheduleEvent(EVENT_SCENT_OF_BLOOD, 30000);
                    return;

                default:
                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_aughAI : public ScriptedAI
{
    mob_aughAI(Creature *c) : ScriptedAI(c) { Reset(); }

    EventMap events;

    void Reset() override
    {
        m_creature->RemoveAllAuras();
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        //m_creature->MonsterYell(SAY_AGGRO, LANG_UNIVERSAL, NULL);
        DoCast(m_creature, SPELL_H_FURY);
        EnterPhaseGround();
    }

    void EnterPhaseGround()
    {
        events.ScheduleEvent(EVENT_PARALYTIC_BLOW_DART, 10000);
        events.ScheduleEvent(EVENT_CLOUD, 18000);
        events.ScheduleEvent(EVENT_FIRE_DRAGON, 40000);
        events.ScheduleEvent(EVENT_TEMPEST, 30000);
    }

    void JustDied(Unit* /*killer*/) override
    {
        //m_creature->MonsterYell(SAY_DIED, LANG_UNIVERSAL, NULL);
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(uiDiff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_PARALYTIC_BLOW_DART:
                    DoCast(m_creature->getVictim(), SPELL_PARALYTIC_BLOW_DART);
                    events.ScheduleEvent(EVENT_PARALYTIC_BLOW_DART, 10000);
                    return;

                case EVENT_CLOUD:
                    DoCast(m_creature->getVictim(), SPELL_CLOUD);
                    events.ScheduleEvent(EVENT_CLOUD, 18000);
                    return;

                case EVENT_FIRE_DRAGON:
                    DoCastVictim(SPELL_H_FIRE_DRAGON);
                    events.ScheduleEvent(EVENT_FIRE_DRAGON, 40000);
                    return;

                case EVENT_TEMPEST:
                    DoCast(m_creature, SPELL_TEMPEST);
                    events.ScheduleEvent(EVENT_TEMPEST, 30000);
                    return;

                default:
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

#define spell_vicious 81677
#define spell_vicious_H 89999

struct MANGOS_DLL_DECL mob_crosilikAI : public ScriptedAI
{
    mob_crosilikAI(Creature *c) : ScriptedAI(c)
    {
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint32 vicious;

    void Reset() override
    {
        vicious = 10000;
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        if (vicious<= diff)
        {
            if (Unit* pTar = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTar, !m_bIsRegularMode ? spell_vicious_H : spell_vicious);
            vicious = 10000;
        }
        else
            vicious -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lockmaw(Creature* pCreature)
{
    return new boss_lockmawAI(pCreature);
}

CreatureAI* GetAI_mob_augh(Creature* pCreature)
{
    return new mob_aughAI(pCreature);
}

CreatureAI* GetAI_mob_crosilikAI(Creature* pCreature)
{
    return new mob_crosilikAI(pCreature);
}

void AddSC_boss_lockmaw()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lockmaw";
    pNewScript->GetAI = &GetAI_boss_lockmaw;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_augh";
    pNewScript->GetAI = &GetAI_mob_augh;
    pNewScript->RegisterSelf();

    
    pNewScript = new Script;
    pNewScript->Name = "mob_crosilik";
    pNewScript->GetAI = &GetAI_mob_crosilikAI;
    pNewScript->RegisterSelf();
}
