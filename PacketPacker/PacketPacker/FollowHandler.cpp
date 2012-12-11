#include "stdafx.h"

#include "follow.h"
#include "ServerHandler.h"
#include "reason.h"
#include "Protocol.h"


bool FollowFollow(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;
	
	bool ret;
	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(!IsLoggedIn(d.handle)){	
		pkt->header.type = FOLLOW_FAILED;
		NetAddStringData(pkt, "reason", REASON_NOT_LOGGED_IN);
		ret = false;
		goto CleanUp;
	}

	ret = Follow(
		NetGetStringData(p, "follower"),
		NetGetStringData(p, "followed"));

	if(ret == false){
		pkt->header.type = FOLLOW_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	else{
		pkt->header.type = FOLLOW_OK;
	}

CleanUp:
	NetSendPacket(d.handle, d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool FollowUnfollow(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;
	
	bool ret;
	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(!IsLoggedIn(d.handle)){	
		pkt->header.type = FOLLOW_FAILED;
		NetAddStringData(pkt, "reason", REASON_NOT_LOGGED_IN);
		ret = false;
		goto CleanUp;
	}

	ret = Unfollow(
		NetGetStringData(p, "follower"),
		NetGetStringData(p, "followed"));

	if(ret == false){
		pkt->header.type = FOLLOW_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	else{
		pkt->header.type = FOLLOW_OK;
	}

CleanUp:
	NetSendPacket(d.handle, d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}

bool FollowQueryFollowingCount(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;
	
	int count;
	bool ret = true;
	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(!IsLoggedIn(d.handle)){	
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_NOT_LOGGED_IN);
		ret = false;
		goto CleanUp;
	}

	count = FollowingCount(
		NetGetStringData(p, "id"));

	if(ret == false){
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	else{
		pkt->header.type = FOLLOW_INFO_FOLLOWING_COUNT;
		NetAddNumberData(pkt, "count", count);
	}

CleanUp:
	NetSendPacket(d.handle, d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool FollowQueryFollowedCount(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;
	
	int count;
	bool ret = true;
	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(!IsLoggedIn(d.handle)){	
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_NOT_LOGGED_IN);
		ret = false;
		goto CleanUp;
	}

	count = FollowedCount(
		NetGetStringData(p, "id"));

	if(ret == false){
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	else{
		pkt->header.type = FOLLOW_INFO_FOLLOWER_COUNT;
		NetAddNumberData(pkt, "count", count);
	}

CleanUp:
	NetSendPacket(d.handle, d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool FollowQueryFollowingList(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;
	
	int count;
	bool ret;
	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(!IsLoggedIn(d.handle)){	
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_NOT_LOGGED_IN);
		ret = false;
		goto CleanUp;
	}

	FollowList list;
	
	ret = QueryFollowingList(
		NetGetStringData(p, "id"),
		&list,
		NetGetNumberData(p, "min"), NetGetNumberData(p, "max"));

	if(ret == false){
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	else{
		pkt->header.type = FOLLOW_INFO_FOLLOWING_LIST;
		NetAddNumberData(pkt, "count", list.count);
		for(int i=0;i<list.count;i++){
			char m[8];
			sprintf(m,"i%d", i+1);
			NetAddStringData(pkt, m, list.id + i*16);

			//printf(" %d : %s\n", i, list.id + i*16); 
		}
		free(list.id);
	}

CleanUp:
	NetSendPacket(d.handle, d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool FollowQueryFollowedList(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;
	
	int count;
	bool ret;
	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(!IsLoggedIn(d.handle)){	
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_NOT_LOGGED_IN);
		ret = false;
		goto CleanUp;
	}

	FollowList list;
	
	ret = QueryFollowerList(
		NetGetStringData(p, "id"),
		&list,
		NetGetNumberData(p, "min"), NetGetNumberData(p, "max"));

	if(ret == false){
		pkt->header.type = FOLLOW_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	else{
		pkt->header.type = FOLLOW_INFO_FOLLOWER_LIST;
		NetAddNumberData(pkt, "count", list.count);
		for(int i=0;i<list.count;i++){
			char m[8];
			sprintf(m,"i%d", i+1);
			NetAddStringData(pkt, m, list.id + i*16);
		}
		free(list.id);
	}

CleanUp:
	NetSendPacket(d.handle, d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
