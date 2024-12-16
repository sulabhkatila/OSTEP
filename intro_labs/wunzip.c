#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("wunzip: file1.zip");
        exit(1);
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        printf("wunzip: cannot open file\n");
        exit(1);
    }

    char *write_file = argv[1];
    write_file[strlen(write_file) - 4] = '\0';

    FILE *wfp = fopen(write_file, "w");

    int count;
    char c;
    while (fread(&count, 4 * sizeof(int), 1, fp) > 0) {
        fread(&c, sizeof(char), 1, fp);

        for (int i = 0; i < count; i++) {
            fprintf(wfp, "%c", c);
        }
    }

    fclose(wfp);
    fclose(fp);

    return 0;
}
