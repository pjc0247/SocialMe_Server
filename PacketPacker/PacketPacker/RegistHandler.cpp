#include "stdafx.h"

#include "NetPacket.h"
#include "User.h"
#include "Protocol.h"
#include "ServerHandler.h"
#include "reason.h"

bool RegistTry(PacketHandlerData d){
	NetPacket *p = d.pkt;
	bool exist = false;
	User *user; 

	user = CreateUser();

	SET(user->id, NetGetStringData(p,"id"));
	SET(user->pw, NetGetStringData(p,"pw"));
	SET(user->nick, NetGetStringData(p,"nick"));
	user->age = NetGetNumberData(p,"age");

	exist = QueryUser(DB(d),user->id, NULL);

	NetPacket *pkt;
	pkt = NetCreatePacket();

	// 이미 존재하는 아이디일경우
	if(exist){
		pkt->header.type = REGIST_EXIST_ID;
		NetAddStringData(pkt,"reason", REASON_ID_ALREADY_EXISTED);
	}
	else{
		bool ret;
		ret = RegistUser(DB(d),NetGetStringData(p,"id"),
					user);

		if(ret){
			pkt->header.type = REGIST_OK;
		}
		else{
			pkt->header.type = REGIST_FAILED;
			NetAddStringData(pkt,"reason", REASON_UNKNOWN);
		}
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);

	DisposeUser(user);

	return true;
}
