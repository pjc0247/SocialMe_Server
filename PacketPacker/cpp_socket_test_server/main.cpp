#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
 
#include <iostream>
#include <vector>
#include <map>
#include <string>

#include <conio.h>

#include "protocol.h"
#include "blacklist.h"
#include "weblog.h"
#include "ping.h"

#include "database.h"

using namespace std;

#pragma comment (lib,"ws2_32")

#define BUFSIZE (1024 * 20)
#define SERVER_PORT 9916 

enum VAR_TYPE {
		VAR_INT,
		VAR_CHAR,
		VAR_FLOAT,
		VAR_STRING
		};
typedef struct
{
	union {
		int i;
		float f;
		char c;
		char *s;
	} data;
	unsigned char type;
} VAR, *LPVAR;
	
typedef struct
{
	char id[32];
	char pw[128];
	char nick[32];

	int x,y,speed;

	bool joined;
	char area[32];

	Database *db;
	map<string,LPVAR> var;
} USER_DATA, *LPUSER_DATA;
typedef struct
{
	SOCKET      hClntSock;
	SOCKADDR_IN clntAddr;
	int n;
	USER_DATA user;

	char ping;

	int fileReceived;
	char fileName[256];
	int fileLength;
	bool fileRecv;
	FILE *filePointer;
	int fileWritten;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct
{
	OVERLAPPED overlapped;
	char buffer[BUFSIZE];
	WSABUF wsaBuf;
} PER_IO_DATA, *LPPER_IO_DATA;

typedef struct
{
	void (*handler)(int ,char *);
} HANDLER;


unsigned int __stdcall ControlThread(void *arg);
unsigned int __stdcall CompletionThread(void* CompletionPortIO);
void RegistHandler(int code,void (*cb)(int,char *));
void ParsePacket(int w,char *msg,int msgLength);
void Send(int w,int p,char *msg);
void SendBinary(int w,int p,char *data,int len);
void SendFile(int w,char *file);
void SendVarNew(int w,int t,char *name,int type);
void SendVarDelete(int w,int t,char *name);
void SendVarChange(int w,int t,char *name,int v);
void SendVarChange(int w,int t,char *name,float v);
void SendVarChange(int w,int t,char *name,char v);
void SendVarChange(int w,int t,char *name,char *v);

//vector<PER_HANDLE_DATA*> clients;
map<int,PER_HANDLE_DATA*> clients;
map<int,HANDLER> handler;
map<string,int> session;

map<string,vector<PER_HANDLE_DATA*>> area;

CRITICAL_SECTION csClients;
CRITICAL_SECTION csSession;
CRITICAL_SECTION csArea;

unsigned long uptime_st;
unsigned int ping_update_st;

//#define __SYNCRONIZE
#ifdef	__SYNCRONIZE
	#define __ENTER(cs) EnterCriticalSection(&cs)
	#define __LEAVE(cs) LeaveCriticalSection(&cs)
#else
	#define __ENTER(cs) // do nothing
	#define __LEAVE(cs) // do nothing
#endif//__SYNCRONIZE


/*
새 클라이언트와 연결이 수립되었을 때
false를 리턴하면 연결 거부.
*/
bool onConnect(int w,char *ip){
	
	// 밴 목록에 있는 아이피인지 조사한다.
	if(isBlockedIP(ip))
		return false;

	return true;
}

/*
클라이언트와의 연결이 끊어졌을 때
*/
void onDisconnect(int w,char *ip){
	char *id = clients[w]->user.id;
	output("%s logged out\n", id);
}

/*
서버를 초기화한다.
*/
void Initialize(){

	LoadBlacklist();
	StartWeblog();
	StartPingUpdate();

	InitializeCriticalSection(&csSession);
	InitializeCriticalSection(&csClients);
	InitializeCriticalSection(&csArea);

	uptime_st = GetTickCount64();
	ping_update_st = GetTickCount64();

	//area[string("main")].push_back(NULL);
}
/*
서버를 종료한다.
*/
void Quit(){
	DeleteCriticalSection(&csSession);
	DeleteCriticalSection(&csClients);
	DeleteCriticalSection(&csArea);

	UnloadBlacklist();
	StopWeblog();
	StopPingUpdate();
}


int main(int argc, char** argv)
{
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

	SetConsoleTitleA("TankOnline Server");

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
	servAddr.sin_port=htons(SERVER_PORT);


	if(bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
		output("bind error\n");
		return 1;
	}

	if(listen(hServSock, 5) == SOCKET_ERROR) {
		output("listen error\n");
		return 1;
	}

	output("server ready - %dcore/%dthread\n",dwProcessor,dwProcessor*2);


	// 프로그램 초기화
	Initialize();

	// 서버 컨트롤러 스레드를 생성
	_beginthreadex(NULL, 0, ControlThread, NULL, 0, NULL);

	int N = 0;
	do
	{      
		SOCKET hClntSock;
		SOCKADDR_IN clntAddr;        
		int addrLen=sizeof(clntAddr);
		bool ret;

		// 새 클라이언트 연결을 수립한다.
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAddr, &addrLen);  

		char t = TRUE;
		setsockopt(hClntSock, IPPROTO_TCP, TCP_NODELAY, &t, sizeof(t));

		output("connected... %s - %d\n", inet_ntoa(clntAddr.sin_addr),N);

		// onConnect 핸들러를 호출한다.
		ret = onConnect(N,inet_ntoa(clntAddr.sin_addr));
		if(ret == false){
			// 연결 거부됨
			output("connection denied\n");
			continue;
		}

		PerHandleData=(LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));          
		PerHandleData->hClntSock=hClntSock;
		PerHandleData->n = N;
		PerHandleData->fileRecv = false;
		memset(&PerHandleData->user,0,sizeof(USER_DATA));
		memcpy(&(PerHandleData->clntAddr), &clntAddr, addrLen);

		CreateIoCompletionPort((HANDLE)hClntSock,hCompletionPort,(DWORD)PerHandleData,0);

		PerIoData = (LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));
		memset(&(PerIoData->overlapped), 0, sizeof(OVERLAPPED));           
		PerIoData->wsaBuf.len = BUFSIZE;
		PerIoData->wsaBuf.buf = PerIoData->buffer;
		Flags=0;

		clients[N] = PerHandleData;

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
	Quit();

	return 0;
}

