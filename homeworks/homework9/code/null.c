#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int *p = NULL;
    printf("p = %d\n", *p);
    sleep(2);
    return 0;
}
