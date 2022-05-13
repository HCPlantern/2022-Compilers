#include "ir_linkedList.h"
#include <stdlib.h>

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

void merge(IRLinkedList* a, const IRLinkedList* b) {
    IRLinkedList* end = endOf(a);
    end->next = b;
}

bool prefix(const char* pre, const char* str) {
    return strncmp(pre, str, strlen(pre)) == 0;
}

void singleIRBackPatch(IR* ir, char* label) {

}

void backPatch(IRLinkedList* list, char* label) {
    while (list != NULL) {
        singleIRBackPatch(list->ir, label);
        list = list->next;
    }
}