/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
 * Copyright (C) 2011 - 2012 MangosR2 <http://github.com/mangosR2/>
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
SDName: Boss_Volazj
SD%Complete: 60%
SDComment: Insanity does not work blizzlike - custom event made, shiver need core support, timers
SDAuthor: Tassadar
SDCategory: Ahn'kahet
EndScriptData */

#include "precompiled.h"
#include "ahnkahet.h"

enum
{
    SPELL_MIND_FLAY                 = 57941,
    SPELL_MIND_FLAY_H               = 59974,
    SPELL_SHADOW_BOLT               = 57942,
    SPELL_SHADOW_BOLT_H             = 59975,

    //Shiver - horrible ability
    SPELL_SHIVER                    = 57949, //Jump Aura 
    SPELL_SHIVER_H                  = 59978,
    SPELL_SHIVER_DMG                = 57952, //Damage
    SPELL_SHIVER_DMG_H              = 59979,
    SPELL_SHIVER_DUMMY              = 57951, //What is this? 

    //This is little complicated:
    //When volajz cast this, on every player is cast different invisibility spell,
    //so they dont see together, but they see four Twisted Visages - images of other
    //four party members, which cast spell like their class.
    SPELL_INSANITY                  = 57496, //This is what volajz casts, it should trigger Twisted Visage spawn spells
    SPELL_INSANITY_PHASE            = 57507, //For use in code
    SPELL_INSANITY_PHASE_1          = 57508, //invis spells
    SPELL_INSANITY_PHASE_2          = 57509,
    SPELL_INSANITY_PHASE_3          = 57510,
    SPELL_INSANITY_PHASE_4          = 57511,
    SPELL_INSANITY_PHASE_5          = 57512,
    SPELL_INSANITY_CHANNEL          = 57561, //Just for visual, Volazj cast this when players are in insanity

    SPELL_TWISTED_VISAGE_MIRROR     = 57507, //Not implented in mangos, but I have patch :)

    /*
    http://www.wowhead.com/?spell=57507 Twisted visage visual
    http://www.wowhead.com/?spells=0&filter=na=twisted+visage so many spells?!
    */

    //Ancient void spells
    SPELL_PRISON                    = 43312,
    SPELL_BLUE_FLAMES               = 42586,
    SPELL_RED_FLAMES                = 42346,
    SPELL_BLUE_BEAM                 = 32840,
    SPELL_SHAKE                     = 44681,
    SPELL_PSYCHIC_SCREAM            = 34322,

    NPC_TWISTED_VISAGE              = 30621,
    NPC_ANCIENT_VOID                = 30622, //Custom 

    MODEL_VOID_LIGHT                = 19702,
    MODEL_VOID_DARK                 = 1132,

    SAY_AGGRO                       = -1619033,
    SAY_INSANITY                    = -1619034,
    SAY_SLAY_1                      = -1619035,
    SAY_SLAY_2                      = -1619036,
    SAY_SLAY_3                      = -1619037,
    SAY_DEATH_1                     = -1619038,
    SAY_DEATH_2                     = -1619039,
    SAY_ANCIENT_VOID                = -1619040,

    SAY_VOID_CORRUPT                = -1619041,
    SAY_VOID_AGGRO                  = -1619042,

    PHASE_NOSTART                   = 0,
    PHASE_FIGHT                     = 1,
    PHASE_INSANITY_1                = 2,  // Wait five seconds until cast is complete, set unattackable
    PHASE_INSANITY_2                = 3, 
    PHASE_INSANITY_3                = 4, 
};
struct Locations
{
    float x, y, z;
    uint32 id;
};
static Locations SpawnLoc[]=
{
    //Around room
    {571.48f, -494.01f, 26.35f},
    {562.58f, -526.48f, 28.97f},
    {524.80f, -552.07f, 26.66f},
    {494.62f, -533.39f, 28.99f},
    {470.88f, -498.74f, 26.35f},
    {483.96f, -468.54f, 28.97f},
    {519.23f, -441.54f, 26.35f},
    {548.45f, -456.50f, 29.00f},

    //Center of the room
    {521.67f, -496.17f, 27.01f}

};
/*######
## boss_volazj
######*/

