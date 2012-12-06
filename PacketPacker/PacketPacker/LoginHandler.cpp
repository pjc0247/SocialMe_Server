#include "stdafx.h"

#include "NetPacket.h"
#include "User.h"
#include "Protocol.h"
#include "ServerHandler.h"

bool LoginTry(PacketHandlerData d){

	NetPacket *p = d.pkt;
	User *user;
	bool exist = false;

	user = CreateUser();

	SET(user->id, NetGetStringData(p,"id"));

	exist = QueryUser(user->id, user);

	NetPacket *pkt;
	pkt = NetCreatePacket();

	// 아이디 존재
	if(exist){
		// 비밀번호 일치
		if(!strcmp(user->pw, NetGetStringData(p,"pw"))){
			pkt->header.type = LOGIN_OK;
			NetAddStringData(pkt,"id", user->id);

			d.handle->user = CreateUser();
			memcpy(d.handle->user, user, sizeof(User));
		}
		// 불일치
		else{
			pkt->header.type = LOGIN_INCORRECT;
			NetAddStringData(pkt,"id", user->id);
			NetAddStringData(pkt,"reason", "pw incorrect");
		}
	}
	// 아이디 업승ㅁ
	else{
		pkt->header.type = LOGIN_INCORRECT;
		NetAddStringData(pkt,"id", user->id);
		NetAddStringData(pkt,"reason", "id not exist");
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);

	DisposeUser(user);

	return true;
}
bool LogoutTry(PacketHandlerData d){

	if(d.handle->user != NULL){
		Logout(d.handle);
		d.handle->user = NULL;
	}
	else{
		NetPacket *p;
		p = NetCreatePacket();
		NetAddStringData(p,"reason", "user not logged in");
		NetSendPacket(d.handle,d.io,p);
		NetDisposePacket(p,true);
	}

	return true;
}