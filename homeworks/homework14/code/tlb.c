#include <stdio.h>
#include <stdlib.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>

#define DEFAULT_INSTRUCTIONS_PER_PAGE 100
#define DEFAULT_NUMBER_OF_PAGES 10

int main(int argc, char *argv[]) {
    size_t page_size;
    size_t len = sizeof(page_size);

    if (sysctlbyname("hw.pagesize", &page_size, &len, NULL, 0) != 0) {
        perror("sysctlbyname");
        return EXIT_FAILURE;
    }

    int number_of_pages = DEFAULT_NUMBER_OF_PAGES;
    int number_of_instructions_per_page = DEFAULT_INSTRUCTIONS_PER_PAGE;
    if (argc >= 2) {
        number_of_pages = atoi(argv[1]);
    }
    if (argc >= 3) {
        number_of_instructions_per_page = atoi(argv[2]);
    }

    int jump = page_size / (int)sizeof(int);
    long total_numbers = number_of_pages * (long)jump;
    int i = 0, j = 0;
    int *a = (int *)malloc(total_numbers * sizeof(int));
    for (i = 0; i < total_numbers; i++) {
        a[i] = 0;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (j = 0; j < number_of_instructions_per_page; j++) {
        for (i = 0; i < total_numbers; i += jump) {
            a[i] += 1;
        }
    }
    gettimeofday(&end, NULL);

    long total_time = end.tv_sec * 1000000 + end.tv_usec -
                      start.tv_sec * 1000000 + start.tv_usec;

    printf("%4f\n", total_time / (double)(number_of_instructions_per_page *
                                          number_of_pages));
    free(a);
    return EXIT_SUCCESS;
}
