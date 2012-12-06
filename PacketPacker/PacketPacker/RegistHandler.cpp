#include "stdafx.h"

#include "NetPacket.h"
#include "User.h"
#include "Protocol.h"
#include "ServerHandler.h"

bool RegistTry(PacketHandlerData d){
	NetPacket *p = d.pkt;
	bool exist = false;
	User *user; 

	user = CreateUser();

	SET(user->id, NetGetStringData(p,"id"));
	SET(user->pw, NetGetStringData(p,"pw"));
	SET(user->nick, NetGetStringData(p,"nick"));
	user->age = NetGetNumberData(p,"age");

	exist = QueryUser(user->id, NULL);

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(exist){
		pkt->header.type = REGIST_EXIST_ID;
		NetAddStringData(pkt,"reason", "id already exist");
	}
	else{
		bool ret;
		ret = RegistUser(NetGetStringData(p,"id"),
					user);

		if(ret){
			pkt->header.type = REGIST_OK;
		}
		else{
			pkt->header.type = REGIST_DENIED;
			NetAddStringData(pkt,"reason", "unknown error");
		}
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);

	DisposeUser(user);

	return true;
}
