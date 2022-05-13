#include "ir_linkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

IRLinkedList* makeList(IR* ir) {
    IRLinkedList* list = malloc(sizeof(IRLinkedList));
    list->ir = ir;
    list->next = NULL;
    return list;
}

// because list comes with at least 1 elements, so it would not hit seg fault
static IRLinkedList* endOf(IRLinkedList* list) {
    while (list->next != NULL) {
        list = list->next;
    }
    return list;
}

IRLinkedList* merge(IRLinkedList* a, IRLinkedList* b) {
    if (a != NULL && b != NULL) {
        IRLinkedList* end = endOf(a);
        end->next = b;
        return a;
    }
    if (a == NULL) {
        return b;
    }
    if (b == NULL) {
        return a;
    }
}

static bool prefix(const char* pre, const char* str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

// assume that the "IR" is completed except that it has no label.
void singleIRBackPatch(IR* ir, char* label) {
    if (prefix("IF", ir->ir) || prefix("GOTO", ir->ir)) {
        char buf[max_single_ir_len];
        sprintf(buf, "%s %s", ir->ir, label);
        sprintf(ir->ir, "%s", buf);
    } else {
        assert(0);
    }
}

void backPatch(IRLinkedList* list, char* label) {
    while (list != NULL) {
        singleIRBackPatch(list->ir, label);
        list = list->next;
    }
}