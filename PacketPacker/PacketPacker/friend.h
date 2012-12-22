#ifndef FRIEND_H
#define FRIEND_H

struct FriendList{
	char *id;
	int count;
};

bool IsFriended(int db,char *src,char *dst);
bool Friend(int db,char *src,char *dst);
bool Unfriend(int db,char *src,char *dst);

int QueryFriendCount(int db,char *id);
bool QueryFriendList(int db,char *id,FriendList *list,int min, int max);

#endif //FRIEND_H