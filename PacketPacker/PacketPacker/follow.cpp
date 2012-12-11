#include "stdafx.h"

#include "follow.h"

#include "database.h"



bool IsFollowing(char *src,char *dst){
	int ret = true;
	int q, i;
	char qm[256];
	char *sp;
	int len;

	sprintf(qm,	"select * from \"follow\" where \"follower\" = \'%s\' and \"followed\" = \'%s\';", src,dst); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	len = DbResultCount(q);
	printf("%d\n", len);

CleanUp:
	DbCloseQuery(q);
	return len;
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
int FollowingCount(char *id){
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
int FollowedCount(char *id){
	int ret = true;
	int q, i;
	char qm[128];
	char *sp;
	int len;

sprintf(qm,	"select * from \"follow\" where \"followed\" = \'%s\'", id); 

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

bool QueryFollowerList(char *id,FollowList *list,int min, int max){
	int ret = true;
	int q, i;
	char qm[256];
	char *sp;
	int len = 0;

	sprintf(qm,	"select \"follower\" from \"follow\" where \"followed\" = \'%s\' and "
				"rownum between %d and %d;", id, min, max); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}
	
	len = max-min+1;
	list->id = (char*)malloc(sizeof(char) * 16 * len);

	for(i=0;;i++)
    {
        if ( !DbCursor(q,1) )
            break;
        if(!DbFetch(q))
			break;
		
       
		len = DbGetString(q, 1, &sp);
		memcpy(list->id + i*16, sp, len + 1);
    }
	list->count = i;

CleanUp:
	DbCloseQuery(q);
	return true;
}

bool QueryFollowingList(char *id,FollowList *list,int min, int max){
	int ret = true;
	int q, i;
	char qm[256];
	char *sp;
	int len = 0;

	sprintf(qm,	"select \"followed\" from \"follow\" where \"follower\" = \'%s\' and "
				"rownum between %d and %d;", id, min, max); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}
	
	len = max-min+1;
	list->id = (char*)malloc(sizeof(char) * 16 * len);

	for(i=0;;i++)
    {
        if( !DbCursor(q,1) )
            break;
        if(!DbFetch(q))
			break;
		
       
		len = DbGetString(q, 1, &sp);
		memcpy(list->id + i*16, sp, len + 1);
    }
	list->count = i;

CleanUp:
	DbCloseQuery(q);
	return true;
}