#include "networking.h"

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
