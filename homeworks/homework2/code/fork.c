#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    printf("Current pid: %d\n", (int)getpid());
    fflush(stdout);

    int x = 100;

    int fr = fork();
    if (fr < 0) {
        fprintf(stderr, "Fork unsuccessful\n");
        exit(1);
    }

    if (fr == 0) {
        // printf("Child pid: %d\n", (int)getpid());
        printf("x in Chlid process: %d\n", x);
        x += 1;
        printf("x in Child process after update: %d\n", x);
    } else {
        printf("x in Parent process: %d\n", x);
        x += 1;
        printf("x in Parent process after update: %d\n", x);
    }
    return 0;
}
