/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod Sample Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"

#include <netinet/in.h>
#include <steam_gameserver.h>
#include <isteamgameserver.h>
#include <vector>
#include <string>
#include <fstream>

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

Fake g_Fake;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_Fake);

ITimer* pTimer;
std::vector<std::string> tokens;

ResultType Fake::OnTimer(ITimer *pTimer, void *pData)
{
    static unsigned int count = 0;
    static int port = 27115;
    if(!SteamGameServer_Init(INADDR_ANY, port, port, eServerModeAuthentication, "1.38.2.8"))
    {
        printf("SteamGameServer_Init call failed\n");
    }
    if (SteamGameServer())
    {
        SteamGameServer()->SetProduct("730");
        SteamGameServer()->SetGameDescription("FAKE SERVER");
        SteamGameServer()->SetDedicatedServer(true);
        SteamGameServer()->SetModDir("csgo");

        SteamGameServer()->LogOn(tokens[count].c_str());

        SteamGameServer()->SetBotPlayerCount(32);
        SteamGameServer()->SetMaxPlayerCount(64);
        SteamGameServer()->SetPasswordProtected(false);
        SteamGameServer()->SetRegion("255");
        SteamGameServer()->SetServerName("FAKE SERVER");
        SteamGameServer()->SetMapName("de_dust2");
 
        SteamGameServer()->SetAdvertiseServerActive(true);
        SteamGameServer()->SetGameTags("fake_server");
    }
    else
    {
        printf("SteamGameServer() interface is invalid\n");
    }

    port++;
    count++;

    if(count >= tokens.size())
    {
        return Pl_Stop;
    }

    return Pl_Continue;
}

void Fake::OnTimerEnd(ITimer *pTimer, void *pData)
{
    pTimer = nullptr;
}

bool Fake::SDK_OnLoad(char *error, size_t maxlen, bool late)
{
    char path[PLATFORM_MAX_PATH];
	smutils->BuildPath(Path_SM, path, PLATFORM_MAX_PATH, "configs/tokens.txt");

	if (!libsys->PathExists(path))
	{
		smutils->Format(error, maxlen - 1, "File %s not found", path);
		return false;
	}

    std::string buffer;
    std::ifstream file(path);

    while (getline(file, buffer))
    {
        tokens.push_back(buffer);
    }

	file.close();

    pTimer = timersys->CreateTimer(this, 10.0, nullptr, TIMER_FLAG_REPEAT);
    
    return true;
}

void Fake::SDK_OnUnload()
{
    
}