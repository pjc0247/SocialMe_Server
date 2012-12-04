#ifndef USER_H
#define USER_H

struct User{
	char id[16];
	char nick[16];
	char comment[128];

	int age;
	int sex;
	char location[128];
	char job[32];
	char hobby[64];
};

#endif // USER_H