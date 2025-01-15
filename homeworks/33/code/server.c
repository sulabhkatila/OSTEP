#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_types/_socklen_t.h>
#include <sys/time.h>
#include <unistd.h>

#include "networking.h"

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
    int listener = get_listener(PORT, BACKLOG);

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
        while (recv(newfd, request, MAX_REQUEST_SIZE, 0) < 0);
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
