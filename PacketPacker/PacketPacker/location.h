#ifndef LOCATION_H
#define LOCATION_H

struct Location{
	char id[16];
	int lat,lon;
	int time;
};

enum LocationColumnIndex{
	LOCATION_INDEX_ID=1,
	LOCATION_INDEX_TIME,
	LOCATION_INDEX_LATITUDE,
	LOCATION_INDEX_LONGITUDE
};

bool PushLocation(int db,char *id,int lat,int lon);
Location *QueryLocation(int db,int *cnt);

#endif // LOCATION_H