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

        Unit* creator = m_creature->GetCreator();
        if (!creator || creator->GetTypeId() != TYPEID_PLAYER)
            return;

        if (SpellEntry const * talent = ((Player*)creator)->GetKnownTalentRankById(11434))
            if (roll_chance_i(talent->GetProcChance()))
                m_creature->CastSpell(m_creature, SPELL_FLAME_ORB_EXPLODE, true, NULL, NULL, creator->GetObjectGuid());
    }

    void UpdateAI(const uint32 diff)
    {
        if (!init)
        {
            init = true;
            if (Unit* creator = m_creature->GetCreator())
            {
                m_creature->m_movementInfo.AddMovementFlag(MOVEFLAG_HOVER);
                m_creature->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
                m_creature->SetFloatValue(UNIT_FIELD_HOVERHEIGHT, 2.5f);
                m_creature->SetWalk(false);

                WorldPacket data;
                data.Initialize(SMSG_SPLINE_MOVE_SET_HOVER, 8 + 4 + 1);
                data.WriteGuidMask<3, 7, 0, 1, 4, 6, 2, 5>(m_creature->GetObjectGuid());
                data.WriteGuidBytes<2, 4, 3, 1, 7, 0, 5, 6>(m_creature->GetObjectGuid());
                m_creature->SendMessageToSet(&data, true);

                Position destPos;
                m_creature->GetPosition(destPos.x, destPos.y, destPos.z);
                creator->GetNearPoint(creator, destPos.x, destPos.y, destPos.z, creator->GetObjectBoundingRadius(), 50.0f, creator->GetOrientation());

                m_creature->GetMotionMaster()->MovePoint(m_creature->GetCreatureInfo()->movementId, destPos.x, destPos.y, destPos.z, true);
            }
        }

        if (m_seekTimer && m_creature->GetCreatorGuid())
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
