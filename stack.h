// stack.h
#ifndef STACK_H
#define STACK_H

#include <pthread.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* top;
    pthread_mutex_t mutex;
    pthread_spinlock_t lock;
} Stack;

void init(Stack* stack, int n);
int isEmpty(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);

#endif // STACK_H
