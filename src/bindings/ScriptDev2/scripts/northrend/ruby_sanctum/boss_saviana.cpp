/* Copyright (C) 2010 /dev/rsa for ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: boss_ragefire
SD%Complete: 99%
SDComment: by notagain && /dev/rsa && carlos93
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

enum BossSpells
{
    SPELL_ENRAGE                    = 78722, //soft enrage + fire nova
    SPELL_FLAME_BREATH              = 74404,
    SPELL_BEACON                    = 74453, //mark for conflag, in enter to fly phase, 2 in 10, 5 in 25
    SPELL_CONFLAGATION              = 74452, // after fly up
    SPELL_CONFLAGATION_1            = 74455, // Triggered?
    SPELL_CONFLAGATION_2            = 74456, // Aura
};

enum Yells
{
    SAY_AGGRO                       = -1666400,
    SAY_SLAY_1                      = -1666401,
    SAY_SLAY_2                      = -1666402,
    SAY_DEATH                       = -1666403,
    SAY_SPECIAL                     = -1666404,
    SAY_ENRAGE                      = -1666405,
};

enum
{
    PHASE_GROUND                    = 0,
    PHASE_FLYING                    = 1,
    PHASE_AIR                       = 2,

    POINT_LAND                      = 0,
    POINT_AIR                       = 1,
};

static Locations SpawnLoc[]=
{
    {3151.3898f, 636.8519f, 78.7396f},  // 0 Saviana start point
    {3149.6350f, 668.9644f, 90.5070f},  // 1 Saviana fly phase, o=4,69
};

struct MANGOS_DLL_DECL boss_ragefireAI : public ScriptedAI
{
    boss_ragefireAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance *m_pInstance;
    Difficulty m_uiMapDifficulty;

    bool m_bIs25Man;
    bool m_bIsHeroic;
    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiFlameBreathTimer;
    uint32 m_uiEnrageTimer;
    uint32 m_uiFlyingTimer;

    void Reset()
    {
        if (!m_pInstance)
            return;

        m_creature->SetRespawnDelay(7*DAY);

        if (m_creature->isAlive())
            m_pInstance->SetData(TYPE_RAGEFIRE, NOT_STARTED);

        m_uiPhase                   = PHASE_GROUND;
        m_uiPhaseTimer              = 32000;
        m_uiEnrageTimer             = urand(20000,30000);
        m_uiFlameBreathTimer        = urand(6000, 10000);
    }

    void MovementInform(uint32 uiMovementType, uint32 uiData)
    {
        if (uiMovementType != POINT_MOTION_TYPE)
            return;

        if (uiData == POINT_AIR)
        {
            m_uiPhase = PHASE_AIR;

            int max = m_bIs25Man ? 5 : 2;

            for (int i = 0; i < max; ++i)
            {
                if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0, SPELL_BEACON, SELECT_FLAG_PLAYER))
                    m_creature->CastSpell(pTarget, SPELL_BEACON, true);
            }
        }
        else if (uiData == POINT_LAND)
        {
            m_uiPhase = PHASE_GROUND;
            SetCombatMovement(true);
            m_creature->SetLevitate(false);
            m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_UNK_2);

            if (m_creature->getVictim())
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        if (pVictim->GetTypeId() == TYPEID_PLAYER)
            DoScriptText(SAY_SLAY_1 - urand(0, 1), m_creature);
    }

    void JustReachedHome()
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_RAGEFIRE, FAIL);

        SetCombatMovement(true);
        m_creature->SetLevitate(false);
        m_creature->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_UNK_2);
    }

    void Aggro(Unit *who) 
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_RAGEFIRE, IN_PROGRESS);
        m_creature->SetInCombatWithZone();
        DoScriptText(SAY_AGGRO, m_creature, who);
    }

    void JustDied(Unit *killer)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_RAGEFIRE, DONE);
        DoScriptText(SAY_DEATH, m_creature, killer);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_uiPhase)
        {
        case PHASE_GROUND:
            if (m_uiFlameBreathTimer < uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, SPELL_FLAME_BREATH) == CAST_OK)
                    m_uiFlameBreathTimer = urand(10000,15000);
                if (m_uiPhaseTimer <= 5000)
                    m_uiPhaseTimer = 5000;
            }
            else
                m_uiFlameBreathTimer -= uiDiff;

            if (m_uiEnrageTimer < uiDiff)
            {
                if(DoCastSpellIfCan(m_creature, SPELL_ENRAGE) == CAST_OK)
                {
                    m_uiEnrageTimer = urand(20000,30000);
                    DoScriptText(SAY_ENRAGE, m_creature);
                    if (m_uiPhaseTimer <= 10000)
                        m_uiPhaseTimer = 10000;
                }
            }
            else
                m_uiEnrageTimer -= uiDiff;

            if (m_uiPhaseTimer < uiDiff)
            {
                m_uiPhaseTimer = 15000;
                m_uiPhase = PHASE_FLYING;
                SetCombatMovement(false);
                m_creature->SetLevitate(true);
                m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(POINT_AIR, SpawnLoc[1].x, SpawnLoc[1].y, SpawnLoc[1].z);
            }
            else
                m_uiPhaseTimer -= uiDiff;

            DoMeleeAttackIfReady();
            break;
        case PHASE_FLYING:
            if (m_creature->IsNonMeleeSpellCasted(true))
                return;

            if (m_uiFlyingTimer <= uiDiff)
            {
                m_uiFlyingTimer = 30000;
                m_creature->AI()->EnterEvadeMode();
                return;
            }
            else
                m_uiFlyingTimer -= uiDiff;

            return;
        case PHASE_AIR:
            if (m_uiPhaseTimer < uiDiff)
            {
                m_uiPhase = PHASE_FLYING;
                m_uiPhaseTimer = 30000;
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MovePoint(POINT_LAND, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z);
            }
            else
                m_uiPhaseTimer -= uiDiff;

            return;
        default:
            break;
        }
    }
};

CreatureAI* GetAI_boss_ragefire(Creature* pCreature)
{
    return new boss_ragefireAI(pCreature);
}

void AddSC_boss_ragefire()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_ragefire";
    newscript->GetAI = &GetAI_boss_ragefire;
    newscript->RegisterSelf();
}
