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
SDName: boss_shannox
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

enum Shannox_Yells
{
    SAY_AGGRO                                    = -1999971,
    EMOTE_SOFT_ENRAGE                             = -1999972,
    SAY_ON_DOGS_FALL                             = -1999973,
    SAY_ON_DEAD                                     = -1999974,
    SAY_DOG_FOOD                                 = -1999975,
    SAY_FETCH_SUPPER                             = -1999976,
    SAY_GO_FOR_THROAT                             = -1999977,
    SAY_BURN_ONE                                 = -1999978,
    SAY_ON_KILL_ONE                                 = -1999979,
    SAY_RIPLIMB                                     = -1999980,
    SAY_ON_KILL_TWO                                 = -1999981,
    SAY_BURN_TWO                                 = -1999982,
    SAY_INTRO_SPECH_PART_ONE                     = -1999983,
    SAY_INTRO_SPECH_PART_TWO                     = -1999984,
};

enum ePhases
{
    PHASE_NON = 0, 

    PHASE_SHANNOX_HAS_SPEER = 1,
    PHASE_SPEAR_ON_THE_GROUND = 2,
    PHASE_RIPLIMB_GOS_TO_SPEER = 3,
    PHASE_RIPLIMB_BRINGS_SPEER = 4,
};

enum Actions
{
    ACTION_SAY_RIPLIMB, //Shannox yell when Riplimb bites someone.
    ACTION_START_EVENT_TO_RESPAWN_RIPLIMB, // Used by Riplimb when he dies to start Shannox respawn event.
};

enum Spells
{
    //Shannox
    SPELL_ARCTIC_SLASH_10N = 99931,
    SPELL_ARCTIC_SLASH_25N = 101201,
    SPELL_ARCTIC_SLASH_10H = 101202,
    SPELL_ARCTIC_SLASH_25H = 101203,

    SPELL_BERSERK = 26662,

    SPELL_CALL_SPEAR = 100663,
    SPELL_HURL_SPEAR = 100002, // Dummy Effect & Damage.
    SPELL_HURL_SPEAR_SUMMON = 99978, //Summons Spear of Shannox.
    SPELL_MAGMA_RUPTURE_SHANNOX = 99840,

    SPELL_FRENZY_SHANNOX = 100522,

    // Riplimb
    SPELL_LIMB_RIP = 99832,
    SPELL_DOGGED_DETERMINATION = 101111,

    // Rageface
    SPELL_FACE_RAGE = 99947,
    SPELL_RAGE = 100415,

    SPELL_FACE_RAGE_10N = 100129, // Buff to remove damage aura.
    SPELL_FACE_RAGE_25N = 101212,
    SPELL_FACE_RAGE_10H = 101213,
    SPELL_FACE_RAGE_25H = 101214,

    // Both Dogs
    SPELL_FRENZIED_DEVOLUTION = 100064,
    SPELL_FEEDING_FRENZY_H = 100655,

    SPELL_WARY_10N = 100167, // Buff when the Dog goes into a Trap.
    SPELL_WARY_25N = 101215,
    SPELL_WARY_10H = 101216,
    SPELL_WARY_25H = 101217,

    // Misc
    SPELL_SEPERATION_ANXIETY = 99835,

    //Spear Abilities
    SPELL_MAGMA_FLARE = 100495, // Inflicts Fire damage to enemies within 50 yards.
    SPELL_MAGMA_RUPTURE = 100003, // Calls forth magma eruptions to damage nearby foes. (Dummy Effect)
    SPELL_MAGMA_RUPTURE_VISUAL = 99841,
    RED_TARGET_AUR = 99988,
    SPEAR_VISIBLE_ON_GROUND = 100005,
    SPEAR_VISIBLE_FETCH = 100026,

    //Trap Abilities
    SPELL_SUMMON_CRYSTAL_PRISSON = 99836,
    CRYSTAL_PRISON_EFFECT = 99837,
    SPELL_SUMMON_IMMOLATION_TRAP = 99839,

    SPELL_IMMOLATION_TRAP_10N = 99838, 
    SPELL_IMMOLATION_TRAP_25N = 101208,
    SPELL_IMMOLATION_TRAP_10H = 101209,
    SPELL_IMMOLATION_TRAP_25H = 101210,

};

enum Events
{
    //Shannox
    EVENT_SUMMON_IMMOLATION_TRAP = 1, // Every 10s
    EVENT_BERSERK = 2, // After 10m
    EVENT_ARCING_SLASH = 3, // Every 12s
    EVENT_HURL_SPEAR_OR_MAGMA_RUPTUTRE = 4, // Every 42s
    EVENT_HURL_SPEAR = 5,
    EVENT_HURL_SPEAR_2 = 6,
    EVENT_SUMMON_CRYSTAL_PRISON = 7, // Every 25s
    EVENT_SUMMON_SPEAR = 8,
    EVENT_DESPAWN_SPEAR = 9,

    //Riplimb
    EVENT_LIMB_RIP = 10,
    EVENT_RIPLIMB_RESPAWN_H = 11,
    EVENT_TAKING_SPEAR_DELAY = 12,

    //Rageface
    EVENT_FACE_RAGE = 13, // Every 31s
    EVENT_CHANGE_TARGET = 14,

    // Trigger for the Crystal Trap
    EVENT_CRYSTAL_TRAP_TRIGGER = 15,

