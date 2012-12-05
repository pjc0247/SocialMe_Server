#ifndef SERVER_H
#define SERVER_H

#include <WinSock2.h>
#include "ServerController.h"

#define BUFSIZE 1024

typedef struct
{
	SOCKET      hClntSock;
	SOCKADDR_IN clntAddr;
	int n;

	void *packet;

} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct
{
	OVERLAPPED overlapped;
	char buffer[BUFSIZE];
	WSABUF wsaBuf;

	int dataIndex;

	int size;
	int recvState;
	int bytesToRecv;
} PER_IO_DATA, *LPPER_IO_DATA;

int RunServer(int port);

#endif //SERVER_H

