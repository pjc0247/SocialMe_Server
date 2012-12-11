#include "stdafx.h"

#include "User.h"
#include "Server.h"

#include <map>
#include <string>
using namespace std;

map<string,Session> session;


bool PushSession(void *_handle, void *_io){
	PER_HANDLE_DATA* handle = (PER_HANDLE_DATA*)_handle;
	Session s;
	s.handle = handle;
	s.io = (PER_IO_DATA*)_io;
	session[string(handle->user->id)] = s;
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
		map<string,Session>::iterator itor;

		itor = session.find(string(handle->user->id));
		session.erase(itor);

		DisposeUser(handle->user);
		return true;
	}
	else return false;
}

bool isSessionOpened(char *id){
	map<string,Session>::iterator itor;
	bool find = false;

//	__ENTER(csSession);
	for(itor=session.begin();itor!=session.end();++itor){
		if(itor->first == string(id)){
			find = true;
			break;
		}
	}
//	__LEAVE(csSession);
	return find;
}
void *GetConnectionFromID(char *id){
	if(isSessionOpened(id))
		return (void*)&session[id];
	else return NULL;
}