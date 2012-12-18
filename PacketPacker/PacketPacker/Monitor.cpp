#include "stdafx.h"

#include <process.h>

#include <Windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi")

#include "monitor.h"

int hMonitorThread;
HANDLE hSleepEvent;
DWORD processID = GetCurrentProcessId();
HANDLE hProcess;

HWND hConsole;

int MONITOR_INTERVAL = 1000;


bool StartConnectionMonitoring();
void EndConnectionMonitoring();

void UpdateMemoryStatus(){

	PROCESS_MEMORY_COUNTERS pmc;

	if ( GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)) )
	{
		char msg[256];
		sprintf(msg, "SocialMe Server / %dKb", pmc.WorkingSetSize / 1024);
		SetConsoleTitleA(msg);
	}

}
unsigned int __stdcall MonitorThread(void* pComPort){

	while(1){
		WaitForSingleObject(hSleepEvent, MONITOR_INTERVAL);

		UpdateMemoryStatus();
	}
	return 0;
}

void StartMonitoring(){

	SetConsoleTitleA("SocialMe Server");
ReFind:
	hConsole = FindWindowA(NULL, "SocialMe Server");
	if(hConsole == NULL)
		goto ReFind;

	output("console handle %x\n", hConsole);

	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	hSleepEvent = CreateEventA(NULL,false,false,"SoMeSleepingEvent");
	hMonitorThread = _beginthreadex(
		NULL,NULL,MonitorThread,NULL,NULL,NULL);

	StartConnectionMonitoring();
}
void EndMonitoring(){
	CloseHandle( hProcess );
	CloseHandle(hSleepEvent);
	_endthreadex(hMonitorThread);

	EndConnectionMonitoring();
}