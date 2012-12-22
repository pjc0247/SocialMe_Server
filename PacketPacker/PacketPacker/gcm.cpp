#include "stdafx.h"

#include <Windows.h>
#include <WinInet.h>

#include <stdarg.h>

#include <string>
using namespace std;

#pragma comment (lib,"wininet")

void GCM_Testbed(){
	HANDLE hConnect = InternetOpenA("GCM", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if(hConnect == NULL)
	{
		return;
	}

	HANDLE hHttp = InternetConnectA(hConnect, "android.googleapis.com", INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, NULL);
	if(hHttp == NULL)
	{
		InternetCloseHandle(hConnect);
		hHttp = NULL;
		return;
	}

	HANDLE hReq = HttpOpenRequestA(hHttp, "POST", "/gcm/send", "HTTP/1.1", NULL, NULL, 
		INTERNET_FLAG_NO_COOKIES|INTERNET_FLAG_SECURE|INTERNET_FLAG_IGNORE_CERT_CN_INVALID|INTERNET_FLAG_IGNORE_CERT_DATE_INVALID, 0);
	if(hReq == NULL)
	{
		InternetCloseHandle(hConnect);
		InternetCloseHandle(hHttp);
		return;
	}

	string strHeader;
	strHeader += "Content-Type:application/x-www-form-urlencoded;charset=UTF-8";
	strHeader += "\r\n";
	strHeader += "Authorization:key=";
	strHeader += "AIzaSyBnEaX8e6hwBme6tLY6aQQ7znUzF3vzYLA";
	strHeader += "\r\n\r\n";

	char szHead[8192];
	int nLen;
	va_list va;
	//va_start(va, strHeader.c_str());
	//nLen=_vsnprintf_s(szHead, 8190, strHeader.c_str(), va);
	//va_end(va);
	sprintf(szHead, strHeader.c_str());
	HttpAddRequestHeadersA(hReq , szHead, strlen(szHead), HTTP_ADDREQ_FLAG_REPLACE|HTTP_ADDREQ_FLAG_ADD);

	char szSendData[1000];
	memset(szSendData, 0, 1000);
	sprintf_s(szSendData, 1000,
		"collapse_key=msg&"
		"time_to_live=108&"
		"delay_while_idle=1&"
		"%s&"
		"registration_id=%s",
		"data.test=asdf", "APA91bEWxTv1F_zBmBm6NYOqxiB1jQaoTDlSh1JqKOpGGqgvxK5p6b3kOgYO2ZsZQsQmdiEAHLER0TULIxPOi54H-EQvCHHq91evWSC4DakvKkyVPx5Rnozxw1gAq_3N8aAavlfAsHssyv2SrPqJrogZW-k_YoxHfA");

	HttpSendRequest(hReq , NULL,  0, (LPVOID)szSendData, strlen(szSendData));

	DWORD dwStatusCode;
	DWORD dwStatusCodeLen;

	HttpQueryInfoA(hReq ,
		HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER,
		&dwStatusCode, &dwStatusCodeLen, NULL);

	char szBuffer[1024];
	DWORD dwRead;
	string strResult;

	while (::InternetReadFile (hReq, szBuffer, 1024, &dwRead) && dwRead > 0)
	{
		szBuffer [dwRead] = 0;
		strResult += szBuffer;
	}

	printf("%s\n", strResult.c_str());
}