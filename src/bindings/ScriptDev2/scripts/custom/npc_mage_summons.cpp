#include "precompiled.h"

struct MANGOS_DLL_DECL npc_mage_flame_orbAI : public ScriptedAI
{
    Unit *mage;

    npc_mage_flame_orbAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }



    void Reset()
    {
        mage = NULL;
    }

    void Aggro(Unit* pWho)
    {
    }

    void UpdateAI(const uint32 diff)
    {
        if(!mage)
            if(mage = m_creature->GetCreator())
            {
                Position destPos;
                m_creature->GetPosition(destPos.x, destPos.y, destPos.z);

                    mage->GetNearPoint(mage, destPos.x, destPos.y, destPos.z, mage->GetObjectBoundingRadius(), 50.0f, 0.0f);

                m_creature->GetMotionMaster()->MovePoint(0, destPos.x, destPos.y, destPos.z);
            }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        m_creature->StopMoving();
    }
};

CreatureAI* GetAI_mage_flame_orb(Creature* pCreature)
{
    return new npc_mage_flame_orbAI(pCreature);
}

void AddSC_npc_flame_orb()
{
    Script *pNewScript;
    pNewScript = new Script;
    pNewScript->Name="npc_mage_flame_orb";
    pNewScript->GetAI = &GetAI_mage_flame_orb;
    pNewScript->RegisterSelf();
}
