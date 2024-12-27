
#include <stdio.h>
#include <stdlib.h>
#define SIZE 100

int main() {
    int *data = (int *)malloc(SIZE * sizeof(int *));
    printf("data[0]: %d\n", data[0]);
    data[100] = 5;
    free(data);
}
