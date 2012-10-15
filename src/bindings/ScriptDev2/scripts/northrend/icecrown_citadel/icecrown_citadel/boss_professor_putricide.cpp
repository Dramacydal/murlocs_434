/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: boss_proffesor_putricide
SD%Complete: 70%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */
// Need implement model (aura?) for phase 2 and visual effects
// I don't know how do mutated_abomination :(
#include "precompiled.h"
#include "icecrown_citadel.h"
#include "QuestDef.h"

enum BossSpells
{
    SPELL_UNSTABLE_EXPERIMENT     = 70351, // Script effect not work on 10154. Spawn manually.
    SPELL_TEAR_GAS                = 71617,
    SPELL_TEAR_GAS_1              = 71615,
    SPELL_TEAR_GAS_2              = 71618,
    SPELL_CREATE_CONCOCTION       = 71621,
    SPELL_MALLEABLE_GOO           = 70852,
    SPELL_GUZZLE_POTIONS          = 71893,
    SPELL_MUTATED_STRENGTH        = 71603,
    SPELL_MUTATED_PLAGUE          = 72672,
    SPELL_OOZE_THROW              = 70342, // is triggered spell
//
    SPELL_GREEN_BOTTLE_0          = 71826,
    SPELL_ORANGE_BOTTLE_0         = 71827,
    SPELL_GREEN_BOTTLE_1          = 71702,
    SPELL_ORANGE_BOTTLE_1         = 71703,
//
    SPELL_THROW_BOTTLE_1          = 71273,
    SPELL_THROW_BOTTLE_2          = 71275,
    SPELL_THROW_BOTTLE_3          = 71276,
//
    NPC_GAS_CLOUD                 = 37562,
    SPELL_GASEOUS_BLOAT           = 70672,
    SPELL_EXPUNGED_GAS            = 70701,
//
    NPC_VOLATILE_OOZE             = 37697,
    SPELL_OOZE_ADHESIVE           = 70447,
    SPELL_OOZE_ERUPTION           = 70492,
//
    SPELL_OOZE_GAS_PROTECTION     = 70812,
    SPELL_OOZE_BEAM_PROTECTION    = 71530,
    SPELL_OOZE_TANK_PROTECTION    = 71770,
//
    NPC_MUTATED_ABOMINATION       = 37672,
    SPELL_MUTATED_TRANSFORMATION  = 70311,
    SPELL_EAT_OOZE                = 72527,
    SPELL_REGURGITATED_OOZE       = 70539,
    SPELL_MUTATED_SLASH           = 70542,
    SPELL_MUTATED_AURA            = 70405,
//
    NPC_CHOKING_GAS_BOMB          = 38159,
    SPELL_CHOKING_GAS             = 71259,
    SPELL_CHOKING_GAS_AURA        = 71278,
    SPELL_CHOKING_GAS_EXPLODE     = 71279,
    SPELL_CHOKING_GAS_EXPLODE_TRIGGER     = 71280,
    SPELL_ORANGE_RADIATION        = 45857, //Additional visual
//
    NPC_OOZE_PUDDLE               = 37690,
    SPELL_SLIME_PUDDLE            = 70343,
    SPELL_SLIME_PUDDLE_AURA       = 70346,

    SPELL_BERSERK                 = 47008,
//
    VIEW_1                        = 30881,
    VIEW_2                        = 30881,
    VIEW_3                        = 30993,
};

static Locations SpawnLoc[]=
{
    {4356.779785f, 3263.510010f, 389.398010f, 1.586f},  // 0 Putricide start point o=1.586
    {4295.081055f, 3188.883545f, 389.330261f, 4.270f},  // 1 Puticide Festergut say, o=4.27
    {4417.302246f, 3188.219971f, 389.332520f, 5.102f},  // 2 Putricide Rotface say o=5.102
};

struct MANGOS_DLL_DECL boss_proffesor_putricideAI : public BSWScriptedAI
{
    boss_proffesor_putricideAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *pInstance;
    uint8 stage;
    bool intro;
    uint32 UpdateTimer;
    bool movementstarted;
    uint8 nextPoint;
    uint8 slimetype;

