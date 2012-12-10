#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "follow.h"

#include "Server.h"
#include "monitor.h"

#define SERVER_PORT 9919

int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	DbConnect();
	
	printf("%d \n", FollowCount("pjc0247"));
	/*Follow("pjc0247", "anz54176");
	Follow("pjc0247", "anz5123116");
	Follow("pjc0247", "anz541236");
	Follow("pjc0247", "anz5111276");
	Follow("pjc0247", "anz54171414");*/
	printf("%d\n", IsFollowing("pjc0247", "anzs541236"));


	StartMonitoring();
	RunServer(SERVER_PORT);
	
	EndMonitoring();

	DbDisconnect();

	return 0;
}


