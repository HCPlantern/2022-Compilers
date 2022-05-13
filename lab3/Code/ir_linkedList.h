#ifndef IR_LINKEDLIST_H
#define IR_LINKEDLIST_H

#include "ir.h"

typedef struct _IRLinkedList IRLinkedList;

struct _IRLinkedList {
    IR* ir;
    IRLinkedList* next;
};

IRLinkedList* makeList(IR* ir);
void merge(IRLinkedList* a, IRLinkedList* b);
void backPatch(IRLinkedList* linkedList, char* label);

#endif