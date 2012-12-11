#include "stdafx.h"

#include "User.h"
#include "Server.h"

#include <map>
#include <string>
using namespace std;

map<string,PER_HANDLE_DATA*> session;


bool PushSession(void *_handle){
	PER_HANDLE_DATA* handle = (PER_HANDLE_DATA*)_handle;
	session[string(handle->user->id)] = (PER_HANDLE_DATA*)handle;
	return true;
}
bool IsLoggedIn(void *_handle){
	PER_HANDLE_DATA *handle = (PER_HANDLE_DATA  *)_handle;
	if(handle->user != NULL)
		return true;
	else return false;
}

bool Logout(void *_handle){
	PER_HANDLE_DATA *handle = (PER_HANDLE_DATA  *)_handle;
	if(IsLoggedIn(handle)){
		map<string,PER_HANDLE_DATA*>::iterator itor;

		itor = session.find(string(handle->user->id));
		session.erase(itor);

		DisposeUser(handle->user);
		return true;
	}
	else return false;
}

