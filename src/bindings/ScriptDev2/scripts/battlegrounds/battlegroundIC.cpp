/* Copyright (C) 2006 - 2011 ScriptDev2 <http://www.scriptdev2.com/>
   Copyright (C) 2011 MangosR2_ScriptDev2
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
SDName: Battleground_IoC
SD%Complete: %?
SDComment: Battleground isle of conquest
SDCategory: Battleground_IoC
EndScriptData */

#include "precompiled.h"
#include "BattleGround/BattleGroundIC.h"
#include "Vehicle.h"

/*######
## go_ic_teleport
## npc_ic_vehicle
## npc_ic_cannon
## boss_bg_ioc
######*/

#define MAX_PORTALS        15

enum
{
    SPELL_TELEPORT_EFFECT  = 66548,         // teleport effect

    NPC_CATAPULT           = 34793,
    NPC_DEMOLISHER         = 34775,
    NPC_GLAIVE_A           = 34802,
    NPC_GLAIVE_H           = 35273,

    // bosses
    SPELL_BRUTAL_STRIKE     = 58460,
    SPELL_RAGE              = 66776,
    SPELL_DAGGER_THROW      = 67280,
    SPELL_CRUSHING_LEAP     = 68506,
};

static float SpawnLocation[MAX_PORTALS][3]=
{
    {324.635f, -749.128f, 49.3602f}, // alli left outside
    {311.92f, -913.972f, 48.8159f},  // a r o
    {425.675f, -857.09f, 48.5104f},  // a f o
    {1236.53f, -669.415f, 48.2729f}, // h l o
    {1235.09f, -857.898f, 48.9163f}, // h r o
    {1143.25f, -779.599f, 48.629f},  // h f o

    {326.285f, -777.366f, 49.0208f}, // a l i
    {323.54f, -888.361f, 48.9197f},  // a r i
    {397.089f, -859.382f, 48.8993f}, // a f i
    {1235.53f, -683.872f, 49.304f},  // h l i
    {1233.27f, -844.526f, 48.8824f}, // h r i
    {1158.76f, -746.182f, 48.6277f}, // horde front inside

    {827.958f, -994.467f, 134.071f}, // gunship portals (not working yet)
    {738.613f, -1106.58f, 134.745f},
    {672.283f, -1156.99f, 133.706f},
};

bool GOHello_go_ic_teleport(Player* pPlayer, GameObject* pGo)
{
    if (pPlayer->GetMapId() != 628)
        return false;

    if (BattleGround* bg = pPlayer->GetBattleGround())                  // no need to check for faction, since only the eligible players can click the portal
    {
        for (uint32 i = 0; i < MAX_PORTALS; ++i)
        {
            if (pPlayer->GetDistance2d(SpawnLocation[i][0], SpawnLocation[i][1]) < 10.0f)
            {
                // teleports outside walls
                if (i <= 5)
                    pPlayer->NearTeleportTo(SpawnLocation[i + 6][0], SpawnLocation[i + 6][1], SpawnLocation[i + 6][2], pPlayer->GetOrientation());
                // teleports within the keep
                else if (i > 5 && i <= 11)
                    pPlayer->NearTeleportTo(SpawnLocation[i - 6][0], SpawnLocation[i - 6][1], SpawnLocation[i - 6][2], pPlayer->GetOrientation());
                // gunship teleports
                else
                {
                    BattleGroundIC* bgIC = (BattleGroundIC*)bg;
                    Transport* gunship = NULL;
                    uint8 idx = pPlayer->GetTeam() == ALLIANCE ? 0 : 1;
                    if (idx)
                        gunship = bgIC->gunshipHorde;
                    else
                        gunship = bgIC->gunshipAlliance;

                    if (gunship)
                    {
                        pPlayer->m_movementInfo.SetTransportData(gunship->GetObjectGuid(), BG_IC_Trans_Offsets[idx][0],  BG_IC_Trans_Offsets[idx][1],  BG_IC_Trans_Offsets[idx][2],  BG_IC_Trans_Offsets[idx][3], 0, -1);
                        pPlayer->NearTeleportTo(gunship->GetPositionX() +  BG_IC_Trans_Offsets[idx][0], gunship->GetPositionY() +  BG_IC_Trans_Offsets[idx][1], gunship->GetPositionZ() +  BG_IC_Trans_Offsets[idx][2], gunship->GetOrientation() +  BG_IC_Trans_Offsets[idx][3]);
                    }
                }
                return true;
            }
        }
    }
    return false;
}