    // Trigger for the Immolation Trap
    EVENT_IMMOLATION_TRAP_TRIGGER = 16,

    // Trigger for self Dispawn (Crystal Prison)
    EVENT_CRYSTAL_PRISON_DESPAWN = 17,

    // Event trigger to reset Yells infight
    EVENT_RIPLIMB_RESET_SHANNOX_YELL = 18,
    EVENT_SHANNOX_RESET_INTRO_YELL = 19,
    EVENT_SHANNOX_SEC_INTRO_YELL = 20,

    EVENT_PRISON_DOG_ATTACK_RAGEFACE = 21,
    EVENT_PRISON_DOG_ATTACK_RIPLIMB = 22,

    // Misc
    EVENT_UPDATE_MOTION = 23,

};

Position2 const bucketListPositions[5] =  {{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f},
{0.0f,0.0f,0.0f,0.0f}};

// Dogs Walking Distance to Shannox
const float walkRagefaceDistance = 8;
const float walkRagefaceAngle = 7;

const float walkRiplimbDistance = 9;
const float walkRiplimbAngle = 6;

// If the Distance between Shannox & Dogs > This Value, all 3 get the Seperation Buff
const float maxDistanceBetweenShannoxAndDogs = 110;

// The equipment Template of Shannox Spear
const int ShannoxSpearEquipmentTemplate = 53000;

struct MANGOS_DLL_DECL boss_shannoxAI : public ScriptedAI
{
    boss_shannoxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    bool bucketListCheckPoints[5];
    bool hurlSpeer;
    bool introSpeechDone;
    std::list<ObjectGuid> summons;

    EventMap events;

    void Reset() override
    {
        // TODO Add not Tauntable Flag

        introSpeechDone = false;
        uiPhase = PHASE_NON;

        m_creature->RemoveAllAuras();
        hurlSpeer = false;
        events.Reset();

        DespawnCreatures(NPC_CRYSTAL_PRISON, 5000.0f);
        DespawnCreatures(NPC_SHANNOX_SPEAR, 5000.0f);
        DespawnCreatures(NPC_FAKE_SHANNOX_SPEAR, 5000.0f);
        DespawnCreatures(NPC_CRYSTAL_TRAP, 5000.0f);

        if (Creature* riplimb = GetRiplimb())  // Prevents Crashes
        {
            riplimb->Respawn();
            ((ScriptedAI*)riplimb->AI())->Reset();
        }
        else
            m_creature->SummonCreature(NPC_RIPLIMB, m_creature->GetPositionX()-5 , m_creature->GetPositionY()-5 ,m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);

        if (Creature* rageface = GetRageface())  // Prevents Crashes
        {
            rageface->Respawn();
            ((ScriptedAI*)rageface->AI())->Reset();
        }
        else
            m_creature->SummonCreature(NPC_RAGEFACE, m_creature->GetPositionX()+5 , m_creature->GetPositionY()+5, m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);

        m_creature->LoadEquipment(ShannoxSpearEquipmentTemplate);
    }

    void SetData(uint32 type, uint32 data) override
    {
        switch (type)
        {
            case DATA_PHASE:
                uiPhase = data;
                break;
        }
    }

    uint32 GetData (uint32 id) override
    {
        switch (id)
        {
            case DATA_PHASE:
                return uiPhase;
        }
        return 0;
    }

    void DespawnCreatures(uint32 entry, float distance)
    {
        std::list<Creature*> creatures;
        GetCreatureListWithEntryInGrid(creatures, m_creature, entry, distance);

        if (creatures.empty())
            return;

        for (std::list<Creature*>::iterator iter = creatures.begin(); iter != creatures.end(); ++iter)
            (*iter)->ForcedDespawn();
    }
    
