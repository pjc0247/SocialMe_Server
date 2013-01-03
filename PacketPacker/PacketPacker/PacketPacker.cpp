// PacketPacker.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "NetPacket.h"

#include <map>
using namespace std;

extern map<char *,SOCKET> msockMap;

int NetIntialize(){
	return 0;
}
void NetQuit(){
}

// NetSerialize
// NetUnserialize

int NetRecv(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,int size){
	DWORD read = -1;
	DWORD Flags;

	PerIoData->wsaBuf.len = size;
	PerIoData->wsaBuf.buf = PerIoData->buffer;
	PerIoData->size = PerIoData->bytesToRecv = size;
	PerIoData->overlapped.flag = ASYNC_RECV;
	Flags=0;

	//printf("recv %d\n", size);

	WSARecv(PerHandleData->hClntSock,
		&(PerIoData->wsaBuf),
		1,                
		&read,                                 
		&Flags,
		&(PerIoData->overlapped),
		NULL
		);

	//printf("%d %d\n", read, PerIoData->wsaBuf.len);	
	if(read == -1){
		//PerHandleData->disconnected = true;
	}

	//WaitForSingleObject(PerIoData->overlapped.hEvent,INFINITE);
//	int read = 0;
	//read = recv();
	return read;
}

void CALLBACK SendCallback(DWORD err,DWORD sent,LPWSAOVERLAPPED overlapped,DWORD flags){

}
int NetSend(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,void *data,int size){
	DWORD written = 0;
	DWORD Flags = 0;
	WSABUF buf;

	buf.buf = (char *)data;
	buf.len = size;
	PerIoData->overlapped.flag = ASYNC_SEND;

	//printf("%d \n", buf.len);

	WSASend(PerHandleData->hClntSock,
		&buf, 1,&written,
		Flags,
		NULL,SendCallback);
	//written = send();
	return written;
}
int NetSendMsg(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,void *data,int size){
	DWORD written = 0;
	DWORD Flags = 0;
	WSABUF buf;

	buf.buf = (char *)data;
	buf.len = size;
	PerIoData->overlapped.flag = ASYNC_SEND;

	//printf("%d \n", buf.len);

	WSASend(msockMap[inet_ntoa(PerHandleData->clntAddr.sin_addr)],
		&buf, 1,&written,
		Flags,
		NULL,SendCallback);
	//written = send();
	return written;
}


unsigned long NetGetTimestamp(){
	return 0;
}


bool NetRecvPacketData(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,NetPacketData *data){

	/*PerIoData->bytesToRecv = sizeof(char) * 16;
	PerIoData->recvState = NET_RECV_DATANAME;
	NetRecv(PerHandleData,PerIoData,(void *)data->name, MAX_NAME_LENGTH);

	Sleep(10);

	PerIoData->bytesToRecv = sizeof(int);
	PerIoData->recvState = NET_RECV_DATASIZE;
	NetRecv(PerHandleData,PerIoData,(void *)data->size, sizeof(int));

	data->data = malloc(data->size);
	if(data->data == NULL)
		return false;

	PerIoData->bytesToRecv = data->size;
	PerIoData->recvState = NET_RECV_DATA;
	NetRecv(PerHandleData,PerIoData,data->data, data->size);
	*/
	return true;
}
bool NetRecvPacket(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData){
	NetPacket *packet = (NetPacket*)PerHandleData->packet;

	PerIoData->recvState = NET_RECV_HEADER;
	PerIoData->bytesToRecv = sizeof(NetPacketHeader);
	PerIoData->dataIndex = 0;
	// 헤더 정보 수신
	bool headerRet = true;
	if(NetRecv(PerHandleData,PerIoData, sizeof(NetPacketHeader)) !=
		sizeof(NetPacketHeader)){
			headerRet = false;

			return false;
	}

	/*
	// 메모리 할당
	bool allocRet = true;
	packet->data = (NetPacketData*)malloc(
		sizeof(NetPacketData) * packet->header.count);
	if(packet->data == NULL){
		allocRet = false;

		return false;
	}
	*/

	// 데이터 수신
	/*bool dataRet  = true;
	for(int i=0;i<packet->header.count;i++){
		dataRet = dataRet & 
					NetRecvPacketData(PerHandleData,PerIoData,&packet->data[i]);
		PerIoData->dataIndex ++;
		if(dataRet == false)
			return false;
	}*/

	//return headerRet & allocRet & dataRet;
	return true;
}

bool NetSendPacketData(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,NetPacketData *data){

	NetSend(PerHandleData,PerIoData,(void *)data->name, MAX_NAME_LENGTH);
	NetSend(PerHandleData,PerIoData,(void *)&data->size, sizeof(int));

	NetSend(PerHandleData,PerIoData,data->data, data->size);

	return true;
}
bool NetSendPacket(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData, NetPacket *packet){
	// 타임 스탬프 기입
	packet->header.timestamp = NetGetTimestamp();

	// 헤더 정보 전송
	bool headerRet = true;
	if(NetSend(PerHandleData,PerIoData,(void *)&packet->header,sizeof(NetPacketHeader)) != 
		sizeof(NetPacketHeader)){
			headerRet = false;

			return false;
	}

	// 서브 데이터 전송
	bool dataRet = true;
	for(int i=0;i<packet->header.count;i++){
		dataRet = dataRet &
					NetSendPacketData(PerHandleData,PerIoData,&packet->data[i]);

		if(dataRet == false)
			return false;
	}

	//return headerRet & dataRet;
	return true;
}

bool NetSendMsgPacketData(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData,NetPacketData *data){

	NetSendMsg(PerHandleData,PerIoData,(void *)data->name, MAX_NAME_LENGTH);
	NetSendMsg(PerHandleData,PerIoData,(void *)&data->size, sizeof(int));

	NetSendMsg(PerHandleData,PerIoData,data->data, data->size);

	return true;
}
bool NetSendMsgPacket(PER_HANDLE_DATA *PerHandleData,PER_IO_DATA *PerIoData, NetPacket *packet){
	// 타임 스탬프 기입
	packet->header.timestamp = NetGetTimestamp();

	// 헤더 정보 전송
	bool headerRet = true;
	if(NetSendMsg(PerHandleData,PerIoData,(void *)&packet->header,sizeof(NetPacketHeader)) != 
		sizeof(NetPacketHeader)){
			headerRet = false;

			return false;
	}

	// 서브 데이터 전송
	bool dataRet = true;
	for(int i=0;i<packet->header.count;i++){
		dataRet = dataRet &
					NetSendPacketData(PerHandleData,PerIoData,&packet->data[i]);

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
	//printf("%d\n", data->size);
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
	data.data = (void*)&i;
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
	//printf("%d\n", data.size);
	data.data = (void*)msg;
	sprintf(data.name, name);
	NetAddData(packet,&data);
}

int NetGetNumberData(NetPacket *packet, const char *name){
	NetPacketData *data;
	data = NetGetData(packet,name);
	return *(int*)data->data;
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



