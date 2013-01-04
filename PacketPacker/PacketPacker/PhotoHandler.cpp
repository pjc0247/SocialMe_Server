#include "stdafx.h"

#include "Protocol.h"
#include "ServerHandler.h"
#include "photo.h"
#include "reason.h"

bool PhotoPush(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	PhotoPost post;

	
	post.lat = NetGetNumberData(p, "lat");
	post.lon = NetGetNumberData(p, "lon");
	SET(post.photo, NetGetStringData(p, "photo"));
	SET(post.comment, NetGetStringData(p, "comment"));

	ret = PushPhoto(DB(d),d.handle->user->id, &post);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret){
		pkt->header.type = PHOTO_OK;
		NetAddNumberData(pkt, "photo_id", post.photo_id);	
	}
	else{
		pkt->header.type = PHOTO_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool PhotoDelete(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	ret = DeletePhoto(DB(d),d.handle->user->id, NetGetNumberData(p, "photo_id"));

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret){
		pkt->header.type = PHOTO_OK;
	}
	else{
		pkt->header.type = PHOTO_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool PhotoQuery(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	int min, max;

	min = NetGetNumberData(p, "min");
	max = NetGetNumberData(p, "max");

	PhotoPost *plist;
	NetPacket *pkt;
	int len;

	pkt = NetCreatePacket();

	plist = QueryPhotoList(DB(d),NetGetStringData(p, "id")
							, min, max, &len);

	if(plist == NULL){
		pkt->header.type = PHOTO_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_MEMORY_ERROR);
		goto CleanUp;
	}
	else
		pkt->header.type = PHOTO_INFO;

	NetAddNumberData(pkt, "count", len);
	for(int i=0;i<len;i++){
		char msg[8];

		printf("%s %d\n", plist[i].comment, plist[i].lat);

		sprintf(msg,"i%d", i+1);
		NetAddStringData(pkt, msg, plist[i].id);
		sprintf(msg,"c%d", i+1);
		NetAddStringData(pkt, msg, plist[i].comment);
		sprintf(msg,"p%d", i+1);
		NetAddStringData(pkt, msg, plist[i].photo);
		sprintf(msg,"pi%d", i+1);
		NetAddNumberData(pkt, msg, plist[i].photo_id);
		sprintf(msg,"t%d", i+1);
		NetAddNumberData(pkt, msg, plist[i].time);
		sprintf(msg,"lt%d", i+1);
		NetAddNumberData(pkt, msg, plist[i].lat);
		sprintf(msg,"ln%d", i+1);
		NetAddNumberData(pkt, msg, plist[i].lon);
		sprintf(msg,"l%d", i+1);
		NetAddNumberData(pkt, msg, plist[i].like);
	}

CleanUp:
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	free(plist);

	return ret;
}

bool PhotoLike(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret;

	ret = LikePhoto(DB(d), d.handle->user->id, NetGetNumberData(p, "photo_id"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(ret == true){
		pkt->header.type = PHOTO_OK;
	}
	else{
		pkt->header.type = PHOTO_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}
bool PhotoDislike(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret;

	ret = DislikePhoto(DB(d), d.handle->user->id, NetGetNumberData(p, "photo_id"));

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(ret == true){
		pkt->header.type = PHOTO_OK;
	}
	else{
		pkt->header.type = PHOTO_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}

	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	return ret;
}