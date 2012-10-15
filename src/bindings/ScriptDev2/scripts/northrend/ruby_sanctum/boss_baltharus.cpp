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
SDName: boss_baltharus
SD%Complete: 90%
SDComment: by notagain && /dev/rsa && carlos93
SDCategory: Ruby Sanctum
EndScriptData */

#include "precompiled.h"
#include "ruby_sanctum.h"

static Locations SpawnLoc[]=
{
    {3152.329834f, 359.41757f, 85.301605f},    // Baltharus target point
    {3153.06f, 389.486f, 86.2596f},            // Baltharus initial point
};

enum Says
{
    SAY_BALTHARUS_AGGRO             = -1666300,
    SAY_BALTHARUS_SLAY_1            = -1666301,
    SAY_BALTHARUS_SLAY_2            = -1666302,
    SAY_BALTHARUS_DEATH             = -1666303,
    SAY_BALTHARUS_SPECIAL_1         = -1666304,
    SAY_BALTHARUS_YELL              = -1666305,
};

enum BossSpells
{
    SPELL_BLADE_TEMPEST             = 75125, // every 22 secs
    SPELL_ENERVATING_BRAND          = 74502, // friendlys in 12yards = 74505
    SPELL_REPELLING_WAVE            = 74509, // once if call clone
    SPELL_SABER_LASH                = 40504, // every 10-15 secs
    SPELL_SUMMON_CLONE              = 74511, // summons npc 39899 (Clone)
    SPELL_CHANNEL_SPELL             = 76221, // Channeling dummy spell
};

/*######
## boss_baltharus
######*/

struct MANGOS_DLL_DECL boss_baltharusAI : public ScriptedAI
{
    boss_baltharusAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiMapDifficulty = pCreature->GetMap()->GetDifficulty();
        m_bIsHeroic = m_uiMapDifficulty > RAID_DIFFICULTY_25MAN_NORMAL;
        m_bIs25Man = (m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_NORMAL || m_uiMapDifficulty == RAID_DIFFICULTY_25MAN_HEROIC);
        Reset();
    }

    ScriptedInstance *m_pInstance;
    Difficulty m_uiMapDifficulty;
    Creature* pDummyTarget;
    Creature* pClone;
    bool m_bInCombat;
    bool m_bIntro;
    bool m_bIs25Man;
    bool m_bIsHeroic;
    uint32 m_uiStage;
    uint32 m_uiBladeTempestTimer;
    uint32 m_uiEnervatingBrandTimer;
    uint32 m_uiSaberLashTimer;

    void Reset()
    {
        if (!m_pInstance)
            return;

        if (m_creature->isAlive())
            m_pInstance->SetData(TYPE_BALTHARUS, NOT_STARTED);

        m_creature->SetRespawnDelay(7*DAY);
        pClone = NULL;
        m_uiStage = 0;
        m_uiBladeTempestTimer = 22000;
        m_uiEnervatingBrandTimer = 10000;
        m_uiSaberLashTimer = 5000;
        m_bInCombat = false;
        m_bIntro = false;
        if (pDummyTarget = m_pInstance->GetSingleCreatureFromStorage(NPC_BALTHARUS_TARGET))
        {
            if (!pDummyTarget->isAlive())
                pDummyTarget->Respawn();

            pDummyTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pDummyTarget->GetMotionMaster()->MoveIdle();
        }
        else if (pDummyTarget = m_creature->SummonCreature(NPC_BALTHARUS_TARGET, SpawnLoc[0].x, SpawnLoc[0].y, SpawnLoc[0].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 1000))
        {
            pDummyTarget->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
            pDummyTarget->GetMotionMaster()->MoveIdle();
        }

        if (Creature* pTarget = m_pInstance->GetSingleCreatureFromStorage(NPC_XERESTRASZA))
            m_creature->SetGuidValue(UNIT_FIELD_TARGET, pTarget->GetObjectGuid());
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_BALTHARUS, FAIL);
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        ScriptedAI::MoveInLineOfSight(pWho);

        if (!m_pInstance || m_bIntro || pWho->GetTypeId() != TYPEID_PLAYER || !pWho->IsWithinDistInMap(m_creature, 60.0f))
            return;

        m_pInstance->SetData(TYPE_EVENT, 10);
        DoScriptText(SAY_BALTHARUS_YELL,m_creature);
        m_bIntro = true;
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        DoScriptText(SAY_BALTHARUS_DEATH,m_creature);
        m_pInstance->SetData(TYPE_BALTHARUS, DONE);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_BALTHARUS_SLAY_1 - urand(0,1),m_creature,pVictim);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (!m_pInstance || !pSummoned)
            return;

        if (pSummoned->GetEntry() != NPC_BALTHARUS_TARGET )
        {
            if (!pClone)
                pClone = pSummoned;
            else if (!pClone->isAlive())
                pClone = pSummoned;

            pClone->SetInCombatWithZone();
        }
    }

    void SummonedCreatureJustDied(Creature* pSummoned)
    {
        if (!m_pInstance || !pSummoned)
            return;

        if (pSummoned == pClone)
            pClone = NULL;
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        if (pWho->GetTypeId() != TYPEID_PLAYER)
            return;

        if (pDummyTarget)
            pDummyTarget->ForcedDespawn();

        m_bInCombat = true;
        m_creature->InterruptNonMeleeSpells(true);
        SetCombatMovement(true);
        m_pInstance->SetData(TYPE_BALTHARUS, IN_PROGRESS);
        DoScriptText(SAY_BALTHARUS_AGGRO,m_creature);
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (!m_pInstance)
            return;

        if (!m_creature || !m_creature->isAlive())
            return;

        if (pClone && pClone->isAlive())
            uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (!m_bInCombat && !m_creature->IsNonMeleeSpellCasted(false))
            m_creature->CastSpell(pDummyTarget, SPELL_CHANNEL_SPELL, false);

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        switch (m_uiStage)
        {
        case 0:
            if (m_creature->GetHealthPercent() <= 66.0f)
                m_uiStage = 1;

            break;
        case 1:
            m_creature->InterruptNonMeleeSpells(true);
            if (m_bIs25Man)
            {
                m_creature->CastSpell(m_creature, SPELL_SUMMON_CLONE, true);
                DoScriptText(SAY_BALTHARUS_SPECIAL_1,m_creature);
            }
            m_uiStage = 2;
            break;
        case 2:
            if (m_creature->IsNonMeleeSpellCasted(false))
                return;

            if (m_bIs25Man)
                m_creature->CastSpell(m_creature, SPELL_REPELLING_WAVE, false);

            m_uiStage = 3;
            break;
        case 3:
            if (m_creature->GetHealthPercent() <= 50.0f)
                m_uiStage = 4;

            break;
        case 4:
            m_creature->InterruptNonMeleeSpells(true);
            if (!m_bIs25Man)
            {
               m_creature->CastSpell(m_creature, SPELL_SUMMON_CLONE, true);
               DoScriptText(SAY_BALTHARUS_SPECIAL_1,m_creature);
            }
            m_uiStage = 5;
            break;
        case 5:
            if (m_creature->IsNonMeleeSpellCasted(true))
                return;

            m_creature->CastSpell(m_creature, SPELL_REPELLING_WAVE, false);
            m_uiStage = 6;
            break;
        case 6:
            if (m_creature->GetHealthPercent() <= 33.0f)
                m_uiStage = 7;

            break;
        case 7:
            m_creature->InterruptNonMeleeSpells(true);
            if (m_bIs25Man)
            {
               m_creature->CastSpell(m_creature, SPELL_SUMMON_CLONE, true);
               DoScriptText(SAY_BALTHARUS_SPECIAL_1,m_creature);
            }
            m_uiStage = 8;
            break;
        case 8:
            if (m_creature->IsNonMeleeSpellCasted(true))
                return;

            if (m_bIs25Man)
                m_creature->CastSpell(m_creature, SPELL_REPELLING_WAVE, false);

            m_uiStage = 9;
            break;
        case 9:
            break;
        default:
            break;
        }

        if (m_uiBladeTempestTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLADE_TEMPEST) == CAST_OK)
                m_uiBladeTempestTimer = urand(20000, 25000);
        }
        else
            m_uiBladeTempestTimer -= uiDiff;

        if (m_uiEnervatingBrandTimer < uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ENERVATING_BRAND) == CAST_OK)
                    m_uiEnervatingBrandTimer = urand(10000, 13000);
            }
        }
        else
            m_uiEnervatingBrandTimer -= uiDiff;

        if (m_uiSaberLashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SABER_LASH) == CAST_OK)
                m_uiSaberLashTimer = urand(8000, 12000);
        }
        else
            m_uiSaberLashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_baltharus(Creature* pCreature)
{
    return new boss_baltharusAI(pCreature);
}

