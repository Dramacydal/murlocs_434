#include "precompiled.h"
#include "stonecore.h"

/***Note: Spell 78807 and 92426 - requires range in 15 yd ***/
/***Note: Spell 92427 - requires pre-cast so that 92426 works - 94661 damage***/

enum Yells
{
    OZRUK_AGGRO     = 1,
    OZRUK_SHIELD,
    OZRUK_KILL,
    OZRUK_DEATH,
    WARNING_SLAM,
    WARNING_SHAT,
};

enum Spells
{
    SPELL_GROUND_SLAM   = 78903,  //Very violent. GTFO.
    SPELL_RUPTURE       = 80803,  //This - is implemented a little hacky.
    SPELL_RUPTURE_DMG   = 92381,
    SPELL_RUPTURE_SPK   = 92383,

    SPELL_ELEM_BULKWAR  = 78939,
    SPELL_ELEM_SPIKE    = 78835,
    SPELL_SHATTER       = 78807,
    SPELL_PARALYZE      = 92426,
    SPELL_PARALYZE_DMG  = 94661,
};

enum Events
{
    EVENT_GROUND_SLAM,
    EVENT_ELEM_BULKWAR,
    EVENT_ELEM_SPIKE,
    EVENT_SHATTER,
    EVENT_PARALYZE,
    EVENT_PARALYZE_DAMAGE,
};

enum Creatures
{
    NPC_RUPTURE   = 49597,
};

/*** Note: Doors are unknown. ***/

struct MANGOS_DLL_DECL boss_ozrukAI : public ScriptedAI
{
    boss_ozrukAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    EventMap events;

    void Reset()
    {
        events.Reset();

        if (m_pInstance)
            m_pInstance->SetBossState(BOSS_OZRUK, NOT_STARTED);

        //summons.DespawnAll();
    }

