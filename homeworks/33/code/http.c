#include "http.h"

#include <string.h>
#include <sys/socket.h>

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

int send_http_response(int fd, char *res_buff) {
    int len = strlen(res_buff);
    if (send(fd, DEFAULT_HEADER, strlen(DEFAULT_HEADER), 0) < 0) {
        return -1;
    }
    return send(fd, res_buff, len, 0);
}
