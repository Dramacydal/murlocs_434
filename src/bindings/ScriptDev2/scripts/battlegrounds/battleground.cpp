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
SDName: Battleground
SD%Complete: 100
SDComment: Spirit guides in battlegrounds will revive all players every 30 sec
SDCategory: Battlegrounds
EndScriptData */

#include "precompiled.h"
#include "BattleGround/BattleGroundSA.h"
#include "OutdoorPvP/OutdoorPvP.h"
#include "BattleField/BattleFieldWG.h"
#include "Vehicle.h"

// **** Script Info ****
// Spiritguides in battlegrounds resurrecting many players at once
// every 30 seconds - through a channeled spell, which gets autocasted
// the whole time
// if spiritguide despawns all players around him will get teleported
// to the next spiritguide
// here i'm not sure, if a dummyspell exist for it

// **** Quick Info ****
// battleground spiritguides - this script handles gossipHello
// and JustDied also it let autocast the channel-spell

enum
{
    SPELL_SPIRIT_HEAL_CHANNEL       = 22011,                // Spirit Heal Channel

    SPELL_SPIRIT_HEAL               = 22012,                // Spirit Heal
    SPELL_SPIRIT_HEAL_MANA          = 44535,                // in battlegrounds player get this no-mana-cost-buff

    SPELL_WAITING_TO_RESURRECT      = 2584                  // players who cancel this aura don't want a resurrection
};

struct MANGOS_DLL_DECL npc_spirit_guideAI : public ScriptedAI
{
    npc_spirit_guideAI(Creature* pCreature) : ScriptedAI(pCreature) {Reset();}

    void Reset()
    {
    }

    void UpdateAI(const uint32 uiDiff)
    {
        // auto cast the whole time this spell
        if (!m_creature->GetCurrentSpell(CURRENT_CHANNELED_SPELL))
        {
            m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL, true);
            m_creature->CastSpell(m_creature, SPELL_SPIRIT_HEAL_CHANNEL, false);
        }
    }

    void CorpseRemoved(uint32 &)
    {
        // TODO: would be better to cast a dummy spell
        Map* pMap = m_creature->GetMap();

        if (!pMap || !pMap->IsBattleGround())
            return;

        Map::PlayerList const &PlayerList = pMap->GetPlayers();

        for(Map::PlayerList::const_iterator itr = PlayerList.begin(); itr != PlayerList.end(); ++itr)
        {
            Player* pPlayer = itr->getSource();
            if (!pPlayer || !pPlayer->IsWithinDistInMap(m_creature, 20.0f) || !pPlayer->HasAura(SPELL_WAITING_TO_RESURRECT))
                continue;

            // repop player again - now this node won't be counted and another node is searched
            pPlayer->RepopAtGraveyard();
        }
    }

    void SpellHitTarget (Unit* pUnit, const SpellEntry* pSpellEntry)
    {
        if (pSpellEntry->Id == SPELL_SPIRIT_HEAL && pUnit->GetTypeId() == TYPEID_PLAYER
            && pUnit->HasAura(SPELL_WAITING_TO_RESURRECT))
            pUnit->CastSpell(pUnit, SPELL_SPIRIT_HEAL_MANA, true);
    }
};

bool GossipHello_npc_spirit_guide(Player* pPlayer, Creature* pCreature)
{
    pPlayer->CastSpell(pPlayer, SPELL_WAITING_TO_RESURRECT, true);
    return true;
}

CreatureAI* GetAI_npc_spirit_guide(Creature* pCreature)
{
    return new npc_spirit_guideAI(pCreature);
}

/*######
## go_def_portal
######*/

static float SpawnLocation[BG_SA_GATE_COUNT - 1][3] =
{
    { 1394.07f,  72.3632f, 31.0541f },  // Green
    { 1468.12f,   -225.7f, 30.8969f },  // Blue
    { 1216.12f,  47.7665f, 54.2785f },  // Purple
    { 1255.73f, -233.153f, 56.4357f },  // Red
    { 1065.02f, -89.9522f, 81.0758f },  // Yellow
};

