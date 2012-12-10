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

int MONITOR_INTERVAL = 1000;



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
	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	hSleepEvent = CreateEventA(NULL,false,false,"SoMeSleepingEvent");
	hMonitorThread = _beginthreadex(
		NULL,NULL,MonitorThread,NULL,NULL,NULL);
}
void EndMonitoring(){
	CloseHandle( hProcess );
	CloseHandle(hSleepEvent);
	_endthreadex(hMonitorThread);
}