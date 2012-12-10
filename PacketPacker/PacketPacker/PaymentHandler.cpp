#include "stdafx.h"
#include "Payment.h"
#include "ServerHandler.h"

#include "Protocol.h"

#include <time.h>

bool PaymentQuery(PacketHandlerData d){
	bool ret = true;

	NetPacket *p;
	p = d.pkt;

	int min,max;
	Payment *pay;

	min = NetGetNumberData(p, "min");
	max = NetGetNumberData(p, "max");

	pay = (Payment*)malloc(sizeof(Payment) * (max-min+1));
	if(pay == NULL){
		ret = false;

		NetPacket *pkt;
		pkt = NetCreatePacket();
		pkt->header.type = PAYMENT_QUERY_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
		NetDisposePacket(pkt,true);

		return false;
	}

	int len = QueryPayment(d.handle->user->id, pay, min, max);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	pkt->header.type = PAYMENT_SIZE;
	NetAddNumberData(pkt, "size", len);
	NetDisposePacket(pkt, true);
	NetSendPacket(d.handle,d.io, pkt);

	for(int i=0;i<len;i++){
		NetPacket *pkt;
		pkt = NetCreatePacket();

		pkt->header.type = PAYMENT_INFO;
		NetAddStringData(pkt, "id", pay[i].id);
		NetAddStringData(pkt, "comment", pay[i].comment);
		NetAddNumberData(pkt, "value", pay[i].value);
		NetAddNumberData(pkt, "time", pay[i].time);

		NetSendPacket(d.handle,d.io, pkt);
		NetDisposePacket(pkt,true);
	}

	free(p);

	return  ret;
}
bool PaymentPush(PacketHandlerData d){
	NetPacket *p;
	p = d.pkt;

	Payment pay;
	bool ret = true;

	pay.time = time(NULL);
	SET(pay.id, d.handle->user->id);
	SET(pay.comment, NetGetStringData(p,"comment"));
	pay.value = NetGetNumberData(p, "value");
	ret = PushPayment(&pay);

	NetPacket *pkt;
	pkt = NetCreatePacket();
	if(ret){
		pkt->header.type = PAYMENT_PUSH_OK;	
	}
	else{
		pkt->header.type = PAYMENT_PUSH_FAILED;
		NetAddStringData(pkt, "reason", REASON_UNKNOWN);
	}
	NetSendPacket(d.handle,d.io, pkt);
	NetDisposePacket(pkt, true);

	return ret;
}