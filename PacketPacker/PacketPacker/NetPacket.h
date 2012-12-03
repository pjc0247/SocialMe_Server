#ifndef NETPACKET_H
#define NETPACKET_H

#if defined(WIN32) || defined(_WIN32)
	#include "NetSocket_Windows.h"
#else
	#include "NetSocket_Bsd.h"
#endif 

// 패킷 서브 데이터 이름의 최대 길이
#define MAX_NAME_LENGTH 16

struct NetPacketHeader{
	int type;
	int timestamp;
	int count;
};

struct NetPacketData{
	char name[MAX_NAME_LENGTH];
	int size;
	void *data;
};

struct NetPacket{
	NetPacketHeader header;
	NetPacketData *data;
};


int NetIntialize();
void NetQuit();

// NetSerialize
// NetUnserialize

int NetRecv(void *data,int size);
int NetSend(void *data,int size);

unsigned long NetGetTimestamp();


bool NetRecvPacketData(NetPacketData *data);
bool NetRecvPacket(NetPacket *packet);

bool NetSendPacketData(NetPacketData *data);
bool NetSendPacket(NetPacket *packet);

NetPacket *NetCreatePacket();

void NetDisposeData(NetPacketData *data);
void NetDisposePacket(NetPacket *packet,bool disposeData);

void NetAddData(NetPacket *packet,NetPacketData *data);
NetPacketData *NetGetData(NetPacket *packet, const char *name);

void NetAddNumberData(NetPacket *packet, const char *name, int i);
void NetAddCharacterData(NetPacket *packet, const char *name, char c);
void NetAddStringData(NetPacket *packet, const char *name, const char *msg);

int NetGetNumberData(NetPacket *packet, const char *name);
char NetGetCharacterData(NetPacket *packet, const char *name);
char *NetGetStringData(NetPacket *packet, const char *name);

#endif //NETPACKET_H