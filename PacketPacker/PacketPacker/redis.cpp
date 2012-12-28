#include "stdafx.h"

#include "../../Redis/include/redispp.h"
#include "redis.h"

#include <string>

using namespace std;
using namespace redispp;

Connection *redis;

bool RedisConnect(char *ip, int _port){
	char port[32];
	sprintf(port, "%d", _port);
	redis = new Connection(ip, port);
	return true;
}
void RedisDisconnect(){
	delete redis;
}

void RedisSet(char *name, char *value){
	redis->set(string(name), string(value));
}
void RedisGet(char *name,char *v){
	sprintf(v,((string)redis->get(string(name))).c_str());
}
void RedisDelete(char *name){
	redis->del(string(name));
}
bool RedisExist(char *name){
	return (bool)redis->exists(string(name));
}
