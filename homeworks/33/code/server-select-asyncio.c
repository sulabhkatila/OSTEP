#include <aio.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_select.h>
#include <sys/_types/_socklen_t.h>
#include <sys/aio.h>
#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "http.h"
#include "networking.h"

#define PORT "3000"
#define BACKLOG 10

#define REQ_SIZE 256
#define RES_SIZE 256

#define FILENAMELEN 20
#define QUERYLEN 20

#define MAX_CONCURRENT 10

void print_filecbs(struct aiocb *cbs) {
    printf("aio_fildes: %d\naio_nbytes: %ul\naio_buf: %s\n\n", cbs->aio_fildes,
           cbs->aio_nbytes, (char *)cbs->aio_buf);
}

void handle_connection(int listener) {
    int newfd;

    struct sockaddr_storage client_addr_info;
    char client_addr[INET6_ADDRSTRLEN];
    socklen_t client_sin_size = sizeof(client_addr_info);

    size_t nbytes, total_size, temp;
    char request_buffer[REQ_SIZE], response_buffers[MAX_CONCURRENT][RES_SIZE];
    char method[METHODLEN], filename[FILENAMELEN], query[QUERYLEN],
        http_version[5];

    // Event-based concurrency essentials
    int i, j, maxfd, filecb_index = 0;
    maxfd = listener;

    fd_set read_set, active_read_set;
    FD_ZERO(&active_read_set);
    FD_SET(listener, &active_read_set);

    struct aiocb filecbs[MAX_CONCURRENT];
    int client_sockets[MAX_CONCURRENT] = {
        0};  // assume stdins are not to be worried about

    while (1) {
        read_set = active_read_set;

        select(maxfd + 1, &read_set, NULL, NULL, NULL);

        for (i = 3; i <= maxfd;
             i++) {  // stdin, stdout, and stderr are not to be worried about
            if (FD_ISSET(
                    i,
                    &read_set)) {  // Either new client initiated a connection
                                   // Or old client is ready to be read from

                if (i == listener) {  // someone's initiated connection

                    newfd =
                        accept(listener, (struct sockaddr *)&client_addr_info,
                               &client_sin_size);
                    if (newfd == -1) continue;

                    get_address((struct sockaddr *)&client_addr_info,
                                client_addr, client_sin_size);
                    printf("New Connection (from %s) on socket %d\n",
                           client_addr, newfd);

                    if (newfd > maxfd) maxfd = newfd;
                    FD_SET(newfd, &active_read_set);

                } else {
                    // read for the first time
                    nbytes = recv(i, request_buffer, sizeof(request_buffer), 0);
                    if (nbytes ==
                        -1) {  // Connection closed without sending request

                        printf("Connection was closed in socket %d\n", i);
                        close(i);
                        FD_CLR(i, &active_read_set);
                        if (i == maxfd) maxfd--;

                        continue;
                    }

                    printf("Request: %s\n", request_buffer);

                    parse_http_request(request_buffer, method, filename, query,
                                       http_version);

                    printf("method: %s\n", method);
                    printf("filename: %s\n", filename);

                    // Assume sincere client
                    int filefd = open(filename, O_RDONLY);
                    if (filefd == -1) {
                        perror("Open");
                        close(i);

                        FD_CLR(i, &active_read_set);
                        if (i == maxfd) maxfd--;

                        continue;
                    }

                    nbytes = 0, total_size = sizeof(response_buffers[0]) - 1;

                    struct aiocb filecb;

                    memset(&filecb, 0, sizeof(filecb));
                    filecb.aio_fildes = filefd;
                    filecb.aio_offset = 0;
                    filecb.aio_buf = response_buffers[filecb_index];
                    filecb.aio_nbytes = total_size;

                    if (aio_read(&filecb) == -1) {
                        perror("aio_read");
                        close(filefd);
                        close(i);

                        FD_CLR(i, &active_read_set);
                        if (i == maxfd) maxfd--;

                        continue;
                    }

                    filecbs[filecb_index] = filecb;
                    client_sockets[filecb_index] = i;
                    filecb_index = (filecb_index + 1) %
                                   (MAX_CONCURRENT);  // assume prior requests
                                                      // will be completed
                }
            }

            // Check if file, asked by client, is read
            print_filecbs(filecbs);
            for (j = 0; j < MAX_CONCURRENT; j++) {
                printf("Checking if read...\n");
                if (client_sockets[j] > 0) {
                    int val = aio_error(filecbs + j);
                    print_filecbs(filecbs + j);
                    if (val == EINPROGRESS) {
                        continue;
                    }

                    int read = aio_return(filecbs + j);
                    printf("Amount read: %d\n", read);
                    if (val == 0) {
                        response_buffers[j][read] =
                            '\0';  // send_http_response
                                   // expects null termination

                        printf("The following was read: %s\n",
                               response_buffers[j]);

                        if (send_http_response(i, response_buffers[j]) == -1) {
                            perror("send");
                        }
                    } else {
                        printf("val: %d\n", val);
                        perror("aio_error");
                    }
                    close(filecbs[j].aio_fildes);
                    client_sockets[j] = 0;

                    printf("Closing connection in socket %d\n", i);
                    close(i);

                    FD_CLR(i, &active_read_set);
                    if (i == maxfd) maxfd--;
                }
            }
        }
    }
}

#include "main.c"
