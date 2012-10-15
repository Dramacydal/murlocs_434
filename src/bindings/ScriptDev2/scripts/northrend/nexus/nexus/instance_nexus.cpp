/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: instance_nexus
SD%Complete: 75%
SDComment:
SDCategory: The Nexus
EndScriptData */

#include "precompiled.h"
#include "nexus.h"

bool GOUse_go_containment_sphere(Player* pPlayer, GameObject* pGo)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)pGo->GetInstanceData();

    if (!pInstance)
        return false;

    switch(pGo->GetEntry())
    {
        case GO_CONTAINMENT_SPHERE_TELESTRA: pInstance->SetData(TYPE_TELESTRA, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ANOMALUS: pInstance->SetData(TYPE_ANOMALUS, SPECIAL); break;
        case GO_CONTAINMENT_SPHERE_ORMOROK:  pInstance->SetData(TYPE_ORMOROK, SPECIAL);  break;
    }

    if (Creature *pCaster = GetClosestCreatureWithEntry(pGo, NPC_BREATH_CASTER, 15.0f))
        pCaster->ForcedDespawn();

    pGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
    return false;
}

instance_nexus::instance_nexus(Map* pMap) : ScriptedInstance(pMap)
{
    Initialize();

    for (uint8 i = 0; i < MAX_SPECIAL_ACHIEV_CRITS; ++i)
        m_abAchievCriteria[i] = false;
}

void instance_nexus::Initialize()
{
    memset(&m_auiEncounter, 0, sizeof(m_auiEncounter));
}

void instance_nexus::OnObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_CONTAINMENT_SPHERE_TELESTRA:
            if (m_auiEncounter[TYPE_TELESTRA] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_CONTAINMENT_SPHERE_ANOMALUS:
            if (m_auiEncounter[TYPE_ANOMALUS] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;
        case GO_CONTAINMENT_SPHERE_ORMOROK:
            if (m_auiEncounter[TYPE_ORMOROK] == DONE)
                pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            break;

        default:
            return;
    }
    m_mGoEntryGuidStore[pGo->GetEntry()] = pGo->GetObjectGuid();
}

void instance_nexus::OnCreatureCreate(Creature* pCreature)
{
    if (pCreature->GetEntry() == NPC_KERISTRASZA)
        m_mNpcEntryGuidStore[NPC_KERISTRASZA] = pCreature->GetObjectGuid();
}

uint32 instance_nexus::GetData(uint32 uiType)
{
    if (uiType < MAX_ENCOUNTER)
        return m_auiEncounter[uiType];

    return 0;
}

void instance_nexus::SetData(uint32 uiType, uint32 uiData)
{
    switch (uiType)
    {
        case TYPE_TELESTRA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_SPLIT_PERSONALITY, true);
            if (uiData == DONE)
            {
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_CONTAINMENT_SPHERE_TELESTRA))
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case TYPE_ANOMALUS:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                SetSpecialAchievementCriteria(TYPE_ACHIEV_CHAOS_THEORY, true);
            if (uiData == DONE)
            {
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_CONTAINMENT_SPHERE_ANOMALUS))
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case TYPE_ORMOROK:
            m_auiEncounter[uiType] = uiData;
            if (uiData == DONE)
            {
                if (GameObject* pGo = GetSingleGameObjectFromStorage(GO_CONTAINMENT_SPHERE_ORMOROK))
                    pGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NO_INTERACT);
            }
            break;
        case TYPE_KERISTRASZA:
            m_auiEncounter[uiType] = uiData;
            if (uiData == IN_PROGRESS)
                m_sIntenseColdFailPlayers.clear();
            break;
        case TYPE_INTENSE_COLD_FAILED:
            // Insert the players who fail the achiev and haven't been already inserted in the set
            if (m_sIntenseColdFailPlayers.find(uiData) == m_sIntenseColdFailPlayers.end())
                m_sIntenseColdFailPlayers.insert(uiData);
            break;
        default:
            error_log("SD2: Instance Nexus: ERROR SetData = %u for type %u does not exist/not implemented.", uiType, uiData);
            return;
    }

    if (m_auiEncounter[TYPE_TELESTRA] == SPECIAL && m_auiEncounter[TYPE_ANOMALUS] == SPECIAL && m_auiEncounter[TYPE_ORMOROK] == SPECIAL)
    {
        // release Keristrasza from her prison here
        m_auiEncounter[TYPE_KERISTRASZA] = SPECIAL;

        Creature* pCreature = GetSingleCreatureFromStorage(NPC_KERISTRASZA);
        if (pCreature && pCreature->isAlive())
        {
            if (pCreature->isAlive())
            {
                pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE | UNIT_FLAG_OOC_NOT_ATTACKABLE);
                pCreature->RemoveAurasDueToSpell(SPELL_FROZEN_PRISON);
            }
        }
    }

    if (uiData == DONE)
    {
        OUT_SAVE_INST_DATA;

        std::ostringstream saveStream;
        saveStream << m_auiEncounter[0] << " " << m_auiEncounter[1] << " " << m_auiEncounter[2] << " " << m_auiEncounter[3];

        m_strInstData = saveStream.str();

        SaveToDB();
        OUT_SAVE_INST_DATA_COMPLETE;
    }
}

