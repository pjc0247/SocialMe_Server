#include "stdafx.h"

#include "Server.h"

#include "blacklist.h"
#include "User.h"

#include <conio.h>

extern unsigned long uptime_st;

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
				/*
				TODO 연결된 클라이언트 리스트 나열
				*/
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
		else if(!strcmp(cmd,"user")){
			char *p1;
			p1 = strtok(NULL," ");
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
			}
			break;
		}
	}
}

bool StartServerController(){
	_beginthreadex(NULL, 0, ControlThread, NULL, 0, NULL);

	return true;
}