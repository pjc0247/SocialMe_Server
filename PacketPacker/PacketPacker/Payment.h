#ifndef PAYMENT_H
#define PAYMENT_H

struct Payment{
	char id[16];			// 결제자 아이디
	int time;				// 결제 시각
	char comment[128];		// 사용내역
	int value;				// 결제량
};

enum PaymentColumnIndex{
	PAYMENT_INDEX_ID=1,
	PAYMENT_INDEX_TIME,
	PAYMENT_INDEX_COMMENT,
	PAYMENT_INDEX_VALUE
};

int QueryPayment(char *receiver,Payment *_p,int min, int max);
bool PushPayment(Payment *p);

#endif //PAYMENT_H