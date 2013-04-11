#include "precompiled.h"
#include "stonecore.h"

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

    void Reset() { }

    void EnterCombat(Unit* /*who*/) { }

    void UpdateAI(const uint32 Diff)
    {
        DoMeleeAttackIfReady();
    }

    void Aggro(Unit* who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AZIL, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AZIL, FAIL);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_AZIL, DONE);
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