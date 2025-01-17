#ifndef __HTTP_H_
#define __HTTP_H_

#define DEFAULT_HEADER "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"

#define METHODLEN 7

void parse_http_request(char *req_buff, char *method, char *file, char *query, char *http_v);
int send_http_response(int fd, char *res_buff);

#endif
