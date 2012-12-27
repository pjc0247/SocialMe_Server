#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "photo.h"

#include "Server.h"
#include "monitor.h"

#include <locale.h>

#include "redis.h"

#define SERVER_PORT 9919

void GCM_Testbed();

int _tmain(int argc, _TCHAR* argv[])
{
	#ifdef _DEBUG
		_tsetlocale(LC_ALL, _T("korean"));
	#endif

		WSADATA wsaData;
    WORD version;
    version = MAKEWORD( 2, 0 );
    WSAStartup( version, &wsaData );

	RedisConnect(REDIS_SERVER,REDIS_PORT);

	while(1){
		RedisSet("asdfb", "SDF");
		RedisSet("asdfc", "SDFa");

		char v[128];
		RedisGet("asdf", v);
	}

	//	GCM_Testbed();

	LoadBlacklist();

	StartMonitoring();
	RunServer(SERVER_PORT);
	
	EndMonitoring();

	return 0;
}


