/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
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

#include "Common.h"
#include "Database/DatabaseEnv.h"
#include "World.h"
#include "Player.h"
#include "Opcodes.h"
#include "Chat.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Language.h"
#include "AccountMgr.h"
#include "ArenaTeam.h"
#include "ScriptMgr.h"
#include "SystemConfig.h"
#include "revision.h"
#include "revision_nr.h"
#include "Util.h"
#include "ObjectMgr.h"
#include "VMapFactory.h"
#include "SQLStorages.h"
#include <iostream>

#ifndef sMapMgr
    #include "MapManager.h"
#endif

bool ChatHandler::HandleHelpCommand(char* args)
{
    if(!*args)
    {
        ShowHelpForCommand(getCommandTable(), "help");
        ShowHelpForCommand(getCommandTable(), "");
    }
    else
    {
        if (!ShowHelpForCommand(getCommandTable(), args))
            SendSysMessage(LANG_NO_CMD);
    }

    return true;
}

bool ChatHandler::HandleCommandsCommand(char* /*args*/)
{
    ShowHelpForCommand(getCommandTable(), "");
    return true;
}

bool ChatHandler::HandleAccountCommand(char* args)
{
    // let show subcommands at unexpected data in args
    if (*args)
        return false;

    AccountTypes gmlevel = GetAccessLevel();
    PSendSysMessage(LANG_ACCOUNT_LEVEL, uint32(gmlevel));
    return true;
}

