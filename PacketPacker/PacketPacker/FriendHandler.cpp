#include "stdafx.h"

#include "friend.h"
#include "ServerHandler.h"
#include "Protocol.h"
#include "reason.h"

bool FriendFriend(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	ret = Friend(DB(d), 
		d.handle->user->id,
		NetGetStringData(p, "id"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(ret == true){
		pkt->header.type = FRIEND_ADD_OK;
	}
	else{
		pkt->header.type = FRIEND_ADD_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}

bool FriendUnfriend(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	ret = Unfriend(DB(d), 
		d.handle->user->id,
		NetGetStringData(p, "id"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(ret == true){
		pkt->header.type = FRIEND_DELETE_OK;
	}
	else{
		pkt->header.type = FRIEND_DELETE_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}

bool FriendQueryFriend(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	ret = IsFriended(DB(d),
		NetGetStringData(p, "id1"),
		NetGetStringData(p, "id2"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	pkt->header.type = FRIEND_INFO_FRIEND;
	NetAddNumberData(pkt, "result", ret);

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool FriendQueryCount(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	int ret = true;

	ret = QueryFriendCount(DB(d), NetGetStringData(p, "id"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(ret == -1){
		pkt->header.type = FRIEND_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	else{
		pkt->header.type = FRIEND_INFO_COUNT;
		NetAddNumberData(pkt, "count", ret);
	}

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}

bool FriendQueryList(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;
	FriendList list;

	ret = QueryFriendList(DB(d),
		NetGetStringData(p, "id"),
		&list,
		NetGetNumberData(p, "min"),
		NetGetNumberData(p, "max"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(ret == false){
		pkt->header.type = FRIEND_QUERY_FAILED;
	}
	else{
		pkt->header.type = FRIEND_INFO_LIST;

		NetAddNumberData(pkt, "count", list.count);
		for(int i=0;i<list.count;i++){
			char m[8];
			sprintf(m, "i%d", i+1);
			NetAddStringData(pkt, m, list.id + i*16);
		}
		free(list.id);
	}

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}