#include "stdafx.h"

#include "photo.h"
#include "Protocol.h"
#include "database.h"
#include <time.h>

bool PushPhotoComment(int db,char *id,PhotoComment *p){
	bool ret = true;
	int q;
	char qm[512];

	sprintf(qm,	"insert into \"photo_comment\" "
		"(\"photo_id\",\"id\",\"time\",\"comment\") "
		"values "
		"(%d,\'%s\',%d,\'%s\');",
		p->photo_id, id,(long)time(NULL), p->comment);

	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);

	sprintf(qm, "select LAST_INSERT_ID();");
	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	p->comment_id = DbGetNumber(q,1);

	DbCloseQuery(q);

	return ret;
}

bool DeletePhotoComment(int db,char *id,int comment_id){
	bool ret = true;
	int q;
	char qm[512];


	sprintf(qm,	"delete from photo_comment where \"photo_id\"=%d and \"id\"=\'%s\';",
		comment_id, id);

	q = DbPrepare(db,qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);
	return ret;
}

PhotoComment *QueryPhotoCommentList(int db,int photo_id,int min, int max,int *cnt){
	int ret = true;
	int q, i;
	char qm[256];
	int len = 0, count =0;
	char *sp;

	sprintf(qm,	"select * from \"photo_comment\" where \"photo_id\"=\'%s\' and "
				"rownum between %d and %d;", photo_id, min, max); 

	q = DbPrepare(db,qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}
	
	PhotoComment *list = NULL;
	list = (PhotoComment*)malloc(sizeof(PhotoComment) * (max-min+1));

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

		list[i].photo_id = photo_id;

		len = DbGetNumber(q, PHOTO_COMMENT_INDEX_PHOTOID);
		list[i].photo_id = len;

		len = DbGetString(q, PHOTO_COMMENT_INDEX_ID, &sp);
		strcpy(list[i].id, sp);

		len = DbGetNumber(q, PHOTO_COMMENT_INDEX_TIME);
		list[i].time = len;

		len = DbGetString(q, PHOTO_COMMENT_INDEX_COMMENT, &sp);
		strcpy(list[i].comment, sp);
    }
	count = i;

	*cnt = count;

CleanUp:
	DbCloseQuery(q);
	return list;
}