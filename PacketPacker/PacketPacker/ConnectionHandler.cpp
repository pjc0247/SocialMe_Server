#include "stdafx.h"

#include "User.h"
#include "Protocol.h"
#include "NetPacket.h"

#include "ServerHandler.h"

bool OnConnected(PacketHandlerData d){

	// TODO : 블랙 리스트와 대조하여 연결 허용,. 거부
}
bool OnDisconnected(PacketHandlerData d){

	// TODO : 로그인 중이면 로그아웃 처리

	if(d.handle->user != NULL){
		LogoutTry(d);
	}
}