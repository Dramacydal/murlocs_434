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
SDName: boss_scourgelord_tyrannus
SD%Complete: 50%
SDComment: missing intro and outro; encounter need vehicle support
SDCategory: Pit of Saron
EndScriptData */

#include "precompiled.h"
#include "pit_of_saron.h"
#include "Vehicle.h"

enum
{
    SAY_GENERAL_TRASH                   = -1658051,
    SAY_PREFIGHT_1                      = -1658053,
    SAY_PREFIGHT_2                      = -1658054,
    SAY_AGGRO                           = -1658055,
    SAY_SLAY_1                          = -1658056,
    SAY_SLAY_2                          = -1658057,
    SAY_DEATH                           = -1658058,
    SAY_MARK                            = -1658059,
    SAY_SMASH                           = -1658061,

    EMOTE_RIMEFANG_ICEBOLT              = -1658060,
    EMOTE_SMASH                         = -1658062,

    SPELL_FORCEFUL_SMASH                = 69155,
    SPELL_FORCEFUL_SMASH_H              = 69627,
    SPELL_OVERLORDS_BRAND               = 69172,
    SPELL_DARK_MIGHT                    = 69167,
    SPELL_DARK_MIGHT_H                  = 69629,
    SPELL_HOARFROST                     = 69246,
    SPELL_MARK_OF_RIMEFANG              = 69275,
    SPELL_ICY_BLAST                     = 69233,
    SPELL_ICY_BLAST_H                   = 69646,
    SPELL_ICY_BLAST_SLOW                = 69238,
    SPELL_ICY_BLAST_SLOW_H              = 69628,

    SAY_OUTRO1_SLAVE_HORDE              = -1658061,
    SAY_OUTRO1_SLAVE_ALLY               = -1658061,
    SAY_OUTRO2_SLAVE                    = -1658062,
    SAY_OUTRO3_HORDE                    = -1658064,
    SAY_OUTRO3_ALY                      = -1658063,
    SAY_OUTRO4_HORDE                    = -1658067,
    SAY_OUTRO4_ALY                      = -1658065,
    SAY_OUTRO5_ALY                      = -1658066,

    SPELL_FROST_BOMB                    = 70521,

    NPC_ICY_BLAST                       = 36731,
    SPELL_ICY_BLAST_AURA                = 69238,
    SPELL_ICY_BLAST_AURA_H              = 69628,

    EQUIP_ID                            = 51796,

    NPC_DAILY_DUNGEON                   = 22852,
};

const float RimefangSummon[4] = {1013.827f, 169.71f, 628.157f, 5.31f};

struct LocationsXY
{
    float x, y, z, o;
    uint32 id;
};

enum gauntlet
{
    SAY_GAUNTLET1            = -1610081,
    SAY_GAUNTLET2            = -1610082,
    SAY_TUNNEL               = -1610083,
    // icicle event
    NPC_COLLAPSING_ICICLE    = 36847,
    SPELL_ICICLE             = 69428,
    SPELL_ICICLE_DMG         = 69426,
    SPELL_ICICLE_SUMMON      = 69424,
    // mobs
    // first 2 waves
    NPC_WRATHBRINGER         = 36840,
    NPC_FLAMEBEARER          = 36893,
    NPC_DEATHBRINGER         = 36892,
    // another 2 waves
    NPC_FALLEN_WARRIOR       = 36841,
    NPC_WRATHBONE_COLDWRAITH = 36842,
    NPC_WRATHBONE_SORCERER   = 37728,    // this is for the end event, not used
    NPC_GLACIAL_REVENANT     = 36874,
};

static LocationsXY SummonLoc[]=
{
    {1060.955f, 107.274f, 628.424f},
    {1052.122f, 103.916f, 628.454f},
    {1068.363f, 110.432f, 629.009f},
};

static LocationsXY MoveLoc[]=
{
    {1019.006f, 129.684f, 628.156f}, 
    {1003.889f, 159.652f, 628.159f},
    {1015.389f, 183.650f, 628.156f},
    {1065.827f, 210.836f, 628.156f},
    {1072.659f, 204.432f, 628.156f},
};

