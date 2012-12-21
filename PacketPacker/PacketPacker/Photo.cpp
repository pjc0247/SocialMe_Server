#include "stdafx.h"

#include "photo.h"
#include "Protocol.h"
#include "database.h"

bool PushPhoto(int db,char *id,PhotoPost *p){
	bool ret = true;
	int q;
	char qm[512];

	sprintf(qm,	"insert into \"photo\" "
		"(\"id\",\"time\",\"photo\",\"lat\",\"lon\",\"comment\") "
		"values "
		"(\'%s\',%d,\'%s\',%d,%d,\'%s\');",
		id, p->time, p->photo, p->lat, p->lon, p->comment);

	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);

	sprintf(qm, "select LAST_INSERT_ID();");
	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	p->photo_id = DbGetNumber(q,1);

	DbCloseQuery(q);

	return ret;
}

bool DeletePhoto(int db,char *id,int photo_id){
	bool ret = true;
	int q;
	char qm[512];


	sprintf(qm,	"delete from photo where \"photo_id\" = %d and \"id\" = \'%s\';",
		photo_id, id);

	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);
	return ret;
}

PhotoPost *QueryPhotoList(int db,char *id,int min, int max,int *cnt){
	int ret = true;
	int q, i;
	char qm[256];
	int len = 0, count =0;
	char *sp;

	sprintf(qm,	"select * from \"photo\" where \"id\" = \'%s\' and "
				"rownum between %d and %d;", id, min, max); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}
	
	PhotoPost *list = NULL;
	list = (PhotoPost*)malloc(sizeof(PhotoPost) * (max-min+1));

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

		SET(list[i].id, id);

		len = DbGetNumber(q, PHOTO_INDEX_PHOTOID);
		list[i].photo_id = len;

		len = DbGetNumber(q, PHOTO_INDEX_TIME);
		list[i].time = len;

		len = DbGetString(q, PHOTO_INDEX_PHOTO, &sp);
		strcpy(list[i].photo, sp);

		len = DbGetNumber(q, PHOTO_INDEX_LAT);
		list[i].lat = len;

		len = DbGetNumber(q, PHOTO_INDEX_LON);
		list[i].lon = len;
		
		len = DbGetString(q, PHOTO_INDEX_COMMENT, &sp);
		strcpy(list[i].comment, sp);
    }
	count = i;

	*cnt = count;

CleanUp:
	DbCloseQuery(q);
	return list;
}