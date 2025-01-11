#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Integer Set

//
// Btree node
typedef struct node_t {
    int size;
    int *list;
    struct node_t *children;

    pthread_mutex_t mutex;
} node_t;

node_t *node_t_init(int size) {
    node_t *node = (node_t *)malloc(sizeof(node_t));
    node->list = (int *)malloc(sizeof(int) * size);
    node->children = (node_t *)malloc(
        sizeof(node_t) *
        (size + 1));  // B trees can have at most 1 more
                      // children than the max possible size of node

    return node;
}

void lock_node(node_t *node) {
    if (node == NULL) {
        return;
    }

    if (pthread_mutex_lock(&node->mutex) != 0) {
        perror("pthread_mutex_lock");
        exit(1);
    }
}

void unlock_node(node_t *node) {
    if (node == NULL) {
        return;
    }

    if (pthread_mutex_unlock(&node->mutex) != 0) {
        perror("pthread_mutex_unlock");
        exit(1);
    }
}

//
// Btree
typedef struct btree_t {
    node_t *dummy;  // Dummy will have one child and point to root node

    void (*insert)(struct btree_t *, int);
    void (*delete)(struct btree_t *, int);
    int (*search)(struct btree_t *, int);  // 1 if exists, 0 otherwise
} btree_t;

void insert(btree_t *tree, int val) {}
void delete(btree_t *tree, int val) {}
int search(btree_t *tree, int val) {}

btree_t *btree_init() {}
