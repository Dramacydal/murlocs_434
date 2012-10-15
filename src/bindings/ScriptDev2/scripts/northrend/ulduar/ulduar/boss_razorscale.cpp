/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_razorscale
SD%Complete:
SDComment: harpoons display should change when clicked
SDCategory: Ulduar
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    //yells/emotes
    SAY_INTRO               = -1603030,
    SAY_AGGRO1              = -1603031,
    SAY_AGGRO2              = -1603032,
    SAY_AGGRO3              = -1603033,
    SAY_GROUND              = -1603034,
    EMOTE_DEEP_BREATH       = -1603035,
    SAY_FIRES_EXTINGUISH    = -1603036,
    EMOTE_HARPOON           = -1603337,
    EMOTE_GROUNDED          = -1603338,

    //razorscale air phase
    SPELL_FIREBALL              = 62796,
    SPELL_FIREBALL_H            = 63815,
    SPELL_WING_BUFFET           = 62666,
    SPELL_STUN                  = 62794,
    SPELL_SUMMON_DWARF          = 62916,
    //both
    SPELL_BERSERK               = 47008,
    DEVOURING_FLAME_MISSILE     = 63236,
    SPELL_FLAME_BREATH          = 63317,
    SPELL_FLAME_BREATH_H        = 64021,
    NPC_CONTROLLER              = 33233,  // used for casting deep breath on turrets
    //ground
    SPELL_FLAME_BUFFET          = 64016,
    SPELL_FLAME_BUFFET_H        = 64023,
    SPELL_FUSE_ARMOR            = 64771,

    //devouring flame target
    NPC_DEVOURING_TARGET        = 34188,
    NPC_DEVOURING_TARGET_H      = 34189,
    AURA_DEVOURING_FLAME        = 64709,
    AURA_DEVOURING_FLAME_H      = 64734,

    // mole machine
    NPC_MOLE_MACHINE            = 33245,    // used to summon adds in phase 1
    NPC_HARPOONS_DUMMY          = 33282,    // used to cast spells for harpoons
    SPELL_SUMMON_MOLE_MACHINE   = 73071,

    // harpoons
    SPELL_HARPOON_SHOT          = 63659,
    GO_REPAIR_HARPOON_4         = 194543, // BROKEN HARBOON 194565
    GO_REPAIR_HARPOON_3         = 194542,
    GO_REPAIR_HARPOON_2         = 194541,
    GO_REPAIR_HARPOON_1         = 194519,
    //dark rune watcher
    SPELL_LIGHTNING_BOLT        = 63809,
    SPELL_LIGHTNING_BOLT_H      = 64696,
    SPELL_CHAIN_LIGHTNING       = 64758,
    SPELL_CHAIN_LIGHTNING_H     = 64759,

    //dark rune sentinel
    SPELL_BATTLE_SHOUT          = 46763,
    SPELL_BATTLE_SHOUT_H        = 64062,
    SPELL_WHIRLWIND             = 63808,

    //dark rune guardian
    SPELL_STORMSTRIKE           = 64757,

    //NPC ids
    MOB_DARK_RUNE_WATCHER       = 33453,
    MOB_DARK_RUNE_SENTINEL      = 33846,
    MOB_DARK_RUNE_GUARDIAN      = 33388,
 
    NPC_EXP_ENGINEER            = 33287,
};

//Positional defines 
struct LocationsXY
{
    float x, y, z, o;
    uint32 id;
};

static LocationsXY PositionLoc[]=
{
    {621.633301f, -228.671371f, 391.180328f},//right
    {564.140198f, -222.049149f, 391.517212f},//left
    {591.629761f, -209.629761f, 392.629761f},//middle
    {587.629761f, -179.022522f, 391.625061f},//ground
    {587.629761f, -179.022522f, 450.415070f},//air
};


#define HOME_X                      587.546997f
#define HOME_Y                      -174.927002f
 
#define GOSSIP_START     "Holt Klingenschuppe auf den Boden!"

enum RazorscalePhase
{
    PHASE_AIR           = 1,
    PHASE_GROUND        = 2,
    PHASE_PERMAGROUND   = 3,
};

