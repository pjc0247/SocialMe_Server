#include "stdafx.h"
#include "Payment.h"

#include "database.h"



bool QueryPayment(char *receiver,Payment **_p,int min, int max){
	int ret = true;
	int q;
	char qm[512];
	char *sp;
	int len;

	sprintf(qm,	"select * from \"payment\" where receiver_id = \'%s\' and"
				"rownum between %d and %d;", receiver, min, max); 

	q = DbPrepare(qm);
	
	if(!DbExecute(q)){
		printf("Execute failed\n");

		ret = false;
		goto CleanUp;
	}

	for(int i=0;;i++)
    {
        if ( DbCursor(q,1) )
            break;
       
        DbFetch(q);
       
		Payment *p;
		p = _p[i];

        // ID
		len = DbGetString(q, PAYMENT_INDEX_ID, &sp);
		memcpy(p->id, sp, len + 1);

		// TIME
		len = DbGetNumber(q, PAYMENT_INDEX_TIME);
		p->time = len;

		// SENDER
		len = DbGetString(q, PAYMENT_INDEX_COMMENT, &sp);
		memcpy(p->comment,sp,len + 1);

		// RECEIVER
		len = DbGetNumber(q, PAYMENT_INDEX_VALUE);
		p->value = len;
    }

	if(!DbNext(q)){
		ret = false;
		goto CleanUp;
	}

	if(p != NULL){
		
	}

CleanUp:;

	DbCloseQuery(q);

	return ret;
	
	return true;
}
bool PushPayment(Payment *p){
	bool ret = true;
	int q;
	char qm[512];

	sprintf(qm,	"insert into \"payment\" "
		"(\"id\",\"time\",\"comment\",\"value\") "
		"values "
		"(\'%s\',%d,\'%s\',%d);",
		p->id, p->time, p->comment, p->value);

	q = DbPrepare(qm);

	ret = DbExecute(q);

	if(ret == false){
		return false;
	}

	DbCloseQuery(q);
	return ret;
}