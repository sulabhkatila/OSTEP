#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 256
#define ZIP_EXPTENSION ".zip"

// Run Length Encoding
void write_encoded(char *readbuffer, FILE *writefile) {
    char prev = *readbuffer;
    int count = 0;

    while (1) {
        if (*readbuffer != prev) {
            fwrite(&count, sizeof(int), 1, writefile);
            fwrite(&prev, sizeof(char), 1, writefile);

            // reset
            count = 1;
            prev = *readbuffer;
        }

        readbuffer++;
        count++;

        if (*readbuffer == '\0') {
            return;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>", argv[0]);
        exit(1);
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "unable to open file: %s", argv[1]);
    }

    char writefile[strlen(argv[1]) + strlen(ZIP_EXPTENSION)];
    strcat(writefile, argv[1]);
    strcat(writefile, ZIP_EXPTENSION);

    FILE *wf = fopen(writefile, "w");

    char buffer[MAX_LINE];
    buffer[0] = 'a';
    while (fgets(buffer, sizeof(buffer) / sizeof(buffer[0]), f) != NULL) {
        write_encoded(buffer, wf);
    }

    fclose(wf);
    fclose(f);
}
