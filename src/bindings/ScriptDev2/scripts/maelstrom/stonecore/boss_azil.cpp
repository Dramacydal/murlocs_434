#include "precompiled.h"
#include "stonecore.h"

#define SAY_AGGRO "For my death, countless more will fall. The burden is now yours to bear."
#define SAY_P2 "Witness the power bestowed upon me by Deathwing! Feel the fury of earth!"
#define SAY_EARTH "The world will be reborn in flames!"
#define SAY_KILL "A sacrifice for you, master."

enum Spells
{
    // Phase 1
    SPELL_CURSE_OF_BLOOD_N = 59009, // Random player
    SPELL_CURSE_OF_BLOOD_H = 92663,
    SPELL_FORCE_GRIP = 79351, // Victim - Decast
    SPELL_SUMMON_GRAVITY_WELL = 79340, // Summon Puit
    SPELL_SUMMON_GRAVITY_WELL_AURA = 79244, // Puit Aura
    SPELL_SUMMON_GRAVITY_WELL_DAMAGE = 47756, // Puit Damage
    // Phase 2
    SPELL_SEISMIC_SHARD = 79002, // Random player
    SPELL_ENERGY_SHIELD_N = 79050, // Shield
    SPELL_ENERGY_SHIELD_H = 92667, // Shield
};

enum Summons
{
    MOB_DEVOUT_FOLLOWER = 42428,
    MOB_GRAVITY_WELL = 42499,
};

Position2 addSpawnLocations[3] =
{
    {1387.417725f, 1020.721863f, 211.693954f, 3.581235f},
    {1265.707520f, 958.458191f, 205.395737f, 0.608493f},
    {1331.645508f, 991.484741f, 207.999710f, 0.00f},
};

enum Phases
{
    PHASE_ALL = 0,
    PHASE_NORMAL = 1,
    PHASE_SUMMONS = 2,
};

struct MANGOS_DLL_DECL boss_azilAI : public ScriptedAI
{
public:
    boss_azilAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    std::list<ObjectGuid> summons;

    uint8 Phase;
    bool Phased;
    uint8 SpawnCount;
    uint8 PhaseCount;

    uint32 CurseBloodTimer;
    uint32 ForceGripTimer;
    uint32 SummonGravityWellTimer;
    uint32 SummonDevoutTimer;
    uint32 Phase2EndTimer;

    void RemoveSummons()
    {
        if (!m_pInstance)
            return;

        for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* unit = m_pInstance->instance->GetAnyTypeCreature(*itr))
                unit->ForcedDespawn();

