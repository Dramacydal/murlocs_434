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
 SDName: Boss_Ymiron
 SD%Complete: 20%
 SDComment:
 SDCategory: Utgarde Pinnacle
 EndScriptData */

#include "precompiled.h"
#include "utgarde_pinnacle.h"
#include "MotionMaster.h"

enum
{
    SAY_AGGRO                       = -1575031,
    SAY_SUMMON_BJORN                = -1575032,
    SAY_SUMMON_HALDOR               = -1575033,
    SAY_SUMMON_RANULF               = -1575034,
    SAY_SUMMON_TORGYN               = -1575035,
    SAY_SLAY_1                      = -1575036,
    SAY_SLAY_2                      = -1575037,
    SAY_SLAY_3                      = -1575038,
    SAY_SLAY_4                      = -1575039,
    SAY_DEATH                       = -1575040,

    SPELL_BANE                      = 48294,
    SPELL_BANE_H                    = 59301,
    SPELL_DARK_SLASH                = 48292,
    SPELL_FETID_ROT                 = 48291,
    SPELL_FETID_ROT_H               = 59300,
    SPELL_SCREAMS_OF_THE_DEAD       = 51750,
    SPELL_SPIRIT_BURST              = 48529,
    SPELL_SPIRIT_BURST_H            = 59305,
    SPELL_SPIRIT_STRIKE             = 48423,
    SPELL_SPIRIT_STRIKE_H           = 59304,

    SPELL_SUMMON_AVENGING_SPIRIT    = 48593,
    SPELL_SUMMON_SPIRIT_FOUNT       = 48386,

    SPELL_CHANNEL_SPIRIT_TO_YMIRON  = 48316,
    SPELL_CHANNEL_YMIRON_TO_SPIRIT  = 48307,

    SPELL_SPIRIT_FOUNT              = 48380,
    SPELL_SPIRIT_FOUNT_H            = 59320

};

enum Ghost{
    NO_GHOST= 0,
    BJORN   = 1,
    HALDOR  = 2,
    RANULF  = 3,
    TORGYN  = 4
};

enum Creatures
{
    CREATURE_BJORN          = 27303,
    CREATURE_BJORN_VISUAL   = 27304,
    CREATURE_HALDOR         = 27307,
    CREATURE_HALDOR_VISUAL  = 27310,
    CREATURE_RANULF         = 27308,
    CREATURE_RANULF_VISUAL  = 27311,
    CREATURE_TORGYN         = 27309,
    CREATURE_TORGYN_VISUAL  = 27312,
    CREATURE_SPIRIT_FOUNT   = 27339,
    CREATURE_AVENGING_SPIRIT = 27386
};

struct ActiveBoatStruct
{
        uint32 npc;
        int32 say;
        float MoveX, MoveY, MoveZ, SpawnX, SpawnY, SpawnZ, SpawnO;
};

static ActiveBoatStruct ActiveBot[4] =
{
{ CREATURE_BJORN_VISUAL, SAY_SUMMON_BJORN, 404.379f, -335.335f, 104.756f, 413.594f, -335.408f, 107.995f, 3.157f },
{ CREATURE_HALDOR_VISUAL, SAY_SUMMON_HALDOR, 380.813f, -335.069f, 104.756f, 369.994f, -334.771f, 107.995f, 6.232f },
{ CREATURE_RANULF_VISUAL, SAY_SUMMON_RANULF, 381.546f, -314.362f, 104.756f, 370.841f, -314.426f, 107.995f, 6.232f },
{ CREATURE_TORGYN_VISUAL, SAY_SUMMON_TORGYN, 404.310f, -314.761f, 104.756f, 413.992f, -314.703f, 107.995f, 3.157f } };

/*######
 ## boss_ymiron
 ######*/

