#include "stdafx.h"

#include "User.h"
#include "Protocol.h"

#include "NetPacket.h"
#include "ServerHandler.h"

#include "reason.h"

bool PostQuery(PacketHandlerData d){
	int ret;

	ret = QueryUserPost(DB(d), d.handle->user->id);

	NetPacket *p;
	p = NetCreatePacket();

	if(ret == -1){
		p->header.type = POST_QUERY_FAILED;
		NetAddStringData(p, "reason", REASON_UNKNOWN);
	}
	else{
		p->header.type = POST_INFO_COUNT;
		NetAddNumberData(p, "count", ret);
	}

	NetSendPacket(d.handle,d.io, p);
	NetDisposePacket(p, true);

	return ret;
}
bool PostIncrease(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret;
	User *u;
	NetPacket *pkt;

	pkt = NetCreatePacket();
	u = CreateUser();
	if(u == NULL){
		pkt->header.type = POST_FAILED;
		NetAddStringData(pkt, "reason", REASON_MEMORY_ERROR);

		ret = false;
		goto CleanUp;
	}

	u->post = NetGetNumberData(p, "value");
	ret = UpdateUser(DB(d),NetGetStringData(p,"id"),u);

	DisposeUser(u);

CleanUp:
	NetSendPacket(d.handle,d.io, pkt);
	NetDisposePacket(pkt, true);
	return ret;
}
bool PostDecrease(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret;
	User *u;
	NetPacket *pkt;

	pkt = NetCreatePacket();
	u = CreateUser();
	if(u == NULL){
		pkt->header.type = POST_FAILED;
		NetAddStringData(pkt, "reason", REASON_MEMORY_ERROR);

		ret = false;
		goto CleanUp;
	}

	u->post = -NetGetNumberData(p, "value");
	ret = UpdateUser(DB(d),NetGetStringData(p,"id"),u);

	DisposeUser(u);

CleanUp:
	NetSendPacket(d.handle,d.io, pkt);
	NetDisposePacket(pkt, true);
	return ret;
}