#ifndef __UDP_H__
#define __UDP_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int UDP_Open(int port);
int UDP_FillSockAddr(struct sockaddr_in *addr, char *hostname, int port);
int UDP_Write(int sd, struct sockaddr_in *addr, char *buffer, int n);
int UDP_Read(int sd, struct sockaddr_in *addr, char *buffer, int n);

#endif // __UDP_H__