bool ChatHandler::HandleStartCommand(char* /*args*/)
{
    Player *chr = m_session->GetPlayer();

    if(chr->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    if(chr->isInCombat())
    {
        SendSysMessage(LANG_YOU_IN_COMBAT);
        SetSentErrorMessage(true);
        return false;
    }

    // cast spell Stuck
    chr->CastSpell(chr,7355,false);
    return true;
}

bool ChatHandler::HandleServerInfoCommand(char* /*args*/)
{
    char const* full;
    if(m_session)
        full = _FULLVERSION(REVISION_DATE,REVISION_TIME,"1738","|cffffffff|Hurl:" REVISION_ID "|h" REVISION_ID "|h|r");
    else
        full = _FULLVERSION(REVISION_DATE,REVISION_TIME,"1738",REVISION_ID);
    SendSysMessage(full);

    if (GetAccessLevel() >= SEC_ADMINISTRATOR)
    {
        if (sScriptMgr.IsScriptLibraryLoaded())
        {
            char const* ver = sScriptMgr.GetScriptLibraryVersion();
            if (ver && *ver)
                PSendSysMessage(LANG_USING_SCRIPT_LIB, ver);
            else
                SendSysMessage(LANG_USING_SCRIPT_LIB_UNKNOWN);
        }
        else
            SendSysMessage(LANG_USING_SCRIPT_LIB_NONE);

        PSendSysMessage(LANG_USING_WORLD_DB,sWorld.GetDBVersion());
        PSendSysMessage(LANG_USING_EVENT_AI,sWorld.GetCreatureEventAIVersion());

        PSendSysMessage("Map Manager Update Diff: %u ms, max: %u ms", sWorld.m_mapManagerUpdateTime, sWorld.m_mapManagerMaxUpdateTime);

        if (sWorld.mtmapsEnable())
            PSendSysMessage("mtmaps is ACTIVATED. Number of threads: %u", sWorld.getConfig(CONFIG_UINT32_NUMTHREADS));
        else
            PSendSysMessage("mtmaps is DEACTIVATED");
    }

    if (GetAccessLevel() > SEC_CURATOR)
    {
        PSendSysMessage(LANG_CONNECTED_USERS, sWorld.GetActiveSessionCount(), sWorld.GetMaxActiveSessionCount(), sWorld.GetQueuedSessionCount(), sWorld.GetMaxQueuedSessionCount());

        PSendSysMessage("World Update Diff: %u ms, max: %u ms", sWorld.GetUpdateTime(), sWorld.GetMaxUpdateTime());

        PSendSysMessage("Statistics counting: max spell handle time %u ms", sStatMgr.spell_work.first);
        PSendSysMessage("Statistics counting: max spell handle time spell id %u", sStatMgr.spell_work.second);
        //calculate total memory reserved by all SQL storages
        PSendSysMessage("sObjectMgr memory usage: %u mb", sObjectMgr.GetMemoryUsage() / 1024 / 1024);
        //todo: copy patch from stable
        //PSendSysMessage("VMAP memory usage: %u mb", VMAP::VMapFactory::createOrGetVMapManager()->getMemUsage() / 1024 / 1024);
        PSendSysMessage("Grid's map size: %u mb", sMapMgr.GetTotalGridsSize() / 1024 / 1024);
    }

    PSendSysMessage(LANG_UPTIME, secsToTimeString(sWorld.GetUptime()).c_str());
    PSendSysMessage("Average players latency: %u ms", sWorld.GetAvgLatency());

#ifdef SQL_STAT_COLLECT 
    if (GetAccessLevel() > SEC_CURATOR)
    {
        uint32 queryPerSecond[3];
        queryPerSecond[0] = CharacterDatabase.getQueryPerSecond();
        queryPerSecond[1] = WorldDatabase.getQueryPerSecond();
        queryPerSecond[2] = LoginDatabase.getQueryPerSecond();

        uint32 queryAvgLifetime[3];
        queryAvgLifetime[0] = CharacterDatabase.getQueryAvgLifetime();
        queryAvgLifetime[1] = WorldDatabase.getQueryAvgLifetime();
        queryAvgLifetime[2] = LoginDatabase.getQueryAvgLifetime();

        int32 longQueryes[3];
        longQueryes[0] = CharacterDatabase.getLongQueryes();
        longQueryes[1] = WorldDatabase.getLongQueryes();
        longQueryes[2] = LoginDatabase.getLongQueryes();

        uint32 fetchAvgTime[3];
        fetchAvgTime[0] = CharacterDatabase.getFetchAvgTime(); 
        fetchAvgTime[1] = WorldDatabase.getFetchAvgTime();
        fetchAvgTime[2] = LoginDatabase.getFetchAvgTime();

        uint32 fetchPerSecond[3];
        fetchPerSecond[0] = CharacterDatabase.getFetchPerSecond();
        fetchPerSecond[1] = WorldDatabase.getFetchPerSecond();
        fetchPerSecond[2] = LoginDatabase.getFetchPerSecond();

        uint32 realQueryAvgPerformTime[3];
        realQueryAvgPerformTime[0] = CharacterDatabase.getRealQueryAvgPerformTime();
        realQueryAvgPerformTime[1] = WorldDatabase.getRealQueryAvgPerformTime();
        realQueryAvgPerformTime[2] = LoginDatabase.getRealQueryAvgPerformTime();

        PSendSysMessage("DB statistics: ");
        PSendSysMessage("  Query per second %u (%u, %u, %u)", queryPerSecond[0] + queryPerSecond[1] + queryPerSecond[2], queryPerSecond[0], queryPerSecond[1], queryPerSecond[2]);   
        PSendSysMessage("  Query average lifetime %u (%u, %u, %u)", queryAvgLifetime[0] + queryAvgLifetime[1] + queryAvgLifetime[2], queryAvgLifetime[0], queryAvgLifetime[1], queryAvgLifetime[2]);
        PSendSysMessage("  Long queryes %i (%i, %i, %i)", longQueryes[0] + longQueryes[1] + longQueryes[2], longQueryes[0], longQueryes[1], longQueryes[2]);
        PSendSysMessage("  Fetch average time %u (%u, %u, %u)", fetchAvgTime[0] + fetchAvgTime[1] + fetchAvgTime[2], fetchAvgTime[0], fetchAvgTime[1], fetchAvgTime[2]);
        PSendSysMessage("  Fetches per second %u (%u, %u, %u)", fetchPerSecond[0] + fetchPerSecond[1] + fetchPerSecond[2], fetchPerSecond[0], fetchPerSecond[1], fetchPerSecond[2]);
        PSendSysMessage("  mysql_query average time %u (%u, %u, %u)", realQueryAvgPerformTime[0] + realQueryAvgPerformTime[1] + realQueryAvgPerformTime[2], realQueryAvgPerformTime[0], realQueryAvgPerformTime[1], realQueryAvgPerformTime[2]);
    }
#endif

    return true;
}

bool ChatHandler::HandleDismountCommand(char* /*args*/)
{
    //If player is not mounted, so go out :)
    if (!m_session->GetPlayer( )->IsMounted())
    {
        SendSysMessage(LANG_CHAR_NON_MOUNTED);
        SetSentErrorMessage(true);
        return false;
    }

    if(m_session->GetPlayer( )->IsTaxiFlying())
    {
        SendSysMessage(LANG_YOU_IN_FLIGHT);
        SetSentErrorMessage(true);
        return false;
    }

    m_session->GetPlayer()->Unmount();
    m_session->GetPlayer()->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
    return true;
}

bool ChatHandler::HandleSaveCommand(char* /*args*/)
{
    Player *player=m_session->GetPlayer();

    // save GM account without delay and output message (testing, etc)
    if(GetAccessLevel() > SEC_CURATOR)
    {
        player->SaveToDB();
        SendSysMessage(LANG_PLAYER_SAVED);
        return true;
    }

    // save or plan save after 20 sec (logout delay) if current next save time more this value and _not_ output any messages to prevent cheat planning
    uint32 save_interval = sWorld.getConfig(CONFIG_UINT32_INTERVAL_SAVE);
    if (save_interval==0 || (save_interval > 20*IN_MILLISECONDS && player->GetSaveTimer() <= save_interval - 20*IN_MILLISECONDS))
        player->SaveToDB();

    return true;
}

bool ChatHandler::HandleGMListIngameCommand(char* /*args*/)
{
    std::list< std::pair<std::string, bool> > names;

    {
        HashMapHolder<Player>::ReadGuard g(HashMapHolder<Player>::GetLock());
        HashMapHolder<Player>::MapType &m = sObjectAccessor.GetPlayers();
        for (HashMapHolder<Player>::MapType::const_iterator itr = m.begin(); itr != m.end(); ++itr)
        {
            AccountTypes itr_sec = itr->second->GetSession()->GetSecurity();
            if ((itr->second->isGameMaster() || (itr_sec > SEC_PLAYER && itr_sec <= (AccountTypes)sWorld.getConfig(CONFIG_UINT32_GM_LEVEL_IN_GM_LIST))) &&
                (!m_session || itr->second->IsVisibleGloballyFor(m_session->GetPlayer())))
                names.push_back(std::make_pair<std::string, bool>(GetNameLink(itr->second), itr->second->isAcceptWhispers()));
        }
    }

    if (!names.empty())
    {
        SendSysMessage(LANG_GMS_ON_SRV);

        char const* accepts = GetMangosString(LANG_GM_ACCEPTS_WHISPER);
        char const* not_accept = GetMangosString(LANG_GM_NO_WHISPER);
        for(std::list<std::pair< std::string, bool> >::const_iterator iter = names.begin(); iter != names.end(); ++iter)
            PSendSysMessage("%s - %s", iter->first.c_str(), iter->second ? accepts : not_accept);
    }
    else
        SendSysMessage(LANG_GMS_NOT_LOGGED);

    return true;
}

bool ChatHandler::HandleAccountPasswordCommand(char* args)
{
    // allow use from RA, but not from console (not have associated account id)
    if (!GetAccountId())
    {
        SendSysMessage (LANG_RA_ONLY_COMMAND);
        SetSentErrorMessage (true);
        return false;
    }

    // allow or quoted string with possible spaces or literal without spaces
    char *old_pass = ExtractQuotedOrLiteralArg(&args);
    char *new_pass = ExtractQuotedOrLiteralArg(&args);
    char *new_pass_c = ExtractQuotedOrLiteralArg(&args);

    if (!old_pass || !new_pass || !new_pass_c)
        return false;

    std::string password_old = old_pass;
    std::string password_new = new_pass;
    std::string password_new_c = new_pass_c;

    if (password_new != password_new_c)
    {
        SendSysMessage (LANG_NEW_PASSWORDS_NOT_MATCH);
        SetSentErrorMessage (true);
        return false;
    }

    if (!sAccountMgr.CheckPassword (GetAccountId(), password_old))
    {
        SendSysMessage (LANG_COMMAND_WRONGOLDPASSWORD);
        SetSentErrorMessage (true);
        return false;
    }

    AccountOpResult result = sAccountMgr.ChangePassword(GetAccountId(), password_new);

    switch(result)
    {
        case AOR_OK:
            SendSysMessage(LANG_COMMAND_PASSWORD);
            break;
        case AOR_PASS_TOO_LONG:
            SendSysMessage(LANG_PASSWORD_TOO_LONG);
            SetSentErrorMessage(true);
            return false;
        case AOR_NAME_NOT_EXIST:                            // not possible case, don't want get account name for output
        default:
            SendSysMessage(LANG_COMMAND_NOTCHANGEPASSWORD);
            SetSentErrorMessage(true);
            return false;
    }

    // OK, but avoid normal report for hide passwords, but log use command for anyone
    LogCommand(".account password *** *** ***");
    SetSentErrorMessage(true);
    return false;
}

bool ChatHandler::HandleAccountLockCommand(char* args)
{
    // allow use from RA, but not from console (not have associated account id)
    if (!GetAccountId())
    {
        SendSysMessage (LANG_RA_ONLY_COMMAND);
        SetSentErrorMessage (true);
        return false;
    }

    bool value;
    if (!ExtractOnOff(&args, value))
    {
        SendSysMessage(LANG_USE_BOL);
        SetSentErrorMessage(true);
        return false;
    }

    if (value)
    {
        LoginDatabase.PExecute( "UPDATE account SET locked = '1' WHERE id = '%u'", GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKLOCKED);
    }
    else
    {
        LoginDatabase.PExecute( "UPDATE account SET locked = '0' WHERE id = '%u'", GetAccountId());
        PSendSysMessage(LANG_COMMAND_ACCLOCKUNLOCKED);
    }

    return true;
}

/// Display the 'Message of the day' for the realm
bool ChatHandler::HandleServerMotdCommand(char* /*args*/)
{
    PSendSysMessage(LANG_MOTD_CURRENT, sWorld.GetMotd());
    return true;
}

bool ChatHandler::HandleCharDisplayMainhandCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        if (!m_session->IsPremiumActive())
        {
            PSendSysMessage(LANG_ONLY_PREMIUM);
            SetSentErrorMessage(true);
            return false;
        }
        else if (m_session->GetPremiumType() < PREMIUM_EXTENDED)
        {
            PSendSysMessage(LANG_ONLY_PREMIUM_EXTENDED);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_16_ENTRYID, newItem, 15))
    {
        pl->m_vis->m_visMainhand = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayHeadCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !m_session->IsPremiumActive() && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        PSendSysMessage(LANG_ONLY_PREMIUM);
        SetSentErrorMessage(true);
        return false;
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_1_ENTRYID, newItem, 0))
    {
        pl->m_vis->m_visHead = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayShouldersCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !m_session->IsPremiumActive() && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        PSendSysMessage(LANG_ONLY_PREMIUM);
        SetSentErrorMessage(true);
        return false;
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_3_ENTRYID, newItem, 2))
    {
        pl->m_vis->m_visShoulders = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayChestCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        if (!m_session->IsPremiumActive())
        {
            PSendSysMessage(LANG_ONLY_PREMIUM);
            SetSentErrorMessage(true);
            return false;
        }
        else if (m_session->GetPremiumType() < PREMIUM_EXTENDED)
        {
            PSendSysMessage(LANG_ONLY_PREMIUM_EXTENDED);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_5_ENTRYID, newItem, 4))
    {
        pl->m_vis->m_visChest = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayWaistCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !m_session->IsPremiumActive() && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        PSendSysMessage(LANG_ONLY_PREMIUM);
        SetSentErrorMessage(true);
        return false;
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_6_ENTRYID, newItem, 5))
    {
        pl->m_vis->m_visWaist = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayLegsCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        if (!m_session->IsPremiumActive())
        {
            PSendSysMessage(LANG_ONLY_PREMIUM);
            SetSentErrorMessage(true);
            return false;
        }
        else if (m_session->GetPremiumType() < PREMIUM_EXTENDED)
        {
            PSendSysMessage(LANG_ONLY_PREMIUM_EXTENDED);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_7_ENTRYID, newItem, 6))
    {
        pl->m_vis->m_visLegs = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayFeetCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !m_session->IsPremiumActive() && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        PSendSysMessage(LANG_ONLY_PREMIUM);
        SetSentErrorMessage(true);
        return false;
    }
    
    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_8_ENTRYID, newItem, 7))
    {
        pl->m_vis->m_visFeet = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayWristsCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        if (!m_session->IsPremiumActive())
        {
            PSendSysMessage(LANG_ONLY_PREMIUM);
            SetSentErrorMessage(true);
            return false;
        }
        else if (m_session->GetPremiumType() < PREMIUM_EXTENDED)
        {
            PSendSysMessage(LANG_ONLY_PREMIUM_EXTENDED);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_9_ENTRYID, newItem, 8))
    {
        pl->m_vis->m_visWrists = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayHandsCommand(char* args) //Суки, покажите свои руки!
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !m_session->IsPremiumActive() && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        PSendSysMessage(LANG_ONLY_PREMIUM);
        SetSentErrorMessage(true);
        return false;
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_10_ENTRYID, newItem, 9))
    {
        pl->m_vis->m_visHands = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayBackCommand(char* args)
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        if (!m_session->IsPremiumActive())
        {
            PSendSysMessage(LANG_ONLY_PREMIUM);
            SetSentErrorMessage(true);
            return false;
        }
        else if (m_session->GetPremiumType() < PREMIUM_EXTENDED)
        {
            PSendSysMessage(LANG_ONLY_PREMIUM_EXTENDED);
            return false;
        }
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_15_ENTRYID, newItem, 14))
    {
        pl->m_vis->m_visBack = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayOffhandCommand(char* args) //Суки, покажите свои руки!
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        if (!m_session->IsPremiumActive())
        {
            PSendSysMessage(LANG_ONLY_PREMIUM);
            SetSentErrorMessage(true);
            return false;
        }
        else if (m_session->GetPremiumType() < PREMIUM_EXTENDED)
        {
            PSendSysMessage(LANG_ONLY_PREMIUM_EXTENDED);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_17_ENTRYID, newItem, 16))
    {
        pl->m_vis->m_visOffhand = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleCharDisplayRangedCommand(char* args) //Суки, покажите свои руки!
{
    if (m_session->GetSecurity() < SEC_MODERATOR && !sWorld.getConfig(CONFIG_BOOL_CHARACTER_VISUALS_OVERRIDE))
    {
        if (!m_session->IsPremiumActive())
        {
            PSendSysMessage(LANG_ONLY_PREMIUM);
            SetSentErrorMessage(true);
            return false;
        }
        else if (m_session->GetPremiumType() < PREMIUM_EXTENDED)
        {
            PSendSysMessage(LANG_ONLY_PREMIUM_EXTENDED);
            SetSentErrorMessage(true);
            return false;
        }
    }

    if(!*args)
        return false;
    char* cId = ExtractKeyFromLink(&args,"Hitem");
    if(!cId)
        return false;
            
    uint32 newItem = (uint32)atol(cId);

    Player* pl = m_session->GetPlayer();

    if(!pl->m_vis)
        pl->m_vis = new Visuals;

    if (pl->HandleChangeSlotModel(PLAYER_VISIBLE_ITEM_18_ENTRYID, newItem, 17))
    {
        pl->m_vis->m_visRanged = newItem;
        pl->m_vis->m_type = VISUALS_TYPE_PREMIUM_ACC;
        return true;
    }
    else
        return false;
}

bool ChatHandler::HandleVoteMuteCommand(char* args)
{
    if (sStatMgr.to_mute_acct)
    {
        SendSysMessage("Voting player is already in progress...");
        SetSentErrorMessage(true);
        return false;
    }

    if (!*args)
        return false;

    std::string name = ExtractPlayerNameFromLink(&args);
    if (name.empty())
        return false;

    Player* pl = sObjectMgr.GetPlayer(name.c_str());
    if (!pl)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (pl->GetSession() == m_session)
    {
        SendSysMessage("Can't mute yourself.");
        SetSentErrorMessage(true);
        return false;
    }

    // Can't mute opposite faction
    if (pl->GetTeam() != m_session->GetPlayer()->GetTeam())
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        SetSentErrorMessage(true);
        return false;
    }

    if (pl->GetSession()->GetSecurity() > SEC_PLAYER && m_session->GetSecurity() <= pl->GetSession()->GetSecurity())
    {
        SendSysMessage(LANG_YOURS_SECURITY_IS_LOW);
        SetSentErrorMessage(true);
        return false;
    }

    if (!pl->CanSpeak())
    {
        SendSysMessage("Already muted.");
        SetSentErrorMessage(true);
        return false;
    }

    sStatMgr.to_mute_acct = pl->GetSession()->GetAccountId();
    sStatMgr.mute_counter = time(NULL) + 60;
    sStatMgr.mute_votes[m_session->GetAccountId()] = true;
    sStatMgr.mute_chat_team = pl->GetTeam();

    std::stringstream argstr;
    argstr << m_session->GetPlayer()->GetName() << " called a vote to mute player " << pl->GetName() << "\n";
    argstr << "Print \".vote yes\" to agree, \".vote no\" to disagree in next 60 seconds.";
    sWorld.SendTeamText(sStatMgr.mute_chat_team, LANG_SYSTEMMESSAGE, argstr.str().c_str());
    return true;
}

