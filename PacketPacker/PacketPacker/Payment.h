#ifndef PAYMENT_H
#define PAYMENT_H

struct Payment{
	char id[16];			// ������ ���̵�
	int time;				// ���� �ð�
	char comment[128];		// ��볻��
	int value;				// ������
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