#include <semaphore.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/semaphore.h>
#include <unistd.h>

#include "common_threads.h"

#define s1name "s1"
#define s2name "s2"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

sem_t *s1, *s2;

void *child_1(void *arg) {
    printf("child 1: before\n");

    sem_post(s1);
    sem_wait(s2);
    printf("child 1: after\n");
    return NULL;
}

void *child_2(void *arg) {
    printf("child 2: before\n");

    sem_post(s2);
    sem_wait(s1);
    printf("child 2: after\n");
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t p1, p2;
    printf("parent: begin\n");

    s1 = sem_open(s1name, O_CREAT | O_EXCL, 0644, 0);
    s2 = sem_open(s2name, O_CREAT | O_EXCL, 0644, 0);

    Pthread_create(&p1, NULL, child_1, NULL);
    Pthread_create(&p2, NULL, child_2, NULL);
    Pthread_join(p1, NULL);
    Pthread_join(p2, NULL);

    printf("parent: end\n");

    sem_close(s1);
    sem_close(s2);

    sem_unlink(s1name);
    sem_unlink(s2name);

    return 0;
}
