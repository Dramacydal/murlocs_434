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
SDName: boss_forgemaster_garfrost
SD%Complete: 70
SDComment: TODO movement to the forges currently workaround (need core support for Jump-MMGen)
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"

enum saysSD2
{
    SAY_AGGRO                           = -1658014,
    SAY_SLAY_1                          = -1658015,
    SAY_SLAY_2                          = -1658019,
    SAY_BOULDER_HIT                     = -1658022,         // TODO How must this be handled?
    SAY_DEATH                           = -1658017,

    SAY_FORGE_1                         = -1658018,
    SAY_FORGE_2                         = -1658071,
    SAY_TYRANNUS_GARFROST               = -1658020,

    EMOTE_THROW_SARONITE                = -1658022,
    EMOTE_DEEP_FREEZE                   = -1658023,

    SPELL_PERMAFROST                    = 70326,
    SPELL_PERMAFROST_AURA               = 68786,
    SPELL_THROW_SARONITE                = 68788,
    SPELL_THUNDERING_STOMP              = 68771,
    SPELL_FORGE_FROZEN_BLADE            = 68774,
    SPELL_CHILLING_WAVE                 = 68778,
    SPELL_CHILLING_WAVE_H               = 70333,
    SPELL_FORGE_FROSTBORN_MACE          = 68785,
    SPELL_DEEP_FREEZE                   = 70381,
    SPELL_DEEP_FREEZE_H                 = 72930,
    SPELL_FORGE_MACE                    = 68785,
    SPELL_FORGE_MACE_H                  = 70335,
    SPELL_FORGE_BLADE                   = 68774,
    SPELL_FORGE_BLADE_H                 = 70334,

    SAY_FREE_SLAVE_HORDE                = -1658013,
    SAY_FREE_SLAVE_ALLY                 = -1658012,
    SAY_TYRANNUS_OUTRO                  = -1658020,

    PHASE_NO_ENCHANTMENT                = 1,
    PHASE_BLADE_ENCHANTMENT             = 2,
    PHASE_MACE_ENCHANTMENT              = 3,
    PHASE_MOVEMENT                      = 4,

    EQUIP_ID_SWORD                      = 49345,
    EQUIP_ID_MACE                       = 49344,

    ACHIEV_DOESNT_GO_TO_ELEVEN          = 4524,
};

static const float aGarfrostMoveLocs[2][3] =
{
    {719.785f, -230.227f, 527.033f},
    {657.539f, -203.564f, 526.691f},
};

//Positional defines 
struct LocationsXY
{
    float x, y, z, o;
    uint32 id;
};

static LocationsXY SummonLoc[]=
{
    {719.812f, -167.183f, 526.721f,},
    {698.703f, -165.497f, 527.464f,},
    {671.455f, -167.968f, 526.741f,},
};

static LocationsXY MoveLoc[]=
{
    {677.445f, -186.521f, 526.702f},
    {708.190f, -194.619f, 526.805f},
    {687.257f, -193.644f, 526.717f},
};

#define HOME_X                      712.927f
#define HOME_Y                      -231.605f

struct MANGOS_DLL_DECL npc_martin_gorkunAI: public ScriptedAI
{
    npc_martin_gorkunAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_uiTyrannusGuid.Clear();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSpeech_Timer;
    uint8 m_uiOutro_Phase;
    bool m_bIsOutro;
    uint32 creatureEntry;

    ObjectGuid m_uiTyrannusGuid;

    void Reset()
    {
        m_uiOutro_Phase     = 0;
        m_uiSpeech_Timer    = 1000;
        m_bIsOutro          = true;
        m_uiTyrannusGuid.Clear();
        creatureEntry = m_creature->GetEntry();
    }

    void SummonHordeSlaves()
    {
        for (uint8 i = 0; i < 5; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_HORDE_1, SummonLoc[0].x + urand(0, 20), SummonLoc[0].y + urand(0, 20), SummonLoc[0].z, SummonLoc[0].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[0].x + urand(0, 20), MoveLoc[0].y + urand(0, 20), MoveLoc[0].z);
        }

