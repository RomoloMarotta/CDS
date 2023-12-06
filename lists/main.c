#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdint.h>
#include "set.h"

#define KEY_MAX_VALUE 	(0x3ff)
#define KEY_MIN_VALUE 	(0)
#define ITERATIONS		5
#define MAX_THREADS		(1<<ITERATIONS)
#define SECONDS			2
#define ITEMS			(0x1ff+1)

volatile bool stop = false;
volatile int barrier = 0;
volatile int end_barrier = 0;
volatile long ops = 0;
ll_set *gset = NULL;

pthread_t ptids[MAX_THREADS];

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
#else
	printf("|   LOCK-FREE ");
#endif
	printf("|");
	for(i=1;i<=MAX_THREADS;i<<=1)
	{
		fflush(stdout);

		ops = 0L;
		stop = false;
		end_barrier = 0;
		barrier = 0;

		__sync_bool_compare_and_swap(&end_barrier, 0, i+1);

		for(j=0;j<i;j++)	pthread_create(ptids +j, NULL, stress_test, NULL);
		
		while(barrier != i);

		__sync_fetch_and_add(&barrier, 1);
		
		sleep(SECONDS);
		
		__sync_bool_compare_and_swap(&stop, false, true);
		
		for(j=0;j<i;j++)	pthread_join(ptids[j],  NULL);
		key =  ops/SECONDS/1000;
		printf("*");
		if(key < 10) printf("***");		
		else if(key < 100) printf("**");		
		else if(key < 1000) printf("*");		
		printf("%ld KOps*|", ops/SECONDS/1000);		
	}
	printf("\n");

}