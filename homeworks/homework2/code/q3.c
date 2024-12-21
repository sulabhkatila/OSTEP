#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILENAME "fireblade.txt"

int main() {
    // Inter process communication to let the other process know that we have
    // written in a file
    int write_to = open(FILENAME, O_CREAT | O_WRONLY, 0644);
    if (write_to < 0) {
        fprintf(stderr, "Unable to open: %s\n", FILENAME);
        exit(1);
    }

    int piped[2];
    if (pipe(piped) != 0) {
        fprintf(stderr, "Unable to pipe\n");
        close(write_to);
        exit(2);
    }

    int fr = fork();
    if (fr < 0) {
        fprintf(stderr, "Unable to fork\n");
        close(write_to);
        close(piped[0]);
        close(piped[1]);
        exit(3);
    }

    if (fr == 0) {
        // child process
        close(piped[0]);

        char *message = "Hello ";
        if (write(write_to, message, strlen(message)) < 0) {
            fprintf(stderr, "Unable to write to file\n");
            close(write_to);
            close(piped[1]);
            exit(1);
        }

        int one = 1;
        if (write(piped[1], &one, sizeof(one)) < 0) {
            fprintf(stderr, "Unable to IPC\n");
            close(piped[1]);
            close(write_to);
            exit(4);
        }

        close(piped[1]);
    } else {
        close(piped[1]);

        int read_val = 0;
        read(piped[0], &read_val, sizeof(read_val));

        if (read_val == 0) {
            fprintf(stderr, "Unable to IPC\n");
            close(piped[0]);
            close(write_to);
            exit(4);
        }

        char *message = "World";
        if (write(write_to, message, strlen(message)) < 0) {
            fprintf(stderr, "Unable to write to file\n");
            close(piped[0]);
            close(write_to);
            exit(1);
        }

        close(piped[0]);
    }

    close(write_to);
}