        summons.clear();
    }

    void Reset() override
    {
        m_creature->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        //m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);

        Phased = false;
        RemoveSummons();

        Phase = PHASE_NORMAL;

        SpawnCount = 20;
        PhaseCount = 0;

        CurseBloodTimer = urand(8000, 13000);
        ForceGripTimer = 22000;
        SummonGravityWellTimer = urand(11000,16000);
        SummonDevoutTimer = urand(19000,27000);

        m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_SHIELD_N);
        m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_SHIELD_H);

        m_creature->GetMotionMaster()->MoveTargetedHome();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        m_creature->SetInCombatWithZone();
        m_creature->MonsterYell(SAY_AGGRO, LANG_UNIVERSAL, NULL);
    }

    void Aggro(Unit* who) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AZIL, IN_PROGRESS);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AZIL, FAIL);
        RemoveSummons();
    }

    void JustDied(Unit* killer) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AZIL, DONE);

        RemoveSummons();
    }

    void KilledUnit(Unit * /*victim*/) override
    {
        m_creature->MonsterYell(SAY_KILL, LANG_UNIVERSAL, NULL);
    }

    void SummonedCreatureDespawn(Creature* summon) override
    {
        switch(summon->GetEntry())
        {
            case MOB_DEVOUT_FOLLOWER:
                SpawnCount--;
                break;
        }
    }

    void JustSummoned(Creature* pSummon)
    {
        switch (pSummon->GetEntry())
        {
            case MOB_DEVOUT_FOLLOWER:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    pSummon->AI()->AttackStart(pTarget);
                summons.push_back(pSummon->GetObjectGuid());
            break;
        }
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (SpawnCount == 0 && Phase == PHASE_SUMMONS)
        {
            m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, false);
            SpawnCount = 10;
            SetCombatMovement(true);
            Phase = PHASE_NORMAL;
            Phased = false;
            CurseBloodTimer = urand(8000,13000);
            ForceGripTimer = 22000;
            SummonGravityWellTimer = urand(11000,16000);
            SummonDevoutTimer = urand(19000,27000);
            m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_SHIELD_N);
            m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_SHIELD_H);
        }

        if (m_creature->GetHealthPercent() < 67 && Phase == PHASE_NORMAL && PhaseCount == 0)
        {
            m_creature->MonsterYell(SAY_P2, LANG_UNIVERSAL, NULL);

            m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
            PhaseCount++;
            SetCombatMovement(false);
            Phase = PHASE_SUMMONS;
            m_creature->NearTeleportTo(1337.89f, 963.287f, 214.184f, 1.8407f);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ENERGY_SHIELD_N : SPELL_ENERGY_SHIELD_H);
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SEISMIC_SHARD);

            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            Phase2EndTimer = 60000;
        }

        if (m_creature->GetHealthPercent() < 34 && Phase == PHASE_NORMAL && PhaseCount == 1)
        {
            m_creature->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_INTERRUPT, true);
            m_creature->MonsterYell(SAY_P2, LANG_UNIVERSAL, NULL);

            PhaseCount++;
            SetCombatMovement(false);
            Phase = PHASE_SUMMONS;
            m_creature->NearTeleportTo(1337.89f, 963.287f, 214.184f, 1.8407f);
            DoCast(m_creature, m_bIsRegularMode ? SPELL_ENERGY_SHIELD_N : SPELL_ENERGY_SHIELD_H);
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_SEISMIC_SHARD);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN,0);
            Phase2EndTimer = 60000;
        }

        if (CurseBloodTimer <= diff && Phase == PHASE_NORMAL)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, m_bIsRegularMode ? SPELL_CURSE_OF_BLOOD_N : SPELL_CURSE_OF_BLOOD_H);
            CurseBloodTimer = urand(5000,7000);
        } else CurseBloodTimer -= diff;

        if (ForceGripTimer <= diff && Phase == PHASE_NORMAL)
        {
            m_creature->InterruptNonMeleeSpells(true);
            DoCastVictim(SPELL_FORCE_GRIP);
            ForceGripTimer = urand(12000,15000);
        } else ForceGripTimer -= diff;

        if (SummonGravityWellTimer <= diff && Phase == PHASE_NORMAL)
        {
            m_creature->MonsterYell(SAY_EARTH, LANG_UNIVERSAL, NULL);
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(target, SPELL_SUMMON_GRAVITY_WELL);
            SummonGravityWellTimer = urand(13000,16000);
        } else SummonGravityWellTimer -= diff;

        if (SummonDevoutTimer <= diff && Phase == PHASE_NORMAL)
        {
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[0].x,addSpawnLocations[0].y,addSpawnLocations[0].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            m_creature->SummonCreature(MOB_DEVOUT_FOLLOWER, addSpawnLocations[1].x,addSpawnLocations[1].y,addSpawnLocations[1].z, addSpawnLocations[0].o, TEMPSUMMON_CORPSE_DESPAWN, 0);
            SummonDevoutTimer = urand(19000,27000);
        } else SummonDevoutTimer -= diff;

        if (Phase == PHASE_SUMMONS)
        {
            if (Phase2EndTimer <= diff)
            {
                SpawnCount = 10;
                SetCombatMovement(true);
                Phase = PHASE_NORMAL;
                Phased = false;
                CurseBloodTimer = urand(8000,13000);
                ForceGripTimer = 22000;
                SummonGravityWellTimer = urand(11000,16000);
                SummonDevoutTimer = urand(19000,27000);
                m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_SHIELD_N);
                m_creature->RemoveAurasDueToSpell(SPELL_ENERGY_SHIELD_H);
            } else Phase2EndTimer -= diff;
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_azil(Creature* pCreature)
{
    return new boss_azilAI(pCreature);
}

void AddSC_boss_azil()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_azil";
    pNewScript->GetAI = &GetAI_boss_azil;
    pNewScript->RegisterSelf();
}