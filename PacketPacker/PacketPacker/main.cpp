#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "payment.h"

#include "Server.h"
#include "monitor.h"

#define SERVER_PORT 9919

int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	DbConnect();
	
	StartMonitoring();
	RunServer(SERVER_PORT);
	
	EndMonitoring();

	DbDisconnect();

	return 0;
}


