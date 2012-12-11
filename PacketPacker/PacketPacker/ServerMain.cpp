#include "stdafx.h"

#include "Server.h"
#include "blacklist.h"

#include "NetPacket.h"
#include "ServerHandler.h"

unsigned long uptime_st;

unsigned int __stdcall CompletionThread(void* pComPort);

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


	DWORD dwProcessor;
	GetSystemInfo(&SystemInfo);
	dwProcessor = SystemInfo.dwNumberOfProcessors;
	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, (dwProcessor * 2));

	for(i=0; i < 1; i++) {
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

	if(listen(hServSock, 5) == SOCKET_ERROR) {
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
			output("connection denied\n");
			continue;
		}

		PerHandleData=(LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));          
		PerHandleData->hClntSock=hClntSock;
		PerHandleData->n = N;
		PerHandleData->packet = NetCreatePacket();
		PerHandleData->user = NULL;
		memcpy(&(PerHandleData->clntAddr), &clntAddr, addrLen);
		memset(PerHandleData->packet,0,sizeof(NetPacket));

		CreateIoCompletionPort((HANDLE)hClntSock,hCompletionPort,(DWORD)PerHandleData,0);

		PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));           

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

	while(1)
	{
		bool ret = GetQueuedCompletionStatus(hCompletionPort,
			&BytesTransferred,
			(LPDWORD)&PerHandleData,
			(LPOVERLAPPED*)&PerIoData,
			INFINITE);

		//printf("recv %d %d %d \n",ret ,  BytesTransferred, PerIoData->overlapped.flag);
		if(PerIoData->overlapped.flag != ASYNC_RECV)
			continue;

		// 0 바이트 수신함 -> 연결 종료됨
		if(BytesTransferred == 0)
		{
			output("close %d\n", PerHandleData->n);
			// 소켓 종료
			closesocket(PerHandleData->hClntSock);
			free(PerHandleData);
			free(PerIoData);

			continue;             
		} 
		//printf("Recv %d bytes\n", BytesTransferred);
		//printf("Recv State : %d, %d\n", PerIoData->recvState,PerIoData->bytesToRecv);


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
			case NET_RECV_HEADER:
				memcpy(&p->header,PerIoData->buffer,PerIoData->size);

				// 메모리 할당
				p->data = (NetPacketData*)malloc(
					sizeof(NetPacketData) * p->header.count);
				if(p->data == NULL){
					output("malloc failed\n");
				}

				PerIoData->recvState = NET_RECV_DATANAME;
				NetRecv(PerHandleData,PerIoData, MAX_NAME_LENGTH);
				break;
			case NET_RECV_DATANAME:
				memcpy(&p->data[PerIoData->dataIndex].name,PerIoData->buffer,MAX_NAME_LENGTH);
				PerIoData->recvState = NET_RECV_DATASIZE;
				NetRecv(PerHandleData,PerIoData, sizeof(int));
				break;
			case NET_RECV_DATASIZE:
				memcpy(&p->data[PerIoData->dataIndex].size,PerIoData->buffer,sizeof(int));
				
				p->data[PerIoData->dataIndex].data = malloc(p->data[PerIoData->dataIndex].size);

				PerIoData->recvState = NET_RECV_DATA;
				NetRecv(PerHandleData,PerIoData, p->data[PerIoData->dataIndex].size);
				break;
			case NET_RECV_DATA:
				
				memcpy(p->data[PerIoData->dataIndex].data,PerIoData->buffer,PerIoData->size);
				
				PerIoData->dataIndex ++;

				// Recieve Complete
				if(PerIoData->dataIndex >= p->header.count){
					PacketHandlerData d;
					d.handle = PerHandleData;
					d.io = PerIoData;
					d.pkt = p;
					ProcessPacket(d);

					// fix : memory leak
					NetDisposePacket(p,false);
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
	}
	return 0;
}