struct MANGOS_DLL_DECL boss_rimefangAI : public ScriptedAI
{
    boss_rimefangAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetCombatMovement(false);
        m_uiMainTargetGUID.Clear();
        Reset();
    }

    instance_pit_of_saron* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiHoarfrostTimer;
    uint32 m_uiIcyBlastTimer;
    uint32 m_uiIcyBlastSlowTimer;
    ObjectGuid m_uiMainTargetGUID;

    void Reset()
    {
        m_uiHoarfrostTimer      = 25000;
        m_uiIcyBlastTimer       = 35000;
        m_uiIcyBlastSlowTimer   = 30000;
        m_uiMainTargetGUID.Clear();
        m_creature->SetByteValue(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_ALWAYS_STAND | UNIT_BYTE1_FLAG_UNK_2);
    }

    void SetMainTarget(ObjectGuid m_uiTargetGUID)
    {
        m_uiMainTargetGUID = m_uiTargetGUID;
    }

    void JustSummoned(Creature* pSummoned)
    {
        if(pSummoned->GetEntry() == NPC_ICY_BLAST)
            pSummoned->CastSpell(pSummoned, SPELL_ICY_BLAST_AURA, false);
    }

    void Aggro(Unit* pWho)
    {
        m_creature->SetInCombatWithZone();

        if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS))
        {
            pTyrannus->AI()->AttackStart(pWho);
            pTyrannus->SetCorpseDelay(3600);
            pTyrannus->ExitVehicle();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_creature->GetVehicleKit())
            {
                if (!m_creature->GetVehicleKit()->GetPassenger(0) && m_pInstance->GetData(TYPE_TYRANNUS) != DONE)
                {
                    if (Creature* pTyrannus = m_pInstance->GetSingleCreatureFromStorage(NPC_TYRANNUS))
                        pTyrannus->ForcedDespawn();

                    m_creature->GetVehicleKit()->Reset();
                }
            }
            return;
        }

        if (m_uiHoarfrostTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_uiMainTargetGUID))
                DoCastSpellIfCan(pTarget, SPELL_HOARFROST);
            else if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, SPELL_HOARFROST);
            DoScriptText(EMOTE_RIMEFANG_ICEBOLT, m_creature);
            m_uiHoarfrostTimer = 20000;
        }
        else
            m_uiHoarfrostTimer -= uiDiff;

        if (m_uiIcyBlastTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_uiMainTargetGUID))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ICY_BLAST : SPELL_ICY_BLAST_H);
            else if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ICY_BLAST : SPELL_ICY_BLAST_H);
            m_uiIcyBlastTimer = 35000;
        }
        else
            m_uiIcyBlastTimer -= uiDiff;

        if (m_uiIcyBlastSlowTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->GetMap()->GetUnit(m_uiMainTargetGUID))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ICY_BLAST_SLOW : SPELL_ICY_BLAST_SLOW_H);
            else if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCastSpellIfCan(pTarget, m_bIsRegularMode ? SPELL_ICY_BLAST_SLOW : SPELL_ICY_BLAST_SLOW_H);
            m_uiIcyBlastSlowTimer = 40000;
        }
        else
            m_uiIcyBlastSlowTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL boss_tyrannusAI : public ScriptedAI
{
    boss_tyrannusAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (instance_pit_of_saron*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        SetEquipmentSlots(false, EQUIP_ID, -1, -1);
        m_uiRimefangGUID.Clear();
        Reset();
    }

    instance_pit_of_saron* m_pInstance;
    bool m_bIsRegularMode;

    uint32 m_uiForcefulSmashTimer;
    uint32 m_uiOverlordsBrandTimer;
    uint32 m_uiDarkMightTimer;
    uint32 m_uiMarkOfRimefangTimer;
    uint32 TeamInInstance;

    ObjectGuid m_uiRimefangGUID;

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
        m_uiRimefangGUID.Clear();
        m_uiForcefulSmashTimer  = 10000;
        m_uiOverlordsBrandTimer = 35000;
        m_uiDarkMightTimer      = 40000;
        m_uiMarkOfRimefangTimer = 30000;
        TeamInInstance = GetFaction();
    }

    void JustReachedHome()
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_TYRANNUS, FAIL);

        m_creature->ForcedDespawn();
        if (Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
        {
            pRimefang->DeleteThreatList();
            if (pRimefang->GetVehicleKit())
                pRimefang->GetVehicleKit()->Reset();
        }
    }

    void Aggro(Unit* pWho)
    {
        if (!m_pInstance)
            return;

        m_pInstance->SetData(TYPE_TYRANNUS, IN_PROGRESS);

        m_creature->SetInCombatWithZone();

        if (Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
            pRimefang->AI()->AttackStart(pWho);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void KilledUnit(Unit* pVictim)
    {
        DoScriptText(urand(0, 1) ? SAY_SLAY_1 : SAY_SLAY_2, m_creature);
    }

    void JustDied(Unit* pKiller)
    {
        // wtf?
        //m_creature->SummonCreature(NPC_DAILY_DUNGEON, 1032.35f, 198.31f, 628.15f, 4.73f, TEMPSUMMON_MANUAL_DESPAWN, 5000);

        m_creature->SetCorpseDelay(3600);

        DoScriptText(SAY_DEATH, m_creature);

        // Temp hack until outro is implemented
        if (Creature* pRimefang = m_pInstance->instance->GetCreature(m_uiRimefangGUID))
        {
            pRimefang->GetMotionMaster()->Clear();
            pRimefang->GetMotionMaster()->MovePoint(0, 844.752f, 358.993f, 645.330f);
            pRimefang->setFaction(35);
            pRimefang->DeleteThreatList();
            pRimefang->RemoveAllAuras();
            pRimefang->ForcedDespawn(10000);
        }

        if (m_pInstance)
            m_pInstance->SetData(TYPE_TYRANNUS, DONE);

        if(TeamInInstance == ALLIANCE)
        {
            m_creature->SummonCreature(NPC_MARTIN_VICTUS_END, 1060.983f, 94.954f, 630.997f, 2.247f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            m_creature->SummonCreature(NPC_JAINA_PART2, 1065.983f, 94.954f, 630.997f, 2.247f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }

        if(TeamInInstance == HORDE)
        {
            m_creature->SummonCreature(NPC_GORKUN_IRONSKULL_END, 1065.983f, 94.954f, 630.997f, 2.247f, TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000);
            m_creature->SummonCreature(NPC_SYLVANAS_PART2, 1060.983f, 94.954f, 630.997f, 2.247f, TEMPSUMMON_DEAD_DESPAWN, 0);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (!m_creature->GetVehicle() && m_pInstance->GetData(TYPE_TYRANNUS) != DONE)
                m_creature->ForcedDespawn();
            if (!m_creature->isDead() && !m_pInstance->instance->GetCreature(m_uiRimefangGUID))
                m_creature->ForcedDespawn();
            return;
        }

        if (m_uiForcefulSmashTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature->getVictim(), m_bIsRegularMode ? SPELL_FORCEFUL_SMASH : SPELL_FORCEFUL_SMASH_H) == CAST_OK)
                m_uiForcefulSmashTimer = 50000;
        }
        else
            m_uiForcefulSmashTimer -= uiDiff;

        if (m_uiOverlordsBrandTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_OVERLORDS_BRAND) == CAST_OK)
                    m_uiOverlordsBrandTimer = 45000;
            }
        }
        else
            m_uiOverlordsBrandTimer -= uiDiff;

        if (m_uiDarkMightTimer < uiDiff)
        {
            if (DoCastSpellIfCan(m_creature, m_bIsRegularMode ? SPELL_DARK_MIGHT : SPELL_DARK_MIGHT_H) == CAST_OK)
            {
                DoScriptText(SAY_SMASH, m_creature);
                DoScriptText(EMOTE_SMASH, m_creature);

                m_uiDarkMightTimer = 60000;
            }
        }
        else
            m_uiDarkMightTimer -= uiDiff;

        if (m_uiMarkOfRimefangTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
            {
                if (DoCastSpellIfCan(pTarget, SPELL_MARK_OF_RIMEFANG) == CAST_OK)
                {
                    if (Creature* pRimefang = m_pInstance->GetSingleCreatureFromStorage(NPC_RIMEFANG))
                        ((boss_rimefangAI*)pRimefang->AI())->SetMainTarget(pTarget->GetObjectGuid());

                    DoScriptText(SAY_MARK, m_creature);
                    m_uiMarkOfRimefangTimer = urand(30000, 40000);
                }
            }
        }
        else
            m_uiMarkOfRimefangTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL npc_colapsing_icicleAI: public ScriptedAI
{
    npc_colapsing_icicleAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        pCreature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        pCreature->SetDisplayId(11686);     // make invisible
        m_creature->setFaction(14);
        SetCombatMovement(false);
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiDamageTimer;

    void Reset()
    {
        DoCast(m_creature, SPELL_ICICLE);
        m_uiDamageTimer = 500;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if(m_uiDamageTimer < uiDiff)
        {
            DoCast(m_creature, SPELL_ICICLE_DMG);
            m_uiDamageTimer = 10000;
        }
        else m_uiDamageTimer -= uiDiff;
    }
};

