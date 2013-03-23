#include "precompiled.h"

enum
{
    SPELL_FLAME_ORB_SNARE       = 82736,
    SPELL_FLAME_ORB_EXPLODE     = 83619,
    SPELL_FLAME_ORB_DAMAGE      = 86719,

    SPELL_FF_ORG_DAMAGE_WSNARE  = 84721,
    SPELL_FF_ORG_DAMAGE         = 95969,
};

struct MANGOS_DLL_DECL npc_mage_orbAI : public ScriptedAI
{
    bool snared;
    bool init;
    uint32 m_seekTimer;

    npc_mage_orbAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void Reset()
    {
        snared = false;
        init = false;
        m_seekTimer = 5000;
    }

    void SpellHitTarget(Unit* pTarget, const SpellEntry* pSpell)
    {
        switch (pSpell->Id)
        {
            case SPELL_FF_ORG_DAMAGE_WSNARE:
            case SPELL_FLAME_ORB_DAMAGE:
            case SPELL_FF_ORG_DAMAGE:
            {
                m_seekTimer = 5000;
                if (!snared)
                {
                    snared = true;
                    m_creature->CastSpell(m_creature, SPELL_FLAME_ORB_SNARE, true);
                }
                break;
            }
        }
    }

    void BeforeDespawn()
    {
        // Flame Orb only
        if (m_creature->GetEntry() != 44214)
            return;

        Unit* owner = m_creature->GetOwner();
        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        if (SpellEntry const * talent = ((Player*)owner)->GetKnownTalentRankById(11434))
            if (roll_chance_i(talent->GetProcChance()))
                m_creature->CastSpell(m_creature, SPELL_FLAME_ORB_EXPLODE, true, NULL, NULL, owner->GetObjectGuid());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!init)
        {
            init = true;
            if (Unit* owner = m_creature->GetCreator())
            {
                m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_HOVER);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
                m_creature->SetFloatValue(UNIT_FIELD_HOVERHEIGHT, 2.5f);

                Position destPos;
                m_creature->GetPosition(destPos.x, destPos.y, destPos.z);
                owner->GetNearPoint(owner, destPos.x, destPos.y, destPos.z, owner->GetObjectBoundingRadius(), 50.0f, owner->GetOrientation());

                m_creature->GetMotionMaster()->MovePoint(m_creature->GetCreatureInfo()->movementId, destPos.x, destPos.y, destPos.z, true);
            }
        }

        if (m_seekTimer && m_creature->GetOwnerGuid())
        {
            if (m_seekTimer < diff)
                m_creature->ForcedDespawn(500);
            else
                m_seekTimer -= diff;
        }
    }
};

CreatureAI* GetAI_mage_orb(Creature* pCreature)
{
    return new npc_mage_orbAI(pCreature);
}

void AddSC_npc_mage_summons()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "npc_mage_orb";
    pNewScript->GetAI = &GetAI_mage_orb;
    pNewScript->RegisterSelf();
}
