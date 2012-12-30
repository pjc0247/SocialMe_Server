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

bool PushPhoto(int db,char *id,PhotoPost *p);
bool DeletePhoto(int db,char *id,int photo_id);
PhotoPost *QueryPhotoList(int db,char *id,int min, int max,int *cnt);

bool QueryLikePhoto(int db,char *id,int photo_id);
bool LikePhoto(int db,char *id,int photo_id);
bool DislikePhoto(int db,char *id,int photo_id);

#endif // PHOTO_H