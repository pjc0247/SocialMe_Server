#include "stdafx.h"

#include "User.h"
#include "Protocol.h"
#include "NetPacket.h"
#include "reason.h"
#include "ServerHandler.h"

bool OnConnected(PacketHandlerData d){

	// TODO : �� ����Ʈ�� �����Ͽ� ���� ���,. �ź�

	return true;
}
bool OnDisconnected(PacketHandlerData d){

	// TODO : �α��� ���̸� �α׾ƿ� ó��

	if(d.handle->user != NULL){
		LogoutTry(d);
	}
	return true;
}

bool OnPing(PacketHandlerData d){
	return true;
}