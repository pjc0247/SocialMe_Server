#ifndef MESSAGE_H
#define MESSAGE_H


struct Message{
	int id;
	int time;
	char sender[16];
	char receiver[16];
	int type;
	char msg[160];
	void *content;
};

enum MessageType{
	MESSAGE_TEXT,
	MESSAGE_IMG
};
enum MessageColumnIndex{
	MESSAGE_INDEX_ID=1,
	MESSAGE_INDEX_TIME,
	MESSAGE_INDEX_SENDER,
	MESSAGE_INDEX_RECEIVER,
	MESSAGE_INDEX_TYPE,
	MESSAGE_INDEX_CONTENT
};
enum MessageQueryResult{
	RESULT_FAILED,
	RESULT_NEXT,
	RESULT_END
};

int QueryMessage(char *receiver,Message *m);
bool PushMessage(Message *msg);
bool DeleteMessage(char *id,int mid);

#endif //MESSAGE_H