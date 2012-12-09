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
	
	SET(m.msg, "HelloMessage!");
	SET(m.sender,"pjc0247");
	SET(m.receiver,"anz4187");
	m.time = 0;
	m.type = 0;
	PushMessage(&m);

	RunServer(SERVER_PORT);
	
	DbDisconnect();

	return 0;
}


