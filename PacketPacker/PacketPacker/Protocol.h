/*
 *  SocialMe Server Project
 *				written by pjc0247
 *	
 *	Protocol definations
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H


enum SocialMeProtocol {

	HELLO = 1,
	SERVER_BUSY,

	PING,

	LOGIN_TRY,
	LOGIN_INCORRECT,
	LOGIN_FAILED,
	LOGIN_OK,

	LOGOUT_TRY,
	LOGOUT_OK,
	LOGOUT_FAILED,

	REGIST_TRY,
	REGIST_EXIST_ID,
	REGIST_OK,
	REGIST_FAILED,

	USER_QUERY,
	USER_QUERY_IMG,
	USER_QUERY_FAILED,
	USER_INFO,
	USER_INFO_IMG,
	USER_UPDATE,
	USER_UPDATE_OK,
	USER_UPDATE_FAILED,

	FRIEND_QUERY_LIST,
	FRIEND_QUERY_FAILED,

	FOLLOW_FOLLOW,
	FOLLOW_UNFOLLOW,
	FOLLOW_FAILED,
	FOLLOW_OK,
	FOLLOW_QUERY_FOLLOWING_COUNT,
	FOLLOW_QUERY_FOLLOWER_COUNT,
	FOLLOW_QUERY_FOLLOWING_LIST,
	FOLLOW_QUERY_FOLLOWER_LIST,
	FOLLOW_INFO_FOLLOWING_COUNT,
	FOLLOW_INFO_FOLLOWER_COUNT,
	FOLLOW_INFO_FOLLOWING_LIST,
	FOLLOW_INFO_FOLLOWER_LIST,
	FOLLOW_QUERY_FAILED,

	MESSAGE_PUSH,
	MESSAGE_PUSH_OK,
	MESSAGE_PUSH_FAILED,
	MESSAGE_NOTIFY,
	MESSAGE_NOTIFY_OK,
	MESSAGE_QUERY,
	MESSAGE_QUERY_FAILED,
	MESSAGE_INFO,

	PHOTO_PUSH,
	PHOTO_PUSH_OK,
	PHOTO_PUSH_FAILED,
	PHOTO_QUERY,
	PHOTO_QUERY_FAILED,
	PHOTO_INFO,

	PAYMENT_PUSH,
	PAYMENT_PUSH_OK,
	PAYMENT_PUSH_FAILED,
	PAYMENT_QUERY,
	PAYMENT_QUERY_FAILED,
	PAYMENT_SIZE,
	PAYMENT_INFO

	};

#endif // PROTOCOL_H
