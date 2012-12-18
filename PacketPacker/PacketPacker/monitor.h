#ifndef MONITOR_H
#define MONITOR_H

extern int MONITOR_INTERVAL;

void StartMonitoring();
void EndMonitoring();

void PushConnection(int n,char *ip);
void DeleteConnection(int n,char *ip);

#endif //MONITOR_H