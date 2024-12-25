#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 1024 * 1024
#define COUNT 100

int main() {
    char *p[COUNT];
    for (int i = 0; i < COUNT; i++) {
        p[i] = (char *)malloc(SIZE);
        sleep(2);
    }
}
