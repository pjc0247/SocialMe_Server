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

	case USER_UPDATE:
		UserUpdate(d);
		break;
	case USER_QUERY:
		UserQuery(d);
		break;
	case USER_QUERY_IMG:
		UserQueryImage(d);
		break;

	case MESSAGE_PUSH:
		MessagePush(d);
		break;
	case MESSAGE_QUERY:
		MessageQuery(d);
		break;

	case PAYMENT_QUERY:
		PaymentQuery(d);
		break;
	case PAYMENT_PUSH:
		PaymentPush(d);
		break;

	default:
		output("unknown packet\n");
		break;
	}

	return true;
}