#include "stdafx.h"

#include "ServerHandler.h"

#include "Protocol.h"

bool ProcessPacket(PacketHandlerData d){

	switch(d.pkt->header.type){
		
	case REGIST_TRY:
		RegistTry(d);
		break;

	case LOGIN_TRY:
		LoginTry(d);
		break;
	case LOGOUT_TRY:
		LogoutTry(d);
		break;

	default:
		output("unknown packet\n");
		break;
	}

	return true;
}