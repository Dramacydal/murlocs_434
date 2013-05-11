/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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
SDName: end_of_time
SD%Complete: 0
SDComment: Placeholder
SDCategory: End of Time
EndScriptData */

#include "precompiled.h"
#include "end_of_time.h"

enum
{
    TEXT_TELE_START     = 2000006859,
    TEXT_TELE_BLACK     = 2000006860,
    TEXT_TELE_AZURE     = 2000006861,
    TEXT_TELE_RUBY      = 2000006862,
    TEXT_TELE_EMERALD   = 2000006863,
    TEXT_TELE_BRONZE    = 2000006864,
};

bool OnGossipHello_go_time_transit_device(Player* who, GameObject* go)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData();
    if (!pInstance || !who)
        return true;

    uint32 encounterMask = pInstance->GetData(TYPE_ENCOUNTER_MASK);

    who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TEXT_TELE_START, GOSSIP_SENDER_MAIN, 0);
    if (encounterMask & (1 << (TYPE_BAINE - 1)))
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TEXT_TELE_BLACK, GOSSIP_SENDER_MAIN, 1);
    if (encounterMask & (1 << (TYPE_JAINA - 1)))
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TEXT_TELE_AZURE, GOSSIP_SENDER_MAIN, 2);
    if (pInstance->GetData(TYPE_BAINE) == DONE || pInstance->GetData(TYPE_JAINA) == DONE)
    {
        if (encounterMask & (1 << (TYPE_SYLVANAS - 1)))
            who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TEXT_TELE_RUBY, GOSSIP_SENDER_MAIN, 3);
        if (encounterMask & (1 << (TYPE_TYRANDE - 1)))
            who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TEXT_TELE_EMERALD, GOSSIP_SENDER_MAIN, 4);
    }
    if (pInstance->GetData(TYPE_SYLVANAS) == DONE || pInstance->GetData(TYPE_TYRANDE) == DONE)
        who->ADD_GOSSIP_ITEM_ID(GOSSIP_ICON_CHAT, TEXT_TELE_BRONZE, GOSSIP_SENDER_MAIN, 5);

    who->SEND_GOSSIP_MENU(who->GetGossipTextId(go), go->GetObjectGuid());

    return true;
}

bool OnGossipSelect_go_time_transit_device(Player* who, GameObject* go, uint32 sender, uint32 action)
{
    if (who->IsInCombat())
        return true;

    if (sender != GOSSIP_SENDER_MAIN)
        return true;

    switch (action)
    {
        case 0: who->CastSpell(who, SPELL_TELEPORT_ENTRANCE, true); break;
        case 1: who->CastSpell(who, SPELL_TELEPORT_BLACK_DRAGONSHRINE, true); break;
        case 2: who->CastSpell(who, SPELL_TELEPORT_BLUE_DRAGONSHRINE, true); break;
        case 3: who->CastSpell(who, SPELL_TELEPORT_RUBY_DRAGONSHRINE, true); break;
        case 4: who->CastSpell(who, SPELL_TELEPORT_EMERALD_DRAGONSHRINE, true); break;
        case 5: who->CastSpell(who, SPELL_TELEPORT_BRONZE_DRAGONSHRINE, true); break;
    }

    return true;
}

bool OnGoUse_go_hourglass_of_time(Player* who, GameObject* go)
{
    ScriptedInstance* pInstance = (ScriptedInstance*)go->GetInstanceData();
    if (!pInstance || !who)
        return true;

    ((instance_end_of_time*)pInstance)->OnHourglassUse(who);
    return true;
}

void AddSC_end_of_time()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "go_time_transit_device_end_of_time";
    pNewScript->pGossipHelloGO = &OnGossipHello_go_time_transit_device;
    pNewScript->pGossipSelectGO = &OnGossipSelect_go_time_transit_device;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "go_hourglass_of_time";
    pNewScript->pGOUse = &OnGoUse_go_hourglass_of_time;
    pNewScript->RegisterSelf();
}
