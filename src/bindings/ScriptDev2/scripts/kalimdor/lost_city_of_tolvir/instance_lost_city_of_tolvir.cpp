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
SDName: instance_blackrock_caverns
SD%Complete: 10
SDComment: Placeholder
SDCategory: Blackrock Caverns
EndScriptData */

#include "precompiled.h"
#include "lost_city_of_tolvir.h"

instance_lost_city_of_tolvir::instance_lost_city_of_tolvir(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();
};

void instance_lost_city_of_tolvir::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
    tunnelList.clear();
}

void instance_lost_city_of_tolvir::OnCreatureCreate(Creature* pCreature)
{
    switch(pCreature->GetEntry())
    {
        case NPC_HUSAM:
        case NPC_BARIM:
        case NPC_AUGH:
        case NPC_LOCKMAW:
        case NPC_SIAMAT:
            m_mNpcEntryGuidStore[pCreature->GetEntry()] = pCreature->GetObjectGuid();
            break;
        case NPC_WIND_TUNNEL:
            tunnelList.push_back(pCreature->GetObjectGuid());
            pCreature->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
            if (!SiamatAvailable())
                pCreature->SetVisibility(VISIBILITY_OFF);
            break;
    }
}

bool instance_lost_city_of_tolvir::SiamatAvailable()
{
    return m_auiEncounter[TYPE_HUSAM] == DONE && m_auiEncounter[TYPE_BARIM] == DONE && m_auiEncounter[TYPE_LOCKMAW] == DONE;
}

void instance_lost_city_of_tolvir::OnObjectCreate(GameObject* pGo)
{
    if (pGo->GetEntry() == GO_SIAMAT_PLATFORM)
    {
        if (SiamatAvailable())
            pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);
        m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
        pGo->SetActiveObjectState(true);
    }
}

void instance_lost_city_of_tolvir::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_HUSAM:
        case TYPE_BARIM:
        case TYPE_LOCKMAW:
        case TYPE_SIAMAT:
            m_auiEncounter[uiType] = uiData;
            break;
    }

    if (uiData == DONE)
    {
        if (SiamatAvailable())
        {
            if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_SIAMAT_PLATFORM))
                pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_DESTROYED);

            for (std::list<ObjectGuid>::iterator itr = tunnelList.begin(); itr != tunnelList.end(); ++itr)
                if (Creature* tunnel = instance->GetAnyTypeCreature(*itr))
                    tunnel->SetVisibility(VISIBILITY_ON);
        }

        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " "
                   << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

