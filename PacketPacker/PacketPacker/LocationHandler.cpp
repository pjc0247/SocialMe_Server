#include "stdafx.h"

#include "Protocol.h"
#include "ServerHandler.h"
#include "location.h"
#include "reason.h"

bool LocationPush(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	printf("push loca\n %d %d\n",NetGetNumberData(p,"lat"),NetGetNumberData(p,"lon"));

	ret = PushLocation(DB(d),
			d.handle->user->id,
			NetGetNumberData(p,"lat"),
			NetGetNumberData(p,"lon"));

	if(ret == false){
		NetPacket *pkt;
		pkt = NetCreatePacket();
		pkt->header.type = LOCATION_PUSH_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
		NetDisposePacket(pkt, true);
	}

	return ret;
}
bool LocationQuery(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	bool ret = true;

	int min, max;

	Location *plist;
	NetPacket *pkt;
	int len;

	pkt = NetCreatePacket();

	printf("ASDF");
	plist = QueryLocation(DB(d)
							, &len);


	if(plist == NULL){
		pkt->header.type = LOCATION_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_MEMORY_ERROR);
		goto CleanUp;
	}
	else
		pkt->header.type = LOCATION_INFO;

	NetAddNumberData(pkt, "count", len);
	for(int i=0;i<len;i++){
		char msg[8];

		sprintf(msg,"lt%d", i+1);
		NetAddNumberData(pkt, msg, plist[i].lat);
		sprintf(msg,"ln%d", i+1);
		NetAddNumberData(pkt, msg, plist[i].lon);
		sprintf(msg,"i%d", i+1);
		NetAddStringData(pkt, msg, plist[i].id);
	}

CleanUp:
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt, true);

	free(plist);

	return ret;
}
