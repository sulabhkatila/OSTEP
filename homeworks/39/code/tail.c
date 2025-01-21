#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define magic 100

int main(int argc, char *argv[]) {
    if (argc != 3 || argv[1][0] != '-' || strlen(argv[1]) == 1) {
        printf("Are you dumb?\nUsage: %s -<numberoflines> <filename>\n\n",
               argv[0]);
        exit(1);
    }

    int lines, temp;
    char *filename = argv[2];
    for (int i = 1; i < strlen(argv[1]); i++) {
        temp = argv[1][i] - '0';
        if (temp > 9 || temp < 0) {
            printf("Are you dumb?\nNumber of lines should be a NUMBER\n\n");
            exit(1);
        }

        lines = lines * 10 + temp;
    }

    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        printf("Are you dumb?\nFilename should be a valid FILENAME\n\n");
        exit(1);
    }

    struct stat *sb;
    fstat(fd, sb);

    char *entire_file =
        mmap(NULL, sizeof(char) * magic * magic, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    int total_lines = 0;
    int offset[magic];
    offset[0] = 0;
    for (int i = 0; i < strlen(entire_file); i++) {
        if (entire_file[i] == '\n') {
            offset[++total_lines] = i + 1;
        }
    }

    printf("%s", entire_file + offset[total_lines - lines]);
}
