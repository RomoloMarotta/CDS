// stack.c
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clock_constant.h"

#define UNROLL 100
#define ITERS  4
#define MAX    16

#define TRY_FAILED -2
#define EMPTY      -1
#define OK          1

__thread int   local_allocs = 1;
__thread Collision* local_ptr = NULL;
__thread int mid = 0;
__thread struct drand48_data randBuffer;

volatile int gid = 0;

void per_thread_init(){
    srand48_r(time(NULL), &randBuffer); 
    mid = __sync_add_and_fetch(&gid, 1);
    if(mid == (MAX_THREAD)){
        printf("too much threads\n");
        exit(1);
    }
    mid *= CACHE_MULT_L;
}

void init(Stack* stack, int n) {
    bzero(stack, sizeof(Stack));
    for (int i = n; i > 0; --i) {
        push(stack, i);
    }
}

int isEmpty(Stack* stack) {
    return stack->top == NULL;
}

void backoff(int cnt, int max){
        max = max << (cnt+1);
	if(max > MAX) max = max;
        if(cnt && ITERS){
           unsigned long long wait = CLOCK_READ();
           long r = 0;   lrand48_r(&randBuffer, &r);
           //r = r % max;
           if(r<0)r*=-1;
           r = r % (CLOCKS_PER_US*MAX);
           while((CLOCK_READ()-wait) < r);
        }
}


long get_rand(){
long r = 0;   lrand48_r(&randBuffer, &r);
r = r % COLLISION_SIZE;
return r;
}


int try_push(Stack* stack, Node *newNode) {
    newNode->next = stack->top;
    if(__sync_bool_compare_and_swap(&stack->top, newNode->next, newNode)) return OK;
    return TRY_FAILED;
}

int try_pop(Stack* stack){
    Node* topNode = stack->top;
    
    if (topNode == NULL) return EMPTY; // Stack is empty
    
    if (__sync_bool_compare_and_swap(&stack->top, topNode, topNode->next))
       return topNode->data;
    
    return TRY_FAILED;
}


int try_collision(Stack *stack, Collision *mine, Collision *his, int him){
    int res = TRY_FAILED;
    if(mine->op_type == PUSH){ // mine is a push
        if(__sync_bool_compare_and_swap(&stack->locations_arr[him], his, mine))
            res = OK;
    }
    else{ //mine is a pop
        if(__sync_bool_compare_and_swap(&stack->locations_arr[him], his, NULL)){
            res = his->payload.data;
            stack->locations_arr[mid] = NULL;
	    //if(res == TRY_FAILED){printf("ARGh\n");}
        }
    }
    return res;
}


int elimination_op(Stack* stack, Collision* mystr, int cnt){
    Collision *histr;
    int him;
    long pos = get_rand() * CACHE_MULT_C;
    int res = TRY_FAILED;
    
    mystr->id = mid;
    stack->locations_arr[mid] = mystr;

begin:

    do{
        him = stack->collisions_arr[pos];
    }while(!__sync_bool_compare_and_swap(&stack->collisions_arr[pos], him, mid));
    
if(!him) goto begin;
    if(him){
        histr = stack->locations_arr[him];
        if(histr && histr->id == him && mystr->op_type != histr->op_type){
            if(__sync_bool_compare_and_swap(&stack->locations_arr[mid], mystr, NULL)){
                res = try_collision(stack, mystr, histr, him);
            }
            else{
                if(mystr->op_type == PUSH) res = OK; // is a push
                else{
                        res = stack->locations_arr[mid]->payload.data; 
                        stack->locations_arr[mid] = NULL;
                        
                }
            }
            goto out;     
        }
    }
    
    backoff(cnt, ITERS);
    
    if(!__sync_bool_compare_and_swap(&stack->locations_arr[mid], mystr, NULL) ){
        if(mystr->op_type == PUSH) res = OK; // is a push
        else{
                res = stack->locations_arr[mid]->payload.data; 
                stack->locations_arr[mid] = NULL;
        }
    }

out:
    return res;
}


Collision *c_alloc(){
    local_allocs--;
    if(!mid) per_thread_init();

    if(!local_allocs){
      local_ptr = (Collision*)malloc(sizeof(Collision)*UNROLL);
      if (local_ptr == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
      }
      local_allocs = UNROLL-1;
    }
    return &local_ptr[local_allocs];
}

void c_free_last(){
local_allocs++;
}

void push(Stack* stack, int value) {
    Node* newNode;
    int cnt = 0, val = 0;
    Collision *ts = c_alloc();


    ts->op_type = PUSH;
    newNode = (Node*) &local_ptr[local_allocs];
    newNode->data = value;
    
    do {
        if(try_push(stack, newNode) == OK) break;
        val = elimination_op(stack, ts, cnt);
        if(val != TRY_FAILED) break;
        cnt+=2;
    } while (1);

    return;
}



int pop(Stack* stack) {
    int val = 0, cnt = 0;
    Collision *ts = c_alloc();

    ts->op_type = POP;

    do {
        val = try_pop(stack);
        if(val != TRY_FAILED) break;
        val = try_pop(stack);
        if(val != TRY_FAILED) break;
        val = elimination_op(stack, ts, cnt);
        if(val != TRY_FAILED) break;
        cnt++;
    } while (1);

    //free(topNode);
    return val;
}
