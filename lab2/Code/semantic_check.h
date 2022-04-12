#ifndef SEMANTIC_H
#define SEMANTIC_H

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#include "node.h"
#include "type.h"



void semantic_check(Node* node);
void check_ExtDef(Node* node);
void check_ExtDecList(Node* specifier, Node* node);
FieldList check_VarDec(Node* specifier, Node* node, bool in_struct);
void check_func(Node* specifier);
void check_CompSt(Node* compst);
void check_DefList(Node* compst);
void check_StmtList(Node* stmtlist);
void check_undefined_func();

#endif