void instance_nexus::SetSpecialAchievementCriteria(uint32 uiType, bool bIsMet)
{
    if (uiType < MAX_SPECIAL_ACHIEV_CRITS)
        m_abAchievCriteria[uiType] = bIsMet;
}

bool instance_nexus::CheckAchievementCriteriaMeet(uint32 uiCriteriaId, Player const* pSource, Unit const* pTarget, uint32 uiMiscValue1 /* = 0*/)
{
    switch (uiCriteriaId)
    {
        case ACHIEV_CRIT_CHAOS_THEORY:
            return m_abAchievCriteria[TYPE_ACHIEV_CHAOS_THEORY];
        case ACHIEV_CRIT_SPLIT_PERSONALITY:
            return m_abAchievCriteria[TYPE_ACHIEV_SPLIT_PERSONALITY];
        case ACHIEV_CRIT_INTENSE_COLD:
            // Return true if not found in the set
            return m_sIntenseColdFailPlayers.find(pSource->GetGUIDLow()) == m_sIntenseColdFailPlayers.end();

        default:
            return false;
    }
}

void instance_nexus::Load(const char* chrIn)
{
    if (!chrIn)
    {
        OUT_LOAD_INST_DATA_FAIL;
        return;
    }

    OUT_LOAD_INST_DATA(chrIn);

    std::istringstream loadStream(chrIn);
    loadStream >> m_auiEncounter[0] >> m_auiEncounter[1] >> m_auiEncounter[2] >> m_auiEncounter[3];

    for(uint8 i = 0; i < MAX_ENCOUNTER; ++i)
    {
        if (m_auiEncounter[i] == IN_PROGRESS)
            m_auiEncounter[i] = NOT_STARTED;
    }

    OUT_LOAD_INST_DATA_COMPLETE;
}

InstanceData* GetInstanceData_instance_nexus(Map* pMap)
{
    return new instance_nexus(pMap);
}

/*####
# boss_commander_kolurg
####*/
// both commanders share this enum
enum
{
    SAY_AGGRO                                          = -1576024,   // need correct text
    SAY_KILL                                           = -1576025,   // need correct text
    SAY_DEATH                                          = -1576026,   // need correct text

    SPELL_BATTLE_SHOUT                                 = 31403,  // Increases the melee attack power of nearby friendly targets for 2 min.
    SPELL_CHARGE                                       = 60067,  // Charges an enemy, inflicting normal damage plus 75 and stuns the opponent for 2 sec.
    SPELL_FRIGHTENING_SHOUT                            = 19134,  // Shouts at nearby enemies causing them to flee in fear for 6 sec
    SPELL_WHIRLWIND_1                                  = 38618,  // triggers spell 38619 Attacks nearby enemies in a whirlwind of steel that lasts until cancelled. and inflicts double damage
    SPELL_WHIRLWIND_2                                  = 38619   // Attacks nearby enemies in a whirlwind of steel that inflicts 200% of weapon damage.
};

struct MANGOS_DLL_DECL boss_commander_kolurgAI : public ScriptedAI // triggers spell 38619 Attacks nearby enemies in a whirlwind of steel that lasts until cancelled. and inflicts double damage
{
    boss_commander_kolurgAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        //m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        //m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    //ScriptedInstance* m_pInstance;
    //bool m_bIsRegularMode;

    uint32 SPELL_BATTLE_SHOUT_Timer;
    uint32 SPELL_CHARGE_Timer;
    uint32 SPELL_FRIGHTENING_SHOUT_Timer;
    uint32 SPELL_WHIRLWIND_1_Timer;
    uint32 SPELL_WHIRLWIND_2_Timer;

    void Reset()
    {
        SPELL_BATTLE_SHOUT_Timer = 3000;   // needs adjusting
        SPELL_CHARGE_Timer = 2000;        // needs adjusting
        SPELL_FRIGHTENING_SHOUT_Timer = 2000;  // needs adjusting
        SPELL_WHIRLWIND_1_Timer = 0;   // THIS IS NOT RIGHT JUST A PLACEHOLDER FOR NOW
        SPELL_WHIRLWIND_2_Timer = 2000;   // needs adjusting


        //if (m_pInstance)
            //m_pInstance->SetData(TYPE_COMMANDER_KOLURG, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
       /* DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_COMMANDER_KOLURG, IN_PROGRESS);*/
    }

    //void AttackStart(Unit* who) {}

    //void MoveInLineOfSight(Unit* who) {}

