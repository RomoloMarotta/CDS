// stack.h
#ifndef STACK_H
#define STACK_H

#include <pthread.h>

#define COLLISION_SIZE 20

typedef struct Node {
    int data;
    struct Node* volatile next;
} Node;

typedef struct {
   void *payload;
   int tid;
   int oid;
   char pad[64-16];
} Collision;


typedef struct {
    Node* top;
    pthread_mutex_t mutex;
    pthread_spinlock_t lock;
    char pad[64-sizeof(pthread_mutex_t)- sizeof(pthread_spinlock_t) -sizeof(void*)];
    Collision *cells[COLLISION_SIZE];
} Stack;


void init(Stack* stack, int n);
int isEmpty(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);

#endif // STACK_H