        for (uint8 i = 5; i < 10; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_HORDE_2, SummonLoc[1].x + urand(0, 10), SummonLoc[1].y - urand(0, 10), SummonLoc[1].z, SummonLoc[1].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[2].x + urand(0, 20), MoveLoc[2].y - urand(0, 20), MoveLoc[2].z);
        }

        for (uint8 i = 10; i < 15; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_HORDE_3, SummonLoc[2].x - urand(0, 20), SummonLoc[2].y - urand(0, 20), SummonLoc[2].z, SummonLoc[2].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[1].x - urand(0, 20), MoveLoc[1].y - urand(0, 20), MoveLoc[1].z);
        }
    }

    void SummonAlySlaves()
    {
        for (uint8 i = 0; i < 5; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_ALY_1, SummonLoc[0].x + urand(0, 20), SummonLoc[0].y + urand(0, 20), SummonLoc[0].z, SummonLoc[0].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[0].x + urand(0, 20), MoveLoc[0].y + urand(0, 20), MoveLoc[0].z);
        }

        for (uint8 i = 5; i < 10; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_ALY_2, SummonLoc[1].x + urand(0, 10), SummonLoc[1].y - urand(0, 10), SummonLoc[1].z, SummonLoc[1].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[2].x + urand(0, 20), MoveLoc[2].y - urand(0, 20), MoveLoc[2].z);
        }

        for (uint8 i = 10; i < 15; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_ALY_3, SummonLoc[2].x - urand(0, 20), SummonLoc[2].y - urand(0, 20), SummonLoc[2].z, SummonLoc[2].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[1].x - urand(0, 20), MoveLoc[1].y - urand(0, 20), MoveLoc[1].z);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_bIsOutro)
        {
            if(m_uiSpeech_Timer < uiDiff)
            {
                switch(m_uiOutro_Phase)
                {
                case 0:
                    switch (creatureEntry)
                    {
                    case NPC_MARTIN_VICTUS_SLAVE:
                        SummonAlySlaves();
                        break;
                    case NPC_GORKUN_IRONSKULL_SLAVE:
                        SummonHordeSlaves();
                        break;
                    }
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 6000;
                    break;
                case 1:
                    switch (creatureEntry)
                    {
                    case NPC_MARTIN_VICTUS_SLAVE:
                        DoScriptText(SAY_FREE_SLAVE_ALLY,m_creature);
                        break;
                    case NPC_GORKUN_IRONSKULL_SLAVE:
                        DoScriptText(SAY_FREE_SLAVE_HORDE,m_creature);
                        break;
                    }
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 10000;
                    break;
                case 2:
                    if(Creature* pTyrannus = m_creature->SummonCreature(NPC_TYRANNUS_INTRO, 695.46f, -156.31f, 546.061f, 4.7f, TEMPSUMMON_TIMED_DESPAWN, 10000))
                    {
                        //pTyrannus->SetGuidValue(UNIT_FIELD_TARGET, m_creature->GetObjectGuid());
                        pTyrannus->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        m_uiTyrannusGuid = pTyrannus->GetObjectGuid();
                    }
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 3000;
                    break;
                case 3:
                    if(Creature* pTyrannus = m_pInstance->instance->GetCreature(m_uiTyrannusGuid))
                        DoScriptText(SAY_TYRANNUS_OUTRO, pTyrannus);
                    m_bIsOutro = false;
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 10000;
                    break;

                default:
                    m_uiSpeech_Timer = 100000;
                }
            }else m_uiSpeech_Timer -= uiDiff;
        }
    }
};

struct MANGOS_DLL_DECL boss_forgemaster_garfrostAI : public ScriptedAI
{
    boss_forgemaster_garfrostAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiThrowSaroniteTimer;
    uint32 m_uiPhase;
    uint32 TeamInInstance;
    uint32 m_uiChillingWaveTimer;
    uint32 m_uiDeepFreezeTimer;

    uint32 GetFaction()
    {
        uint32 faction = 0;
        Map *map = m_creature->GetMap();
        if (map->IsDungeon())
        {
            Map::PlayerList const &PlayerList = map->GetPlayers();

            if (!PlayerList.isEmpty())
            {
                if (Player* pPlayer = PlayerList.begin()->getSource())
                    faction = pPlayer->GetTeam();
            }
        }
        return faction;
    }

    void Reset()
    {
        m_uiThrowSaroniteTimer = 13000;
        m_uiChillingWaveTimer = 10000;
        m_uiDeepFreezeTimer = 10000;
        SetCombatMovement(true);
        m_uiPhase = PHASE_NO_ENCHANTMENT;
        TeamInInstance = GetFaction();
    }

