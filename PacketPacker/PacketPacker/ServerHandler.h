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
#include "reason.h"

struct PacketHandlerData{
	NetPacket *pkt;
	PER_IO_DATA *io;
	PER_HANDLE_DATA *handle;
};

bool RegistTry(PacketHandlerData d);

bool LoginTry(PacketHandlerData d);
bool LogoutTry(PacketHandlerData d);

bool UserUpdate(PacketHandlerData d);
bool UserQuery(PacketHandlerData d);
bool UserQueryImage(PacketHandlerData d);

bool MessageQuery(PacketHandlerData d);
bool MessagePush(PacketHandlerData d);

bool PaymentQuery(PacketHandlerData d);
bool PaymentPush(PacketHandlerData d);

bool ProcessPacket(PacketHandlerData d);

#endif //HANDLER_H