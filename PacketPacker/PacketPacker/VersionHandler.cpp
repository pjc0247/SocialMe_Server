#include "stdafx.h"

#include "ServerHandler.h"
#include "Protocol.h"

bool VersionCheck(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	int major, minor, fix;

	major = NetGetNumberData(p, "major");
	minor = NetGetNumberData(p, "minor");
	fix = NetGetNumberData(p, "fix");

	NetPacket *pkt;
	pkt = NetCreatePacket();

	if(major != SERVER_VERSION_MAJOR ||
		minor != SERVER_VERSION_MINOR ||
		fix != SERVER_VERSION_FIX){
			pkt->header.type = VERSION_FAILED;
	}
	else{
		pkt->header.type = VERSION_OK;
	}

	NetSendPacket(d.handle, d.io, pkt);
	NetDisposePacket(pkt, true);

	return true;
}