// main.c
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "clock_constant.h"



int RATE = 1;

typedef struct {
    Stack* stack;
    struct drand48_data seed;
    int operationCount;
} ThreadData;

volatile bool stop = false;

void* sleepThreadFunction(void* arg) {
    int sleepTime = *((int*)arg);
    sleep(sleepTime);
    stop = true;
    return NULL;
}

void wait_active(long r){
    unsigned long long wait;
//    r = r %(RATE*2);
r = RATE;
    wait=CLOCK_READ();
    while( (CLOCK_READ()-wait) < (CLOCKS_PER_US*r) );
}


void* threadFunction(void* arg) {
    ThreadData* threadData = (ThreadData*)arg;
    Stack* stack = threadData->stack;
    while (!stop) {
        //int r = rand();
        long r = 0; lrand48_r(&threadData->seed, &r);
        wait_active(r);
        int operation = r % 2;

        if (operation)
            push(stack, r % 100);
        else
       // wait_active();
            pop(stack);


        //threadData->operationCount++;
        threadData->operationCount++;
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <number_of_threads> <initial_elements> <sleep_time> <us_wait>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int numThreads = atoi(argv[1]);
    int initialElements = atoi(argv[2]);
    int sleepTime = atoi(argv[3]);
    RATE = atoi(argv[4]);

    if (numThreads <= 0 || initialElements < 0 || sleepTime < 0) {
        fprintf(stderr, "Invalid number of threads, initial elements, or sleep time\n");
        return EXIT_FAILURE;
    }

    pthread_t threads[numThreads];
    pthread_t sleepThread;
    Stack stack;
    init(&stack, initialElements);

    ThreadData threadData[numThreads];
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].stack = &stack;
        threadData[i].operationCount = 0;
        srand48_r(i,&threadData[i].seed);
    }

    if (pthread_create(&sleepThread, NULL, sleepThreadFunction, &sleepTime) != 0) {
        perror("Error creating sleep thread");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < numThreads; ++i) {
        if (pthread_create(&threads[i], NULL, threadFunction, (void*)&threadData[i]) != 0) {
            perror("Error creating main thread");
            return EXIT_FAILURE;
        }
    }

    pthread_join(sleepThread, NULL);
    int sum=0;
    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
        sum+= threadData[i].operationCount;
    }

    double operationsPerSecond = (double)sum / sleepTime;

    printf(" %.2f", operationsPerSecond/1000000.0);

    while (!isEmpty(&stack)) {
        pop(&stack);
    }

    return EXIT_SUCCESS;
}

