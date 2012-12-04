#include "stdafx.h"

#include <Windows.h>
#include <assert.h>

#include "../../Cubrid/include/cas_cci.h"
#pragma comment (lib,"../../Cubrid/lib/cascci")

#include "database.h"


int nDB;

bool DbConnect(){
	nDB = cci_connect(DB_SERVER, DB_PORT, DB_NAME, DB_USER_ID, DB_USER_PW);
	if(nDB >= 0){
		output("DB Connected");
		return true;
	}
	else{
		output("DB Connect failed (%d)", nDB);
		return false;
	}
}
void DbDisconnect(){
	cci_disconnect(nDB, NULL);
}

int CubTest(){
	 int nColCount = 0, nLen = 0;
    int i = 0, nDB = 0, nRet = 0, nReq = 0; 
    char *pszBuff = NULL;
    
    T_CCI_ERROR cciErr;
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