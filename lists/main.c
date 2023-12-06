#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include "set.h"

#define KEY_MAX_VALUE 	(0x3ff)
#define KEY_MIN_VALUE 	(0)
#define ITERATIONS		8 //5
#define MAX_THREADS		(1<<ITERATIONS)
#define SECONDS			1
#define ITEMS			(0x1ff+1)

volatile bool stop = false;
volatile int barrier = 0;
volatile int end_barrier = 0;
volatile long ops = 0;
ll_set *gset = NULL;

pthread_t ptids[MAX_THREADS];

int thread_count[8]={1, 2, 5, 10, 20, 30, 40, 80};

void* stress_test(void *arg){
	struct drand48_data buffer;
	int seed = 0;
	long my_ops = 0;
	long key;
	seed = __sync_fetch_and_add(&barrier, 1);
	srand48_r(seed+17, &buffer);
	
	while(end_barrier != barrier);

	while(stop == false){
		lrand48_r(&buffer, &key);
		insert(gset, key & KEY_MAX_VALUE);
		lrand48_r(&buffer, &key);
		delete(gset, key & KEY_MAX_VALUE);
		my_ops+=2;
	}
	
	__sync_fetch_and_add(&ops, my_ops);
	return NULL;
}


int main(){
	struct drand48_data buffer;
	int i,j;
	long key;
	srand48_r(17, &buffer);
	gset = init_ll_set(); 
	
	for(i=0;i<ITEMS;i++){
		lrand48_r(&buffer, &key);		
		key &= KEY_MAX_VALUE;
		insert(gset, key);
	}

#if LOCK_MODE  == PESSIMISTIC
	printf("| PESSIMISTIC ");
#elif LOCK_MODE == CHAINED
	printf("|     CHAINED ");
#elif LOCK_MODE == OPTIMISTIC
	printf("|  OPTIMISTIC ");
#elif LOCK_MODE == LOCKFREE
	printf("|   LOCK-FREE ");
#else
	printf("|   IDEAL     ");
#endif
	printf("|");
	for(i=0;i<8;i++)
	{
		fflush(stdout);
#if LOCK_MODE != IDEAL

		ops = 0L;
		stop = false;
		end_barrier = 0;
		barrier = 0;
		__sync_bool_compare_and_swap(&end_barrier, 0, thread_count[i]+1);

		for(j=0;j<thread_count[i];j++)	pthread_create(ptids +j, NULL, stress_test, NULL);
		
		while(barrier != thread_count[i]);

		__sync_fetch_and_add(&barrier, 1);
		
		sleep(SECONDS);
		
		__sync_bool_compare_and_swap(&stop, false, true);
		
		for(j=0;j<thread_count[i];j++)	pthread_join(ptids[j],  NULL);
#else
                ops = thread_count[i]*800000*SECONDS;
#endif
		key =  ops/SECONDS/1000;
		//printf("*");
		if(key < 10) printf("****");		
		else if(key < 100) printf("***");		
		else if(key < 1000) printf("**");		
		else if(key < 10000) printf("*");		
		printf("%ld KOps*|", ops/SECONDS/1000);		
	}
	printf("\n");

}
