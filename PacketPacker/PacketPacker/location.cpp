#include "stdafx.h"

#include "location.h"
#include "database.h"

#include <time.h>

bool PushLocation(int db,char *id,int lat,int lon){
	bool ret = true;
	int q;
	char qm[512];

	sprintf(qm,	"insert into \"location\" "
		"(\"id\",\"time\",\"lat\",\"long\") "
		"values "
		"(\'%s\',%d,%d,%d);",
		id, time(NULL),lat, lon);

	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);
	return ret;
}
Location *QueryLocation(int db,char *id, int min, int max, int *cnt){
	int ret = true;
	int q, i;
	char qm[256];
	int len = 0, count =0;
	char *sp;

	sprintf(qm,	"select * from \"location\" where \"id\" = \'%s\' "
				"rownum between %d and %d;", id, min, max); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}
	
	Location *list = NULL;
	list = (Location*)malloc(sizeof(Location) * (max-min+1));

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

		len = DbGetNumber(q, LOCATION_INDEX_TIME);
		list[i].time = len;

		len = DbGetNumber(q, LOCATION_INDEX_LATITUDE);
		list[i].lat = len;

		len = DbGetNumber(q, LOCATION_INDEX_LONGITUDE);
		list[i].lon = len;
    }
	count = i;

	*cnt = count;

CleanUp:
	DbCloseQuery(q);
	return list;
}