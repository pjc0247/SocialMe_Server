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

	// TODO : 블랙 리스트와 대조하여 연결 허용,. 거부

	d.handle->db = DbConnect();

	Session ses;
	ses.io = d.io;
	ses.handle = d.handle;
	conn.push_back(ses);

	return true;
}
bool OnDisconnected(PacketHandlerData d){

	// TODO : 로그인 중이면 로그아웃 처리

	DbDisconnect(d.handle->db);

	/*if(d.handle->user != NULL){
		LogoutTry(d);
	}*/
	return true;
}

bool OnPing(PacketHandlerData d){
	return true;
}