    void EnterEvadeMode()
    {
        events.Reset();

        if (m_pInstance)
        {
            m_pInstance->SetBossState(BOSS_OZRUK, FAIL);
            m_pInstance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, m_creature); // Remove

            //m_pInstance->HandleGameObject(0000000, true); // Open the doors.
            //m_pInstance->HandleGameObject(0000000, true);
        }
    }

    void Aggro(Unit* who)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OZRUK, IN_PROGRESS);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_OZRUK, FAIL);
    }

    void JustDied(Unit* killer)
    {
        if (m_pInstance)
        {
            m_pInstance->SetBossState(BOSS_OZRUK, DONE);
            m_pInstance->SetData(TYPE_OZRUK, DONE);

            //m_pInstance->HandleGameObject(0000000, true); // Open the doors.
            //m_pInstance->HandleGameObject(0000000, true);

            m_pInstance->SendEncounterUnit(ENCOUNTER_FRAME_DISENGAGE, m_creature); // Remove
        }
    }

    void EnterCombat(Unit* /*who*/)
    {
        if (m_pInstance)
        {
            m_pInstance->SetBossState(BOSS_OZRUK, IN_PROGRESS);
            m_pInstance->SendEncounterUnit(ENCOUNTER_FRAME_ENGAGE, m_creature); // Add

            //m_pInstance->HandleGameObject(0000000, false); // Close the doors.
            //m_pInstance->HandleGameObject(0000000, false);
        }

        events.ScheduleEvent(EVENT_GROUND_SLAM, 7000);
        events.ScheduleEvent(EVENT_ELEM_BULKWAR, 4000);
        events.ScheduleEvent(EVENT_ELEM_SPIKE, 12000);

        m_creature->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 diff)
    {
        if (!UpdateVictim() || m_creature->IsNonMeleeSpellCasted(false))
            return;

        events.Update(diff);

            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {       
                    case EVENT_GROUND_SLAM:
                        DoCastVictim(SPELL_GROUND_SLAM);
                        events.ScheduleEvent(EVENT_GROUND_SLAM, urand(10000, 12000));
                        break;

                    case EVENT_ELEM_BULKWAR:
                        DoCast(m_creature, SPELL_ELEM_BULKWAR);
                        events.ScheduleEvent(EVENT_ELEM_BULKWAR, urand(7000, 11000));
                        break;

                    case EVENT_ELEM_SPIKE:
                        DoCast(m_creature, SPELL_ELEM_SPIKE);
                        if(!m_bIsRegularMode)
                        {
                            events.ScheduleEvent(EVENT_PARALYZE, 9500);
                            m_creature->CastSpell(m_creature, 92427, true);
                        }
                        events.ScheduleEvent(EVENT_SHATTER, 10000);
                        break;

                    case EVENT_PARALYZE:
                        DoCastAOE(SPELL_PARALYZE);
                        events.ScheduleEvent(EVENT_PARALYZE_DAMAGE, 7500);
                        break;

                    case EVENT_SHATTER:
                        DoCastAOE(SPELL_SHATTER);
                        events.ScheduleEvent(EVENT_ELEM_SPIKE, urand(12000, 15000));
                        break;

                    case EVENT_PARALYZE_DAMAGE:
                        if (Unit* target = m_creature->getVictim())
                        {
                            std::list<HostileReference*> t_list = m_creature->getThreatManager().getThreatList();
                            for (std::list<HostileReference*>::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                            {
                                Unit* target = m_creature->GetMap()->GetUnit((*itr)->getUnitGuid());
                                if (target && target->HasAura(SPELL_PARALYZE))
                                {
                                    target->CastSpell(target, SPELL_PARALYZE_DMG, true, NULL, NULL, m_creature->GetObjectGuid());
                                }
                            }
                        }
                }
            }

            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_ruptureAI : public ScriptedAI // 51422
{
    npc_ruptureAI(Creature* creature) : ScriptedAI(creature)
    {
        m_pInstance = (ScriptedInstance*)creature->GetInstanceData();
        timerAura    = 100;
        //creature->SetReactState(REACT_PASSIVE);
    }

    ScriptedInstance* m_pInstance;
    uint32 timerAura;

    void Reset() { }

    void EnterCombat(Unit* who)
    {
        m_creature->SetSpeedRate(MOVE_RUN, 0.8f);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        //m_creature->SetReactState(REACT_PASSIVE);
        m_creature->SetInCombatWithZone();
        float x, y, z;
        m_creature->GetClosePoint(x, y, z, m_creature->GetObjectBoundingRadius() / 3, 100.0f);
        m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
        m_creature->ForcedDespawn(6000);
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_pInstance && m_pInstance->GetData(TYPE_OZRUK) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (timerAura <= diff)
        {
        if (Unit* target = m_creature->getVictim())
            if (target && target->IsWithinDistInMap(m_creature, 2.0f))
            {
                DoCast(target, SPELL_RUPTURE_DMG);
                DoCast(target, SPELL_RUPTURE_SPK);
            }
            timerAura = 500;
        } else timerAura -= diff;
    }
};
/*
class spell_ozruk_paralyze : public SpellScriptLoader
{
    public:
        spell_ozruk_paralyze() :  SpellScriptLoader("spell_ozruk_paralyze") { }

        class spell_ozruk_paralyze_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_ozruk_paralyze_AuraScript);

            void OnRemove(AuraEffect const* /*aurEff* /, AuraEffectHandleModes /*mode* /)
            {
                switch (GetTargetApplication()->GetRemoveMode())
                {
                    case AURA_REMOVE_BY_ENEMY_SPELL:
                        break;
                    case AURA_REMOVE_BY_EXPIRE:
                        GetTarget()->CastSpell(GetTarget(), 94661, true);
                        break;
                    case AURA_REMOVE_BY_DEATH:
                        break;
                    default:
                        return;
                }
            }

            void Register()
            {
                AfterEffectRemove += AuraEffectRemoveFn(spell_ozruk_paralyze_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_ozruk_paralyze_AuraScript();
        }
};*/

CreatureAI* GetAI_boss_ozruk(Creature* pCreature)
{
    return new boss_ozrukAI(pCreature);
}

CreatureAI* GetAI_npc_rupture(Creature* pCreature)
{
    return new npc_ruptureAI(pCreature);
}

void AddSC_boss_ozruk()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ozruk";
    pNewScript->GetAI = &GetAI_boss_ozruk;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_rupture";
    pNewScript->GetAI = &GetAI_npc_rupture;
    pNewScript->RegisterSelf();
}
