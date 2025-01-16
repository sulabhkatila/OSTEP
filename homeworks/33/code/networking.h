#include <stdlib.h>
#include <sys/socket.h>

#ifndef __NETWORKING_H__
#define __NETWORKING_H__

#define OK "200 OK"
#define BADREQ "400 Bad Request"
#define NOTFOUND "404 Not Found"
#define INTERNAL_SERVER_ERROR "500 Internal Server Error"

#define KEEPALIVE KEEP_ALIVE(100, 5) // default
#define KEEP_ALIVE(m, n) m * 1000 + n
#define KEEP_ALIVE_TIMEOUT(mn) mn % 1000
#define KEEP_ALIVE_MAX(mn) mn / 1000

#define CLOSE 0

void check(int val, int expec, char *msg);
void get_address(struct sockaddr *addr_info, char *buffer, size_t sin_size);
int get_listener(char *port, int backlog);
int get_client(char *hostname, char *port);
void parse_http_request(char *req_buff, char *method, char *file, char *query, char *http_v);
int send_http_response(int fd, char *status, char *buf, int keep_alive);

#endif
