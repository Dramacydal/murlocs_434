#include "precompiled.h"

enum
{
    SPELL_APPARITION_CLONE          = 87213,
    SPELL_APPARITION_VISUAL         = 87427,
    SPELL_APPARITION_DEATH_VISUAL   = 87529,
    SPELL_APPARITION_DAMAGE         = 87532,
};

struct MANGOS_DLL_DECL npc_shadowy_apparitionAI : public ScriptedAI
{
    npc_shadowy_apparitionAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool init;
    bool despawned;
    uint32 summonedBySpell;
    ObjectGuid targetGuid;
    ObjectGuid ownerGuid;

    void Reset() override
    {
        init = false;
        despawned = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
        ownerGuid.Clear();
        targetGuid.Clear();

        Unit* owner = m_creature->GetOwner();
        if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
            return;

        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 2048);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_2, owner->GetUInt32Value(UNIT_FIELD_BYTES_2));
        m_creature->SetUInt32Value(UNIT_FIELD_FLAGS, UNIT_FLAG_PVP_ATTACKABLE);
        m_creature->SetFloatValue(UNIT_FIELD_BOUNDINGRADIUS, DEFAULT_WORLD_OBJECT_SIZE);
        m_creature->SetFloatValue(UNIT_FIELD_COMBATREACH, 1.5f);

        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID,     owner->GetUInt32Value(PLAYER_VISIBLE_ITEM_16_ENTRYID));
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, owner->GetUInt32Value(PLAYER_VISIBLE_ITEM_17_ENTRYID));
        m_creature->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, owner->GetUInt32Value(PLAYER_VISIBLE_ITEM_18_ENTRYID));
        m_creature->SetSpeedRate(MOVE_RUN, owner->GetSpeedRate(MOVE_RUN), true);
        m_creature->CastSpell(m_creature, SPELL_APPARITION_CLONE, true);
        m_creature->CastSpell(m_creature, SPELL_APPARITION_VISUAL, true);
    }

    void MovementInform(uint32 moveType, uint32 pointId) override
    {
        if (moveType != CHASE_MOTION_TYPE)
            return;

        if (despawned)
            return;

        Unit* victim = m_creature->GetMap()->GetUnit(targetGuid);
        if (!victim)
            return;

        if (victim->GetDistance(m_creature) > 1.5f)
            return;

        despawned = true;
        m_creature->CastSpell(victim, SPELL_APPARITION_DAMAGE, true);
        m_creature->CastSpell(m_creature, SPELL_APPARITION_DEATH_VISUAL, true);
        if (Unit* creator = m_creature->GetCreator())
            // Item - Priest T13 Shadow 4P Bonus (Shadowfiend and Shadowy Apparition)
            if (creator->HasAura(105844))
                for (int i = 0; i < 3; ++i)
                    creator->CastSpell(creator, 77487, true);

        m_creature->ForcedDespawn(200);
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (despawned)
            return;

        if (who->GetObjectGuid() != targetGuid)
            return;

        if (who->GetDistance(m_creature) > 1.5f)
            return;

        despawned = true;
        m_creature->CastSpell(who, SPELL_APPARITION_DAMAGE, true, NULL, NULL, ownerGuid);
        m_creature->CastSpell(m_creature, SPELL_APPARITION_DEATH_VISUAL, true);
        if (Unit* creator = m_creature->GetCreator())
            // Item - Priest T13 Shadow 4P Bonus (Shadowfiend and Shadowy Apparition)
            if (creator->HasAura(105844))
                for (int i = 0; i < 3; ++i)
                    creator->CastSpell(creator, 77487, true);

        m_creature->ForcedDespawn(200);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!init)
        {
            m_creature->FixateTarget(m_creature->getVictim());
            init = true;
            ownerGuid = m_creature->GetCreatorGuid();
            if (Unit* victim = m_creature->getVictim())
                targetGuid = victim->GetObjectGuid();
            else
            {
                m_creature->ForcedDespawn();
                despawned = true;
                return;
            }
        }
    }
};

CreatureAI* GetAI_npc_shadowy_apparition(Creature* pCreature)
{
    return new npc_shadowy_apparitionAI(pCreature);
}

void AddSC_npc_priest_summons()
{
    Script* pNewScript;
    pNewScript = new Script;
    pNewScript->Name = "npc_shadowy_apparition";
    pNewScript->GetAI = &GetAI_npc_shadowy_apparition;
    pNewScript->RegisterSelf();
}
