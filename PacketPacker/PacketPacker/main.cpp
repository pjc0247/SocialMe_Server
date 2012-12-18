#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "photo.h"

#include "Server.h"
#include "monitor.h"

#include <locale.h>

#define SERVER_PORT 9919

int _tmain(int argc, _TCHAR* argv[])
{
	#ifdef _DEBUG
		_tsetlocale(LC_ALL, _T("korean"));
	#endif

	LoadBlacklist();

	DbConnect();

	StartMonitoring();
	RunServer(SERVER_PORT);
	
	EndMonitoring();

	DbDisconnect();

	return 0;
}


