#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

void check(int i, int val, char message[]) {
    if (i < val) {
        fprintf(stderr, "%s\n", message);
        exit(1);
    }
}

void check_fork(int i) {
    char *message = "Forking is a nono";
    check(i, 0, message);
}

int main() {
    char *function_to_call = "/bin/ls";
    char path[5];
    path[4] = '\0';
    for (int i = 0; i < 4; i++) {
        path[i] = function_to_call[i];
    }
    char *file_name = &function_to_call[strlen(path) + 1];

    printf("file_name: %s\n", file_name);
    fflush(stdout);

    int fr = fork();
    check_fork(fr);

    if (fr == 0) {
        // execl
        execl(function_to_call, function_to_call, (char *)NULL);
    } else {
        wait(NULL);
        printf("\n");
        fflush(stdout);

        fr = fork();
        check_fork(fr);

        if (fr == 0) {
            execle(function_to_call, function_to_call, (char *)NULL,
                   (char *)NULL);
        } else {
            wait(NULL);
            printf("\n");
            fflush(stdout);

            fr = fork();
            check_fork(fr);

            if (fr == 0) {
                execlp(file_name, function_to_call, (char *)NULL);
            } else {
                wait(NULL);
                printf("\n");
                fflush(stdout);

                fr = fork();
                check_fork(fr);

                if (fr == 0) {
                    char f2c[strlen(function_to_call) + 1];
                    strcpy(f2c, function_to_call);
                    char *const args[] = {f2c, (char *)NULL};

                    execv(function_to_call, args);
                } else {
                    wait(NULL);
                    printf("\n");
                    fflush(stdout);

                    fr = fork();
                    check_fork(fr);

                    if (fr == 0) {
                        char f2c[strlen(path) + 1];
                        strcpy(f2c, path);
                        char *const args[] = {f2c, (char *)NULL};
                        execvp(file_name, args);

                    } else {
                        wait(NULL);
                        printf("\n");
                        fflush(stdout);

                        fr = fork();
                        check_fork(fr);

                        if (fr == 0) {
                            char f2c[strlen(file_name) + 1];
                            strcpy(f2c, file_name);
                            char *const args[] = {file_name, (char *)NULL};
                            execvP(file_name, "/bin", args);
                        } else {
                            wait(NULL);
                        }
                    }
                }
            }
        }
    }
}
