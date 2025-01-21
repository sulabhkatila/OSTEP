#include <stdio.h>
#include <string.h>
#include <sys/_types/_s_ifmt.h>
#include <sys/stat.h>

#define magicnumber 100

void fill_file_mode(unsigned short mode_num, char *mode) {
    switch (S_IFMT & mode_num) {
        case S_IFREG:
            sprintf(mode, "regular");
            break;
        case S_IFDIR:
            sprintf(mode, "direcotry");
            break;
        default:
            sprintf(mode, "some-fancy-thing");
            break;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
    }

    struct stat st;
    char error[magicnumber], mode[magicnumber];

    for (int i = 1; i < argc; i++) {
        if (stat(argv[i], &st) == -1) {
            snprintf(error, sizeof(error), "stat error for \"%s\"", argv[i]);
            perror("stat");
            continue;
        }

        fill_file_mode(st.st_mode, mode);

        printf("File: '%s'\n", argv[i]);
        printf("Size: %lld    Blocks: %lld    IO Block: %d    %s\n", st.st_size,
               st.st_blocks, st.st_blksize, mode);
        printf("...\n");
    }
}