    void Reset()
    {
        if (!pInstance)
            return;
        if (m_creature->isAlive()) pInstance->SetData(TYPE_PUTRICIDE, NOT_STARTED);
        stage = 0;
        slimetype = 0;
        intro = false;
        movementstarted = false;
        UpdateTimer = 1000;
        resetTimers();
        m_creature->SetRespawnDelay(7*DAY);
    }

    void EnterEvadeMode()
    {
        if (!pInstance) 
            return;

        m_creature->RemoveAllAuras();
        m_creature->DeleteThreatList();
        m_creature->CombatStop(true);
        m_creature->LoadCreatureAddon(false);
        if (m_creature->isAlive())
            m_creature->GetMotionMaster()->MoveTargetedHome();

        m_creature->SetLootRecipient(NULL);
        m_creature->SetHealth(m_creature->GetMaxHealth());
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetDisplayId(VIEW_1);

        Reset();
    }

    void MoveInLineOfSight(Unit* pWho) 
    {
        ScriptedAI::MoveInLineOfSight(pWho);

        if (!pInstance || intro)
            return;
        if (pInstance->GetData(TYPE_EVENT_NPC) == NPC_PROFESSOR_PUTRICIDE ||
            !pWho->IsWithinDistInMap(m_creature, 60.0f))
            return;

        DoScriptText(-1631240,m_creature, pWho);
        intro = true;
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0,1))
        {
            case 0:
                DoScriptText(-1631241,m_creature,pVictim);
                break;
            case 1:
                DoScriptText(-1631242,m_creature,pVictim);
                break;
        }
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type != POINT_MOTION_TYPE || !movementstarted)
            return;
        if (id == nextPoint) 
        {
            movementstarted = false;
            m_creature->GetMotionMaster()->MovementExpired();
        }
    }

    void Aggro(Unit *pWho) 
    {
        if (!pInstance)
            return;
        if (!pWho || pWho->GetTypeId() != TYPEID_PLAYER) 
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_PUTRICIDE))
        {
            pInstance->SendGMEncounterNotification(pWho, TYPE_PUTRICIDE);
            EnterEvadeMode();
            return;
        }

        pInstance->SetData(TYPE_PUTRICIDE, IN_PROGRESS);
        DoScriptText(-1631249,m_creature, pWho);

        if (is25())
        {
            Map::PlayerList const& pPlayers = pInstance->instance->GetPlayers();
            if (!pPlayers.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                {
                    Unit* pTarget = itr->getSource();
                    if (pTarget && pTarget->isAlive() && pTarget->HasAura(SPELL_SHADOWS_EDGE))
                        doAura(SHADOW_INFUSION_AURA, pTarget);
                }
            }
        }
    }

    void JustDied(Unit *killer)
    {
        if (!pInstance)
            return;
        pInstance->SetData(TYPE_PUTRICIDE, DONE);
        DoScriptText(-1631243,m_creature, killer);

        if (is25())
        {
            Map::PlayerList const& pPlayers = pInstance->instance->GetPlayers();
            if (!pPlayers.isEmpty())
            {
                DungeonPersistentState* state = (DungeonPersistentState*)pInstance->instance->GetPersistentState();
                for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                {
                    Player* pTarget = itr->getSource();
                    if (pTarget && pTarget->isAlive() && pTarget->HasAura(SPELL_SHADOWS_EDGE) && pTarget->FindQuestSlot(24749) != MAX_QUEST_LOG_SIZE)
                    {
                        pTarget->AreaExploredOrEventHappens(24749);
                        pTarget->AddSpellCooldown(QUEST_BLOOD_INFUSION_24756, 0, state->GetResetTime());
                    }
                }
            }
        }
    }

    void JustSummoned(Creature* summoned)
    {
        if(!pInstance || !summoned)
            return;

        if ( summoned->GetEntry() == NPC_VOLATILE_OOZE ||
            summoned->GetEntry() == NPC_GAS_CLOUD)
            if (Unit* pTarget = doSelectRandomPlayer())
            {
                summoned->SetInCombatWith(pTarget);
                summoned->AddThreat(pTarget, 100.0f);
            }
    }

    void StartMovement(uint32 id)
    {
        nextPoint = id;
        m_creature->GetMotionMaster()->Clear();
        m_creature->GetMotionMaster()->MovePoint(id, SpawnLoc[id].x, SpawnLoc[id].y, SpawnLoc[id].z);
        movementstarted = true;
    }

    void CallOoze()
    {
        if (doCast(SPELL_UNSTABLE_EXPERIMENT) == CAST_OK)
             switch(slimetype)
             {
                case 0:
                    doSummon(NPC_VOLATILE_OOZE);
                    slimetype = 1;
                    break;
                case 1:
                    doSummon(NPC_GAS_CLOUD);
                    slimetype = 0;
                    break;

                default:
                    break;
             }
    }

    void JustReachedHome()
    {
        if (pInstance) pInstance->SetData(TYPE_PUTRICIDE, FAIL);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!pDoneBy || !m_creature || !m_creature->isAlive() || !pInstance)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_PUTRICIDE))
        {
            pInstance->SendGMEncounterNotification(pDoneBy, TYPE_PUTRICIDE);
            EnterEvadeMode();
            uiDamage = 0;
            return;
        }

        if(pDoneBy->GetTypeId() == TYPEID_UNIT)
        {
            Creature* byCreature = (Creature*)pDoneBy;
            if (byCreature && byCreature->GetCreatureInfo())
            {
                uint32 cEnt = byCreature->GetCreatureInfo()->Entry;
                if ((cEnt == NPC_GAS_CLOUD) || (cEnt == NPC_VOLATILE_OOZE) || (cEnt == NPC_MUTATED_ABOMINATION) || (cEnt == NPC_CHOKING_GAS_BOMB) ||
                    (cEnt == NPC_OOZE_PUDDLE))
                    uiDamage = 0;
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (pInstance->GetData(TYPE_EVENT_NPC) == NPC_PROFESSOR_PUTRICIDE)
        {
            UpdateTimer = pInstance->GetData(TYPE_EVENT_TIMER);
            if (UpdateTimer <= diff)
            {
                debug_log("EventMGR: creature %u received signal %u ",m_creature->GetEntry(),pInstance->GetData(TYPE_EVENT));
                switch (pInstance->GetData(TYPE_EVENT))
                {
                    case 500:
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->NearTeleportTo(SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z, SpawnLoc[1].o);
                        DoScriptText(-1631201, m_creature);
                        UpdateTimer = 60000;
                        pInstance->SetData(TYPE_EVENT,510);
                        break;
                    case 510:
                        UpdateTimer = 5000;
                        if (pInstance->GetData(TYPE_FESTERGUT) == DONE)
                            pInstance->SetData(TYPE_EVENT,550);
                        if (pInstance->GetData(TYPE_FESTERGUT) == FAIL)
                            pInstance->SetData(TYPE_EVENT,630);
                        break;
                    case 550:
                        DoScriptText(-1631202, m_creature);
                        UpdateTimer = 10000;
                        pInstance->SetData(TYPE_EVENT,630);
                        break;
                    case 600:
                        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_creature->NearTeleportTo(SpawnLoc[2].x, SpawnLoc[2].y, SpawnLoc[2].z, SpawnLoc[2].o);
                        DoScriptText(-1631220, m_creature);
                        UpdateTimer = 60000;
                        pInstance->SetData(TYPE_EVENT,610);
                        break;
                    case 610:
                        UpdateTimer = 5000;
                        if (pInstance->GetData(TYPE_ROTFACE) == DONE)
                            pInstance->SetData(TYPE_EVENT,620);
                        if (pInstance->GetData(TYPE_ROTFACE) == FAIL)
                            pInstance->SetData(TYPE_EVENT,630);
                        break;
                    case 620:
                        DoScriptText(-1631202, m_creature);
                        UpdateTimer = 10000;
                        pInstance->SetData(TYPE_EVENT,630);
                        break;
                    case 630:
                        m_creature->NearTeleportTo(SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, SpawnLoc[0].o);
                        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        UpdateTimer = 2000;
                        pInstance->SetData(TYPE_EVENT,690);
                        break;
                    default:
                        break;
                }
            }
            else
                UpdateTimer -= diff;
            pInstance->SetData(TYPE_EVENT_TIMER, UpdateTimer);
        }

        if (m_creature->isInCombat())
        {
            if(!m_creature->getVictim() || (m_creature->getVictim() && m_creature->getVictim()->GetTypeId() == TYPEID_UNIT))
            {
                m_creature->DeleteThreatList();
                Unit* pTarget = doSelectRandomPlayerAtRange(5.0f);
                if (pTarget)
                {
                    m_creature->SetInCombatWith(pTarget);
                    m_creature->AddThreat(pTarget, 100.0f);
                }
                else
                    EnterEvadeMode();
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch(stage)
        {
            case 0: 
                if (timedQuery(SPELL_UNSTABLE_EXPERIMENT, diff))
                    CallOoze();

                timedCast(SPELL_OOZE_THROW, diff);

                if (timedQuery(SPELL_MALLEABLE_GOO, diff))
                    doCast(SPELL_MALLEABLE_GOO);

                DoMeleeAttackIfReady();

                if (m_creature->GetHealthPercent() < 80.0f )
                    stage = 1;

                break;
            case 1: 
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->AttackStop();
                SetCombatMovement(false);
                doCast(SPELL_TEAR_GAS_1);
                DoScriptText(-1631245,m_creature);
                StartMovement(0);
                stage = 2;
                break;
            case 2:
                if (movementstarted)
                    return;
                doCast(SPELL_CREATE_CONCOCTION);
                stage = 3;
                break;
            case 3:
                if (m_creature->IsNonMeleeSpellCasted(true,false,false) ||
                    !doSelectRandomPlayer(SPELL_TEAR_GAS_1, false))
                    return;
                DoScriptText(-1631246,m_creature);
                m_creature->SetDisplayId(VIEW_2);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                SetCombatMovement(true);
                stage = 4;
                break;
            case 4: 
                if (timedQuery(SPELL_UNSTABLE_EXPERIMENT, diff))
                    CallOoze();

                if (timedQuery(SPELL_THROW_BOTTLE_1, diff))
                    switch(urand(0,2))
                    {
                        case 0:
                            doCast(SPELL_THROW_BOTTLE_1);
                            break;
                        case 1:
                            doCast(SPELL_THROW_BOTTLE_2);
                            break;
                        case 2:
                            doCast(SPELL_THROW_BOTTLE_3);
                            break;
                        default:
                            break;
                    }

                timedCast(SPELL_OOZE_THROW, diff);

                timedCast(SPELL_MALLEABLE_GOO, diff);

                if (timedQuery(SPELL_MALLEABLE_GOO, diff))
                    doCast(SPELL_MALLEABLE_GOO);

                if (m_creature->GetDisplayId() != VIEW_2)
                    m_creature->SetDisplayId(VIEW_2);

                DoMeleeAttackIfReady();

                if (m_creature->GetHealthPercent() < 35.0f )
                    stage = 5;

                break;
            case 5:
                m_creature->InterruptNonMeleeSpells(true);
                m_creature->AttackStop();
                SetCombatMovement(false);
                doCast(SPELL_TEAR_GAS_1);
                DoScriptText(-1631245,m_creature);
                StartMovement(0);
                stage = 6;
                break;
            case 6:
                if (movementstarted) return;
                doCast(SPELL_GUZZLE_POTIONS);
                stage = 7;
                break;
            case 7:
                if (m_creature->IsNonMeleeSpellCasted(true,false,false))
                    return;
                if (m_creature->GetDisplayId() != VIEW_3)
                    m_creature->SetDisplayId(VIEW_3);
                if (!doSelectRandomPlayer(SPELL_TEAR_GAS_1, false))
                    return;
                DoScriptText(-1631247,m_creature);
                m_creature->SetDisplayId(VIEW_3);
                doCast(SPELL_MUTATED_STRENGTH);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                SetCombatMovement(true);
                stage = 8;
                break;
            case 8:
                timedCast(SPELL_MUTATED_PLAGUE, diff);
                if (m_creature->GetDisplayId() != VIEW_3)
                    m_creature->SetDisplayId(VIEW_3);
                DoMeleeAttackIfReady();

                break;
            default:
                break;
        }

        if (timedQuery(SPELL_BERSERK, diff))
        {
            doCast(SPELL_BERSERK);
            DoScriptText(-1631244,m_creature);
        }
    }
};

CreatureAI* GetAI_boss_proffesor_putricide(Creature* pCreature)
{
    return new boss_proffesor_putricideAI(pCreature);
}

struct MANGOS_DLL_DECL mob_icc_gas_cloudAI : public BSWScriptedAI
{
    mob_icc_gas_cloudAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Unit* pTarget;
    bool expunded;
    uint32 delay;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetWalk(true);
        m_creature->SetSpeedRate(MOVE_WALK, 0.5f);
        m_creature->SetSpeedRate(MOVE_RUN, 0.2f);
        pTarget = NULL;
        expunded = false;
        delay = 10000;
    }

    void JustDied(Unit *killer)
    {
        if (!m_pInstance) return;
        if (pTarget && pTarget->isAlive())
            doRemove(SPELL_GASEOUS_BLOAT, pTarget);
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance || who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!pTarget || pTarget != who )
            pTarget = who;
        else
            return;

        delay = 10000;

        if (pTarget)
            doAura(SPELL_GASEOUS_BLOAT, pTarget);

        DoStartMovement(who);
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;
        m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_PUTRICIDE) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (expunded)
            m_creature->ForcedDespawn();

        if (m_creature->getVictim() && m_creature->getVictim()->GetTypeId() != TYPEID_PLAYER)
        {
            if (Unit* pTarget = doSelectRandomPlayer())
            {
                m_creature->SetInCombatWith(pTarget);
                m_creature->AddThreat(pTarget, 100.0f);
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!pTarget) Aggro(m_creature->getVictim());

        if (delay <= uiDiff)
        {
            if (pTarget && pTarget->isAlive() && pTarget->IsWithinDistInMap(m_creature, 3.0f))
            {
               doCast(SPELL_EXPUNGED_GAS, pTarget);
               doRemove(SPELL_GASEOUS_BLOAT, pTarget);
               expunded = true;
            }
        }
        else
            delay -= uiDiff;
    }
};

