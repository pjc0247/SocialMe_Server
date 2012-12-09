#include "stdafx.h"

#include "database.h"

#include "Message.h"


int QueryMessage(char *receiver,Message *m){
	bool ret = true;
	int q;
	char qm[512];
	char *sp;
	int len;

	sprintf(qm,"select * from \"message\" where id = \'%s\';", receiver); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	if(!DbNext(q)){
		ret = false;
		goto CleanUp;
	}

	if(m != NULL){
		// ID
		len = DbGetNumber(q, MESSAGE_INDEX_ID);
		m->id = len;

		// TIME
		len = DbGetString(q, MESSAGE_INDEX_TIME, &sp);
		memcpy(m->time,sp,len + 1);

		// SENDER
		len = DbGetString(q, MESSAGE_INDEX_SENDER, &sp);
		memcpy(m->sender,sp,len + 1);

		// RECEIVER
		len = DbGetString(q, MESSAGE_INDEX_RECEIVER, &sp);
		memcpy(m->receiver,sp,len + 1);

		// TYPE
		len = DbGetNumber(q, MESSAGE_INDEX_TYPE);
		m->type = len;

		// CONTENT
		len = DbGetString(q, MESSAGE_INDEX_CONTENT, &sp);
		memcpy(m->content,sp,len + 1);
	}

CleanUp:;

	DbCloseQuery(q);

	return ret;
	
	return true;
}

bool PushMessage(Message *msg){
	bool ret = true;
	int q;
	char qm[512];

	sprintf(qm,	"insert into \"message\" "
		"(\"time\",\"sender_id\",\"receiver_id\",\"type\",\"content\") "
		"values "
		"(%d,\'%s\',\'%s\',%d,\'%s\');",
		msg->time, msg->sender, msg->receiver,
		msg->type, msg->msg);

	printf("%s\n", qm);

	q = DbPrepare(qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);
	return ret;
}