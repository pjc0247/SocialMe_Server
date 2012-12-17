#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "photo.h"

#include "Server.h"
#include "monitor.h"

#define SERVER_PORT 9919

int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	DbConnect();

	PhotoPost p;

	p.time = 0;
	SET(p.comment, "hi");
	SET(p.photo, "asdf");
	p.lat = 1234;
	p.lon = 41235;

	PushPhoto("pjc0247",&p);

	User u;

	SET(u.id,"pjc0247");
	SET(u.pw, "040404");
	SET(u.nick, "anz");
	u.age = 18;

	RegistUser("pjc0247", &u);

	PhotoPost *plist = NULL;
	
	plist = (PhotoPost*)malloc(sizeof(PhotoPost) * 5);
	int len = QueryPhotoList("pjc0247", plist, 1, 5);

	
	printf("%d\n", len);
	for(int i=0;i<len;i++){
		printf("%d | %s | %d \n", plist[i].photo_id, plist[i].comment, plist[i].lat);	
	}
	free(plist);

	StartMonitoring();
	RunServer(SERVER_PORT);
	
	EndMonitoring();

	DbDisconnect();

	return 0;
}


