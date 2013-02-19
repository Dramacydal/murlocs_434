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
SDName: boss_sindragosa
SD%Complete: 80%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */
// Need correct timers and models
#include "precompiled.h"
#include "icecrown_citadel.h"
#include "QuestDef.h"

enum BossSpells
{
    // Sindragosa
    SPELL_FROST_AURA_1       = 70084,
    SPELL_CLEAVE_1           = 19983,
    SPELL_TAIL_SMASH         = 71077,
    SPELL_FROST_BREATH_1     = 69649,
    SPELL_PERMEATING_CHILL   = 70107,
    SPELL_UNCHAINED_MAGIC    = 69762,
    SPELL_INSTABILITY        = 69766,
    SPELL_ICY_GRIP           = 70117,
    SPELL_BLISTERING_COLD    = 70123,

    SPELL_FROST_BEACON       = 70126,
    SPELL_ICY_TOMB           = 70157,
    SPELL_ASPHYXATION        = 71665,
    SPELL_FROST_BOMB         = 69845,
    SPELL_FROST_BOMB_TRIGGER = 69846,
    SPELL_FROST_BOMB_VISUAL  = 64624,
    SPELL_FROST_BOMB_VISUAL2 = 64626,
    SPELL_ICE_TOMB_TRIGGER   = 69675,
    SPELL_MYSTIC_BUFFET      = 70128,

    NPC_ICE_TOMB             = 36980,
    NPC_FROST_BOMB           = 37186,

    SPELL_FLY                = 59553,

    SPELL_BERSERK            = 47008,

    // Rimefang
    SPELL_FROST_AURA         = 71387,
    SPELL_FROST_BREATH       = 71386,
    SPELL_ICY_BLAST          = 71376,

    // Spinestalker
    SPELL_BELLOWING_ROAR     = 36922,
    SPELL_CLEAVE             = 40505,
    SPELL_TAIL_SWEEP         = 71369,
};

static Locations SpawnLoc[]=
{
    {4408.052734f, 2484.825439f, 203.374207f},  // 0 Sindragosa spawn
    {4474.239746f, 2484.243896f, 231.0f},       // 1 Sindragosa fly o=3.11
    {4474.239746f, 2484.243896f, 203.380402f},  // 2 Sindragosa fly - ground point o=3.11
};

struct MANGOS_DLL_DECL boss_sindragosaAI : public BSWScriptedAI
{
    boss_sindragosaAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool MovementStarted;
    bool gripped;
    Unit* marked[5];
    uint8 bombs;

    void Reset()
    {
        if (!pInstance)
            return;

        resetTimers();
        setStage(0);
        bombs = 0;
        gripped = false;
        m_creature->SetRespawnDelay(7*DAY);
        m_creature->SetSpeedRate(MOVE_RUN, 1.0f);
        m_creature->SetSpeedRate(MOVE_WALK, 1.0f);

        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);

