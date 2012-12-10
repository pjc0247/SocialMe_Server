#include "stdafx.h"

#include <Windows.h>
#include <assert.h>

#include "../../Cubrid/include/cas_cci.h"
#pragma comment (lib,"../../Cubrid/lib/cascci")

#include "database.h"


int nDB;
T_CCI_ERROR cciErr;

bool DbConnect(){	
	nDB = cci_connect(DB_SERVER, DB_PORT, DB_NAME, DB_USER_ID, DB_USER_PW);
	if(nDB >= 0){
		output("DB Connected\n");

		char buf[128];
		cci_get_db_version(nDB,buf,128);

		output("DB Version : Cubrid %s\n", buf);

		return true;
	}
	else{
		output("DB Connect failed (%d)\n", nDB);

		char msg[256];
		cci_get_error_msg(nDB,NULL,msg,256);
		output(msg);

		return false;
	}
}
void DbDisconnect(){
	cci_disconnect(nDB, NULL);
}

int DbPrepare(char *query){
	int qid;
	qid = cci_prepare(nDB, query, 0, &cciErr);
	return qid;
}
void DbCloseQuery(int qid){
	cci_close_req_handle(qid);
}

bool DbExecute(int qid){
	int ret = cci_execute(qid, 0, 0, &cciErr);

	if(ret >= 0)
		return true;
	else
		return false;
}
int DbResultCount(int qid){
	int len = 1;
	
	cci_cursor(qid, 1, CCI_CURSOR_FIRST, &cciErr);

	while(true){
		if (!DbCursor(qid,1) )
            break;
        if(!DbFetch(qid))
			break;
		
		len ++;
	}

	cci_cursor(qid, 1, CCI_CURSOR_FIRST, &cciErr);

	return len;
}

bool DbCursor(int qid,int n){
	int ret = cci_cursor(qid, n, CCI_CURSOR_CURRENT, &cciErr);

	if(ret == CCI_ER_NO_MORE_DATA)
		return false;
	return true;
}
bool DbFetch(int qid){
	int ret = cci_fetch(qid, &cciErr);
	
	if(ret >= 0)
		return true;
	return false;
}
bool DbNext(int qid){
	bool ret = true;
	ret = DbCursor(qid,1);
	if(ret == false)
		return false;
	ret = DbFetch(qid);
	return ret;
}
int DbGetString(int qid,int col,char **buf){
	int len, ret;
	ret = cci_get_data(qid, col, CCI_A_TYPE_STR, buf, &len);
	if(ret >= 0)
		return len;
	else return -1;
}
int DbGetNumber(int qid,int col){
	int len, ret;
	int num;
	ret = cci_get_data(qid, col, CCI_A_TYPE_INT, &num, &len);
	if(ret >= 0)
		return num;
	else return -1;
}



int CubTest(){
	 int nColCount = 0, nLen = 0;
    int i = 0, nDB = 0, nRet = 0, nReq = 0; 
    char *pszBuff = NULL;
    
    
    T_CCI_COL_INFO *pcciCol;
    T_CCI_SQLX_CMD cciCmdType;
    
    assert((nDB = cci_connect(DB_SERVER, DB_PORT, DB_NAME, DB_USER_ID, DB_USER_PW)) >= 0);
    assert((nReq = cci_prepare(nDB, "SELECT * FROM athlete", 0, &cciErr)) >= 0);
    assert((pcciCol = cci_get_result_info(nReq, &cciCmdType, &nColCount)) != NULL);
        
    for (i = 1; i <= nColCount; i++)
    {
        printf("%s\t", CCI_GET_RESULT_INFO_NAME(pcciCol, i));
    }

    printf("\n");

    assert(cci_execute(nReq, 0, 0, &cciErr) >= 0);

    while (1) 
    {
        if ( cci_cursor(nReq, 1, CCI_CURSOR_CURRENT, &cciErr) == CCI_ER_NO_MORE_DATA )
            break;
       
        assert(cci_fetch(nReq, &cciErr) >= 0);
       
        for (i = 1; i<= nColCount; i++)
        {
            assert(cci_get_data(nReq, i, CCI_A_TYPE_STR, &pszBuff, &nLen) >= 0);
            printf("%s\t", pszBuff);
        }
        printf("\n");
    }

    assert(cci_close_req_handle(nReq) >= 0);
    assert(cci_disconnect(nDB, &cciErr) >= 0);
    
    return 0;

   
}