#include "semantic_check.h"
typedef struct _Stack* Stack;

#define STACK_DEPTH 100

struct _Stack {
    Table* tables[STACK_DEPTH];
    size_t top;
};