#include "networking.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

void check(int val, int expec, char msg[]) {
    if (val != expec) {
        fprintf(stderr, "%s\n", msg);
        exit(1);
    }
}

void get_address(struct sockaddr *addr_info, char *buffer, size_t sin_size) {
    void *addr;
    if (addr_info->sa_family == AF_INET) {
        addr = &(((struct sockaddr_in *)addr_info)->sin_addr);
    } else {
        addr = &(((struct sockaddr_in6 *)addr_info)->sin6_addr);
    }

    inet_ntop(addr_info->sa_family, addr, buffer, sin_size);
}

int get_listener(char *port, int backlog) {
    int listener;  // the descriptor for the socket
    struct addrinfo hints, *res,
        *p;  // res for all the available sockets that meet our hints and p is
             // to loop over the sockets

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;  // IpV4
    hints.ai_socktype =
        SOCK_STREAM;  // because TCP. Had it been UDP, we'd go with SOCK_DGRAM
    hints.ai_flags =
        AI_PASSIVE;  // because it is needed to be ready for connect

    check(getaddrinfo(NULL, port, &hints, &res), 0,
          "getaddrinfo");  // Get all the possible socket addresses

    for (p = res; p != NULL;
         p = p->ai_next) {  // loop over the addresses and get the first one
                            // that is okay
        if ((listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) ==
            -1) {  // Try another socket address
            continue;
        }

        if (bind(listener, p->ai_addr, p->ai_addrlen) ==
            -1) {  // bind the socket // if unsuccessfull with this socket
                   // address get another
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    check(p == NULL, 0,
          "No Sockets available");  // check we ended up with a valid socket

    check((listen(listener, backlog) == -1), 0, "listen");

    return listener;
}

int get_client(char *hostname, char *port) { return 1; }

void parse_http_request(char *req_buff, char *method, char *file, char *query,
                        char *http_v) {
    method[0] = '\0';
    file[0] = '\0';
    query[0] = '\0';
    http_v[0] = '\0';

    // GET /file?query HTTP/1.1
    // ...
    char *temp_file;
    char *temp_query;
    char *temp_version;

    // Parse method
    char *temp_method = strtok(req_buff, " ");
    if (temp_method) {
        strcpy(method, temp_method);
    }

    // Parse file and query
    temp_file = strtok(NULL, " ");
    if (temp_file) {
        temp_query = strchr(temp_file, '?');
        if (temp_query) {
            *temp_query = '\0';
            strcpy(query, temp_query + 1);
        }
        strcpy(file, ++temp_file);
    }

    // Parse http version
    char *http_sec = strtok(NULL, "\n");
    if (http_sec) {
        temp_version = strchr(http_sec, '/');
        if (temp_version) {
            *temp_version = '\0';
            strcpy(http_v, temp_version + 1);
        }
    }
}

int send_http_response(int fd, char *status, char *buf, int keep_alive) {
    // keep_alive can be at max 99999, the first 3 digits are the max and
    // the last 3 are the timeout

    printf("the fd to send is: %d\n", fd);
    printf("ths status to send is: %s\n", status);
    printf("the buf to send is: %s\n", buf);
    printf("keep alive: %d\n", keep_alive);

    char connection[strlen("Keep-Alive\nKeep-Alive: timeout=100, max=100")];
    if (keep_alive) {
        sprintf(connection, "Keep-Alive\nKeep-Alive: timeout=%d, max=%d",
                keep_alive % 100, keep_alive / 100);
    } else {
        strncpy(connection, "close", sizeof("close"));
    }

    char response[strlen(buf) + 100];
    sprintf(response,
            "HTTP/1.1 %s\nContent-Type: text/html\nContent-Length: "
            "%ld\nConnection: "
            "%s\n\n%s",
            status,
            strlen("HTTP/1.1 \nContent-Type: text/html\nContent-Length: "
                   "\nConnection: \n\n") +
                strlen(status) + strlen(buf) + strlen("1024"),
            connection, buf);

    size_t total_size = sizeof(response), temp = 0;

    printf("sending: %s", response);
    fflush(stdout);

    while (((temp = send(fd, response, total_size, 0)) > 0) &&
           (total_size -= temp) > 0) {
        printf("sending\n");
    };

    return send(fd, buf, strlen(response), 0);
}