CreatureAI* GetAI_mob_icc_gas_cloud(Creature* pCreature)
{
    return new mob_icc_gas_cloudAI(pCreature);
}

struct MANGOS_DLL_DECL mob_icc_volatile_oozeAI : public BSWScriptedAI
{
    mob_icc_volatile_oozeAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    Unit* pTarget;
    bool finita;
    uint32 delay;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetWalk(true);
        m_creature->SetSpeedRate(MOVE_WALK, 0.5f);
        m_creature->SetSpeedRate(MOVE_RUN, 0.2f);
        pTarget = NULL;
        finita = false;
        delay = 10000;
    }

    void JustDied(Unit *killer)
    {
        if (!m_pInstance) return;
        if (pTarget && pTarget->isAlive())
            doRemove(SPELL_OOZE_ADHESIVE, pTarget);
    }

    void Aggro(Unit *who)
    {
        if (!m_pInstance || !who || who->GetTypeId() != TYPEID_PLAYER)
            return;

        if (!pTarget || pTarget != who )
            pTarget = who;
        else
            return;

        delay = 10000;

        if (pTarget)
            doAura(SPELL_OOZE_ADHESIVE, pTarget);

        DoStartMovement(pTarget);
    }

    void JustReachedHome()
    {
        if (!m_pInstance) return;
             m_creature->ForcedDespawn();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance )
            return;

        if ((m_pInstance->GetData(TYPE_PUTRICIDE) != IN_PROGRESS) || finita)
            m_creature->ForcedDespawn();

        if (m_creature->getVictim() && m_creature->getVictim()->GetTypeId() != TYPEID_PLAYER)
        {
            if (Unit* pTarget = doSelectRandomPlayer())
            {
                m_creature->SetInCombatWith(pTarget);
                m_creature->AddThreat(pTarget, 100.0f);
            }
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (!pTarget) 
            Aggro(m_creature->getVictim());

        if (delay <= uiDiff)
        {
            if (pTarget && pTarget->isAlive() && pTarget->IsWithinDistInMap(m_creature, 3.0f))
            {
                doCast(SPELL_OOZE_ERUPTION);
                doRemove(SPELL_OOZE_ADHESIVE, pTarget);
                finita = true;
            }
        }
        else
            delay -= uiDiff;
    }
};

