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
SDName: ulduar_teleport
SD%Complete: 95%
SDComment: by /dev/rsa
SDCategory: Ulduar instance
EndScriptData */

#include "precompiled.h"
#include "ulduar.h"

enum
{
    PORTALS_COUNT               = 9,
    TELEPORT_GOSSIP_MESSAGE     = 14424,

    // spells
    SPELL_TELEPORT_BASE_CAMP    = 64014,
    SPELL_TELEPORT_GROUNDS      = 64032,
    SPELL_TELEPORT_FORGE        = 64028,
    SPELL_TELEPORT_SCRAPYARD    = 64031,
    SPELL_TELEPORT_ANTECHAMBER  = 64030,
    SPELL_TELEPORT_WALKWAY      = 64029,
    SPELL_TELEPORT_CONSERVATORY = 64024,
    SPELL_TELEPORT_SPARK        = 65061,
    SPELL_TELEPORT_PRISON       = 65042,
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
{-3050001,603, -706.122f, -92.6024f, 430.176f, 4.19f, SPELL_TELEPORT_BASE_CAMP,   true,true, TYPE_LEVIATHAN},  // base camp
{-3050002,603, 131.248f,  -35.3802f, 410.104f, 0,     SPELL_TELEPORT_GROUNDS,     true,true, TYPE_LEVIATHAN_TP},  // formation ground
{-3050003,603, 553.233f,  -12.3247f, 409.979f, 0,     SPELL_TELEPORT_FORGE,       false,true,TYPE_LEVIATHAN}, //
{-3050004,603, 926.292f,  -11.4635f, 418.895f, 3.19f, SPELL_TELEPORT_SCRAPYARD,   false,true,TYPE_XT002_TP},        //
{-3050005,603, 1498.09f,  -24.246f,  421.267f, 0,     SPELL_TELEPORT_ANTECHAMBER, false,true,TYPE_XT002},        //
{-3050006,603, 1859.45f,  -24.1f,    449.2f,   0,     SPELL_TELEPORT_WALKWAY,     false,true,TYPE_KOLOGARN},        //
{-3050007,603, 2086.27f,  -24.3134f, 421.539f, 0,     SPELL_TELEPORT_CONSERVATORY,false,true,TYPE_AURIAYA},         //
{-3050008,603, 2517.3979f, 2568.89f, 412.99f,  6.17f, SPELL_TELEPORT_SPARK,       false,true,TYPE_MIMIRON_TP},          //
{-3050009,603, 1854.782f, -11.3819f, 335.27f,  5.86f, SPELL_TELEPORT_PRISON,      false,true,TYPE_VEZAX},           //
};


bool GOGossipSelect_go_ulduar_teleporter(Player *pPlayer, GameObject* pGo, uint32 sender, uint32 action)
{
    int32 damage = 0;
    if(sender != GOSSIP_SENDER_MAIN) return false;

    if(!pPlayer->getAttackers().empty()) return false;

    if(action >= 0 && action <= PORTALS_COUNT)
    pPlayer->TeleportTo(PortalLoc[action].map_num, PortalLoc[action].x, PortalLoc[action].y, PortalLoc[action].z, PortalLoc[action].o);
    if (PortalLoc[action].spellID != 0 )
        if (SpellEntry const* spell = (SpellEntry *)GetSpellStore()->LookupEntry(PortalLoc[action].spellID))
        {
            SpellAuraHolder *holder = CreateSpellAuraHolder(spell, pPlayer, pPlayer);
            Aura *aura = CreateAura(spell, EFFECT_INDEX_2, NULL, holder, pPlayer);
            holder->AddAura(aura, EFFECT_INDEX_2);
            pPlayer->AddSpellAuraHolder(holder);
        }

    pPlayer->CLOSE_GOSSIP_MENU();
    return true;
}

bool GOGossipHello_go_ulduar_teleporter(Player *pPlayer, GameObject* pGo)
{
    ScriptedInstance *pInstance = (ScriptedInstance *) pGo->GetInstanceData();

    if (!pInstance || !pPlayer) return false;
    if (pPlayer->isInCombat()) return true;

    for(uint8 i = 0; i < PORTALS_COUNT; i++) {
    if ((PortalLoc[i].active == true && 
        (PortalLoc[i].state == true || 
        pInstance->GetData(PortalLoc[i].encounter) == DONE ||
        pInstance->GetData(PortalLoc[i].encounter) == IN_PROGRESS))
        || pPlayer->isGameMaster())
             pPlayer->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_TAXI, PortalLoc[i].textNum, GOSSIP_SENDER_MAIN, i);
    };
    pPlayer->SEND_GOSSIP_MENU(TELEPORT_GOSSIP_MESSAGE, pGo->GetGUID());
    return true;
}

void AddSC_ulduar_teleport()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "go_ulduar_teleporter";
    newscript->pGossipHelloGO  = &GOGossipHello_go_ulduar_teleporter;
    newscript->pGossipSelectGO = &GOGossipSelect_go_ulduar_teleporter;
    newscript->RegisterSelf();
}
