#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

#include "udp.h"

#define PORT 3000
#define BACKLOG 10
#define MAX_REQUEST 100

void get_address(struct sockaddr *addr_info, char *buffer, socklen_t sin_size) {
    void *addr;
    if ((addr_info->sa_family == AF_INET) {
        addr = &(((struct sockaddr_in *)addr_info)->sin_addr);
    } else {
        addr = &(((struct sockaddr_in6 *)addr_info)->sin6_addr);
    }

    inet_ntop(addr_info->sa_family, addr, buffer, sin_size);
}

int main() {
    int udp_listener = UDP_Open(PORT);
    if (listen(udp_listener, BACKLOG) == -1) {
        perror("listen");

        close(udp_listener);
        exit(1);
    }

    int newfd;
    struct sockaddr_storage claddr;
    socklen_t claddr_len;
    char clip[INET6_ADDRSTRLEN];
    char req_buff[MAX_REQUEST];
    while (1) {
        printf("Waiting for Connection in PORT: %d ...\n", PORT);
        if ((newfd = accept(udp_listener, (struct sockaddr *)&claddr,
                            &claddr_len)) == -1) {
            perror("accept");
            continue;
        }

        get_address((struct sockaddr *)&claddr, clip, claddr_len);
        printf("Got a connection from: %s\n", clip);

        UDP_Read(newfd, (struct sockaddr_in *)&claddr, req_buff, claddr_len);

        UDP_Write(newfd, (struct sockaddr_in *)&claddr, "ack", claddr_len);
    }

    close(udp_listener);
}
