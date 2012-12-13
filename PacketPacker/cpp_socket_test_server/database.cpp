#include "stdafx.h"

#include "database.h"

Database::Database(){
	fp = NULL;
}
Database::~Database(){
	close();
}
void Database::close(){
	if(fp != NULL)
		fclose(fp);
	fp = NULL;
}
bool Database::open(const char *file){
	output("open database - %s\n", file);

	sprintf(fileName,file);

	fp = fopen(file,"rb+");
	if(fp == NULL)
		fp = fopen(file,"wb+");
	else{
		parse();
		close();
	}
	return true;
}
Database *Database::create(const char *file){
	Database *cd = new Database();
	cd->open(file);
	return cd;
}
void Database::parse(){
	int n;

	fread(&n,sizeof(int),1,fp);

	/*
	DataStruct

	count of data (4b)
	--Data
	----length of name(4b)
	----name
	----data type(4b)
	----length of data(4b)
	----data
	*/

	for(int i=0;i<n;i++){
		int len,type;
		char name[32];
		void *data;
		fread(&len,sizeof(int),1,fp);
		fread(name,sizeof(char),len,fp);
		name[len] = '\0';
		fread(&type,sizeof(int),1,fp);
		fread(&len,sizeof(int),1,fp);	
		data = malloc(len);
		fread(data,sizeof(len),1,fp);

		Datum d;
		d.len = len;
		d.data = data;
		this->data[string(name)] = d;

		printf("load - %s\n", name);
	}
}
void *Database::get(const char *name){
	//return this->data[string(name)].data;
	static int zero = 0;
	map<string,Datum>::iterator itor;
	for(itor=data.begin();itor!=data.end();++itor){
		if(itor->first == string(name)){
			return (void*)itor->second.data;
		}
	}
	return (void*)&zero;
}
void Database::set(const char *name,void *data,int len){
	Datum d;
	d.data = data;
	d.len = len;

	this->data[string(name)] = d;
}
void Database::save(){
	map<string,Datum>::iterator it = data.begin();

	if(fp == NULL){
		open(fileName);
	}

	fseek(fp,0,SEEK_SET);

	int size = data.size();
	fwrite(&size,sizeof(int),1,fp);

	for(;it!=data.end();++it){
		int len,type;
		char name[32];
		void *data;

		len = it->first.length();
		fwrite(&len,sizeof(int),1,fp);

		sprintf(name,it->first.c_str());
		fwrite(name,sizeof(char),len,fp);

		type = 1;
		fwrite(&type,sizeof(int),1,fp);

		len = it->second.len;
		fwrite(&len,sizeof(int),1,fp);
		
		data = it->second.data;
		fwrite(data,sizeof(len),1,fp);
	}

	close();
}