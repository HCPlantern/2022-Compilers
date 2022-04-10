#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "node.h"
#include "hash_table.h"
#include "type.h"
#include "stack.h"



void semantic_check(Node* node);
void check_ExtDef(Node* node);
void check_ExtDecList(Node* specifier, Node* node);
FieldList check_VarDec(Node* specifier, Node* node, bool in_struct);
void check_func(Node* specifier, Node* fundec, Node* compst);

#endif