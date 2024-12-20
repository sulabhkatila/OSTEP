#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fr = fork();
    if (fr == 0) {
        printf("Hello from child [%d - %d]\n", getpid(), fr);
        printf("Return value of wait in child: %d\n", wait(NULL));
    } else {
        printf("Hello from parent [%d - %d]\n", getpid(), fr);
        printf("Return value of wait in parent: %d\n", wait(NULL));
    }
}
