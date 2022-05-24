#ifndef IR_LINKEDLIST_H
#define IR_LINKEDLIST_H

#include "ir.h"

typedef struct _IRLinkedList IRLinkedList;
typedef struct tree_node Node;

struct _IRLinkedList {
    IR* ir;
    IRLinkedList* next;
};

IRLinkedList* makeList(IR* ir);
IRLinkedList* merge(IRLinkedList* a, IRLinkedList* b);
void backPatch(IRLinkedList* linkedList, Node* M, bool is_while_M1);

#endif