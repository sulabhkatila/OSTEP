#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Hello from main [PID - %d]\n", getpid());
    int fr = fork();
    if (fr < 0) {
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }

    if (fr == 0) {
        close(STDOUT_FILENO);
        printf("Hello from child [PID - %d]\n", getpid());
        fprintf(stdout, "Hello from child [PID - %d]\n", getpid());
    } else {
        wait(NULL);
        printf("Hello from parent [PID - %d]\n", getpid());
    }
    printf("hello in the end\n");
    return 0;
}
