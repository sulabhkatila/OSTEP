#include <_time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_pthread/_pthread_mutex_t.h>
#include <sys/_pthread/_pthread_t.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DEFAULT_ITERATIONS 1e6
#define DEFAULT_THREADS 1

// Single thread performance = 12000 ms

typedef struct counter_t {
    int count;
} counter_t;

counter_t init() { return (counter_t){0}; }

void increment(counter_t *counter, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    counter->count++;
    pthread_mutex_unlock(mutex);
}

void decrement(counter_t *counter, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    counter->count--;
    pthread_mutex_unlock(mutex);
}

typedef struct incrementer_args_t {
    counter_t *counter;
    pthread_mutex_t *mutex;
    int iterations;
} incrementer_args_t;

void *increment_counter(void *arguments) {
    incrementer_args_t *args = (incrementer_args_t *)arguments;

    counter_t *counter = args->counter;
    pthread_mutex_t *mutex = args->mutex;
    int iterations = args->iterations;

    if (iterations < 0) {
        exit(1);
    }

    for (int i = 0; i < iterations; i++) {
        increment(counter, mutex);
    }

    return NULL;
}

counter_t counter;
pthread_mutex_t mutex;

int main(int argc, char *argv[]) {
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Mutex init\n");
        exit(1);
    }

    int number_of_cpu;
    size_t size = sizeof(number_of_cpu);
    sysctlbyname("hw.logicalcpu", &number_of_cpu, &size, NULL, 0);
    printf("Btw, the total number of logical cores is %d\n\n", number_of_cpu);

    counter = init();

    int thread_count = DEFAULT_THREADS;
    if (argc > 1) {
        thread_count = atoi(argv[1]);
    }

    pthread_t threads[thread_count];
    struct timeval start, end;

    gettimeofday(&start, NULL);

    for (int i = 0; i < thread_count; i++) {
        incrementer_args_t args = {
            &counter,
            &mutex,
            DEFAULT_ITERATIONS,
        };
        pthread_create(threads + i, NULL, (void *)increment_counter,
                       (void *)&args);
    }

    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL);
    }

    gettimeofday(&end, NULL);

    long total_time =
        ((end.tv_sec - start.tv_sec) * 1e6) + (end.tv_usec - start.tv_usec);
    printf("Total time\n%ld ms\n%f s\n", total_time, total_time / (double)1e6);

    printf("Value in counter: %d\n", counter.count);
    return 0;
}
