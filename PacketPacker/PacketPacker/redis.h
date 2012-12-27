#ifndef REDIS_H
#define REDIS_H

#define REDIS_SERVER				"127.0.0.1"
#define REDIS_PORT					6379
#define REDIS_CONNECTION_TIMEOUT	2000


bool RedisConnect(char *ip, int port);
void RedisDisconnect();
void RedisSet(char *name, char *value);
void RedisGet(char *name, char *value);
void RedisDelete(char *name);

#endif //REDIS_H