#ifndef PROTOCOL_H
#define PROTOCOL_H


enum SocialMeProtocol {

	HELLO = 1,

	LOGIN_TRY,
	LOGIN_INCORRECT,
	LOGIN_DENIED,
	LOGIN_OK,

	LOGOUT_TRY,
	LOGOUT_OK,
	LOGOUT_DENIED,

	REGIST_TRY,
	REGIST_EXIST_ID,
	REGIST_DENIED,

	USER_QUERY,
	USER_QUERY_IMG,
	USER_QUERY_DENIED,
	USER_INFO,
	USER_INFO_IMG,
	USER_UPDATE,
	USER_UPDATE_OK,
	USER_UPDATE_DENIED,

	FRIEND_QUERY_LIST,
	FRIEND_QUERY_DENIED

	};

#endif // PROTOCOL_H
