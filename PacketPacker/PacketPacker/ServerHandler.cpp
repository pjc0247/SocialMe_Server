#include "stdafx.h"

#include "ServerHandler.h"

#include "Protocol.h"

bool ProcessPacket(NetPacket *p){

	switch(p->header.type){
	case REGIST_TRY:
		RegistTry(p);
		break;
	default:
		output("unknown packet\n");
		break;
	}

	return true;
}