#include "stdafx.h"

#include "blacklist.h"
#include "database.h"
#include "payment.h"

#include "Server.h"

#define SERVER_PORT 9919


int _tmain(int argc, _TCHAR* argv[])
{
	LoadBlacklist();

	DbConnect();
	
	Payment *p;

	p = (Payment *)malloc(sizeof(Payment) * 5);

	int len = QueryPayment("pjc0247", p, 1, 5);
	printf("%d \n", len);

	printf("%d\n", p[3].value);

	RunServer(SERVER_PORT);
	
	DbDisconnect();

	return 0;
}


