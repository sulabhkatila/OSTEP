#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char *file_name = "test.txt";
    int fd = open(file_name, O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    char *buf = "Hello, World!";
    int len = strlen(buf);
    int ret = write(fd, buf, len);
    if (ret < 0) {
        perror("write");
        exit(1);
    }

    close(fd);
    return 0;
}
