#include <stdlib.h>
#include <sys/socket.h>

#ifndef __NETWORKING_H__
#define __NETWORKING_H__

void check(int val, int expec, char *msg);
void get_address(struct sockaddr *addr_info, char *buffer, size_t sin_size);
int get_listener(char *port, int backlog);
int get_client(char *hostname, char *port);

#endif
