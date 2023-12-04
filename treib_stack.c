// stack.c
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>


void init(Stack* stack, int n) {
    stack->top = NULL;

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

    do {
        newNode->next = stack->top;
    } while (!__sync_bool_compare_and_swap(&stack->top, newNode->next, newNode));
}

int pop(Stack* stack) {
    Node* topNode;

    do {
        topNode = stack->top;
        if (topNode == NULL) {
            return -1; // Stack is empty
        }
    } while (!__sync_bool_compare_and_swap(&stack->top, topNode, topNode->next));

    int value = topNode->data;
    //free(topNode);
    return value;
}
