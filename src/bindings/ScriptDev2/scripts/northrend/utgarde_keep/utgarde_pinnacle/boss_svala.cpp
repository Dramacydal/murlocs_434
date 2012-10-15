/* Copyright (C) 2006 - 2012 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: Boss_Svala
SD%Complete: 30%
SDComment: The way spells for intro works could use more research.
SDCategory: Utgarde Pinnacle
EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"

enum
{
    SAY_INTRO_1                 = -1575000,
    SAY_INTRO_2_ARTHAS          = -1575001,
    SAY_INTRO_3                 = -1575002,
    SAY_INTRO_4_ARTHAS          = -1575003,
    SAY_INTRO_5                 = -1575004,

    SAY_AGGRO                   = -1575005,
    SAY_SLAY_1                  = -1575006,
    SAY_SLAY_2                  = -1575007,
    SAY_SLAY_3                  = -1575008,
    SAY_SACRIFICE_1             = -1575009,
    SAY_SACRIFICE_2             = -1575010,
    SAY_SACRIFICE_3             = -1575011,
    SAY_SACRIFICE_4             = -1575012,
    SAY_SACRIFICE_5             = -1575013,
    SAY_DEATH                   = -1575014,

    NPC_SVALA_SORROW            = 26668,
    NPC_ARTHAS_IMAGE            = 29280,
    NPC_RITUAL_TARGET           = 27327,
    NPC_PARALYSER               = 27281,

    SPELL_ARTHAS_VISUAL         = 54134,

    // don't know how these should work in relation to each other
    SPELL_TRANSFORMING          = 54205,
    SPELL_TRANSFORMING_FLOATING = 54140,
    SPELL_TRANSFORMING_CHANNEL  = 54142,

    SPELL_RITUAL_OF_SWORD       = 48276,
    SPELL_CALL_FLAMES           = 48258,
    SPELL_SINISTER_STRIKE       = 15667,
    SPELL_SINISTER_STRIKE_H     = 59409,

    SPELL_PARALYZE              = 48278,
};

/*######
## boss_svala
######*/

