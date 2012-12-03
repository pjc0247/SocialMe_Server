#include "stdafx.h"

#include "NetPacket.h"
#include "blacklist.h"

#include "Server.h"

#define SERVER_PORT 9919


int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	RunServer(SERVER_PORT);
	
	return 0;
}


