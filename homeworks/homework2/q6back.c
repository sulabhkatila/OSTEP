#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fr = fork();
    int parent = getpid();

    if (fr == 0) {
        printf("Hello from child [%d - %d]\n", getpid(), fr);
        printf("Return value of wait in child: %d\n", waitpid(parent, NULL, 0));
    } else {
        printf("Hello from parent [%d - %d]\n", getpid(), fr);
        printf("Return value of wait in parent: %d\n", waitpid(fr, NULL, 0));
    }
}