struct MANGOS_DLL_DECL boss_ymironAI: public ScriptedAI
{
        boss_ymironAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            m_pInstance = (instance_pinnacle*) pCreature->GetInstanceData();
            m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
            Reset();
        }

        instance_pinnacle* m_pInstance;
        bool m_bIsRegularMode;

        Ghost ghost;

        float m_fPrecentLifeNextBoat;
        ObjectGuid m_ghostGUID;

        uint32 m_uiBane;
        uint32 m_uiFetidRot;
        uint32 m_uiDarkSlash;

        uint32 m_uiSpecialCast;

        GUIDList addsList;

        void Reset()
        {
            m_uiFetidRot = urand(8000, 13000);
            m_uiBane = urand(15000, 18000);
            m_uiDarkSlash = urand(28000, 33000);

            m_uiSpecialCast = 4000;
            addsList.clear();

            ghost = NO_GHOST;

            m_fPrecentLifeNextBoat = 80.0f;
        }

        void JustSummoned(Creature* pSummoned)
        {
            addsList.push_back(pSummoned->GetObjectGuid());
        }

        Ghost GetNextActiveBoot()
        {
            if (!ghost)
                return (Ghost) urand(1,4);
            else
            {
                uint32 result = ghost;
                ++result;
                if (result > TORGYN)
                {
                    result = BJORN;
                }
                return (Ghost) result;
            }
        }

        void Aggro(Unit* pWho)
        {
            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_YMIRON, IN_PROGRESS);
                m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KINGS_BANE, true);
            }
            DoScriptText(SAY_AGGRO, m_creature);
        }

        void KilledUnit(Unit* pVictim)
        {
            switch (urand(0, 3))
            {
                case 0:
                    DoScriptText(SAY_SLAY_1, m_creature);
                    break;
                case 1:
                    DoScriptText(SAY_SLAY_2, m_creature);
                    break;
                case 2:
                    DoScriptText(SAY_SLAY_3, m_creature);
                    break;
                case 3:
                    DoScriptText(SAY_SLAY_4, m_creature);
                    break;
            }
        }

        void EnterEvadeMode()
        {
            ScriptedAI::EnterEvadeMode();
            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_YMIRON, FAIL);
            }
            for(GUIDList::iterator itr = addsList.begin(); itr != addsList.end(); ++itr)
            {
                if (Creature * pTemp = m_creature->GetMap()->GetCreature(*itr))
                {
                    pTemp->ForcedDespawn();
                }
            }
        }

        void JustDied(Unit* pKiller)
        {
            if (m_pInstance)
            {
                m_pInstance->SetData(TYPE_YMIRON, DONE);
            }
            for(GUIDList::iterator itr = addsList.begin(); itr != addsList.end(); ++itr)
            {
                if (Creature * pTemp = m_creature->GetMap()->GetCreature(*itr))
                {
                    pTemp->ForcedDespawn();
                }
            }
            DoScriptText(SAY_DEATH, m_creature);
        }

        void MovementInform(uint32 uiMovementType, uint32 uiData)
        {
            if (Creature* pOldGhost = m_creature->GetMap()->GetCreature(m_ghostGUID))
            {
                pOldGhost->ForcedDespawn();
            }
            if (Creature* pGhost = m_creature->SummonCreature(ActiveBot[ghost-1].npc, ActiveBot[ghost-1].SpawnX, ActiveBot[ghost-1].SpawnY,ActiveBot[ghost-1].SpawnZ,ActiveBot[ghost-1].SpawnO, TEMPSUMMON_CORPSE_DESPAWN, 0))
            {
                m_ghostGUID = pGhost->GetObjectGuid();
                DoCast(pGhost, SPELL_CHANNEL_YMIRON_TO_SPIRIT);
                pGhost->CastSpell(m_creature, SPELL_CHANNEL_SPIRIT_TO_YMIRON,false);
                pGhost->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
                pGhost->SetLevitate(true);
                m_uiSpecialCast = 4000;
            }
            if (m_creature->getVictim())
            {
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            }

        }

        void UpdateAI(const uint32 uiDiff)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (m_creature->GetHealthPercent() < m_fPrecentLifeNextBoat)
            {
                if (DoCastSpellIfCan (m_creature, SPELL_SCREAMS_OF_THE_DEAD) == CAST_OK)
                {
                    m_fPrecentLifeNextBoat -= 20.0f;
                    m_uiBane = urand(15000, 18000);
                    ghost = GetNextActiveBoot();
                    DoScriptText(ActiveBot[ghost-1].say, m_creature);
                    m_creature->GetMotionMaster()->Clear();
                    m_creature->GetMotionMaster()->MovePoint(0, ActiveBot[ghost-1].MoveX, ActiveBot[ghost-1].MoveY, ActiveBot[ghost-1].MoveZ);
                }
            }

            // Normal spells ------------------------------------------------------------------------
            if (m_uiBane <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_BANE : SPELL_BANE_H) == CAST_OK)
                {
                    if (m_pInstance)
                    {
                        m_pInstance->SetSpecialAchievementCriteria(TYPE_ACHIEV_KINGS_BANE, false);
                    }
                    m_uiBane = urand(20000, 25000);
                }
            }
            else
                m_uiBane -= uiDiff;

            if (m_uiFetidRot <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FETID_ROT : SPELL_FETID_ROT_H) == CAST_OK)
                    m_uiFetidRot = urand(10000, 15000);
            }
            else
                m_uiFetidRot -= uiDiff;

            if (m_uiDarkSlash <= uiDiff)
            {
                if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_DARK_SLASH) == CAST_OK)
                    m_uiDarkSlash = urand(30000, 35000);
            }
            else
                m_uiDarkSlash -= uiDiff;

            // Special spells ------------------------------------------------------------------------
            if (ghost != NO_GHOST)
            {
                if (m_uiSpecialCast <= uiDiff)
                {
                    switch(ghost)
                    {
                        
                        case BJORN:
                        {
                            if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_SUMMON_SPIRIT_FOUNT) == CAST_OK)
                                m_uiSpecialCast = 15000;
                            break;
                        }
                        case HALDOR:
                        {
                            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_SPIRIT_STRIKE : SPELL_SPIRIT_STRIKE_H) == CAST_OK)
                                m_uiSpecialCast = 5000;
                            break;
                        }
                        case RANULF:
                        {
                            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_BURST : SPELL_SPIRIT_BURST_H) == CAST_OK)
                                m_uiSpecialCast = 5000;
                            break;
                        }
                        case TORGYN:
                        {
                            if (DoCastSpellIfCan(m_creature, SPELL_SUMMON_AVENGING_SPIRIT) == CAST_OK)
                                m_uiSpecialCast = 15000;
                            break;
                        }
                        default:
                            break;                     
                    }
                }
                else
                    m_uiSpecialCast -= uiDiff;
            }
            
            DoMeleeAttackIfReady();
        }
};