bool ChatHandler::HandleVoteYesCommand(char* args)
{
    if(!sStatMgr.to_mute_acct)
    {
        SendSysMessage("Voting is not in progress...");
        SetSentErrorMessage(true);
        return false;
    }

    if(m_session->GetAccountId() == sStatMgr.to_mute_acct ||  //do not allow called player to vote himself
        sStatMgr.mute_votes.find(m_session->GetAccountId()) != sStatMgr.mute_votes.end())
        return true;

    // Can't vote yes opposite faction
    if (sStatMgr.mute_chat_team != m_session->GetPlayer()->GetTeam())
    {
        SendSysMessage("Voting is not in progress...");
        SetSentErrorMessage(true);
        return false;
    }

    sStatMgr.mute_votes[m_session->GetAccountId()] = true;

    std::stringstream argstr;
    argstr << m_session->GetPlayer()->GetName() << " voted yes.";

    WorldPacket data(SMSG_NOTIFICATION, (argstr.str().size()+1));
    data << argstr.str();
    sWorld.SendGlobalMessage(&data, NULL, sStatMgr.mute_chat_team);
    return true;
}

bool ChatHandler::HandleVoteNoCommand(char* args)
{
    if(!sStatMgr.to_mute_acct)
    {
        SendSysMessage("Voting is not in progress...");
        SetSentErrorMessage(true);
        return false;
    }

    if(m_session->GetAccountId() == sStatMgr.to_mute_acct ||  //do not allow called player to vote himself
        sStatMgr.mute_votes.find(m_session->GetAccountId()) != sStatMgr.mute_votes.end())
        return true;

    // Can't vote no opposite faction
    if (sStatMgr.mute_chat_team != m_session->GetPlayer()->GetTeam())
    {
        SendSysMessage("Voting is not in progress...");
        SetSentErrorMessage(true);
        return false;
    }

    sStatMgr.mute_votes[m_session->GetAccountId()] = false;

    std::stringstream argstr;
    argstr << m_session->GetPlayer()->GetName() << " voted no.";

    WorldPacket data(SMSG_NOTIFICATION, (argstr.str().size()+1));
    data << argstr.str();
    sWorld.SendGlobalMessage(&data, NULL, sStatMgr.mute_chat_team);
    return true;
}

