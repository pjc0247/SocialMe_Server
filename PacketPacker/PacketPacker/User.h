#ifndef USER_H
#define USER_H

struct User{
	char id[16];
	char pw[64];
	char nick[16];
	char comment[128];

	int age;
	int sex;
	char location[128];
	char job[32];
	char hobby[64];
};

enum UserColumnIndex{
	USER_INDEX_ID = 1,
	USER_INDEX_PW,
	USER_INDEX_NICK,
	USER_INDEX_AGE
};

User *CreateUser();
void DisposeUser(User *u);

bool RegistUser(int db,char *id,User *u);
bool QueryUser(int db,char *id,User *u);
bool UpdateUser(int db,char *id,User *u);

/*
 * Session API
 */
bool Logout(void *handle);
bool IsLoggedIn(void *handle);
bool PushSession(void *handle, void *_io);
void *GetConnectionFromID(char *id);

#endif // USER_H