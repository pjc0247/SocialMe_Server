#include "stdafx.h"

#include "location.h"
#include "database.h"

#include <time.h>

bool PushLocation(int db,char *id,int lat,int lon){
	bool ret = true;
	int q;
	char qm[512];

	sprintf(qm,	"update \"account\" set \"lat\"=%d,\"lon\"=%d "
				"where \"id\"=\'%s\';",
				lat,lon,id);

	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);
	return ret;
}
Location *QueryLocation(int db,int *cnt){
	int ret = true;
	int q, i;
	char qm[256];
	int len = 0, count =0;
	char *sp;

	//SELECT * FROM follow f,location l WHERE f.followed=l.id AND f.follower='anz4176';

	sprintf(qm,	"select \"id\",\"lat\",\"lon\" from \"account\""
				""); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	//count = DbResultCount(q);

	count = 100;

	Location *list = NULL;
	list = (Location*)malloc(sizeof(Location) * (count));

	if(list == NULL)
		goto CleanUp;

	for(i=0;;i++)
    {
        if(!DbCursor(q,1))
            break;
        if(!DbFetch(q))
			break;
		
		//PhotoPost *list;
		//list = &_list[i];

		int len;

		len = DbGetString(q, 1, &sp);
		memcpy((void*)list[i].id,sp,len + 1);

		len = DbGetNumber(q, 2);
		list[i].lat = len;

		len = DbGetNumber(q, 3);
		list[i].lon = len;
		
		count ++;
    }
	count = i;

	*cnt = count;

CleanUp:
	DbCloseQuery(q);
	return list;
}
