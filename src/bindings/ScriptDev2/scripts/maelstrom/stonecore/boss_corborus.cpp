#include "precompiled.h"
#include "stonecore.h"

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

    void Reset() {}

    void EnterCombat(Unit* /*who*/) {}

    void UpdateAI(const uint32 Diff)
    {
        DoMeleeAttackIfReady();
    }

    void Aggro(Unit* who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORBORUS, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORBORUS, FAIL);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_CORBORUS, DONE);
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