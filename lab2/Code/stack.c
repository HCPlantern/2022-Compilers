#include "stack.h"
#include <assert.h>

extern Node* syntax_tree_root;

Stack new_stack() {
    Stack s = malloc(sizeof(struct _Stack));
    s->top = 0;
    return s;
}

void push(Stack stack, Table table) {
    if (stack->top >= STACK_DEPTH) {
        printf("Stack Overflow. at stack.c:11.\n");
        assert(0);
    }

    stack->tables[stack->top] = table;
    stack->top++;
}

Table pop(Stack stack) {
    if (stack->top == 0) {
        printf("Stack is empty. at stack.c:21.\n");
        assert(0);
    }

    stack->top--;
    return stack->tables[stack->top];
}

bool both(FieldList f) {
    return true;
}

bool isVar(FieldList f) {
    return f->is_var;
}

bool isStructDef(FieldList f) {
    return !f->is_var;
}

FieldList find_helper(Stack stack, const char* name, bool (*filter)(FieldList)) {
    for (int current = stack->top - 1; current >= 0; current--) {
        FieldList target = find_field(stack->tables[current], name);
        if (target != NULL && filter(target)) {
            return target;
        }
    }

    return NULL;
}

FieldList find_any_in_stack(Stack stack, const char* name) {
    return find_helper(stack, name, both);
}

FieldList find_var_in_stack(Stack stack, const char* name) {
    return find_helper(stack, name, isVar);
}

FieldList find_struct_def_in_stack(Stack stack, const char* name) {
    return find_helper(stack, name, isStructDef); 
}