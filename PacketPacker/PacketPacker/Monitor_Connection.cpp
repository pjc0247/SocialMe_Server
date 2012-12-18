#include "stdafx.h"

#include "monitor.h"
#include "resource.h"

#include <Windows.h>

extern HWND hConsole;
HWND hConnDlg, hConnList;
HANDLE hConnThread;

int conn_cnt = 0;

CRITICAL_SECTION csConn;

DWORD WINAPI ConnThread(LPVOID arg);
BOOL CALLBACK ConnCallback(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam);

bool StartConnectionMonitoring(){

	__INIT(&csConn);

	hConnThread = CreateThread(NULL,0,
		ConnThread,NULL,NULL,NULL);

	return true;
}
void EndConnectionMonitoring(){

	__DELETE(&csConn);

	TerminateThread(hConnThread,0);
}

void PushConnection(int n,char *ip){
	char msg[128];
	sprintf(msg, "%d - %s", n, ip);

	__ENTER(&csConn);
	conn_cnt ++;
	SendMessageA(hConnList, LB_ADDSTRING, NULL, (LPARAM)msg);

	sprintf(msg, "%d¸í", conn_cnt);
	SetDlgItemTextA(hConnDlg, IDC_CONNCNT, msg);
	__LEAVE(&csConn);
}
void DeleteConnection(int n,char *ip){
	__ENTER(&csConn);
	char msg[128];
	char dmsg[128];
	sprintf(dmsg, "%d - %s", n,ip);
	for(int i=0;i<conn_cnt;i++){
		SendMessageA(hConnList, LB_GETTEXT, (WPARAM)i, (LPARAM)msg);
		if(!strcmp(msg, dmsg)){
			SendMessageA(hConnList, LB_DELETESTRING, (WPARAM)i, NULL);
			break;
		}
	}
	conn_cnt --;

	sprintf(msg, "%d¸í", conn_cnt);
	SetDlgItemTextA(hConnDlg, IDC_CONNCNT, msg);
	__LEAVE(&csConn);
}

DWORD WINAPI ConnThread(LPVOID arg){
	DialogBoxA(
		GetModuleHandle(NULL),
		MAKEINTRESOURCEA(IDD_CONN),
		NULL,ConnCallback);
	return 0;
}

BOOL CALLBACK ConnCallback(HWND hDlg,UINT iMessage,WPARAM wParam,LPARAM lParam)
{
	switch(iMessage)
	{
	case WM_INITDIALOG:
		hConnDlg = hDlg;
		hConnList = GetDlgItem(hDlg, IDC_LIST);

		{
			RECT crt, drt;
			GetWindowRect(hConsole, &crt);
			GetWindowRect(hDlg, &drt);

			SetWindowPos(hDlg, NULL,
				crt.right, crt.top,
				drt.right, drt.bottom, 0);
		}

		return TRUE;
	}
	return FALSE;
}