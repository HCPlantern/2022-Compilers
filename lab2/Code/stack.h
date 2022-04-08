#ifndef STACK_H
#define STACK_H

#include "hash_table.h"
typedef struct _Stack* Stack;

#define STACK_DEPTH 100

struct _Stack {
    Table tables[STACK_DEPTH];
    size_t top;
};

Stack new_stack();
void push(Stack stack, Table table);
Table pop(Stack stack);

#endif