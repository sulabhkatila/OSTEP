#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "3000"
#define MAX_REQUEST_SIZE 1024
#define BACKLOG 1

void check(int val, int expec, char msg[]) {
    if (val != expec) {
        fprintf(stderr, "%s\n", msg);
        exit(1);
    }
}

int main() {
    // Get the listener socket
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

    check(getaddrinfo(NULL, PORT, &hints, &res), 0,
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

    check((listen(listener, BACKLOG) == -1), 0, "listen");

    // Now we have a listener socket
    printf("Listening in: 127.0.0.1:%s\n", PORT);

    int newfd;
    struct sockaddr_storage client_addr_info;
    char client_addr4[INET_ADDRSTRLEN];
    socklen_t client_sin_size = sizeof(client_addr_info);

    printf("\n\nWaiting for connection...\n");
    while (1) {
        if ((newfd = accept(listener, (struct sockaddr *)&client_addr_info,
                            &client_sin_size)) == -1) {
            perror("accept");
            continue;
        }
        inet_ntop(client_addr_info.ss_family,
                  &((struct sockaddr_in *)&client_addr_info)->sin_addr,
                  client_addr4, client_sin_size);

        printf("Got connection from: %s", client_addr4);

        char request[MAX_REQUEST_SIZE];
        recv(newfd, request, MAX_REQUEST_SIZE, 0);
        // while (recv(newfd, request, MAX_REQUEST_SIZE, 0) > 0);
        printf("Request: %s\n", request);

        struct timeval tp;
        gettimeofday(&tp, NULL);

        char response[MAX_REQUEST_SIZE];
        sprintf(response,
                "HTTP/1.1 200 OK\nContent-Type: text/html\nConnection: "
                "close\n\n<html><body><h1>%ld</h1></body></html>",
                tp.tv_sec);

        printf("Sending: %s", response);
        fflush(stdout);

        send(newfd, response, strlen(response), 0);

        printf("\nsent\n");
        fflush(stdout);
        close(newfd);
    }
}
