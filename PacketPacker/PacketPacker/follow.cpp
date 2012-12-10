#include "stdafx.h"

#include "follow.h"

#include "database.h"

bool IsFollowing(char *src,char *dst){
	int ret = true;
	int q, i;
	char qm[256];
	char *sp;
	int len;

	sprintf(qm,	"select * from \"follow\" where \"follower\" = \'%s\' and \"followed\" = \'%s\'", src,dst); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	len = DbResultCount(q);

CleanUp:
	DbCloseQuery(q);
	return len-1;
}
bool IsFollowed(char *src,char *dst){
	int ret = true;
	int q, i;
	char qm[256];
	char *sp;
	int len;

	sprintf(qm,	"select * from \"follow\" where \"follower\" = \'%s\' and \"followed\" = \'%s\'", dst,src); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	len = DbResultCount(q);

CleanUp:
	DbCloseQuery(q);
	return len-1;
}

bool Follow(char *src,char *dst){
	bool ret = true;
	int q;
	char qm[128];

	if(IsFollowing(src,dst))
		return false;

	sprintf(qm,	"insert into \"follow\" "
		"(\"follower\",\"followed\") "
		"values "
		"(\'%s\',\'%s\');",
		src,dst);

	q = DbPrepare(qm);

	ret = DbExecute(q);

	if(ret == false){
		goto CleanUp;
	}

CleanUp:
	DbCloseQuery(q);

	return ret;
}
bool Unfollow(char *src,char *dst){
	bool ret = true;
	int q;
	char qm[128];

	if(!IsFollowing(src,dst))
		return false;

	sprintf(qm, "delete from follow where \"follower\" = \'%s\' and \"followed\" = \'%s\'",
			src, dst);
	q = DbPrepare(qm);
	ret = DbExecute(q);

	if(ret == false){
		goto CleanUp;
	}

CleanUp:
	DbCloseQuery(q);

	return ret;
}
int FollowCount(char *id){
	int ret = true;
	int q, i;
	char qm[128];
	char *sp;
	int len;

	sprintf(qm,	"select * from \"follow\" where \"follower\" = \'%s\'", id); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	len = DbResultCount(q);

CleanUp:
	DbCloseQuery(q);

	return len;
}