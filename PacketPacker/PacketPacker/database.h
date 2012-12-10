#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>
using namespace std;

// facebook.wo.tc : 33000
#define DB_SERVER "facebook.wo.tc"
#define DB_PORT 8001

#define DB_NAME "demodb"
#define DB_USER_ID "sma"
#define DB_USER_PW "040404"

bool DbConnect();
void DbDisconnect();

int DbPrepare(char *query);
void DbCloseQuery(int qid);
bool DbExecute(int qid);
bool DbCursor(int qid,int n);
bool DbFetch(int qid);
int DbGetString(int qid,int col,char **buf);
int DbGetNumber(int qid,int col);
bool DbNext(int qid);

int CubTest();

class Database{
public:
	Database();
	~Database();

	static Database *create(const char *);

	bool open(const char *file);

	void close();

	void *get(const char *name);
	
	void set(const char *name,void *data,int len);

	void save();
private:

	void parse();

private:
	struct Datum{
		void *data;
		int len;
	};

	char fileName[256];
	map<string,Datum> data;
	FILE *fp;
};

#endif // DATABSE_H