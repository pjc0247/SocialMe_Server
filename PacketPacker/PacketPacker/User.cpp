#include "stdafx.h"

#include "User.h"

#include <stdlib.h>
#include <memory.h>

User *CreateUser(){
	User *u;
	u = (User*)malloc(sizeof(User));
	memset(u,0,sizeof(User));
	return u;
}
void DisposeUser(User *u){
	free(u);
}


