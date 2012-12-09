#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "Message.h"

#include "Server.h"

#define SERVER_PORT 9919


int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	DbConnect();
	
	Message m;
	
	QueryMessage("anz4187", &m);

	printf("%s \n", m.msg);

	RunServer(SERVER_PORT);
	
	DbDisconnect();

	return 0;
}


