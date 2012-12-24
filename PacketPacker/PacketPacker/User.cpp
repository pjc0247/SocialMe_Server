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
		"(\"id\",\"pw\",\"nick\",\"age\",\"sex\",\"birthday\",\"job\") "
		"values "
		"(\'%s\',\'%s\',\'%s\',%d,%d,%d,\'%s\');",
		id,u->pw,u->nick,u->age,
		u->sex, u->birthday, u->job);

	q = DbPrepare(db,qm);

	if(!DbExecute(q)){
		ret = false;
		return false;
	}
	
	DbCloseQuery(q);

	sprintf(qm,	"insert into \"account2\" "
		"(\"id\",\"android_id\",\"facebook_id\",\"nateon_id\") "
		"values "
		"(\'%s\',\'%s\',\'%s\',\'%s\');",
		id,"","","");

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

		// LAT
		len = DbGetNumber(q, USER_INDEX_LATITUDE);
		u->lat = len;

		// LON
		len = DbGetNumber(q, USER_INDEX_LONGITUDE);
		u->lon = len;

		// SEX
		len = DbGetNumber(q, USER_INDEX_SEX);
		u->sex = len;

		// BIRTH
		len = DbGetNumber(q, USER_INDEX_BIRTHDAY);
		u->birthday = len;

		// JOB
		len = DbGetString(q, USER_INDEX_JOB, &sp);
		memcpy(u->job,sp,len + 1);

		// COMMENT
		len = DbGetString(q, USER_INDEX_COMMENT, &sp);
		memcpy(u->comment,sp,len + 1);
	}

CleanUp:;

	DbCloseQuery(q);

	return ret;
}
bool UpdateUser(int db,char *id,User *u){
	bool ret = true;
	int q;
	char qm[512] = {'\0'};
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
	// lat
	if(u->lat != 0){
		char v[16];
		sprintf(v,"%d,", u->lat);
		strcat(updateItem, "\"lat\"=");
		strcat(updateItem, v);
	}
	// lon
	if(u->lon != 0){
		char v[16];
		sprintf(v,"%d,", u->lon);
		strcat(updateItem, "\"lon\"=");
		strcat(updateItem, v);
	}
	// sex
	if(u->sex != 0){
		char v[16];
		sprintf(v,"%d,", u->sex);
		strcat(updateItem, "\"sex\"=");
		strcat(updateItem, v);
	}
	// birth
	if(u->birthday != 0){
		char v[16];
		sprintf(v,"%d,", u->birthday);
		strcat(updateItem, "\"birthday\"=");
		strcat(updateItem, v);
	}
	// job
	if(u->job != NULL){
		char v[35];
		sprintf(v,"\'%s\',", u->job);
		strcat(updateItem, "\"job\"=");
		strcat(updateItem, v);
	}
	// post
	if(u->post != 0){
		char v[16];

		strcat(updateItem, "\"post\"=\"post\"");
		if(u->post >= 1)
			sprintf(v, "+%d", u->post);
		else
			sprintf(v, "%d", u->post);
		strcat(updateItem,v);
	}

	if(strlen(updateItem) != 0){
		updateItem[strlen(updateItem)-1] = '\0';
	}

	sprintf(qm,	"update \"account\" set %s where \"id\"=\'%s\';",
				updateItem, id);

	printf("%s\n", qm);

	q = DbPrepare(db,qm);

	if(!DbExecute(q)){
		ret = false;
		return false;
	}

	DbCloseQuery(q);

	return ret;
}
int QueryUserPost(int db,char *id){
	int ret = true;
	int q;
	char qm[128];
	int len;

	sprintf(qm,	"select \"post\" from \"account\" where \"id\"=\'%s\'", id); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		len = -1;
		goto CleanUp;
	}

	len = DbGetNumber(q, 1);

CleanUp:
	DbCloseQuery(q);

	return len;
}

bool QueryUserComment(int db,char *id,char *comment){
	int ret = true;
	int q;
	char qm[128];
	int len;
	char *sp;

	sprintf(qm,	"select \"comment\" from \"account\" where \"id\"=\'%s\'", id); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		len = -1;
		goto CleanUp;
	}

	len = DbGetString(q, 1, &sp);
	memcpy(comment,sp,len + 1);

CleanUp:
	DbCloseQuery(q);
	return ret;
}
bool UpdateUserComment(int db,char *id,char *comment){
	int ret = true;
	int q;
	char qm[512];
	int len;
	char *sp;

	sprintf(qm,	"update \"account\" set \"comment\"=\'%s\' where \"id\"=\'%s\'",
			comment,id); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		len = -1;
		goto CleanUp;
	}

CleanUp:
	DbCloseQuery(q);
	return ret;
}

bool QueryUserFacebook(int db,char *id,char *fb){
	int q;
	char qm[256];
	char *sp = NULL;
	int len;
	bool ret = true;
	
	sprintf(qm,"select \"facebook_id\" from \"account2\" where id=\'%s\';", id);
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

	// FACEBOOK
	len = DbGetString(q, 1, &sp);
	memcpy(fb,sp,len + 1);

CleanUp:;
	DbCloseQuery(q);
	return ret;
}
bool QueryUserNateon(int db,char *id,char *nt){
	int q;
	char qm[256];
	char *sp = NULL;
	int len;
	bool ret = true;
	
	sprintf(qm,"select \"nateon_id\" from \"account2\" where id=\'%s\';", id);
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

	// NATEON
	len = DbGetString(q, 1, &sp);
	memcpy(nt,sp,len + 1);

CleanUp:;
	DbCloseQuery(q);
	return ret;
}

bool UpdateUserFacebook(int db,char *id,char *fb){
	int q;
	char qm[256];
	char *sp = NULL;
	int len;
	bool ret = true;
	
	sprintf(qm,"update \"account2\" set \"facebook_id\"=\'%s\' where id=\'%s\';",fb,id);
	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

CleanUp:;
	DbCloseQuery(q);
	return ret;
}
bool UpdateUserNateon(int db,char *id,char *nt){
	int q;
	char qm[256];
	char *sp = NULL;
	int len;
	bool ret = true;
	
	sprintf(qm,"update \"account2\" set \"nateon_id\"=\'%s\' where id=\'%s\';",nt,id);
	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

CleanUp:;
	DbCloseQuery(q);
	return ret;
}