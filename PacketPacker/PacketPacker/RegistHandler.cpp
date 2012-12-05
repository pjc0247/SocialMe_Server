#include "stdafx.h"

#include "NetPacket.h"
#include "User.h"
#include "Protocol.h"

bool RegistTry(NetPacket *p){

	bool exist = false;
	User *user; 

	user = CreateUser();

	SET(user->id, NetGetStringData(p,"id"));
	SET(user->pw, NetGetStringData(p,"pw"));
	SET(user->nick, NetGetStringData(p,"nick"));
	user->age = NetGetNumberData(p,"age");

	exist = QueryUser(user->id, NULL);
	if(exist){
		NetPacket *pkt;
		pkt = NetCreatePacket();
		pkt->header.type = REGIST_EXIST_ID;
		NetAddStringData(pkt,"reason", "id already exist");
		NetDisposePacket(pkt,true);
	}
	else{
		RegistUser(NetGetStringData(p,"id"),
					user);
	}

	DisposeUser(user);

	return true;
}