//expedition commander
// start the event
struct MANGOS_DLL_DECL npc_expedition_commanderAI : public ScriptedAI
{
    npc_expedition_commanderAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bHasPlayerNear = false;
        Reset();
    }

    instance_ulduar* m_pInstance;

    bool m_bHasPlayerNear;
    bool m_bIsIntro;
    ObjectGuid m_uiPlayerGUID;
    uint32 m_uiSpeech_Timer;
    uint32 m_uiIntro_Phase;

    void Reset()
    {
        m_uiPlayerGUID.Clear();
        m_uiSpeech_Timer    = 3000;
        m_bIsIntro          = false;
        m_uiIntro_Phase     = 0;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (!m_bHasPlayerNear && m_creature->IsWithinDistInMap(pWho, 40.0f))
        {
            DoScriptText(SAY_INTRO, m_creature);
            m_bHasPlayerNear = true;
        }
    }

    void GetRazorDown()
    {
        if (Creature* pTemp = m_pInstance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
        {
            pTemp->SetInCombatWithZone();
            if(Unit* pPlayer = m_creature->GetMap()->GetUnit( m_uiPlayerGUID))
            {
                pTemp->AddThreat(pPlayer,0.0f);
                pTemp->AI()->AttackStart(pPlayer);
            }
        }
    }

    void BeginRazorscaleEvent(Player* pPlayer)
    {
        m_uiPlayerGUID      = pPlayer->GetObjectGuid();
        m_bIsIntro          = true;
        m_uiSpeech_Timer    = 3000;
        m_uiIntro_Phase     = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_bIsIntro)
        {
            if(m_uiSpeech_Timer < uiDiff)
            {
                switch(m_uiIntro_Phase)
                {
                case 0:
                    if(Creature* pEngineer = GetClosestCreatureWithEntry(m_creature, NPC_EXP_ENGINEER, 50.0f))
                        DoScriptText(SAY_AGGRO1, pEngineer);
                            GetRazorDown();
                    ++m_uiIntro_Phase;
                    m_uiSpeech_Timer = 5000;
                    break;
                case 1:
                    DoScriptText(SAY_AGGRO2, m_creature);
                    ++m_uiIntro_Phase;
                    m_uiSpeech_Timer = 7000;
                    break;
                case 2:
                    if(Creature* pEngineer = GetClosestCreatureWithEntry(m_creature, NPC_EXP_ENGINEER, 50.0f))
                        DoScriptText(SAY_AGGRO3, pEngineer);
                    ++m_uiIntro_Phase;
                    m_uiSpeech_Timer = 5000;
                    break;
                case 3:
                    m_bIsIntro = false;
                    ++m_uiIntro_Phase;
                    m_uiSpeech_Timer = 10000;
                    break;
                default:
                    m_uiSpeech_Timer = 100000;
                }
            }else m_uiSpeech_Timer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_expedition_commander(Creature* pCreature)
{
    return new npc_expedition_commanderAI(pCreature);
}

bool GossipHello_npc_expedition_commander(Player* pPlayer, Creature* pCreature)
{
    instance_ulduar* pInstance = (instance_ulduar *) pCreature->GetInstanceData();

    if(pInstance->GetData(TYPE_RAZORSCALE) != DONE)
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_START, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetObjectGuid());
    return true;
}

bool GossipSelect_npc_expedition_commander(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();
        pCreature->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
        ((npc_expedition_commanderAI*)pCreature->AI())->BeginRazorscaleEvent(pPlayer);
    }

    return true;
}

// devouring_flame_target
struct MANGOS_DLL_DECL mob_devouring_flame_targetAI : public ScriptedAI
{
    mob_devouring_flame_targetAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiDeath_Timer;