struct MANGOS_DLL_DECL boss_volazjAI : public ScriptedAI
{
    boss_volazjAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPhase;
    ObjectGuid m_uiLastShiverTargetGUID;
    uint8 m_uiShiverJumpTimer;
    uint8 m_uiLastSacrifaceHP;

    uint32 m_uiMindFlayTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiShiverTimer;
    uint32 m_uiCheckTimer;

    //Insanity
    uint32 m_uiInsanityCastTimer;

    void Reset()
    {
        m_uiPhase = PHASE_NOSTART;
        m_uiLastSacrifaceHP = 0;

        m_uiMindFlayTimer = 10000;
        m_uiShadowBoltTimer = 5000;
        m_uiShiverTimer = 18000;
        m_uiCheckTimer = 1000;
        m_uiShiverJumpTimer = 0;

        m_creature->SetRespawnDelay(DAY);

        //Insanity
        m_uiInsanityCastTimer = 5000;

        if (m_pInstance && m_pInstance->GetData(TYPE_VOLAZJ) != DONE)
            m_pInstance->SetData(TYPE_VOLAZJ, NOT_STARTED);
    }

    void Aggro(Unit* pWho)
    {
        DoScriptText(SAY_AGGRO, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLAZJ, IN_PROGRESS);
        m_uiPhase = PHASE_FIGHT;

        Map* pMap = m_creature->GetMap();
    }

    void EnterEvadeMode()
    {
        if(m_uiPhase != PHASE_FIGHT)
            return;

        m_creature->GetMotionMaster()->MoveTargetedHome();
        m_pInstance->SetData(TYPE_VOLAZJ, FAIL);
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(urand(0, 1) ? SAY_DEATH_1 : SAY_DEATH_2, m_creature);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_VOLAZJ, DONE);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiPhase == PHASE_FIGHT)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            //Spells
            //Mind Flay
            if(m_uiMindFlayTimer <= uiDiff)
            {
                DoCast(m_creature->getVictim(), m_bIsRegularMode ? SPELL_MIND_FLAY : SPELL_MIND_FLAY_H);
                m_uiMindFlayTimer = 10000 + rand()%10000;
            }else m_uiMindFlayTimer -= uiDiff;

            //Shadowbolt voley
            if(m_uiShadowBoltTimer <= uiDiff)
            {
                DoCast(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H);
                m_uiShadowBoltTimer = 8000 + rand()%5000;
            }else m_uiShadowBoltTimer -= uiDiff;

            //Shiver
            if(m_uiShiverTimer <= uiDiff)
            {
                DoCast(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0), m_bIsRegularMode ? SPELL_SHIVER : SPELL_SHIVER_H);
                m_uiShiverTimer = 30000;
            }else m_uiShiverTimer -= uiDiff;

            //Health check
            if(m_uiCheckTimer <= uiDiff)
            {
                uint8 health = m_creature->GetHealth()*100 / m_creature->GetMaxHealth();
                if(m_uiLastSacrifaceHP == 0 && health <= 50)
                {
                    m_creature->InterruptNonMeleeSpells(true);
                    SetCombatMovement(false);    
                    m_uiLastSacrifaceHP = 50;
                    DoCast(m_creature, SPELL_INSANITY);
                    m_uiPhase = PHASE_INSANITY_1;
                    return;
                }
                m_uiCheckTimer = 1000;
            }else m_uiCheckTimer -= uiDiff;  

            DoMeleeAttackIfReady();
        }else if(m_uiPhase == PHASE_INSANITY_1)
        {
            //Wait until cast is complete
            if(m_uiInsanityCastTimer <= uiDiff)
            {
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                DoCast(m_creature, SPELL_INSANITY_CHANNEL);
                DoInsanity();
                m_uiInsanityCastTimer = 5000;
                m_uiCheckTimer = 5000;
                m_uiPhase = PHASE_INSANITY_2;
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MovementExpired(false);
                DoScriptText(SAY_ANCIENT_VOID, m_creature);
            }else m_uiInsanityCastTimer -= uiDiff;
        }else if(m_uiPhase == PHASE_INSANITY_2)
        {
            if(m_uiCheckTimer <= uiDiff)
            {
                if(Creature *pTemp = GetClosestCreatureWithEntry(m_creature, NPC_ANCIENT_VOID, 150.0f))
                {
                    if(!pTemp->isAlive())
                       m_uiPhase = PHASE_INSANITY_3;
                }else m_uiPhase = PHASE_INSANITY_3;
                m_uiCheckTimer = 1000;
            }else m_uiCheckTimer -= uiDiff; 
        }else if(m_uiPhase == PHASE_INSANITY_3)
        {
            m_creature->RemoveAurasDueToSpell(SPELL_INSANITY_CHANNEL);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
            SetCombatMovement(true);
            m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            m_uiPhase = PHASE_FIGHT;
        }

    }
    //This do everything which is needed by Insanity spell (CUSTOM)
    void DoInsanity()
    {
        m_creature->SummonCreature(NPC_ANCIENT_VOID, SpawnLoc[8].x, SpawnLoc[8].y, SpawnLoc[8].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);

        for(int i = 0; i <= 7; ++i)
            m_creature->SummonCreature(NPC_TWISTED_VISAGE, SpawnLoc[i].x, SpawnLoc[i].y, SpawnLoc[i].z, 0, TEMPSUMMON_CORPSE_DESPAWN, 0);

    }
};

