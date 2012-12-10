#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "payment.h"

#include "Server.h"

#define SERVER_PORT 9919


int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	DbConnect();
	
	RunServer(SERVER_PORT);
	
	DbDisconnect();

	return 0;
}


