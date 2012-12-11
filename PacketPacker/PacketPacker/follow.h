#ifndef FOLLOW_H
#define FOLLOW_H

struct FollowList{
	char *id;
	int count;
};

bool IsFollowing(char *src,char *dst);
bool IsFollowed(char *src,char *dst);
bool Follow(char *src,char *dst);
bool Unfollow(char *src,char *dst);
int FollowingCount(char *id);
int FollowedCount(char *id);

bool QueryFollowerList(char *id,FollowList *list,int min, int max);
bool QueryFollowingList(char *id,FollowList *list,int min, int max);

#endif // FOLLOW_H