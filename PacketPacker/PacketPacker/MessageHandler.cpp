#include "stdafx.h"
#include "Message.h"
#include "ServerHandler.h"
#include "Protocol.h"
#include "reason.h"

#include <time.h>

bool MessageQuery(PacketHandlerData d){
	bool ret = true;

	if(!IsLoggedIn(d.handle)){
		NetPacket *p;
		p = NetCreatePacket();
		p->header.type = MESSAGE_QUERY_FAILED;
		NetAddStringData(p, "reason", REASON_NOT_LOGGED_IN);
		NetDisposePacket(p,true);
		return false;
	}

	while(true){
		Message m;
		if(QueryMessage(d.handle->user->id, &m) == RESULT_FAILED)
			break;

		NetPacket *p;
		p = NetCreatePacket();
		NetAddStringData(p, "sender", m.sender);
		NetAddStringData(p, "receiver", m.sender);
		NetAddStringData(p, "msg", m.msg);
		NetAddNumberData(p, "time", m.time);
		NetAddNumberData(p, "id", m.id);
		NetAddNumberData(p, "type", m.type);
		NetSendPacket(d.handle,d.io, p);
		NetDisposePacket(p,true);
	}

	return  ret;
}
bool MessagePush(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	Message m;
	bool ret = true;


	if(!IsLoggedIn(d.handle)){
		NetPacket *p;
		p = NetCreatePacket();
		p->header.type = MESSAGE_PUSH_FAILED;
		NetAddStringData(p, "reason", REASON_NOT_LOGGED_IN);
		NetDisposePacket(p,true);
		return false;
	}

	m.time = time(NULL);
	SET(m.sender, d.handle->user->id);
	SET(m.receiver, NetGetStringData(p,"id"));
	SET(m.msg, NetGetStringData(p,"msg"));
	m.type = NetGetNumberData(p,"type");
	ret = PushMessage(&m);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret){
		pkt->header.type = MESSAGE_PUSH_OK;
	}
	else{
		pkt->header.type = MESSAGE_PUSH_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	NetSendPacket(d.handle,d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}