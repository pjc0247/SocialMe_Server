#include "stdafx.h"

#include "Server.h"
#include "blacklist.h"

#include "NetPacket.h"
#include "ServerHandler.h"
#include "monitor.h"

#include "Protocol.h"

#include <map>
#include <list>
#include <string>
using namespace std;

unsigned long uptime_st;
list<Session> conn;

unsigned int __stdcall CompletionThread(void* pComPort);
unsigned int __stdcall PingThread(void* pComPort);

list<Session>::iterator DeleteConn(Session s){
	list<Session>::iterator itor;

	for(itor=conn.begin();itor!=conn.end();++itor){
		if(itor->handle == s.handle){
			printf("delete at %x\n", itor->handle);
			
			return conn.erase(itor);
		}
	}
}

int RunServer(int port){
	WSADATA wsaData;
	HANDLE hCompletionPort;      
	SYSTEM_INFO SystemInfo;
	SOCKADDR_IN servAddr;
	LPPER_IO_DATA PerIoData;
	LPPER_HANDLE_DATA PerHandleData;

	SOCKET hServSock;
	int i;

	SetConsoleTitleA("SocialMe Server");

	// Windows Socket의 초기화
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		output("startup error\n");

	_beginthreadex(NULL, 0, PingThread, NULL, 0, NULL);

	DWORD dwProcessor;
	GetSystemInfo(&SystemInfo);
	dwProcessor = SystemInfo.dwNumberOfProcessors;
	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, (dwProcessor * 2));

	for(i=0; i < 8; i++) {
		//CreateThread(NULL, 0, CompletionThread, (LPVOID)hCompletionPort, 0, NULL);
		_beginthreadex(NULL, 0, CompletionThread, (LPVOID)hCompletionPort, 0, NULL);
	}

	hServSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(port);

	if(bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		output("bind error\n");
		return 1;
	}

	if(listen(hServSock, SERVER_LISTEN_QUEUE) == SOCKET_ERROR) {
		output("listen error\n");
		return 1;
	}

	output("server ready - %dcore/%dthread\n",dwProcessor,dwProcessor*2);

	uptime_st = GetTickCount64();

	// 서버 컨트롤러 스레드를 생성
	StartServerController();

	int N = 0;
	do
	{      
		SOCKET hClntSock;
		SOCKADDR_IN clntAddr;        
		int addrLen=sizeof(clntAddr);
		bool ret;

		// 새 클라이언트 연결을 수립한다.
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAddr, &addrLen);  
		
		//char t = TRUE;
		//setsockopt(hClntSock, IPPROTO_TCP, TCP_NODELAY, &t, sizeof(t));

		output("connected... %s - %d\n", inet_ntoa(clntAddr.sin_addr),N);

		// 블랙리스트에서 접속된 아이피를 조회한다.
		ret = isBlockedIP(inet_ntoa(clntAddr.sin_addr));
		if(ret == true){
			// 연결 거부됨
			output("connection FAILED\n");
			continue;
		}

		PerHandleData=(LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));          
		PerHandleData->hClntSock=hClntSock;
		PerHandleData->n = N;
		PerHandleData->packet = NetCreatePacket();
		PerHandleData->user = NULL;
		PerHandleData->disconnected = false;
		PerHandleData->pingtime = 0;
		PerHandleData->lastResp = 0;
		PerHandleData->db = 0;
		memcpy(&(PerHandleData->clntAddr), &clntAddr, addrLen);
		memset(PerHandleData->packet,0,sizeof(NetPacket));

		CreateIoCompletionPort((HANDLE)hClntSock,hCompletionPort,(DWORD)PerHandleData,0);

		PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));           

		PacketHandlerData d;
		//bool ret;
		d.handle = PerHandleData;
		d.io = PerIoData;
		ret = OnConnected(d);

		PushConnection(PerHandleData->n, inet_ntoa(clntAddr.sin_addr));

		NetRecvPacket(PerHandleData,PerIoData);
	}while(++N);


	WSACleanup();

	return 0;
}