CreatureAI* GetAI_boss_ymiron(Creature* pCreature)
{
    return new boss_ymironAI(pCreature);
}

struct MANGOS_DLL_DECL npc_spirit_fountAI: public ScriptedAI
{
        npc_spirit_fountAI(Creature* pCreature) : ScriptedAI(pCreature)
        {
            m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
            Reset();
        }

        bool m_bIsRegularMode;

        void Reset()
        {
            m_creature->SetSpeedRate(MOVE_WALK, 0.4f);
            m_creature->SetSpeedRate(MOVE_RUN, 0.4f);
        }

        void EnterEvadeMode()
        {
            ScriptedAI::EnterEvadeMode();
            m_creature->ForcedDespawn();
        }

        void UpdateAI(const uint32  uiDiff)
        {
            DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_SPIRIT_FOUNT : SPELL_SPIRIT_FOUNT_H, CAST_AURA_NOT_PRESENT);
        }
};

CreatureAI* GetAI_npc_spirit_fount(Creature* pCreature)
{
    return new npc_spirit_fountAI(pCreature);
}

void AddSC_boss_ymiron()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_ymiron";
    pNewScript->GetAI = &GetAI_boss_ymiron;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_spirit_fount";
    pNewScript->GetAI = &GetAI_npc_spirit_fount;
    pNewScript->RegisterSelf();
}