static float TeleLocation[BG_SA_GATE_COUNT - 1][3] =
{
    { 1388.78f,   103.15f, 34.4902f },  // Green
    { 1441.08f, -240.659f,  35.263f },  // Blue
    { 1193.33f,   69.945f, 58.0450f },  // Purple
    { 1228.01f, -235.402f, 60.0280f },  // Red
    { 1043.07f,  -88.100f, 87.11295f }, // Yellow
};

#define SPELL_DEFENDERS_PORTAL_TRIGGERED 54643

bool GO_Use_Portal_Sota(Player* pPlayer, GameObject* pGo)
{
    if (BattleGround* bg = pPlayer->GetBattleGround())
    {
        if (bg->GetTypeID(true) == BATTLEGROUND_SA && pPlayer->GetTeam() == bg->GetDefender())
        {
            if (!pPlayer->HasAura(SPELL_DEFENDERS_PORTAL_TRIGGERED))
            {
                for (uint8 i = 0; i < BG_SA_GATE_COUNT - 1; ++i)
                {
                    if (pGo->GetPositionX() == SpawnLocation[i][0] &&
                        pGo->GetPositionY() == SpawnLocation[i][1] &&
                        pGo->GetPositionZ() == SpawnLocation[i][2])
                    {
                        ++i;
                        BattleGroundSA* bgsa = (BattleGroundSA*)bg;

                        if (bgsa->GetGateStatus(BG_SA_T_GO_GATE_YELLOW_MOON) == BG_SA_GATE_STATUS_DESTROYED || 
                            bgsa->GetGateStatus(BG_SA_T_GO_GATE_PURPLE_AMETHYST) == BG_SA_GATE_STATUS_DESTROYED ||
                            bgsa->GetGateStatus(BG_SA_T_GO_GATE_RED_SUN) == BG_SA_GATE_STATUS_DESTROYED)
                            i = BG_SA_T_GO_GATE_YELLOW_MOON;
                        else
                        {
                            if (bgsa->GetGateStatus(BG_SA_T_GO_GATE_GREEN_EMERALD) == BG_SA_GATE_STATUS_DESTROYED ||
                                bgsa->GetGateStatus(BG_SA_T_GO_GATE_BLUE_SAPPHIRE) == BG_SA_GATE_STATUS_DESTROYED)
                            {
                                if (i == BG_SA_T_GO_GATE_GREEN_EMERALD)
                                    i = BG_SA_T_GO_GATE_PURPLE_AMETHYST;
                                else if (i == BG_SA_T_GO_GATE_BLUE_SAPPHIRE)
                                    i = BG_SA_T_GO_GATE_RED_SUN;
                            }
                        }
                        --i;

                        pPlayer->TeleportTo(bg->GetMapId(), TeleLocation[i][0], TeleLocation[i][1], TeleLocation[i][2], pPlayer->GetOrientation());
                        return false;
                    }
                }
            }
            else
                pPlayer->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, NULL);
        }
        else
            pPlayer->SendEquipError(EQUIP_ERR_CANT_DO_RIGHT_NOW, NULL);

        return true;
    }

    return true;
}

bool GOUse_go_def_portal(Player* pPlayer, GameObject* pGo)
{
    if (pGo->GetMapId() == 607)
        return GO_Use_Portal_Sota(pPlayer, pGo);
    else
    {
        if (pPlayer->HasAura(SPELL_TELEPORT_PORTAL))
            return true;

        for (uint8 i = 0; i < WG_TELEPORTS_COUNT - 2; ++i)
        {
            if (pGo->GetDistance2d(WGTeleportLocations[i].x, WGTeleportLocations[i].y) < 5.0f)
            {
                pPlayer->NearTeleportTo(WGTeleportLocations[i].destX, WGTeleportLocations[i].destY, WGTeleportLocations[i].destZ, pPlayer->GetOrientation(), true);
                pPlayer->CastSpell(pPlayer, SPELL_TELEPORT_PORTAL, true);
                break;
            }
        }
    }

    return true;
}

void AddSC_battleground()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "npc_spirit_guide";
    pNewScript->GetAI = &GetAI_npc_spirit_guide;
    pNewScript->pGossipHello = &GossipHello_npc_spirit_guide;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_def_portal";
    pNewScript->pGOUse = &GOUse_go_def_portal;
    pNewScript->RegisterSelf();
}
