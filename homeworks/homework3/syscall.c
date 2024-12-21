#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#define SYSCALL_COUNT 1000000
#define TEST_ITERATION 10

int main() {
    int logfile = open("syscall.log", O_CREAT | O_WRONLY | O_APPEND, 0644);

    struct timeval start, end;

    long total = 0;
    for (int i = 0; i < TEST_ITERATION; i++) {
        gettimeofday(&start, NULL);
        for (int j = 0; j < SYSCALL_COUNT; j++) {
            int test = open("syscall.c", 0);
            close(test);
        }

        gettimeofday(&end, NULL);

        long total_time = (end.tv_sec - start.tv_sec) * 1000000 +
                          (end.tv_usec - start.tv_usec);

        total += total_time;

        char buf[100];
        sprintf(buf, "%ld\n", total_time);
        write(logfile, buf, strlen(buf));
    }

    write(logfile, "\n- - - - - - - - - - - - - - - -\n", 33);

    char buf[100];
    sprintf(buf, "Average time per call: %.2f microseconds\n",
            total / (float)(2 * TEST_ITERATION * SYSCALL_COUNT));
    write(logfile, buf, strlen(buf));

    write(logfile, "\n- - - - - - - - - - - - - - - -\n", 33);
    write(logfile, "\n- - - - - - - - - - - - - - - -\n", 33);

    close(logfile);

    printf("Average time per call: %.2f microseconds\n",
           total / (float)(2 * TEST_ITERATION * SYSCALL_COUNT));
    return 0;
}
