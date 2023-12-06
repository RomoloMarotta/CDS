// stack.c
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

void init(Stack* stack, int n) {
    stack->top = NULL;
    pthread_mutex_init(&stack->mutex, NULL);

    for (int i = n; i > 0; --i) {
        push(stack, i);
    }
}

int isEmpty(Stack* stack) {
    return stack->top == NULL;
}

void push(Stack* stack, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    newNode->data = value;
    newNode->next = NULL;

    pthread_mutex_lock(&stack->mutex);

    newNode->next = stack->top;
    stack->top = newNode;

    pthread_mutex_unlock(&stack->mutex);
}

int pop(Stack* stack) {
    pthread_mutex_lock(&stack->mutex);

    if (isEmpty(stack)) {
        pthread_mutex_unlock(&stack->mutex);
        return -1;
    } else {
        Node* topNode = stack->top;
        int value = topNode->data;
        stack->top = topNode->next;
        free(topNode);

        pthread_mutex_unlock(&stack->mutex);
        return value;
    }
}
