/*
 *  SocialMe Server Project
 *				written by pjc0247
 *	
 *	Network, Packet packing Module
 */

#ifndef NETPACKET_H
#define NETPACKET_H

#if defined(WIN32) || defined(_WIN32)
	#include "NetSocket_Windows.h"
#else
	#include "NetSocket_Bsd.h"
#endif 

#include "Server.h"

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

enum NetRecvState 
{
	NET_RECV_HEADER,
	NET_RECV_DATANAME,
	NET_RECV_DATASIZE,
	NET_RECV_DATA
};


int NetIntialize();
void NetQuit();

// NetSerialize
// NetUnserialize

int NetRecv(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,int size);
int NetSend(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,void *data,int size);
int NetSendMsg(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,void *data,int size);

unsigned long NetGetTimestamp();


bool NetRecvPacketData(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,NetPacketData *data);
bool NetRecvPacket(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData);

bool NetSendPacketData(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,NetPacketData *data);
bool NetSendPacket(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData, NetPacket *packet);
bool NetSendMsgPacket(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData, NetPacket *packet);

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