CreatureAI* GetAI_mob_icc_volatile_ooze(Creature* pCreature)
{
    return new mob_icc_volatile_oozeAI(pCreature);
}

struct MANGOS_DLL_DECL mob_choking_gas_bombAI : public BSWScriptedAI
{
    mob_choking_gas_bombAI(Creature *pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool finita;
    uint32 copy_timer;

    void Reset()
    {
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetInCombatWithZone();
        m_creature->SetDisplayId(11686);
        SetCombatMovement(false);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->CastSpell(m_creature, SPELL_ORANGE_RADIATION, false);
        finita = false;
        copy_timer = 8000;
    }

    void AttackStart(Unit *pWho)
    {
        return;
    }

    void JustReachedHome()
    {
        return;
    }

    void JustDied(Unit *killer)
    {
        if (finita) return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance) return;

        if (m_pInstance->GetData(TYPE_PUTRICIDE) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (m_creature->getVictim() && m_creature->getVictim()->GetTypeId() != TYPEID_PLAYER)
        {
            if (Unit* pTarget = doSelectRandomPlayer())
            {
                m_creature->SetInCombatWith(pTarget);
                m_creature->AddThreat(pTarget, 100.0f);
            }
        }

        if (!m_creature->HasAura(SPELL_CHOKING_GAS))
            m_creature->CastSpell(m_creature, SPELL_CHOKING_GAS, false);

        if (!finita && m_creature->HasAura(SPELL_CHOKING_GAS_EXPLODE_TRIGGER))
        {
//            m_creature->CastSpell(m_creature,SPELL_CHOKING_GAS_EXPLODE,false);
            finita = true;
        }

        if (!finita)
        {
            if (copy_timer <= uiDiff)
            {
                float fPosX, fPosY, fPosZ;
                m_creature->GetPosition(fPosX, fPosY, fPosZ);
                if (Creature* pCopy = m_creature->SummonCreature(m_creature->GetEntry(), fPosX, fPosY, fPosZ, 0, TEMPSUMMON_TIMED_DESPAWN, 11200))
                     pCopy->CastSpell(pCopy, SPELL_CHOKING_GAS_EXPLODE_TRIGGER, false);
                copy_timer = 3600000;
            }
            else
                copy_timer -= uiDiff;
        }

    }

};

CreatureAI* GetAI_mob_choking_gas_bomb(Creature* pCreature)
{
    return new mob_choking_gas_bombAI(pCreature);
}

struct MANGOS_DLL_DECL mob_ooze_puddleAI : public BSWScriptedAI
{
    mob_ooze_puddleAI(Creature *pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    float m_Size;
    float m_Size0;
    uint32 grow_timer;

    void Reset()
    {
        if(!m_pInstance)
            return;
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetDisplayId(11686);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        m_creature->SetInCombatWithZone();
        SetCombatMovement(false);
        m_Size0 = m_creature->GetObjectScale();
        if (m_Size0 > 1.0f)
        {
            m_creature->SetObjectScale(1.0f);
            m_Size0 = 1.0f;
        }
        m_Size = m_Size0;
        grow_timer = 500;
    }

    void AttackStart(Unit *who)
    {
        return;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_PUTRICIDE) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (m_creature->getVictim() && m_creature->getVictim()->GetTypeId() != TYPEID_PLAYER)
        {
            if (Unit* pTarget = doSelectRandomPlayer())
            {
                m_creature->SetInCombatWith(pTarget);
                m_creature->AddThreat(pTarget, 100.0f);
            }
        }
        if (!m_creature->HasAura(SPELL_SLIME_PUDDLE))
            m_creature->CastSpell(m_creature, SPELL_SLIME_PUDDLE, false);

        // Override especially for clean core
        if (m_Size / m_Size0 >= 2.6f) m_creature->ForcedDespawn();

        if (grow_timer <= uiDiff)
        {
            m_Size = m_Size*1.01;
            m_creature->SetObjectScale(m_Size);
            grow_timer = 500;
        }
        else
            grow_timer -= uiDiff;
    }

};

CreatureAI* GetAI_mob_ooze_puddle(Creature* pCreature)
{
    return new mob_ooze_puddleAI(pCreature);
}

void AddSC_boss_professor_putricide()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_proffesor_putricide";
    newscript->GetAI = &GetAI_boss_proffesor_putricide;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_icc_volatile_ooze";
    newscript->GetAI = &GetAI_mob_icc_volatile_ooze;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_icc_gas_cloud";
    newscript->GetAI = &GetAI_mob_icc_gas_cloud;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_choking_gas_bomb";
    newscript->GetAI = &GetAI_mob_choking_gas_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ooze_puddle";
    newscript->GetAI = &GetAI_mob_ooze_puddle;
    newscript->RegisterSelf();
}
