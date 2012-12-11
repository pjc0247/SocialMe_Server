/*
 *  SocialMe Server Project
 *				written by pjc0247
 *	
 *	Data structs, Server Main
 */

#ifndef SERVER_H
#define SERVER_H

#include <WinSock2.h>
#include "ServerController.h"

#include "User.h"

#define SERVER_PORT 9919
#define BUFSIZE 1024

enum OverlappedFlags{
	ASYNC_SEND = 10,
	ASYNC_RECV
};

typedef struct OVERLAPPEDEX : OVERLAPPED
{
	DWORD flag;
} OVERLAPPEDEX;

typedef struct
{
	SOCKET      hClntSock;
	SOCKADDR_IN clntAddr;
	int n;
	bool disconnected;

	User *user;

	void *packet;

} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct
{
	OVERLAPPEDEX overlapped;
	char buffer[BUFSIZE];
	WSABUF wsaBuf;

	int dataIndex;

	int size;
	int recvState;
	int bytesToRecv;
} PER_IO_DATA, *LPPER_IO_DATA;

typedef struct {
	PER_HANDLE_DATA *handle;
	PER_IO_DATA *io;
}Session;

int RunServer(int port);

#endif //SERVER_H