    void Reset()
    {
        m_uiDeath_Timer = 25500;
        m_creature->SetDisplayId(11686);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoCast(m_creature,  m_bIsRegularMode ? AURA_DEVOURING_FLAME : AURA_DEVOURING_FLAME_H);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // think that unnecessary because summon spell 63308 with duration 22 seconds
        if (m_uiDeath_Timer < uiDiff)
            m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
        else m_uiDeath_Timer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_devouring_flame_target(Creature* pCreature)
{
    return new mob_devouring_flame_targetAI(pCreature);
}

// dark rune watcher
struct MANGOS_DLL_DECL mob_dark_rune_watcherAI : public ScriptedAI
{
    mob_dark_rune_watcherAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiSpell_Timer;

    void Reset()
    {
        m_uiSpell_Timer = urand(5000, 10000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiSpell_Timer < diff)
        {
            switch(urand(0, 1))
            {
                case 0:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_LIGHTNING_BOLT : SPELL_LIGHTNING_BOLT_H);
                break;
                case 1:
                    DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_CHAIN_LIGHTNING : SPELL_CHAIN_LIGHTNING_H);
                break;
            }
            m_uiSpell_Timer = urand(5000, 10000);
        }else m_uiSpell_Timer -= diff;        

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_watcher(Creature* pCreature)
{
    return new mob_dark_rune_watcherAI(pCreature);
}

// dark rune sentinel
struct MANGOS_DLL_DECL mob_dark_rune_sentinelAI : public ScriptedAI
{
    mob_dark_rune_sentinelAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiWhirl_Timer;
    uint32 m_uiShout_Timer;

    void Reset()
    {
        m_uiWhirl_Timer = 10000;
        m_uiShout_Timer = 2000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiWhirl_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_WHIRLWIND) == CAST_OK)
                m_uiWhirl_Timer = urand(10000, 15000);
        }
        else
            m_uiWhirl_Timer -= diff;

        if (m_uiShout_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_BATTLE_SHOUT : SPELL_BATTLE_SHOUT_H) == CAST_OK)
                m_uiShout_Timer = 30000;
        }
        else
            m_uiShout_Timer -= diff;

        DoMeleeAttackIfReady();
    }

};

CreatureAI* GetAI_mob_dark_rune_sentinel(Creature* pCreature)
{
    return new mob_dark_rune_sentinelAI(pCreature);
}

// dark rune guardian
struct MANGOS_DLL_DECL mob_dark_rune_guardianAI : public ScriptedAI
{
    mob_dark_rune_guardianAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiStormstrike_Timer;

    void Reset()
    {
        m_uiStormstrike_Timer = 10000;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiStormstrike_Timer < diff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_STORMSTRIKE) == CAST_OK)
                m_uiStormstrike_Timer = urand(7000, 13000);
        }
        else
            m_uiStormstrike_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_dark_rune_guardian(Creature* pCreature)
{
    return new mob_dark_rune_guardianAI(pCreature);
}

