// testbed.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <locale.h>

#include "NetPacket.h"
#include "../../../PacketPacker/PacketPacker/Protocol.h"

#pragma comment (lib, "ws2_32")

#define SERVER_ADDR "localhost"
#define SERVER_PORT 9919
	
SOCKET hSocket;

DWORD WINAPI ReceiveThread(LPVOID arg){
	while(1){
		char buffer[1025];
		int len;
		len = recv(hSocket,buffer,1024,0);


		if(len == -1){
			MessageBoxA(NULL,"서버와의 연결이 끊겼습니다.","error",
				MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
			break;
		}

		printf("%s\n", buffer);
	}
	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	SOCKADDR_IN servAddr;
	HANDLE hThread;
	hostent *host;

	setlocale(LC_ALL,"kor");

	

	if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		fputs("WSAStartup() error", stderr);


	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if(hSocket == INVALID_SOCKET)
		printf("cannot create socket\n");




	host = gethostbyname(SERVER_ADDR);


	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	/*servAddr.sin_addr.s_addr = inet_addr(
										SERVER_ADDR);*/
	servAddr.sin_addr.s_addr = *(ULONG*)host->h_addr_list[0];
	servAddr.sin_port = htons(
										SERVER_PORT);


	if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR){
		printf("cannot connect to server\n");
		MessageBoxA(NULL,"서버와에 연결할 수 없습니다..","error",
				MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
		return 0;
	}


	char t = TRUE;
	setsockopt(hSocket, IPPROTO_TCP, TCP_NODELAY, &t, sizeof(t));


	//hThread = CreateThread(NULL,NULL,ReceiveThread,NULL,NULL,NULL);

	
	
	char a[32];
	sprintf(a,"QWER");

	NetPacket *p;

	/*p = NetCreatePacket();
	p->header.type = LOGIN_TRY;
	NetAddStringData(p,"id", "pjc0247");
	NetAddStringData(p,"pw", "040404");
	NetSendPacket(hSocket,p);
	NetDisposePacket(p,true);*/

	p = NetCreatePacket();
	p->header.type = MESSAGE_PUSH;
	NetAddStringData(p,"id","pjc");
	NetAddStringData(p,"msg","pjc");
	NetAddNumberData(p,"type", 2);
	NetSendPacket(hSocket,p);
	NetDisposePacket(p,true);

	
	NetPacket *pkt;

	pkt = NetCreatePacket();

	NetRecvPacket(hSocket,pkt);
	NetRecvPacket(hSocket,pkt);
	printf("%d\n", pkt->header.type);

	while(1){
		;
	};


	closesocket(hSocket);
	WSACleanup();


	return 0;

}

