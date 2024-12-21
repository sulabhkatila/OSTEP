#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int piped[2];
    if (pipe(piped) < 0) {
        fprintf(stderr, "Pipe failed\n");
        exit(1);
    }

    int fr = fork();
    if (fr < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }

    if (fr == 0) {
        // Child 1
        close(piped[0]);
        dup2(piped[1], STDOUT_FILENO);

        printf("Hello from child [PID - %d]\n", getpid());
        close(piped[1]);
    } else {
        fr = fork();
        if (fr < 0) {
            fprintf(stderr, "Fork failed\n");
            exit(1);
        }

        if (fr == 0) {
            // Child 2
            close(piped[1]);
            dup2(piped[0], STDIN_FILENO);

            char buff[100];
            scanf("%s", buff);
            printf("Message from child 1: %s\n", buff);
            close(piped[0]);
        } else {
            // Parent Process
            close(piped[0]);
            close(piped[1]);
        }
    }
}
