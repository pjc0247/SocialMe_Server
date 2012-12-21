#include "stdafx.h"

#include "User.h"
#include "Protocol.h"
#include "NetPacket.h"
#include "reason.h"
#include "ServerHandler.h"
#include "database.h"

#include <list>
using namespace std;

extern list<Session> conn;

bool OnConnected(PacketHandlerData d){

	// TODO : �� ����Ʈ�� �����Ͽ� ���� ���,. �ź�

	d.handle->db = DbConnect();

	Session ses;
	ses.io = d.io;
	ses.handle = d.handle;
	conn.push_back(ses);

	return true;
}
bool OnDisconnected(PacketHandlerData d){

	// TODO : �α��� ���̸� �α׾ƿ� ó��

	DbDisconnect(d.handle->db);

	/*if(d.handle->user != NULL){
		LogoutTry(d);
	}*/
	return true;
}

bool OnPing(PacketHandlerData d){
	return true;
}