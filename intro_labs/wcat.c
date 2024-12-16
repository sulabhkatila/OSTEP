#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s %s", argv[0], "<filename>");
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        return 2;
    }

    char buffer[2];
    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        printf("%s", buffer);
    }

    fclose(f);
}