struct MANGOS_DLL_DECL npc_sylvanas_jaina_pos_endAI: public ScriptedAI
{
    npc_sylvanas_jaina_pos_endAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_lGuards.clear();
        lSlavesList.clear();
        Reset();
    }

    ScriptedInstance* m_pInstance;

    uint32 m_uiSpeech_Timer;
    uint8 m_uiOutro_Phase;
    bool m_bIsOutro;
    uint32 creatureEntry;
    uint32 TeamInInstance;

    ObjectGuid m_uiMartinGuid;
    ObjectGuid m_uiGorkunGuid;
    ObjectGuid m_uiTyrannusGuid;
    ObjectGuid m_uiSindragosaGuid;
    GUIDList m_lGuards;

    std::list<Creature*> lSlavesList;

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
        m_lGuards.clear();
        lSlavesList.clear();
        m_uiOutro_Phase     = 0;
        m_uiSpeech_Timer    = 1000;
        m_bIsOutro          = true;
        TeamInInstance = GetFaction();
        creatureEntry = m_creature->GetEntry();

        m_uiMartinGuid.Clear();
        m_uiGorkunGuid.Clear();
        m_uiTyrannusGuid.Clear();
        m_uiSindragosaGuid.Clear();
    }

    void TeleportPlayers()
    {
        Map* pMap = m_creature->GetMap();
        if(pMap)
        {
            Map::PlayerList const &lPlayers = pMap->GetPlayers();
            if (!lPlayers.isEmpty())
            {
                for(Map::PlayerList::const_iterator itr = lPlayers.begin(); itr != lPlayers.end(); ++itr)
                {
                    if (Player* pPlayer = itr->getSource())
                        pPlayer->TeleportTo(m_creature->GetMapId(), 1065.983f, 94.954f, 630.997f, 2.247f);
                }
            }
        }
    }

    void SummonHordeSlaves()
    {
        for (uint8 i = 0; i < 5; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_HORDE_1, SummonLoc[0].x + urand(0, 20), SummonLoc[0].y + urand(0, 20), SummonLoc[0].z, SummonLoc[0].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[0].x + urand(0, 20), MoveLoc[0].y + urand(0, 20), MoveLoc[0].z);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 5; i < 10; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_HORDE_2, SummonLoc[1].x + urand(0, 10), SummonLoc[1].y - urand(0, 10), SummonLoc[1].z, SummonLoc[1].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[2].x + urand(0, 20), MoveLoc[2].y - urand(0, 20), MoveLoc[2].z);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 10; i < 15; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_HORDE_3, SummonLoc[2].x - urand(0, 20), SummonLoc[2].y - urand(0, 20), SummonLoc[2].z, SummonLoc[2].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[1].x - urand(0, 20), MoveLoc[1].y - urand(0, 20), MoveLoc[1].z);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }
    }

    void SummonAlySlaves()
    {
        for (uint8 i = 0; i < 5; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_ALY_1, SummonLoc[0].x + urand(0, 20), SummonLoc[0].y + urand(0, 20), SummonLoc[0].z, SummonLoc[0].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[0].x + urand(0, 20), MoveLoc[0].y + urand(0, 20), MoveLoc[0].z);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 5; i < 10; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_ALY_2, SummonLoc[1].x + urand(0, 10), SummonLoc[1].y - urand(0, 10), SummonLoc[1].z, SummonLoc[1].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[2].x + urand(0, 20), MoveLoc[2].y - urand(0, 20), MoveLoc[2].z);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }

        for (uint8 i = 10; i < 15; i++)
        {
            Creature *pTemp = m_creature->SummonCreature(NPC_SLAVE_ALY_3, SummonLoc[2].x - urand(0, 20), SummonLoc[2].y - urand(0, 20), SummonLoc[2].z, SummonLoc[2].o, TEMPSUMMON_DEAD_DESPAWN, 0);
            if (pTemp)
            {
                pTemp->GetMotionMaster()->MovePoint(0, MoveLoc[1].x - urand(0, 20), MoveLoc[1].y - urand(0, 20), MoveLoc[1].z);
                m_lGuards.push_back(pTemp->GetObjectGuid());
            }
        }
    }

    void SummonAlyAssist()
    {
        Creature *pElandra = m_creature->SummonCreature(NPC_ELANDRA, SummonLoc[0].x, SummonLoc[0].y, SummonLoc[0].z, SummonLoc[20].o, TEMPSUMMON_DEAD_DESPAWN, 0);
        if (pElandra)
            pElandra->GetMotionMaster()->MovePoint(0, MoveLoc[3].x, MoveLoc[3].y, MoveLoc[3].z);
        Creature *pKoreln = m_creature->SummonCreature(NPC_KORELN, SummonLoc[1].x, SummonLoc[1].y, SummonLoc[1].z, SummonLoc[21].o, TEMPSUMMON_DEAD_DESPAWN, 0);
        if (pKoreln)
            pKoreln->GetMotionMaster()->MovePoint(0, MoveLoc[4].x, MoveLoc[4].y, MoveLoc[4].z);
    }

    void SummonHordeAssist()
    {
        Creature *pLoralen = m_creature->SummonCreature(NPC_LORALEN, SummonLoc[0].x, SummonLoc[0].y, SummonLoc[0].z, SummonLoc[20].o, TEMPSUMMON_DEAD_DESPAWN, 0);
        if (pLoralen)
            pLoralen->GetMotionMaster()->MovePoint(0, MoveLoc[3].x, MoveLoc[3].y, MoveLoc[3].z);
        Creature *pKelira = m_creature->SummonCreature(NPC_KALIRA, SummonLoc[1].x, SummonLoc[1].y, SummonLoc[1].z, SummonLoc[21].o, TEMPSUMMON_DEAD_DESPAWN, 0);
        if (pKelira)
            pKelira->GetMotionMaster()->MovePoint(0, MoveLoc[4].x, MoveLoc[4].y, MoveLoc[4].z);
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
                    if (TeamInInstance == ALLIANCE)
                    {
                        switch (creatureEntry)
                        {
                        case NPC_JAINA_PART2:
                            if(Creature* pSylvanas = GetClosestCreatureWithEntry(m_creature, NPC_SYLVANAS_PART2, 100.0f))
                            {
                                pSylvanas->ForcedDespawn();
                            }
                            break;
                        }
                    }
                    if (TeamInInstance == HORDE)
                    {
                        switch (creatureEntry)
                        {
                            case NPC_JAINA_PART2:
                            if(Creature* pSylvanas = GetClosestCreatureWithEntry(m_creature, NPC_SYLVANAS_PART2, 100.0f))
                            {
                                pSylvanas->ForcedDespawn();
                            }
                            break;
                            case NPC_SYLVANAS_PART2:
                            if(Creature* pJaina = GetClosestCreatureWithEntry(m_creature, NPC_JAINA_PART2, 100.0f))
                            {
                                pJaina->ForcedDespawn();
                            }
                            break;
                        }
                    }
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 5000;
                    break;
                case 1:
                    if(Creature* pTyrannus = GetClosestCreatureWithEntry(m_creature, NPC_RIMEFANG, 100.0f))
                    {
                        pTyrannus->ForcedDespawn();
                        pTyrannus->DealDamage(pTyrannus, pTyrannus->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        m_uiTyrannusGuid = pTyrannus->GetObjectGuid();
                    }
                    switch (creatureEntry)
                    {
                    case NPC_JAINA_PART2:
                        if(Creature* pMartin = GetClosestCreatureWithEntry(m_creature, NPC_MARTIN_VICTUS_END, 100.0f))
                        {
                            DoScriptText(SAY_OUTRO1_SLAVE_ALLY, pMartin);
                            SummonAlySlaves();
                            pMartin->GetMotionMaster()->MovePoint(0, 1014.670f, 158.714f, 628.156f);
                            pMartin->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            m_uiMartinGuid = pMartin->GetObjectGuid();
                        }
                        break;
                    case NPC_SYLVANAS_PART2:
                        if(Creature* pGorkun = GetClosestCreatureWithEntry(m_creature, NPC_GORKUN_IRONSKULL_END, 100.0f))
                        {
                            DoScriptText(SAY_OUTRO1_SLAVE_HORDE, pGorkun);
                            SummonHordeSlaves();
                            pGorkun->GetMotionMaster()->MovePoint(0, 1014.670f, 158.714f, 628.156f);
                            pGorkun->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);
                            m_uiGorkunGuid = pGorkun->GetObjectGuid();
                        }
                        break;
                    }
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 18000;
                    break;
                case 2:
                    if(Creature* pSindragosa = m_creature->SummonCreature(NPC_SINDRAGOSA, 977.224f, 164.056f, 653.216f,  0.3f, TEMPSUMMON_TIMED_DESPAWN, 18000))
                    {
                        m_uiSindragosaGuid = pSindragosa->GetObjectGuid();
                    }
                    switch (creatureEntry)
                    {
                    case NPC_JAINA_PART2:
                        if(Creature* pMartin = m_pInstance->instance->GetCreature(m_uiMartinGuid))
                            DoScriptText(SAY_OUTRO2_SLAVE, pMartin);
                        break;
                    case NPC_SYLVANAS_PART2:
                        if(Creature* pGorkun = m_pInstance->instance->GetCreature(m_uiGorkunGuid))
                            DoScriptText(SAY_OUTRO2_SLAVE, pGorkun);
                        break;
                    }
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 13000;
                    break;
                case 3:
                    switch (creatureEntry)
                    {
                    case NPC_JAINA_PART2:
                        if(Creature* pMartin = m_pInstance->instance->GetCreature(m_uiMartinGuid))
                        {
                            if(Creature* pSindragosa = m_pInstance->instance->GetCreature(m_uiSindragosaGuid))
                            {
                                pSindragosa->CastSpell(pMartin, SPELL_FROST_BOMB, false);
                            }
                        }
                        DoScriptText(SAY_OUTRO3_ALY, m_creature);
                        break;
                    case NPC_SYLVANAS_PART2:
                        if(Creature* pGorkun = m_pInstance->instance->GetCreature(m_uiGorkunGuid))
                        {
                            if(Creature* pSindragosa = m_pInstance->instance->GetCreature(m_uiSindragosaGuid))
                            {
                                pSindragosa->CastSpell(pGorkun, SPELL_FROST_BOMB, false);
                            }
                        }
                        DoScriptText(SAY_OUTRO3_HORDE, m_creature);
                        break;
                    }
                    TeleportPlayers();
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 5000;
                    break;
                case 4:
                    for (GUIDList::iterator i = m_lGuards.begin(); i != m_lGuards.end(); ++i)
                    {
                        if (Creature *pTemp = m_creature->GetMap()->GetCreature(*i))
                        {
                            pTemp->SetDeathState(JUST_DIED);
                            pTemp->SetHealth(0);
                            pTemp->SetStandFlags(UNIT_STAND_STATE_DEAD);
                            pTemp->DealDamage(pTemp, pTemp->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                    }
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 5000;
                case 5:
                    switch (creatureEntry)
                    {
                    case NPC_JAINA_PART2:
                        if(Creature* pMartin = m_pInstance->instance->GetCreature(m_uiMartinGuid))
                        {
                            pMartin->SetDeathState(JUST_DIED);
                            pMartin->SetHealth(0);
                            pMartin->SetStandFlags(UNIT_STAND_STATE_DEAD);
                            pMartin->DealDamage(pMartin, pMartin->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                        DoScriptText(SAY_OUTRO4_ALY, m_creature);
                        m_creature->GetMotionMaster()->MovePoint(0, 1068.709f, 208.378f, 628.156f);
                        SummonAlyAssist();
                        m_uiSpeech_Timer = 7000;
                        break;
                    case NPC_SYLVANAS_PART2:
                        if(Creature* pGorkun = m_pInstance->instance->GetCreature(m_uiGorkunGuid))
                        {
                            pGorkun->SetDeathState(JUST_DIED);
                            pGorkun->SetHealth(0);
                            pGorkun->SetStandFlags(UNIT_STAND_STATE_DEAD);
                            pGorkun->DealDamage(pGorkun, pGorkun->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        }
                        DoScriptText(SAY_OUTRO4_HORDE, m_creature);
                        m_creature->GetMotionMaster()->MovePoint(0, 1068.709f, 208.378f, 628.156f);
                        SummonHordeAssist();
                        m_bIsOutro = false;
                        break;
                    }
                    ++m_uiOutro_Phase;
                    break;
                case 6:
                    switch (creatureEntry)
                    {
                    case NPC_JAINA_PART2:
                        DoScriptText(SAY_OUTRO5_ALY, m_creature);
                        break;
                    case NPC_SYLVANAS_PART2:
                        break;
                    }
                    m_bIsOutro = false;
                    ++m_uiOutro_Phase;
                    m_uiSpeech_Timer = 5000;
                    break;

                default:
                    m_uiSpeech_Timer = 100000;
                }
            }else m_uiSpeech_Timer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_boss_tyrannus(Creature* pCreature)
{
    return new boss_tyrannusAI (pCreature);
}

CreatureAI* GetAI_boss_rimefang(Creature* pCreature)
{
    return new boss_rimefangAI (pCreature);
}

CreatureAI* GetAI_npc_colapsing_icicle(Creature* pCreature)
{
    return new npc_colapsing_icicleAI (pCreature);
}

CreatureAI* GetAI_npc_sylvanas_jaina_pos_end(Creature* pCreature)
{
    return new npc_sylvanas_jaina_pos_endAI (pCreature);
}

void AddSC_boss_tyrannus()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name="boss_scourgelord_tyrannus";
    pNewScript->GetAI = &GetAI_boss_tyrannus;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="boss_rimefang";
    pNewScript->GetAI = &GetAI_boss_rimefang;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name="npc_colapsing_icicle";
    pNewScript->GetAI = &GetAI_npc_colapsing_icicle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->GetAI = &GetAI_npc_sylvanas_jaina_pos_end;
    pNewScript->Name = "npc_slyvanas_jaina_pos_end";
    pNewScript->RegisterSelf();
}
