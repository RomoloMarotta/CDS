// stack.c
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include "clock_constant.h"

#define UNROLL 100
#define ITERS  0

__thread int   local_allocs = 1;
__thread Node* local_ptr = NULL;
__thread int initr = 0;
__thread struct drand48_data randBuffer;

void init(Stack* stack, int n) {
    stack->top = NULL;

    for (int i = n; i > 0; --i) {
        push(stack, i);
    }
}

int isEmpty(Stack* stack) {
    return stack->top == NULL;
}

void backoff(int cnt, int max){
        if(cnt && ITERS){
           unsigned long long wait = CLOCK_READ();
           long r = 0;   lrand48_r(&randBuffer, &r);
           r = r % ( max << (cnt+1));
           r = CLOCKS_PER_US*r;
           while((CLOCK_READ()-wait) < r);
        }
}


void push(Stack* stack, int value) {
    Node* newNode;
    local_allocs--;
    if(!initr) {initr=1;srand48_r(time(NULL), &randBuffer);}

    if(!local_allocs){
      local_ptr = (Node*)malloc(sizeof(Node)*UNROLL);
      if (local_ptr == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
      }
      local_allocs = UNROLL-1;
    }
    newNode = &local_ptr[local_allocs];
    newNode->data = value;
    int cnt = 0;
    do {
	backoff(cnt, ITERS);
        cnt++;
        newNode->next = stack->top;
    } while (!__sync_bool_compare_and_swap(&stack->top, newNode->next, newNode));
}

int pop(Stack* stack) {
    Node* topNode;
    if(!initr) {initr=1;srand48_r(time(NULL), &randBuffer);}
    int cnt = 0;
    do {
        topNode = stack->top;
        if (topNode == NULL) {
            return -1; // Stack is empty
        }
        backoff(cnt, ITERS);
    } while (!__sync_bool_compare_and_swap(&stack->top, topNode, topNode->next));

    int value = topNode->data;
    //free(topNode);
    return value;
}