bool ChatHandler::HandleGetFromBackupCommand(char* args)
{
    return true;
   /* //acct, lvl, questsComp, flightPath, spells, deletedSpells, gold, name, race, class
    Player *player=m_session->GetPlayer();
    if (!player)
      return true;

    QueryResult* backup = CharacterDatabase.PQuery("SELECT acct, level, finished_quests, spells, deleted_spells, gold, guid, restored, banned, ready_to_restore, skills FROM charactersBckp WHERE name = '%s' and race = '%u' and class = '%u'", player->GetName(), player->getRace(), player->getClass());
    if (backup)
    {
        Field* backupFld = backup->Fetch();
        uint64 bAccId = backupFld[0].GetUInt64();
        uint32 bLvl = backupFld[1].GetUInt32();
        std::string bFinQuests = backupFld[2].GetCppString();
        std::string bSpells = backupFld[3].GetCppString();
        std::string bDelSpells = backupFld[4].GetCppString();
        uint32 bGold = backupFld[5].GetUInt32();
        uint64 bGuid = backupFld[6].GetUInt64();
        uint8 bRestored = backupFld[7].GetUInt8();
        uint8 bBanned = backupFld[8].GetUInt8();
        uint8 bReady = backupFld[9].GetUInt8();
        std::string bSkills = backupFld[10].GetCppString();

        if (bRestored)
        {
            PSendSysMessage("Персонаж уже был восстановлен.");
            delete backup;
            return true;
        }

        if (bBanned)
        {
            PSendSysMessage("Не пытайтесь восстановить забаненных персонажей.");
            delete backup;
            return true;
        }

        if (!bReady)
        {
            PSendSysMessage("Персонаж не синхронизирован. Посетите наш сайт для получения необходимой информации http://wow-russian.ru");
            delete backup;
            return true;
        }

        if (bAccId != m_session->GetAccountId())
        {
            PSendSysMessage("Персонаж не принадлежит вам.");
            delete backup;
            return true;
        }

        if (player->getLevel() < bLvl)
            HandleCharacterLevel(player, player->GetGUID(), player->getLevel(), bLvl);

        char* end;
        char* start = (char*)bFinQuests.c_str();

      /*  while(true) {
            end = strchr(start,',');
            if(!end)
                break;
            *end=0;

            player->SetQuestStatus(atol(start), QUEST_STATUS_COMPLETE);

            start = end + 1;
        }*//*

        start = (char*)bSpells.c_str();

        while(true) {
            end = strchr(start,',');
            if(!end)
                break;
            *end=0;

            player->learnSpell(atol(start),false);

            start = end + 1;
        }

        start = (char*)bDelSpells.c_str();

        while(true) {
            end = strchr(start,',');
            if(!end)
                break;
            *end=0;

            player->removeSpell(atol(start), false, false);

            start = end + 1;
        }

        start = (char*)bSkills.c_str();
        uint32 skillLine, skillCurrVal, skillMaxVal;

        while(true) {
            end = strchr(start,';');
            if(!end)
                break;
            *end=0;

            skillLine = atol(start);

            start = end + 1;

            end = strchr(start,';');
            if(!end)
                break;
            *end=0;

            skillCurrVal = atol(start);

            start = end + 1;

            end = strchr(start,';');
            if(!end)
                break;
            *end=0;

            skillMaxVal = atol(start);

            start = end + 1;

            player->SetSkill(skillLine, skillCurrVal, skillMaxVal);
        }

        player->SetMoney(bGold);

        QueryResult* bItems = CharacterDatabase.PQuery("SELECT entry, count FROM backupPlrItems where ownerguid = '%u'", bGuid);
        if (bItems)
        {
            /*for (uint32 i = 0;i<65535;++i)
            {
              player->DestroyItem(i >> 8, i & 0xFF, false);
            }*//*
            do
            {
                Field* itemsFld = bItems->Fetch();
                uint32 bItmEntry = itemsFld[0].GetUInt32();
                uint32 bItmCnt = itemsFld[1].GetUInt32();

                Item* item = Item::CreateItem(bItmEntry, bItmCnt, player);

                if (!item)
                    continue;

                // fill mail
                std::string subject = player->GetSession()->GetMangosString(LANG_NOT_EQUIPPED_ITEM);
                std::string body = "Present from fallen World";

                uint32 itemTextId = sObjectMgr.CreateItemText(body);

                MailDraft mi(subject, itemTextId);

                CharacterDatabase.BeginTransaction();
                item->SaveToDB();
                CharacterDatabase.CommitTransaction();
                
                mi.AddItem(item);

                mi.SendMailTo(MailReceiver(player), MailSender(player, MAIL_STATIONERY_GM));

            } while( bItems->NextRow());

            delete bItems;
        }

        delete backup;

        player->resetTalents(true);
        player->SendTalentsInfoData(false);

        CharacterDatabase.PExecute("UPDATE charactersBckp SET restored = 1 WHERE name = '%s' and race = '%u' and class = '%u'", player->GetName(), player->getRace(), player->getClass());

        PSendSysMessage("Персонаж восстановлен. Удачной игры");

    } else {
        PSendSysMessage("Персонаж с таким именем, классом и расой не найден в бэкапе");
        return true;
    }
    return true;*/
}