uint32 instance_lost_city_of_tolvir::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_lost_city_of_tolvir::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for (uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_lost_city_of_tolvir(Map* pMap)
{
    return new instance_lost_city_of_tolvir(pMap);
}

enum Spells
{
    // Blaze of Heavens
    SPELL_BLAZE                 = 48906,

    // Harbinger of Death
    SPELL_SOUL_SEVER            = 82255,
    SPELL_WAIL_DARKNESS         = 82533,
    H_SPELL_WAIL_DARKNESS       = 90038,

    // Minion of Siamat
    SPELL_CHAIN_LIGHTNING       = 44704,
    H_SPELL_CHAIN_LIGHTNING     = 90027,

    // Neferset Darkcaster
    SPELL_HEX                   = 82760,
    SPELL_METEOR                = 84032,
    H_SPELL_METEOR              = 90023,
    SPELL_SHADOW_BOLT           = 82765,
    H_SPELL_SHADOW_BOLT         = 90036,

    // Neferset Plaguebringer
    SPELL_DISEASE_BREATH        = 73976,
    H_SPELL_DISEASE_BREATH      = 90005,
    SPELL_INFECTIOUS_PLAGUE     = 82768,

    // Neferset Theurgist
    SPELL_MIXTURE = 82759,
    SPELL_RITUAL_BLOODLETTING   = 82753,
    H_SPELL_RITUAL_BLOODLETTING = 89993,
    SPELL_VICIOUS_LEECHES       = 82757,

    // Neferset Torturer
    SPELL_BRANDED_TONGUE        = 82744,
    H_SPELL_BRANDED_TONGUE      = 90037,
    SPELL_SERUM_TORMENT         = 82750,
    H_SPELL_SERUM_TORMENT       = 89994,

    // Oathsworn Axemaster
    SPELL_SKULL_CRUSH           = 44922,
    SPELL_SLAM                  = 82763,

    // Oathsworn Captain
    SPELL_SKULL_CRACK           = 82670,

    // Oathsworn Myrmidon
    SPELL_BLADED_SHIELD         = 82671,
    H_SPELL_BLADED_SHIELD       = 95184,
    SPELL_RESISTANCE            = 82673,

    // Oathsworn Pathfinder
    SPELL_CALL_FALCON           = 82792,
    SPELL_SHOOT                 = 83877,
    SPELL_WING_CLIP             = 82764,

    // Oathsworn Scorpid Keeper
    SPELL_BLINK                 = 84123,
    SPELL_CALL_SCORPID          = 84126,
    SPELL_EVASION               = 84122,

    // Oathsworn Skinner
    SPELL_FAN_KNIVES            = 82789,
    H_SPELL_FAN_KNIVES          = 90034,
    SPELL_SPINAL_PIERCE         = 84031,

    // Oathsworn Tamer
    SPELL_FERAL_LINK            = 84230,

    // Wanderer
    SPELL_CALL_CROCO            = 82791,
    SPELL_FORKED_SHOT           = 82794,
    H_SPELL_FORKED_SHOT         = 89992,

    // Pygmy Brute
    SPELL_IMPALE                = 83783,
    H_SPELL_IMPALE              = 89990,
    SPELL_SHOCKWAVE             = 83785,
    H_SPELL_SHOCKWAVE           = 90024,

    // Pygmy Fire Breather
    SPELL_DRAGON_BREATH         = 83776,
    H_SPELL_DRAGON_BREATH       = 90026,
    SPELL_FIRE_BLAST            = 83778,
    H_SPELL_FIREBLAST           = 90025,

    // Pygmy Scout
    SPELL_TOXIC_DART            = 83780,
    H_SPELL_TOXIC_DART          = 89991,

    // Servent of Siamat
    SPELL_LIGHTNING_NOVA        = 84544,
    H_SPELL_LIGHTNING_NOVA      = 90015,
    SPELL_THUNDER_CRASH         = 84521,
};

enum eEvents
{
    EVENT_NONE,
    EVENT_BLAZE,
    EVENT_SOUL_SEVER,
    EVENT_WAIL_DARKNESS,
    EVENT_CHAIN_LIGHTNING,
    EVENT_HEX,
    EVENT_METEOR,
    EVENT_SHADOW_BOLT,
    EVENT_DISEASE_BREATH,
    EVENT_INFECTIOUS_PLAGUE,
    EVENT_MIXTURE,
    EVENT_RITUAL_BLOODLETTING,
    EVENT_VICIOUS_LEECHES,
    EVENT_BRANDED_TONGUE,
    EVENT_SERUM_TORMENT,
    EVENT_SKULL_CRUSH,
    EVENT_SLAM,
    EVENT_SKULL_CRACK,
    EVENT_BLADED_SHIELD,
    EVENT_RESISTANCE,
    EVENT_CALL_FALCON,
    EVENT_SHOOT,
    EVENT_WING_CLIP,
    EVENT_BLINK,
    EVENT_CALL_SCORPID,
    EVENT_EVASION,
    EVENT_FAN_KNIVES,
    EVENT_SPINAL_PIERCE,
    EVENT_FERAL_LINK,
    EVENT_CALL_CROCO,
    EVENT_FORKED_SHOT,
    EVENT_IMPALE,
    EVENT_SHOCKWAVE,
    EVENT_DRAGON_BREATH,
    EVENT_FIRE_BLAST,
    EVENT_TOXIC_DART,
    EVENT_LIGHTNING_NOVA,
    EVENT_THUNDER_CRASH,
};

struct MANGOS_DLL_DECL npc_neferset_darkcasterAI : public ScriptedAI
{
    npc_neferset_darkcasterAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset() override
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        events.ScheduleEvent(EVENT_HEX, 8000);
        events.ScheduleEvent(EVENT_METEOR, 5000);
        events.ScheduleEvent(EVENT_SHADOW_BOLT, 6000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_HEX:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_HEX);
                    events.RescheduleEvent(EVENT_HEX, 5000);
                    return;
                case EVENT_METEOR:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_METEOR);
                    events.RescheduleEvent(EVENT_METEOR, 5000);
                    return;
                case EVENT_SHADOW_BOLT:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_SHADOW_BOLT);
                    events.RescheduleEvent(EVENT_SHADOW_BOLT, 3000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_neferset_darkcasterAI(Creature* pCreature)
{
    return new npc_neferset_darkcasterAI(pCreature);
}

struct MANGOS_DLL_DECL npc_neferset_plaguebringerAI : public ScriptedAI
{
    npc_neferset_plaguebringerAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_DISEASE_BREATH, 5000);
        events.ScheduleEvent(EVENT_INFECTIOUS_PLAGUE, 7000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_DISEASE_BREATH:
                    DoCast(m_creature->getVictim(), SPELL_DISEASE_BREATH);
                    events.RescheduleEvent(EVENT_DISEASE_BREATH, 5000);
                    return;
                case EVENT_INFECTIOUS_PLAGUE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_INFECTIOUS_PLAGUE);
                    events.RescheduleEvent(EVENT_INFECTIOUS_PLAGUE, 5000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_neferset_plaguebringerAI(Creature* pCreature)
{
    return new npc_neferset_plaguebringerAI(pCreature);
}

struct MANGOS_DLL_DECL npc_neferset_theurgistAI : public ScriptedAI
{
    npc_neferset_theurgistAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_MIXTURE, 10000);
        events.ScheduleEvent(EVENT_RITUAL_BLOODLETTING, 4000);
        events.ScheduleEvent(EVENT_VICIOUS_LEECHES, 7000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_MIXTURE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_MIXTURE);
                    events.RescheduleEvent(EVENT_MIXTURE, 6000);
                    return;
                case EVENT_RITUAL_BLOODLETTING:
                    DoCast(m_creature->getVictim(), SPELL_RITUAL_BLOODLETTING);
                    events.RescheduleEvent(EVENT_RITUAL_BLOODLETTING, 2000);
                    return;
                case EVENT_VICIOUS_LEECHES:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_VICIOUS_LEECHES);
                    events.RescheduleEvent(EVENT_VICIOUS_LEECHES, 5000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_neferset_theurgist(Creature* pCreature)
{
    return new npc_neferset_theurgistAI(pCreature);
}

struct MANGOS_DLL_DECL npc_neferset_torturerAI : public ScriptedAI
{
    npc_neferset_torturerAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_BRANDED_TONGUE, 5000);
        events.ScheduleEvent(EVENT_SERUM_TORMENT, 4000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_BRANDED_TONGUE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_BRANDED_TONGUE);
                    events.RescheduleEvent(EVENT_BRANDED_TONGUE, 3000);
                    return;
                case EVENT_SERUM_TORMENT:
                    DoCast(m_creature->getVictim(), SPELL_SERUM_TORMENT);
                    events.RescheduleEvent(EVENT_SERUM_TORMENT, 2000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_neferset_torturer(Creature* pCreature)
{
    return new npc_neferset_torturerAI(pCreature);
}

struct MANGOS_DLL_DECL npc_oathsworn_axemasterAI : public ScriptedAI
{
    npc_oathsworn_axemasterAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_SKULL_CRUSH, 5000);
        events.ScheduleEvent(EVENT_SLAM, 3000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_SKULL_CRUSH:
                    DoCast(m_creature->getVictim(), SPELL_SKULL_CRUSH);
                    events.RescheduleEvent(EVENT_SKULL_CRUSH, 3000);
                    return;
                case EVENT_SLAM:
                    DoCast(m_creature->getVictim(), SPELL_SLAM);
                    events.RescheduleEvent(EVENT_SLAM, 2000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oathsworn_axemaster(Creature* pCreature)
{
    return new npc_oathsworn_axemasterAI(pCreature);
}

struct MANGOS_DLL_DECL npc_oathsworn_captainAI : public ScriptedAI
{
    npc_oathsworn_captainAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_SKULL_CRACK, 3000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_SKULL_CRACK:
                    DoCast(m_creature->getVictim(), SPELL_SKULL_CRACK);
                    events.RescheduleEvent(EVENT_SKULL_CRACK, 3000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oathsworn_captain(Creature* pCreature)
{
    return new npc_oathsworn_captainAI(pCreature);
}

struct MANGOS_DLL_DECL npc_oathsworn_myrmidonAI : public ScriptedAI
{
    npc_oathsworn_myrmidonAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_BLADED_SHIELD, 5000);
        events.ScheduleEvent(EVENT_RESISTANCE, 11000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_BLADED_SHIELD:
                    DoCast(m_creature->getVictim(), SPELL_BLADED_SHIELD);
                    events.RescheduleEvent(EVENT_BLADED_SHIELD, 5000);
                    return;
                case EVENT_RESISTANCE:
                    DoCast(m_creature->getVictim(), SPELL_SLAM);
                    events.RescheduleEvent(EVENT_RESISTANCE, 11000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oathsworn_myrmidon(Creature* pCreature)
{
    return new npc_oathsworn_myrmidonAI(pCreature);
}

struct MANGOS_DLL_DECL npc_oathsworn_pathfinderAI : public ScriptedAI
{
    npc_oathsworn_pathfinderAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_CALL_FALCON, 8000);
        events.ScheduleEvent(EVENT_SHOOT, 2000);
        events.ScheduleEvent(EVENT_WING_CLIP, 4000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_CALL_FALCON:
                    DoCast(m_creature->getVictim(), SPELL_CALL_FALCON);
                    events.RescheduleEvent(EVENT_CALL_FALCON, 8000);
                    return;
                case EVENT_SHOOT:
                    DoCast(m_creature->getVictim(), SPELL_SHOOT);
                    events.RescheduleEvent(EVENT_SHOOT, 1000);
                    return;
                case EVENT_WING_CLIP:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_WING_CLIP);
                    events.RescheduleEvent(EVENT_WING_CLIP, 3000);
                    return;
                }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oathsworn_pathfinder(Creature* pCreature)
{
    return new npc_oathsworn_pathfinderAI(pCreature);
}

struct MANGOS_DLL_DECL npc_oathsworn_scorpid_keeperAI : public ScriptedAI
{
    npc_oathsworn_scorpid_keeperAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_CALL_SCORPID, 8000);
        events.ScheduleEvent(EVENT_EVASION, 4000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_CALL_SCORPID:
                    DoCast(m_creature->getVictim(), SPELL_CALL_SCORPID);
                    events.RescheduleEvent(EVENT_CALL_SCORPID, 8000);
                    return;
                case EVENT_EVASION:
                    DoCast(m_creature->getVictim(), SPELL_EVASION);
                    events.RescheduleEvent(EVENT_EVASION, 5000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oathsworn_scorpid_keeper(Creature* pCreature)
{
    return new npc_oathsworn_scorpid_keeperAI(pCreature);
}

struct MANGOS_DLL_DECL npc_oathsworn_skinnerAI : public ScriptedAI
{
    npc_oathsworn_skinnerAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_FAN_KNIVES, 5000);
        events.ScheduleEvent(EVENT_SPINAL_PIERCE, 7000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_FAN_KNIVES:
                    DoCast(m_creature->getVictim(), SPELL_FAN_KNIVES);
                    events.RescheduleEvent(EVENT_FAN_KNIVES, 3000);
                    return;
                case EVENT_SPINAL_PIERCE:
                    DoCast(m_creature->getVictim(), SPELL_SPINAL_PIERCE);
                    events.RescheduleEvent(EVENT_SPINAL_PIERCE, 7000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_oathsworn_skinner(Creature* pCreature)
{
    return new npc_oathsworn_skinnerAI(pCreature);
}

struct MANGOS_DLL_DECL npc_pygmy_bruteAI : public ScriptedAI
{
    npc_pygmy_bruteAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_IMPALE, 1000);
        events.ScheduleEvent(EVENT_SHOCKWAVE, 5000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_IMPALE:
                    DoCast(m_creature->getVictim(), SPELL_IMPALE);
                    events.RescheduleEvent(EVENT_IMPALE, 1000);
                    return;
                case EVENT_SHOCKWAVE:
                    DoCast(m_creature->getVictim(), SPELL_SHOCKWAVE);
                    events.RescheduleEvent(EVENT_SHOCKWAVE, 4000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_pygmy_brute(Creature* pCreature)
{
    return new npc_pygmy_bruteAI(pCreature);
}

struct MANGOS_DLL_DECL npc_pygmy_firebreatherAI : public ScriptedAI
{
    npc_pygmy_firebreatherAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_DRAGON_BREATH, 3000);
        events.ScheduleEvent(EVENT_FIRE_BLAST, 4000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_SKULL_CRACK:
                    DoCast(m_creature->getVictim(), SPELL_SKULL_CRACK);
                    events.RescheduleEvent(SPELL_DRAGON_BREATH, 3000);
                    return;
                case EVENT_FIRE_BLAST:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_FIRE_BLAST);
                    events.RescheduleEvent(EVENT_FIRE_BLAST, 2000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_pygmy_firebreather(Creature* pCreature)
{
    return new npc_pygmy_firebreatherAI(pCreature);
}

struct MANGOS_DLL_DECL npc_pygmy_scoutAI : public ScriptedAI
{
    npc_pygmy_scoutAI(Creature* c) : ScriptedAI(c)
    {
        Reset();
    }

    EventMap events;

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit* /*who*/)
    {
        events.ScheduleEvent(EVENT_TOXIC_DART, 1000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch(eventId)
            {
                case EVENT_TOXIC_DART:
                    DoCast(m_creature->getVictim(), SPELL_TOXIC_DART);
                    events.RescheduleEvent(EVENT_TOXIC_DART, 1000);
                    return;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_pygmy_scout(Creature* pCreature)
{
    return new npc_pygmy_scoutAI(pCreature);
}

struct TunnelHelperStruct
{
    float srcX;
    float srcY;
    float destX;
    float destY;
    float destZ;
};

#define MAX_WIND_TUNNELS 6

TunnelHelperStruct tunnelLocations[MAX_WIND_TUNNELS] =
{
    { -10891.0f, -1379.0f, -10917.14f, -1388.46f, 35.53f },
    { -10887.0f, -1447.0f, -10917.21f, -1426.66f, 35.53f },
    { -10964.0f, -1481.0f, -10954.76f, -1437.18f, 35.71f },
    { -11027.0f, -1427.0f, -10986.18f, -1413.69f, 35.53f },
    { -10998.0f, -1356.0f, -10977.65f, -1375.88f, 35.71f },
    { -10930.0f, -1302.0f, -10941.38f, -1359.28f, 35.53f },
};

bool OnGossipHello_npc_wind_tunnel(Player* who, Creature* creature)
{
    if (who->IsInCombat())
        return true;

    for (int i = 0; i < MAX_WIND_TUNNELS; ++i)
        if (who->GetDistance2d(tunnelLocations[i].srcX, tunnelLocations[i].srcY) < 5.0f)
        {
            who->GetMotionMaster()->MoveJump(tunnelLocations[i].destX, tunnelLocations[i].destY, tunnelLocations[i].destZ, 55.0f, 10.0f);
            break;
        }

    return true;
}

void AddSC_instance_lost_city_of_tolvir()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_lost_city_of_tolvir";
    pNewScript->GetInstanceData = &GetInstanceData_instance_lost_city_of_tolvir;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_neferset_darkcaster";
    pNewScript->GetAI = &GetAI_npc_neferset_darkcasterAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_neferset_plaguebringer";
    pNewScript->GetAI = &GetAI_npc_neferset_plaguebringerAI;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_neferset_theurgist";
    pNewScript->GetAI = &GetAI_npc_neferset_theurgist;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_neferset_torturer";
    pNewScript->GetAI = &GetAI_npc_neferset_torturer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oathsworn_axemaster";
    pNewScript->GetAI = &GetAI_npc_oathsworn_axemaster;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oathsworn_captain";
    pNewScript->GetAI = &GetAI_npc_oathsworn_captain;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oathsworn_myrmidon";
    pNewScript->GetAI = &GetAI_npc_oathsworn_myrmidon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oathsworn_pathfinder";
    pNewScript->GetAI = &GetAI_npc_oathsworn_pathfinder;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oathsworn_scorpid_keeper";
    pNewScript->GetAI = &GetAI_npc_oathsworn_scorpid_keeper;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_oathsworn_skinner";
    pNewScript->GetAI = &GetAI_npc_oathsworn_skinner;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_pygmy_brute";
    pNewScript->GetAI = &GetAI_npc_pygmy_brute;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_pygmy_firebreather";
    pNewScript->GetAI = &GetAI_npc_pygmy_firebreather;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_pygmy_scout";
    pNewScript->GetAI = &GetAI_npc_pygmy_scout;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_wind_tunnel";
    pNewScript->pGossipHello = &OnGossipHello_npc_wind_tunnel;
    pNewScript->RegisterSelf();
}
