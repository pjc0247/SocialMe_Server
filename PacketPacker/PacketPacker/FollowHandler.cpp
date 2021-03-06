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

	ret = Follow(DB(d),
		d.handle->user->id,
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

	ret = Unfollow(DB(d),
		d.handle->user->id,
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

bool FollowQueryFollowing(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret;

	ret = IsFollowing(DB(d),
		NetGetStringData(p, "id1"),
		NetGetStringData(p, "id2"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	pkt->header.type = FOLLOW_INFO_FOLLOWING;
	NetAddNumberData(pkt, "result", ret);

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool FollowQueryFollowed(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret;

	ret = IsFollowed(DB(d),
		NetGetStringData(p, "id1"),
		NetGetStringData(p, "id2"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	pkt->header.type = FOLLOW_INFO_FOLLOWED;
	NetAddNumberData(pkt, "result", ret);

	NetSendPacket(d.handle,d.io,pkt);
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

	count = FollowingCount(DB(d),
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

	count = FollowedCount(DB(d),
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
	
	ret = QueryFollowingList(DB(d),
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
	
	ret = QueryFollowerList(DB(d),
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
