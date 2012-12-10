#include "stdafx.h"
#include <stdio.h>

#include "blacklist.h"

#include <string>
#include <map>
using namespace std;

map<string,bool> blacklist;

void LoadBlacklist(){
	FILE*fp;
	fp = fopen(BLACKLIST_PATH,"r");
	if(fp == NULL) return;

	while(1){
		if(feof(fp)) break;
		char ip[128];
		fscanf(fp,"%s\n", ip);
		blacklist[string(ip)] = true;
	}

	fclose(fp);

	output("blacklist loaded\n");
}
void UnloadBlacklist(){
	blacklist.clear();

	output("blacklist unloaded\n");
}
void ClearBlacklist(){
	blacklist.clear();

	output("blacklist cleared\n");
}
bool isBlockedIP(const char *ip){
	//return true;
	return blacklist[string(ip)];
}
void EnumBlacklist(){
	map<string,bool>::iterator itor;

	output(">> blacklist\n");
	for(itor=blacklist.begin();itor!=blacklist.end();++itor){
		output(">>   %s\n", 
			(itor->first.c_str()));
	}
	output("\n");
}