#include "stdafx.h"

#include "ServerHandler.h"

#include "Protocol.h"

bool ProcessPacket(PER_HANDLE_DATA *handle,PER_IO_DATA*io,NetPacket *pkt){

	PacketHandlerData d;
	
	d.handle = handle;
	d.io = io;
	d.pkt = pkt;

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
	case USER_QUERY_COMMENT:
		UserQueryComment(d);
		break;
	case USER_UPDATE_COMMENT:
		UserUpdateComment(d);
		break;
	case USER_QUERY_FACEBOOK:
		UserQueryFacebook(d);
		break;
	case USER_UPDATE_FACEBOOK:
		UserUpdateFacebook(d);
		break;
	case USER_QUERY_NATEON:
		UserQueryNateon(d);
		break;
	case USER_UPDATE_NATEON:
		UserUpdateNateon(d);
		break;

	case LOCATION_PUSH:
		LocationPush(d);
		break;
	case LOCATION_QUERY:
		LocationQuery(d);
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
	case FOLLOW_QUERY_FOLLOWING:
		FollowQueryFollowing(d);
		break;
	case FOLLOW_QUERY_FOLLOWED:
		FollowQueryFollowed(d);
		break;

	case FRIEND_ADD:
		FriendFriend(d);
		break;
	case FRIEND_DELETE:
		FriendUnfriend(d);
		break;
	case FRIEND_QUERY_FRIEND:
		FriendQueryFriend(d);
		break;
	case FRIEND_QUERY_COUNT:
		FriendQueryCount(d);
		break;
	case FRIEND_QUERY_LIST:
		FriendQueryList(d);
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

	case POST_INC:
		PostIncrease(d);
		break;
	case POST_DEC:
		PostDecrease(d);
		break;
	case POST_QUERY_COUNT:
		PostQuery(d);
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

	NetDisposePacket(pkt, true);
	pkt = NetCreatePacket();
	NetRecvPacket(handle,io);

	return true;
}