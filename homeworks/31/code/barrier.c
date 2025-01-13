#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/semaphore.h>
#include <unistd.h>

#include "common_threads.h"

#define s1name "_s1__"
#define s2name "_s2__"

// If done correctly, each child should print their "before" message
// before either prints their "after" message. Test by adding sleep(1)
// calls in various locations.

// You likely need two semaphores to do this correctly, and some
// other integers to track things.

typedef struct __barrier_t {
    sem_t *s1;
    sem_t *s2;

    int waiting;
    int max;
} barrier_t;

// the single barrier we are using for this program
barrier_t b;

void barrier_init(barrier_t *b, int num_threads) {
    b->s1 = sem_open(s1name, O_CREAT | O_EXCL, 0644, 1);
    b->s2 = sem_open(s2name, O_CREAT | O_EXCL, 0644, 0);
    b->waiting = 0;
    b->max = num_threads;
}

void barrier(barrier_t *b) {
    sem_wait(b->s1);
    b->waiting = b->waiting + 1;
    sem_post(b->s1);

    if (b->waiting < b->max) {
        sem_wait(b->s2);
    }
    sem_post(b->s2);
}

void destroy(barrier_t *b) {
    sem_close(b->s1);
    sem_close(b->s2);
    sem_unlink(s1name);
    sem_unlink(s2name);
}

//
// XXX: don't change below here (just run it!)
//
typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *)arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}

// run with a single argument indicating the number of
// threads you wish to create (1 or more)
int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);

    int i;
    for (i = 0; i < num_threads; i++) {
        t[i].thread_id = i;
        Pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (i = 0; i < num_threads; i++) Pthread_join(p[i], NULL);

    destroy(&b);

    printf("parent: end\n");

    return 0;
}
