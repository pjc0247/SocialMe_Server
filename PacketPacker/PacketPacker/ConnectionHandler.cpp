#include "stdafx.h"

#include "User.h"
#include "Protocol.h"
#include "NetPacket.h"

#include "ServerHandler.h"

bool OnConnected(PacketHandlerData d){

	// TODO : �� ����Ʈ�� �����Ͽ� ���� ���,. �ź�
}
bool OnDisconnected(PacketHandlerData d){

	// TODO : �α��� ���̸� �α׾ƿ� ó��

	if(d.handle->user != NULL){
		LogoutTry(d);
	}
}