void ControlShell(){
	output("--ControlShell\n");
	while(1){
		char line[256];
		char *cmd;

		output("<< ");
		gets(line);

		cmd = strtok(line," ");
		
		if(!strcmp(cmd,"exit")){
			output(">> exit control shell\n");
			break;
		}
		else if(!strcmp(cmd,"builddate")){
			output(">> %s\n", __DATE__);
		}
		else if(!strcmp(cmd,"clear")){
			char *p1;
			p1 = strtok(NULL," ");

			if(!strcmp(p1,"log")){
				ClearLog();
			}
			else if(!strcmp(p1,"blacklist")){
				ClearBlacklist();
			}
		}
		else if(!strcmp(cmd,"reload")){
			char *p1;
			p1 = strtok(NULL," ");

			if(!strcmp(p1,"blacklist")){
				UnloadBlacklist();
				LoadBlacklist();
				output(">> blacklist reloaded\n");
			}
		}
		else if(!strcmp(cmd,"list")){
			char *p1;
			p1 = strtok(NULL," ");

			if(!strcmp(p1,"connection")){
				map<int,PER_HANDLE_DATA*>::iterator itor;
			
				output(">> connection list\n");
				for(itor=clients.begin();itor!=clients.end();++itor){
					output(">>   %d - %s\n", 
						itor->first,inet_ntoa(itor->second->clntAddr.sin_addr));
				}
				output("\n");
			}
			else if(!strcmp(p1,"blacklist")){
				EnumBlacklist();
			}
		}
		else if(!strcmp(cmd,"uptime")){
			char *p1;
			p1 = strtok(NULL," ");

			if(p1 == NULL){
				unsigned long tick = GetTickCount64();
				tick = tick - uptime_st;
				tick /= 1000;
				printf("%d\n", tick);
				output(">> server uptime : %dday %dhour %dminute %dsecond\n",
							tick / 60 / 60 / 24,
							tick % (3600 * 24) / 60 / 60,
							tick % 3600 / 60,
							tick % 60);
			}
			else if(!strcmp(p1,"system")){
				unsigned long tick = GetTickCount64();
				tick /= 1000;
				output(">> system uptime : %dday %dhour %dminute %dsecond\n",
							tick / 60 / 60 / 24,
							tick % (3600 * 24) / 60 / 60,
							tick % 3600 / 60,
							tick % 60);
			}
			
		}
	}
}
unsigned int __stdcall ControlThread(void *arg){
	output("Server ControlPanel Ready\n");
	while(1){
		int ch;
		ch = _getch();

		switch(ch){
		case 'b':
			{
				char msg[256];

				output("message : ");
				gets(msg);

				//Broadcast(CHAT_SYSTEM,msg);
			}
			break;
		case 'i':
			{
				output("TankOnline Server, %s\n", __DATE__);
			}
			break;
		case 's':
			{
				ControlShell();
			}
			break;
		case 'q':
			{
				exit(0);
			}
			break;
		case 'p':
			{
				char t[32];
				sprintf(t,"%d", GetTickCount64());
				//Broadcast(PING,t);
			}
			break;
		}
	}
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

			onDisconnect(
				PerHandleData->n,
				inet_ntoa(PerHandleData->clntAddr.sin_addr)
				);

			closesocket(PerHandleData->hClntSock);
			free(PerHandleData);
			free(PerIoData);
			continue;             
		}             
		PerIoData->wsaBuf.len = BytesTransferred;
		PerIoData->wsaBuf.buf[BytesTransferred] = '\0';

		ParsePacket(PerHandleData->n,
			PerIoData->wsaBuf.buf,
			PerIoData->wsaBuf.len);

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
void Send(int w,int p,char *msg){
	char packet[1024];
	sprintf(packet,"%d:%s\r\n",
							p,msg);
	DWORD sent;
	WSABUF buf;
	buf.buf = packet;
	buf.len = strlen(packet);
	WSASend(clients[w]->hClntSock,&buf,1,&sent,0,NULL,NULL);
}
void SendBinary(int w,int p,char *data,int len){
	char *packet;
	packet = (char *)malloc(sizeof(char) * len + 7);
	sprintf(packet,"%d:%s\r\n",
							p,data);
	DWORD sent;
	WSABUF buf;
	buf.buf = packet;
	buf.len = len + 6;
	WSASend(clients[w]->hClntSock,&buf,1,&sent,0,NULL,NULL);
}
void SendFile(int w,char *file){
	//FILE *fp = fopen(file,"rb");
	HANDLE fp = CreateFileA(
						file,
						GENERIC_READ,
						FILE_SHARE_READ,
						NULL,
						OPEN_EXISTING,
						0,
						NULL);
	char msg[32];

	if(fp == INVALID_HANDLE_VALUE){
		output("failed to send file\n");
		Send(w,FILE_ERR,"error");
		return;
	}
	sprintf(msg,"%d", GetFileSize(fp,NULL));
	Send(w,FILE_LENGTH,msg);
	for(int i=strlen(file);i>=0;i--){
		if(file[i] == '\\'){
			sprintf(msg,"%s", file+i+1);
			output("send file %s\n", msg);
			Send(w,FILE_NAME,msg);
			break;
		}
	}
	while(1){
		char buffer[128];
		DWORD dwRead;
		int sent;

		ReadFile(fp,buffer,128,&dwRead,NULL);
		sent = send(clients[w]->hClntSock,buffer,dwRead,0);

		if(sent == -1){
			printf("send failed - %s\n", file);
//			printf("send aborted %s\n", fileName);
			break;
		}
		if(dwRead != 128)
			break;
	}

	CloseHandle(fp);
}

