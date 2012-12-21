#ifndef FOLLOW_H
#define FOLLOW_H

struct FollowList{
	char *id;
	int count;
};

bool IsFollowing(int db,char *src,char *dst);
bool IsFollowed(int db,char *src,char *dst);
bool Follow(int db,char *src,char *dst);
bool Unfollow(int db,char *src,char *dst);
int FollowingCount(int db,char *id);
int FollowedCount(int db,char *id);

bool QueryFollowerList(int db,char *id,FollowList *list,int min, int max);
bool QueryFollowingList(int db,char *id,FollowList *list,int min, int max);

#endif // FOLLOW_H