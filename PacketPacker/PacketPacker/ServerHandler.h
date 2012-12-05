#ifndef HANDLER_H
#define HANDLER_H

#include "NetPacket.h"

bool RegistTry(NetPacket *p);

bool ProcessPacket(NetPacket *p);

#endif //HANDLER_H