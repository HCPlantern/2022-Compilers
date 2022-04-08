#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "node.h"
#include "type.h"

typedef struct _TableEntry* TableEntry;
typedef struct _TableNode* TableNode;

#define TABLE_LEN 16384 

typedef TableEntry Table[TABLE_LEN];
Table table;


struct _TableEntry {
    int len;
    TableNode next;
};

struct _TableNode {
    FieldList field;
    TableNode next;
};

void init_table();
int cal_key(FieldList field);
void add_table_node(FieldList field);

void semantic_check(Node* node);

void check_ExtDef(Node* node);
void check_ExtDecList(Node* specifier, Node* node);
void check_VarDec(Node* specifier, Node* node);

#endif