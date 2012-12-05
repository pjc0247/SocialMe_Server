// PacketPacker.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "NetPacket.h"



int NetIntialize(){
	return 0;
}
void NetQuit(){
}

// NetSerialize
// NetUnserialize

int NetSend(SOCKET hSocket,void *data,int size){
	int written = 0;
		printf("send %d bytes\n", size);
	written = send(hSocket,(char *)data,size,0);
	return written;
}


unsigned long NetGetTimestamp(){
	return 0;
}


bool NetSendPacketData(SOCKET hSocket,NetPacketData *data){

	NetSend(hSocket,(void *)data->name, MAX_NAME_LENGTH);
	//printf("ss %d\n", data->size);
	NetSend(hSocket,(void *)&data->size, sizeof(int));

	NetSend(hSocket,data->data, data->size);

	return true;
}
bool NetSendPacket(SOCKET hSocket,NetPacket *packet){

	// 타임 스탬프 기입
	packet->header.timestamp = NetGetTimestamp();

	// 헤더 정보 전송
	bool headerRet = true;
	if(NetSend(hSocket,(void *)&packet->header,sizeof(NetPacketHeader)) != 
		sizeof(NetPacketHeader)){
			headerRet = false;

			return false;
	}

	// 서브 데이터 전송
	bool dataRet = true;
	for(int i=0;i<packet->header.count;i++){
		dataRet = dataRet &
					NetSendPacketData(hSocket,&packet->data[i]);

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
	//printf("%x ", data->data);
	free(data->data);
}
void NetDisposePacket(NetPacket *packet,bool disposeData){

	if(disposeData == true){
		for(int i=0;i<packet->header.count;i++)
			NetDisposeData(&packet->data[i]);
		free(packet->data);
	}
	free(packet);
}

void NetAddData(NetPacket *packet,NetPacketData *data){
	packet->data = (NetPacketData*)realloc(packet->data,
		sizeof(NetPacketData) * (packet->header.count + 1));

	packet->data[packet->header.count].data = malloc(data->size);
	sprintf(packet->data[packet->header.count].name, data->name);
	packet->data[packet->header.count].size = data->size;
	memcpy(packet->data[packet->header.count].data,data->data,data->size);

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
	data.size = strlen(msg) + 1;
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



