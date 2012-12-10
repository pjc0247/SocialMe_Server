#ifndef FOLLOW_H
#define FOLLOW_H

bool IsFollowing(char *src,char *dst);
bool IsFollowed(char *src,char *dst);
bool Follow(char *src,char *dst);
bool Unfollow(char *src,char *dst);
int FollowCount(char *id);

#endif // FOLLOW_H