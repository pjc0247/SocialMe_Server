#include "stdafx.h"

#include "NetPacket.h"
#include "User.h"

bool RegistTry(NetPacket *p){

	User *user; 

	user = CreateUser();

	printf("ASDF %s\n",p->data[0].data);

	SET(user->id, NetGetStringData(p,"id"));
	SET(user->pw, NetGetStringData(p,"pw"));
	SET(user->nick, NetGetStringData(p,"nick"));
	user->age = NetGetNumberData(p,"age");

	printf("%d \n", user->age);

	RegistUser(NetGetStringData(p,"id"),
				user);

	DisposeUser(user);

	return true;
}
