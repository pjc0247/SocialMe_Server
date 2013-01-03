#include "stdafx.h"

#include "friend.h"

#include "database.h"

bool IsFriended(int db,char *src,char *dst){
	int ret = true;
	int q;
	char qm[256];
	int len;

	sprintf(qm,	"select * from friend f1,friend f2 "
				"where (f1.\"friend\"=\'%s\' and f1.\"id\"=\'%s\') and "
				"f2.\"friend\"=\'%s\' and f2.\"id\"=\'%s\';",
				src,dst,dst,src); 

	q = DbPrepare(db,qm);
	
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

bool Friend(int db,char *src,char *dst){
	bool ret = true;
	int q;
	char qm[256];

	if(IsFriended(db,src,dst))
		return false;

	sprintf(qm,	"insert into \"friend\" "
		"(\"id\",\"friend\") "
		"values "
		"(\'%s\',\'%s\');",
		src,dst);
	q = DbPrepare(db,qm);
	ret = DbExecute(q);
	if(ret == false){
		goto CleanUp;
	}
	DbCloseQuery(q);

	/*sprintf(qm,	"insert into \"friend\" "
		"(\"id\",\"friend\") "
		"values "
		"(\'%s\',\'%s\');",
		dst,src);
	q = DbPrepare(db,qm);
	ret = DbExecute(q);
	if(ret == false){
		goto CleanUp;
	}
	DbCloseQuery(q);*/

	sprintf(qm, "update \"account\" set \"friends\"=\"friends\"+1 where \"id\"=\'%s\'",
		src, dst);
	q = DbPrepare(db,qm);
	ret = DbExecute(q);
	if(ret == false){
		goto CleanUp;
	}


CleanUp:
	DbCloseQuery(q);

	return ret;
}
bool Unfriend(int db,char *src,char *dst){
	bool ret = true;
	int q;
	char qm[256];

	if(!IsFriended(db,src,dst))
		return false;

	sprintf(qm, "delete from friend where "
				"(\"id\"=\'%s\' and \"friend\"=\'%s\')",
				src, dst,src,dst);
	q = DbPrepare(db,qm);
	ret = DbExecute(q);
	if(ret == false){
		goto CleanUp;
	}
	DbCloseQuery(q);

	sprintf(qm, "update \"account\" set \"friends\"=\"friends\"-1 where \"id\"=\'%s\'",
			src, dst);
	q = DbPrepare(db,qm);
	ret = DbExecute(q);
	if(ret == false){
		goto CleanUp;
	}

CleanUp:
	DbCloseQuery(q);

	return ret;
}
int QueryFriendCount(int db,char *id){
	int ret = true;
	int q;
	char qm[128];
	int len;

	sprintf(qm,	"select \"friends\" from \"account\" where \"id\" = \'%s\'", id); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	len = DbGetNumber(q, 1);

CleanUp:
	DbCloseQuery(q);

	return len;
}

bool QueryFriendList(int db,char *id,FriendList *list,int min, int max){
	int ret = true;
	int q, i;
	char qm[256];
	int len = 0;
	char *sp;

	sprintf(qm,	"select \"friend\" from \"friend\" where \"id\"=\'%s\' and "
				"rownum between %d and %d;", id, min, max); 

	q = DbPrepare(db,qm);
	
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

bool QueryWaitingList(int db,char *id,FriendList *list,int min, int max){
	int ret = true;
	int q, i;
	char qm[256];
	int len = 0;
	char *sp;

	sprintf(qm,	"select \"friend\" from \"friend\" where \"id\"=\'%s\' and "
				"rownum between %d and %d;", id, min, max); 

	q = DbPrepare(db,qm);
	
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
