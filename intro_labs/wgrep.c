#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE 256

unsigned int len(char *string) {
    unsigned int p = 0;

    while (1) {
        if (string[p] == '\0' || string[p] == '\n') {
            return p;
        }

        p++;
    }
}

int contains(char *buffer, int bufflen, char *word, int wordlen) {
    int l = 0;
    while (l <= bufflen - wordlen) {
        for (int i = 0; i < wordlen; i++) {
            if (buffer[i + l] != word[i]) {
                break;
            }

            if (i == wordlen - 1) {
                return 1;
            }
        }

        l++;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Fuck");
        exit(1);
    }

    FILE *f = fopen(argv[2], "r");
    if (f == NULL) {
        fprintf(stderr, "Fucked file");
        exit(2);
    }

    char buffer[MAX_LINE];

    while (fgets(buffer, sizeof(buffer), f) != NULL) {
        if (contains(buffer, len(buffer), argv[1], len(argv[1]))) {
            printf("%s", buffer);
        }
    }

    fclose(f);
}
