// stack.h
#ifndef STACK_H
#define STACK_H

#include <pthread.h>

#define COLLISION_SIZE 20
#define MAX_THREAD     1024

#define PUSH 0
#define POP  1

typedef struct Node {
    struct Node* volatile next;
    int data;
} Node;

typedef struct {
   Node payload;
   int op_type;
   pthread_t id;
} Collision;


#define CACHE_MULT_C 32
#define CACHE_MULT_L 32

typedef struct {
    Node* top;
    pthread_mutex_t mutex;
    pthread_spinlock_t lock;
    char pad[64-sizeof(pthread_mutex_t)- sizeof(pthread_spinlock_t) -sizeof(void*)];
    volatile int collisions_arr[COLLISION_SIZE*CACHE_MULT_C];
    Collision *volatile locations_arr[MAX_THREAD*CACHE_MULT_L];
} Stack;


void init(Stack* stack, int n);
int isEmpty(Stack* stack);
void push(Stack* stack, int value);
int pop(Stack* stack);

#endif // STACK_H
