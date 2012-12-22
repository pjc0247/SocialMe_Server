#include "stdafx.h"

#include "NetPacket.h"
#include "ServerHandler.h"

#include "Protocol.h"
#include "reason.h"

void UserDeny(PacketHandlerData d,int type, const char *reason){
	NetPacket *p;
	p = NetCreatePacket();
	p->header.type = type;
	NetAddStringData(p, "reason", reason);
	NetSendPacket(d.handle, d.io, p);
	NetDisposePacket(p,true);
}

bool UserQuery(PacketHandlerData d){
	NetPacket *p;
	User *user;
	bool ret;

	p = d.pkt;

	user = CreateUser();
	if(user == NULL){
		UserDeny(d, USER_QUERY_FAILED,
			REASON_MEMORY_ERROR);
		return false;
	}
	ret = QueryUser(DB(d),NetGetStringData(p,"id"), user);
	if(!ret){
		ret = false;
		goto Cleanup;
	}

	NetPacket *pkt;
	pkt = NetCreatePacket();
	pkt->header.type = USER_INFO;
	NetAddStringData(pkt, "id", NetGetStringData(p,"id"));
	NetAddStringData(pkt, "nick", user->nick);
	NetAddNumberData(pkt, "age", user->age);
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	DisposeUser(user);
	return ret;
}
bool UserQueryImage(PacketHandlerData d){
	NetPacket *p;
	User *user;
	bool ret;

	p = d.pkt;

	user = CreateUser();
	if(user == NULL){
		UserDeny(d, USER_QUERY_FAILED,
			REASON_MEMORY_ERROR);
		return false;
	}
	ret = QueryUser(DB(d),NetGetStringData(p,"id"), user);
	if(!ret){
		ret = false;
		goto Cleanup;
	}

Cleanup:
	DisposeUser(user);
	return ret;
}
bool UserUpdate(PacketHandlerData d){
	NetPacket *p;
	User *user;
	bool ret = true;

	p = d.pkt;

	if(!IsLoggedIn(d.handle)){
		UserDeny(d, USER_UPDATE_FAILED,
			REASON_NOT_LOGGED_IN);
		return false;
	}

	user = CreateUser();
	if(user == NULL){
		UserDeny(d, USER_UPDATE_FAILED,
			REASON_MEMORY_ERROR);
		return false;
	}

	SET(user->nick, NetGetStringData(d.pkt, "nick"));
	user->age = NetGetNumberData(d.pkt, "age");

	ret = UpdateUser(DB(d),d.handle->user->id, user);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_UPDATE_OK;
		NetAddStringData(pkt, "id", NetGetStringData(p,"id"));	
	}
	else{
		pkt->header.type = USER_UPDATE_FAILED;
		NetAddStringData(pkt, "id", NetGetStringData(p,"id"));
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	DisposeUser(user);
	return ret;
}