        Creature* pTemp1 = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_RIMEFANG));
        Creature* pTemp2 = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_SPINESTALKER));

        if (pTemp1 && pTemp1->isAlive() || pTemp2 && pTemp2->isAlive())
            m_creature->ForcedDespawn();
    }

    void KilledUnit(Unit* pVictim)
    {
        switch (urand(0,1)) 
        {
            case 0:
                DoScriptText(-1631421,m_creature,pVictim);
                break;
            case 1:
                DoScriptText(-1631422,m_creature,pVictim);
                break;
        }
    }

    void JustReachedHome()
    {
        if (!pInstance)
            return;

        pInstance->SetData(TYPE_SINDRAGOSA, FAIL);
        doRemoveFromAll(SPELL_ICY_TOMB);
        DoScriptText(-1631422,m_creature);

        if (Creature* pTemp = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_RIMEFANG)))
           if (!pTemp->isAlive())
                pTemp->SetRespawnDelay(7*DAY);
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_SPINESTALKER)))
           if (!pTemp->isAlive())
                pTemp->SetRespawnDelay(7*DAY);
    }

    void EnterEvadeMode()
    {
        if (!pInstance)
            return;

        if (getStage() == 4)
            return;

        SetCombatMovement(true);
        m_creature->RemoveAurasDueToSpell(SPELL_FLY);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
        m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
        m_creature->SetWalk(false);

        ScriptedAI::EnterEvadeMode();
    }

    void Aggro(Unit *who) 
    {
        if(!pInstance)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_SINDRAGOSA))
        {
            pInstance->SendGMEncounterNotification(who, TYPE_SINDRAGOSA);
            EnterEvadeMode();
            return;
        }

        DoScriptText(-1631420,m_creature,who);
        doCast(SPELL_FROST_AURA_1);

        if (is25())
        {
            Map::PlayerList const& pPlayers = pInstance->instance->GetPlayers();
            if (!pPlayers.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                {
                    Unit* pTarget = itr->getSource();
                    if (pTarget && pTarget->isAlive() && pTarget->HasAura(SPELL_SHADOWS_EDGE))
                        doAura(FROST_IMBUED_BLADE_AURA, pTarget);
                }
            }
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32& uiDamage)
    {
        if (!pInstance)
            return;

        if (!pInstance->IsEncounterAvailable(TYPE_SINDRAGOSA))
        {
            pInstance->SendGMEncounterNotification(pDoneBy, TYPE_SINDRAGOSA);
            EnterEvadeMode();
            uiDamage = 0;
            return;
        }
    }

    void JustDied(Unit *killer)
    {
        if (!pInstance)
            return;

        doRemoveFromAll(SPELL_ICY_TOMB);
        pInstance->SetData(TYPE_SINDRAGOSA, DONE);
        DoScriptText(-1631423,m_creature,killer);

        if (is25())
        {
            Map::PlayerList const& pPlayers = pInstance->instance->GetPlayers();
            if (!pPlayers.isEmpty())
            {
                for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                {
                    Player* pTarget = itr->getSource();
                    if (pTarget && pTarget->isAlive() && pTarget->HasAura(SPELL_SHADOWS_EDGE) && pTarget->FindQuestSlot(24757) != MAX_QUEST_LOG_SIZE && !pTarget->HasSpellCooldown(QUEST_FROST_INFUSION_24757))
                        pTarget->AreaExploredOrEventHappens(24757);
                }
            }
        }

        if (Creature* pTemp = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_RIMEFANG)))
            pTemp->SetRespawnDelay(7*DAY);
        if (Creature* pTemp = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_SPINESTALKER)))
            pTemp->SetRespawnDelay(7*DAY);
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (!pInstance)
            return;

        if (type != POINT_MOTION_TYPE || !MovementStarted)
            return;

        if (id == 1)
        {
            m_creature->GetMotionMaster()->MovementExpired();
            MovementStarted = false;
        }
    }

    void IceMark()
    {
        for (uint8 i = 0; i < getSpellData(SPELL_FROST_BEACON); i++)
            marked[i] = NULL;

        for (uint8 i = 0; i < getSpellData(SPELL_FROST_BEACON); i++)
            if (marked[i] = doSelectRandomPlayer(SPELL_FROST_BEACON, false, 200.0f))
                doCast(SPELL_FROST_BEACON, marked[i]);
    }

    void IceBlock()
    {
        for (uint8 i = 0; i < getSpellData(SPELL_FROST_BEACON); ++i)
        {
            if (marked[i] && marked[i]->isAlive())
            {
                doCast(SPELL_ICY_TOMB, marked[i]);
                marked[i]->RemoveAurasDueToSpell(SPELL_FROST_BEACON);
                float fPosX, fPosY, fPosZ;
                marked[i]->GetPosition(fPosX, fPosY, fPosZ);

                if (Unit* pTemp1 = doSummon(NPC_ICE_TOMB,fPosX, fPosY, fPosZ))
                    pTemp1->AddThreat(marked[i], 1000.0f);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_creature->isInCombat())
        {
            if (!m_creature->getVictim() || m_creature->getVictim() && m_creature->getVictim()->GetTypeId() == TYPEID_UNIT)
            {
                m_creature->DeleteThreatList();
                Unit* pTarget = doSelectRandomPlayerAtRange(20.0f);
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

        switch(getStage())
        {
            case 0:
                timedCast(SPELL_CLEAVE_1, diff);
                timedCast(SPELL_TAIL_SMASH, diff);
                timedCast(SPELL_FROST_BREATH_1, diff);
                timedCast(SPELL_PERMEATING_CHILL, diff);

                timedCast(SPELL_UNCHAINED_MAGIC, diff);

                if (timedQuery(SPELL_ICY_GRIP, diff) && !gripped)
                {
                    doCast(SPELL_ICY_GRIP);
                    gripped = true;
                }

                if (gripped && !m_creature->IsNonMeleeSpellCasted(true,false,false))
                {
                    DoScriptText(-1631426,m_creature);
                    doCast(SPELL_BLISTERING_COLD);
                    gripped = false;
                }

                if (timedQuery(SPELL_FROST_BEACON, diff) && m_creature->GetHealthPercent() < 85.0f)
                    setStage(1);

                if (m_creature->GetHealthPercent() < 35.0f) 
                {
                    doCast(SPELL_MYSTIC_BUFFET);
                    setStage(9);
                    DoScriptText(-1631429,m_creature);
                }
                break;
            case 1: 
                DoScriptText(-1631425,m_creature);
                IceMark();
                setStage(2);
                MovementStarted = true;
                SetCombatMovement(false);
                m_creature->CastSpell(m_creature,SPELL_FLY, true);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 50331648);
                m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 50331648);
                m_creature->SetLevitate(true);
                m_creature->GetMotionMaster()->MovePoint(1, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_FLY_SIT_GROUND_UP);
                break;
            case 2:
                if (!MovementStarted) 
                {
                    setStage(3);
                    m_creature->SetOrientation(3.1f);
                    m_creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, EMOTE_STATE_FLY_FALL);
                }
                break;
            case 3:
                IceBlock();
                setStage(4);
                break;
            case 4: 
                if (timedQuery(SPELL_FROST_BOMB, diff))
                {
                    if (Unit* pTemp = doSelectRandomPlayerAtRange(300.0f))
                    {
                        doCast(SPELL_FROST_BOMB_TRIGGER, pTemp);
                        ++bombs;
                    }
                }

                timedCast(SPELL_FROST_BREATH_1, diff);

                if (timedQuery(SPELL_FROST_BEACON, diff) || bombs >= getSpellData(SPELL_FROST_BOMB))
                    setStage(5);
                break;
            case 5:
                bombs = 0;
                MovementStarted = true;
                SetCombatMovement(false);
                m_creature->GetMotionMaster()->MovePoint(1, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z);
                setStage(6);
                m_creature->HandleEmoteCommand(EMOTE_STATE_FLY_SIT_GROUND);
                break;
            case 6:
                if (!MovementStarted) 
                {
                    setStage(0);
                    SetCombatMovement(true);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    m_creature->RemoveAurasDueToSpell(SPELL_FLY);
                    m_creature->SetUInt32Value(UNIT_FIELD_BYTES_0, 0);
                    m_creature->SetUInt32Value(UNIT_FIELD_BYTES_1, 0);
                    m_creature->SetLevitate(false);
                }
                break;
            case 9:
                timedCast(SPELL_CLEAVE_1, diff);
                timedCast(SPELL_TAIL_SMASH, diff);
                timedCast(SPELL_FROST_BREATH_1, diff);
                timedCast(SPELL_PERMEATING_CHILL, diff);
                timedCast(SPELL_UNCHAINED_MAGIC, diff);

                if (timedQuery(SPELL_ICY_GRIP, diff) && !gripped)
                {
                    doCast(SPELL_ICY_GRIP);
                    gripped = true;
                }

                if (gripped && !m_creature->IsNonMeleeSpellCasted(true,false,false))
                {
                    DoScriptText(-1631426,m_creature);
                    doCast(SPELL_BLISTERING_COLD);
                    gripped = false;
                }
                break;
            default:
                break;
        }

        if (timedQuery(SPELL_BERSERK, diff))
        {
            doCast(SPELL_BERSERK);
            DoScriptText(-1631424,m_creature);
        }

        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_boss_sindragosa(Creature* pCreature)
{
    return new boss_sindragosaAI(pCreature);
}

struct MANGOS_DLL_DECL mob_ice_tombAI : public BSWScriptedAI
{
    mob_ice_tombAI(Creature *pCreature) : BSWScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint64 victimGUID;

    void Reset()
    {
        SetCombatMovement(false);
        victimGUID = 0;
        m_creature->SetRespawnDelay(7*DAY);
    }

    void Aggro(Unit* pWho)
    {
        if (!victimGUID && pWho && pWho->GetTypeId() == TYPEID_PLAYER)
        {
             if (pWho->HasAura(SPELL_ICY_TOMB))
             {
                 victimGUID = pWho->GetGUID();
                 m_creature->SetInCombatWith(pWho);
             }
             else if (Unit* pTarget = doSelectRandomPlayer(SPELL_ICY_TOMB,true,3.0f))
             {
                 victimGUID = pTarget->GetGUID();
                 m_creature->SetInCombatWith(pTarget);
             }
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (uiDamage > m_creature->GetHealth())
            if (Player* pVictim = m_creature->GetMap()->GetPlayer(victimGUID))
                doRemove(SPELL_ICY_TOMB, pVictim);
    }

    void AttackStart(Unit *pWho) { }

    void KilledUnit(Unit* _Victim)
    {
        if (Player* pVictim = m_creature->GetMap()->GetPlayer(victimGUID))
            doRemove(SPELL_ICY_TOMB,pVictim);
    }

    void JustDied(Unit* Killer)
    {
        if (Player* pVictim = m_creature->GetMap()->GetPlayer(victimGUID)) 
            doRemove(SPELL_ICY_TOMB,pVictim);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if ((m_pInstance && m_pInstance->GetData(TYPE_SINDRAGOSA) != IN_PROGRESS) ||
           (victimGUID && m_creature->GetMap()->GetPlayer(victimGUID) && !m_creature->GetMap()->GetPlayer(victimGUID)->HasAura(SPELL_ICY_TOMB)))
        {
            if (Player* pVictim = m_creature->GetMap()->GetPlayer(victimGUID))
                doRemove(SPELL_ICY_TOMB,pVictim);

            m_creature->ForcedDespawn();
        }
    }
};

CreatureAI* GetAI_mob_ice_tomb(Creature* pCreature)
{
    return new mob_ice_tombAI(pCreature);
}

struct MANGOS_DLL_DECL mob_frost_bombAI : public ScriptedAI
{
    mob_frost_bombAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint32 boom_timer;
    uint32 visual_timer;
    bool finita;

    void Reset()
    {
        SetCombatMovement(false);
        m_creature->SetRespawnDelay(7*DAY);
        visual_timer = 6000;
        boom_timer = 9000;
        finita = false;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
    }

    void AttackStart(Unit *pWho) { }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_pInstance && m_pInstance->GetData(TYPE_SINDRAGOSA) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (finita)
        {
            m_creature->CastSpell(m_creature, SPELL_FROST_BOMB, true);
            m_creature->ForcedDespawn();
        }

        if (visual_timer <= uiDiff)
        {
            m_creature->CastSpell(m_creature, SPELL_FROST_BOMB_VISUAL, true);
            visual_timer= DAY;
        }
        else
            visual_timer -= uiDiff;

        if (boom_timer <= uiDiff)
        {
            m_creature->CastSpell(m_creature,SPELL_FROST_BOMB_VISUAL2,false);
            finita = true;
        }
        else
            boom_timer -= uiDiff;
    }
};

CreatureAI* GetAI_mob_frost_bomb(Creature* pCreature)
{
    return new mob_frost_bombAI(pCreature);
}

struct MANGOS_DLL_DECL mob_rimefangAI : public BSWScriptedAI
{
    mob_rimefangAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    Creature* pBrother;

    void Reset()
    {
        if (!pInstance)
            return;

        pBrother = NULL;
        if (pInstance->GetData(TYPE_SINDRAGOSA) != DONE)
            pInstance->SetData(TYPE_SINDRAGOSA, NOT_STARTED);

        resetTimers();
        m_creature->SetRespawnDelay(7*DAY);
    }

    void MoveInLineOfSight(Unit* pWho) 
    {
        if (!pInstance || !pWho)
            return;

        if (pWho->GetTypeId() != TYPEID_PLAYER) 
            return;

        if (!m_creature->isInCombat() && pWho->IsWithinDistInMap(m_creature, 60.0f))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            m_creature->GetMotionMaster()->MovementExpired();
            AttackStart(pWho);
            SetCombatMovement(true);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustReachedHome()
    {
        if (pInstance)
            if (pInstance->GetData(TYPE_SINDRAGOSA) != DONE)
                pInstance->SetData(TYPE_SINDRAGOSA, FAIL);
    }

    void Aggro(Unit *who) 
    {
        if(!pInstance)
            return;
        if (pInstance->GetData(TYPE_SINDRAGOSA) != DONE)
            pInstance->SetData(TYPE_SINDRAGOSA, IN_PROGRESS);
        pBrother = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_SPINESTALKER));
        if (pBrother)
        {
            if (!pBrother->isAlive())
                pBrother->Respawn();
            pBrother->SetInCombatWithZone();
        }
    }

    void JustDied(Unit *killer)
    {
        if(!pInstance)
            return;
        if (pInstance->GetData(TYPE_SINDRAGOSA) == DONE) 
            return;

        if (pBrother && !pBrother->isAlive())
        {
            if (Creature* pSindr = m_creature->SummonCreature(NPC_SINDRAGOSA, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 3.17f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, HOUR*IN_MILLISECONDS, true))
                pSindr->SetCreatorGuid(ObjectGuid());
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (pInstance->GetData(TYPE_SINDRAGOSA) == DONE)
        {
            m_creature->SetRespawnDelay(DAY);
            m_creature->ForcedDespawn();
            return;
        }

        doCastAll(diff);
        DoMeleeAttackIfReady();
    }
};


CreatureAI* GetAI_mob_rimefang(Creature* pCreature)
{
    return new mob_rimefangAI(pCreature);
}

struct MANGOS_DLL_DECL mob_spinestalkerAI : public BSWScriptedAI
{
    mob_spinestalkerAI(Creature* pCreature) : BSWScriptedAI(pCreature)
    {
        pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* pInstance;
    Creature* pBrother;

    void Reset()
    {
        if(!pInstance)
            return;

        if (pInstance->GetData(TYPE_SINDRAGOSA) != DONE)
            pInstance->SetData(TYPE_SINDRAGOSA, NOT_STARTED);
        resetTimers();
        m_creature->SetRespawnDelay(7*DAY);
    }

    void MoveInLineOfSight(Unit* pWho) 
    {
        if (!pInstance || !pWho)
            return;

        if (pWho->GetTypeId() != TYPEID_PLAYER) 
            return;

        if (!m_creature->isInCombat() && pWho->IsWithinDistInMap(m_creature, 60.0f))
        {
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            m_creature->GetMotionMaster()->MovementExpired();
            AttackStart(pWho);
            SetCombatMovement(true);
        }
        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void JustReachedHome()
    {
        if (pInstance)
            if (pInstance->GetData(TYPE_SINDRAGOSA) != DONE)
                pInstance->SetData(TYPE_SINDRAGOSA, FAIL);
    }

    void Aggro(Unit *who) 
    {
        if (!pInstance)
            return;
        if (pInstance->GetData(TYPE_SINDRAGOSA) != DONE)
            pInstance->SetData(TYPE_SINDRAGOSA, IN_PROGRESS);
        pBrother = m_creature->GetMap()->GetCreature(pInstance->GetData64(NPC_RIMEFANG));
        if (pBrother)
        {
            if (!pBrother->isAlive())
                pBrother->Respawn();
            pBrother->SetInCombatWithZone();
        }
    }

    void JustDied(Unit *killer)
    {
        if (!pInstance)
            return;
        if (pInstance->GetData(TYPE_SINDRAGOSA) == DONE)
            return;

        if (pBrother && !pBrother->isAlive())
        {
            if (Creature* pSindr = m_creature->SummonCreature(NPC_SINDRAGOSA, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 3.17f, TEMPSUMMON_TIMED_OOC_OR_DEAD_DESPAWN, HOUR*IN_MILLISECONDS, true))
                pSindr->SetCreatorGuid(ObjectGuid());
        }
    }

    void UpdateAI(const uint32 diff)
    {

        if (!pInstance || !m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (pInstance->GetData(TYPE_SINDRAGOSA) == DONE)
        {
            m_creature->SetRespawnDelay(DAY);
            m_creature->ForcedDespawn();
            return;
        }

        doCastAll(diff);
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_spinestalker(Creature* pCreature)
{
    return new mob_spinestalkerAI(pCreature);
}

enum
{
    GOSSIP_TEXT_SINDRA_COMPLETED    = 90014,
    GOSSIP_TEXT_SINDRA_IN_PROGRESS  = 90015,
    GOSSIP_TEXT_HELP_US             = 90016,

    GOSSIP_OPTION_HELP              = 2000006675,
};

bool GossipHello_npc_sindragosa_helper(Player* pPlayer, Creature* pCreature)
{
    ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
    if (!m_pInstance)
        return true;

    if (m_pInstance->GetData(TYPE_SINDRAGOSA) == DONE)
    {
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_SINDRA_COMPLETED, pCreature->GetObjectGuid());
        return true;
    }

    Creature* pRimefang = pCreature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIMEFANG));
    Creature* pSpinestalker = pCreature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_SPINESTALKER));
    Creature* pSindragosa = pCreature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_SINDRAGOSA));

    if (pSindragosa || (!pRimefang || pRimefang->isAlive()) && (!pSpinestalker || pSpinestalker->isAlive()))
    {
        pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_SINDRA_IN_PROGRESS, pCreature->GetObjectGuid());
        return true;
    }

    pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, GOSSIP_OPTION_HELP, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
    pPlayer->SEND_GOSSIP_MENU(GOSSIP_TEXT_HELP_US, pCreature->GetObjectGuid());

    return true;
}

