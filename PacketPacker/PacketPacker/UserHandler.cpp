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
	NetAddStringData(pkt, "job", user->job);
	NetAddNumberData(pkt, "age", user->age);
	NetAddNumberData(pkt, "birthday", user->birthday);
	NetAddNumberData(pkt, "sex", user->sex);
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
	SET(user->job, NetGetStringData(d.pkt, "job"));
	user->age = NetGetNumberData(d.pkt, "age");
	user->birthday = NetGetNumberData(d.pkt, "birthday");

	ret = UpdateUser(DB(d),d.handle->user->id, user);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_UPDATE_OK;
	}
	else{
		pkt->header.type = USER_UPDATE_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	DisposeUser(user);
	return ret;
}
bool UserUpdateComment(PacketHandlerData d){
	NetPacket *p;
	bool ret = true;

	p = d.pkt;

	if(!IsLoggedIn(d.handle)){
		UserDeny(d, USER_UPDATE_FAILED,
			REASON_NOT_LOGGED_IN);
		return false;
	}

	ret = UpdateUserComment(
		DB(d),
		d.handle->user->id,
		NetGetStringData(p,"comment"));

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_UPDATE_OK;
	}
	else{
		pkt->header.type = USER_UPDATE_FAILED;
		NetAddStringData(pkt, "reason", NetGetStringData(p,"id"));
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	return ret;
}
bool UserQueryComment(PacketHandlerData d){
	NetPacket *p;
	bool ret = true;

	p = d.pkt;

	if(!IsLoggedIn(d.handle)){
		UserDeny(d, USER_QUERY_FAILED,
			REASON_NOT_LOGGED_IN);
		return false;
	}

	char comment[281];
	ret = QueryUserComment(
		DB(d),
		NetGetStringData(p, "id"),
		comment);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_INFO_COMMENT;
		NetAddStringData(pkt, "comment", comment);
	}
	else{
		pkt->header.type = USER_QUERY_FAILED;
		NetAddStringData(pkt, "reason", NetGetStringData(p,"id"));
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	return ret;
}

bool UserUpdateFacebook(PacketHandlerData d){
	NetPacket *p;
	bool ret = true;

	p = d.pkt;

	if(!IsLoggedIn(d.handle)){
		UserDeny(d, USER_UPDATE_FAILED,
			REASON_NOT_LOGGED_IN);
		return false;
	}

	ret = UpdateUserFacebook(
		DB(d),
		d.handle->user->id,
		NetGetStringData(p,"facebook"));

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_UPDATE_OK;
	}
	else{
		pkt->header.type = USER_UPDATE_FAILED;
		NetAddStringData(pkt, "reason", NetGetStringData(p,"id"));
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	return ret;
}
bool UserQueryFacebook(PacketHandlerData d){
	NetPacket *p;
	bool ret = true;

	p = d.pkt;

	if(!IsLoggedIn(d.handle)){
		UserDeny(d, USER_QUERY_FAILED,
			REASON_NOT_LOGGED_IN);
		return false;
	}

	char fb[281];
	ret = QueryUserFacebook(
		DB(d),
		NetGetStringData(p, "id"),
		fb);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_INFO_FACEBOOK;
		NetAddStringData(pkt, "facebook", fb);
	}
	else{
		pkt->header.type = USER_QUERY_FAILED;
		NetAddStringData(pkt, "reason", NetGetStringData(p,"id"));
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	return ret;
}

bool UserUpdateNateon(PacketHandlerData d){
	NetPacket *p;
	bool ret = true;

	p = d.pkt;

	if(!IsLoggedIn(d.handle)){
		UserDeny(d, USER_UPDATE_FAILED,
			REASON_NOT_LOGGED_IN);
		return false;
	}

	ret = UpdateUserNateon(
		DB(d),
		d.handle->user->id,
		NetGetStringData(p,"nateon"));

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_UPDATE_OK;
	}
	else{
		pkt->header.type = USER_UPDATE_FAILED;
		NetAddStringData(pkt, "reason", NetGetStringData(p,"id"));
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	return ret;
}
bool UserQueryNateon(PacketHandlerData d){
	NetPacket *p;
	bool ret = true;

	p = d.pkt;

	if(!IsLoggedIn(d.handle)){
		UserDeny(d, USER_QUERY_FAILED,
			REASON_NOT_LOGGED_IN);
		return false;
	}

	char nt[281];
	ret = QueryUserNateon(
		DB(d),
		NetGetStringData(p, "id"),
		nt);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret == true){
		pkt->header.type = USER_INFO_NATEON;
		NetAddStringData(pkt, "nateon", nt);
	}
	else{
		pkt->header.type = USER_QUERY_FAILED;
		NetAddStringData(pkt, "reason", NetGetStringData(p,"id"));
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);
Cleanup:
	return ret;
}