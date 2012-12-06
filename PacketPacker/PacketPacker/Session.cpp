#include "stdafx.h"

#include "User.h"
#include "Server.h"


bool IsLoggedIn(void *_handle){
	PER_HANDLE_DATA *handle = (PER_HANDLE_DATA  *)_handle;
	if(handle->user != NULL)
		return true;
	else return false;
}

bool Logout(void *_handle){
	PER_HANDLE_DATA *handle = (PER_HANDLE_DATA  *)_handle;
	if(IsLoggedIn(handle)){
		DisposeUser(handle->user);
		return true;
	}
	else return false;
}