void ParsePacket(int w,char *msg,int msgLength){
	int read;
	vector<char *> token;
	token.push_back(strtok(msg,"\r\n"));

	// 파일 수신중이면
	if(clients[w]->fileRecv == true){
		// 남은 써야 될 양이 현재 받은 패킷보다 클 때
		if((clients[w]->fileLength - clients[w]->fileWritten) > msgLength){
			fwrite(msg,sizeof(char),msgLength,clients[w]->filePointer);
			clients[w]->fileWritten += msgLength;
			return;
		}
		// 남은 써야 될 양이 현재 받은 패킷보다 작음 -> 남은만큼 쓰고 나머지 파싱
		else{
			fwrite(msg,sizeof(char),
					clients[w]->fileLength - clients[w]->fileWritten,
					clients[w]->filePointer);
			fclose(clients[w]->filePointer);
			msg = msg + clients[w]->fileLength - clients[w]->fileWritten;
			clients[w]->fileRecv = false;
			clients[w]->fileReceived = true;

			if(clients[w]->fileLength - clients[w]->fileWritten == msgLength)
				return;
		}
	}

	// 1차 파싱
	//   \r\n을 기준으로 패킷을 나눈다.
	char msg2[1024];
	memcpy(msg2,msg,sizeof(char) * msgLength);
	while(1){
		char *tok;
		tok = strtok(NULL,"\r\n");
		if(tok != NULL){
			token.push_back(tok);
		}
		else break;
	}

	// 2차 파싱
	//   코드번호와 메세지를 나눈다.
	read = 0;
	for(int i=0;i<token.size();i++){
		int code;
		char *msg = NULL;
		for(int j=0;j<strlen(token[i]);j++){
			if(token[i][j] == ':'){
				token[i][j] = '\n';
				code = atoi(token[i]);
				msg = token[i] + j+1;
				break;
			}
		}
		read += strlen(token[i]) + 2;

		// 파일 길이를 전송받았을 때
		if(code == FILE_LENGTH){
			clients[w]->fileLength = atoi(msg);
			output("file length : %d\n", clients[w]->fileLength);
		}
		// 파일 이름을 전송받았을 때 -> 파일 수신을 시작한다.
		else if(code == FILE_NAME){
			sprintf(clients[w]->fileName,msg);
			output("file name : %s\n", clients[w]->fileName);

			clients[w]->filePointer = fopen(clients[w]->fileName,"wb");
			if(clients[w]->filePointer == NULL){
				output("failed to create file\n");
			}
			clients[w]->fileWritten = 0;
			clients[w]->fileRecv = true;

			if(i != token.size()-1){
				ParsePacket(w,msg2+read,msgLength-read);
				break;
			}
			break;
		}

		if(handler[code].handler != NULL){
			handler[code].handler(w,msg);
		}
	}
}
void RegistHandler(int code,void (*cb)(int,char *)){
	handler[code].handler = cb;
}