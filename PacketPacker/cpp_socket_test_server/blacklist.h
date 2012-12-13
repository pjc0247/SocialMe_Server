#ifndef BLACKLIST_H
#define BLACKLIST_H

void LoadBlacklist();
void UnloadBlacklist();
void ClearBlacklist();
bool isBlockedIP(const char *ip);

void EnumBlacklist();

#endif // BLACKLIST_H