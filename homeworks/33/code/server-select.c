#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_fd_def.h>
#include <sys/_types/_socklen_t.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "networking.h"

#define PORT "3000"
#define MAX_REQUEST_SIZE 1024
#define BACKLOG 10

void *get_address(struct sockaddr *addr_info) {
    if (addr_info->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)addr_info)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)addr_info)->sin6_addr);
}

void handle_connection(int listener) {
    int max_fd, i;
    size_t nbytes;
    char readbuf[256], resbuf[256];

    struct timeval tp;
    gettimeofday(&tp, NULL);

    sprintf(resbuf,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n\r\n"
            "<html><body><h1>%ld</h1></body></html>",
            strlen("<html><body><h1></h1></body></html>") + 10, tp.tv_sec);

    int newfd;
    struct sockaddr_storage client_addr_info;
    char client_addr4[INET6_ADDRSTRLEN];
    socklen_t client_sin_size = sizeof(client_addr_info);

    fd_set active_fd_set, read_fd_set;
    FD_ZERO(&active_fd_set);
    FD_SET(listener, &active_fd_set);

    max_fd = listener;

    while (1) {
        printf("in loop\n");
        fflush(stdout);

        read_fd_set = active_fd_set;
        check(select(max_fd + 1, &read_fd_set, NULL, NULL, NULL) == -1, 0,
              "select");

        // Run through the existing connections for data to read
        for (i = 0; i <= max_fd; i++) {
            printf("in for\n");
            fflush(stdout);
            if (FD_ISSET(
                    i, &read_fd_set)) {  // This descriptor's state was updated
                if (i == listener) {
                    printf("in listener\n");
                    fflush(stdout);
                    // We have a new connection
                    if ((newfd = accept(listener,
                                        (struct sockaddr *)&client_addr_info,
                                        &client_sin_size)) == -1) {
                        continue;
                    }

                    printf("in setting fd to fd_set\n");
                    fflush(stdout);
                    FD_SET(newfd, &active_fd_set);
                    if (newfd > max_fd) {
                        max_fd = newfd;
                    }

                    inet_ntop(client_addr_info.ss_family,
                              get_address((struct sockaddr *)&client_addr_info),
                              client_addr4, INET6_ADDRSTRLEN);
                    printf("Got a new connection in socket %d from: %s\n", i,
                           client_addr4);
                } else {
                    printf("in reading request\n");
                    fflush(stdout);
                    // Accepted contact has to be dealth with
                    if ((nbytes = recv(i, readbuf, sizeof(readbuf), 0) > 0)) {
                        send(i, resbuf, strlen(resbuf), 0);
                        printf("Closing Connection in socket %d\n", i);
                    } else {  // Connected has been closed without
                              // sending anything, that bastard
                        printf("Connection was closed in socket %d\n", i);
                    }
                    if (i == max_fd) {
                        max_fd--;
                    }
                    close(i);
                    FD_CLR(i, &active_fd_set);
                }
            }
        }
    }
}

int main() {
    // Get the listener socket
    int listener = get_listener(PORT, BACKLOG);

    printf("Listening in: 127.0.0.1:%s\n", PORT);

    // handle connection
    handle_connection(listener);
}