struct MANGOS_DLL_DECL npc_ic_vehicleAI : public ScriptedAI
{
    npc_ic_vehicleAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        done = false;
        gotFaction = false;
    }

    bool done;
    bool gotFaction;
    BattleGround *bg;

    void Aggro(Unit* /*who*/){ m_creature->CombatStop(); }

    void EnterCombat(Unit *pEnemy)
    {
        if (!m_creature->isCharmed())
            m_creature->CombatStop();
    }

    void StartEvent(Player* pPlayer, Creature* pCreature)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (VehicleKit *vehicle = pCreature->GetVehicleKit())
            {
                if (!pCreature->GetCharmerGuid().IsEmpty())
                    pPlayer->EnterVehicle(vehicle);
                else
                {
                    pPlayer->EnterVehicle(vehicle);
                    pPlayer->CastSpell(pCreature, 60968, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->isCharmed())
        {
            if (m_creature->isInCombat())
                m_creature->CombatStop();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!done)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap || !pMap->IsBattleGround())
                    return;

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                Map::PlayerList::const_iterator itr = PlayerList.begin();
                Player *player = itr->getSource();
                if (player)
                {
                    bg = player->GetBattleGround();
                    done = true;
                }
            }

            if (bg)
            {
                if (gotFaction == false)
                {
                    if (m_creature->GetEntry() == NPC_DEMOLISHER)
                        m_creature->setFaction(bg->GetVehicleFaction(VEHICLE_BG_DEMOLISHER));
                    else if (m_creature->GetEntry() == NPC_CATAPULT)
                        m_creature->setFaction(bg->GetVehicleFaction(VEHICLE_IC_CATAPULT));
                    // Glaive throwers have separate ID for horde/alli
                    else if (m_creature->GetEntry() == NPC_GLAIVE_A)
                        m_creature->setFaction(VEHICLE_FACTION_ALLIANCE);
                    else if (m_creature->GetEntry() == NPC_GLAIVE_H)
                        m_creature->setFaction(VEHICLE_FACTION_HORDE);

                    gotFaction = true;
                }
            }
        }
    }
};

CreatureAI* GetAI_npc_ic_vehicle(Creature* pCreature)
{
    return new npc_ic_vehicleAI(pCreature);
}

bool GossipHello_npc_ic_vehicle(Player* pPlayer, Creature* pCreature)
{
     pPlayer->CLOSE_GOSSIP_MENU();
     ((npc_ic_vehicleAI*)pCreature->AI())->StartEvent(pPlayer, pCreature);
         return true;
}

struct MANGOS_DLL_DECL npc_ic_cannonAI : public ScriptedAI
{
    npc_ic_cannonAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        SetCombatMovement(false);
        Reset();
    }

    void Reset()
    {
        done = false;
    }

    bool done;
    BattleGround *bg;

    void Aggro(Unit* who){ m_creature->CombatStop(); }

    void StartEvent(Player* pPlayer, Creature* pCreature)
    {
        if (BattleGround *bg = pPlayer->GetBattleGround())
        {
            if (bg->GetStatus() == STATUS_WAIT_JOIN)
                return;

            if (VehicleKit *vehicle = pCreature->GetVehicleKit())
            {
                if (!pCreature->GetCharmerGuid().IsEmpty())
                    pPlayer->EnterVehicle(vehicle);
                else
                {
                    pPlayer->EnterVehicle(vehicle);
                    pPlayer->CastSpell(pCreature, 60968, true);
                }
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (m_creature->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE))
            m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

        if (!m_creature->isCharmed())
        {
            if (m_creature->isInCombat())
                m_creature->CombatStop();

            m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

            if (!done)
            {
                Map* pMap = m_creature->GetMap();

                if (!pMap || !pMap->IsBattleGround())
                    return;

                Map::PlayerList const &PlayerList = pMap->GetPlayers();
                Map::PlayerList::const_iterator itr = PlayerList.begin();
                Player *player = itr->getSource();
                if (player)
                {
                    bg = player->GetBattleGround();
                    done = true;
                }
            }

            if (bg)
            {
                // simplest way to determine factions Ive found
                float x = m_creature->GetPositionX();
                if (x > 1000.0f)
                    m_creature->setFaction(VEHICLE_FACTION_HORDE);
                else
                    m_creature->setFaction(VEHICLE_FACTION_ALLIANCE);
            }
        }
    }
};

CreatureAI* GetAI_npc_ic_cannon(Creature* pCreature)
{
    return new npc_ic_cannonAI(pCreature);
}

bool GossipHello_npc_ic_cannon(Player* pPlayer, Creature* pCreature)
{
     pPlayer->CLOSE_GOSSIP_MENU();
     ((npc_ic_cannonAI*)pCreature->AI())->StartEvent(pPlayer, pCreature);
         return true;
}

struct MANGOS_DLL_DECL boss_bg_iocAI : public ScriptedAI
{
    boss_bg_iocAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 m_uiBrutalStrikeTimer;
    uint32 m_uiDaggerThrowTimer;

    void Reset()
    {
        m_uiBrutalStrikeTimer = urand(5000, 10000);
        m_uiDaggerThrowTimer = urand(15000, 20000);
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (m_uiBrutalStrikeTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0))
                DoCast(pTarget, SPELL_BRUTAL_STRIKE);
            m_uiBrutalStrikeTimer = urand(5000, 10000);
        }
        else
            m_uiBrutalStrikeTimer -= uiDiff;

        if (m_uiDaggerThrowTimer < uiDiff)
        {
            if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                DoCast(pTarget, SPELL_DAGGER_THROW);
            m_uiDaggerThrowTimer = urand(15000, 20000);
        }
        else
            m_uiDaggerThrowTimer -= uiDiff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_bg_ioc(Creature* pCreature)
{
    return new boss_bg_iocAI(pCreature);
}

void AddSC_battlegroundIC()
{
    Script *pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_ic_teleport";
    pNewScript->pGOUse = &GOHello_go_ic_teleport;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ic_vehicle";
    pNewScript->GetAI = &GetAI_npc_ic_vehicle;
    pNewScript->pGossipHello = &GossipHello_npc_ic_vehicle;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_ic_cannon";
    pNewScript->GetAI = &GetAI_npc_ic_cannon;
    pNewScript->pGossipHello = &GossipHello_npc_ic_cannon;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "boss_bg_ioc";
    pNewScript->GetAI = &GetAI_boss_bg_ioc;
    pNewScript->RegisterSelf();
}
