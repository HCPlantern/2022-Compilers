#ifndef IR_GENERATOR_H
#define IR_GENERATOR_H
#include "node.h"

void assign_gen(Node* father, Node* lValue, Node* exp);
void not_gen(Node* father, Node* exp);
void and_gen(Node* father, Node* exp1, Node* exp2);
void or_gen(Node* father, Node* exp1, Node* exp2);
void relop_gen(Node* father, Node* exp1, Node* exp2);
void plus_gen(Node* father, Node* exp1, Node* exp2);
void minus_gen(Node* father, Node* exp1, Node* exp2);
void star_gen(Node* father, Node* exp1, Node* exp2);
void div_gen(Node* father, Node* exp1, Node* exp2);
void negative_gen(Node* father, Node* exp);
void parathesis_reduce(Node* father, Node* exp);
void negative_gen(Node* father, Node* exp);
// void tilde_gen(Node* exp); // vm does not support tilde.
void func_call_gen(Node* father, Node* id, Node* args); // special process for read() and write(x)
void array_store_gen(Node* father, Node* array, Node* index);
void array_load_gen(Node* father, Node* array, Node* index);
void field_store_gen(Node* father, Node* base, Node* field);
void field_load_gen(Node* father, Node* base, Node* field);
void id_gen(Node* father, Node* id);
void int_gen(Node* father, Node* _int);
void float_gen(Node* father, Node* _float);

#endif