struct MANGOS_DLL_DECL boss_svalaAI : public ScriptedAI
{
    boss_svalaAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_bIsIntroDone = false;
        Reset();
    }

    instance_pinnacle* m_pInstance;
    bool m_bIsRegularMode;

    ObjectGuid ArthasGuid;

    bool m_bIsIntroDone;
    uint32 m_uiIntroTimer;
    uint32 m_uiIntroCount;

    uint32 m_uiSinisterStrike;
    uint32 m_uiCallFlamesTimer;
    uint32 m_uiMoveToTarget;
    float m_uiRitualPercent;
    uint32 m_uiRitualAdds;

    void Reset()
    {
        m_uiRitualAdds = 0;
        m_uiIntroTimer = 2500;
        m_uiIntroCount = 0;

        m_uiSinisterStrike = 2000;
        m_uiCallFlamesTimer = urand(10000, 15000);
        m_uiMoveToTarget = 0;
        m_uiRitualPercent = 75;
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (m_creature->isAlive() && m_pInstance && m_pInstance->GetData(TYPE_SVALA) > IN_PROGRESS)
        {
            if (m_creature->GetEntry() != NPC_SVALA_SORROW)
                m_creature->UpdateEntry(NPC_SVALA_SORROW);

            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

            m_bIsIntroDone = true;
        }
    }

    void DecreaseAdds()
    {
        if (m_uiRitualAdds == 0)
            return;

        --m_uiRitualAdds;

        if (m_uiRitualAdds == 0)
            m_creature->InterruptNonMeleeSpells(false);
    }

    void JustReachedHome()
    {
        DoMoveToPosition();
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (pWho && pWho->GetTypeId() == TYPEID_PLAYER && ((Player*)pWho)->isGameMaster())
            return;

        if (!m_bIsIntroDone)
        {
            if (m_pInstance && m_pInstance->GetData(TYPE_SVALA) == IN_PROGRESS)
            {
                m_pInstance->SetData(TYPE_SVALA, SPECIAL);

                float fX, fY, fZ;
                m_creature->GetClosePoint(fX, fY, fZ, m_creature->GetObjectBoundingRadius(), 16.0f, 0.0f);

                // we assume m_creature is spawned in proper location
                m_creature->SummonCreature(NPC_ARTHAS_IMAGE, fX, fY, fZ, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 60000);
            }

            return;
        }

        ScriptedAI::MoveInLineOfSight(pWho);
    }

    void Aggro(Unit* pWho)
    {
        //m_creature->SetLevitate(false);
        DoScriptText(SAY_AGGRO, m_creature);
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (pSummoned->GetEntry() == NPC_ARTHAS_IMAGE)
        {
            pSummoned->CastSpell(pSummoned, SPELL_ARTHAS_VISUAL, true);
            ArthasGuid = pSummoned->GetObjectGuid();
            pSummoned->SetFacingToObject(m_creature);
        }
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_TRANSFORMING)
        {
            if (Creature* pArthas = m_creature->GetMap()->GetCreature(ArthasGuid))
                pArthas->InterruptNonMeleeSpells(true);

            m_creature->UpdateEntry(NPC_SVALA_SORROW);
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        switch(urand(0, 2))
        {
            case 0: DoScriptText(SAY_SLAY_1, m_creature); break;
            case 1: DoScriptText(SAY_SLAY_2, m_creature); break;
            case 2: DoScriptText(SAY_SLAY_3, m_creature); break;
        }
    }

    void JustDied(Unit* pKiller)
    {
        DoScriptText(SAY_DEATH, m_creature);

        if (m_pInstance)
            m_pInstance->SetData(TYPE_SVALA, DONE);
    }

    void DoMoveToPosition()
    {
        float fX, fZ, fY;
        m_creature->GetRespawnCoord(fX, fY, fZ);

        m_creature->SetLevitate(true);
        m_creature->GetMotionMaster()->MovePoint(0, fX, fY, fZ + 5.0f);
    }

    void DamageTaken(Unit* pDealer, uint32& uiDamage)
    {
        if (m_uiRitualAdds)
            uiDamage = 0;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_bIsIntroDone)
                return;

            if (Creature* pArthas = m_creature->GetMap()->GetCreature(ArthasGuid))
            {
                if (pArthas->isAlive())
                {
                    if (m_uiIntroTimer < uiDiff)
                    {
                        m_uiIntroTimer = 10000;

                        switch(m_uiIntroCount)
                        {
                            case 0:
                                DoScriptText(SAY_INTRO_1, m_creature);
                                break;
                            case 1:
                                DoScriptText(SAY_INTRO_2_ARTHAS, pArthas);
                                break;
                            case 2:
                                pArthas->CastSpell(m_creature, SPELL_TRANSFORMING_CHANNEL, false);
                                m_creature->CastSpell(m_creature, SPELL_TRANSFORMING_FLOATING, false);
                                DoMoveToPosition();
                                break;
                            case 3:
                                m_creature->CastSpell(m_creature, SPELL_TRANSFORMING, false);
                                DoScriptText(SAY_INTRO_3, m_creature);
                                break;
                            case 4:
                                DoScriptText(SAY_INTRO_4_ARTHAS, pArthas);
                                break;
                            case 5:
                                DoScriptText(SAY_INTRO_5, m_creature);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                                m_creature->RemoveAurasDueToSpell(SPELL_TRANSFORMING_FLOATING);
                                m_bIsIntroDone = true;
                                break;
                        }

                        ++m_uiIntroCount;
                    }
                    else
                        m_uiIntroTimer -= uiDiff;
                }
            }

            return;
        }

        if (m_uiSinisterStrike < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SINISTER_STRIKE : SPELL_SINISTER_STRIKE_H) == CAST_OK)
                m_uiSinisterStrike = urand(5000, 7000);
        }
        else
            m_uiSinisterStrike -= uiDiff;

        if(m_uiCallFlamesTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_CALL_FLAMES) == CAST_OK)
                m_uiCallFlamesTimer = urand(10000, 15000);
        }
        else
            m_uiCallFlamesTimer -= uiDiff;

        if (m_creature->GetHealthPercent() < m_uiRitualPercent)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_RITUAL_OF_SWORD) == CAST_OK)
            {
                m_uiRitualPercent -= 25.0f;
                m_uiRitualAdds = 3;
            }
        }
        
        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_svala(Creature* pCreature)
{
    return new boss_svalaAI(pCreature);
}
struct MANGOS_DLL_DECL npc_paralyzerAI : public ScriptedAI
{
    npc_paralyzerAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pinnacle*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    instance_pinnacle* m_pInstance;
    bool m_bIsRegularMode;

