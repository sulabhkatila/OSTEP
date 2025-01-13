#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/semaphore.h>
#include <unistd.h>

#include "common_threads.h"

#define SEM_NAME "semmy"

sem_t *s;

void *child(void *arg) {
    printf("child\n");

    sem_post(s);  //
    sem_close(s);

    return NULL;
}
void sleep_maybe() {
    if (rand() % 2 == 0) {
        sleep(2);
    }
}

int main(int argc, char *argv[]) {
    pthread_t p;
    printf("parent: begin\n");

    sem_unlink(SEM_NAME);
    sem_unlink(SEM_NAME);
    sem_unlink(SEM_NAME);
    sem_unlink(SEM_NAME);
    sem_unlink(SEM_NAME);
    sem_unlink(SEM_NAME);
    sem_unlink(SEM_NAME);

    s = sem_open(SEM_NAME, O_CREAT | O_EXCL, 0644, 0);
    if (s == SEM_FAILED) {
        perror("sem_open (parent)");
        exit(1);
    }

    Pthread_create(&p, NULL, child, NULL);

    sleep_maybe();
    sem_wait(s);  //
    // Pthread_join(p, NULL);

    printf("parent: end\n");

    sem_close(s);
    sem_unlink(SEM_NAME);
    return 0;
}
