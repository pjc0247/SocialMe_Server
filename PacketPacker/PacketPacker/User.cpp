#include "stdafx.h"

#include "User.h"

#include "database.h"

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


bool RegistUser(int db,char *id,User *u){
	int q;
	char qm[512];
	char *sp = NULL;
	bool ret = true;


	sprintf(qm,	"insert into \"account\" "
		"(\"id\",\"pw\",\"nick\",\"age\") "
		"values "
		"(\'%s\',\'%s\',\'%s\',%d);",
		id,u->pw,u->nick,u->age);

	q = DbPrepare(db,qm);

	if(!DbExecute(q)){
		ret = false;
		return false;
	}
	
	DbCloseQuery(q);
	return ret;
}
bool QueryUser(int db,char *id,User *u){
	int q;
	char qm[64];
	char *sp = NULL;
	int len;
	bool ret = true;
	
	sprintf(qm,"select * from \"account\" where id = \'%s\';", id);
	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	if(!DbNext(q)){
		ret = false;
		goto CleanUp;
	}

	if(u != NULL){
		// ID
		len = DbGetString(q, USER_INDEX_ID, &sp);
		memcpy(u->id,sp,len + 1);

		// PW
		len = DbGetString(q, USER_INDEX_PW, &sp);
		memcpy(u->pw,sp,len + 1);

		// NICK
		len = DbGetString(q, USER_INDEX_NICK, &sp);
		memcpy(u->nick,sp,len + 1);

		// AGE
		len = DbGetNumber(q, USER_INDEX_AGE);
		u->age = len;
	}

CleanUp:;

	DbCloseQuery(q);

	return ret;
}
bool UpdateUser(int db,char *id,User *u){
	bool ret = true;
	int q;
	char qm[128];
	char *sp = NULL;

	char updateItem[128] = {'\0'};

	// NICK
	if(u->nick != NULL){
		strcat(updateItem, "\"nick\"=\'");
		strcat(updateItem, u->nick);
		strcat(updateItem, "\',");
	}
	// AGE
	if(u->age != 0){
		char v[16];
		sprintf(v,"%d,", u->age);
		strcat(updateItem, "\"age\"=");
		strcat(updateItem, v);
	}

	if(strlen(updateItem) != 0){
		updateItem[strlen(updateItem)-1] = '\0';
	}

	sprintf(qm,	"update \"account\" set %s where \"id\"=\'%s\';",
				updateItem, id);

	q = DbPrepare(db,qm);

	if(!DbExecute(q)){
		ret = false;
		return false;
	}

	DbCloseQuery(q);

	return ret;
}


