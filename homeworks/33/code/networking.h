#ifndef __NETWORKING_H__
#define __NETWORKING_H__

void check(int val, int expec, char *msg);
int get_listener(char *port, int backlog);
int get_client(char *hostname, char *port);

#endif
