#include "stdafx.h"

#include "database.h"

#include "User.h"
#include "Message.h"


int QueryMessage(char *receiver,Message *m){
	int ret = RESULT_NEXT;
	int q;
	char qm[512];
	char *sp;
	int len;

	sprintf(qm,"select * from \"message\" where receiver_id = \'%s\';", receiver)                       ; 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = RESULT_FAILED;
		goto CleanUp;
	}

	if(!DbNext(q)){
		ret = RESULT_FAILED;
		goto CleanUp;
	}

	if(m != NULL){
		// ID
		len = DbGetNumber(q, MESSAGE_INDEX_ID);
		m->id = len;

		// TIME
		len = DbGetNumber(q, MESSAGE_INDEX_TIME);
		m->time = len;

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
		memcpy(m->msg,sp,len + 1);
	}

	DbCloseQuery(q);

	sprintf(qm, "delete from message where \"message_id\" = %d",
			m->id);
	q = DbPrepare(qm);
	ret = DbExecute(q);
	

CleanUp:;

	DbCloseQuery(q);

	return ret;
}

bool DeleteMessage(char *id,int mid){
	int q;
	char qm[256];
	bool ret;

	sprintf(qm, "delete from message where \"message_id\" = %d and \"receiver\" = \'%s\'",
			mid,id);
	q = DbPrepare(qm);
	ret = DbExecute(q);

	DbCloseQuery(q);

	return ret;
}
#include "../../Cubrid/include/cas_cci.h"
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

	q = DbPrepare(qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}
	
	DbCloseQuery(q);

	sprintf(qm, "select LAST_INSERT_ID();");
	q = DbPrepare(qm);

	ret = DbExecute(q);

	msg->id = DbGetNumber(q,1));

	DbCloseQuery(q);

	return ret;
}