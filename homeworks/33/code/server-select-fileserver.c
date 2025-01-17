#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
#include <sys/_types/_socklen_t.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "http.h"
#include "networking.h"

void handle_connection(int listener) {
    int maxfd, i, nbytes = 0;
    char req[256], file_buf[256], res[256 + 20];

    int newfd;
    struct sockaddr_storage client_addr_info;
    char client_addr[INET6_ADDRSTRLEN];
    socklen_t client_sin_size = sizeof(client_addr_info);

    fd_set active_set, read_set;
    FD_ZERO(&active_set);
    FD_SET(listener, &active_set);

    maxfd = listener;

    while (1) {
        read_set = active_set;
        check((select(maxfd + 1, &read_set, NULL, NULL, NULL) == -1), 0,
              "Select failed");

        for (i = 0; i <= maxfd; i++) {
            if (FD_ISSET(i, &read_set)) {
                if (i == listener) {
                    // There is a new connection to be addressed
                    if ((newfd = accept(i, (struct sockaddr *)&client_addr_info,
                                        &client_sin_size)) == -1) {
                        perror("accept");
                        continue;
                    }

                    FD_SET(newfd, &active_set);
                    if (newfd > maxfd) {
                        maxfd = newfd;
                    }

                    get_address((struct sockaddr *)&client_addr_info,
                                client_addr, client_sin_size);
                    printf("Got Connection from %s, in socket %d\n",
                           client_addr, newfd);

                } else {
                    // Read from the new connection and send response
                    if ((nbytes = recv(i, req, sizeof(req), 0)) <= 0) {
                        // Connection was closed without sending a request
                        printf("Connection was closed in socket %d\n", i);
                    } else {
                        char method[5], file[50], query[10], http_v[5];

                        printf("the request is: %s\n\n", req);

                        parse_http_request(req, method, file, query, http_v);

                        // No File checks needed, we have very sincere clients

                        printf("file: %s was asked\n", file);

                        int filefd = open(file, O_RDONLY);

                        size_t total_read = 0, temp = 0;
                        while (((temp = read(filefd, file_buf,
                                             sizeof(file_buf) - total_read)) >
                                0) &&
                               (total_read += temp) < sizeof(file_buf));
                        if (temp == -1) {
                            perror("read");
                            exit(1);
                        }
                        close(filefd);
                        file_buf[total_read] = '\0';

                        printf("the value in filebuf after reading: %s\n",
                               file_buf);

                        sprintf(res,
                                "<html>\n<body>\n<h1>file you asked "
                                "for:\n</br>\n%s</h1>\n</body>\n</html>",
                                file_buf);

                        printf(
                            "the value in buf before calling send function: "
                            "%s\n",
                            res);
                        fflush(stdout);

                        send_http_response(i, res);

                        printf("sent\n");
                    }

                    printf("Closing connection in socket %d", i);

                    close(i);
                    FD_CLR(i, &active_set);

                    if (i == maxfd) {
                        maxfd--;
                    }
                }
            }
        }
    }
}

#include "main.c"
