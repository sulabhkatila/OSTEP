#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "common_threads.h"

int balance = 0;
pthread_mutex_t mutex;

void* worker(void* arg) {
    pthread_mutex_lock(&mutex);
    balance++;  // unprotected access
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, char* argv[]) {
    pthread_t p;
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Mutex");
        exit(1);
    }

    Pthread_create(&p, NULL, worker, NULL);
    pthread_mutex_lock(&mutex);
    balance++;  // unprotected access
    pthread_mutex_unlock(&mutex);
    Pthread_join(p, NULL);
    return 0;
}
