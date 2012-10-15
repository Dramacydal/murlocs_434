/* Copyright (C) 2006 - 2011 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
SDName: icecrown_teleport
SD%Complete: 100%
SDComment: by /dev/rsa
SDCategory: Icecrown Citadel
EndScriptData */
#include "precompiled.h"
#include "icecrown_citadel.h"

enum 
{
    PORTALS_COUNT = 7
};

struct t_Locations
{
    int textNum;
    uint32 map_num;
    float x, y, z, o;
    uint32 spellID;
    bool state;
    bool active;
    uint32 encounter;
};

static t_Locations PortalLoc[]=
{
    {-3631600,MAP_NUM,-17.1928f, 2211.44f, 30.1158f,3.14f,70856,true,true,TYPE_TELEPORT}, //
    {-3631601,MAP_NUM,-503.62f, 2211.47f, 62.8235f,3.14f,70856,false,true,TYPE_MARROWGAR},  //
    {-3631602,MAP_NUM,-615.145f, 2211.47f, 199.972f,0,70857,false,true,TYPE_DEATHWHISPER}, //
    {-3631603,MAP_NUM,-549.131f, 2211.29f, 539.291f,0,70858,false,true,TYPE_FLIGHT_WAR}, //
    {-3631604,MAP_NUM,4198.42f, 2769.22f, 351.065f,0,70859,false,true,TYPE_SAURFANG}, //
    {-3631606,MAP_NUM,4356.580078f, 2565.75f, 220.401993f,4.90f,70861,false,true,TYPE_VALITHRIA}, //
    {-3631607,MAP_NUM,528.767273f, -2124.845947f, 1050.1f,3.14f, 70860,false,true,TYPE_KINGS_OF_ICC}, //
};


bool GOGossipSelect_go_icecrown_teleporter(Player *pPlayer, GameObject* pGo, uint32 sender, uint32 action)
{
    if(sender != GOSSIP_SENDER_MAIN)
        return false;

    if(!pPlayer->getAttackers().empty())
        return false;

    if(action >= 0 && action < PORTALS_COUNT)
    {
        pPlayer->TeleportTo(PortalLoc[action].map_num, PortalLoc[action].x, PortalLoc[action].y, PortalLoc[action].z, PortalLoc[action].o);
        if (action == PORTALS_COUNT - 1)
            pPlayer->CastSpell(pPlayer, 12438, true);
    }

    if (PortalLoc[action].spellID != 0)
        pPlayer->_AddAura(PortalLoc[action].spellID, 2000);

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

bool GOGossipHello_go_icecrown_teleporter(Player *pPlayer, GameObject* pGo)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGo->GetInstanceData();

    if (!pInstance || !pPlayer)
        return false;
    if (pPlayer->isInCombat())
        return true;

    for(uint8 i = 0; i < PORTALS_COUNT; i++)
        if (PortalLoc[i].active == true && (PortalLoc[i].state || pInstance->GetData(PortalLoc[i].encounter) == DONE || pPlayer->isGameMaster()))
            pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, PortalLoc[i].textNum, GOSSIP_SENDER_MAIN, i);

    pPlayer->SEND_GOSSIP_MENU(TELEPORT_GOSSIP_MESSAGE, pGo->GetGUID());
    return true;
}

bool GOHello_go_plague_sigil(Player *player, GameObject* pGo)
{
    instance_icecrown_spire* pInstance = (instance_icecrown_spire*)pGo->GetInstanceData();
    if(!pInstance)
        return false;

    if (pInstance->GetData(TYPE_FESTERGUT) == DONE &&
        pInstance->GetData(TYPE_ROTFACE) == DONE)
        {
            pInstance->DoOpenDoorGuid(pInstance->GetData64(GO_SCIENTIST_DOOR_ORANGE));
            pInstance->DoOpenDoorGuid(pInstance->GetData64(GO_SCIENTIST_DOOR_GREEN));
            pInstance->DoOpenDoorGuid(pInstance->GetData64(GO_SCIENTIST_DOOR_COLLISION));
        }
    return true;
}

bool GOHello_go_bloodwing_sigil(Player *player, GameObject* pGo)
{
    instance_icecrown_spire* pInstance = (instance_icecrown_spire*)pGo->GetInstanceData();
    if (!pInstance)
        return false;

    if (pInstance->GetData(TYPE_SAURFANG) == DONE)
        pInstance->DoOpenDoorGuid(pInstance->GetData64(GO_BLOODWING_DOOR));

    return true;
}

bool GOHello_go_frostwing_sigil(Player *player, GameObject* pGo)
{
    instance_icecrown_spire* pInstance = (instance_icecrown_spire*)pGo->GetInstanceData();
    if(!pInstance)
        return false;

    if (pInstance->GetData(TYPE_SAURFANG) == DONE)
        pInstance->DoOpenDoorGuid(pInstance->GetData64(GO_FROSTWING_DOOR));

    return true;
}


void AddSC_icecrown_teleporter()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_icecrown_teleporter";
    newscript->pGossipHelloGO  = &GOGossipHello_go_icecrown_teleporter;
    newscript->pGossipSelectGO = &GOGossipSelect_go_icecrown_teleporter;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_plague_sigil";
    newscript->pGOUse  = &GOHello_go_plague_sigil;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_bloodwing_sigil";
    newscript->pGOUse  = &GOHello_go_bloodwing_sigil;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_frostwing_sigil";
    newscript->pGOUse  = &GOHello_go_frostwing_sigil;
    newscript->RegisterSelf();
}