bool GossipSelect_npc_sindragosa_helper(Player* pPlayer, Creature* pCreature, uint32 sender, uint32 action)
{
    pPlayer->CLOSE_GOSSIP_MENU();

    if (sender == GOSSIP_SENDER_MAIN && action == GOSSIP_ACTION_INFO_DEF + 1)
    {
        ScriptedInstance* m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        if (!m_pInstance)
            return true;

        if (m_pInstance->GetData(TYPE_SINDRAGOSA) == DONE)
            return true;

        Creature* pRimefang = pCreature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_RIMEFANG));
        Creature* pSpinestalker = pCreature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_SPINESTALKER));
        Creature* pSindragosa = pCreature->GetMap()->GetCreature(m_pInstance->GetData64(NPC_SINDRAGOSA));

        if (pSindragosa || (!pRimefang || pRimefang->isAlive()) && (!pSpinestalker || pSpinestalker->isAlive()))
            return true;

        if (pRimefang && !pRimefang->isAlive())
        {
            pRimefang->Respawn();
            pRimefang->AI()->EnterEvadeMode();
        }

        if (pSpinestalker && !pSpinestalker->isAlive())
        {
            pSpinestalker->Respawn();
            pSpinestalker->AI()->EnterEvadeMode();
        }
    }

    return true;
}

void AddSC_boss_sindragosa()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_sindragosa";
    newscript->GetAI = &GetAI_boss_sindragosa;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_rimefang";
    newscript->GetAI = &GetAI_mob_rimefang;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_spinestalker";
    newscript->GetAI = &GetAI_mob_spinestalker;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_ice_tomb";
    newscript->GetAI = &GetAI_mob_ice_tomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_frost_bomb";
    newscript->GetAI = &GetAI_mob_frost_bomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_singragosa_helper";
    newscript->pGossipHello = &GossipHello_npc_sindragosa_helper;
    newscript->pGossipSelect = &GossipSelect_npc_sindragosa_helper;
    newscript->RegisterSelf();
}