/// mole machine
// used to summon dwarfes
struct MANGOS_DLL_DECL mob_mole_machineAI : public ScriptedAI
{
    mob_mole_machineAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        pCreature->SetDisplayId(11686);     // make invisible
        SetCombatMovement(false);
        Reset();
    }

    instance_ulduar* m_pInstance;

    uint32 m_uiSummonTimer;
    bool m_bIsSentinel;

    void Reset()
    {
        m_uiSummonTimer     = 8000;
        m_bIsSentinel       = false;
        DoCast(m_creature, SPELL_SUMMON_MOLE_MACHINE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_uiSummonTimer < diff)
        {
            // summon 2 dwarfes
            if(!m_bIsSentinel)
            {
                if (Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_WATCHER, m_creature->GetPositionX() + 5, m_creature->GetPositionY() + 5, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                {
                    pTemp->SetInCombatWithZone();
                    pTemp->GetMotionMaster()->MovePoint(0, HOME_X, HOME_Y, m_creature->GetPositionZ());
                }
                if (Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_GUARDIAN, m_creature->GetPositionX() - 5, m_creature->GetPositionY() - 5, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                {
                    pTemp->SetInCombatWithZone();
                    pTemp->GetMotionMaster()->MovePoint(0, HOME_X, HOME_Y, m_creature->GetPositionZ());
                }
            }
            // summon 1 sentinel
            else
            {
                if (Creature* pTemp = m_creature->SummonCreature(MOB_DARK_RUNE_SENTINEL, m_creature->GetPositionX() - 5, m_creature->GetPositionY() - 5, m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000))
                {
                    pTemp->SetInCombatWithZone();
                    pTemp->GetMotionMaster()->MovePoint(0, HOME_X, HOME_Y, m_creature->GetPositionZ());
                }
            }
            m_uiSummonTimer = 60000;
        }else m_uiSummonTimer -= diff;
    }
};

CreatureAI* GetAI_mob_mole_machine(Creature* pCreature)
{
    return new mob_mole_machineAI(pCreature);
}

//razorscale
struct MANGOS_DLL_DECL boss_razorscaleAI : public ScriptedAI
{
    boss_razorscaleAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_pInstance = (instance_ulduar*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ulduar* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiFireball_Timer;
    uint32 m_uiDevouring_Flame_Timer;
    uint32 m_uiFlame_Buffet_Timer;
    uint32 m_uiFuse_Armor_Timer;
    uint32 m_uiFlame_Breath_Timer;
    uint32 m_uiWave_spawn;
    uint32 m_uiBerserk_Timer;
    uint32 m_uiGroundStepTimer;
    uint32 m_uiGroundStepCount;
    uint32 m_uiRepairHarpoonTimer;
    
    uint8 m_uiHarpoonsRepaired;
    uint8 m_uiMaxHarpoons;
    uint8 m_uiHarpoonsUsed;
    uint8 m_uiFlyNo;
    uint8 m_uiScorchedDwarves;

    GUIDList m_lRepairHarpoonsGUID;
    GUIDList m_lHarpoonsDummyGUID;

    RazorscalePhase razorscalePhase;

    void Reset()
    {
        m_uiMaxHarpoons = m_bIsRegularMode ? 2 : 4;
        //if (m_pInstance->m_lBreakHarpoonGUID.size() != m_uiMaxHarpoons)
            //m_creature->MonsterSay("Fehler in der Suche der Harpoonen", LANG_UNIVERSAL);
        BreakHarpoons();

        m_uiFireball_Timer          = 10000;    // 10 secs for the first
        m_uiDevouring_Flame_Timer   = 18000;    // 18 secs first, 12 seconds after
        m_uiWave_spawn              = urand(5000, 10000);
        m_uiBerserk_Timer           = 600000;   // 10 min
        m_uiRepairHarpoonTimer      = 51000;
        m_uiHarpoonsRepaired        = 0;
        
        razorscalePhase = PHASE_AIR;
        m_uiFlyNo           = 0;
        m_uiHarpoonsUsed    = 0;
        m_uiScorchedDwarves = 0;
        m_creature->SetWalk(true);
        SetCombatMovement(false);
        //  make boss fly
        m_creature->SetLevitate(true);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);

        m_creature->GetMotionMaster()->MoveIdle();

        if (Creature* pCommander = m_pInstance->GetSingleCreatureFromStorage(NPC_COMMANDER))
            pCommander->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, DONE);

        BreakHarpoons();
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, IN_PROGRESS);
        m_creature->MonsterMoveWithSpeed(PositionLoc[4].x, PositionLoc[4].y, PositionLoc[4].z, 28);
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_FLAME_BREATH || pSpell->Id == SPELL_FLAME_BREATH_H)
        {
            if (pTarget->GetEntry() == MOB_DARK_RUNE_GUARDIAN) // only this one
            {
                 ++m_uiScorchedDwarves;
                 if (m_uiScorchedDwarves > 25)
                 {
                     if (m_pInstance)
                         m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_IRON_DWARF_MEDIUM_RARE, true);
                 }
                 m_creature->DealDamage(pTarget, pTarget->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAZORSCALE, FAIL);
    }

    void BreakHarpoons()
    {
        // reset harpoons
        for (GUIDVector::iterator iter = m_pInstance->m_lBreakHarpoonGUID.begin(); iter != m_pInstance->m_lBreakHarpoonGUID.end(); ++iter)
        {
            if (GameObject* pBreakHarpoon = m_creature->GetMap()->GetGameObject(*iter))
            {
                pBreakHarpoon->SetPhaseMask(1, true);                
            }
        }
        for (GUIDList::iterator iter = m_lRepairHarpoonsGUID.begin(); iter != m_lRepairHarpoonsGUID.end(); ++iter)
        {
            if (GameObject* pRepairHarpoon = m_creature->GetMap()->GetGameObject(*iter))
            {
                pRepairHarpoon->Delete();
            }
        }
        for (GUIDList::iterator iter = m_lHarpoonsDummyGUID.begin(); iter != m_lHarpoonsDummyGUID.end(); ++iter)
        {
            if (Creature* pHarpoonDummy = m_creature->GetMap()->GetCreature(*iter))
            {
                pHarpoonDummy->ForcedDespawn();
            }
        }
        m_lHarpoonsDummyGUID.clear();
        m_lRepairHarpoonsGUID.clear();
    }

    void RepairHarpoons()
    {
        if(GameObject* pHarpoon = m_creature->GetMap()->GetGameObject(m_pInstance->m_lBreakHarpoonGUID.at(m_uiHarpoonsRepaired)))
        {
            switch(m_uiHarpoonsRepaired)
            {
                case 0:
                {
                    if (GameObject* pRepairHarpoon =  pHarpoon->SummonGameobject(GO_REPAIR_HARPOON_1, pHarpoon->GetPositionX(),pHarpoon->GetPositionY(),pHarpoon->GetPositionZ(), 4.732974f, 0))
                    {
                        m_lRepairHarpoonsGUID.push_back(pRepairHarpoon->GetObjectGuid());
                    }
                    break;
                }
                case 1:
                {
                    if (GameObject* pRepairHarpoon =  pHarpoon->SummonGameobject(GO_REPAIR_HARPOON_2, pHarpoon->GetPositionX(),pHarpoon->GetPositionY(),pHarpoon->GetPositionZ(), 5.269379f, 0))
                    {
                        m_lRepairHarpoonsGUID.push_back(pRepairHarpoon->GetObjectGuid());
                    }
                    break;
                }
                case 2:
                {
                    if (GameObject* pRepairHarpoon =  pHarpoon->SummonGameobject(GO_REPAIR_HARPOON_3, pHarpoon->GetPositionX(),pHarpoon->GetPositionY(),pHarpoon->GetPositionZ(), pHarpoon->GetOrientation(), 0))
                        m_lRepairHarpoonsGUID.push_back(pRepairHarpoon->GetObjectGuid());
                    break;
                }
                case 3:
                {
                    if (GameObject* pRepairHarpoon =  pHarpoon->SummonGameobject(GO_REPAIR_HARPOON_4, pHarpoon->GetPositionX(),pHarpoon->GetPositionY(),pHarpoon->GetPositionZ(), pHarpoon->GetAngle(m_creature), 0))
                        m_lRepairHarpoonsGUID.push_back(pRepairHarpoon->GetObjectGuid());
                    break;
                }
            }
            if (Creature* pHarpoonDummy = pHarpoon->SummonCreature(NPC_HARPOONS_DUMMY, pHarpoon->GetPositionX(), pHarpoon->GetPositionY(), pHarpoon->GetPositionZ(),0 , TEMPSUMMON_DEAD_DESPAWN, 0))
            {
                m_lHarpoonsDummyGUID.push_back(pHarpoonDummy->GetObjectGuid());
            }
            pHarpoon->SetPhaseMask(128,true);
        }
    }

    void SetToGroundPhase()
    {
        if(Creature* pCommander = m_pInstance->GetSingleCreatureFromStorage(NPC_COMMANDER))
            DoScriptText(SAY_GROUND, pCommander);

        // make boss land
        m_creature->SetLevitate(false);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);

        // ground position
        m_creature->MonsterMoveWithSpeed(PositionLoc[3].x, PositionLoc[3].y, PositionLoc[3].z, 40.0f);

        // timers
        m_uiHarpoonsUsed    = 0;
        m_uiGroundStepTimer = 2000;
        m_uiGroundStepCount = 0;
        razorscalePhase = PHASE_GROUND;
    }

    void SetToAirPhase()
    {
        if(Creature* pCommander = m_pInstance->GetSingleCreatureFromStorage(NPC_COMMANDER))
            DoScriptText(SAY_FIRES_EXTINGUISH, pCommander);
        //  make boss fly
        m_creature->SetLevitate(true);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
        razorscalePhase             = PHASE_AIR;
        m_uiFireball_Timer          = 10000;
        m_uiDevouring_Flame_Timer   = 18000;
        m_uiWave_spawn              = urand(5000, 10000);
        m_uiRepairHarpoonTimer      = 50000;
        m_uiHarpoonsRepaired        = 0;
        
        // achiev counter
        m_uiFlyNo++;
        if (m_uiFlyNo > 1)
        {
            if (m_pInstance)
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_QUICK_SHAVE, false);
        }
    }

    void SetToPermGroundedPhase()
    {
        if (m_creature->HasAura(SPELL_STUN))
            m_creature->RemoveAurasDueToSpell(SPELL_STUN);

        DoScriptText(EMOTE_GROUNDED, m_creature);
        razorscalePhase             = PHASE_PERMAGROUND;
        m_uiDevouring_Flame_Timer   = 12000;
        m_uiFlame_Buffet_Timer      = 10000; //every 10 secs
        m_uiFuse_Armor_Timer        = 13000; //every ~13
        m_uiFlame_Breath_Timer      = 6000;  //every 14
        SetCombatMovement(true);
        BreakHarpoons();
        //  make boss land
        m_creature->SetLevitate(false);
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, 0);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // make boss land at 50% hp
        if (m_creature->GetHealthPercent() < 50 && razorscalePhase != PHASE_PERMAGROUND)
        {
            SetToPermGroundedPhase();
        }

        switch(razorscalePhase)
        {
            case PHASE_AIR:
            {
                // Switch from Air to Ground Phase
                if (m_uiHarpoonsUsed == m_uiMaxHarpoons)
                {
                    SetToGroundPhase();
                    return;
                }
                // air position check (sometimes it falls to the ground like a rock
                if(m_creature->GetPositionZ() < 440.0f)
                {
                    m_creature->MonsterMoveWithSpeed(PositionLoc[4].x, PositionLoc[4].y, PositionLoc[4].z, 28);
                }

                // air spells
                if (m_uiFireball_Timer < uiDiff)
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
                        DoCast(target, m_bIsRegularMode ? SPELL_FIREBALL : SPELL_FIREBALL_H);
                        m_uiFireball_Timer = 2000;
                }else
                    m_uiFireball_Timer -= uiDiff;

                if (m_uiDevouring_Flame_Timer < uiDiff)
                {
                    if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, (uint32)0, SELECT_FLAG_PLAYER))
                        DoCast(target, DEVOURING_FLAME_MISSILE);
                        m_uiDevouring_Flame_Timer = 12000;
                }else
                    m_uiDevouring_Flame_Timer -= uiDiff;

                // repair harpoons
                if (m_uiRepairHarpoonTimer < uiDiff && m_uiHarpoonsRepaired < m_uiMaxHarpoons && m_uiHarpoonsRepaired < m_pInstance->m_lBreakHarpoonGUID.size()) // i know the double check
                {
                    RepairHarpoons();
                    ++m_uiHarpoonsRepaired;
                    DoScriptText(EMOTE_HARPOON, m_creature);
                    m_uiRepairHarpoonTimer = 20000;
                }
                else
                    m_uiRepairHarpoonTimer -= uiDiff;

                break;
            }
            case PHASE_GROUND:
            {
                if (m_uiGroundStepTimer < uiDiff)
                {
                    switch (m_uiGroundStepCount)
                    {
                        case 0:
                            DoCast(m_creature, SPELL_STUN, true);
                            m_uiGroundStepTimer = 33000;
                            break;
                        case 1:
                            if (Creature* pCommander = m_pInstance->GetSingleCreatureFromStorage(NPC_COMMANDER))
                                m_creature->SetFacingToObject(pCommander);
                            m_creature->RemoveAurasDueToSpell(SPELL_STUN);
                            DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                            DoCast(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H);
                            m_uiGroundStepTimer    = 7000;
                            break;
                        case 2:
                            DoCast(m_creature, SPELL_WING_BUFFET);
                            BreakHarpoons();
                            m_uiGroundStepTimer = 4000;
                            break;
                        case 3:
                            SetToAirPhase();
                            m_uiGroundStepTimer = 4000;
                            break;
                        default: // debug next action Fly 
                            m_uiGroundStepCount = 2;
                            m_creature->RemoveAurasDueToSpell(SPELL_STUN);
                            m_uiGroundStepTimer = 4000;
                            break;
                    }
                    ++m_uiGroundStepCount;
                }
                else
                    m_uiGroundStepTimer -= uiDiff;

                break;
            }
            case PHASE_PERMAGROUND:
            {
                if (m_uiDevouring_Flame_Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), DEVOURING_FLAME_MISSILE) == CAST_OK)
                        m_uiDevouring_Flame_Timer = 12000;
                }
                else
                    m_uiDevouring_Flame_Timer -= uiDiff;

                if (m_uiFuse_Armor_Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_FUSE_ARMOR) == CAST_OK)
                        m_uiFuse_Armor_Timer = 13000;
                }
                else
                    m_uiFuse_Armor_Timer -= uiDiff;

                if (m_uiFlame_Buffet_Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_BUFFET : SPELL_FLAME_BUFFET_H) == CAST_OK)
                        m_uiFlame_Buffet_Timer = 13000;
                }
                else
                    m_uiFlame_Buffet_Timer -= uiDiff;

                if (m_uiFlame_Breath_Timer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_FLAME_BREATH : SPELL_FLAME_BREATH_H) == CAST_OK)
                    {
                        DoScriptText(EMOTE_DEEP_BREATH, m_creature);
                        m_uiFlame_Breath_Timer = 14000;
                    }
                }
                else
                    m_uiFlame_Breath_Timer -= uiDiff;

                DoMeleeAttackIfReady();

                break;
            }
            default:
                EnterEvadeMode();
        }

        // ground adds only in Air and grounded phase NOT in Permagrounded Phase
        if (m_uiWave_spawn < uiDiff && razorscalePhase != PHASE_PERMAGROUND)
        {
            m_creature->SummonCreature(NPC_MOLE_MACHINE, PositionLoc[0].x, PositionLoc[0].y, PositionLoc[0].z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
            m_creature->SummonCreature(NPC_MOLE_MACHINE, PositionLoc[1].x, PositionLoc[1].y, PositionLoc[1].z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
            if (roll_chance_i(33))
            {
                if(Creature* pTemp = m_creature->SummonCreature(NPC_MOLE_MACHINE, PositionLoc[2].x, PositionLoc[2].y, PositionLoc[2].z, 0, TEMPSUMMON_TIMED_DESPAWN, 15000))
                    if (mob_mole_machineAI* molAI = (mob_mole_machineAI*)pTemp->AI())
                        molAI->m_bIsSentinel = true;
            }
            m_uiWave_spawn = urand(40000, 50000);
        }
        else
            m_uiWave_spawn -= uiDiff;

        // berserk
        if (m_uiBerserk_Timer < uiDiff)
        {
            DoCastSpellIfCan(m_creature, SPELL_BERSERK, CAST_TRIGGERED | CAST_AURA_NOT_PRESENT);
        }
        else
            m_uiBerserk_Timer -= uiDiff;

        if (m_creature->GetDistance2d(HOME_X, HOME_Y) > 100)
            EnterEvadeMode();
    }
};

