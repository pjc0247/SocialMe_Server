/*
 *  SocialMe Server Project
 *				written by pjc0247
 *	
 *	Packet Handler
 */

#ifndef HANDLER_H
#define HANDLER_H

#include "Server.h"
#include "NetPacket.h"

struct PacketHandlerData{
	NetPacket *pkt;
	PER_IO_DATA *io;
	PER_HANDLE_DATA *handle;
};

bool OnConnected(PacketHandlerData d);
bool OnDisconnected(PacketHandlerData d);

bool VersionCheck(PacketHandlerData d);

bool RegistTry(PacketHandlerData d);

bool LoginTry(PacketHandlerData d);
bool LogoutTry(PacketHandlerData d);

bool UserUpdate(PacketHandlerData d);
bool UserQuery(PacketHandlerData d);
bool UserQueryImage(PacketHandlerData d);
bool UserUpdateComment(PacketHandlerData d);
bool UserQueryComment(PacketHandlerData d);
bool UserUpdateFacebook(PacketHandlerData d);
bool UserQueryFacebook(PacketHandlerData d);
bool UserUpdateNateon(PacketHandlerData d);
bool UserQueryNateon(PacketHandlerData d);

bool LocationPush(PacketHandlerData d);
bool LocationQuery(PacketHandlerData d);

bool FollowFollow(PacketHandlerData d);
bool FollowUnfollow(PacketHandlerData d);
bool FollowQueryFollowingCount(PacketHandlerData d);
bool FollowQueryFollowedCount(PacketHandlerData d);
bool FollowQueryFollowingList(PacketHandlerData d);
bool FollowQueryFollowedList(PacketHandlerData d);
bool FollowQueryFollowing(PacketHandlerData d);
bool FollowQueryFollowed(PacketHandlerData d);

bool FriendFriend(PacketHandlerData d);
bool FriendUnfriend(PacketHandlerData d);
bool FriendQueryFriend(PacketHandlerData d);
bool FriendQueryCount(PacketHandlerData d);
bool FriendQueryList(PacketHandlerData d);

bool MessageQuery(PacketHandlerData d);
bool MessagePush(PacketHandlerData d);
bool MessageNotifyOk(PacketHandlerData d);

bool PhotoPush(PacketHandlerData d);
bool PhotoDelete(PacketHandlerData d);
bool PhotoQuery(PacketHandlerData d);

bool PostIncrease(PacketHandlerData d);
bool PostDecrease(PacketHandlerData d);
bool PostQuery(PacketHandlerData d);

bool PaymentQuery(PacketHandlerData d);
bool PaymentPush(PacketHandlerData d);


bool ProcessPacket(PER_HANDLE_DATA *handle,PER_IO_DATA*io,NetPacket *pkt);

#endif //HANDLER_H