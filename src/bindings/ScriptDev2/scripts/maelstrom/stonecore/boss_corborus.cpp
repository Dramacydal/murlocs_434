#include "precompiled.h"
#include "stonecore.h"

enum Spells
{
    SPELL_CRYSTAL_BARRAGE            = 86881,
    SPELL_CRYSTAL_BARRAGE_H          = 92648,
    SPELL_DAMPENING_WAVE             = 82415,
    SPELL_DAMPENING_WAVE_H           = 92650,
    SPELL_BURROW                     = 26381,
};

struct MANGOS_DLL_DECL boss_corborusAI : public ScriptedAI
{
    boss_corborusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 _SummonBorerTimer;
    uint32 b_BorrowTimer;
    uint32 _CrystalTimer;
    uint32 _DampeningTimer;
    bool b_BORROW;
    std::list<ObjectGuid> summons;

    void Reset() override
    {
        _SummonBorerTimer    = 33000;
        b_BORROW             = false;
        _CrystalTimer        = 13600;
        _DampeningTimer      = 25000;
        DespawnSummons();
    }

    void DespawnSummons()
    {
        if (!m_pInstance)
            return;

        for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* unit = m_pInstance->instance->GetAnyTypeCreature(*itr))
                unit->ForcedDespawn();

        summons.clear();
    }

    void EnterCombat(Unit* /*who*/) override
    {
        m_creature->SetInCombatWithZone();
    }

    void Aggro(Unit* who) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORBORUS, IN_PROGRESS);
    }

    void SummonRockBorer()
    {
        if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            for (uint8 i = 0; i < (m_bIsRegularMode ? 2 : 4); ++i)
        m_creature->SummonCreature(NPC_ROCK_BORER, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 0);
    }

    void JustSummoned(Creature* unit) override
    {
        summons.push_back(unit->GetObjectGuid());
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORBORUS, FAIL);

        DespawnSummons();
    }

    void JustDied(Unit* killer) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORBORUS, DONE);

        DespawnSummons();
    }

    void UpdateAI(const uint32 Diff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (_SummonBorerTimer <= Diff)
        {
            b_BORROW = true;
            DoCast(m_creature, SPELL_BURROW);
            SummonRockBorer();
            _SummonBorerTimer = 30000;
            b_BorrowTimer = 9000;
        }
        else
        {
            _SummonBorerTimer -= Diff;
            b_BorrowTimer -= Diff;
        }

        if (b_BorrowTimer <= Diff)
            b_BORROW = false;

        if (_CrystalTimer <= Diff)
        {
            if (Unit* target = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                if (m_bIsRegularMode)
                    DoCast(target,SPELL_CRYSTAL_BARRAGE);
                else
                    DoCast(target,SPELL_CRYSTAL_BARRAGE_H);

                _CrystalTimer = 11000;
        }
        else
            _CrystalTimer -= Diff;

        if (_DampeningTimer <= Diff)
        {
            if (m_bIsRegularMode)
                DoCast(m_creature, SPELL_DAMPENING_WAVE);
            else
                DoCast(m_creature, SPELL_DAMPENING_WAVE_H);

            _DampeningTimer = 20000;
        }
        else
            _DampeningTimer -= Diff;

        if (!b_BORROW)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_corborus(Creature* pCreature)
{
    return new boss_corborusAI(pCreature);
}

void AddSC_boss_corborus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_corborus";
    pNewScript->GetAI = &GetAI_boss_corborus;
    pNewScript->RegisterSelf();
}