CreatureAI* GetAI_boss_razorscale(Creature* pCreature)
{
    return new boss_razorscaleAI(pCreature);
}

bool GOHello_go_repair_harpoon(Player* pPlayer, GameObject* pGo)
{
    instance_ulduar* m_pInstance = (instance_ulduar*)pGo->GetInstanceData();

    if (!m_pInstance)
        return false;
    if (Creature* pRazor = m_pInstance->GetSingleCreatureFromStorage(NPC_RAZORSCALE))
        if (boss_razorscaleAI* pRazorAI = (boss_razorscaleAI*)pRazor->AI())
            pRazorAI->m_uiHarpoonsUsed++;

    return false;
}

void AddSC_boss_razorscale()
{
    Script* NewScript;

    NewScript = new Script;
    NewScript->Name = "boss_razorscale";
    NewScript->GetAI = GetAI_boss_razorscale;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_devouring_flame_target";
    NewScript->GetAI = &GetAI_mob_devouring_flame_target;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_dark_rune_watcher";
    NewScript->GetAI = &GetAI_mob_dark_rune_watcher;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_dark_rune_sentinel";
    NewScript->GetAI = &GetAI_mob_dark_rune_sentinel;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_dark_rune_guardian";
    NewScript->GetAI = &GetAI_mob_dark_rune_guardian;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "mob_mole_machine";
    NewScript->GetAI = &GetAI_mob_mole_machine;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "npc_expedition_commander";
    NewScript->GetAI = &GetAI_npc_expedition_commander;
    NewScript->pGossipHello = &GossipHello_npc_expedition_commander;
    NewScript->pGossipSelect = &GossipSelect_npc_expedition_commander;
    NewScript->RegisterSelf();

    NewScript = new Script;
    NewScript->Name = "go_repair_harpoon";
    NewScript->pGOUse = &GOHello_go_repair_harpoon;
    NewScript->RegisterSelf();
}
