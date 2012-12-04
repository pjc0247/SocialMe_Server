#include "stdafx.h"

#include "NetPacket.h"
#include "blacklist.h"
#include "database.h"
#include "User.h"

#include "Server.h"

#define SERVER_PORT 9919


int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	DbConnect();

	User *u = CreateUser();
	bool ret;

	u->age = 13;
	SET(u->nick,"Anz");
	SET(u->pw,"Anz");

	RegistUser("anz4176",u);
	

	RunServer(SERVER_PORT);
	
	DbDisconnect();

	return 0;
}