/*######
## mob_twisted_visage
######*/

struct MANGOS_DLL_DECL mob_twisted_visageAI : public ScriptedAI
{
    mob_twisted_visageAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;

    void Reset()
    {
        m_creature->GetMotionMaster()->MovePoint(0, SpawnLoc[8].x, SpawnLoc[8].y, SpawnLoc[8].z);
    }
    void AttackStart(Unit *pWho)
    {
        return;
    }
    void MovementInform(uint32 uiType, uint32 uiPointId)
    {
        if(uiType != POINT_MOTION_TYPE)
                return;

        switch(uiPointId)
        {
            case 0:
                if(Creature *pVoid = GetClosestCreatureWithEntry(m_creature, NPC_ANCIENT_VOID, 30.0f))
                {
                    float newsize = pVoid->GetFloatValue(OBJECT_FIELD_SCALE_X) + 0.25f;
                    uint32 health = pVoid->GetHealth() + 20000;
                    pVoid->SetHealth(health);
                    pVoid->SetFloatValue(OBJECT_FIELD_SCALE_X, newsize);
                }
                m_creature->ForcedDespawn();
                break;
        }
    }
    void UpdateAI(const uint32 uiDiff)
    {

    }
};

/*######
## mob_ancient_void
######*/

struct MANGOS_DLL_DECL mob_ancient_voidAI : public ScriptedAI
{
    mob_ancient_voidAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_ahnkahet*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        defaultsize = m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X);
        Reset();
    }

    instance_ahnkahet* m_pInstance;
    bool m_bIsRegularMode;
    uint8 m_uiPhase;
    float defaultsize;
    uint8 m_uiTransformPhase;
    uint32 m_uiDelayTimer;
    uint32 m_uiPhysicScreamTimer;
    uint32 m_uiShadowBoltTimer;
    uint32 m_uiOutOfCombatTimer;
    float m_fVisages;

    void Reset()
    {
        m_fVisages = 0.0f;
        m_uiDelayTimer = 1000;
        m_uiOutOfCombatTimer = 20000;
        m_uiPhase = 1;
        m_uiTransformPhase = 1;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_STUN);
        m_creature->SetDisplayId(MODEL_VOID_LIGHT);
        SetCombatMovement(false);
        m_creature->SetFloatValue(OBJECT_FIELD_SCALE_X, defaultsize);
        DoCast(m_creature, SPELL_PRISON);
        DoCast(m_creature, SPELL_BLUE_BEAM, true);

        m_uiPhysicScreamTimer = 0;
        m_uiShadowBoltTimer = 8000;
        if(m_bIsRegularMode)
            m_creature->SetHealth(200000);
        else
            m_creature->SetHealth(300000);
    }
    void DoTransform(uint8 phase)
    {
        switch(phase)
        {
            case 1:
                m_fVisages = (m_creature->GetFloatValue(OBJECT_FIELD_SCALE_X) - 1.0f) / 0.25;
                SetPhysicScreamTimer();
                DoCast(m_creature, SPELL_BLUE_FLAMES);
                DoScriptText(SAY_VOID_CORRUPT, m_creature);
                m_creature->RemoveAurasDueToSpell(SPELL_PRISON);
                break;
            case 2:
                DoCast(m_creature, SPELL_RED_FLAMES);
                m_creature->RemoveAurasDueToSpell(SPELL_BLUE_BEAM);
                break;
            case 3:
                DoCast(m_creature, SPELL_SHAKE);
                m_creature->SetDisplayId(MODEL_VOID_DARK);
                break;
            case 4:
                m_creature->RemoveAurasDueToSpell(SPELL_RED_FLAMES);
                break;
            case 5:
                DoScriptText(SAY_VOID_AGGRO, m_creature);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                SetCombatMovement(true);
                if(m_creature->getVictim())
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                m_uiPhase = 3;
                break;
        }
    }
    void SetPhysicScreamTimer()
    {
        if(m_fVisages < 3)
            m_uiPhysicScreamTimer = 23000;
        else if(m_fVisages < 5)
            m_uiPhysicScreamTimer = 17000;
        else if(m_fVisages < 7)
            m_uiPhysicScreamTimer = 11000;
        else if(m_fVisages < 9)
            m_uiPhysicScreamTimer = 5000;
    }
    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiPhase == 1)
        {
            if(m_uiDelayTimer <= uiDiff)
            {
                if(!GetClosestCreatureWithEntry(m_creature, NPC_TWISTED_VISAGE, 150.0f))
                    m_uiPhase = 2;
                m_uiDelayTimer = 1000;
            }else m_uiDelayTimer -= uiDiff;
            return;
        }
        if(m_uiPhase == 2)
        {
            if(m_uiDelayTimer <= uiDiff)
            {
                DoTransform(m_uiTransformPhase);
                m_uiTransformPhase++;
                m_uiDelayTimer = 2500;
            }else m_uiDelayTimer -= uiDiff;
        }

        if(m_uiPhase != 3)
            return;
        if(!m_creature->isInCombat())
        {
            if(m_uiOutOfCombatTimer <= uiDiff)
                m_creature->ForcedDespawn();
            else m_uiOutOfCombatTimer -= uiDiff;

            return;
        }

        if(m_uiPhysicScreamTimer <= uiDiff)
        {
            DoCast(m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0),SPELL_PSYCHIC_SCREAM);
            SetPhysicScreamTimer();
        }else m_uiPhysicScreamTimer -= uiDiff;

        //Shadowbolt voley
        if(m_uiShadowBoltTimer <= uiDiff)
        {
            DoCast(m_creature, m_bIsRegularMode ? SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT_H);
            m_uiShadowBoltTimer = 8000;
        }else m_uiShadowBoltTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};
CreatureAI* GetAI_boss_volazj(Creature* pCreature)
{
    return new boss_volazjAI(pCreature);
}

CreatureAI* GetAI_mob_twisted_visage(Creature* pCreature)
{
    return new mob_twisted_visageAI(pCreature);
}

CreatureAI* GetAI_mob_ancient_void(Creature* pCreature)
{
    return new mob_ancient_voidAI(pCreature);
}

void AddSC_boss_volazj()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_volazj";
    pNewScript->GetAI = &GetAI_boss_volazj;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_twisted_visage";
    pNewScript->GetAI = &GetAI_mob_twisted_visage;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "mob_ancient_void";
    pNewScript->GetAI = &GetAI_mob_ancient_void;
    pNewScript->RegisterSelf();
}
