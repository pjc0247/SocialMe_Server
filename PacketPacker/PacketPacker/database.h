#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>
using namespace std;

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