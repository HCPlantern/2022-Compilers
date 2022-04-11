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
bool both(FieldList f);
bool isVar(FieldList f);
bool isStructDef(FieldList f);
FieldList find_helper(Stack stack, const char* name, bool (*filter)(FieldList));
FieldList find_any_in_stack(Stack stack, const char* name);
FieldList find_var_in_stack(Stack stack, const char* name);
FieldList find_struct_def_in_stack(Stack stack, const char* name);


#endif