    void DespawnAllSummons()
    {
        for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* unit = m_creature->GetMap()->GetAnyTypeCreature(*itr))
                unit->ForcedDespawn();
    }

    void JustSummoned(Creature* who) override
    {
        summons.push_back(who->GetObjectGuid());
    }

    void Aggro(Unit* pWho) override
    {
        m_creature->SetInCombatWithZone();
        m_creature->CallForHelp(50.0f);
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHANNOX, IN_PROGRESS);

        events.ScheduleEvent(EVENT_SUMMON_IMMOLATION_TRAP, 10000);
        events.ScheduleEvent(EVENT_ARCING_SLASH, 12000);
        events.ScheduleEvent(EVENT_HURL_SPEAR_OR_MAGMA_RUPTUTRE, 20000);
        events.ScheduleEvent(EVENT_SUMMON_CRYSTAL_PRISON, 25000);
        events.ScheduleEvent(EVENT_BERSERK, 10 * MINUTE * IN_MILLISECONDS);

        m_creature->SetRespawnCoord(m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), m_creature->GetOrientation());

        DoScriptText(SAY_AGGRO, m_creature, pWho);

        uiPhase = PHASE_SHANNOX_HAS_SPEER;
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHANNOX, DONE);

        //DoScriptText(SAY_ON_DEAD, m_creature);
        DespawnAllSummons();

        if (Creature* rageface = GetRageface())
            rageface->ForcedDespawn();

        if (Creature* riplimb = GetRiplimb())
            riplimb->ForcedDespawn();

        uiPhase = PHASE_NON;
    }

    void KilledUnit(Unit* pVictim) override
    {
        //DoScriptText(urand(0, 1) ? SAY_ON_KILL_ONE : SAY_ON_KILL_TWO, m_creature);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_SHANNOX, FAIL);
    }

    void DoAction(const int32 action) override
    {
        switch(action)
        {
            case ACTION_SAY_RIPLIMB:
                //DoScriptText(SAY_RIPLIMB,m_creature);
                break;
            case ACTION_START_EVENT_TO_RESPAWN_RIPLIMB:
                events.ScheduleEvent(EVENT_RIPLIMB_RESPAWN_H, 30000);
                break;
        }
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (m_creature->IsNonMeleeSpellCasted(false))
            return;

        events.Update(uiDiff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_SUMMON_IMMOLATION_TRAP:
                if (Unit* tempTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(tempTarget,SPELL_SUMMON_IMMOLATION_TRAP);
                events.ScheduleEvent(EVENT_SUMMON_IMMOLATION_TRAP, 10000);
                break;

            case EVENT_BERSERK:
                DoCast(m_creature, SPELL_BERSERK);
                break;

            case EVENT_SHANNOX_RESET_INTRO_YELL:
                introSpeechDone = false;
                break;

            case EVENT_SHANNOX_SEC_INTRO_YELL:
                //DoScriptText(SAY_INTRO_SPECH_PART_TWO,m_creature);
                break;

            case EVENT_ARCING_SLASH:
                if (uiPhase == PHASE_SHANNOX_HAS_SPEER)
                {
                    DoCastVictim(SPELL_ARCTIC_SLASH_10N);
                    events.ScheduleEvent(EVENT_ARCING_SLASH, 12000);
                }
                else
                    events.ScheduleEvent(EVENT_ARCING_SLASH, 500);
                break;
            case EVENT_HURL_SPEAR:
                if (Creature *fakeSpear = GetClosestCreatureWithEntry(m_creature, NPC_FAKE_SHANNOX_SPEAR, 5000.0f, true))
                {
                    DoCast(fakeSpear,SPELL_HURL_SPEAR);
                    events.ScheduleEvent(EVENT_HURL_SPEAR_2, 1000);
                }
                break;
            case EVENT_HURL_SPEAR_2:
                if (Creature *fakeSpear = GetClosestCreatureWithEntry(m_creature, NPC_FAKE_SHANNOX_SPEAR, 5000.0f, true))
                {
                    // Shifts the Event back if Shannox has not the Spear yet
                    m_creature->LoadEquipment(0, true);

                    m_creature->SummonCreature(NPC_SHANNOX_SPEAR,fakeSpear->GetPositionX(),fakeSpear->GetPositionY(),fakeSpear->GetPositionZ(), 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);

                    DespawnCreatures(NPC_FAKE_SHANNOX_SPEAR, 5000.0f);
                }
                break;
            case EVENT_DESPAWN_SPEAR:
                m_creature->LoadEquipment(ShannoxSpearEquipmentTemplate);
                DespawnCreatures(NPC_SHANNOX_SPEAR, 5000.0f);
                break;
            case EVENT_SUMMON_SPEAR:
                if (Creature* spear = GetSpear())
                    DoCast(spear, SPELL_CALL_SPEAR);

                events.ScheduleEvent(EVENT_DESPAWN_SPEAR, 700);
                break;
            case EVENT_HURL_SPEAR_OR_MAGMA_RUPTUTRE:
            {
                Creature* riplimg = GetRiplimb();
                if(riplimg && riplimg->isDead())
                {
                    // Cast Magma Rupture when Ripclimb is Death
                    DoCastVictim(SPELL_MAGMA_RUPTURE_SHANNOX);
                    m_creature->LoadEquipment(0, true);
                    m_creature->SummonCreature(NPC_SHANNOX_SPEAR,m_creature->GetPositionX()-(cos(float(m_creature->GetOrientation()))+6),m_creature->GetPositionY()-(sin(float(m_creature->GetOrientation()))+6),m_creature->GetPositionZ(), m_creature->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
                    events.ScheduleEvent(EVENT_SUMMON_SPEAR, 4000);
                    events.ScheduleEvent(EVENT_HURL_SPEAR_OR_MAGMA_RUPTUTRE, 42000);
                }
                else
                {
                    // Throw Spear if Riplimb is Alive and Shannox has the Spear
                    if (uiPhase == PHASE_SHANNOX_HAS_SPEER && riplimg)
                    {
                        events.ScheduleEvent(EVENT_HURL_SPEAR_OR_MAGMA_RUPTUTRE, 42000);

                        m_creature->SummonCreature(NPC_FAKE_SHANNOX_SPEAR,riplimg->GetPositionX()+irand(-30,30),riplimg->GetPositionY()+irand(-30,30),riplimg->GetPositionZ(), riplimg->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);

                        if (Creature *fakeSpear = GetClosestCreatureWithEntry(m_creature, NPC_FAKE_SHANNOX_SPEAR, 5000.0f, true))
                            events.ScheduleEvent(EVENT_HURL_SPEAR, 2000);

                        uiPhase = PHASE_SPEAR_ON_THE_GROUND;
                        //DoCast(SPELL_HURL_SPEAR_DUMMY_SCRIPT);

                        DoScriptText(RAND(SAY_BURN_ONE,SAY_BURN_TWO), m_creature);

                    }
                    else
                        events.ScheduleEvent(EVENT_HURL_SPEAR_OR_MAGMA_RUPTUTRE, 10000);
                }

                break;
            }
            case EVENT_SUMMON_CRYSTAL_PRISON:
                if (Unit* tempTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    DoCast(tempTarget,SPELL_SUMMON_CRYSTAL_PRISSON);
                events.ScheduleEvent(EVENT_SUMMON_CRYSTAL_PRISON, 25000);
                break;

            case EVENT_RIPLIMB_RESPAWN_H:
                if (GetRiplimb())
                {
                    DoScriptText(SAY_FETCH_SUPPER, m_creature);
                    GetRiplimb()->Respawn();
                    GetRiplimb()->SetInCombatWithZone();
                }
                break;

            default:
                break;
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        Creature* riplimb = GetRiplimb();
        Creature* rageface = GetRageface();
        Creature* spear = GetSpear();
        if (((riplimb && riplimb->GetDistance2d(m_creature) >= maxDistanceBetweenShannoxAndDogs && riplimb->isAlive()) || (rageface && rageface->GetDistance2d(m_creature) >= maxDistanceBetweenShannoxAndDogs) && rageface->isAlive()) && !m_creature->HasAura(SPELL_SEPERATION_ANXIETY))
            DoCast(m_creature, SPELL_SEPERATION_ANXIETY);

        if (uiPhase == PHASE_RIPLIMB_BRINGS_SPEER && riplimb && riplimb->GetDistance(m_creature) <= 1)
        {
            uiPhase = PHASE_SHANNOX_HAS_SPEER;
            m_creature->LoadEquipment(ShannoxSpearEquipmentTemplate);

            DespawnCreatures(NPC_SHANNOX_SPEAR, 5000.0f);

            riplimb->RemoveAurasDueToSpell(SPELL_DOGGED_DETERMINATION);
            riplimb->RemoveAurasDueToSpell(SPEAR_VISIBLE_FETCH);

            riplimb->SetActiveObjectState(true);
            riplimb->GetMotionMaster()->MoveChase(riplimb->getVictim());
            riplimb->AI()->MoveInLineOfSight(riplimb->getVictim());
        }
        else if (uiPhase == PHASE_RIPLIMB_GOS_TO_SPEER && riplimb && riplimb->isAlive())
        {
            if (spear && !riplimb->HasAura(CRYSTAL_PRISON_EFFECT))
            {
                riplimb->GetMotionMaster()->MoveIdle();
                riplimb->GetMotionMaster()->MovePoint(0,spear->GetPositionX(),spear->GetPositionY(),spear->GetPositionZ());
            }
        }
        else if (uiPhase == PHASE_RIPLIMB_BRINGS_SPEER && riplimb && riplimb->isAlive())
        {
            riplimb->GetMotionMaster()->MoveIdle();
            riplimb->GetMotionMaster()->MovePoint(0,m_creature->GetPositionX(),m_creature->GetPositionY(),m_creature->GetPositionZ());
        }

        DoMeleeAttackIfReady();
    }

    uint32 uiPhase;

    Creature* GetSpear()
    {
        return GetClosestCreatureWithEntry(m_creature, NPC_SHANNOX_SPEAR, 5000.0f, true);
    }

    Creature* GetRageface()
    {
        return GetClosestCreatureWithEntry(m_creature, NPC_RAGEFACE, 5000.0f, false);
    }

    Creature* GetRiplimb()
    {
        return GetClosestCreatureWithEntry(m_creature, NPC_RIPLIMB, 5000.0f, false);
    }
};

CreatureAI* GetAI_boss_shannox(Creature* pCreature)
{
    return new boss_shannoxAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_ragefaceAI : public ScriptedAI
{
    npc_ragefaceAI(Creature *c) : ScriptedAI(c)
    {
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    bool frenzy;
    bool prisonStartAttack;

    void Reset() override
    {
        m_creature->RemoveAllAuras();
        events.Reset();
        frenzy = false;
        prisonStartAttack = false;
    }

    void KilledUnit(Unit * /*victim*/) override
    {
    }

    void JustDied(Unit * /*victim*/) override
    {
        if (Creature* shannox = GetShannox())
        {
            if (SpellAuraHolder* holder = shannox->GetSpellAuraHolder(SPELL_FRENZY_SHANNOX))
                holder->SetStackAmount(2);
            else
                DoCast(shannox,SPELL_FRENZY_SHANNOX);
            //shannox->MonsterTextEmote(SAY_ON_DOGS_FALL, 0, true);
            //shannox->MonsterTextEmote(EMOTE_SOFT_ENRAGE, 0, true);
        }
    }

    void EnterCombat(Unit * /*who*/) override
    {
        m_creature->GetMotionMaster()->MoveIdle();

        if (!m_bIsRegularMode)
            m_creature->CastSpell(m_creature, SPELL_FEEDING_FRENZY_H, true);

        m_creature->SetInCombatWithZone();
        m_creature->CallForHelp(50);

        m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

        events.ScheduleEvent(EVENT_CHANGE_TARGET, urand(5000,11000));

        events.ScheduleEvent(EVENT_FACE_RAGE, urand(24000, 30000));
    }

    void SelectNewTarget()
    {
        if (!m_creature->HasAura(CRYSTAL_PRISON_EFFECT))
            if (Unit* shallTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                m_creature->getThreatManager().clearReferences();
                m_creature->AddThreat(shallTarget, 500.0f);

                if (m_creature->GetDistance(shallTarget) >= 13.0f)
                {
                    float x, y, z;
                    shallTarget->GetContactPoint(m_creature, x, y, z);
                    float speedXY = GetExactDistance2d(m_creature->GetPositionX(), m_creature->GetPositionY(), x, y) * 10.0f / 15;
                    m_creature->GetMotionMaster()->MoveJump(x, y, z, speedXY, 15);
                }

                m_creature->GetMotionMaster()->MoveChase(shallTarget);
                m_creature->AI()->AttackStart(shallTarget);
            }
    }

    void DamageTaken(Unit* who, uint32& damage) override
    {
        if (damage >= 40000 && m_creature->HasAuraOfDifficulty(SPELL_FACE_RAGE_10N))
        {
            m_creature->getVictim()->RemoveAurasDueToSpell(SPELL_FACE_RAGE);
            m_creature->RemoveAurasDueToSpell(SPELL_FACE_RAGE_10N);
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (m_creature->getVictim())
            if(!m_creature->HasAuraOfDifficulty(SPELL_FACE_RAGE_10N) && m_creature->getVictim()->HasAura(SPELL_FACE_RAGE) && !m_creature->HasAura(CRYSTAL_PRISON_EFFECT))
            {
                if (Unit* RageTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    m_creature->getThreatManager().clearReferences();
                    m_creature->AddThreat(RageTarget, 500.0f);

                    if (m_creature->GetDistance(RageTarget) >= 13.0f)
                    {
                        float x, y, z;
                        RageTarget->GetContactPoint(m_creature, x, y, z);
                        float speedXY = GetExactDistance2d(m_creature->GetPositionX(), m_creature->GetPositionY(), x, y) * 10.0f / 15;
                        m_creature->GetMotionMaster()->MoveJump(x, y, z, speedXY, 15);
                    }

                    m_creature->GetMotionMaster()->MoveChase(RageTarget);
                    m_creature->AI()->AttackStart(RageTarget);
                }
                m_creature->getVictim()->RemoveAurasDueToSpell(SPELL_FACE_RAGE);
            }

        if (Creature* shannox = GetShannox())
        {
            if(!m_creature->isInCombat())
            {
                if (m_creature->GetDistance(shannox) > 20.0f)
                    m_creature->SetPosition(shannox->GetPositionX()+5,shannox->GetPositionY()-5,shannox->GetPositionZ(),shannox->GetOrientation());
                else
                    m_creature->GetMotionMaster()->MoveFollow(shannox, walkRagefaceDistance, walkRagefaceAngle);
            }
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_CHANGE_TARGET:
                if (!m_creature->getVictim()->HasAura(SPELL_RAGE) && !m_creature->HasAuraOfDifficulty(SPELL_FACE_RAGE_10N))
                    SelectNewTarget();

                events.ScheduleEvent(EVENT_CHANGE_TARGET, urand(5000,11000));
                break;
            case EVENT_FACE_RAGE:
                if (Unit* FaceRageTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    m_creature->getThreatManager().clearReferences();
                    m_creature->AddThreat(FaceRageTarget, 500.0f);

                    if (m_creature->GetDistance(FaceRageTarget) >= 13.0f)
                    {
                        float x, y, z;
                        FaceRageTarget->GetContactPoint(m_creature, x, y, z);
                        float speedXY = GetExactDistance2d(m_creature->GetPositionX(), m_creature->GetPositionY(), x, y) * 10.0f / 15;
                        m_creature->GetMotionMaster()->MoveJump(x, y, z, speedXY, 15);
                    }

                    m_creature->GetMotionMaster()->MoveChase(FaceRageTarget);
                    m_creature->AI()->AttackStart(FaceRageTarget);
                    DoCast(FaceRageTarget,SPELL_FACE_RAGE);
                }

                events.ScheduleEvent(EVENT_FACE_RAGE, 61000);
                break;
            case EVENT_PRISON_DOG_ATTACK_RAGEFACE:
                if (Creature *prison = GetClosestCreatureWithEntry(m_creature, NPC_CRYSTAL_PRISON, 50.0f, true))
                {
                    int32 dmg = prison->GetMaxHealth() * 0.20f;
                    m_creature->DealDamage(prison,dmg, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    if (prison->isAlive())
                        events.ScheduleEvent(EVENT_PRISON_DOG_ATTACK_RAGEFACE, 1000);
                }
                break;
            default:
                break;
            }
        }

        if (m_creature->HasAura(CRYSTAL_PRISON_EFFECT) && !prisonStartAttack)
            if (Creature *prison = GetClosestCreatureWithEntry(m_creature, NPC_CRYSTAL_PRISON, 50.0f, true))
            {
                events.ScheduleEvent(EVENT_PRISON_DOG_ATTACK_RAGEFACE, 1000);
                prisonStartAttack = true;
            }

        if(Creature* shannox = GetShannox())
        {
            if(shannox->GetHealthPercent() <= 30 && frenzy == false && m_bIsRegularMode)
            {
                frenzy = true;
                DoCast(m_creature, SPELL_FRENZIED_DEVOLUTION);
            }

            if(shannox->GetDistance2d(m_creature) >= maxDistanceBetweenShannoxAndDogs && !m_creature->HasAura(SPELL_SEPERATION_ANXIETY)) //TODO Sniff right Distance
            {
                DoCast(m_creature, SPELL_SEPERATION_ANXIETY);
            }

        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    Creature* GetShannox()
    {
        return GetClosestCreatureWithEntry(m_creature, NPC_SHANNOX, 5000.0f, true);
    }

};

CreatureAI* GetAI_npc_rageface(Creature* pCreature)
{
    return new npc_ragefaceAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_riplimbAI : public ScriptedAI
{
    npc_riplimbAI(Creature *c) : ScriptedAI(c), vehicle(c->GetVehicleKit())
    {
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    bool frenzy;
    bool inTakingSpearPhase;
    bool firstLimbRip;
    bool prisonStartAttack;

    VehicleKit* vehicle;

    void Reset() override
    {
        m_creature->RemoveAllAuras();
        events.Reset();
        prisonStartAttack = false;
        frenzy = false; // Is needed, that Frenzy is not casted twice on Riplimb
        inTakingSpearPhase = false; // Is needed for correct execution of the Phases
        firstLimbRip = false;
    }

    void KilledUnit(Unit * /*victim*/) override
    {
    }

    void JustDied(Unit * /*victim*/) override
    {
        if (Creature* shannox = GetShannox())
        {
            // Heroic: Respawn Riplimb 30s after he is Death (Trigger)
            if (!m_bIsRegularMode)
            {
                shannox->AI()->DoAction(ACTION_START_EVENT_TO_RESPAWN_RIPLIMB);
                shannox->AI()->DoAction(ACTION_SAY_RIPLIMB);
            }

            if (SpellAuraHolder* holder = shannox->GetSpellAuraHolder(SPELL_FRENZY_SHANNOX))
                holder->SetStackAmount(2);
            else
                DoCast(shannox, SPELL_FRENZY_SHANNOX, true);
            //shannox->MonsterTextEmote(SAY_ON_DOGS_FALL, 0, true);
            //shannox->MonsterTextEmote(EMOTE_SOFT_ENRAGE, 0, true);
        }
    }

    void EnterCombat(Unit * who) override
    {
        m_creature->GetMotionMaster()->MoveIdle();

        if (!m_bIsRegularMode)
            m_creature->CastSpell(m_creature, SPELL_FEEDING_FRENZY_H, true);

        m_creature->SetInCombatWithZone();
        m_creature->CallForHelp(50);

        prisonStartAttack = false;
        inTakingSpearPhase = false;

        me->GetMotionMaster()->MoveChase(me->getVictim());

        events.ScheduleEvent(EVENT_LIMB_RIP, 12000); //TODO Find out the correct Time
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!me->getVictim()) {}

        if(!me->isInCombat())
        {
            if (Creature* shannox = GetShannox())
                if (me->GetDistance(shannox) > 20.0f)
                    me->SetPosition(shannox->GetPositionX()+5,shannox->GetPositionY()-5,shannox->GetPositionZ(),0);
                else
                    me->GetMotionMaster()->MoveFollow(shannox, walkRiplimbDistance, walkRiplimbAngle);
        }

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
            case EVENT_LIMB_RIP:
                DoCastVictim(SPELL_LIMB_RIP);
                    
                if(GetShannox() && !firstLimbRip)
                {
                    firstLimbRip = true;
                    //DoScriptText(SAY_GO_FOR_THROAT, GetShannox());
                    events.ScheduleEvent(EVENT_RIPLIMB_RESET_SHANNOX_YELL, 45000); // Resets Yell after 45s
                }

                events.ScheduleEvent(EVENT_LIMB_RIP, 12000); //TODO Find out the correct Time
                break;
            case EVENT_TAKING_SPEAR_DELAY:
                inTakingSpearPhase = false;
                if (Creature* shannox = GetShannox())
                    shannox->AI()->SetData(DATA_PHASE, PHASE_RIPLIMB_GOS_TO_SPEER);
                break;
            case EVENT_RIPLIMB_RESET_SHANNOX_YELL:
                firstLimbRip = false;
                break;
            case EVENT_PRISON_DOG_ATTACK_RIPLIMB:
                if (Creature *prison = GetClosestCreatureWithEntry(m_creature, NPC_CRYSTAL_PRISON, 50.0f, true))
                {
                    int32 dmg = prison->GetMaxHealth() * 0.10;
                    m_creature->DealDamage(prison,dmg, NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    if (prison->isAlive())
                        events.ScheduleEvent(EVENT_PRISON_DOG_ATTACK_RIPLIMB, 1000);
                }
                break;
            default:
                break;
            }
        }

        if (me->HasAura(CRYSTAL_PRISON_EFFECT) && !prisonStartAttack)
            if (Creature *prison = GetClosestCreatureWithEntry(m_creature, NPC_CRYSTAL_PRISON, 50.0f, true))
            {
                events.ScheduleEvent(EVENT_PRISON_DOG_ATTACK_RIPLIMB, 1000);
                prisonStartAttack = true;
            }

        if(Creature* shannox = GetShannox())
        {
            if(shannox->GetHealthPercent() <= 30 && frenzy == false && m_bIsRegularMode)
            {
                frenzy = true;
                DoCast(me, SPELL_FRENZIED_DEVOLUTION);
            }

            if(shannox->GetDistance2d(me) >= maxDistanceBetweenShannoxAndDogs && !me->HasAura(SPELL_SEPERATION_ANXIETY)) //TODO Sniff right Distance
                DoCast(me, SPELL_SEPERATION_ANXIETY);

            if (shannox->AI()->GetData(DATA_PHASE) == PHASE_SPEAR_ON_THE_GROUND && !inTakingSpearPhase)
            {
                inTakingSpearPhase = true;
                events.ScheduleEvent(EVENT_TAKING_SPEAR_DELAY, 7500);
            }

            if (Creature* spear = GetSpear())
            {
                if (Creature* shannox = GetShannox())
                    if (shannox->AI()->GetData(DATA_PHASE) == PHASE_RIPLIMB_GOS_TO_SPEER && spear->GetDistance(me) <= 1)
                    {
                        shannox->AI()->SetData(DATA_PHASE, PHASE_RIPLIMB_BRINGS_SPEER);
                        spear->ForcedDespawn();
                        m_creature->CastSpell(m_creature, SPEAR_VISIBLE_FETCH, true);

                        DoCast(m_creature, SPELL_DOGGED_DETERMINATION);
                    }
            }
        }

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

    inline Creature* GetShannox()
    {
        return GetClosestCreatureWithEntry(m_creature, NPC_SHANNOX, 5000.0f, true);
    }

    inline Creature* GetSpear()
    {
        return GetClosestCreatureWithEntry(m_creature, NPC_SHANNOX_SPEAR, 5000.0f, true);
    }
};

CreatureAI* GetAI_npc_riplimb(Creature* pCreature)
{
    return new npc_riplimbAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_shannox_spearAI : public ScriptedAI
{
    npc_shannox_spearAI(Creature *c) : ScriptedAI(c)
    {
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    void Reset() override
    {
        me->CastSpell(me, SPEAR_VISIBLE_ON_GROUND, true);
        //me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
    }

    void JustDied(Unit * /*victim*/) override
    {
    }

    void EnterCombat(Unit * /*who*/) override
    {
        DoCast(m_creature, SPELL_MAGMA_FLARE);

        if (Creature* shannox = GetShannox())
        {
            // Calcs 3 Circles
            for(float r = 0; r <= 30; r = r+10)
                for(float x = 0; x <= r*2; x = x + 2)
                {
                    shannox->CastSpell(me->GetPositionX()+ cos(x)*r,me->GetPositionY()+ sin(x)*r,
                        me->GetPositionZ(),SPELL_MAGMA_RUPTURE_VISUAL,true);

                    shannox->CastSpell(me->GetPositionX()-cos(x)*r,me->GetPositionY()-sin(x)*r,
                        me->GetPositionZ(),SPELL_MAGMA_RUPTURE_VISUAL,true);
                }
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;
    }

    Creature* GetShannox()
    {
        return GetClosestCreatureWithEntry(me, NPC_SHANNOX, 5000.0f, true);
    }
};

CreatureAI* GetAI_npc_shannox_spear(Creature* pCreature)
{
    return new npc_shannox_spearAI(pCreature);
}

/////

struct MANGOS_DLL_DECL npc_immolation_trapAI : public Scripted_NoMovementAI
{
    npc_immolation_trapAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        tempTarget = NULL;
        //me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        events.Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;
    Unit* tempTarget;

    void JustDied(Unit * /*victim*/)
    {
    }

    void Reset()
    {
        events.Reset();
    }

    void EnterCombat(Unit * /*who*/)
    {
        events.ScheduleEvent(EVENT_IMMOLATION_TRAP_TRIGGER, 2000);
    }

    void UpdateAI(const uint32 diff)
    {
        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_IMMOLATION_TRAP_TRIGGER:
                    //Riplimb and Rageface has a higher Priority than Players...

                    Creature* riplimb = GetRiplimb();
                    Creature* shannox = GetShannox();
                    Creature* rageface = GetRageface();

                    if (riplimb && riplimb->GetDistance(me) <= 3.0f && !riplimb->HasAuraOfDifficulty(SPELL_WARY_10N) && riplimb->isAlive() && shannox->AI()->GetData(DATA_PHASE) == PHASE_SHANNOX_HAS_SPEER)
                        tempTarget = riplimb;
                    else if (rageface && rageface->GetDistance(me) <= 3.0f && !rageface->HasAuraOfDifficulty(SPELL_WARY_10N) && rageface->isAlive())
                        tempTarget = rageface;
                    else 
                        tempTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

                    if (!tempTarget || tempTarget->HasAura(CRYSTAL_PRISON_EFFECT)) // If no Target exists try to get a new Target in 0,5s
                        events.ScheduleEvent(EVENT_IMMOLATION_TRAP_TRIGGER, 500);
                    else
                    {
                        DoCast(tempTarget, SPELL_IMMOLATION_TRAP_10N);

                        // Cast Spell Wary on Dogs
                        if (tempTarget->GetEntry() == NPC_RIPLIMB || tempTarget->GetEntry() == NPC_RAGEFACE)
                            tempTarget->CastSpell(tempTarget, SPELL_WARY_10N, true);

                        me->ForcedDespawn();
                    }

                    break;
                default:
                    break;
            }
        }
    }

    Creature* GetShannox()
    {
        return GetClosestCreatureWithEntry(me, NPC_SHANNOX, 5000.0f, true);
    }

    Creature* GetRiplimb()
    {
        return GetClosestCreatureWithEntry(me, NPC_RIPLIMB, 5000.0f, true);
    }

    Creature* GetRageface()
    {
        return GetClosestCreatureWithEntry(me, NPC_RAGEFACE, 5000.0f, true);
    }
};

CreatureAI* GetAI_npc_immolation_trap(Creature* pCreature)
{
    return new npc_immolation_trapAI(pCreature);
}

////

struct MANGOS_DLL_DECL npc_crystal_trapAI : public Scripted_NoMovementAI
{
    npc_crystal_trapAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        m_pInstance = (ScriptedInstance*)c->GetInstanceData();
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    EventMap events;
    Unit* tempTarget;
    Creature* myPrison;

    void JustDied(Unit * /*victim*/) override
    {
    }

    void Reset()
    {
        tempTarget = NULL;
        myPrison = NULL;
        //me->SetReactState(REACT_PASSIVE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_DISABLE_MOVE | UNIT_FLAG_NOT_SELECTABLE);
        events.Reset();
    }

    void EnterCombat(Unit * /*who*/)
    {
        events.ScheduleEvent(EVENT_CRYSTAL_TRAP_TRIGGER, 2000); //  The trap arms after 2 seconds.
    }

    void UpdateAI(const uint32 diff)
    {

        events.Update(diff);

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_CRYSTAL_TRAP_TRIGGER:

                    //Riplimb has a higher Priority than Players...

                    Creature* riplimb = GetRiplimb();
                    Creature* rageface = GetRageface();
                    Creature* shannox = GetShannox();

                    if(riplimb && riplimb->GetDistance(me) <= 2.0f && !riplimb->HasAuraOfDifficulty(SPELL_WARY_10N) && riplimb->isAlive() && shannox->AI()->GetData(DATA_PHASE) == PHASE_SHANNOX_HAS_SPEER)
                        tempTarget = GetRiplimb();
                    else if(rageface && rageface->GetDistance(me) <= 3.0f && !rageface->HasAuraOfDifficulty(SPELL_WARY_10N) && rageface->isAlive())
                        tempTarget = GetRageface();
                    else
                        tempTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0);

                    if (!tempTarget || tempTarget->HasAura(CRYSTAL_PRISON_EFFECT)) // If no Target exists try to get a new Target in 0,5s
                        events.ScheduleEvent(EVENT_CRYSTAL_TRAP_TRIGGER, 500);
                    else
                    { 
                        // Intialize Prison if tempTarget was set
                        DoCast(tempTarget,CRYSTAL_PRISON_EFFECT);
                        myPrison = me->SummonCreature(NPC_CRYSTAL_PRISON,tempTarget->GetPositionX(), tempTarget->GetPositionY(), tempTarget->GetPositionZ(),0, TEMPSUMMON_MANUAL_DESPAWN);

                        if (myPrison)
                        {
                            //me->SetReactState(REACT_AGGRESSIVE);
                            myPrison->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        }

                        events.ScheduleEvent(EVENT_CRYSTAL_PRISON_DESPAWN, 20000);
                    }

                    break;

                case EVENT_CRYSTAL_PRISON_DESPAWN:

                    if (tempTarget)
                    {
                        myPrison->ForcedDespawn();
                        tempTarget->RemoveAurasDueToSpell(CRYSTAL_PRISON_EFFECT);
                        // Cast Spell Wary on Dogs
                        if(tempTarget->GetEntry() == NPC_RIPLIMB || tempTarget->GetEntry() == NPC_RAGEFACE)
                            tempTarget->CastSpell(tempTarget, SPELL_WARY_10N, true);

                        me->ForcedDespawn();
                    }
                    break;
                default:
                    break;
            }
        }

        if(myPrison && myPrison->isDead())
        {
            myPrison->ForcedDespawn();
            tempTarget->RemoveAurasDueToSpell(CRYSTAL_PRISON_EFFECT);
            // Cast Spell Wary on Dogs
            if(tempTarget->GetEntry() == NPC_RIPLIMB || tempTarget->GetEntry() == NPC_RAGEFACE)
                tempTarget->CastSpell(tempTarget, SPELL_WARY_10N, true);

            me->ForcedDespawn();
        }
    }

    Creature* GetShannox()
    {
        return GetClosestCreatureWithEntry(me, NPC_SHANNOX, 5000.0f, true);
    }

    Creature* GetRiplimb()
    {
        return GetClosestCreatureWithEntry(me, NPC_RIPLIMB, 5000.0f, true);
    }

    Creature* GetRageface()
    {
        return GetClosestCreatureWithEntry(me, NPC_RAGEFACE, 5000.0f, true);
    }
};

CreatureAI* GetAI_npc_crystal_trap(Creature* pCreature)
{
    return new npc_crystal_trapAI(pCreature);
}

void AddSC_boss_shannox()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_shannox";
    pNewScript->GetAI = &GetAI_boss_shannox;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rageface";
    pNewScript->GetAI = &GetAI_npc_rageface;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_riplimb";
    pNewScript->GetAI = &GetAI_npc_riplimb;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_shannox_spear";
    pNewScript->GetAI = &GetAI_npc_shannox_spear;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_immolation_trap";
    pNewScript->GetAI = &GetAI_npc_immolation_trap;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_crystal_trap";
    pNewScript->GetAI = &GetAI_npc_crystal_trap;
    pNewScript->RegisterSelf();
}