unsigned int __stdcall CompletionThread(void* pComPort)
{
	HANDLE hCompletionPort =(HANDLE)pComPort;
	DWORD BytesTransferred;

	LPPER_HANDLE_DATA      PerHandleData;
	LPPER_IO_DATA          PerIoData;

	DWORD flags;

	while(1){
		BOOL ret = GetQueuedCompletionStatus(hCompletionPort,
			&BytesTransferred,
			(LPDWORD)&PerHandleData,
			(LPOVERLAPPED*)&PerIoData,
			INFINITE);

		
		//printf("recv %d %d %d \n",ret ,  BytesTransferred, PerIoData->overlapped.flag);
		if(PerIoData->overlapped.flag != ASYNC_RECV)
			continue;

		// 0 바이트 수신함 -> 연결 종료됨
		if((BytesTransferred == 0 || !ret)
			&&
			PerHandleData->disconnected == false)
		{
			output("close a %d\n", PerHandleData->n);
			// 소켓 종료

			PacketHandlerData d;
			d.handle = PerHandleData;
			d.io = PerIoData;
			OnDisconnected(d);

			Logout(PerHandleData);
			Session s = {PerHandleData, PerIoData};
			DeleteConnection(PerHandleData->n, inet_ntoa(PerHandleData->clntAddr.sin_addr));
			DeleteConn(s);
			closesocket(PerHandleData->hClntSock);
			free(PerHandleData);
			free(PerIoData);

			continue;             
		} 
		//printf("Recv %d bytes\n", BytesTransferred);
		//printf("Recv State : %d, %d\n", PerIoData->recvState,PerIoData->bytesToRecv);


		// 마지막 응답 시간 갱신
		PerHandleData->lastResp = GetTickCount();

		// 패킷을 목표량만큼 전부 받지 못하면
		if(BytesTransferred < PerIoData->bytesToRecv){
			DWORD read;
			DWORD Flags;

			// 현재 받은걸 제외하고 재수신을 요청
			PerIoData->wsaBuf.len = PerIoData->bytesToRecv - BytesTransferred;
			PerIoData->wsaBuf.buf = PerIoData->buffer + BytesTransferred;
			PerIoData->bytesToRecv -= BytesTransferred;
			Flags=0;

			WSARecv(PerHandleData->hClntSock,
				&(PerIoData->wsaBuf),
				1,                
				&read,                                 
				&Flags,
				&(PerIoData->overlapped),
				NULL
				);

			continue;
		}

		NetPacket *p = (NetPacket *)PerHandleData->packet;
		switch(PerIoData->recvState){
			// 패킷의 헤더를 수신한다
			case NET_RECV_HEADER:
				memcpy(&p->header,PerIoData->buffer,PerIoData->size);

				// 메모리 할당
				p->data = (NetPacketData*)malloc(
					sizeof(NetPacketData) * p->header.count);
				if(p->data == NULL){
					output("malloc failed\n");
				}

				if(p->header.count == 0){
					ProcessPacket(PerHandleData,PerIoData,p);

					NetDisposePacket(p, true);
					p = NetCreatePacket();
					NetRecvPacket(PerHandleData,PerIoData);
				}
				else{
					PerIoData->recvState = NET_RECV_DATANAME;
					NetRecv(PerHandleData,PerIoData, MAX_NAME_LENGTH);
				}
				break;

			// 패킷 데이터의 이름을 수신한다
			case NET_RECV_DATANAME:
				memcpy(&p->data[PerIoData->dataIndex].name,PerIoData->buffer,MAX_NAME_LENGTH);
				PerIoData->recvState = NET_RECV_DATASIZE;
				NetRecv(PerHandleData,PerIoData, sizeof(int));
				break;

			// 패킷 데이터의 사이즈를 수신한다
			case NET_RECV_DATASIZE:
				memcpy(&p->data[PerIoData->dataIndex].size,PerIoData->buffer,sizeof(int));
				
				p->data[PerIoData->dataIndex].data = malloc(p->data[PerIoData->dataIndex].size);

				PerIoData->recvState = NET_RECV_DATA;
				NetRecv(PerHandleData,PerIoData, p->data[PerIoData->dataIndex].size);
				break;

			// 패킷 데이터 내용을 수신한다
			case NET_RECV_DATA:
				
				memcpy(p->data[PerIoData->dataIndex].data,PerIoData->buffer,PerIoData->size);
				
				PerIoData->dataIndex ++;

				// Recieve Complete
				if(PerIoData->dataIndex >= p->header.count){
					ProcessPacket(PerHandleData,PerIoData,p);

					NetDisposePacket(p, true);
					p = NetCreatePacket();
					NetRecvPacket(PerHandleData,PerIoData);
				}
				else{
					PerIoData->recvState = NET_RECV_DATANAME;
					NetRecv(PerHandleData,PerIoData, MAX_NAME_LENGTH);
				}
				break;

			// 알 수 없는 패킷, 다시 패킷 헤더를 수신한다.
			default:
				printf("unknown packet\n");
				NetRecvPacket(PerHandleData,PerIoData);
				break;
		}

		memset(&PerIoData->overlapped, 0, sizeof(OVERLAPPED));

		if(PerHandleData->disconnected){
			output("close %d\n", PerHandleData->n);
			// 소켓 종료
			PacketHandlerData d;
			d.handle = PerHandleData;
			d.io = PerIoData;
			OnDisconnected(d);

			Logout(PerHandleData);
			Session s = {PerHandleData, PerIoData};
			DeleteConnection(PerHandleData->n, inet_ntoa(PerHandleData->clntAddr.sin_addr));
			DeleteConn(s);
			closesocket(PerHandleData->hClntSock);
			free(PerHandleData);
			free(PerIoData);
			continue;
		}
	}
	return 0;
}

unsigned int __stdcall PingThread(void* pComPort)
{
	HANDLE hSleepEvent;

	hSleepEvent = CreateEventA(
						NULL,false,false,
						NULL);

	while(1){
		list<Session>::iterator itor;

		for(itor=conn.begin();itor!=conn.end();){
			// 마지막 응답 후 30초 이상 지연된 접속에만
			if(itor->handle->pingtime == 0 &&
				GetTickCount() - itor->handle->lastResp >= 3000){
				NetPacket *p;
				p = NetCreatePacket();
				p->header.type = PING;
				NetSendPacket(itor->handle,itor->io, p);
				NetDisposePacket(p, true);

				itor->handle->pingtime = GetTickCount();
			}

			// 핑 제한시간 지남
			if(itor->handle->pingtime != 0 &&
				GetTickCount() - itor->handle->pingtime >= 4000){

				printf("close at %d (ping timeout)\n", itor->handle->n);

				PacketHandlerData d;
				d.handle = itor->handle;
				d.io = itor->io;
				OnDisconnected(d);

				Logout(itor->handle);
				Session s = {itor->handle,itor->io};
				DeleteConnection(itor->handle->n, inet_ntoa(itor->handle->clntAddr.sin_addr));
				closesocket(itor->handle->hClntSock);
				free(itor->handle);
				free(itor->io);

				itor = DeleteConn(s);
			}
			else
				++itor;
		}

		WaitForSingleObject(hSleepEvent,
							3000);
	}

	return 0;
}