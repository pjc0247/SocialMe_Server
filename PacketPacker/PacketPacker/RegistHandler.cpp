#include "stdafx.h"

#include "NetPacket.h"
#include "User.h"
#include "Protocol.h"
#include "ServerHandler.h"
#include "reason.h"

bool RegistTry(PacketHandlerData d){
	NetPacket *p = d.pkt;
	bool exist = false;
	User *user; 

	user = CreateUser();

	SET(user->id, NetGetStringData(p,"id"));
	SET(user->pw, NetGetStringData(p,"pw"));
	SET(user->nick, NetGetStringData(p,"nick"));
	SET(user->job, NetGetStringData(p, "job"));
	user->age = NetGetNumberData(p,"age");
	user->sex = NetGetNumberData(p, "sex");
	user->birthday = NetGetNumberData(p, "birthday");

	exist = QueryUser(DB(d),user->id, NULL);

	NetPacket *pkt;
	pkt = NetCreatePacket();

	// �̹� �����ϴ� ���̵��ϰ��
	if(exist){
		pkt->header.type = REGIST_EXIST_ID;
		NetAddStringData(pkt,"reason", REASON_ID_ALREADY_EXISTED);
	}
	else{
		bool ret;
		ret = RegistUser(DB(d),NetGetStringData(p,"id"),
					user);
		ret &= UpdateUserAndroid(DB(d), NetGetStringData(p,"id"),
					NetGetStringData(p,"android"));


		if(ret){
			pkt->header.type = REGIST_OK;
		}
		else{
			pkt->header.type = REGIST_FAILED;
			NetAddStringData(pkt,"reason", REASON_UNKNOWN);
		}
	}
	NetSendPacket(d.handle,d.io,pkt);
	NetDisposePacket(pkt,true);

	DisposeUser(user);

	return true;
}
