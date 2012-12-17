#include "stdafx.h"

#include "ServerHandler.h"

#include "Protocol.h"

bool ProcessPacket(PacketHandlerData d){

	switch(d.pkt->header.type){
		
	case REGIST_TRY:
		RegistTry(d);
		break;

	case LOGIN_TRY:
		LoginTry(d);
		break;
	case LOGOUT_TRY:
		LogoutTry(d);
		break;

	case USER_UPDATE:
		UserUpdate(d);
		break;
	case USER_QUERY:
		UserQuery(d);
		break;
	case USER_QUERY_IMG:
		UserQueryImage(d);
		break;

	case FOLLOW_FOLLOW:
		FollowFollow(d);
		break;
	case FOLLOW_UNFOLLOW:
		FollowUnfollow(d);
		break;
	case FOLLOW_QUERY_FOLLOWER_COUNT:
		FollowQueryFollowedCount(d);
		break;
	case FOLLOW_QUERY_FOLLOWING_COUNT:
		FollowQueryFollowingCount(d);
		break;
	case FOLLOW_QUERY_FOLLOWER_LIST:
		FollowQueryFollowedList(d);
		break;
	case FOLLOW_QUERY_FOLLOWING_LIST:
		FollowQueryFollowingList(d);
		break;

	case MESSAGE_PUSH:
		MessagePush(d);
		break;
	case MESSAGE_QUERY:
		MessageQuery(d);
		break;
	case MESSAGE_NOTIFY_OK:
		MessageNotifyOk(d);
		break;

	case PHOTO_PUSH:
		PhotoPush(d);
		break;
	case PHOTO_DELETE:
		PhotoDelete(d);
		break;
	case PHOTO_QUERY:
		PhotoQuery(d);
		break;

	case PAYMENT_QUERY:
		PaymentQuery(d);
		break;
	case PAYMENT_PUSH:
		PaymentPush(d);
		break;

	default:
		output("unknown packet\n");
		break;
	}

	return true;
}