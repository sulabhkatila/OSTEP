#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define magicnum 50

void error_if_not(void *p) {
    if (p == NULL) {
        perror("Error");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    char *directory = ".";
    int lflag = 0;

    // Assume sincere user
    if (argc > 1 && argv[1][0] == '-') {
        lflag = 1;
    } else if (argc > 1) {
        directory = argv[1];
    }

    if (argc > 2) {
        directory = argv[2];
    }
    // // //

    DIR *dir = opendir(directory);
    error_if_not(dir);

    struct dirent *dirinfo;
    while ((dirinfo = readdir(dir)) != NULL) {
        if (lflag) {
            if (fork() != 0) {
                execl("./stat", "./stat", dirinfo->d_name);
            }
        } else {
            printf("%s\n", dirinfo->d_name);
        }
        wait(NULL);
    }
}
