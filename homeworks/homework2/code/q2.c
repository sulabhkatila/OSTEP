#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char *file_name = "fireblade.txt";

    int fd = open(file_name, O_CREAT | O_WRONLY | O_TRUNC, 0644);

    int fr = fork();
    if (fr < 0) {
        fprintf(stderr, "Forking was rejected");
        exit(1);
    }

    if (fr == 0) {
        // Child process
        char *to_write = "Child process writes\n";
        int written = write(fd, to_write, strlen(to_write));
        if (written < 0) {
            fprintf(stderr, "Writing was rejected");
            exit(1);
        }
    } else {
        // Parent process
        printf("Parent process writes\n");
        printf("waiting for child process to write\n");
        printf("\n");
        fflush(stdout);
        char *to_write = "Parent process writes\n";
        int written = write(fd, to_write, strlen(to_write));
        if (written < 0) {
            fprintf(stderr, "Writing was rejected");
            exit(1);
        }
        printf("Parent process has written\n");
    }

    close(fd);
    return 0;
}