/*######
## mob_baltharus_clone
######*/

struct MANGOS_DLL_DECL mob_baltharus_cloneAI : public ScriptedAI
{
    mob_baltharus_cloneAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance *m_pInstance;
    uint32 m_uiBladeTempestTimer;
    uint32 m_uiEnervatingBrandTimer;
    uint32 m_uiSaberLashTimer;

    void Reset()
    {
        if (!m_pInstance)
            return;

        m_creature->SetRespawnDelay(7*DAY);
        m_uiBladeTempestTimer = 22000;
        m_uiEnervatingBrandTimer = 10000;
        m_uiSaberLashTimer = 5000;
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(SAY_BALTHARUS_SLAY_1 - urand(0,1),m_creature,pVictim);
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        m_creature->SetVisibility(VISIBILITY_OFF);
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        m_creature->SetInCombatWithZone();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (m_pInstance->GetData(TYPE_BALTHARUS) != IN_PROGRESS)
            m_creature->ForcedDespawn();

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBladeTempestTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_BLADE_TEMPEST) == CAST_OK)
                m_uiBladeTempestTimer = urand(20000, 25000);
        }
        else
            m_uiBladeTempestTimer -= uiDiff;

        if (m_uiEnervatingBrandTimer < uiDiff)
        {
            if (Unit *pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_ENERVATING_BRAND) == CAST_OK)
                    m_uiEnervatingBrandTimer = urand(10000, 13000);
            }
        }
        else
            m_uiEnervatingBrandTimer -= uiDiff;

        if (m_uiSaberLashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_SABER_LASH) == CAST_OK)
                m_uiSaberLashTimer = urand(8000, 12000);
        }
        else
            m_uiSaberLashTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_baltharus_clone(Creature* pCreature)
{
    return new mob_baltharus_cloneAI(pCreature);
}

void AddSC_boss_baltharus()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_baltharus";
    newscript->GetAI = &GetAI_boss_baltharus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_baltharus_clone";
    newscript->GetAI = &GetAI_mob_baltharus_clone;
    newscript->RegisterSelf();
}
