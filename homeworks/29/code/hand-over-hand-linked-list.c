#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEFAULT_THREAD_COUNT 4

typedef struct linkedlistnode {
    int val;
    pthread_mutex_t mutex;
    struct linkedlistnode *next;
} linkedlistnode;

typedef struct linkedlist {
    linkedlistnode *dummy;  // dummy->next is the first item in the list

    void (*insert)(struct linkedlist *, int);
    bool (*search)(struct linkedlist *, int);
    void (*delete)(struct linkedlist *, int);
    void (*print_nodes)(struct linkedlist *);
} linkedlist;

void lock_node(linkedlistnode *node) {
    if (node == NULL) return;

    if (pthread_mutex_lock(&node->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
}

void unlock_node(linkedlistnode *node) {
    if (node == NULL) return;

    if (pthread_mutex_unlock(&node->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

bool search(linkedlist *list, int val) {
    linkedlistnode *pointer = list->dummy;
    pointer = pointer->next;

    while (pointer != NULL) {
        if (pointer->val == val) {
            unlock_node(pointer);
            return true;
        }
        linkedlistnode *temp = pointer;
        pointer = pointer->next;
    }
    return false;
}

void insert(linkedlist *list, int val) {
    linkedlistnode *newnode = (linkedlistnode *)malloc(sizeof(linkedlistnode));
    if (newnode == NULL) {
        perror("malloc for newnode");
        exit(1);
    }

    newnode->val = val;
    if (pthread_mutex_init(&newnode->mutex, NULL) != 0) {
        perror("pthread_mutex_init");
        exit(1);
    }
    newnode->next = NULL;

    lock_node(list->dummy);
    newnode->next = list->dummy->next;
    list->dummy->next = newnode;
    unlock_node(list->dummy);
}

void delete(linkedlist *list, int val) {
    linkedlistnode *temp;

    lock_node(list->dummy);
    linkedlistnode *prev = list->dummy, *curr = list->dummy;
    lock_node(curr->next);
    // unlock_node(list->dummy);

    curr = curr->next;
    while (curr != NULL) {
        if (curr->val == val) {
            prev->next = prev->next->next;

            unlock_node(prev);
            unlock_node(curr);

            free(curr);
            return;
        }

        temp = prev;
        prev = curr;
        unlock_node(temp);

        lock_node(curr->next);
        curr = curr->next;
    }
    unlock_node(prev);
}

void print_nodes(linkedlist *list) {
    linkedlistnode *curr = list->dummy->next;
    if (curr == NULL) {
        return;
    }

    do {
        printf("%d", curr->val);
        curr = curr->next;
    } while (curr != NULL && printf(" -> "));
    printf("\n");
}

linkedlist *ll_init() {
    linkedlistnode *dummy = (linkedlistnode *)malloc(sizeof(linkedlistnode));
    dummy->val = 0;
    if (pthread_mutex_init(&dummy->mutex, NULL)) {
        perror("pthread_mutex_init");
        exit(1);
    }

    dummy->next = NULL;

    linkedlist *ll = (linkedlist *)malloc(sizeof(linkedlist));
    *ll = (linkedlist){
        dummy,

        insert, search, delete, print_nodes,
    };

    return ll;
}

void sleep_maybe() {
    if (rand() % 2 == 0) {
        sleep(2);
    }
}

void *do_something(linkedlist *ll) {
    ll->insert(ll, 1);
    sleep_maybe();
    ll->insert(ll, 2);
    sleep_maybe();
    ll->insert(ll, 3);
    sleep_maybe();
    ll->insert(ll, 4);
    sleep_maybe();
    ll->insert(ll, 5);
    sleep_maybe();
    ll->insert(ll, 6);
    sleep_maybe();
    ll->insert(ll, 7);
    sleep_maybe();
    ll->insert(ll, 8);
    sleep_maybe();
    ll->insert(ll, 9);
    sleep_maybe();
    ll->insert(ll, 10);
    sleep_maybe();
    ll->delete(ll, 8);
    sleep_maybe();
    ll->delete(ll, 9);
    sleep_maybe();
    ll->delete(ll, 10);

    return NULL;
}

int main() {
    linkedlist *ll = ll_init();

    int thread_count = DEFAULT_THREAD_COUNT;
    pthread_t threads[thread_count];

    for (int i = 0; i < thread_count; i++) {
        pthread_create(threads + i, NULL, (void *)do_something, ll);
    }

    for (int i = 0; i < thread_count; i++) {
        if (pthread_join(threads[i], NULL)) {
            char msg[100];
            sprintf(msg, "pthread_join #%d:\n", i);
            perror(msg);
            exit(1);
        }
    }

    ll->print_nodes(ll);

    // total numbers of nodes in linkedlist
    int total_nodes = 0;
    linkedlistnode *currr = ll->dummy->next;
    while (currr != NULL) {
        total_nodes++;
        currr = currr->next;
    }
    printf("Total nodes in linkedlist: %d\n", total_nodes);

    // free entire linkedlist
    linkedlistnode *curr = ll->dummy->next;
    while (curr != NULL) {
        linkedlistnode *temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(ll->dummy);
    free(ll);

    return 0;
}
