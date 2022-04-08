#include "stack.h"

Stack new_stack() {
    Stack s = malloc(sizeof(struct _Stack));
    s->top = 0;
    return s;
}

void push(Stack stack, Table* table) {
    if (stack->top >= STACK_DEPTH) {
        printf("Stack Overflow. at stack.c:11.\n");
        assert(0);
    }

    stack->tables[stack->top] = table;
    stack->top++;
}

Table* pop(Stack stack) {
    if (stack->top == 0) {
        printf("Stack is empty. at stack.c:21.\n");
        assert(0);
    }

    stack->top--;
    return stack->tables[stack->top];
}