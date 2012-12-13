#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

#include <list>
#include <string>
using namespace std;


#include "weblog.h"

#define BUFSIZE 1024
#define SMALLBUF 100
unsigned int __stdcall ClntConnect(void *arg);
char* ContentType(char* file);
void SendData(SOCKET sock, char* ct, char* fileName);
void SendErrorMSG(SOCKET sock);
void ErrorHandling(char *message);

unsigned int hWeblogThread;

extern list<string> logs;

unsigned int __stdcall WeblogThread(void *arg){
	SOCKET hServSock;
	SOCKET hClntSock;

	HANDLE hThread;
	DWORD dwThreadID;

	SOCKADDR_IN servAddr;
	SOCKADDR_IN clntAddr;
	int clntAddrSize;

	hServSock=socket(PF_INET, SOCK_STREAM, 0);   
	if(hServSock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(WEBLOG_PORT);

	if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr))==SOCKET_ERROR)
		ErrorHandling("bind() error");

	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");


	output("Weblog server ready - port %d\n",WEBLOG_PORT);
	while(1){
		clntAddrSize=sizeof(clntAddr);
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAddr, &clntAddrSize);
		if(hClntSock==INVALID_SOCKET)
			ErrorHandling("accept() error");

		hThread = (HANDLE)_beginthreadex(NULL, 0, ClntConnect, (void*)hClntSock, 0, (unsigned *)&dwThreadID);
		if(hThread == 0) {
			printf("cannot create weblog client thread\n");
		}
	}
	return 0;
}

void StartWeblog(){
	hWeblogThread = _beginthreadex(NULL, 0, WeblogThread, NULL, 0, NULL);
}
void StopWeblog(){
	_endthreadex(hWeblogThread);
}


unsigned int __stdcall ClntConnect(void *arg)
{
	SOCKET hClntSock=(SOCKET)arg;
	char buf[BUFSIZE];
	char method[SMALLBUF];
	char ct[SMALLBUF];
	char fileName[SMALLBUF];

	int len;
	
	len = recv(hClntSock, buf, BUFSIZE, 0);
	buf[len] = '\0';

	if(strstr(buf, "HTTP/")==NULL){
		SendErrorMSG(hClntSock);
		closesocket(hClntSock);
		return 1;
	}

	strcpy(method, strtok(buf, " /"));
	if(strcmp(method, "GET"))
		SendErrorMSG(hClntSock);

	strcpy(fileName, strtok(NULL, " /"));
	strcpy(ct, "text/html");

	SendData(hClntSock, ct, fileName); 
	return 0;
}

void SendData(SOCKET sock, char* ct, char* fileName) 
{	
	char protocol[]="HTTP/1.1 200 OK\r\n";
	char server[]="Server:AppDeungE\r\n";
	char cntLen[]="Content-length:";
	char connectionType[]="Connection: close\r\n";
	char cntType[SMALLBUF];
	char buf[BUFSIZE];
	int len;

	if(!strcmp(fileName,"log")){
		sprintf(ct,"text/html");

		string document =
				string("<html>\r\n"
							"<link rel=\"shortcut icon\" type=\"image/x-icon\" href=\"http://118.36.72.114:9919/favicon.icon\"/>\r\n"
							"<head>\r\n"
								"<title>Server Weblog zzz</title>\r\n"
							"</head>\r\n"
							"<body>\r\n");

		list<string>::iterator itor;

		for(itor=logs.begin();itor!=logs.end();++itor){
			document += *itor;
			document += string("<br>");
		}

		document += string("</body>\r\n"
						"</html>\r\n");

		sprintf(cntType, "Content-type:%s;charset=UTF-8\r\n\r\n", ct);
		sprintf(cntLen,"Content-length:%d\r\n", document.length());
		send(sock, protocol, strlen(protocol), 0);
		send(sock, server, strlen(server), 0);
		send(sock, cntLen, strlen(cntLen), 0); 
		send(sock, cntType, strlen(cntType), 0);
		send(sock,document.c_str(), strlen(document.c_str()), 0);
	}
	else{
		sprintf(ct,"text/plain");

		char path[256];
		sprintf(path,"data2\\%s", fileName);
		FILE *fp = fopen(path,"rb");

		if(fp == NULL){
			output("unable to open file %s\n", fileName);
			SendErrorMSG(sock);
			return;
		}
		output("send file %s\n", fileName);

		sprintf(cntType, "Content-type:%s\r\n\r\n", ct);
		send(sock, protocol, strlen(protocol), 0);
		send(sock, server, strlen(server), 0);
		send(sock, cntLen, strlen(cntLen), 0); 
		send(sock, cntType, strlen(cntType), 0);
		
		while(1){
			char buf[128];
			int len;
			
			len = fread(buf,1,128,fp);
			send(sock,buf,len,0);

			if(len < 128) break;
		}

		fclose(fp);
	}

	closesocket(sock);
}

void SendErrorMSG(SOCKET sock)
{	
	char protocol[]="HTTP/1.0 400 Bad Request\r\n";
	char server[]="Server:Best Http Server \r\n";
	char cntLen[]="Content-length:2048\r\n";
	char cntType[]="Content-type:text/html\r\n\r\n";
	char content[]="<html><head><title>NETWORK</title></head>"
		"<body><font size=+5><br>gogogogogogogoggoogogoback"
		"</font></body></html>";

	send(sock, protocol, strlen(protocol), 0);
	send(sock, server, strlen(server), 0);
	send(sock, cntLen, strlen(cntLen), 0); 
	send(sock, cntType, strlen(cntType), 0);
	send(sock, content, strlen(content), 0);

	closesocket(sock);
}

char* ContentType(char* file){ /* Content-Type ±¸ºÐ */
	char extension[SMALLBUF];
	char fileName[SMALLBUF];
	strcpy(fileName, file);
	strtok(fileName, ".");
	strcpy(extension, strtok(NULL, "."));
	if(!strcmp(extension, "html")||!strcmp(extension, "htm")) return "text/html";
	if(!strcmp(extension, "txt")||!strcmp(extension, "c")) return "text/plain";

	return "text/plain";
}


void ErrorHandling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
}