    void Aggro(Unit* pWho)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARFROST, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature, pWho);
        DoCastSpellIfCan(m_creature, SPELL_PERMAFROST);
    }

    void JustDied(Unit* pKiller)
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_GARFROST, DONE);

        DoScriptText(SAY_DEATH, m_creature, pKiller);

        if(TeamInInstance == ALLIANCE)
        {
            Creature *pMartin = m_creature->SummonCreature(NPC_MARTIN_VICTUS_SLAVE, 695.46f, -156.31f, 528.061f, 4.77f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            if (pMartin)
                pMartin->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX() + 15, m_creature->GetPositionY() - 5, m_creature->GetPositionZ());
        }

        if(TeamInInstance == HORDE)
        {
            Creature *pGorkun = m_creature->SummonCreature(NPC_GORKUN_IRONSKULL_SLAVE, 695.46f, -156.31f, 528.061f, 4.77f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            if (pGorkun)
                pGorkun->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX() + 15, m_creature->GetPositionY() - 5, m_creature->GetPositionZ());
        }
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0,1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void MovementInform(uint32 uiMotionType, uint32 uiPointId)
    {
        // TODO Change to jump movement type when proper implemented
        if (uiMotionType != POINT_MOTION_TYPE)
            return;

        if (uiPointId != PHASE_BLADE_ENCHANTMENT && uiPointId != PHASE_MACE_ENCHANTMENT)
            return;

        // Cast and say expected spell
        DoCastSpellIfCan(m_creature, uiPointId == PHASE_BLADE_ENCHANTMENT ? SPELL_FORGE_FROZEN_BLADE : SPELL_FORGE_FROSTBORN_MACE);
        m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        DoScriptText(uiPointId == PHASE_BLADE_ENCHANTMENT ? SAY_FORGE_1 : SAY_FORGE_2, m_creature);

        m_uiThrowSaroniteTimer += 5000;                     // Delay next Saronit
        m_uiPhase = uiPointId;
        SetCombatMovement(true);

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

        // Do nothing more while moving
        if (m_uiPhase == PHASE_MOVEMENT)
            return;

        // Casted in every phase
        if (m_uiThrowSaroniteTimer < uiDiff)
        {
            // TODO - only target players?
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_THROW_SARONITE) == CAST_OK)
                {
                    DoScriptText(EMOTE_THROW_SARONITE, m_creature, pTarget);
                    m_uiThrowSaroniteTimer = 16000;
                }
            }
        }
        else
            m_uiThrowSaroniteTimer -= uiDiff;

        switch (m_uiPhase)
        {
            case PHASE_NO_ENCHANTMENT:
            {
                if (m_creature->GetHealthPercent() < 66.0f)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THUNDERING_STOMP, CAST_INTERRUPT_PREVIOUS);
                    SetCombatMovement(false);

                    // TODO This should actually be jump movement
                    m_creature->GetMotionMaster()->MovePoint(PHASE_BLADE_ENCHANTMENT, aGarfrostMoveLocs[0][0], aGarfrostMoveLocs[0][1], aGarfrostMoveLocs[0][2]);
                    m_uiPhase = PHASE_MOVEMENT;

                    // Stop further action
                    return;
                }
                break;
            }
            case PHASE_BLADE_ENCHANTMENT:
            {
                if (m_creature->GetHealthPercent() < 33.0f)
                {
                    DoCastSpellIfCan(m_creature, SPELL_THUNDERING_STOMP, CAST_INTERRUPT_PREVIOUS);
                    SetCombatMovement(false);

                    // TODO This should actually be jump movement
                    m_creature->GetMotionMaster()->MovePoint(PHASE_MACE_ENCHANTMENT, aGarfrostMoveLocs[1][0], aGarfrostMoveLocs[1][1], aGarfrostMoveLocs[1][2]);
                    m_uiPhase = PHASE_MOVEMENT;

                    // Stop further action
                    return;
                }

                if (m_uiChillingWaveTimer < uiDiff)
                {
                    if (DoCastSpellIfCan(m_creature->getVictim(), SPELL_CHILLING_WAVE) == CAST_OK)
                        m_uiChillingWaveTimer = 14000;
                }
                else
                    m_uiChillingWaveTimer -= uiDiff;

                break;
            }
            case PHASE_MACE_ENCHANTMENT:
            {
                if (m_uiDeepFreezeTimer < uiDiff)
                {
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                    {
                        if (DoCastSpellIfCan(pTarget, SPELL_DEEP_FREEZE) == CAST_OK)
                        {
                            DoScriptText(EMOTE_DEEP_FREEZE, m_creature, pTarget);
                            m_uiDeepFreezeTimer = 20000;
                        }
                    }
                }
                else
                    m_uiDeepFreezeTimer -= uiDiff;

                break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_forgemaster_garfrost(Creature* pCreature)
{
    return new boss_forgemaster_garfrostAI(pCreature);
}

CreatureAI* GetAI_npc_martin_gorkun(Creature* pCreature)
{
    return new npc_martin_gorkunAI (pCreature);
}

void AddSC_boss_garfrost()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_forgemaster_garfrost";
    pNewScript->GetAI = &GetAI_boss_forgemaster_garfrost;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="npc_martin_gorkun";
    pNewScript->GetAI = &GetAI_npc_martin_gorkun;
    pNewScript->RegisterSelf();
}