    uint32 spellTimer;
    uint32 svalaCheckTimer;

    void Reset()
    {
        spellTimer = 200;
        svalaCheckTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_pInstance)
            return;

        if (spellTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, SPELL_PARALYZE) == CAST_OK)
                spellTimer = 40000;
        }
        else
            spellTimer -= uiDiff;

        if (svalaCheckTimer < uiDiff)
        {
            svalaCheckTimer = 1000;
            Creature* pSvala = m_pInstance->GetSingleCreatureFromStorage(NPC_SVALA);
            if (!pSvala || pSvala->isDead())
                m_creature->ForcedDespawn();
            else if (pSvala)
            {
                if (!pSvala->IsNonMeleeSpellCasted(false))
                {
                    Map::PlayerList const& pPlayers = m_pInstance->instance->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = pPlayers.begin(); itr != pPlayers.end(); ++itr)
                    {
                        Player* pTarget = itr->getSource();
                        if (pTarget && pTarget->isAlive() && pTarget->HasAura(SPELL_PARALYZE))
                            pSvala->DealDamage(pTarget, pTarget->GetMaxHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                    m_creature->ForcedDespawn();
                }
            }
        }
        else
            svalaCheckTimer -= uiDiff;
    }

    void JustDied(Unit* pKiller)
    {
        if (!m_pInstance)
            return;

        if (Creature* pSvala = m_pInstance->GetSingleCreatureFromStorage(NPC_SVALA))
            ((boss_svalaAI*)pSvala->AI())->DecreaseAdds();
    }
};

CreatureAI* GetAI_npc_paralyzer(Creature* pCreature)
{
    return new npc_paralyzerAI(pCreature);
}

struct MANGOS_DLL_DECL npc_ritual_targetAI : public ScriptedAI
{
    npc_ritual_targetAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    void AttackStart(Unit* pWho){}

    void Reset(){}

    void UpdateAI(const uint32 uiDiff){}
};



CreatureAI* GetAI_ritual_target(Creature* pCreature)
{
    return new npc_ritual_targetAI(pCreature);
}


bool AreaTrigger_at_svala_intro(Player* pPlayer, AreaTriggerEntry const* pAt)
{
    if (instance_pinnacle* pInstance = (instance_pinnacle*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(TYPE_SVALA) == NOT_STARTED)
            pInstance->SetData(TYPE_SVALA, IN_PROGRESS);
    }

    return false;
}

bool ProcessEventId_event_call_flames(uint32 uiEventId, Object* pSource, Object* pTarget, bool bIsStart)
{
    if (instance_pinnacle* pInstance = (instance_pinnacle*)((Creature*)pSource)->GetInstanceData())
    {
        pInstance->DoProcessCallFlamesEvent();
        return true;
    }
    return false;
}

void AddSC_boss_svala()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_svala";
    pNewScript->GetAI = &GetAI_boss_svala;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_paralyzer";
    pNewScript->GetAI = &GetAI_npc_paralyzer;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ritual_target";
    pNewScript->GetAI = &GetAI_ritual_target;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "at_svala_intro";
    pNewScript->pAreaTrigger = &AreaTrigger_at_svala_intro;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "event_call_flames";
    pNewScript->pProcessEventId = &ProcessEventId_event_call_flames;
    pNewScript->RegisterSelf();
}
