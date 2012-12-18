#ifndef SYSUTIL_H
#define SYSUTIL_H

#define __INIT(cs)   InitializeCriticalSection(cs)
#define __DELETE(cs) DeleteCriticalSection(cs)
#define __ENTER(cs)  EnterCriticalSection(cs)
#define __LEAVE(cs)  LeaveCriticalSection(cs)

#endif //SYSUTIL_H