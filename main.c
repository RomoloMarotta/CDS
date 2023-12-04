// main.c
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

typedef struct {
    Stack* stack;
    int operationCount;
} ThreadData;

volatile bool stop = false;

void* sleepThreadFunction(void* arg) {
    int sleepTime = *((int*)arg);
    sleep(sleepTime);
    stop = true;
    return NULL;
}

void* threadFunction(void* arg) {
    ThreadData* threadData = (ThreadData*)arg;
    Stack* stack = threadData->stack;

    while (!stop) {
        int operation = rand() % 2;

        if (operation == 0) {
            push(stack, rand() % 100);
        } else {
            pop(stack);
        }

        threadData->operationCount++;
    }

    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <number_of_threads> <initial_elements> <sleep_time>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int numThreads = atoi(argv[1]);
    int initialElements = atoi(argv[2]);
    int sleepTime = atoi(argv[3]);

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