    void KilledUnit(Unit *victim)
    {
        //DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* killer)
    {
        //DoScriptText(SAY_DEATH, m_creature);

        //if (m_pInstance)
            //m_pInstance->SetData(TYPE_COMMANDER_KOLURG, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SPELL_BATTLE_SHOUT_Timer < diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BATTLE_SHOUT);
            SPELL_BATTLE_SHOUT_Timer = 3000 + rand()%5000;
        }else SPELL_BATTLE_SHOUT_Timer -= diff;

        if (SPELL_CHARGE_Timer < diff)  // this spell needs to be a distance check on victims in distance not a timer on victim
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHARGE);
            SPELL_CHARGE_Timer = 2000 + rand()%5000;
        }else SPELL_CHARGE_Timer -= diff;

        if (SPELL_FRIGHTENING_SHOUT_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRIGHTENING_SHOUT);
            SPELL_FRIGHTENING_SHOUT_Timer = 2000 + rand()%5000;
        }else SPELL_FRIGHTENING_SHOUT_Timer -= diff;

        if (SPELL_WHIRLWIND_2_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND_2);
            SPELL_WHIRLWIND_2_Timer = 2000 + rand()%5000;
        }else SPELL_WHIRLWIND_2_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_commander_kolurg(Creature* pCreature)
{
    return new boss_commander_kolurgAI (pCreature);
}

/*####
# boss_commander_stoutbeard
####*/

struct MANGOS_DLL_DECL boss_commander_stoutbeardAI : public ScriptedAI
{
    boss_commander_stoutbeardAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        //m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        //m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    //ScriptedInstance* m_pInstance;
    //bool m_bIsRegularMode;

    uint32 SPELL_BATTLE_SHOUT_Timer;
    uint32 SPELL_CHARGE_Timer;
    uint32 SPELL_FRIGHTENING_SHOUT_Timer;
    uint32 SPELL_WHIRLWIND_1_Timer;
    uint32 SPELL_WHIRLWIND_2_Timer;

    void Reset()
    {
        SPELL_BATTLE_SHOUT_Timer = 3000;   // needs adjusting
        SPELL_CHARGE_Timer = 2000;        // needs adjusting
        SPELL_FRIGHTENING_SHOUT_Timer = 2000;  // needs adjusting
        SPELL_WHIRLWIND_1_Timer = 0;   // THIS IS NOT RIGHT JUST A PLACEHOLDER FOR NOW
        SPELL_WHIRLWIND_2_Timer = 2000;   // needs adjusting


        //if (m_pInstance)
            //m_pInstance->SetData(TYPE_COMMANDER_STOUTBEARD, NOT_STARTED);
    }

    void EnterCombat(Unit* who)
    {
       /* DoScriptText(SAY_AGGRO, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_COMMANDER_STOUTBEARD, IN_PROGRESS);*/
    }

    //void AttackStart(Unit* who) {}

    //void MoveInLineOfSight(Unit* who) {}

    void KilledUnit(Unit *victim)
    {
        //DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* killer)
    {
        //DoScriptText(SAY_DEATH, m_creature);

        //if (m_pInstance)
            //m_pInstance->SetData(TYPE_COMMANDER_STOUTBEARD, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SPELL_BATTLE_SHOUT_Timer < diff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BATTLE_SHOUT);
            SPELL_BATTLE_SHOUT_Timer = 3000 + rand()%5000;
        }else SPELL_BATTLE_SHOUT_Timer -= diff;

        if (SPELL_CHARGE_Timer < diff)  // this spell needs to be a distance check on victims in distance not a timer on victim
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHARGE);
            SPELL_CHARGE_Timer = 2000 + rand()%5000;
        }else SPELL_CHARGE_Timer -= diff;

        if (SPELL_FRIGHTENING_SHOUT_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_FRIGHTENING_SHOUT);
            SPELL_FRIGHTENING_SHOUT_Timer = 2000 + rand()%5000;
        }else SPELL_FRIGHTENING_SHOUT_Timer -= diff;

        if (SPELL_WHIRLWIND_2_Timer < diff)
        {
            DoCastSpellIfCan(m_creature->getVictim(), SPELL_WHIRLWIND_2);
            SPELL_WHIRLWIND_2_Timer = 2000 + rand()%5000;
        }else SPELL_WHIRLWIND_2_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_commander_stoutbeard(Creature* pCreature)
{
    return new boss_commander_stoutbeardAI (pCreature);
}

void AddSC_instance_nexus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "instance_nexus";
    pNewScript->GetInstanceData = &GetInstanceData_instance_nexus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_containment_sphere";
    pNewScript->pGOUse = &GOUse_go_containment_sphere;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_commander_stoutbeard";
    pNewScript->GetAI = &GetAI_boss_commander_stoutbeard;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_commander_kolurg";
    pNewScript->GetAI = &GetAI_boss_commander_kolurg;
    pNewScript->RegisterSelf();
}
