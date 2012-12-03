// PacketPacker.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>




// ��Ŷ ���� ������ �̸��� �ִ� ����
#define MAX_NAME_LENGTH 16

struct NetPacketHeader{
	int type;

	unsigned long timestamp;

	unsigned char count;
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


int NetIntialize(){
	return 0;
}
void NetQuit(){
}

// NetSerialize
// NetUnserialize

int NetRecv(void *data,int size){
	int read = 0;
	//read = recv();
	return read;
}
int NetSend(void *data,int size){
	int written = 0;
	//written = send();
	return written;
}


unsigned long NetGetTimestamp(){
	return 0;
}


bool NetRecvPacketData(NetPacketData *data){

	NetRecv((void *)data->name, MAX_NAME_LENGTH);
	NetRecv((void *)data->size, sizeof(int));

	data->data = malloc(data->size);
	if(data->data == NULL)
		return false;

	NetRecv(data->data, data->size);

	return true;
}
bool NetRecvPacket(NetPacket *packet){

	// ��� ���� ����
	bool headerRet = true;
	if(NetRecv((void *)&packet->header, sizeof(NetPacketHeader)) !=
		sizeof(NetPacketHeader)){
			headerRet = false;

			return false;
	}

	// �޸� �Ҵ�
	bool allocRet = true;
	packet->data = (NetPacketData*)malloc(
		sizeof(NetPacketData) * packet->header.count);
	if(packet->data == NULL){
		allocRet = false;

		return false;
	}

	// ������ ����
	bool dataRet  = true;
	for(int i=0;i<packet->header.count;i++){
		dataRet = dataRet & NetRecvPacketData(&packet->data[i]);
		if(dataRet == false)
			return false;
	}

	//return headerRet & allocRet & dataRet;
	return true;
}

bool NetSendPacketData(NetPacketData *data){

	NetSend((void *)data->name, MAX_NAME_LENGTH);
	NetSend((void *)data->size, sizeof(int));

	NetSend(data->data, data->size);

	return true;
}
bool NetSendPacket(NetPacket *packet){

	// Ÿ�� ������ ����
	packet->header.timestamp = NetGetTimestamp();

	// ��� ���� ����
	bool headerRet = true;
	if(NetSend((void *)&packet->header,sizeof(NetPacketHeader)) != 
		sizeof(NetPacketHeader)){
			headerRet = false;

			return false;
	}

	// ���� ������ ����
	bool dataRet = true;
	for(int i=0;i<packet->header.count;i++){
		dataRet = dataRet & NetSendPacketData(&packet->data[i]);

		if(dataRet == false)
			return false;
	}

	//return headerRet & dataRet;
	return true;
}


NetPacket *NetCreatePacket(){
	NetPacket *packet;
	packet = (NetPacket*)malloc(sizeof(NetPacket));
	memset(packet,0,sizeof(NetPacket));
	return packet;
}

void NetDisposeData(NetPacketData *data){
	free(data->data);
	free(data);
}
void NetDisposePacket(NetPacket *packet,bool disposeData){

	if(disposeData == true){
		for(int i=0;i<packet->header.count;i++)
			NetDisposeData(&packet->data[i]);
	}
	free(packet);
}

void NetAddData(NetPacket *packet,NetPacketData *data){
	packet->data = (NetPacketData*)realloc(packet->data,
		sizeof(NetPacketData) * (packet->header.count + 1));

	packet->data[packet->header.count] = *data;

	packet->header.count ++;
}
NetPacketData *NetGetData(NetPacket *packet, const char *name){
	NetPacketData *data = NULL;

	for(int i=0;i<packet->header.count;i++){
		if(!strcmp(packet->data[i].name, name)){
			data = &packet->data[i];
			break;
		}
	}

	return data;
}

void NetAddNumberData(NetPacket *packet, const char *name, int i){
	NetPacketData data;
	data.size = sizeof(int);
	data.data = (void*)i;
	sprintf(data.name, name);
	NetAddData(packet,&data);
}
void NetAddCharacterData(NetPacket *packet, const char *name, char c){
	NetPacketData data;
	data.size = sizeof(char);
	data.data = (void*)c;
	sprintf(data.name, name);
	NetAddData(packet,&data);
}
void NetAddStringData(NetPacket *packet, const char *name, const char *msg){
	NetPacketData data;
	data.size = strlen(msg);
	data.data = (void*)msg;
	sprintf(data.name, name);
	NetAddData(packet,&data);
}

int NetGetNumberData(NetPacket *packet, const char *name){
	NetPacketData *data;
	data = NetGetData(packet,name);
	return (int)data->data;
}
char NetGetCharacterData(NetPacket *packet, const char *name){
	NetPacketData *data;
	data = NetGetData(packet,name);
	return (char)data->data;
}
char *NetGetStringData(NetPacket *packet, const char *name){
	NetPacketData *data;
	data = NetGetData(packet,name);
	return (char*)data->data;
}



