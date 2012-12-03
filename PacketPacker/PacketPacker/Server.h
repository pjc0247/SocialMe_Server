#ifndef SERVER_H
#define SERVER_H

#include "NetPacket.h"

#include "ServerController.h"

#define BUFSIZE 1024

typedef struct
{
	SOCKET      hClntSock;
	SOCKADDR_IN clntAddr;
	int n;

} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct
{
	OVERLAPPED overlapped;
	char buffer[BUFSIZE];
	WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;


int RunServer(int port);

#endif //SERVER_H