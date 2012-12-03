#include "stdafx.h"

#include "Server.h"
#include "blacklist.h"

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
	DWORD RecvBytes;
	int i;
	DWORD Flags;

	SetConsoleTitleA("SocialMe Server");

	// Windows Socket의 초기화
	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		output("startup error\n");


	DWORD dwProcessor;
	GetSystemInfo(&SystemInfo);
	dwProcessor = SystemInfo.dwNumberOfProcessors;
	hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, (dwProcessor * 2));

	for(i=0; i < (dwProcessor * 2); i++) {
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
		memcpy(&(PerHandleData->clntAddr), &clntAddr, addrLen);

		CreateIoCompletionPort((HANDLE)hClntSock,hCompletionPort,(DWORD)PerHandleData,0);

		PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));           
		PerIoData->wsaBuf.len = BUFSIZE;
		PerIoData->wsaBuf.buf = PerIoData->buffer;
		Flags=0;

		WSARecv(PerHandleData->hClntSock,
			&(PerIoData->wsaBuf),
			1,                
			&RecvBytes,                                 
			&Flags,
			&(PerIoData->overlapped),
			NULL
			);        
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
		GetQueuedCompletionStatus(hCompletionPort,
			&BytesTransferred,
			(LPDWORD)&PerHandleData,
			(LPOVERLAPPED*)&PerIoData,
			INFINITE);

		// 0 바이트 수신함 -> 연결 종료됨
		if(BytesTransferred == 0)
		{
			output("closed %d\n",PerHandleData->n);

			closesocket(PerHandleData->hClntSock);
			free(PerHandleData);
			free(PerIoData);
			continue;             
		}             
		PerIoData->wsaBuf.len = BytesTransferred;
		PerIoData->wsaBuf.buf[BytesTransferred] = '\0';

		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));
		PerIoData->wsaBuf.len=BUFSIZE;
		PerIoData->wsaBuf.buf=PerIoData->buffer;

		flags=0;
		WSARecv(PerHandleData->hClntSock,
			&(PerIoData->wsaBuf),
			1,
			NULL,
			&flags,
			&(PerIoData->overlapped),
			NULL
			);      

	}
	return 0;
}