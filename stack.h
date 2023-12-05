// stack.h
#ifndef STACK_H
#define STACK_H

#include <pthread.h>

typedef struct Node {
    int data;
    struct Node* volatile next;
} Node;

typedef struct {
    Node* top;
    pthread_mutex_t mutex;
    pthread_spinlock_t lock;
    char pad[64-sizeof(pthread_mutex_t)- sizeof(pthread_spinlock_t) -sizeof(void*)];
} Stack;

void init(Stack* stack, int n);
int isEmpty(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);

#endif // STACK_H