bool ChatHandler::HandleDkStartCommand(char* args)
{
    if (!m_session)
        return true;

    Player* pl = m_session->GetPlayer();

    if (pl->getClass() != CLASS_DEATH_KNIGHT)
    {
        PSendSysMessage("You are not Death Knight.");
        return true;
    }

    uint32 talcount = pl->CalculateTalentsPoints();
    NumTalentsAtLevelEntry const* entry = sNumTalentsAtLevelStore.LookupEntry(pl->getLevel());
    if (!entry)
    {
        PSendSysMessage("Internal error.");
        return true;
    }

    if (talcount == entry->Talents)
    {
        PSendSysMessage("All quests are already completed.");
        return true;
    }

    if (pl->isInCombat())
    {
        PSendSysMessage("You are in combat!");
        return true;
    }

    if (pl->HasSpellCooldown(8690))
    {
        PSendSysMessage("You can't do that yet!");
        return true;
    }

    pl->AddSpellCooldown(8690, 0, time(NULL) + 30 * MINUTE);
    pl->TeleportTo(609, 2353.788f, -5665.562f, 427.026f, 0.622f);

    return true;
}

bool ChatHandler::HandleCharacterCheckDkQuestsCommand(char* args)
{
    if (!m_session)
        return true;

    Player* player = m_session->GetPlayer();
    Player* target = getSelectedPlayer();
    if (m_session->GetSecurity() <= SEC_CURATOR)
        target = player;

    if (!target)
    {
        SendSysMessage(LANG_PLAYER_NOT_FOUND);
        return false;
    }

    std::string playerlink = playerLink(target->GetName());

    if (target->getClass() != CLASS_DEATH_KNIGHT)
    {
        PSendSysMessage(LANG_DKQ_COMMAND_NOT_DK, playerlink.c_str());
        return false;
    }

    uint32 quests[45][2] =
    {
        {12593, 12593},     // In Service Of The Lich King
        {12619, 12619},     // The Emblazoned Runeblade
        {12842, 12842},     // Runeforging: Preparation For Battle
        {12848, 12848},     // The Endless Hunger

        {12636, 12636},     // The Eye Of Acherus
        {12641, 12641},     // Death Comes From On High
        {12657, 12657},     // The Might Of The Scourge
        {12850, 12850},     // Report To Scourge Commander Thalanor

        {12849, 12849},     // The Power Of Blood, Frost And Unholy

        {12670, 12670},     // The Scarlet Harvest
        {12678, 12678},     // If Chaos Drives, Let Suffering Hold The Reins
        {12697, 12697},     // Gothik the Harvester
        {12698, 12698},     // The Gift That Keeps On Giving
        {12700, 12700},     // An Attack Of Opportunity
        {12701, 12701},     // Massacre At Light's Point
        {12680, 12680},     // Grand Theft Palomino
        {12687, 12687},     // Into the Realm of Shadows
        {12679, 12679},     // Tonight We Dine In Havenshire
        {12733, 12733},     // Death's Challenge
        {12706, 12706},     // Victory At Death's Breach!
        {12714, 12714},     // The Will Of The Lich King

        {12715, 12715},     // The Crypt of Remembrance
        {12711, 12711},     // Abandoned Mail
        {12716, 12716},     // The Plaguebringer's Request
        {12719, 12719},     // Nowhere To Run And Nowhere To Hide
        {12720, 12720},     // How To Win Friends And Influence Enemies
        {12722, 12722},     // Lambs To The Slaughter

        {12723, 12723},     // Behind Scarlet Lines
        {12724, 12724},     // The Path Of The Righteous Crusader
        {12725, 12725},     // Brothers In Death
        {12727, 12727},     // Bloody Breakout
        {12738, 12738},     // A Cry For Vengeance!
        /*
        12739 - [A Special Surprise] tauren
        12742 - [A Special Surprise] human
        12743 - [A Special Surprise] night elf
        12744 - [A Special Surprise] dwarf
        12745 - [A Special Surprise] gnome
        12746 - [A Special Surprise] draenei
        12747 - [A Special Surprise] blood elf
        12748 - [A Special Surprise] orc
        12749 - [A Special Surprise] troll
        12750 - [A Special Surprise] undead
        */
        {0, 0},             // A Special Surprise
        {12751, 12751},     // A Sort Of Homecoming
        {12754, 12754},     // Ambush At The Overlook
        {12755, 12755},     // A Meeting With Fate
        {12756, 12756},     // The Scarlet Onslaught Emerges

        {12757, 12757},     // Scarlet Armies Approach...
        {12778, 12778},     // The Scarlet Apocalypse
        {12779, 12779},     // An End To All Things...
        {12800, 12800},     // The Lich King's Command
        {12801, 12801},     // The Light of Dawn
        {13165, 13165},     // Taking Back Acherus
        {13166, 13166},     // The Battle For The Ebon Hold
        {13188, 13189}      // Where Kings Walk; Warchief's Blessing
    };

    uint8 teamIndex = target->GetTeam() == ALLIANCE ? 0 : 1;

    uint8 count = 0;
    uint8 loc_idx = GetSessionDbLocaleIndex();

    PSendSysMessage(LANG_DKQ_COMMAND_QUEST_LIST, playerlink.c_str());
    for (uint8 i = 0; i < 45; ++i)
    {
        if (quests[i][teamIndex] == 0)
        {
            switch(target->getRace())
            {
                case RACE_TAUREN:   quests[i][teamIndex] = 12739; break;
                case RACE_HUMAN:    quests[i][teamIndex] = 12742; break;
                case RACE_NIGHTELF: quests[i][teamIndex] = 12743; break;
                case RACE_DWARF:    quests[i][teamIndex] = 12744; break;
                case RACE_GNOME:    quests[i][teamIndex] = 12745; break;
                case RACE_DRAENEI:  quests[i][teamIndex] = 12746; break;
                case RACE_BLOODELF: quests[i][teamIndex] = 12747; break;
                case RACE_ORC:      quests[i][teamIndex] = 12748; break;
                case RACE_TROLL:    quests[i][teamIndex] = 12749; break;
                case RACE_UNDEAD:   quests[i][teamIndex] = 12750; break;
            }
        }

        Quest const* qinfo = sObjectMgr.GetQuestTemplate(quests[i][teamIndex]);
        if (!qinfo)
            continue;

        QuestStatus status = target->GetQuestStatus(qinfo->GetQuestId());
        bool rewarded = target->GetQuestRewardStatus(qinfo->GetQuestId());
        if (status == QUEST_STATUS_COMPLETE && rewarded)
            continue;

        ++count;
        std::string title;
        if (QuestLocale const *il = sObjectMgr.GetQuestLocale(qinfo->GetQuestId()))
        {
            if (loc_idx >= il->Title.size())
                title = il->Title[0];
            else
                title = il->Title[loc_idx];
        }

        if (title.empty())
            title = qinfo->GetTitle();

        if (status == QUEST_STATUS_COMPLETE && !rewarded)
            PSendSysMessage(LANG_DKQ_COMMAND_QUEST, qinfo->GetQuestId(), qinfo->GetQuestId(), qinfo->GetQuestLevel(), title.c_str(), "COMPLETE");
        else if (status == QUEST_STATUS_INCOMPLETE)
            PSendSysMessage(LANG_DKQ_COMMAND_QUEST, qinfo->GetQuestId(), qinfo->GetQuestId(), qinfo->GetQuestLevel(), title.c_str(), "INCOMPLETE");
        else
            PSendSysMessage(LANG_DKQ_COMMAND_QUEST, qinfo->GetQuestId(), qinfo->GetQuestId(), qinfo->GetQuestLevel(), title.c_str(), "NOT STARTED");
    }
    if (!count)
        PSendSysMessage(LANG_DKQ_COMMAND_ALL_COMPLETED, playerlink.c_str());

    return true;
}

