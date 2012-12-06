#include "stdafx.h"

#include <Windows.h>

#include <stdio.h>
#include <stdarg.h>

#include <list>
using namespace std;

#include "log.h"

list<string> logs;

FILE *fp = NULL;


void StartLog();
void EndLog();

class LogStarter {
public:
	LogStarter(){
		StartLog();
	}
	~LogStarter(){
		EndLog();
	}
} _logstarter;

void StartLog(){
	SYSTEMTIME time;

	GetLocalTime(&time);

	char msg[256];
	sprintf(msg,"log\\%d_%d_%d_%d_%d.log",
		time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute);

	fp = fopen(msg,"w");
}
void EndLog(){
	fclose(fp);
}

void ClearLog(){
	logs.clear();
}
void output(const char *fmt, ...)
{
	char buffer[1024];
	char buffer2[1024];
	va_list ap;
	int len;
	SYSTEMTIME time;

	va_start(ap, fmt);
	len = vsprintf(buffer, fmt, ap);
	va_end(ap);

	GetLocalTime(&time);
	sprintf(buffer2,"[%d.%d | %d:%d:%d:%d] %s",
		time.wMonth,time.wDay,
		time.wHour,time.wMinute,time.wSecond,time.wMilliseconds,
		buffer);
					
	printf(buffer2);
	fprintf(fp,buffer2);

	logs.push_back(string(buffer2));
}
