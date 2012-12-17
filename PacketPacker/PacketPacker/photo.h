#ifndef PHOTO_H
#define PHOTO_H

struct PhotoPost{
	int photo_id;
	char id[16];
	int time;
	char photo[16];
	int lat,lon;
	char comment[281];
};

enum PhotoColumnIndex{
	PHOTO_INDEX_PHOTOID=1,
	PHOTO_INDEX_ID,
	PHOTO_INDEX_TIME,
	PHOTO_INDEX_PHOTO,
	PHOTO_INDEX_LAT,
	PHOTO_INDEX_LON,
	PHOTO_INDEX_COMMENT
};

bool PushPhoto(char *id,PhotoPost *p);
bool DeletePhoto(char *id,int photo_id);
int QueryPhotoList(char *id,PhotoPost *list,int min, int max);

#endif // PHOTO_H