bool ChatHandler::HandleAllowSpectateCommand(char* args)
{
    if (!sWorld.getConfig(CONFIG_BOOL_SPECTATING_ENABLED))
    {
        SendSysMessage(LANG_SPECTATING_DISABLED);
        SetSentErrorMessage(true);
        return false;
    }

    if (m_session->GetPlayer()->ToggleAllowSpectate())
        SendSysMessage(LANG_SPECTATING_ALLOWED);
    else
        SendSysMessage(LANG_SPECTATING_FORBIDDEN);

    return true;
}

bool ChatHandler::HandleSpectateCommand(char* args)
{
    if (!sWorld.getConfig(CONFIG_BOOL_SPECTATING_ENABLED))
    {
        SendSysMessage(LANG_SPECTATING_DISABLED);
        SetSentErrorMessage(true);
        return false;
    }

    Player* target;
    if (!ExtractPlayerTarget(&args, &target))
        return false;

    if (!m_session->GetPlayer()->Spectate(target, true))
    {
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleUnSpectateCommand(char* args)
{
    if (!sWorld.getConfig(CONFIG_BOOL_SPECTATING_ENABLED))
    {
        SendSysMessage(LANG_SPECTATING_DISABLED);
        SetSentErrorMessage(true);
        return false;
    }

    if (!m_session->GetPlayer()->UnSpectate(true))
    {
        SetSentErrorMessage(true);
        return false;
    }

    return true;
}

bool ChatHandler::HandleSetPreferedBGCommand(char* args)
{
    if (!*args)
        return false;

    uint32 typeId;
    if (!ExtractUInt32(&args, typeId))
        return false;

    switch(typeId)
    {
        case 0:
            typeId = BATTLEGROUND_RB;
            SendSysMessage(LANG_PREFERED_BG_SET_RB);
            break;
        case 1:
            typeId = BATTLEGROUND_WS;
            SendSysMessage(LANG_PREFERED_BG_SET_WS);
            break;
        case 2:
            typeId = BATTLEGROUND_AB;
            SendSysMessage(LANG_PREFERED_BG_SET_AB);
            break;
        case 3:
            typeId = BATTLEGROUND_EY;
            SendSysMessage(LANG_PREFERED_BG_SET_EY);
            break;
        case 4:
            typeId = BATTLEGROUND_AV;
            SendSysMessage(LANG_PREFERED_BG_SET_AV);
            break;
        case 5:
            typeId = BATTLEGROUND_SA;
            SendSysMessage(LANG_PREFERED_BG_SET_SA);
            break;
        case 6:
            //typeId = BATTLEGROUND_IC;
            //SendSysMessage(LANG_PREFERED_BG_SET_IC);
            //break;
        default:
            PSendSysMessage(LANG_PREFERED_BG_WRONG, typeId);
            SetSentErrorMessage(true);
            return false;
    }

    m_session->GetPlayer()->SetPreferedBG(uint8(typeId));
    return true;
}

bool ChatHandler::HandleLeaveVehicleCommand(char* args)
{
    Player* plr = m_session->GetPlayer();

    if (!plr->GetVehicle())
        PSendSysMessage(LANG_NOT_ON_VEHICLE);
    else
    {
        PSendSysMessage(LANG_VEHICLE_DISMOUNTED);
        plr->ExitVehicle();
    }

    return true;
}
bool ChatHandler::HandleMmrShowCommand(char* args)
{
    Player* plr = m_session->GetPlayer();

    for (uint8 i = 0; i < MAX_ARENA_SLOT; ++i)
    {
        uint32 mmr = 1500;

        bool checkDb = true;
        if (uint32 teamId = plr->GetArenaTeamId(i))
        {
            if (ArenaTeam* team = sObjectMgr.GetArenaTeamById(teamId))
            {
                if (ArenaTeamMember* member = team->GetMember(plr->GetObjectGuid()))
                {
                    mmr = member->MatchMakerRating;
                    checkDb = false;
                }
            }
        }

        if (checkDb)
        {
            if (QueryResult* result = CharacterDatabase.PQuery("SELECT `mmr` FROM `character_mmr` WHERE `guid` = '%u' AND `slot` = '%u'",
                plr->GetObjectGuid().GetCounter(), i))
                mmr = (*result)[0].GetUInt32();
        }

        uint8 type = i == 0 ? ARENA_TYPE_2v2 : (i == 1 ? ARENA_TYPE_3v3 : ARENA_TYPE_5v5);
        PSendSysMessage(LANG_YOUR_NXN_MMR, type, type, mmr);
    }

    return true;
}

bool ChatHandler::HandleMmrResetCommand(char* args)
{
    if (!*args)
        return false;

    uint32 type;
    if (!ExtractUInt32(&args, type))
        return false;

    uint8 slot = 0;
    switch(type)
    {
        case 2: slot = 0; break;
        case 3: slot = 1; break;
        case 5: slot = 2; break;
        default:
            PSendSysMessage(LANG_WRONG_ARENA_TYPE);
            SetSentErrorMessage(true);
            return false;
    }

    Player* plr = m_session->GetPlayer();
    if (plr->GetArenaTeamId(slot))
    {
        PSendSysMessage(LANG_MUST_LEAVE_ARENA_TYPE, type, type);
        SetSentErrorMessage(true);
        return false;
    }

    CharacterDatabase.PExecute("DELETE FROM `character_mmr` WHERE `guid` = '%u' AND `slot` = '%u'",
        plr->GetObjectGuid().GetCounter(), slot);

    PSendSysMessage(LANG_MMR_RESET_FOR_TYPE, type, type);

    return true;
}

bool ChatHandler::HandleQuestCompleteBuggedCommand(char* args)
{
    BuggedQuestsMap& buggedQuests = sObjectMgr.GetBuggedQuests();

    Player* player = m_session->GetPlayer();

    uint8 loc_idx = GetSessionDbLocaleIndex();

    for (BuggedQuestsMap::iterator itr = buggedQuests.begin(); itr != buggedQuests.end(); ++itr)
    {
        uint32 entry = itr->first;
        Quest const* pQuest = sObjectMgr.GetQuestTemplate(itr->first);

        // If player doesn't have the quest
        if (!pQuest || player->GetQuestStatus(entry) == QUEST_STATUS_NONE || player->GetQuestRewardStatus(entry))
            continue;

        // Add quest items for quests that require items
        for(uint8 x = 0; x < QUEST_ITEM_OBJECTIVES_COUNT; ++x)
        {
            uint32 id = pQuest->ReqItemId[x];
            uint32 count = pQuest->ReqItemCount[x];
            if (!id || !count)
                continue;

            uint32 curItemCount = player->GetItemCount(id, true);

            ItemPosCountVec dest;
            uint8 msg = player->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, id, count - curItemCount );
            if (msg == EQUIP_ERR_OK)
            {
                Item* item = player->StoreNewItem( dest, id, true);
                player->SendNewItem(item,count - curItemCount, true, false);
            }
        }

        // All creature/GO slain/casted (not required, but otherwise it will display "Creature slain 0/10")
        for (uint8 i = 0; i < QUEST_OBJECTIVES_COUNT; ++i)
        {
            int32 creature = pQuest->ReqCreatureOrGOId[i];
            uint32 creaturecount = pQuest->ReqCreatureOrGOCount[i];

            if (uint32 spell_id = pQuest->ReqSpell[i])
            {
                for(uint16 z = 0; z < creaturecount; ++z)
                    player->CastedCreatureOrGO(creature, ObjectGuid(), spell_id);
            }
            else if (creature > 0)
            {
                if (CreatureInfo const* cInfo = ObjectMgr::GetCreatureTemplate(creature))
                    for(uint16 z = 0; z < creaturecount; ++z)
                        player->KilledMonster(cInfo, ObjectGuid());
            }
            else if (creature < 0)
            {
                for(uint16 z = 0; z < creaturecount; ++z)
                    player->CastedCreatureOrGO(-creature, ObjectGuid(), 0);
            }
        }

        // If the quest requires reputation to complete
        if (uint32 repFaction = pQuest->GetRepObjectiveFaction())
        {
            uint32 repValue = pQuest->GetRepObjectiveValue();
            uint32 curRep = player->GetReputationMgr().GetReputation(repFaction);
            if (curRep < repValue)
                if (FactionEntry const *factionEntry = sFactionStore.LookupEntry(repFaction))
                    player->GetReputationMgr().SetReputation(factionEntry,repValue);
        }

        // If the quest requires money
        int32 ReqOrRewMoney = pQuest->GetRewOrReqMoney();
        if (ReqOrRewMoney < 0)
            player->ModifyMoney(-ReqOrRewMoney);

        player->CompleteQuest(entry);

        std::string title = pQuest->GetTitle();
        sObjectMgr.GetQuestLocaleStrings(entry, loc_idx, &title);
        PSendSysMessage(LANG_BUGGED_QUESTS_COMPLETED, pQuest->GetQuestId(), pQuest->GetQuestId(), pQuest->GetQuestLevel(), title.c_str());
    }

    return true;
}
